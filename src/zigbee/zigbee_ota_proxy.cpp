/*
 * Zigbee OTA Upgrade Proxy Implementation
 */

#include "zigbee_ota_proxy.h"
#include "zigbee_coordinator.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <chrono>
#include <algorithm>

namespace zigbee {

// OTA Cluster Commands
constexpr uint8_t OTA_CMD_IMAGE_NOTIFY = 0x00;
constexpr uint8_t OTA_CMD_QUERY_NEXT_IMAGE_RESPONSE = 0x02;
constexpr uint8_t OTA_CMD_IMAGE_BLOCK_RESPONSE = 0x05;
constexpr uint8_t OTA_CMD_UPGRADE_END_RESPONSE = 0x07;

// OTA Status Codes
constexpr uint8_t OTA_STATUS_SUCCESS = 0x00;
constexpr uint8_t OTA_STATUS_FAILURE = 0x01;
constexpr uint8_t OTA_STATUS_NOT_AUTHORIZED = 0x7E;
constexpr uint8_t OTA_STATUS_INVALID_IMAGE = 0x96;
constexpr uint8_t OTA_STATUS_WAIT_FOR_DATA = 0x97;
constexpr uint8_t OTA_STATUS_NO_IMAGE_AVAILABLE = 0x98;
constexpr uint8_t OTA_STATUS_REQUIRE_MORE_IMAGE = 0x99;
constexpr uint8_t OTA_STATUS_ABORT = 0x95;

// OTA Cluster ID
constexpr uint16_t OTA_CLUSTER_ID = 0x0019;

// Helper function
static uint64_t getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

// ==============================================================================
// Constructor / Destructor
// ==============================================================================

ZigbeeOtaProxy::ZigbeeOtaProxy(ZigbeeCoordinator* coordinator)
    : coordinator_(coordinator),
      imageLoaded_(false),
      maxConcurrentUpgrades_(3),
      defaultBlockSize_(64),
      defaultRetryCount_(3) {
    std::cout << "[OTA-Proxy] Initialized" << std::endl;
}

ZigbeeOtaProxy::~ZigbeeOtaProxy() {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    activeSessions_.clear();
}

// ==============================================================================
// Image Management
// ==============================================================================

bool ZigbeeOtaProxy::loadImage(const std::string& imagePath) {
    std::cout << "[OTA-Proxy] Loading image: " << imagePath << std::endl;

    if (!parseImageHeader(imagePath)) {
        std::cerr << "[OTA-Proxy] Failed to parse image header" << std::endl;
        return false;
    }

    currentImagePath_ = imagePath;
    imageLoaded_ = true;

    std::cout << "[OTA-Proxy] Image loaded successfully" << std::endl;
    std::cout << "[OTA-Proxy]   Manufacturer: 0x" << std::hex << currentImageHeader_.manufacturerCode << std::endl;
    std::cout << "[OTA-Proxy]   Image Type: 0x" << std::hex << currentImageHeader_.imageType << std::endl;
    std::cout << "[OTA-Proxy]   File Version: 0x" << std::hex << currentImageHeader_.fileVersion << std::endl;
    std::cout << "[OTA-Proxy]   Size: " << std::dec << currentImageHeader_.totalImageSize << " bytes" << std::endl;

    return true;
}

bool ZigbeeOtaProxy::parseImageHeader(const std::string& imagePath) {
    std::ifstream file(imagePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[OTA-Proxy] Failed to open image file: " << imagePath << std::endl;
        return false;
    }

    // Read header
    uint8_t headerData[56];
    file.read(reinterpret_cast<char*>(headerData), sizeof(headerData));

    if (!file.good()) {
        std::cerr << "[OTA-Proxy] Failed to read image header" << std::endl;
        return false;
    }

    // Parse header fields
    size_t offset = 0;

    // Magic number (4 bytes)
    currentImageHeader_.magicNumber = *reinterpret_cast<uint32_t*>(&headerData[offset]);
    offset += 4;

    if (currentImageHeader_.magicNumber != OTA_MAGIC_NUMBER) {
        std::cerr << "[OTA-Proxy] Invalid magic number: 0x" << std::hex
                  << currentImageHeader_.magicNumber << std::endl;
        return false;
    }

    // Header version (2 bytes)
    currentImageHeader_.headerVersion = *reinterpret_cast<uint16_t*>(&headerData[offset]);
    offset += 2;

    // Header length (2 bytes)
    currentImageHeader_.headerLength = *reinterpret_cast<uint16_t*>(&headerData[offset]);
    offset += 2;

    // Field control (2 bytes)
    currentImageHeader_.fieldControl = *reinterpret_cast<uint16_t*>(&headerData[offset]);
    offset += 2;

    // Manufacturer code (2 bytes)
    currentImageHeader_.manufacturerCode = *reinterpret_cast<uint16_t*>(&headerData[offset]);
    offset += 2;

    // Image type (2 bytes)
    currentImageHeader_.imageType = *reinterpret_cast<uint16_t*>(&headerData[offset]);
    offset += 2;

    // File version (4 bytes)
    currentImageHeader_.fileVersion = *reinterpret_cast<uint32_t*>(&headerData[offset]);
    offset += 4;

    // Zigbee stack version (2 bytes)
    currentImageHeader_.zigbeeStackVersion = *reinterpret_cast<uint16_t*>(&headerData[offset]);
    offset += 2;

    // Header string (32 bytes)
    std::memcpy(currentImageHeader_.headerString, &headerData[offset], 32);
    offset += 32;

    // Total image size (4 bytes)
    currentImageHeader_.totalImageSize = *reinterpret_cast<uint32_t*>(&headerData[offset]);
    offset += 4;

    // Parse optional fields based on field control
    currentImageHeader_.hasSecurityCredentials = (currentImageHeader_.fieldControl & 0x01) != 0;
    currentImageHeader_.hasDeviceSpecificFile = (currentImageHeader_.fieldControl & 0x02) != 0;
    currentImageHeader_.hasHardwareVersions = (currentImageHeader_.fieldControl & 0x04) != 0;

    if (currentImageHeader_.hasHardwareVersions) {
        if (file.good()) {
            file.read(reinterpret_cast<char*>(&currentImageHeader_.minHardwareVersion), 2);
            file.read(reinterpret_cast<char*>(&currentImageHeader_.maxHardwareVersion), 2);
        }
    }

    file.close();
    return true;
}

// ==============================================================================
// Image Announcement
// ==============================================================================

bool ZigbeeOtaProxy::announceImage(uint16_t manufacturerCode, uint16_t imageType) {
    if (!imageLoaded_) {
        std::cerr << "[OTA-Proxy] No image loaded" << std::endl;
        return false;
    }

    std::cout << "[OTA-Proxy] Announcing image to all devices" << std::endl;

    // Build image notify command payload
    std::vector<uint8_t> payload;
    payload.push_back(0x00);  // Payload type: query jitter
    payload.push_back(100);   // Query jitter value

    // For now, we would broadcast this to all devices
    // In a real implementation, this would use broadcast addressing

    std::cout << "[OTA-Proxy] Image announcement sent" << std::endl;
    return true;
}

bool ZigbeeOtaProxy::announceImageToDevice(uint64_t ieeeAddress, uint8_t endpoint) {
    if (!imageLoaded_) {
        std::cerr << "[OTA-Proxy] No image loaded" << std::endl;
        return false;
    }

    std::cout << "[OTA-Proxy] Announcing image to device 0x" << std::hex << ieeeAddress << std::endl;

    // Build image notify command
    std::vector<uint8_t> payload;
    payload.push_back(0x03);  // Payload type: manufacturer, image type, and file version
    payload.push_back(100);   // Query jitter

    // Manufacturer code
    payload.push_back(currentImageHeader_.manufacturerCode & 0xFF);
    payload.push_back((currentImageHeader_.manufacturerCode >> 8) & 0xFF);

    // Image type
    payload.push_back(currentImageHeader_.imageType & 0xFF);
    payload.push_back((currentImageHeader_.imageType >> 8) & 0xFF);

    // File version
    payload.push_back(currentImageHeader_.fileVersion & 0xFF);
    payload.push_back((currentImageHeader_.fileVersion >> 8) & 0xFF);
    payload.push_back((currentImageHeader_.fileVersion >> 16) & 0xFF);
    payload.push_back((currentImageHeader_.fileVersion >> 24) & 0xFF);

    return coordinator_->sendZclCommand(ieeeAddress, endpoint, OTA_CLUSTER_ID, OTA_CMD_IMAGE_NOTIFY, payload);
}

// ==============================================================================
// Upgrade Management
// ==============================================================================

bool ZigbeeOtaProxy::startUpgrade(uint64_t ieeeAddress, uint8_t endpoint, const std::string& imagePath) {
    std::cout << "[OTA-Proxy] Starting upgrade for device 0x" << std::hex << ieeeAddress << std::endl;

    // Load image if not already loaded
    if (imagePath != currentImagePath_ || !imageLoaded_) {
        if (!loadImage(imagePath)) {
            return false;
        }
    }

    // Check concurrent upgrade limit
    {
        std::lock_guard<std::mutex> lock(sessionsMutex_);
        if (activeSessions_.size() >= maxConcurrentUpgrades_) {
            std::cerr << "[OTA-Proxy] Maximum concurrent upgrades reached" << std::endl;
            return false;
        }
    }

    // Create session
    OtaUpgradeSession* session = createSession(ieeeAddress, endpoint);
    if (!session) {
        return false;
    }

    session->imageHeader = currentImageHeader_;
    session->imagePath = imagePath;
    session->totalSize = currentImageHeader_.totalImageSize;
    session->status = OtaUpgradeStatus::ANNOUNCING;

    // Announce image to device
    return announceImageToDevice(ieeeAddress, endpoint);
}

bool ZigbeeOtaProxy::cancelUpgrade(uint64_t ieeeAddress) {
    std::cout << "[OTA-Proxy] Cancelling upgrade for device 0x" << std::hex << ieeeAddress << std::endl;

    std::lock_guard<std::mutex> lock(sessionsMutex_);
    auto it = activeSessions_.find(ieeeAddress);
    if (it == activeSessions_.end()) {
        return false;
    }

    if (errorCallback_) {
        errorCallback_(ieeeAddress, "Upgrade cancelled by user");
    }

    activeSessions_.erase(it);
    return true;
}

// ==============================================================================
// OTA Request Handlers
// ==============================================================================

bool ZigbeeOtaProxy::handleQueryNextImageRequest(uint64_t ieeeAddress, uint8_t endpoint,
                                                 uint16_t manufacturerCode, uint16_t imageType,
                                                 uint32_t currentFileVersion, uint16_t hardwareVersion) {
    std::cout << "[OTA-Proxy] Query next image request from device 0x" << std::hex << ieeeAddress << std::endl;
    std::cout << "[OTA-Proxy]   Manufacturer: 0x" << manufacturerCode << std::endl;
    std::cout << "[OTA-Proxy]   Image Type: 0x" << imageType << std::endl;
    std::cout << "[OTA-Proxy]   Current Version: 0x" << currentFileVersion << std::endl;

    if (!imageLoaded_) {
        std::cout << "[OTA-Proxy] No image available" << std::endl;
        return sendQueryNextImageResponse(ieeeAddress, endpoint, OTA_STATUS_NO_IMAGE_AVAILABLE);
    }

    // Validate image compatibility
    if (!validateImageCompatibility(manufacturerCode, imageType, hardwareVersion)) {
        std::cout << "[OTA-Proxy] Image not compatible with device" << std::endl;
        return sendQueryNextImageResponse(ieeeAddress, endpoint, OTA_STATUS_NO_IMAGE_AVAILABLE);
    }

    // Check if new version is available
    if (currentImageHeader_.fileVersion <= currentFileVersion) {
        std::cout << "[OTA-Proxy] Device already has latest version" << std::endl;
        return sendQueryNextImageResponse(ieeeAddress, endpoint, OTA_STATUS_NO_IMAGE_AVAILABLE);
    }

    // Create or update session
    OtaUpgradeSession* session = createSession(ieeeAddress, endpoint);
    if (!session) {
        return sendQueryNextImageResponse(ieeeAddress, endpoint, OTA_STATUS_FAILURE);
    }

    session->imageHeader = currentImageHeader_;
    session->imagePath = currentImagePath_;
    session->totalSize = currentImageHeader_.totalImageSize;
    session->currentOffset = 0;
    session->status = OtaUpgradeStatus::TRANSFERRING;

    std::cout << "[OTA-Proxy] Image available for device" << std::endl;
    return sendQueryNextImageResponse(ieeeAddress, endpoint, OTA_STATUS_SUCCESS);
}

bool ZigbeeOtaProxy::handleImageBlockRequest(uint64_t ieeeAddress, uint8_t endpoint,
                                             uint32_t fileOffset, uint8_t maxDataSize) {
    std::cout << "[OTA-Proxy] Image block request from device 0x" << std::hex << ieeeAddress
              << " offset: " << std::dec << fileOffset << std::endl;

    std::lock_guard<std::mutex> lock(sessionsMutex_);
    auto it = activeSessions_.find(ieeeAddress);
    if (it == activeSessions_.end()) {
        std::cerr << "[OTA-Proxy] No active session for device" << std::endl;
        return false;
    }

    OtaUpgradeSession& session = it->second;

    // Validate offset
    if (fileOffset >= session.totalSize) {
        std::cerr << "[OTA-Proxy] Invalid offset: " << fileOffset << std::endl;
        return sendImageBlockResponse(ieeeAddress, endpoint, OTA_STATUS_INVALID_IMAGE, fileOffset, {});
    }

    // Determine block size
    uint8_t blockSize = std::min(maxDataSize, session.blockSize);
    blockSize = std::min(blockSize, MAX_BLOCK_SIZE);
    blockSize = std::max(blockSize, MIN_BLOCK_SIZE);

    // Calculate actual data size for this block
    uint32_t remaining = session.totalSize - fileOffset;
    size_t dataSize = std::min(static_cast<uint32_t>(blockSize), remaining);

    // Read image block
    std::vector<uint8_t> blockData;
    if (!readImageBlock(fileOffset, dataSize, blockData)) {
        std::cerr << "[OTA-Proxy] Failed to read image block" << std::endl;
        return sendImageBlockResponse(ieeeAddress, endpoint, OTA_STATUS_FAILURE, fileOffset, {});
    }

    // Update session
    session.currentOffset = fileOffset + dataSize;
    session.status = OtaUpgradeStatus::TRANSFERRING;
    updateSessionActivity(ieeeAddress);

    // Report progress
    if (progressCallback_) {
        progressCallback_(ieeeAddress, session.currentOffset, session.totalSize);
    }

    std::cout << "[OTA-Proxy] Sending block: " << dataSize << " bytes, progress: "
              << (session.currentOffset * 100 / session.totalSize) << "%" << std::endl;

    return sendImageBlockResponse(ieeeAddress, endpoint, OTA_STATUS_SUCCESS, fileOffset, blockData);
}

bool ZigbeeOtaProxy::handleUpgradeEndRequest(uint64_t ieeeAddress, uint8_t endpoint, uint8_t status) {
    std::cout << "[OTA-Proxy] Upgrade end request from device 0x" << std::hex << ieeeAddress
              << " status: " << static_cast<int>(status) << std::endl;

    std::lock_guard<std::mutex> lock(sessionsMutex_);
    auto it = activeSessions_.find(ieeeAddress);
    if (it == activeSessions_.end()) {
        std::cerr << "[OTA-Proxy] No active session for device" << std::endl;
        return false;
    }

    OtaUpgradeSession& session = it->second;

    bool success = (status == OTA_STATUS_SUCCESS);

    if (success) {
        std::cout << "[OTA-Proxy] Upgrade completed successfully" << std::endl;
        session.status = OtaUpgradeStatus::COMPLETED;

        if (completeCallback_) {
            completeCallback_(ieeeAddress, true);
        }
    } else {
        std::cerr << "[OTA-Proxy] Upgrade failed with status: " << static_cast<int>(status) << std::endl;
        session.status = OtaUpgradeStatus::FAILED;

        if (completeCallback_) {
            completeCallback_(ieeeAddress, false);
        }

        if (errorCallback_) {
            errorCallback_(ieeeAddress, "Device reported upgrade failure");
        }
    }

    // Send response (device will reboot after receiving this)
    uint32_t upgradeTime = 0;  // Immediate upgrade
    sendUpgradeEndResponse(ieeeAddress, endpoint, OTA_STATUS_SUCCESS, upgradeTime);

    // Remove session after short delay
    activeSessions_.erase(it);

    return true;
}

// ==============================================================================
// Response Senders
// ==============================================================================

bool ZigbeeOtaProxy::sendQueryNextImageResponse(uint64_t ieeeAddress, uint8_t endpoint, uint8_t status) {
    std::vector<uint8_t> payload;
    payload.push_back(status);

    if (status == OTA_STATUS_SUCCESS) {
        // Add image information
        payload.push_back(currentImageHeader_.manufacturerCode & 0xFF);
        payload.push_back((currentImageHeader_.manufacturerCode >> 8) & 0xFF);

        payload.push_back(currentImageHeader_.imageType & 0xFF);
        payload.push_back((currentImageHeader_.imageType >> 8) & 0xFF);

        payload.push_back(currentImageHeader_.fileVersion & 0xFF);
        payload.push_back((currentImageHeader_.fileVersion >> 8) & 0xFF);
        payload.push_back((currentImageHeader_.fileVersion >> 16) & 0xFF);
        payload.push_back((currentImageHeader_.fileVersion >> 24) & 0xFF);

        payload.push_back(currentImageHeader_.totalImageSize & 0xFF);
        payload.push_back((currentImageHeader_.totalImageSize >> 8) & 0xFF);
        payload.push_back((currentImageHeader_.totalImageSize >> 16) & 0xFF);
        payload.push_back((currentImageHeader_.totalImageSize >> 24) & 0xFF);
    }

    return coordinator_->sendZclCommand(ieeeAddress, endpoint, OTA_CLUSTER_ID,
                                       OTA_CMD_QUERY_NEXT_IMAGE_RESPONSE, payload);
}

bool ZigbeeOtaProxy::sendImageBlockResponse(uint64_t ieeeAddress, uint8_t endpoint, uint8_t status,
                                            uint32_t fileOffset, const std::vector<uint8_t>& data) {
    std::vector<uint8_t> payload;
    payload.push_back(status);

    if (status == OTA_STATUS_SUCCESS && !data.empty()) {
        // Add manufacturer code
        payload.push_back(currentImageHeader_.manufacturerCode & 0xFF);
        payload.push_back((currentImageHeader_.manufacturerCode >> 8) & 0xFF);

        // Add image type
        payload.push_back(currentImageHeader_.imageType & 0xFF);
        payload.push_back((currentImageHeader_.imageType >> 8) & 0xFF);

        // Add file version
        payload.push_back(currentImageHeader_.fileVersion & 0xFF);
        payload.push_back((currentImageHeader_.fileVersion >> 8) & 0xFF);
        payload.push_back((currentImageHeader_.fileVersion >> 16) & 0xFF);
        payload.push_back((currentImageHeader_.fileVersion >> 24) & 0xFF);

        // Add file offset
        payload.push_back(fileOffset & 0xFF);
        payload.push_back((fileOffset >> 8) & 0xFF);
        payload.push_back((fileOffset >> 16) & 0xFF);
        payload.push_back((fileOffset >> 24) & 0xFF);

        // Add data size
        payload.push_back(data.size() & 0xFF);

        // Add image data
        payload.insert(payload.end(), data.begin(), data.end());
    }

    return coordinator_->sendZclCommand(ieeeAddress, endpoint, OTA_CLUSTER_ID,
                                       OTA_CMD_IMAGE_BLOCK_RESPONSE, payload);
}

bool ZigbeeOtaProxy::sendUpgradeEndResponse(uint64_t ieeeAddress, uint8_t endpoint,
                                           uint8_t status, uint32_t upgradeTime) {
    std::vector<uint8_t> payload;

    // Add manufacturer code
    payload.push_back(currentImageHeader_.manufacturerCode & 0xFF);
    payload.push_back((currentImageHeader_.manufacturerCode >> 8) & 0xFF);

    // Add image type
    payload.push_back(currentImageHeader_.imageType & 0xFF);
    payload.push_back((currentImageHeader_.imageType >> 8) & 0xFF);

    // Add file version
    payload.push_back(currentImageHeader_.fileVersion & 0xFF);
    payload.push_back((currentImageHeader_.fileVersion >> 8) & 0xFF);
    payload.push_back((currentImageHeader_.fileVersion >> 16) & 0xFF);
    payload.push_back((currentImageHeader_.fileVersion >> 24) & 0xFF);

    // Add current time (UTC)
    uint32_t currentTime = getCurrentTimestamp() / 1000;
    payload.push_back(currentTime & 0xFF);
    payload.push_back((currentTime >> 8) & 0xFF);
    payload.push_back((currentTime >> 16) & 0xFF);
    payload.push_back((currentTime >> 24) & 0xFF);

    // Add upgrade time
    payload.push_back(upgradeTime & 0xFF);
    payload.push_back((upgradeTime >> 8) & 0xFF);
    payload.push_back((upgradeTime >> 16) & 0xFF);
    payload.push_back((upgradeTime >> 24) & 0xFF);

    return coordinator_->sendZclCommand(ieeeAddress, endpoint, OTA_CLUSTER_ID,
                                       OTA_CMD_UPGRADE_END_RESPONSE, payload);
}

// ==============================================================================
// Session Management
// ==============================================================================

OtaUpgradeSession* ZigbeeOtaProxy::createSession(uint64_t ieeeAddress, uint8_t endpoint) {
    std::lock_guard<std::mutex> lock(sessionsMutex_);

    auto& session = activeSessions_[ieeeAddress];
    session.ieeeAddress = ieeeAddress;
    session.endpoint = endpoint;
    session.currentOffset = 0;
    session.status = OtaUpgradeStatus::IDLE;
    session.startTime = getCurrentTimestamp();
    session.lastActivityTime = session.startTime;
    session.retriesRemaining = defaultRetryCount_;
    session.blockSize = defaultBlockSize_;

    return &session;
}

void ZigbeeOtaProxy::removeSession(uint64_t ieeeAddress) {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    activeSessions_.erase(ieeeAddress);
}

void ZigbeeOtaProxy::updateSessionActivity(uint64_t ieeeAddress) {
    auto it = activeSessions_.find(ieeeAddress);
    if (it != activeSessions_.end()) {
        it->second.lastActivityTime = getCurrentTimestamp();
    }
}

void ZigbeeOtaProxy::cleanupStaleSessions() {
    std::lock_guard<std::mutex> lock(sessionsMutex_);

    uint64_t now = getCurrentTimestamp();
    auto it = activeSessions_.begin();

    while (it != activeSessions_.end()) {
        if (now - it->second.lastActivityTime > SESSION_TIMEOUT_MS) {
            std::cout << "[OTA-Proxy] Removing stale session for device 0x"
                      << std::hex << it->first << std::endl;

            if (errorCallback_) {
                errorCallback_(it->first, "Session timeout");
            }

            it = activeSessions_.erase(it);
        } else {
            ++it;
        }
    }
}

// ==============================================================================
// Utility Functions
// ==============================================================================

bool ZigbeeOtaProxy::readImageBlock(uint32_t offset, size_t size, std::vector<uint8_t>& data) {
    std::ifstream file(currentImagePath_, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(offset);
    if (!file.good()) {
        return false;
    }

    data.resize(size);
    file.read(reinterpret_cast<char*>(data.data()), size);

    if (!file.good() && !file.eof()) {
        return false;
    }

    return true;
}

bool ZigbeeOtaProxy::validateImageCompatibility(uint16_t manufacturerCode, uint16_t imageType,
                                                uint16_t hardwareVersion) {
    // Check manufacturer code
    if (currentImageHeader_.manufacturerCode != manufacturerCode &&
        currentImageHeader_.manufacturerCode != 0xFFFF) {
        return false;
    }

    // Check image type
    if (currentImageHeader_.imageType != imageType &&
        currentImageHeader_.imageType != 0xFFFF) {
        return false;
    }

    // Check hardware version if specified
    if (currentImageHeader_.hasHardwareVersions) {
        if (hardwareVersion < currentImageHeader_.minHardwareVersion ||
            hardwareVersion > currentImageHeader_.maxHardwareVersion) {
            return false;
        }
    }

    return true;
}

// ==============================================================================
// Getters / Setters
// ==============================================================================

std::vector<OtaUpgradeSession> ZigbeeOtaProxy::getActiveSessions() const {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    std::vector<OtaUpgradeSession> sessions;
    for (const auto& pair : activeSessions_) {
        sessions.push_back(pair.second);
    }
    return sessions;
}

const OtaUpgradeSession* ZigbeeOtaProxy::getSession(uint64_t ieeeAddress) const {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    auto it = activeSessions_.find(ieeeAddress);
    if (it != activeSessions_.end()) {
        return &it->second;
    }
    return nullptr;
}

OtaImageHeader ZigbeeOtaProxy::getImageHeader() const {
    return currentImageHeader_;
}

bool ZigbeeOtaProxy::isImageLoaded() const {
    return imageLoaded_;
}

void ZigbeeOtaProxy::onProgress(OtaProgressCallback callback) {
    progressCallback_ = callback;
}

void ZigbeeOtaProxy::onComplete(OtaCompleteCallback callback) {
    completeCallback_ = callback;
}

void ZigbeeOtaProxy::onError(OtaErrorCallback callback) {
    errorCallback_ = callback;
}

void ZigbeeOtaProxy::setMaxConcurrentUpgrades(size_t maxConcurrent) {
    maxConcurrentUpgrades_ = maxConcurrent;
}

void ZigbeeOtaProxy::setBlockSize(uint8_t blockSize) {
    defaultBlockSize_ = std::min(blockSize, MAX_BLOCK_SIZE);
    defaultBlockSize_ = std::max(defaultBlockSize_, MIN_BLOCK_SIZE);
}

void ZigbeeOtaProxy::setRetryCount(uint16_t retries) {
    defaultRetryCount_ = retries;
}

} // namespace zigbee
