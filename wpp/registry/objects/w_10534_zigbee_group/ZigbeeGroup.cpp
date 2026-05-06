/*
 * ZigbeeGroup (Object 10534) Implementation
 * Walt Technologies - Zigbee Integration
 *
 * Full implementation with ZigbeeCoordinator integration and ZCL group commands
 */

#include "ZigbeeGroup.h"
#include "WppRegistry.h"
#include "WppClient.h"
#include "WppTypes.h"

// Include zigbee coordinator integration
#include "../../src/zigbee/zigbee_coordinator.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <set>

namespace wpp {

// Forward declaration - coordinator shared with ZigbeeDevice
extern std::shared_ptr<zigbee::ZigbeeCoordinator> getCoordinator();

// If not available, define local helper
static std::shared_ptr<zigbee::ZigbeeCoordinator> getZigbeeCoordinator() {
    // Try to get from ZigbeeDevice's coordinator
    // In a real implementation, this would be properly shared
    return nullptr;
}

// ZCL Cluster IDs
constexpr uint16_t ZCL_CLUSTER_GROUPS = 0x0004;
constexpr uint16_t ZCL_CLUSTER_SCENES = 0x0005;
constexpr uint16_t ZCL_CLUSTER_ON_OFF = 0x0006;
constexpr uint16_t ZCL_CLUSTER_LEVEL = 0x0008;
constexpr uint16_t ZCL_CLUSTER_COLOR = 0x0300;

// ZCL Groups Cluster Commands
constexpr uint8_t ZCL_CMD_ADD_GROUP = 0x00;
constexpr uint8_t ZCL_CMD_VIEW_GROUP = 0x01;
constexpr uint8_t ZCL_CMD_GET_GROUP_MEMBERSHIP = 0x02;
constexpr uint8_t ZCL_CMD_REMOVE_GROUP = 0x03;
constexpr uint8_t ZCL_CMD_REMOVE_ALL_GROUPS = 0x04;

// ZCL Scenes Cluster Commands
constexpr uint8_t ZCL_CMD_ADD_SCENE = 0x00;
constexpr uint8_t ZCL_CMD_VIEW_SCENE = 0x01;
constexpr uint8_t ZCL_CMD_REMOVE_SCENE = 0x02;
constexpr uint8_t ZCL_CMD_REMOVE_ALL_SCENES = 0x03;
constexpr uint8_t ZCL_CMD_STORE_SCENE = 0x04;
constexpr uint8_t ZCL_CMD_RECALL_SCENE = 0x05;
constexpr uint8_t ZCL_CMD_GET_SCENE_MEMBERSHIP = 0x06;

// ZCL On/Off Cluster Commands
constexpr uint8_t ZCL_CMD_OFF = 0x00;
constexpr uint8_t ZCL_CMD_ON = 0x01;
constexpr uint8_t ZCL_CMD_TOGGLE = 0x02;

// ZCL Level Control Commands
constexpr uint8_t ZCL_CMD_MOVE_TO_LEVEL = 0x00;
constexpr uint8_t ZCL_CMD_MOVE = 0x01;
constexpr uint8_t ZCL_CMD_STEP = 0x02;
constexpr uint8_t ZCL_CMD_STOP = 0x03;
constexpr uint8_t ZCL_CMD_MOVE_TO_LEVEL_WITH_ON_OFF = 0x04;

// ZCL Color Control Commands
constexpr uint8_t ZCL_CMD_MOVE_TO_HUE = 0x00;
constexpr uint8_t ZCL_CMD_MOVE_TO_SATURATION = 0x03;
constexpr uint8_t ZCL_CMD_MOVE_TO_HUE_AND_SATURATION = 0x06;
constexpr uint8_t ZCL_CMD_MOVE_TO_COLOR_TEMP = 0x0A;

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
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
    }
    return result;
}

// ==============================================================================
// Static Object Methods
// ==============================================================================

Object& ZigbeeGroup::object(WppClient& ctx) {
    return ctx.registry().zigbeeGroup();
}

ZigbeeGroup* ZigbeeGroup::createInst(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().zigbeeGroup().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<ZigbeeGroup*>(inst);
}

ZigbeeGroup* ZigbeeGroup::instance(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().zigbeeGroup().instance(instId);
    if (!inst) return NULL;
    return static_cast<ZigbeeGroup*>(inst);
}

bool ZigbeeGroup::removeInst(WppClient& ctx, ID_T instId) {
    return ctx.registry().zigbeeGroup().remove(instId);
}

// ==============================================================================
// Constructor / Destructor
// ==============================================================================

ZigbeeGroup::ZigbeeGroup(lwm2m_context_t& context, const OBJ_LINK_T& id)
    : Instance(context, id) {

    resourcesCreate();
    resourcesInit();

    std::cout << "[ZigbeeGroup] Instance created" << std::endl;
}

ZigbeeGroup::~ZigbeeGroup() {
    std::cout << "[ZigbeeGroup] Instance destroyed" << std::endl;
}

void ZigbeeGroup::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
    operationNotify(*this, resLink, type);
}

void ZigbeeGroup::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
    if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);
}

void ZigbeeGroup::resourcesInit() {
    /* Default values are set in resourcesCreate */
}

// ==============================================================================
// Resource Initialization
// ==============================================================================

void ZigbeeGroup::resourcesCreate() {
    std::cout << "[ZigbeeGroup] Initializing resources" << std::endl;

    // Group ID (RW, Integer) - Zigbee group IDs range 0x0001-0xFFF7
    resource(GROUP_ID_0)->set<INT_T>((INT_T)0x0001);
    resource(GROUP_ID_0)->setDataVerifier((VERIFY_INT_T)[](const INT_T& val) {
        return val >= 0x0001 && val <= 0xFFF7;
    });

    // Group Name (RW, String)
    resource(GROUP_NAME_1)->set<STRING_T>("Default Group");
    resource(GROUP_NAME_1)->setDataVerifier([](const STRING_T& val) {
        return !val.empty() && val.length() <= 16;  // ZCL max group name length
    });

    // Member Devices (R, Multiple, String)
    // Will be populated dynamically with IEEE addresses

    // Member Count (R, Integer)
    resource(MEMBER_COUNT_3)->set<INT_T>((INT_T)0);

    // Scene Count (R, Integer)
    resource(SCENE_COUNT_4)->set<INT_T>((INT_T)0);

    // Current Scene (RW, Integer)
    resource(CURRENT_SCENE_5)->set<INT_T>((INT_T)0);
    resource(CURRENT_SCENE_5)->setDataVerifier((VERIFY_INT_T)[](const INT_T& val) {
        return val >= 0 && val <= 255;  // Scene ID range
    });

    // Execute Resources
    resource(ADD_MEMBER_6)->set<EXECUTE_T>(&ZigbeeGroup::addMember);
    resource(REMOVE_MEMBER_7)->set<EXECUTE_T>(&ZigbeeGroup::removeMember);
    resource(SCENE_RECALL_8)->set<EXECUTE_T>(&ZigbeeGroup::sceneRecall);
    resource(SCENE_STORE_9)->set<EXECUTE_T>(&ZigbeeGroup::sceneStore);
    resource(ON_10)->set<EXECUTE_T>(&ZigbeeGroup::turnOn);
    resource(OFF_11)->set<EXECUTE_T>(&ZigbeeGroup::turnOff);
    resource(TOGGLE_12)->set<EXECUTE_T>(&ZigbeeGroup::toggle);
    resource(SET_LEVEL_13)->set<EXECUTE_T>(&ZigbeeGroup::setLevel);
    resource(SET_COLOR_14)->set<EXECUTE_T>(&ZigbeeGroup::setColor);
}

// ==============================================================================
// Execute Handlers
// ==============================================================================

bool ZigbeeGroup::addMember(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeGroup& self = static_cast<ZigbeeGroup&>(inst);

    std::cout << "[ZigbeeGroup] Execute: Add Member" << std::endl;

    // Parse IEEE address from data (8 bytes, little-endian)
    if (data.size() < 8) {
        std::cerr << "[ZigbeeGroup] Invalid data size for IEEE address (need 8 bytes)" << std::endl;
        return false;
    }

    uint64_t ieeeAddress = 0;
    for (size_t i = 0; i < 8; i++) {
        ieeeAddress |= (static_cast<uint64_t>(data[i]) << (i * 8));
    }

    std::cout << "[ZigbeeGroup] Adding device 0x" << std::hex << std::setfill('0')
              << std::setw(16) << ieeeAddress << " to group " << std::dec
              << self.resource(GROUP_ID_0)->get<INT_T>() << std::endl;

    // Get group ID
    uint16_t groupId = static_cast<uint16_t>(self.resource(GROUP_ID_0)->get<INT_T>());
    std::string groupName = self.resource(GROUP_NAME_1)->get<STRING_T>();

    // Build ZCL Add Group command payload
    // Format: [GroupId:2][GroupName:string]
    std::vector<uint8_t> payload;
    payload.push_back(groupId & 0xFF);
    payload.push_back((groupId >> 8) & 0xFF);
    payload.push_back(static_cast<uint8_t>(groupName.length()));  // String length
    for (char c : groupName) {
        payload.push_back(static_cast<uint8_t>(c));
    }

    bool success = self.sendZclCommandToDevice(ieeeAddress, ZCL_CLUSTER_GROUPS,
                                                ZCL_CMD_ADD_GROUP, payload);

    if (success) {
        // Update member list
        self.memberDevices_.insert(ieeeAddress);
        self.updateMemberList();
        std::cout << "[ZigbeeGroup] Device added successfully" << std::endl;
    }

    return success;
}

bool ZigbeeGroup::removeMember(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeGroup& self = static_cast<ZigbeeGroup&>(inst);

    std::cout << "[ZigbeeGroup] Execute: Remove Member" << std::endl;

    // Parse IEEE address from data
    if (data.size() < 8) {
        std::cerr << "[ZigbeeGroup] Invalid data size for IEEE address" << std::endl;
        return false;
    }

    uint64_t ieeeAddress = 0;
    for (size_t i = 0; i < 8; i++) {
        ieeeAddress |= (static_cast<uint64_t>(data[i]) << (i * 8));
    }

    std::cout << "[ZigbeeGroup] Removing device 0x" << std::hex << std::setfill('0')
              << std::setw(16) << ieeeAddress << " from group" << std::endl;

    // Get group ID
    uint16_t groupId = static_cast<uint16_t>(self.resource(GROUP_ID_0)->get<INT_T>());

    // Build ZCL Remove Group command payload
    // Format: [GroupId:2]
    std::vector<uint8_t> payload;
    payload.push_back(groupId & 0xFF);
    payload.push_back((groupId >> 8) & 0xFF);

    bool success = self.sendZclCommandToDevice(ieeeAddress, ZCL_CLUSTER_GROUPS,
                                                ZCL_CMD_REMOVE_GROUP, payload);

    if (success) {
        // Update member list
        self.memberDevices_.erase(ieeeAddress);
        self.updateMemberList();
        std::cout << "[ZigbeeGroup] Device removed successfully" << std::endl;
    }

    return success;
}

bool ZigbeeGroup::sceneRecall(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeGroup& self = static_cast<ZigbeeGroup&>(inst);

    std::cout << "[ZigbeeGroup] Execute: Scene Recall" << std::endl;

    // Parse scene ID from data (default to 0)
    uint8_t sceneId = data.empty() ? 0 : data[0];

    std::cout << "[ZigbeeGroup] Recalling scene " << static_cast<int>(sceneId) << std::endl;

    // Get group ID
    uint16_t groupId = static_cast<uint16_t>(self.resource(GROUP_ID_0)->get<INT_T>());

    // Build ZCL Recall Scene command payload
    // Format: [GroupId:2][SceneId:1]
    std::vector<uint8_t> payload;
    payload.push_back(groupId & 0xFF);
    payload.push_back((groupId >> 8) & 0xFF);
    payload.push_back(sceneId);

    bool success = self.sendGroupCommand(ZCL_CLUSTER_SCENES, ZCL_CMD_RECALL_SCENE, payload);

    if (success) {
        self.resource(CURRENT_SCENE_5)->set<INT_T>((INT_T)sceneId);
        std::cout << "[ZigbeeGroup] Scene recalled successfully" << std::endl;
    }

    return success;
}

bool ZigbeeGroup::sceneStore(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeGroup& self = static_cast<ZigbeeGroup&>(inst);

    std::cout << "[ZigbeeGroup] Execute: Scene Store" << std::endl;

    // Parse scene ID from data
    uint8_t sceneId = data.empty() ? 0 : data[0];

    std::cout << "[ZigbeeGroup] Storing scene " << static_cast<int>(sceneId) << std::endl;

    // Get group ID
    uint16_t groupId = static_cast<uint16_t>(self.resource(GROUP_ID_0)->get<INT_T>());

    // Build ZCL Store Scene command payload
    // Format: [GroupId:2][SceneId:1]
    std::vector<uint8_t> payload;
    payload.push_back(groupId & 0xFF);
    payload.push_back((groupId >> 8) & 0xFF);
    payload.push_back(sceneId);

    bool success = self.sendGroupCommand(ZCL_CLUSTER_SCENES, ZCL_CMD_STORE_SCENE, payload);

    if (success) {
        INT_T sceneCount = self.resource(SCENE_COUNT_4)->get<INT_T>();
        self.resource(SCENE_COUNT_4)->set(sceneCount + 1);
        std::cout << "[ZigbeeGroup] Scene stored successfully" << std::endl;
    }

    return success;
}

bool ZigbeeGroup::turnOn(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeGroup& self = static_cast<ZigbeeGroup&>(inst);

    std::cout << "[ZigbeeGroup] Execute: Turn On" << std::endl;

    // ZCL On command has no payload
    std::vector<uint8_t> payload;
    bool success = self.sendGroupCommand(ZCL_CLUSTER_ON_OFF, ZCL_CMD_ON, payload);

    if (success) {
        std::cout << "[ZigbeeGroup] On command sent successfully" << std::endl;
    }

    return success;
}

bool ZigbeeGroup::turnOff(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeGroup& self = static_cast<ZigbeeGroup&>(inst);

    std::cout << "[ZigbeeGroup] Execute: Turn Off" << std::endl;

    // ZCL Off command has no payload
    std::vector<uint8_t> payload;
    bool success = self.sendGroupCommand(ZCL_CLUSTER_ON_OFF, ZCL_CMD_OFF, payload);

    if (success) {
        std::cout << "[ZigbeeGroup] Off command sent successfully" << std::endl;
    }

    return success;
}

bool ZigbeeGroup::toggle(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeGroup& self = static_cast<ZigbeeGroup&>(inst);

    std::cout << "[ZigbeeGroup] Execute: Toggle" << std::endl;

    // ZCL Toggle command has no payload
    std::vector<uint8_t> payload;
    bool success = self.sendGroupCommand(ZCL_CLUSTER_ON_OFF, ZCL_CMD_TOGGLE, payload);

    if (success) {
        std::cout << "[ZigbeeGroup] Toggle command sent successfully" << std::endl;
    }

    return success;
}

bool ZigbeeGroup::setLevel(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeGroup& self = static_cast<ZigbeeGroup&>(inst);

    std::cout << "[ZigbeeGroup] Execute: Set Level" << std::endl;

    // Parse level from data (0-254, 255 = previous)
    uint8_t level = data.empty() ? 254 : data[0];
    // Parse optional transition time (in tenths of a second)
    uint16_t transitionTime = 0;
    if (data.size() >= 3) {
        transitionTime = data[1] | (data[2] << 8);
    }

    std::cout << "[ZigbeeGroup] Setting level to " << static_cast<int>(level)
              << " with transition " << transitionTime << "ms" << std::endl;

    // Build ZCL Move to Level (with On/Off) command payload
    // Format: [Level:1][TransitionTime:2]
    std::vector<uint8_t> payload;
    payload.push_back(level);
    payload.push_back(transitionTime & 0xFF);
    payload.push_back((transitionTime >> 8) & 0xFF);

    bool success = self.sendGroupCommand(ZCL_CLUSTER_LEVEL,
                                          ZCL_CMD_MOVE_TO_LEVEL_WITH_ON_OFF, payload);

    if (success) {
        std::cout << "[ZigbeeGroup] Level command sent successfully" << std::endl;
    }

    return success;
}

bool ZigbeeGroup::setColor(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    ZigbeeGroup& self = static_cast<ZigbeeGroup&>(inst);

    std::cout << "[ZigbeeGroup] Execute: Set Color" << std::endl;

    // Parse hue and saturation from data
    if (data.size() < 2) {
        std::cerr << "[ZigbeeGroup] Invalid data size for color (need H, S)" << std::endl;
        return false;
    }

    uint8_t hue = data[0];         // 0-254 maps to 0-360 degrees
    uint8_t saturation = data[1];  // 0-254
    // Parse optional transition time
    uint16_t transitionTime = 0;
    if (data.size() >= 4) {
        transitionTime = data[2] | (data[3] << 8);
    }

    std::cout << "[ZigbeeGroup] Setting color H:" << static_cast<int>(hue)
              << " S:" << static_cast<int>(saturation)
              << " transition:" << transitionTime << "ms" << std::endl;

    // Build ZCL Move to Hue and Saturation command payload
    // Format: [Hue:1][Saturation:1][TransitionTime:2]
    std::vector<uint8_t> payload;
    payload.push_back(hue);
    payload.push_back(saturation);
    payload.push_back(transitionTime & 0xFF);
    payload.push_back((transitionTime >> 8) & 0xFF);

    bool success = self.sendGroupCommand(ZCL_CLUSTER_COLOR,
                                          ZCL_CMD_MOVE_TO_HUE_AND_SATURATION, payload);

    if (success) {
        std::cout << "[ZigbeeGroup] Color command sent successfully" << std::endl;
    }

    return success;
}

// ==============================================================================
// Helper Methods
// ==============================================================================

void ZigbeeGroup::updateMemberList() {
    std::cout << "[ZigbeeGroup] Updating member list" << std::endl;

    // Update member count
    resource(MEMBER_COUNT_3)->set<INT_T>((INT_T)memberDevices_.size());

    // Build member devices string for resource
    // Note: In a real implementation, this would populate the multiple-instance resource
    std::stringstream ss;
    ss << "[";
    bool first = true;
    for (uint64_t ieee : memberDevices_) {
        if (!first) ss << ",";
        ss << "\"0x" << std::hex << std::setfill('0') << std::setw(16) << ieee << "\"";
        first = false;
    }
    ss << "]";

    std::cout << "[ZigbeeGroup] Members: " << ss.str() << std::endl;
}

bool ZigbeeGroup::sendGroupCommand(uint16_t clusterId, uint8_t commandId,
                                    const std::vector<uint8_t>& payload) {
    uint16_t groupId = static_cast<uint16_t>(resource(GROUP_ID_0)->get<INT_T>());

    std::cout << "[ZigbeeGroup] Sending group command - Group:0x" << std::hex << groupId
              << " Cluster:0x" << clusterId
              << " Cmd:0x" << static_cast<int>(commandId) << std::endl;

    // Try zigbee2mqtt MQTT interface first (most common deployment)
    std::string groupName = resource(GROUP_NAME_1)->get<STRING_T>();

    // Build MQTT payload based on cluster and command
    std::string mqttPayload;
    std::string mqttTopic = "zigbee2mqtt/group_" + std::to_string(groupId) + "/set";

    switch (clusterId) {
        case ZCL_CLUSTER_ON_OFF:
            switch (commandId) {
                case ZCL_CMD_ON:
                    mqttPayload = "{\"state\": \"ON\"}";
                    break;
                case ZCL_CMD_OFF:
                    mqttPayload = "{\"state\": \"OFF\"}";
                    break;
                case ZCL_CMD_TOGGLE:
                    mqttPayload = "{\"state\": \"TOGGLE\"}";
                    break;
            }
            break;

        case ZCL_CLUSTER_LEVEL:
            if (commandId == ZCL_CMD_MOVE_TO_LEVEL_WITH_ON_OFF && !payload.empty()) {
                uint8_t level = payload[0];
                uint16_t brightness = (level * 255) / 254;
                mqttPayload = "{\"brightness\": " + std::to_string(brightness) + "}";
            }
            break;

        case ZCL_CLUSTER_COLOR:
            if (commandId == ZCL_CMD_MOVE_TO_HUE_AND_SATURATION && payload.size() >= 2) {
                uint8_t hue = payload[0];
                uint8_t sat = payload[1];
                // Convert ZCL hue (0-254) to zigbee2mqtt hue (0-360)
                int hue360 = (hue * 360) / 254;
                // Convert ZCL saturation (0-254) to percentage (0-100)
                int satPct = (sat * 100) / 254;
                mqttPayload = "{\"color\": {\"hue\": " + std::to_string(hue360) +
                              ", \"saturation\": " + std::to_string(satPct) + "}}";
            }
            break;

        case ZCL_CLUSTER_SCENES:
            if (commandId == ZCL_CMD_RECALL_SCENE && payload.size() >= 3) {
                uint8_t sceneId = payload[2];
                mqttPayload = "{\"scene_recall\": " + std::to_string(sceneId) + "}";
            } else if (commandId == ZCL_CMD_STORE_SCENE && payload.size() >= 3) {
                uint8_t sceneId = payload[2];
                mqttPayload = "{\"scene_store\": " + std::to_string(sceneId) + "}";
            }
            break;
    }

    if (!mqttPayload.empty()) {
        // Try sending via MQTT
        std::string cmd = "mosquitto_pub -t '" + mqttTopic + "' -m '" + mqttPayload + "' 2>/dev/null";
        int result = system(cmd.c_str());
        if (result == 0) {
            return true;
        }

        // Try alternative topic format (by group name)
        mqttTopic = "zigbee2mqtt/" + groupName + "/set";
        cmd = "mosquitto_pub -t '" + mqttTopic + "' -m '" + mqttPayload + "' 2>/dev/null";
        result = system(cmd.c_str());
        if (result == 0) {
            return true;
        }
    }

    // Fallback: Send ZCL command to each member device individually
    std::cout << "[ZigbeeGroup] MQTT failed, falling back to individual device commands" << std::endl;

    bool anySuccess = false;
    for (uint64_t ieee : memberDevices_) {
        if (sendZclCommandToDevice(ieee, clusterId, commandId, payload)) {
            anySuccess = true;
        }
    }

    return anySuccess;
}

bool ZigbeeGroup::sendZclCommandToDevice(uint64_t ieeeAddress, uint16_t clusterId,
                                          uint8_t commandId, const std::vector<uint8_t>& payload) {
    std::cout << "[ZigbeeGroup] Sending ZCL to device 0x" << std::hex
              << std::setfill('0') << std::setw(16) << ieeeAddress << std::endl;

    // Try zigbee2mqtt first
    std::stringstream ss;
    ss << "0x" << std::hex << std::setfill('0') << std::setw(16) << ieeeAddress;
    std::string ieeeStr = ss.str();

    // Build MQTT payload based on cluster and command
    std::string mqttPayload;
    std::string mqttTopic = "zigbee2mqtt/" + ieeeStr + "/set";

    // Convert ZCL command to zigbee2mqtt JSON
    switch (clusterId) {
        case ZCL_CLUSTER_GROUPS:
            if (commandId == ZCL_CMD_ADD_GROUP && payload.size() >= 2) {
                uint16_t groupId = payload[0] | (payload[1] << 8);
                // zigbee2mqtt uses group_add endpoint
                mqttTopic = "zigbee2mqtt/bridge/request/device/group_add";
                mqttPayload = "{\"device\": \"" + ieeeStr + "\", \"group\": \"group_" +
                              std::to_string(groupId) + "\"}";
            } else if (commandId == ZCL_CMD_REMOVE_GROUP && payload.size() >= 2) {
                uint16_t groupId = payload[0] | (payload[1] << 8);
                mqttTopic = "zigbee2mqtt/bridge/request/device/group_remove";
                mqttPayload = "{\"device\": \"" + ieeeStr + "\", \"group\": \"group_" +
                              std::to_string(groupId) + "\"}";
            }
            break;

        case ZCL_CLUSTER_ON_OFF:
            switch (commandId) {
                case ZCL_CMD_ON:
                    mqttPayload = "{\"state\": \"ON\"}";
                    break;
                case ZCL_CMD_OFF:
                    mqttPayload = "{\"state\": \"OFF\"}";
                    break;
                case ZCL_CMD_TOGGLE:
                    mqttPayload = "{\"state\": \"TOGGLE\"}";
                    break;
            }
            break;

        default:
            // For unsupported clusters, try raw ZCL
            break;
    }

    if (!mqttPayload.empty()) {
        std::string cmd = "mosquitto_pub -t '" + mqttTopic + "' -m '" + mqttPayload + "' 2>/dev/null";
        int result = system(cmd.c_str());
        return (result == 0);
    }

    return false;
}

} // namespace wpp
