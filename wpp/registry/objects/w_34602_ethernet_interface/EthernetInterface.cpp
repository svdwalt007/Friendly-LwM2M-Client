/*
 * EthernetInterface (Object 34602)
 * Walt Technologies - OpenWRT One Router
 *
 * Full Linux sysfs integration for Ethernet interface monitoring
 */

#include "w_34602_ethernet_interface/EthernetInterface.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"
#include "WppTaskQueue.h"

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <array>
#include <memory>
#include <algorithm>

#define TAG "EthernetInterface"

namespace wpp {

// Helper function to execute shell commands and capture output
static std::string execCommand(const std::string& cmd) {
    std::array<char, 256> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    // Remove trailing newline and whitespace
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' ')) {
        result.pop_back();
    }
    return result;
}

// Helper function to read a sysfs file value
static std::string readSysfsFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    std::string value;
    std::getline(file, value);
    file.close();

    // Remove trailing whitespace
    while (!value.empty() && (value.back() == '\n' || value.back() == '\r' || value.back() == ' ')) {
        value.pop_back();
    }
    return value;
}

// Helper function to read a sysfs file as integer
static int64_t readSysfsInt(const std::string& path) {
    std::string value = readSysfsFile(path);
    if (value.empty()) {
        return 0;
    }
    try {
        return std::stoll(value);
    } catch (...) {
        return 0;
    }
}

// Helper function to write a sysfs file
static bool writeSysfsFile(const std::string& path, const std::string& value) {
    std::ofstream file(path);
    if (!file.is_open()) {
        WPP_LOGE(TAG, "Failed to open sysfs file for writing: %s", path.c_str());
        return false;
    }
    file << value;
    file.close();
    return true;
}

EthernetInterface::EthernetInterface(Object& object, INST_T instId)
    : Instance(object, instId), _updateTaskId(nullptr) {
    WPP_LOGD(TAG, "Creating EthernetInterface instance %d", instId);
}

EthernetInterface::~EthernetInterface() {
    // Remove periodic update task if it exists
    if (_updateTaskId) {
        WppTaskQueue::removeTask(_updateTaskId);
        _updateTaskId = nullptr;
    }
    WPP_LOGD(TAG, "Destroying EthernetInterface instance");
}

Object& EthernetInterface::object(WppClient& client) {
    return client.registry().ethernetInterface();
}

Instance* EthernetInterface::instance(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().ethernetInterface().instance(instId);
    if (!inst) return NULL;
    return static_cast<EthernetInterface*>(inst);
}

Instance* EthernetInterface::createInst(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().ethernetInterface().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<EthernetInterface*>(inst);
}

bool EthernetInterface::remove(WppClient& client, INST_T instId) {
    return client.registry().ethernetInterface().remove(instId);
}

std::string EthernetInterface::getInterfaceName() const {
    // Get the interface name from resource if set, otherwise use default
    auto res = resource(INTERFACE_NAME_0);
    if (res) {
        return res->get<STRING_T>();
    }
    return "eth0";
}

void EthernetInterface::updateInterfaceStatus() {
    std::string iface = getInterfaceName();
    std::string sysfsBase = "/sys/class/net/" + iface + "/";

    WPP_LOGD(TAG, "Updating status for interface: %s", iface.c_str());

    // Check if interface exists
    std::string operstate = readSysfsFile(sysfsBase + "operstate");
    if (operstate.empty()) {
        WPP_LOGW(TAG, "Interface %s not found in sysfs", iface.c_str());
        return;
    }

    // Read MAC address
    std::string macAddr = readSysfsFile(sysfsBase + "address");
    if (!macAddr.empty()) {
        // Convert to uppercase for consistency
        std::transform(macAddr.begin(), macAddr.end(), macAddr.begin(), ::toupper);
        resource(MAC_ADDRESS_5)->set<STRING_T>(macAddr);
        WPP_LOGD(TAG, "MAC address: %s", macAddr.c_str());
    }

    // Read link status (carrier)
    int64_t carrier = readSysfsInt(sysfsBase + "carrier");
    bool linkUp = (carrier == 1);
    resource(LINK_STATUS_4)->set<BOOL_T>(linkUp);
    WPP_LOGD(TAG, "Link status: %s", linkUp ? "UP" : "DOWN");

    // Read interface enabled status from operstate
    bool enabled = (operstate != "down" && operstate != "unknown");
    resource(INTERFACE_ENABLED_13)->set<BOOL_T>(enabled);

    // Read MTU
    int64_t mtu = readSysfsInt(sysfsBase + "mtu");
    if (mtu > 0) {
        resource(MTU_6)->set<INT_T>(static_cast<INT_T>(mtu));
        WPP_LOGD(TAG, "MTU: %lld", mtu);
    }

    // Read speed (only available when link is up)
    if (linkUp) {
        int64_t speed = readSysfsInt(sysfsBase + "speed");
        if (speed > 0) {
            resource(SPEED_2)->set<INT_T>(static_cast<INT_T>(speed));
            WPP_LOGD(TAG, "Speed: %lld Mbps", speed);
        } else {
            // Speed might be -1 if not available, keep previous value
            WPP_LOGD(TAG, "Speed not available");
        }

        // Read duplex mode
        std::string duplex = readSysfsFile(sysfsBase + "duplex");
        if (duplex == "full") {
            resource(DUPLEX_3)->set<INT_T>(FULL_DUPLEX);
            WPP_LOGD(TAG, "Duplex: Full");
        } else if (duplex == "half") {
            resource(DUPLEX_3)->set<INT_T>(HALF_DUPLEX);
            WPP_LOGD(TAG, "Duplex: Half");
        }
    } else {
        // Link is down, set unknown/default values
        resource(SPEED_2)->set<INT_T>(0);
        resource(DUPLEX_3)->set<INT_T>(FULL_DUPLEX);
    }

    // Read traffic statistics
    std::string statsBase = sysfsBase + "statistics/";

    int64_t txBytes = readSysfsInt(statsBase + "tx_bytes");
    resource(TOTAL_BYTES_SENT_7)->set<INT_T>(static_cast<INT_T>(txBytes));

    int64_t rxBytes = readSysfsInt(statsBase + "rx_bytes");
    resource(TOTAL_BYTES_RECEIVED_8)->set<INT_T>(static_cast<INT_T>(rxBytes));

    int64_t txPackets = readSysfsInt(statsBase + "tx_packets");
    resource(TOTAL_PACKETS_SENT_9)->set<INT_T>(static_cast<INT_T>(txPackets));

    int64_t rxPackets = readSysfsInt(statsBase + "rx_packets");
    resource(TOTAL_PACKETS_RECEIVED_10)->set<INT_T>(static_cast<INT_T>(rxPackets));

    int64_t txErrors = readSysfsInt(statsBase + "tx_errors");
    resource(TRANSMIT_ERRORS_11)->set<INT_T>(static_cast<INT_T>(txErrors));

    int64_t rxErrors = readSysfsInt(statsBase + "rx_errors");
    resource(RECEIVE_ERRORS_12)->set<INT_T>(static_cast<INT_T>(rxErrors));

    WPP_LOGD(TAG, "Statistics - TX: %lld bytes (%lld packets, %lld errors), RX: %lld bytes (%lld packets, %lld errors)",
             txBytes, txPackets, txErrors, rxBytes, rxPackets, rxErrors);
}

bool EthernetInterface::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing resources");

    std::vector<Resource> resources = {
        {INTERFACE_NAME_0,        ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {PORT_TYPE_1,             ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {SPEED_2,                 ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {DUPLEX_3,                ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {LINK_STATUS_4,           ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {MAC_ADDRESS_5,           ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {MTU_6,                   ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TOTAL_BYTES_SENT_7,      ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TOTAL_BYTES_RECEIVED_8,  ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TOTAL_PACKETS_SENT_9,    ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TOTAL_PACKETS_RECEIVED_10, ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TRANSMIT_ERRORS_11,      ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {RECEIVE_ERRORS_12,       ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {INTERFACE_ENABLED_13,    ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
    };

    setupResources(std::move(resources));

    // Determine interface name based on instance ID
    // Instance 0 = eth0 (WAN port on OpenWRT One - 2.5G)
    // Instance 1 = lan1 (LAN port 1)
    // Instance 2 = lan2 (LAN port 2)
    // Instance 3 = lan3 (LAN port 3)
    // Instance 4 = lan4 (LAN port 4)
    std::string iface;
    PortType portType;

    INST_T instId = this->instId();
    if (instId == 0) {
        iface = "eth0";
        portType = PORT_WAN;
    } else if (instId >= 1 && instId <= 4) {
        iface = "lan" + std::to_string(instId);
        portType = PORT_LAN;
    } else {
        // Default fallback
        iface = "eth" + std::to_string(instId);
        portType = PORT_LAN;
    }

    WPP_LOGI(TAG, "Instance %d configured for interface: %s", instId, iface.c_str());

    // Set interface name and port type
    resource(INTERFACE_NAME_0)->set<STRING_T>(iface);
    resource(PORT_TYPE_1)->set<INT_T>(portType);

    // Set initial default values (will be updated from sysfs)
    resource(SPEED_2)->set<INT_T>(0);
    resource(DUPLEX_3)->set<INT_T>(FULL_DUPLEX);
    resource(LINK_STATUS_4)->set<BOOL_T>(false);
    resource(MAC_ADDRESS_5)->set<STRING_T>("00:00:00:00:00:00");
    resource(MTU_6)->set<INT_T>(1500);
    resource(TOTAL_BYTES_SENT_7)->set<INT_T>(0);
    resource(TOTAL_BYTES_RECEIVED_8)->set<INT_T>(0);
    resource(TOTAL_PACKETS_SENT_9)->set<INT_T>(0);
    resource(TOTAL_PACKETS_RECEIVED_10)->set<INT_T>(0);
    resource(TRANSMIT_ERRORS_11)->set<INT_T>(0);
    resource(RECEIVE_ERRORS_12)->set<INT_T>(0);
    resource(INTERFACE_ENABLED_13)->set<BOOL_T>(true);

    // Set validators
    resource(PORT_TYPE_1)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value == PORT_LAN || value == PORT_WAN;
        }
    );

    resource(DUPLEX_3)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value == HALF_DUPLEX || value == FULL_DUPLEX;
        }
    );

    resource(MTU_6)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= 68 && value <= 9000; // Min IP MTU to jumbo frames
        }
    );

    // Set write handlers for writable resources
    resource(MTU_6)->setDataObserver(
        (OBSERVE_INT_T)[this](const INT_T& oldValue, const INT_T& newValue) {
            std::string iface = getInterfaceName();
            std::string sysfsPath = "/sys/class/net/" + iface + "/mtu";

            if (writeSysfsFile(sysfsPath, std::to_string(newValue))) {
                WPP_LOGI(TAG, "MTU changed from %lld to %lld on %s", oldValue, newValue, iface.c_str());
            } else {
                WPP_LOGE(TAG, "Failed to change MTU on %s", iface.c_str());
                // Revert to old value
                resource(MTU_6)->set<INT_T>(oldValue);
            }
        }
    );

    resource(INTERFACE_ENABLED_13)->setDataObserver(
        (OBSERVE_BOOL_T)[this](const BOOL_T& oldValue, const BOOL_T& newValue) {
            std::string iface = getInterfaceName();

            // Use ip command to bring interface up/down
            std::string cmd = "ip link set " + iface + " " + (newValue ? "up" : "down");
            int result = system(cmd.c_str());

            if (result == 0) {
                WPP_LOGI(TAG, "Interface %s %s", iface.c_str(), newValue ? "enabled" : "disabled");
                // Update status after change
                updateInterfaceStatus();
            } else {
                WPP_LOGE(TAG, "Failed to %s interface %s", newValue ? "enable" : "disable", iface.c_str());
                // Revert to old value
                resource(INTERFACE_ENABLED_13)->set<BOOL_T>(oldValue);
            }
        }
    );

    // Read current values from sysfs
    updateInterfaceStatus();

    // Set up periodic status updates (every 30 seconds)
    _updateTaskId = (void*)WppTaskQueue::addTask(30, [this](WppClient& client, void* ctx) {
        updateInterfaceStatus();
        return false; // Keep running
    });

    WPP_LOGI(TAG, "Periodic updates scheduled every 30 seconds for interface %s", iface.c_str());

    return true;
}

bool EthernetInterface::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case MTU_6: {
            const INT_T* mtu = static_cast<const INT_T*>(data);
            if (*mtu < 68 || *mtu > 9000) {
                WPP_LOGE(TAG, "Invalid MTU value: %lld (must be 68-9000)", *mtu);
                return false;
            }
            break;
        }
        case PORT_TYPE_1: {
            const INT_T* portType = static_cast<const INT_T*>(data);
            if (*portType != PORT_LAN && *portType != PORT_WAN) {
                WPP_LOGE(TAG, "Invalid port type: %lld", *portType);
                return false;
            }
            break;
        }
        case DUPLEX_3: {
            const INT_T* duplex = static_cast<const INT_T*>(data);
            if (*duplex != HALF_DUPLEX && *duplex != FULL_DUPLEX) {
                WPP_LOGE(TAG, "Invalid duplex mode: %lld", *duplex);
                return false;
            }
            break;
        }
        default:
            break;
    }
    return true;
}

} // namespace wpp
