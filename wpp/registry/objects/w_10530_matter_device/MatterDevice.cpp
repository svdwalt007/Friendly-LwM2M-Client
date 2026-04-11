/*
 * MatterDevice Implementation (Object 10530)
 * Matter Device Instance for LwM2M
 */

#include "w_10530_matter_device/MatterDevice.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"
#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10530_MATTER_DEVICE

#include <cstdio>
#include <cstring>
#include <sstream>
#include <iomanip>

// Include Matter SDK integration
#ifdef WITH_MATTER_SDK
#include "matter/matter_sdk_integration.h"
#include "matter/matter_cluster_client.h"
#include "matter/matter_device_types.h"
#endif

#define TAG "MatterDevice"

using namespace wpp;

/* Static object methods */
Object& MatterDevice::object(WppClient& client) {
    return client.registry().getObject(MATTER_DEVICE_OBJECT_ID);
}

Instance* MatterDevice::createInst(WppClient& client, INST_T instId) {
    return object(client).createInstance(instId);
}

Instance* MatterDevice::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool MatterDevice::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

/* Instance lifecycle */
MatterDevice::MatterDevice(Object& object, INST_T instId)
    : Instance(object, instId)
    , m_nodeId(0) {
    WPP_LOGD(TAG, "MatterDevice instance %d created", instId);
}

MatterDevice::~MatterDevice() {
    WPP_LOGD(TAG, "MatterDevice instance %d destroyed", instId());
}

/* Initialize resources */
bool MatterDevice::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing MatterDevice resources for instance %d", instId());

    // Initialize default values
    set<STRING_T>(NODE_ID_0, "0000000000000000");
    set<INT_T>(VENDOR_ID_1, 0);
    set<INT_T>(PRODUCT_ID_2, 0);
    set<INT_T>(DEVICE_TYPE_3, 0);
    set<STRING_T>(DEVICE_TYPE_NAME_4, "Unknown");
    set<BOOL_T>(REACHABLE_6, false);
    set<STRING_T>(FIRMWARE_VERSION_7, "");
    set<STRING_T>(SERIAL_NUMBER_8, "");
    set<STRING_T>(PRODUCT_NAME_9, "");
    set<TIME_T>(LAST_SEEN_10, 0);
    set<STRING_T>(IP_ADDRESS_11, "");
    set<INT_T>(RSSI_12, 0);

    // Set execute handlers
    setExecute(INTERACT_13, interact);
    setExecute(READ_ATTRIBUTE_14, readAttribute);
    setExecute(WRITE_ATTRIBUTE_15, writeAttribute);
    setExecute(SEND_COMMAND_16, sendCommand);

    return true;
}

/* Validation */
bool MatterDevice::validate(ID_T resId, const void *data, size_t size) {
    // All resources are read-only or execute-only
    return true;
}

/* Device management */
void MatterDevice::setNodeId(uint64_t nodeId) {
    m_nodeId = nodeId;

    // Convert to hex string
    char buf[32];
    snprintf(buf, sizeof(buf), "%016llx", nodeId);
    set<STRING_T>(NODE_ID_0, buf);

    // Query device information
    queryDeviceInformation();
}

uint64_t MatterDevice::getNodeId() const {
    return m_nodeId;
}

void MatterDevice::updateDeviceInfo() {
    queryDeviceInformation();
}

/* Execute handlers */
bool MatterDevice::interact(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MatterDevice& device = static_cast<MatterDevice&>(inst);
    WPP_LOGD(TAG, "Interacting with device 0x%llx", device.m_nodeId);

    // Parse interaction command
    std::string command(data.begin(), data.end());
    WPP_LOGD(TAG, "Interaction command: %s", command.c_str());

#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();

    // Simple interaction commands
    if (command == "ping" || command == "refresh") {
        if (controller.updateDeviceInfo(device.m_nodeId)) {
            device.updateDeviceInfo();
            return true;
        }
    } else if (command == "on") {
        // Turn device on (for OnOff devices)
        auto client = controller.getClusterClient(device.m_nodeId, 1);
        if (client && client->onOffTurnOn()) {
            return true;
        }
    } else if (command == "off") {
        // Turn device off
        auto client = controller.getClusterClient(device.m_nodeId, 1);
        if (client && client->onOffTurnOff()) {
            return true;
        }
    } else if (command == "toggle") {
        // Toggle device
        auto client = controller.getClusterClient(device.m_nodeId, 1);
        if (client && client->onOffToggle()) {
            return true;
        }
    }
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - stub interaction");
    return true;
#endif

    return false;
}

bool MatterDevice::readAttribute(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MatterDevice& device = static_cast<MatterDevice&>(inst);
    WPP_LOGD(TAG, "Reading attribute from device 0x%llx", device.m_nodeId);

    // Parse attribute read args: "endpoint:cluster:attribute"
    std::string args(data.begin(), data.end());
    uint16_t endpoint;
    uint32_t cluster, attribute;

    if (!device.parseInteractionArgs(args, endpoint, cluster, attribute)) {
        WPP_LOGE(TAG, "Invalid attribute read arguments: %s", args.c_str());
        return false;
    }

#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();
    std::string value;

    if (controller.readAttribute(device.m_nodeId, endpoint, cluster, attribute, value)) {
        WPP_LOGD(TAG, "Read attribute value: %s", value.c_str());
        return true;
    }
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - stub read");
    return true;
#endif

    return false;
}

bool MatterDevice::writeAttribute(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MatterDevice& device = static_cast<MatterDevice&>(inst);
    WPP_LOGD(TAG, "Writing attribute to device 0x%llx", device.m_nodeId);

    // Parse attribute write args: "endpoint:cluster:attribute:value"
    std::string args(data.begin(), data.end());

    // Find last colon to separate value
    size_t lastColon = args.rfind(':');
    if (lastColon == std::string::npos) {
        WPP_LOGE(TAG, "Invalid attribute write arguments: %s", args.c_str());
        return false;
    }

    std::string path = args.substr(0, lastColon);
    std::string value = args.substr(lastColon + 1);

    uint16_t endpoint;
    uint32_t cluster, attribute;

    if (!device.parseInteractionArgs(path, endpoint, cluster, attribute)) {
        WPP_LOGE(TAG, "Invalid attribute write path: %s", path.c_str());
        return false;
    }

#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();

    if (controller.writeAttribute(device.m_nodeId, endpoint, cluster, attribute, value)) {
        WPP_LOGD(TAG, "Attribute written successfully");
        return true;
    }
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - stub write");
    return true;
#endif

    return false;
}

bool MatterDevice::sendCommand(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MatterDevice& device = static_cast<MatterDevice&>(inst);
    WPP_LOGD(TAG, "Sending command to device 0x%llx", device.m_nodeId);

    // Parse command args: "endpoint:cluster:command:args"
    std::string args(data.begin(), data.end());

    // Find last colon to separate command args
    size_t lastColon = args.rfind(':');
    if (lastColon == std::string::npos) {
        WPP_LOGE(TAG, "Invalid command arguments: %s", args.c_str());
        return false;
    }

    std::string path = args.substr(0, lastColon);
    std::string cmdArgs = args.substr(lastColon + 1);

    uint16_t endpoint;
    uint32_t cluster, command;

    if (!device.parseInteractionArgs(path, endpoint, cluster, command)) {
        WPP_LOGE(TAG, "Invalid command path: %s", path.c_str());
        return false;
    }

#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();
    std::string response;

    if (controller.sendCommand(device.m_nodeId, endpoint, cluster, command, cmdArgs, response)) {
        WPP_LOGD(TAG, "Command executed successfully: %s", response.c_str());
        return true;
    }
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - stub command");
    return true;
#endif

    return false;
}

/* Helper methods */
bool MatterDevice::queryDeviceInformation() {
#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();

    auto deviceInfo = controller.getDeviceInfo(m_nodeId);
    if (!deviceInfo) {
        WPP_LOGE(TAG, "Failed to get device info for 0x%llx", m_nodeId);
        return false;
    }

    // Update resources
    set<INT_T>(VENDOR_ID_1, deviceInfo->vendorId);
    set<INT_T>(PRODUCT_ID_2, deviceInfo->productId);
    set<INT_T>(DEVICE_TYPE_3, deviceInfo->deviceType);
    set<STRING_T>(DEVICE_TYPE_NAME_4, matter::getDeviceTypeName(deviceInfo->deviceType));
    set<BOOL_T>(REACHABLE_6, deviceInfo->reachable);
    set<STRING_T>(FIRMWARE_VERSION_7, deviceInfo->firmwareVersion);
    set<STRING_T>(SERIAL_NUMBER_8, deviceInfo->serialNumber);
    set<TIME_T>(LAST_SEEN_10, deviceInfo->lastSeen);
    set<STRING_T>(IP_ADDRESS_11, deviceInfo->ipAddress);
    set<INT_T>(RSSI_12, deviceInfo->rssi);

    // Update endpoints
    if (!deviceInfo->endpoints.empty()) {
        for (size_t i = 0; i < deviceInfo->endpoints.size(); i++) {
            set<INT_T>(ENDPOINTS_5, deviceInfo->endpoints[i], i);
        }
    }

    // Try to get product name from Basic Information cluster
    auto client = controller.getClusterClient(m_nodeId, 1);
    if (client) {
        std::string productName;
        if (client->basicGetProductName(productName)) {
            set<STRING_T>(PRODUCT_NAME_9, productName);
        }
    }

    return true;
#else
    // Stub implementation
    set<INT_T>(VENDOR_ID_1, 0xFFF1);
    set<INT_T>(PRODUCT_ID_2, 0x8000);
    set<INT_T>(DEVICE_TYPE_3, 256); // Light bulb
    set<STRING_T>(DEVICE_TYPE_NAME_4, "On/Off Light");
    set<BOOL_T>(REACHABLE_6, true);
    set<STRING_T>(FIRMWARE_VERSION_7, "1.0.0");
    set<STRING_T>(PRODUCT_NAME_9, "Matter Device");
    return true;
#endif
}

std::string MatterDevice::getDeviceTypeName(uint16_t deviceType) {
#ifdef WITH_MATTER_SDK
    return matter::getDeviceTypeName(deviceType);
#else
    return "Unknown Device";
#endif
}

bool MatterDevice::parseInteractionArgs(const std::string& args, uint16_t& endpoint,
                                       uint32_t& cluster, uint32_t& attributeOrCommand) {
    // Parse format: "endpoint:cluster:attribute/command"
    // Example: "1:6:0" for endpoint 1, OnOff cluster (6), OnOff attribute (0)

    int ep, cl, attr;
    if (sscanf(args.c_str(), "%d:%d:%d", &ep, &cl, &attr) == 3) {
        endpoint = static_cast<uint16_t>(ep);
        cluster = static_cast<uint32_t>(cl);
        attributeOrCommand = static_cast<uint32_t>(attr);
        return true;
    }

    // Try hex format
    if (sscanf(args.c_str(), "%d:%x:%x", &ep, &cl, &attr) == 3) {
        endpoint = static_cast<uint16_t>(ep);
        cluster = static_cast<uint32_t>(cl);
        attributeOrCommand = static_cast<uint32_t>(attr);
        return true;
    }

    return false;
}

#endif // OBJ_W_10530_MATTER_DEVICE
