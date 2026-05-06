/*******************************************************************************
 * TinyDTLS Provider Implementation
 *
 * Wrapper for TinyDTLS 0.8.6 DTLS library.
 * Preserves existing functionality and RFC 9146 CID support.
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

#ifdef WITH_TINYDTLS_PROVIDER

#include "TinyDtlsProvider.h"
#include <cstring>
#include <iostream>

using namespace std;

// Constructor
TinyDtlsProvider::TinyDtlsProvider() : m_initialized(false) {
}

// Destructor
TinyDtlsProvider::~TinyDtlsProvider() {
    if (m_initialized) {
        cleanup();
    }
}

// Provider information
const char* TinyDtlsProvider::getName() const {
    return "TinyDTLS";
}

const char* TinyDtlsProvider::getVersion() const {
    return "0.8.6";
}

bool TinyDtlsProvider::supportsCID() const {
#ifdef DTLS_CID
    return true;
#else
    return false;
#endif
}

bool TinyDtlsProvider::supportsDTLS13() const {
    return false; // TinyDTLS only supports DTLS 1.2
}

// Lifecycle
int TinyDtlsProvider::init() {
    if (m_initialized) {
        return 0; // Already initialized
    }

    dtls_init();
    m_initialized = true;
    return 0;
}

int TinyDtlsProvider::cleanup() {
    if (!m_initialized) {
        return 0;
    }

    m_initialized = false;
    return 0;
}

// Context management
void* TinyDtlsProvider::createContext() {
    TinyDtlsContext* ctx = new TinyDtlsContext();
    if (!ctx) {
        return nullptr;
    }

    // Initialize context members
    ctx->dtls_ctx = nullptr;
    ctx->send_callback = nullptr;
    ctx->recv_callback = nullptr;
    ctx->event_callback = nullptr;
    ctx->user_data = nullptr;

    // Create TinyDTLS context
    ctx->dtls_ctx = dtls_new_context(ctx);
    if (!ctx->dtls_ctx) {
        delete ctx;
        return nullptr;
    }

    // Set up TinyDTLS handlers (C++17 compatible initialization)
    static dtls_handler_t handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.write = staticSendCallback;
    handlers.read = staticReadCallback;
    handlers.event = staticEventCallback;
#if DTLS_WITH_PSK
    handlers.get_psk_info = staticGetPskInfo;
    handlers.get_ecdsa_key = nullptr;
    handlers.verify_ecdsa_key = nullptr;
#elif DTLS_WITH_RPK
    handlers.get_psk_info = nullptr;
    handlers.get_ecdsa_key = staticGetEcdsaKey;
    handlers.verify_ecdsa_key = staticVerifyEcdsaKey;
#endif

    dtls_set_handler(ctx->dtls_ctx, &handlers);

    return ctx;
}

void TinyDtlsProvider::destroyContext(void* ctx) {
    if (!ctx) {
        return;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx);
    if (tiny_ctx->dtls_ctx) {
        dtls_free_context(tiny_ctx->dtls_ctx);
    }

    delete tiny_ctx;
}

// Session management
void* TinyDtlsProvider::createSession(void* ctx, const session_t* session) {
    if (!ctx || !session) {
        return nullptr;
    }

    // For TinyDTLS, we just copy the session structure
    session_t* new_session = new session_t();
    if (!new_session) {
        return nullptr;
    }

    memcpy(new_session, session, sizeof(session_t));
    return new_session;
}

void TinyDtlsProvider::destroySession(void* dtls_session) {
    if (dtls_session) {
        delete static_cast<session_t*>(dtls_session);
    }
}

// Security configuration
int TinyDtlsProvider::setPSK(void* ctx, const char* identity,
                             const uint8_t* key, size_t key_len) {
#if DTLS_WITH_PSK
    if (!ctx || !identity || !key) {
        return -1;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx);
    tiny_ctx->psk.identity = identity;
    tiny_ctx->psk.psk_key.assign(key, key + key_len);
    return 0;
#else
    return -1; // PSK not supported in this build
#endif
}

int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
                             const uint8_t* pub_key, size_t pub_len) {
#if DTLS_WITH_RPK
    if (!ctx || !priv_key || !pub_key) {
        return -1;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx);
    tiny_ctx->rpk.priv_key.assign(priv_key, priv_key + priv_len);
    tiny_ctx->rpk.pub_key.assign(pub_key, pub_key + pub_len);
    return 0;
#else
    return -1; // RPK not supported in this build
#endif
}

int TinyDtlsProvider::setCertificate(void* ctx, const char* cert_file,
                                     const char* key_file, const char* ca_file) {
    // TinyDTLS does not support X.509 certificates
    return -1;
}

// I/O operations
int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
    // This method is called by application to send encrypted data
    // TinyDTLS handles this via dtls_write which calls the send callback
    // This method is not directly used in TinyDTLS architecture
    return -1;
}

int TinyDtlsProvider::recv(void* dtls_session, uint8_t* data, size_t max_len) {
    // This method is called by application to receive decrypted data
    // TinyDTLS delivers decrypted data via the read callback
    // This method is not directly used in TinyDTLS architecture
    return -1;
}

int TinyDtlsProvider::handleMessage(void* ctx, const session_t* session,
                                    const uint8_t* data, size_t len) {
    if (!ctx || !session || !data) {
        return -1;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx);
    if (!tiny_ctx->dtls_ctx) {
        return -1;
    }

    // Cast away const for TinyDTLS API (it doesn't modify the data)
    return dtls_handle_message(tiny_ctx->dtls_ctx,
                               const_cast<session_t*>(session),
                               const_cast<uint8_t*>(data),
                               len);
}

// Handshake
int TinyDtlsProvider::connect(void* dtls_session) {
    if (!dtls_session) {
        return -1;
    }

    // For TinyDTLS, we need the context as well
    // This is a limitation of the current architecture
    // The Connection class will call dtls_connect directly
    return -1;
}

int TinyDtlsProvider::accept(void* dtls_session) {
    // TinyDTLS handles accept implicitly in dtls_handle_message
    return 0;
}

bool TinyDtlsProvider::isHandshakeComplete(void* dtls_session) const {
    // TinyDTLS doesn't expose a direct way to check handshake status
    // We rely on the event callback
    return true; // Assume complete for now
}

// CID support (RFC 9146)
int TinyDtlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
#ifdef DTLS_CID
    // TinyDTLS CID support is configured at compile time
    // CID is automatically negotiated if both peers support it
    return 0;
#else
    return -1; // CID not supported
#endif
}

bool TinyDtlsProvider::isCIDNegotiated(void* dtls_session) const {
#ifdef DTLS_CID
    // Would need to check peer state, but TinyDTLS doesn't expose this easily
    return true; // Assume negotiated if CID is enabled
#else
    return false;
#endif
}

int TinyDtlsProvider::getCIDVersion(void* dtls_session) const {
#ifdef DTLS_CID
    return 54; // RFC 9146 (Type 54)
#else
    return -1;
#endif
}

void* TinyDtlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
#ifdef DTLS_CID
    if (!ctx || !cid) {
        return nullptr;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx);
    if (!tiny_ctx->dtls_ctx) {
        return nullptr;
    }

    // TinyDTLS uses dtls_get_peer_cid but we'd need to iterate peers
    // This is a complex operation not directly supported
    return nullptr;
#else
    return nullptr;
#endif
}

// Callbacks
void TinyDtlsProvider::setSendCallback(void* ctx, SendCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx);
    tiny_ctx->send_callback = callback;
    tiny_ctx->user_data = user_data;
}

void TinyDtlsProvider::setRecvCallback(void* ctx, RecvCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx);
    tiny_ctx->recv_callback = callback;
    tiny_ctx->user_data = user_data;
}

void TinyDtlsProvider::setEventCallback(void* ctx, EventCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx);
    tiny_ctx->event_callback = callback;
    tiny_ctx->user_data = user_data;
}

// Static callback handlers
int TinyDtlsProvider::staticSendCallback(struct dtls_context_t *ctx, session_t *session,
                                         uint8 *data, size_t len) {
    if (!ctx || !ctx->app) {
        return -1;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx->app);
    if (tiny_ctx->send_callback) {
        return tiny_ctx->send_callback(session, data, len, tiny_ctx->user_data);
    }

    return -1;
}

int TinyDtlsProvider::staticReadCallback(struct dtls_context_t *ctx, session_t *session,
                                         uint8 *data, size_t len) {
    if (!ctx || !ctx->app) {
        return -1;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx->app);
    if (tiny_ctx->recv_callback) {
        return tiny_ctx->recv_callback(session, data, len, tiny_ctx->user_data);
    }

    return 0;
}

int TinyDtlsProvider::staticEventCallback(struct dtls_context_t *ctx, session_t *session,
                                          dtls_alert_level_t level, unsigned short code) {
    if (!ctx || !ctx->app) {
        return -1;
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx->app);
    if (tiny_ctx->event_callback) {
        // Convert TinyDTLS event to generic event code
        int event = (level == DTLS_ALERT_LEVEL_FATAL) ? 1 : 0;
        tiny_ctx->event_callback(session, event, tiny_ctx->user_data);
    }

    return 0;
}

#if DTLS_WITH_PSK
int TinyDtlsProvider::staticGetPskInfo(struct dtls_context_t *ctx, const session_t *session,
                                       dtls_credentials_type_t type,
                                       const unsigned char *id, size_t id_len,
                                       unsigned char *result, size_t result_length) {
    if (!ctx || !ctx->app) {
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx->app);

    switch (type) {
        case DTLS_PSK_IDENTITY: {
            if (result_length < tiny_ctx->psk.identity.size()) {
                cerr << "TinyDtlsProvider: PSK identity buffer too small" << endl;
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }
            memcpy(result, tiny_ctx->psk.identity.data(), tiny_ctx->psk.identity.size());
            return tiny_ctx->psk.identity.size();
        }

        case DTLS_PSK_KEY: {
            if (result_length < tiny_ctx->psk.psk_key.size()) {
                cerr << "TinyDtlsProvider: PSK key buffer too small" << endl;
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }
            memcpy(result, tiny_ctx->psk.psk_key.data(), tiny_ctx->psk.psk_key.size());
            return tiny_ctx->psk.psk_key.size();
        }

        case DTLS_PSK_HINT:
            // PSK hint is optional
            return 0;

        default:
            cerr << "TinyDtlsProvider: Unsupported PSK request type: " << type << endl;
            return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }
}
#endif

#if DTLS_WITH_RPK
// RPK key storage for static callback
static dtls_ecdsa_key_t g_ecdsa_key;

int TinyDtlsProvider::staticGetEcdsaKey(struct dtls_context_t *ctx, const session_t *session,
                                        const dtls_ecdsa_key_t **result) {
    if (!ctx || !ctx->app) {
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx->app);

    // Populate global key structure from stored data
    if (tiny_ctx->rpk.priv_key.size() != DTLS_EC_KEY_SIZE) {
        cerr << "TinyDtlsProvider: Invalid private key size" << endl;
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    // Expecting: 1 byte prefix + 32 bytes x + 32 bytes y
    if (tiny_ctx->rpk.pub_key.size() != (1 + DTLS_EC_KEY_SIZE * 2)) {
        cerr << "TinyDtlsProvider: Invalid public key size" << endl;
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    g_ecdsa_key.curve = DTLS_ECDH_CURVE_SECP256R1;
    g_ecdsa_key.priv_key = const_cast<uint8_t*>(tiny_ctx->rpk.priv_key.data());
    g_ecdsa_key.pub_key_x = const_cast<uint8_t*>(tiny_ctx->rpk.pub_key.data() + 1);
    g_ecdsa_key.pub_key_y = const_cast<uint8_t*>(tiny_ctx->rpk.pub_key.data() + 1 + DTLS_EC_KEY_SIZE);

    *result = &g_ecdsa_key;
    return 0;
}

int TinyDtlsProvider::staticVerifyEcdsaKey(struct dtls_context_t *ctx, const session_t *session,
                                           const unsigned char *other_pub_x,
                                           const unsigned char *other_pub_y,
                                           size_t key_size) {
    // For now, accept all keys (no verification)
    // In production, should verify against trusted public keys
    return 0;
}
#endif

// Additional required methods for abstract interface compliance

int TinyDtlsProvider::getOurCIDLength(void* dtls_session) const {
#ifdef DTLS_CID
    // TinyDTLS uses fixed CID length
    return 8;  // Default CID length
#else
    return 0;  // CID not supported
#endif
}

int TinyDtlsProvider::getPeerCIDLength(void* dtls_session) const {
#ifdef DTLS_CID
    // TinyDTLS uses fixed CID length for peers
    return 8;  // Default CID length
#else
    return 0;  // CID not supported
#endif
}

void* TinyDtlsProvider::getPeer(void* ctx, const session_t* session) {
    if (!ctx || !session) {
        return nullptr;
    }

#ifdef WITH_TINYDTLS_PROVIDER
    TinyDtlsContext* tiny_ctx = static_cast<TinyDtlsContext*>(ctx);
    if (!tiny_ctx->dtls_ctx) {
        return nullptr;
    }

    // Get peer from TinyDTLS context
    return dtls_get_peer(tiny_ctx->dtls_ctx, session);
#else
    return nullptr;
#endif
}

const char* TinyDtlsProvider::getLastError() const {
    // TinyDTLS doesn't provide error strings
    return "See console output for errors";
}

void TinyDtlsProvider::setDebugLogging(bool enable) {
    // TinyDTLS debug logging is controlled at compile time
    // We can't enable/disable it at runtime
    if (enable) {
        cerr << "TinyDtlsProvider: Debug logging is compile-time option" << endl;
    }
}

#endif // WITH_TINYDTLS_PROVIDER
