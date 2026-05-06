/*******************************************************************************
 * DTLS Provider Factory Implementation
 *
 * Factory pattern for creating DTLS provider instances.
 * Supports runtime selection of DTLS library implementation.
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

// IMPORTANT: Include provider headers BEFORE DtlsProviderFactory.h
// This ensures DTLS library headers (which define session_t) are included
// before DtlsProvider.h's forward declaration
#ifdef WITH_TINYDTLS_PROVIDER
#include "TinyDtlsProvider.h"
#endif

#ifdef WITH_MBEDTLS_PROVIDER
#include "MbedTlsProvider.h"
#endif

#ifdef WITH_OPENSSL_PROVIDER
#include "OpenSslProvider.h"
#endif

#ifdef WITH_WOLFSSL_PROVIDER
#include "WolfSslProvider.h"
#endif

// Include factory header after providers
#include "DtlsProviderFactory.h"
#include <algorithm>
#include <cctype>

// Helper function to convert string to lowercase
static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Create provider by type
DtlsProvider* DtlsProviderFactory::create(DtlsProviderType type) {
    switch (type) {
        case DtlsProviderType::AUTO:
            return createDefault();

        case DtlsProviderType::TINYDTLS:
            return createTinyDTLS();

        case DtlsProviderType::MBEDTLS:
            return createMbedTLS();

        case DtlsProviderType::OPENSSL:
            return createOpenSSL();

        case DtlsProviderType::WOLFSSL:
            return createWolfSSL();

        default:
            return nullptr;
    }
}

// Create provider by name
DtlsProvider* DtlsProviderFactory::create(const std::string& name) {
    DtlsProviderType type = nameToType(name);
    return create(type);
}

// Create default provider (priority: mbedTLS > TinyDTLS > OpenSSL > wolfSSL)
DtlsProvider* DtlsProviderFactory::createDefault() {
    // First choice: mbedTLS (best for embedded/production)
    if (isProviderAvailable(DtlsProviderType::MBEDTLS)) {
        return createMbedTLS();
    }

    // Second choice: TinyDTLS (proven in current deployment)
    if (isProviderAvailable(DtlsProviderType::TINYDTLS)) {
        return createTinyDTLS();
    }

    // Third choice: OpenSSL (widely available)
    if (isProviderAvailable(DtlsProviderType::OPENSSL)) {
        return createOpenSSL();
    }

    // Fourth choice: wolfSSL (performance)
    if (isProviderAvailable(DtlsProviderType::WOLFSSL)) {
        return createWolfSSL();
    }

    // No providers available
    return nullptr;
}

// Get list of available provider names
std::vector<std::string> DtlsProviderFactory::getAvailableProviders() {
    std::vector<std::string> providers;

#ifdef WITH_TINYDTLS_PROVIDER
    providers.push_back("tinydtls");
#endif

#ifdef WITH_MBEDTLS_PROVIDER
    providers.push_back("mbedtls");
#endif

#ifdef WITH_OPENSSL_PROVIDER
    providers.push_back("openssl");
#endif

#ifdef WITH_WOLFSSL_PROVIDER
    providers.push_back("wolfssl");
#endif

    return providers;
}

// Get detailed provider information
std::vector<DtlsProviderInfo> DtlsProviderFactory::getProviderInfo() {
    std::vector<DtlsProviderInfo> info;

#ifdef WITH_TINYDTLS_PROVIDER
    info.push_back({
        "tinydtls",
        "0.8.6",
        true,
        true,   // CID support
        false,  // No DTLS 1.3
        "TinyDTLS 0.8.6 - Embedded DTLS for constrained devices"
    });
#else
    info.push_back({
        "tinydtls",
        "N/A",
        false,
        false,
        false,
        "TinyDTLS (not compiled in)"
    });
#endif

#ifdef WITH_MBEDTLS_PROVIDER
    info.push_back({
        "mbedtls",
        "3.6+",
        true,
        true,   // CID support
        false,  // DTLS 1.3 in development
        "mbedTLS 3.6+ - Production-grade DTLS (recommended)"
    });
#else
    info.push_back({
        "mbedtls",
        "N/A",
        false,
        false,
        false,
        "mbedTLS (not compiled in)"
    });
#endif

#ifdef WITH_OPENSSL_PROVIDER
    info.push_back({
        "openssl",
        "3.x",
        true,
        true,   // CID support in 3.2+
        true,   // DTLS 1.3 support
        "OpenSSL 3.x - Industry standard SSL/TLS"
    });
#else
    info.push_back({
        "openssl",
        "N/A",
        false,
        false,
        false,
        "OpenSSL (not compiled in)"
    });
#endif

#ifdef WITH_WOLFSSL_PROVIDER
    info.push_back({
        "wolfssl",
        "5.x",
        true,
        false,  // CID support varies
        true,   // DTLS 1.3 support
        "wolfSSL 5.x - Performance-focused SSL/TLS"
    });
#else
    info.push_back({
        "wolfssl",
        "N/A",
        false,
        false,
        false,
        "wolfSSL (not compiled in)"
    });
#endif

    return info;
}

// Check if provider is available by name
bool DtlsProviderFactory::isProviderAvailable(const std::string& name) {
    DtlsProviderType type = nameToType(name);
    return isProviderAvailable(type);
}

// Check if provider is available by type
bool DtlsProviderFactory::isProviderAvailable(DtlsProviderType type) {
    switch (type) {
        case DtlsProviderType::TINYDTLS:
#ifdef WITH_TINYDTLS_PROVIDER
            return true;
#else
            return false;
#endif

        case DtlsProviderType::MBEDTLS:
#ifdef WITH_MBEDTLS_PROVIDER
            return true;
#else
            return false;
#endif

        case DtlsProviderType::OPENSSL:
#ifdef WITH_OPENSSL_PROVIDER
            return true;
#else
            return false;
#endif

        case DtlsProviderType::WOLFSSL:
#ifdef WITH_WOLFSSL_PROVIDER
            return true;
#else
            return false;
#endif

        case DtlsProviderType::AUTO:
            // AUTO is always "available" - it selects best available
            return true;

        default:
            return false;
    }
}

// Get default provider name
std::string DtlsProviderFactory::getDefaultProviderName() {
    // Same priority as createDefault()
    if (isProviderAvailable(DtlsProviderType::MBEDTLS)) {
        return "mbedtls";
    }
    if (isProviderAvailable(DtlsProviderType::TINYDTLS)) {
        return "tinydtls";
    }
    if (isProviderAvailable(DtlsProviderType::OPENSSL)) {
        return "openssl";
    }
    if (isProviderAvailable(DtlsProviderType::WOLFSSL)) {
        return "wolfssl";
    }
    return "none";
}

// Convert provider name to type
DtlsProviderType DtlsProviderFactory::nameToType(const std::string& name) {
    std::string lower_name = toLower(name);

    if (lower_name == "auto") {
        return DtlsProviderType::AUTO;
    } else if (lower_name == "tinydtls") {
        return DtlsProviderType::TINYDTLS;
    } else if (lower_name == "mbedtls") {
        return DtlsProviderType::MBEDTLS;
    } else if (lower_name == "openssl") {
        return DtlsProviderType::OPENSSL;
    } else if (lower_name == "wolfssl") {
        return DtlsProviderType::WOLFSSL;
    }

    // Unknown name defaults to AUTO
    return DtlsProviderType::AUTO;
}

// Convert provider type to name
std::string DtlsProviderFactory::typeToName(DtlsProviderType type) {
    switch (type) {
        case DtlsProviderType::AUTO:
            return "auto";
        case DtlsProviderType::TINYDTLS:
            return "tinydtls";
        case DtlsProviderType::MBEDTLS:
            return "mbedtls";
        case DtlsProviderType::OPENSSL:
            return "openssl";
        case DtlsProviderType::WOLFSSL:
            return "wolfssl";
        default:
            return "unknown";
    }
}

// Private: Create TinyDTLS provider
DtlsProvider* DtlsProviderFactory::createTinyDTLS() {
#ifdef WITH_TINYDTLS_PROVIDER
    return new TinyDtlsProvider();
#else
    return nullptr;
#endif
}

// Private: Create mbedTLS provider
DtlsProvider* DtlsProviderFactory::createMbedTLS() {
#ifdef WITH_MBEDTLS_PROVIDER
    return new MbedTlsProvider();
#else
    return nullptr;
#endif
}

// Private: Create OpenSSL provider
DtlsProvider* DtlsProviderFactory::createOpenSSL() {
#ifdef WITH_OPENSSL_PROVIDER
    return new OpenSslProvider();
#else
    return nullptr;
#endif
}

// Private: Create wolfSSL provider
DtlsProvider* DtlsProviderFactory::createWolfSSL() {
#ifdef WITH_WOLFSSL_PROVIDER
    return new WolfSslProvider();
#else
    return nullptr;
#endif
}
