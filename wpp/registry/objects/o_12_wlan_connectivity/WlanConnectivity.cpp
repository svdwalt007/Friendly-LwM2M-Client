/*
 * WlanConnectivity.cpp
 * OMA LwM2M Object ID 12 - WLAN Connectivity
 *
 * This object provides WiFi interface management for 2.4GHz and 5GHz bands
 * Supports Access Point and Client modes
 * Integrates with OpenWRT UCI for configuration
 */

#include "WlanConnectivity.h"
#include "WlanConnectivityInfo.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppTaskQueue.h"
#include "WppLogs.h"

#ifdef OPENWRT_BUILD
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#endif

#define TAG "WlanConnectivity"

namespace wpp {

/* Static object methods */
Object& WlanConnectivity::object(WppClient& client) {
    return client.registry().wlanConnectivity();
}

Instance* WlanConnectivity::createInst(WppClient& client, INST_T instId) {
    Object& obj = object(client);
    return obj.createInstance(instId);
}

Instance* WlanConnectivity::instance(WppClient& client, INST_T instId) {
    Object& obj = object(client);
    return obj.instance(instId);
}

bool WlanConnectivity::remove(WppClient& client, INST_T instId) {
    Object& obj = object(client);
    return obj.remove(instId);
}

/* Constructor */
WlanConnectivity::WlanConnectivity(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "WlanConnectivity instance %d created", instId);
}

/* Destructor */
WlanConnectivity::~WlanConnectivity() {
    WPP_LOGD(TAG, "WlanConnectivity instance destroyed");
}

/* Resource initialization */
bool WlanConnectivity::initResources(ItemOp *) {
    std::string interfaceName;

#ifdef OPENWRT_BUILD
    // Determine interface name based on instance ID
    // Instance 0 = wlan0 (2.4GHz), Instance 1 = wlan1 (5GHz)
    if (instId() == 0) {
        interfaceName = "wlan0";
    } else if (instId() == 1) {
        interfaceName = "wlan1";
    } else {
        interfaceName = "wlan" + std::to_string(instId());
    }
#else
    interfaceName = "wlan" + std::to_string(instId());
#endif

    // Resource 0: Interface Name (R)
    set<STRING_T>(INTERFACE_NAME_0, interfaceName);

    // Resource 1: Enable (RW)
    set<BOOL_T>(ENABLE_1, true);

    // Resource 2: Radio Enabled (RW)
    set<BOOL_T>(RADIO_ENABLED_2, true);

    // Resource 3: Status (R)
    set<INT_T>(STATUS_3, InterfaceStatus::DISABLED);

    // Resource 4: BSSID (R)
    set<STRING_T>(BSSID_4, "00:00:00:00:00:00");

    // Resource 5: SSID (RW)
    set<STRING_T>(SSID_5, "OpenWRT");

    // Resource 6: Broadcast SSID (RW)
    set<BOOL_T>(BROADCAST_SSID_6, true);

    // Resource 7: Beacon Enabled (RW)
    set<BOOL_T>(BEACON_ENABLED_7, true);

    // Resource 8: Mode (RW)
    set<INT_T>(MODE_8, OperatingMode::ACCESS_POINT);

    // Resource 9: Channel (RW)
    if (instId() == 0) {
        set<INT_T>(CHANNEL_9, 6);  // 2.4GHz default channel
    } else {
        set<INT_T>(CHANNEL_9, 36); // 5GHz default channel
    }

    // Resource 10: Auto Channel (RW)
    set<BOOL_T>(AUTO_CHANNEL_10, false);

    // Resource 11: Supported Channels (R)
    if (instId() == 0) {
        // 2.4GHz channels
        set<STRING_T>(SUPPORTED_CHANNELS_11, "1,2,3,4,5,6,7,8,9,10,11,12,13");
    } else {
        // 5GHz channels
        set<STRING_T>(SUPPORTED_CHANNELS_11, "36,40,44,48,52,56,60,64,100,104,108,112,116,120,124,128,132,136,140,149,153,157,161,165");
    }

    // Resource 12: Channels In Use (R)
    set<STRING_T>(CHANNELS_IN_USE_12, "");

    // Resource 13: Regulatory Domain (RW)
    set<STRING_T>(REGULATORY_DOMAIN_13, "US");

    // Resource 14: Standard (RW)
    if (instId() == 0) {
        set<INT_T>(STANDARD_14, WifiStandard::IEEE_802_11BGN);
    } else {
        set<INT_T>(STANDARD_14, WifiStandard::IEEE_802_11AC);
    }

    // Resource 15: Authentication Mode (RW)
    set<INT_T>(AUTHENTICATION_MODE_15, AuthMode::AUTH_PSK);

    // Resource 16: Encryption Mode (RW)
    set<INT_T>(ENCRYPTION_MODE_16, EncryptMode::ENC_AES);

    // Resource 17: WPA Pre Shared Key (W)
    set<STRING_T>(WPA_PRE_SHARED_KEY_17, "");

    // Resource 18: WPA Key Phrase (W)
    set<STRING_T>(WPA_KEY_PHRASE_18, "");

    // Resource 19: WEP Encryption Type (RW)
    set<INT_T>(WEP_ENCRYPTION_TYPE_19, WepEncType::WEP_NONE);

    // Resource 20: WEP Key Index (RW)
    set<INT_T>(WEP_KEY_INDEX_20, 1);

    // Resource 21: WEP Key Phrase (W)
    set<STRING_T>(WEP_KEY_PHRASE_21, "");

    // WEP Keys 22-25 (W)
    set<STRING_T>(WEP_KEY_1_22, "");
    set<STRING_T>(WEP_KEY_2_23, "");
    set<STRING_T>(WEP_KEY_3_24, "");
    set<STRING_T>(WEP_KEY_4_25, "");

    // RADIUS Configuration 26-28 (RW)
    set<STRING_T>(RADIUS_SERVER_26, "");
    set<INT_T>(RADIUS_SERVER_PORT_27, 1812);
    set<STRING_T>(RADIUS_SECRET_28, "");

    // WMM (WiFi Multimedia) 29-30
    set<BOOL_T>(WMM_SUPPORTED_29, true);
    set<BOOL_T>(WMM_ENABLED_30, true);

    // MAC Filtering 31-32
    set<BOOL_T>(MAC_CONTROL_ENABLED_31, false);
    set<STRING_T>(MAC_ADDRESS_LIST_32, "");

    // Statistics (R) - Initialize to 0
    set<INT_T>(TOTAL_BYTES_SENT_33, 0);
    set<INT_T>(TOTAL_BYTES_RECEIVED_34, 0);
    set<INT_T>(TOTAL_PACKETS_SENT_35, 0);
    set<INT_T>(TOTAL_PACKETS_RECEIVED_36, 0);
    set<INT_T>(TRANSMIT_ERRORS_37, 0);
    set<INT_T>(RECEIVE_ERRORS_38, 0);
    set<INT_T>(UNICAST_PACKETS_SENT_39, 0);
    set<INT_T>(UNICAST_PACKETS_RECEIVED_40, 0);
    set<INT_T>(MULTICAST_PACKETS_SENT_41, 0);
    set<INT_T>(MULTICAST_PACKETS_RECEIVED_42, 0);
    set<INT_T>(BROADCAST_PACKETS_SENT_43, 0);
    set<INT_T>(BROADCAST_PACKETS_RECEIVED_44, 0);
    set<INT_T>(DISCARD_PACKETS_SENT_45, 0);
    set<INT_T>(DISCARD_PACKETS_RECEIVED_46, 0);
    set<INT_T>(UNKNOWN_PACKETS_RECEIVED_47, 0);

#ifdef OPENWRT_BUILD
    // Load configuration from OpenWRT UCI
    loadFromUCI();

    // Update statistics from sysfs
    updateStatistics();

    // Set up periodic statistics update (every 30 seconds)
    WppTaskQueue::addTask(30, [this](WppClient& client, void* ctx) {
        updateStatistics();
        return false; // Keep running
    });
#endif

    WPP_LOGI(TAG, "WlanConnectivity instance %d initialized: %s",
             instId(), interfaceName.c_str());

    return true;
}

/* Validation */
bool WlanConnectivity::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case CHANNEL_9: {
            INT_T channel = *(INT_T*)data;
            if (instId() == 0) {
                // 2.4GHz: channels 1-13
                if (channel < 1 || channel > 13) {
                    WPP_LOGE(TAG, "Invalid 2.4GHz channel: %d", channel);
                    return false;
                }
            } else {
                // 5GHz: specific channels only
                std::vector<int> validChannels = {36,40,44,48,52,56,60,64,100,104,108,112,116,120,124,128,132,136,140,149,153,157,161,165};
                if (std::find(validChannels.begin(), validChannels.end(), channel) == validChannels.end()) {
                    WPP_LOGE(TAG, "Invalid 5GHz channel: %d", channel);
                    return false;
                }
            }
            break;
        }

        case SSID_5: {
            STRING_T ssid((const char*)data, size);
            if (ssid.empty() || ssid.length() > 32) {
                WPP_LOGE(TAG, "Invalid SSID length: %zu", ssid.length());
                return false;
            }
            break;
        }

        case WPA_KEY_PHRASE_18: {
            STRING_T key((const char*)data, size);
            if (key.length() < 8 || key.length() > 63) {
                WPP_LOGE(TAG, "Invalid WPA key length: %zu (must be 8-63)", key.length());
                return false;
            }
            break;
        }

        default:
            break;
    }

    return true;
}

#ifdef OPENWRT_BUILD
/* Load configuration from OpenWRT UCI */
void WlanConnectivity::loadFromUCI() {
    std::string ifname = get<STRING_T>(INTERFACE_NAME_0);

    // Read wireless configuration
    std::string cmd = "uci get wireless.@wifi-iface[" + std::to_string(instId()) + "].ssid 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            std::string ssid(buffer);
            // Remove trailing newline
            if (!ssid.empty() && ssid[ssid.length()-1] == '\n') {
                ssid.erase(ssid.length()-1);
            }
            if (!ssid.empty()) {
                set<STRING_T>(SSID_5, ssid);
            }
        }
        pclose(pipe);
    }

    // Read device configuration
    cmd = "uci get wireless.radio" + std::to_string(instId()) + ".channel 2>/dev/null";
    pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[32];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            int channel = atoi(buffer);
            if (channel > 0) {
                set<INT_T>(CHANNEL_9, channel);
            }
        }
        pclose(pipe);
    }

    // Check if interface is up
    std::string statusPath = "/sys/class/net/" + ifname + "/operstate";
    std::ifstream statusFile(statusPath);
    if (statusFile.is_open()) {
        std::string status;
        std::getline(statusFile, status);
        if (status == "up") {
            set<INT_T>(STATUS_3, InterfaceStatus::UP);
        } else {
            set<INT_T>(STATUS_3, InterfaceStatus::DISABLED);
        }
        statusFile.close();
    }
}

/* Update statistics from sysfs */
void WlanConnectivity::updateStatistics() {
    std::string ifname = get<STRING_T>(INTERFACE_NAME_0);
    std::string statsPath = "/sys/class/net/" + ifname + "/statistics/";

    // Check if interface exists
    struct stat statbuf;
    if (stat(("/sys/class/net/" + ifname).c_str(), &statbuf) != 0) {
        WPP_LOGD(TAG, "Interface %s not found", ifname.c_str());
        return;
    }

    // Read statistics
    auto readStat = [&](const std::string& name) -> INT_T {
        std::ifstream file(statsPath + name);
        if (file.is_open()) {
            INT_T value;
            file >> value;
            return value;
        }
        return 0;
    };

    // Update byte counters
    set<INT_T>(TOTAL_BYTES_SENT_33, readStat("tx_bytes"));
    set<INT_T>(TOTAL_BYTES_RECEIVED_34, readStat("rx_bytes"));

    // Update packet counters
    set<INT_T>(TOTAL_PACKETS_SENT_35, readStat("tx_packets"));
    set<INT_T>(TOTAL_PACKETS_RECEIVED_36, readStat("rx_packets"));

    // Update error counters
    set<INT_T>(TRANSMIT_ERRORS_37, readStat("tx_errors"));
    set<INT_T>(RECEIVE_ERRORS_38, readStat("rx_errors"));

    // Update multicast/broadcast counters
    set<INT_T>(MULTICAST_PACKETS_SENT_41, readStat("tx_multicast"));
    set<INT_T>(MULTICAST_PACKETS_RECEIVED_42, readStat("rx_multicast"));

    // Update drop counters
    set<INT_T>(DISCARD_PACKETS_SENT_45, readStat("tx_dropped"));
    set<INT_T>(DISCARD_PACKETS_RECEIVED_46, readStat("rx_dropped"));

    // Read BSSID if available
    std::string bssidPath = "/sys/class/net/" + ifname + "/address";
    std::ifstream bssidFile(bssidPath);
    if (bssidFile.is_open()) {
        std::string bssid;
        std::getline(bssidFile, bssid);
        if (!bssid.empty()) {
            set<STRING_T>(BSSID_4, bssid);
        }
        bssidFile.close();
    }

    WPP_LOGD(TAG, "Statistics updated for %s: TX=%lld bytes, RX=%lld bytes",
             ifname.c_str(),
             get<INT_T>(TOTAL_BYTES_SENT_33),
             get<INT_T>(TOTAL_BYTES_RECEIVED_34));
}
#endif // OPENWRT_BUILD

} // namespace wpp
