/**
 * @file rollback_manager.h
 * @brief Complete Rollback and Recovery Manager for Friendly LwM2M Client
 * 
 * Provides comprehensive firmware rollback capabilities including:
 * - A/B partition management with atomic slot switching
 * - Snapshot creation and persistent storage
 * - Multi-component rollback coordination
 * - Recovery mode management
 * - Boot success verification
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_ROLLBACK_MANAGER_H
#define FRIENDLY_LWM2M_ROLLBACK_MANAGER_H

#include "platform/platform_abstraction.h"

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <mutex>
#include <functional>
#include <optional>
#include <fstream>

namespace lwm2m {
namespace firmware {

/**
 * @brief Result codes for rollback operations
 */
enum class RollbackResult {
    SUCCESS = 0,
    ERROR_NOT_INITIALIZED,
    ERROR_SNAPSHOT_NOT_FOUND,
    ERROR_SNAPSHOT_CORRUPTED,
    ERROR_PARTITION_NOT_BOOTABLE,
    ERROR_PARTITION_WRITE_FAILED,
    ERROR_SLOT_SWITCH_FAILED,
    ERROR_RECOVERY_FAILED,
    ERROR_COMPONENT_MISMATCH,
    ERROR_VERSION_MISMATCH,
    ERROR_STORAGE_FULL,
    ERROR_IO_FAILURE,
    ERROR_PLATFORM_ERROR,
    ERROR_UNKNOWN
};

/**
 * @brief Firmware component information
 */
struct ComponentInfo {
    std::string name;               ///< Component name (e.g., "kernel", "rootfs")
    std::string version;            ///< Semantic version string
    std::string device;             ///< Device/partition path
    uint64_t size;                  ///< Component size in bytes
    std::vector<uint8_t> checksum;  ///< SHA256 checksum
    bool mandatory;                 ///< Component is mandatory for boot
    int updatePriority;             ///< Update order priority (lower = first)
    std::map<std::string, std::string> metadata;  ///< Additional metadata
};

/**
 * @brief Firmware snapshot structure
 */
struct FirmwareSnapshot {
    std::string id;                             ///< Unique snapshot ID
    std::string version;                        ///< Overall firmware version
    std::chrono::system_clock::time_point timestamp;  ///< Creation timestamp
    platform::PartitionSlot slot;               ///< Source partition slot
    std::vector<ComponentInfo> components;      ///< Component information
    std::vector<uint8_t> stateChecksum;         ///< Overall state checksum
    std::string description;                    ///< User-provided description
    bool verified;                              ///< Snapshot has been verified
    std::map<std::string, std::string> metadata;  ///< Additional metadata
};

/**
 * @brief Rollback event types
 */
enum class RollbackEvent {
    SNAPSHOT_CREATED,
    SNAPSHOT_DELETED,
    ROLLBACK_STARTED,
    ROLLBACK_COMPLETED,
    ROLLBACK_FAILED,
    SLOT_SWITCHED,
    RECOVERY_ENTERED,
    RECOVERY_EXITED,
    BOOT_VERIFIED
};

/**
 * @brief Callback for rollback events
 */
using RollbackEventCallback = std::function<void(RollbackEvent event, 
                                                  const std::string& details)>;

/**
 * @brief Callback for rollback progress
 */
using RollbackProgressCallback = std::function<void(const std::string& operation,
                                                     int percentComplete)>;

/**
 * @brief Complete Rollback Manager Implementation
 */
class RollbackManager {
public:
    /**
     * @brief Configuration for rollback manager
     */
    struct Config {
        std::string snapshotStoragePath = "/var/lib/lwm2m/snapshots";
        size_t maxSnapshots = 5;
        bool autoSnapshot = true;          ///< Auto-create snapshot before updates
        bool autoRollbackOnFailure = true; ///< Auto-rollback on boot failure
        int maxBootAttempts = 3;           ///< Max boot attempts before rollback
        bool verifyAfterRollback = true;   ///< Verify partition after rollback
        std::chrono::seconds bootVerificationTimeout{120};  ///< Time to verify boot

        Config() = default;
    };

    /**
     * @brief Constructor
     * @param config Configuration settings
     */
    explicit RollbackManager(const Config& config = Config());

    /**
     * @brief Destructor
     */
    ~RollbackManager();

    // Prevent copying
    RollbackManager(const RollbackManager&) = delete;
    RollbackManager& operator=(const RollbackManager&) = delete;

    /**
     * @brief Initialize the rollback manager
     * @return Result code
     */
    RollbackResult initialize();

    /**
     * @brief Check if manager is initialized
     * @return true if initialized
     */
    bool isInitialized() const { return initialized_; }

    // ========================================================================
    // Snapshot Management
    // ========================================================================

    /**
     * @brief Create a snapshot of current firmware state
     * @param version Version string for the snapshot
     * @param description Optional description
     * @return Result code
     */
    RollbackResult createSnapshot(const std::string& version,
                                   const std::string& description = "");

    /**
     * @brief Get all available snapshots
     * @return Vector of snapshot information
     */
    std::vector<FirmwareSnapshot> getAvailableSnapshots() const;

    /**
     * @brief Get a specific snapshot by ID or version
     * @param idOrVersion Snapshot ID or version string
     * @return Snapshot if found
     */
    std::optional<FirmwareSnapshot> getSnapshot(const std::string& idOrVersion) const;

    /**
     * @brief Delete a snapshot
     * @param idOrVersion Snapshot ID or version to delete
     * @return Result code
     */
    RollbackResult deleteSnapshot(const std::string& idOrVersion);

    /**
     * @brief Verify snapshot integrity
     * @param idOrVersion Snapshot ID or version to verify
     * @return Result code
     */
    RollbackResult verifySnapshot(const std::string& idOrVersion);

    /**
     * @brief Prune old snapshots to stay within limit
     * @param keepCount Number of snapshots to keep
     * @return Number of snapshots deleted
     */
    int pruneSnapshots(size_t keepCount = 0);

    // ========================================================================
    // A/B Partition Management
    // ========================================================================

    /**
     * @brief Get currently active partition slot
     * @return Active slot identifier
     */
    platform::PartitionSlot getActiveSlot() const;

    /**
     * @brief Get inactive partition slot (for updates)
     * @return Inactive slot identifier
     */
    platform::PartitionSlot getInactiveSlot() const;

    /**
     * @brief Check if a slot is bootable
     * @param slot Slot to check
     * @return true if bootable
     */
    bool isSlotBootable(platform::PartitionSlot slot) const;

    /**
     * @brief Mark a slot as bootable or not
     * @param slot Slot to modify
     * @param bootable Bootable status
     * @return Result code
     */
    RollbackResult setSlotBootable(platform::PartitionSlot slot, bool bootable);

    /**
     * @brief Switch to a different boot slot
     * @param targetSlot Target slot to switch to
     * @return Result code
     */
    RollbackResult switchSlot(platform::PartitionSlot targetSlot);

    /**
     * @brief Atomically switch slots (switch and reboot)
     * @param targetSlot Target slot
     * @param rebootDelay Delay before reboot in seconds
     * @return Result code
     */
    RollbackResult atomicSlotSwitch(platform::PartitionSlot targetSlot,
                                     int rebootDelay = 3);

    /**
     * @brief Get partition information for a slot
     * @param slot Slot to query
     * @return Partition info if available
     */
    std::optional<platform::PartitionInfo> getPartitionInfo(
        platform::PartitionSlot slot) const;

    // ========================================================================
    // Rollback Operations
    // ========================================================================

    /**
     * @brief Rollback to a specific version
     * @param targetVersion Version to rollback to (empty = previous)
     * @param progress Progress callback
     * @return Result code
     */
    RollbackResult rollback(const std::string& targetVersion = "",
                            RollbackProgressCallback progress = nullptr);

    /**
     * @brief Rollback to the inactive slot
     * @param progress Progress callback
     * @return Result code
     */
    RollbackResult rollbackToInactiveSlot(RollbackProgressCallback progress = nullptr);

    /**
     * @brief Rollback specific components
     * @param componentNames Components to rollback
     * @param targetVersion Target version
     * @param progress Progress callback
     * @return Result code
     */
    RollbackResult rollbackComponents(const std::vector<std::string>& componentNames,
                                       const std::string& targetVersion,
                                       RollbackProgressCallback progress = nullptr);

    /**
     * @brief Cancel an in-progress rollback
     * @return Result code
     */
    RollbackResult cancelRollback();

    /**
     * @brief Check if rollback is in progress
     * @return true if rollback is active
     */
    bool isRollbackInProgress() const { return rollbackInProgress_; }

    // ========================================================================
    // Recovery Mode
    // ========================================================================

    /**
     * @brief Enter recovery mode
     * @return Result code
     */
    RollbackResult enterRecoveryMode();

    /**
     * @brief Exit recovery mode and boot normally
     * @return Result code
     */
    RollbackResult exitRecoveryMode();

    /**
     * @brief Check if currently in recovery mode
     * @return true if in recovery mode
     */
    bool isRecoveryMode() const;

    // ========================================================================
    // Boot Verification
    // ========================================================================

    /**
     * @brief Mark current boot as successful
     * @return Result code
     */
    RollbackResult markBootSuccessful();

    /**
     * @brief Get remaining boot attempts for current slot
     * @return Number of attempts remaining
     */
    int getRemainingBootAttempts() const;

    /**
     * @brief Check if boot verification is pending
     * @return true if verification needed
     */
    bool isBootVerificationPending() const;

    /**
     * @brief Start boot verification timer
     * @param callback Callback if verification times out
     * @return Result code
     */
    RollbackResult startBootVerification(std::function<void()> timeoutCallback = nullptr);

    // ========================================================================
    // Component Management
    // ========================================================================

    /**
     * @brief Register a firmware component
     * @param component Component information
     * @return Result code
     */
    RollbackResult registerComponent(const ComponentInfo& component);

    /**
     * @brief Get registered components
     * @return Vector of component info
     */
    std::vector<ComponentInfo> getRegisteredComponents() const;

    /**
     * @brief Update component version info
     * @param name Component name
     * @param newVersion New version string
     * @param newChecksum New checksum
     * @return Result code
     */
    RollbackResult updateComponentVersion(const std::string& name,
                                           const std::string& newVersion,
                                           const std::vector<uint8_t>& newChecksum);

    // ========================================================================
    // Event Handling
    // ========================================================================

    /**
     * @brief Register event callback
     * @param callback Callback function
     */
    void setEventCallback(RollbackEventCallback callback);

    /**
     * @brief Get last error message
     * @return Error message string
     */
    std::string getLastError() const { return lastError_; }

private:
    // Internal methods
    RollbackResult loadSnapshots();
    RollbackResult saveSnapshots();
    RollbackResult saveSnapshot(const FirmwareSnapshot& snapshot);
    std::string generateSnapshotId();
    std::vector<uint8_t> calculateStateChecksum();
    void emitEvent(RollbackEvent event, const std::string& details = "");
    RollbackResult platformResultToRollbackResult(platform::PlatformResult result);
    std::string serializeSnapshot(const FirmwareSnapshot& snapshot);
    std::optional<FirmwareSnapshot> deserializeSnapshot(const std::string& data);

    // Configuration
    Config config_;

    // Platform managers
    std::shared_ptr<platform::IPartitionManager> partitionManager_;
    std::shared_ptr<platform::IFlashManager> flashManager_;
    std::shared_ptr<platform::IBootloaderControl> bootloaderControl_;
    std::shared_ptr<platform::ISystemManager> systemManager_;

    // State
    bool initialized_ = false;
    bool rollbackInProgress_ = false;
    std::vector<FirmwareSnapshot> snapshots_;
    std::vector<ComponentInfo> components_;
    std::string lastError_;

    // Event callback
    RollbackEventCallback eventCallback_;

    // Thread safety
    mutable std::mutex mutex_;
};

} // namespace firmware
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_ROLLBACK_MANAGER_H
