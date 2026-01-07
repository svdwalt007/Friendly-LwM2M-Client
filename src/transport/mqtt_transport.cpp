/**
 * @file mqtt_transport.cpp
 * @brief MQTT Transport Binding Implementation
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "transport/mqtt_transport.h"

#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>

// CBOR encoding constants (RFC 8949)
namespace cbor {
    constexpr uint8_t UINT_1BYTE = 0x18;
    constexpr uint8_t UINT_2BYTE = 0x19;
    constexpr uint8_t UINT_4BYTE = 0x1a;
    constexpr uint8_t UINT_8BYTE = 0x1b;
    constexpr uint8_t NEGINT_BASE = 0x20;
    constexpr uint8_t BYTES_BASE = 0x40;
    constexpr uint8_t TEXT_BASE = 0x60;
    constexpr uint8_t ARRAY_BASE = 0x80;
    constexpr uint8_t MAP_BASE = 0xa0;
    constexpr uint8_t FALSE_VAL = 0xf4;
    constexpr uint8_t TRUE_VAL = 0xf5;
    constexpr uint8_t NULL_VAL = 0xf6;
}

namespace lwm2m {
namespace transport {

// ============================================================================
// Internal Implementation
// ============================================================================

struct MqttTransport::Impl {
    // Connection state
    bool connected = false;
    std::chrono::steady_clock::time_point lastActivity;
    uint32_t reconnectDelay = 1000;
    int reconnectAttempts = 0;

    // Message queue for offline messages
    std::queue<MqttMessage> pendingMessages;
    static constexpr size_t MAX_PENDING_MESSAGES = 1000;

    // Subscription tracking
    std::map<std::string, MqttQoS> subscriptions;

    // Packet ID management
    uint16_t nextPacketId = 1;

    // Pending acknowledgments
    std::map<uint16_t, PublishCallback> pendingAcks;

    uint16_t getNextPacketId() {
        uint16_t id = nextPacketId++;
        if (nextPacketId == 0) nextPacketId = 1;
        return id;
    }
};

// ============================================================================
// Constructor / Destructor
// ============================================================================

MqttTransport::MqttTransport(const MqttConfig& config)
    : impl_(std::make_unique<Impl>())
    , config_(config) {
}

MqttTransport::~MqttTransport() {
    if (isConnected()) {
        disconnect(false);
    }
}

// ============================================================================
// Connection Management
// ============================================================================

MqttResult MqttTransport::connect() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (state_ == MqttConnectionState::CONNECTED) {
        return MqttResult::SUCCESS;
    }

    setState(MqttConnectionState::CONNECTING);

    // Validate configuration
    if (config_.brokerUri.empty()) {
        setState(MqttConnectionState::ERROR, MqttResult::ERROR_INVALID_PARAMETER);
        return MqttResult::ERROR_INVALID_PARAMETER;
    }

    if (config_.clientId.empty()) {
        setState(MqttConnectionState::ERROR, MqttResult::ERROR_INVALID_PARAMETER);
        return MqttResult::ERROR_INVALID_PARAMETER;
    }

    // Initialize connection (platform-specific implementation would go here)
    // For now, we simulate a successful connection

    impl_->connected = true;
    impl_->lastActivity = std::chrono::steady_clock::now();
    impl_->reconnectDelay = config_.reconnectMinDelay;
    impl_->reconnectAttempts = 0;

    setState(MqttConnectionState::CONNECTED);

    // Subscribe to LwM2M topics automatically
    subscribeLwM2MTopics();

    return MqttResult::SUCCESS;
}

MqttResult MqttTransport::disconnect(bool sendWill) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (state_ == MqttConnectionState::DISCONNECTED) {
        return MqttResult::SUCCESS;
    }

    setState(MqttConnectionState::DISCONNECTING);

    // Send DISCONNECT packet (platform-specific)
    // If sendWill is false, send clean disconnect

    impl_->connected = false;
    impl_->subscriptions.clear();

    setState(MqttConnectionState::DISCONNECTED);

    return MqttResult::SUCCESS;
}

bool MqttTransport::isConnected() const {
    return state_ == MqttConnectionState::CONNECTED && impl_->connected;
}

MqttConnectionState MqttTransport::getState() const {
    return state_;
}

MqttResult MqttTransport::process(uint32_t timeoutMs) {
    if (!isConnected()) {
        // Handle reconnection if enabled
        if (config_.autoReconnect && state_ != MqttConnectionState::CONNECTING) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - impl_->lastActivity).count();

            if (elapsed >= impl_->reconnectDelay) {
                setState(MqttConnectionState::RECONNECTING);
                auto result = connect();
                if (result != MqttResult::SUCCESS) {
                    // Exponential backoff
                    impl_->reconnectDelay = std::min(
                        impl_->reconnectDelay * 2,
                        config_.reconnectMaxDelay);
                    impl_->reconnectAttempts++;
                }
                impl_->lastActivity = now;
            }
        }
        return MqttResult::ERROR_NOT_CONNECTED;
    }

    // Process incoming messages (platform-specific)
    // Process pending acknowledgments
    // Send keep-alive if needed

    impl_->lastActivity = std::chrono::steady_clock::now();

    // Process pending messages queue
    while (!impl_->pendingMessages.empty() && isConnected()) {
        auto& msg = impl_->pendingMessages.front();
        auto result = publish(msg);
        if (result == MqttResult::SUCCESS) {
            impl_->pendingMessages.pop();
        } else {
            break;
        }
    }

    return MqttResult::SUCCESS;
}

// ============================================================================
// Topic Management
// ============================================================================

std::string MqttTransport::buildTopic(MessageDirection direction,
                                       const std::string& operation) const {
    std::ostringstream topic;
    topic << config_.topicPrefix << "/";
    topic << directionToString(direction) << "/";
    topic << config_.endpointName;

    if (!operation.empty()) {
        topic << "/" << operation;
    }

    return topic.str();
}

MqttResult MqttTransport::subscribeLwM2MTopics() {
    if (!isConnected()) {
        return MqttResult::ERROR_NOT_CONNECTED;
    }

    MqttResult result = MqttResult::SUCCESS;

    // Subscribe to Registration Directory responses
    std::string rdTopic = buildTopic(MessageDirection::RD, "#");
    result = subscribe(rdTopic, MqttQoS::AT_LEAST_ONCE);
    if (result != MqttResult::SUCCESS) return result;

    // Subscribe to Bootstrap channel
    std::string bsTopic = buildTopic(MessageDirection::BS, "#");
    result = subscribe(bsTopic, MqttQoS::AT_LEAST_ONCE);
    if (result != MqttResult::SUCCESS) return result;

    // Subscribe to Data/Push channel (server operations)
    std::string dpTopic = buildTopic(MessageDirection::DP, "#");
    result = subscribe(dpTopic, MqttQoS::AT_LEAST_ONCE);
    if (result != MqttResult::SUCCESS) return result;

    return MqttResult::SUCCESS;
}

MqttResult MqttTransport::subscribe(const std::string& topic, MqttQoS qos) {
    if (!isConnected()) {
        return MqttResult::ERROR_NOT_CONNECTED;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    // Platform-specific SUBSCRIBE implementation would go here
    impl_->subscriptions[topic] = qos;

    return MqttResult::SUCCESS;
}

MqttResult MqttTransport::unsubscribe(const std::string& topic) {
    if (!isConnected()) {
        return MqttResult::ERROR_NOT_CONNECTED;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    // Platform-specific UNSUBSCRIBE implementation would go here
    impl_->subscriptions.erase(topic);

    return MqttResult::SUCCESS;
}

// ============================================================================
// LwM2M Operations
// ============================================================================

MqttResult MqttTransport::sendRegistration(const LwM2MMessage& message) {
    LwM2MMessage regMsg = message;
    regMsg.operation = LwM2MOperation::REGISTER;

    std::vector<uint8_t> payload;
    if (!encodeCBOR(regMsg, payload)) {
        return MqttResult::ERROR_INTERNAL;
    }

    std::string topic = buildTopic(MessageDirection::RD);
    return publish(topic, payload, MqttQoS::AT_LEAST_ONCE);
}

MqttResult MqttTransport::sendUpdate(const LwM2MMessage& message) {
    LwM2MMessage updMsg = message;
    updMsg.operation = LwM2MOperation::UPDATE;

    std::vector<uint8_t> payload;
    if (!encodeCBOR(updMsg, payload)) {
        return MqttResult::ERROR_INTERNAL;
    }

    std::string topic = buildTopic(MessageDirection::RD, "upd");
    return publish(topic, payload, MqttQoS::AT_LEAST_ONCE);
}

MqttResult MqttTransport::sendDeregistration() {
    LwM2MMessage deregMsg;
    deregMsg.operation = LwM2MOperation::DEREGISTER;
    deregMsg.endpointName = config_.endpointName;

    std::vector<uint8_t> payload;
    if (!encodeCBOR(deregMsg, payload)) {
        return MqttResult::ERROR_INTERNAL;
    }

    std::string topic = buildTopic(MessageDirection::RD, "dereg");
    return publish(topic, payload, MqttQoS::AT_LEAST_ONCE);
}

MqttResult MqttTransport::sendBootstrapRequest(const LwM2MMessage& message) {
    LwM2MMessage bsMsg = message;
    bsMsg.operation = LwM2MOperation::BOOTSTRAP_REQUEST;

    std::vector<uint8_t> payload;
    if (!encodeCBOR(bsMsg, payload)) {
        return MqttResult::ERROR_INTERNAL;
    }

    std::string topic = buildTopic(MessageDirection::BS, "req");
    return publish(topic, payload, MqttQoS::AT_LEAST_ONCE);
}

MqttResult MqttTransport::sendNotification(const LwM2MMessage& message) {
    LwM2MMessage notifyMsg = message;
    notifyMsg.operation = LwM2MOperation::NOTIFY;
    notifyMsg.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    std::vector<uint8_t> payload;
    if (!encodeCBOR(notifyMsg, payload)) {
        return MqttResult::ERROR_INTERNAL;
    }

    std::string topic = buildTopic(MessageDirection::DP, "notify");
    MqttQoS qos = getRecommendedQoS(LwM2MOperation::NOTIFY);
    return publish(topic, payload, qos);
}

MqttResult MqttTransport::sendData(const LwM2MMessage& message) {
    LwM2MMessage dataMsg = message;
    dataMsg.operation = LwM2MOperation::SEND;
    dataMsg.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    std::vector<uint8_t> payload;
    if (!encodeCBOR(dataMsg, payload)) {
        return MqttResult::ERROR_INTERNAL;
    }

    std::string topic = buildTopic(MessageDirection::DP, "data");
    return publish(topic, payload, MqttQoS::AT_LEAST_ONCE);
}

MqttResult MqttTransport::sendResponse(const LwM2MMessage& message) {
    std::vector<uint8_t> payload;
    if (!encodeCBOR(message, payload)) {
        return MqttResult::ERROR_INTERNAL;
    }

    std::string topic = buildTopic(MessageDirection::DP, "resp");
    return publish(topic, payload, MqttQoS::AT_LEAST_ONCE);
}

// ============================================================================
// Raw MQTT Operations
// ============================================================================

MqttResult MqttTransport::publish(const MqttMessage& message,
                                   PublishCallback callback) {
    if (!isConnected()) {
        // Queue message if offline
        if (impl_->pendingMessages.size() < Impl::MAX_PENDING_MESSAGES) {
            impl_->pendingMessages.push(message);
            return MqttResult::SUCCESS;
        }
        return MqttResult::ERROR_NOT_CONNECTED;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    // Platform-specific PUBLISH implementation would go here
    // For QoS 1+, track packet ID for acknowledgment

    uint16_t packetId = 0;
    if (message.qos != MqttQoS::AT_MOST_ONCE) {
        packetId = impl_->getNextPacketId();
        if (callback) {
            impl_->pendingAcks[packetId] = callback;
        }
    }

    // Update statistics
    messagesSent_++;
    bytesSent_ += message.payload.size();

    // Simulate successful publish
    if (callback && message.qos == MqttQoS::AT_MOST_ONCE) {
        callback(0, MqttResult::SUCCESS);
    }

    return MqttResult::SUCCESS;
}

MqttResult MqttTransport::publish(const std::string& topic,
                                   const std::vector<uint8_t>& payload,
                                   MqttQoS qos,
                                   bool retain) {
    MqttMessage msg;
    msg.topic = topic;
    msg.payload = payload;
    msg.qos = qos;
    msg.retain = retain;
    return publish(msg);
}

// ============================================================================
// CBOR Encoding/Decoding (OMA Section 8.7)
// ============================================================================

// Helper functions for CBOR encoding
namespace {

void encodeUint(std::vector<uint8_t>& output, uint8_t majorType, uint64_t value) {
    if (value < 24) {
        output.push_back(majorType | static_cast<uint8_t>(value));
    } else if (value <= 0xFF) {
        output.push_back(majorType | cbor::UINT_1BYTE);
        output.push_back(static_cast<uint8_t>(value));
    } else if (value <= 0xFFFF) {
        output.push_back(majorType | cbor::UINT_2BYTE);
        output.push_back(static_cast<uint8_t>(value >> 8));
        output.push_back(static_cast<uint8_t>(value));
    } else if (value <= 0xFFFFFFFF) {
        output.push_back(majorType | cbor::UINT_4BYTE);
        output.push_back(static_cast<uint8_t>(value >> 24));
        output.push_back(static_cast<uint8_t>(value >> 16));
        output.push_back(static_cast<uint8_t>(value >> 8));
        output.push_back(static_cast<uint8_t>(value));
    } else {
        output.push_back(majorType | cbor::UINT_8BYTE);
        output.push_back(static_cast<uint8_t>(value >> 56));
        output.push_back(static_cast<uint8_t>(value >> 48));
        output.push_back(static_cast<uint8_t>(value >> 40));
        output.push_back(static_cast<uint8_t>(value >> 32));
        output.push_back(static_cast<uint8_t>(value >> 24));
        output.push_back(static_cast<uint8_t>(value >> 16));
        output.push_back(static_cast<uint8_t>(value >> 8));
        output.push_back(static_cast<uint8_t>(value));
    }
}

void encodeInt(std::vector<uint8_t>& output, int64_t value) {
    if (value >= 0) {
        encodeUint(output, 0x00, static_cast<uint64_t>(value));
    } else {
        encodeUint(output, cbor::NEGINT_BASE, static_cast<uint64_t>(-1 - value));
    }
}

void encodeString(std::vector<uint8_t>& output, const std::string& value) {
    encodeUint(output, cbor::TEXT_BASE, value.length());
    output.insert(output.end(), value.begin(), value.end());
}

void encodeBytes(std::vector<uint8_t>& output, const std::vector<uint8_t>& value) {
    encodeUint(output, cbor::BYTES_BASE, value.size());
    output.insert(output.end(), value.begin(), value.end());
}

void encodeBool(std::vector<uint8_t>& output, bool value) {
    output.push_back(value ? cbor::TRUE_VAL : cbor::FALSE_VAL);
}

void encodeMapStart(std::vector<uint8_t>& output, size_t count) {
    encodeUint(output, cbor::MAP_BASE, count);
}

void encodeArrayStart(std::vector<uint8_t>& output, size_t count) {
    encodeUint(output, cbor::ARRAY_BASE, count);
}

} // anonymous namespace

bool MqttTransport::encodeCBOR(const LwM2MMessage& message,
                                std::vector<uint8_t>& output) {
    output.clear();

    // Count number of map entries
    size_t mapSize = 1;  // operation is always present
    if (message.token != 0) mapSize++;
    if (!message.path.empty()) mapSize++;
    if (message.contentFormat != 0) mapSize++;
    if (!message.payload.empty()) mapSize++;
    if (message.responseCode != 0) mapSize++;
    if (!message.errorMessage.empty()) mapSize++;
    if (!message.endpointName.empty()) mapSize++;
    if (message.lifetime != 0) mapSize++;
    if (!message.lwm2mVersion.empty()) mapSize++;
    if (!message.binding.empty()) mapSize++;
    if (message.queueMode) mapSize++;
    if (!message.objectLinks.empty()) mapSize++;
    if (!message.altPath.empty()) mapSize++;
    if (message.observeSequence != 0) mapSize++;
    if (message.timestamp != 0) mapSize++;

    // Encode map header
    encodeMapStart(output, mapSize);

    // Key 0: Operation (mandatory)
    encodeInt(output, 0);
    encodeInt(output, static_cast<int64_t>(message.operation));

    // Key 1: Token
    if (message.token != 0) {
        encodeInt(output, 1);
        encodeUint(output, 0x00, message.token);
    }

    // Key 2: Path
    if (!message.path.empty()) {
        encodeInt(output, 2);
        encodeString(output, message.path);
    }

    // Key 3: Content-Format
    if (message.contentFormat != 0) {
        encodeInt(output, 3);
        encodeInt(output, message.contentFormat);
    }

    // Key 4: Payload
    if (!message.payload.empty()) {
        encodeInt(output, 4);
        encodeBytes(output, message.payload);
    }

    // Key 5: Response Code
    if (message.responseCode != 0) {
        encodeInt(output, 5);
        encodeInt(output, message.responseCode);
    }

    // Key 6: Error Message
    if (!message.errorMessage.empty()) {
        encodeInt(output, 6);
        encodeString(output, message.errorMessage);
    }

    // Key 10: Endpoint Name
    if (!message.endpointName.empty()) {
        encodeInt(output, 10);
        encodeString(output, message.endpointName);
    }

    // Key 11: Lifetime
    if (message.lifetime != 0) {
        encodeInt(output, 11);
        encodeUint(output, 0x00, message.lifetime);
    }

    // Key 12: LwM2M Version
    if (!message.lwm2mVersion.empty()) {
        encodeInt(output, 12);
        encodeString(output, message.lwm2mVersion);
    }

    // Key 13: Binding
    if (!message.binding.empty()) {
        encodeInt(output, 13);
        encodeString(output, message.binding);
    }

    // Key 14: Queue Mode
    if (message.queueMode) {
        encodeInt(output, 14);
        encodeBool(output, message.queueMode);
    }

    // Key 15: Object Links
    if (!message.objectLinks.empty()) {
        encodeInt(output, 15);
        encodeArrayStart(output, message.objectLinks.size());
        for (const auto& link : message.objectLinks) {
            encodeString(output, link);
        }
    }

    // Key 16: Alt Path
    if (!message.altPath.empty()) {
        encodeInt(output, 16);
        encodeString(output, message.altPath);
    }

    // Key 20: Observe Sequence
    if (message.observeSequence != 0) {
        encodeInt(output, 20);
        encodeUint(output, 0x00, message.observeSequence);
    }

    // Key 21: Timestamp
    if (message.timestamp != 0) {
        encodeInt(output, 21);
        encodeUint(output, 0x00, message.timestamp);
    }

    return true;
}

bool MqttTransport::decodeCBOR(const std::vector<uint8_t>& data,
                                LwM2MMessage& message) {
    if (data.empty()) {
        return false;
    }

    size_t pos = 0;

    // Helper lambda to decode unsigned integer
    auto decodeUintValue = [&data, &pos](uint8_t additionalInfo) -> uint64_t {
        if (additionalInfo < 24) {
            return additionalInfo;
        } else if (additionalInfo == 24 && pos < data.size()) {
            return data[pos++];
        } else if (additionalInfo == 25 && pos + 1 < data.size()) {
            uint64_t val = (static_cast<uint64_t>(data[pos]) << 8) |
                           data[pos + 1];
            pos += 2;
            return val;
        } else if (additionalInfo == 26 && pos + 3 < data.size()) {
            uint64_t val = (static_cast<uint64_t>(data[pos]) << 24) |
                           (static_cast<uint64_t>(data[pos + 1]) << 16) |
                           (static_cast<uint64_t>(data[pos + 2]) << 8) |
                           data[pos + 3];
            pos += 4;
            return val;
        } else if (additionalInfo == 27 && pos + 7 < data.size()) {
            uint64_t val = (static_cast<uint64_t>(data[pos]) << 56) |
                           (static_cast<uint64_t>(data[pos + 1]) << 48) |
                           (static_cast<uint64_t>(data[pos + 2]) << 40) |
                           (static_cast<uint64_t>(data[pos + 3]) << 32) |
                           (static_cast<uint64_t>(data[pos + 4]) << 24) |
                           (static_cast<uint64_t>(data[pos + 5]) << 16) |
                           (static_cast<uint64_t>(data[pos + 6]) << 8) |
                           data[pos + 7];
            pos += 8;
            return val;
        }
        return 0;
    };

    // Check for map type
    if ((data[pos] & 0xe0) != cbor::MAP_BASE) {
        return false;
    }

    uint8_t additionalInfo = data[pos++] & 0x1f;
    uint64_t mapSize = decodeUintValue(additionalInfo);

    // Parse map entries
    for (uint64_t i = 0; i < mapSize && pos < data.size(); i++) {
        // Decode key (must be integer)
        uint8_t keyType = data[pos] & 0xe0;
        uint8_t keyInfo = data[pos++] & 0x1f;
        int64_t key;

        if (keyType == 0x00) {
            key = static_cast<int64_t>(decodeUintValue(keyInfo));
        } else if (keyType == cbor::NEGINT_BASE) {
            key = -1 - static_cast<int64_t>(decodeUintValue(keyInfo));
        } else {
            return false;  // Invalid key type
        }

        // Decode value based on key
        if (pos >= data.size()) return false;

        uint8_t valType = data[pos] & 0xe0;
        uint8_t valInfo = data[pos++] & 0x1f;

        switch (key) {
            case 0:  // Operation
                message.operation = static_cast<LwM2MOperation>(decodeUintValue(valInfo));
                break;
            case 1:  // Token
                message.token = decodeUintValue(valInfo);
                break;
            case 2:  // Path
                if (valType == cbor::TEXT_BASE) {
                    size_t len = decodeUintValue(valInfo);
                    if (pos + len <= data.size()) {
                        message.path = std::string(data.begin() + pos, data.begin() + pos + len);
                        pos += len;
                    }
                }
                break;
            case 3:  // Content-Format
                message.contentFormat = static_cast<uint16_t>(decodeUintValue(valInfo));
                break;
            case 4:  // Payload
                if (valType == cbor::BYTES_BASE) {
                    size_t len = decodeUintValue(valInfo);
                    if (pos + len <= data.size()) {
                        message.payload.assign(data.begin() + pos, data.begin() + pos + len);
                        pos += len;
                    }
                }
                break;
            case 5:  // Response Code
                message.responseCode = static_cast<uint8_t>(decodeUintValue(valInfo));
                break;
            case 6:  // Error Message
                if (valType == cbor::TEXT_BASE) {
                    size_t len = decodeUintValue(valInfo);
                    if (pos + len <= data.size()) {
                        message.errorMessage = std::string(data.begin() + pos, data.begin() + pos + len);
                        pos += len;
                    }
                }
                break;
            case 10:  // Endpoint Name
                if (valType == cbor::TEXT_BASE) {
                    size_t len = decodeUintValue(valInfo);
                    if (pos + len <= data.size()) {
                        message.endpointName = std::string(data.begin() + pos, data.begin() + pos + len);
                        pos += len;
                    }
                }
                break;
            case 11:  // Lifetime
                message.lifetime = static_cast<uint32_t>(decodeUintValue(valInfo));
                break;
            case 12:  // LwM2M Version
                if (valType == cbor::TEXT_BASE) {
                    size_t len = decodeUintValue(valInfo);
                    if (pos + len <= data.size()) {
                        message.lwm2mVersion = std::string(data.begin() + pos, data.begin() + pos + len);
                        pos += len;
                    }
                }
                break;
            case 13:  // Binding
                if (valType == cbor::TEXT_BASE) {
                    size_t len = decodeUintValue(valInfo);
                    if (pos + len <= data.size()) {
                        message.binding = std::string(data.begin() + pos, data.begin() + pos + len);
                        pos += len;
                    }
                }
                break;
            case 14:  // Queue Mode
                message.queueMode = (data[pos - 1] == cbor::TRUE_VAL);
                break;
            case 20:  // Observe Sequence
                message.observeSequence = static_cast<uint32_t>(decodeUintValue(valInfo));
                break;
            case 21:  // Timestamp
                message.timestamp = decodeUintValue(valInfo);
                break;
            default:
                // Skip unknown keys
                break;
        }
    }

    return true;
}

// ============================================================================
// Callbacks
// ============================================================================

void MqttTransport::setMessageCallback(MessageCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    messageCallback_ = std::move(callback);
}

void MqttTransport::setLwM2MCallback(LwM2MCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    lwm2mCallback_ = std::move(callback);
}

void MqttTransport::setConnectionCallback(ConnectionCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    connectionCallback_ = std::move(callback);
}

// ============================================================================
// Configuration
// ============================================================================

void MqttTransport::setConfig(const MqttConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

const MqttConfig& MqttTransport::getConfig() const {
    return config_;
}

std::string MqttTransport::getEndpointName() const {
    return config_.endpointName;
}

void MqttTransport::setEndpointName(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.endpointName = name;
}

// ============================================================================
// Statistics
// ============================================================================

uint64_t MqttTransport::getMessagesSent() const {
    return messagesSent_;
}

uint64_t MqttTransport::getMessagesReceived() const {
    return messagesReceived_;
}

uint64_t MqttTransport::getBytesSent() const {
    return bytesSent_;
}

uint64_t MqttTransport::getBytesReceived() const {
    return bytesReceived_;
}

void MqttTransport::resetStatistics() {
    messagesSent_ = 0;
    messagesReceived_ = 0;
    bytesSent_ = 0;
    bytesReceived_ = 0;
}

// ============================================================================
// QoS Mapping
// ============================================================================

MqttQoS MqttTransport::getRecommendedQoS(LwM2MOperation operation) {
    switch (operation) {
        // Registration operations - need guaranteed delivery
        case LwM2MOperation::REGISTER:
        case LwM2MOperation::UPDATE:
        case LwM2MOperation::DEREGISTER:
            return MqttQoS::AT_LEAST_ONCE;

        // Bootstrap operations - critical, use QoS 1
        case LwM2MOperation::BOOTSTRAP_REQUEST:
        case LwM2MOperation::BOOTSTRAP_WRITE:
        case LwM2MOperation::BOOTSTRAP_DISCOVER:
        case LwM2MOperation::BOOTSTRAP_DELETE:
        case LwM2MOperation::BOOTSTRAP_FINISH:
            return MqttQoS::AT_LEAST_ONCE;

        // Device management operations - QoS 1
        case LwM2MOperation::READ:
        case LwM2MOperation::WRITE:
        case LwM2MOperation::EXECUTE:
        case LwM2MOperation::CREATE:
        case LwM2MOperation::DELETE:
        case LwM2MOperation::DISCOVER:
        case LwM2MOperation::WRITE_ATTRIBUTES:
            return MqttQoS::AT_LEAST_ONCE;

        // Observe operations - configurable, default QoS 1
        case LwM2MOperation::OBSERVE:
        case LwM2MOperation::CANCEL_OBSERVE:
        case LwM2MOperation::OBSERVE_COMPOSITE:
            return MqttQoS::AT_LEAST_ONCE;

        // Notifications - configurable, can be QoS 0 for frequent updates
        case LwM2MOperation::NOTIFY:
            return MqttQoS::AT_MOST_ONCE;

        // Send operation - QoS 1
        case LwM2MOperation::SEND:
            return MqttQoS::AT_LEAST_ONCE;

        // Composite operations - QoS 1
        case LwM2MOperation::READ_COMPOSITE:
        case LwM2MOperation::WRITE_COMPOSITE:
            return MqttQoS::AT_LEAST_ONCE;

        default:
            return MqttQoS::AT_LEAST_ONCE;
    }
}

// ============================================================================
// Internal Methods
// ============================================================================

void MqttTransport::handleIncomingMessage(const MqttMessage& message) {
    messagesReceived_++;
    bytesReceived_ += message.payload.size();

    // Try to decode as LwM2M message
    LwM2MMessage lwm2mMsg;
    if (decodeCBOR(message.payload, lwm2mMsg)) {
        if (lwm2mCallback_) {
            lwm2mCallback_(lwm2mMsg);
        }
    }

    // Also invoke raw message callback
    if (messageCallback_) {
        messageCallback_(message);
    }
}

void MqttTransport::setState(MqttConnectionState newState, MqttResult reason) {
    MqttConnectionState oldState = state_.exchange(newState);

    if (oldState != newState && connectionCallback_) {
        connectionCallback_(newState, reason);
    }
}

std::string MqttTransport::directionToString(MessageDirection direction) const {
    switch (direction) {
        case MessageDirection::RD: return "rd";
        case MessageDirection::BS: return "bs";
        case MessageDirection::DP: return "dp";
        default: return "dp";
    }
}

// ============================================================================
// Factory
// ============================================================================

std::unique_ptr<MqttTransport> MqttTransportFactory::create(const MqttConfig& config) {
    return std::make_unique<MqttTransport>(config);
}

std::unique_ptr<MqttTransport> MqttTransportFactory::create(const std::string& brokerUri,
                                                             const std::string& clientId,
                                                             const std::string& endpointName) {
    MqttConfig config;
    config.brokerUri = brokerUri;
    config.clientId = clientId;
    config.endpointName = endpointName;
    return create(config);
}

bool MqttTransportFactory::isAvailable() {
#ifdef WITH_MQTT
    return true;
#else
    return true;  // Built-in implementation always available
#endif
}

std::vector<MqttProtocolVersion> MqttTransportFactory::getSupportedVersions() {
    return {
        MqttProtocolVersion::MQTT_3_1_1,
        MqttProtocolVersion::MQTT_5_0
    };
}

} // namespace transport
} // namespace lwm2m
