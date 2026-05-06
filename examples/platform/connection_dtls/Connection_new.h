#ifndef CONNECTION_H
#define CONNECTION_H

#include <netinet/in.h>
#include <string>
#include "WppConnection.h"
#include "WppRegistry.h"
#include "DtlsProvider.h"
#include "DtlsProviderFactory.h"

// TinyDTLS includes only if TinyDTLS provider is enabled
#ifdef WITH_TINYDTLS_PROVIDER
extern "C" {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "tinydtls.h"
#include "dtls.h"
#pragma GCC diagnostic pop
#undef min
}
#else
// Define session_t for non-TinyDTLS providers (compatible with TinyDTLS structure)
// DtlsProvider.h forward declares it, we provide the complete definition here
#ifndef _DTLS_SESSION_T_DEFINED
#define _DTLS_SESSION_T_DEFINED
struct session_t {
    socklen_t size;
    union {
        struct sockaddr     sa;
        struct sockaddr_storage st;
        struct sockaddr_in  sin;
        struct sockaddr_in6 sin6;
    } addr;
    int ifindex;
};
#endif
#endif

using namespace std;
using namespace wpp;

// After 40sec of inactivity we rehandshake
#define DTLS_NAT_TIMEOUT 40

class Connection : public WppConnection {
    // TinyDTLS callback friends (only if TinyDTLS provider is enabled)
#ifdef WITH_TINYDTLS_PROVIDER
    friend int get_psk_info(struct dtls_context_t *ctx, const session_t *session, dtls_credentials_type_t type, const unsigned char *id, size_t id_len, unsigned char *result, size_t result_length);
    friend int get_ecdsa_key(struct dtls_context_t *ctx, const session_t *session, const dtls_ecdsa_key_t **result);
    friend int verify_ecdsa_key(struct dtls_context_t *ctx, const session_t *session, const unsigned char *other_pub_x, const unsigned char *other_pub_y, size_t key_size);
    friend int send_to_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len);
    friend int read_from_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len);
#endif

private:
    struct dtls_connection_t {
        dtls_connection_t *next;
        int sock;
        sockaddr_in6 addr;
        size_t addrLen;

        // Provider-agnostic DTLS session
        void* provider_session;  // Opaque provider session pointer

#ifdef WITH_TINYDTLS_PROVIDER
        // Legacy TinyDTLS session (for backward compatibility)
        session_t* dtlsSession;
#if DTLS_WITH_PSK
        OPAQUE_T pubKey;
        OPAQUE_T privKey;
#elif DTLS_WITH_RPK
        dtls_ecdsa_key_t ecdsa_key;
#endif
        dtls_context_t* dtlsContext;
#endif

        time_t lastSend; // Last time data was sent (for NAT timeouts)
    };

public:
    /**
     * Constructor with port and address family.
     * Uses default DTLS provider (auto-detected).
     */
    Connection(string port, int addressFamily);

    /**
     * Constructor with explicit provider selection.
     * @param port Local port to bind to
     * @param addressFamily AF_INET or AF_INET6
     * @param providerName DTLS provider name ("auto", "tinydtls", "mbedtls", "openssl", "wolfssl")
     */
    Connection(string port, int addressFamily, const string& providerName);

    ~Connection();

    // WppConnection interface
    SESSION_T connect(Lwm2mSecurity& security) override;
    void disconnect(SESSION_T session) override;
    bool sessionCmp(SESSION_T session1, SESSION_T session2) override;
    bool sendPacket(const Packet &packet) override;

    void loop();

    /**
     * Get the name of the active DTLS provider.
     * @return Provider name (e.g., "TinyDTLS", "mbedTLS", "OpenSSL", "wolfSSL")
     */
    const char* getProviderName() const;

    /**
     * Get the version of the active DTLS provider.
     * @return Version string
     */
    const char* getProviderVersion() const;

    /**
     * Check if the active provider supports Connection ID (CID).
     * @return true if CID is supported, false otherwise
     */
    bool providerSupportsCID() const;

    /**
     * Check if Connection ID (CID) is negotiated for a session.
     * @param session The session to check
     * @return true if CID is negotiated, false otherwise
     */
    bool isCidNegotiated(SESSION_T session);

    /**
     * Get the CID version being used (54=RFC9146).
     * @param session The session to query
     * @return CID version or -1 if not negotiated
     */
    int getCidVersion(SESSION_T session);

private:
    bool openSocket();
    dtls_connection_t* createNewConn(sockaddr* addr, size_t addrLen);
    int sendData(const Packet& packet);
    bool rehandshake(dtls_connection_t* conn, bool sendCloseNotify);
    dtls_connection_t* connFind(const sockaddr_storage* addr, size_t addrLen);
    bool sockAddrCmp(sockaddr* x, sockaddr* y);
    int getPort(sockaddr* x);

    bool setupSecurityKeys(Lwm2mSecurity& security, dtls_connection_t* conn);

    string uriToPort(string uri);
    string uriToHost(string uri);

    // Provider callbacks
    static int providerSendCallback(const session_t* session, const uint8_t* data,
                                    size_t len, void* user_data);
    static int providerRecvCallback(const session_t* session, const uint8_t* data,
                                    size_t len, void* user_data);
    static void providerEventCallback(const session_t* session, int event, void* user_data);

#ifdef WITH_TINYDTLS_PROVIDER
    // Legacy TinyDTLS-specific methods
    const OPAQUE_T& getPublicKey(dtls_connection_t* conn);
    const OPAQUE_T& getSecretKey(dtls_connection_t* conn);
#if DTLS_WITH_RPK
    const dtls_ecdsa_key_t& getEcdsaKey(dtls_connection_t* conn);
#endif
#endif

private:
    string _port;
    int _addressFamily;
    int _connFd;

    // DTLS Provider abstraction
    DtlsProvider* _provider;
    void* _providerContext;

#ifdef WITH_TINYDTLS_PROVIDER
    // Legacy TinyDTLS context (for backward compatibility)
    dtls_context_t* _dtlsContext;
#endif

    dtls_connection_t* _connections;
};

#endif //CONNECTION_H
