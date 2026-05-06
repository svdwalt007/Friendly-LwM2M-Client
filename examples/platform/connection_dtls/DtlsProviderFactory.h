/*******************************************************************************
 * DTLS Provider Factory
 *
 * Factory pattern for creating DTLS provider instances.
 * Supports runtime selection of DTLS library implementation.
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

#ifndef DTLS_PROVIDER_FACTORY_H
#define DTLS_PROVIDER_FACTORY_H

#include "DtlsProvider.h"
#include <string>
#include <vector>
#include <memory>

/**
 * DTLS Provider type enumeration
 */
enum class DtlsProviderType {
    AUTO,       // Auto-detect best available provider
    TINYDTLS,   // TinyDTLS 0.8.6 (embedded, lightweight)
    MBEDTLS,    // mbedTLS 3.6+ (recommended default)
    OPENSSL,    // OpenSSL 3.x (industry standard)
    WOLFSSL     // wolfSSL 5.x (performance-focused)
};

/**
 * Provider information structure
 */
struct DtlsProviderInfo {
    std::string name;           // Provider name
    std::string version;        // Version string
    bool available;             // Available in this build
    bool supports_cid;          // RFC 9146 CID support
    bool supports_dtls13;       // DTLS 1.3 support
    std::string description;    // Brief description
};

/**
 * DTLS Provider Factory
 *
 * Creates appropriate DTLS provider instances based on selection criteria.
 * Handles provider availability detection and default selection logic.
 */
class DtlsProviderFactory {
public:
    /**
     * Create DTLS provider by type
     * @param type Provider type
     * @return DtlsProvider instance, nullptr if not available
     */
    static DtlsProvider* create(DtlsProviderType type);

    /**
     * Create DTLS provider by name
     * @param name Provider name ("tinydtls", "mbedtls", "openssl", "wolfssl", "auto")
     * @return DtlsProvider instance, nullptr if not available
     */
    static DtlsProvider* create(const std::string& name);

    /**
     * Create default DTLS provider
     * Selection priority: mbedTLS > TinyDTLS > OpenSSL > wolfSSL
     * @return DtlsProvider instance, nullptr if no providers available
     */
    static DtlsProvider* createDefault();

    /**
     * Get list of available providers
     * @return Vector of provider names
     */
    static std::vector<std::string> getAvailableProviders();

    /**
     * Get detailed information about all providers
     * @return Vector of provider information structures
     */
    static std::vector<DtlsProviderInfo> getProviderInfo();

    /**
     * Check if provider is available
     * @param name Provider name
     * @return true if provider available, false otherwise
     */
    static bool isProviderAvailable(const std::string& name);

    /**
     * Check if provider is available
     * @param type Provider type
     * @return true if provider available, false otherwise
     */
    static bool isProviderAvailable(DtlsProviderType type);

    /**
     * Get default provider name
     * @return Name of default provider
     */
    static std::string getDefaultProviderName();

    /**
     * Convert provider name to type
     * @param name Provider name
     * @return Provider type, AUTO if unknown
     */
    static DtlsProviderType nameToType(const std::string& name);

    /**
     * Convert provider type to name
     * @param type Provider type
     * @return Provider name string
     */
    static std::string typeToName(DtlsProviderType type);

private:
    // Helper functions for provider creation
    static DtlsProvider* createTinyDTLS();
    static DtlsProvider* createMbedTLS();
    static DtlsProvider* createOpenSSL();
    static DtlsProvider* createWolfSSL();
};

#endif // DTLS_PROVIDER_FACTORY_H
