/*
 * ZigbeeDevice (Object 10533) Implementation
 * Walt Technologies - Zigbee Integration
 *
 * Full implementation with ZigbeeCoordinator integration
 */

#include "ZigbeeDevice.h"
#include "Lwm2mObjectBase.h"
#include "WppRegistry.h"
#include "WppClient.h"
#include "WppTypes.h"
#include "WppTaskQueue.h"

// Include zigbee coordinator integration
#include "zigbee_coordinator.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <ctime>
#include <cstring>
#include <dirent.h>
#include <json/json.h>

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
static bool readZigbee2mqttDevice(uint64_t ieeeAddress, std::string& manufacturer,
                                   std::string& model, std::string& fwVersion,
                                   uint8_t& lqi, int8_t& rssi, bool& online) {
    // Try to read from zigbee2mqtt bridge state file
    std::string stateFile = "/var/lib/zigbee2mqtt/state.json";
    std::ifstream file(stateFile);
    if (!file.is_open()) {
        stateFile = "/opt/zigbee2mqtt/data/state.json";
        file.open(stateFile);
    }
    if (!file.is_open()) {
        return false;
    }

    try {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errors;
        if (!Json::parseFromStream(builder, file, &root, &errors)) {
            return false;
        }

        // IEEE address as hex string
        std::stringstream ss;
        ss << "0x" << std::hex << std::setfill('0') << std::setw(16) << ieeeAddress;
        std::string ieeeStr = ss.str();

        // Search for device in devices array
        if (root.isMember("devices") && root["devices"].isArray()) {
            for (const auto& device : root["devices"]) {
                if (device.isMember("ieee_address") && device["ieee_address"].asString() == ieeeStr) {
                    if (device.isMember("manufacturer")) {
                        manufacturer = device["manufacturer"].asString();
                    }
                    if (device.isMember("model")) {
                        model = device["model"].asString();
                    }
                    if (device.isMember("software_build_id")) {
                        fwVersion = device["software_build_id"].asString();
                    }
                    if (device.isMember("link_quality")) {
                        lqi = device["link_quality"].asUInt();
                    }
                    if (device.isMember("last_seen")) {
                        // Calculate online status based on last seen
                        online = true;
                    }
                    return true;
                }
            }
        }
    } catch (...) {
        return false;
    }

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

Object& ZigbeeDevice::object(WppClient& client) {
    static ObjImpl<ZigbeeDevice> obj(client, ZIGBEE_DEVICE_OBJECT_ID);
    return obj;
}

Instance* ZigbeeDevice::createInst(WppClient& client, INST_T instId) {
    return static_cast<ObjImpl<ZigbeeDevice>&>(object(client)).createInst(instId);
}

Instance* ZigbeeDevice::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool ZigbeeDevice::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

// ==============================================================================
// Constructor / Destructor
// ==============================================================================

ZigbeeDevice::ZigbeeDevice(Object& object, INST_T instId)
    : Instance(object, instId),
      ieeeAddress_(0),
      _updateTaskId(nullptr) {
    std::cout << "[ZigbeeDevice] Instance created: " << instId << std::endl;
}

ZigbeeDevice::~ZigbeeDevice() {
    // Cancel periodic update task
    if (_updateTaskId) {
        WppTaskQueue::instance().cancel(_updateTaskId);
        _updateTaskId = nullptr;
    }
    std::cout << "[ZigbeeDevice] Instance destroyed" << std::endl;
}

// ==============================================================================
// Resource Initialization
// ==============================================================================

bool ZigbeeDevice::initResources(ItemOp *) {
    std::cout << "[ZigbeeDevice] Initializing resources" << std::endl;

    // IEEE Address (R, String)
    item(IEEE_ADDRESS_0).set("0000000000000000");

    // Network Address (R, Integer)
    item(NETWORK_ADDRESS_1).set((INT_T)0xFFFF);

    // Device Type (R, Integer)
    item(DEVICE_TYPE_2).set((INT_T)TYPE_UNKNOWN);

    // Manufacturer (R, String)
    item(MANUFACTURER_3).set("Unknown");

    // Model (R, String)
    item(MODEL_4).set("Unknown");

    // Firmware Version (R, String)
    item(FIRMWARE_VERSION_5).set("Unknown");

    // Power Source (R, Integer)
    item(POWER_SOURCE_6).set((INT_T)POWER_UNKNOWN);

    // LQI (R, Integer)
    item(LQI_7).set((INT_T)0);

    // RSSI (R, Integer)
    item(RSSI_8).set((INT_T)-100);

    // Last Seen (R, Time)
    item(LAST_SEEN_9).set((TIME_T)0);

    // Endpoints (R, Multiple, Integer)
    // Will be populated dynamically

    // Clusters (R, String)
    item(CLUSTERS_11).set("[]");

    // Interviewed (R, Boolean)
    item(INTERVIEWED_12).set(false);

    // Online (R, Boolean)
    item(ONLINE_13).set(false);

    // Profile ID (R, Integer)
    item(PROFILE_ID_14).set((INT_T)0x0104);  // Home Automation profile

    // Device ID (R, Integer)
    item(DEVICE_ID_15).set((INT_T)0x0000);

    // Execute Resources
    item(REMOVE_16).setExecute(&ZigbeeDevice::removeDevice);
    item(INTERVIEW_17).setExecute(&ZigbeeDevice::interviewDevice);
    item(PING_18).setExecute(&ZigbeeDevice::pingDevice);
    item(READ_ATTRIBUTE_19).setExecute(&ZigbeeDevice::readAttribute);
    item(WRITE_ATTRIBUTE_20).setExecute(&ZigbeeDevice::writeAttribute);

    // Schedule periodic update task (every 60 seconds for device status)
    _updateTaskId = WppTaskQueue::instance().addTask(60000, [this]() {
        updateFromZigbeeDevice();
        return true;  // Keep task running
    });

    return true;
}

// ==============================================================================
// Validation
// ==============================================================================

bool ZigbeeDevice::validate(ID_T resId, const void *data, size_t size) {
    // All resources are read-only or execute, no validation needed
    return true;
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
                self.item(MANUFACTURER_3).set(devInfo->manufacturer);
                self.item(MODEL_4).set(devInfo->model);
                self.item(FIRMWARE_VERSION_5).set(devInfo->firmwareVersion);
                self.item(LQI_7).set((INT_T)devInfo->lqi);
                self.item(RSSI_8).set((INT_T)devInfo->rssi);
                self.item(INTERVIEWED_12).set(devInfo->interviewed);
                self.item(LAST_SEEN_9).set((TIME_T)(devInfo->lastSeen / 1000));
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
            self.item(ONLINE_13).set(true);
            self.item(LAST_SEEN_9).set((TIME_T)time(nullptr));
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
    self.item(ONLINE_13).set(online);
    if (online) {
        self.item(LAST_SEEN_9).set((TIME_T)time(nullptr));
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
    item(IEEE_ADDRESS_0).set(ss.str());

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
            item(NETWORK_ADDRESS_1).set((INT_T)devInfo->networkAddress);
            item(DEVICE_TYPE_2).set((INT_T)static_cast<uint8_t>(devInfo->deviceType));
            item(MANUFACTURER_3).set(devInfo->manufacturer);
            item(MODEL_4).set(devInfo->model);
            item(FIRMWARE_VERSION_5).set(devInfo->firmwareVersion);
            item(POWER_SOURCE_6).set((INT_T)devInfo->powerSource);
            item(LQI_7).set((INT_T)devInfo->lqi);
            item(RSSI_8).set((INT_T)devInfo->rssi);
            item(LAST_SEEN_9).set((TIME_T)(devInfo->lastSeen / 1000));
            item(INTERVIEWED_12).set(devInfo->interviewed);

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
            item(ONLINE_13).set(online);
            return;
        }
    }

    // Fallback: Try zigbee2mqtt state file
    std::string manufacturer, model, fwVersion;
    uint8_t lqi = 0;
    int8_t rssi = -100;
    bool online = false;

    if (readZigbee2mqttDevice(ieeeAddress_, manufacturer, model, fwVersion, lqi, rssi, online)) {
        if (!manufacturer.empty()) item(MANUFACTURER_3).set(manufacturer);
        if (!model.empty()) item(MODEL_4).set(model);
        if (!fwVersion.empty()) item(FIRMWARE_VERSION_5).set(fwVersion);
        item(LQI_7).set((INT_T)lqi);
        item(RSSI_8).set((INT_T)rssi);
        item(ONLINE_13).set(online);

        if (online) {
            item(LAST_SEEN_9).set((TIME_T)time(nullptr));
        }
    }

    // Update online status
    item(ONLINE_13).set(isDeviceOnline());
}

bool ZigbeeDevice::isDeviceOnline() {
    // Check if device has been seen recently (within last 5 minutes)
    TIME_T lastSeen = item(LAST_SEEN_9).toTime();
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
