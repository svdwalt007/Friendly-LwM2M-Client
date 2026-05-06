/*******************************************************************************
 * mbedTLS Provider Implementation
 *
 * Wrapper for mbedTLS 3.6+ DTLS library.
 * Production-grade DTLS with native RFC 9146 CID support.
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

#ifndef MBED_TLS_PROVIDER_H
#define MBED_TLS_PROVIDER_H

#include "DtlsProvider.h"
#include <string>
#include <vector>
#include <map>

// Forward declare mbedTLS structures
struct mbedtls_ssl_context;
struct mbedtls_ssl_config;
struct mbedtls_ctr_drbg_context;
struct mbedtls_entropy_context;
struct mbedtls_x509_crt;
struct mbedtls_pk_context;
struct mbedtls_timing_delay_context;

/**
 * mbedTLS Provider
 *
 * Implements DTLS provider interface using mbedTLS 3.6+.
 * Supports PSK, RPK, and Certificate authentication modes.
 * Includes native RFC 9146 Connection ID (CID) support.
 * Recommended default for production deployments.
 */
class MbedTlsProvider : public DtlsProvider {
public:
    MbedTlsProvider();
    virtual ~MbedTlsProvider();

    // Provider information
    virtual const char* getName() const override;
    virtual const char* getVersion() const override;
    virtual bool supportsCID() const override;
    virtual bool supportsDTLS13() const override;

    // Lifecycle
    virtual int init() override;
    virtual int cleanup() override;

    // Context management
    virtual void* createContext() override;
    virtual void destroyContext(void* ctx) override;

    // Session management
    virtual void* createSession(void* ctx, const session_t* session) override;
    virtual void destroySession(void* dtls_session) override;

    // Security configuration
    virtual int setPSK(void* ctx, const char* identity,
                       const uint8_t* key, size_t key_len) override;
    virtual int setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
                       const uint8_t* pub_key, size_t pub_len) override;
    virtual int setCertificate(void* ctx, const char* cert_file,
                               const char* key_file, const char* ca_file) override;

    // I/O operations
    virtual int send(void* dtls_session, const uint8_t* data, size_t len) override;
    virtual int recv(void* dtls_session, uint8_t* data, size_t max_len) override;
    virtual int handleMessage(void* ctx, const session_t* session,
                              const uint8_t* data, size_t len) override;

    // Handshake
    virtual int connect(void* dtls_session) override;
    virtual int accept(void* dtls_session) override;
    virtual bool isHandshakeComplete(void* dtls_session) const override;

    // CID support (RFC 9146)
    virtual int enableCID(void* dtls_session, uint8_t cid_len) override;
    virtual bool isCIDNegotiated(void* dtls_session) const override;
    virtual int getCIDVersion(void* dtls_session) const override;
    virtual void* getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) override;
    virtual int getOurCIDLength(void* dtls_session) const override;
    virtual int getPeerCIDLength(void* dtls_session) const override;

    // Peer information
    virtual void* getPeer(void* ctx, const session_t* session) override;

    // Error handling
    virtual const char* getLastError() const override;
    virtual void setDebugLogging(bool enable) override;

    // Callbacks
    virtual void setSendCallback(void* ctx, SendCallback callback, void* user_data) override;
    virtual void setRecvCallback(void* ctx, RecvCallback callback, void* user_data) override;
    virtual void setEventCallback(void* ctx, EventCallback callback, void* user_data) override;

private:
    bool m_initialized;

    // Internal context structure to wrap mbedTLS context
    struct MbedTlsContext {
        mbedtls_ssl_config* config;
        mbedtls_ctr_drbg_context* ctr_drbg;
        mbedtls_entropy_context* entropy;

        // Credentials
        std::string psk_identity;
        std::vector<uint8_t> psk_key;

        mbedtls_x509_crt* cert_chain;
        mbedtls_pk_context* private_key;
        mbedtls_x509_crt* ca_cert;

        // Callbacks
        SendCallback send_callback;
        RecvCallback recv_callback;
        EventCallback event_callback;
        void* user_data;

        // Session management
        std::map<std::string, void*> sessions; // Map session address to SSL context

        // CID configuration
        bool cid_enabled;
        uint8_t cid_length;

        MbedTlsContext();
        ~MbedTlsContext();
    };

    // Internal session structure
    struct MbedTlsSession {
        mbedtls_ssl_context* ssl;
        mbedtls_timing_delay_context* timer;
        const session_t* peer_session;  // Opaque pointer, never dereferenced
        bool handshake_complete;
        MbedTlsContext* parent_ctx;

        // I/O buffer for partial reads/writes
        std::vector<uint8_t> recv_buffer;
        size_t recv_offset;

        MbedTlsSession();
        ~MbedTlsSession();
    };

    // Static callback handlers for mbedTLS
    static int staticSendCallback(void* ctx, const unsigned char* buf, size_t len);
    static int staticRecvCallback(void* ctx, unsigned char* buf, size_t len);
    static int staticRecvTimeoutCallback(void* ctx, unsigned char* buf, size_t len,
                                         uint32_t timeout);

    // PSK callback
    static int staticPskCallback(void* parameter, mbedtls_ssl_context* ssl,
                                 const unsigned char* psk_identity,
                                 size_t identity_len);

    // Helper functions
    static std::string sessionToString(const session_t* session);
    static int mbedtlsToGenericError(int mbedtls_error);
};

#endif // MBED_TLS_PROVIDER_H
