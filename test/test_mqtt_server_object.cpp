/**
 * @file test_mqtt_server_object.cpp
 * @brief Unit tests for OMA LwM2M MQTT Server Object (ID: 24)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "objects/mqtt_server_object.h"

#include <string>

namespace lwm2m {
namespace objects {
namespace test {

class MqttServerObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
        obj_ = std::make_unique<MqttServerObject>(3);
        obj_->initialize();
    }

    std::unique_ptr<MqttServerObject> obj_;
};

// ============================================================================
// Object Identity
// ============================================================================

TEST_F(MqttServerObjectTest, ObjectIdIsTwentyFour) {
    EXPECT_EQ(MqttServerObject::getObjectId(), 24u);
    EXPECT_EQ(MQTT_SERVER_OBJECT_ID, 24u);
}

TEST_F(MqttServerObjectTest, InstanceIdMatchesConstructor) {
    EXPECT_EQ(obj_->getInstanceId(), 3u);
}

TEST_F(MqttServerObjectTest, DefaultsAfterInitialize) {
    EXPECT_EQ(obj_->getShortServerId(), 3u);
    EXPECT_EQ(obj_->getBrokerPort(), 8883u);
    EXPECT_EQ(obj_->getQoS(), MqttQosLevel::AT_LEAST_ONCE);
    EXPECT_EQ(obj_->getProtocolVersion(), MqttProtocol::MQTT_3_1_1);
    EXPECT_EQ(obj_->getTopicPrefix(), "lwm2m");
    EXPECT_TRUE(obj_->getBrokerUri().empty());
    EXPECT_TRUE(obj_->getClientId().empty());
}

// ============================================================================
// Resource Metadata
// ============================================================================

TEST_F(MqttServerObjectTest, PasswordIsNotReadable) {
    EXPECT_FALSE(MqttServerObject::isReadable(MqttServerResourceId::PASSWORD));
}

TEST_F(MqttServerObjectTest, AllOtherResourcesAreReadable) {
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::SHORT_SERVER_ID));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::BROKER_URI));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::BROKER_PORT));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::CLIENT_ID));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::CLEAN_SESSION));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::KEEP_ALIVE));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::QOS));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::PROTOCOL_VERSION));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::USERNAME));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::TOPIC_PREFIX));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::RETAIN));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::SESSION_EXPIRY));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::MAX_PACKET_SIZE));
    EXPECT_TRUE(MqttServerObject::isReadable(MqttServerResourceId::RECEIVE_MAXIMUM));
}

TEST_F(MqttServerObjectTest, ShortServerIdIsNotWritable) {
    EXPECT_FALSE(MqttServerObject::isWritable(MqttServerResourceId::SHORT_SERVER_ID));
}

TEST_F(MqttServerObjectTest, OtherResourcesAreWritable) {
    EXPECT_TRUE(MqttServerObject::isWritable(MqttServerResourceId::BROKER_URI));
    EXPECT_TRUE(MqttServerObject::isWritable(MqttServerResourceId::BROKER_PORT));
    EXPECT_TRUE(MqttServerObject::isWritable(MqttServerResourceId::CLIENT_ID));
    EXPECT_TRUE(MqttServerObject::isWritable(MqttServerResourceId::PASSWORD));
    EXPECT_TRUE(MqttServerObject::isWritable(MqttServerResourceId::USERNAME));
    EXPECT_TRUE(MqttServerObject::isWritable(MqttServerResourceId::QOS));
}

TEST_F(MqttServerObjectTest, MandatoryResources) {
    EXPECT_TRUE(MqttServerObject::isMandatory(MqttServerResourceId::SHORT_SERVER_ID));
    EXPECT_TRUE(MqttServerObject::isMandatory(MqttServerResourceId::BROKER_URI));
    EXPECT_TRUE(MqttServerObject::isMandatory(MqttServerResourceId::BROKER_PORT));
    EXPECT_TRUE(MqttServerObject::isMandatory(MqttServerResourceId::CLIENT_ID));
    EXPECT_FALSE(MqttServerObject::isMandatory(MqttServerResourceId::QOS));
    EXPECT_FALSE(MqttServerObject::isMandatory(MqttServerResourceId::PROTOCOL_VERSION));
    EXPECT_FALSE(MqttServerObject::isMandatory(MqttServerResourceId::USERNAME));
}

// ============================================================================
// Read Operations
// ============================================================================

TEST_F(MqttServerObjectTest, ReadIntegers) {
    int64_t value = -1;
    EXPECT_TRUE(obj_->read(MqttServerResourceId::SHORT_SERVER_ID, value));
    EXPECT_EQ(value, 3);
    EXPECT_TRUE(obj_->read(MqttServerResourceId::BROKER_PORT, value));
    EXPECT_EQ(value, 8883);
    EXPECT_TRUE(obj_->read(MqttServerResourceId::KEEP_ALIVE, value));
    EXPECT_EQ(value, 60);
    EXPECT_TRUE(obj_->read(MqttServerResourceId::QOS, value));
    EXPECT_EQ(value, static_cast<int64_t>(MqttQosLevel::AT_LEAST_ONCE));
    EXPECT_TRUE(obj_->read(MqttServerResourceId::PROTOCOL_VERSION, value));
    EXPECT_EQ(value, static_cast<int64_t>(MqttProtocol::MQTT_3_1_1));
    EXPECT_TRUE(obj_->read(MqttServerResourceId::SESSION_EXPIRY, value));
    EXPECT_EQ(value, 3600);
    EXPECT_TRUE(obj_->read(MqttServerResourceId::MAX_PACKET_SIZE, value));
    EXPECT_EQ(value, 268435456);
    EXPECT_TRUE(obj_->read(MqttServerResourceId::RECEIVE_MAXIMUM, value));
    EXPECT_EQ(value, 65535);
}

TEST_F(MqttServerObjectTest, ReadIntegerOnNonIntegerFails) {
    int64_t value = 0;
    EXPECT_FALSE(obj_->read(MqttServerResourceId::BROKER_URI, value));
    EXPECT_FALSE(obj_->read(MqttServerResourceId::CLEAN_SESSION, value));
    EXPECT_FALSE(obj_->read(MqttServerResourceId::USERNAME, value));
}

TEST_F(MqttServerObjectTest, ReadStrings) {
    std::string value = "preset";
    EXPECT_TRUE(obj_->read(MqttServerResourceId::BROKER_URI, value));
    EXPECT_TRUE(value.empty());
    value = "preset";
    EXPECT_TRUE(obj_->read(MqttServerResourceId::CLIENT_ID, value));
    EXPECT_TRUE(value.empty());
    value = "preset";
    EXPECT_TRUE(obj_->read(MqttServerResourceId::USERNAME, value));
    EXPECT_TRUE(value.empty());
    value = "preset";
    EXPECT_TRUE(obj_->read(MqttServerResourceId::TOPIC_PREFIX, value));
    EXPECT_EQ(value, "lwm2m");
}

TEST_F(MqttServerObjectTest, ReadStringOnNonStringFails) {
    std::string value;
    EXPECT_FALSE(obj_->read(MqttServerResourceId::BROKER_PORT, value));
    EXPECT_FALSE(obj_->read(MqttServerResourceId::CLEAN_SESSION, value));
}

TEST_F(MqttServerObjectTest, ReadBooleans) {
    bool value = false;
    EXPECT_TRUE(obj_->read(MqttServerResourceId::CLEAN_SESSION, value));
    EXPECT_TRUE(value);

    EXPECT_TRUE(obj_->read(MqttServerResourceId::RETAIN, value));
    EXPECT_FALSE(value);
}

TEST_F(MqttServerObjectTest, ReadBooleanOnNonBooleanFails) {
    bool value = false;
    EXPECT_FALSE(obj_->read(MqttServerResourceId::BROKER_URI, value));
    EXPECT_FALSE(obj_->read(MqttServerResourceId::QOS, value));
}

// ============================================================================
// Write - Strings
// ============================================================================

TEST_F(MqttServerObjectTest, WriteStrings) {
    EXPECT_TRUE(obj_->write(MqttServerResourceId::BROKER_URI, std::string{"mqtts://broker.example:8883"}));
    EXPECT_TRUE(obj_->write(MqttServerResourceId::CLIENT_ID, std::string{"device-001"}));
    EXPECT_TRUE(obj_->write(MqttServerResourceId::USERNAME, std::string{"sensor"}));
    EXPECT_TRUE(obj_->write(MqttServerResourceId::PASSWORD, std::string{"s3cret"}));
    EXPECT_TRUE(obj_->write(MqttServerResourceId::TOPIC_PREFIX, std::string{"factory/lwm2m"}));

    EXPECT_EQ(obj_->getBrokerUri(), "mqtts://broker.example:8883");
    EXPECT_EQ(obj_->getClientId(), "device-001");
    EXPECT_EQ(obj_->getTopicPrefix(), "factory/lwm2m");
    auto cfg = obj_->getConfig();
    EXPECT_EQ(cfg.password, "s3cret");
    EXPECT_EQ(cfg.username, "sensor");
}

TEST_F(MqttServerObjectTest, WriteStringOnNonStringFails) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::BROKER_PORT, std::string{"8883"}));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::CLEAN_SESSION, std::string{"true"}));
}

// ============================================================================
// Write - Integers
// ============================================================================

TEST_F(MqttServerObjectTest, WriteBrokerPortValid) {
    EXPECT_TRUE(obj_->write(MqttServerResourceId::BROKER_PORT, int64_t{1883}));
    EXPECT_EQ(obj_->getBrokerPort(), 1883u);
    EXPECT_TRUE(obj_->write(MqttServerResourceId::BROKER_PORT, int64_t{0}));
    EXPECT_EQ(obj_->getBrokerPort(), 0u);
    EXPECT_TRUE(obj_->write(MqttServerResourceId::BROKER_PORT, int64_t{65535}));
    EXPECT_EQ(obj_->getBrokerPort(), 65535u);
}

TEST_F(MqttServerObjectTest, WriteBrokerPortRejectsOutOfRange) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::BROKER_PORT, int64_t{-1}));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::BROKER_PORT, int64_t{65536}));
}

TEST_F(MqttServerObjectTest, WriteKeepAliveValid) {
    EXPECT_TRUE(obj_->write(MqttServerResourceId::KEEP_ALIVE, int64_t{120}));
    int64_t v;
    obj_->read(MqttServerResourceId::KEEP_ALIVE, v);
    EXPECT_EQ(v, 120);
}

TEST_F(MqttServerObjectTest, WriteKeepAliveRejectsOutOfRange) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::KEEP_ALIVE, int64_t{-5}));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::KEEP_ALIVE, int64_t{70000}));
}

TEST_F(MqttServerObjectTest, WriteQosValid) {
    EXPECT_TRUE(obj_->write(MqttServerResourceId::QOS, int64_t{0}));
    EXPECT_EQ(obj_->getQoS(), MqttQosLevel::AT_MOST_ONCE);
    EXPECT_TRUE(obj_->write(MqttServerResourceId::QOS, int64_t{1}));
    EXPECT_EQ(obj_->getQoS(), MqttQosLevel::AT_LEAST_ONCE);
    EXPECT_TRUE(obj_->write(MqttServerResourceId::QOS, int64_t{2}));
    EXPECT_EQ(obj_->getQoS(), MqttQosLevel::EXACTLY_ONCE);
}

TEST_F(MqttServerObjectTest, WriteQosRejectsOutOfRange) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::QOS, int64_t{-1}));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::QOS, int64_t{3}));
}

TEST_F(MqttServerObjectTest, WriteProtocolVersionAcceptsFourAndFive) {
    EXPECT_TRUE(obj_->write(MqttServerResourceId::PROTOCOL_VERSION, int64_t{4}));
    EXPECT_EQ(obj_->getProtocolVersion(), MqttProtocol::MQTT_3_1_1);
    EXPECT_TRUE(obj_->write(MqttServerResourceId::PROTOCOL_VERSION, int64_t{5}));
    EXPECT_EQ(obj_->getProtocolVersion(), MqttProtocol::MQTT_5_0);
}

TEST_F(MqttServerObjectTest, WriteProtocolVersionRejectsOthers) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::PROTOCOL_VERSION, int64_t{3}));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::PROTOCOL_VERSION, int64_t{0}));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::PROTOCOL_VERSION, int64_t{6}));
}

TEST_F(MqttServerObjectTest, WriteSessionExpiryRejectsNegative) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::SESSION_EXPIRY, int64_t{-1}));
    EXPECT_TRUE(obj_->write(MqttServerResourceId::SESSION_EXPIRY, int64_t{0}));
    EXPECT_TRUE(obj_->write(MqttServerResourceId::SESSION_EXPIRY, int64_t{86400}));
    int64_t v;
    obj_->read(MqttServerResourceId::SESSION_EXPIRY, v);
    EXPECT_EQ(v, 86400);
}

TEST_F(MqttServerObjectTest, WriteMaxPacketSizeRejectsNegative) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::MAX_PACKET_SIZE, int64_t{-1}));
    EXPECT_TRUE(obj_->write(MqttServerResourceId::MAX_PACKET_SIZE, int64_t{4096}));
}

TEST_F(MqttServerObjectTest, WriteReceiveMaximumRange) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::RECEIVE_MAXIMUM, int64_t{-1}));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::RECEIVE_MAXIMUM, int64_t{70000}));
    EXPECT_TRUE(obj_->write(MqttServerResourceId::RECEIVE_MAXIMUM, int64_t{1024}));
}

TEST_F(MqttServerObjectTest, WriteShortServerIdIsRejected) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::SHORT_SERVER_ID, int64_t{99}));
    EXPECT_EQ(obj_->getShortServerId(), 3u);
}

TEST_F(MqttServerObjectTest, WriteIntegerOnNonIntegerFails) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::BROKER_URI, int64_t{1}));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::CLEAN_SESSION, int64_t{1}));
}

// ============================================================================
// Write - Booleans
// ============================================================================

TEST_F(MqttServerObjectTest, WriteCleanSessionAndRetain) {
    EXPECT_TRUE(obj_->write(MqttServerResourceId::CLEAN_SESSION, false));
    EXPECT_TRUE(obj_->write(MqttServerResourceId::RETAIN, true));
    bool v;
    EXPECT_TRUE(obj_->read(MqttServerResourceId::CLEAN_SESSION, v));
    EXPECT_FALSE(v);
    EXPECT_TRUE(obj_->read(MqttServerResourceId::RETAIN, v));
    EXPECT_TRUE(v);
}

TEST_F(MqttServerObjectTest, WriteBooleanOnNonBooleanFails) {
    EXPECT_FALSE(obj_->write(MqttServerResourceId::BROKER_PORT, true));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::QOS, true));
}

// ============================================================================
// Configuration Operations
// ============================================================================

TEST_F(MqttServerObjectTest, GetAndSetConfig) {
    MqttServerConfig cfg;
    cfg.shortServerId = 50;
    cfg.brokerUri = "mqtt://test:1883";
    cfg.brokerPort = 1883;
    cfg.clientId = "abc";
    cfg.cleanSession = false;
    cfg.keepAlive = 30;
    cfg.qos = MqttQosLevel::EXACTLY_ONCE;
    cfg.protocolVersion = MqttProtocol::MQTT_5_0;
    cfg.username = "u";
    cfg.password = "p";
    cfg.topicPrefix = "iot";
    cfg.retain = true;
    cfg.sessionExpiry = 7200;

    obj_->setConfig(cfg);

    auto out = obj_->getConfig();
    EXPECT_EQ(out.shortServerId, 50u);
    EXPECT_EQ(out.brokerUri, "mqtt://test:1883");
    EXPECT_EQ(out.brokerPort, 1883u);
    EXPECT_EQ(out.qos, MqttQosLevel::EXACTLY_ONCE);
    EXPECT_EQ(out.protocolVersion, MqttProtocol::MQTT_5_0);
    EXPECT_EQ(out.topicPrefix, "iot");
    EXPECT_TRUE(out.retain);
    EXPECT_FALSE(out.cleanSession);
}

// ============================================================================
// Callbacks
// ============================================================================

TEST_F(MqttServerObjectTest, CallbackFiresOnSuccessfulWrite) {
    int callbackCount = 0;
    MqttServerResourceId lastResource = MqttServerResourceId::SHORT_SERVER_ID;
    uint16_t lastInstance = 0xFFFF;
    obj_->setConfigChangeCallback(
        [&](uint16_t inst, MqttServerResourceId rid) {
            ++callbackCount;
            lastResource = rid;
            lastInstance = inst;
        });

    obj_->write(MqttServerResourceId::BROKER_URI, std::string{"mqtt://x"});
    obj_->write(MqttServerResourceId::QOS, int64_t{2});
    obj_->write(MqttServerResourceId::RETAIN, true);

    EXPECT_EQ(callbackCount, 3);
    EXPECT_EQ(lastResource, MqttServerResourceId::RETAIN);
    EXPECT_EQ(lastInstance, 3u);
}

TEST_F(MqttServerObjectTest, CallbackDoesNotFireOnFailedWrite) {
    int callbackCount = 0;
    obj_->setConfigChangeCallback(
        [&](uint16_t, MqttServerResourceId) { ++callbackCount; });

    EXPECT_FALSE(obj_->write(MqttServerResourceId::SHORT_SERVER_ID, int64_t{1}));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::QOS, int64_t{99}));
    EXPECT_FALSE(obj_->write(MqttServerResourceId::PROTOCOL_VERSION, int64_t{6}));
    EXPECT_EQ(callbackCount, 0);
}

// ============================================================================
// Factory
// ============================================================================

TEST(MqttServerObjectFactoryTest, CreateDefault) {
    auto obj = MqttServerObjectFactory::create(8);
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->getInstanceId(), 8u);
    EXPECT_EQ(obj->getShortServerId(), 8u);
    EXPECT_EQ(obj->getBrokerPort(), 8883u);
}

TEST(MqttServerObjectFactoryTest, CreateFromConfig) {
    MqttServerConfig cfg;
    cfg.shortServerId = 21;
    cfg.brokerUri = "mqtts://prod:8883";
    cfg.brokerPort = 8883;
    cfg.clientId = "prod-client";
    cfg.protocolVersion = MqttProtocol::MQTT_5_0;

    auto obj = MqttServerObjectFactory::create(cfg, 2);
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->getInstanceId(), 2u);
    EXPECT_EQ(obj->getShortServerId(), 21u);
    EXPECT_EQ(obj->getBrokerUri(), "mqtts://prod:8883");
    EXPECT_EQ(obj->getProtocolVersion(), MqttProtocol::MQTT_5_0);
}

} // namespace test
} // namespace objects
} // namespace lwm2m
