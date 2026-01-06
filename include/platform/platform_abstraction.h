/**
 * @file platform_abstraction.h
 * @brief Platform Abstraction Layer for Friendly LwM2M Client
 * 
 * Provides abstract interfaces for platform-specific operations including
 * partition management, flash operations, bootloader control, and system
 * management. Concrete implementations exist for OpenWRT, generic Linux,
 * and other embedded platforms.
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_PLATFORM_ABSTRACTION_H
#define FRIENDLY_LWM2M_PLATFORM_ABSTRACTION_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <optional>

namespace lwm2m {
namespace platform {

/**
 * @brief Result codes for platform operations
 */
enum class PlatformResult {
    SUCCESS = 0,
    ERROR_NOT_SUPPORTED,
    ERROR_NOT_FOUND,
    ERROR_PERMISSION_DENIED,
    ERROR_IO_FAILURE,
    ERROR_INVALID_PARTITION,
    ERROR_PARTITION_LOCKED,
    ERROR_CHECKSUM_MISMATCH,
    ERROR_INSUFFICIENT_SPACE,
    ERROR_BOOTLOADER_ERROR,
    ERROR_RECOVERY_FAILED,
    ERROR_TIMEOUT,
    ERROR_BUSY,
    ERROR_UNKNOWN
};

/**
 * @brief Partition slot identifier
 */
enum class PartitionSlot : uint8_t {
    SLOT_A = 0,
    SLOT_B = 1,
    RECOVERY = 2,
    BOOTLOADER = 3
};

/**
 * @brief Partition information structure
 */
struct PartitionInfo {
    std::string name;           ///< Partition name (e.g., "rootfs_a")
    std::string device;         ///< Device path (e.g., "/dev/mtd3")
    PartitionSlot slot;         ///< Slot identifier
    uint64_t offset;            ///< Offset in flash
    uint64_t size;              ///< Partition size in bytes
    uint64_t usedSize;          ///< Used space in bytes
    bool isActive;              ///< Currently active partition
    bool isBootable;            ///< Contains valid bootable image
    bool isWriteProtected;      ///< Write protection status
    std::string version;        ///< Firmware version string
    std::vector<uint8_t> checksum;  ///< SHA256 checksum
    uint64_t lastModified;      ///< Unix timestamp of last modification
};

/**
 * @brief Boot environment variable
 */
struct BootEnvVar {
    std::string name;
    std::string value;
};

/**
 * @brief System information structure
 */
struct SystemInfo {
    std::string platform;       ///< Platform name (e.g., "openwrt")
    std::string architecture;   ///< CPU architecture
    std::string kernelVersion;  ///< Kernel version string
    std::string boardName;      ///< Board/device name
    uint64_t totalRam;          ///< Total RAM in bytes
    uint64_t freeRam;           ///< Free RAM in bytes
    uint64_t totalFlash;        ///< Total flash in bytes
    uint64_t freeFlash;         ///< Free flash in bytes
    std::string bootloaderType; ///< Bootloader type (u-boot, grub, etc.)
    std::string bootloaderVersion;
};

/**
 * @brief Flash operation progress callback
 */
using FlashProgressCallback = std::function<void(uint64_t current, uint64_t total)>;

/**
 * @brief Abstract interface for partition management
 */
class IPartitionManager {
public:
    virtual ~IPartitionManager() = default;

    /**
     * @brief Initialize partition manager
     * @return Platform result code
     */
    virtual PlatformResult initialize() = 0;

    /**
     * @brief Get list of all partitions
     * @return Vector of partition information
     */
    virtual std::vector<PartitionInfo> getPartitions() = 0;

    /**
     * @brief Get information about a specific partition
     * @param slot Partition slot
     * @return Partition info if found
     */
    virtual std::optional<PartitionInfo> getPartition(PartitionSlot slot) = 0;

    /**
     * @brief Get currently active boot slot
     * @return Active slot identifier
     */
    virtual PartitionSlot getActiveSlot() = 0;

    /**
     * @brief Check if a partition is bootable
     * @param slot Partition slot to check
     * @return true if bootable, false otherwise
     */
    virtual bool isSlotBootable(PartitionSlot slot) = 0;

    /**
     * @brief Mark a slot as bootable or non-bootable
     * @param slot Partition slot
     * @param bootable Bootable status
     * @return Platform result code
     */
    virtual PlatformResult setSlotBootable(PartitionSlot slot, bool bootable) = 0;

    /**
     * @brief Switch active boot slot
     * @param targetSlot Target slot to switch to
     * @return Platform result code
     */
    virtual PlatformResult switchSlot(PartitionSlot targetSlot) = 0;

    /**
     * @brief Get the inactive slot (for writing updates)
     * @return Inactive slot identifier
     */
    virtual PartitionSlot getInactiveSlot() = 0;

    /**
     * @brief Verify partition integrity
     * @param slot Partition slot to verify
     * @param expectedChecksum Expected SHA256 checksum (optional)
     * @return Platform result code
     */
    virtual PlatformResult verifyPartition(PartitionSlot slot, 
                                           const std::vector<uint8_t>& expectedChecksum = {}) = 0;

    /**
     * @brief Calculate partition checksum
     * @param slot Partition slot
     * @return SHA256 checksum or empty on error
     */
    virtual std::vector<uint8_t> calculateChecksum(PartitionSlot slot) = 0;
};

/**
 * @brief Abstract interface for flash operations
 */
class IFlashManager {
public:
    virtual ~IFlashManager() = default;

    /**
     * @brief Initialize flash manager
     * @return Platform result code
     */
    virtual PlatformResult initialize() = 0;

    /**
     * @brief Read data from flash
     * @param device Flash device path
     * @param offset Offset to read from
     * @param size Number of bytes to read
     * @param buffer Output buffer
     * @return Platform result code
     */
    virtual PlatformResult read(const std::string& device, uint64_t offset, 
                                 uint64_t size, std::vector<uint8_t>& buffer) = 0;

    /**
     * @brief Write data to flash
     * @param device Flash device path
     * @param offset Offset to write to
     * @param data Data to write
     * @param progress Progress callback (optional)
     * @return Platform result code
     */
    virtual PlatformResult write(const std::string& device, uint64_t offset,
                                  const std::vector<uint8_t>& data,
                                  FlashProgressCallback progress = nullptr) = 0;

    /**
     * @brief Erase flash region
     * @param device Flash device path
     * @param offset Start offset
     * @param size Size to erase
     * @return Platform result code
     */
    virtual PlatformResult erase(const std::string& device, uint64_t offset, uint64_t size) = 0;

    /**
     * @brief Get flash device information
     * @param device Flash device path
     * @param eraseSize Output: erase block size
     * @param totalSize Output: total device size
     * @return Platform result code
     */
    virtual PlatformResult getDeviceInfo(const std::string& device,
                                          uint64_t& eraseSize, uint64_t& totalSize) = 0;

    /**
     * @brief Lock flash region (write protect)
     * @param device Flash device path
     * @param offset Start offset
     * @param size Size to lock
     * @return Platform result code
     */
    virtual PlatformResult lock(const std::string& device, uint64_t offset, uint64_t size) = 0;

    /**
     * @brief Unlock flash region
     * @param device Flash device path
     * @param offset Start offset
     * @param size Size to unlock
     * @return Platform result code
     */
    virtual PlatformResult unlock(const std::string& device, uint64_t offset, uint64_t size) = 0;
};

/**
 * @brief Abstract interface for bootloader control
 */
class IBootloaderControl {
public:
    virtual ~IBootloaderControl() = default;

    /**
     * @brief Initialize bootloader control
     * @return Platform result code
     */
    virtual PlatformResult initialize() = 0;

    /**
     * @brief Get bootloader type
     * @return Bootloader type string
     */
    virtual std::string getBootloaderType() = 0;

    /**
     * @brief Get boot environment variable
     * @param name Variable name
     * @return Variable value if found
     */
    virtual std::optional<std::string> getEnv(const std::string& name) = 0;

    /**
     * @brief Set boot environment variable
     * @param name Variable name
     * @param value Variable value
     * @return Platform result code
     */
    virtual PlatformResult setEnv(const std::string& name, const std::string& value) = 0;

    /**
     * @brief Get all boot environment variables
     * @return Map of environment variables
     */
    virtual std::map<std::string, std::string> getAllEnv() = 0;

    /**
     * @brief Save environment to persistent storage
     * @return Platform result code
     */
    virtual PlatformResult saveEnv() = 0;

    /**
     * @brief Set boot slot for next reboot
     * @param slot Target boot slot
     * @return Platform result code
     */
    virtual PlatformResult setBootSlot(PartitionSlot slot) = 0;

    /**
     * @brief Get boot slot for next reboot
     * @return Boot slot
     */
    virtual PartitionSlot getBootSlot() = 0;

    /**
     * @brief Set boot attempt counter
     * @param slot Slot to set counter for
     * @param attempts Number of remaining attempts
     * @return Platform result code
     */
    virtual PlatformResult setBootAttempts(PartitionSlot slot, int attempts) = 0;

    /**
     * @brief Get remaining boot attempts
     * @param slot Slot to query
     * @return Remaining attempts
     */
    virtual int getBootAttempts(PartitionSlot slot) = 0;

    /**
     * @brief Mark current boot as successful
     * @return Platform result code
     */
    virtual PlatformResult markBootSuccessful() = 0;

    /**
     * @brief Request reboot to recovery mode
     * @return Platform result code
     */
    virtual PlatformResult requestRecoveryBoot() = 0;
};

/**
 * @brief Abstract interface for system management
 */
class ISystemManager {
public:
    virtual ~ISystemManager() = default;

    /**
     * @brief Get system information
     * @return System info structure
     */
    virtual SystemInfo getSystemInfo() = 0;

    /**
     * @brief Reboot the system
     * @param delay Delay in seconds before reboot
     * @return Platform result code
     */
    virtual PlatformResult reboot(int delay = 0) = 0;

    /**
     * @brief Shutdown the system
     * @param delay Delay in seconds before shutdown
     * @return Platform result code
     */
    virtual PlatformResult shutdown(int delay = 0) = 0;

    /**
     * @brief Enter recovery mode
     * @return Platform result code
     */
    virtual PlatformResult enterRecoveryMode() = 0;

    /**
     * @brief Check if running in recovery mode
     * @return true if in recovery mode
     */
    virtual bool isRecoveryMode() = 0;

    /**
     * @brief Sync filesystems
     * @return Platform result code
     */
    virtual PlatformResult syncFilesystems() = 0;

    /**
     * @brief Get current firmware version
     * @return Version string
     */
    virtual std::string getFirmwareVersion() = 0;

    /**
     * @brief Execute system command
     * @param command Command to execute
     * @param output Command output
     * @return Exit code
     */
    virtual int executeCommand(const std::string& command, std::string& output) = 0;
};

/**
 * @brief Platform factory for creating platform-specific implementations
 */
class PlatformFactory {
public:
    /**
     * @brief Detect current platform and create appropriate managers
     * @return true if platform detected successfully
     */
    static bool detectPlatform();

    /**
     * @brief Get partition manager instance
     * @return Shared pointer to partition manager
     */
    static std::shared_ptr<IPartitionManager> getPartitionManager();

    /**
     * @brief Get flash manager instance
     * @return Shared pointer to flash manager
     */
    static std::shared_ptr<IFlashManager> getFlashManager();

    /**
     * @brief Get bootloader control instance
     * @return Shared pointer to bootloader control
     */
    static std::shared_ptr<IBootloaderControl> getBootloaderControl();

    /**
     * @brief Get system manager instance
     * @return Shared pointer to system manager
     */
    static std::shared_ptr<ISystemManager> getSystemManager();

    /**
     * @brief Get detected platform name
     * @return Platform name string
     */
    static std::string getPlatformName();

private:
    static std::shared_ptr<IPartitionManager> partitionManager_;
    static std::shared_ptr<IFlashManager> flashManager_;
    static std::shared_ptr<IBootloaderControl> bootloaderControl_;
    static std::shared_ptr<ISystemManager> systemManager_;
    static std::string platformName_;
    static bool initialized_;
};

} // namespace platform
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_PLATFORM_ABSTRACTION_H
