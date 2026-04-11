/*
 * ThreadNetwork Implementation (Object 10531)
 * Thread Network Configuration and Status for Matter
 */

#include "w_10531_thread_network/ThreadNetwork.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"
#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10531_THREAD_NETWORK

#include <cstdio>
#include <cstring>
#include <sstream>
#include <random>
#include <iomanip>

// Include Matter SDK integration
#ifdef WITH_MATTER_SDK
#include "matter/matter_sdk_integration.h"
#endif

#define TAG "ThreadNetwork"

using namespace wpp;

/* Static object methods */
Object& ThreadNetwork::object(WppClient& client) {
    return client.registry().getObject(THREAD_NETWORK_OBJECT_ID);
}

Instance* ThreadNetwork::createInst(WppClient& client, INST_T instId) {
    return object(client).createInstance(instId);
}

Instance* ThreadNetwork::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool ThreadNetwork::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

/* Instance lifecycle */
ThreadNetwork::ThreadNetwork(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "ThreadNetwork instance %d created", instId);
}

ThreadNetwork::~ThreadNetwork() {
    WPP_LOGD(TAG, "ThreadNetwork instance %d destroyed", instId());
}

/* Initialize resources */
bool ThreadNetwork::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing ThreadNetwork resources for instance %d", instId());

    // Initialize default values
    set<STRING_T>(NETWORK_NAME_0, DEFAULT_THREAD_NETWORK_NAME);
    set<INT_T>(PAN_ID_1, DEFAULT_THREAD_PAN_ID);
    set<INT_T>(CHANNEL_2, DEFAULT_THREAD_CHANNEL);
    set<STRING_T>(EXTENDED_PAN_ID_3, generateRandomExtendedPanId());
    set<STRING_T>(NETWORK_KEY_4, generateRandomKey());
    set<TIME_T>(ACTIVE_TIMESTAMP_5, 0);
    set<INT_T>(BORDER_ROUTER_STATE_6, STATE_DISABLED);
    set<INT_T>(DEVICES_COUNT_8, 0);

    // Set execute handlers
    setExecute(CREATE_NETWORK_9, createNetwork);
    setExecute(START_BORDER_ROUTER_10, startBorderRouter);
    setExecute(STOP_BORDER_ROUTER_11, stopBorderRouter);

    return true;
}

/* Validation */
bool ThreadNetwork::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case NETWORK_NAME_0: {
            const STRING_T& name = *(const STRING_T*)data;
            if (name.empty() || name.length() > 16) {
                WPP_LOGE(TAG, "Invalid network name length (max 16 characters)");
                return false;
            }
            break;
        }
        case PAN_ID_1: {
            INT_T panId = *(const INT_T*)data;
            if (panId < 0 || panId > 0xFFFF) {
                WPP_LOGE(TAG, "Invalid PAN ID: %lld", panId);
                return false;
            }
            break;
        }
        case CHANNEL_2: {
            INT_T channel = *(const INT_T*)data;
            if (channel < THREAD_CHANNEL_MIN || channel > THREAD_CHANNEL_MAX) {
                WPP_LOGE(TAG, "Invalid Thread channel: %lld (must be %d-%d)",
                        channel, THREAD_CHANNEL_MIN, THREAD_CHANNEL_MAX);
                return false;
            }
            break;
        }
        case EXTENDED_PAN_ID_3: {
            const STRING_T& xpanid = *(const STRING_T*)data;
            if (xpanid.length() != THREAD_EXTENDED_PAN_ID_LENGTH) {
                WPP_LOGE(TAG, "Invalid Extended PAN ID length (must be %d hex chars)",
                        THREAD_EXTENDED_PAN_ID_LENGTH);
                return false;
            }
            // Validate hex string
            for (char c : xpanid) {
                if (!isxdigit(c)) {
                    WPP_LOGE(TAG, "Invalid Extended PAN ID (must be hex)");
                    return false;
                }
            }
            break;
        }
        case NETWORK_KEY_4: {
            const STRING_T& key = *(const STRING_T*)data;
            if (key.length() != THREAD_NETWORK_KEY_LENGTH) {
                WPP_LOGE(TAG, "Invalid Network Key length (must be %d hex chars)",
                        THREAD_NETWORK_KEY_LENGTH);
                return false;
            }
            // Validate hex string
            for (char c : key) {
                if (!isxdigit(c)) {
                    WPP_LOGE(TAG, "Invalid Network Key (must be hex)");
                    return false;
                }
            }
            break;
        }
        default:
            break;
    }
    return true;
}

/* Execute handlers */
bool ThreadNetwork::createNetwork(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ThreadNetwork& network = static_cast<ThreadNetwork&>(inst);
    WPP_LOGD(TAG, "Creating Thread network...");

    if (!network.validateNetworkParameters()) {
        WPP_LOGE(TAG, "Invalid network parameters");
        return false;
    }

    if (!network.applyNetworkConfiguration()) {
        WPP_LOGE(TAG, "Failed to apply network configuration");
        return false;
    }

    network.set<TIME_T>(ACTIVE_TIMESTAMP_5, time(nullptr));
    network.updateOperationalDataset();

    WPP_LOGD(TAG, "Thread network created successfully");
    return true;
}

bool ThreadNetwork::startBorderRouter(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ThreadNetwork& network = static_cast<ThreadNetwork&>(inst);
    WPP_LOGD(TAG, "Starting Thread Border Router...");

    // Check if network is created
    if (network.get<TIME_T>(ACTIVE_TIMESTAMP_5) == 0) {
        WPP_LOGE(TAG, "Network not created - call Create Network first");
        return false;
    }

    network.updateBorderRouterState(STATE_STARTING);

#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();

    STRING_T networkName = network.get<STRING_T>(NETWORK_NAME_0);
    STRING_T extendedPanId = network.get<STRING_T>(EXTENDED_PAN_ID_3);
    STRING_T networkKey = network.get<STRING_T>(NETWORK_KEY_4);
    INT_T panId = network.get<INT_T>(PAN_ID_1);
    INT_T channel = network.get<INT_T>(CHANNEL_2);

    if (!controller.enableThreadBorderRouter(networkName, extendedPanId, networkKey,
                                            static_cast<uint16_t>(panId),
                                            static_cast<uint8_t>(channel))) {
        WPP_LOGE(TAG, "Failed to enable Thread Border Router");
        network.updateBorderRouterState(STATE_ERROR);
        return false;
    }

    network.updateBorderRouterState(STATE_RUNNING);
    WPP_LOGD(TAG, "Thread Border Router started successfully");
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - stub Thread BR start");
    network.updateBorderRouterState(STATE_RUNNING);
#endif

    return true;
}

bool ThreadNetwork::stopBorderRouter(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ThreadNetwork& network = static_cast<ThreadNetwork&>(inst);
    WPP_LOGD(TAG, "Stopping Thread Border Router...");

#ifdef WITH_MATTER_SDK
    auto& controller = matter::MatterController::getInstance();
    controller.disableThreadBorderRouter();
#else
    // Stub implementation
    WPP_LOGD(TAG, "Matter SDK not available - stub Thread BR stop");
#endif

    network.updateBorderRouterState(STATE_STOPPED);
    WPP_LOGD(TAG, "Thread Border Router stopped");

    return true;
}

/* Helper methods */
bool ThreadNetwork::validateNetworkParameters() {
    // Validate all network parameters
    STRING_T networkName = get<STRING_T>(NETWORK_NAME_0);
    STRING_T extendedPanId = get<STRING_T>(EXTENDED_PAN_ID_3);
    STRING_T networkKey = get<STRING_T>(NETWORK_KEY_4);
    INT_T channel = get<INT_T>(CHANNEL_2);

    if (networkName.empty() || networkName.length() > 16) {
        return false;
    }

    if (extendedPanId.length() != THREAD_EXTENDED_PAN_ID_LENGTH) {
        return false;
    }

    if (networkKey.length() != THREAD_NETWORK_KEY_LENGTH) {
        return false;
    }

    if (channel < THREAD_CHANNEL_MIN || channel > THREAD_CHANNEL_MAX) {
        return false;
    }

    return true;
}

bool ThreadNetwork::applyNetworkConfiguration() {
#ifdef WITH_MATTER_SDK
    // Apply Thread network configuration
    // In a real implementation, this would configure the Thread stack
    return true;
#else
    // Stub implementation
    return true;
#endif
}

void ThreadNetwork::updateOperationalDataset() {
    // Create Thread Operational Dataset TLV
    std::ostringstream oss;

    STRING_T networkName = get<STRING_T>(NETWORK_NAME_0);
    STRING_T extendedPanId = get<STRING_T>(EXTENDED_PAN_ID_3);
    STRING_T networkKey = get<STRING_T>(NETWORK_KEY_4);
    INT_T panId = get<INT_T>(PAN_ID_1);
    INT_T channel = get<INT_T>(CHANNEL_2);

    // Simplified dataset format
    oss << "THREAD_DATASET:";
    oss << "name=" << networkName << ",";
    oss << "xpanid=" << extendedPanId << ",";
    oss << "key=" << networkKey << ",";
    oss << "panid=" << std::hex << panId << ",";
    oss << "channel=" << std::dec << channel;

    std::string dataset = oss.str();

    // Store as opaque data
    OPAQUE_T opaqueData(dataset.begin(), dataset.end());
    set<OPAQUE_T>(OPERATIONAL_DATASET_7, opaqueData);
}

void ThreadNetwork::updateBorderRouterState(BorderRouterState state) {
    set<INT_T>(BORDER_ROUTER_STATE_6, static_cast<INT_T>(state));

    const char* stateStr = "Unknown";
    switch (state) {
        case STATE_DISABLED: stateStr = "Disabled"; break;
        case STATE_STOPPED: stateStr = "Stopped"; break;
        case STATE_STARTING: stateStr = "Starting"; break;
        case STATE_RUNNING: stateStr = "Running"; break;
        case STATE_ERROR: stateStr = "Error"; break;
    }

    WPP_LOGD(TAG, "Border Router state: %s", stateStr);
}

std::string ThreadNetwork::generateRandomKey() {
    // Generate random 128-bit (16 bytes = 32 hex characters) network key
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::ostringstream oss;
    for (int i = 0; i < 16; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
    }

    return oss.str();
}

std::string ThreadNetwork::generateRandomExtendedPanId() {
    // Generate random 64-bit (8 bytes = 16 hex characters) Extended PAN ID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::ostringstream oss;
    for (int i = 0; i < 8; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
    }

    return oss.str();
}

#endif // OBJ_W_10531_THREAD_NETWORK
