/*
 * Matter Device Types
 * Friendly LwM2M Client - Matter Device Type Definitions
 *
 * Defines Matter device types according to Matter 1.0/1.1 specification
 */

#ifndef MATTER_DEVICE_TYPES_H
#define MATTER_DEVICE_TYPES_H

#include <cstdint>
#include <string>

namespace matter {

// ============================================================================
// Device Type Definitions (Matter Specification)
// ============================================================================

namespace DeviceTypes {
    // Utility Device Types (0x0000 - 0x00FF)
    constexpr uint16_t ROOT_NODE = 0x0016;
    constexpr uint16_t OTA_REQUESTOR = 0x0012;
    constexpr uint16_t OTA_PROVIDER = 0x0014;
    constexpr uint16_t AGGREGATOR = 0x000E;
    constexpr uint16_t BRIDGED_NODE = 0x0013;

    // Lighting Device Types (0x0100 - 0x01FF)
    constexpr uint16_t ON_OFF_LIGHT = 0x0100;
    constexpr uint16_t DIMMABLE_LIGHT = 0x0101;
    constexpr uint16_t COLOR_TEMPERATURE_LIGHT = 0x010C;
    constexpr uint16_t EXTENDED_COLOR_LIGHT = 0x010D;

    // Smart Plug Device Types (0x0300 - 0x03FF)
    constexpr uint16_t ON_OFF_PLUG_IN_UNIT = 0x010A;
    constexpr uint16_t DIMMABLE_PLUG_IN_UNIT = 0x010B;

    // Switch Device Types (0x0100 - 0x01FF)
    constexpr uint16_t ON_OFF_LIGHT_SWITCH = 0x0103;
    constexpr uint16_t DIMMER_SWITCH = 0x0104;
    constexpr uint16_t COLOR_DIMMER_SWITCH = 0x0105;
    constexpr uint16_t CONTROL_BRIDGE = 0x0840;
    constexpr uint16_t GENERIC_SWITCH = 0x000F;

    // Sensor Device Types (0x0300 - 0x03FF)
    constexpr uint16_t CONTACT_SENSOR = 0x0015;
    constexpr uint16_t LIGHT_SENSOR = 0x0106;
    constexpr uint16_t OCCUPANCY_SENSOR = 0x0107;
    constexpr uint16_t TEMPERATURE_SENSOR = 0x0302;
    constexpr uint16_t PRESSURE_SENSOR = 0x0305;
    constexpr uint16_t FLOW_SENSOR = 0x0306;
    constexpr uint16_t HUMIDITY_SENSOR = 0x0307;
    constexpr uint16_t AIR_QUALITY_SENSOR = 0x002C;

    // Closure Device Types (0x0200 - 0x02FF)
    constexpr uint16_t DOOR_LOCK = 0x000A;
    constexpr uint16_t DOOR_LOCK_CONTROLLER = 0x000B;
    constexpr uint16_t WINDOW_COVERING = 0x0202;
    constexpr uint16_t WINDOW_COVERING_CONTROLLER = 0x0203;

    // HVAC Device Types (0x0300 - 0x03FF)
    constexpr uint16_t HEATING_COOLING_UNIT = 0x0300;
    constexpr uint16_t THERMOSTAT = 0x0301;
    constexpr uint16_t FAN = 0x002B;
    constexpr uint16_t AIR_PURIFIER = 0x002D;

    // Media Device Types (0x0500 - 0x05FF)
    constexpr uint16_t BASIC_VIDEO_PLAYER = 0x0028;
    constexpr uint16_t CASTING_VIDEO_PLAYER = 0x0023;
    constexpr uint16_t SPEAKER = 0x0022;
    constexpr uint16_t CONTENT_APP = 0x0024;

    // Appliance Device Types (0x0700 - 0x07FF)
    constexpr uint16_t DISHWASHER = 0x0075;
    constexpr uint16_t LAUNDRY_WASHER = 0x0073;
    constexpr uint16_t ROBOTIC_VACUUM_CLEANER = 0x0074;
    constexpr uint16_t ROOM_AIR_CONDITIONER = 0x0072;
    constexpr uint16_t REFRIGERATOR = 0x0070;
    constexpr uint16_t TEMPERATURE_CONTROLLED_CABINET = 0x0071;

    // Generic Device Types
    constexpr uint16_t GENERIC_ACTUATOR = 0x0002;
    constexpr uint16_t GENERIC_SENSOR = 0x0003;
}

// ============================================================================
// Device Type Information
// ============================================================================

struct DeviceTypeInfo {
    uint16_t deviceType;
    std::string name;
    std::string category;
    bool supportsOnOff;
    bool supportsLevel;
    bool supportsColor;
    bool supportsTemperature;
    bool supportsLock;
    bool supportsCovering;
};

// ============================================================================
// Device Type Helper Functions
// ============================================================================

/**
 * Get device type name
 */
inline std::string getDeviceTypeName(uint16_t deviceType) {
    switch (deviceType) {
        // Utility
        case DeviceTypes::ROOT_NODE: return "Root Node";
        case DeviceTypes::OTA_REQUESTOR: return "OTA Requestor";
        case DeviceTypes::OTA_PROVIDER: return "OTA Provider";
        case DeviceTypes::AGGREGATOR: return "Aggregator";
        case DeviceTypes::BRIDGED_NODE: return "Bridged Node";

        // Lighting
        case DeviceTypes::ON_OFF_LIGHT: return "On/Off Light";
        case DeviceTypes::DIMMABLE_LIGHT: return "Dimmable Light";
        case DeviceTypes::COLOR_TEMPERATURE_LIGHT: return "Color Temperature Light";
        case DeviceTypes::EXTENDED_COLOR_LIGHT: return "Extended Color Light";

        // Smart Plugs
        case DeviceTypes::ON_OFF_PLUG_IN_UNIT: return "On/Off Plug-in Unit";
        case DeviceTypes::DIMMABLE_PLUG_IN_UNIT: return "Dimmable Plug-in Unit";

        // Switches
        case DeviceTypes::ON_OFF_LIGHT_SWITCH: return "On/Off Light Switch";
        case DeviceTypes::DIMMER_SWITCH: return "Dimmer Switch";
        case DeviceTypes::COLOR_DIMMER_SWITCH: return "Color Dimmer Switch";
        case DeviceTypes::CONTROL_BRIDGE: return "Control Bridge";
        case DeviceTypes::GENERIC_SWITCH: return "Generic Switch";

        // Sensors
        case DeviceTypes::CONTACT_SENSOR: return "Contact Sensor";
        case DeviceTypes::LIGHT_SENSOR: return "Light Sensor";
        case DeviceTypes::OCCUPANCY_SENSOR: return "Occupancy Sensor";
        case DeviceTypes::TEMPERATURE_SENSOR: return "Temperature Sensor";
        case DeviceTypes::PRESSURE_SENSOR: return "Pressure Sensor";
        case DeviceTypes::FLOW_SENSOR: return "Flow Sensor";
        case DeviceTypes::HUMIDITY_SENSOR: return "Humidity Sensor";
        case DeviceTypes::AIR_QUALITY_SENSOR: return "Air Quality Sensor";

        // Closures
        case DeviceTypes::DOOR_LOCK: return "Door Lock";
        case DeviceTypes::DOOR_LOCK_CONTROLLER: return "Door Lock Controller";
        case DeviceTypes::WINDOW_COVERING: return "Window Covering";
        case DeviceTypes::WINDOW_COVERING_CONTROLLER: return "Window Covering Controller";

        // HVAC
        case DeviceTypes::HEATING_COOLING_UNIT: return "Heating/Cooling Unit";
        case DeviceTypes::THERMOSTAT: return "Thermostat";
        case DeviceTypes::FAN: return "Fan";
        case DeviceTypes::AIR_PURIFIER: return "Air Purifier";

        // Media
        case DeviceTypes::BASIC_VIDEO_PLAYER: return "Basic Video Player";
        case DeviceTypes::CASTING_VIDEO_PLAYER: return "Casting Video Player";
        case DeviceTypes::SPEAKER: return "Speaker";
        case DeviceTypes::CONTENT_APP: return "Content App";

        // Appliances
        case DeviceTypes::DISHWASHER: return "Dishwasher";
        case DeviceTypes::LAUNDRY_WASHER: return "Laundry Washer";
        case DeviceTypes::ROBOTIC_VACUUM_CLEANER: return "Robotic Vacuum Cleaner";
        case DeviceTypes::ROOM_AIR_CONDITIONER: return "Room Air Conditioner";
        case DeviceTypes::REFRIGERATOR: return "Refrigerator";
        case DeviceTypes::TEMPERATURE_CONTROLLED_CABINET: return "Temperature Controlled Cabinet";

        // Generic
        case DeviceTypes::GENERIC_ACTUATOR: return "Generic Actuator";
        case DeviceTypes::GENERIC_SENSOR: return "Generic Sensor";

        default: return "Unknown Device Type";
    }
}

/**
 * Get device type category
 */
inline std::string getDeviceTypeCategory(uint16_t deviceType) {
    if (deviceType >= 0x0100 && deviceType <= 0x01FF) {
        return "Lighting";
    } else if (deviceType >= 0x0200 && deviceType <= 0x02FF) {
        return "Closures";
    } else if (deviceType >= 0x0300 && deviceType <= 0x03FF) {
        return "HVAC/Sensors";
    } else if (deviceType >= 0x0500 && deviceType <= 0x05FF) {
        return "Media";
    } else if (deviceType >= 0x0700 && deviceType <= 0x07FF) {
        return "Appliances";
    } else if (deviceType <= 0x00FF) {
        return "Utility";
    }
    return "Other";
}

/**
 * Check if device type supports OnOff cluster
 */
inline bool supportsOnOff(uint16_t deviceType) {
    return deviceType == DeviceTypes::ON_OFF_LIGHT ||
           deviceType == DeviceTypes::DIMMABLE_LIGHT ||
           deviceType == DeviceTypes::COLOR_TEMPERATURE_LIGHT ||
           deviceType == DeviceTypes::EXTENDED_COLOR_LIGHT ||
           deviceType == DeviceTypes::ON_OFF_PLUG_IN_UNIT ||
           deviceType == DeviceTypes::DIMMABLE_PLUG_IN_UNIT ||
           deviceType == DeviceTypes::ON_OFF_LIGHT_SWITCH ||
           deviceType == DeviceTypes::FAN;
}

/**
 * Check if device type supports Level Control cluster
 */
inline bool supportsLevelControl(uint16_t deviceType) {
    return deviceType == DeviceTypes::DIMMABLE_LIGHT ||
           deviceType == DeviceTypes::COLOR_TEMPERATURE_LIGHT ||
           deviceType == DeviceTypes::EXTENDED_COLOR_LIGHT ||
           deviceType == DeviceTypes::DIMMABLE_PLUG_IN_UNIT ||
           deviceType == DeviceTypes::DIMMER_SWITCH ||
           deviceType == DeviceTypes::COLOR_DIMMER_SWITCH;
}

/**
 * Check if device type supports Color Control cluster
 */
inline bool supportsColorControl(uint16_t deviceType) {
    return deviceType == DeviceTypes::COLOR_TEMPERATURE_LIGHT ||
           deviceType == DeviceTypes::EXTENDED_COLOR_LIGHT ||
           deviceType == DeviceTypes::COLOR_DIMMER_SWITCH;
}

/**
 * Check if device type is a sensor
 */
inline bool isSensor(uint16_t deviceType) {
    return deviceType == DeviceTypes::CONTACT_SENSOR ||
           deviceType == DeviceTypes::LIGHT_SENSOR ||
           deviceType == DeviceTypes::OCCUPANCY_SENSOR ||
           deviceType == DeviceTypes::TEMPERATURE_SENSOR ||
           deviceType == DeviceTypes::PRESSURE_SENSOR ||
           deviceType == DeviceTypes::FLOW_SENSOR ||
           deviceType == DeviceTypes::HUMIDITY_SENSOR ||
           deviceType == DeviceTypes::AIR_QUALITY_SENSOR ||
           deviceType == DeviceTypes::GENERIC_SENSOR;
}

/**
 * Check if device type is an actuator
 */
inline bool isActuator(uint16_t deviceType) {
    return supportsOnOff(deviceType) ||
           supportsLevelControl(deviceType) ||
           deviceType == DeviceTypes::DOOR_LOCK ||
           deviceType == DeviceTypes::WINDOW_COVERING ||
           deviceType == DeviceTypes::THERMOSTAT ||
           deviceType == DeviceTypes::GENERIC_ACTUATOR;
}

/**
 * Get full device type information
 */
inline DeviceTypeInfo getDeviceTypeInfo(uint16_t deviceType) {
    DeviceTypeInfo info;
    info.deviceType = deviceType;
    info.name = getDeviceTypeName(deviceType);
    info.category = getDeviceTypeCategory(deviceType);
    info.supportsOnOff = supportsOnOff(deviceType);
    info.supportsLevel = supportsLevelControl(deviceType);
    info.supportsColor = supportsColorControl(deviceType);
    info.supportsTemperature = (deviceType == DeviceTypes::TEMPERATURE_SENSOR ||
                                deviceType == DeviceTypes::THERMOSTAT);
    info.supportsLock = (deviceType == DeviceTypes::DOOR_LOCK);
    info.supportsCovering = (deviceType == DeviceTypes::WINDOW_COVERING);
    return info;
}

} // namespace matter

#endif // MATTER_DEVICE_TYPES_H
