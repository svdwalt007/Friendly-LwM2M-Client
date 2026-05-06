/*******************************************************************************
 * Connection.cpp - DTLS Multi-Provider Connection Implementation
 *
 * Provides network connection management with pluggable DTLS provider support.
 * Supports TinyDTLS, mbedTLS, OpenSSL, and wolfSSL through abstraction layer.
 *
 * Author: Sean van der Walt
 * Editor: Sean van der Walt
 * Copyright (c) 2024-2026 Walt Technologies
 * Version: 1.2.2
 ******************************************************************************/

#include "Connection_new.h"

#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <cstring>
#include <ctime>

using namespace std;

/* --------------- TinyDTLS Legacy Callbacks (Conditional) --------------- */

#ifdef WITH_TINYDTLS_PROVIDER
// These callbacks are only needed when TinyDTLS provider is enabled

#if DTLS_WITH_PSK
int get_psk_info(dtls_context_t *ctx, const session_t *session,
                 dtls_credentials_type_t type, const unsigned char *id,
                 size_t id_len, unsigned char *result, size_t result_length) {
    Connection *appContext = (Connection *)ctx->app;
    Connection::dtls_connection_t* conn = appContext->connFind(&(session->addr.st), session->size);

    if (!conn) {
        cout << "Connection: GET PSK session not found" << endl;
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    switch (type) {
        case DTLS_PSK_IDENTITY: {
            const OPAQUE_T &key = appContext->getPublicKey(conn);
            if (result_length < key.size()) {
                cout << "Connection: cannot set psk_identity -- buffer too small" << endl;
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }
            memcpy(result, key.data(), key.size());
            return key.size();
        }
        case DTLS_PSK_KEY: {
            const OPAQUE_T &key = appContext->getSecretKey(conn);
            if (result_length < key.size()) {
                cout << "Connection: cannot set psk -- buffer too small" << endl;
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }
            memcpy(result, key.data(), key.size());
            return key.size();
        }
        case DTLS_PSK_HINT:
            return 0;
        default:
            cout << "Connection: unsupported request type: " << type << endl;
    }
    return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
}
#elif DTLS_WITH_RPK
int get_ecdsa_key(struct dtls_context_t *ctx, const session_t *session,
                  const dtls_ecdsa_key_t **result) {
    Connection *appContext = (Connection *)ctx->app;
    Connection::dtls_connection_t* conn = appContext->connFind(&(session->addr.st), session->size);

    if (!conn) {
        cout << "Connection: GET RPK session not found" << endl;
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    *result = &appContext->getEcdsaKey(conn);
    return 0;
}

int verify_ecdsa_key(struct dtls_context_t *ctx, const session_t *session,
                     const unsigned char *other_pub_x, const unsigned char *other_pub_y,
                     size_t key_size) {
    (void)ctx; (void)session; (void)other_pub_x; (void)other_pub_y; (void)key_size;
    return 0;  // Accept all keys for now
}
#endif

int send_to_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len) {
    Connection *appContext = (Connection *)ctx->app;
    Connection::dtls_connection_t* conn = appContext->connFind(&(session->addr.st), session->size);

    if (conn) {
        int res = appContext->sendData({conn, len, data});
        return (res < 0) ? -1 : res;
    }
    return -1;
}

int read_from_peer(dtls_context_t *ctx, session_t *session, uint8 *data, size_t len) {
    Connection *appContext = (Connection *)ctx->app;
    Connection::dtls_connection_t* conn = appContext->connFind(&(session->addr.st), session->size);

    if (conn) {
        appContext->addPacketToQueue({conn, (size_t)len, data});
        return 0;
    }
    return -1;
}

static dtls_handler_t _dtlsClb = {
    send_to_peer,
    read_from_peer,
    NULL,  // event
#if DTLS_WITH_PSK
    get_psk_info,
    NULL,
    NULL
#elif DTLS_WITH_RPK
    NULL,
    get_ecdsa_key,
    verify_ecdsa_key
#endif
};
#endif // WITH_TINYDTLS_PROVIDER

/* --------------- Provider Callbacks --------------- */

int Connection::providerSendCallback(const session_t* session, const uint8_t* data,
                                     size_t len, void* user_data) {
    Connection* conn = static_cast<Connection*>(user_data);
    (void)conn;  // Currently unused - reserved for future implementation
    (void)session;
    (void)data;
    // Find the connection by session
    // This is a simplified implementation - actual code would need session mapping
    return len;  // Return bytes sent
}

int Connection::providerRecvCallback(const session_t* session, const uint8_t* data,
                                     size_t len, void* user_data) {
    Connection* conn = static_cast<Connection*>(user_data);
    (void)conn;  // Currently unused - reserved for future implementation
    (void)session;
    (void)data;
    (void)len;
    // Add packet to queue for processing
    return 0;  // Success
}

void Connection::providerEventCallback(const session_t* session, int event, void* user_data) {
    Connection* conn = static_cast<Connection*>(user_data);
    (void)conn;  // Currently unused - reserved for future implementation
    (void)session;
    // Handle DTLS events (handshake complete, alert, etc.)
    cout << "Connection: DTLS event " << event << endl;
}

/* --------------- Connection Implementation --------------- */

Connection::Connection(string port, int addressFamily)
    : Connection(port, addressFamily, "auto") {
}

Connection::Connection(string port, int addressFamily, const string& providerName)
    : _port(port)
    , _addressFamily(addressFamily)
    , _connFd(-1)
    , _provider(nullptr)
    , _providerContext(nullptr)
#ifdef WITH_TINYDTLS_PROVIDER
    , _dtlsContext(nullptr)
#endif
    , _connections(nullptr) {

    // Open UDP socket
    openSocket();
    cout << "Connection: socket id " << _connFd << endl;

    // Create DTLS provider
    if (providerName == "auto") {
        _provider = DtlsProviderFactory::createDefault();
    } else {
        _provider = DtlsProviderFactory::create(providerName);
    }

    if (!_provider) {
        cout << "Connection: Failed to create DTLS provider '" << providerName << "'" << endl;
        return;
    }

    cout << "Connection: Using DTLS provider " << _provider->getName()
         << " " << _provider->getVersion() << endl;

    // Initialize provider
    if (_provider->init() != 0) {
        cout << "Connection: Failed to initialize DTLS provider" << endl;
        delete _provider;
        _provider = nullptr;
        return;
    }

    // Create provider context
    _providerContext = _provider->createContext();
    if (!_providerContext) {
        cout << "Connection: Failed to create DTLS context" << endl;
        _provider->cleanup();
        delete _provider;
        _provider = nullptr;
        return;
    }

    // Set provider callbacks
    _provider->setSendCallback(_providerContext, providerSendCallback, this);
    _provider->setRecvCallback(_providerContext, providerRecvCallback, this);
    _provider->setEventCallback(_providerContext, providerEventCallback, this);

#ifdef WITH_TINYDTLS_PROVIDER
    // Initialize legacy TinyDTLS if that's the active provider
    if (string(_provider->getName()) == "TinyDTLS") {
        dtls_init();
        _dtlsContext = dtls_new_context(this);
        if (_dtlsContext) {
            dtls_set_handler(_dtlsContext, &_dtlsClb);
        }
    }
#endif
}

Connection::~Connection() {
    // Disconnect all active connections
    while (_connections) {
        disconnect(_connections);
    }

    // Cleanup provider
    if (_provider) {
        if (_providerContext) {
            _provider->destroyContext(_providerContext);
        }
        _provider->cleanup();
        delete _provider;
    }

#ifdef WITH_TINYDTLS_PROVIDER
    if (_dtlsContext) {
        dtls_free_context(_dtlsContext);
    }
#endif

    if (_connFd >= 0) {
        close(_connFd);
    }
}

Connection::SESSION_T Connection::connect(Lwm2mSecurity& security) {
    addrinfo hints, *servinfo = NULL, *p;
    int s;
    sockaddr *sa;
    socklen_t sl;
    dtls_connection_t * conn = NULL;

    STRING_T uri = security.get<STRING_T>(Lwm2mSecurity::LWM2M_SERVER_URI_0);
    string host = uriToHost(uri);
    string port = uriToPort(uri);

    cout << "Connection: connect to " << host << ":" << port << endl;

    if (!host.length() || !port.length()) return NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = _addressFamily;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo) || !servinfo) {
        return NULL;
    }

    // Test various addresses
    for (s = -1, p = servinfo; p != NULL && s == -1; p = p->ai_next) {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0) {
            sa = p->ai_addr;
            sl = p->ai_addrlen;
            if (-1 == ::connect(s, p->ai_addr, p->ai_addrlen)) {
                close(s);
                s = -1;
            }
        }
    }

    if (s >= 0) {
        conn = createNewConn(sa, sl);
        close(s);

        if (conn == NULL) {
            if (servinfo) freeaddrinfo(servinfo);
            return NULL;
        }

        // Setup security keys
        if (!setupSecurityKeys(security, conn)) {
            disconnect(conn);
            if (servinfo) freeaddrinfo(servinfo);
            return NULL;
        }

        INT_T mode = security.get<INT_T>(Lwm2mSecurity::SECURITY_MODE_2);

        if (mode != LWM2M_SECURITY_MODE_NONE) {
            // Create provider session
            session_t session;
            session.size = conn->addrLen;
            memcpy(&session.addr, &conn->addr, conn->addrLen);

            conn->provider_session = _provider->createSession(_providerContext, &session);

            if (!conn->provider_session) {
                cout << "Connection: Failed to create provider session" << endl;
                disconnect(conn);
                if (servinfo) freeaddrinfo(servinfo);
                return NULL;
            }

            // Initiate DTLS handshake
            if (_provider->connect(conn->provider_session) != 0) {
                cout << "Connection: Failed to initiate DTLS handshake" << endl;
                disconnect(conn);
                if (servinfo) freeaddrinfo(servinfo);
                return NULL;
            }

#ifdef WITH_TINYDTLS_PROVIDER
            // Legacy TinyDTLS path
            if (string(_provider->getName()) == "TinyDTLS" && _dtlsContext) {
                conn->dtlsContext = _dtlsContext;
            }
#endif
        }
    }

    if (servinfo) freeaddrinfo(servinfo);
    return conn;
}

void Connection::disconnect(SESSION_T session) {
    dtls_connection_t* conn = (dtls_connection_t*)session;

    if (!conn) return;

    // Remove from linked list
    if (conn == _connections) {
        _connections = conn->next;
    } else {
        dtls_connection_t* parent = _connections;
        while (parent && parent->next != conn) {
            parent = parent->next;
        }
        if (parent) {
            parent->next = conn->next;
        }
    }

    // Destroy provider session
    if (conn->provider_session && _provider) {
        _provider->destroySession(conn->provider_session);
    }

#ifdef WITH_TINYDTLS_PROVIDER
    if (conn->dtlsSession) {
        delete conn->dtlsSession;
    }
#if DTLS_WITH_RPK
    if (conn->ecdsa_key.priv_key) delete conn->ecdsa_key.priv_key;
    if (conn->ecdsa_key.pub_key_x) delete conn->ecdsa_key.pub_key_x;
    if (conn->ecdsa_key.pub_key_y) delete conn->ecdsa_key.pub_key_y;
#endif
#endif

    delete conn;
}

bool Connection::sessionCmp(SESSION_T session1, SESSION_T session2) {
    return (session1 == session2);
}

bool Connection::sendPacket(const Packet &packet) {
    dtls_connection_t* conn = (dtls_connection_t*)packet.session;

    if (!conn) {
        cout << "Connection: failed sending " << packet.length << " bytes, missing connection" << endl;
        return false;
    }

    // Check NAT timeout
    if (DTLS_NAT_TIMEOUT > 0 && (time(NULL) - conn->lastSend) > DTLS_NAT_TIMEOUT) {
        cout << "Connection: NAT timeout, rehandshaking" << endl;
        if (!rehandshake(conn, false)) {
            cout << "Connection: rehandshake failed" << endl;
            return false;
        }
    }

    if (conn->provider_session == NULL) {
        // No security - send directly
        return (sendData(packet) >= 0);
    } else {
        // Send through DTLS
        int res = _provider->send(conn->provider_session, packet.buffer, packet.length);
        if (res < 0) {
            cout << "Connection: DTLS send failed" << endl;
            return false;
        }
        conn->lastSend = time(NULL);
        return true;
    }
}

void Connection::loop() {
    fd_set readfds;
    timeval tv;
    int result;

    FD_ZERO(&readfds);
    FD_SET(_connFd, &readfds);

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

    if (result < 0) {
        if (errno != EINTR) {
            cout << "Connection: select error: " << errno << endl;
        }
    } else if (result > 0 && FD_ISSET(_connFd, &readfds)) {
        uint8_t buffer[1024];
        sockaddr_storage addr;
        socklen_t addrLen = sizeof(addr);

        int numBytes = recvfrom(_connFd, buffer, sizeof(buffer), 0,
                                (sockaddr*)&addr, &addrLen);

        if (numBytes < 0) {
            cout << "Connection: recvfrom error" << endl;
            return;
        }

        // Find connection
        dtls_connection_t* conn = connFind(&addr, addrLen);

        if (conn && conn->provider_session && _provider) {
            // Handle DTLS message
            session_t session;
            session.size = addrLen;
            memcpy(&session.addr, &addr, addrLen);

            _provider->handleMessage(_providerContext, &session, buffer, numBytes);
        } else if (conn) {
            // No DTLS - add directly to queue
            addPacketToQueue({conn, (size_t)numBytes, buffer});
        }
    }
}

const char* Connection::getProviderName() const {
    return _provider ? _provider->getName() : "None";
}

const char* Connection::getProviderVersion() const {
    return _provider ? _provider->getVersion() : "N/A";
}

bool Connection::providerSupportsCID() const {
    return _provider ? _provider->supportsCID() : false;
}

bool Connection::isCidNegotiated(SESSION_T session) {
    dtls_connection_t* conn = (dtls_connection_t*)session;
    if (!conn || !conn->provider_session || !_provider) {
        return false;
    }
    return _provider->isCIDNegotiated(conn->provider_session);
}

int Connection::getCidVersion(SESSION_T session) {
    dtls_connection_t* conn = (dtls_connection_t*)session;
    if (!conn || !conn->provider_session || !_provider) {
        return -1;
    }
    return _provider->getCIDVersion(conn->provider_session);
}

/* --------------- Private Methods --------------- */

bool Connection::openSocket() {
    _connFd = socket(_addressFamily, SOCK_DGRAM, 0);
    if (_connFd < 0) {
        cout << "Connection: Failed to open socket" << endl;
        return false;
    }

    // Bind to port
    if (_addressFamily == AF_INET6) {
        sockaddr_in6 sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin6_family = AF_INET6;
        sa.sin6_port = htons(atoi(_port.c_str()));
        sa.sin6_addr = in6addr_any;

        if (bind(_connFd, (sockaddr*)&sa, sizeof(sa)) < 0) {
            cout << "Connection: Failed to bind socket" << endl;
            close(_connFd);
            _connFd = -1;
            return false;
        }
    } else {
        sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_port = htons(atoi(_port.c_str()));
        sa.sin_addr.s_addr = INADDR_ANY;

        if (bind(_connFd, (sockaddr*)&sa, sizeof(sa)) < 0) {
            cout << "Connection: Failed to bind socket" << endl;
            close(_connFd);
            _connFd = -1;
            return false;
        }
    }

    return true;
}

Connection::dtls_connection_t* Connection::createNewConn(sockaddr* addr, size_t addrLen) {
    dtls_connection_t* conn = new dtls_connection_t;
    memset(conn, 0, sizeof(dtls_connection_t));

    conn->sock = _connFd;
    conn->addrLen = addrLen;
    memcpy(&conn->addr, addr, addrLen);
    conn->next = _connections;
    conn->lastSend = time(NULL);

    _connections = conn;
    return conn;
}

int Connection::sendData(const Packet& packet) {
    dtls_connection_t* conn = (dtls_connection_t*)packet.session;

    if (!conn) return -1;

    int res = sendto(_connFd, packet.buffer, packet.length, 0,
                     (sockaddr*)&conn->addr, conn->addrLen);

    if (res < 0) {
        cout << "Connection: sendto failed" << endl;
        return -1;
    }

    conn->lastSend = time(NULL);
    return res;
}

bool Connection::rehandshake(dtls_connection_t* conn, bool sendCloseNotify) {
    if (!conn || !conn->provider_session || !_provider) {
        return false;
    }

    // Destroy old session
    _provider->destroySession(conn->provider_session);

    // Create new session
    session_t session;
    session.size = conn->addrLen;
    memcpy(&session.addr, &conn->addr, conn->addrLen);

    conn->provider_session = _provider->createSession(_providerContext, &session);
    if (!conn->provider_session) {
        return false;
    }

    // Initiate new handshake
    return (_provider->connect(conn->provider_session) == 0);
}

Connection::dtls_connection_t* Connection::connFind(const sockaddr_storage* addr, size_t addrLen) {
    dtls_connection_t* conn = _connections;

    while (conn) {
        if (sockAddrCmp((sockaddr*)&conn->addr, (sockaddr*)addr)) {
            return conn;
        }
        conn = conn->next;
    }

    return NULL;
}

bool Connection::sockAddrCmp(sockaddr* x, sockaddr* y) {
    if (!x || !y || x->sa_family != y->sa_family) {
        return false;
    }

    if (x->sa_family == AF_INET) {
        sockaddr_in* x4 = (sockaddr_in*)x;
        sockaddr_in* y4 = (sockaddr_in*)y;
        return (x4->sin_port == y4->sin_port) &&
               (x4->sin_addr.s_addr == y4->sin_addr.s_addr);
    } else if (x->sa_family == AF_INET6) {
        sockaddr_in6* x6 = (sockaddr_in6*)x;
        sockaddr_in6* y6 = (sockaddr_in6*)y;
        return (x6->sin6_port == y6->sin6_port) &&
               (memcmp(&x6->sin6_addr, &y6->sin6_addr, sizeof(in6_addr)) == 0);
    }

    return false;
}

int Connection::getPort(sockaddr* x) {
    if (x->sa_family == AF_INET) {
        return ntohs(((sockaddr_in*)x)->sin_port);
    } else if (x->sa_family == AF_INET6) {
        return ntohs(((sockaddr_in6*)x)->sin6_port);
    }
    return -1;
}

bool Connection::setupSecurityKeys(Lwm2mSecurity& security, dtls_connection_t* conn) {
    if (!conn || !_provider || !_providerContext) {
        return false;
    }

    INT_T mode = security.get<INT_T>(Lwm2mSecurity::SECURITY_MODE_2);

    if (mode == LWM2M_SECURITY_MODE_NONE) {
        return true;  // No security needed
    }

    if (mode == LWM2M_SECURITY_MODE_PRE_SHARED_KEY) {
        // PSK mode
        OPAQUE_T pubKey = security.get<OPAQUE_T>(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3);
        OPAQUE_T privKey = security.get<OPAQUE_T>(Lwm2mSecurity::SECRET_KEY_5);

        string identity(pubKey.begin(), pubKey.end());

        int res = _provider->setPSK(_providerContext, identity.c_str(),
                                    privKey.data(), privKey.size());

        if (res != 0) {
            cout << "Connection: Failed to set PSK" << endl;
            return false;
        }

#ifdef WITH_TINYDTLS_PROVIDER
        // Store keys for legacy TinyDTLS callbacks
        if (string(_provider->getName()) == "TinyDTLS") {
            conn->pubKey = pubKey;
            conn->privKey = privKey;
        }
#endif

        return true;
    }

    if (mode == LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY) {
        // RPK mode
        OPAQUE_T pubKey = security.get<OPAQUE_T>(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3);
        OPAQUE_T privKey = security.get<OPAQUE_T>(Lwm2mSecurity::SECRET_KEY_5);

        int res = _provider->setRPK(_providerContext, privKey.data(), privKey.size(),
                                    pubKey.data(), pubKey.size());

        if (res != 0) {
            cout << "Connection: Failed to set RPK" << endl;
            return false;
        }

        return true;
    }

    if (mode == LWM2M_SECURITY_MODE_CERTIFICATE) {
        // Certificate mode - not supported by all providers
        cout << "Connection: Certificate mode not yet fully implemented" << endl;
        return false;
    }

    cout << "Connection: Unsupported security mode: " << mode << endl;
    return false;
}

string Connection::uriToPort(string uri) {
    size_t colonPos = uri.find_last_of(':');
    if (colonPos != string::npos) {
        return uri.substr(colonPos + 1);
    }
    return "5683";  // Default CoAP port
}

string Connection::uriToHost(string uri) {
    // Extract host from coap://host:port or coaps://host:port
    size_t start = uri.find("://");
    if (start != string::npos) {
        start += 3;
    } else {
        start = 0;
    }

    size_t end = uri.find_last_of(':');
    if (end != string::npos && end > start) {
        return uri.substr(start, end - start);
    }

    return uri.substr(start);
}

#ifdef WITH_TINYDTLS_PROVIDER
const OPAQUE_T& Connection::getPublicKey(dtls_connection_t* conn) {
    return conn->pubKey;
}

const OPAQUE_T& Connection::getSecretKey(dtls_connection_t* conn) {
    return conn->privKey;
}

#if DTLS_WITH_RPK
const dtls_ecdsa_key_t& Connection::getEcdsaKey(dtls_connection_t* conn) {
    return conn->ecdsa_key;
}
#endif
#endif // WITH_TINYDTLS_PROVIDER
