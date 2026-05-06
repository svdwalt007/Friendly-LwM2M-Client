/*
 * ZigbeeCoordinator (Object 10532) Implementation
 * Walt Technologies - Zigbee Integration
 */

#include "ZigbeeCoordinator.h"
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

Object& ZigbeeCoordinator::object(WppClient& ctx) {
    return ctx.registry().zigbeeCoordinator();
}

ZigbeeCoordinator* ZigbeeCoordinator::createInst(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().zigbeeCoordinator().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<ZigbeeCoordinator*>(inst);
}

ZigbeeCoordinator* ZigbeeCoordinator::instance(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().zigbeeCoordinator().instance(instId);
    if (!inst) return NULL;
    return static_cast<ZigbeeCoordinator*>(inst);
}

bool ZigbeeCoordinator::removeInst(WppClient& ctx, ID_T instId) {
    return ctx.registry().zigbeeCoordinator().remove(instId);
}

// ==============================================================================
// Constructor / Destructor
// ==============================================================================

ZigbeeCoordinator::ZigbeeCoordinator(lwm2m_context_t& context, const OBJ_LINK_T& id)
    : Instance(context, id),
      coordinator_(nullptr),
      refreshRunning_(false) {

    resourcesCreate();
    resourcesInit();

    std::cout << "[ZigbeeCoordinator] Instance created" << std::endl;
}

ZigbeeCoordinator::~ZigbeeCoordinator() {
    stopPeriodicRefresh();
    shutdownCoordinator();
    std::cout << "[ZigbeeCoordinator] Instance destroyed" << std::endl;
}

// ==============================================================================
// Resource Initialization
// ==============================================================================

void ZigbeeCoordinator::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
    operationNotify(*this, resLink, type);
}

void ZigbeeCoordinator::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
    if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);
}

void ZigbeeCoordinator::resourcesCreate() {
    std::vector<Resource> resources = {
        {NETWORK_STATE_0,       ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {PAN_ID_1,              ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {EXTENDED_PAN_ID_2,     ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {CHANNEL_3,             ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {NETWORK_KEY_4,         ItemOp(ItemOp::WRITE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::OPAQUE},
        {PERMIT_JOIN_5,         ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
        {DEVICE_COUNT_6,        ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {COORDINATOR_IEEE_7,    ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {COORDINATOR_TYPE_8,    ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {FIRMWARE_VERSION_9,    ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {TX_POWER_10,           ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {SECURITY_LEVEL_11,     ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {SERIAL_PORT_12,        ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {BAUD_RATE_13,          ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {ROUTE_TABLE_SIZE_14,   ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {NEIGHBOR_TABLE_SIZE_15, ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {FORM_NETWORK_16,       ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
        {LEAVE_NETWORK_17,      ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
        {PERMIT_JOIN_CMD_18,    ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
        {START_TOUCHLINK_19,    ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
        {BACKUP_NETWORK_20,     ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
    };
    setupResources(std::move(resources));

    // Set up data validation for resources
    resource(CHANNEL_3)->setDataVerifier((VERIFY_INT_T)[](const INT_T& channel) {
        if (channel < 11 || channel > 26) {
            std::cerr << "[ZigbeeCoordinator] Invalid channel: " << channel << std::endl;
            return false;
        }
        return true;
    });

    resource(TX_POWER_10)->setDataVerifier((VERIFY_INT_T)[](const INT_T& power) {
        if (power < -20 || power > 20) {
            std::cerr << "[ZigbeeCoordinator] Invalid TX power: " << power << std::endl;
            return false;
        }
        return true;
    });

    resource(SECURITY_LEVEL_11)->setDataVerifier((VERIFY_INT_T)[](const INT_T& level) {
        if (level < 0 || level > 7) {
            std::cerr << "[ZigbeeCoordinator] Invalid security level: " << level << std::endl;
            return false;
        }
        return true;
    });

    resource(BAUD_RATE_13)->setDataVerifier((VERIFY_INT_T)[](const INT_T& baud) {
        if (baud != 9600 && baud != 19200 && baud != 38400 &&
            baud != 57600 && baud != 115200) {
            std::cerr << "[ZigbeeCoordinator] Invalid baud rate: " << baud << std::endl;
            return false;
        }
        return true;
    });

    // Set up execute handlers
    resource(FORM_NETWORK_16)->set<EXECUTE_T>(&ZigbeeCoordinator::formNetwork);
    resource(LEAVE_NETWORK_17)->set<EXECUTE_T>(&ZigbeeCoordinator::leaveNetwork);
    resource(PERMIT_JOIN_CMD_18)->set<EXECUTE_T>(&ZigbeeCoordinator::permitJoin);
    resource(START_TOUCHLINK_19)->set<EXECUTE_T>(&ZigbeeCoordinator::startTouchlink);
    resource(BACKUP_NETWORK_20)->set<EXECUTE_T>(&ZigbeeCoordinator::backupNetwork);
}

void ZigbeeCoordinator::resourcesInit() {
    std::cout << "[ZigbeeCoordinator] Initializing resources" << std::endl;

    // Network State (R, Integer)
    resource(NETWORK_STATE_0)->set<INT_T>((INT_T)STATE_OFFLINE);

    // PAN ID (RW, Integer)
    resource(PAN_ID_1)->set<INT_T>((INT_T)0x1A62);

    // Extended PAN ID (RW, String)
    resource(EXTENDED_PAN_ID_2)->set<STRING_T>("DD2211AA44556677");

    // Channel (RW, Integer)
    resource(CHANNEL_3)->set<INT_T>((INT_T)15);

    // Network Key (W, Opaque) - not readable for security
    // Key will be set when needed

    // Permit Join Status (R, Boolean)
    resource(PERMIT_JOIN_5)->set<BOOL_T>(false);

    // Device Count (R, Integer)
    resource(DEVICE_COUNT_6)->set<INT_T>((INT_T)0);

    // Coordinator IEEE Address (R, String)
    resource(COORDINATOR_IEEE_7)->set<STRING_T>("0000000000000000");

    // Coordinator Type (R, Integer)
    resource(COORDINATOR_TYPE_8)->set<INT_T>((INT_T)TYPE_EZSP);

    // Firmware Version (R, String)
    resource(FIRMWARE_VERSION_9)->set<STRING_T>("Unknown");

    // TX Power (RW, Integer)
    resource(TX_POWER_10)->set<INT_T>((INT_T)20);

    // Security Level (RW, Integer)
    resource(SECURITY_LEVEL_11)->set<INT_T>((INT_T)5);

    // Serial Port (RW, String)
    resource(SERIAL_PORT_12)->set<STRING_T>("/dev/ttyUSB0");

    // Baud Rate (RW, Integer)
    resource(BAUD_RATE_13)->set<INT_T>((INT_T)115200);

    // Route Table Size (R, Integer)
    resource(ROUTE_TABLE_SIZE_14)->set<INT_T>((INT_T)0);

    // Neighbor Table Size (R, Integer)
    resource(NEIGHBOR_TABLE_SIZE_15)->set<INT_T>((INT_T)0);

    // Load saved configuration if exists
    loadNetworkConfig();

    // Initialize coordinator
    initializeCoordinator();

    // Start periodic refresh
    startPeriodicRefresh();
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
    params.panId = static_cast<uint16_t>(self.resource(PAN_ID_1)->get<INT_T>());
    params.channel = static_cast<uint8_t>(self.resource(CHANNEL_3)->get<INT_T>());
    params.txPower = static_cast<uint8_t>(self.resource(TX_POWER_10)->get<INT_T>());
    params.securityLevel = static_cast<uint8_t>(self.resource(SECURITY_LEVEL_11)->get<INT_T>());

    // Parse extended PAN ID
    std::string extPanIdStr = self.resource(EXTENDED_PAN_ID_2)->get<STRING_T>();
    params.extendedPanId = std::stoull(extPanIdStr, nullptr, 16);

    // Form network
    bool success = self.coordinator_->formNetwork(params);

    if (success) {
        self.resource(NETWORK_STATE_0)->set<INT_T>((INT_T)STATE_READY);
        self.saveNetworkConfig();
    } else {
        self.resource(NETWORK_STATE_0)->set<INT_T>((INT_T)STATE_ERROR);
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
        self.resource(NETWORK_STATE_0)->set<INT_T>((INT_T)STATE_OFFLINE);
        self.resource(DEVICE_COUNT_6)->set<INT_T>((INT_T)0);
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
        self.resource(PERMIT_JOIN_5)->set(duration > 0);
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
    std::string serialPort = resource(SERIAL_PORT_12)->get<STRING_T>();
    INT_T baudRate = resource(BAUD_RATE_13)->get<INT_T>();
    INT_T coordType = resource(COORDINATOR_TYPE_8)->get<INT_T>();

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
        resource(COORDINATOR_IEEE_7)->set(ieeeStr);

        resource(FIRMWARE_VERSION_9)->set(coordinator_->getFirmwareVersion());

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
            resource(NETWORK_STATE_0)->set(lwm2mState);
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

    resource(NETWORK_STATE_0)->set<INT_T>(lwm2mState);
}

void ZigbeeCoordinator::updateDeviceCount() {
    if (!coordinator_) {
        return;
    }

    size_t count = coordinator_->getDeviceCount();
    resource(DEVICE_COUNT_6)->set<INT_T>((INT_T)count);
}

void ZigbeeCoordinator::updateRoutingTables() {
    if (!coordinator_) {
        return;
    }

    auto routeTable = coordinator_->getRouteTable();
    resource(ROUTE_TABLE_SIZE_14)->set<INT_T>((INT_T)routeTable.size());

    auto neighborTable = coordinator_->getNeighborTable();
    resource(NEIGHBOR_TABLE_SIZE_15)->set<INT_T>((INT_T)neighborTable.size());
}

bool ZigbeeCoordinator::saveNetworkConfig() {
    std::cout << "[ZigbeeCoordinator] Saving network configuration" << std::endl;

    std::ofstream file("/etc/zigbee/network_config.json");
    if (!file.is_open()) {
        std::cerr << "[ZigbeeCoordinator] Failed to open config file for writing" << std::endl;
        return false;
    }

    file << "{\n";
    file << "  \"panId\": " << resource(PAN_ID_1)->get<INT_T>() << ",\n";
    file << "  \"extendedPanId\": \"" << resource(EXTENDED_PAN_ID_2)->get<STRING_T>() << "\",\n";
    file << "  \"channel\": " << resource(CHANNEL_3)->get<INT_T>() << ",\n";
    file << "  \"txPower\": " << resource(TX_POWER_10)->get<INT_T>() << ",\n";
    file << "  \"securityLevel\": " << resource(SECURITY_LEVEL_11)->get<INT_T>() << "\n";
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
