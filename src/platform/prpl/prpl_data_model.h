/**
 * @file prpl_data_model.h
 * @brief TR-181 Data Model Access Layer for prplOS
 *
 * Provides high-level access to TR-181 i2 Device:2 data model through
 * Ambiorix bus. Includes type conversions, caching, and convenience
 * methods for common TR-181 objects:
 * - Device.DeviceInfo
 * - Device.WiFi
 * - Device.Ethernet
 * - Device.IP
 * - Device.Hosts
 * - Device.Time
 * - Device.ManagementServer
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_PRPL_DATA_MODEL_H
#define FRIENDLY_LWM2M_PRPL_DATA_MODEL_H

#include "prpl_amxb_client.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <chrono>
#include <optional>

namespace lwm2m {
namespace platform {
namespace prpl {

/**
 * @brief WiFi operating standard
 */
enum class WiFiStandard {
    UNKNOWN,
    IEEE_802_11A,
    IEEE_802_11B,
    IEEE_802_11G,
    IEEE_802_11N,
    IEEE_802_11AC,
    IEEE_802_11AX,
    IEEE_802_11BE
};

/**
 * @brief WiFi frequency band
 */
enum class WiFiBand {
    UNKNOWN,
    BAND_2_4GHZ,
    BAND_5GHZ,
    BAND_6GHZ
};

/**
 * @brief WiFi radio information
 */
struct WiFiRadioInfo {
    std::string path;                  // Device.WiFi.Radio.{i}
    bool enabled = false;
    std::string name;                  // Radio name
    WiFiStandard standard = WiFiStandard::UNKNOWN;
    std::string operatingStandards;    // Comma-separated list
    uint32_t channel = 0;
    WiFiBand band = WiFiBand::UNKNOWN;
    int8_t transmitPower = 0;          // dBm
    std::vector<uint32_t> possibleChannels;
    uint32_t operatingChannelBandwidth = 20; // MHz
};

/**
 * @brief WiFi SSID configuration
 */
struct WiFiSSIDInfo {
    std::string path;                  // Device.WiFi.SSID.{i}
    bool enabled = false;
    std::string ssid;
    std::string bssid;
    std::string status;                // Up, Down, Unknown, Dormant, etc.
};

/**
 * @brief WiFi access point information
 */
struct WiFiAccessPointInfo {
    std::string path;                  // Device.WiFi.AccessPoint.{i}
    bool enabled = false;
    std::string ssidReference;         // Reference to SSID object
    std::string securityMode;          // None, WEP, WPA, WPA2, WPA3, etc.
    uint32_t associatedDeviceCount = 0;
};

/**
 * @brief WiFi associated device (station)
 */
struct WiFiAssociatedDevice {
    std::string path;                  // Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}
    std::string macAddress;
    std::string ipAddress;
    bool active = false;
    int8_t signalStrength = 0;         // dBm
    uint32_t lastDataDownlinkRate = 0; // Kbps
    uint32_t lastDataUplinkRate = 0;   // Kbps
    uint64_t bytesReceived = 0;
    uint64_t bytesSent = 0;
};

/**
 * @brief Ethernet interface information
 */
struct EthernetInterfaceInfo {
    std::string path;                  // Device.Ethernet.Interface.{i}
    bool enabled = false;
    std::string name;
    std::string status;                // Up, Down, Unknown, Dormant, etc.
    std::string macAddress;
    uint32_t maxBitRate = 0;           // Mbps
    std::string duplexMode;            // Half, Full, Auto
};

/**
 * @brief IP interface information
 */
struct IPInterfaceInfo {
    std::string path;                  // Device.IP.Interface.{i}
    bool enabled = false;
    std::string name;
    std::string type;                  // Normal, Loopback, Tunnel, etc.
    std::string status;                // Up, Down, Unknown, Dormant, etc.
    std::vector<std::string> ipv4Addresses;
    std::vector<std::string> ipv6Addresses;
};

/**
 * @brief Host entry from Device.Hosts.Host table
 */
struct HostInfo {
    std::string path;                  // Device.Hosts.Host.{i}
    std::string ipAddress;
    std::string macAddress;
    std::string hostName;
    bool active = false;
    std::string interfaceType;         // Ethernet, WiFi, etc.
    std::string addressSource;         // DHCP, Static, etc.
    std::chrono::system_clock::time_point leaseTimeRemaining;
};

/**
 * @brief Device information
 */
struct DeviceInfo {
    std::string manufacturer;
    std::string manufacturerOUI;
    std::string modelName;
    std::string description;
    std::string productClass;
    std::string serialNumber;
    std::string hardwareVersion;
    std::string softwareVersion;
    std::string provisioningCode;
    uint32_t upTime = 0;               // seconds
    std::chrono::system_clock::time_point firstUseDate;
};

/**
 * @brief TR-181 Data Model Access Layer
 *
 * Provides high-level access to TR-181 data model through Ambiorix.
 * Includes caching, type conversion, and convenience methods.
 */
class PrplDataModel {
public:
    /**
     * @brief Constructor
     * @param amxbClient Ambiorix client instance
     */
    explicit PrplDataModel(std::shared_ptr<PrplAmxbClient> amxbClient);

    /**
     * @brief Destructor
     */
    ~PrplDataModel();

    // Prevent copying
    PrplDataModel(const PrplDataModel&) = delete;
    PrplDataModel& operator=(const PrplDataModel&) = delete;

    /**
     * @brief Initialize data model
     * @return true on success
     */
    bool initialize();

    // ========================================================================
    // Device.DeviceInfo
    // ========================================================================

    /**
     * @brief Get device information
     * @return Map of DeviceInfo parameters
     */
    std::map<std::string, std::string> getDeviceInfo();

    /**
     * @brief Get structured device info
     * @return DeviceInfo structure
     */
    DeviceInfo getDeviceInfoStructured();

    /**
     * @brief Get device uptime
     * @return Uptime in seconds
     */
    uint32_t getUptime();

    // ========================================================================
    // Device.WiFi
    // ========================================================================

    /**
     * @brief Get WiFi radio information
     * @return Vector of WiFi radios
     */
    std::vector<WiFiRadioInfo> getWiFiRadios();

    /**
     * @brief Get specific WiFi radio
     * @param index Radio index (1-based)
     * @return WiFi radio info or nullopt
     */
    std::optional<WiFiRadioInfo> getWiFiRadio(uint32_t index);

    /**
     * @brief Get WiFi SSIDs
     * @return Vector of WiFi SSIDs
     */
    std::vector<WiFiSSIDInfo> getWiFiSSIDs();

    /**
     * @brief Get WiFi access points
     * @return Vector of WiFi access points
     */
    std::vector<WiFiAccessPointInfo> getWiFiAccessPoints();

    /**
     * @brief Get WiFi associated devices (stations)
     * @return Vector of associated devices
     */
    std::vector<WiFiAssociatedDevice> getWiFiAssociatedDevices();

    /**
     * @brief Get WiFi stations for specific access point
     * @param apIndex Access point index (1-based)
     * @return Vector of associated devices
     */
    std::vector<WiFiAssociatedDevice> getWiFiStations(uint32_t apIndex);

    /**
     * @brief Get all WiFi stations (wrapper for compatibility)
     * @return Map of station path to parameters
     */
    std::map<std::string, std::map<std::string, std::string>> getWiFiStations();

    // ========================================================================
    // Device.Ethernet
    // ========================================================================

    /**
     * @brief Get Ethernet interfaces
     * @return Vector of Ethernet interfaces
     */
    std::vector<EthernetInterfaceInfo> getEthernetInterfaces();

    /**
     * @brief Get specific Ethernet interface
     * @param index Interface index (1-based)
     * @return Ethernet interface info or nullopt
     */
    std::optional<EthernetInterfaceInfo> getEthernetInterface(uint32_t index);

    // ========================================================================
    // Device.IP
    // ========================================================================

    /**
     * @brief Get IP interfaces
     * @return Vector of IP interfaces
     */
    std::vector<IPInterfaceInfo> getIPInterfaces();

    /**
     * @brief Get IP interfaces (wrapper for compatibility)
     * @return Map of interface path to parameters
     */
    std::map<std::string, std::map<std::string, std::string>> getIPInterfacesMap();

    /**
     * @brief Get specific IP interface
     * @param index Interface index (1-based)
     * @return IP interface info or nullopt
     */
    std::optional<IPInterfaceInfo> getIPInterface(uint32_t index);

    // ========================================================================
    // Device.Hosts
    // ========================================================================

    /**
     * @brief Get host table entries
     * @return Vector of host entries
     */
    std::vector<HostInfo> getHosts();

    /**
     * @brief Get active hosts only
     * @return Vector of active host entries
     */
    std::vector<HostInfo> getActiveHosts();

    /**
     * @brief Get host count
     * @return Number of host entries
     */
    uint32_t getHostCount();

    // ========================================================================
    // Utility Methods
    // ========================================================================

    /**
     * @brief Convert WiFi standard string to enum
     * @param standard Standard string (e.g., "ax", "802.11ax")
     * @return WiFi standard enum
     */
    static WiFiStandard parseWiFiStandard(const std::string& standard);

    /**
     * @brief Convert WiFi band to enum
     * @param band Band string (e.g., "2.4GHz", "5GHz")
     * @return WiFi band enum
     */
    static WiFiBand parseWiFiBand(const std::string& band);

    /**
     * @brief Convert channel to band
     * @param channel Channel number
     * @return WiFi band enum
     */
    static WiFiBand channelToBand(uint32_t channel);

    /**
     * @brief Enable/disable caching
     * @param enable Enable flag
     * @param ttlSeconds Cache TTL in seconds (default: 5)
     */
    void setCacheEnabled(bool enable, uint32_t ttlSeconds = 5);

    /**
     * @brief Clear all caches
     */
    void clearCache();

private:
    std::shared_ptr<PrplAmxbClient> amxbClient_;
    bool initialized_;
    bool cacheEnabled_;
    uint32_t cacheTTL_;

    // Cache structures
    struct CacheEntry {
        std::chrono::steady_clock::time_point timestamp;
        std::map<std::string, std::string> data;
    };
    std::map<std::string, CacheEntry> cache_;

    // Helper methods
    std::map<std::string, std::string> getCached(const std::string& path);
    void updateCache(const std::string& path, const std::map<std::string, std::string>& data);
    bool isCacheValid(const std::string& path);

    // TR-181 path helpers
    std::string getWiFiRadioPath(uint32_t index);
    std::string getWiFiSSIDPath(uint32_t index);
    std::string getWiFiAccessPointPath(uint32_t index);
    std::string getEthernetInterfacePath(uint32_t index);
    std::string getIPInterfacePath(uint32_t index);

    // Parsing helpers
    WiFiRadioInfo parseWiFiRadio(const std::map<std::string, std::string>& params);
    WiFiSSIDInfo parseWiFiSSID(const std::map<std::string, std::string>& params);
    WiFiAccessPointInfo parseWiFiAccessPoint(const std::map<std::string, std::string>& params);
    WiFiAssociatedDevice parseWiFiAssociatedDevice(const std::map<std::string, std::string>& params);
    EthernetInterfaceInfo parseEthernetInterface(const std::map<std::string, std::string>& params);
    IPInterfaceInfo parseIPInterface(const std::map<std::string, std::string>& params);
    HostInfo parseHost(const std::map<std::string, std::string>& params);
};

} // namespace prpl
} // namespace platform
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_PRPL_DATA_MODEL_H
