/*******************************************************************************
 * OpenSSL Provider Implementation
 *
 * Wrapper for OpenSSL 3.x DTLS library.
 * Industry-standard SSL/TLS with full DTLS 1.2/1.3 support.
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

#ifndef OPENSSL_PROVIDER_H
#define OPENSSL_PROVIDER_H

#include "DtlsProvider.h"
#include <string>
#include <vector>
#include <map>

// Forward declare OpenSSL structures
typedef struct ssl_ctx_st SSL_CTX;
typedef struct ssl_st SSL;
typedef struct bio_st BIO;

/**
 * OpenSSL Provider
 *
 * Implements DTLS provider interface using OpenSSL 3.x.
 * Supports PSK, RPK, and Certificate authentication modes.
 * Supports DTLS 1.2 and DTLS 1.3.
 * CID support available in OpenSSL 3.2+.
 */
class OpenSslProvider : public DtlsProvider {
public:
    OpenSslProvider();
    virtual ~OpenSslProvider();

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

    // Internal context structure to wrap OpenSSL context
    struct OpenSslContext {
        SSL_CTX* ssl_ctx;

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

        OpenSslContext();
        ~OpenSslContext();
    };

    // Internal session structure
    struct OpenSslSession {
        SSL* ssl;
        BIO* read_bio;
        BIO* write_bio;
        session_t peer_session;
        bool handshake_complete;
        OpenSslContext* parent_ctx;

        // I/O buffers
        std::vector<uint8_t> recv_buffer;
        size_t recv_offset;

        OpenSslSession();
        ~OpenSslSession();
    };

    // Static callback handlers for OpenSSL
    static unsigned int staticPskClientCallback(SSL* ssl, const char* hint,
                                                 char* identity, unsigned int max_identity_len,
                                                 unsigned char* psk, unsigned int max_psk_len);
    static unsigned int staticPskServerCallback(SSL* ssl, const char* identity,
                                                 unsigned char* psk, unsigned int max_psk_len);

    // BIO callbacks
    static int staticBioWrite(BIO* bio, const char* data, int len);
    static int staticBioRead(BIO* bio, char* data, int len);
    static long staticBioCtrl(BIO* bio, int cmd, long larg, void* parg);
    static int staticBioCreate(BIO* bio);
    static int staticBioDestroy(BIO* bio);

    // Helper functions
    static std::string sessionToString(const session_t* session);
    static int opensslToGenericError(int ssl_error);
};

#endif // OPENSSL_PROVIDER_H
