/**
 * @file mqtt_server_object.h
 * @brief OMA LwM2M MQTT Server Object (ID: 24)
 *
 * Defines the MQTT Server configuration object for LwM2M MQTT transport binding.
 * This object stores MQTT broker connection parameters and credentials.
 *
 * Object ID: 24
 * URN: urn:oma:lwm2m:oma:24
 * Multiple Instances: Yes
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_MQTT_SERVER_OBJECT_H
#define FRIENDLY_LWM2M_MQTT_SERVER_OBJECT_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace lwm2m {
namespace objects {

/**
 * @brief MQTT Server Object ID
 */
constexpr uint16_t MQTT_SERVER_OBJECT_ID = 24;

/**
 * @brief Resource IDs for MQTT Server Object (ID: 24)
 */
enum class MqttServerResourceId : uint16_t {
    SHORT_SERVER_ID = 0,        ///< Short Server ID (Integer, R, Mandatory)
    BROKER_URI = 1,             ///< MQTT Broker URI (String, RW, Mandatory)
    BROKER_PORT = 2,            ///< MQTT Broker Port (Integer, RW, Mandatory)
    CLIENT_ID = 3,              ///< MQTT Client Identifier (String, RW, Mandatory)
    CLEAN_SESSION = 4,          ///< Clean Session flag (Boolean, RW, Optional)
    KEEP_ALIVE = 5,             ///< Keep-Alive interval in seconds (Integer, RW, Optional)
    QOS = 6,                    ///< Default QoS level 0-2 (Integer, RW, Optional)
    PROTOCOL_VERSION = 7,       ///< MQTT Protocol Version 4=3.1.1, 5=5.0 (Integer, RW, Optional)
    USERNAME = 8,               ///< Authentication Username (String, RW, Optional)
    PASSWORD = 9,               ///< Authentication Password (String, W, Optional)
    TOPIC_PREFIX = 10,          ///< Topic Prefix for LwM2M (String, RW, Optional)
    RETAIN = 11,                ///< Default Retain flag (Boolean, RW, Optional)
    SESSION_EXPIRY = 12,        ///< MQTT 5.0 Session Expiry (Integer, RW, Optional)
    MAX_PACKET_SIZE = 13,       ///< Maximum Packet Size (Integer, RW, Optional)
    RECEIVE_MAXIMUM = 14        ///< MQTT 5.0 Receive Maximum (Integer, RW, Optional)
};

/**
 * @brief MQTT Protocol versions
 */
enum class MqttProtocol : uint8_t {
    MQTT_3_1_1 = 4,     ///< MQTT 3.1.1
    MQTT_5_0 = 5        ///< MQTT 5.0
};

/**
 * @brief MQTT QoS levels
 */
enum class MqttQosLevel : uint8_t {
    AT_MOST_ONCE = 0,   ///< QoS 0: Fire and forget
    AT_LEAST_ONCE = 1,  ///< QoS 1: Acknowledged delivery
    EXACTLY_ONCE = 2    ///< QoS 2: Assured delivery
};

/**
 * @brief MQTT Server configuration structure
 */
struct MqttServerConfig {
    uint16_t shortServerId = 0;
    std::string brokerUri;
    uint16_t brokerPort = 8883;
    std::string clientId;
    bool cleanSession = true;
    uint16_t keepAlive = 60;
    MqttQosLevel qos = MqttQosLevel::AT_LEAST_ONCE;
    MqttProtocol protocolVersion = MqttProtocol::MQTT_3_1_1;
    std::string username;
    std::string password;
    std::string topicPrefix = "lwm2m";
    bool retain = false;
    uint32_t sessionExpiry = 3600;
    uint32_t maxPacketSize = 268435456;
    uint16_t receiveMaximum = 65535;
};

/**
 * @brief Callback for configuration changes
 */
using ConfigChangeCallback = std::function<void(uint16_t instanceId,
                                                 MqttServerResourceId resourceId)>;

/**
 * @brief MQTT Server Object Implementation
 *
 * Manages MQTT broker connection configurations for LwM2M transport.
 */
class MqttServerObject {
public:
    /**
     * @brief Constructor
     * @param instanceId Object instance ID
     */
    explicit MqttServerObject(uint16_t instanceId = 0);

    /**
     * @brief Destructor
     */
    ~MqttServerObject();

    // Prevent copying
    MqttServerObject(const MqttServerObject&) = delete;
    MqttServerObject& operator=(const MqttServerObject&) = delete;

    /**
     * @brief Initialize the object
     * @return true on success
     */
    bool initialize();

    // ========================================================================
    // Resource Read Operations
    // ========================================================================

    /**
     * @brief Read string resource
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(MqttServerResourceId resourceId, std::string& value) const;

    /**
     * @brief Read integer resource
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(MqttServerResourceId resourceId, int64_t& value) const;

    /**
     * @brief Read boolean resource
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(MqttServerResourceId resourceId, bool& value) const;

    // ========================================================================
    // Resource Write Operations
    // ========================================================================

    /**
     * @brief Write string resource
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(MqttServerResourceId resourceId, const std::string& value);

    /**
     * @brief Write integer resource
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(MqttServerResourceId resourceId, int64_t value);

    /**
     * @brief Write boolean resource
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(MqttServerResourceId resourceId, bool value);

    // ========================================================================
    // Configuration Access
    // ========================================================================

    /**
     * @brief Get full configuration
     * @return Server configuration
     */
    MqttServerConfig getConfig() const;

    /**
     * @brief Set full configuration
     * @param config Configuration to set
     */
    void setConfig(const MqttServerConfig& config);

    /**
     * @brief Get Short Server ID
     * @return Server ID
     */
    uint16_t getShortServerId() const { return config_.shortServerId; }

    /**
     * @brief Get Broker URI
     * @return URI string
     */
    std::string getBrokerUri() const { return config_.brokerUri; }

    /**
     * @brief Get Broker Port
     * @return Port number
     */
    uint16_t getBrokerPort() const { return config_.brokerPort; }

    /**
     * @brief Get Client ID
     * @return Client identifier
     */
    std::string getClientId() const { return config_.clientId; }

    /**
     * @brief Get QoS level
     * @return QoS level
     */
    MqttQosLevel getQoS() const { return config_.qos; }

    /**
     * @brief Get Topic Prefix
     * @return Topic prefix
     */
    std::string getTopicPrefix() const { return config_.topicPrefix; }

    /**
     * @brief Get Protocol Version
     * @return Protocol version
     */
    MqttProtocol getProtocolVersion() const { return config_.protocolVersion; }

    // ========================================================================
    // Callbacks
    // ========================================================================

    /**
     * @brief Set configuration change callback
     * @param callback Callback function
     */
    void setConfigChangeCallback(ConfigChangeCallback callback);

    // ========================================================================
    // Object Information
    // ========================================================================

    /**
     * @brief Get object ID
     * @return Object ID (24)
     */
    static uint16_t getObjectId() { return MQTT_SERVER_OBJECT_ID; }

    /**
     * @brief Get instance ID
     * @return Instance ID
     */
    uint16_t getInstanceId() const { return instanceId_; }

    /**
     * @brief Check if resource is readable
     * @param resourceId Resource ID
     * @return true if readable
     */
    static bool isReadable(MqttServerResourceId resourceId);

    /**
     * @brief Check if resource is writable
     * @param resourceId Resource ID
     * @return true if writable
     */
    static bool isWritable(MqttServerResourceId resourceId);

    /**
     * @brief Check if resource is mandatory
     * @param resourceId Resource ID
     * @return true if mandatory
     */
    static bool isMandatory(MqttServerResourceId resourceId);

private:
    uint16_t instanceId_;
    MqttServerConfig config_;
    ConfigChangeCallback configChangeCallback_;

    void notifyChange(MqttServerResourceId resourceId);
};

/**
 * @brief Factory for creating MQTT Server Object instances
 */
class MqttServerObjectFactory {
public:
    /**
     * @brief Create new instance
     * @param instanceId Instance ID
     * @return Object instance
     */
    static std::unique_ptr<MqttServerObject> create(uint16_t instanceId = 0);

    /**
     * @brief Create from configuration
     * @param config Configuration
     * @param instanceId Instance ID
     * @return Object instance
     */
    static std::unique_ptr<MqttServerObject> create(const MqttServerConfig& config,
                                                     uint16_t instanceId = 0);
};

} // namespace objects
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_MQTT_SERVER_OBJECT_H
