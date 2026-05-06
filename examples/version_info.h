/*******************************************************************************
 * Friendly LwM2M Client - Version Information
 *
 * This file contains version information for the client and all libraries
 ******************************************************************************/

#ifndef VERSION_INFO_H
#define VERSION_INFO_H

#include <string>

// Client Version
#define FRIENDLY_CLIENT_VERSION_MAJOR 1
#define FRIENDLY_CLIENT_VERSION_MINOR 2
#define FRIENDLY_CLIENT_VERSION_PATCH 2
#define FRIENDLY_CLIENT_VERSION "1.2.2"
#define FRIENDLY_CLIENT_BUILD_DATE __DATE__
#define FRIENDLY_CLIENT_BUILD_TIME __TIME__

// LwM2M Protocol Version
#define LWM2M_VERSION "1.0/1.1"

// CoAP Library Version (libcoap)
#define COAP_LIBRARY "libcoap"
#define COAP_VERSION "4.3.4"

// DTLS Library Versions
// Note: Use DTLS_LIB_VERSION to avoid conflict with TinyDTLS's DTLS_LIB_VERSION macro
#ifdef WITH_TINYDTLS
    #define DTLS_LIBRARY "TinyDTLS"
    #define DTLS_LIB_VERSION "0.8.6"
    #define DTLS_PROVIDER_NAME "TinyDTLS (Embedded)"
#elif defined(WITH_OPENSSL)
    #include <openssl/opensslv.h>
    #define DTLS_LIBRARY "OpenSSL"
    #define DTLS_LIB_VERSION OPENSSL_VERSION_TEXT
    #define DTLS_PROVIDER_NAME "OpenSSL"
#elif defined(WITH_MBEDTLS)
    #include "mbedtls/version.h"
    #define DTLS_LIBRARY "mbedTLS"
    #define DTLS_LIB_VERSION MBEDTLS_LIB_VERSION_STRING
    #define DTLS_PROVIDER_NAME "mbedTLS"
#elif defined(WITH_WOLFSSL)
    #include <wolfssl/version.h>
    #define DTLS_LIBRARY "wolfSSL"
    #define DTLS_LIB_VERSION LIBWOLFSSL_VERSION_STRING
    #define DTLS_PROVIDER_NAME "wolfSSL"
#else
    #define DTLS_LIBRARY "None"
    #define DTLS_LIB_VERSION "N/A"
    #define DTLS_PROVIDER_NAME "No DTLS"
#endif

// DTLS Connection ID Support (RFC 9146)
#ifdef DTLS_CID
    #define CID_SUPPORT "RFC 9146 (Type 54) + draft (Type 53)"
    #define CID_VERSION "1.0"
#else
    #define CID_SUPPORT "Not available"
    #define CID_VERSION "N/A"
#endif

// Feature Flags
#ifdef WITH_UDP_DTLS
    #define FEATURE_DTLS "Enabled"
#else
    #define FEATURE_DTLS "Disabled"
#endif

#ifdef DTLS_WITH_PSK
    #define FEATURE_PSK "Enabled"
#else
    #define FEATURE_PSK "Disabled"
#endif

#ifdef DTLS_WITH_RPK
    #define FEATURE_RPK "Enabled"
#else
    #define FEATURE_RPK "Disabled"
#endif

#ifdef DTLS_WITH_CERT
    #define FEATURE_CERT "Enabled"
#else
    #define FEATURE_CERT "Disabled"
#endif

namespace version {

/**
 * Get full version banner for display
 */
inline std::string getVersionBanner() {
    return std::string(
        "==================================================================\n"
        "  Friendly LwM2M Client v" FRIENDLY_CLIENT_VERSION "\n"
        "  Build: " FRIENDLY_CLIENT_BUILD_DATE " " FRIENDLY_CLIENT_BUILD_TIME "\n"
        "==================================================================\n"
        "\n"
        "Protocol & Library Versions:\n"
        "  LwM2M Protocol:     " LWM2M_VERSION "\n"
        "  CoAP Library:       " COAP_LIBRARY " " COAP_VERSION "\n"
        "  DTLS Provider:      " DTLS_PROVIDER_NAME "\n"
        "  DTLS Library:       " DTLS_LIBRARY " " DTLS_LIB_VERSION "\n"
        "  CID Support:        " CID_SUPPORT "\n"
        "\n"
        "Security Features:\n"
        "  DTLS:               " FEATURE_DTLS "\n"
        "  PSK:                " FEATURE_PSK "\n"
        "  RPK:                " FEATURE_RPK "\n"
        "  Certificate:        " FEATURE_CERT "\n"
        "==================================================================\n"
    );
}

/**
 * Get short version string
 */
inline std::string getVersionString() {
    return "Friendly LwM2M Client v" FRIENDLY_CLIENT_VERSION
           " (LwM2M " LWM2M_VERSION ", " DTLS_PROVIDER_NAME ")";
}

/**
 * Get version for --version flag
 */
inline std::string getDetailedVersion() {
    return std::string(
        "Friendly LwM2M Client v" FRIENDLY_CLIENT_VERSION "\n"
        "Built: " FRIENDLY_CLIENT_BUILD_DATE " " FRIENDLY_CLIENT_BUILD_TIME "\n"
        "\n"
        "Components:\n"
        "  LwM2M:  " LWM2M_VERSION "\n"
        "  CoAP:   " COAP_LIBRARY " " COAP_VERSION "\n"
        "  DTLS:   " DTLS_LIBRARY " " DTLS_LIB_VERSION "\n"
        "  CID:    " CID_SUPPORT "\n"
    );
}

} // namespace version

#endif // VERSION_INFO_H
