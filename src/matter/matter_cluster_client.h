/*
 * Matter Cluster Client Header
 * Friendly LwM2M Client - Matter Cluster Interaction Layer
 *
 * Provides high-level cluster client implementations for common Matter clusters:
 * - OnOff, Level Control, Color Control
 * - Temperature Measurement, Occupancy Sensing
 * - Door Lock, Window Covering, Generic Switch
 */

#ifndef MATTER_CLUSTER_CLIENT_H
#define MATTER_CLUSTER_CLIENT_H

#include "matter_sdk_integration.h"
#include <string>
#include <cstdint>
#include <memory>
#include <map>
#include <functional>

namespace matter {

// ============================================================================
// Cluster IDs (Matter 1.0/1.1 Standard Clusters)
// ============================================================================

namespace ClusterIds {
    constexpr ClusterId ON_OFF = 0x0006;
    constexpr ClusterId LEVEL_CONTROL = 0x0008;
    constexpr ClusterId COLOR_CONTROL = 0x0300;
    constexpr ClusterId TEMPERATURE_MEASUREMENT = 0x0402;
    constexpr ClusterId OCCUPANCY_SENSING = 0x0406;
    constexpr ClusterId DOOR_LOCK = 0x0101;
    constexpr ClusterId WINDOW_COVERING = 0x0102;
    constexpr ClusterId SWITCH = 0x003B;
    constexpr ClusterId BASIC_INFORMATION = 0x0028;
    constexpr ClusterId DESCRIPTOR = 0x001D;
}

// ============================================================================
// Attribute IDs
// ============================================================================

namespace OnOffAttributes {
    constexpr AttributeId ON_OFF = 0x0000;
}

namespace LevelControlAttributes {
    constexpr AttributeId CURRENT_LEVEL = 0x0000;
    constexpr AttributeId MIN_LEVEL = 0x0002;
    constexpr AttributeId MAX_LEVEL = 0x0003;
}

namespace ColorControlAttributes {
    constexpr AttributeId CURRENT_HUE = 0x0000;
    constexpr AttributeId CURRENT_SATURATION = 0x0001;
    constexpr AttributeId COLOR_MODE = 0x0008;
    constexpr AttributeId COLOR_TEMPERATURE = 0x0007;
}

namespace TemperatureMeasurementAttributes {
    constexpr AttributeId MEASURED_VALUE = 0x0000;
    constexpr AttributeId MIN_MEASURED_VALUE = 0x0001;
    constexpr AttributeId MAX_MEASURED_VALUE = 0x0002;
}

namespace OccupancySensingAttributes {
    constexpr AttributeId OCCUPANCY = 0x0000;
    constexpr AttributeId OCCUPANCY_SENSOR_TYPE = 0x0001;
}

namespace DoorLockAttributes {
    constexpr AttributeId LOCK_STATE = 0x0000;
    constexpr AttributeId LOCK_TYPE = 0x0001;
}

namespace WindowCoveringAttributes {
    constexpr AttributeId CURRENT_POSITION_LIFT = 0x0008;
    constexpr AttributeId CURRENT_POSITION_TILT = 0x0009;
    constexpr AttributeId TARGET_POSITION_LIFT = 0x000B;
    constexpr AttributeId TARGET_POSITION_TILT = 0x000C;
}

// ============================================================================
// Command IDs
// ============================================================================

namespace OnOffCommands {
    constexpr CommandId OFF = 0x00;
    constexpr CommandId ON = 0x01;
    constexpr CommandId TOGGLE = 0x02;
}

namespace LevelControlCommands {
    constexpr CommandId MOVE_TO_LEVEL = 0x00;
    constexpr CommandId MOVE = 0x01;
    constexpr CommandId STEP = 0x02;
    constexpr CommandId STOP = 0x03;
}

namespace ColorControlCommands {
    constexpr CommandId MOVE_TO_HUE = 0x00;
    constexpr CommandId MOVE_TO_SATURATION = 0x03;
    constexpr CommandId MOVE_TO_HUE_AND_SATURATION = 0x06;
    constexpr CommandId MOVE_TO_COLOR_TEMPERATURE = 0x0A;
}

namespace DoorLockCommands {
    constexpr CommandId LOCK_DOOR = 0x00;
    constexpr CommandId UNLOCK_DOOR = 0x01;
}

namespace WindowCoveringCommands {
    constexpr CommandId UP_OR_OPEN = 0x00;
    constexpr CommandId DOWN_OR_CLOSE = 0x01;
    constexpr CommandId STOP_MOTION = 0x02;
    constexpr CommandId GO_TO_LIFT_PERCENTAGE = 0x05;
    constexpr CommandId GO_TO_TILT_PERCENTAGE = 0x08;
}

// ============================================================================
// Matter Cluster Client
// ============================================================================

class MatterClusterClient {
public:
    MatterClusterClient(NodeId nodeId, EndpointId endpoint);
    ~MatterClusterClient();

    NodeId getNodeId() const { return m_nodeId; }
    EndpointId getEndpoint() const { return m_endpoint; }

    // Generic attribute operations
    bool readAttribute(ClusterId cluster, AttributeId attribute, std::string& value);
    bool writeAttribute(ClusterId cluster, AttributeId attribute, const std::string& value);
    bool sendCommand(ClusterId cluster, CommandId command, const std::string& args, std::string& response);

    // ========================================================================
    // OnOff Cluster (0x0006)
    // ========================================================================

    bool onOffGetState(bool& isOn);
    bool onOffTurnOn();
    bool onOffTurnOff();
    bool onOffToggle();

    // ========================================================================
    // Level Control Cluster (0x0008)
    // ========================================================================

    bool levelGetCurrentLevel(uint8_t& level);
    bool levelMoveToLevel(uint8_t level, uint16_t transitionTime = 0);
    bool levelMove(bool moveUp, uint8_t rate = 0);
    bool levelStep(bool stepUp, uint8_t stepSize, uint16_t transitionTime = 0);
    bool levelStop();

    // ========================================================================
    // Color Control Cluster (0x0300)
    // ========================================================================

    bool colorGetCurrentHue(uint8_t& hue);
    bool colorGetCurrentSaturation(uint8_t& saturation);
    bool colorGetColorTemperature(uint16_t& temperature);
    bool colorMoveToHue(uint8_t hue, uint8_t direction, uint16_t transitionTime = 0);
    bool colorMoveToSaturation(uint8_t saturation, uint16_t transitionTime = 0);
    bool colorMoveToHueAndSaturation(uint8_t hue, uint8_t saturation, uint16_t transitionTime = 0);
    bool colorMoveToColorTemperature(uint16_t temperature, uint16_t transitionTime = 0);

    // ========================================================================
    // Temperature Measurement Cluster (0x0402)
    // ========================================================================

    bool temperatureGetMeasuredValue(int16_t& temperature);
    bool temperatureGetMinValue(int16_t& minTemperature);
    bool temperatureGetMaxValue(int16_t& maxTemperature);

    // ========================================================================
    // Occupancy Sensing Cluster (0x0406)
    // ========================================================================

    bool occupancyGetState(bool& occupied);
    bool occupancyGetSensorType(uint8_t& sensorType);

    // ========================================================================
    // Door Lock Cluster (0x0101)
    // ========================================================================

    bool doorLockGetState(uint8_t& lockState);
    bool doorLockLock(const std::string& pinCode = "");
    bool doorLockUnlock(const std::string& pinCode = "");

    // ========================================================================
    // Window Covering Cluster (0x0102)
    // ========================================================================

    bool windowCoveringGetLiftPosition(uint8_t& position);
    bool windowCoveringGetTiltPosition(uint8_t& position);
    bool windowCoveringUpOrOpen();
    bool windowCoveringDownOrClose();
    bool windowCoveringStop();
    bool windowCoveringGoToLiftPercentage(uint8_t percentage);
    bool windowCoveringGoToTiltPercentage(uint8_t percentage);

    // ========================================================================
    // Generic Switch Cluster (0x003B)
    // ========================================================================

    bool switchGetCurrentPosition(uint8_t& position);

    // ========================================================================
    // Basic Information Cluster (0x0028)
    // ========================================================================

    bool basicGetVendorName(std::string& vendorName);
    bool basicGetVendorID(uint16_t& vendorId);
    bool basicGetProductName(std::string& productName);
    bool basicGetProductID(uint16_t& productId);
    bool basicGetNodeLabel(std::string& nodeLabel);
    bool basicGetSerialNumber(std::string& serialNumber);
    bool basicGetSoftwareVersion(std::string& softwareVersion);

    // ========================================================================
    // Descriptor Cluster (0x001D)
    // ========================================================================

    bool descriptorGetDeviceTypeList(std::vector<uint32_t>& deviceTypes);
    bool descriptorGetServerList(std::vector<ClusterId>& clusters);
    bool descriptorGetPartsList(std::vector<EndpointId>& endpoints);

private:
    // Helper methods for encoding/decoding attribute values
    bool readBoolAttribute(ClusterId cluster, AttributeId attribute, bool& value);
    bool readUint8Attribute(ClusterId cluster, AttributeId attribute, uint8_t& value);
    bool readUint16Attribute(ClusterId cluster, AttributeId attribute, uint16_t& value);
    bool readInt16Attribute(ClusterId cluster, AttributeId attribute, int16_t& value);
    bool readStringAttribute(ClusterId cluster, AttributeId attribute, std::string& value);

    bool writeBoolAttribute(ClusterId cluster, AttributeId attribute, bool value);
    bool writeUint8Attribute(ClusterId cluster, AttributeId attribute, uint8_t value);
    bool writeUint16Attribute(ClusterId cluster, AttributeId attribute, uint16_t value);

    NodeId m_nodeId;
    EndpointId m_endpoint;
};

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * Convert Matter cluster name to cluster ID
 */
ClusterId getClusterIdByName(const std::string& clusterName);

/**
 * Convert cluster ID to cluster name
 */
std::string getClusterNameById(ClusterId clusterId);

/**
 * Get list of supported clusters
 */
std::vector<std::string> getSupportedClusters();

/**
 * Check if cluster is supported
 */
bool isClusterSupported(ClusterId clusterId);

/**
 * Parse JSON command arguments
 */
bool parseCommandArgs(const std::string& jsonArgs, std::map<std::string, std::string>& args);

/**
 * Create JSON response
 */
std::string createJsonResponse(bool success, const std::string& message = "", const std::map<std::string, std::string>& data = {});

} // namespace matter

#endif // MATTER_CLUSTER_CLIENT_H
