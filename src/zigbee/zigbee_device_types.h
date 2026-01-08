/*
 * Zigbee Device Type Definitions
 *
 * Standard Zigbee device types based on Zigbee Alliance specifications.
 * Includes device type IDs, profile IDs, and device capability definitions.
 */

#ifndef ZIGBEE_DEVICE_TYPES_H
#define ZIGBEE_DEVICE_TYPES_H

#include <cstdint>
#include <string>

namespace zigbee {

/**
 * @brief Zigbee Profile IDs
 */
namespace ZigbeeProfile {
    constexpr uint16_t HOME_AUTOMATION = 0x0104;      // HA (Home Automation)
    constexpr uint16_t LIGHT_LINK = 0xC05E;           // ZLL (Zigbee Light Link)
    constexpr uint16_t GREEN_POWER = 0xA1E0;          // Green Power
    constexpr uint16_t SMART_ENERGY = 0x0109;         // SE (Smart Energy)
    constexpr uint16_t ZIGBEE_3_0 = 0x0104;           // Zigbee 3.0 uses HA profile
}

/**
 * @brief Home Automation Device Type IDs
 */
namespace HADeviceType {
    // Generic
    constexpr uint16_t ON_OFF_SWITCH = 0x0000;
    constexpr uint16_t LEVEL_CONTROL_SWITCH = 0x0001;
    constexpr uint16_t ON_OFF_OUTPUT = 0x0002;
    constexpr uint16_t LEVEL_CONTROLLABLE_OUTPUT = 0x0003;
    constexpr uint16_t SCENE_SELECTOR = 0x0004;
    constexpr uint16_t CONFIGURATION_TOOL = 0x0005;
    constexpr uint16_t REMOTE_CONTROL = 0x0006;
    constexpr uint16_t COMBINED_INTERFACE = 0x0007;
    constexpr uint16_t RANGE_EXTENDER = 0x0008;
    constexpr uint16_t MAINS_POWER_OUTLET = 0x0009;
    constexpr uint16_t DOOR_LOCK = 0x000A;
    constexpr uint16_t DOOR_LOCK_CONTROLLER = 0x000B;
    constexpr uint16_t SIMPLE_SENSOR = 0x000C;
    constexpr uint16_t CONSUMPTION_AWARENESS_DEVICE = 0x000D;
    constexpr uint16_t HOME_GATEWAY = 0x0050;
    constexpr uint16_t SMART_PLUG = 0x0051;
    constexpr uint16_t WHITE_GOODS = 0x0052;
    constexpr uint16_t METER_INTERFACE = 0x0053;

    // Lighting
    constexpr uint16_t ON_OFF_LIGHT = 0x0100;
    constexpr uint16_t DIMMABLE_LIGHT = 0x0101;
    constexpr uint16_t COLOR_DIMMABLE_LIGHT = 0x0102;
    constexpr uint16_t ON_OFF_LIGHT_SWITCH = 0x0103;
    constexpr uint16_t DIMMER_SWITCH = 0x0104;
    constexpr uint16_t COLOR_DIMMER_SWITCH = 0x0105;
    constexpr uint16_t LIGHT_SENSOR = 0x0106;
    constexpr uint16_t OCCUPANCY_SENSOR = 0x0107;
    constexpr uint16_t ON_OFF_BALLAST = 0x0108;
    constexpr uint16_t DIMMABLE_BALLAST = 0x0109;
    constexpr uint16_t ON_OFF_PLUGIN_UNIT = 0x010A;
    constexpr uint16_t DIMMABLE_PLUGIN_UNIT = 0x010B;
    constexpr uint16_t COLOR_TEMPERATURE_LIGHT = 0x010C;
    constexpr uint16_t EXTENDED_COLOR_LIGHT = 0x010D;
    constexpr uint16_t LIGHT_LEVEL_SENSOR = 0x010E;
    constexpr uint16_t COLOR_CONTROLLER = 0x0800;
    constexpr uint16_t COLOR_SCENE_CONTROLLER = 0x0810;
    constexpr uint16_t NON_COLOR_CONTROLLER = 0x0820;
    constexpr uint16_t NON_COLOR_SCENE_CONTROLLER = 0x0830;
    constexpr uint16_t CONTROL_BRIDGE = 0x0840;
    constexpr uint16_t ON_OFF_SENSOR = 0x0850;

    // Closures
    constexpr uint16_t SHADE = 0x0200;
    constexpr uint16_t SHADE_CONTROLLER = 0x0201;
    constexpr uint16_t WINDOW_COVERING_DEVICE = 0x0202;
    constexpr uint16_t WINDOW_COVERING_CONTROLLER = 0x0203;

    // HVAC
    constexpr uint16_t HEATING_COOLING_UNIT = 0x0300;
    constexpr uint16_t THERMOSTAT = 0x0301;
    constexpr uint16_t TEMPERATURE_SENSOR = 0x0302;
    constexpr uint16_t PUMP = 0x0303;
    constexpr uint16_t PUMP_CONTROLLER = 0x0304;
    constexpr uint16_t PRESSURE_SENSOR = 0x0305;
    constexpr uint16_t FLOW_SENSOR = 0x0306;
    constexpr uint16_t MINI_SPLIT_AC = 0x0307;

    // Intruder Alarm Systems (IAS)
    constexpr uint16_t IAS_CONTROL_INDICATING_EQUIPMENT = 0x0400;
    constexpr uint16_t IAS_ANCILLARY_CONTROL_EQUIPMENT = 0x0401;
    constexpr uint16_t IAS_ZONE = 0x0402;
    constexpr uint16_t IAS_WARNING_DEVICE = 0x0403;
}

/**
 * @brief IAS Zone Types
 */
namespace IASZoneType {
    constexpr uint16_t STANDARD_CIE = 0x0000;
    constexpr uint16_t MOTION_SENSOR = 0x000D;
    constexpr uint16_t CONTACT_SWITCH = 0x0015;
    constexpr uint16_t FIRE_SENSOR = 0x0028;
    constexpr uint16_t WATER_SENSOR = 0x002A;
    constexpr uint16_t CO_SENSOR = 0x002B;
    constexpr uint16_t PERSONAL_EMERGENCY_DEVICE = 0x002C;
    constexpr uint16_t VIBRATION_MOVEMENT_SENSOR = 0x002D;
    constexpr uint16_t REMOTE_CONTROL = 0x010F;
    constexpr uint16_t KEY_FOB = 0x0115;
    constexpr uint16_t KEYPAD = 0x021D;
    constexpr uint16_t STANDARD_WARNING_DEVICE = 0x0225;
    constexpr uint16_t GLASS_BREAK_SENSOR = 0x0226;
    constexpr uint16_t SECURITY_REPEATER = 0x0229;
}

/**
 * @brief Power Source Types
 */
namespace PowerSource {
    constexpr uint8_t UNKNOWN = 0x00;
    constexpr uint8_t MAINS_SINGLE_PHASE = 0x01;
    constexpr uint8_t MAINS_THREE_PHASE = 0x02;
    constexpr uint8_t BATTERY = 0x03;
    constexpr uint8_t DC_SOURCE = 0x04;
    constexpr uint8_t EMERGENCY_MAINS_CONSTANTLY_POWERED = 0x05;
    constexpr uint8_t EMERGENCY_MAINS_TRANSFER_SWITCH = 0x06;
}

/**
 * @brief Device capability flags
 */
struct DeviceCapabilities {
    bool supportsOnOff;
    bool supportsLevelControl;
    bool supportsColorControl;
    bool supportsColorTemperature;
    bool supportsScenes;
    bool supportsGroups;
    bool supportsSecurity;
    bool supportsMeasurement;
    bool supportsMetering;
    bool supportsOTA;

    DeviceCapabilities()
        : supportsOnOff(false),
          supportsLevelControl(false),
          supportsColorControl(false),
          supportsColorTemperature(false),
          supportsScenes(false),
          supportsGroups(false),
          supportsSecurity(false),
          supportsMeasurement(false),
          supportsMetering(false),
          supportsOTA(false) {}
};

/**
 * @brief Get device type name from device ID
 * @param deviceId Device type ID
 * @return Device type name string
 */
inline std::string getDeviceTypeName(uint16_t deviceId) {
    switch (deviceId) {
        // Generic
        case HADeviceType::ON_OFF_SWITCH: return "On/Off Switch";
        case HADeviceType::LEVEL_CONTROL_SWITCH: return "Level Control Switch";
        case HADeviceType::ON_OFF_OUTPUT: return "On/Off Output";
        case HADeviceType::LEVEL_CONTROLLABLE_OUTPUT: return "Level Controllable Output";
        case HADeviceType::SCENE_SELECTOR: return "Scene Selector";
        case HADeviceType::CONFIGURATION_TOOL: return "Configuration Tool";
        case HADeviceType::REMOTE_CONTROL: return "Remote Control";
        case HADeviceType::COMBINED_INTERFACE: return "Combined Interface";
        case HADeviceType::RANGE_EXTENDER: return "Range Extender";
        case HADeviceType::MAINS_POWER_OUTLET: return "Mains Power Outlet";
        case HADeviceType::DOOR_LOCK: return "Door Lock";
        case HADeviceType::DOOR_LOCK_CONTROLLER: return "Door Lock Controller";
        case HADeviceType::SIMPLE_SENSOR: return "Simple Sensor";
        case HADeviceType::HOME_GATEWAY: return "Home Gateway";
        case HADeviceType::SMART_PLUG: return "Smart Plug";

        // Lighting
        case HADeviceType::ON_OFF_LIGHT: return "On/Off Light";
        case HADeviceType::DIMMABLE_LIGHT: return "Dimmable Light";
        case HADeviceType::COLOR_DIMMABLE_LIGHT: return "Color Dimmable Light";
        case HADeviceType::ON_OFF_LIGHT_SWITCH: return "On/Off Light Switch";
        case HADeviceType::DIMMER_SWITCH: return "Dimmer Switch";
        case HADeviceType::COLOR_DIMMER_SWITCH: return "Color Dimmer Switch";
        case HADeviceType::LIGHT_SENSOR: return "Light Sensor";
        case HADeviceType::OCCUPANCY_SENSOR: return "Occupancy Sensor";
        case HADeviceType::COLOR_TEMPERATURE_LIGHT: return "Color Temperature Light";
        case HADeviceType::EXTENDED_COLOR_LIGHT: return "Extended Color Light";

        // Closures
        case HADeviceType::SHADE: return "Shade";
        case HADeviceType::SHADE_CONTROLLER: return "Shade Controller";
        case HADeviceType::WINDOW_COVERING_DEVICE: return "Window Covering";
        case HADeviceType::WINDOW_COVERING_CONTROLLER: return "Window Covering Controller";

        // HVAC
        case HADeviceType::HEATING_COOLING_UNIT: return "Heating/Cooling Unit";
        case HADeviceType::THERMOSTAT: return "Thermostat";
        case HADeviceType::TEMPERATURE_SENSOR: return "Temperature Sensor";
        case HADeviceType::PUMP: return "Pump";
        case HADeviceType::PUMP_CONTROLLER: return "Pump Controller";
        case HADeviceType::PRESSURE_SENSOR: return "Pressure Sensor";
        case HADeviceType::FLOW_SENSOR: return "Flow Sensor";

        // IAS
        case HADeviceType::IAS_CONTROL_INDICATING_EQUIPMENT: return "IAS Control & Indicating Equipment";
        case HADeviceType::IAS_ANCILLARY_CONTROL_EQUIPMENT: return "IAS Ancillary Control Equipment";
        case HADeviceType::IAS_ZONE: return "IAS Zone";
        case HADeviceType::IAS_WARNING_DEVICE: return "IAS Warning Device";

        default: return "Unknown Device";
    }
}

/**
 * @brief Get IAS zone type name
 * @param zoneType Zone type ID
 * @return Zone type name string
 */
inline std::string getIASZoneTypeName(uint16_t zoneType) {
    switch (zoneType) {
        case IASZoneType::STANDARD_CIE: return "Standard CIE";
        case IASZoneType::MOTION_SENSOR: return "Motion Sensor";
        case IASZoneType::CONTACT_SWITCH: return "Contact Switch";
        case IASZoneType::FIRE_SENSOR: return "Fire Sensor";
        case IASZoneType::WATER_SENSOR: return "Water Sensor";
        case IASZoneType::CO_SENSOR: return "CO Sensor";
        case IASZoneType::PERSONAL_EMERGENCY_DEVICE: return "Personal Emergency Device";
        case IASZoneType::VIBRATION_MOVEMENT_SENSOR: return "Vibration/Movement Sensor";
        case IASZoneType::REMOTE_CONTROL: return "Remote Control";
        case IASZoneType::KEY_FOB: return "Key Fob";
        case IASZoneType::KEYPAD: return "Keypad";
        case IASZoneType::GLASS_BREAK_SENSOR: return "Glass Break Sensor";
        default: return "Unknown Zone Type";
    }
}

/**
 * @brief Determine device capabilities based on device type
 * @param deviceId Device type ID
 * @return Device capabilities structure
 */
inline DeviceCapabilities getDeviceCapabilities(uint16_t deviceId) {
    DeviceCapabilities caps;

    switch (deviceId) {
        case HADeviceType::ON_OFF_LIGHT:
        case HADeviceType::ON_OFF_LIGHT_SWITCH:
        case HADeviceType::ON_OFF_OUTPUT:
        case HADeviceType::ON_OFF_PLUGIN_UNIT:
        case HADeviceType::SMART_PLUG:
        case HADeviceType::MAINS_POWER_OUTLET:
            caps.supportsOnOff = true;
            caps.supportsGroups = true;
            caps.supportsScenes = true;
            caps.supportsOTA = true;
            break;

        case HADeviceType::DIMMABLE_LIGHT:
        case HADeviceType::DIMMER_SWITCH:
        case HADeviceType::DIMMABLE_PLUGIN_UNIT:
            caps.supportsOnOff = true;
            caps.supportsLevelControl = true;
            caps.supportsGroups = true;
            caps.supportsScenes = true;
            caps.supportsOTA = true;
            break;

        case HADeviceType::COLOR_DIMMABLE_LIGHT:
        case HADeviceType::EXTENDED_COLOR_LIGHT:
            caps.supportsOnOff = true;
            caps.supportsLevelControl = true;
            caps.supportsColorControl = true;
            caps.supportsGroups = true;
            caps.supportsScenes = true;
            caps.supportsOTA = true;
            break;

        case HADeviceType::COLOR_TEMPERATURE_LIGHT:
            caps.supportsOnOff = true;
            caps.supportsLevelControl = true;
            caps.supportsColorTemperature = true;
            caps.supportsGroups = true;
            caps.supportsScenes = true;
            caps.supportsOTA = true;
            break;

        case HADeviceType::TEMPERATURE_SENSOR:
        case HADeviceType::LIGHT_SENSOR:
        case HADeviceType::PRESSURE_SENSOR:
            caps.supportsMeasurement = true;
            caps.supportsOTA = true;
            break;

        case HADeviceType::OCCUPANCY_SENSOR:
            caps.supportsMeasurement = true;
            caps.supportsOTA = true;
            break;

        case HADeviceType::IAS_ZONE:
            caps.supportsSecurity = true;
            caps.supportsOTA = true;
            break;

        case HADeviceType::THERMOSTAT:
            caps.supportsOnOff = true;
            caps.supportsMeasurement = true;
            caps.supportsScenes = true;
            caps.supportsOTA = true;
            break;

        case HADeviceType::DOOR_LOCK:
            caps.supportsOnOff = true;
            caps.supportsSecurity = true;
            caps.supportsOTA = true;
            break;

        default:
            caps.supportsOTA = true;
            break;
    }

    return caps;
}

} // namespace zigbee

#endif // ZIGBEE_DEVICE_TYPES_H
