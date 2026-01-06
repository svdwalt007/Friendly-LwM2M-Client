/**
 * @file mqtt_transport.h
 * @brief LwM2M MQTT Transport Binding (OMA LwM2M v1.2.1/v1.2.2)
 * 
 * Implements Section 8 of OMA-TS-LightweightM2M_Transport-V1_2_1 specification.
 * Provides MQTT as an alternative transport binding for LwM2M messaging.
 * 
 * Features:
 * - MQTT 3.1.1 and MQTT 5.0 support
 * - TLS/SSL security with PSK, Certificate, and token-based auth
 * - Topic-based request/response model
 * - QoS mapping for reliability
 * - Automatic reconnection and session persistence
 * - LwM2M COSE Object (23) and MQTT Server Object (24) integration
 * 
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_MQTT_TRANSPORT_H
#define FRIENDLY_LWM2M_MQTT_TRANSPORT_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>
#include <chrono>
#include <optional>
#include <queue>

namespace lwm2m {
namespace transport {

// Forward declarations
class MqttClient;
class MqttMessage;

// ============================================================================
// Constants and Enumerations
// ============================================================================

/**
 * @brief MQTT Protocol versions supported
 */
enum class MqttVersion : uint8_t {
    MQTT_3_1_1 = 4,   // MQTT v3.1.1
    MQTT_5_0 = 5      // MQTT v5.0
};

/**
 * @brief MQTT Quality of Service levels
 */
enum class MqttQoS : uint8_t {
    AT_MOST_ONCE = 0,   // Fire and forget
    AT_LEAST_ONCE = 1,  // Acknowledged delivery
    EXACTLY_ONCE = 2    // Assured delivery
};

/**
 * @brief MQTT connection states
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
 * @brief MQTT security modes per LwM2M specification
 */
enum class MqttSecurityMode : uint8_t {
    NO_SEC = 0,           // No security (development only)
    PSK = 1,              // Pre-Shared Key
    RPK = 2,              // Raw Public Key
    CERTIFICATE = 3,      // X.509 Certificate
    TOKEN = 4             // Token-based (OAuth2/JWT)
};

/**
 * @brief LwM2M operation types for MQTT
 */
enum class LwM2MOperation : uint8_t {
    // Bootstrap Interface
    BOOTSTRAP_REQUEST = 0,
    BOOTSTRAP_WRITE = 1,
    BOOTSTRAP_READ = 2,
    BOOTSTRAP_DISCOVER = 3,
    BOOTSTRAP_DELETE = 4,
    BOOTSTRAP_FINISH = 5,
    BOOTSTRAP_PACK = 6,
    
    // Registration Interface
    REGISTER = 10,
    UPDATE = 11,
    DEREGISTER = 12,
    
    // Device Management Interface
    READ = 20,
    WRITE = 21,
    EXECUTE = 22,
    CREATE = 23,
    DELETE = 24,
    DISCOVER = 25,
    WRITE_ATTRIBUTES = 26,
    READ_COMPOSITE = 27,
    WRITE_COMPOSITE = 28,
    
    // Information Reporting Interface
    OBSERVE = 30,
    CANCEL_OBSERVE = 31,
    OBSERVE_COMPOSITE = 32,
    CANCEL_OBSERVE_COMPOSITE = 33,
    NOTIFY = 34,
    SEND = 35
};

/**
 * @brief MQTT transport result codes
 */
enum class MqttResult : int {
    SUCCESS = 0,
    ERROR_NOT_CONNECTED = -1,
    ERROR_TIMEOUT = -2,
    ERROR_PROTOCOL = -3,
    ERROR_AUTHENTICATION = -4,
    ERROR_AUTHORIZATION = -5,
    ERROR_BAD_REQUEST = -6,
    ERROR_NOT_FOUND = -7,
    ERROR_METHOD_NOT_ALLOWED = -8,
    ERROR_INTERNAL = -9,
    ERROR_SERVICE_UNAVAILABLE = -10,
    ERROR_NETWORK = -11,
    ERROR_TLS = -12,
    ERROR_INVALID_TOPIC = -13,
    ERROR_PAYLOAD_TOO_LARGE = -14,
    ERROR_QUEUE_FULL = -15
};

/**
 * @brief LwM2M response result codes for MQTT (Table 8.5)
 */
enum class LwM2MResponseCode : uint8_t {
    // Success
    CHANGED = 0,          // 2.04
    CONTENT = 1,          // 2.05
    CREATED = 2,          // 2.01
    DELETED = 3,          // 2.02
    
    // Client errors
    BAD_REQUEST = 10,     // 4.00
    UNAUTHORIZED = 11,    // 4.01
    FORBIDDEN = 12,       // 4.03
    NOT_FOUND = 13,       // 4.04
    METHOD_NOT_ALLOWED = 14,  // 4.05
    NOT_ACCEPTABLE = 15,  // 4.06
    CONFLICT = 16,        // 4.09
    PRECONDITION_FAILED = 17, // 4.12
    ENTITY_TOO_LARGE = 18,    // 4.13
    UNSUPPORTED_FORMAT = 19,  // 4.15
    
    // Server errors
    INTERNAL_ERROR = 20,  // 5.00
    NOT_IMPLEMENTED = 21, // 5.01
    SERVICE_UNAVAILABLE = 22, // 5.03
    GATEWAY_TIMEOUT = 23  // 5.04
};

// ============================================================================
// Data Structures
// ============================================================================

/**
 * @brief MQTT Server configuration (LwM2M MQTT Server Object 24)
 */
struct MqttServerConfig {
    uint16_t shortServerId = 0;          // Resource 0: Short Server ID
    std::string brokerUri;                // Resource 1: MQTT Broker URI
    uint16_t brokerPort = 8883;           // Resource 2: MQTT Broker Port
    std::string clientId;                 // Resource 3: Client Identifier
    bool cleanSession = true;             // Resource 4: Clean Session
    uint16_t keepAlive = 60;              // Resource 5: Keep Alive (seconds)
    MqttQoS defaultQoS = MqttQoS::AT_LEAST_ONCE;  // Resource 6: QoS
    MqttVersion version = MqttVersion::MQTT_3_1_1; // Resource 7: Protocol Version
    std::string username;                 // Resource 8: Username
    std::string password;                 // Resource 9: Password
    std::string topicPrefix;              // Resource 10: Topic Prefix
    bool retainMessages = false;          // Resource 11: Retain Flag
    uint32_t sessionExpiry = 0;           // Resource 12: Session Expiry (MQTT 5.0)
    uint16_t receiveMaximum = 65535;      // Resource 13: Receive Maximum (MQTT 5.0)
    uint32_t maxPacketSize = 0;           // Resource 14: Maximum Packet Size
};

/**
 * @brief MQTT COSE Security configuration (LwM2M COSE Object 23)
 */
struct MqttCoseConfig {
    uint16_t shortServerId = 0;           // Resource 0: Short Server ID
    MqttSecurityMode securityMode = MqttSecurityMode::CERTIFICATE;  // Resource 1
    std::vector<uint8_t> publicKey;       // Resource 2: Public Key or Identity
    std::vector<uint8_t> privateKey;      // Resource 3: Secret Key
    std::vector<uint8_t> serverPublicKey; // Resource 4: Server Public Key
    std::string certificateChain;         // Resource 5: Certificate Chain (PEM)
    std::string caCertificate;            // Resource 6: CA Certificate (PEM)
    std::string sni;                      // Resource 7: Server Name Indication
    bool verifyPeer = true;               // Resource 8: Verify Peer Certificate
};

/**
 * @brief MQTT message payload structure per LwM2M specification
 */
struct LwM2MPayload {
    LwM2MOperation operation;             // Operation type
    uint32_t token = 0;                   // Request/response correlation token
    std::string path;                     // Object/Instance/Resource path
    uint16_t contentFormat = 0;           // Content format (SenML JSON, CBOR, etc.)
    std::vector<uint8_t> data;            // Payload data
    std::map<std::string, std::string> options;  // Additional options
    
    // Response-specific fields
    std::optional<LwM2MResponseCode> responseCode;
    std::optional<std::string> errorMessage;
};

/**
 * @brief MQTT topic structure per LwM2M specification Section 8.2
 */
struct MqttTopic {
    std::string prefix;         // Topic prefix (configurable)
    std::string direction;      // "rd" (registration), "bs" (bootstrap), "dp" (data)
    std::string endpointName;   // Client endpoint name
    std::string operation;      // Operation-specific suffix
    
    /**
     * @brief Build the full topic string
     */
    std::string toString() const {
        std::string topic = prefix;
        if (!topic.empty() && topic.back() != '/') topic += "/";
        topic += direction + "/" + endpointName;
        if (!operation.empty()) topic += "/" + operation;
        return topic;
    }
    
    /**
     * @brief Parse a topic string
     */
    static std::optional<MqttTopic> parse(const std::string& topicStr,
                                           const std::string& prefix = "lwm2m");
};

/**
 * @brief Registration parameters for MQTT binding
 */
struct MqttRegistrationParams {
    std::string endpointName;             // Endpoint Client Name
    uint32_t lifetime = 86400;            // Registration lifetime (seconds)
    std::string lwm2mVersion = "1.2";     // LwM2M version
    std::string binding = "M";            // Binding mode ("M" for MQTT)
    bool queueMode = false;               // Queue mode enabled
    std::vector<std::string> objectLinks; // Supported objects
    std::string alternatePath;            // Alternate path prefix
};

// ============================================================================
// Callback Types
// ============================================================================

/**
 * @brief Callback for incoming LwM2M requests
 */
using RequestCallback = std::function<void(const LwM2MPayload& request, 
                                            std::function<void(const LwM2MPayload&)> respond)>;

/**
 * @brief Callback for connection state changes
 */
using ConnectionCallback = std::function<void(MqttConnectionState state, 
                                               const std::string& reason)>;

/**
 * @brief Callback for publish confirmations
 */
using PublishCallback = std::function<void(uint32_t token, MqttResult result)>;

/**
 * @brief Callback for notification delivery
 */
using NotificationCallback = std::function<void(const std::string& path,
                                                 const std::vector<uint8_t>& data,
                                                 uint16_t contentFormat)>;

// ============================================================================
// MQTT Transport Class
// ============================================================================

/**
 * @brief LwM2M MQTT Transport Binding Implementation
 * 
 * Implements the MQTT transport binding as specified in
 * OMA-TS-LightweightM2M_Transport-V1_2_1 Section 8.
 */
class MqttTransport {
public:
    /**
     * @brief Configuration structure
     */
    struct Config {
        MqttServerConfig server;
        MqttCoseConfig security;
        
        // Connection settings
        uint32_t connectTimeout = 30;      // Connection timeout (seconds)
        uint32_t responseTimeout = 60;     // Response timeout (seconds)
        uint32_t reconnectMinDelay = 1;    // Minimum reconnect delay (seconds)
        uint32_t reconnectMaxDelay = 300;  // Maximum reconnect delay (seconds)
        bool autoReconnect = true;         // Enable automatic reconnection
        
        // Message settings
        size_t maxQueueSize = 1000;        // Maximum queued messages
        size_t maxPayloadSize = 65536;     // Maximum payload size
        bool enableCbor = true;            // Use CBOR encoding (vs JSON)
    };

    /**
     * @brief Constructor
     * @param config Configuration settings
     */
    explicit MqttTransport(const Config& config);

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
     * @param graceful Send disconnect packet
     * @return Result code
     */
    MqttResult disconnect(bool graceful = true);

    /**
     * @brief Check if connected
     * @return true if connected
     */
    bool isConnected() const;

    /**
     * @brief Get current connection state
     * @return Connection state
     */
    MqttConnectionState getConnectionState() const;

    /**
     * @brief Set connection state callback
     * @param callback Callback function
     */
    void setConnectionCallback(ConnectionCallback callback);

    // ========================================================================
    // Bootstrap Interface (Section 8.3.1)
    // ========================================================================

    /**
     * @brief Send Bootstrap Request
     * @param endpointName Client endpoint name
     * @param preferredFormat Preferred content format
     * @return Token for request/response correlation
     */
    uint32_t bootstrapRequest(const std::string& endpointName,
                               uint16_t preferredFormat = 60);

    /**
     * @brief Handle incoming Bootstrap Write
     * @param callback Callback for bootstrap data
     */
    void onBootstrapWrite(RequestCallback callback);

    /**
     * @brief Handle incoming Bootstrap Discover
     * @param callback Callback to provide object list
     */
    void onBootstrapDiscover(RequestCallback callback);

    /**
     * @brief Handle Bootstrap Finish
     * @param callback Callback when bootstrap completes
     */
    void onBootstrapFinish(std::function<void()> callback);

    // ========================================================================
    // Registration Interface (Section 8.3.2)
    // ========================================================================

    /**
     * @brief Register with LwM2M Server
     * @param params Registration parameters
     * @return Token for request/response correlation
     */
    uint32_t registerClient(const MqttRegistrationParams& params);

    /**
     * @brief Update registration
     * @param lifetime New lifetime (0 = no change)
     * @param binding New binding (empty = no change)
     * @param objectLinks Updated object list (empty = no change)
     * @return Token for request/response correlation
     */
    uint32_t updateRegistration(uint32_t lifetime = 0,
                                 const std::string& binding = "",
                                 const std::vector<std::string>& objectLinks = {});

    /**
     * @brief Deregister from server
     * @return Token for request/response correlation
     */
    uint32_t deregister();

    // ========================================================================
    // Device Management Interface (Section 8.3.3)
    // ========================================================================

    /**
     * @brief Handle Read request
     * @param callback Callback to provide resource data
     */
    void onRead(RequestCallback callback);

    /**
     * @brief Handle Write request
     * @param callback Callback to process write
     */
    void onWrite(RequestCallback callback);

    /**
     * @brief Handle Execute request
     * @param callback Callback to execute operation
     */
    void onExecute(RequestCallback callback);

    /**
     * @brief Handle Create request
     * @param callback Callback to create instance
     */
    void onCreate(RequestCallback callback);

    /**
     * @brief Handle Delete request
     * @param callback Callback to delete instance
     */
    void onDelete(RequestCallback callback);

    /**
     * @brief Handle Discover request
     * @param callback Callback to provide discovery info
     */
    void onDiscover(RequestCallback callback);

    /**
     * @brief Handle Write-Attributes request
     * @param callback Callback to set attributes
     */
    void onWriteAttributes(RequestCallback callback);

    /**
     * @brief Handle Read-Composite request
     * @param callback Callback to provide multiple resources
     */
    void onReadComposite(RequestCallback callback);

    /**
     * @brief Handle Write-Composite request
     * @param callback Callback to write multiple resources
     */
    void onWriteComposite(RequestCallback callback);

    // ========================================================================
    // Information Reporting Interface (Section 8.3.4)
    // ========================================================================

    /**
     * @brief Handle Observe request
     * @param callback Callback to start observation
     */
    void onObserve(RequestCallback callback);

    /**
     * @brief Handle Cancel-Observe request
     * @param callback Callback to stop observation
     */
    void onCancelObserve(RequestCallback callback);

    /**
     * @brief Send notification
     * @param path Resource path
     * @param data Resource data
     * @param contentFormat Content format
     * @param confirmable Whether to use QoS 1 (acknowledged)
     * @return Token for delivery confirmation
     */
    uint32_t sendNotification(const std::string& path,
                               const std::vector<uint8_t>& data,
                               uint16_t contentFormat,
                               bool confirmable = true);

    /**
     * @brief Send data (client-initiated push)
     * @param paths Resource paths
     * @param data SenML-encoded data
     * @param contentFormat Content format
     * @return Token for delivery confirmation
     */
    uint32_t sendData(const std::vector<std::string>& paths,
                       const std::vector<uint8_t>& data,
                       uint16_t contentFormat);

    // ========================================================================
    // Response Handling
    // ========================================================================

    /**
     * @brief Set callback for registration responses
     * @param callback Callback with result
     */
    void onRegistrationResponse(std::function<void(uint32_t token, 
                                                    LwM2MResponseCode code,
                                                    const std::string& location)> callback);

    /**
     * @brief Set callback for bootstrap responses
     * @param callback Callback with result
     */
    void onBootstrapResponse(std::function<void(uint32_t token,
                                                 LwM2MResponseCode code)> callback);

    /**
     * @brief Set general response callback
     * @param callback Callback for any response
     */
    void onResponse(std::function<void(uint32_t token, const LwM2MPayload&)> callback);

    // ========================================================================
    // Utility Methods
    // ========================================================================

    /**
     * @brief Process pending messages
     * @param timeout Maximum time to wait (milliseconds)
     * @return Number of messages processed
     */
    int processMessages(uint32_t timeout = 100);

    /**
     * @brief Get next available token
     * @return Unique token value
     */
    uint32_t nextToken();

    /**
     * @brief Get statistics
     */
    struct Statistics {
        uint64_t messagesSent = 0;
        uint64_t messagesReceived = 0;
        uint64_t bytesOut = 0;
        uint64_t bytesIn = 0;
        uint64_t reconnections = 0;
        uint64_t errors = 0;
        std::chrono::system_clock::time_point lastActivity;
    };
    Statistics getStatistics() const;

    /**
     * @brief Get last error message
     * @return Error description
     */
    std::string getLastError() const;

private:
    // Internal implementation
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================================
// MQTT Server Object (Object ID: 24)
// ============================================================================

/**
 * @brief LwM2M MQTT Server Object Implementation
 * 
 * Provides configuration for MQTT transport binding.
 */
class MqttServerObject {
public:
    static constexpr uint16_t OBJECT_ID = 24;

    /**
     * @brief Resource IDs
     */
    enum class ResourceId : uint16_t {
        SHORT_SERVER_ID = 0,
        MQTT_BROKER_URI = 1,
        MQTT_BROKER_PORT = 2,
        CLIENT_IDENTIFIER = 3,
        CLEAN_SESSION = 4,
        KEEP_ALIVE = 5,
        QOS = 6,
        PROTOCOL_VERSION = 7,
        USERNAME = 8,
        PASSWORD = 9,
        TOPIC_PREFIX = 10,
        RETAIN = 11,
        SESSION_EXPIRY = 12,
        RECEIVE_MAXIMUM = 13,
        MAXIMUM_PACKET_SIZE = 14
    };

    /**
     * @brief Create instance from configuration
     */
    static MqttServerConfig fromInstance(uint16_t instanceId);

    /**
     * @brief Update instance with configuration
     */
    static void updateInstance(uint16_t instanceId, const MqttServerConfig& config);
};

// ============================================================================
// MQTT COSE Object (Object ID: 23)
// ============================================================================

/**
 * @brief LwM2M COSE Object Implementation
 * 
 * Provides security credentials for MQTT transport.
 */
class MqttCoseObject {
public:
    static constexpr uint16_t OBJECT_ID = 23;

    /**
     * @brief Resource IDs
     */
    enum class ResourceId : uint16_t {
        SHORT_SERVER_ID = 0,
        SECURITY_MODE = 1,
        PUBLIC_KEY_OR_IDENTITY = 2,
        SECRET_KEY = 3,
        SERVER_PUBLIC_KEY = 4,
        CERTIFICATE_CHAIN = 5,
        CA_CERTIFICATE = 6,
        SNI = 7,
        VERIFY_PEER = 8
    };

    /**
     * @brief Create instance from configuration
     */
    static MqttCoseConfig fromInstance(uint16_t instanceId);

    /**
     * @brief Update instance with configuration
     */
    static void updateInstance(uint16_t instanceId, const MqttCoseConfig& config);
};

// ============================================================================
// Payload Serialization
// ============================================================================

/**
 * @brief MQTT Payload serializer/deserializer per Section 8.7
 */
class MqttPayloadCodec {
public:
    /**
     * @brief Encode payload to CBOR
     * @param payload LwM2M payload
     * @return CBOR-encoded bytes
     */
    static std::vector<uint8_t> encodeCbor(const LwM2MPayload& payload);

    /**
     * @brief Decode payload from CBOR
     * @param data CBOR-encoded bytes
     * @return Decoded payload
     */
    static std::optional<LwM2MPayload> decodeCbor(const std::vector<uint8_t>& data);

    /**
     * @brief Encode payload to JSON
     * @param payload LwM2M payload
     * @return JSON string
     */
    static std::string encodeJson(const LwM2MPayload& payload);

    /**
     * @brief Decode payload from JSON
     * @param json JSON string
     * @return Decoded payload
     */
    static std::optional<LwM2MPayload> decodeJson(const std::string& json);

    /**
     * @brief Get CBOR content format ID
     */
    static constexpr uint16_t CONTENT_FORMAT_CBOR = 60;

    /**
     * @brief Get JSON content format ID
     */
    static constexpr uint16_t CONTENT_FORMAT_JSON = 11543;

    /**
     * @brief Get SenML CBOR content format ID
     */
    static constexpr uint16_t CONTENT_FORMAT_SENML_CBOR = 112;

    /**
     * @brief Get SenML JSON content format ID
     */
    static constexpr uint16_t CONTENT_FORMAT_SENML_JSON = 110;
};

} // namespace transport
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_MQTT_TRANSPORT_H
