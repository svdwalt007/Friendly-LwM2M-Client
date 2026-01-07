/*
 * OpenWrtDeviceInfo.h
 *
 * OpenWRT-specific Device Object (3) implementation helper
 * Provides real hardware/system data for OpenWRT One Router
 */

#ifndef WPP_OPENWRT_DEVICE_INFO_H
#define WPP_OPENWRT_DEVICE_INFO_H

#include <string>
#include <fstream>
#include <sstream>
#include <ctime>

namespace wpp {

/**
 * @brief OpenWRT Device Information Helper
 *
 * Provides OpenWRT-specific implementations for Device Object resources
 * tailored for OpenWRT One router with MediaTek MT7981B SoC.
 */
class OpenWrtDeviceInfo {
public:
    /**
     * @brief Get manufacturer name
     * @return Manufacturer string
     */
    static std::string getManufacturer() {
        // OpenWRT One is manufactured by Banana Pi
        return "Banana Pi (OpenWRT One)";
    }

    /**
     * @brief Get model number
     * @return Model number string
     */
    static std::string getModelNumber() {
        // Read from devicetree or return default
        std::string model = readFile("/proc/device-tree/model");
        if (!model.empty()) {
            // Clean up the string
            model.erase(std::remove(model.begin(), model.end(), '\0'), model.end());
            return model;
        }
        return "OpenWRT One (BPI-R4)";
    }

    /**
     * @brief Get serial number
     * @return Serial number string (MAC-based for uniqueness)
     */
    static std::string getSerialNumber() {
        // Use WAN MAC address as serial number
        std::string mac = readFile("/sys/class/net/eth1/address");
        if (!mac.empty()) {
            mac.erase(std::remove(mac.begin(), mac.end(), '\n'), mac.end());
            mac.erase(std::remove(mac.begin(), mac.end(), ':'), mac.end());
            return "OWO-" + mac; // OpenWRT One prefix
        }
        return "OWO-UNKNOWN";
    }

    /**
     * @brief Get firmware version
     * @return Firmware version string
     */
    static std::string getFirmwareVersion() {
        // Read OpenWRT version
        std::string version = readFile("/etc/openwrt_version");
        if (!version.empty()) {
            version.erase(std::remove(version.end()-1, version.end(), '\n'), version.end());
            return version;
        }

        // Fallback to release info
        version = readFile("/etc/openwrt_release");
        if (!version.empty()) {
            // Parse DISTRIB_RELEASE='...'
            size_t start = version.find("DISTRIB_RELEASE='");
            if (start != std::string::npos) {
                start += 17;
                size_t end = version.find("'", start);
                if (end != std::string::npos) {
                    return version.substr(start, end - start);
                }
            }
        }
        return "Unknown";
    }

    /**
     * @brief Get hardware version
     * @return Hardware version string
     */
    static std::string getHardwareVersion() {
        // OpenWRT One hardware revision
        return "v1.0 (MT7981B)";
    }

    /**
     * @brief Get software version (kernel)
     * @return Software/kernel version string
     */
    static std::string getSoftwareVersion() {
        std::string version = readFile("/proc/version");
        if (!version.empty()) {
            // Extract kernel version
            size_t start = version.find("Linux version ");
            if (start != std::string::npos) {
                start += 14;
                size_t end = version.find(" ", start);
                if (end != std::string::npos) {
                    return version.substr(start, end - start);
                }
            }
        }
        return "Unknown";
    }

    /**
     * @brief Get device type
     * @return Device type string
     */
    static std::string getDeviceType() {
        return "Router/IoT Gateway";
    }

    /**
     * @brief Get available power sources
     * @return Vector of power source types (0=DC, 4=PoE, 5=USB)
     */
    static std::vector<int> getAvailablePowerSources() {
        std::vector<int> sources;
        sources.push_back(0);  // DC power (12V barrel jack)
        sources.push_back(4);  // Power over Ethernet (802.3af/at)
        sources.push_back(5);  // USB Type-C (PD up to 30W)
        return sources;
    }

    /**
     * @brief Get power source voltage
     * @param sourceIndex Power source index
     * @return Voltage in millivolts
     */
    static int getPowerSourceVoltage(int sourceIndex) {
        // OpenWRT One power specifications
        switch(sourceIndex) {
            case 0: return 12000; // 12V DC
            case 1: return 48000; // 48V PoE (negotiated down)
            case 2: return 5000;  // 5V USB-C (can be up to 20V with PD)
            default: return 0;
        }
    }

    /**
     * @brief Get power source current
     * @param sourceIndex Power source index
     * @return Current in milliamps
     */
    static int getPowerSourceCurrent(int sourceIndex) {
        // Estimated current draw (OpenWRT One typical: 5-10W)
        switch(sourceIndex) {
            case 0: return 833;  // ~10W @ 12V
            case 1: return 208;  // ~10W @ 48V
            case 2: return 2000; // ~10W @ 5V
            default: return 0;
        }
    }

    /**
     * @brief Get total memory
     * @return Total RAM in KB
     */
    static int getMemoryTotal() {
        std::ifstream meminfo("/proc/meminfo");
        std::string line;
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal:") == 0) {
                std::istringstream iss(line);
                std::string label;
                int value;
                iss >> label >> value;
                return value; // Already in KB
            }
        }
        return 1048576; // Default: 1GB for OpenWRT One
    }

    /**
     * @brief Get free memory
     * @return Free RAM in KB
     */
    static int getMemoryFree() {
        std::ifstream meminfo("/proc/meminfo");
        std::string line;
        int memFree = 0;
        int buffers = 0;
        int cached = 0;

        while (std::getline(meminfo, line)) {
            std::istringstream iss(line);
            std::string label;
            int value;
            iss >> label >> value;

            if (label == "MemFree:") memFree = value;
            else if (label == "Buffers:") buffers = value;
            else if (label == "Cached:") cached = value;
        }

        // Available memory = Free + Buffers + Cached
        return memFree + buffers + cached;
    }

    /**
     * @brief Get current time
     * @return Unix timestamp
     */
    static time_t getCurrentTime() {
        return std::time(nullptr);
    }

    /**
     * @brief Get UTC offset
     * @return UTC offset string (e.g., "+00:00")
     */
    static std::string getUtcOffset() {
        std::time_t now = std::time(nullptr);
        std::tm local_tm = *std::localtime(&now);
        std::tm gmt_tm = *std::gmtime(&now);

        int offset_sec = std::mktime(&local_tm) - std::mktime(&gmt_tm);
        int offset_hours = offset_sec / 3600;
        int offset_mins = (std::abs(offset_sec) % 3600) / 60;

        char buffer[10];
        std::snprintf(buffer, sizeof(buffer), "%+03d:%02d", offset_hours, offset_mins);
        return std::string(buffer);
    }

    /**
     * @brief Get timezone
     * @return Timezone string
     */
    static std::string getTimezone() {
        std::string tz = readFile("/etc/TZ");
        if (!tz.empty()) {
            tz.erase(std::remove(tz.end()-1, tz.end(), '\n'), tz.end());
            return tz;
        }

        // Try timezone link
        char buffer[256];
        ssize_t len = readlink("/etc/localtime", buffer, sizeof(buffer)-1);
        if (len != -1) {
            buffer[len] = '\0';
            std::string path(buffer);
            size_t pos = path.find("/zoneinfo/");
            if (pos != std::string::npos) {
                return path.substr(pos + 10);
            }
        }

        return "UTC";
    }

    /**
     * @brief Get supported binding modes
     * @return Binding mode string (e.g., "U" for UDP)
     */
    static std::string getSupportedBindings() {
        // OpenWRT One supports UDP binding
        return "U";
    }

    /**
     * @brief Perform reboot
     * @return true if reboot initiated
     */
    static bool performReboot() {
        // Use OpenWRT's reboot command
        return (std::system("reboot &") == 0);
    }

    /**
     * @brief Perform factory reset
     * @return true if factory reset initiated
     */
    static bool performFactoryReset() {
        // Use OpenWRT's firstboot command (erases overlay)
        // Then reboot
        return (std::system("firstboot -y && reboot &") == 0);
    }

private:
    /**
     * @brief Read file contents
     * @param path File path
     * @return File contents or empty string
     */
    static std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return "";

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};

} // namespace wpp

#endif // WPP_OPENWRT_DEVICE_INFO_H
