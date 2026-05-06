/*******************************************************************************
 * DTLS Provider Abstraction Interface
 *
 * Abstract interface for DTLS implementations, allowing runtime selection
 * between different DTLS libraries (TinyDTLS, mbedTLS, OpenSSL, wolfSSL).
 *
 * Copyright (c) 2024-2026 Walt Technologies
 ******************************************************************************/

#ifndef DTLS_PROVIDER_H
#define DTLS_PROVIDER_H

#include <cstdint>
#include <cstddef>
#include <string>

// session_t type is used as an opaque pointer in the interface
// We declare both the struct tag and typedef to be compatible with different libraries:
// - TinyDTLS: typedef struct {...} session_t;  (anonymous struct, typedef name)
// - Our declaration: struct session_t; typedef struct session_t session_t;
//
// When TinyDTLS headers are included BEFORE this file (as in TinyDtlsProvider.h),
// their typedef takes precedence and our declarations are compatible/ignored
#ifndef _DTLS_SESSION_H_  // Skip if TinyDTLS session.h already included
struct session_t;
typedef struct session_t session_t;
#endif

/**
 * DTLS Provider interface
 *
 * All DTLS library implementations must implement this interface.
 * Provides unified API for DTLS operations regardless of underlying library.
 */
class DtlsProvider {
public:
    virtual ~DtlsProvider() = default;

    // =========================================================================
    // Provider Information
    // =========================================================================

    /**
     * Get provider name (e.g., "TinyDTLS", "mbedTLS", "OpenSSL")
     */
    virtual const char* getName() const = 0;

    /**
     * Get provider version string
     */
    virtual const char* getVersion() const = 0;

    /**
     * Check if provider supports DTLS Connection ID (RFC 9146)
     */
    virtual bool supportsCID() const = 0;

    /**
     * Check if provider supports DTLS 1.3
     */
    virtual bool supportsDTLS13() const = 0;

    // =========================================================================
    // Lifecycle Management
    // =========================================================================

    /**
     * Initialize DTLS provider
     * @return 0 on success, negative on error
     */
    virtual int init() = 0;

    /**
     * Cleanup DTLS provider resources
     * @return 0 on success, negative on error
     */
    virtual int cleanup() = 0;

    // =========================================================================
    // Context Management
    // =========================================================================

    /**
     * Create DTLS context
     * @return Opaque context pointer, NULL on error
     */
    virtual void* createContext() = 0;

    /**
     * Destroy DTLS context
     * @param ctx Context pointer from createContext()
     */
    virtual void destroyContext(void* ctx) = 0;

    // =========================================================================
    // Session Management
    // =========================================================================

    /**
     * Create DTLS session
     * @param ctx DTLS context
     * @param session Network session information
     * @return Opaque session pointer, NULL on error
     */
    virtual void* createSession(void* ctx, const session_t* session) = 0;

    /**
     * Destroy DTLS session
     * @param dtls_session Session pointer from createSession()
     */
    virtual void destroySession(void* dtls_session) = 0;

    /**
     * Get peer from session (for context lookups)
     * @param ctx DTLS context
     * @param session Network session
     * @return Opaque peer pointer, NULL if not found
     */
    virtual void* getPeer(void* ctx, const session_t* session) = 0;

    // =========================================================================
    // Security Configuration
    // =========================================================================

    /**
     * Configure Pre-Shared Key (PSK) mode
     * @param ctx DTLS context
     * @param identity PSK identity string
     * @param key PSK key bytes
     * @param key_len Length of PSK key
     * @return 0 on success, negative on error
     */
    virtual int setPSK(void* ctx, const char* identity,
                       const uint8_t* key, size_t key_len) = 0;

    /**
     * Configure Raw Public Key (RPK) mode
     * @param ctx DTLS context
     * @param priv_key Private key bytes
     * @param priv_len Length of private key
     * @param pub_key Public key bytes
     * @param pub_len Length of public key
     * @return 0 on success, negative on error
     */
    virtual int setRPK(void* ctx,
                       const uint8_t* priv_key, size_t priv_len,
                       const uint8_t* pub_key, size_t pub_len) = 0;

    /**
     * Configure Certificate mode
     * @param ctx DTLS context
     * @param cert_file Path to certificate file
     * @param key_file Path to private key file
     * @param ca_file Path to CA certificate file (optional)
     * @return 0 on success, negative on error
     */
    virtual int setCertificate(void* ctx, const char* cert_file,
                               const char* key_file, const char* ca_file = nullptr) = 0;

    // =========================================================================
    // DTLS I/O Operations
    // =========================================================================

    /**
     * Send data over DTLS session
     * @param dtls_session Session pointer
     * @param data Data to send
     * @param len Length of data
     * @return Number of bytes sent, negative on error
     */
    virtual int send(void* dtls_session, const uint8_t* data, size_t len) = 0;

    /**
     * Receive data from DTLS session
     * @param dtls_session Session pointer
     * @param data Buffer for received data
     * @param max_len Maximum buffer size
     * @return Number of bytes received, negative on error
     */
    virtual int recv(void* dtls_session, uint8_t* data, size_t max_len) = 0;

    /**
     * Handle incoming DTLS message
     * @param ctx DTLS context
     * @param session Network session
     * @param data Incoming data
     * @param len Length of data
     * @return 0 on success, negative on error
     */
    virtual int handleMessage(void* ctx, const session_t* session,
                              const uint8_t* data, size_t len) = 0;

    // =========================================================================
    // Handshake Operations
    // =========================================================================

    /**
     * Initiate DTLS handshake (client side)
     * @param dtls_session Session pointer
     * @return 0 on success, negative on error
     */
    virtual int connect(void* dtls_session) = 0;

    /**
     * Accept DTLS handshake (server side)
     * @param dtls_session Session pointer
     * @return 0 on success, negative on error
     */
    virtual int accept(void* dtls_session) = 0;

    /**
     * Check if handshake is complete
     * @param dtls_session Session pointer
     * @return true if handshake complete, false otherwise
     */
    virtual bool isHandshakeComplete(void* dtls_session) const = 0;

    // =========================================================================
    // Connection ID (CID) Support - RFC 9146
    // =========================================================================

    /**
     * Enable Connection ID for session
     * @param dtls_session Session pointer
     * @param cid_len Requested CID length (1-8 bytes, 0 to disable)
     * @return 0 on success, negative on error
     */
    virtual int enableCID(void* dtls_session, uint8_t cid_len) = 0;

    /**
     * Check if CID was negotiated
     * @param dtls_session Session pointer
     * @return true if CID negotiated, false otherwise
     */
    virtual bool isCIDNegotiated(void* dtls_session) const = 0;

    /**
     * Get negotiated CID version
     * @param dtls_session Session pointer
     * @return CID version (0=none, 1=draft/Type53, 2=RFC9146/Type54)
     */
    virtual int getCIDVersion(void* dtls_session) const = 0;

    /**
     * Get our (local) CID length
     * @param dtls_session Session pointer
     * @return CID length in bytes, 0 if not negotiated
     */
    virtual int getOurCIDLength(void* dtls_session) const = 0;

    /**
     * Get peer's CID length
     * @param dtls_session Session pointer
     * @return CID length in bytes, 0 if not negotiated
     */
    virtual int getPeerCIDLength(void* dtls_session) const = 0;

    /**
     * Get peer by CID (for NAT rebinding support)
     * @param ctx DTLS context
     * @param cid CID bytes
     * @param cid_len CID length
     * @return Opaque peer pointer, NULL if not found
     */
    virtual void* getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) = 0;

    // =========================================================================
    // Callback Configuration
    // =========================================================================

    /**
     * Callback function type for data ready to send over network
     * @param session Network session
     * @param data Data to send
     * @param len Length of data
     * @param user_data User-provided context
     * @return Number of bytes sent, negative on error
     */
    typedef int (*SendCallback)(const session_t* session, const uint8_t* data,
                                 size_t len, void* user_data);

    /**
     * Callback function type for received application data
     * @param session Network session
     * @param data Decrypted application data
     * @param len Length of data
     * @param user_data User-provided context
     * @return 0 on success, negative on error
     */
    typedef int (*RecvCallback)(const session_t* session, const uint8_t* data,
                                 size_t len, void* user_data);

    /**
     * Callback function type for DTLS events
     * @param session Network session
     * @param event Event type (0=connected, 1=closed, 2=error)
     * @param user_data User-provided context
     */
    typedef void (*EventCallback)(const session_t* session, int event,
                                   void* user_data);

    /**
     * Set network send callback
     * @param ctx DTLS context
     * @param callback Send callback function
     * @param user_data User-provided context pointer
     */
    virtual void setSendCallback(void* ctx, SendCallback callback,
                                  void* user_data) = 0;

    /**
     * Set application data receive callback
     * @param ctx DTLS context
     * @param callback Receive callback function
     * @param user_data User-provided context pointer
     */
    virtual void setRecvCallback(void* ctx, RecvCallback callback,
                                  void* user_data) = 0;

    /**
     * Set event callback
     * @param ctx DTLS context
     * @param callback Event callback function
     * @param user_data User-provided context pointer
     */
    virtual void setEventCallback(void* ctx, EventCallback callback,
                                    void* user_data) = 0;

    // =========================================================================
    // Utility Functions
    // =========================================================================

    /**
     * Get last error message
     * @return Error string (provider-specific)
     */
    virtual const char* getLastError() const = 0;

    /**
     * Enable/disable debug logging
     * @param enable true to enable, false to disable
     */
    virtual void setDebugLogging(bool enable) = 0;
};

#endif // DTLS_PROVIDER_H
