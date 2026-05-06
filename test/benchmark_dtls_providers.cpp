/*******************************************************************************
 * DTLS Provider Performance Benchmarks
 *
 * Performance benchmark suite for comparing DTLS provider implementations.
 * Measures throughput, latency, and resource usage.
 *
 * Author: Sean van der Walt
 * Editor: Sean van der Walt
 * Copyright (c) 2024-2026 Walt Technologies
 * Version: 1.2.2
 ******************************************************************************/

#include <benchmark/benchmark.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include "../examples/platform/connection_dtls/DtlsProvider.h"
#include "../examples/platform/connection_dtls/DtlsProviderFactory.h"

// Test configuration
constexpr const char* TEST_PSK_IDENTITY = "BenchmarkClient";
constexpr const uint8_t TEST_PSK_KEY[] = {
    0x73, 0x65, 0x63, 0x72, 0x65, 0x74, 0x6b, 0x65,
    0x79, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
};

// Mock session for benchmarks
session_t* createBenchmarkSession() {
    static session_t session;
    memset(&session, 0, sizeof(session));
    session.size = sizeof(struct sockaddr_in);
    session.addr.sin.sin_family = AF_INET;
    session.addr.sin.sin_port = htons(5684);
    inet_pton(AF_INET, "127.0.0.1", &session.addr.sin.sin_addr);
    return &session;
}

// =============================================================================
// Initialization Benchmarks
// =============================================================================

static void BM_ProviderInit(benchmark::State& state, const std::string& providerName) {
    for (auto _ : state) {
        if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
            state.SkipWithError("Provider not available");
            return;
        }

        DtlsProvider* provider = DtlsProviderFactory::create(providerName);
        if (!provider) {
            state.SkipWithError("Failed to create provider");
            return;
        }

        benchmark::DoNotOptimize(provider->init());

        provider->cleanup();
        delete provider;
    }

    state.SetLabel(providerName);
}

BENCHMARK_CAPTURE(BM_ProviderInit, TinyDTLS, "tinydtls");
BENCHMARK_CAPTURE(BM_ProviderInit, mbedTLS, "mbedtls");
BENCHMARK_CAPTURE(BM_ProviderInit, OpenSSL, "openssl");
BENCHMARK_CAPTURE(BM_ProviderInit, wolfSSL, "wolfssl");

// =============================================================================
// Context Creation Benchmarks
// =============================================================================

static void BM_ContextCreation(benchmark::State& state, const std::string& providerName) {
    if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
        state.SkipWithError("Provider not available");
        return;
    }

    DtlsProvider* provider = DtlsProviderFactory::create(providerName);
    if (!provider) {
        state.SkipWithError("Failed to create provider");
        return;
    }

    provider->init();

    for (auto _ : state) {
        void* ctx = provider->createContext();
        benchmark::DoNotOptimize(ctx);
        if (ctx) {
            provider->destroyContext(ctx);
        }
    }

    provider->cleanup();
    delete provider;

    state.SetLabel(providerName);
}

BENCHMARK_CAPTURE(BM_ContextCreation, TinyDTLS, "tinydtls");
BENCHMARK_CAPTURE(BM_ContextCreation, mbedTLS, "mbedtls");
BENCHMARK_CAPTURE(BM_ContextCreation, OpenSSL, "openssl");
BENCHMARK_CAPTURE(BM_ContextCreation, wolfSSL, "wolfssl");

// =============================================================================
// Session Creation Benchmarks
// =============================================================================

static void BM_SessionCreation(benchmark::State& state, const std::string& providerName) {
    if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
        state.SkipWithError("Provider not available");
        return;
    }

    DtlsProvider* provider = DtlsProviderFactory::create(providerName);
    if (!provider) {
        state.SkipWithError("Failed to create provider");
        return;
    }

    provider->init();
    void* ctx = provider->createContext();

    if (!ctx) {
        provider->cleanup();
        delete provider;
        state.SkipWithError("Failed to create context");
        return;
    }

    provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY));

    session_t* mockSession = createBenchmarkSession();

    for (auto _ : state) {
        void* session = provider->createSession(ctx, mockSession);
        benchmark::DoNotOptimize(session);
        if (session) {
            provider->destroySession(session);
        }
    }

    provider->destroyContext(ctx);
    provider->cleanup();
    delete provider;

    state.SetLabel(providerName);
}

BENCHMARK_CAPTURE(BM_SessionCreation, TinyDTLS, "tinydtls");
BENCHMARK_CAPTURE(BM_SessionCreation, mbedTLS, "mbedtls");
BENCHMARK_CAPTURE(BM_SessionCreation, OpenSSL, "openssl");
BENCHMARK_CAPTURE(BM_SessionCreation, wolfSSL, "wolfssl");

// =============================================================================
// PSK Configuration Benchmarks
// =============================================================================

static void BM_PSKConfiguration(benchmark::State& state, const std::string& providerName) {
    if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
        state.SkipWithError("Provider not available");
        return;
    }

    DtlsProvider* provider = DtlsProviderFactory::create(providerName);
    if (!provider) {
        state.SkipWithError("Failed to create provider");
        return;
    }

    provider->init();
    void* ctx = provider->createContext();

    if (!ctx) {
        provider->cleanup();
        delete provider;
        state.SkipWithError("Failed to create context");
        return;
    }

    for (auto _ : state) {
        int result = provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY));
        benchmark::DoNotOptimize(result);
    }

    provider->destroyContext(ctx);
    provider->cleanup();
    delete provider;

    state.SetLabel(providerName);
}

BENCHMARK_CAPTURE(BM_PSKConfiguration, TinyDTLS, "tinydtls");
BENCHMARK_CAPTURE(BM_PSKConfiguration, mbedTLS, "mbedtls");
BENCHMARK_CAPTURE(BM_PSKConfiguration, OpenSSL, "openssl");
BENCHMARK_CAPTURE(BM_PSKConfiguration, wolfSSL, "wolfssl");

// =============================================================================
// Memory Footprint Benchmarks
// =============================================================================

static void BM_MemoryFootprint(benchmark::State& state, const std::string& providerName) {
    if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
        state.SkipWithError("Provider not available");
        return;
    }

    for (auto _ : state) {
        state.PauseTiming();

        DtlsProvider* provider = DtlsProviderFactory::create(providerName);
        if (!provider) {
            state.SkipWithError("Failed to create provider");
            return;
        }

        provider->init();
        void* ctx = provider->createContext();

        if (!ctx) {
            provider->cleanup();
            delete provider;
            state.SkipWithError("Failed to create context");
            return;
        }

        provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY));

        // Create multiple sessions to test memory scaling
        std::vector<void*> sessions;
        session_t* mockSession = createBenchmarkSession();

        state.ResumeTiming();

        for (int i = 0; i < 10; i++) {
            mockSession->addr.sin.sin_port = htons(5684 + i);
            void* session = provider->createSession(ctx, mockSession);
            if (session) {
                sessions.push_back(session);
            }
        }

        state.PauseTiming();

        // Cleanup
        for (void* session : sessions) {
            provider->destroySession(session);
        }

        provider->destroyContext(ctx);
        provider->cleanup();
        delete provider;

        state.ResumeTiming();
    }

    state.SetLabel(providerName);
}

BENCHMARK_CAPTURE(BM_MemoryFootprint, TinyDTLS, "tinydtls");
BENCHMARK_CAPTURE(BM_MemoryFootprint, mbedTLS, "mbedtls");
BENCHMARK_CAPTURE(BM_MemoryFootprint, OpenSSL, "openssl");
BENCHMARK_CAPTURE(BM_MemoryFootprint, wolfSSL, "wolfssl");

// =============================================================================
// Data Encryption/Decryption Benchmarks (Simulated)
// =============================================================================

static void BM_DataEncryption(benchmark::State& state, const std::string& providerName) {
    if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
        state.SkipWithError("Provider not available");
        return;
    }

    DtlsProvider* provider = DtlsProviderFactory::create(providerName);
    if (!provider) {
        state.SkipWithError("Failed to create provider");
        return;
    }

    provider->init();
    void* ctx = provider->createContext();

    if (!ctx) {
        provider->cleanup();
        delete provider;
        state.SkipWithError("Failed to create context");
        return;
    }

    provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY));

    session_t* mockSession = createBenchmarkSession();
    void* session = provider->createSession(ctx, mockSession);

    if (!session) {
        provider->destroyContext(ctx);
        provider->cleanup();
        delete provider;
        state.SkipWithError("Failed to create session");
        return;
    }

    // Test data - typical LwM2M CoAP message size
    const uint8_t testData[128] = {0};

    int64_t totalBytes = 0;

    for (auto _ : state) {
        // Note: This will fail without actual handshake, but measures API overhead
        int result = provider->send(session, testData, sizeof(testData));
        benchmark::DoNotOptimize(result);
        totalBytes += sizeof(testData);
    }

    state.SetBytesProcessed(totalBytes);
    state.SetLabel(providerName);

    provider->destroySession(session);
    provider->destroyContext(ctx);
    provider->cleanup();
    delete provider;
}

BENCHMARK_CAPTURE(BM_DataEncryption, TinyDTLS, "tinydtls");
BENCHMARK_CAPTURE(BM_DataEncryption, mbedTLS, "mbedtls");
BENCHMARK_CAPTURE(BM_DataEncryption, OpenSSL, "openssl");
BENCHMARK_CAPTURE(BM_DataEncryption, wolfSSL, "wolfssl");

// =============================================================================
// CID Feature Benchmarks
// =============================================================================

static void BM_CIDOperations(benchmark::State& state, const std::string& providerName) {
    if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
        state.SkipWithError("Provider not available");
        return;
    }

    DtlsProvider* provider = DtlsProviderFactory::create(providerName);
    if (!provider) {
        state.SkipWithError("Failed to create provider");
        return;
    }

    if (!provider->supportsCID()) {
        delete provider;
        state.SkipWithError("Provider does not support CID");
        return;
    }

    provider->init();
    void* ctx = provider->createContext();

    if (!ctx) {
        provider->cleanup();
        delete provider;
        state.SkipWithError("Failed to create context");
        return;
    }

    provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY));

    session_t* mockSession = createBenchmarkSession();
    void* session = provider->createSession(ctx, mockSession);

    if (!session) {
        provider->destroyContext(ctx);
        provider->cleanup();
        delete provider;
        state.SkipWithError("Failed to create session");
        return;
    }

    for (auto _ : state) {
        int result = provider->enableCID(session, 8);
        benchmark::DoNotOptimize(result);

        bool negotiated = provider->isCIDNegotiated(session);
        benchmark::DoNotOptimize(negotiated);
    }

    state.SetLabel(providerName);

    provider->destroySession(session);
    provider->destroyContext(ctx);
    provider->cleanup();
    delete provider;
}

BENCHMARK_CAPTURE(BM_CIDOperations, TinyDTLS, "tinydtls");
BENCHMARK_CAPTURE(BM_CIDOperations, mbedTLS, "mbedtls");
BENCHMARK_CAPTURE(BM_CIDOperations, OpenSSL, "openssl");
BENCHMARK_CAPTURE(BM_CIDOperations, wolfSSL, "wolfssl");

// =============================================================================
// Concurrent Session Benchmarks
// =============================================================================

static void BM_ConcurrentSessions(benchmark::State& state, const std::string& providerName) {
    if (!DtlsProviderFactory::isProviderAvailable(providerName)) {
        state.SkipWithError("Provider not available");
        return;
    }

    const int numSessions = state.range(0);

    DtlsProvider* provider = DtlsProviderFactory::create(providerName);
    if (!provider) {
        state.SkipWithError("Failed to create provider");
        return;
    }

    provider->init();
    void* ctx = provider->createContext();

    if (!ctx) {
        provider->cleanup();
        delete provider;
        state.SkipWithError("Failed to create context");
        return;
    }

    provider->setPSK(ctx, TEST_PSK_IDENTITY, TEST_PSK_KEY, sizeof(TEST_PSK_KEY));

    for (auto _ : state) {
        state.PauseTiming();
        std::vector<void*> sessions;
        session_t* mockSession = createBenchmarkSession();
        state.ResumeTiming();

        for (int i = 0; i < numSessions; i++) {
            mockSession->addr.sin.sin_port = htons(5684 + i);
            void* session = provider->createSession(ctx, mockSession);
            if (session) {
                sessions.push_back(session);
            }
        }

        state.PauseTiming();
        for (void* session : sessions) {
            provider->destroySession(session);
        }
        state.ResumeTiming();
    }

    state.SetLabel(providerName + " (" + std::to_string(numSessions) + " sessions)");

    provider->destroyContext(ctx);
    provider->cleanup();
    delete provider;
}

BENCHMARK_CAPTURE(BM_ConcurrentSessions, TinyDTLS_10, "tinydtls")->Arg(10);
BENCHMARK_CAPTURE(BM_ConcurrentSessions, TinyDTLS_100, "tinydtls")->Arg(100);
BENCHMARK_CAPTURE(BM_ConcurrentSessions, mbedTLS_10, "mbedtls")->Arg(10);
BENCHMARK_CAPTURE(BM_ConcurrentSessions, mbedTLS_100, "mbedtls")->Arg(100);
BENCHMARK_CAPTURE(BM_ConcurrentSessions, OpenSSL_10, "openssl")->Arg(10);
BENCHMARK_CAPTURE(BM_ConcurrentSessions, OpenSSL_100, "openssl")->Arg(100);
BENCHMARK_CAPTURE(BM_ConcurrentSessions, wolfSSL_10, "wolfssl")->Arg(10);
BENCHMARK_CAPTURE(BM_ConcurrentSessions, wolfSSL_100, "wolfssl")->Arg(100);

// =============================================================================
// Main
// =============================================================================

int main(int argc, char** argv) {
    std::cout << "============================================================" << std::endl;
    std::cout << "DTLS Provider Performance Benchmarks v1.2.2" << std::endl;
    std::cout << "Author: Sean van der Walt" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Available DTLS Providers:" << std::endl;

    std::vector<std::string> providers = DtlsProviderFactory::getAvailableProviders();
    for (const auto& p : providers) {
        DtlsProvider* provider = DtlsProviderFactory::create(p);
        if (provider) {
            std::cout << "  - " << provider->getName() << " " << provider->getVersion();
            if (provider->supportsCID()) {
                std::cout << " [CID]";
            }
            if (provider->supportsDTLS13()) {
                std::cout << " [DTLS 1.3]";
            }
            std::cout << std::endl;
            delete provider;
        }
    }
    std::cout << std::endl;

    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();

    return 0;
}
