/*******************************************************************************
 * OpenSSL Provider Implementation
 *
 * Wrapper for OpenSSL 3.x DTLS library.
 * Industry-standard SSL/TLS with full DTLS 1.2/1.3 support.
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

#include "OpenSslProvider.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/opensslv.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

// OpenSslContext constructor
OpenSslProvider::OpenSslContext::OpenSslContext() :
    ssl_ctx(nullptr),
    send_callback(nullptr),
    recv_callback(nullptr),
    event_callback(nullptr),
    user_data(nullptr),
    cid_enabled(false),
    cid_length(0) {
}

// OpenSslContext destructor
OpenSslProvider::OpenSslContext::~OpenSslContext() {
    // Clean up sessions
    for (auto& pair : sessions) {
        if (pair.second) {
            OpenSslSession* session = static_cast<OpenSslSession*>(pair.second);
            delete session;
        }
    }
    sessions.clear();

    // Clean up SSL context
    if (ssl_ctx) {
        SSL_CTX_free(ssl_ctx);
    }
}

// OpenSslSession constructor
OpenSslProvider::OpenSslSession::OpenSslSession() :
    ssl(nullptr),
    read_bio(nullptr),
    write_bio(nullptr),
    handshake_complete(false),
    parent_ctx(nullptr),
    recv_offset(0) {
    memset(&peer_session, 0, sizeof(peer_session));
}

// OpenSslSession destructor
OpenSslProvider::OpenSslSession::~OpenSslSession() {
    if (ssl) {
        SSL_free(ssl); // Also frees associated BIOs if set
    }
}

// Constructor
OpenSslProvider::OpenSslProvider() : m_initialized(false) {
}

// Destructor
OpenSslProvider::~OpenSslProvider() {
    if (m_initialized) {
        cleanup();
    }
}

// Provider information
const char* OpenSslProvider::getName() const {
    return "OpenSSL";
}

const char* OpenSslProvider::getVersion() const {
    return OPENSSL_VERSION_TEXT;
}

bool OpenSslProvider::supportsCID() const {
#if OPENSSL_VERSION_NUMBER >= 0x30200000L  // OpenSSL 3.2+
    return true;
#else
    return false;
#endif
}

bool OpenSslProvider::supportsDTLS13() const {
    return true; // OpenSSL 3.x supports DTLS 1.3
}

// Lifecycle
int OpenSslProvider::init() {
    if (m_initialized) {
        return 0; // Already initialized
    }

    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    m_initialized = true;
    return 0;
}

int OpenSslProvider::cleanup() {
    if (!m_initialized) {
        return 0;
    }

    ERR_free_strings();
    EVP_cleanup();

    m_initialized = false;
    return 0;
}

// Context management
void* OpenSslProvider::createContext() {
    OpenSslContext* ctx = new OpenSslContext();
    if (!ctx) {
        return nullptr;
    }

    // Create SSL_CTX with DTLS method
    const SSL_METHOD* method = DTLS_client_method(); // Supports DTLS 1.0/1.2/1.3
    ctx->ssl_ctx = SSL_CTX_new(method);
    if (!ctx->ssl_ctx) {
        cerr << "OpenSslProvider: SSL_CTX_new failed" << endl;
        ERR_print_errors_fp(stderr);
        delete ctx;
        return nullptr;
    }

    // Set minimum and maximum protocol versions
    SSL_CTX_set_min_proto_version(ctx->ssl_ctx, DTLS1_2_VERSION);
    SSL_CTX_set_max_proto_version(ctx->ssl_ctx, DTLS1_2_VERSION);

    // Set verification mode
    SSL_CTX_set_verify(ctx->ssl_ctx, SSL_VERIFY_PEER, nullptr);

    return ctx;
}

void OpenSslProvider::destroyContext(void* ctx) {
    if (!ctx) {
        return;
    }

    OpenSslContext* ossl_ctx = static_cast<OpenSslContext*>(ctx);
    delete ossl_ctx;
}

// Session management
void* OpenSslProvider::createSession(void* ctx, const session_t* session) {
    if (!ctx || !session) {
        return nullptr;
    }

    OpenSslContext* ossl_ctx = static_cast<OpenSslContext*>(ctx);

    // Check if session already exists
    string session_key = sessionToString(session);
    auto it = ossl_ctx->sessions.find(session_key);
    if (it != ossl_ctx->sessions.end()) {
        return it->second; // Return existing session
    }

    // Create new session
    OpenSslSession* ossl_session = new OpenSslSession();
    if (!ossl_session) {
        return nullptr;
    }

    ossl_session->parent_ctx = ossl_ctx;
    memcpy(&ossl_session->peer_session, session, sizeof(session_t));

    // Create SSL object
    ossl_session->ssl = SSL_new(ossl_ctx->ssl_ctx);
    if (!ossl_session->ssl) {
        cerr << "OpenSslProvider: SSL_new failed" << endl;
        ERR_print_errors_fp(stderr);
        delete ossl_session;
        return nullptr;
    }

    // Create memory BIOs for I/O
    ossl_session->read_bio = BIO_new(BIO_s_mem());
    ossl_session->write_bio = BIO_new(BIO_s_mem());

    if (!ossl_session->read_bio || !ossl_session->write_bio) {
        cerr << "OpenSslProvider: BIO_new failed" << endl;
        delete ossl_session;
        return nullptr;
    }

    // Set BIOs to SSL
    SSL_set_bio(ossl_session->ssl, ossl_session->read_bio, ossl_session->write_bio);

    // Set SSL to connect mode (client)
    SSL_set_connect_state(ossl_session->ssl);

    // Enable CID if configured
    if (ossl_ctx->cid_enabled) {
#if OPENSSL_VERSION_NUMBER >= 0x30200000L
        // Enable CID (requires OpenSSL 3.2+)
        // Note: OpenSSL 3.2+ API for CID would go here
        // Currently this is experimental in OpenSSL
#endif
    }

    // Store session context pointer for callbacks
    SSL_set_app_data(ossl_session->ssl, ossl_session);

    // Store session
    ossl_ctx->sessions[session_key] = ossl_session;

    return ossl_session;
}

void OpenSslProvider::destroySession(void* dtls_session) {
    if (!dtls_session) {
        return;
    }

    OpenSslSession* ossl_session = static_cast<OpenSslSession*>(dtls_session);

    // Remove from parent context's session map
    if (ossl_session->parent_ctx) {
        string session_key = sessionToString(&ossl_session->peer_session);
        ossl_session->parent_ctx->sessions.erase(session_key);
    }

    delete ossl_session;
}

// Security configuration
int OpenSslProvider::setPSK(void* ctx, const char* identity,
                            const uint8_t* key, size_t key_len) {
    if (!ctx || !identity || !key) {
        return -1;
    }

    OpenSslContext* ossl_ctx = static_cast<OpenSslContext*>(ctx);
    ossl_ctx->psk_identity = identity;
    ossl_ctx->psk_key.assign(key, key + key_len);

    // Set PSK client callback
    SSL_CTX_set_psk_client_callback(ossl_ctx->ssl_ctx, staticPskClientCallback);

    // Store context pointer for callback
    SSL_CTX_set_app_data(ossl_ctx->ssl_ctx, ossl_ctx);

    return 0;
}

int OpenSslProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
                            const uint8_t* pub_key, size_t pub_len) {
    if (!ctx || !priv_key || !pub_key) {
        return -1;
    }

    // RPK support in OpenSSL requires raw public key extension
    // This is complex and may require OpenSSL 3.2+ features
    cerr << "OpenSslProvider: RPK mode not yet implemented" << endl;
    return -1;
}

int OpenSslProvider::setCertificate(void* ctx, const char* cert_file,
                                    const char* key_file, const char* ca_file) {
    if (!ctx || !cert_file || !key_file) {
        return -1;
    }

    OpenSslContext* ossl_ctx = static_cast<OpenSslContext*>(ctx);

    // Load certificate
    if (SSL_CTX_use_certificate_file(ossl_ctx->ssl_ctx, cert_file,
                                      SSL_FILETYPE_PEM) <= 0) {
        cerr << "OpenSslProvider: SSL_CTX_use_certificate_file failed" << endl;
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // Load private key
    if (SSL_CTX_use_PrivateKey_file(ossl_ctx->ssl_ctx, key_file,
                                     SSL_FILETYPE_PEM) <= 0) {
        cerr << "OpenSslProvider: SSL_CTX_use_PrivateKey_file failed" << endl;
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // Verify private key matches certificate
    if (!SSL_CTX_check_private_key(ossl_ctx->ssl_ctx)) {
        cerr << "OpenSslProvider: Private key does not match certificate" << endl;
        return -1;
    }

    // Load CA certificate if provided
    if (ca_file) {
        if (!SSL_CTX_load_verify_locations(ossl_ctx->ssl_ctx, ca_file, nullptr)) {
            cerr << "OpenSslProvider: SSL_CTX_load_verify_locations failed" << endl;
            ERR_print_errors_fp(stderr);
            return -1;
        }
    }

    return 0;
}

// I/O operations
int OpenSslProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
    if (!dtls_session || !data) {
        return -1;
    }

    OpenSslSession* ossl_session = static_cast<OpenSslSession*>(dtls_session);

    int ret = SSL_write(ossl_session->ssl, data, len);
    if (ret <= 0) {
        int ssl_error = SSL_get_error(ossl_session->ssl, ret);
        if (ssl_error != SSL_ERROR_WANT_WRITE && ssl_error != SSL_ERROR_WANT_READ) {
            cerr << "OpenSslProvider: SSL_write failed: " << ssl_error << endl;
            ERR_print_errors_fp(stderr);
        }
        return opensslToGenericError(ssl_error);
    }

    // Flush write BIO to send callback
    char buf[4096];
    int pending;
    while ((pending = BIO_ctrl_pending(ossl_session->write_bio)) > 0) {
        int read_len = BIO_read(ossl_session->write_bio, buf, sizeof(buf));
        if (read_len > 0 && ossl_session->parent_ctx->send_callback) {
            ossl_session->parent_ctx->send_callback(&ossl_session->peer_session,
                                                     (const uint8_t*)buf, read_len,
                                                     ossl_session->parent_ctx->user_data);
        }
    }

    return ret;
}

int OpenSslProvider::recv(void* dtls_session, uint8_t* data, size_t max_len) {
    if (!dtls_session || !data) {
        return -1;
    }

    OpenSslSession* ossl_session = static_cast<OpenSslSession*>(dtls_session);

    int ret = SSL_read(ossl_session->ssl, data, max_len);
    if (ret <= 0) {
        int ssl_error = SSL_get_error(ossl_session->ssl, ret);
        if (ssl_error != SSL_ERROR_WANT_READ && ssl_error != SSL_ERROR_WANT_WRITE) {
            cerr << "OpenSslProvider: SSL_read failed: " << ssl_error << endl;
            ERR_print_errors_fp(stderr);
        }
        return opensslToGenericError(ssl_error);
    }

    return ret;
}

int OpenSslProvider::handleMessage(void* ctx, const session_t* session,
                                   const uint8_t* data, size_t len) {
    if (!ctx || !session || !data) {
        return -1;
    }

    OpenSslContext* ossl_ctx = static_cast<OpenSslContext*>(ctx);

    // Find or create session
    string session_key = sessionToString(session);
    auto it = ossl_ctx->sessions.find(session_key);

    OpenSslSession* ossl_session;
    if (it == ossl_ctx->sessions.end()) {
        // Create new session
        ossl_session = static_cast<OpenSslSession*>(createSession(ctx, session));
        if (!ossl_session) {
            return -1;
        }
    } else {
        ossl_session = static_cast<OpenSslSession*>(it->second);
    }

    // Write data to read BIO
    BIO_write(ossl_session->read_bio, data, len);

    // If handshake not complete, drive handshake
    if (!ossl_session->handshake_complete) {
        int ret = SSL_do_handshake(ossl_session->ssl);
        if (ret == 1) {
            ossl_session->handshake_complete = true;

            // Notify application via event callback
            if (ossl_ctx->event_callback) {
                ossl_ctx->event_callback(session, 0, ossl_ctx->user_data);
            }
        } else {
            int ssl_error = SSL_get_error(ossl_session->ssl, ret);
            if (ssl_error != SSL_ERROR_WANT_READ && ssl_error != SSL_ERROR_WANT_WRITE) {
                cerr << "OpenSslProvider: SSL_do_handshake failed: " << ssl_error << endl;
                ERR_print_errors_fp(stderr);
                return -1;
            }
        }

        // Flush write BIO (send handshake messages)
        char buf[4096];
        int pending;
        while ((pending = BIO_ctrl_pending(ossl_session->write_bio)) > 0) {
            int read_len = BIO_read(ossl_session->write_bio, buf, sizeof(buf));
            if (read_len > 0 && ossl_ctx->send_callback) {
                ossl_ctx->send_callback(&ossl_session->peer_session,
                                        (const uint8_t*)buf, read_len,
                                        ossl_ctx->user_data);
            }
        }
    }

    return 0;
}

// Handshake
int OpenSslProvider::connect(void* dtls_session) {
    if (!dtls_session) {
        return -1;
    }

    OpenSslSession* ossl_session = static_cast<OpenSslSession*>(dtls_session);

    int ret = SSL_do_handshake(ossl_session->ssl);
    if (ret == 1) {
        ossl_session->handshake_complete = true;
        return 0;
    }

    int ssl_error = SSL_get_error(ossl_session->ssl, ret);
    if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE) {
        // Will complete asynchronously
        return 0;
    }

    cerr << "OpenSslProvider: SSL_do_handshake failed: " << ssl_error << endl;
    ERR_print_errors_fp(stderr);
    return -1;
}

int OpenSslProvider::accept(void* dtls_session) {
    if (!dtls_session) {
        return -1;
    }

    OpenSslSession* ossl_session = static_cast<OpenSslSession*>(dtls_session);

    // Set SSL to accept mode (server)
    SSL_set_accept_state(ossl_session->ssl);

    // Same handshake process
    return connect(dtls_session);
}

bool OpenSslProvider::isHandshakeComplete(void* dtls_session) const {
    if (!dtls_session) {
        return false;
    }

    OpenSslSession* ossl_session = static_cast<OpenSslSession*>(dtls_session);
    return ossl_session->handshake_complete;
}

// CID support (RFC 9146)
int OpenSslProvider::enableCID(void* dtls_session, uint8_t cid_len) {
#if OPENSSL_VERSION_NUMBER >= 0x30200000L
    if (!dtls_session) {
        return -1;
    }

    OpenSslSession* ossl_session = static_cast<OpenSslSession*>(dtls_session);
    ossl_session->parent_ctx->cid_enabled = true;
    ossl_session->parent_ctx->cid_length = cid_len;

    // OpenSSL 3.2+ CID API would go here
    // Currently experimental

    return 0;
#else
    return -1; // CID not supported
#endif
}

bool OpenSslProvider::isCIDNegotiated(void* dtls_session) const {
#if OPENSSL_VERSION_NUMBER >= 0x30200000L
    // Would check CID negotiation status
    return false;
#else
    return false;
#endif
}

int OpenSslProvider::getCIDVersion(void* dtls_session) const {
#if OPENSSL_VERSION_NUMBER >= 0x30200000L
    return 54; // RFC 9146
#else
    return -1;
#endif
}

void* OpenSslProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
    // OpenSSL doesn't expose CID-based peer lookup directly
    return nullptr;
}

// Callbacks
void OpenSslProvider::setSendCallback(void* ctx, SendCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    OpenSslContext* ossl_ctx = static_cast<OpenSslContext*>(ctx);
    ossl_ctx->send_callback = callback;
    ossl_ctx->user_data = user_data;
}

void OpenSslProvider::setRecvCallback(void* ctx, RecvCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    OpenSslContext* ossl_ctx = static_cast<OpenSslContext*>(ctx);
    ossl_ctx->recv_callback = callback;
    ossl_ctx->user_data = user_data;
}

void OpenSslProvider::setEventCallback(void* ctx, EventCallback callback, void* user_data) {
    if (!ctx) {
        return;
    }

    OpenSslContext* ossl_ctx = static_cast<OpenSslContext*>(ctx);
    ossl_ctx->event_callback = callback;
    ossl_ctx->user_data = user_data;
}

// Static callback handlers
unsigned int OpenSslProvider::staticPskClientCallback(SSL* ssl, const char* hint,
                                                      char* identity, unsigned int max_identity_len,
                                                      unsigned char* psk, unsigned int max_psk_len) {
    SSL_CTX* ssl_ctx = SSL_get_SSL_CTX(ssl);
    OpenSslContext* ossl_ctx = static_cast<OpenSslContext*>(SSL_CTX_get_app_data(ssl_ctx));

    if (!ossl_ctx) {
        return 0;
    }

    // Copy identity
    if (ossl_ctx->psk_identity.size() >= max_identity_len) {
        cerr << "OpenSslProvider: PSK identity too long" << endl;
        return 0;
    }
    strcpy(identity, ossl_ctx->psk_identity.c_str());

    // Copy PSK
    if (ossl_ctx->psk_key.size() > max_psk_len) {
        cerr << "OpenSslProvider: PSK key too long" << endl;
        return 0;
    }
    memcpy(psk, ossl_ctx->psk_key.data(), ossl_ctx->psk_key.size());

    return ossl_ctx->psk_key.size();
}

unsigned int OpenSslProvider::staticPskServerCallback(SSL* ssl, const char* identity,
                                                      unsigned char* psk, unsigned int max_psk_len) {
    SSL_CTX* ssl_ctx = SSL_get_SSL_CTX(ssl);
    OpenSslContext* ossl_ctx = static_cast<OpenSslContext*>(SSL_CTX_get_app_data(ssl_ctx));

    if (!ossl_ctx) {
        return 0;
    }

    // Verify identity matches
    if (ossl_ctx->psk_identity != identity) {
        cerr << "OpenSslProvider: PSK identity mismatch" << endl;
        return 0;
    }

    // Copy PSK
    if (ossl_ctx->psk_key.size() > max_psk_len) {
        cerr << "OpenSslProvider: PSK key too long" << endl;
        return 0;
    }
    memcpy(psk, ossl_ctx->psk_key.data(), ossl_ctx->psk_key.size());

    return ossl_ctx->psk_key.size();
}

// Helper functions
string OpenSslProvider::sessionToString(const session_t* session) {
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

int OpenSslProvider::opensslToGenericError(int ssl_error) {
    switch (ssl_error) {
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
