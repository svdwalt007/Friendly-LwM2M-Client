/**
 * @file mqtt_server_object.cpp
 * @brief OMA LwM2M MQTT Server Object (ID: 24) Implementation
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "objects/mqtt_server_object.h"

namespace lwm2m {
namespace objects {

// ============================================================================
// Constructor / Destructor
// ============================================================================

MqttServerObject::MqttServerObject(uint16_t instanceId)
    : instanceId_(instanceId) {
}

MqttServerObject::~MqttServerObject() = default;

// ============================================================================
// Initialization
// ============================================================================

bool MqttServerObject::initialize() {
    // Set default values
    config_.shortServerId = instanceId_;
    config_.brokerPort = 8883;
    config_.cleanSession = true;
    config_.keepAlive = 60;
    config_.qos = MqttQosLevel::AT_LEAST_ONCE;
    config_.protocolVersion = MqttProtocol::MQTT_3_1_1;
    config_.topicPrefix = "lwm2m";
    config_.retain = false;
    config_.sessionExpiry = 3600;
    config_.maxPacketSize = 268435456;
    config_.receiveMaximum = 65535;

    return true;
}

// ============================================================================
// Resource Read Operations
// ============================================================================

bool MqttServerObject::read(MqttServerResourceId resourceId, std::string& value) const {
    switch (resourceId) {
        case MqttServerResourceId::BROKER_URI:
            value = config_.brokerUri;
            return true;
        case MqttServerResourceId::CLIENT_ID:
            value = config_.clientId;
            return true;
        case MqttServerResourceId::USERNAME:
            value = config_.username;
            return true;
        case MqttServerResourceId::TOPIC_PREFIX:
            value = config_.topicPrefix;
            return true;
        default:
            return false;
    }
}

bool MqttServerObject::read(MqttServerResourceId resourceId, int64_t& value) const {
    switch (resourceId) {
        case MqttServerResourceId::SHORT_SERVER_ID:
            value = config_.shortServerId;
            return true;
        case MqttServerResourceId::BROKER_PORT:
            value = config_.brokerPort;
            return true;
        case MqttServerResourceId::KEEP_ALIVE:
            value = config_.keepAlive;
            return true;
        case MqttServerResourceId::QOS:
            value = static_cast<int64_t>(config_.qos);
            return true;
        case MqttServerResourceId::PROTOCOL_VERSION:
            value = static_cast<int64_t>(config_.protocolVersion);
            return true;
        case MqttServerResourceId::SESSION_EXPIRY:
            value = config_.sessionExpiry;
            return true;
        case MqttServerResourceId::MAX_PACKET_SIZE:
            value = config_.maxPacketSize;
            return true;
        case MqttServerResourceId::RECEIVE_MAXIMUM:
            value = config_.receiveMaximum;
            return true;
        default:
            return false;
    }
}

bool MqttServerObject::read(MqttServerResourceId resourceId, bool& value) const {
    switch (resourceId) {
        case MqttServerResourceId::CLEAN_SESSION:
            value = config_.cleanSession;
            return true;
        case MqttServerResourceId::RETAIN:
            value = config_.retain;
            return true;
        default:
            return false;
    }
}

// ============================================================================
// Resource Write Operations
// ============================================================================

bool MqttServerObject::write(MqttServerResourceId resourceId, const std::string& value) {
    if (!isWritable(resourceId)) {
        return false;
    }

    switch (resourceId) {
        case MqttServerResourceId::BROKER_URI:
            config_.brokerUri = value;
            break;
        case MqttServerResourceId::CLIENT_ID:
            config_.clientId = value;
            break;
        case MqttServerResourceId::USERNAME:
            config_.username = value;
            break;
        case MqttServerResourceId::PASSWORD:
            config_.password = value;
            break;
        case MqttServerResourceId::TOPIC_PREFIX:
            config_.topicPrefix = value;
            break;
        default:
            return false;
    }

    notifyChange(resourceId);
    return true;
}

bool MqttServerObject::write(MqttServerResourceId resourceId, int64_t value) {
    if (!isWritable(resourceId)) {
        return false;
    }

    switch (resourceId) {
        case MqttServerResourceId::BROKER_PORT:
            if (value < 0 || value > 65535) return false;
            config_.brokerPort = static_cast<uint16_t>(value);
            break;
        case MqttServerResourceId::KEEP_ALIVE:
            if (value < 0 || value > 65535) return false;
            config_.keepAlive = static_cast<uint16_t>(value);
            break;
        case MqttServerResourceId::QOS:
            if (value < 0 || value > 2) return false;
            config_.qos = static_cast<MqttQosLevel>(value);
            break;
        case MqttServerResourceId::PROTOCOL_VERSION:
            if (value != 4 && value != 5) return false;
            config_.protocolVersion = static_cast<MqttProtocol>(value);
            break;
        case MqttServerResourceId::SESSION_EXPIRY:
            if (value < 0) return false;
            config_.sessionExpiry = static_cast<uint32_t>(value);
            break;
        case MqttServerResourceId::MAX_PACKET_SIZE:
            if (value < 0) return false;
            config_.maxPacketSize = static_cast<uint32_t>(value);
            break;
        case MqttServerResourceId::RECEIVE_MAXIMUM:
            if (value < 0 || value > 65535) return false;
            config_.receiveMaximum = static_cast<uint16_t>(value);
            break;
        default:
            return false;
    }

    notifyChange(resourceId);
    return true;
}

bool MqttServerObject::write(MqttServerResourceId resourceId, bool value) {
    if (!isWritable(resourceId)) {
        return false;
    }

    switch (resourceId) {
        case MqttServerResourceId::CLEAN_SESSION:
            config_.cleanSession = value;
            break;
        case MqttServerResourceId::RETAIN:
            config_.retain = value;
            break;
        default:
            return false;
    }

    notifyChange(resourceId);
    return true;
}

// ============================================================================
// Configuration Access
// ============================================================================

MqttServerConfig MqttServerObject::getConfig() const {
    return config_;
}

void MqttServerObject::setConfig(const MqttServerConfig& config) {
    config_ = config;
}

// ============================================================================
// Callbacks
// ============================================================================

void MqttServerObject::setConfigChangeCallback(ConfigChangeCallback callback) {
    configChangeCallback_ = std::move(callback);
}

void MqttServerObject::notifyChange(MqttServerResourceId resourceId) {
    if (configChangeCallback_) {
        configChangeCallback_(instanceId_, resourceId);
    }
}

// ============================================================================
// Resource Metadata
// ============================================================================

bool MqttServerObject::isReadable(MqttServerResourceId resourceId) {
    // All resources except PASSWORD are readable
    return resourceId != MqttServerResourceId::PASSWORD;
}

bool MqttServerObject::isWritable(MqttServerResourceId resourceId) {
    // All resources except SHORT_SERVER_ID are writable
    return resourceId != MqttServerResourceId::SHORT_SERVER_ID;
}

bool MqttServerObject::isMandatory(MqttServerResourceId resourceId) {
    switch (resourceId) {
        case MqttServerResourceId::SHORT_SERVER_ID:
        case MqttServerResourceId::BROKER_URI:
        case MqttServerResourceId::BROKER_PORT:
        case MqttServerResourceId::CLIENT_ID:
            return true;
        default:
            return false;
    }
}

// ============================================================================
// Factory
// ============================================================================

std::unique_ptr<MqttServerObject> MqttServerObjectFactory::create(uint16_t instanceId) {
    auto obj = std::make_unique<MqttServerObject>(instanceId);
    obj->initialize();
    return obj;
}

std::unique_ptr<MqttServerObject> MqttServerObjectFactory::create(const MqttServerConfig& config,
                                                                   uint16_t instanceId) {
    auto obj = std::make_unique<MqttServerObject>(instanceId);
    obj->initialize();
    obj->setConfig(config);
    return obj;
}

} // namespace objects
} // namespace lwm2m
