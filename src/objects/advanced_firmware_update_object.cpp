/**
 * @file advanced_firmware_update_object.cpp
 * @brief OMA LwM2M Advanced Firmware Update Object Implementation
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#include "objects/advanced_firmware_update_object.h"
#include "platform/platform_abstraction.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <regex>

#include <curl/curl.h>
#include <openssl/sha.h>

namespace fs = std::filesystem;

namespace lwm2m {
namespace objects {

// ============================================================================
// Utility Functions
// ============================================================================

static size_t curlWriteCallback(void* contents, size_t size, size_t nmemb, 
                                 std::vector<uint8_t>* data) {
    size_t totalSize = size * nmemb;
    uint8_t* bytes = static_cast<uint8_t*>(contents);
    data->insert(data->end(), bytes, bytes + totalSize);
    return totalSize;
}

static std::vector<uint8_t> calculateSHA256(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);
    SHA256(data.data(), data.size(), hash.data());
    return hash;
}

static std::string bytesToHex(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;
    for (auto b : bytes) {
        ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(b);
    }
    return ss.str();
}

// ============================================================================
// Constructor / Destructor
// ============================================================================

AdvancedFirmwareUpdateObject::AdvancedFirmwareUpdateObject(uint16_t instanceId,
                                                             const Config& config)
    : config_(config)
    , instanceId_(instanceId)
    , deltaAlgorithm_(config.defaultDeltaAlgorithm)
    , blockSize_(config.defaultBlockSize) {
    
    lastStateChange_ = std::chrono::system_clock::now();
}

AdvancedFirmwareUpdateObject::~AdvancedFirmwareUpdateObject() {
    // Request cancellation and wait for worker thread
    cancelRequested_ = true;
    
    if (workerThread_ && workerThread_->joinable()) {
        workerThread_->join();
    }
}

bool AdvancedFirmwareUpdateObject::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);

    // Create directories
    try {
        fs::create_directories(config_.downloadPath);
        fs::create_directories(config_.backupPath);
    } catch (const std::exception& e) {
        conflictReason_ = "Failed to create directories: " + std::string(e.what());
        return false;
    }

    // Initialize rollback manager
    if (config_.rollbackEnabled) {
        firmware::RollbackManager::Config rbConfig;
        rbConfig.snapshotStoragePath = config_.backupPath;
        rbConfig.maxSnapshots = config_.maxRollbackSnapshots;
        rbConfig.autoSnapshot = true;
        
        rollbackManager_ = std::make_shared<firmware::RollbackManager>(rbConfig);
        auto result = rollbackManager_->initialize();
        
        if (result != firmware::RollbackResult::SUCCESS) {
            // Non-fatal, continue without rollback
            rollbackManager_.reset();
        }
    }

    // Initialize delta processor
    deltaProcessor_ = firmware::DeltaAlgorithmFactory::create(deltaAlgorithm_);

    // Initialize block transfer handler
    blockTransfer_ = std::make_unique<transport::BlockwiseTransfer>();
    blockTransfer_->setPreferredBlockSize(
        static_cast<transport::BlockwiseTransfer::BlockSize>(blockSize_));

    // Get current firmware version
    auto systemManager = platform::PlatformFactory::getSystemManager();
    if (systemManager) {
        sourceVersion_ = systemManager->getFirmwareVersion();
    }

    return true;
}

// ============================================================================
// LwM2M Resource Operations
// ============================================================================

bool AdvancedFirmwareUpdateObject::read(FirmwareResourceId resourceId, 
                                         std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    switch (resourceId) {
        case FirmwareResourceId::PACKAGE_URI:
            value = packageUri_;
            return true;
            
        case FirmwareResourceId::PKG_NAME:
            value = packageName_;
            return true;
            
        case FirmwareResourceId::PKG_VERSION:
            value = packageVersion_;
            return true;
            
        case FirmwareResourceId::DELTA_PACKAGE_URI:
            value = deltaPackageUri_;
            return true;
            
        case FirmwareResourceId::COMPONENT_LIST:
            value = getComponentsJson();
            return true;
            
        case FirmwareResourceId::SOURCE_VERSION:
            value = sourceVersion_;
            return true;
            
        case FirmwareResourceId::TARGET_VERSION:
            value = targetVersion_;
            return true;
            
        case FirmwareResourceId::CONFLICT_REASON:
            value = conflictReason_;
            return true;
            
        default:
            return false;
    }
}

bool AdvancedFirmwareUpdateObject::read(FirmwareResourceId resourceId, 
                                         int64_t& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    switch (resourceId) {
        case FirmwareResourceId::STATE:
            value = static_cast<int64_t>(state_.load());
            return true;
            
        case FirmwareResourceId::UPDATE_RESULT:
            value = static_cast<int64_t>(updateResult_.load());
            return true;
            
        case FirmwareResourceId::PROTOCOL_SUPPORT:
            value = static_cast<int64_t>(ProtocolSupport::COAP) |
                    static_cast<int64_t>(ProtocolSupport::COAPS) |
                    static_cast<int64_t>(ProtocolSupport::HTTP) |
                    static_cast<int64_t>(ProtocolSupport::HTTPS);
            return true;
            
        case FirmwareResourceId::DELIVERY_METHOD:
            value = static_cast<int64_t>(DeliveryMethod::BOTH);
            return true;
            
        case FirmwareResourceId::DELTA_ALGORITHM:
            value = static_cast<int64_t>(deltaAlgorithm_);
            return true;
            
        case FirmwareResourceId::BLOCK_SIZE:
            value = blockSize_;
            return true;
            
        case FirmwareResourceId::DIFFERENTIAL_DOWNLOAD:
            value = differentialDownload_ ? 1 : 0;
            return true;
            
        case FirmwareResourceId::ROLLBACK_SUPPORT:
            value = rollbackEnabled_ ? 1 : 0;
            return true;
            
        case FirmwareResourceId::UPDATE_PROGRESS:
            value = progress_.load();
            return true;
            
        case FirmwareResourceId::SEVERITY:
            value = static_cast<int64_t>(severity_);
            return true;
            
        case FirmwareResourceId::LAST_STATE_CHANGE:
            value = std::chrono::duration_cast<std::chrono::seconds>(
                lastStateChange_.time_since_epoch()).count();
            return true;
            
        case FirmwareResourceId::MAXIMUM_BLOCK_SIZE:
            value = 1024;  // Max standard CoAP block size
            return true;
            
        default:
            return false;
    }
}

bool AdvancedFirmwareUpdateObject::read(FirmwareResourceId resourceId,
                                         std::vector<uint8_t>& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    switch (resourceId) {
        case FirmwareResourceId::CHECKSUM:
            value = expectedChecksum_;
            return true;
            
        default:
            return false;
    }
}

bool AdvancedFirmwareUpdateObject::write(FirmwareResourceId resourceId,
                                          const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    switch (resourceId) {
        case FirmwareResourceId::PACKAGE_URI:
            if (state_ != FirmwareState::IDLE) {
                conflictReason_ = "Update already in progress";
                return false;
            }
            packageUri_ = value;
            // Trigger download
            mutex_.unlock();
            startDownload(value, false);
            mutex_.lock();
            return true;
            
        case FirmwareResourceId::DELTA_PACKAGE_URI:
            if (state_ != FirmwareState::IDLE) {
                conflictReason_ = "Update already in progress";
                return false;
            }
            deltaPackageUri_ = value;
            mutex_.unlock();
            startDownload(value, true);
            mutex_.lock();
            return true;
            
        default:
            return false;
    }
}

bool AdvancedFirmwareUpdateObject::write(FirmwareResourceId resourceId,
                                          int64_t value) {
    std::lock_guard<std::mutex> lock(mutex_);

    switch (resourceId) {
        case FirmwareResourceId::DELTA_ALGORITHM:
            if (value >= 1 && value <= 3) {
                deltaAlgorithm_ = static_cast<firmware::DeltaAlgorithm>(value);
                deltaProcessor_ = firmware::DeltaAlgorithmFactory::create(deltaAlgorithm_);
                return true;
            }
            return false;
            
        case FirmwareResourceId::BLOCK_SIZE:
            if (value >= 16 && value <= 1024) {
                blockSize_ = static_cast<uint32_t>(value);
                return true;
            }
            return false;
            
        case FirmwareResourceId::DIFFERENTIAL_DOWNLOAD:
            differentialDownload_ = (value != 0);
            return true;
            
        case FirmwareResourceId::ROLLBACK_SUPPORT:
            rollbackEnabled_ = (value != 0);
            return true;
            
        default:
            return false;
    }
}

bool AdvancedFirmwareUpdateObject::write(FirmwareResourceId resourceId,
                                          const std::vector<uint8_t>& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    switch (resourceId) {
        case FirmwareResourceId::PACKAGE:
            if (state_ != FirmwareState::IDLE && 
                state_ != FirmwareState::DOWNLOADING) {
                conflictReason_ = "Cannot receive package in current state";
                return false;
            }
            // Receive full package or chunk
            mutex_.unlock();
            return receivePackageData(value, receivedData_.size(), false);
            
        case FirmwareResourceId::DELTA_PACKAGE:
            if (state_ != FirmwareState::IDLE &&
                state_ != FirmwareState::DOWNLOADING) {
                conflictReason_ = "Cannot receive delta in current state";
                return false;
            }
            mutex_.unlock();
            return receivePackageData(value, receivedData_.size(), true);
            
        case FirmwareResourceId::CHECKSUM:
            expectedChecksum_ = value;
            return true;
            
        default:
            return false;
    }
}

bool AdvancedFirmwareUpdateObject::execute(FirmwareResourceId resourceId,
                                            const std::string& arguments) {
    switch (resourceId) {
        case FirmwareResourceId::UPDATE:
            return startUpdate();
            
        case FirmwareResourceId::CANCEL:
            return cancelUpdate();
            
        default:
            return false;
    }
}

// ============================================================================
// Firmware Update Operations
// ============================================================================

bool AdvancedFirmwareUpdateObject::startDownload(const std::string& uri, 
                                                   bool isDelta) {
    if (state_ != FirmwareState::IDLE) {
        conflictReason_ = "Download already in progress";
        return false;
    }

    // Store URI
    if (isDelta) {
        deltaPackageUri_ = uri;
    } else {
        packageUri_ = uri;
    }

    // Clear previous data
    receivedData_.clear();
    cancelRequested_ = false;

    // Start download thread
    if (workerThread_ && workerThread_->joinable()) {
        workerThread_->join();
    }
    
    workerThread_ = std::make_unique<std::thread>(
        &AdvancedFirmwareUpdateObject::downloadThread, this, uri, isDelta);

    return true;
}

void AdvancedFirmwareUpdateObject::downloadThread(const std::string& uri, 
                                                    bool isDelta) {
    setState(FirmwareState::DOWNLOADING);
    setProgress(0, "Starting download");

    std::vector<uint8_t> data;
    bool success = downloadFromUri(uri, data);

    if (cancelRequested_) {
        setState(FirmwareState::IDLE);
        setResult(UpdateResult::DEFAULT);
        return;
    }

    if (!success) {
        setState(FirmwareState::IDLE);
        setResult(UpdateResult::CONNECTION_LOST);
        return;
    }

    // Store downloaded data
    {
        std::lock_guard<std::mutex> lock(mutex_);
        receivedData_ = std::move(data);
    }

    // Validate package
    setState(FirmwareState::VALIDATING);
    setProgress(90, "Validating package");

    if (!validatePackage(receivedData_)) {
        setState(FirmwareState::IDLE);
        setResult(UpdateResult::INTEGRITY_CHECK_FAILED);
        return;
    }

    setState(FirmwareState::DOWNLOADED);
    setResult(UpdateResult::DEFAULT);
    setProgress(100, "Download complete");
}

bool AdvancedFirmwareUpdateObject::downloadFromUri(const std::string& uri,
                                                    std::vector<uint8_t>& data) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        conflictReason_ = "Failed to initialize CURL";
        return false;
    }

    data.clear();
    
    curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3600L);  // 1 hour timeout
    
    // Progress callback
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, 
        [](void* clientp, curl_off_t dltotal, curl_off_t dlnow,
           curl_off_t ultotal, curl_off_t ulnow) -> int {
            auto* self = static_cast<AdvancedFirmwareUpdateObject*>(clientp);
            if (self->cancelRequested_) {
                return 1;  // Abort transfer
            }
            if (dltotal > 0) {
                int progress = static_cast<int>((dlnow * 80) / dltotal);
                self->setProgress(progress, "Downloading");
            }
            return 0;
        });
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);

    CURLcode res = curl_easy_perform(curl);
    
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        conflictReason_ = "Download failed: " + std::string(curl_easy_strerror(res));
        return false;
    }

    if (httpCode != 200) {
        conflictReason_ = "HTTP error: " + std::to_string(httpCode);
        return false;
    }

    return true;
}

bool AdvancedFirmwareUpdateObject::receivePackageData(
    const std::vector<uint8_t>& data, uint64_t offset, bool isDelta) {
    
    std::lock_guard<std::mutex> lock(mutex_);

    if (offset == 0) {
        receivedData_.clear();
        setState(FirmwareState::DOWNLOADING);
    }

    // Append data
    if (offset != receivedData_.size()) {
        conflictReason_ = "Unexpected data offset";
        return false;
    }

    receivedData_.insert(receivedData_.end(), data.begin(), data.end());

    // Update progress
    if (expectedSize_ > 0) {
        int progress = static_cast<int>((receivedData_.size() * 80) / expectedSize_);
        setProgress(progress, "Receiving data");
    }

    return true;
}

bool AdvancedFirmwareUpdateObject::startUpdate() {
    if (state_ != FirmwareState::DOWNLOADED) {
        conflictReason_ = "No firmware package ready for update";
        return false;
    }

    cancelRequested_ = false;

    // Start update thread
    if (workerThread_ && workerThread_->joinable()) {
        workerThread_->join();
    }

    workerThread_ = std::make_unique<std::thread>(
        &AdvancedFirmwareUpdateObject::updateThread, this);

    return true;
}

void AdvancedFirmwareUpdateObject::updateThread() {
    setState(FirmwareState::UPDATING);
    setProgress(0, "Starting update");

    // Create pre-update snapshot if rollback enabled
    if (rollbackEnabled_ && rollbackManager_) {
        setProgress(5, "Creating backup snapshot");
        rollbackManager_->createSnapshot(sourceVersion_, "Pre-update backup");
    }

    bool success = false;

    // Determine if this is a delta update
    if (!deltaPackageUri_.empty() || 
        (receivedData_.size() > 8 && 
         std::memcmp(receivedData_.data(), "BSDIFF", 6) == 0)) {
        // Delta update
        setState(FirmwareState::APPLYING_DELTA);
        setProgress(20, "Applying delta update");
        success = applyDeltaUpdate(receivedData_);
    } else {
        // Full update
        setProgress(20, "Applying full update");
        success = applyFullUpdate(receivedData_);
    }

    if (cancelRequested_) {
        setState(FirmwareState::IDLE);
        setResult(UpdateResult::DEFAULT);
        return;
    }

    if (!success) {
        // Attempt rollback if enabled
        if (rollbackEnabled_ && rollbackManager_) {
            setProgress(80, "Update failed, attempting rollback");
            auto rollbackResult = rollbackManager_->rollback();
            if (rollbackResult != firmware::RollbackResult::SUCCESS) {
                setResult(UpdateResult::ROLLBACK_FAILED);
            } else {
                setResult(UpdateResult::UPDATE_FAILED);
            }
        } else {
            setResult(UpdateResult::UPDATE_FAILED);
        }
        setState(FirmwareState::IDLE);
        return;
    }

    // Finalize update
    if (!finalizeUpdate()) {
        setResult(UpdateResult::UPDATE_FAILED);
        setState(FirmwareState::IDLE);
        return;
    }

    setResult(UpdateResult::SUCCESS);
    setState(FirmwareState::WAITING_FOR_REBOOT);
    setProgress(100, "Update complete, waiting for reboot");

    // Trigger update complete callback
    if (updateCompleteCallback_) {
        updateCompleteCallback_(UpdateResult::SUCCESS, "Update completed successfully");
    }

    // Auto-reboot if configured
    if (config_.autoReboot) {
        auto systemManager = platform::PlatformFactory::getSystemManager();
        if (systemManager) {
            systemManager->reboot(config_.rebootDelay);
        }
    }
}

bool AdvancedFirmwareUpdateObject::validatePackage(
    const std::vector<uint8_t>& data) {
    
    // Check if we have expected checksum
    if (!expectedChecksum_.empty()) {
        auto actualChecksum = calculateSHA256(data);
        if (actualChecksum != expectedChecksum_) {
            conflictReason_ = "Checksum mismatch";
            return false;
        }
    }

    // Basic size check
    if (data.size() < 64) {
        conflictReason_ = "Package too small";
        return false;
    }

    // Check for recognized headers
    // Full firmware might have UBI, SquashFS, or other headers
    // Delta packages have their own magic numbers

    return true;
}

bool AdvancedFirmwareUpdateObject::applyFullUpdate(
    const std::vector<uint8_t>& data) {
    
    auto partitionManager = platform::PlatformFactory::getPartitionManager();
    auto flashManager = platform::PlatformFactory::getFlashManager();
    
    if (!partitionManager || !flashManager) {
        conflictReason_ = "Platform not initialized";
        return false;
    }

    // Get inactive slot
    auto inactiveSlot = partitionManager->getInactiveSlot();
    auto partition = partitionManager->getPartition(inactiveSlot);
    
    if (!partition) {
        conflictReason_ = "Inactive partition not found";
        return false;
    }

    // Check size
    if (data.size() > partition->size) {
        conflictReason_ = "Package too large for partition";
        return false;
    }

    setProgress(30, "Writing firmware to partition");

    // Write to inactive partition
    auto result = flashManager->write(partition->device, 0, data,
        [this](uint64_t current, uint64_t total) {
            int progress = 30 + static_cast<int>((current * 40) / total);
            setProgress(progress, "Writing firmware");
        });

    if (result != platform::PlatformResult::SUCCESS) {
        conflictReason_ = "Failed to write firmware";
        return false;
    }

    setProgress(70, "Verifying firmware");

    // Verify written data
    auto checksum = partitionManager->calculateChecksum(inactiveSlot);
    auto expectedChecksum = calculateSHA256(data);
    
    if (checksum != expectedChecksum) {
        conflictReason_ = "Verification failed after write";
        return false;
    }

    setProgress(80, "Marking partition bootable");

    // Mark new partition as bootable
    result = partitionManager->setSlotBootable(inactiveSlot, true);
    if (result != platform::PlatformResult::SUCCESS) {
        conflictReason_ = "Failed to mark partition bootable";
        return false;
    }

    // Switch to new partition
    result = partitionManager->switchSlot(inactiveSlot);
    if (result != platform::PlatformResult::SUCCESS) {
        conflictReason_ = "Failed to switch partition";
        return false;
    }

    return true;
}

bool AdvancedFirmwareUpdateObject::applyDeltaUpdate(
    const std::vector<uint8_t>& deltaData) {
    
    if (!deltaProcessor_) {
        deltaProcessor_ = firmware::DeltaAlgorithmFactory::create(deltaAlgorithm_);
    }

    auto partitionManager = platform::PlatformFactory::getPartitionManager();
    auto flashManager = platform::PlatformFactory::getFlashManager();
    
    if (!partitionManager || !flashManager) {
        conflictReason_ = "Platform not initialized";
        return false;
    }

    // Get active partition data
    auto activeSlot = partitionManager->getActiveSlot();
    auto activePartition = partitionManager->getPartition(activeSlot);
    
    if (!activePartition) {
        conflictReason_ = "Active partition not found";
        return false;
    }

    setProgress(25, "Reading source firmware");

    // Read current firmware
    std::vector<uint8_t> sourceData;
    auto result = flashManager->read(activePartition->device, 0, 
                                      activePartition->usedSize, sourceData);
    
    if (result != platform::PlatformResult::SUCCESS) {
        conflictReason_ = "Failed to read source firmware";
        return false;
    }

    setProgress(40, "Applying delta patch");

    // Apply delta
    std::vector<uint8_t> targetData;
    auto deltaResult = deltaProcessor_->applyDelta(sourceData, deltaData, targetData,
        [this](uint64_t current, uint64_t total) {
            int progress = 40 + static_cast<int>((current * 20) / total);
            setProgress(progress, "Applying delta");
        });

    if (deltaResult != firmware::DeltaResult::SUCCESS) {
        conflictReason_ = "Delta application failed";
        return false;
    }

    setProgress(60, "Writing patched firmware");

    // Write to inactive partition
    auto inactiveSlot = partitionManager->getInactiveSlot();
    auto inactivePartition = partitionManager->getPartition(inactiveSlot);
    
    if (!inactivePartition) {
        conflictReason_ = "Inactive partition not found";
        return false;
    }

    result = flashManager->write(inactivePartition->device, 0, targetData,
        [this](uint64_t current, uint64_t total) {
            int progress = 60 + static_cast<int>((current * 20) / total);
            setProgress(progress, "Writing firmware");
        });

    if (result != platform::PlatformResult::SUCCESS) {
        conflictReason_ = "Failed to write firmware";
        return false;
    }

    setProgress(80, "Switching partitions");

    // Mark and switch
    partitionManager->setSlotBootable(inactiveSlot, true);
    partitionManager->switchSlot(inactiveSlot);

    return true;
}

bool AdvancedFirmwareUpdateObject::finalizeUpdate() {
    auto bootloader = platform::PlatformFactory::getBootloaderControl();
    
    if (bootloader) {
        // Set boot attempts for fallback
        auto partitionManager = platform::PlatformFactory::getPartitionManager();
        if (partitionManager) {
            auto inactiveSlot = partitionManager->getInactiveSlot();
            bootloader->setBootAttempts(inactiveSlot, 3);
        }
    }

    // Update version info
    targetVersion_ = packageVersion_;

    return true;
}

bool AdvancedFirmwareUpdateObject::cancelUpdate() {
    cancelRequested_ = true;
    
    if (state_ == FirmwareState::DOWNLOADING) {
        // Download will check cancel flag
        return true;
    }
    
    if (state_ == FirmwareState::UPDATING || 
        state_ == FirmwareState::APPLYING_DELTA) {
        // Cannot cancel mid-update safely
        conflictReason_ = "Cannot cancel update in progress";
        return false;
    }

    setState(FirmwareState::IDLE);
    setResult(UpdateResult::DEFAULT);
    receivedData_.clear();
    
    return true;
}

// ============================================================================
// Delta Update Operations
// ============================================================================

bool AdvancedFirmwareUpdateObject::startDeltaUpdate(const std::string& deltaUri) {
    return startDownload(deltaUri, true);
}

firmware::DeltaResult AdvancedFirmwareUpdateObject::validateDelta() {
    if (receivedData_.empty()) {
        return firmware::DeltaResult::ERROR_INVALID_INPUT;
    }

    if (!deltaProcessor_) {
        deltaProcessor_ = firmware::DeltaAlgorithmFactory::create(deltaAlgorithm_);
    }

    auto partitionManager = platform::PlatformFactory::getPartitionManager();
    if (!partitionManager) {
        return firmware::DeltaResult::ERROR_UNKNOWN;
    }

    auto activePartition = partitionManager->getPartition(
        partitionManager->getActiveSlot());
    
    if (!activePartition) {
        return firmware::DeltaResult::ERROR_UNKNOWN;
    }

    return deltaProcessor_->validateDelta(receivedData_, activePartition->usedSize);
}

bool AdvancedFirmwareUpdateObject::applyDeltaPackage(
    const std::vector<uint8_t>& deltaData) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    receivedData_ = deltaData;
    
    mutex_.unlock();
    return startUpdate();
}

void AdvancedFirmwareUpdateObject::setDeltaAlgorithm(
    firmware::DeltaAlgorithm algorithm) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    deltaAlgorithm_ = algorithm;
    deltaProcessor_ = firmware::DeltaAlgorithmFactory::create(algorithm);
}

// ============================================================================
// Rollback Operations
// ============================================================================

void AdvancedFirmwareUpdateObject::enableRollback(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    rollbackEnabled_ = enabled;
    
    if (enabled && !rollbackManager_) {
        firmware::RollbackManager::Config config;
        config.snapshotStoragePath = config_.backupPath;
        rollbackManager_ = std::make_shared<firmware::RollbackManager>(config);
        rollbackManager_->initialize();
    }
}

bool AdvancedFirmwareUpdateObject::isRollbackAvailable() const {
    if (!rollbackEnabled_ || !rollbackManager_) {
        return false;
    }
    
    return rollbackManager_->isSlotBootable(rollbackManager_->getInactiveSlot());
}

bool AdvancedFirmwareUpdateObject::rollback(const std::string& targetVersion) {
    if (!rollbackManager_) {
        conflictReason_ = "Rollback not available";
        return false;
    }

    setState(FirmwareState::UPDATING);
    setProgress(0, "Starting rollback");

    auto result = rollbackManager_->rollback(targetVersion,
        [this](const std::string& operation, int percent) {
            setProgress(percent, operation);
        });

    if (result != firmware::RollbackResult::SUCCESS) {
        setState(FirmwareState::IDLE);
        setResult(UpdateResult::ROLLBACK_FAILED);
        conflictReason_ = rollbackManager_->getLastError();
        return false;
    }

    setResult(UpdateResult::SUCCESS);
    setState(FirmwareState::WAITING_FOR_REBOOT);

    // Reboot to complete rollback
    if (config_.autoReboot) {
        auto systemManager = platform::PlatformFactory::getSystemManager();
        if (systemManager) {
            systemManager->reboot(config_.rebootDelay);
        }
    }

    return true;
}

// ============================================================================
// Block-wise Transfer
// ============================================================================

bool AdvancedFirmwareUpdateObject::handleBlock(uint32_t blockNum,
                                                 const std::vector<uint8_t>& data,
                                                 bool moreBlocks) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (blockNum == 0) {
        receivedData_.clear();
        setState(FirmwareState::DOWNLOADING);
    }

    // Verify block order
    if (blockNum != lastBlockNum_ + 1 && blockNum != 0) {
        conflictReason_ = "Block out of order";
        return false;
    }

    receivedData_.insert(receivedData_.end(), data.begin(), data.end());
    lastBlockNum_ = blockNum;

    if (!moreBlocks) {
        // Transfer complete
        setState(FirmwareState::DOWNLOADED);
        setProgress(100, "Transfer complete");
    }

    return true;
}

void AdvancedFirmwareUpdateObject::setBlockSize(uint32_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    blockSize_ = size;
    
    if (blockTransfer_) {
        blockTransfer_->setPreferredBlockSize(
            static_cast<transport::BlockwiseTransfer::BlockSize>(size));
    }
}

// ============================================================================
// Component Management
// ============================================================================

void AdvancedFirmwareUpdateObject::registerComponent(
    const FirmwareComponent& component) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Replace if exists
    auto it = std::find_if(components_.begin(), components_.end(),
        [&component](const FirmwareComponent& c) {
            return c.name == component.name;
        });
    
    if (it != components_.end()) {
        *it = component;
    } else {
        components_.push_back(component);
    }
}

std::vector<FirmwareComponent> AdvancedFirmwareUpdateObject::getComponents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return components_;
}

std::string AdvancedFirmwareUpdateObject::getComponentsJson() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "[";
    
    for (size_t i = 0; i < components_.size(); i++) {
        const auto& comp = components_[i];
        ss << "{";
        ss << "\"name\":\"" << comp.name << "\",";
        ss << "\"version\":\"" << comp.version << "\",";
        ss << "\"size\":" << comp.size << ",";
        ss << "\"mandatory\":" << (comp.mandatory ? "true" : "false") << ",";
        ss << "\"checksum\":\"" << bytesToHex(comp.checksum) << "\"";
        ss << "}" << (i < components_.size() - 1 ? "," : "");
    }
    
    ss << "]";
    return ss.str();
}

// ============================================================================
// Callbacks
// ============================================================================

void AdvancedFirmwareUpdateObject::setStateChangeCallback(
    StateChangeCallback callback) {
    stateChangeCallback_ = callback;
}

void AdvancedFirmwareUpdateObject::setProgressCallback(
    ProgressCallback callback) {
    progressCallback_ = callback;
}

void AdvancedFirmwareUpdateObject::setUpdateCompleteCallback(
    UpdateCompleteCallback callback) {
    updateCompleteCallback_ = callback;
}

// ============================================================================
// Private Methods
// ============================================================================

void AdvancedFirmwareUpdateObject::setState(FirmwareState newState) {
    FirmwareState oldState = state_.exchange(newState);
    lastStateChange_ = std::chrono::system_clock::now();
    
    if (stateChangeCallback_ && oldState != newState) {
        stateChangeCallback_(oldState, newState);
    }
}

void AdvancedFirmwareUpdateObject::setResult(UpdateResult result) {
    updateResult_ = result;
}

void AdvancedFirmwareUpdateObject::setProgress(int progress, 
                                                 const std::string& status) {
    progress_ = progress;
    
    if (progressCallback_) {
        progressCallback_(progress, status);
    }
}

std::string AdvancedFirmwareUpdateObject::generateTempPath() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    return config_.downloadPath + "/fw_" + std::to_string(timestamp);
}

bool AdvancedFirmwareUpdateObject::saveToFile(const std::string& path,
                                                const std::vector<uint8_t>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

bool AdvancedFirmwareUpdateObject::loadFromFile(const std::string& path,
                                                  std::vector<uint8_t>& data) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }
    
    size_t size = file.tellg();
    file.seekg(0);
    
    data.resize(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    return file.good();
}

} // namespace objects
} // namespace lwm2m
