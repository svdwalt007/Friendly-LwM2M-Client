/*******************************************************************************
 * DTLS Multi-Provider Unit Tests
 *
 * Comprehensive test suite for DTLS provider abstraction layer.
 * Tests all four provider implementations: TinyDTLS, mbedTLS, OpenSSL, wolfSSL
 *
 * Author: Sean van der Walt
 * Copyright (c) 2024-2026 Walt Technologies
 * Version: 1.2.2
 ******************************************************************************/

#include <gtest/gtest.h>

// Include TinyDTLS headers first to define session_t before DtlsProvider.h
#ifdef WITH_TINYDTLS_PROVIDER
extern "C" {
#include "tinydtls.h"
#include "dtls.h"
}
#include "../examples/platform/connection_dtls/TinyDtlsProvider.h"
#endif

#include "../examples/platform/connection_dtls/DtlsProvider.h"
#include "../examples/platform/connection_dtls/DtlsProviderFactory.h"

#ifdef WITH_MBEDTLS_PROVIDER
#include "../examples/platform/connection_dtls/MbedTlsProvider.h"
// Detect whether the linked mbedTLS build has RFC 9146 Connection ID
// compiled in. Older mbedTLS 2.x distributions (e.g. Debian/Ubuntu LTS
// system packages) ship with MBEDTLS_SSL_DTLS_CONNECTION_ID disabled in
// their config.h. The provider's supportsCID()/getCIDVersion() honour the
// same macro, so the tests must skip rather than fail in that case.
#if __has_include(<mbedtls/build_info.h>)
#  include <mbedtls/build_info.h>
#else
#  include <mbedtls/config.h>
#endif
#endif

#ifdef WITH_OPENSSL_PROVIDER
#include "../examples/platform/connection_dtls/OpenSslProvider.h"
#endif

#ifdef WITH_WOLFSSL_PROVIDER
#include "../examples/platform/connection_dtls/WolfSslProvider.h"
#endif

// Test fixture for provider factory
class DtlsProviderFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }

    void TearDown() override {
        // Cleanup code
    }
};

// =============================================================================
// Factory Pattern Tests
// =============================================================================

TEST_F(DtlsProviderFactoryTest, FactoryCreateAutoReturnsValidProvider) {
    DtlsProvider* provider = DtlsProviderFactory::create(DtlsProviderType::AUTO);
    ASSERT_NE(provider, nullptr);
    EXPECT_NE(provider->getName(), nullptr);
    EXPECT_NE(provider->getVersion(), nullptr);
    delete provider;
}

TEST_F(DtlsProviderFactoryTest, FactoryCreateDefaultReturnsValidProvider) {
    DtlsProvider* provider = DtlsProviderFactory::createDefault();
    ASSERT_NE(provider, nullptr);

    // Default should be mbedTLS if available
#ifdef WITH_MBEDTLS_PROVIDER
    EXPECT_STREQ(provider->getName(), "mbedTLS");
#endif

    delete provider;
}

TEST_F(DtlsProviderFactoryTest, FactoryGetAvailableProvidersReturnsNonEmpty) {
    std::vector<std::string> providers = DtlsProviderFactory::getAvailableProviders();
    EXPECT_GT(providers.size(), 0);

    // At least one provider should be available
    bool hasProvider = false;
#ifdef WITH_TINYDTLS_PROVIDER
    hasProvider = hasProvider || (std::find(providers.begin(), providers.end(), "tinydtls") != providers.end());
#endif
#ifdef WITH_MBEDTLS_PROVIDER
    hasProvider = hasProvider || (std::find(providers.begin(), providers.end(), "mbedtls") != providers.end());
#endif
#ifdef WITH_OPENSSL_PROVIDER
    hasProvider = hasProvider || (std::find(providers.begin(), providers.end(), "openssl") != providers.end());
#endif
#ifdef WITH_WOLFSSL_PROVIDER
    hasProvider = hasProvider || (std::find(providers.begin(), providers.end(), "wolfssl") != providers.end());
#endif

    EXPECT_TRUE(hasProvider);
}

TEST_F(DtlsProviderFactoryTest, FactoryNameToTypeConversion) {
    EXPECT_EQ(DtlsProviderFactory::nameToType("auto"), DtlsProviderType::AUTO);
    EXPECT_EQ(DtlsProviderFactory::nameToType("tinydtls"), DtlsProviderType::TINYDTLS);
    EXPECT_EQ(DtlsProviderFactory::nameToType("mbedtls"), DtlsProviderType::MBEDTLS);
    EXPECT_EQ(DtlsProviderFactory::nameToType("openssl"), DtlsProviderType::OPENSSL);
    EXPECT_EQ(DtlsProviderFactory::nameToType("wolfssl"), DtlsProviderType::WOLFSSL);

    // Case insensitive
    EXPECT_EQ(DtlsProviderFactory::nameToType("MBEDTLS"), DtlsProviderType::MBEDTLS);
    EXPECT_EQ(DtlsProviderFactory::nameToType("OpenSSL"), DtlsProviderType::OPENSSL);
}

TEST_F(DtlsProviderFactoryTest, FactoryTypeToNameConversion) {
    EXPECT_STREQ(DtlsProviderFactory::typeToName(DtlsProviderType::AUTO).c_str(), "auto");
    EXPECT_STREQ(DtlsProviderFactory::typeToName(DtlsProviderType::TINYDTLS).c_str(), "tinydtls");
    EXPECT_STREQ(DtlsProviderFactory::typeToName(DtlsProviderType::MBEDTLS).c_str(), "mbedtls");
    EXPECT_STREQ(DtlsProviderFactory::typeToName(DtlsProviderType::OPENSSL).c_str(), "openssl");
    EXPECT_STREQ(DtlsProviderFactory::typeToName(DtlsProviderType::WOLFSSL).c_str(), "wolfssl");
}

TEST_F(DtlsProviderFactoryTest, FactoryGetProviderInfo) {
    std::vector<DtlsProviderInfo> info = DtlsProviderFactory::getProviderInfo();
    EXPECT_EQ(info.size(), 4); // Should have info for all 4 providers

    // Check each provider info
    for (const auto& prov : info) {
        EXPECT_FALSE(prov.name.empty());
        EXPECT_FALSE(prov.version.empty());
        EXPECT_FALSE(prov.description.empty());
    }
}

// =============================================================================
// TinyDTLS Provider Tests
// =============================================================================

#ifdef WITH_TINYDTLS_PROVIDER
class TinyDtlsProviderTest : public ::testing::Test {
protected:
    TinyDtlsProvider* provider;

    void SetUp() override {
        provider = new TinyDtlsProvider();
        ASSERT_NE(provider, nullptr);
    }

    void TearDown() override {
        delete provider;
    }
};

TEST_F(TinyDtlsProviderTest, ProviderInfo) {
    EXPECT_STREQ(provider->getName(), "TinyDTLS");
    EXPECT_STREQ(provider->getVersion(), "0.8.6");
}

TEST_F(TinyDtlsProviderTest, SupportedFeatures) {
    // TinyDTLS supports CID
    EXPECT_TRUE(provider->supportsCID());

    // TinyDTLS does not support DTLS 1.3
    EXPECT_FALSE(provider->supportsDTLS13());
}

TEST_F(TinyDtlsProviderTest, Initialization) {
    EXPECT_EQ(provider->init(), 0);
    EXPECT_EQ(provider->cleanup(), 0);
}

TEST_F(TinyDtlsProviderTest, ContextLifecycle) {
    ASSERT_EQ(provider->init(), 0);

    void* ctx = provider->createContext();
    ASSERT_NE(ctx, nullptr);

    provider->destroyContext(ctx);
    provider->cleanup();
}

TEST_F(TinyDtlsProviderTest, PSKConfiguration) {
    ASSERT_EQ(provider->init(), 0);
    void* ctx = provider->createContext();
    ASSERT_NE(ctx, nullptr);

    const char* identity = "TestClient";
    const uint8_t key[] = {0x73, 0x65, 0x63, 0x72, 0x65, 0x74}; // "secret"

    EXPECT_EQ(provider->setPSK(ctx, identity, key, sizeof(key)), 0);

    provider->destroyContext(ctx);
    provider->cleanup();
}

TEST_F(TinyDtlsProviderTest, CertificateNotSupported) {
    ASSERT_EQ(provider->init(), 0);
    void* ctx = provider->createContext();
    ASSERT_NE(ctx, nullptr);

    // TinyDTLS does not support certificates
    EXPECT_EQ(provider->setCertificate(ctx, "cert.pem", "key.pem", "ca.pem"), -1);

    provider->destroyContext(ctx);
    provider->cleanup();
}
#endif

// =============================================================================
// mbedTLS Provider Tests
// =============================================================================

#ifdef WITH_MBEDTLS_PROVIDER
class MbedTlsProviderTest : public ::testing::Test {
protected:
    MbedTlsProvider* provider;

    void SetUp() override {
        provider = new MbedTlsProvider();
        ASSERT_NE(provider, nullptr);
    }

    void TearDown() override {
        delete provider;
    }
};

TEST_F(MbedTlsProviderTest, ProviderInfo) {
    EXPECT_STREQ(provider->getName(), "mbedTLS");
    EXPECT_NE(provider->getVersion(), nullptr);
}

TEST_F(MbedTlsProviderTest, SupportedFeatures) {
    // mbedTLS supports CID only when the underlying library was built with
    // MBEDTLS_SSL_DTLS_CONNECTION_ID. System mbedTLS packages (Debian/Ubuntu)
    // currently ship with this disabled, so skip the CID assertion in that
    // configuration rather than producing a false failure.
#ifdef MBEDTLS_SSL_DTLS_CONNECTION_ID
    EXPECT_TRUE(provider->supportsCID());
#else
    EXPECT_FALSE(provider->supportsCID());
#endif

    // mbedTLS DTLS 1.3 is in development
    EXPECT_FALSE(provider->supportsDTLS13());
}

TEST_F(MbedTlsProviderTest, Initialization) {
    EXPECT_EQ(provider->init(), 0);
    EXPECT_EQ(provider->cleanup(), 0);
}

TEST_F(MbedTlsProviderTest, ContextLifecycle) {
    ASSERT_EQ(provider->init(), 0);

    void* ctx = provider->createContext();
    ASSERT_NE(ctx, nullptr);

    provider->destroyContext(ctx);
    provider->cleanup();
}

TEST_F(MbedTlsProviderTest, PSKConfiguration) {
    ASSERT_EQ(provider->init(), 0);
    void* ctx = provider->createContext();
    ASSERT_NE(ctx, nullptr);

    const char* identity = "TestClient";
    const uint8_t key[] = {0x73, 0x65, 0x63, 0x72, 0x65, 0x74};

    EXPECT_EQ(provider->setPSK(ctx, identity, key, sizeof(key)), 0);

    provider->destroyContext(ctx);
    provider->cleanup();
}

TEST_F(MbedTlsProviderTest, CIDSupport) {
#ifdef MBEDTLS_SSL_DTLS_CONNECTION_ID
    EXPECT_EQ(provider->getCIDVersion(nullptr), 54); // RFC 9146
#else
    GTEST_SKIP() << "mbedTLS built without MBEDTLS_SSL_DTLS_CONNECTION_ID";
#endif
}
#endif

// =============================================================================
// OpenSSL Provider Tests
// =============================================================================

#ifdef WITH_OPENSSL_PROVIDER
class OpenSslProviderTest : public ::testing::Test {
protected:
    OpenSslProvider* provider;

    void SetUp() override {
        provider = new OpenSslProvider();
        ASSERT_NE(provider, nullptr);
    }

    void TearDown() override {
        delete provider;
    }
};

TEST_F(OpenSslProviderTest, ProviderInfo) {
    EXPECT_STREQ(provider->getName(), "OpenSSL");
    EXPECT_NE(provider->getVersion(), nullptr);
}

TEST_F(OpenSslProviderTest, SupportedFeatures) {
    // OpenSSL supports DTLS 1.3
    EXPECT_TRUE(provider->supportsDTLS13());
}

TEST_F(OpenSslProviderTest, Initialization) {
    EXPECT_EQ(provider->init(), 0);
    EXPECT_EQ(provider->cleanup(), 0);
}

TEST_F(OpenSslProviderTest, ContextLifecycle) {
    ASSERT_EQ(provider->init(), 0);

    void* ctx = provider->createContext();
    ASSERT_NE(ctx, nullptr);

    provider->destroyContext(ctx);
    provider->cleanup();
}

TEST_F(OpenSslProviderTest, PSKConfiguration) {
    ASSERT_EQ(provider->init(), 0);
    void* ctx = provider->createContext();
    ASSERT_NE(ctx, nullptr);

    const char* identity = "TestClient";
    const uint8_t key[] = {0x73, 0x65, 0x63, 0x72, 0x65, 0x74};

    EXPECT_EQ(provider->setPSK(ctx, identity, key, sizeof(key)), 0);

    provider->destroyContext(ctx);
    provider->cleanup();
}
#endif

// =============================================================================
// wolfSSL Provider Tests
// =============================================================================

#ifdef WITH_WOLFSSL_PROVIDER
class WolfSslProviderTest : public ::testing::Test {
protected:
    WolfSslProvider* provider;

    void SetUp() override {
        provider = new WolfSslProvider();
        ASSERT_NE(provider, nullptr);
    }

    void TearDown() override {
        delete provider;
    }
};

TEST_F(WolfSslProviderTest, ProviderInfo) {
    EXPECT_STREQ(provider->getName(), "wolfSSL");
    EXPECT_NE(provider->getVersion(), nullptr);
}

TEST_F(WolfSslProviderTest, SupportedFeatures) {
    // wolfSSL can support CID
    // EXPECT_TRUE(provider->supportsCID()); // Depends on build configuration

    // wolfSSL supports DTLS 1.3
    // EXPECT_TRUE(provider->supportsDTLS13()); // Depends on build configuration
}

TEST_F(WolfSslProviderTest, Initialization) {
    EXPECT_EQ(provider->init(), 0);
    EXPECT_EQ(provider->cleanup(), 0);
}

TEST_F(WolfSslProviderTest, ContextLifecycle) {
    ASSERT_EQ(provider->init(), 0);

    void* ctx = provider->createContext();
    ASSERT_NE(ctx, nullptr);

    provider->destroyContext(ctx);
    provider->cleanup();
}
#endif

// =============================================================================
// Cross-Provider Tests
// =============================================================================

class AllProvidersTest : public ::testing::TestWithParam<std::string> {
protected:
    DtlsProvider* CreateProvider(const std::string& name) {
        return DtlsProviderFactory::create(name);
    }
};

TEST_P(AllProvidersTest, ProviderBasicLifecycle) {
    std::string providerName = GetParam();

    if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
        GTEST_SKIP() << "Provider " << providerName << " not available";
    }

    DtlsProvider* provider = CreateProvider(providerName);
    ASSERT_NE(provider, nullptr) << "Failed to create provider: " << providerName;

    // Test initialization
    EXPECT_EQ(provider->init(), 0);

    // Test context creation
    void* ctx = provider->createContext();
    EXPECT_NE(ctx, nullptr);

    // Test context destruction
    provider->destroyContext(ctx);

    // Test cleanup
    EXPECT_EQ(provider->cleanup(), 0);

    delete provider;
}

TEST_P(AllProvidersTest, ProviderPSKConfiguration) {
    std::string providerName = GetParam();

    if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
        GTEST_SKIP() << "Provider " << providerName << " not available";
    }

    DtlsProvider* provider = CreateProvider(providerName);
    ASSERT_NE(provider, nullptr);

    ASSERT_EQ(provider->init(), 0);
    void* ctx = provider->createContext();
    ASSERT_NE(ctx, nullptr);

    const char* identity = "TestClient";
    const uint8_t key[] = {0x73, 0x65, 0x63, 0x72, 0x65, 0x74};

    int result = provider->setPSK(ctx, identity, key, sizeof(key));
    EXPECT_EQ(result, 0) << "PSK configuration failed for " << providerName;

    provider->destroyContext(ctx);
    provider->cleanup();
    delete provider;
}

INSTANTIATE_TEST_SUITE_P(
    DTLSProviders,
    AllProvidersTest,
    ::testing::Values("tinydtls", "mbedtls", "openssl", "wolfssl")
);

// =============================================================================
// Main
// =============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
