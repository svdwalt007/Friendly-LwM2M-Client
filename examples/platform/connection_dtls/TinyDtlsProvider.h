/*******************************************************************************
 * TinyDTLS Provider Implementation
 *
 * Wrapper for TinyDTLS 0.8.6 DTLS library.
 * Preserves existing functionality and RFC 9146 CID support.
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

#ifndef TINY_DTLS_PROVIDER_H
#define TINY_DTLS_PROVIDER_H

#include <vector>
#include <string>

// Include TinyDTLS headers BEFORE DtlsProvider.h to ensure session_t is defined
#ifdef WITH_TINYDTLS_PROVIDER
extern "C" {
#include "tinydtls.h"
#include "dtls.h"
}

// Undefine TinyDTLS macros that conflict with C++ standard library
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif

// Include after TinyDTLS headers so session_t typedef is already defined
#include "DtlsProvider.h"

/**
 * TinyDTLS Provider
 *
 * Implements DTLS provider interface using TinyDTLS 0.8.6.
 * Supports PSK and RPK authentication modes.
 * Includes RFC 9146 Connection ID (CID) support.
 */
class TinyDtlsProvider : public DtlsProvider {
public:
    TinyDtlsProvider();
    virtual ~TinyDtlsProvider();

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
    virtual int getOurCIDLength(void* dtls_session) const override;
    virtual int getPeerCIDLength(void* dtls_session) const override;
    virtual void* getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) override;
    virtual void* getPeer(void* ctx, const session_t* session) override;

    // Callbacks
    virtual void setSendCallback(void* ctx, SendCallback callback, void* user_data) override;
    virtual void setRecvCallback(void* ctx, RecvCallback callback, void* user_data) override;
    virtual void setEventCallback(void* ctx, EventCallback callback, void* user_data) override;

    // Debug and error handling
    virtual const char* getLastError() const override;
    virtual void setDebugLogging(bool enable) override;

private:
    bool m_initialized;

    // Internal context structure to wrap TinyDTLS context
    struct TinyDtlsContext {
        dtls_context_t* dtls_ctx;
        SendCallback send_callback;
        RecvCallback recv_callback;
        EventCallback event_callback;
        void* user_data;

        // Security credentials storage
        struct {
            std::string identity;
            std::vector<uint8_t> psk_key;
        } psk;

        struct {
            std::vector<uint8_t> priv_key;
            std::vector<uint8_t> pub_key;
        } rpk;
    };

    // Static callback handlers for TinyDTLS
    static int staticSendCallback(struct dtls_context_t *ctx, session_t *session,
                                   uint8 *data, size_t len);
    static int staticReadCallback(struct dtls_context_t *ctx, session_t *session,
                                   uint8 *data, size_t len);
    static int staticEventCallback(struct dtls_context_t *ctx, session_t *session,
                                    dtls_alert_level_t level, unsigned short code);

#if DTLS_WITH_PSK
    static int staticGetPskInfo(struct dtls_context_t *ctx, const session_t *session,
                                dtls_credentials_type_t type,
                                const unsigned char *id, size_t id_len,
                                unsigned char *result, size_t result_length);
#endif

#if DTLS_WITH_RPK
    static int staticGetEcdsaKey(struct dtls_context_t *ctx, const session_t *session,
                                 const dtls_ecdsa_key_t **result);
    static int staticVerifyEcdsaKey(struct dtls_context_t *ctx, const session_t *session,
                                     const unsigned char *other_pub_x,
                                     const unsigned char *other_pub_y,
                                     size_t key_size);
#endif
};

#endif // TINY_DTLS_PROVIDER_H
