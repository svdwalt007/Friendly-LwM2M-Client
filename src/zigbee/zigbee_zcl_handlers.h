/*
 * Zigbee ZCL Cluster Handlers
 *
 * Comprehensive ZCL (Zigbee Cluster Library) cluster handlers supporting:
 * - Basic Cluster (0x0000) - Device information
 * - OnOff Cluster (0x0006) - On/Off control
 * - Level Control Cluster (0x0008) - Dimming control
 * - Color Control Cluster (0x0300) - Color/temperature control
 * - Temperature Measurement Cluster (0x0402) - Temperature sensors
 * - Humidity Measurement Cluster (0x0405) - Humidity sensors
 * - Occupancy Sensing Cluster (0x0406) - Occupancy sensors
 * - IAS Zone Cluster (0x0500) - Security sensors
 * - Metering Cluster (0x0702) - Smart plug metering
 * - OTA Upgrade Cluster (0x0019) - Firmware updates
 */

#ifndef ZIGBEE_ZCL_HANDLERS_H
#define ZIGBEE_ZCL_HANDLERS_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

namespace zigbee {

// Forward declarations
class ZigbeeCoordinator;

/**
 * @brief ZCL Cluster IDs
 */
namespace ZclCluster {
    constexpr uint16_t BASIC = 0x0000;
    constexpr uint16_t POWER_CONFIG = 0x0001;
    constexpr uint16_t IDENTIFY = 0x0003;
    constexpr uint16_t GROUPS = 0x0004;
    constexpr uint16_t SCENES = 0x0005;
    constexpr uint16_t ON_OFF = 0x0006;
    constexpr uint16_t LEVEL_CONTROL = 0x0008;
    constexpr uint16_t DOOR_LOCK = 0x0101;
    constexpr uint16_t WINDOW_COVERING = 0x0102;
    constexpr uint16_t THERMOSTAT = 0x0201;
    constexpr uint16_t COLOR_CONTROL = 0x0300;
    constexpr uint16_t ILLUMINANCE_MEASUREMENT = 0x0400;
    constexpr uint16_t TEMPERATURE_MEASUREMENT = 0x0402;
    constexpr uint16_t PRESSURE_MEASUREMENT = 0x0403;
    constexpr uint16_t HUMIDITY_MEASUREMENT = 0x0405;
    constexpr uint16_t OCCUPANCY_SENSING = 0x0406;
    constexpr uint16_t IAS_ZONE = 0x0500;
    constexpr uint16_t IAS_ACE = 0x0501;
    constexpr uint16_t METERING = 0x0702;
    constexpr uint16_t ELECTRICAL_MEASUREMENT = 0x0B04;
    constexpr uint16_t DIAGNOSTICS = 0x0B05;
    constexpr uint16_t OTA_UPGRADE = 0x0019;
    constexpr uint16_t GREEN_POWER = 0xF000;
}

/**
 * @brief ZCL Data Types
 */
enum class ZclDataType : uint8_t {
    NO_DATA = 0x00,
    DATA8 = 0x08,
    DATA16 = 0x09,
    DATA24 = 0x0A,
    DATA32 = 0x0B,
    DATA40 = 0x0C,
    DATA48 = 0x0D,
    DATA56 = 0x0E,
    DATA64 = 0x0F,
    BOOLEAN = 0x10,
    BITMAP8 = 0x18,
    BITMAP16 = 0x19,
    BITMAP24 = 0x1A,
    BITMAP32 = 0x1B,
    UINT8 = 0x20,
    UINT16 = 0x21,
    UINT24 = 0x22,
    UINT32 = 0x23,
    UINT40 = 0x24,
    UINT48 = 0x25,
    UINT56 = 0x26,
    UINT64 = 0x27,
    INT8 = 0x28,
    INT16 = 0x29,
    INT24 = 0x2A,
    INT32 = 0x2B,
    ENUM8 = 0x30,
    ENUM16 = 0x31,
    FLOAT_SEMI = 0x38,
    FLOAT_SINGLE = 0x39,
    FLOAT_DOUBLE = 0x3A,
    OCTET_STRING = 0x41,
    CHAR_STRING = 0x42,
    LONG_OCTET_STRING = 0x43,
    LONG_CHAR_STRING = 0x44,
    ARRAY = 0x48,
    STRUCTURE = 0x4C,
    TIME_OF_DAY = 0xE0,
    DATE = 0xE1,
    UTC_TIME = 0xE2,
    CLUSTER_ID = 0xE8,
    ATTRIBUTE_ID = 0xE9,
    IEEE_ADDRESS = 0xF0,
    SECURITY_KEY = 0xF1,
    UNKNOWN = 0xFF
};

/**
 * @brief ZCL Attribute Value
 */
struct ZclAttributeValue {
    uint16_t attributeId;
    ZclDataType dataType;
    std::vector<uint8_t> value;

    // Helper functions to extract typed values
    bool asBool() const;
    uint8_t asUint8() const;
    uint16_t asUint16() const;
    uint32_t asUint32() const;
    int8_t asInt8() const;
    int16_t asInt16() const;
    int32_t asInt32() const;
    float asFloat() const;
    std::string asString() const;
};

/**
 * @brief Base ZCL Cluster Handler
 */
class ZclClusterHandler {
public:
    virtual ~ZclClusterHandler() = default;

    /**
     * @brief Get cluster ID
     */
    virtual uint16_t getClusterId() const = 0;

    /**
     * @brief Get cluster name
     */
    virtual std::string getClusterName() const = 0;

    /**
     * @brief Handle incoming ZCL command
     * @param ieeeAddress Source device IEEE address
     * @param endpoint Source endpoint
     * @param commandId Command ID
     * @param payload Command payload
     * @return true if handled successfully
     */
    virtual bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                              uint8_t commandId, const std::vector<uint8_t>& payload) = 0;

    /**
     * @brief Handle attribute report
     * @param ieeeAddress Source device IEEE address
     * @param endpoint Source endpoint
     * @param attributes Reported attributes
     * @return true if handled successfully
     */
    virtual bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                       const std::vector<ZclAttributeValue>& attributes) = 0;

protected:
    ZigbeeCoordinator* coordinator_;
};

/**
 * @brief Basic Cluster Handler (0x0000)
 */
class BasicClusterHandler : public ZclClusterHandler {
public:
    explicit BasicClusterHandler(ZigbeeCoordinator* coordinator);
    ~BasicClusterHandler() override = default;

    uint16_t getClusterId() const override { return ZclCluster::BASIC; }
    std::string getClusterName() const override { return "Basic"; }

    bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                      uint8_t commandId, const std::vector<uint8_t>& payload) override;
    bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                              const std::vector<ZclAttributeValue>& attributes) override;

    // Attribute IDs
    static constexpr uint16_t ATTR_ZCL_VERSION = 0x0000;
    static constexpr uint16_t ATTR_MANUFACTURER = 0x0004;
    static constexpr uint16_t ATTR_MODEL = 0x0005;
    static constexpr uint16_t ATTR_DATE_CODE = 0x0006;
    static constexpr uint16_t ATTR_POWER_SOURCE = 0x0007;
    static constexpr uint16_t ATTR_SW_BUILD_ID = 0x4000;
};

/**
 * @brief OnOff Cluster Handler (0x0006)
 */
class OnOffClusterHandler : public ZclClusterHandler {
public:
    explicit OnOffClusterHandler(ZigbeeCoordinator* coordinator);
    ~OnOffClusterHandler() override = default;

    uint16_t getClusterId() const override { return ZclCluster::ON_OFF; }
    std::string getClusterName() const override { return "OnOff"; }

    bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                      uint8_t commandId, const std::vector<uint8_t>& payload) override;
    bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                              const std::vector<ZclAttributeValue>& attributes) override;

    // Commands
    static constexpr uint8_t CMD_OFF = 0x00;
    static constexpr uint8_t CMD_ON = 0x01;
    static constexpr uint8_t CMD_TOGGLE = 0x02;

    // Attributes
    static constexpr uint16_t ATTR_ON_OFF = 0x0000;

    // Control functions
    bool turnOn(uint64_t ieeeAddress, uint8_t endpoint);
    bool turnOff(uint64_t ieeeAddress, uint8_t endpoint);
    bool toggle(uint64_t ieeeAddress, uint8_t endpoint);
};

/**
 * @brief Level Control Cluster Handler (0x0008)
 */
class LevelControlClusterHandler : public ZclClusterHandler {
public:
    explicit LevelControlClusterHandler(ZigbeeCoordinator* coordinator);
    ~LevelControlClusterHandler() override = default;

    uint16_t getClusterId() const override { return ZclCluster::LEVEL_CONTROL; }
    std::string getClusterName() const override { return "LevelControl"; }

    bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                      uint8_t commandId, const std::vector<uint8_t>& payload) override;
    bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                              const std::vector<ZclAttributeValue>& attributes) override;

    // Commands
    static constexpr uint8_t CMD_MOVE_TO_LEVEL = 0x00;
    static constexpr uint8_t CMD_MOVE = 0x01;
    static constexpr uint8_t CMD_STEP = 0x02;
    static constexpr uint8_t CMD_STOP = 0x03;

    // Attributes
    static constexpr uint16_t ATTR_CURRENT_LEVEL = 0x0000;

    // Control functions
    bool setLevel(uint64_t ieeeAddress, uint8_t endpoint, uint8_t level, uint16_t transitionTime = 0);
};

/**
 * @brief Color Control Cluster Handler (0x0300)
 */
class ColorControlClusterHandler : public ZclClusterHandler {
public:
    explicit ColorControlClusterHandler(ZigbeeCoordinator* coordinator);
    ~ColorControlClusterHandler() override = default;

    uint16_t getClusterId() const override { return ZclCluster::COLOR_CONTROL; }
    std::string getClusterName() const override { return "ColorControl"; }

    bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                      uint8_t commandId, const std::vector<uint8_t>& payload) override;
    bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                              const std::vector<ZclAttributeValue>& attributes) override;

    // Commands
    static constexpr uint8_t CMD_MOVE_TO_HUE = 0x00;
    static constexpr uint8_t CMD_MOVE_TO_SATURATION = 0x03;
    static constexpr uint8_t CMD_MOVE_TO_HUE_AND_SATURATION = 0x06;
    static constexpr uint8_t CMD_MOVE_TO_COLOR = 0x07;
    static constexpr uint8_t CMD_MOVE_TO_COLOR_TEMPERATURE = 0x0A;

    // Attributes
    static constexpr uint16_t ATTR_CURRENT_HUE = 0x0000;
    static constexpr uint16_t ATTR_CURRENT_SATURATION = 0x0001;
    static constexpr uint16_t ATTR_CURRENT_X = 0x0003;
    static constexpr uint16_t ATTR_CURRENT_Y = 0x0004;
    static constexpr uint16_t ATTR_COLOR_TEMPERATURE = 0x0007;

    // Control functions
    bool setColorHueSaturation(uint64_t ieeeAddress, uint8_t endpoint, uint8_t hue, uint8_t saturation);
    bool setColorXY(uint64_t ieeeAddress, uint8_t endpoint, uint16_t x, uint16_t y);
    bool setColorTemperature(uint64_t ieeeAddress, uint8_t endpoint, uint16_t mireds);
};

/**
 * @brief Temperature Measurement Cluster Handler (0x0402)
 */
class TemperatureMeasurementClusterHandler : public ZclClusterHandler {
public:
    explicit TemperatureMeasurementClusterHandler(ZigbeeCoordinator* coordinator);
    ~TemperatureMeasurementClusterHandler() override = default;

    uint16_t getClusterId() const override { return ZclCluster::TEMPERATURE_MEASUREMENT; }
    std::string getClusterName() const override { return "TemperatureMeasurement"; }

    bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                      uint8_t commandId, const std::vector<uint8_t>& payload) override;
    bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                              const std::vector<ZclAttributeValue>& attributes) override;

    // Attributes
    static constexpr uint16_t ATTR_MEASURED_VALUE = 0x0000;
    static constexpr uint16_t ATTR_MIN_MEASURED_VALUE = 0x0001;
    static constexpr uint16_t ATTR_MAX_MEASURED_VALUE = 0x0002;
};

/**
 * @brief Humidity Measurement Cluster Handler (0x0405)
 */
class HumidityMeasurementClusterHandler : public ZclClusterHandler {
public:
    explicit HumidityMeasurementClusterHandler(ZigbeeCoordinator* coordinator);
    ~HumidityMeasurementClusterHandler() override = default;

    uint16_t getClusterId() const override { return ZclCluster::HUMIDITY_MEASUREMENT; }
    std::string getClusterName() const override { return "HumidityMeasurement"; }

    bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                      uint8_t commandId, const std::vector<uint8_t>& payload) override;
    bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                              const std::vector<ZclAttributeValue>& attributes) override;

    // Attributes
    static constexpr uint16_t ATTR_MEASURED_VALUE = 0x0000;
    static constexpr uint16_t ATTR_MIN_MEASURED_VALUE = 0x0001;
    static constexpr uint16_t ATTR_MAX_MEASURED_VALUE = 0x0002;
};

/**
 * @brief Occupancy Sensing Cluster Handler (0x0406)
 */
class OccupancySensingClusterHandler : public ZclClusterHandler {
public:
    explicit OccupancySensingClusterHandler(ZigbeeCoordinator* coordinator);
    ~OccupancySensingClusterHandler() override = default;

    uint16_t getClusterId() const override { return ZclCluster::OCCUPANCY_SENSING; }
    std::string getClusterName() const override { return "OccupancySensing"; }

    bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                      uint8_t commandId, const std::vector<uint8_t>& payload) override;
    bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                              const std::vector<ZclAttributeValue>& attributes) override;

    // Attributes
    static constexpr uint16_t ATTR_OCCUPANCY = 0x0000;
    static constexpr uint16_t ATTR_OCCUPANCY_SENSOR_TYPE = 0x0001;
};

/**
 * @brief IAS Zone Cluster Handler (0x0500) - Security Sensors
 */
class IasZoneClusterHandler : public ZclClusterHandler {
public:
    explicit IasZoneClusterHandler(ZigbeeCoordinator* coordinator);
    ~IasZoneClusterHandler() override = default;

    uint16_t getClusterId() const override { return ZclCluster::IAS_ZONE; }
    std::string getClusterName() const override { return "IasZone"; }

    bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                      uint8_t commandId, const std::vector<uint8_t>& payload) override;
    bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                              const std::vector<ZclAttributeValue>& attributes) override;

    // Commands
    static constexpr uint8_t CMD_ZONE_STATUS_CHANGE = 0x00;
    static constexpr uint8_t CMD_ZONE_ENROLL_REQUEST = 0x01;

    // Attributes
    static constexpr uint16_t ATTR_ZONE_STATE = 0x0000;
    static constexpr uint16_t ATTR_ZONE_TYPE = 0x0001;
    static constexpr uint16_t ATTR_ZONE_STATUS = 0x0002;

    // Zone types
    enum ZoneType {
        STANDARD_CIE = 0x0000,
        MOTION_SENSOR = 0x000D,
        CONTACT_SWITCH = 0x0015,
        FIRE_SENSOR = 0x0028,
        WATER_SENSOR = 0x002A,
        CO_SENSOR = 0x002B,
        VIBRATION_MOVEMENT = 0x002D
    };
};

/**
 * @brief Metering Cluster Handler (0x0702) - Smart Plugs
 */
class MeteringClusterHandler : public ZclClusterHandler {
public:
    explicit MeteringClusterHandler(ZigbeeCoordinator* coordinator);
    ~MeteringClusterHandler() override = default;

    uint16_t getClusterId() const override { return ZclCluster::METERING; }
    std::string getClusterName() const override { return "Metering"; }

    bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                      uint8_t commandId, const std::vector<uint8_t>& payload) override;
    bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                              const std::vector<ZclAttributeValue>& attributes) override;

    // Attributes
    static constexpr uint16_t ATTR_CURRENT_SUMMATION_DELIVERED = 0x0000;
    static constexpr uint16_t ATTR_INSTANTANEOUS_DEMAND = 0x0400;
};

/**
 * @brief OTA Upgrade Cluster Handler (0x0019)
 */
class OtaUpgradeClusterHandler : public ZclClusterHandler {
public:
    explicit OtaUpgradeClusterHandler(ZigbeeCoordinator* coordinator);
    ~OtaUpgradeClusterHandler() override = default;

    uint16_t getClusterId() const override { return ZclCluster::OTA_UPGRADE; }
    std::string getClusterName() const override { return "OtaUpgrade"; }

    bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                      uint8_t commandId, const std::vector<uint8_t>& payload) override;
    bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                              const std::vector<ZclAttributeValue>& attributes) override;

    // Commands
    static constexpr uint8_t CMD_QUERY_NEXT_IMAGE_REQUEST = 0x01;
    static constexpr uint8_t CMD_IMAGE_BLOCK_REQUEST = 0x03;
    static constexpr uint8_t CMD_UPGRADE_END_REQUEST = 0x06;
};

/**
 * @brief ZCL Handler Manager
 */
class ZclHandlerManager {
public:
    explicit ZclHandlerManager(ZigbeeCoordinator* coordinator);
    ~ZclHandlerManager();

    /**
     * @brief Register a cluster handler
     */
    void registerHandler(std::shared_ptr<ZclClusterHandler> handler);

    /**
     * @brief Get handler for cluster
     */
    ZclClusterHandler* getHandler(uint16_t clusterId);

    /**
     * @brief Handle incoming ZCL message
     */
    bool handleZclMessage(uint64_t ieeeAddress, uint8_t endpoint,
                         uint16_t clusterId, uint8_t commandId,
                         const std::vector<uint8_t>& payload);

    /**
     * @brief Handle attribute reports
     */
    bool handleAttributeReports(uint64_t ieeeAddress, uint8_t endpoint,
                               uint16_t clusterId,
                               const std::vector<ZclAttributeValue>& attributes);

private:
    ZigbeeCoordinator* coordinator_;
    std::map<uint16_t, std::shared_ptr<ZclClusterHandler>> handlers_;
};

} // namespace zigbee

#endif // ZIGBEE_ZCL_HANDLERS_H
