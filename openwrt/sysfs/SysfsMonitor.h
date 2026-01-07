/*
 * SysfsMonitor.h
 *
 * OpenWRT sysfs access layer for hardware monitoring
 * Provides access to system information through sysfs interface
 */

#ifndef WPP_SYSFS_MONITOR_H
#define WPP_SYSFS_MONITOR_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace wpp {

/**
 * @brief Sysfs Monitor for Hardware Information
 *
 * Provides access to hardware information through Linux sysfs interface.
 * Useful for populating LwM2M device and connectivity monitoring objects.
 */
class SysfsMonitor {
public:
    /**
     * @brief Battery information structure
     */
    struct BatteryInfo {
        int level;              // Battery level (0-100%)
        int voltage;            // Voltage in mV
        int current;            // Current in mA
        int temperature;        // Temperature in 0.1°C
        std::string status;     // "Charging", "Discharging", "Full", "Unknown"
        std::string health;     // "Good", "Overheat", "Dead", "Unknown"
    };

    /**
     * @brief Memory information structure
     */
    struct MemoryInfo {
        uint64_t totalKB;       // Total memory in KB
        uint64_t freeKB;        // Free memory in KB
        uint64_t availableKB;   // Available memory in KB
        uint64_t buffersKB;     // Buffers in KB
        uint64_t cachedKB;      // Cached memory in KB
    };

    /**
     * @brief CPU information structure
     */
    struct CpuInfo {
        std::string model;      // CPU model name
        int cores;              // Number of CPU cores
        double frequency;       // Current frequency in MHz
        double temperature;     // Temperature in °C
        double usage;           // CPU usage percentage
    };

    /**
     * @brief Network interface information
     */
    struct NetworkInfo {
        std::string name;       // Interface name (e.g., "eth0", "wlan0")
        std::string mac;        // MAC address
        std::string ipv4;       // IPv4 address
        std::string ipv6;       // IPv6 address
        bool isUp;              // Interface is up
        uint64_t rxBytes;       // Received bytes
        uint64_t txBytes;       // Transmitted bytes
        uint64_t rxPackets;     // Received packets
        uint64_t txPackets;     // Transmitted packets
        int signalStrength;     // Signal strength in dBm (for wireless)
        std::string ssid;       // SSID (for wireless)
    };

    /**
     * @brief Storage information
     */
    struct StorageInfo {
        std::string device;     // Device name (e.g., "/dev/mmcblk0")
        std::string mountPoint; // Mount point (e.g., "/overlay")
        uint64_t totalKB;       // Total size in KB
        uint64_t freeKB;        // Free space in KB
        uint64_t usedKB;        // Used space in KB
    };

    /**
     * @brief System information
     */
    struct SystemInfo {
        std::string hostname;   // System hostname
        std::string kernel;     // Kernel version
        std::string model;      // Device model (from /proc/cpuinfo or devicetree)
        uint64_t uptimeSeconds; // System uptime in seconds
        double loadAverage1;    // 1-minute load average
        double loadAverage5;    // 5-minute load average
        double loadAverage15;   // 15-minute load average
    };

    /**
     * @brief Constructor
     */
    SysfsMonitor() = default;

    /**
     * @brief Destructor
     */
    ~SysfsMonitor() = default;

    /**
     * @brief Get battery information
     * @param batteryIndex Battery index (0 for first battery)
     * @return Battery information structure
     */
    BatteryInfo getBatteryInfo(int batteryIndex = 0) const;

    /**
     * @brief Get memory information
     * @return Memory information structure
     */
    MemoryInfo getMemoryInfo() const;

    /**
     * @brief Get CPU information
     * @return CPU information structure
     */
    CpuInfo getCpuInfo() const;

    /**
     * @brief Get network interface information
     * @param interfaceName Interface name (empty for all interfaces)
     * @return Vector of network interface information
     */
    std::vector<NetworkInfo> getNetworkInfo(const std::string& interfaceName = "") const;

    /**
     * @brief Get storage information
     * @param mountPoint Mount point (empty for all mounted filesystems)
     * @return Vector of storage information
     */
    std::vector<StorageInfo> getStorageInfo(const std::string& mountPoint = "") const;

    /**
     * @brief Get system information
     * @return System information structure
     */
    SystemInfo getSystemInfo() const;

    /**
     * @brief Check if battery exists
     * @param batteryIndex Battery index
     * @return true if battery exists
     */
    bool hasBattery(int batteryIndex = 0) const;

    /**
     * @brief Get available network interfaces
     * @return Vector of interface names
     */
    std::vector<std::string> getNetworkInterfaces() const;

    /**
     * @brief Get LED state
     * @param ledName LED name (e.g., "led0", "status")
     * @return LED brightness (0-255), -1 if not found
     */
    int getLedState(const std::string& ledName) const;

    /**
     * @brief Set LED state
     * @param ledName LED name
     * @param brightness LED brightness (0-255)
     * @return true if successful
     */
    bool setLedState(const std::string& ledName, int brightness);

    /**
     * @brief Get GPIO value
     * @param gpioNumber GPIO pin number
     * @return GPIO value (0 or 1), -1 if error
     */
    int getGpioValue(int gpioNumber) const;

    /**
     * @brief Set GPIO value
     * @param gpioNumber GPIO pin number
     * @param value Value to set (0 or 1)
     * @return true if successful
     */
    bool setGpioValue(int gpioNumber, int value);

    /**
     * @brief Get temperature from thermal zone
     * @param zoneIndex Thermal zone index
     * @return Temperature in °C, -273.15 if not available
     */
    double getThermalZoneTemperature(int zoneIndex = 0) const;

    /**
     * @brief Read sysfs file
     * @param path File path
     * @return File contents, empty string if error
     */
    static std::string readSysfsFile(const std::string& path);

    /**
     * @brief Write sysfs file
     * @param path File path
     * @param value Value to write
     * @return true if successful
     */
    static bool writeSysfsFile(const std::string& path, const std::string& value);

    /**
     * @brief Read integer from sysfs file
     * @param path File path
     * @param defaultValue Default value if read fails
     * @return Integer value or default
     */
    static int readSysfsInt(const std::string& path, int defaultValue = 0);

    /**
     * @brief Read uint64 from sysfs file
     * @param path File path
     * @param defaultValue Default value if read fails
     * @return uint64 value or default
     */
    static uint64_t readSysfsUint64(const std::string& path, uint64_t defaultValue = 0);

private:
    /**
     * @brief Parse /proc/meminfo
     * @return Memory info map
     */
    std::map<std::string, uint64_t> parseMeminfo() const;

    /**
     * @brief Parse /proc/cpuinfo
     * @return CPU info map
     */
    std::map<std::string, std::string> parseCpuinfo() const;

    /**
     * @brief Parse /proc/stat for CPU usage
     * @return CPU usage percentage
     */
    double parseCpuUsage() const;

    /**
     * @brief Parse /proc/net/dev for network statistics
     * @return Network statistics map
     */
    std::map<std::string, NetworkInfo> parseNetDev() const;

    /**
     * @brief Get wireless info for interface
     * @param interfaceName Interface name
     * @return Updated NetworkInfo with wireless data
     */
    NetworkInfo getWirelessInfo(const std::string& interfaceName) const;

    /**
     * @brief Parse /proc/mounts for filesystem information
     * @return Vector of storage information
     */
    std::vector<StorageInfo> parseMounts() const;

    /**
     * @brief Get filesystem usage statistics
     * @param mountPoint Mount point
     * @return Storage information
     */
    StorageInfo getFilesystemStats(const std::string& mountPoint) const;
};

} // namespace wpp

#endif // WPP_SYSFS_MONITOR_H
