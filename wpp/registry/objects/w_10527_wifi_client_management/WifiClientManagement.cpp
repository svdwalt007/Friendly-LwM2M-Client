/*
 * WifiClientManagement Implementation (Object 10527)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_10527_wifi_client_management/WifiClientManagement.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"
#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10527_WIFI_CLIENT_MANAGEMENT

#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#include <regex>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <dirent.h>

#define TAG "WifiClientManagement"

using namespace wpp;

/* Static object methods */
Object& WifiClientManagement::object(WppClient& client) {
    return client.registry().getObject(WIFI_CLIENT_MANAGEMENT_OBJECT_ID);
}

Instance* WifiClientManagement::createInst(WppClient& client, INST_T instId) {
    return object(client).createInstance(instId);
}

Instance* WifiClientManagement::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool WifiClientManagement::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

/* Instance lifecycle */
WifiClientManagement::WifiClientManagement(Object& object, INST_T instId)
    : Instance(object, instId),
      connectionStartTime(time(nullptr)),
      lastActivityTime(time(nullptr)),
      lastTxBytes(0),
      lastRxBytes(0) {
    WPP_LOGD(TAG, "WifiClientManagement instance %d created", instId);
}

WifiClientManagement::~WifiClientManagement() {
    WPP_LOGD(TAG, "WifiClientManagement instance %d destroyed", instId());
}

/* Initialize resources */
bool WifiClientManagement::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing WifiClientManagement resources for instance %d", instId());

    // Initialize default values
    set<STRING_T>(MAC_ADDRESS_0, macAddress);
    set<STRING_T>(IP_ADDRESS_1, "");
    set<STRING_T>(HOSTNAME_2, "");
    set<STRING_T>(INTERFACE_3, interface);
    set<INT_T>(SIGNAL_STRENGTH_4, -100);
    set<INT_T>(NOISE_LEVEL_5, -100);
    set<INT_T>(SNR_6, 0);
    set<INT_T>(TX_RATE_7, 0);
    set<INT_T>(RX_RATE_8, 0);
    set<INT_T>(TX_BYTES_9, 0);
    set<INT_T>(RX_BYTES_10, 0);
    set<INT_T>(CONNECTED_TIME_11, 0);
    set<STRING_T>(CAPABILITY_12, "unknown");
    set<STRING_T>(BAND_13, "unknown");
    set<INT_T>(BANDWIDTH_14, 20);
    set<STRING_T>(ACL_STATE_15, "none");
    set<INT_T>(QOS_PRIORITY_16, 0);
    set<BOOL_T>(ISOLATED_17, false);
    set<INT_T>(LAST_ACTIVITY_18, 0);

    // Set execute handlers
    setExecute(DISCONNECT_CLIENT_19, disconnectClient);
    setExecute(ADD_TO_BLACKLIST_20, addToBlacklist);
    setExecute(ADD_TO_WHITELIST_21, addToWhitelist);
    setExecute(STEER_TO_BAND_22, steerToBand);

#ifdef OPENWRT_BUILD
    // Load client information from system
    updateClientStats();
#endif

    return true;
}

/* Validation */
bool WifiClientManagement::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case ACL_STATE_15: {
            const STRING_T& state = *(const STRING_T*)data;
            if (state != "none" && state != "allowed" && state != "denied") {
                WPP_LOGE(TAG, "Invalid ACL state: %s", state.c_str());
                return false;
            }
            break;
        }
        case QOS_PRIORITY_16: {
            INT_T priority = *(const INT_T*)data;
            if (priority < 0 || priority > 7) {
                WPP_LOGE(TAG, "Invalid QoS priority: %lld (must be 0-7)", priority);
                return false;
            }
            break;
        }
    }
    return true;
}

/* Update client statistics */
void WifiClientManagement::updateClientStats() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Updating client statistics for MAC: %s", macAddress.c_str());

    // Update IP and hostname
    std::string ip = getClientIPFromMAC(macAddress);
    if (!ip.empty()) {
        set<STRING_T>(IP_ADDRESS_1, ip);
    }

    std::string hostname = getClientHostnameFromMAC(macAddress);
    if (!hostname.empty()) {
        set<STRING_T>(HOSTNAME_2, hostname);
    }

    // Get detailed stats from hostapd
    getClientInfoFromHostapd();

    // Get additional stats from iwinfo
    getClientInfoFromIwinfo();

    // Get stats from ubus
    getClientInfoFromUbus();

    // Update bandwidth stats
    getClientBandwidthStats();

    // Update connected time
    time_t currentTime = time(nullptr);
    int connectedSeconds = currentTime - connectionStartTime;
    set<INT_T>(CONNECTED_TIME_11, connectedSeconds);

    // Update last activity time
    int inactiveSeconds = currentTime - lastActivityTime;
    set<INT_T>(LAST_ACTIVITY_18, inactiveSeconds);
#endif
}

/* Get client info from hostapd control interface */
bool WifiClientManagement::getClientInfoFromHostapd() {
#ifdef OPENWRT_BUILD
    std::string sockPath = getHostapdControlSocket(interface);
    if (sockPath.empty()) {
        WPP_LOGW(TAG, "Could not find hostapd control socket for %s", interface.c_str());
        return false;
    }

    // Use hostapd_cli to get station info
    std::stringstream cmd;
    cmd << "hostapd_cli -i " << interface << " sta " << macAddress << " 2>/dev/null";

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) {
        WPP_LOGE(TAG, "Failed to execute hostapd_cli command");
        return false;
    }

    char buffer[256];
    std::map<std::string, std::string> staInfo;

    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::string line(buffer);
        line.erase(line.find_last_not_of("\n\r") + 1);

        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);
            staInfo[key] = value;
        }
    }
    pclose(pipe);

    // Parse signal strength
    if (staInfo.count("signal")) {
        try {
            int rssi = std::stoi(staInfo["signal"]);
            set<INT_T>(SIGNAL_STRENGTH_4, rssi);
        } catch (...) {}
    }

    // Parse capabilities
    if (staInfo.count("flags")) {
        std::string capability = parseCapabilityString(staInfo["flags"]);
        set<STRING_T>(CAPABILITY_12, capability);

        // Parse bandwidth from flags
        int bandwidth = getBandwidthFromFlags(staInfo["flags"]);
        set<INT_T>(BANDWIDTH_14, bandwidth);
    }

    // Parse connection time (connected_time in seconds)
    if (staInfo.count("connected_time")) {
        try {
            int connTime = std::stoi(staInfo["connected_time"]);
            connectionStartTime = time(nullptr) - connTime;
            set<INT_T>(CONNECTED_TIME_11, connTime);
        } catch (...) {}
    }

    // Parse inactive time (inactive_msec in milliseconds)
    if (staInfo.count("inactive_msec")) {
        try {
            int inactiveMsec = std::stoi(staInfo["inactive_msec"]);
            int inactiveSec = inactiveMsec / 1000;
            set<INT_T>(LAST_ACTIVITY_18, inactiveSec);
            lastActivityTime = time(nullptr) - inactiveSec;
        } catch (...) {}
    }

    // Parse TX/RX rates (in 100 kbps units)
    if (staInfo.count("rx_rate_info")) {
        std::string rxRateInfo = staInfo["rx_rate_info"];
        size_t ratePos = rxRateInfo.find("rate=");
        if (ratePos != std::string::npos) {
            try {
                int rate = std::stoi(rxRateInfo.substr(ratePos + 5));
                set<INT_T>(RX_RATE_8, rate / 10); // Convert to Mbps
            } catch (...) {}
        }
    }

    if (staInfo.count("tx_rate_info")) {
        std::string txRateInfo = staInfo["tx_rate_info"];
        size_t ratePos = txRateInfo.find("rate=");
        if (ratePos != std::string::npos) {
            try {
                int rate = std::stoi(txRateInfo.substr(ratePos + 5));
                set<INT_T>(TX_RATE_7, rate / 10); // Convert to Mbps
            } catch (...) {}
        }
    }

    // Parse TX/RX bytes
    if (staInfo.count("rx_bytes")) {
        try {
            uint64_t rxBytes = std::stoull(staInfo["rx_bytes"]);
            set<INT_T>(RX_BYTES_10, rxBytes);
            lastRxBytes = rxBytes;
        } catch (...) {}
    }

    if (staInfo.count("tx_bytes")) {
        try {
            uint64_t txBytes = std::stoull(staInfo["tx_bytes"]);
            set<INT_T>(TX_BYTES_9, txBytes);
            lastTxBytes = txBytes;
        } catch (...) {}
    }

    return true;
#else
    return false;
#endif
}

/* Get client info from iwinfo library */
bool WifiClientManagement::getClientInfoFromIwinfo() {
#ifdef OPENWRT_BUILD
    // Use iw command to get additional information
    std::stringstream cmd;
    cmd << "iw dev " << interface << " station get " << macAddress << " 2>/dev/null";

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) {
        return false;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::string line(buffer);

        // Parse signal strength
        if (line.find("signal:") != std::string::npos) {
            std::regex signalRegex("signal:\\s+(-?\\d+)");
            std::smatch match;
            if (std::regex_search(line, match, signalRegex)) {
                int rssi = std::stoi(match[1].str());
                set<INT_T>(SIGNAL_STRENGTH_4, rssi);
            }
        }

        // Parse noise
        if (line.find("signal avg:") != std::string::npos) {
            std::regex noiseRegex("signal avg:\\s+(-?\\d+)");
            std::smatch match;
            if (std::regex_search(line, match, noiseRegex)) {
                int rssiAvg = std::stoi(match[1].str());
                // Estimate noise (typical WiFi noise floor)
                int noise = -95;
                set<INT_T>(NOISE_LEVEL_5, noise);
                set<INT_T>(SNR_6, rssiAvg - noise);
            }
        }

        // Parse TX bitrate
        if (line.find("tx bitrate:") != std::string::npos) {
            std::regex rateRegex("tx bitrate:\\s+(\\d+\\.\\d+)");
            std::smatch match;
            if (std::regex_search(line, match, rateRegex)) {
                float rate = std::stof(match[1].str());
                set<INT_T>(TX_RATE_7, static_cast<int>(rate));
            }
        }

        // Parse RX bitrate
        if (line.find("rx bitrate:") != std::string::npos) {
            std::regex rateRegex("rx bitrate:\\s+(\\d+\\.\\d+)");
            std::smatch match;
            if (std::regex_search(line, match, rateRegex)) {
                float rate = std::stof(match[1].str());
                set<INT_T>(RX_RATE_8, static_cast<int>(rate));
            }
        }

        // Parse RX/TX bytes
        if (line.find("rx bytes:") != std::string::npos) {
            std::regex bytesRegex("rx bytes:\\s+(\\d+)");
            std::smatch match;
            if (std::regex_search(line, match, bytesRegex)) {
                uint64_t rxBytes = std::stoull(match[1].str());
                set<INT_T>(RX_BYTES_10, rxBytes);
            }
        }

        if (line.find("tx bytes:") != std::string::npos) {
            std::regex bytesRegex("tx bytes:\\s+(\\d+)");
            std::smatch match;
            if (std::regex_search(line, match, bytesRegex)) {
                uint64_t txBytes = std::stoull(match[1].str());
                set<INT_T>(TX_BYTES_9, txBytes);
            }
        }
    }
    pclose(pipe);

    // Get frequency to determine band
    cmd.str("");
    cmd << "iw dev " << interface << " info 2>/dev/null | grep channel";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            std::string line(buffer);
            std::regex freqRegex("(\\d+) MHz");
            std::smatch match;
            if (std::regex_search(line, match, freqRegex)) {
                int freq = std::stoi(match[1].str());
                std::string band = determineBand(freq);
                set<STRING_T>(BAND_13, band);
            }
        }
        pclose(pipe);
    }

    return true;
#else
    return false;
#endif
}

/* Get client info from ubus */
bool WifiClientManagement::getClientInfoFromUbus() {
#ifdef OPENWRT_BUILD
    // Use ubus to get hostapd information
    std::stringstream cmd;
    cmd << "ubus call hostapd." << interface << " get_clients 2>/dev/null";

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) {
        return false;
    }

    std::stringstream result;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result << buffer;
    }
    pclose(pipe);

    std::string output = result.str();

    // Simple parsing - look for our MAC address in the output
    if (output.find(macAddress) != std::string::npos) {
        // Parse JSON-like output (simplified)
        // In a real implementation, use a JSON library
        WPP_LOGD(TAG, "Found client in ubus hostapd output");
        return true;
    }

    return false;
#else
    return false;
#endif
}

/* Get client bandwidth statistics */
bool WifiClientManagement::getClientBandwidthStats() {
#ifdef OPENWRT_BUILD
    // Read from /proc/net/dev or use tc/iptables statistics
    // This is a simplified implementation

    // Update bandwidth counters if we have new data
    uint64_t currentTxBytes = get<INT_T>(TX_BYTES_9);
    uint64_t currentRxBytes = get<INT_T>(RX_BYTES_10);

    if (currentTxBytes > lastTxBytes) {
        lastTxBytes = currentTxBytes;
    }

    if (currentRxBytes > lastRxBytes) {
        lastRxBytes = currentRxBytes;
    }

    return true;
#else
    return false;
#endif
}

/* Enumerate all connected WiFi clients */
void WifiClientManagement::enumerateClients(WppClient& client) {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Enumerating WiFi clients");

    // Get list of WiFi interfaces
    std::vector<std::string> interfaces;

    DIR* dir = opendir("/sys/class/net");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string ifname(entry->d_name);
            if (ifname.find("wlan") != std::string::npos ||
                ifname.find("phy") != std::string::npos) {
                interfaces.push_back(ifname);
            }
        }
        closedir(dir);
    }

    // For each interface, get connected clients
    for (const auto& iface : interfaces) {
        std::stringstream cmd;
        cmd << "iw dev " << iface << " station dump 2>/dev/null | grep Station | awk '{print $2}'";

        FILE* pipe = popen(cmd.str().c_str(), "r");
        if (!pipe) continue;

        char buffer[64];
        while (fgets(buffer, sizeof(buffer), pipe)) {
            std::string mac(buffer);
            mac.erase(mac.find_last_not_of("\n\r") + 1);

            if (mac.empty()) continue;

            // Check if instance already exists for this MAC
            bool found = false;
            Object& obj = WifiClientManagement::object(client);

            // Simple search through instances (in production, use a better method)
            for (INST_T i = 0; i < 65535; i++) {
                Instance* inst = obj.instance(i);
                if (inst) {
                    WifiClientManagement* wifiInst = static_cast<WifiClientManagement*>(inst);
                    if (wifiInst->macAddress == mac) {
                        found = true;
                        wifiInst->updateClientStats();
                        break;
                    }
                }
            }

            // Create new instance if not found
            if (!found) {
                WifiClientManagement* newInst = static_cast<WifiClientManagement*>(
                    WifiClientManagement::createInst(client)
                );
                if (newInst) {
                    newInst->macAddress = mac;
                    newInst->interface = iface;
                    newInst->initResources(nullptr);
                    WPP_LOGI(TAG, "Created instance for new client: %s on %s",
                             mac.c_str(), iface.c_str());
                }
            }
        }
        pclose(pipe);
    }
#endif
}

/* Execute handler: Disconnect Client */
bool WifiClientManagement::disconnectClient(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WifiClientManagement& client = static_cast<WifiClientManagement&>(inst);

    WPP_LOGI(TAG, "Disconnecting client: %s", client.macAddress.c_str());

#ifdef OPENWRT_BUILD
    // Use hostapd_cli to deauthenticate the client
    std::stringstream cmd;
    cmd << "hostapd_cli -i " << client.interface << " deauthenticate "
        << client.macAddress << " 2>/dev/null";

    int result = system(cmd.str().c_str());
    if (result == 0) {
        WPP_LOGI(TAG, "Successfully disconnected client: %s", client.macAddress.c_str());
        return true;
    } else {
        WPP_LOGE(TAG, "Failed to disconnect client: %s", client.macAddress.c_str());
        return false;
    }
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot disconnect client");
    return false;
#endif
}

/* Execute handler: Add to Blacklist */
bool WifiClientManagement::addToBlacklist(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WifiClientManagement& client = static_cast<WifiClientManagement&>(inst);

    WPP_LOGI(TAG, "Adding client to blacklist: %s", client.macAddress.c_str());

#ifdef OPENWRT_BUILD
    // Add to hostapd deny list
    std::stringstream cmd;
    cmd << "hostapd_cli -i " << client.interface << " deny_acl ADD_MAC "
        << client.macAddress << " 2>/dev/null";

    int result = system(cmd.str().c_str());
    if (result == 0) {
        client.set<STRING_T>(ACL_STATE_15, "denied");

        // Also disconnect the client
        cmd.str("");
        cmd << "hostapd_cli -i " << client.interface << " deauthenticate "
            << client.macAddress << " 2>/dev/null";
        system(cmd.str().c_str());

        WPP_LOGI(TAG, "Successfully added client to blacklist: %s", client.macAddress.c_str());
        return true;
    } else {
        WPP_LOGE(TAG, "Failed to add client to blacklist: %s", client.macAddress.c_str());
        return false;
    }
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot add to blacklist");
    return false;
#endif
}

/* Execute handler: Add to Whitelist */
bool WifiClientManagement::addToWhitelist(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WifiClientManagement& client = static_cast<WifiClientManagement&>(inst);

    WPP_LOGI(TAG, "Adding client to whitelist: %s", client.macAddress.c_str());

#ifdef OPENWRT_BUILD
    // Add to hostapd accept list
    std::stringstream cmd;
    cmd << "hostapd_cli -i " << client.interface << " accept_acl ADD_MAC "
        << client.macAddress << " 2>/dev/null";

    int result = system(cmd.str().c_str());
    if (result == 0) {
        client.set<STRING_T>(ACL_STATE_15, "allowed");

        // Remove from deny list if present
        cmd.str("");
        cmd << "hostapd_cli -i " << client.interface << " deny_acl DEL_MAC "
            << client.macAddress << " 2>/dev/null";
        system(cmd.str().c_str());

        WPP_LOGI(TAG, "Successfully added client to whitelist: %s", client.macAddress.c_str());
        return true;
    } else {
        WPP_LOGE(TAG, "Failed to add client to whitelist: %s", client.macAddress.c_str());
        return false;
    }
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot add to whitelist");
    return false;
#endif
}

/* Execute handler: Steer to Band */
bool WifiClientManagement::steerToBand(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WifiClientManagement& client = static_cast<WifiClientManagement&>(inst);

    // Extract target band from data (expected: "2.4", "5", or "6")
    std::string targetBand;
    if (data.size() > 0) {
        targetBand = std::string(reinterpret_cast<const char*>(data.data()), data.size());
    } else {
        WPP_LOGE(TAG, "No target band specified for steering");
        return false;
    }

    WPP_LOGI(TAG, "Steering client %s to %s GHz band",
             client.macAddress.c_str(), targetBand.c_str());

#ifdef OPENWRT_BUILD
    return client.sendBssTransitionRequest(client.macAddress, targetBand);
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot steer client");
    return false;
#endif
}

/* Send BSS Transition Management Request (802.11v) */
bool WifiClientManagement::sendBssTransitionRequest(const std::string& mac, const std::string& targetBand) {
#ifdef OPENWRT_BUILD
    // This requires 802.11v support in hostapd
    // Send BSS transition management request to steer client to different band

    std::stringstream cmd;
    cmd << "hostapd_cli -i " << interface << " bss_tm_req " << mac
        << " pref=1 abridged=1 valid_int=255 2>/dev/null";

    int result = system(cmd.str().c_str());
    if (result == 0) {
        WPP_LOGI(TAG, "Sent BSS transition request to %s", mac.c_str());
        return true;
    } else {
        WPP_LOGE(TAG, "Failed to send BSS transition request to %s", mac.c_str());
        return false;
    }
#else
    return false;
#endif
}

/* Execute hostapd control command */
bool WifiClientManagement::executeHostapdCommand(const std::string& interface,
                                                  const std::string& command) {
#ifdef OPENWRT_BUILD
    std::stringstream cmd;
    cmd << "hostapd_cli -i " << interface << " " << command << " 2>/dev/null";

    int result = system(cmd.str().c_str());
    return (result == 0);
#else
    return false;
#endif
}

/* Get hostapd control socket path */
std::string WifiClientManagement::getHostapdControlSocket(const std::string& interface) {
    // Typical paths: /var/run/hostapd/<interface>
    std::string sockPath = "/var/run/hostapd/" + interface;

    // Check if socket exists
    std::ifstream f(sockPath);
    if (f.good()) {
        return sockPath;
    }

    // Try alternative path
    sockPath = "/var/run/hostapd-" + interface;
    std::ifstream f2(sockPath);
    if (f2.good()) {
        return sockPath;
    }

    return "";
}

/* Get client IP address from MAC */
std::string WifiClientManagement::getClientIPFromMAC(const std::string& mac) {
#ifdef OPENWRT_BUILD
    // Check ARP cache
    std::stringstream cmd;
    cmd << "ip neigh show | grep -i " << mac << " | awk '{print $1}'";

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) return "";

    char buffer[64];
    if (fgets(buffer, sizeof(buffer), pipe)) {
        std::string ip(buffer);
        ip.erase(ip.find_last_not_of("\n\r") + 1);
        pclose(pipe);
        return ip;
    }
    pclose(pipe);

    // Check DHCP leases
    cmd.str("");
    cmd << "grep -i " << mac << " /tmp/dhcp.leases 2>/dev/null | awk '{print $3}'";

    pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) return "";

    if (fgets(buffer, sizeof(buffer), pipe)) {
        std::string ip(buffer);
        ip.erase(ip.find_last_not_of("\n\r") + 1);
        pclose(pipe);
        return ip;
    }
    pclose(pipe);
#endif

    return "";
}

/* Get client hostname from MAC */
std::string WifiClientManagement::getClientHostnameFromMAC(const std::string& mac) {
#ifdef OPENWRT_BUILD
    // Check DHCP leases
    std::stringstream cmd;
    cmd << "grep -i " << mac << " /tmp/dhcp.leases 2>/dev/null | awk '{print $4}'";

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) return "";

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), pipe)) {
        std::string hostname(buffer);
        hostname.erase(hostname.find_last_not_of("\n\r") + 1);
        pclose(pipe);

        if (hostname == "*" || hostname.empty()) {
            return "unknown";
        }
        return hostname;
    }
    pclose(pipe);
#endif

    return "unknown";
}

/* Parse capability string from hostapd flags */
std::string WifiClientManagement::parseCapabilityString(const std::string& flags) {
    // Parse WiFi standard from flags
    if (flags.find("[HE]") != std::string::npos) return "802.11ax";
    if (flags.find("[VHT]") != std::string::npos) return "802.11ac";
    if (flags.find("[HT]") != std::string::npos) return "802.11n";
    if (flags.find("[ERP]") != std::string::npos) return "802.11g";

    // Default fallback
    return "802.11a/b/g";
}

/* Determine band from frequency */
std::string WifiClientManagement::determineBand(int frequency) {
    if (frequency >= 5955 && frequency <= 7115) {
        return "6GHz";
    } else if (frequency >= 5170 && frequency <= 5835) {
        return "5GHz";
    } else if (frequency >= 2412 && frequency <= 2484) {
        return "2.4GHz";
    }
    return "unknown";
}

/* Get bandwidth from flags */
int WifiClientManagement::getBandwidthFromFlags(const std::string& flags) {
    if (flags.find("160MHz") != std::string::npos) return 160;
    if (flags.find("80MHz") != std::string::npos) return 80;
    if (flags.find("40MHz") != std::string::npos) return 40;
    return 20; // Default
}

/* Validate MAC address */
bool WifiClientManagement::isValidMAC(const std::string& mac) {
    std::regex macRegex("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");
    return std::regex_match(mac, macRegex);
}

#endif // OBJ_W_10527_WIFI_CLIENT_MANAGEMENT
