/**
 * @file advanced_firmware_update_object.h
 * @brief OMA LwM2M Advanced Firmware Update Object (33405)
 * 
 * Custom object extending standard firmware update (Object 5) with:
 * - Delta firmware update support (BSDIFF, VCDIFF, Courgette)
 * - Multi-component firmware management
 * - A/B partition rollback support
 * - Resumable block-wise transfers
 * - Update progress tracking
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_ADVANCED_FIRMWARE_UPDATE_OBJECT_H
#define FRIENDLY_LWM2M_ADVANCED_FIRMWARE_UPDATE_OBJECT_H

#include "firmware/delta_algorithms.h"
#include "firmware/rollback_manager.h"
#include "transport/blockwise_transfer.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>
#include <map>

namespace lwm2m {
namespace objects {

/**
 * @brief Advanced Firmware Update Object ID
 */
constexpr uint16_t ADVANCED_FIRMWARE_UPDATE_OBJECT_ID = 33405;

/**
 * @brief Resource IDs for Advanced Firmware Update Object
 */
enum class FirmwareResourceId : uint16_t {
    PACKAGE = 0,                    ///< Firmware package (Opaque, W)
    PACKAGE_URI = 1,                ///< Package download URI (String, W)
    UPDATE = 2,                     ///< Execute update (Exec)
    STATE = 3,                      ///< Update state (Integer, R)
    UPDATE_RESULT = 4,              ///< Update result code (Integer, R)
    PKG_NAME = 5,                   ///< Package name (String, R)
    PKG_VERSION = 6,                ///< Package version (String, R)
    PROTOCOL_SUPPORT = 7,           ///< Supported protocols (Integer, R)
    DELIVERY_METHOD = 8,            ///< Delivery method (Integer, R)
    CANCEL = 9,                     ///< Cancel update (Exec)
    
    // Delta update resources (LwM2M v1.2.2)
    DELTA_PACKAGE = 10,             ///< Delta package (Opaque, W)
    DELTA_PACKAGE_URI = 11,         ///< Delta package URI (String, W)
    DELTA_ALGORITHM = 12,           ///< Delta algorithm (Integer, RW)
    BLOCK_SIZE = 13,                ///< Block size for transfer (Integer, RW)
    DIFFERENTIAL_DOWNLOAD = 14,     ///< Enable differential download (Boolean, RW)
    ROLLBACK_SUPPORT = 15,          ///< Rollback capability (Boolean, RW)
    UPDATE_PROGRESS = 16,           ///< Update progress 0-100 (Integer, R)
    COMPONENT_LIST = 17,            ///< Components JSON (String, R)
    SOURCE_VERSION = 18,            ///< Source firmware version (String, R)
    TARGET_VERSION = 19,            ///< Target firmware version (String, R)
    
    // Extended resources
    CHECKSUM = 20,                  ///< Expected SHA256 checksum (Opaque, W)
    CHECKSUM_TYPE = 21,             ///< Checksum algorithm (Integer, W)
    MAX_DEFERRED_PERIOD = 22,       ///< Max deferral time (Integer, RW)
    SEVERITY = 23,                  ///< Update severity (Integer, R)
    LAST_STATE_CHANGE = 24,         ///< Timestamp of last state change (Time, R)
    MAXIMUM_BLOCK_SIZE = 25,        ///< Maximum supported block size (Integer, R)
    CONFLICT_REASON = 26            ///< Conflict/error details (String, R)
};

/**
 * @brief Firmware update states
 */
enum class FirmwareState : uint8_t {
    IDLE = 0,                       ///< No update in progress
    DOWNLOADING = 1,                ///< Download in progress
    DOWNLOADED = 2,                 ///< Download complete, ready to update
    UPDATING = 3,                   ///< Update in progress
    VALIDATING = 4,                 ///< Validating package
    APPLYING_DELTA = 5,             ///< Applying delta patch
    WAITING_FOR_REBOOT = 6          ///< Update applied, waiting for reboot
};

/**
 * @brief Update result codes
 */
enum class UpdateResult : uint8_t {
    DEFAULT = 0,                    ///< Initial state
    SUCCESS = 1,                    ///< Update successful
    NOT_ENOUGH_STORAGE = 2,         ///< Insufficient storage
    OUT_OF_MEMORY = 3,              ///< Out of memory during update
    CONNECTION_LOST = 4,            ///< Connection lost during download
    INTEGRITY_CHECK_FAILED = 5,     ///< Package integrity check failed
    UNSUPPORTED_PACKAGE_TYPE = 6,   ///< Package type not supported
    INVALID_URI = 7,                ///< Invalid package URI
    UPDATE_FAILED = 8,              ///< Update failed
    UNSUPPORTED_PROTOCOL = 9,       ///< Protocol not supported
    
    // Delta-specific results
    DELTA_SOURCE_MISMATCH = 10,     ///< Source version mismatch for delta
    DELTA_ALGORITHM_ERROR = 11,     ///< Delta algorithm error
    ROLLBACK_REQUIRED = 12,         ///< Update requires rollback
    ROLLBACK_FAILED = 13,           ///< Rollback operation failed
    DEFERRED = 14,                  ///< Update deferred by user
    CONFLICT = 15                   ///< Update conflict with current state
};

/**
 * @brief Supported protocol flags
 */
enum class ProtocolSupport : uint8_t {
    COAP = 0x01,
    COAPS = 0x02,
    HTTP = 0x04,
    HTTPS = 0x08,
    COAP_TCP = 0x10,
    COAP_TLS = 0x20
};

/**
 * @brief Delivery method flags
 */
enum class DeliveryMethod : uint8_t {
    PULL = 0,                       ///< Client pulls from server
    PUSH = 1,                       ///< Server pushes to client
    BOTH = 2                        ///< Both methods supported
};

/**
 * @brief Update severity levels
 */
enum class UpdateSeverity : uint8_t {
    MANDATORY = 0,                  ///< Must be applied
    RECOMMENDED = 1,                ///< Recommended update
    OPTIONAL = 2                    ///< Optional update
};

/**
 * @brief Firmware component information
 */
struct FirmwareComponent {
    std::string name;
    std::string version;
    std::string device;
    uint64_t size;
    std::vector<uint8_t> checksum;
    bool mandatory;
    int updateOrder;
};

/**
 * @brief Callback for state changes
 */
using StateChangeCallback = std::function<void(FirmwareState oldState, 
                                                FirmwareState newState)>;

/**
 * @brief Callback for update progress
 */
using ProgressCallback = std::function<void(int progress, 
                                             const std::string& status)>;

/**
 * @brief Callback for update completion
 */
using UpdateCompleteCallback = std::function<void(UpdateResult result,
                                                   const std::string& message)>;

/**
 * @brief Advanced Firmware Update Object Implementation
 */
class AdvancedFirmwareUpdateObject {
public:
    /**
     * @brief Configuration for firmware update object
     */
    struct Config {
        std::string downloadPath = "/tmp/firmware";
        std::string backupPath = "/var/lib/lwm2m/backup";
        size_t maxPackageSize = 256 * 1024 * 1024;  // 256MB
        uint32_t defaultBlockSize = 1024;
        bool autoReboot = true;
        int rebootDelay = 5;
        bool deltaEnabled = true;
        firmware::DeltaAlgorithm defaultDeltaAlgorithm = firmware::DeltaAlgorithm::BSDIFF;
        bool rollbackEnabled = true;
        int maxRollbackSnapshots = 3;
    };

    /**
     * @brief Constructor
     * @param instanceId Object instance ID
     * @param config Configuration
     */
    explicit AdvancedFirmwareUpdateObject(uint16_t instanceId = 0,
                                           const Config& config = Config{});

    /**
     * @brief Destructor
     */
    ~AdvancedFirmwareUpdateObject();

    // Prevent copying
    AdvancedFirmwareUpdateObject(const AdvancedFirmwareUpdateObject&) = delete;
    AdvancedFirmwareUpdateObject& operator=(const AdvancedFirmwareUpdateObject&) = delete;

    /**
     * @brief Initialize the firmware update object
     * @return true on success
     */
    bool initialize();

    // ========================================================================
    // LwM2M Resource Operations
    // ========================================================================

    /**
     * @brief Read resource value
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(FirmwareResourceId resourceId, std::string& value);
    bool read(FirmwareResourceId resourceId, int64_t& value);
    bool read(FirmwareResourceId resourceId, std::vector<uint8_t>& value);

    /**
     * @brief Write resource value
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(FirmwareResourceId resourceId, const std::string& value);
    bool write(FirmwareResourceId resourceId, int64_t value);
    bool write(FirmwareResourceId resourceId, const std::vector<uint8_t>& value);

    /**
     * @brief Execute resource
     * @param resourceId Resource ID
     * @param arguments Optional arguments
     * @return true on success
     */
    bool execute(FirmwareResourceId resourceId, 
                 const std::string& arguments = "");

    // ========================================================================
    // Firmware Update Operations
    // ========================================================================

    /**
     * @brief Start firmware download from URI
     * @param uri Package URI
     * @param isDelta true if delta package
     * @return true if download started
     */
    bool startDownload(const std::string& uri, bool isDelta = false);

    /**
     * @brief Receive firmware package data (push mode)
     * @param data Package data chunk
     * @param offset Offset in package
     * @param isDelta true if delta package
     * @return true on success
     */
    bool receivePackageData(const std::vector<uint8_t>& data, 
                            uint64_t offset,
                            bool isDelta = false);

    /**
     * @brief Start firmware update
     * @return true if update started
     */
    bool startUpdate();

    /**
     * @brief Cancel ongoing update
     * @return true if cancelled
     */
    bool cancelUpdate();

    /**
     * @brief Get current state
     * @return Current firmware state
     */
    FirmwareState getState() const { return state_; }

    /**
     * @brief Get update result
     * @return Last update result
     */
    UpdateResult getUpdateResult() const { return updateResult_; }

    /**
     * @brief Get update progress (0-100)
     * @return Progress percentage
     */
    int getProgress() const { return progress_; }

    /**
     * @brief Get package name
     * @return Package name
     */
    std::string getPackageName() const { return packageName_; }

    /**
     * @brief Get package version
     * @return Package version
     */
    std::string getPackageVersion() const { return packageVersion_; }

    // ========================================================================
    // Delta Update Operations
    // ========================================================================

    /**
     * @brief Start delta update
     * @param deltaUri URI of delta package
     * @return true if started
     */
    bool startDeltaUpdate(const std::string& deltaUri);

    /**
     * @brief Validate delta package
     * @return Validation result
     */
    firmware::DeltaResult validateDelta();

    /**
     * @brief Apply delta package
     * @param deltaData Delta package data
     * @return true on success
     */
    bool applyDeltaPackage(const std::vector<uint8_t>& deltaData);

    /**
     * @brief Set delta algorithm
     * @param algorithm Algorithm to use
     */
    void setDeltaAlgorithm(firmware::DeltaAlgorithm algorithm);

    /**
     * @brief Get delta algorithm
     * @return Current algorithm
     */
    firmware::DeltaAlgorithm getDeltaAlgorithm() const { return deltaAlgorithm_; }

    // ========================================================================
    // Rollback Operations
    // ========================================================================

    /**
     * @brief Enable/disable rollback support
     * @param enabled Enable state
     */
    void enableRollback(bool enabled);

    /**
     * @brief Check if rollback is available
     * @return true if rollback possible
     */
    bool isRollbackAvailable() const;

    /**
     * @brief Perform rollback
     * @param targetVersion Version to rollback to (empty = previous)
     * @return true if successful
     */
    bool rollback(const std::string& targetVersion = "");

    /**
     * @brief Get rollback manager
     * @return Shared pointer to rollback manager
     */
    std::shared_ptr<firmware::RollbackManager> getRollbackManager() const {
        return rollbackManager_;
    }

    // ========================================================================
    // Block-wise Transfer
    // ========================================================================

    /**
     * @brief Handle incoming block
     * @param blockNum Block number
     * @param data Block data
     * @param moreBlocks More blocks expected
     * @return true on success
     */
    bool handleBlock(uint32_t blockNum, const std::vector<uint8_t>& data, 
                     bool moreBlocks);

    /**
     * @brief Set preferred block size
     * @param size Block size in bytes
     */
    void setBlockSize(uint32_t size);

    /**
     * @brief Get current block size
     * @return Block size
     */
    uint32_t getBlockSize() const { return blockSize_; }

    // ========================================================================
    // Component Management
    // ========================================================================

    /**
     * @brief Register firmware component
     * @param component Component information
     */
    void registerComponent(const FirmwareComponent& component);

    /**
     * @brief Get registered components
     * @return Vector of components
     */
    std::vector<FirmwareComponent> getComponents() const;

    /**
     * @brief Get components as JSON string
     * @return JSON representation
     */
    std::string getComponentsJson() const;

    // ========================================================================
    // Callbacks
    // ========================================================================

    /**
     * @brief Set state change callback
     * @param callback Callback function
     */
    void setStateChangeCallback(StateChangeCallback callback);

    /**
     * @brief Set progress callback
     * @param callback Callback function
     */
    void setProgressCallback(ProgressCallback callback);

    /**
     * @brief Set update complete callback
     * @param callback Callback function
     */
    void setUpdateCompleteCallback(UpdateCompleteCallback callback);

    // ========================================================================
    // Object Information
    // ========================================================================

    /**
     * @brief Get object ID
     * @return Object ID (33405)
     */
    static uint16_t getObjectId() { return ADVANCED_FIRMWARE_UPDATE_OBJECT_ID; }

    /**
     * @brief Get instance ID
     * @return Instance ID
     */
    uint16_t getInstanceId() const { return instanceId_; }

private:
    // State management
    void setState(FirmwareState newState);
    void setResult(UpdateResult result);
    void setProgress(int progress, const std::string& status = "");

    // Download handling
    void downloadThread(const std::string& uri, bool isDelta);
    bool downloadFromUri(const std::string& uri, std::vector<uint8_t>& data);

    // Update processing
    void updateThread();
    bool validatePackage(const std::vector<uint8_t>& data);
    bool applyFullUpdate(const std::vector<uint8_t>& data);
    bool applyDeltaUpdate(const std::vector<uint8_t>& data);
    bool finalizeUpdate();

    // Utility methods
    std::string generateTempPath();
    bool saveToFile(const std::string& path, const std::vector<uint8_t>& data);
    bool loadFromFile(const std::string& path, std::vector<uint8_t>& data);

    // Configuration
    Config config_;
    uint16_t instanceId_;

    // State
    std::atomic<FirmwareState> state_{FirmwareState::IDLE};
    std::atomic<UpdateResult> updateResult_{UpdateResult::DEFAULT};
    std::atomic<int> progress_{0};
    std::atomic<bool> cancelRequested_{false};

    // Package information
    std::string packageName_;
    std::string packageVersion_;
    std::string packageUri_;
    std::string deltaPackageUri_;
    std::string sourceVersion_;
    std::string targetVersion_;
    std::vector<uint8_t> expectedChecksum_;
    UpdateSeverity severity_ = UpdateSeverity::RECOMMENDED;
    std::chrono::system_clock::time_point lastStateChange_;
    std::string conflictReason_;

    // Delta update
    firmware::DeltaAlgorithm deltaAlgorithm_;
    bool differentialDownload_ = true;
    bool rollbackEnabled_ = true;
    std::unique_ptr<firmware::IDeltaAlgorithm> deltaProcessor_;

    // Block transfer
    uint32_t blockSize_ = 1024;
    std::vector<uint8_t> receivedData_;
    uint64_t expectedSize_ = 0;
    uint32_t lastBlockNum_ = 0;

    // Components
    std::vector<FirmwareComponent> components_;

    // Managers
    std::shared_ptr<firmware::RollbackManager> rollbackManager_;
    std::unique_ptr<transport::BlockwiseTransfer> blockTransfer_;

    // Callbacks
    StateChangeCallback stateChangeCallback_;
    ProgressCallback progressCallback_;
    UpdateCompleteCallback updateCompleteCallback_;

    // Threading
    std::unique_ptr<std::thread> workerThread_;
    mutable std::mutex mutex_;
};

} // namespace objects
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_ADVANCED_FIRMWARE_UPDATE_OBJECT_H
