/*
 * BearerSelection.cpp
 * OMA LwM2M Object ID 13 - Bearer Selection
 *
 * This object manages preferred network bearer selection
 * Supports automatic bearer switching based on availability and signal strength
 */

#include "BearerSelection.h"
#include "BearerSelectionInfo.h"
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
#include <vector>
#endif

#define TAG "BearerSelection"

namespace wpp {

/* Static object methods */
Object& BearerSelection::object(WppClient& client) {
    return client.registry().bearerSelection();
}

Instance* BearerSelection::createInst(WppClient& client, INST_T instId) {
    Object& obj = object(client);
    return obj.createInstance(instId);
}

Instance* BearerSelection::instance(WppClient& client, INST_T instId) {
    Object& obj = object(client);
    return obj.instance(instId);
}

bool BearerSelection::remove(WppClient& client, INST_T instId) {
    Object& obj = object(client);
    return obj.remove(instId);
}

/* Constructor */
BearerSelection::BearerSelection(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "BearerSelection instance created");
}

/* Destructor */
BearerSelection::~BearerSelection() {
    WPP_LOGD(TAG, "BearerSelection instance destroyed");
}

/* Resource initialization */
bool BearerSelection::initResources(ItemOp *) {
    // Resource 0: Preferred Communications Bearer (RW)
    // Default preference: WLAN > Ethernet > LTE > WCDMA > GSM
    // Format: comma-separated list of bearer IDs in order of preference
    set<STRING_T>(PREFERRED_COMMS_BEARER_0, "21,41,6,2,0");

    // Resource 1: Acceptable RSSI for GSM (RW)
    // Range: -110 to -48 dBm, default: -70 dBm
    set<INT_T>(ACCEPTABLE_RSSI_GSM_1, -70);

    // Resource 2: Acceptable RSCP for UMTS (RW)
    // Range: -120 to -25 dBm, default: -85 dBm
    set<INT_T>(ACCEPTABLE_RSCP_UMTS_2, -85);

    // Resource 3: Acceptable RSRP for LTE (RW)
    // Range: -140 to -44 dBm, default: -95 dBm
    set<INT_T>(ACCEPTABLE_RSRP_LTE_3, -95);

    // Resource 4: Acceptable RSSI for WLAN (RW)
    // Range: -100 to -30 dBm, default: -70 dBm
    set<INT_T>(ACCEPTABLE_RSSI_WLAN_4, -70);

    // Resource 5: Cell Lock List (RW)
    // Format: comma-separated list of cell IDs
    set<STRING_T>(CELL_LOCK_LIST_5, "");

    // Resource 6: Operator List (RW)
    // Format: comma-separated list of MCC-MNC codes (e.g., "310-410,311-480")
    set<STRING_T>(OPERATOR_LIST_6, "");

    // Resource 7: Operator List Mode (RW)
    // 0 = Whitelist (only connect to listed operators)
    // 1 = Blacklist (avoid listed operators)
    set<INT_T>(OPERATOR_LIST_MODE_7, OperatorListMode::WHITELIST);

    // Resource 8: Available Network Bearers (R)
    // Automatically detected available bearers
    set<STRING_T>(AVAILABLE_NETWORK_BEARERS_8, "");

    // Resource 9: Acceptable Signal Strength Variation (RW)
    // Hysteresis value in dB to prevent frequent switching
    // Range: 0-20 dB, default: 5 dB
    set<INT_T>(ACCEPTABLE_SIGNAL_STRENGTH_VAR_9, 5);

    // Resource 10: Higher Priority PLMN Search Timer (RW)
    // Time in seconds to search for higher priority PLMN
    // Range: 60-65535 seconds, default: 600 seconds (10 minutes)
    set<INT_T>(HIGHER_PRIORITY_PLMN_SEARCH_TIMER_10, 600);

    // Resource 11: Attach Without PDN Connection (RW)
    // false = Attach with PDN, true = Attach without PDN
    set<BOOL_T>(ATTACH_WITHOUT_PDN_11, false);

#ifdef OPENWRT_BUILD
    // Load configuration from UCI
    loadFromUCI();

    // Detect available bearers
    updateAvailableBearers();

    // Set up periodic bearer monitoring (every 60 seconds)
    WppTaskQueue::addTask(60, [this](WppClient& client, void* ctx) {
        updateAvailableBearers();
        return false; // Keep running
    });
#endif

    WPP_LOGI(TAG, "BearerSelection instance initialized");

    return true;
}

/* Validation */
bool BearerSelection::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case ACCEPTABLE_RSSI_GSM_1: {
            INT_T rssi = *(INT_T*)data;
            if (rssi < -110 || rssi > -48) {
                WPP_LOGE(TAG, "Invalid GSM RSSI: %d (valid range: -110 to -48)", rssi);
                return false;
            }
            break;
        }

        case ACCEPTABLE_RSCP_UMTS_2: {
            INT_T rscp = *(INT_T*)data;
            if (rscp < -120 || rscp > -25) {
                WPP_LOGE(TAG, "Invalid UMTS RSCP: %d (valid range: -120 to -25)", rscp);
                return false;
            }
            break;
        }

        case ACCEPTABLE_RSRP_LTE_3: {
            INT_T rsrp = *(INT_T*)data;
            if (rsrp < -140 || rsrp > -44) {
                WPP_LOGE(TAG, "Invalid LTE RSRP: %d (valid range: -140 to -44)", rsrp);
                return false;
            }
            break;
        }

        case ACCEPTABLE_RSSI_WLAN_4: {
            INT_T rssi = *(INT_T*)data;
            if (rssi < -100 || rssi > -30) {
                WPP_LOGE(TAG, "Invalid WLAN RSSI: %d (valid range: -100 to -30)", rssi);
                return false;
            }
            break;
        }

        case ACCEPTABLE_SIGNAL_STRENGTH_VAR_9: {
            INT_T variation = *(INT_T*)data;
            if (variation < 0 || variation > 20) {
                WPP_LOGE(TAG, "Invalid signal strength variation: %d (valid range: 0-20)", variation);
                return false;
            }
            break;
        }

        case HIGHER_PRIORITY_PLMN_SEARCH_TIMER_10: {
            INT_T timer = *(INT_T*)data;
            if (timer < 60 || timer > 65535) {
                WPP_LOGE(TAG, "Invalid PLMN search timer: %d (valid range: 60-65535)", timer);
                return false;
            }
            break;
        }

        case OPERATOR_LIST_MODE_7: {
            INT_T mode = *(INT_T*)data;
            if (mode != OperatorListMode::WHITELIST && mode != OperatorListMode::BLACKLIST) {
                WPP_LOGE(TAG, "Invalid operator list mode: %d", mode);
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
void BearerSelection::loadFromUCI() {
    // Try to read bearer preferences from UCI
    std::string cmd = "uci get network.bearer.preference 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            std::string pref(buffer);
            // Remove trailing newline
            if (!pref.empty() && pref[pref.length()-1] == '\n') {
                pref.erase(pref.length()-1);
            }
            if (!pref.empty()) {
                set<STRING_T>(PREFERRED_COMMS_BEARER_0, pref);
            }
        }
        pclose(pipe);
    }

    // Read signal thresholds if configured
    cmd = "uci get network.bearer.wlan_rssi 2>/dev/null";
    pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[32];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            int rssi = atoi(buffer);
            if (rssi >= -100 && rssi <= -30) {
                set<INT_T>(ACCEPTABLE_RSSI_WLAN_4, rssi);
            }
        }
        pclose(pipe);
    }
}

/* Update available network bearers */
void BearerSelection::updateAvailableBearers() {
    std::vector<INT_T> availableBearers;

    // Check for Ethernet
    struct stat statbuf;
    if (stat("/sys/class/net/eth0", &statbuf) == 0) {
        std::ifstream operstate("/sys/class/net/eth0/operstate");
        if (operstate.is_open()) {
            std::string state;
            std::getline(operstate, state);
            if (state == "up") {
                availableBearers.push_back(NetworkBearer::ETHERNET);
            }
            operstate.close();
        }
    }

    // Check for WLAN
    if (stat("/sys/class/net/wlan0", &statbuf) == 0) {
        std::ifstream operstate("/sys/class/net/wlan0/operstate");
        if (operstate.is_open()) {
            std::string state;
            std::getline(operstate, state);
            if (state == "up") {
                availableBearers.push_back(NetworkBearer::WLAN);
            }
            operstate.close();
        }
    }

    // Check for cellular modems
    if (stat("/sys/class/net/wwan0", &statbuf) == 0) {
        // Try to determine cellular technology
        std::string cmd = "mmcli -m 0 --output-keyvalue 2>/dev/null | grep modem.generic.access-technologies.value";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[256];
            if (fgets(buffer, sizeof(buffer), pipe)) {
                std::string tech(buffer);
                if (tech.find("lte") != std::string::npos) {
                    availableBearers.push_back(NetworkBearer::LTE_FDD);
                } else if (tech.find("umts") != std::string::npos || tech.find("hspa") != std::string::npos) {
                    availableBearers.push_back(NetworkBearer::WCDMA);
                } else if (tech.find("gsm") != std::string::npos || tech.find("gprs") != std::string::npos) {
                    availableBearers.push_back(NetworkBearer::GSM);
                }
            }
            pclose(pipe);
        }
    }

    // Convert to comma-separated string
    std::stringstream ss;
    for (size_t i = 0; i < availableBearers.size(); i++) {
        if (i > 0) ss << ",";
        ss << availableBearers[i];
    }

    std::string availableStr = ss.str();
    set<STRING_T>(AVAILABLE_NETWORK_BEARERS_8, availableStr);

    WPP_LOGI(TAG, "Available bearers: %s", availableStr.c_str());
}

/* Detect current active bearer */
BearerSelection::NetworkBearer BearerSelection::detectCurrentBearer() {
    // Check default route
    std::string cmd = "ip route show default 2>/dev/null | head -1";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            std::string route(buffer);

            if (route.find("wlan") != std::string::npos) {
                pclose(pipe);
                return NetworkBearer::WLAN;
            } else if (route.find("eth") != std::string::npos) {
                pclose(pipe);
                return NetworkBearer::ETHERNET;
            } else if (route.find("wwan") != std::string::npos) {
                pclose(pipe);
                // Try to determine cellular type
                return NetworkBearer::LTE_FDD; // Default to LTE
            }
        }
        pclose(pipe);
    }

    return NetworkBearer::ETHERNET; // Default
}
#endif // OPENWRT_BUILD

} // namespace wpp
