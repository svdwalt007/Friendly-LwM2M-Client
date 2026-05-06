/*******************************************************************************
 * DTLS Provider Integration Tests
 *
 * Integration test suite for DTLS providers with LwM2M server interactions.
 * Tests end-to-end functionality including handshake, data transfer, and CID.
 *
 * Author: Sean van der Walt
 * Editor: Sean van der Walt
 * Copyright (c) 2024-2026 Walt Technologies
 * Version: 1.2.2
 ******************************************************************************/

#include <gtest/gtest.h>
#include <thread>
#include <chrono>

// Networking headers for socket structures and functions
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Include TinyDTLS headers first to define session_t before DtlsProvider.h
#ifdef WITH_TINYDTLS_PROVIDER
extern "C" {
#include "tinydtls.h"
#include "dtls.h"
}
#include "../examples/platform/connection_dtls/TinyDtlsProvider.h"
#else
// Define session_t for non-TinyDTLS providers (compatible with TinyDTLS structure)
// DtlsProvider.h forward declares it, we provide the complete definition here
#ifndef _DTLS_SESSION_T_DEFINED
#define _DTLS_SESSION_T_DEFINED
struct session_t {
    socklen_t size;
    union {
        struct sockaddr     sa;
        struct sockaddr_storage st;
        struct sockaddr_in  sin;
        struct sockaddr_in6 sin6;
    } addr;
    int ifindex;
};
#endif
#endif

#include "../examples/platform/connection_dtls/DtlsProvider.h"
#include "../examples/platform/connection_dtls/DtlsProviderFactory.h"

#ifdef WITH_MBEDTLS_PROVIDER
#include "../examples/platform/connection_dtls/MbedTlsProvider.h"
// Detect whether mbedTLS was built with RFC 9146 Connection ID. System
// mbedTLS packages often ship with this disabled.
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

// Test configuration
constexpr const char* TEST_SERVER_HOST = "127.0.0.1";
constexpr const char* TEST_SERVER_PORT = "5684";
constexpr const char* TEST_PSK_IDENTITY = "TestClient";
constexpr const uint8_t TEST_PSK_KEY[] = {0x73, 0x65, 0x63, 0x72, 0x65, 0x74}; // "secret"

// Mock session structure for testing
struct MockSession {
    struct sockaddr_storage addr;
    size_t size;
};

// =============================================================================
// Integration Test Fixture
// =============================================================================

class DtlsIntegrationTest : public ::testing::TestWithParam<std::string> {
protected:
    // Initialise pointer members so TearDown is safe even when SetUp is
    // skipped via GTEST_SKIP() (otherwise the members hold indeterminate
    // values and TearDown's `if (session && provider)` checks segfault).
    DtlsProvider* provider = nullptr;
    void* ctx = nullptr;
    void* session = nullptr;

    void SetUp() override {
        std::string providerName = GetParam();

        if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
            GTEST_SKIP() << "Provider " << providerName << " not available";
        }

        provider = DtlsProviderFactory::create(providerName);
        ASSERT_NE(provider, nullptr) << "Failed to create provider: " << providerName;

        ASSERT_EQ(provider->init(), 0) << "Failed to initialize provider";

        ctx = provider->createContext();
        ASSERT_NE(ctx, nullptr) << "Failed to create context";

        session = nullptr;
    }

    void TearDown() override {
        if (session && provider) {
            provider->destroySession(session);
        }
        if (ctx && provider) {
            provider->destroyContext(ctx);
        }
        if (provider) {
            provider->cleanup();
            delete provider;
        }
    }

    // Helper to create a mock session
    session_t* createMockSession() {
        static session_t mockSession;
        memset(&mockSession, 0, sizeof(mockSession));
        mockSession.size = sizeof(struct sockaddr_in);
        mockSession.addr.sin.sin_family = AF_INET;
        mockSession.addr.sin.sin_port = htons(5684);
        inet_pton(AF_INET, "127.0.0.1", &mockSession.addr.sin.sin_addr);
        return &mockSession;
    }
};

// =============================================================================
// PSK Authentication Tests
// =============================================================================

TEST_P(DtlsIntegrationTest, PSKConfiguration) {
    std::string providerName = GetParam();

    // Configure PSK
    int result = provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY));
    EXPECT_EQ(result, 0) << "PSK configuration failed for " << providerName;
}

TEST_P(DtlsIntegrationTest, PSKSessionCreation) {
    std::string providerName = GetParam();

    // Configure PSK
    ASSERT_EQ(provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY)), 0);

    // Create session
    session_t* mockSession = createMockSession();
    session = provider->createSession(ctx, mockSession);
    EXPECT_NE(session, nullptr) << "Failed to create session with PSK for " << providerName;
}

// =============================================================================
// Handshake Tests
// =============================================================================

TEST_P(DtlsIntegrationTest, HandshakeInitiation) {
    std::string providerName = GetParam();

    // Configure PSK
    ASSERT_EQ(provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY)), 0);

    // Create session
    session_t* mockSession = createMockSession();
    session = provider->createSession(ctx, mockSession);
    ASSERT_NE(session, nullptr);

    // NOTE: This will fail without a real server, but tests the API
    // In a real integration test, you would have a test server running
    int result = provider->connect(session);
    // Don't check result as we don't have a real server
    // Just verify the function doesn't crash
    SUCCEED() << "Handshake initiation completed without crash";
}

TEST_P(DtlsIntegrationTest, HandshakeStateQuery) {
    std::string providerName = GetParam();

    ASSERT_EQ(provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY)), 0);

    session_t* mockSession = createMockSession();
    session = provider->createSession(ctx, mockSession);
    ASSERT_NE(session, nullptr);

    // Initially, handshake should not be complete
    bool complete = provider->isHandshakeComplete(session);
    // Most providers will return false without actual handshake
    SUCCEED() << "Handshake state query: " << (complete ? "complete" : "incomplete");
}

// =============================================================================
// Connection ID (CID) Tests
// =============================================================================

TEST_P(DtlsIntegrationTest, CIDSupport) {
    std::string providerName = GetParam();

    bool supportsCID = provider->supportsCID();

    // Log CID support for each provider
    if (supportsCID) {
        std::cout << providerName << " supports CID" << std::endl;
    } else {
        std::cout << providerName << " does not support CID" << std::endl;
    }

    // Known CID support status. mbedTLS only reports CID when its underlying
    // library was compiled with MBEDTLS_SSL_DTLS_CONNECTION_ID; system
    // packages frequently disable this, so the assertion is gated to match
    // the provider's actual capability.
    if (providerName == "tinydtls" || providerName == "wolfssl") {
        EXPECT_TRUE(supportsCID) << providerName << " should support CID";
    } else if (providerName == "mbedtls") {
#ifdef MBEDTLS_SSL_DTLS_CONNECTION_ID
        EXPECT_TRUE(supportsCID) << providerName << " should support CID";
#endif
    }
}

TEST_P(DtlsIntegrationTest, CIDNegotiation) {
    std::string providerName = GetParam();

    if (!provider->supportsCID()) {
        GTEST_SKIP() << providerName << " does not support CID";
    }

    ASSERT_EQ(provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY)), 0);

    session_t* mockSession = createMockSession();
    session = provider->createSession(ctx, mockSession);
    ASSERT_NE(session, nullptr);

    // Enable CID with 8-byte length
    int result = provider->enableCID(session, 8);
    // Result may vary - some providers need successful handshake first
    SUCCEED() << "CID enable result: " << result;

    // Check if CID was negotiated (will be false without real handshake)
    bool negotiated = provider->isCIDNegotiated(session);
    SUCCEED() << "CID negotiated: " << (negotiated ? "yes" : "no");
}

TEST_P(DtlsIntegrationTest, CIDVersionQuery) {
    std::string providerName = GetParam();

    if (!provider->supportsCID()) {
        GTEST_SKIP() << providerName << " does not support CID";
    }

    // Query CID version (54 = RFC 9146)
    int version = provider->getCIDVersion(nullptr);

    if (version > 0) {
        EXPECT_EQ(version, 54) << providerName << " should support RFC 9146 (version 54)";
        std::cout << providerName << " CID version: " << version << std::endl;
    }
}

// =============================================================================
// Data Transfer Tests (Mock)
// =============================================================================

TEST_P(DtlsIntegrationTest, SendDataAPI) {
    std::string providerName = GetParam();

    ASSERT_EQ(provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY)), 0);

    session_t* mockSession = createMockSession();
    session = provider->createSession(ctx, mockSession);
    ASSERT_NE(session, nullptr);

    // Try to send data (will fail without handshake, but tests API)
    const uint8_t testData[] = "Hello, LwM2M Server!";
    int result = provider->send(session, testData, sizeof(testData));

    // Don't check result as handshake isn't complete
    // Just verify the function doesn't crash
    SUCCEED() << "Send data API called without crash (result: " << result << ")";
}

TEST_P(DtlsIntegrationTest, ReceiveDataAPI) {
    std::string providerName = GetParam();

    ASSERT_EQ(provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY)), 0);

    session_t* mockSession = createMockSession();
    session = provider->createSession(ctx, mockSession);
    ASSERT_NE(session, nullptr);

    // Try to receive data (will fail without actual data, but tests API)
    uint8_t buffer[1024];
    int result = provider->recv(session, buffer, sizeof(buffer));

    // Don't check result as there's no actual data
    // Just verify the function doesn't crash
    SUCCEED() << "Receive data API called without crash (result: " << result << ")";
}

// =============================================================================
// Session Management Tests
// =============================================================================

TEST_P(DtlsIntegrationTest, MultipleSessionCreation) {
    std::string providerName = GetParam();

    ASSERT_EQ(provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY)), 0);

    // Create multiple sessions
    std::vector<void*> sessions;

    for (int i = 0; i < 3; i++) {
        session_t* mockSession = createMockSession();
        // Modify port to make sessions unique
        mockSession->addr.sin.sin_port = htons(5684 + i);

        void* s = provider->createSession(ctx, mockSession);
        EXPECT_NE(s, nullptr) << "Failed to create session " << i;
        if (s) {
            sessions.push_back(s);
        }
    }

    // Clean up sessions
    for (void* s : sessions) {
        provider->destroySession(s);
    }

    // Don't destroy in TearDown since we already did it
    session = nullptr;
}

TEST_P(DtlsIntegrationTest, SessionLifecycle) {
    std::string providerName = GetParam();

    ASSERT_EQ(provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY)), 0);

    // Create session
    session_t* mockSession = createMockSession();
    void* s = provider->createSession(ctx, mockSession);
    ASSERT_NE(s, nullptr);

    // Use session (attempt handshake)
    provider->connect(s);

    // Destroy session
    provider->destroySession(s);

    // Mark as destroyed for TearDown
    session = nullptr;

    SUCCEED() << "Session lifecycle completed successfully";
}

// =============================================================================
// DTLS 1.3 Support Tests
// =============================================================================

TEST_P(DtlsIntegrationTest, DTLS13Support) {
    std::string providerName = GetParam();

    bool supportsDTLS13 = provider->supportsDTLS13();

    std::cout << providerName << " DTLS 1.3 support: "
              << (supportsDTLS13 ? "yes" : "no") << std::endl;

    // Known DTLS 1.3 support status
    if (providerName == "openssl" || providerName == "wolfssl") {
        // These providers should support DTLS 1.3
        std::cout << providerName << " is expected to support DTLS 1.3" << std::endl;
    }
}

// =============================================================================
// Error Handling Tests
// =============================================================================

TEST_P(DtlsIntegrationTest, InvalidPSKHandling) {
    std::string providerName = GetParam();

    // Try to set PSK with null identity
    int result = provider->setPSK(ctx, nullptr, TEST_PSK_KEY, sizeof(TEST_PSK_KEY));
    EXPECT_NE(result, 0) << "Should fail with null identity";

    // Try to set PSK with null key
    result = provider->setPSK(ctx, TEST_PSK_IDENTITY, nullptr, 0);
    EXPECT_NE(result, 0) << "Should fail with null key";
}

TEST_P(DtlsIntegrationTest, NullSessionHandling) {
    std::string providerName = GetParam();

    // Try operations on null session
    int result = provider->send(nullptr, (const uint8_t*)"test", 4);
    EXPECT_NE(result, 0) << "Should fail with null session";

    uint8_t buffer[10];
    result = provider->recv(nullptr, buffer, sizeof(buffer));
    EXPECT_NE(result, 0) << "Should fail with null session";
}

// =============================================================================
// Performance Tests (Basic)
// =============================================================================

TEST_P(DtlsIntegrationTest, SessionCreationPerformance) {
    std::string providerName = GetParam();

    ASSERT_EQ(provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY)), 0);

    auto start = std::chrono::high_resolution_clock::now();

    // Create and destroy 100 sessions
    for (int i = 0; i < 100; i++) {
        session_t* mockSession = createMockSession();
        mockSession->addr.sin.sin_port = htons(5684 + (i % 100));

        void* s = provider->createSession(ctx, mockSession);
        if (s) {
            provider->destroySession(s);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << providerName << " created/destroyed 100 sessions in "
              << duration.count() << " ms" << std::endl;

    // Reasonable performance threshold: < 5 seconds for 100 sessions
    EXPECT_LT(duration.count(), 5000) << providerName << " session creation too slow";
}

// =============================================================================
// Parameterized Test Instantiation
// =============================================================================

INSTANTIATE_TEST_SUITE_P(
    AllProviders,
    DtlsIntegrationTest,
    ::testing::Values("tinydtls", "mbedtls", "openssl", "wolfssl"),
    [](const testing::TestParamInfo<DtlsIntegrationTest::ParamType>& info) {
        return info.param;
    }
);

// =============================================================================
// Provider-Specific Integration Tests
// =============================================================================

#ifdef WITH_MBEDTLS_PROVIDER
TEST(MbedTlsIntegrationTest, CertificateMode) {
    MbedTlsProvider provider;
    ASSERT_EQ(provider.init(), 0);

    void* ctx = provider.createContext();
    ASSERT_NE(ctx, nullptr);

    // This would require actual certificate files
    // For now, just test the API exists
    int result = provider.setCertificate(ctx, "cert.pem", "key.pem", "ca.pem");
    // Expected to fail without real files, but API should work
    SUCCEED() << "Certificate API tested (result: " << result << ")";

    provider.destroyContext(ctx);
    provider.cleanup();
}
#endif

#ifdef WITH_OPENSSL_PROVIDER
TEST(OpenSslIntegrationTest, DTLS13Feature) {
    OpenSslProvider provider;

    // OpenSSL should support DTLS 1.3
    EXPECT_TRUE(provider.supportsDTLS13());

    ASSERT_EQ(provider.init(), 0);
    provider.cleanup();
}
#endif

#ifdef WITH_TINYDTLS_PROVIDER
TEST(TinyDtlsIntegrationTest, RFC9146CID) {
    TinyDtlsProvider provider;

    // TinyDTLS should support RFC 9146 CID
    EXPECT_TRUE(provider.supportsCID());
    EXPECT_EQ(provider.getCIDVersion(nullptr), 54); // RFC 9146

    ASSERT_EQ(provider.init(), 0);
    provider.cleanup();
}
#endif

// =============================================================================
// Main
// =============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "============================================================" << std::endl;
    std::cout << "DTLS Provider Integration Tests v1.2.2" << std::endl;
    std::cout << "Author: Sean van der Walt" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Available DTLS Providers:" << std::endl;

    std::vector<std::string> providers = DtlsProviderFactory::getAvailableProviders();
    for (const auto& p : providers) {
        std::cout << "  - " << p << std::endl;
    }
    std::cout << std::endl;

    std::cout << "NOTE: These tests run without a live LwM2M server." << std::endl;
    std::cout << "      Full integration requires a test server on " << TEST_SERVER_HOST
              << ":" << TEST_SERVER_PORT << std::endl;
    std::cout << std::endl;

    return RUN_ALL_TESTS();
}
