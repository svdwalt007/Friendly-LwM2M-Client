/**
 * @file mqtt_transport.h
 * @brief MQTT Transport Binding for LwM2M (OMA Section 8)
 *
 * Implements MQTT 3.1.1 and MQTT 5.0 transport bindings for LwM2M protocol
 * as specified in OMA-TS-LightweightM2M_Transport-V1_2_2.
 *
 * Features:
 * - MQTT 3.1.1 and 5.0 protocol support
 * - QoS 0, 1, and 2 message delivery
 * - TLS/SSL encryption support
 * - LwM2M topic structure (OMA Section 8.2)
 * - CBOR message encoding (OMA Section 8.7)
 * - Automatic reconnection with exponential backoff
 * - Last Will and Testament (LWT) support
 * - Session persistence
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_MQTT_TRANSPORT_H
#define FRIENDLY_LWM2M_MQTT_TRANSPORT_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <mutex>
#include <atomic>
#include <queue>
#include <chrono>

namespace lwm2m {
namespace transport {

/**
 * @brief MQTT Protocol Version
 */
enum class MqttProtocolVersion : uint8_t {
    MQTT_3_1_1 = 4,     ///< MQTT 3.1.1 (protocol level 4)
    MQTT_5_0 = 5        ///< MQTT 5.0 (protocol level 5)
};

/**
 * @brief MQTT Quality of Service levels
 */
enum class MqttQoS : uint8_t {
    AT_MOST_ONCE = 0,   ///< QoS 0: Fire and forget
    AT_LEAST_ONCE = 1,  ///< QoS 1: Acknowledged delivery
    EXACTLY_ONCE = 2    ///< QoS 2: Assured delivery
};

/**
 * @brief MQTT Connection state
 */
enum class MqttConnectionState : uint8_t {
    DISCONNECTED = 0,
    CONNECTING = 1,
    CONNECTED = 2,
    DISCONNECTING = 3,
    RECONNECTING = 4,
    ERROR = 5
};

/**
 * @brief MQTT Transport result codes
 */
enum class MqttResult {
    SUCCESS = 0,
    ERROR_NOT_CONNECTED,
    ERROR_INVALID_PARAMETER,
    ERROR_TIMEOUT,
    ERROR_PROTOCOL_ERROR,
    ERROR_NETWORK_ERROR,
    ERROR_TLS_ERROR,
    ERROR_AUTHENTICATION_FAILED,
    ERROR_NOT_AUTHORIZED,
    ERROR_BROKER_UNAVAILABLE,
    ERROR_QUOTA_EXCEEDED,
    ERROR_PAYLOAD_TOO_LARGE,
    ERROR_TOPIC_INVALID,
    ERROR_QOS_NOT_SUPPORTED,
    ERROR_INTERNAL
};

/**
 * @brief LwM2M Operation codes for MQTT messages (OMA Section 8.7)
 */
enum class LwM2MOperation : uint8_t {
    // Server -> Client operations
    READ = 1,
    WRITE = 2,
    EXECUTE = 3,
    CREATE = 4,
    DELETE = 5,
    DISCOVER = 6,
    WRITE_ATTRIBUTES = 7,
    OBSERVE = 8,
    CANCEL_OBSERVE = 9,
    READ_COMPOSITE = 10,
    WRITE_COMPOSITE = 11,
    OBSERVE_COMPOSITE = 12,

    // Client -> Server operations
    REGISTER = 20,
    UPDATE = 21,
    DEREGISTER = 22,
    SEND = 23,
    NOTIFY = 24,

    // Bootstrap operations
    BOOTSTRAP_REQUEST = 30,
    BOOTSTRAP_WRITE = 31,
    BOOTSTRAP_DISCOVER = 32,
    BOOTSTRAP_DELETE = 33,
    BOOTSTRAP_FINISH = 34
};

/**
 * @brief MQTT message direction
 */
enum class MessageDirection : uint8_t {
    RD,     ///< Registration Directory
    BS,     ///< Bootstrap
    DP      ///< Data/Push
};

/**
 * @brief TLS/SSL Configuration
 */
struct TlsConfig {
    bool enabled = false;
    std::string caCertPath;
    std::string clientCertPath;
    std::string clientKeyPath;
    std::string pskIdentity;
    std::vector<uint8_t> pskKey;
    bool verifyPeer = true;
    std::string sni;                    ///< Server Name Indication
    std::string alpn;                   ///< Application Layer Protocol Negotiation
    std::vector<std::string> cipherSuites;
};

/**
 * @brief MQTT 5.0 specific configuration
 */
struct Mqtt5Config {
    uint32_t sessionExpiry = 3600;      ///< Session expiry interval (seconds)
    uint32_t messageExpiry = 0;         ///< Message expiry interval (seconds, 0=never)
    uint16_t receiveMaximum = 65535;    ///< Receive maximum
    uint32_t maxPacketSize = 268435456; ///< Maximum packet size (256MB default)
    uint16_t topicAliasMax = 0;         ///< Topic alias maximum
    bool requestResponseInfo = false;
    bool requestProblemInfo = true;
    std::map<std::string, std::string> userProperties;
};

/**
 * @brief MQTT Transport Configuration
 */
struct MqttConfig {
    // Connection settings
    std::string brokerUri;              ///< Broker URI (e.g., "mqtt://broker:1883")
    uint16_t brokerPort = 1883;         ///< Broker port (1883 or 8883 for TLS)
    std::string clientId;               ///< MQTT client identifier
    std::string username;               ///< Authentication username
    std::string password;               ///< Authentication password

    // Protocol settings
    MqttProtocolVersion protocolVersion = MqttProtocolVersion::MQTT_3_1_1;
    bool cleanSession = true;           ///< Clean session flag
    uint16_t keepAlive = 60;            ///< Keep-alive interval (seconds)
    MqttQoS defaultQoS = MqttQoS::AT_LEAST_ONCE;
    bool retainMessages = false;

    // LwM2M settings
    std::string topicPrefix = "lwm2m";  ///< Topic prefix
    std::string endpointName;           ///< LwM2M endpoint name

    // TLS settings
    TlsConfig tls;

    // MQTT 5.0 settings
    Mqtt5Config mqtt5;

    // Connection management
    bool autoReconnect = true;
    uint32_t reconnectMinDelay = 1000;  ///< Minimum reconnect delay (ms)
    uint32_t reconnectMaxDelay = 60000; ///< Maximum reconnect delay (ms)
    uint32_t connectTimeout = 30000;    ///< Connection timeout (ms)

    // Last Will and Testament
    bool lwtEnabled = false;
    std::string lwtTopic;
    std::vector<uint8_t> lwtPayload;
    MqttQoS lwtQoS = MqttQoS::AT_LEAST_ONCE;
    bool lwtRetain = false;
};

/**
 * @brief MQTT Message structure
 */
struct MqttMessage {
    std::string topic;
    std::vector<uint8_t> payload;
    MqttQoS qos = MqttQoS::AT_LEAST_ONCE;
    bool retain = false;
    uint16_t packetId = 0;

    // MQTT 5.0 properties
    uint32_t messageExpiry = 0;
    std::string responseTopic;
    std::vector<uint8_t> correlationData;
    std::string contentType;
    std::map<std::string, std::string> userProperties;
};

/**
 * @brief LwM2M CBOR Message structure (OMA Section 8.7)
 */
struct LwM2MMessage {
    LwM2MOperation operation;
    uint64_t token = 0;
    std::string path;
    uint16_t contentFormat = 0;
    std::vector<uint8_t> payload;
    uint8_t responseCode = 0;
    std::string errorMessage;

    // Registration parameters
    std::string endpointName;
    uint32_t lifetime = 86400;
    std::string lwm2mVersion = "1.2";
    std::string binding = "U";
    bool queueMode = false;
    std::vector<std::string> objectLinks;
    std::string altPath;

    // Notification parameters
    uint32_t observeSequence = 0;
    uint64_t timestamp = 0;
};

/**
 * @brief Callback for incoming messages
 */
using MessageCallback = std::function<void(const MqttMessage& message)>;

/**
 * @brief Callback for LwM2M operations
 */
using LwM2MCallback = std::function<void(const LwM2MMessage& message)>;

/**
 * @brief Callback for connection state changes
 */
using ConnectionCallback = std::function<void(MqttConnectionState state,
                                               MqttResult reason)>;

/**
 * @brief Callback for publish acknowledgment
 */
using PublishCallback = std::function<void(uint16_t packetId, MqttResult result)>;

/**
 * @brief MQTT Transport Implementation
 *
 * Provides MQTT transport binding for LwM2M protocol operations.
 */
class MqttTransport {
public:
    /**
     * @brief Constructor
     * @param config Transport configuration
     */
    explicit MqttTransport(const MqttConfig& config);

    /**
     * @brief Destructor
     */
    ~MqttTransport();

    // Prevent copying
    MqttTransport(const MqttTransport&) = delete;
    MqttTransport& operator=(const MqttTransport&) = delete;

    // ========================================================================
    // Connection Management
    // ========================================================================

    /**
     * @brief Connect to MQTT broker
     * @return Result code
     */
    MqttResult connect();

    /**
     * @brief Disconnect from MQTT broker
     * @param sendWill Send Last Will message
     * @return Result code
     */
    MqttResult disconnect(bool sendWill = false);

    /**
     * @brief Check if connected
     * @return true if connected
     */
    bool isConnected() const;

    /**
     * @brief Get current connection state
     * @return Connection state
     */
    MqttConnectionState getState() const;

    /**
     * @brief Process MQTT events (call periodically)
     * @param timeoutMs Timeout in milliseconds
     * @return Result code
     */
    MqttResult process(uint32_t timeoutMs = 100);

    // ========================================================================
    // Topic Management (OMA Section 8.2)
    // ========================================================================

    /**
     * @brief Build LwM2M topic string
     * @param direction Message direction
     * @param operation Optional operation suffix
     * @return Full topic string
     */
    std::string buildTopic(MessageDirection direction,
                          const std::string& operation = "") const;

    /**
     * @brief Subscribe to LwM2M topics
     * @return Result code
     */
    MqttResult subscribeLwM2MTopics();

    /**
     * @brief Subscribe to specific topic
     * @param topic Topic pattern
     * @param qos QoS level
     * @return Result code
     */
    MqttResult subscribe(const std::string& topic, MqttQoS qos = MqttQoS::AT_LEAST_ONCE);

    /**
     * @brief Unsubscribe from topic
     * @param topic Topic pattern
     * @return Result code
     */
    MqttResult unsubscribe(const std::string& topic);

    // ========================================================================
    // LwM2M Operations
    // ========================================================================

    /**
     * @brief Send LwM2M Registration
     * @param message Registration message
     * @return Result code
     */
    MqttResult sendRegistration(const LwM2MMessage& message);

    /**
     * @brief Send Registration Update
     * @param message Update message
     * @return Result code
     */
    MqttResult sendUpdate(const LwM2MMessage& message);

    /**
     * @brief Send Deregistration
     * @return Result code
     */
    MqttResult sendDeregistration();

    /**
     * @brief Send Bootstrap Request
     * @param message Bootstrap message
     * @return Result code
     */
    MqttResult sendBootstrapRequest(const LwM2MMessage& message);

    /**
     * @brief Send Notification
     * @param message Notification message
     * @return Result code
     */
    MqttResult sendNotification(const LwM2MMessage& message);

    /**
     * @brief Send Data (Send operation)
     * @param message Data message
     * @return Result code
     */
    MqttResult sendData(const LwM2MMessage& message);

    /**
     * @brief Send Response to server operation
     * @param message Response message
     * @return Result code
     */
    MqttResult sendResponse(const LwM2MMessage& message);

    // ========================================================================
    // Raw MQTT Operations
    // ========================================================================

    /**
     * @brief Publish message to topic
     * @param message Message to publish
     * @param callback Optional callback for acknowledgment
     * @return Result code
     */
    MqttResult publish(const MqttMessage& message,
                       PublishCallback callback = nullptr);

    /**
     * @brief Publish raw data to topic
     * @param topic Topic string
     * @param payload Payload data
     * @param qos QoS level
     * @param retain Retain flag
     * @return Result code
     */
    MqttResult publish(const std::string& topic,
                       const std::vector<uint8_t>& payload,
                       MqttQoS qos = MqttQoS::AT_LEAST_ONCE,
                       bool retain = false);

    // ========================================================================
    // CBOR Encoding/Decoding (OMA Section 8.7)
    // ========================================================================

    /**
     * @brief Encode LwM2M message to CBOR
     * @param message LwM2M message
     * @param output Output buffer
     * @return true on success
     */
    static bool encodeCBOR(const LwM2MMessage& message,
                          std::vector<uint8_t>& output);

    /**
     * @brief Decode CBOR to LwM2M message
     * @param data CBOR data
     * @param message Output message
     * @return true on success
     */
    static bool decodeCBOR(const std::vector<uint8_t>& data,
                          LwM2MMessage& message);

    // ========================================================================
    // Callbacks
    // ========================================================================

    /**
     * @brief Set message callback
     * @param callback Callback function
     */
    void setMessageCallback(MessageCallback callback);

    /**
     * @brief Set LwM2M operation callback
     * @param callback Callback function
     */
    void setLwM2MCallback(LwM2MCallback callback);

    /**
     * @brief Set connection state callback
     * @param callback Callback function
     */
    void setConnectionCallback(ConnectionCallback callback);

    // ========================================================================
    // Configuration
    // ========================================================================

    /**
     * @brief Update configuration
     * @param config New configuration
     */
    void setConfig(const MqttConfig& config);

    /**
     * @brief Get current configuration
     * @return Current configuration
     */
    const MqttConfig& getConfig() const;

    /**
     * @brief Get endpoint name
     * @return Endpoint name
     */
    std::string getEndpointName() const;

    /**
     * @brief Set endpoint name
     * @param name Endpoint name
     */
    void setEndpointName(const std::string& name);

    // ========================================================================
    // Statistics
    // ========================================================================

    /**
     * @brief Get messages sent count
     * @return Messages sent
     */
    uint64_t getMessagesSent() const;

    /**
     * @brief Get messages received count
     * @return Messages received
     */
    uint64_t getMessagesReceived() const;

    /**
     * @brief Get bytes sent
     * @return Bytes sent
     */
    uint64_t getBytesSent() const;

    /**
     * @brief Get bytes received
     * @return Bytes received
     */
    uint64_t getBytesReceived() const;

    /**
     * @brief Reset statistics
     */
    void resetStatistics();

    // ========================================================================
    // QoS Mapping (LwM2M to MQTT)
    // ========================================================================

    /**
     * @brief Get recommended QoS for LwM2M operation
     * @param operation LwM2M operation type
     * @return Recommended QoS level
     */
    static MqttQoS getRecommendedQoS(LwM2MOperation operation);

private:
    // Internal implementation
    struct Impl;
    std::unique_ptr<Impl> impl_;

    // Configuration
    MqttConfig config_;

    // State
    std::atomic<MqttConnectionState> state_{MqttConnectionState::DISCONNECTED};

    // Callbacks
    MessageCallback messageCallback_;
    LwM2MCallback lwm2mCallback_;
    ConnectionCallback connectionCallback_;

    // Statistics
    std::atomic<uint64_t> messagesSent_{0};
    std::atomic<uint64_t> messagesReceived_{0};
    std::atomic<uint64_t> bytesSent_{0};
    std::atomic<uint64_t> bytesReceived_{0};

    // Synchronization
    mutable std::mutex mutex_;

    // Internal methods
    void handleIncomingMessage(const MqttMessage& message);
    void setState(MqttConnectionState newState, MqttResult reason = MqttResult::SUCCESS);
    std::string directionToString(MessageDirection direction) const;
};

/**
 * @brief MQTT Transport Factory
 */
class MqttTransportFactory {
public:
    /**
     * @brief Create MQTT transport from configuration
     * @param config Configuration
     * @return Transport instance
     */
    static std::unique_ptr<MqttTransport> create(const MqttConfig& config);

    /**
     * @brief Create MQTT transport from URI
     * @param brokerUri Broker URI
     * @param clientId Client identifier
     * @param endpointName LwM2M endpoint name
     * @return Transport instance
     */
    static std::unique_ptr<MqttTransport> create(const std::string& brokerUri,
                                                  const std::string& clientId,
                                                  const std::string& endpointName);

    /**
     * @brief Check if MQTT support is available
     * @return true if MQTT library is available
     */
    static bool isAvailable();

    /**
     * @brief Get supported MQTT versions
     * @return List of supported versions
     */
    static std::vector<MqttProtocolVersion> getSupportedVersions();
};

} // namespace transport
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_MQTT_TRANSPORT_H
