/*
 * ZigbeeDevice (Object 10533) Implementation
 * Walt Technologies - Zigbee Integration
 *
 * Full implementation with ZigbeeCoordinator integration
 */

#include "ZigbeeDevice.h"
#include "WppRegistry.h"
#include "WppClient.h"
#include "WppTypes.h"
#include "task_queue/WppTaskQueue.h"

// Include zigbee coordinator integration
#include "../../src/zigbee/zigbee_coordinator.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <ctime>
#include <cstring>
#include <dirent.h>
#include <unistd.h>

namespace wpp {

// Singleton coordinator reference (shared across all Zigbee objects)
static std::shared_ptr<zigbee::ZigbeeCoordinator> g_coordinator = nullptr;
static std::mutex g_coordinatorMutex;

// Helper to get/initialize coordinator
static std::shared_ptr<zigbee::ZigbeeCoordinator> getCoordinator() {
    std::lock_guard<std::mutex> lock(g_coordinatorMutex);
    return g_coordinator;
}

static void setCoordinator(std::shared_ptr<zigbee::ZigbeeCoordinator> coord) {
    std::lock_guard<std::mutex> lock(g_coordinatorMutex);
    g_coordinator = coord;
}

// Helper to execute shell commands
static std::string execCommand(const std::string& cmd) {
    std::array<char, 256> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (pipe) {
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }
    // Trim trailing newline
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
    }
    return result;
}

// Helper to read zigbee2mqtt device from JSON bridge state
// TODO: Implement without JSON dependency (use coordinator's device cache)
static bool readZigbee2mqttDevice(uint64_t ieeeAddress, std::string& manufacturer,
                                   std::string& model, std::string& fwVersion,
                                   uint8_t& lqi, int8_t& rssi, bool& online) {
    // Stub implementation - should query coordinator's device cache
    return false;
}

// Helper to find coordinator serial port
static std::string findCoordinatorPort() {
    // Check common locations
    const char* ports[] = {
        "/dev/ttyUSB0",
        "/dev/ttyUSB1",
        "/dev/ttyACM0",
        "/dev/ttyACM1",
        "/dev/serial/by-id/usb-Silicon_Labs_*",
        "/dev/serial/by-id/usb-Texas_Instruments_*"
    };

    for (const char* port : ports) {
        if (access(port, F_OK) == 0) {
            return port;
        }
    }

    // Try to find by scanning /dev/serial/by-id/
    DIR* dir = opendir("/dev/serial/by-id");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            if (name.find("Silicon") != std::string::npos ||
                name.find("Texas") != std::string::npos ||
                name.find("CC2531") != std::string::npos ||
                name.find("CC2652") != std::string::npos ||
                name.find("SONOFF") != std::string::npos) {
                closedir(dir);
                return std::string("/dev/serial/by-id/") + name;
            }
        }
        closedir(dir);
    }

    return "/dev/ttyUSB0";  // Default fallback
}

// ==============================================================================
// Static Object Methods
// ==============================================================================

Object& ZigbeeDevice::object(WppClient& ctx) {
    return ctx.registry().zigbeeDevice();
}

ZigbeeDevice* ZigbeeDevice::createInst(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().zigbeeDevice().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<ZigbeeDevice*>(inst);
}

ZigbeeDevice* ZigbeeDevice::instance(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().zigbeeDevice().instance(instId);
    if (!inst) return NULL;
    return static_cast<ZigbeeDevice*>(inst);
}

bool ZigbeeDevice::removeInst(WppClient& ctx, ID_T instId) {
    return ctx.registry().zigbeeDevice().remove(instId);
}

// ==============================================================================
// Constructor / Destructor
// ==============================================================================

ZigbeeDevice::ZigbeeDevice(lwm2m_context_t& context, const OBJ_LINK_T& id)
    : Instance(context, id),
      ieeeAddress_(0),
      _updateTaskId(0) {

    resourcesCreate();
    resourcesInit();

    std::cout << "[ZigbeeDevice] Instance created" << std::endl;
}

ZigbeeDevice::~ZigbeeDevice() {
    // Cancel periodic update task
    if (_updateTaskId) {
        WppTaskQueue::requestToRemoveTask(_updateTaskId);
        _updateTaskId = 0;
    }
    std::cout << "[ZigbeeDevice] Instance destroyed" << std::endl;
}

// ==============================================================================
// Resource Initialization
// ==============================================================================

void ZigbeeDevice::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
    operationNotify(*this, resLink, type);
}

void ZigbeeDevice::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
    if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);
}

void ZigbeeDevice::resourcesCreate() {
    std::vector<Resource> resources = {
        {IEEE_ADDRESS_0,      ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {NETWORK_ADDRESS_1,   ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {DEVICE_TYPE_2,       ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {MANUFACTURER_3,      ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {MODEL_4,             ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {FIRMWARE_VERSION_5,  ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {POWER_SOURCE_6,      ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {LQI_7,               ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {RSSI_8,              ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {LAST_SEEN_9,         ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::TIME},
        {ENDPOINTS_10,        ItemOp(ItemOp::READ), IS_SINGLE::MULTIPLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {CLUSTERS_11,         ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {INTERVIEWED_12,      ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
        {ONLINE_13,           ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {PROFILE_ID_14,       ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {DEVICE_ID_15,        ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {REMOVE_16,           ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
        {INTERVIEW_17,        ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
        {PING_18,             ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
        {READ_ATTRIBUTE_19,   ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
        {WRITE_ATTRIBUTE_20,  ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
    };
    setupResources(std::move(resources));

    // Set up execute handlers
    resource(REMOVE_16)->set<EXECUTE_T>(&ZigbeeDevice::removeDevice);
    resource(INTERVIEW_17)->set<EXECUTE_T>(&ZigbeeDevice::interviewDevice);
    resource(PING_18)->set<EXECUTE_T>(&ZigbeeDevice::pingDevice);
    resource(READ_ATTRIBUTE_19)->set<EXECUTE_T>(&ZigbeeDevice::readAttribute);
    resource(WRITE_ATTRIBUTE_20)->set<EXECUTE_T>(&ZigbeeDevice::writeAttribute);
}

void ZigbeeDevice::resourcesInit() {
    std::cout << "[ZigbeeDevice] Initializing resources" << std::endl;

    // IEEE Address (R, String)
    resource(IEEE_ADDRESS_0)->set<STRING_T>("0000000000000000");

    // Network Address (R, Integer)
    resource(NETWORK_ADDRESS_1)->set<INT_T>((INT_T)0xFFFF);

    // Device Type (R, Integer)
    resource(DEVICE_TYPE_2)->set<INT_T>((INT_T)TYPE_UNKNOWN);

    // Manufacturer (R, String)
    resource(MANUFACTURER_3)->set<STRING_T>("Unknown");

    // Model (R, String)
    resource(MODEL_4)->set<STRING_T>("Unknown");

    // Firmware Version (R, String)
    resource(FIRMWARE_VERSION_5)->set<STRING_T>("Unknown");

    // Power Source (R, Integer)
    resource(POWER_SOURCE_6)->set<INT_T>((INT_T)POWER_UNKNOWN);

    // LQI (R, Integer)
    resource(LQI_7)->set<INT_T>((INT_T)0);

    // RSSI (R, Integer)
    resource(RSSI_8)->set<INT_T>((INT_T)-100);

    // Last Seen (R, Time)
    resource(LAST_SEEN_9)->set<TIME_T>((TIME_T)0);

    // Endpoints (R, Multiple, Integer)
    // Will be populated dynamically

    // Clusters (R, String)
    resource(CLUSTERS_11)->set<STRING_T>("[]");

    // Interviewed (R, Boolean)
    resource(INTERVIEWED_12)->set<BOOL_T>(false);

    // Online (R, Boolean)
    resource(ONLINE_13)->set<BOOL_T>(false);

    // Profile ID (R, Integer)
    resource(PROFILE_ID_14)->set<INT_T>((INT_T)0x0104);  // Home Automation profile

    // Device ID (R, Integer)
    resource(DEVICE_ID_15)->set<INT_T>((INT_T)0x0000);

    // Schedule periodic update task (every 60 seconds for device status)
    _updateTaskId = WppTaskQueue::addTask(60, [this](WppClient& client, void* ctx) {
        updateFromZigbeeDevice();
        return true;  // Keep task running
    });
}

// ==============================================================================
// Execute Handlers
// ==============================================================================

bool ZigbeeDevice::removeDevice(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeDevice& self = static_cast<ZigbeeDevice&>(inst);

    std::cout << "[ZigbeeDevice] Execute: Remove Device IEEE=0x"
              << std::hex << self.ieeeAddress_ << std::endl;

    auto coordinator = getCoordinator();
    if (coordinator && coordinator->isReady()) {
        // Use coordinator to remove device
        if (coordinator->removeDevice(self.ieeeAddress_)) {
            std::cout << "[ZigbeeDevice] Device removal initiated" << std::endl;
            return true;
        }
    }

    // Fallback: Try zigbee2mqtt MQTT command
    std::string ieeeStr;
    {
        std::stringstream ss;
        ss << "0x" << std::hex << std::setfill('0') << std::setw(16) << self.ieeeAddress_;
        ieeeStr = ss.str();
    }

    std::string cmd = "mosquitto_pub -t 'zigbee2mqtt/bridge/request/device/remove' "
                      "-m '{\"id\": \"" + ieeeStr + "\"}' 2>/dev/null";
    int result = system(cmd.c_str());

    if (result == 0) {
        std::cout << "[ZigbeeDevice] Remove command sent via MQTT" << std::endl;
        return true;
    }

    std::cerr << "[ZigbeeDevice] Failed to remove device" << std::endl;
    return false;
}

bool ZigbeeDevice::interviewDevice(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeDevice& self = static_cast<ZigbeeDevice&>(inst);

    std::cout << "[ZigbeeDevice] Execute: Interview Device IEEE=0x"
              << std::hex << self.ieeeAddress_ << std::endl;

    auto coordinator = getCoordinator();
    if (coordinator && coordinator->isReady()) {
        // Use coordinator to interview device
        if (coordinator->interviewDevice(self.ieeeAddress_)) {
            // Update resources from coordinator data
            const zigbee::ZigbeeDeviceInfo* devInfo = coordinator->getDevice(self.ieeeAddress_);
            if (devInfo) {
                self.resource(MANUFACTURER_3)->set(devInfo->manufacturer);
                self.resource(MODEL_4)->set(devInfo->model);
                self.resource(FIRMWARE_VERSION_5)->set(devInfo->firmwareVersion);
                self.resource(LQI_7)->set<INT_T>((INT_T)devInfo->lqi);
                self.resource(RSSI_8)->set<INT_T>((INT_T)devInfo->rssi);
                self.resource(INTERVIEWED_12)->set(devInfo->interviewed);
                self.resource(LAST_SEEN_9)->set<TIME_T>((TIME_T)(devInfo->lastSeen / 1000));
            }
            return true;
        }
    }

    // Fallback: Try zigbee2mqtt MQTT command for interview
    std::string ieeeStr;
    {
        std::stringstream ss;
        ss << "0x" << std::hex << std::setfill('0') << std::setw(16) << self.ieeeAddress_;
        ieeeStr = ss.str();
    }

    std::string cmd = "mosquitto_pub -t 'zigbee2mqtt/bridge/request/device/interview' "
                      "-m '{\"id\": \"" + ieeeStr + "\"}' 2>/dev/null";
    int result = system(cmd.c_str());

    if (result == 0) {
        std::cout << "[ZigbeeDevice] Interview command sent via MQTT" << std::endl;
        self.updateFromZigbeeDevice();
        return true;
    }

    return false;
}

bool ZigbeeDevice::pingDevice(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeDevice& self = static_cast<ZigbeeDevice&>(inst);

    std::cout << "[ZigbeeDevice] Execute: Ping Device IEEE=0x"
              << std::hex << self.ieeeAddress_ << std::endl;

    auto coordinator = getCoordinator();
    if (coordinator && coordinator->isReady()) {
        // Send identify cluster command (brief flash)
        std::vector<uint8_t> payload = {0x03, 0x00};  // Identify for 3 seconds
        if (coordinator->sendZclCommand(self.ieeeAddress_, 1, 0x0003, 0x00, payload)) {
            self.resource(ONLINE_13)->set<BOOL_T>(true);
            self.resource(LAST_SEEN_9)->set<TIME_T>((TIME_T)time(nullptr));
            std::cout << "[ZigbeeDevice] Ping successful" << std::endl;
            return true;
        }
    }

    // Fallback: Send via zigbee2mqtt
    std::string ieeeStr;
    {
        std::stringstream ss;
        ss << "0x" << std::hex << std::setfill('0') << std::setw(16) << self.ieeeAddress_;
        ieeeStr = ss.str();
    }

    // Publish identify command via MQTT
    std::string cmd = "mosquitto_pub -t 'zigbee2mqtt/" + ieeeStr + "/set' "
                      "-m '{\"identify\": 5}' 2>/dev/null";
    int result = system(cmd.c_str());

    bool online = (result == 0);
    self.resource(ONLINE_13)->set(online);
    if (online) {
        self.resource(LAST_SEEN_9)->set<TIME_T>((TIME_T)time(nullptr));
    }

    return online;
}

bool ZigbeeDevice::readAttribute(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeDevice& self = static_cast<ZigbeeDevice&>(inst);

    std::cout << "[ZigbeeDevice] Execute: Read Attribute" << std::endl;

    // Parse attribute read request from data
    // Format: [endpoint:1][clusterId:2][attributeId:2]
    if (data.size() < 5) {
        std::cerr << "[ZigbeeDevice] Invalid data size for read attribute (need 5 bytes)" << std::endl;
        return false;
    }

    uint8_t endpoint = data[0];
    uint16_t clusterId = data[1] | (data[2] << 8);
    uint16_t attributeId = data[3] | (data[4] << 8);

    std::cout << "[ZigbeeDevice] Reading EP=" << (int)endpoint
              << " Cluster=0x" << std::hex << clusterId
              << " Attr=0x" << attributeId << std::endl;

    auto coordinator = getCoordinator();
    if (coordinator && coordinator->isReady()) {
        // Build ZCL Read Attributes command payload
        std::vector<uint8_t> payload;
        payload.push_back(attributeId & 0xFF);
        payload.push_back((attributeId >> 8) & 0xFF);

        // ZCL command 0x00 = Read Attributes
        return coordinator->sendZclCommand(self.ieeeAddress_, endpoint, clusterId, 0x00, payload);
    }

    // Fallback: Use zigbee2mqtt converter if available
    return false;
}

bool ZigbeeDevice::writeAttribute(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeDevice& self = static_cast<ZigbeeDevice&>(inst);

    std::cout << "[ZigbeeDevice] Execute: Write Attribute" << std::endl;

    // Parse attribute write request from data
    // Format: [endpoint:1][clusterId:2][attributeId:2][dataType:1][value:n]
    if (data.size() < 6) {
        std::cerr << "[ZigbeeDevice] Invalid data size for write attribute" << std::endl;
        return false;
    }

    uint8_t endpoint = data[0];
    uint16_t clusterId = data[1] | (data[2] << 8);
    uint16_t attributeId = data[3] | (data[4] << 8);
    uint8_t dataType = data[5];

    std::cout << "[ZigbeeDevice] Writing EP=" << (int)endpoint
              << " Cluster=0x" << std::hex << clusterId
              << " Attr=0x" << attributeId
              << " Type=0x" << (int)dataType << std::endl;

    auto coordinator = getCoordinator();
    if (coordinator && coordinator->isReady()) {
        // Build ZCL Write Attributes command payload
        std::vector<uint8_t> payload;
        payload.push_back(attributeId & 0xFF);
        payload.push_back((attributeId >> 8) & 0xFF);
        payload.push_back(dataType);
        // Append value bytes
        for (size_t i = 6; i < data.size(); i++) {
            payload.push_back(data[i]);
        }

        // ZCL command 0x02 = Write Attributes
        return coordinator->sendZclCommand(self.ieeeAddress_, endpoint, clusterId, 0x02, payload);
    }

    return false;
}

// ==============================================================================
// Helper Methods
// ==============================================================================

void ZigbeeDevice::setIeeeAddress(uint64_t ieeeAddress) {
    ieeeAddress_ = ieeeAddress;

    // Convert to hex string
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << ieeeAddress;
    resource(IEEE_ADDRESS_0)->set<STRING_T>(ss.str());

    // Update device information
    updateFromZigbeeDevice();
}

uint64_t ZigbeeDevice::getIeeeAddress() const {
    return ieeeAddress_;
}

void ZigbeeDevice::updateFromZigbeeDevice() {
    if (ieeeAddress_ == 0) {
        return;
    }

    std::cout << "[ZigbeeDevice] Updating device info for 0x"
              << std::hex << ieeeAddress_ << std::endl;

    // Try coordinator first
    auto coordinator = getCoordinator();
    if (coordinator && coordinator->isReady()) {
        const zigbee::ZigbeeDeviceInfo* devInfo = coordinator->getDevice(ieeeAddress_);
        if (devInfo) {
            resource(NETWORK_ADDRESS_1)->set<INT_T>((INT_T)devInfo->networkAddress);
            resource(DEVICE_TYPE_2)->set<INT_T>((INT_T)static_cast<uint8_t>(devInfo->deviceType));
            resource(MANUFACTURER_3)->set<STRING_T>(devInfo->manufacturer);
            resource(MODEL_4)->set<STRING_T>(devInfo->model);
            resource(FIRMWARE_VERSION_5)->set<STRING_T>(devInfo->firmwareVersion);
            resource(POWER_SOURCE_6)->set<INT_T>((INT_T)devInfo->powerSource);
            resource(LQI_7)->set<INT_T>((INT_T)devInfo->lqi);
            resource(RSSI_8)->set<INT_T>((INT_T)devInfo->rssi);
            resource(LAST_SEEN_9)->set<TIME_T>((TIME_T)(devInfo->lastSeen / 1000));
            resource(INTERVIEWED_12)->set<BOOL_T>(devInfo->interviewed);

            // Build endpoints JSON
            std::stringstream epJson;
            epJson << "[";
            for (size_t i = 0; i < devInfo->endpoints.size(); i++) {
                if (i > 0) epJson << ",";
                epJson << (int)devInfo->endpoints[i];
            }
            epJson << "]";
            // Note: For multiple instance resources, we'd set each endpoint

            // Update online status
            bool online = isDeviceOnline();
            resource(ONLINE_13)->set<BOOL_T>(online);
            return;
        }
    }

    // Fallback: Try zigbee2mqtt state file
    std::string manufacturer, model, fwVersion;
    uint8_t lqi = 0;
    int8_t rssi = -100;
    bool online = false;

    if (readZigbee2mqttDevice(ieeeAddress_, manufacturer, model, fwVersion, lqi, rssi, online)) {
        if (!manufacturer.empty()) resource(MANUFACTURER_3)->set<STRING_T>(manufacturer);
        if (!model.empty()) resource(MODEL_4)->set<STRING_T>(model);
        if (!fwVersion.empty()) resource(FIRMWARE_VERSION_5)->set<STRING_T>(fwVersion);
        resource(LQI_7)->set<INT_T>((INT_T)lqi);
        resource(RSSI_8)->set<INT_T>((INT_T)rssi);
        resource(ONLINE_13)->set<BOOL_T>(online);

        if (online) {
            resource(LAST_SEEN_9)->set<TIME_T>((TIME_T)time(nullptr));
        }
    }

    // Update online status
    resource(ONLINE_13)->set<BOOL_T>(isDeviceOnline());
}

bool ZigbeeDevice::isDeviceOnline() {
    // Check if device has been seen recently (within last 5 minutes)
    TIME_T lastSeen = resource(LAST_SEEN_9)->get<TIME_T>();
    TIME_T now = time(nullptr);

    return (lastSeen > 0 && (now - lastSeen) < 300);  // 5 minutes
}

// ==============================================================================
// Coordinator Management (static)
// ==============================================================================

void ZigbeeDevice::initializeCoordinator(const std::string& serialPort) {
    auto coord = std::make_shared<zigbee::ZigbeeCoordinator>(
        serialPort.empty() ? findCoordinatorPort() : serialPort,
        zigbee::CoordinatorType::EZSP
    );

    if (coord->initialize()) {
        setCoordinator(coord);
        std::cout << "[ZigbeeDevice] Coordinator initialized" << std::endl;
    } else {
        std::cerr << "[ZigbeeDevice] Failed to initialize coordinator" << std::endl;
    }
}

void ZigbeeDevice::shutdownCoordinator() {
    auto coord = getCoordinator();
    if (coord) {
        coord->shutdown();
        setCoordinator(nullptr);
    }
}

} // namespace wpp
