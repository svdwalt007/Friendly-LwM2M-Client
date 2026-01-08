/*
 * Matter Cluster Client Implementation
 * Friendly LwM2M Client - Matter Cluster Interaction
 */

#include "matter_cluster_client.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>

#ifdef WITH_MATTER_SDK
#include <app/InteractionModelEngine.h>
#include <app/ReadClient.h>
#include <app/WriteClient.h>
#include <app/CommandSender.h>
#endif

namespace matter {

// ============================================================================
// MatterClusterClient Implementation
// ============================================================================

MatterClusterClient::MatterClusterClient(NodeId nodeId, EndpointId endpoint)
    : m_nodeId(nodeId)
    , m_endpoint(endpoint) {
    std::cout << "[Cluster] Created cluster client for node 0x" << std::hex << nodeId
              << std::dec << ", endpoint " << endpoint << std::endl;
}

MatterClusterClient::~MatterClusterClient() {
}

// ============================================================================
// Generic Operations
// ============================================================================

bool MatterClusterClient::readAttribute(ClusterId cluster, AttributeId attribute, std::string& value) {
    std::cout << "[Cluster] Reading attribute - Cluster: 0x" << std::hex << cluster
              << ", Attribute: 0x" << attribute << std::dec << std::endl;

#ifdef WITH_MATTER_SDK
    // Real Matter SDK read implementation
    // Use chip::app::ReadClient to read attribute
    return false;
#else
    // Stub implementation
    std::ostringstream oss;
    oss << "{\"cluster\":\"0x" << std::hex << cluster << "\",\"attribute\":\"0x"
        << attribute << std::dec << "\",\"value\":\"stub_value\"}";
    value = oss.str();
    return true;
#endif
}

bool MatterClusterClient::writeAttribute(ClusterId cluster, AttributeId attribute, const std::string& value) {
    std::cout << "[Cluster] Writing attribute - Cluster: 0x" << std::hex << cluster
              << ", Attribute: 0x" << attribute << std::dec << ", Value: " << value << std::endl;

#ifdef WITH_MATTER_SDK
    // Real Matter SDK write implementation
    // Use chip::app::WriteClient to write attribute
    return false;
#else
    // Stub implementation
    return true;
#endif
}

bool MatterClusterClient::sendCommand(ClusterId cluster, CommandId command,
                                     const std::string& args, std::string& response) {
    std::cout << "[Cluster] Sending command - Cluster: 0x" << std::hex << cluster
              << ", Command: 0x" << command << std::dec << std::endl;

#ifdef WITH_MATTER_SDK
    // Real Matter SDK command implementation
    // Use chip::app::CommandSender to send command
    return false;
#else
    // Stub implementation
    response = createJsonResponse(true, "Command executed (stub)");
    return true;
#endif
}

// ============================================================================
// Helper Methods for Typed Reads/Writes
// ============================================================================

bool MatterClusterClient::readBoolAttribute(ClusterId cluster, AttributeId attribute, bool& value) {
    std::string strValue;
    if (!readAttribute(cluster, attribute, strValue)) {
        return false;
    }

    // Parse JSON response
    // Simplified parsing for stub - production needs proper JSON parser
    value = (strValue.find("true") != std::string::npos) || (strValue.find("1") != std::string::npos);
    return true;
}

bool MatterClusterClient::readUint8Attribute(ClusterId cluster, AttributeId attribute, uint8_t& value) {
    std::string strValue;
    if (!readAttribute(cluster, attribute, strValue)) {
        return false;
    }

    // Parse value - simplified for stub
    value = 128; // Stub value
    return true;
}

bool MatterClusterClient::readUint16Attribute(ClusterId cluster, AttributeId attribute, uint16_t& value) {
    std::string strValue;
    if (!readAttribute(cluster, attribute, strValue)) {
        return false;
    }

    value = 2000; // Stub value
    return true;
}

bool MatterClusterClient::readInt16Attribute(ClusterId cluster, AttributeId attribute, int16_t& value) {
    std::string strValue;
    if (!readAttribute(cluster, attribute, strValue)) {
        return false;
    }

    value = 2300; // Stub value (23.00°C in 0.01°C units)
    return true;
}

bool MatterClusterClient::readStringAttribute(ClusterId cluster, AttributeId attribute, std::string& value) {
    return readAttribute(cluster, attribute, value);
}

bool MatterClusterClient::writeBoolAttribute(ClusterId cluster, AttributeId attribute, bool value) {
    std::string strValue = value ? "true" : "false";
    return writeAttribute(cluster, attribute, strValue);
}

bool MatterClusterClient::writeUint8Attribute(ClusterId cluster, AttributeId attribute, uint8_t value) {
    std::ostringstream oss;
    oss << static_cast<int>(value);
    return writeAttribute(cluster, attribute, oss.str());
}

bool MatterClusterClient::writeUint16Attribute(ClusterId cluster, AttributeId attribute, uint16_t value) {
    std::ostringstream oss;
    oss << value;
    return writeAttribute(cluster, attribute, oss.str());
}

// ============================================================================
// OnOff Cluster (0x0006)
// ============================================================================

bool MatterClusterClient::onOffGetState(bool& isOn) {
    return readBoolAttribute(ClusterIds::ON_OFF, OnOffAttributes::ON_OFF, isOn);
}

bool MatterClusterClient::onOffTurnOn() {
    std::string response;
    return sendCommand(ClusterIds::ON_OFF, OnOffCommands::ON, "", response);
}

bool MatterClusterClient::onOffTurnOff() {
    std::string response;
    return sendCommand(ClusterIds::ON_OFF, OnOffCommands::OFF, "", response);
}

bool MatterClusterClient::onOffToggle() {
    std::string response;
    return sendCommand(ClusterIds::ON_OFF, OnOffCommands::TOGGLE, "", response);
}

// ============================================================================
// Level Control Cluster (0x0008)
// ============================================================================

bool MatterClusterClient::levelGetCurrentLevel(uint8_t& level) {
    return readUint8Attribute(ClusterIds::LEVEL_CONTROL, LevelControlAttributes::CURRENT_LEVEL, level);
}

bool MatterClusterClient::levelMoveToLevel(uint8_t level, uint16_t transitionTime) {
    std::ostringstream oss;
    oss << "{\"level\":" << static_cast<int>(level) << ",\"transitionTime\":" << transitionTime << "}";

    std::string response;
    return sendCommand(ClusterIds::LEVEL_CONTROL, LevelControlCommands::MOVE_TO_LEVEL, oss.str(), response);
}

bool MatterClusterClient::levelMove(bool moveUp, uint8_t rate) {
    std::ostringstream oss;
    oss << "{\"moveMode\":" << (moveUp ? 0 : 1) << ",\"rate\":" << static_cast<int>(rate) << "}";

    std::string response;
    return sendCommand(ClusterIds::LEVEL_CONTROL, LevelControlCommands::MOVE, oss.str(), response);
}

bool MatterClusterClient::levelStep(bool stepUp, uint8_t stepSize, uint16_t transitionTime) {
    std::ostringstream oss;
    oss << "{\"stepMode\":" << (stepUp ? 0 : 1) << ",\"stepSize\":" << static_cast<int>(stepSize)
        << ",\"transitionTime\":" << transitionTime << "}";

    std::string response;
    return sendCommand(ClusterIds::LEVEL_CONTROL, LevelControlCommands::STEP, oss.str(), response);
}

bool MatterClusterClient::levelStop() {
    std::string response;
    return sendCommand(ClusterIds::LEVEL_CONTROL, LevelControlCommands::STOP, "", response);
}

// ============================================================================
// Color Control Cluster (0x0300)
// ============================================================================

bool MatterClusterClient::colorGetCurrentHue(uint8_t& hue) {
    return readUint8Attribute(ClusterIds::COLOR_CONTROL, ColorControlAttributes::CURRENT_HUE, hue);
}

bool MatterClusterClient::colorGetCurrentSaturation(uint8_t& saturation) {
    return readUint8Attribute(ClusterIds::COLOR_CONTROL, ColorControlAttributes::CURRENT_SATURATION, saturation);
}

bool MatterClusterClient::colorGetColorTemperature(uint16_t& temperature) {
    return readUint16Attribute(ClusterIds::COLOR_CONTROL, ColorControlAttributes::COLOR_TEMPERATURE, temperature);
}

bool MatterClusterClient::colorMoveToHue(uint8_t hue, uint8_t direction, uint16_t transitionTime) {
    std::ostringstream oss;
    oss << "{\"hue\":" << static_cast<int>(hue) << ",\"direction\":" << static_cast<int>(direction)
        << ",\"transitionTime\":" << transitionTime << "}";

    std::string response;
    return sendCommand(ClusterIds::COLOR_CONTROL, ColorControlCommands::MOVE_TO_HUE, oss.str(), response);
}

bool MatterClusterClient::colorMoveToSaturation(uint8_t saturation, uint16_t transitionTime) {
    std::ostringstream oss;
    oss << "{\"saturation\":" << static_cast<int>(saturation) << ",\"transitionTime\":" << transitionTime << "}";

    std::string response;
    return sendCommand(ClusterIds::COLOR_CONTROL, ColorControlCommands::MOVE_TO_SATURATION, oss.str(), response);
}

bool MatterClusterClient::colorMoveToHueAndSaturation(uint8_t hue, uint8_t saturation, uint16_t transitionTime) {
    std::ostringstream oss;
    oss << "{\"hue\":" << static_cast<int>(hue) << ",\"saturation\":" << static_cast<int>(saturation)
        << ",\"transitionTime\":" << transitionTime << "}";

    std::string response;
    return sendCommand(ClusterIds::COLOR_CONTROL, ColorControlCommands::MOVE_TO_HUE_AND_SATURATION, oss.str(), response);
}

bool MatterClusterClient::colorMoveToColorTemperature(uint16_t temperature, uint16_t transitionTime) {
    std::ostringstream oss;
    oss << "{\"colorTemperature\":" << temperature << ",\"transitionTime\":" << transitionTime << "}";

    std::string response;
    return sendCommand(ClusterIds::COLOR_CONTROL, ColorControlCommands::MOVE_TO_COLOR_TEMPERATURE, oss.str(), response);
}

// ============================================================================
// Temperature Measurement Cluster (0x0402)
// ============================================================================

bool MatterClusterClient::temperatureGetMeasuredValue(int16_t& temperature) {
    return readInt16Attribute(ClusterIds::TEMPERATURE_MEASUREMENT,
                             TemperatureMeasurementAttributes::MEASURED_VALUE,
                             temperature);
}

bool MatterClusterClient::temperatureGetMinValue(int16_t& minTemperature) {
    return readInt16Attribute(ClusterIds::TEMPERATURE_MEASUREMENT,
                             TemperatureMeasurementAttributes::MIN_MEASURED_VALUE,
                             minTemperature);
}

bool MatterClusterClient::temperatureGetMaxValue(int16_t& maxTemperature) {
    return readInt16Attribute(ClusterIds::TEMPERATURE_MEASUREMENT,
                             TemperatureMeasurementAttributes::MAX_MEASURED_VALUE,
                             maxTemperature);
}

// ============================================================================
// Occupancy Sensing Cluster (0x0406)
// ============================================================================

bool MatterClusterClient::occupancyGetState(bool& occupied) {
    uint8_t occupancy;
    if (!readUint8Attribute(ClusterIds::OCCUPANCY_SENSING, OccupancySensingAttributes::OCCUPANCY, occupancy)) {
        return false;
    }
    occupied = (occupancy & 0x01) != 0;
    return true;
}

bool MatterClusterClient::occupancyGetSensorType(uint8_t& sensorType) {
    return readUint8Attribute(ClusterIds::OCCUPANCY_SENSING,
                             OccupancySensingAttributes::OCCUPANCY_SENSOR_TYPE,
                             sensorType);
}

// ============================================================================
// Door Lock Cluster (0x0101)
// ============================================================================

bool MatterClusterClient::doorLockGetState(uint8_t& lockState) {
    return readUint8Attribute(ClusterIds::DOOR_LOCK, DoorLockAttributes::LOCK_STATE, lockState);
}

bool MatterClusterClient::doorLockLock(const std::string& pinCode) {
    std::ostringstream oss;
    if (!pinCode.empty()) {
        oss << "{\"pinCode\":\"" << pinCode << "\"}";
    }

    std::string response;
    return sendCommand(ClusterIds::DOOR_LOCK, DoorLockCommands::LOCK_DOOR, oss.str(), response);
}

bool MatterClusterClient::doorLockUnlock(const std::string& pinCode) {
    std::ostringstream oss;
    if (!pinCode.empty()) {
        oss << "{\"pinCode\":\"" << pinCode << "\"}";
    }

    std::string response;
    return sendCommand(ClusterIds::DOOR_LOCK, DoorLockCommands::UNLOCK_DOOR, oss.str(), response);
}

// ============================================================================
// Window Covering Cluster (0x0102)
// ============================================================================

bool MatterClusterClient::windowCoveringGetLiftPosition(uint8_t& position) {
    return readUint8Attribute(ClusterIds::WINDOW_COVERING,
                             WindowCoveringAttributes::CURRENT_POSITION_LIFT,
                             position);
}

bool MatterClusterClient::windowCoveringGetTiltPosition(uint8_t& position) {
    return readUint8Attribute(ClusterIds::WINDOW_COVERING,
                             WindowCoveringAttributes::CURRENT_POSITION_TILT,
                             position);
}

bool MatterClusterClient::windowCoveringUpOrOpen() {
    std::string response;
    return sendCommand(ClusterIds::WINDOW_COVERING, WindowCoveringCommands::UP_OR_OPEN, "", response);
}

bool MatterClusterClient::windowCoveringDownOrClose() {
    std::string response;
    return sendCommand(ClusterIds::WINDOW_COVERING, WindowCoveringCommands::DOWN_OR_CLOSE, "", response);
}

bool MatterClusterClient::windowCoveringStop() {
    std::string response;
    return sendCommand(ClusterIds::WINDOW_COVERING, WindowCoveringCommands::STOP_MOTION, "", response);
}

bool MatterClusterClient::windowCoveringGoToLiftPercentage(uint8_t percentage) {
    std::ostringstream oss;
    oss << "{\"percentage\":" << static_cast<int>(percentage) << "}";

    std::string response;
    return sendCommand(ClusterIds::WINDOW_COVERING, WindowCoveringCommands::GO_TO_LIFT_PERCENTAGE, oss.str(), response);
}

bool MatterClusterClient::windowCoveringGoToTiltPercentage(uint8_t percentage) {
    std::ostringstream oss;
    oss << "{\"percentage\":" << static_cast<int>(percentage) << "}";

    std::string response;
    return sendCommand(ClusterIds::WINDOW_COVERING, WindowCoveringCommands::GO_TO_TILT_PERCENTAGE, oss.str(), response);
}

// ============================================================================
// Generic Switch Cluster (0x003B)
// ============================================================================

bool MatterClusterClient::switchGetCurrentPosition(uint8_t& position) {
    // Switch cluster typically uses events rather than attributes
    // For stub, we return a dummy value
    position = 0;
    return true;
}

// ============================================================================
// Basic Information Cluster (0x0028)
// ============================================================================

bool MatterClusterClient::basicGetVendorName(std::string& vendorName) {
    return readStringAttribute(ClusterIds::BASIC_INFORMATION, 0x0001, vendorName);
}

bool MatterClusterClient::basicGetVendorID(uint16_t& vendorId) {
    return readUint16Attribute(ClusterIds::BASIC_INFORMATION, 0x0002, vendorId);
}

bool MatterClusterClient::basicGetProductName(std::string& productName) {
    return readStringAttribute(ClusterIds::BASIC_INFORMATION, 0x0003, productName);
}

bool MatterClusterClient::basicGetProductID(uint16_t& productId) {
    return readUint16Attribute(ClusterIds::BASIC_INFORMATION, 0x0004, productId);
}

bool MatterClusterClient::basicGetNodeLabel(std::string& nodeLabel) {
    return readStringAttribute(ClusterIds::BASIC_INFORMATION, 0x0005, nodeLabel);
}

bool MatterClusterClient::basicGetSerialNumber(std::string& serialNumber) {
    return readStringAttribute(ClusterIds::BASIC_INFORMATION, 0x000F, serialNumber);
}

bool MatterClusterClient::basicGetSoftwareVersion(std::string& softwareVersion) {
    uint16_t version;
    if (readUint16Attribute(ClusterIds::BASIC_INFORMATION, 0x0009, version)) {
        std::ostringstream oss;
        oss << (version >> 8) << "." << (version & 0xFF);
        softwareVersion = oss.str();
        return true;
    }
    return false;
}

// ============================================================================
// Descriptor Cluster (0x001D)
// ============================================================================

bool MatterClusterClient::descriptorGetDeviceTypeList(std::vector<uint32_t>& deviceTypes) {
    // Stub implementation
    deviceTypes.clear();
    deviceTypes.push_back(256); // Light bulb
    return true;
}

bool MatterClusterClient::descriptorGetServerList(std::vector<ClusterId>& clusters) {
    // Stub implementation
    clusters.clear();
    clusters.push_back(ClusterIds::ON_OFF);
    clusters.push_back(ClusterIds::LEVEL_CONTROL);
    clusters.push_back(ClusterIds::BASIC_INFORMATION);
    clusters.push_back(ClusterIds::DESCRIPTOR);
    return true;
}

bool MatterClusterClient::descriptorGetPartsList(std::vector<EndpointId>& endpoints) {
    // Stub implementation
    endpoints.clear();
    endpoints.push_back(1);
    return true;
}

// ============================================================================
// Helper Functions
// ============================================================================

ClusterId getClusterIdByName(const std::string& clusterName) {
    if (clusterName == "OnOff") return ClusterIds::ON_OFF;
    if (clusterName == "LevelControl") return ClusterIds::LEVEL_CONTROL;
    if (clusterName == "ColorControl") return ClusterIds::COLOR_CONTROL;
    if (clusterName == "TemperatureMeasurement") return ClusterIds::TEMPERATURE_MEASUREMENT;
    if (clusterName == "OccupancySensing") return ClusterIds::OCCUPANCY_SENSING;
    if (clusterName == "DoorLock") return ClusterIds::DOOR_LOCK;
    if (clusterName == "WindowCovering") return ClusterIds::WINDOW_COVERING;
    if (clusterName == "Switch") return ClusterIds::SWITCH;
    if (clusterName == "BasicInformation") return ClusterIds::BASIC_INFORMATION;
    if (clusterName == "Descriptor") return ClusterIds::DESCRIPTOR;
    return 0xFFFFFFFF;
}

std::string getClusterNameById(ClusterId clusterId) {
    switch (clusterId) {
        case ClusterIds::ON_OFF: return "OnOff";
        case ClusterIds::LEVEL_CONTROL: return "LevelControl";
        case ClusterIds::COLOR_CONTROL: return "ColorControl";
        case ClusterIds::TEMPERATURE_MEASUREMENT: return "TemperatureMeasurement";
        case ClusterIds::OCCUPANCY_SENSING: return "OccupancySensing";
        case ClusterIds::DOOR_LOCK: return "DoorLock";
        case ClusterIds::WINDOW_COVERING: return "WindowCovering";
        case ClusterIds::SWITCH: return "Switch";
        case ClusterIds::BASIC_INFORMATION: return "BasicInformation";
        case ClusterIds::DESCRIPTOR: return "Descriptor";
        default: return "Unknown";
    }
}

std::vector<std::string> getSupportedClusters() {
    return {
        "OnOff",
        "LevelControl",
        "ColorControl",
        "TemperatureMeasurement",
        "OccupancySensing",
        "DoorLock",
        "WindowCovering",
        "Switch",
        "BasicInformation",
        "Descriptor"
    };
}

bool isClusterSupported(ClusterId clusterId) {
    return clusterId == ClusterIds::ON_OFF ||
           clusterId == ClusterIds::LEVEL_CONTROL ||
           clusterId == ClusterIds::COLOR_CONTROL ||
           clusterId == ClusterIds::TEMPERATURE_MEASUREMENT ||
           clusterId == ClusterIds::OCCUPANCY_SENSING ||
           clusterId == ClusterIds::DOOR_LOCK ||
           clusterId == ClusterIds::WINDOW_COVERING ||
           clusterId == ClusterIds::SWITCH ||
           clusterId == ClusterIds::BASIC_INFORMATION ||
           clusterId == ClusterIds::DESCRIPTOR;
}

bool parseCommandArgs(const std::string& jsonArgs, std::map<std::string, std::string>& args) {
    // Simplified JSON parsing - production should use a proper JSON parser
    args.clear();
    if (jsonArgs.empty()) {
        return true;
    }

    // Basic JSON parsing (very simplified)
    size_t pos = 0;
    while (pos < jsonArgs.length()) {
        size_t keyStart = jsonArgs.find('"', pos);
        if (keyStart == std::string::npos) break;
        size_t keyEnd = jsonArgs.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;

        std::string key = jsonArgs.substr(keyStart + 1, keyEnd - keyStart - 1);

        size_t valueStart = jsonArgs.find(':', keyEnd);
        if (valueStart == std::string::npos) break;
        valueStart++;

        // Skip whitespace
        while (valueStart < jsonArgs.length() && isspace(jsonArgs[valueStart])) {
            valueStart++;
        }

        size_t valueEnd;
        std::string value;

        if (jsonArgs[valueStart] == '"') {
            // String value
            valueStart++;
            valueEnd = jsonArgs.find('"', valueStart);
            if (valueEnd == std::string::npos) break;
            value = jsonArgs.substr(valueStart, valueEnd - valueStart);
            valueEnd++;
        } else {
            // Number value
            valueEnd = jsonArgs.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) valueEnd = jsonArgs.length();
            value = jsonArgs.substr(valueStart, valueEnd - valueStart);
            // Trim whitespace
            size_t end = value.find_last_not_of(" \t\n\r");
            if (end != std::string::npos) {
                value = value.substr(0, end + 1);
            }
        }

        args[key] = value;
        pos = valueEnd;
    }

    return true;
}

std::string createJsonResponse(bool success, const std::string& message,
                               const std::map<std::string, std::string>& data) {
    std::ostringstream oss;
    oss << "{\"success\":" << (success ? "true" : "false");

    if (!message.empty()) {
        oss << ",\"message\":\"" << message << "\"";
    }

    if (!data.empty()) {
        oss << ",\"data\":{";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) oss << ",";
            oss << "\"" << pair.first << "\":\"" << pair.second << "\"";
            first = false;
        }
        oss << "}";
    }

    oss << "}";
    return oss.str();
}

} // namespace matter
