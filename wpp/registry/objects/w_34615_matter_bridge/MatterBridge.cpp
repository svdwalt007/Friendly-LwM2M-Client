/*
 * MatterBridge Implementation (Object 34615)
 * Matter Bridge Controller for LwM2M
 */

#include "w_34615_matter_bridge/MatterBridge.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"
#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34615_MATTER_BRIDGE

#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>

// Include Matter SDK integration
#ifdef WITH_MATTER_SDK
#include "matter/matter_sdk_integration.h"
#endif

#define TAG "MatterBridge"

using namespace wpp;

/* Static object methods */
Object& MatterBridge::object(WppClient& client) {
    return client.registry().getObject(MATTER_BRIDGE_OBJECT_ID);
}

Instance* MatterBridge::createInst(WppClient& client, INST_T instId) {
    return object(client).createInstance(instId);
}

Instance* MatterBridge::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool MatterBridge::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

/* Instance lifecycle */
MatterBridge::MatterBridge(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "MatterBridge instance %d created", instId);
}

MatterBridge::~MatterBridge() {
    WPP_LOGD(TAG, "MatterBridge instance %d destroyed", instId());
    shutdownMatterController();
}

/* Initialize resources */
bool MatterBridge::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing MatterBridge resources for instance %d", instId());

    // Initialize default values
    set<STRING_T>(CONTROLLER_STATUS_0, "Inactive");
    set<INT_T>(COMMISSIONED_DEVICES_COUNT_1, 0);
    set<STRING_T>(THREAD_NETWORK_SSID_2, "");
    set<BOOL_T>(THREAD_ENABLED_3, false);
    set<STRING_T>(FABRIC_ID_4, "");
    set<INT_T>(VENDOR_ID_5, DEFAULT_VENDOR_ID);
    set<INT_T>(PRODUCT_ID_6, DEFAULT_PRODUCT_ID);

    // Set execute handlers
    setExecute(COMMISSION_DEVICE_7, commissionDevice);
    setExecute(REMOVE_DEVICE_8, removeDevice);
    setExecute(DISCOVER_DEVICES_9, discoverDevices);
    setExecute(ENABLE_THREAD_BR_10, enableThreadBR);

    // Initialize Matter controller
    if (!initializeMatterController()) {
        WPP_LOGE(TAG, "Failed to initialize Matter controller");
        set<STRING_T>(CONTROLLER_STATUS_0, "Error: Initialization failed");
        return false;
    }

    return true;
}

/* Validation */
bool MatterBridge::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case THREAD_NETWORK_SSID_2: {
            const STRING_T& ssid = *(const STRING_T*)data;
            if (ssid.length() > 16) {
                WPP_LOGE(TAG, "Thread network name too long (max 16 characters)");
                return false;
            }
            break;
        }
        case VENDOR_ID_5: {
            INT_T vendorId = *(const INT_T*)data;
            if (vendorId < 0 || vendorId > 0xFFFF) {
                WPP_LOGE(TAG, "Invalid vendor ID: %lld", vendorId);
                return false;
            }
            break;
        }
        case PRODUCT_ID_6: {
            INT_T productId = *(const INT_T*)data;
            if (productId < 0 || productId > 0xFFFF) {
                WPP_LOGE(TAG, "Invalid product ID: %lld", productId);
                return false;
            }
            break;
        }
        default:
            break;
    }
    return true;
}

/* Execute handlers */
bool MatterBridge::commissionDevice(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MatterBridge& bridge = static_cast<MatterBridge&>(inst);
    WPP_LOGD(TAG, "Commissioning device...");

    // Parse commissioning payload from execute data
    std::string payload(data.begin(), data.end());
    if (payload.empty()) {
        WPP_LOGE(TAG, "Empty commissioning payload");
        return false;
    }

    WPP_LOGD(TAG, "Commissioning payload: %s", payload.c_str());

#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();

    // Parse setup payload
    matter::CommissioningParams params;
    if (!controller.parseSetupPayload(payload, params)) {
        WPP_LOGE(TAG, "Failed to parse commissioning payload");
        return false;
    }

    // Set commissioning callback
    auto statusCallback = [&bridge](matter::CommissioningStatus status, const std::string& message) {
        WPP_LOGD(TAG, "Commissioning status: %s", message.c_str());
        bridge.set<STRING_T>(CONTROLLER_STATUS_0, message);

        if (status == matter::CommissioningStatus::COMPLETED) {
            bridge.updateDeviceCount();
        }
    };

    // Start commissioning
    if (!controller.commissionDevice(params, statusCallback)) {
        WPP_LOGE(TAG, "Failed to start device commissioning");
        return false;
    }

    bridge.set<STRING_T>(CONTROLLER_STATUS_0, "Commissioning in progress");
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - stub commissioning");
    bridge.set<STRING_T>(CONTROLLER_STATUS_0, "Device commissioned (stub)");
    INT_T count = bridge.get<INT_T>(COMMISSIONED_DEVICES_COUNT_1);
    bridge.set<INT_T>(COMMISSIONED_DEVICES_COUNT_1, count + 1);
#endif

    return true;
}

bool MatterBridge::removeDevice(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MatterBridge& bridge = static_cast<MatterBridge&>(inst);
    WPP_LOGD(TAG, "Removing device...");

    // Parse node ID from execute data (as hex string)
    std::string nodeIdStr(data.begin(), data.end());
    if (nodeIdStr.empty()) {
        WPP_LOGE(TAG, "Empty node ID");
        return false;
    }

#ifdef WITH_MATTER_SDK
    uint64_t nodeId;
    if (sscanf(nodeIdStr.c_str(), "%llx", &nodeId) != 1) {
        WPP_LOGE(TAG, "Invalid node ID format: %s", nodeIdStr.c_str());
        return false;
    }

    auto& controller = matter::MatterController::getInstance();
    if (!controller.removeDevice(nodeId)) {
        WPP_LOGE(TAG, "Failed to remove device: 0x%llx", nodeId);
        return false;
    }

    bridge.updateDeviceCount();
    bridge.set<STRING_T>(CONTROLLER_STATUS_0, "Device removed");
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - stub remove");
    INT_T count = bridge.get<INT_T>(COMMISSIONED_DEVICES_COUNT_1);
    if (count > 0) {
        bridge.set<INT_T>(COMMISSIONED_DEVICES_COUNT_1, count - 1);
    }
    bridge.set<STRING_T>(CONTROLLER_STATUS_0, "Device removed (stub)");
#endif

    return true;
}

bool MatterBridge::discoverDevices(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MatterBridge& bridge = static_cast<MatterBridge&>(inst);
    WPP_LOGD(TAG, "Starting device discovery...");

#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();

    auto discoveryCallback = [](const matter::MatterDeviceInfo& device) {
        WPP_LOGD(TAG, "Discovered device: 0x%llx (Vendor: 0x%04x, Product: 0x%04x)",
                 device.nodeId, device.vendorId, device.productId);
    };

    // Start BLE discovery by default
    if (!controller.startDiscovery(matter::CommissioningMethod::BLE, discoveryCallback, 30)) {
        WPP_LOGE(TAG, "Failed to start device discovery");
        return false;
    }

    bridge.set<STRING_T>(CONTROLLER_STATUS_0, "Discovery started");
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - stub discovery");
    bridge.set<STRING_T>(CONTROLLER_STATUS_0, "Discovery started (stub)");
#endif

    return true;
}

bool MatterBridge::enableThreadBR(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MatterBridge& bridge = static_cast<MatterBridge&>(inst);
    WPP_LOGD(TAG, "Enabling Thread Border Router...");

    if (!bridge.setupThreadBorderRouter()) {
        WPP_LOGE(TAG, "Failed to enable Thread Border Router");
        return false;
    }

    bridge.set<BOOL_T>(THREAD_ENABLED_3, true);
    bridge.set<STRING_T>(CONTROLLER_STATUS_0, "Thread Border Router enabled");

    return true;
}

/* Helper methods */
bool MatterBridge::initializeMatterController() {
    WPP_LOGD(TAG, "Initializing Matter controller...");

#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();

    INT_T vendorId = get<INT_T>(VENDOR_ID_5);
    INT_T productId = get<INT_T>(PRODUCT_ID_6);

    if (!controller.initialize(1, "/var/lib/matter",
                              static_cast<uint16_t>(vendorId),
                              static_cast<uint16_t>(productId))) {
        WPP_LOGE(TAG, "Matter controller initialization failed");
        return false;
    }

    // Set callbacks
    controller.setDeviceStateChangeCallback([this](uint64_t nodeId, bool online) {
        WPP_LOGD(TAG, "Device 0x%llx state changed: %s", nodeId, online ? "online" : "offline");
        updateDeviceCount();
    });

    controller.setErrorCallback([this](int errorCode, const std::string& errorMessage) {
        WPP_LOGE(TAG, "Matter error %d: %s", errorCode, errorMessage.c_str());
        set<STRING_T>(CONTROLLER_STATUS_0, "Error: " + errorMessage);
    });

    set<STRING_T>(FABRIC_ID_4, controller.getFabricId());
    set<STRING_T>(CONTROLLER_STATUS_0, "Active");
    updateDeviceCount();
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - using stub");
    set<STRING_T>(FABRIC_ID_4, "stub-fabric-0000000000000001");
    set<STRING_T>(CONTROLLER_STATUS_0, "Active (stub mode)");
#endif

    return true;
}

void MatterBridge::shutdownMatterController() {
#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();
    controller.shutdown();
#endif
}

void MatterBridge::updateControllerStatus() {
#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();
    set<STRING_T>(CONTROLLER_STATUS_0, controller.getControllerStatus());
#endif
}

void MatterBridge::updateDeviceCount() {
#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();
    set<INT_T>(COMMISSIONED_DEVICES_COUNT_1, controller.getCommissionedDeviceCount());
#endif
}

std::string MatterBridge::parseCommissioningPayload(const std::string& payload) {
    // Parse and validate commissioning payload
    // Can be QR code (MT:...) or manual pairing code (digits)
    if (payload.find("MT:") == 0) {
        return payload; // QR code
    } else if (payload.length() >= 11 && payload.find_first_not_of("0123456789") == std::string::npos) {
        return payload; // Manual pairing code
    }
    return "";
}

bool MatterBridge::setupThreadBorderRouter() {
#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();

    STRING_T networkName = get<STRING_T>(THREAD_NETWORK_SSID_2);
    if (networkName.empty()) {
        networkName = "matter-thread-network";
        set<STRING_T>(THREAD_NETWORK_SSID_2, networkName);
    }

    // Generate Thread network credentials
    // In production, these should be properly generated or configured
    std::string extendedPanId = "1111111122222222";
    std::string networkKey = "00112233445566778899aabbccddeeff";
    uint16_t panId = 0x1234;
    uint8_t channel = 15;

    if (!controller.enableThreadBorderRouter(networkName, extendedPanId, networkKey, panId, channel)) {
        WPP_LOGE(TAG, "Failed to enable Thread Border Router");
        return false;
    }

    WPP_LOGD(TAG, "Thread Border Router enabled successfully");
    return true;
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - stub Thread BR");
    return true;
#endif
}

#endif // OBJ_W_34615_MATTER_BRIDGE
