/*******************************************************************************
 * mbedTLS Provider Implementation
 *
 * Wrapper for mbedTLS 3.6+ DTLS library.
 * Production-grade DTLS with native RFC 9146 CID support.
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

#ifdef WITH_MBEDTLS_PROVIDER

#include "MbedTlsProvider.h"

#include "mbedtls/ssl.h"
#include "mbedtls/ssl_cookie.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509.h"
#include "mbedtls/pk.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/timing.h"
#include "mbedtls/version.h"

#ifdef MBEDTLS_SSL_DTLS_CONNECTION_ID
#include "mbedtls/ssl_cid.h"
#endif

#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <netinet/in.h>
#include <arpa/inet.h>

// Define mbedTLS network error constants if not available
// These are from mbedtls/net_sockets.h but we define them here
// to avoid dependency on mbedTLS network module
#ifndef MBEDTLS_ERR_NET_SEND_FAILED
#define MBEDTLS_ERR_NET_SEND_FAILED -0x004E  /**< Sending information failed */
#endif
#ifndef MBEDTLS_ERR_NET_RECV_FAILED
#define MBEDTLS_ERR_NET_RECV_FAILED -0x004C  /**< Reading information failed */
#endif

using namespace std;

// MbedTlsContext constructor
MbedTlsProvider::MbedTlsContext::MbedTlsContext() :
    config(nullptr),
    ctr_drbg(nullptr),
    entropy(nullptr),
    cert_chain(nullptr),
    private_key(nullptr),
    ca_cert(nullptr),
    send_callback(nullptr),
    recv_callback(nullptr),
    event_callback(nullptr),
    user_data(nullptr),
    cid_enabled(false),
    cid_length(0) {
}

// MbedTlsContext destructor
MbedTlsProvider::MbedTlsContext::~MbedTlsContext() {
    // Clean up sessions
    for (auto& pair : sessions) {
        if (pair.second) {
            MbedTlsSession* session = static_cast<MbedTlsSession*>(pair.second);
            delete session;
        }
    }
    sessions.clear();

    // Clean up credentials
    if (cert_chain) {
        mbedtls_x509_crt_free(cert_chain);
        delete cert_chain;
    }
    if (private_key) {
        mbedtls_pk_free(private_key);
        delete private_key;
    }
    if (ca_cert) {
        mbedtls_x509_crt_free(ca_cert);
        delete ca_cert;
    }

    // Clean up SSL config
    if (config) {
        mbedtls_ssl_config_free(config);
        delete config;
    }

    // Clean up RNG
    if (ctr_drbg) {
        mbedtls_ctr_drbg_free(ctr_drbg);
        delete ctr_drbg;
    }
    if (entropy) {
        mbedtls_entropy_free(entropy);
        delete entropy;
    }
}

// MbedTlsSession constructor
MbedTlsProvider::MbedTlsSession::MbedTlsSession() :
    ssl(nullptr),
    timer(nullptr),
    peer_session(nullptr),
    handshake_complete(false),
    parent_ctx(nullptr),
    recv_offset(0) {
}

// MbedTlsSession destructor
MbedTlsProvider::MbedTlsSession::~MbedTlsSession() {
    if (ssl) {
        mbedtls_ssl_free(ssl);
        delete ssl;
    }
    if (timer) {
        delete timer;
    }
    // peer_session is not owned by us, just a pointer, so don't delete
}

// Constructor
MbedTlsProvider::MbedTlsProvider() : m_initialized(false) {
}

// Destructor
MbedTlsProvider::~MbedTlsProvider() {
    if (m_initialized) {
        cleanup();
    }
}

// Provider information
const char* MbedTlsProvider::getName() const {
    return "mbedTLS";
}

const char* MbedTlsProvider::getVersion() const {
    return MBEDTLS_VERSION_STRING;
}

bool MbedTlsProvider::supportsCID() const {
#ifdef MBEDTLS_SSL_DTLS_CONNECTION_ID
    return true;
#else
    return false;
#endif
}

bool MbedTlsProvider::supportsDTLS13() const {
    // mbedTLS 3.6 has DTLS 1.3 in development, not fully stable yet
    return false;
}

// Lifecycle
int MbedTlsProvider::init() {
    if (m_initialized) {
        return 0; // Already initialized
    }

    m_initialized = true;
    return 0;
}

int MbedTlsProvider::cleanup() {
    if (!m_initialized) {
        return 0;
    }

    m_initialized = false;
    return 0;
}

// Context management
void* MbedTlsProvider::createContext() {
    MbedTlsContext* ctx = new MbedTlsContext();
    if (!ctx) {
        return nullptr;
    }

    // Initialize entropy
    ctx->entropy = new mbedtls_entropy_context();
    mbedtls_entropy_init(ctx->entropy);

    // Initialize RNG
    ctx->ctr_drbg = new mbedtls_ctr_drbg_context();
    mbedtls_ctr_drbg_init(ctx->ctr_drbg);

    const char* pers = "lwm2m_dtls_client";
    int ret = mbedtls_ctr_drbg_seed(ctx->ctr_drbg, mbedtls_entropy_func,
                                     ctx->entropy,
                                     (const unsigned char*)pers,
                                     strlen(pers));
    if (ret != 0) {
        cerr << "MbedTlsProvider: mbedtls_ctr_drbg_seed failed: -0x"
             << hex << -ret << dec << endl;
        delete ctx;
        return nullptr;
    }

    // Initialize SSL config
    ctx->config = new mbedtls_ssl_config();
    mbedtls_ssl_config_init(ctx->config);

    ret = mbedtls_ssl_config_defaults(ctx->config,
                                       MBEDTLS_SSL_IS_CLIENT,
                                       MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                       MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        cerr << "MbedTlsProvider: mbedtls_ssl_config_defaults failed: -0x"
             << hex << -ret << dec << endl;
        delete ctx;
        return nullptr;
    }

    // Set RNG
    mbedtls_ssl_conf_rng(ctx->config, mbedtls_ctr_drbg_random, ctx->ctr_drbg);

    // Set DTLS cookies (for server-side, but good practice)
    // Note: Would need cookie context for full server support

    return ctx;
}

void MbedTlsProvider::destroyContext(void* ctx) {
    if (!ctx) {
        return;
    }

    MbedTlsContext* mbed_ctx = static_cast<MbedTlsContext*>(ctx);
    delete mbed_ctx;
}

// Session management
void* MbedTlsProvider::createSession(void* ctx, const session_t* session) {
    if (!ctx || !session) {
        return nullptr;
    }

    MbedTlsContext* mbed_ctx = static_cast<MbedTlsContext*>(ctx);

    // Check if session already exists
    string session_key = sessionToString(session);
    auto it = mbed_ctx->sessions.find(session_key);
    if (it != mbed_ctx->sessions.end()) {
        return it->second; // Return existing session
    }

    // Create new session
    MbedTlsSession* mbed_session = new MbedTlsSession();
    if (!mbed_session) {
        return nullptr;
    }

    mbed_session->parent_ctx = mbed_ctx;

    // Store pointer to peer session (opaque, never dereferenced)
    mbed_session->peer_session = session;

    // Initialize SSL context
    mbed_session->ssl = new mbedtls_ssl_context();
    mbedtls_ssl_init(mbed_session->ssl);

    int ret = mbedtls_ssl_setup(mbed_session->ssl, mbed_ctx->config);
    if (ret != 0) {
        cerr << "MbedTlsProvider: mbedtls_ssl_setup failed: -0x"
             << hex << -ret << dec << endl;
        delete mbed_session;
        return nullptr;
    }

    // Set I/O callbacks
    mbedtls_ssl_set_bio(mbed_session->ssl, mbed_session,
                        staticSendCallback, staticRecvCallback,
                        staticRecvTimeoutCallback);

    // Set timer callbacks
    mbed_session->timer = new mbedtls_timing_delay_context();
    mbedtls_ssl_set_timer_cb(mbed_session->ssl, mbed_session->timer,
                             mbedtls_timing_set_delay,
                             mbedtls_timing_get_delay);

    // Enable CID if configured
    if (mbed_ctx->cid_enabled) {
#ifdef MBEDTLS_SSL_DTLS_CONNECTION_ID
        ret = mbedtls_ssl_set_cid(mbed_session->ssl, MBEDTLS_SSL_CID_ENABLED,
                                  nullptr, 0); // Auto-generate CID
        if (ret != 0) {
            cerr << "MbedTlsProvider: mbedtls_ssl_set_cid failed: -0x"
                 << hex << -ret << dec << endl;
        }
#endif
    }

    // Store session
    mbed_ctx->sessions[session_key] = mbed_session;

    return mbed_session;
}

void MbedTlsProvider::destroySession(void* dtls_session) {
    if (!dtls_session) {
        return;
    }

    MbedTlsSession* mbed_session = static_cast<MbedTlsSession*>(dtls_session);

    // Remove from parent context's session map
    if (mbed_session->parent_ctx && mbed_session->peer_session) {
        string session_key = sessionToString(mbed_session->peer_session);
        mbed_session->parent_ctx->sessions.erase(session_key);
    }

    delete mbed_session;
}

// Security configuration
int MbedTlsProvider::setPSK(void* ctx, const char* identity,
                            const uint8_t* key, size_t key_len) {
    if (!ctx || !identity || !key) {
        return -1;
    }

    MbedTlsContext* mbed_ctx = static_cast<MbedTlsContext*>(ctx);
    mbed_ctx->psk_identity = identity;
    mbed_ctx->psk_key.assign(key, key + key_len);

    // Configure PSK in SSL config
    int ret = mbedtls_ssl_conf_psk(mbed_ctx->config,
                                    mbed_ctx->psk_key.data(),
                                    mbed_ctx->psk_key.size(),
                                    (const unsigned char*)mbed_ctx->psk_identity.c_str(),
                                    mbed_ctx->psk_identity.size());
    if (ret != 0) {
        cerr << "MbedTlsProvider: mbedtls_ssl_conf_psk failed: -0x"
             << hex << -ret << dec << endl;
        return -1;
    }

    return 0;
}

int MbedTlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
                            const uint8_t* pub_key, size_t pub_len) {
    if (!ctx || !priv_key || !pub_key) {
        return -1;
    }

    // RPK support in mbedTLS requires raw public key extension
    // This is complex and may require mbedTLS 3.6+ features
    // For now, return unsupported
    cerr << "MbedTlsProvider: RPK mode not yet implemented" << endl;
    return -1;
}

int MbedTlsProvider::setCertificate(void* ctx, const char* cert_file,
                                    const char* key_file, const char* ca_file) {
    if (!ctx || !cert_file || !key_file) {
        return -1;
    }

    MbedTlsContext* mbed_ctx = static_cast<MbedTlsContext*>(ctx);

    // Load certificate chain
    mbed_ctx->cert_chain = new mbedtls_x509_crt();
    mbedtls_x509_crt_init(mbed_ctx->cert_chain);

    int ret = mbedtls_x509_crt_parse_file(mbed_ctx->cert_chain, cert_file);
    if (ret != 0) {
        cerr << "MbedTlsProvider: mbedtls_x509_crt_parse_file failed: -0x"
             << hex << -ret << dec << endl;
        return -1;
    }

    // Load private key
    mbed_ctx->private_key = new mbedtls_pk_context();
    mbedtls_pk_init(mbed_ctx->private_key);

    ret = mbedtls_pk_parse_keyfile(mbed_ctx->private_key, key_file, nullptr);
    if (ret != 0) {
        cerr << "MbedTlsProvider: mbedtls_pk_parse_keyfile failed: -0x"
             << hex << -ret << dec << endl;
        return -1;
    }

    // Configure certificate and key
    ret = mbedtls_ssl_conf_own_cert(mbed_ctx->config,
                                     mbed_ctx->cert_chain,
                                     mbed_ctx->private_key);
    if (ret != 0) {
        cerr << "MbedTlsProvider: mbedtls_ssl_conf_own_cert failed: -0x"
             << hex << -ret << dec << endl;
        return -1;
    }

    // Load CA certificate if provided
    if (ca_file) {
        mbed_ctx->ca_cert = new mbedtls_x509_crt();
        mbedtls_x509_crt_init(mbed_ctx->ca_cert);

        ret = mbedtls_x509_crt_parse_file(mbed_ctx->ca_cert, ca_file);
        if (ret != 0) {
            cerr << "MbedTlsProvider: mbedtls_x509_crt_parse_file (CA) failed: -0x"
                 << hex << -ret << dec << endl;
            return -1;
        }

        mbedtls_ssl_conf_ca_chain(mbed_ctx->config, mbed_ctx->ca_cert, nullptr);
        mbedtls_ssl_conf_authmode(mbed_ctx->config, MBEDTLS_SSL_VERIFY_REQUIRED);
    }

    return 0;
}

// I/O operations
int MbedTlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
    if (!dtls_session || !data) {
        return -1;
    }

    MbedTlsSession* mbed_session = static_cast<MbedTlsSession*>(dtls_session);

    int ret = mbedtls_ssl_write(mbed_session->ssl, data, len);
    if (ret < 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ) {
            cerr << "MbedTlsProvider: mbedtls_ssl_write failed: -0x"
                 << hex << -ret << dec << endl;
        }
        return mbedtlsToGenericError(ret);
    }

    return ret;
}

int MbedTlsProvider::recv(void* dtls_session, uint8_t* data, size_t max_len) {
    if (!dtls_session || !data) {
        return -1;
    }

    MbedTlsSession* mbed_session = static_cast<MbedTlsSession*>(dtls_session);

    int ret = mbedtls_ssl_read(mbed_session->ssl, data, max_len);
    if (ret < 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            cerr << "MbedTlsProvider: mbedtls_ssl_read failed: -0x"
                 << hex << -ret << dec << endl;
        }
        return mbedtlsToGenericError(ret);
    }

    return ret;
}

int MbedTlsProvider::handleMessage(void* ctx, const session_t* session,
                                   const uint8_t* data, size_t len) {
    if (!ctx || !session || !data) {
        return -1;
    }

    MbedTlsContext* mbed_ctx = static_cast<MbedTlsContext*>(ctx);

    // Find or create session
    string session_key = sessionToString(session);
    auto it = mbed_ctx->sessions.find(session_key);

    MbedTlsSession* mbed_session;
    if (it == mbed_ctx->sessions.end()) {
        // Create new session
        mbed_session = static_cast<MbedTlsSession*>(createSession(ctx, session));
        if (!mbed_session) {
            return -1;
        }
    } else {
        mbed_session = static_cast<MbedTlsSession*>(it->second);
    }

    // Store received data in session buffer
    mbed_session->recv_buffer.insert(mbed_session->recv_buffer.end(), data, data + len);

    // If handshake not complete, drive handshake
    if (!mbed_session->handshake_complete) {
        int ret;
        while ((ret = mbedtls_ssl_handshake(mbed_session->ssl)) != 0) {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                cerr << "MbedTlsProvider: mbedtls_ssl_handshake failed: -0x"
                     << hex << -ret << dec << endl;
                return -1;
            }
            break; // Need more data
        }

        if (ret == 0) {
            mbed_session->handshake_complete = true;

            // Notify application via event callback
            if (mbed_ctx->event_callback) {
                mbed_ctx->event_callback(session, 0, mbed_ctx->user_data);
            }
        }
    }

    return 0;
}

// Handshake
int MbedTlsProvider::connect(void* dtls_session) {
    if (!dtls_session) {
        return -1;
    }

    MbedTlsSession* mbed_session = static_cast<MbedTlsSession*>(dtls_session);

    int ret;
    while ((ret = mbedtls_ssl_handshake(mbed_session->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            cerr << "MbedTlsProvider: mbedtls_ssl_handshake failed: -0x"
                 << hex << -ret << dec << endl;
            return -1;
        }
        // Will complete asynchronously
        return 0;
    }

    mbed_session->handshake_complete = true;
    return 0;
}

int MbedTlsProvider::accept(void* dtls_session) {
    // Same as connect for DTLS
    return connect(dtls_session);
}

bool MbedTlsProvider::isHandshakeComplete(void* dtls_session) const {
    if (!dtls_session) {
        return false;
    }

    MbedTlsSession* mbed_session = static_cast<MbedTlsSession*>(dtls_session);
    return mbed_session->handshake_complete;
}

// CID support (RFC 9146)
int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
#ifdef MBEDTLS_SSL_DTLS_CONNECTION_ID
    if (!dtls_session) {
        return -1;
    }

    MbedTlsSession* mbed_session = static_cast<MbedTlsSession*>(dtls_session);
    mbed_session->parent_ctx->cid_enabled = true;
    mbed_session->parent_ctx->cid_length = cid_len;

    int ret = mbedtls_ssl_set_cid(mbed_session->ssl, MBEDTLS_SSL_CID_ENABLED,
                                   nullptr, 0); // Auto-generate
    if (ret != 0) {
        cerr << "MbedTlsProvider: mbedtls_ssl_set_cid failed: -0x"
             << hex << -ret << dec << endl;
        return -1;
    }

    return 0;
#else
    return -1; // CID not supported
#endif
}

bool MbedTlsProvider::isCIDNegotiated(void* dtls_session) const {
#ifdef MBEDTLS_SSL_DTLS_CONNECTION_ID
    if (!dtls_session) {
        return false;
    }

    MbedTlsSession* mbed_session = static_cast<MbedTlsSession*>(dtls_session);

    int cid_enabled;
    unsigned char peer_cid[MBEDTLS_SSL_CID_OUT_LEN_MAX];
    size_t peer_cid_len;

    int ret = mbedtls_ssl_get_peer_cid(mbed_session->ssl, &cid_enabled,
                                        peer_cid, &peer_cid_len);
    if (ret == 0 && cid_enabled == MBEDTLS_SSL_CID_ENABLED) {
        return true;
    }

    return false;
#else
    return false;
#endif
}

int MbedTlsProvider::getCIDVersion(void* dtls_session) const {
#ifdef MBEDTLS_SSL_DTLS_CONNECTION_ID
    return 54; // RFC 9146
#else
    return -1;
#endif
}

void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
    // mbedTLS doesn't expose CID-based peer lookup directly
    // Would need to maintain separate CID mapping
    return nullptr;
}

// Callbacks
void MbedTlsProvider::setSendCallback(void* ctx, SendCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    MbedTlsContext* mbed_ctx = static_cast<MbedTlsContext*>(ctx);
    mbed_ctx->send_callback = callback;
    mbed_ctx->user_data = user_data;
}

void MbedTlsProvider::setRecvCallback(void* ctx, RecvCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    MbedTlsContext* mbed_ctx = static_cast<MbedTlsContext*>(ctx);
    mbed_ctx->recv_callback = callback;
    mbed_ctx->user_data = user_data;
}

void MbedTlsProvider::setEventCallback(void* ctx, EventCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    MbedTlsContext* mbed_ctx = static_cast<MbedTlsContext*>(ctx);
    mbed_ctx->event_callback = callback;
    mbed_ctx->user_data = user_data;
}

// Static callback handlers
int MbedTlsProvider::staticSendCallback(void* ctx, const unsigned char* buf, size_t len) {
    if (!ctx) {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    MbedTlsSession* mbed_session = static_cast<MbedTlsSession*>(ctx);
    if (!mbed_session->parent_ctx || !mbed_session->parent_ctx->send_callback ||
        !mbed_session->peer_session) {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    int ret = mbed_session->parent_ctx->send_callback(mbed_session->peer_session,
                                                       buf, len,
                                                       mbed_session->parent_ctx->user_data);
    if (ret < 0) {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    return ret;
}

int MbedTlsProvider::staticRecvCallback(void* ctx, unsigned char* buf, size_t len) {
    return staticRecvTimeoutCallback(ctx, buf, len, 0);
}

int MbedTlsProvider::staticRecvTimeoutCallback(void* ctx, unsigned char* buf, size_t len,
                                                uint32_t timeout) {
    if (!ctx) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    MbedTlsSession* mbed_session = static_cast<MbedTlsSession*>(ctx);

    // Read from buffered data
    if (mbed_session->recv_buffer.empty()) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    }

    size_t to_copy = min(len, mbed_session->recv_buffer.size() - mbed_session->recv_offset);
    memcpy(buf, mbed_session->recv_buffer.data() + mbed_session->recv_offset, to_copy);
    mbed_session->recv_offset += to_copy;

    // Clear buffer if all data consumed
    if (mbed_session->recv_offset >= mbed_session->recv_buffer.size()) {
        mbed_session->recv_buffer.clear();
        mbed_session->recv_offset = 0;
    }

    return to_copy;
}

// Helper functions
// Convert session_t pointer to string key for session map
// Since session_t is an opaque type (defined by DTLS library), we can't
// access its members. Use the pointer value as a unique identifier.
string MbedTlsProvider::sessionToString(const session_t* session) {
    if (!session) {
        return "";
    }

    ostringstream oss;
    oss << reinterpret_cast<uintptr_t>(session);
    return oss.str();
}

int MbedTlsProvider::mbedtlsToGenericError(int mbedtls_error) {
    if (mbedtls_error >= 0) {
        return mbedtls_error;
    }

    // Map common mbedTLS errors to generic errors
    switch (mbedtls_error) {
        case MBEDTLS_ERR_SSL_WANT_READ:
        case MBEDTLS_ERR_SSL_WANT_WRITE:
            return 0; // Not an error, just needs more I/O

        case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
            return -2; // Connection closed

        default:
            return -1; // Generic error
    }
}

// Get our CID length
int MbedTlsProvider::getOurCIDLength(void* dtls_session) const {
    // mbedTLS CID support - return 0 for now (not implemented)
    (void)dtls_session;
    return 0;
}

// Get peer CID length
int MbedTlsProvider::getPeerCIDLength(void* dtls_session) const {
    // mbedTLS CID support - return 0 for now (not implemented)
    (void)dtls_session;
    return 0;
}

// Get peer by session
void* MbedTlsProvider::getPeer(void* ctx, const session_t* session) {
    // Not used in mbedTLS provider - return nullptr
    (void)ctx;
    (void)session;
    return nullptr;
}

// Get last error
const char* MbedTlsProvider::getLastError() const {
    return "mbedTLS error"; // TODO: Implement proper error tracking
}

// Set debug logging
void MbedTlsProvider::setDebugLogging(bool enable) {
    // TODO: Implement mbedTLS debug logging
    (void)enable;
}

#endif // WITH_MBEDTLS_PROVIDER
