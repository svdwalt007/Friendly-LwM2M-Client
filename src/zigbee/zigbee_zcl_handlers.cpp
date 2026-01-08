/*
 * Zigbee ZCL Cluster Handlers Implementation
 */

#include "zigbee_zcl_handlers.h"
#include "zigbee_coordinator.h"
#include <iostream>
#include <cstring>
#include <algorithm>

namespace zigbee {

// ==============================================================================
// ZclAttributeValue Helper Functions
// ==============================================================================

bool ZclAttributeValue::asBool() const {
    if (value.empty()) return false;
    return value[0] != 0;
}

uint8_t ZclAttributeValue::asUint8() const {
    if (value.empty()) return 0;
    return value[0];
}

uint16_t ZclAttributeValue::asUint16() const {
    if (value.size() < 2) return 0;
    return static_cast<uint16_t>(value[0]) | (static_cast<uint16_t>(value[1]) << 8);
}

uint32_t ZclAttributeValue::asUint32() const {
    if (value.size() < 4) return 0;
    return static_cast<uint32_t>(value[0]) |
           (static_cast<uint32_t>(value[1]) << 8) |
           (static_cast<uint32_t>(value[2]) << 16) |
           (static_cast<uint32_t>(value[3]) << 24);
}

int8_t ZclAttributeValue::asInt8() const {
    if (value.empty()) return 0;
    return static_cast<int8_t>(value[0]);
}

int16_t ZclAttributeValue::asInt16() const {
    uint16_t u = asUint16();
    return static_cast<int16_t>(u);
}

int32_t ZclAttributeValue::asInt32() const {
    uint32_t u = asUint32();
    return static_cast<int32_t>(u);
}

float ZclAttributeValue::asFloat() const {
    if (value.size() < 4) return 0.0f;
    float result;
    std::memcpy(&result, value.data(), sizeof(float));
    return result;
}

std::string ZclAttributeValue::asString() const {
    if (value.empty()) return "";
    // First byte is length for character strings
    if (dataType == ZclDataType::CHAR_STRING) {
        uint8_t len = value[0];
        if (value.size() < len + 1) return "";
        return std::string(reinterpret_cast<const char*>(value.data() + 1), len);
    }
    return std::string(reinterpret_cast<const char*>(value.data()), value.size());
}

// ==============================================================================
// Basic Cluster Handler
// ==============================================================================

BasicClusterHandler::BasicClusterHandler(ZigbeeCoordinator* coordinator) {
    coordinator_ = coordinator;
}

bool BasicClusterHandler::handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                                       uint8_t commandId, const std::vector<uint8_t>& payload) {
    std::cout << "[ZCL-Basic] Command 0x" << std::hex << static_cast<int>(commandId)
              << " from device 0x" << ieeeAddress << std::endl;

    // Basic cluster typically doesn't receive commands
    return true;
}

bool BasicClusterHandler::handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                               const std::vector<ZclAttributeValue>& attributes) {
    std::cout << "[ZCL-Basic] Attribute report from device 0x" << std::hex << ieeeAddress << std::endl;

    for (const auto& attr : attributes) {
        switch (attr.attributeId) {
            case ATTR_MANUFACTURER:
                std::cout << "[ZCL-Basic]   Manufacturer: " << attr.asString() << std::endl;
                break;
            case ATTR_MODEL:
                std::cout << "[ZCL-Basic]   Model: " << attr.asString() << std::endl;
                break;
            case ATTR_POWER_SOURCE:
                std::cout << "[ZCL-Basic]   Power Source: " << static_cast<int>(attr.asUint8()) << std::endl;
                break;
            case ATTR_ZCL_VERSION:
                std::cout << "[ZCL-Basic]   ZCL Version: " << static_cast<int>(attr.asUint8()) << std::endl;
                break;
            default:
                std::cout << "[ZCL-Basic]   Unknown attribute 0x" << std::hex << attr.attributeId << std::endl;
                break;
        }
    }

    return true;
}

// ==============================================================================
// OnOff Cluster Handler
// ==============================================================================

OnOffClusterHandler::OnOffClusterHandler(ZigbeeCoordinator* coordinator) {
    coordinator_ = coordinator;
}

bool OnOffClusterHandler::handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                                       uint8_t commandId, const std::vector<uint8_t>& payload) {
    std::cout << "[ZCL-OnOff] Command 0x" << std::hex << static_cast<int>(commandId)
              << " from device 0x" << ieeeAddress << std::endl;

    switch (commandId) {
        case CMD_OFF:
            std::cout << "[ZCL-OnOff] Device turned OFF" << std::endl;
            break;
        case CMD_ON:
            std::cout << "[ZCL-OnOff] Device turned ON" << std::endl;
            break;
        case CMD_TOGGLE:
            std::cout << "[ZCL-OnOff] Device toggled" << std::endl;
            break;
        default:
            std::cout << "[ZCL-OnOff] Unknown command" << std::endl;
            return false;
    }

    return true;
}

bool OnOffClusterHandler::handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                               const std::vector<ZclAttributeValue>& attributes) {
    std::cout << "[ZCL-OnOff] Attribute report from device 0x" << std::hex << ieeeAddress << std::endl;

    for (const auto& attr : attributes) {
        if (attr.attributeId == ATTR_ON_OFF) {
            bool state = attr.asBool();
            std::cout << "[ZCL-OnOff]   OnOff state: " << (state ? "ON" : "OFF") << std::endl;
        }
    }

    return true;
}

bool OnOffClusterHandler::turnOn(uint64_t ieeeAddress, uint8_t endpoint) {
    std::cout << "[ZCL-OnOff] Turning ON device 0x" << std::hex << ieeeAddress << std::endl;
    std::vector<uint8_t> payload;  // No payload for ON command
    return coordinator_->sendZclCommand(ieeeAddress, endpoint, ZclCluster::ON_OFF, CMD_ON, payload);
}

bool OnOffClusterHandler::turnOff(uint64_t ieeeAddress, uint8_t endpoint) {
    std::cout << "[ZCL-OnOff] Turning OFF device 0x" << std::hex << ieeeAddress << std::endl;
    std::vector<uint8_t> payload;  // No payload for OFF command
    return coordinator_->sendZclCommand(ieeeAddress, endpoint, ZclCluster::ON_OFF, CMD_OFF, payload);
}

bool OnOffClusterHandler::toggle(uint64_t ieeeAddress, uint8_t endpoint) {
    std::cout << "[ZCL-OnOff] Toggling device 0x" << std::hex << ieeeAddress << std::endl;
    std::vector<uint8_t> payload;  // No payload for TOGGLE command
    return coordinator_->sendZclCommand(ieeeAddress, endpoint, ZclCluster::ON_OFF, CMD_TOGGLE, payload);
}

// ==============================================================================
// Level Control Cluster Handler
// ==============================================================================

LevelControlClusterHandler::LevelControlClusterHandler(ZigbeeCoordinator* coordinator) {
    coordinator_ = coordinator;
}

bool LevelControlClusterHandler::handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                                              uint8_t commandId, const std::vector<uint8_t>& payload) {
    std::cout << "[ZCL-Level] Command 0x" << std::hex << static_cast<int>(commandId)
              << " from device 0x" << ieeeAddress << std::endl;
    return true;
}

bool LevelControlClusterHandler::handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                                       const std::vector<ZclAttributeValue>& attributes) {
    std::cout << "[ZCL-Level] Attribute report from device 0x" << std::hex << ieeeAddress << std::endl;

    for (const auto& attr : attributes) {
        if (attr.attributeId == ATTR_CURRENT_LEVEL) {
            uint8_t level = attr.asUint8();
            std::cout << "[ZCL-Level]   Current level: " << static_cast<int>(level) << std::endl;
        }
    }

    return true;
}

bool LevelControlClusterHandler::setLevel(uint64_t ieeeAddress, uint8_t endpoint,
                                          uint8_t level, uint16_t transitionTime) {
    std::cout << "[ZCL-Level] Setting level to " << static_cast<int>(level)
              << " for device 0x" << std::hex << ieeeAddress << std::endl;

    std::vector<uint8_t> payload;
    payload.push_back(level);
    payload.push_back(transitionTime & 0xFF);
    payload.push_back((transitionTime >> 8) & 0xFF);

    return coordinator_->sendZclCommand(ieeeAddress, endpoint, ZclCluster::LEVEL_CONTROL,
                                       CMD_MOVE_TO_LEVEL, payload);
}

// ==============================================================================
// Color Control Cluster Handler
// ==============================================================================

ColorControlClusterHandler::ColorControlClusterHandler(ZigbeeCoordinator* coordinator) {
    coordinator_ = coordinator;
}

bool ColorControlClusterHandler::handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                                              uint8_t commandId, const std::vector<uint8_t>& payload) {
    std::cout << "[ZCL-Color] Command 0x" << std::hex << static_cast<int>(commandId)
              << " from device 0x" << ieeeAddress << std::endl;
    return true;
}

bool ColorControlClusterHandler::handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                                       const std::vector<ZclAttributeValue>& attributes) {
    std::cout << "[ZCL-Color] Attribute report from device 0x" << std::hex << ieeeAddress << std::endl;

    for (const auto& attr : attributes) {
        switch (attr.attributeId) {
            case ATTR_CURRENT_HUE:
                std::cout << "[ZCL-Color]   Hue: " << static_cast<int>(attr.asUint8()) << std::endl;
                break;
            case ATTR_CURRENT_SATURATION:
                std::cout << "[ZCL-Color]   Saturation: " << static_cast<int>(attr.asUint8()) << std::endl;
                break;
            case ATTR_COLOR_TEMPERATURE:
                std::cout << "[ZCL-Color]   Color Temp: " << attr.asUint16() << " mireds" << std::endl;
                break;
        }
    }

    return true;
}

bool ColorControlClusterHandler::setColorHueSaturation(uint64_t ieeeAddress, uint8_t endpoint,
                                                       uint8_t hue, uint8_t saturation) {
    std::cout << "[ZCL-Color] Setting color H:" << static_cast<int>(hue)
              << " S:" << static_cast<int>(saturation) << std::endl;

    std::vector<uint8_t> payload;
    payload.push_back(hue);
    payload.push_back(saturation);
    payload.push_back(0x00);  // Transition time LSB
    payload.push_back(0x00);  // Transition time MSB

    return coordinator_->sendZclCommand(ieeeAddress, endpoint, ZclCluster::COLOR_CONTROL,
                                       CMD_MOVE_TO_HUE_AND_SATURATION, payload);
}

bool ColorControlClusterHandler::setColorXY(uint64_t ieeeAddress, uint8_t endpoint,
                                           uint16_t x, uint16_t y) {
    std::cout << "[ZCL-Color] Setting color X:" << x << " Y:" << y << std::endl;

    std::vector<uint8_t> payload;
    payload.push_back(x & 0xFF);
    payload.push_back((x >> 8) & 0xFF);
    payload.push_back(y & 0xFF);
    payload.push_back((y >> 8) & 0xFF);
    payload.push_back(0x00);  // Transition time LSB
    payload.push_back(0x00);  // Transition time MSB

    return coordinator_->sendZclCommand(ieeeAddress, endpoint, ZclCluster::COLOR_CONTROL,
                                       CMD_MOVE_TO_COLOR, payload);
}

bool ColorControlClusterHandler::setColorTemperature(uint64_t ieeeAddress, uint8_t endpoint,
                                                     uint16_t mireds) {
    std::cout << "[ZCL-Color] Setting color temperature to " << mireds << " mireds" << std::endl;

    std::vector<uint8_t> payload;
    payload.push_back(mireds & 0xFF);
    payload.push_back((mireds >> 8) & 0xFF);
    payload.push_back(0x00);  // Transition time LSB
    payload.push_back(0x00);  // Transition time MSB

    return coordinator_->sendZclCommand(ieeeAddress, endpoint, ZclCluster::COLOR_CONTROL,
                                       CMD_MOVE_TO_COLOR_TEMPERATURE, payload);
}

// ==============================================================================
// Temperature Measurement Cluster Handler
// ==============================================================================

TemperatureMeasurementClusterHandler::TemperatureMeasurementClusterHandler(ZigbeeCoordinator* coordinator) {
    coordinator_ = coordinator;
}

bool TemperatureMeasurementClusterHandler::handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                                                        uint8_t commandId, const std::vector<uint8_t>& payload) {
    // Temperature measurement typically doesn't receive commands
    return true;
}

bool TemperatureMeasurementClusterHandler::handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                                                 const std::vector<ZclAttributeValue>& attributes) {
    std::cout << "[ZCL-Temp] Temperature report from device 0x" << std::hex << ieeeAddress << std::endl;

    for (const auto& attr : attributes) {
        if (attr.attributeId == ATTR_MEASURED_VALUE) {
            int16_t temp = attr.asInt16();
            float tempCelsius = temp / 100.0f;  // Temperature in 0.01°C units
            std::cout << "[ZCL-Temp]   Temperature: " << tempCelsius << "°C" << std::endl;
        }
    }

    return true;
}

// ==============================================================================
// Humidity Measurement Cluster Handler
// ==============================================================================

HumidityMeasurementClusterHandler::HumidityMeasurementClusterHandler(ZigbeeCoordinator* coordinator) {
    coordinator_ = coordinator;
}

bool HumidityMeasurementClusterHandler::handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                                                     uint8_t commandId, const std::vector<uint8_t>& payload) {
    // Humidity measurement typically doesn't receive commands
    return true;
}

bool HumidityMeasurementClusterHandler::handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                                              const std::vector<ZclAttributeValue>& attributes) {
    std::cout << "[ZCL-Humidity] Humidity report from device 0x" << std::hex << ieeeAddress << std::endl;

    for (const auto& attr : attributes) {
        if (attr.attributeId == ATTR_MEASURED_VALUE) {
            uint16_t humidity = attr.asUint16();
            float humidityPercent = humidity / 100.0f;  // Humidity in 0.01% units
            std::cout << "[ZCL-Humidity]   Humidity: " << humidityPercent << "%" << std::endl;
        }
    }

    return true;
}

// ==============================================================================
// Occupancy Sensing Cluster Handler
// ==============================================================================

OccupancySensingClusterHandler::OccupancySensingClusterHandler(ZigbeeCoordinator* coordinator) {
    coordinator_ = coordinator;
}

bool OccupancySensingClusterHandler::handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                                                  uint8_t commandId, const std::vector<uint8_t>& payload) {
    // Occupancy sensing typically doesn't receive commands
    return true;
}

bool OccupancySensingClusterHandler::handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                                           const std::vector<ZclAttributeValue>& attributes) {
    std::cout << "[ZCL-Occupancy] Occupancy report from device 0x" << std::hex << ieeeAddress << std::endl;

    for (const auto& attr : attributes) {
        if (attr.attributeId == ATTR_OCCUPANCY) {
            bool occupied = (attr.asUint8() & 0x01) != 0;
            std::cout << "[ZCL-Occupancy]   Occupancy: " << (occupied ? "OCCUPIED" : "UNOCCUPIED") << std::endl;
        }
    }

    return true;
}

// ==============================================================================
// IAS Zone Cluster Handler
// ==============================================================================

IasZoneClusterHandler::IasZoneClusterHandler(ZigbeeCoordinator* coordinator) {
    coordinator_ = coordinator;
}

bool IasZoneClusterHandler::handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                                         uint8_t commandId, const std::vector<uint8_t>& payload) {
    std::cout << "[ZCL-IASZone] Command 0x" << std::hex << static_cast<int>(commandId)
              << " from device 0x" << ieeeAddress << std::endl;

    switch (commandId) {
        case CMD_ZONE_STATUS_CHANGE:
            if (payload.size() >= 2) {
                uint16_t zoneStatus = payload[0] | (payload[1] << 8);
                std::cout << "[ZCL-IASZone]   Zone Status: 0x" << std::hex << zoneStatus << std::endl;
                std::cout << "[ZCL-IASZone]     Alarm1: " << ((zoneStatus & 0x01) ? "ACTIVE" : "INACTIVE") << std::endl;
                std::cout << "[ZCL-IASZone]     Alarm2: " << ((zoneStatus & 0x02) ? "ACTIVE" : "INACTIVE") << std::endl;
                std::cout << "[ZCL-IASZone]     Tamper: " << ((zoneStatus & 0x04) ? "TAMPERED" : "OK") << std::endl;
                std::cout << "[ZCL-IASZone]     Battery: " << ((zoneStatus & 0x08) ? "LOW" : "OK") << std::endl;
            }
            break;

        case CMD_ZONE_ENROLL_REQUEST:
            std::cout << "[ZCL-IASZone] Zone enrollment request" << std::endl;
            // Should send enrollment response
            break;

        default:
            return false;
    }

    return true;
}

bool IasZoneClusterHandler::handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                                  const std::vector<ZclAttributeValue>& attributes) {
    std::cout << "[ZCL-IASZone] Attribute report from device 0x" << std::hex << ieeeAddress << std::endl;

    for (const auto& attr : attributes) {
        switch (attr.attributeId) {
            case ATTR_ZONE_TYPE:
                std::cout << "[ZCL-IASZone]   Zone Type: 0x" << std::hex << attr.asUint16() << std::endl;
                break;
            case ATTR_ZONE_STATUS:
                std::cout << "[ZCL-IASZone]   Zone Status: 0x" << std::hex << attr.asUint16() << std::endl;
                break;
        }
    }

    return true;
}

// ==============================================================================
// Metering Cluster Handler
// ==============================================================================

MeteringClusterHandler::MeteringClusterHandler(ZigbeeCoordinator* coordinator) {
    coordinator_ = coordinator;
}

bool MeteringClusterHandler::handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                                          uint8_t commandId, const std::vector<uint8_t>& payload) {
    std::cout << "[ZCL-Metering] Command 0x" << std::hex << static_cast<int>(commandId)
              << " from device 0x" << ieeeAddress << std::endl;
    return true;
}

bool MeteringClusterHandler::handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                                   const std::vector<ZclAttributeValue>& attributes) {
    std::cout << "[ZCL-Metering] Metering report from device 0x" << std::hex << ieeeAddress << std::endl;

    for (const auto& attr : attributes) {
        switch (attr.attributeId) {
            case ATTR_CURRENT_SUMMATION_DELIVERED: {
                uint64_t energy = attr.asUint32();  // Simplified, actual is 48-bit
                std::cout << "[ZCL-Metering]   Total Energy: " << energy << " Wh" << std::endl;
                break;
            }
            case ATTR_INSTANTANEOUS_DEMAND: {
                int32_t power = attr.asInt32();
                std::cout << "[ZCL-Metering]   Instantaneous Power: " << power << " W" << std::endl;
                break;
            }
        }
    }

    return true;
}

// ==============================================================================
// OTA Upgrade Cluster Handler
// ==============================================================================

OtaUpgradeClusterHandler::OtaUpgradeClusterHandler(ZigbeeCoordinator* coordinator) {
    coordinator_ = coordinator;
}

bool OtaUpgradeClusterHandler::handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                                            uint8_t commandId, const std::vector<uint8_t>& payload) {
    std::cout << "[ZCL-OTA] Command 0x" << std::hex << static_cast<int>(commandId)
              << " from device 0x" << ieeeAddress << std::endl;

    switch (commandId) {
        case CMD_QUERY_NEXT_IMAGE_REQUEST:
            std::cout << "[ZCL-OTA] Device requesting next image" << std::endl;
            // Should respond with image notify or no image available
            break;

        case CMD_IMAGE_BLOCK_REQUEST:
            std::cout << "[ZCL-OTA] Device requesting image block" << std::endl;
            // Should respond with image block data
            break;

        case CMD_UPGRADE_END_REQUEST:
            std::cout << "[ZCL-OTA] Device completed upgrade" << std::endl;
            // Should respond with upgrade end response
            break;

        default:
            return false;
    }

    return true;
}

bool OtaUpgradeClusterHandler::handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                                     const std::vector<ZclAttributeValue>& attributes) {
    std::cout << "[ZCL-OTA] Attribute report from device 0x" << std::hex << ieeeAddress << std::endl;
    return true;
}

// ==============================================================================
// ZCL Handler Manager
// ==============================================================================

ZclHandlerManager::ZclHandlerManager(ZigbeeCoordinator* coordinator)
    : coordinator_(coordinator) {

    // Register all standard handlers
    registerHandler(std::make_shared<BasicClusterHandler>(coordinator));
    registerHandler(std::make_shared<OnOffClusterHandler>(coordinator));
    registerHandler(std::make_shared<LevelControlClusterHandler>(coordinator));
    registerHandler(std::make_shared<ColorControlClusterHandler>(coordinator));
    registerHandler(std::make_shared<TemperatureMeasurementClusterHandler>(coordinator));
    registerHandler(std::make_shared<HumidityMeasurementClusterHandler>(coordinator));
    registerHandler(std::make_shared<OccupancySensingClusterHandler>(coordinator));
    registerHandler(std::make_shared<IasZoneClusterHandler>(coordinator));
    registerHandler(std::make_shared<MeteringClusterHandler>(coordinator));
    registerHandler(std::make_shared<OtaUpgradeClusterHandler>(coordinator));

    std::cout << "[ZCL] Handler manager initialized with " << handlers_.size() << " handlers" << std::endl;
}

ZclHandlerManager::~ZclHandlerManager() {
    handlers_.clear();
}

void ZclHandlerManager::registerHandler(std::shared_ptr<ZclClusterHandler> handler) {
    if (handler) {
        handlers_[handler->getClusterId()] = handler;
        std::cout << "[ZCL] Registered handler for cluster 0x" << std::hex
                  << handler->getClusterId() << " (" << handler->getClusterName() << ")" << std::endl;
    }
}

ZclClusterHandler* ZclHandlerManager::getHandler(uint16_t clusterId) {
    auto it = handlers_.find(clusterId);
    if (it != handlers_.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool ZclHandlerManager::handleZclMessage(uint64_t ieeeAddress, uint8_t endpoint,
                                        uint16_t clusterId, uint8_t commandId,
                                        const std::vector<uint8_t>& payload) {
    auto handler = getHandler(clusterId);
    if (handler) {
        return handler->handleCommand(ieeeAddress, endpoint, commandId, payload);
    }

    std::cout << "[ZCL] No handler for cluster 0x" << std::hex << clusterId << std::endl;
    return false;
}

bool ZclHandlerManager::handleAttributeReports(uint64_t ieeeAddress, uint8_t endpoint,
                                              uint16_t clusterId,
                                              const std::vector<ZclAttributeValue>& attributes) {
    auto handler = getHandler(clusterId);
    if (handler) {
        return handler->handleAttributeReport(ieeeAddress, endpoint, attributes);
    }

    std::cout << "[ZCL] No handler for cluster 0x" << std::hex << clusterId << std::endl;
    return false;
}

} // namespace zigbee
