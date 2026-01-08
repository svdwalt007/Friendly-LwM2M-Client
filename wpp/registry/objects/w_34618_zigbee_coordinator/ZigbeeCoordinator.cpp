/*
 * ZigbeeCoordinator (Object 34618) Implementation
 * Walt Technologies - Zigbee Integration
 */

#include "ZigbeeCoordinator.h"
#include "Lwm2mObjectBase.h"
#include "WppRegistry.h"
#include "WppClient.h"
#include "WppTypes.h"

#include "../../src/zigbee/zigbee_coordinator.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

namespace wpp {

// ==============================================================================
// Static Object Methods
// ==============================================================================

Object& ZigbeeCoordinator::object(WppClient& client) {
    static ObjImpl<ZigbeeCoordinator> obj(client, ZIGBEE_COORDINATOR_OBJECT_ID);
    return obj;
}

Instance* ZigbeeCoordinator::createInst(WppClient& client, INST_T instId) {
    return static_cast<ObjImpl<ZigbeeCoordinator>&>(object(client)).createInst(instId);
}

Instance* ZigbeeCoordinator::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool ZigbeeCoordinator::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

// ==============================================================================
// Constructor / Destructor
// ==============================================================================

ZigbeeCoordinator::ZigbeeCoordinator(Object& object, INST_T instId)
    : Instance(object, instId),
      coordinator_(nullptr),
      refreshRunning_(false) {

    std::cout << "[ZigbeeCoordinator] Instance created: " << instId << std::endl;
}

ZigbeeCoordinator::~ZigbeeCoordinator() {
    stopPeriodicRefresh();
    shutdownCoordinator();
    std::cout << "[ZigbeeCoordinator] Instance destroyed" << std::endl;
}

// ==============================================================================
// Resource Initialization
// ==============================================================================

bool ZigbeeCoordinator::initResources(ItemOp *) {
    std::cout << "[ZigbeeCoordinator] Initializing resources" << std::endl;

    // Network State (R, Integer)
    item(NETWORK_STATE_0).set((INT_T)STATE_OFFLINE);

    // PAN ID (RW, Integer)
    item(PAN_ID_1).set((INT_T)0x1A62);

    // Extended PAN ID (RW, String)
    item(EXTENDED_PAN_ID_2).set("DD2211AA44556677");

    // Channel (RW, Integer)
    item(CHANNEL_3).set((INT_T)15);

    // Network Key (W, Opaque) - not readable for security
    // Key will be set when needed

    // Permit Join Status (R, Boolean)
    item(PERMIT_JOIN_5).set(false);

    // Device Count (R, Integer)
    item(DEVICE_COUNT_6).set((INT_T)0);

    // Coordinator IEEE Address (R, String)
    item(COORDINATOR_IEEE_7).set("0000000000000000");

    // Coordinator Type (R, Integer)
    item(COORDINATOR_TYPE_8).set((INT_T)TYPE_EZSP);

    // Firmware Version (R, String)
    item(FIRMWARE_VERSION_9).set("Unknown");

    // TX Power (RW, Integer)
    item(TX_POWER_10).set((INT_T)20);

    // Security Level (RW, Integer)
    item(SECURITY_LEVEL_11).set((INT_T)5);

    // Serial Port (RW, String)
    item(SERIAL_PORT_12).set("/dev/ttyUSB0");

    // Baud Rate (RW, Integer)
    item(BAUD_RATE_13).set((INT_T)115200);

    // Route Table Size (R, Integer)
    item(ROUTE_TABLE_SIZE_14).set((INT_T)0);

    // Neighbor Table Size (R, Integer)
    item(NEIGHBOR_TABLE_SIZE_15).set((INT_T)0);

    // Execute Resources
    item(FORM_NETWORK_16).setExecute(&ZigbeeCoordinator::formNetwork);
    item(LEAVE_NETWORK_17).setExecute(&ZigbeeCoordinator::leaveNetwork);
    item(PERMIT_JOIN_CMD_18).setExecute(&ZigbeeCoordinator::permitJoin);
    item(START_TOUCHLINK_19).setExecute(&ZigbeeCoordinator::startTouchlink);
    item(BACKUP_NETWORK_20).setExecute(&ZigbeeCoordinator::backupNetwork);

    // Load saved configuration if exists
    loadNetworkConfig();

    // Initialize coordinator
    initializeCoordinator();

    // Start periodic refresh
    startPeriodicRefresh();

    return true;
}

// ==============================================================================
// Validation
// ==============================================================================

bool ZigbeeCoordinator::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case CHANNEL_3: {
            INT_T channel = *static_cast<const INT_T*>(data);
            if (channel < 11 || channel > 26) {
                std::cerr << "[ZigbeeCoordinator] Invalid channel: " << channel << std::endl;
                return false;
            }
            break;
        }

        case TX_POWER_10: {
            INT_T power = *static_cast<const INT_T*>(data);
            if (power < -20 || power > 20) {
                std::cerr << "[ZigbeeCoordinator] Invalid TX power: " << power << std::endl;
                return false;
            }
            break;
        }

        case SECURITY_LEVEL_11: {
            INT_T level = *static_cast<const INT_T*>(data);
            if (level < 0 || level > 7) {
                std::cerr << "[ZigbeeCoordinator] Invalid security level: " << level << std::endl;
                return false;
            }
            break;
        }

        case BAUD_RATE_13: {
            INT_T baud = *static_cast<const INT_T*>(data);
            if (baud != 9600 && baud != 19200 && baud != 38400 &&
                baud != 57600 && baud != 115200) {
                std::cerr << "[ZigbeeCoordinator] Invalid baud rate: " << baud << std::endl;
                return false;
            }
            break;
        }
    }

    return true;
}

// ==============================================================================
// Execute Handlers
// ==============================================================================

bool ZigbeeCoordinator::formNetwork(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeCoordinator& self = static_cast<ZigbeeCoordinator&>(inst);

    std::cout << "[ZigbeeCoordinator] Execute: Form Network" << std::endl;

    if (!self.coordinator_) {
        std::cerr << "[ZigbeeCoordinator] Coordinator not initialized" << std::endl;
        return false;
    }

    // Get network parameters from resources
    zigbee::ZigbeeNetworkParams params;
    params.panId = static_cast<uint16_t>(self.item(PAN_ID_1).toInt());
    params.channel = static_cast<uint8_t>(self.item(CHANNEL_3).toInt());
    params.txPower = static_cast<uint8_t>(self.item(TX_POWER_10).toInt());
    params.securityLevel = static_cast<uint8_t>(self.item(SECURITY_LEVEL_11).toInt());

    // Parse extended PAN ID
    std::string extPanIdStr = self.item(EXTENDED_PAN_ID_2).toString();
    params.extendedPanId = std::stoull(extPanIdStr, nullptr, 16);

    // Form network
    bool success = self.coordinator_->formNetwork(params);

    if (success) {
        self.item(NETWORK_STATE_0).set((INT_T)STATE_READY);
        self.saveNetworkConfig();
    } else {
        self.item(NETWORK_STATE_0).set((INT_T)STATE_ERROR);
    }

    return success;
}

bool ZigbeeCoordinator::leaveNetwork(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeCoordinator& self = static_cast<ZigbeeCoordinator&>(inst);

    std::cout << "[ZigbeeCoordinator] Execute: Leave Network" << std::endl;

    if (!self.coordinator_) {
        std::cerr << "[ZigbeeCoordinator] Coordinator not initialized" << std::endl;
        return false;
    }

    bool success = self.coordinator_->leaveNetwork();

    if (success) {
        self.item(NETWORK_STATE_0).set((INT_T)STATE_OFFLINE);
        self.item(DEVICE_COUNT_6).set((INT_T)0);
    }

    return success;
}

bool ZigbeeCoordinator::permitJoin(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeCoordinator& self = static_cast<ZigbeeCoordinator&>(inst);

    std::cout << "[ZigbeeCoordinator] Execute: Permit Join" << std::endl;

    if (!self.coordinator_) {
        std::cerr << "[ZigbeeCoordinator] Coordinator not initialized" << std::endl;
        return false;
    }

    // Parse duration from execute argument (default: 60 seconds)
    uint16_t duration = 60;
    if (!data.empty() && data.size() >= 2) {
        duration = static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
    }

    bool success = self.coordinator_->permitJoin(duration);

    if (success) {
        self.item(PERMIT_JOIN_5).set(duration > 0);
    }

    return success;
}

bool ZigbeeCoordinator::startTouchlink(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeCoordinator& self = static_cast<ZigbeeCoordinator&>(inst);

    std::cout << "[ZigbeeCoordinator] Execute: Start Touchlink" << std::endl;

    if (!self.coordinator_) {
        std::cerr << "[ZigbeeCoordinator] Coordinator not initialized" << std::endl;
        return false;
    }

    return self.coordinator_->touchlinkCommission();
}

bool ZigbeeCoordinator::backupNetwork(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeCoordinator& self = static_cast<ZigbeeCoordinator&>(inst);

    std::cout << "[ZigbeeCoordinator] Execute: Backup Network" << std::endl;

    return self.saveNetworkConfig();
}

// ==============================================================================
// Helper Methods
// ==============================================================================

bool ZigbeeCoordinator::initializeCoordinator() {
    std::string serialPort = item(SERIAL_PORT_12).toString();
    INT_T baudRate = item(BAUD_RATE_13).toInt();
    INT_T coordType = item(COORDINATOR_TYPE_8).toInt();

    zigbee::CoordinatorType type;
    switch (coordType) {
        case TYPE_EZSP:
            type = zigbee::CoordinatorType::EZSP;
            break;
        case TYPE_Z_STACK:
            type = zigbee::CoordinatorType::Z_STACK;
            break;
        case TYPE_CONBEE:
            type = zigbee::CoordinatorType::CONBEE;
            break;
        case TYPE_ZIGATE:
            type = zigbee::CoordinatorType::ZIGATE;
            break;
        default:
            type = zigbee::CoordinatorType::EZSP;
            break;
    }

    std::cout << "[ZigbeeCoordinator] Initializing coordinator on " << serialPort
              << " at " << baudRate << " baud" << std::endl;

    coordinator_ = std::make_shared<zigbee::ZigbeeCoordinator>(
        serialPort,
        type,
        static_cast<uint32_t>(baudRate)
    );

    if (coordinator_->initialize()) {
        // Update coordinator info
        uint64_t ieeeAddr = coordinator_->getCoordinatorIeeeAddress();
        char ieeeStr[17];
        snprintf(ieeeStr, sizeof(ieeeStr), "%016llX", (unsigned long long)ieeeAddr);
        item(COORDINATOR_IEEE_7).set(ieeeStr);

        item(FIRMWARE_VERSION_9).set(coordinator_->getFirmwareVersion());

        // Register callbacks
        coordinator_->onNetworkStateChanged([this](zigbee::NetworkState state) {
            INT_T lwm2mState;
            switch (state) {
                case zigbee::NetworkState::OFFLINE:
                    lwm2mState = STATE_OFFLINE;
                    break;
                case zigbee::NetworkState::FORMING:
                    lwm2mState = STATE_FORMING;
                    break;
                case zigbee::NetworkState::JOINED:
                    lwm2mState = STATE_JOINED;
                    break;
                case zigbee::NetworkState::READY:
                    lwm2mState = STATE_READY;
                    break;
                case zigbee::NetworkState::LEAVING:
                    lwm2mState = STATE_LEAVING;
                    break;
                case zigbee::NetworkState::ERROR:
                    lwm2mState = STATE_ERROR;
                    break;
                default:
                    lwm2mState = STATE_OFFLINE;
                    break;
            }
            item(NETWORK_STATE_0).set(lwm2mState);
        });

        coordinator_->onDeviceJoined([this](const zigbee::ZigbeeDeviceInfo& device) {
            std::cout << "[ZigbeeCoordinator] Device joined: 0x" << std::hex
                      << device.ieeeAddress << std::endl;
            updateDeviceCount();
        });

        coordinator_->onDeviceLeft([this](uint64_t ieeeAddress) {
            std::cout << "[ZigbeeCoordinator] Device left: 0x" << std::hex
                      << ieeeAddress << std::endl;
            updateDeviceCount();
        });

        return true;
    }

    std::cerr << "[ZigbeeCoordinator] Failed to initialize coordinator" << std::endl;
    return false;
}

void ZigbeeCoordinator::shutdownCoordinator() {
    if (coordinator_) {
        coordinator_->shutdown();
        coordinator_.reset();
    }
}

void ZigbeeCoordinator::updateNetworkState() {
    if (!coordinator_) {
        return;
    }

    zigbee::NetworkState state = coordinator_->getNetworkState();
    INT_T lwm2mState;

    switch (state) {
        case zigbee::NetworkState::OFFLINE:
            lwm2mState = STATE_OFFLINE;
            break;
        case zigbee::NetworkState::FORMING:
            lwm2mState = STATE_FORMING;
            break;
        case zigbee::NetworkState::JOINED:
            lwm2mState = STATE_JOINED;
            break;
        case zigbee::NetworkState::READY:
            lwm2mState = STATE_READY;
            break;
        case zigbee::NetworkState::LEAVING:
            lwm2mState = STATE_LEAVING;
            break;
        case zigbee::NetworkState::ERROR:
            lwm2mState = STATE_ERROR;
            break;
        default:
            lwm2mState = STATE_OFFLINE;
            break;
    }

    item(NETWORK_STATE_0).set(lwm2mState);
}

void ZigbeeCoordinator::updateDeviceCount() {
    if (!coordinator_) {
        return;
    }

    size_t count = coordinator_->getDeviceCount();
    item(DEVICE_COUNT_6).set((INT_T)count);
}

void ZigbeeCoordinator::updateRoutingTables() {
    if (!coordinator_) {
        return;
    }

    auto routeTable = coordinator_->getRouteTable();
    item(ROUTE_TABLE_SIZE_14).set((INT_T)routeTable.size());

    auto neighborTable = coordinator_->getNeighborTable();
    item(NEIGHBOR_TABLE_SIZE_15).set((INT_T)neighborTable.size());
}

bool ZigbeeCoordinator::saveNetworkConfig() {
    std::cout << "[ZigbeeCoordinator] Saving network configuration" << std::endl;

    std::ofstream file("/etc/zigbee/network_config.json");
    if (!file.is_open()) {
        std::cerr << "[ZigbeeCoordinator] Failed to open config file for writing" << std::endl;
        return false;
    }

    file << "{\n";
    file << "  \"panId\": " << item(PAN_ID_1).toInt() << ",\n";
    file << "  \"extendedPanId\": \"" << item(EXTENDED_PAN_ID_2).toString() << "\",\n";
    file << "  \"channel\": " << item(CHANNEL_3).toInt() << ",\n";
    file << "  \"txPower\": " << item(TX_POWER_10).toInt() << ",\n";
    file << "  \"securityLevel\": " << item(SECURITY_LEVEL_11).toInt() << "\n";
    file << "}\n";

    file.close();
    return true;
}

bool ZigbeeCoordinator::loadNetworkConfig() {
    std::cout << "[ZigbeeCoordinator] Loading network configuration" << std::endl;

    std::ifstream file("/etc/zigbee/network_config.json");
    if (!file.is_open()) {
        std::cout << "[ZigbeeCoordinator] No saved configuration found" << std::endl;
        return false;
    }

    // Simple JSON parsing (in production, use a proper JSON library)
    // For now, we'll skip the implementation

    file.close();
    return true;
}

void ZigbeeCoordinator::refreshState() {
    updateNetworkState();
    updateDeviceCount();
    updateRoutingTables();
}

void ZigbeeCoordinator::startPeriodicRefresh() {
    refreshRunning_ = true;
    refreshThread_ = std::thread([this]() {
        while (refreshRunning_) {
            refreshState();
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    });
}

void ZigbeeCoordinator::stopPeriodicRefresh() {
    refreshRunning_ = false;
    if (refreshThread_.joinable()) {
        refreshThread_.join();
    }
}

} // namespace wpp
