/**
 * @file test_lwm2m_cose_object.cpp
 * @brief Unit tests for OMA LwM2M COSE Object (ID: 23)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "objects/lwm2m_cose_object.h"

#include <vector>
#include <string>

namespace lwm2m {
namespace objects {
namespace test {

class CoseObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
        obj_ = std::make_unique<Lwm2mCoseObject>(7);
        obj_->initialize();
    }

    std::unique_ptr<Lwm2mCoseObject> obj_;
};

// ============================================================================
// Object Identity
// ============================================================================

TEST_F(CoseObjectTest, ObjectIdIsTwentyThree) {
    EXPECT_EQ(Lwm2mCoseObject::getObjectId(), 23u);
    EXPECT_EQ(LWM2M_COSE_OBJECT_ID, 23u);
}

TEST_F(CoseObjectTest, InstanceIdMatchesConstructor) {
    EXPECT_EQ(obj_->getInstanceId(), 7u);
}

TEST_F(CoseObjectTest, DefaultConstructorUsesZero) {
    Lwm2mCoseObject defaultObj;
    EXPECT_EQ(defaultObj.getInstanceId(), 0u);
}

TEST_F(CoseObjectTest, InitialiseSetsDefaults) {
    EXPECT_EQ(obj_->getShortServerId(), 7u);
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::NONE);
    EXPECT_TRUE(obj_->isVerifyPeerEnabled());
}

// ============================================================================
// Resource Metadata
// ============================================================================

TEST_F(CoseObjectTest, SecretKeyIsNotReadable) {
    EXPECT_FALSE(Lwm2mCoseObject::isReadable(CoseResourceId::SECRET_KEY));
    EXPECT_FALSE(Lwm2mCoseObject::isReadable(CoseResourceId::TOKEN));
}

TEST_F(CoseObjectTest, OtherResourcesAreReadable) {
    EXPECT_TRUE(Lwm2mCoseObject::isReadable(CoseResourceId::SHORT_SERVER_ID));
    EXPECT_TRUE(Lwm2mCoseObject::isReadable(CoseResourceId::SECURITY_MODE));
    EXPECT_TRUE(Lwm2mCoseObject::isReadable(CoseResourceId::PUBLIC_KEY_OR_IDENTITY));
    EXPECT_TRUE(Lwm2mCoseObject::isReadable(CoseResourceId::SERVER_PUBLIC_KEY));
    EXPECT_TRUE(Lwm2mCoseObject::isReadable(CoseResourceId::CERTIFICATE_CHAIN));
    EXPECT_TRUE(Lwm2mCoseObject::isReadable(CoseResourceId::CA_CERTIFICATE));
    EXPECT_TRUE(Lwm2mCoseObject::isReadable(CoseResourceId::SNI));
    EXPECT_TRUE(Lwm2mCoseObject::isReadable(CoseResourceId::VERIFY_PEER));
    EXPECT_TRUE(Lwm2mCoseObject::isReadable(CoseResourceId::ALPN));
    EXPECT_TRUE(Lwm2mCoseObject::isReadable(CoseResourceId::TOKEN_ENDPOINT));
}

TEST_F(CoseObjectTest, ShortServerIdIsNotWritable) {
    EXPECT_FALSE(Lwm2mCoseObject::isWritable(CoseResourceId::SHORT_SERVER_ID));
}

TEST_F(CoseObjectTest, OtherResourcesAreWritable) {
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::SECURITY_MODE));
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::PUBLIC_KEY_OR_IDENTITY));
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::SECRET_KEY));
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::SERVER_PUBLIC_KEY));
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::CERTIFICATE_CHAIN));
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::CA_CERTIFICATE));
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::SNI));
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::VERIFY_PEER));
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::ALPN));
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::TOKEN));
    EXPECT_TRUE(Lwm2mCoseObject::isWritable(CoseResourceId::TOKEN_ENDPOINT));
}

TEST_F(CoseObjectTest, MandatoryResources) {
    EXPECT_TRUE(Lwm2mCoseObject::isMandatory(CoseResourceId::SHORT_SERVER_ID));
    EXPECT_TRUE(Lwm2mCoseObject::isMandatory(CoseResourceId::SECURITY_MODE));
    EXPECT_FALSE(Lwm2mCoseObject::isMandatory(CoseResourceId::PUBLIC_KEY_OR_IDENTITY));
    EXPECT_FALSE(Lwm2mCoseObject::isMandatory(CoseResourceId::SECRET_KEY));
    EXPECT_FALSE(Lwm2mCoseObject::isMandatory(CoseResourceId::SNI));
    EXPECT_FALSE(Lwm2mCoseObject::isMandatory(CoseResourceId::VERIFY_PEER));
}

// ============================================================================
// Read - Integer
// ============================================================================

TEST_F(CoseObjectTest, ReadShortServerId) {
    int64_t value = -1;
    EXPECT_TRUE(obj_->read(CoseResourceId::SHORT_SERVER_ID, value));
    EXPECT_EQ(value, 7);
}

TEST_F(CoseObjectTest, ReadSecurityModeDefaultsToNone) {
    int64_t value = -1;
    EXPECT_TRUE(obj_->read(CoseResourceId::SECURITY_MODE, value));
    EXPECT_EQ(value, static_cast<int64_t>(CoseSecurityMode::NONE));
}

TEST_F(CoseObjectTest, ReadIntegerOnNonIntegerResourceFails) {
    int64_t value = 0;
    EXPECT_FALSE(obj_->read(CoseResourceId::SNI, value));
    EXPECT_FALSE(obj_->read(CoseResourceId::VERIFY_PEER, value));
    EXPECT_FALSE(obj_->read(CoseResourceId::PUBLIC_KEY_OR_IDENTITY, value));
}

// ============================================================================
// Read - String
// ============================================================================

TEST_F(CoseObjectTest, ReadStringDefaultsAreEmpty) {
    std::string value = "preset";
    EXPECT_TRUE(obj_->read(CoseResourceId::SNI, value));
    EXPECT_TRUE(value.empty());
    value = "preset";
    EXPECT_TRUE(obj_->read(CoseResourceId::ALPN, value));
    EXPECT_TRUE(value.empty());
    value = "preset";
    EXPECT_TRUE(obj_->read(CoseResourceId::CA_CERTIFICATE, value));
    EXPECT_TRUE(value.empty());
    value = "preset";
    EXPECT_TRUE(obj_->read(CoseResourceId::CERTIFICATE_CHAIN, value));
    EXPECT_TRUE(value.empty());
    value = "preset";
    EXPECT_TRUE(obj_->read(CoseResourceId::TOKEN_ENDPOINT, value));
    EXPECT_TRUE(value.empty());
}

TEST_F(CoseObjectTest, ReadStringOnNonStringResourceFails) {
    std::string value;
    EXPECT_FALSE(obj_->read(CoseResourceId::SHORT_SERVER_ID, value));
    EXPECT_FALSE(obj_->read(CoseResourceId::VERIFY_PEER, value));
    EXPECT_FALSE(obj_->read(CoseResourceId::PUBLIC_KEY_OR_IDENTITY, value));
}

// ============================================================================
// Read - Boolean
// ============================================================================

TEST_F(CoseObjectTest, ReadVerifyPeerDefault) {
    bool value = false;
    EXPECT_TRUE(obj_->read(CoseResourceId::VERIFY_PEER, value));
    EXPECT_TRUE(value);
}

TEST_F(CoseObjectTest, ReadBooleanOnNonBooleanFails) {
    bool value = false;
    EXPECT_FALSE(obj_->read(CoseResourceId::SHORT_SERVER_ID, value));
    EXPECT_FALSE(obj_->read(CoseResourceId::SNI, value));
}

// ============================================================================
// Read - Opaque
// ============================================================================

TEST_F(CoseObjectTest, ReadOpaqueDefaultsAreEmpty) {
    std::vector<uint8_t> value{0xFF, 0xEE};
    EXPECT_TRUE(obj_->read(CoseResourceId::PUBLIC_KEY_OR_IDENTITY, value));
    EXPECT_TRUE(value.empty());

    value = {0xAA};
    EXPECT_TRUE(obj_->read(CoseResourceId::SERVER_PUBLIC_KEY, value));
    EXPECT_TRUE(value.empty());
}

TEST_F(CoseObjectTest, ReadOpaqueOnNonOpaqueFails) {
    std::vector<uint8_t> value;
    EXPECT_FALSE(obj_->read(CoseResourceId::SNI, value));
    EXPECT_FALSE(obj_->read(CoseResourceId::SHORT_SERVER_ID, value));
}

// ============================================================================
// Write - Integer
// ============================================================================

TEST_F(CoseObjectTest, WriteSecurityModeAcceptsValidRange) {
    EXPECT_TRUE(obj_->write(CoseResourceId::SECURITY_MODE, int64_t{0}));
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::NONE);
    EXPECT_TRUE(obj_->write(CoseResourceId::SECURITY_MODE, int64_t{1}));
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::PSK);
    EXPECT_TRUE(obj_->write(CoseResourceId::SECURITY_MODE, int64_t{2}));
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::RPK);
    EXPECT_TRUE(obj_->write(CoseResourceId::SECURITY_MODE, int64_t{3}));
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::CERTIFICATE);
    EXPECT_TRUE(obj_->write(CoseResourceId::SECURITY_MODE, int64_t{4}));
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::TOKEN);
}

TEST_F(CoseObjectTest, WriteSecurityModeRejectsOutOfRange) {
    EXPECT_FALSE(obj_->write(CoseResourceId::SECURITY_MODE, int64_t{-1}));
    EXPECT_FALSE(obj_->write(CoseResourceId::SECURITY_MODE, int64_t{5}));
    EXPECT_FALSE(obj_->write(CoseResourceId::SECURITY_MODE, int64_t{100}));
}

TEST_F(CoseObjectTest, WriteShortServerIdIsRejected) {
    EXPECT_FALSE(obj_->write(CoseResourceId::SHORT_SERVER_ID, int64_t{99}));
    EXPECT_EQ(obj_->getShortServerId(), 7u);
}

TEST_F(CoseObjectTest, WriteIntegerOnNonIntegerResourceFails) {
    EXPECT_FALSE(obj_->write(CoseResourceId::SNI, int64_t{1}));
    EXPECT_FALSE(obj_->write(CoseResourceId::PUBLIC_KEY_OR_IDENTITY, int64_t{1}));
}

// ============================================================================
// Write - String
// ============================================================================

TEST_F(CoseObjectTest, WriteAndReadStringResources) {
    EXPECT_TRUE(obj_->write(CoseResourceId::CERTIFICATE_CHAIN, std::string{"/etc/cert.pem"}));
    EXPECT_TRUE(obj_->write(CoseResourceId::CA_CERTIFICATE, std::string{"/etc/ca.pem"}));
    EXPECT_TRUE(obj_->write(CoseResourceId::SNI, std::string{"server.example.com"}));
    EXPECT_TRUE(obj_->write(CoseResourceId::ALPN, std::string{"coap"}));
    EXPECT_TRUE(obj_->write(CoseResourceId::TOKEN, std::string{"jwt.token.here"}));
    EXPECT_TRUE(obj_->write(CoseResourceId::TOKEN_ENDPOINT, std::string{"https://auth/token"}));

    std::string s;
    EXPECT_TRUE(obj_->read(CoseResourceId::CERTIFICATE_CHAIN, s));
    EXPECT_EQ(s, "/etc/cert.pem");
    EXPECT_TRUE(obj_->read(CoseResourceId::CA_CERTIFICATE, s));
    EXPECT_EQ(s, "/etc/ca.pem");
    EXPECT_TRUE(obj_->read(CoseResourceId::SNI, s));
    EXPECT_EQ(s, "server.example.com");
    EXPECT_TRUE(obj_->read(CoseResourceId::ALPN, s));
    EXPECT_EQ(s, "coap");
    EXPECT_TRUE(obj_->read(CoseResourceId::TOKEN_ENDPOINT, s));
    EXPECT_EQ(s, "https://auth/token");
}

TEST_F(CoseObjectTest, WriteStringOnNonStringResourceFails) {
    EXPECT_FALSE(obj_->write(CoseResourceId::SECURITY_MODE, std::string{"PSK"}));
    EXPECT_FALSE(obj_->write(CoseResourceId::PUBLIC_KEY_OR_IDENTITY, std::string{"x"}));
    EXPECT_FALSE(obj_->write(CoseResourceId::VERIFY_PEER, std::string{"true"}));
}

// ============================================================================
// Write - Boolean
// ============================================================================

TEST_F(CoseObjectTest, WriteVerifyPeer) {
    EXPECT_TRUE(obj_->write(CoseResourceId::VERIFY_PEER, false));
    EXPECT_FALSE(obj_->isVerifyPeerEnabled());
    EXPECT_TRUE(obj_->write(CoseResourceId::VERIFY_PEER, true));
    EXPECT_TRUE(obj_->isVerifyPeerEnabled());
}

TEST_F(CoseObjectTest, WriteBooleanOnNonBooleanFails) {
    EXPECT_FALSE(obj_->write(CoseResourceId::SECURITY_MODE, true));
    EXPECT_FALSE(obj_->write(CoseResourceId::SNI, true));
}

// ============================================================================
// Write - Opaque
// ============================================================================

TEST_F(CoseObjectTest, WriteAndReadOpaqueResources) {
    std::vector<uint8_t> identity{'i', 'd', '0', '1'};
    std::vector<uint8_t> secret{0xDE, 0xAD, 0xBE, 0xEF};
    std::vector<uint8_t> serverPub{0x30, 0x59, 0x30, 0x13};

    EXPECT_TRUE(obj_->write(CoseResourceId::PUBLIC_KEY_OR_IDENTITY, identity));
    EXPECT_TRUE(obj_->write(CoseResourceId::SECRET_KEY, secret));
    EXPECT_TRUE(obj_->write(CoseResourceId::SERVER_PUBLIC_KEY, serverPub));

    EXPECT_EQ(obj_->getPskIdentity(), identity);
    EXPECT_EQ(obj_->getServerPublicKey(), serverPub);

    std::vector<uint8_t> readBack;
    EXPECT_TRUE(obj_->read(CoseResourceId::PUBLIC_KEY_OR_IDENTITY, readBack));
    EXPECT_EQ(readBack, identity);
    EXPECT_TRUE(obj_->read(CoseResourceId::SERVER_PUBLIC_KEY, readBack));
    EXPECT_EQ(readBack, serverPub);
}

TEST_F(CoseObjectTest, WriteOpaqueOnNonOpaqueFails) {
    std::vector<uint8_t> data{0x01, 0x02};
    EXPECT_FALSE(obj_->write(CoseResourceId::SNI, data));
    EXPECT_FALSE(obj_->write(CoseResourceId::SECURITY_MODE, data));
}

// ============================================================================
// Configuration Operations
// ============================================================================

TEST_F(CoseObjectTest, GetConfigReturnsCurrentState) {
    obj_->write(CoseResourceId::SNI, std::string{"example.org"});
    obj_->write(CoseResourceId::SECURITY_MODE, int64_t{1});

    auto cfg = obj_->getConfig();
    EXPECT_EQ(cfg.shortServerId, 7u);
    EXPECT_EQ(cfg.sni, "example.org");
    EXPECT_EQ(cfg.securityMode, CoseSecurityMode::PSK);
}

TEST_F(CoseObjectTest, SetConfigReplacesAllFields) {
    CoseConfig cfg;
    cfg.shortServerId = 42;
    cfg.securityMode = CoseSecurityMode::CERTIFICATE;
    cfg.certificateChain = "/x.pem";
    cfg.caCertificate = "/ca.pem";
    cfg.verifyPeer = false;
    cfg.alpn = "coap";

    obj_->setConfig(cfg);

    EXPECT_EQ(obj_->getShortServerId(), 42u);
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::CERTIFICATE);
    EXPECT_FALSE(obj_->isVerifyPeerEnabled());
    std::string s;
    obj_->read(CoseResourceId::ALPN, s);
    EXPECT_EQ(s, "coap");
}

// ============================================================================
// Security Helpers
// ============================================================================

TEST_F(CoseObjectTest, SetPskCredentialsTriggersCallback) {
    int callbackCount = 0;
    CoseResourceId lastResource = CoseResourceId::SHORT_SERVER_ID;
    obj_->setConfigChangeCallback(
        [&](uint16_t, CoseResourceId rid) { ++callbackCount; lastResource = rid; });

    std::vector<uint8_t> id{'a', 'b'};
    std::vector<uint8_t> key{0x01, 0x02, 0x03};
    obj_->setPskCredentials(id, key);

    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::PSK);
    EXPECT_EQ(obj_->getPskIdentity(), id);
    EXPECT_EQ(callbackCount, 1);
    EXPECT_EQ(lastResource, CoseResourceId::SECURITY_MODE);
}

TEST_F(CoseObjectTest, SetCertificateCredentials) {
    obj_->setCertificateCredentials("/path/cert.pem", "/path/ca.pem");
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::CERTIFICATE);
    std::string s;
    EXPECT_TRUE(obj_->read(CoseResourceId::CERTIFICATE_CHAIN, s));
    EXPECT_EQ(s, "/path/cert.pem");
    EXPECT_TRUE(obj_->read(CoseResourceId::CA_CERTIFICATE, s));
    EXPECT_EQ(s, "/path/ca.pem");
}

TEST_F(CoseObjectTest, SetTokenCredentialsWithEndpoint) {
    obj_->setTokenCredentials("eyJ.payload.sig", "https://auth.example/token");
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::TOKEN);
    std::string s;
    EXPECT_TRUE(obj_->read(CoseResourceId::TOKEN_ENDPOINT, s));
    EXPECT_EQ(s, "https://auth.example/token");
}

TEST_F(CoseObjectTest, SetTokenCredentialsWithoutEndpoint) {
    obj_->setTokenCredentials("eyJ.payload.sig");
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::TOKEN);
    std::string s = "preset";
    EXPECT_TRUE(obj_->read(CoseResourceId::TOKEN_ENDPOINT, s));
    EXPECT_TRUE(s.empty());
}

TEST_F(CoseObjectTest, ClearCredentialsResetsSecurityMode) {
    std::vector<uint8_t> id{'a'}, key{0x01, 0x02};
    obj_->setPskCredentials(id, key);
    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::PSK);

    obj_->clearCredentials();

    EXPECT_EQ(obj_->getSecurityMode(), CoseSecurityMode::NONE);
    EXPECT_TRUE(obj_->getPskIdentity().empty());
    EXPECT_TRUE(obj_->getServerPublicKey().empty());
    auto cfg = obj_->getConfig();
    EXPECT_TRUE(cfg.secretKey.empty());
    EXPECT_TRUE(cfg.token.empty());
}

// ============================================================================
// Callbacks
// ============================================================================

TEST_F(CoseObjectTest, CallbackFiresOnEachWrite) {
    int callbackCount = 0;
    CoseResourceId lastResource = CoseResourceId::SHORT_SERVER_ID;
    uint16_t lastInstance = 0xFFFF;
    obj_->setConfigChangeCallback(
        [&](uint16_t inst, CoseResourceId rid) {
            ++callbackCount;
            lastResource = rid;
            lastInstance = inst;
        });

    obj_->write(CoseResourceId::SNI, std::string{"a"});
    obj_->write(CoseResourceId::VERIFY_PEER, false);
    obj_->write(CoseResourceId::SECURITY_MODE, int64_t{1});

    EXPECT_EQ(callbackCount, 3);
    EXPECT_EQ(lastResource, CoseResourceId::SECURITY_MODE);
    EXPECT_EQ(lastInstance, 7u);
}

TEST_F(CoseObjectTest, FailedWriteDoesNotFireCallback) {
    int callbackCount = 0;
    obj_->setConfigChangeCallback(
        [&](uint16_t, CoseResourceId) { ++callbackCount; });

    EXPECT_FALSE(obj_->write(CoseResourceId::SHORT_SERVER_ID, int64_t{1}));
    EXPECT_FALSE(obj_->write(CoseResourceId::SECURITY_MODE, int64_t{99}));
    EXPECT_EQ(callbackCount, 0);
}

// ============================================================================
// Factory
// ============================================================================

TEST(CoseObjectFactoryTest, CreateDefault) {
    auto obj = Lwm2mCoseObjectFactory::create(11);
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->getInstanceId(), 11u);
    EXPECT_EQ(obj->getShortServerId(), 11u);
    EXPECT_EQ(obj->getSecurityMode(), CoseSecurityMode::NONE);
}

TEST(CoseObjectFactoryTest, CreateFromConfig) {
    CoseConfig cfg;
    cfg.shortServerId = 99;
    cfg.securityMode = CoseSecurityMode::RPK;
    cfg.sni = "rpk.example";

    auto obj = Lwm2mCoseObjectFactory::create(cfg, 5);
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->getInstanceId(), 5u);
    EXPECT_EQ(obj->getShortServerId(), 99u);
    EXPECT_EQ(obj->getSecurityMode(), CoseSecurityMode::RPK);
    std::string s;
    obj->read(CoseResourceId::SNI, s);
    EXPECT_EQ(s, "rpk.example");
}

TEST(CoseObjectFactoryTest, CreateWithPsk) {
    std::vector<uint8_t> id{'p', 's', 'k'};
    std::vector<uint8_t> key{0xCA, 0xFE};

    auto obj = Lwm2mCoseObjectFactory::createWithPsk(50, id, key, 3);
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->getInstanceId(), 3u);
    EXPECT_EQ(obj->getShortServerId(), 50u);
    EXPECT_EQ(obj->getSecurityMode(), CoseSecurityMode::PSK);
    EXPECT_EQ(obj->getPskIdentity(), id);
}

} // namespace test
} // namespace objects
} // namespace lwm2m
