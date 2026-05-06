/*******************************************************************************
 * wolfSSL Provider Implementation
 *
 * Wrapper for wolfSSL 5.x DTLS library.
 * Performance-focused SSL/TLS with DTLS 1.2/1.3 support.
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

#ifndef WOLFSSL_PROVIDER_H
#define WOLFSSL_PROVIDER_H

#include "DtlsProvider.h"
#include <string>
#include <vector>
#include <map>

// Forward declare wolfSSL structures
typedef struct WOLFSSL_CTX WOLFSSL_CTX;
typedef struct WOLFSSL WOLFSSL;

/**
 * wolfSSL Provider
 *
 * Implements DTLS provider interface using wolfSSL 5.x.
 * Supports PSK, RPK, and Certificate authentication modes.
 * Supports DTLS 1.2 and DTLS 1.3.
 * Performance-optimized for embedded systems.
 */
class WolfSslProvider : public DtlsProvider {
public:
    WolfSslProvider();
    virtual ~WolfSslProvider();

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

    // Callbacks
    virtual void setSendCallback(void* ctx, SendCallback callback, void* user_data) override;
    virtual void setRecvCallback(void* ctx, RecvCallback callback, void* user_data) override;
    virtual void setEventCallback(void* ctx, EventCallback callback, void* user_data) override;

private:
    bool m_initialized;

    // Internal context structure to wrap wolfSSL context
    struct WolfSslContext {
        WOLFSSL_CTX* ctx;

        // Credentials
        std::string psk_identity;
        std::vector<uint8_t> psk_key;

        // Callbacks
        SendCallback send_callback;
        RecvCallback recv_callback;
        EventCallback event_callback;
        void* user_data;

        // Session management
        std::map<std::string, void*> sessions;

        // CID configuration
        bool cid_enabled;
        uint8_t cid_length;

        WolfSslContext();
        ~WolfSslContext();
    };

    // Internal session structure
    struct WolfSslSession {
        WOLFSSL* ssl;
        session_t peer_session;
        bool handshake_complete;
        WolfSslContext* parent_ctx;

        // I/O buffers
        std::vector<uint8_t> recv_buffer;
        size_t recv_offset;

        WolfSslSession();
        ~WolfSslSession();
    };

    // Static callback handlers for wolfSSL
    static unsigned int staticPskClientCallback(WOLFSSL* ssl, const char* hint,
                                                char* identity, unsigned int id_max_len,
                                                unsigned char* key, unsigned int key_max_len);
    static unsigned int staticPskServerCallback(WOLFSSL* ssl, const char* identity,
                                                unsigned char* key, unsigned int key_max_len);

    // I/O callbacks
    static int staticRecvCallback(WOLFSSL* ssl, char* buf, int sz, void* ctx);
    static int staticSendCallback(WOLFSSL* ssl, char* buf, int sz, void* ctx);

    // Helper functions
    static std::string sessionToString(const session_t* session);
    static int wolfsslToGenericError(int wolfssl_error);
};

#endif // WOLFSSL_PROVIDER_H
