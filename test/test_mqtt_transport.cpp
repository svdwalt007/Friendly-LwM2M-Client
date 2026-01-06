/**
 * @file test_mqtt_transport.cpp
 * @brief Unit tests for LwM2M MQTT Transport Binding
 * 
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "transport/mqtt_transport.h"
#include <thread>
#include <chrono>

using namespace lwm2m::transport;

// ============================================================================
// MqttTopic Tests
// ============================================================================

class MqttTopicTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MqttTopicTest, ParseRegistrationTopic) {
    auto topic = MqttTopic::parse("lwm2m/rd/endpoint123", "lwm2m");
    
    ASSERT_TRUE(topic.has_value());
    EXPECT_EQ(topic->prefix, "lwm2m");
    EXPECT_EQ(topic->direction, "rd");
    EXPECT_EQ(topic->endpointName, "endpoint123");
    EXPECT_TRUE(topic->operation.empty());
}

TEST_F(MqttTopicTest, ParseBootstrapTopic) {
    auto topic = MqttTopic::parse("lwm2m/bs/device001/req", "lwm2m");
    
    ASSERT_TRUE(topic.has_value());
    EXPECT_EQ(topic->direction, "bs");
    EXPECT_EQ(topic->endpointName, "device001");
    EXPECT_EQ(topic->operation, "req");
}

TEST_F(MqttTopicTest, ParseDataPushTopic) {
    auto topic = MqttTopic::parse("lwm2m/dp/sensor42/notify", "lwm2m");
    
    ASSERT_TRUE(topic.has_value());
    EXPECT_EQ(topic->direction, "dp");
    EXPECT_EQ(topic->endpointName, "sensor42");
    EXPECT_EQ(topic->operation, "notify");
}

TEST_F(MqttTopicTest, ParseCustomPrefixTopic) {
    auto topic = MqttTopic::parse("custom/prefix/rd/mydevice", "custom/prefix");
    
    ASSERT_TRUE(topic.has_value());
    EXPECT_EQ(topic->direction, "rd");
    EXPECT_EQ(topic->endpointName, "mydevice");
}

TEST_F(MqttTopicTest, ParseInvalidDirection) {
    auto topic = MqttTopic::parse("lwm2m/invalid/endpoint", "lwm2m");
    EXPECT_FALSE(topic.has_value());
}

TEST_F(MqttTopicTest, ParseEmptyEndpoint) {
    auto topic = MqttTopic::parse("lwm2m/rd/", "lwm2m");
    EXPECT_FALSE(topic.has_value());
}

TEST_F(MqttTopicTest, BuildTopic) {
    MqttTopic topic;
    topic.prefix = "lwm2m";
    topic.direction = "rd";
    topic.endpointName = "device123";
    topic.operation = "resp";
    
    EXPECT_EQ(topic.toString(), "lwm2m/rd/device123/resp");
}

TEST_F(MqttTopicTest, BuildTopicNoOperation) {
    MqttTopic topic;
    topic.prefix = "lwm2m";
    topic.direction = "bs";
    topic.endpointName = "client01";
    
    EXPECT_EQ(topic.toString(), "lwm2m/bs/client01");
}

// ============================================================================
// MqttPayloadCodec Tests
// ============================================================================

class MqttPayloadCodecTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MqttPayloadCodecTest, EncodeCborBasicPayload) {
    LwM2MPayload payload;
    payload.operation = LwM2MOperation::REGISTER;
    payload.token = 12345;
    payload.path = "/3/0";
    
    auto encoded = MqttPayloadCodec::encodeCbor(payload);
    
    EXPECT_FALSE(encoded.empty());
    // CBOR map should start with 0xA* or 0xB*
    EXPECT_TRUE((encoded[0] & 0xE0) == 0xA0 || encoded[0] == 0xB8 || encoded[0] == 0xB9);
}

TEST_F(MqttPayloadCodecTest, DecodeCborBasicPayload) {
    // First encode
    LwM2MPayload original;
    original.operation = LwM2MOperation::READ;
    original.token = 54321;
    original.path = "/3/0/0";
    original.contentFormat = 110;
    
    auto encoded = MqttPayloadCodec::encodeCbor(original);
    
    // Then decode
    auto decoded = MqttPayloadCodec::decodeCbor(encoded);
    
    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded->operation, original.operation);
    EXPECT_EQ(decoded->token, original.token);
    EXPECT_EQ(decoded->path, original.path);
    EXPECT_EQ(decoded->contentFormat, original.contentFormat);
}

TEST_F(MqttPayloadCodecTest, EncodeJsonBasicPayload) {
    LwM2MPayload payload;
    payload.operation = LwM2MOperation::WRITE;
    payload.token = 99999;
    payload.path = "/5/0/1";
    
    std::string json = MqttPayloadCodec::encodeJson(payload);
    
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("\"op\":"), std::string::npos);
    EXPECT_NE(json.find("\"tk\":99999"), std::string::npos);
    EXPECT_NE(json.find("\"path\":\"/5/0/1\""), std::string::npos);
}

TEST_F(MqttPayloadCodecTest, DecodeJsonBasicPayload) {
    std::string json = R"({"op":22,"tk":11111,"path":"/3/0/9"})";
    
    auto decoded = MqttPayloadCodec::decodeJson(json);
    
    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded->operation, LwM2MOperation::EXECUTE);
    EXPECT_EQ(decoded->token, 11111u);
    EXPECT_EQ(decoded->path, "/3/0/9");
}

TEST_F(MqttPayloadCodecTest, EncodeWithResponseCode) {
    LwM2MPayload payload;
    payload.operation = LwM2MOperation::READ;
    payload.token = 1;
    payload.responseCode = LwM2MResponseCode::CONTENT;
    payload.data = {0x01, 0x02, 0x03, 0x04};
    
    auto encoded = MqttPayloadCodec::encodeCbor(payload);
    auto decoded = MqttPayloadCodec::decodeCbor(encoded);
    
    ASSERT_TRUE(decoded.has_value());
    EXPECT_TRUE(decoded->responseCode.has_value());
    EXPECT_EQ(*decoded->responseCode, LwM2MResponseCode::CONTENT);
}

TEST_F(MqttPayloadCodecTest, EncodeWithOptions) {
    LwM2MPayload payload;
    payload.operation = LwM2MOperation::REGISTER;
    payload.token = 1;
    payload.options["ep"] = "mydevice";
    payload.options["lt"] = "86400";
    
    std::string json = MqttPayloadCodec::encodeJson(payload);
    
    EXPECT_NE(json.find("\"opt\":"), std::string::npos);
    EXPECT_NE(json.find("\"ep\":\"mydevice\""), std::string::npos);
}

TEST_F(MqttPayloadCodecTest, DecodeCborInvalidData) {
    std::vector<uint8_t> invalid = {0xFF, 0xFF, 0xFF};
    auto decoded = MqttPayloadCodec::decodeCbor(invalid);
    EXPECT_FALSE(decoded.has_value());
}

TEST_F(MqttPayloadCodecTest, DecodeEmptyData) {
    std::vector<uint8_t> empty;
    auto decoded = MqttPayloadCodec::decodeCbor(empty);
    EXPECT_FALSE(decoded.has_value());
}

// ============================================================================
// MqttServerConfig Tests
// ============================================================================

class MqttServerConfigTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MqttServerConfigTest, DefaultValues) {
    MqttServerConfig config;
    
    EXPECT_EQ(config.brokerPort, 8883);
    EXPECT_TRUE(config.cleanSession);
    EXPECT_EQ(config.keepAlive, 60);
    EXPECT_EQ(config.defaultQoS, MqttQoS::AT_LEAST_ONCE);
    EXPECT_EQ(config.version, MqttVersion::MQTT_3_1_1);
    EXPECT_FALSE(config.retainMessages);
}

TEST_F(MqttServerConfigTest, CustomConfiguration) {
    MqttServerConfig config;
    config.brokerUri = "mqtt.example.com";
    config.brokerPort = 1883;
    config.clientId = "lwm2m_client_001";
    config.cleanSession = false;
    config.keepAlive = 120;
    config.version = MqttVersion::MQTT_5_0;
    config.sessionExpiry = 3600;
    
    EXPECT_EQ(config.brokerUri, "mqtt.example.com");
    EXPECT_EQ(config.brokerPort, 1883);
    EXPECT_EQ(config.clientId, "lwm2m_client_001");
    EXPECT_FALSE(config.cleanSession);
    EXPECT_EQ(config.keepAlive, 120);
    EXPECT_EQ(config.version, MqttVersion::MQTT_5_0);
    EXPECT_EQ(config.sessionExpiry, 3600u);
}

// ============================================================================
// MqttCoseConfig Tests
// ============================================================================

class MqttCoseConfigTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MqttCoseConfigTest, DefaultValues) {
    MqttCoseConfig config;
    
    EXPECT_EQ(config.securityMode, MqttSecurityMode::CERTIFICATE);
    EXPECT_TRUE(config.verifyPeer);
    EXPECT_TRUE(config.publicKey.empty());
    EXPECT_TRUE(config.privateKey.empty());
}

TEST_F(MqttCoseConfigTest, PskConfiguration) {
    MqttCoseConfig config;
    config.securityMode = MqttSecurityMode::PSK;
    config.publicKey = {'i', 'd', 'e', 'n', 't', 'i', 't', 'y'};
    config.privateKey = {'s', 'e', 'c', 'r', 'e', 't'};
    
    EXPECT_EQ(config.securityMode, MqttSecurityMode::PSK);
    EXPECT_EQ(config.publicKey.size(), 8u);
    EXPECT_EQ(config.privateKey.size(), 6u);
}

// ============================================================================
// MqttTransport Tests
// ============================================================================

class MqttTransportTest : public ::testing::Test {
protected:
    void SetUp() override {
        MqttTransport::Config config;
        config.server.brokerUri = "localhost";
        config.server.brokerPort = 1883;
        config.server.clientId = "test_client";
        config.server.topicPrefix = "lwm2m";
        config.security.securityMode = MqttSecurityMode::NO_SEC;
        config.autoReconnect = false;
        
        transport_ = std::make_unique<MqttTransport>(config);
    }
    
    void TearDown() override {
        transport_.reset();
    }
    
    std::unique_ptr<MqttTransport> transport_;
};

TEST_F(MqttTransportTest, InitialState) {
    EXPECT_EQ(transport_->getConnectionState(), MqttConnectionState::DISCONNECTED);
    EXPECT_FALSE(transport_->isConnected());
}

TEST_F(MqttTransportTest, ConnectDisconnect) {
    // This test may fail if there's no MQTT broker running
    // It's primarily to test the API
    auto result = transport_->connect();
    // Not asserting success as broker may not be available
    
    transport_->disconnect();
    EXPECT_EQ(transport_->getConnectionState(), MqttConnectionState::DISCONNECTED);
}

TEST_F(MqttTransportTest, TokenGeneration) {
    uint32_t token1 = transport_->nextToken();
    uint32_t token2 = transport_->nextToken();
    uint32_t token3 = transport_->nextToken();
    
    EXPECT_NE(token1, token2);
    EXPECT_NE(token2, token3);
    EXPECT_EQ(token2, token1 + 1);
    EXPECT_EQ(token3, token2 + 1);
}

TEST_F(MqttTransportTest, StatisticsInitialValues) {
    auto stats = transport_->getStatistics();
    
    EXPECT_EQ(stats.messagesSent, 0u);
    EXPECT_EQ(stats.messagesReceived, 0u);
    EXPECT_EQ(stats.bytesOut, 0u);
    EXPECT_EQ(stats.bytesIn, 0u);
    EXPECT_EQ(stats.reconnections, 0u);
    EXPECT_EQ(stats.errors, 0u);
}

TEST_F(MqttTransportTest, ConnectionCallback) {
    bool callbackInvoked = false;
    MqttConnectionState lastState = MqttConnectionState::DISCONNECTED;
    
    transport_->setConnectionCallback([&](MqttConnectionState state, const std::string&) {
        callbackInvoked = true;
        lastState = state;
    });
    
    transport_->connect();
    
    // Callback should be invoked during connect attempt
    // State depends on whether broker is available
}

TEST_F(MqttTransportTest, RegisterClient) {
    MqttRegistrationParams params;
    params.endpointName = "test_endpoint";
    params.lifetime = 86400;
    params.lwm2mVersion = "1.2";
    params.binding = "M";
    params.objectLinks = {"</3/0>", "</1/0>"};
    
    uint32_t token = transport_->registerClient(params);
    
    EXPECT_GT(token, 0u);
}

TEST_F(MqttTransportTest, SendNotification) {
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};
    
    uint32_t token = transport_->sendNotification("/3/0/0", data, 110, true);
    
    EXPECT_GT(token, 0u);
}

TEST_F(MqttTransportTest, SetCallbacks) {
    bool readCalled = false;
    bool writeCalled = false;
    bool executeCalled = false;
    
    transport_->onRead([&](const LwM2MPayload&, auto) { readCalled = true; });
    transport_->onWrite([&](const LwM2MPayload&, auto) { writeCalled = true; });
    transport_->onExecute([&](const LwM2MPayload&, auto) { executeCalled = true; });
    
    // Callbacks are set, would be invoked on incoming messages
    // Just verify no crash on setting
}

// ============================================================================
// MqttServerObject Tests
// ============================================================================

class MqttServerObjectTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MqttServerObjectTest, ObjectId) {
    EXPECT_EQ(MqttServerObject::OBJECT_ID, 24);
}

TEST_F(MqttServerObjectTest, ResourceIds) {
    using R = MqttServerObject::ResourceId;
    
    EXPECT_EQ(static_cast<uint16_t>(R::SHORT_SERVER_ID), 0);
    EXPECT_EQ(static_cast<uint16_t>(R::MQTT_BROKER_URI), 1);
    EXPECT_EQ(static_cast<uint16_t>(R::MQTT_BROKER_PORT), 2);
    EXPECT_EQ(static_cast<uint16_t>(R::CLIENT_IDENTIFIER), 3);
    EXPECT_EQ(static_cast<uint16_t>(R::CLEAN_SESSION), 4);
    EXPECT_EQ(static_cast<uint16_t>(R::KEEP_ALIVE), 5);
    EXPECT_EQ(static_cast<uint16_t>(R::QOS), 6);
    EXPECT_EQ(static_cast<uint16_t>(R::PROTOCOL_VERSION), 7);
}

// ============================================================================
// MqttCoseObject Tests
// ============================================================================

class MqttCoseObjectTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MqttCoseObjectTest, ObjectId) {
    EXPECT_EQ(MqttCoseObject::OBJECT_ID, 23);
}

TEST_F(MqttCoseObjectTest, ResourceIds) {
    using R = MqttCoseObject::ResourceId;
    
    EXPECT_EQ(static_cast<uint16_t>(R::SHORT_SERVER_ID), 0);
    EXPECT_EQ(static_cast<uint16_t>(R::SECURITY_MODE), 1);
    EXPECT_EQ(static_cast<uint16_t>(R::PUBLIC_KEY_OR_IDENTITY), 2);
    EXPECT_EQ(static_cast<uint16_t>(R::SECRET_KEY), 3);
    EXPECT_EQ(static_cast<uint16_t>(R::SERVER_PUBLIC_KEY), 4);
}

// ============================================================================
// Content Format Tests
// ============================================================================

class ContentFormatTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ContentFormatTest, ContentFormatIds) {
    EXPECT_EQ(MqttPayloadCodec::CONTENT_FORMAT_CBOR, 60);
    EXPECT_EQ(MqttPayloadCodec::CONTENT_FORMAT_JSON, 11543);
    EXPECT_EQ(MqttPayloadCodec::CONTENT_FORMAT_SENML_CBOR, 112);
    EXPECT_EQ(MqttPayloadCodec::CONTENT_FORMAT_SENML_JSON, 110);
}

// ============================================================================
// Registration Parameters Tests
// ============================================================================

class RegistrationParamsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(RegistrationParamsTest, DefaultValues) {
    MqttRegistrationParams params;
    
    EXPECT_EQ(params.lifetime, 86400u);
    EXPECT_EQ(params.lwm2mVersion, "1.2");
    EXPECT_EQ(params.binding, "M");
    EXPECT_FALSE(params.queueMode);
}

TEST_F(RegistrationParamsTest, CustomValues) {
    MqttRegistrationParams params;
    params.endpointName = "custom_device";
    params.lifetime = 3600;
    params.lwm2mVersion = "1.2.2";
    params.binding = "MQ";
    params.queueMode = true;
    params.objectLinks = {"</0>", "</1>", "</3>", "</5>"};
    
    EXPECT_EQ(params.endpointName, "custom_device");
    EXPECT_EQ(params.lifetime, 3600u);
    EXPECT_EQ(params.lwm2mVersion, "1.2.2");
    EXPECT_EQ(params.binding, "MQ");
    EXPECT_TRUE(params.queueMode);
    EXPECT_EQ(params.objectLinks.size(), 4u);
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
