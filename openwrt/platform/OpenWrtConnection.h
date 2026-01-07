/*
 * OpenWrtConnection.h
 *
 * OpenWRT-specific connection implementation for LwM2M client
 * Integrates with OpenWRT network infrastructure
 */

#ifndef OPENWRT_CONNECTION_H
#define OPENWRT_CONNECTION_H

#include <netinet/in.h>
#include <string>
#include <map>
#include "WppConnection.h"
#include "WppRegistry.h"

namespace wpp {

/**
 * @brief OpenWRT-specific WppConnection implementation
 *
 * Provides network connectivity using OpenWRT's network infrastructure.
 * Features:
 * - Automatic interface selection based on UCI configuration
 * - Support for IPv4 and IPv6
 * - Network event handling
 * - Interface monitoring and failover
 */
class OpenWrtConnection : public WppConnection {
private:
    struct connection_t {
        struct connection_t* next;
        int sock;
        struct sockaddr_in6 addr;
        size_t addrLen;
    };

public:
    /**
     * @brief Constructor
     * @param interfaceName Network interface name (from UCI, e.g., "wan", "lan")
     * @param port Local CoAP port
     * @param addressFamily AF_INET or AF_INET6
     */
    OpenWrtConnection(const std::string& interfaceName = "wan",
                      const std::string& port = "5683",
                      int addressFamily = AF_INET6);

    /**
     * @brief Destructor
     */
    ~OpenWrtConnection();

    // WppConnection interface implementation
    SESSION_T connect(Lwm2mSecurity& security) override;
    void disconnect(SESSION_T session) override;
    bool sessionCmp(SESSION_T session1, SESSION_T session2) override;
    bool sendPacket(const Packet& packet) override;

    /**
     * @brief Main loop for receiving packets
     *
     * Should be called regularly to receive incoming packets.
     * Uses non-blocking socket operations.
     */
    void loop();

    /**
     * @brief Set network interface
     * @param interfaceName Interface name (e.g., "wan", "eth0")
     * @return true if successful
     */
    bool setInterface(const std::string& interfaceName);

    /**
     * @brief Get current network interface
     * @return Interface name
     */
    std::string getInterface() const;

    /**
     * @brief Check if connection is ready
     * @return true if socket is open and ready
     */
    bool isReady() const;

    /**
     * @brief Get local IP address
     * @return Local IP address string
     */
    std::string getLocalAddress() const;

    /**
     * @brief Get local port
     * @return Local port number
     */
    int getLocalPort() const;

private:
    /**
     * @brief Open UDP socket
     * @return true if successful
     */
    bool openSocket();

    /**
     * @brief Close socket and cleanup
     */
    void closeSocket();

    /**
     * @brief Create new connection entry
     * @param addr Peer address
     * @param addrLen Address length
     * @return Connection entry pointer
     */
    connection_t* createNewConn(struct sockaddr* addr, size_t addrLen);

    /**
     * @brief Find existing connection by address
     * @param addr Peer address
     * @param addrLen Address length
     * @return Connection entry pointer or nullptr
     */
    connection_t* findConnection(struct sockaddr* addr, size_t addrLen);

    /**
     * @brief Parse port from CoAP URI
     * @param uri URI string (e.g., "coap://example.com:5683")
     * @return Port string
     */
    std::string uriToPort(const std::string& uri);

    /**
     * @brief Parse host from CoAP URI
     * @param uri URI string
     * @return Host string
     */
    std::string uriToHost(const std::string& uri);

    /**
     * @brief Resolve hostname to address
     * @param host Hostname or IP address
     * @param port Port number
     * @param addr Output address structure
     * @param addrLen Output address length
     * @return true if successful
     */
    bool resolveAddress(const std::string& host, const std::string& port,
                       struct sockaddr_storage* addr, socklen_t* addrLen);

    /**
     * @brief Get IP address from interface using OpenWRT methods
     * @param interfaceName Interface name
     * @param addressFamily AF_INET or AF_INET6
     * @return IP address string or empty
     */
    std::string getInterfaceAddress(const std::string& interfaceName, int addressFamily);

    /**
     * @brief Bind socket to specific interface
     * @return true if successful
     */
    bool bindToInterface();

private:
    std::string m_interfaceName;
    std::string m_port;
    int m_addressFamily;
    int m_sockFd;
    connection_t* m_connections;

    // Local address info
    struct sockaddr_storage m_localAddr;
    socklen_t m_localAddrLen;
};

} // namespace wpp

#endif // OPENWRT_CONNECTION_H
