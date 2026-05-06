/*******************************************************************************
 * wolfSSL Provider Implementation
 *
 * Wrapper for wolfSSL 5.x DTLS library.
 * Performance-focused SSL/TLS with DTLS 1.2/1.3 support.
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

#include "WolfSslProvider.h"

#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/version.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

// WolfSslContext constructor
WolfSslProvider::WolfSslContext::WolfSslContext() :
    ctx(nullptr),
    send_callback(nullptr),
    recv_callback(nullptr),
    event_callback(nullptr),
    user_data(nullptr),
    cid_enabled(false),
    cid_length(0) {
}

// WolfSslContext destructor
WolfSslProvider::WolfSslContext::~WolfSslContext() {
    // Clean up sessions
    for (auto& pair : sessions) {
        if (pair.second) {
            WolfSslSession* session = static_cast<WolfSslSession*>(pair.second);
            delete session;
        }
    }
    sessions.clear();

    // Clean up wolfSSL context
    if (ctx) {
        wolfSSL_CTX_free(ctx);
    }
}

// WolfSslSession constructor
WolfSslProvider::WolfSslSession::WolfSslSession() :
    ssl(nullptr),
    handshake_complete(false),
    parent_ctx(nullptr),
    recv_offset(0) {
    memset(&peer_session, 0, sizeof(peer_session));
}

// WolfSslSession destructor
WolfSslProvider::WolfSslSession::~WolfSslSession() {
    if (ssl) {
        wolfSSL_free(ssl);
    }
}

// Constructor
WolfSslProvider::WolfSslProvider() : m_initialized(false) {
}

// Destructor
WolfSslProvider::~WolfSslProvider() {
    if (m_initialized) {
        cleanup();
    }
}

// Provider information
const char* WolfSslProvider::getName() const {
    return "wolfSSL";
}

const char* WolfSslProvider::getVersion() const {
    return LIBWOLFSSL_VERSION_STRING;
}

bool WolfSslProvider::supportsCID() const {
#ifdef WOLFSSL_DTLS_CID
    return true;
#else
    return false;
#endif
}

bool WolfSslProvider::supportsDTLS13() const {
#ifdef WOLFSSL_DTLS13
    return true;
#else
    return false;
#endif
}

// Lifecycle
int WolfSslProvider::init() {
    if (m_initialized) {
        return 0; // Already initialized
    }

    // Initialize wolfSSL library
    if (wolfSSL_Init() != SSL_SUCCESS) {
        cerr << "WolfSslProvider: wolfSSL_Init failed" << endl;
        return -1;
    }

    m_initialized = true;
    return 0;
}

int WolfSslProvider::cleanup() {
    if (!m_initialized) {
        return 0;
    }

    wolfSSL_Cleanup();
    m_initialized = false;
    return 0;
}

// Context management
void* WolfSslProvider::createContext() {
    WolfSslContext* ctx = new WolfSslContext();
    if (!ctx) {
        return nullptr;
    }

    // Create wolfSSL context with DTLSv1.2 method
    ctx->ctx = wolfSSL_CTX_new(wolfDTLSv1_2_client_method());
    if (!ctx->ctx) {
        cerr << "WolfSslProvider: wolfSSL_CTX_new failed" << endl;
        delete ctx;
        return nullptr;
    }

    // Set verification mode
    wolfSSL_CTX_set_verify(ctx->ctx, SSL_VERIFY_PEER, nullptr);

    // Store context pointer for callbacks
    wolfSSL_CTX_SetIORecv(ctx->ctx, staticRecvCallback);
    wolfSSL_CTX_SetIOSend(ctx->ctx, staticSendCallback);

    return ctx;
}

void WolfSslProvider::destroyContext(void* ctx) {
    if (!ctx) {
        return;
    }

    WolfSslContext* wolf_ctx = static_cast<WolfSslContext*>(ctx);
    delete wolf_ctx;
}

// Session management
void* WolfSslProvider::createSession(void* ctx, const session_t* session) {
    if (!ctx || !session) {
        return nullptr;
    }

    WolfSslContext* wolf_ctx = static_cast<WolfSslContext*>(ctx);

    // Check if session already exists
    string session_key = sessionToString(session);
    auto it = wolf_ctx->sessions.find(session_key);
    if (it != wolf_ctx->sessions.end()) {
        return it->second; // Return existing session
    }

    // Create new session
    WolfSslSession* wolf_session = new WolfSslSession();
    if (!wolf_session) {
        return nullptr;
    }

    wolf_session->parent_ctx = wolf_ctx;
    memcpy(&wolf_session->peer_session, session, sizeof(session_t));

    // Create wolfSSL object
    wolf_session->ssl = wolfSSL_new(wolf_ctx->ctx);
    if (!wolf_session->ssl) {
        cerr << "WolfSslProvider: wolfSSL_new failed" << endl;
        delete wolf_session;
        return nullptr;
    }

    // Set I/O context
    wolfSSL_SetIOReadCtx(wolf_session->ssl, wolf_session);
    wolfSSL_SetIOWriteCtx(wolf_session->ssl, wolf_session);

    // Enable CID if configured
    if (wolf_ctx->cid_enabled) {
#ifdef WOLFSSL_DTLS_CID
        if (wolfSSL_dtls_cid_use(wolf_session->ssl) != SSL_SUCCESS) {
            cerr << "WolfSslProvider: wolfSSL_dtls_cid_use failed" << endl;
        }
#endif
    }

    // Store session
    wolf_ctx->sessions[session_key] = wolf_session;

    return wolf_session;
}

void WolfSslProvider::destroySession(void* dtls_session) {
    if (!dtls_session) {
        return;
    }

    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(dtls_session);

    // Remove from parent context's session map
    if (wolf_session->parent_ctx) {
        string session_key = sessionToString(&wolf_session->peer_session);
        wolf_session->parent_ctx->sessions.erase(session_key);
    }

    delete wolf_session;
}

// Security configuration
int WolfSslProvider::setPSK(void* ctx, const char* identity,
                            const uint8_t* key, size_t key_len) {
    if (!ctx || !identity || !key) {
        return -1;
    }

    WolfSslContext* wolf_ctx = static_cast<WolfSslContext*>(ctx);
    wolf_ctx->psk_identity = identity;
    wolf_ctx->psk_key.assign(key, key + key_len);

    // Set PSK client callback
    wolfSSL_CTX_set_psk_client_callback(wolf_ctx->ctx, staticPskClientCallback);

    // Store context pointer for callback
    wolfSSL_CTX_SetIORecv(wolf_ctx->ctx, staticRecvCallback);

    return 0;
}

int WolfSslProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
                            const uint8_t* pub_key, size_t pub_len) {
    if (!ctx || !priv_key || !pub_key) {
        return -1;
    }

    // RPK support in wolfSSL
    cerr << "WolfSslProvider: RPK mode not yet implemented" << endl;
    return -1;
}

int WolfSslProvider::setCertificate(void* ctx, const char* cert_file,
                                    const char* key_file, const char* ca_file) {
    if (!ctx || !cert_file || !key_file) {
        return -1;
    }

    WolfSslContext* wolf_ctx = static_cast<WolfSslContext*>(ctx);

    // Load certificate
    if (wolfSSL_CTX_use_certificate_file(wolf_ctx->ctx, cert_file,
                                          SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        cerr << "WolfSslProvider: wolfSSL_CTX_use_certificate_file failed" << endl;
        return -1;
    }

    // Load private key
    if (wolfSSL_CTX_use_PrivateKey_file(wolf_ctx->ctx, key_file,
                                         SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        cerr << "WolfSslProvider: wolfSSL_CTX_use_PrivateKey_file failed" << endl;
        return -1;
    }

    // Load CA certificate if provided
    if (ca_file) {
        if (wolfSSL_CTX_load_verify_locations(wolf_ctx->ctx, ca_file, nullptr)
            != SSL_SUCCESS) {
            cerr << "WolfSslProvider: wolfSSL_CTX_load_verify_locations failed" << endl;
            return -1;
        }
    }

    return 0;
}

// I/O operations
int WolfSslProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
    if (!dtls_session || !data) {
        return -1;
    }

    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(dtls_session);

    int ret = wolfSSL_write(wolf_session->ssl, data, len);
    if (ret <= 0) {
        int error = wolfSSL_get_error(wolf_session->ssl, ret);
        if (error != SSL_ERROR_WANT_WRITE && error != SSL_ERROR_WANT_READ) {
            cerr << "WolfSslProvider: wolfSSL_write failed: " << error << endl;
            char error_str[80];
            wolfSSL_ERR_error_string(error, error_str);
            cerr << "Error: " << error_str << endl;
        }
        return wolfsslToGenericError(error);
    }

    return ret;
}

int WolfSslProvider::recv(void* dtls_session, uint8_t* data, size_t max_len) {
    if (!dtls_session || !data) {
        return -1;
    }

    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(dtls_session);

    int ret = wolfSSL_read(wolf_session->ssl, data, max_len);
    if (ret <= 0) {
        int error = wolfSSL_get_error(wolf_session->ssl, ret);
        if (error != SSL_ERROR_WANT_READ && error != SSL_ERROR_WANT_WRITE) {
            cerr << "WolfSslProvider: wolfSSL_read failed: " << error << endl;
            char error_str[80];
            wolfSSL_ERR_error_string(error, error_str);
            cerr << "Error: " << error_str << endl;
        }
        return wolfsslToGenericError(error);
    }

    return ret;
}

int WolfSslProvider::handleMessage(void* ctx, const session_t* session,
                                   const uint8_t* data, size_t len) {
    if (!ctx || !session || !data) {
        return -1;
    }

    WolfSslContext* wolf_ctx = static_cast<WolfSslContext*>(ctx);

    // Find or create session
    string session_key = sessionToString(session);
    auto it = wolf_ctx->sessions.find(session_key);

    WolfSslSession* wolf_session;
    if (it == wolf_ctx->sessions.end()) {
        // Create new session
        wolf_session = static_cast<WolfSslSession*>(createSession(ctx, session));
        if (!wolf_session) {
            return -1;
        }
    } else {
        wolf_session = static_cast<WolfSslSession*>(it->second);
    }

    // Store received data in session buffer
    wolf_session->recv_buffer.insert(wolf_session->recv_buffer.end(), data, data + len);

    // If handshake not complete, drive handshake
    if (!wolf_session->handshake_complete) {
        int ret = wolfSSL_connect(wolf_session->ssl);
        if (ret == SSL_SUCCESS) {
            wolf_session->handshake_complete = true;

            // Notify application via event callback
            if (wolf_ctx->event_callback) {
                wolf_ctx->event_callback(session, 0, wolf_ctx->user_data);
            }
        } else {
            int error = wolfSSL_get_error(wolf_session->ssl, ret);
            if (error != SSL_ERROR_WANT_READ && error != SSL_ERROR_WANT_WRITE) {
                cerr << "WolfSslProvider: wolfSSL_connect failed: " << error << endl;
                char error_str[80];
                wolfSSL_ERR_error_string(error, error_str);
                cerr << "Error: " << error_str << endl;
                return -1;
            }
        }
    }

    return 0;
}

// Handshake
int WolfSslProvider::connect(void* dtls_session) {
    if (!dtls_session) {
        return -1;
    }

    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(dtls_session);

    int ret = wolfSSL_connect(wolf_session->ssl);
    if (ret == SSL_SUCCESS) {
        wolf_session->handshake_complete = true;
        return 0;
    }

    int error = wolfSSL_get_error(wolf_session->ssl, ret);
    if (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE) {
        // Will complete asynchronously
        return 0;
    }

    cerr << "WolfSslProvider: wolfSSL_connect failed: " << error << endl;
    char error_str[80];
    wolfSSL_ERR_error_string(error, error_str);
    cerr << "Error: " << error_str << endl;
    return -1;
}

int WolfSslProvider::accept(void* dtls_session) {
    if (!dtls_session) {
        return -1;
    }

    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(dtls_session);

    int ret = wolfSSL_accept(wolf_session->ssl);
    if (ret == SSL_SUCCESS) {
        wolf_session->handshake_complete = true;
        return 0;
    }

    int error = wolfSSL_get_error(wolf_session->ssl, ret);
    if (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE) {
        // Will complete asynchronously
        return 0;
    }

    cerr << "WolfSslProvider: wolfSSL_accept failed: " << error << endl;
    return -1;
}

bool WolfSslProvider::isHandshakeComplete(void* dtls_session) const {
    if (!dtls_session) {
        return false;
    }

    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(dtls_session);
    return wolf_session->handshake_complete;
}

// CID support (RFC 9146)
int WolfSslProvider::enableCID(void* dtls_session, uint8_t cid_len) {
#ifdef WOLFSSL_DTLS_CID
    if (!dtls_session) {
        return -1;
    }

    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(dtls_session);
    wolf_session->parent_ctx->cid_enabled = true;
    wolf_session->parent_ctx->cid_length = cid_len;

    if (wolfSSL_dtls_cid_use(wolf_session->ssl) != SSL_SUCCESS) {
        cerr << "WolfSslProvider: wolfSSL_dtls_cid_use failed" << endl;
        return -1;
    }

    return 0;
#else
    return -1; // CID not supported
#endif
}

bool WolfSslProvider::isCIDNegotiated(void* dtls_session) const {
#ifdef WOLFSSL_DTLS_CID
    if (!dtls_session) {
        return false;
    }

    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(dtls_session);
    return wolfSSL_dtls_cid_is_enabled(wolf_session->ssl) == 1;
#else
    return false;
#endif
}

int WolfSslProvider::getCIDVersion(void* dtls_session) const {
#ifdef WOLFSSL_DTLS_CID
    return 54; // RFC 9146
#else
    return -1;
#endif
}

void* WolfSslProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
    // wolfSSL doesn't expose CID-based peer lookup directly
    return nullptr;
}

// Callbacks
void WolfSslProvider::setSendCallback(void* ctx, SendCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    WolfSslContext* wolf_ctx = static_cast<WolfSslContext*>(ctx);
    wolf_ctx->send_callback = callback;
    wolf_ctx->user_data = user_data;
}

void WolfSslProvider::setRecvCallback(void* ctx, RecvCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    WolfSslContext* wolf_ctx = static_cast<WolfSslContext*>(ctx);
    wolf_ctx->recv_callback = callback;
    wolf_ctx->user_data = user_data;
}

void WolfSslProvider::setEventCallback(void* ctx, EventCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    WolfSslContext* wolf_ctx = static_cast<WolfSslContext*>(ctx);
    wolf_ctx->event_callback = callback;
    wolf_ctx->user_data = user_data;
}

// Static callback handlers
unsigned int WolfSslProvider::staticPskClientCallback(WOLFSSL* ssl, const char* hint,
                                                      char* identity, unsigned int id_max_len,
                                                      unsigned char* key, unsigned int key_max_len) {
    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(wolfSSL_GetIOReadCtx(ssl));
    if (!wolf_session || !wolf_session->parent_ctx) {
        return 0;
    }

    WolfSslContext* wolf_ctx = wolf_session->parent_ctx;

    // Copy identity
    if (wolf_ctx->psk_identity.size() >= id_max_len) {
        cerr << "WolfSslProvider: PSK identity too long" << endl;
        return 0;
    }
    strcpy(identity, wolf_ctx->psk_identity.c_str());

    // Copy PSK
    if (wolf_ctx->psk_key.size() > key_max_len) {
        cerr << "WolfSslProvider: PSK key too long" << endl;
        return 0;
    }
    memcpy(key, wolf_ctx->psk_key.data(), wolf_ctx->psk_key.size());

    return wolf_ctx->psk_key.size();
}

unsigned int WolfSslProvider::staticPskServerCallback(WOLFSSL* ssl, const char* identity,
                                                      unsigned char* key, unsigned int key_max_len) {
    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(wolfSSL_GetIOReadCtx(ssl));
    if (!wolf_session || !wolf_session->parent_ctx) {
        return 0;
    }

    WolfSslContext* wolf_ctx = wolf_session->parent_ctx;

    // Verify identity matches
    if (wolf_ctx->psk_identity != identity) {
        cerr << "WolfSslProvider: PSK identity mismatch" << endl;
        return 0;
    }

    // Copy PSK
    if (wolf_ctx->psk_key.size() > key_max_len) {
        cerr << "WolfSslProvider: PSK key too long" << endl;
        return 0;
    }
    memcpy(key, wolf_ctx->psk_key.data(), wolf_ctx->psk_key.size());

    return wolf_ctx->psk_key.size();
}

// I/O callbacks
int WolfSslProvider::staticRecvCallback(WOLFSSL* ssl, char* buf, int sz, void* ctx) {
    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(ctx);
    if (!wolf_session) {
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    // Read from buffered data
    if (wolf_session->recv_buffer.empty()) {
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }

    size_t to_copy = min((size_t)sz, wolf_session->recv_buffer.size() - wolf_session->recv_offset);
    memcpy(buf, wolf_session->recv_buffer.data() + wolf_session->recv_offset, to_copy);
    wolf_session->recv_offset += to_copy;

    // Clear buffer if all data consumed
    if (wolf_session->recv_offset >= wolf_session->recv_buffer.size()) {
        wolf_session->recv_buffer.clear();
        wolf_session->recv_offset = 0;
    }

    return to_copy;
}

int WolfSslProvider::staticSendCallback(WOLFSSL* ssl, char* buf, int sz, void* ctx) {
    WolfSslSession* wolf_session = static_cast<WolfSslSession*>(ctx);
    if (!wolf_session || !wolf_session->parent_ctx) {
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    if (!wolf_session->parent_ctx->send_callback) {
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    int ret = wolf_session->parent_ctx->send_callback(&wolf_session->peer_session,
                                                       (const uint8_t*)buf, sz,
                                                       wolf_session->parent_ctx->user_data);
    if (ret < 0) {
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    return ret;
}

// Helper functions
string WolfSslProvider::sessionToString(const session_t* session) {
    if (!session) {
        return "";
    }

    ostringstream oss;

    if (session->addr.sa.sa_family == AF_INET) {
        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
        oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
    } else if (session->addr.sa.sa_family == AF_INET6) {
        char addr_str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
        oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
    }

    return oss.str();
}

int WolfSslProvider::wolfsslToGenericError(int wolfssl_error) {
    switch (wolfssl_error) {
        case SSL_ERROR_NONE:
            return 0;

        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
            return 0; // Not an error, just needs more I/O

        case SSL_ERROR_ZERO_RETURN:
            return -2; // Connection closed

        default:
            return -1; // Generic error
    }
}
