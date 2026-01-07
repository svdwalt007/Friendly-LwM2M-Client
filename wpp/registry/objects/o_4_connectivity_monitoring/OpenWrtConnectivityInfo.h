/*
 * OpenWrtConnectivityInfo.h
 *
 * OpenWRT-specific Connectivity Monitoring Object (4) implementation helper
 * Provides real network connectivity data for OpenWRT One Router
 */

#ifndef WPP_OPENWRT_CONNECTIVITY_INFO_H
#define WPP_OPENWRT_CONNECTIVITY_INFO_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <dirent.h>

namespace wpp {

/**
 * @brief OpenWRT Connectivity Information Helper
 *
 * Provides OpenWRT-specific implementations for Connectivity Monitoring Object
 * resources tailored for OpenWRT One router networking capabilities.
 */
class OpenWrtConnectivityInfo {
public:
    /**
     * @brief Network interface information
     */
    struct InterfaceInfo {
        std::string name;
        std::string ipv4;
        std::string ipv6;
        std::string mac;
        bool isUp;
        int signalStrength;  // dBm for wireless
        int linkQuality;     // 0-100 for wireless
        uint64_t rxBytes;
        uint64_t txBytes;
    };

    /**
     * @brief Get network bearer type (ETHERNET, WLAN, etc.)
     * @return Bearer type code
     */
    static int getNetworkBearer() {
        // Check if WAN interface is wireless
        if (isWirelessInterface("wan") || isWirelessInterface("wwan")) {
            return 21; // WLAN
        }

        // Check if using cellular modem
        if (hasCellularModem()) {
            return 6; // LTE_FDD (most common)
        }

        // Default to Ethernet (OpenWRT One has 2.5GbE WAN)
        return 41; // ETHERNET
    }

    /**
     * @brief Get available network bearers
     * @return Vector of available bearer types
     */
    static std::vector<int> getAvailableNetworkBearers() {
        std::vector<int> bearers;

        // OpenWRT One always has Ethernet
        bearers.push_back(41); // ETHERNET

        // Check for WLAN interfaces
        if (hasWirelessInterface()) {
            bearers.push_back(21); // WLAN
        }

        // Check for cellular modem (USB)
        if (hasCellularModem()) {
            bearers.push_back(6); // LTE_FDD
        }

        return bearers;
    }

    /**
     * @brief Get radio signal strength
     * @return Signal strength in dBm (for wireless) or 0
     */
    static int getRadioSignalStrength() {
        // Try to get signal from wireless interfaces
        for (const auto& iface : {"wlan0", "wlan1", "wwan0"}) {
            int signal = getWirelessSignal(iface);
            if (signal != 0) {
                return signal;
            }
        }

        // For Ethernet, return nominal value
        return -50; // Good signal strength indicator
    }

    /**
     * @brief Get link quality
     * @return Link quality percentage (0-100)
     */
    static int getLinkQuality() {
        // For wireless interfaces
        for (const auto& iface : {"wlan0", "wlan1", "wwan0"}) {
            int quality = getWirelessQuality(iface);
            if (quality != 0) {
                return quality;
            }
        }

        // For Ethernet, check link speed and return quality based on that
        if (isInterfaceUp("eth1")) {
            int speed = getLinkSpeed("eth1");
            if (speed >= 1000) return 100; // Gigabit or better
            if (speed >= 100) return 70;   // 100Mbps
            if (speed >= 10) return 40;    // 10Mbps
        }

        return 50; // Default moderate quality
    }

    /**
     * @brief Get IP addresses
     * @return Vector of IP addresses (IPv4 and IPv6)
     */
    static std::vector<std::string> getIpAddresses() {
        std::vector<std::string> addresses;

        // Get primary WAN interface addresses
        std::vector<std::string> wanAddrs = getInterfaceAddresses("eth1");
        addresses.insert(addresses.end(), wanAddrs.begin(), wanAddrs.end());

        // Also include LAN for management
        std::vector<std::string> lanAddrs = getInterfaceAddresses("br-lan");
        addresses.insert(addresses.end(), lanAddrs.begin(), lanAddrs.end());

        // Remove duplicates
        std::sort(addresses.begin(), addresses.end());
        addresses.erase(std::unique(addresses.begin(), addresses.end()), addresses.end());

        return addresses;
    }

    /**
     * @brief Get router IP addresses (gateway)
     * @return Vector of router/gateway IP addresses
     */
    static std::vector<std::string> getRouterIpAddresses() {
        std::vector<std::string> gateways;

        // Read default gateway from routing table
        std::ifstream route("/proc/net/route");
        std::string line;

        // Skip header
        std::getline(route, line);

        while (std::getline(route, line)) {
            std::istringstream iss(line);
            std::string iface, dest, gateway;
            iss >> iface >> dest >> gateway;

            // Default route has destination 00000000
            if (dest == "00000000" && gateway != "00000000") {
                // Convert hex to IP
                unsigned int gw;
                std::sscanf(gateway.c_str(), "%X", &gw);

                char ip[16];
                std::snprintf(ip, sizeof(ip), "%d.%d.%d.%d",
                    (gw & 0xFF), ((gw >> 8) & 0xFF),
                    ((gw >> 16) & 0xFF), ((gw >> 24) & 0xFF));

                gateways.push_back(std::string(ip));
            }
        }

        // Also check for IPv6 default gateway
        std::ifstream route6("/proc/net/ipv6_route");
        while (std::getline(route6, line)) {
            if (line.find("00000000000000000000000000000000 00") == 0) {
                // Parse IPv6 gateway
                std::istringstream iss(line);
                std::string dest, prefix, src, slen, gateway;
                iss >> dest >> prefix >> src >> slen >> gateway;

                if (gateway != "00000000000000000000000000000000") {
                    // Convert hex to IPv6 (simplified)
                    gateways.push_back(formatIPv6(gateway));
                }
                break;
            }
        }

        return gateways;
    }

    /**
     * @brief Get link utilization
     * @return Link utilization percentage (0-100)
     */
    static int getLinkUtilization() {
        // Calculate based on current traffic and link speed
        static uint64_t lastRx = 0, lastTx = 0;
        static time_t lastTime = 0;

        InterfaceInfo info = getInterfaceInfo("eth1"); // WAN interface
        time_t now = time(nullptr);

        if (lastTime == 0) {
            lastRx = info.rxBytes;
            lastTx = info.txBytes;
            lastTime = now;
            return 0;
        }

        time_t deltaTime = now - lastTime;
        if (deltaTime == 0) return 0;

        uint64_t deltaRx = info.rxBytes - lastRx;
        uint64_t deltaTx = info.txBytes - lastTx;

        // Calculate bandwidth in bps
        uint64_t bps = ((deltaRx + deltaTx) * 8) / deltaTime;

        // Get link speed
        int linkSpeedMbps = getLinkSpeed("eth1");
        if (linkSpeedMbps == 0) linkSpeedMbps = 1000; // Assume 1Gbps

        uint64_t linkSpeedBps = (uint64_t)linkSpeedMbps * 1000000;

        // Calculate utilization percentage
        int utilization = (int)((bps * 100) / linkSpeedBps);
        if (utilization > 100) utilization = 100;

        // Update last values
        lastRx = info.rxBytes;
        lastTx = info.txBytes;
        lastTime = now;

        return utilization;
    }

    /**
     * @brief Get APN (for cellular connections)
     * @return APN string or empty
     */
    static std::vector<std::string> getAPN() {
        std::vector<std::string> apns;

        // Try to read from modem configuration
        std::string apn = readFile("/tmp/modem_apn");
        if (!apn.empty()) {
            apn.erase(std::remove(apn.end()-1, apn.end(), '\n'), apn.end());
            apns.push_back(apn);
        }

        return apns;
    }

    /**
     * @brief Get Cell ID (for cellular connections)
     * @return Cell ID or 0
     */
    static int getCellId() {
        // Try to read from modem info
        std::string cellId = readFile("/tmp/modem_cell_id");
        if (!cellId.empty()) {
            return std::atoi(cellId.c_str());
        }
        return 0;
    }

    /**
     * @brief Get SMNC (Mobile Network Code)
     * @return SMNC or 0
     */
    static int getSMNC() {
        std::string smnc = readFile("/tmp/modem_mnc");
        if (!smnc.empty()) {
            return std::atoi(smnc.c_str());
        }
        return 0;
    }

    /**
     * @brief Get SMCC (Mobile Country Code)
     * @return SMCC or 0
     */
    static int getSMCC() {
        std::string smcc = readFile("/tmp/modem_mcc");
        if (!smcc.empty()) {
            return std::atoi(smcc.c_str());
        }
        return 0;
    }

private:
    /**
     * @brief Check if interface is wireless
     */
    static bool isWirelessInterface(const std::string& iface) {
        std::string wirelessPath = "/sys/class/net/" + iface + "/wireless";
        std::ifstream file(wirelessPath);
        return file.good();
    }

    /**
     * @brief Check if any wireless interface exists
     */
    static bool hasWirelessInterface() {
        DIR* dir = opendir("/sys/class/net");
        if (!dir) return false;

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_name[0] == '.') continue;
            if (isWirelessInterface(entry->d_name)) {
                closedir(dir);
                return true;
            }
        }
        closedir(dir);
        return false;
    }

    /**
     * @brief Check if cellular modem is present
     */
    static bool hasCellularModem() {
        // Check for common modem device paths
        return (access("/dev/cdc-wdm0", F_OK) == 0 ||
                access("/dev/ttyUSB0", F_OK) == 0 ||
                access("/sys/class/net/wwan0", F_OK) == 0);
    }

    /**
     * @brief Get wireless signal strength
     */
    static int getWirelessSignal(const std::string& iface) {
        std::string path = "/sys/class/net/" + iface + "/wireless/signal";
        std::ifstream file(path);
        if (file.good()) {
            int signal;
            file >> signal;
            return signal;
        }

        // Try using iwinfo via ubus
        std::string cmd = "ubus call network.wireless status | grep -A 10 '" + iface + "' | grep signal | awk '{print $2}' | tr -d ','";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[32];
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                pclose(pipe);
                return std::atoi(buffer);
            }
            pclose(pipe);
        }

        return 0;
    }

    /**
     * @brief Get wireless link quality
     */
    static int getWirelessQuality(const std::string& iface) {
        std::string path = "/sys/class/net/" + iface + "/wireless/quality";
        std::ifstream file(path);
        if (file.good()) {
            int quality;
            file >> quality;
            return quality;
        }

        // Calculate from signal strength
        int signal = getWirelessSignal(iface);
        if (signal != 0) {
            // Convert dBm to quality percentage
            // Typical range: -30 dBm (excellent) to -90 dBm (poor)
            if (signal >= -50) return 100;
            if (signal >= -60) return 80;
            if (signal >= -70) return 60;
            if (signal >= -80) return 40;
            if (signal >= -90) return 20;
            return 10;
        }

        return 0;
    }

    /**
     * @brief Check if interface is up
     */
    static bool isInterfaceUp(const std::string& iface) {
        std::string path = "/sys/class/net/" + iface + "/operstate";
        std::string state = readFile(path);
        return (state.find("up") != std::string::npos);
    }

    /**
     * @brief Get interface link speed
     */
    static int getLinkSpeed(const std::string& iface) {
        std::string path = "/sys/class/net/" + iface + "/speed";
        std::ifstream file(path);
        if (file.good()) {
            int speed;
            file >> speed;
            return speed;
        }
        return 0;
    }

    /**
     * @brief Get interface addresses
     */
    static std::vector<std::string> getInterfaceAddresses(const std::string& iface) {
        std::vector<std::string> addresses;

        // Get IPv4 address
        std::string cmd = "ip -4 addr show " + iface + " | grep inet | awk '{print $2}' | cut -d/ -f1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                std::string addr(buffer);
                addr.erase(std::remove(addr.end()-1, addr.end(), '\n'), addr.end());
                if (!addr.empty()) addresses.push_back(addr);
            }
            pclose(pipe);
        }

        // Get IPv6 address
        cmd = "ip -6 addr show " + iface + " | grep inet6 | grep global | awk '{print $2}' | cut -d/ -f1";
        pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                std::string addr(buffer);
                addr.erase(std::remove(addr.end()-1, addr.end(), '\n'), addr.end());
                if (!addr.empty()) addresses.push_back(addr);
            }
            pclose(pipe);
        }

        return addresses;
    }

    /**
     * @brief Get interface information
     */
    static InterfaceInfo getInterfaceInfo(const std::string& iface) {
        InterfaceInfo info;
        info.name = iface;
        info.isUp = isInterfaceUp(iface);
        info.signalStrength = getWirelessSignal(iface);
        info.linkQuality = getWirelessQuality(iface);

        // Get traffic statistics
        std::string rxPath = "/sys/class/net/" + iface + "/statistics/rx_bytes";
        std::string txPath = "/sys/class/net/" + iface + "/statistics/tx_bytes";

        std::ifstream rxFile(rxPath);
        if (rxFile.good()) rxFile >> info.rxBytes;

        std::ifstream txFile(txPath);
        if (txFile.good()) txFile >> info.txBytes;

        return info;
    }

    /**
     * @brief Format IPv6 address from hex string
     */
    static std::string formatIPv6(const std::string& hex) {
        if (hex.length() != 32) return "";

        std::string formatted;
        for (size_t i = 0; i < 32; i += 4) {
            if (i > 0) formatted += ":";
            formatted += hex.substr(i, 4);
        }
        return formatted;
    }

    /**
     * @brief Read file contents
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

#endif // WPP_OPENWRT_CONNECTIVITY_INFO_H
