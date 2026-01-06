/**
 * @file mqtt_transport.cpp
 * @brief LwM2M MQTT Transport Binding Implementation
 * 
 * Implements Section 8 of OMA-TS-LightweightM2M_Transport-V1_2_1 specification.
 * 
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "transport/mqtt_transport.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <thread>
#include <condition_variable>
#include <cstring>

// External MQTT library would be included here
// #include <mosquitto.h>  // or paho-mqtt-c

namespace lwm2m {
namespace transport {

// ============================================================================
// CBOR Key Mappings (Section 8.7)
// ============================================================================

namespace cbor_keys {
    // Request/Response keys
    constexpr int OPERATION = 0;
    constexpr int TOKEN = 1;
    constexpr int PATH = 2;
    constexpr int CONTENT_FORMAT = 3;
    constexpr int PAYLOAD = 4;
    constexpr int RESULT = 5;
    constexpr int ERROR_MSG = 6;
    
    // Registration keys
    constexpr int ENDPOINT = 10;
    constexpr int LIFETIME = 11;
    constexpr int LWM2M_VERSION = 12;
    constexpr int BINDING = 13;
    constexpr int QUEUE_MODE = 14;
    constexpr int OBJECTS = 15;
    constexpr int ALT_PATH = 16;
    
    // Notification keys
    constexpr int OBSERVE_SEQ = 20;
    constexpr int TIMESTAMP = 21;
}

// ============================================================================
// MqttTopic Implementation
// ============================================================================

std::optional<MqttTopic> MqttTopic::parse(const std::string& topicStr,
                                           const std::string& prefix) {
    MqttTopic topic;
    topic.prefix = prefix;
    
    std::string remaining = topicStr;
    
    // Remove prefix if present
    if (!prefix.empty()) {
        std::string prefixWithSlash = prefix;
        if (prefixWithSlash.back() != '/') prefixWithSlash += "/";
        
        if (remaining.find(prefixWithSlash) == 0) {
            remaining = remaining.substr(prefixWithSlash.length());
        } else if (remaining.find(prefix) == 0) {
            remaining = remaining.substr(prefix.length());
            if (!remaining.empty() && remaining[0] == '/') {
                remaining = remaining.substr(1);
            }
        }
    }
    
    // Parse direction (rd/bs/dp)
    size_t pos = remaining.find('/');
    if (pos == std::string::npos) return std::nullopt;
    
    topic.direction = remaining.substr(0, pos);
    remaining = remaining.substr(pos + 1);
    
    // Validate direction
    if (topic.direction != "rd" && topic.direction != "bs" && topic.direction != "dp") {
        return std::nullopt;
    }
    
    // Parse endpoint name
    pos = remaining.find('/');
    if (pos == std::string::npos) {
        topic.endpointName = remaining;
        topic.operation = "";
    } else {
        topic.endpointName = remaining.substr(0, pos);
        topic.operation = remaining.substr(pos + 1);
    }
    
    if (topic.endpointName.empty()) return std::nullopt;
    
    return topic;
}

// ============================================================================
// MqttTransport::Impl
// ============================================================================

class MqttTransport::Impl {
public:
    explicit Impl(const Config& config)
        : config_(config)
        , state_(MqttConnectionState::DISCONNECTED)
        , nextToken_(generateInitialToken())
        , running_(false)
    {
    }
    
    ~Impl() {
        disconnect(true);
    }
    
    // Connection Management
    MqttResult connect() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (state_ == MqttConnectionState::CONNECTED) {
            return MqttResult::SUCCESS;
        }
        
        state_ = MqttConnectionState::CONNECTING;
        notifyConnectionState(MqttConnectionState::CONNECTING, "Initiating connection");
        
        // Build connection URI
        std::string uri = buildBrokerUri();
        
        // Initialize TLS if required
        if (config_.security.securityMode != MqttSecurityMode::NO_SEC) {
            if (!initializeTls()) {
                state_ = MqttConnectionState::ERROR;
                lastError_ = "TLS initialization failed";
                notifyConnectionState(MqttConnectionState::ERROR, lastError_);
                return MqttResult::ERROR_TLS;
            }
        }
        
        // Connect to broker
        // In real implementation, this would use mosquitto_connect() or similar
        
        // Simulate successful connection for now
        state_ = MqttConnectionState::CONNECTED;
        stats_.lastActivity = std::chrono::system_clock::now();
        
        // Subscribe to client topics
        subscribeToClientTopics();
        
        notifyConnectionState(MqttConnectionState::CONNECTED, "Connected to broker");
        
        return MqttResult::SUCCESS;
    }
    
    MqttResult disconnect(bool graceful) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (state_ == MqttConnectionState::DISCONNECTED) {
            return MqttResult::SUCCESS;
        }
        
        state_ = MqttConnectionState::DISCONNECTING;
        notifyConnectionState(MqttConnectionState::DISCONNECTING, "Disconnecting");
        
        running_ = false;
        
        if (graceful) {
            // Send MQTT DISCONNECT packet
            // mosquitto_disconnect(client_);
        }
        
        // Clean up subscriptions
        subscriptions_.clear();
        
        state_ = MqttConnectionState::DISCONNECTED;
        notifyConnectionState(MqttConnectionState::DISCONNECTED, "Disconnected");
        
        return MqttResult::SUCCESS;
    }
    
    bool isConnected() const {
        return state_ == MqttConnectionState::CONNECTED;
    }
    
    MqttConnectionState getConnectionState() const {
        return state_;
    }
    
    void setConnectionCallback(ConnectionCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        connectionCallback_ = std::move(callback);
    }
    
    // Bootstrap Interface
    uint32_t bootstrapRequest(const std::string& endpointName,
                               uint16_t preferredFormat) {
        uint32_t token = nextToken();
        
        LwM2MPayload payload;
        payload.operation = LwM2MOperation::BOOTSTRAP_REQUEST;
        payload.token = token;
        payload.contentFormat = preferredFormat;
        payload.options["ep"] = endpointName;
        
        MqttTopic topic;
        topic.prefix = config_.server.topicPrefix;
        topic.direction = "bs";
        topic.endpointName = endpointName;
        
        publish(topic.toString(), serializePayload(payload), 
                config_.server.defaultQoS);
        
        return token;
    }
    
    void onBootstrapWrite(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        bootstrapWriteCallback_ = std::move(callback);
    }
    
    void onBootstrapDiscover(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        bootstrapDiscoverCallback_ = std::move(callback);
    }
    
    void onBootstrapFinish(std::function<void()> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        bootstrapFinishCallback_ = std::move(callback);
    }
    
    // Registration Interface
    uint32_t registerClient(const MqttRegistrationParams& params) {
        uint32_t token = nextToken();
        
        LwM2MPayload payload;
        payload.operation = LwM2MOperation::REGISTER;
        payload.token = token;
        payload.options["ep"] = params.endpointName;
        payload.options["lt"] = std::to_string(params.lifetime);
        payload.options["lwm2m"] = params.lwm2mVersion;
        payload.options["b"] = params.binding;
        
        if (params.queueMode) {
            payload.options["Q"] = "true";
        }
        
        // Encode object links
        std::ostringstream oss;
        for (size_t i = 0; i < params.objectLinks.size(); ++i) {
            if (i > 0) oss << ",";
            oss << "<" << params.objectLinks[i] << ">";
        }
        payload.data = std::vector<uint8_t>(oss.str().begin(), oss.str().end());
        payload.contentFormat = 40;  // application/link-format
        
        MqttTopic topic;
        topic.prefix = config_.server.topicPrefix;
        topic.direction = "rd";
        topic.endpointName = params.endpointName;
        
        endpointName_ = params.endpointName;
        registrationLifetime_ = params.lifetime;
        
        publish(topic.toString(), serializePayload(payload),
                config_.server.defaultQoS);
        
        // Store pending request
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pendingRequests_[token] = PendingRequest{
                LwM2MOperation::REGISTER,
                std::chrono::steady_clock::now(),
                config_.responseTimeout
            };
        }
        
        return token;
    }
    
    uint32_t updateRegistration(uint32_t lifetime,
                                 const std::string& binding,
                                 const std::vector<std::string>& objectLinks) {
        uint32_t token = nextToken();
        
        LwM2MPayload payload;
        payload.operation = LwM2MOperation::UPDATE;
        payload.token = token;
        
        if (lifetime > 0) {
            payload.options["lt"] = std::to_string(lifetime);
            registrationLifetime_ = lifetime;
        }
        
        if (!binding.empty()) {
            payload.options["b"] = binding;
        }
        
        if (!objectLinks.empty()) {
            std::ostringstream oss;
            for (size_t i = 0; i < objectLinks.size(); ++i) {
                if (i > 0) oss << ",";
                oss << "<" << objectLinks[i] << ">";
            }
            payload.data = std::vector<uint8_t>(oss.str().begin(), oss.str().end());
            payload.contentFormat = 40;
        }
        
        MqttTopic topic;
        topic.prefix = config_.server.topicPrefix;
        topic.direction = "rd";
        topic.endpointName = endpointName_;
        topic.operation = "upd";
        
        publish(topic.toString(), serializePayload(payload),
                config_.server.defaultQoS);
        
        return token;
    }
    
    uint32_t deregister() {
        uint32_t token = nextToken();
        
        LwM2MPayload payload;
        payload.operation = LwM2MOperation::DEREGISTER;
        payload.token = token;
        
        MqttTopic topic;
        topic.prefix = config_.server.topicPrefix;
        topic.direction = "rd";
        topic.endpointName = endpointName_;
        topic.operation = "dereg";
        
        publish(topic.toString(), serializePayload(payload),
                config_.server.defaultQoS);
        
        return token;
    }
    
    // Device Management Interface handlers
    void onRead(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        readCallback_ = std::move(callback);
    }
    
    void onWrite(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        writeCallback_ = std::move(callback);
    }
    
    void onExecute(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        executeCallback_ = std::move(callback);
    }
    
    void onCreate(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        createCallback_ = std::move(callback);
    }
    
    void onDelete(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        deleteCallback_ = std::move(callback);
    }
    
    void onDiscover(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        discoverCallback_ = std::move(callback);
    }
    
    void onWriteAttributes(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        writeAttributesCallback_ = std::move(callback);
    }
    
    void onReadComposite(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        readCompositeCallback_ = std::move(callback);
    }
    
    void onWriteComposite(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        writeCompositeCallback_ = std::move(callback);
    }
    
    // Information Reporting Interface
    void onObserve(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        observeCallback_ = std::move(callback);
    }
    
    void onCancelObserve(RequestCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        cancelObserveCallback_ = std::move(callback);
    }
    
    uint32_t sendNotification(const std::string& path,
                               const std::vector<uint8_t>& data,
                               uint16_t contentFormat,
                               bool confirmable) {
        uint32_t token = nextToken();
        
        LwM2MPayload payload;
        payload.operation = LwM2MOperation::NOTIFY;
        payload.token = token;
        payload.path = path;
        payload.data = data;
        payload.contentFormat = contentFormat;
        
        // Add observe sequence number
        uint32_t seq = getNextObserveSequence(path);
        payload.options["seq"] = std::to_string(seq);
        payload.options["ts"] = std::to_string(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()
        );
        
        MqttTopic topic;
        topic.prefix = config_.server.topicPrefix;
        topic.direction = "dp";
        topic.endpointName = endpointName_;
        topic.operation = "notify";
        
        MqttQoS qos = confirmable ? MqttQoS::AT_LEAST_ONCE : MqttQoS::AT_MOST_ONCE;
        publish(topic.toString(), serializePayload(payload), qos);
        
        return token;
    }
    
    uint32_t sendData(const std::vector<std::string>& paths,
                       const std::vector<uint8_t>& data,
                       uint16_t contentFormat) {
        uint32_t token = nextToken();
        
        LwM2MPayload payload;
        payload.operation = LwM2MOperation::SEND;
        payload.token = token;
        payload.data = data;
        payload.contentFormat = contentFormat;
        
        // Encode paths
        std::ostringstream oss;
        for (size_t i = 0; i < paths.size(); ++i) {
            if (i > 0) oss << ",";
            oss << paths[i];
        }
        payload.options["paths"] = oss.str();
        
        MqttTopic topic;
        topic.prefix = config_.server.topicPrefix;
        topic.direction = "dp";
        topic.endpointName = endpointName_;
        topic.operation = "send";
        
        publish(topic.toString(), serializePayload(payload),
                config_.server.defaultQoS);
        
        return token;
    }
    
    // Response handling
    void onRegistrationResponse(std::function<void(uint32_t, LwM2MResponseCode,
                                                    const std::string&)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        registrationResponseCallback_ = std::move(callback);
    }
    
    void onBootstrapResponse(std::function<void(uint32_t, LwM2MResponseCode)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        bootstrapResponseCallback_ = std::move(callback);
    }
    
    void onResponse(std::function<void(uint32_t, const LwM2MPayload&)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        generalResponseCallback_ = std::move(callback);
    }
    
    // Message processing
    int processMessages(uint32_t timeout) {
        // In real implementation, this would call mosquitto_loop() or similar
        
        int processed = 0;
        auto deadline = std::chrono::steady_clock::now() + 
                        std::chrono::milliseconds(timeout);
        
        while (std::chrono::steady_clock::now() < deadline) {
            std::unique_lock<std::mutex> lock(mutex_);
            
            if (incomingQueue_.empty()) {
                // Wait for messages
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            
            auto msg = std::move(incomingQueue_.front());
            incomingQueue_.pop();
            lock.unlock();
            
            handleIncomingMessage(msg.first, msg.second);
            processed++;
        }
        
        // Check for timed out requests
        checkPendingRequestTimeouts();
        
        // Handle reconnection if needed
        if (state_ == MqttConnectionState::DISCONNECTED && config_.autoReconnect) {
            attemptReconnect();
        }
        
        return processed;
    }
    
    uint32_t nextToken() {
        return nextToken_++;
    }
    
    Statistics getStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return stats_;
    }
    
    std::string getLastError() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return lastError_;
    }

private:
    struct PendingRequest {
        LwM2MOperation operation;
        std::chrono::steady_clock::time_point timestamp;
        uint32_t timeout;
    };
    
    Config config_;
    std::atomic<MqttConnectionState> state_;
    std::atomic<uint32_t> nextToken_;
    std::atomic<bool> running_;
    
    std::string endpointName_;
    uint32_t registrationLifetime_ = 86400;
    std::string registrationLocation_;
    
    mutable std::mutex mutex_;
    std::string lastError_;
    Statistics stats_;
    
    // Subscriptions
    std::map<std::string, MqttQoS> subscriptions_;
    
    // Pending requests
    std::map<uint32_t, PendingRequest> pendingRequests_;
    
    // Observe sequence numbers
    std::map<std::string, uint32_t> observeSequences_;
    
    // Message queues
    std::queue<std::pair<std::string, std::vector<uint8_t>>> incomingQueue_;
    
    // Callbacks
    ConnectionCallback connectionCallback_;
    
    // Bootstrap callbacks
    RequestCallback bootstrapWriteCallback_;
    RequestCallback bootstrapDiscoverCallback_;
    std::function<void()> bootstrapFinishCallback_;
    
    // Registration callbacks
    std::function<void(uint32_t, LwM2MResponseCode, const std::string&)> 
        registrationResponseCallback_;
    std::function<void(uint32_t, LwM2MResponseCode)> bootstrapResponseCallback_;
    std::function<void(uint32_t, const LwM2MPayload&)> generalResponseCallback_;
    
    // Device management callbacks
    RequestCallback readCallback_;
    RequestCallback writeCallback_;
    RequestCallback executeCallback_;
    RequestCallback createCallback_;
    RequestCallback deleteCallback_;
    RequestCallback discoverCallback_;
    RequestCallback writeAttributesCallback_;
    RequestCallback readCompositeCallback_;
    RequestCallback writeCompositeCallback_;
    
    // Reporting callbacks
    RequestCallback observeCallback_;
    RequestCallback cancelObserveCallback_;
    
    // Reconnection state
    uint32_t reconnectDelay_ = 1;
    std::chrono::steady_clock::time_point lastReconnectAttempt_;
    
    static uint32_t generateInitialToken() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(1, 0xFFFFFF);
        return dis(gen);
    }
    
    std::string buildBrokerUri() {
        std::string scheme = "mqtt";
        if (config_.security.securityMode != MqttSecurityMode::NO_SEC) {
            scheme = "mqtts";
        }
        
        std::ostringstream oss;
        oss << scheme << "://" << config_.server.brokerUri 
            << ":" << config_.server.brokerPort;
        return oss.str();
    }
    
    bool initializeTls() {
        // Initialize TLS context based on security mode
        switch (config_.security.securityMode) {
            case MqttSecurityMode::PSK:
                // Configure PSK credentials
                // mosquitto_tls_psk_set(client_, psk_id, psk_key, cipher);
                break;
                
            case MqttSecurityMode::CERTIFICATE:
                // Configure certificates
                // mosquitto_tls_set(client_, ca_cert, ca_path, cert, key, pw_callback);
                if (config_.security.verifyPeer) {
                    // mosquitto_tls_opts_set(client_, SSL_VERIFY_PEER, ...);
                }
                if (!config_.security.sni.empty()) {
                    // Set SNI
                }
                break;
                
            case MqttSecurityMode::RPK:
                // Configure raw public key
                break;
                
            case MqttSecurityMode::TOKEN:
                // Configure OAuth2/JWT token
                // Token goes in username/password fields
                break;
                
            default:
                break;
        }
        
        return true;
    }
    
    void subscribeToClientTopics() {
        // Subscribe to server-to-client topics
        std::vector<std::string> topics = {
            buildTopic("rd", "+", "resp"),      // Registration responses
            buildTopic("bs", "+", "resp"),      // Bootstrap responses
            buildTopic("dp", "+", "req"),       // Server requests
        };
        
        for (const auto& topic : topics) {
            subscribe(topic, config_.server.defaultQoS);
        }
    }
    
    std::string buildTopic(const std::string& direction,
                            const std::string& endpoint,
                            const std::string& operation = "") {
        std::ostringstream oss;
        if (!config_.server.topicPrefix.empty()) {
            oss << config_.server.topicPrefix;
            if (config_.server.topicPrefix.back() != '/') oss << "/";
        }
        oss << direction << "/" << endpoint;
        if (!operation.empty()) {
            oss << "/" << operation;
        }
        return oss.str();
    }
    
    void subscribe(const std::string& topic, MqttQoS qos) {
        // mosquitto_subscribe(client_, nullptr, topic.c_str(), static_cast<int>(qos));
        subscriptions_[topic] = qos;
    }
    
    void publish(const std::string& topic, 
                  const std::vector<uint8_t>& payload,
                  MqttQoS qos) {
        // mosquitto_publish(client_, nullptr, topic.c_str(), 
        //                   payload.size(), payload.data(),
        //                   static_cast<int>(qos), 
        //                   config_.server.retainMessages);
        
        stats_.messagesSent++;
        stats_.bytesOut += payload.size();
        stats_.lastActivity = std::chrono::system_clock::now();
    }
    
    std::vector<uint8_t> serializePayload(const LwM2MPayload& payload) {
        if (config_.enableCbor) {
            return MqttPayloadCodec::encodeCbor(payload);
        } else {
            std::string json = MqttPayloadCodec::encodeJson(payload);
            return std::vector<uint8_t>(json.begin(), json.end());
        }
    }
    
    std::optional<LwM2MPayload> deserializePayload(const std::vector<uint8_t>& data) {
        // Try CBOR first
        auto result = MqttPayloadCodec::decodeCbor(data);
        if (result) return result;
        
        // Fall back to JSON
        std::string json(data.begin(), data.end());
        return MqttPayloadCodec::decodeJson(json);
    }
    
    void handleIncomingMessage(const std::string& topic,
                                const std::vector<uint8_t>& payload) {
        stats_.messagesReceived++;
        stats_.bytesIn += payload.size();
        stats_.lastActivity = std::chrono::system_clock::now();
        
        auto parsedTopic = MqttTopic::parse(topic, config_.server.topicPrefix);
        if (!parsedTopic) {
            lastError_ = "Invalid topic format: " + topic;
            stats_.errors++;
            return;
        }
        
        auto lwm2mPayload = deserializePayload(payload);
        if (!lwm2mPayload) {
            lastError_ = "Failed to deserialize payload";
            stats_.errors++;
            return;
        }
        
        // Route to appropriate handler
        if (parsedTopic->direction == "rd") {
            handleRegistrationMessage(*parsedTopic, *lwm2mPayload);
        } else if (parsedTopic->direction == "bs") {
            handleBootstrapMessage(*parsedTopic, *lwm2mPayload);
        } else if (parsedTopic->direction == "dp") {
            handleDataMessage(*parsedTopic, *lwm2mPayload);
        }
    }
    
    void handleRegistrationMessage(const MqttTopic& topic,
                                    const LwM2MPayload& payload) {
        if (topic.operation == "resp") {
            // Registration response
            if (payload.responseCode) {
                std::string location;
                auto it = payload.options.find("location");
                if (it != payload.options.end()) {
                    location = it->second;
                    registrationLocation_ = location;
                }
                
                // Remove from pending
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    pendingRequests_.erase(payload.token);
                }
                
                if (registrationResponseCallback_) {
                    registrationResponseCallback_(payload.token,
                                                   *payload.responseCode,
                                                   location);
                }
            }
        }
    }
    
    void handleBootstrapMessage(const MqttTopic& topic,
                                 const LwM2MPayload& payload) {
        auto respondFn = [this, token = payload.token](const LwM2MPayload& response) {
            sendResponse(token, response);
        };
        
        switch (payload.operation) {
            case LwM2MOperation::BOOTSTRAP_WRITE:
                if (bootstrapWriteCallback_) {
                    bootstrapWriteCallback_(payload, respondFn);
                }
                break;
                
            case LwM2MOperation::BOOTSTRAP_DISCOVER:
                if (bootstrapDiscoverCallback_) {
                    bootstrapDiscoverCallback_(payload, respondFn);
                }
                break;
                
            case LwM2MOperation::BOOTSTRAP_FINISH:
                if (bootstrapFinishCallback_) {
                    bootstrapFinishCallback_();
                }
                break;
                
            default:
                if (payload.responseCode && bootstrapResponseCallback_) {
                    bootstrapResponseCallback_(payload.token, *payload.responseCode);
                }
                break;
        }
    }
    
    void handleDataMessage(const MqttTopic& topic,
                            const LwM2MPayload& payload) {
        auto respondFn = [this, token = payload.token](const LwM2MPayload& response) {
            sendResponse(token, response);
        };
        
        switch (payload.operation) {
            case LwM2MOperation::READ:
                if (readCallback_) readCallback_(payload, respondFn);
                break;
                
            case LwM2MOperation::WRITE:
                if (writeCallback_) writeCallback_(payload, respondFn);
                break;
                
            case LwM2MOperation::EXECUTE:
                if (executeCallback_) executeCallback_(payload, respondFn);
                break;
                
            case LwM2MOperation::CREATE:
                if (createCallback_) createCallback_(payload, respondFn);
                break;
                
            case LwM2MOperation::DELETE:
                if (deleteCallback_) deleteCallback_(payload, respondFn);
                break;
                
            case LwM2MOperation::DISCOVER:
                if (discoverCallback_) discoverCallback_(payload, respondFn);
                break;
                
            case LwM2MOperation::WRITE_ATTRIBUTES:
                if (writeAttributesCallback_) writeAttributesCallback_(payload, respondFn);
                break;
                
            case LwM2MOperation::READ_COMPOSITE:
                if (readCompositeCallback_) readCompositeCallback_(payload, respondFn);
                break;
                
            case LwM2MOperation::WRITE_COMPOSITE:
                if (writeCompositeCallback_) writeCompositeCallback_(payload, respondFn);
                break;
                
            case LwM2MOperation::OBSERVE:
                if (observeCallback_) observeCallback_(payload, respondFn);
                break;
                
            case LwM2MOperation::CANCEL_OBSERVE:
                if (cancelObserveCallback_) cancelObserveCallback_(payload, respondFn);
                break;
                
            default:
                // Unknown operation
                if (generalResponseCallback_) {
                    generalResponseCallback_(payload.token, payload);
                }
                break;
        }
    }
    
    void sendResponse(uint32_t token, const LwM2MPayload& response) {
        LwM2MPayload payload = response;
        payload.token = token;
        
        MqttTopic topic;
        topic.prefix = config_.server.topicPrefix;
        topic.direction = "dp";
        topic.endpointName = endpointName_;
        topic.operation = "resp";
        
        publish(topic.toString(), serializePayload(payload),
                config_.server.defaultQoS);
    }
    
    uint32_t getNextObserveSequence(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex_);
        return ++observeSequences_[path];
    }
    
    void checkPendingRequestTimeouts() {
        auto now = std::chrono::steady_clock::now();
        std::vector<uint32_t> timedOut;
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto& [token, req] : pendingRequests_) {
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                    now - req.timestamp).count();
                if (elapsed > req.timeout) {
                    timedOut.push_back(token);
                }
            }
            
            for (auto token : timedOut) {
                pendingRequests_.erase(token);
            }
        }
        
        // Notify timeouts
        for (auto token : timedOut) {
            stats_.errors++;
            if (generalResponseCallback_) {
                LwM2MPayload timeoutPayload;
                timeoutPayload.token = token;
                timeoutPayload.responseCode = LwM2MResponseCode::GATEWAY_TIMEOUT;
                timeoutPayload.errorMessage = "Request timed out";
                generalResponseCallback_(token, timeoutPayload);
            }
        }
    }
    
    void attemptReconnect() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - lastReconnectAttempt_).count();
        
        if (elapsed < reconnectDelay_) {
            return;
        }
        
        lastReconnectAttempt_ = now;
        state_ = MqttConnectionState::RECONNECTING;
        notifyConnectionState(MqttConnectionState::RECONNECTING, "Attempting reconnection");
        
        stats_.reconnections++;
        
        if (connect() == MqttResult::SUCCESS) {
            reconnectDelay_ = config_.reconnectMinDelay;
        } else {
            // Exponential backoff
            reconnectDelay_ = std::min(reconnectDelay_ * 2, config_.reconnectMaxDelay);
        }
    }
    
    void notifyConnectionState(MqttConnectionState state, const std::string& reason) {
        if (connectionCallback_) {
            connectionCallback_(state, reason);
        }
    }
};

// ============================================================================
// MqttTransport Public Implementation
// ============================================================================

MqttTransport::MqttTransport(const Config& config)
    : impl_(std::make_unique<Impl>(config)) {
}

MqttTransport::~MqttTransport() = default;

MqttResult MqttTransport::connect() {
    return impl_->connect();
}

MqttResult MqttTransport::disconnect(bool graceful) {
    return impl_->disconnect(graceful);
}

bool MqttTransport::isConnected() const {
    return impl_->isConnected();
}

MqttConnectionState MqttTransport::getConnectionState() const {
    return impl_->getConnectionState();
}

void MqttTransport::setConnectionCallback(ConnectionCallback callback) {
    impl_->setConnectionCallback(std::move(callback));
}

uint32_t MqttTransport::bootstrapRequest(const std::string& endpointName,
                                          uint16_t preferredFormat) {
    return impl_->bootstrapRequest(endpointName, preferredFormat);
}

void MqttTransport::onBootstrapWrite(RequestCallback callback) {
    impl_->onBootstrapWrite(std::move(callback));
}

void MqttTransport::onBootstrapDiscover(RequestCallback callback) {
    impl_->onBootstrapDiscover(std::move(callback));
}

void MqttTransport::onBootstrapFinish(std::function<void()> callback) {
    impl_->onBootstrapFinish(std::move(callback));
}

uint32_t MqttTransport::registerClient(const MqttRegistrationParams& params) {
    return impl_->registerClient(params);
}

uint32_t MqttTransport::updateRegistration(uint32_t lifetime,
                                            const std::string& binding,
                                            const std::vector<std::string>& objectLinks) {
    return impl_->updateRegistration(lifetime, binding, objectLinks);
}

uint32_t MqttTransport::deregister() {
    return impl_->deregister();
}

void MqttTransport::onRead(RequestCallback callback) {
    impl_->onRead(std::move(callback));
}

void MqttTransport::onWrite(RequestCallback callback) {
    impl_->onWrite(std::move(callback));
}

void MqttTransport::onExecute(RequestCallback callback) {
    impl_->onExecute(std::move(callback));
}

void MqttTransport::onCreate(RequestCallback callback) {
    impl_->onCreate(std::move(callback));
}

void MqttTransport::onDelete(RequestCallback callback) {
    impl_->onDelete(std::move(callback));
}

void MqttTransport::onDiscover(RequestCallback callback) {
    impl_->onDiscover(std::move(callback));
}

void MqttTransport::onWriteAttributes(RequestCallback callback) {
    impl_->onWriteAttributes(std::move(callback));
}

void MqttTransport::onReadComposite(RequestCallback callback) {
    impl_->onReadComposite(std::move(callback));
}

void MqttTransport::onWriteComposite(RequestCallback callback) {
    impl_->onWriteComposite(std::move(callback));
}

void MqttTransport::onObserve(RequestCallback callback) {
    impl_->onObserve(std::move(callback));
}

void MqttTransport::onCancelObserve(RequestCallback callback) {
    impl_->onCancelObserve(std::move(callback));
}

uint32_t MqttTransport::sendNotification(const std::string& path,
                                          const std::vector<uint8_t>& data,
                                          uint16_t contentFormat,
                                          bool confirmable) {
    return impl_->sendNotification(path, data, contentFormat, confirmable);
}

uint32_t MqttTransport::sendData(const std::vector<std::string>& paths,
                                  const std::vector<uint8_t>& data,
                                  uint16_t contentFormat) {
    return impl_->sendData(paths, data, contentFormat);
}

void MqttTransport::onRegistrationResponse(
    std::function<void(uint32_t, LwM2MResponseCode, const std::string&)> callback) {
    impl_->onRegistrationResponse(std::move(callback));
}

void MqttTransport::onBootstrapResponse(
    std::function<void(uint32_t, LwM2MResponseCode)> callback) {
    impl_->onBootstrapResponse(std::move(callback));
}

void MqttTransport::onResponse(
    std::function<void(uint32_t, const LwM2MPayload&)> callback) {
    impl_->onResponse(std::move(callback));
}

int MqttTransport::processMessages(uint32_t timeout) {
    return impl_->processMessages(timeout);
}

uint32_t MqttTransport::nextToken() {
    return impl_->nextToken();
}

MqttTransport::Statistics MqttTransport::getStatistics() const {
    return impl_->getStatistics();
}

std::string MqttTransport::getLastError() const {
    return impl_->getLastError();
}

// ============================================================================
// MqttPayloadCodec Implementation
// ============================================================================

std::vector<uint8_t> MqttPayloadCodec::encodeCbor(const LwM2MPayload& payload) {
    std::vector<uint8_t> result;
    
    // Simple CBOR map encoding
    // In production, use a proper CBOR library like tinycbor
    
    // Calculate number of items
    int items = 2;  // operation and token always present
    if (!payload.path.empty()) items++;
    if (payload.contentFormat != 0) items++;
    if (!payload.data.empty()) items++;
    if (payload.responseCode) items++;
    if (payload.errorMessage) items++;
    items += payload.options.size();
    
    // CBOR map header
    if (items < 24) {
        result.push_back(0xA0 | items);  // Map with N items
    } else {
        result.push_back(0xB8);
        result.push_back(items);
    }
    
    // Helper to encode integer
    auto encodeInt = [&result](int key, int64_t value) {
        // Key
        if (key < 24) {
            result.push_back(key);
        } else {
            result.push_back(0x18);
            result.push_back(key);
        }
        // Value
        if (value >= 0 && value < 24) {
            result.push_back(static_cast<uint8_t>(value));
        } else if (value >= 0 && value < 256) {
            result.push_back(0x18);
            result.push_back(static_cast<uint8_t>(value));
        } else if (value >= 0 && value < 65536) {
            result.push_back(0x19);
            result.push_back((value >> 8) & 0xFF);
            result.push_back(value & 0xFF);
        } else {
            result.push_back(0x1A);
            result.push_back((value >> 24) & 0xFF);
            result.push_back((value >> 16) & 0xFF);
            result.push_back((value >> 8) & 0xFF);
            result.push_back(value & 0xFF);
        }
    };
    
    // Helper to encode string
    auto encodeString = [&result](int key, const std::string& value) {
        // Key
        if (key < 24) {
            result.push_back(key);
        } else {
            result.push_back(0x18);
            result.push_back(key);
        }
        // Value (text string)
        size_t len = value.length();
        if (len < 24) {
            result.push_back(0x60 | len);
        } else if (len < 256) {
            result.push_back(0x78);
            result.push_back(len);
        } else {
            result.push_back(0x79);
            result.push_back((len >> 8) & 0xFF);
            result.push_back(len & 0xFF);
        }
        result.insert(result.end(), value.begin(), value.end());
    };
    
    // Helper to encode bytes
    auto encodeBytes = [&result](int key, const std::vector<uint8_t>& value) {
        // Key
        if (key < 24) {
            result.push_back(key);
        } else {
            result.push_back(0x18);
            result.push_back(key);
        }
        // Value (byte string)
        size_t len = value.size();
        if (len < 24) {
            result.push_back(0x40 | len);
        } else if (len < 256) {
            result.push_back(0x58);
            result.push_back(len);
        } else {
            result.push_back(0x59);
            result.push_back((len >> 8) & 0xFF);
            result.push_back(len & 0xFF);
        }
        result.insert(result.end(), value.begin(), value.end());
    };
    
    // Encode fields
    encodeInt(cbor_keys::OPERATION, static_cast<int>(payload.operation));
    encodeInt(cbor_keys::TOKEN, payload.token);
    
    if (!payload.path.empty()) {
        encodeString(cbor_keys::PATH, payload.path);
    }
    
    if (payload.contentFormat != 0) {
        encodeInt(cbor_keys::CONTENT_FORMAT, payload.contentFormat);
    }
    
    if (!payload.data.empty()) {
        encodeBytes(cbor_keys::PAYLOAD, payload.data);
    }
    
    if (payload.responseCode) {
        encodeInt(cbor_keys::RESULT, static_cast<int>(*payload.responseCode));
    }
    
    if (payload.errorMessage) {
        encodeString(cbor_keys::ERROR_MSG, *payload.errorMessage);
    }
    
    // Encode options as additional key-value pairs
    int optKey = 100;
    for (const auto& [key, value] : payload.options) {
        encodeString(optKey++, key + "=" + value);
    }
    
    return result;
}

std::optional<LwM2MPayload> MqttPayloadCodec::decodeCbor(const std::vector<uint8_t>& data) {
    if (data.empty()) return std::nullopt;
    
    // Check for CBOR map
    if ((data[0] & 0xE0) != 0xA0 && data[0] != 0xB8 && data[0] != 0xB9) {
        return std::nullopt;
    }
    
    // Simplified CBOR decoding
    // In production, use a proper CBOR library
    
    LwM2MPayload payload;
    size_t pos = 0;
    
    // Get number of items
    int items = 0;
    if ((data[pos] & 0xE0) == 0xA0) {
        items = data[pos] & 0x1F;
        pos++;
    } else if (data[pos] == 0xB8) {
        items = data[++pos];
        pos++;
    }
    
    // Helper to decode integer
    auto decodeInt = [&data, &pos]() -> int64_t {
        uint8_t first = data[pos++];
        if ((first & 0x1F) < 24) {
            return first & 0x1F;
        } else if ((first & 0x1F) == 24) {
            return data[pos++];
        } else if ((first & 0x1F) == 25) {
            int64_t val = (data[pos] << 8) | data[pos + 1];
            pos += 2;
            return val;
        } else if ((first & 0x1F) == 26) {
            int64_t val = (data[pos] << 24) | (data[pos + 1] << 16) |
                          (data[pos + 2] << 8) | data[pos + 3];
            pos += 4;
            return val;
        }
        return 0;
    };
    
    // Helper to decode string
    auto decodeString = [&data, &pos]() -> std::string {
        uint8_t first = data[pos++];
        size_t len = 0;
        if ((first & 0x1F) < 24) {
            len = first & 0x1F;
        } else if ((first & 0x1F) == 24) {
            len = data[pos++];
        } else if ((first & 0x1F) == 25) {
            len = (data[pos] << 8) | data[pos + 1];
            pos += 2;
        }
        std::string result(data.begin() + pos, data.begin() + pos + len);
        pos += len;
        return result;
    };
    
    // Helper to decode bytes
    auto decodeBytes = [&data, &pos]() -> std::vector<uint8_t> {
        uint8_t first = data[pos++];
        size_t len = 0;
        if ((first & 0x1F) < 24) {
            len = first & 0x1F;
        } else if ((first & 0x1F) == 24) {
            len = data[pos++];
        } else if ((first & 0x1F) == 25) {
            len = (data[pos] << 8) | data[pos + 1];
            pos += 2;
        }
        std::vector<uint8_t> result(data.begin() + pos, data.begin() + pos + len);
        pos += len;
        return result;
    };
    
    for (int i = 0; i < items && pos < data.size(); i++) {
        int key = decodeInt();
        
        switch (key) {
            case cbor_keys::OPERATION:
                payload.operation = static_cast<LwM2MOperation>(decodeInt());
                break;
            case cbor_keys::TOKEN:
                payload.token = decodeInt();
                break;
            case cbor_keys::PATH:
                payload.path = decodeString();
                break;
            case cbor_keys::CONTENT_FORMAT:
                payload.contentFormat = decodeInt();
                break;
            case cbor_keys::PAYLOAD:
                payload.data = decodeBytes();
                break;
            case cbor_keys::RESULT:
                payload.responseCode = static_cast<LwM2MResponseCode>(decodeInt());
                break;
            case cbor_keys::ERROR_MSG:
                payload.errorMessage = decodeString();
                break;
            default:
                // Skip unknown keys
                if ((data[pos] & 0xE0) == 0x60) {
                    decodeString();
                } else if ((data[pos] & 0xE0) == 0x40) {
                    decodeBytes();
                } else {
                    decodeInt();
                }
                break;
        }
    }
    
    return payload;
}

std::string MqttPayloadCodec::encodeJson(const LwM2MPayload& payload) {
    std::ostringstream oss;
    oss << "{";
    
    oss << "\"op\":" << static_cast<int>(payload.operation);
    oss << ",\"tk\":" << payload.token;
    
    if (!payload.path.empty()) {
        oss << ",\"path\":\"" << payload.path << "\"";
    }
    
    if (payload.contentFormat != 0) {
        oss << ",\"ct\":" << payload.contentFormat;
    }
    
    if (!payload.data.empty()) {
        // Base64 encode the data
        oss << ",\"data\":\"";
        static const char* base64_chars = 
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        
        size_t i = 0;
        while (i < payload.data.size()) {
            uint32_t octet_a = i < payload.data.size() ? payload.data[i++] : 0;
            uint32_t octet_b = i < payload.data.size() ? payload.data[i++] : 0;
            uint32_t octet_c = i < payload.data.size() ? payload.data[i++] : 0;
            uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;
            
            oss << base64_chars[(triple >> 18) & 0x3F];
            oss << base64_chars[(triple >> 12) & 0x3F];
            oss << base64_chars[(triple >> 6) & 0x3F];
            oss << base64_chars[triple & 0x3F];
        }
        oss << "\"";
    }
    
    if (payload.responseCode) {
        oss << ",\"res\":" << static_cast<int>(*payload.responseCode);
    }
    
    if (payload.errorMessage) {
        oss << ",\"err\":\"" << *payload.errorMessage << "\"";
    }
    
    if (!payload.options.empty()) {
        oss << ",\"opt\":{";
        bool first = true;
        for (const auto& [key, value] : payload.options) {
            if (!first) oss << ",";
            oss << "\"" << key << "\":\"" << value << "\"";
            first = false;
        }
        oss << "}";
    }
    
    oss << "}";
    return oss.str();
}

std::optional<LwM2MPayload> MqttPayloadCodec::decodeJson(const std::string& json) {
    // Simple JSON parser for LwM2M payloads
    // In production, use a proper JSON library like nlohmann/json
    
    LwM2MPayload payload;
    
    auto findValue = [&json](const std::string& key) -> std::string {
        std::string searchKey = "\"" + key + "\":";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return "";
        
        pos += searchKey.length();
        while (pos < json.size() && std::isspace(json[pos])) pos++;
        
        if (json[pos] == '"') {
            // String value
            pos++;
            size_t end = json.find('"', pos);
            if (end == std::string::npos) return "";
            return json.substr(pos, end - pos);
        } else {
            // Number or other value
            size_t end = json.find_first_of(",}", pos);
            if (end == std::string::npos) return "";
            return json.substr(pos, end - pos);
        }
    };
    
    std::string opStr = findValue("op");
    if (!opStr.empty()) {
        payload.operation = static_cast<LwM2MOperation>(std::stoi(opStr));
    }
    
    std::string tkStr = findValue("tk");
    if (!tkStr.empty()) {
        payload.token = std::stoul(tkStr);
    }
    
    payload.path = findValue("path");
    
    std::string ctStr = findValue("ct");
    if (!ctStr.empty()) {
        payload.contentFormat = std::stoi(ctStr);
    }
    
    std::string resStr = findValue("res");
    if (!resStr.empty()) {
        payload.responseCode = static_cast<LwM2MResponseCode>(std::stoi(resStr));
    }
    
    std::string errStr = findValue("err");
    if (!errStr.empty()) {
        payload.errorMessage = errStr;
    }
    
    return payload;
}

// ============================================================================
// MqttServerObject Implementation
// ============================================================================

MqttServerConfig MqttServerObject::fromInstance(uint16_t instanceId) {
    MqttServerConfig config;
    // In production, this would read from the LwM2M object store
    return config;
}

void MqttServerObject::updateInstance(uint16_t instanceId, 
                                       const MqttServerConfig& config) {
    // In production, this would write to the LwM2M object store
}

// ============================================================================
// MqttCoseObject Implementation
// ============================================================================

MqttCoseConfig MqttCoseObject::fromInstance(uint16_t instanceId) {
    MqttCoseConfig config;
    // In production, this would read from the LwM2M object store
    return config;
}

void MqttCoseObject::updateInstance(uint16_t instanceId,
                                     const MqttCoseConfig& config) {
    // In production, this would write to the LwM2M object store
}

} // namespace transport
} // namespace lwm2m
