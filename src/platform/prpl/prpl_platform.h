/**
 * @file prpl_platform.h
 * @brief prplOS Platform Abstraction Layer for Friendly LwM2M Client
 *
 * Provides platform implementations for prplOS-based systems including:
 * - Ambiorix (amxb/amxd) bus integration
 * - TR-181 data model access
 * - USP agent interaction
 * - prpl reference platform API integration
 * - EasyMesh R2/R4 support
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_PRPL_PLATFORM_H
#define FRIENDLY_LWM2M_PRPL_PLATFORM_H

#include "platform/platform_abstraction.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <mutex>

// Forward declarations for Ambiorix types
struct _amxb_bus_ctx_t;
typedef struct _amxb_bus_ctx_t amxb_bus_ctx_t;

struct _amxd_dm_t;
typedef struct _amxd_dm_t amxd_dm_t;

namespace lwm2m {
namespace platform {
namespace prpl {

// Forward declarations
class PrplAmxbClient;
class PrplDataModel;
class PrplEasyMesh;

/**
 * @brief prplOS detection result
 */
struct PrplOSInfo {
    bool isPresent = false;
    std::string version;
    std::string variant;           // prplWrt, prplMesh, etc.
    bool hasAmbiorix = false;
    bool hasUSPAgent = false;
    bool hasEasyMesh = false;
    std::string boardName;
    std::string chipsetModel;
};

/**
 * @brief Network interface information from TR-181
 */
struct NetworkInterface {
    std::string name;              // eth0, wlan0, etc.
    std::string type;              // Ethernet, WiFi
    std::string status;            // Up, Down, Unknown, Dormant
    bool enabled = false;
    std::string macAddress;
    std::vector<std::string> ipv4Addresses;
    std::vector<std::string> ipv6Addresses;
    uint64_t bytesReceived = 0;
    uint64_t bytesSent = 0;
    uint64_t packetsReceived = 0;
    uint64_t packetsSent = 0;
    uint32_t mtu = 1500;
};

/**
 * @brief WiFi station information from TR-181
 */
struct WiFiStation {
    std::string macAddress;
    std::string ipAddress;
    int8_t signalStrength = 0;     // dBm
    uint32_t linkRate = 0;         // Mbps
    uint64_t bytesReceived = 0;
    uint64_t bytesSent = 0;
    bool isActive = false;
    std::string ssid;
    std::string bssid;
    std::string band;              // 2.4GHz, 5GHz, 6GHz
};

/**
 * @brief Service status information
 */
struct ServiceStatus {
    std::string name;
    std::string state;             // running, stopped, failed
    bool enabled = false;
    uint32_t pid = 0;
    std::string description;
};

/**
 * @brief Partition Manager for prplOS
 *
 * Implements partition management using prplOS bootloader interface
 * and Ambiorix data model access.
 */
class PrplPartitionManager : public IPartitionManager {
public:
    explicit PrplPartitionManager(std::shared_ptr<PrplAmxbClient> amxbClient);
    ~PrplPartitionManager() override;

    PlatformResult initialize() override;
    std::vector<PartitionInfo> getPartitions() override;
    std::optional<PartitionInfo> getPartition(PartitionSlot slot) override;
    PartitionSlot getActiveSlot() override;
    bool isSlotBootable(PartitionSlot slot) override;
    PlatformResult setSlotBootable(PartitionSlot slot, bool bootable) override;
    PlatformResult switchSlot(PartitionSlot targetSlot) override;
    PartitionSlot getInactiveSlot() override;
    PlatformResult verifyPartition(PartitionSlot slot,
                                   const std::vector<uint8_t>& expectedChecksum = {}) override;
    std::vector<uint8_t> calculateChecksum(PartitionSlot slot) override;

private:
    std::shared_ptr<PrplAmxbClient> amxbClient_;
    std::map<PartitionSlot, PartitionInfo> partitions_;
    mutable std::mutex mutex_;

    bool detectPartitions();
    std::string slotToDevice(PartitionSlot slot);
};

/**
 * @brief Flash Manager for prplOS
 *
 * Implements flash operations using prplOS MTD interface
 * and firmware update mechanisms.
 */
class PrplFlashManager : public IFlashManager {
public:
    explicit PrplFlashManager(std::shared_ptr<PrplAmxbClient> amxbClient);
    ~PrplFlashManager() override;

    PlatformResult initialize() override;
    PlatformResult read(const std::string& device, uint64_t offset,
                        uint64_t size, std::vector<uint8_t>& buffer) override;
    PlatformResult write(const std::string& device, uint64_t offset,
                         const std::vector<uint8_t>& data,
                         FlashProgressCallback progress = nullptr) override;
    PlatformResult erase(const std::string& device, uint64_t offset, uint64_t size) override;
    PlatformResult getDeviceInfo(const std::string& device,
                                 uint64_t& eraseSize, uint64_t& totalSize) override;
    PlatformResult lock(const std::string& device, uint64_t offset, uint64_t size) override;
    PlatformResult unlock(const std::string& device, uint64_t offset, uint64_t size) override;

private:
    std::shared_ptr<PrplAmxbClient> amxbClient_;
    std::map<std::string, int> deviceHandles_;
    mutable std::mutex mutex_;

    bool isMTDDevice(const std::string& device);
};

/**
 * @brief Bootloader Control for prplOS
 *
 * Implements bootloader control using prplOS boot environment
 * and Ambiorix integration.
 */
class PrplBootloaderControl : public IBootloaderControl {
public:
    explicit PrplBootloaderControl(std::shared_ptr<PrplAmxbClient> amxbClient);
    ~PrplBootloaderControl() override;

    PlatformResult initialize() override;
    std::string getBootloaderType() override;
    std::optional<std::string> getEnv(const std::string& name) override;
    PlatformResult setEnv(const std::string& name, const std::string& value) override;
    std::map<std::string, std::string> getAllEnv() override;
    PlatformResult saveEnv() override;
    PlatformResult setBootSlot(PartitionSlot slot) override;
    PartitionSlot getBootSlot() override;
    PlatformResult setBootAttempts(PartitionSlot slot, int attempts) override;
    int getBootAttempts(PartitionSlot slot) override;
    PlatformResult markBootSuccessful() override;
    PlatformResult requestRecoveryBoot() override;

private:
    std::shared_ptr<PrplAmxbClient> amxbClient_;
    std::map<std::string, std::string> bootEnv_;
    std::string bootloaderType_;
    mutable std::mutex mutex_;

    bool loadBootEnv();
    bool writeBootEnv();
};

/**
 * @brief System Manager for prplOS
 *
 * Implements system management using prplOS APIs,
 * Ambiorix data model, and TR-181 integration.
 */
class PrplSystemManager : public ISystemManager {
public:
    explicit PrplSystemManager(std::shared_ptr<PrplAmxbClient> amxbClient,
                               std::shared_ptr<PrplDataModel> dataModel);
    ~PrplSystemManager() override;

    SystemInfo getSystemInfo() override;
    PlatformResult reboot(int delay = 0) override;
    PlatformResult shutdown(int delay = 0) override;
    PlatformResult enterRecoveryMode() override;
    bool isRecoveryMode() override;
    PlatformResult syncFilesystems() override;
    std::string getFirmwareVersion() override;
    int executeCommand(const std::string& command, std::string& output) override;

    // prplOS-specific extensions
    /**
     * @brief Get prplOS information
     * @return prplOS detection info
     */
    PrplOSInfo getPrplOSInfo();

    /**
     * @brief Get network interfaces from TR-181
     * @return Vector of network interfaces
     */
    std::vector<NetworkInterface> getNetworkInterfaces();

    /**
     * @brief Get WiFi stations from TR-181
     * @return Vector of connected WiFi stations
     */
    std::vector<WiFiStation> getWiFiStations();

    /**
     * @brief Get service status
     * @param serviceName Service name
     * @return Service status information
     */
    std::optional<ServiceStatus> getServiceStatus(const std::string& serviceName);

    /**
     * @brief Restart a service
     * @param serviceName Service name
     * @return Platform result code
     */
    PlatformResult restartService(const std::string& serviceName);

private:
    std::shared_ptr<PrplAmxbClient> amxbClient_;
    std::shared_ptr<PrplDataModel> dataModel_;
    PrplOSInfo prplInfo_;
    mutable std::mutex mutex_;

    bool detectPrplOS();
    void queryTR181SystemInfo();
};

/**
 * @brief Main prplOS Platform class
 *
 * Factory for creating prplOS-specific platform implementations
 * with Ambiorix integration and TR-181 data model access.
 */
class PrplPlatform {
public:
    /**
     * @brief Detect if running on prplOS
     * @return Detection result with platform info
     */
    static PrplOSInfo detectPrplOS();

    /**
     * @brief Initialize prpl platform
     * @param busUri Ambiorix bus URI (e.g., "ubus:/var/run/ubus/ubus.sock")
     * @return true on success
     */
    static bool initialize(const std::string& busUri = "");

    /**
     * @brief Shutdown prpl platform
     */
    static void shutdown();

    /**
     * @brief Check if platform is initialized
     * @return true if initialized
     */
    static bool isInitialized();

    /**
     * @brief Get Ambiorix client instance
     * @return Shared pointer to Ambiorix client
     */
    static std::shared_ptr<PrplAmxbClient> getAmxbClient();

    /**
     * @brief Get data model instance
     * @return Shared pointer to data model
     */
    static std::shared_ptr<PrplDataModel> getDataModel();

    /**
     * @brief Get EasyMesh integration instance
     * @return Shared pointer to EasyMesh integration
     */
    static std::shared_ptr<PrplEasyMesh> getEasyMesh();

    /**
     * @brief Register with PlatformFactory
     * @return true on success
     */
    static bool registerWithFactory();

private:
    static std::shared_ptr<PrplAmxbClient> amxbClient_;
    static std::shared_ptr<PrplDataModel> dataModel_;
    static std::shared_ptr<PrplEasyMesh> easyMesh_;
    static std::shared_ptr<PrplPartitionManager> partitionManager_;
    static std::shared_ptr<PrplFlashManager> flashManager_;
    static std::shared_ptr<PrplBootloaderControl> bootloaderControl_;
    static std::shared_ptr<PrplSystemManager> systemManager_;
    static bool initialized_;
    static std::mutex initMutex_;
};

} // namespace prpl
} // namespace platform
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_PRPL_PLATFORM_H
