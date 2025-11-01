/*
 * OpenWrtConnection.cpp
 *
 * OpenWRT-specific connection implementation
 */

#include "OpenWrtConnection.h"
#include "WppClient.h"
#include "WppPlatform.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>

namespace wpp {

OpenWrtConnection::OpenWrtConnection(const std::string& interfaceName,
                                     const std::string& port,
                                     int addressFamily)
    : m_interfaceName(interfaceName)
    , m_port(port)
    , m_addressFamily(addressFamily)
    , m_sockFd(-1)
    , m_connections(nullptr)
    , m_localAddrLen(0)
{
    std::memset(&m_localAddr, 0, sizeof(m_localAddr));
    openSocket();
}

OpenWrtConnection::~OpenWrtConnection() {
    closeSocket();
}

bool OpenWrtConnection::openSocket() {
    if (m_sockFd >= 0) {
        closeSocket();
    }

    // Create UDP socket
    m_sockFd = socket(m_addressFamily, SOCK_DGRAM, 0);
    if (m_sockFd < 0) {
        WppPlatform::print("Failed to create socket: %s\n", strerror(errno));
        return false;
    }

    // Set socket to non-blocking mode
    int flags = fcntl(m_sockFd, F_GETFL, 0);
    if (flags < 0 || fcntl(m_sockFd, F_SETFL, flags | O_NONBLOCK) < 0) {
        WppPlatform::print("Failed to set socket non-blocking: %s\n", strerror(errno));
        closeSocket();
        return false;
    }

    // Enable address reuse
    int reuse = 1;
    if (setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        WppPlatform::print("Failed to set SO_REUSEADDR: %s\n", strerror(errno));
    }

    // Prepare bind address
    struct sockaddr_storage addr;
    socklen_t addrLen;
    std::memset(&addr, 0, sizeof(addr));

    if (m_addressFamily == AF_INET6) {
        struct sockaddr_in6* addr6 = (struct sockaddr_in6*)&addr;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(std::stoi(m_port));
        addr6->sin6_addr = in6addr_any;
        addrLen = sizeof(struct sockaddr_in6);

        // Enable dual-stack (IPv4 and IPv6)
        int no = 0;
        setsockopt(m_sockFd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no));
    } else {
        struct sockaddr_in* addr4 = (struct sockaddr_in*)&addr;
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(std::stoi(m_port));
        addr4->sin_addr.s_addr = INADDR_ANY;
        addrLen = sizeof(struct sockaddr_in);
    }

    // Bind socket
    if (bind(m_sockFd, (struct sockaddr*)&addr, addrLen) < 0) {
        WppPlatform::print("Failed to bind socket to port %s: %s\n",
                          m_port.c_str(), strerror(errno));
        closeSocket();
        return false;
    }

    // Store local address
    m_localAddrLen = sizeof(m_localAddr);
    if (getsockname(m_sockFd, (struct sockaddr*)&m_localAddr, &m_localAddrLen) < 0) {
        WppPlatform::print("Failed to get local address: %s\n", strerror(errno));
    }

    // Bind to specific interface if specified
    if (!m_interfaceName.empty()) {
        bindToInterface();
    }

    WppPlatform::print("OpenWRT Connection: Socket opened on port %s\n", m_port.c_str());
    return true;
}

void OpenWrtConnection::closeSocket() {
    if (m_sockFd >= 0) {
        close(m_sockFd);
        m_sockFd = -1;
    }

    // Free connection list
    while (m_connections) {
        connection_t* next = m_connections->next;
        delete m_connections;
        m_connections = next;
    }
}

bool OpenWrtConnection::bindToInterface() {
    if (m_sockFd < 0 || m_interfaceName.empty()) {
        return false;
    }

    // Bind socket to network interface
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, m_interfaceName.c_str(), IFNAMSIZ - 1);

    if (setsockopt(m_sockFd, SOL_SOCKET, SO_BINDTODEVICE,
                   &ifr, sizeof(ifr)) < 0) {
        WppPlatform::print("Failed to bind to interface %s: %s\n",
                          m_interfaceName.c_str(), strerror(errno));
        return false;
    }

    WppPlatform::print("Bound to interface: %s\n", m_interfaceName.c_str());
    return true;
}

WppConnection::SESSION_T OpenWrtConnection::connect(Lwm2mSecurity& security) {
    std::string uri = security.getUri();
    std::string host = uriToHost(uri);
    std::string port = uriToPort(uri);

    if (host.empty() || port.empty()) {
        WppPlatform::print("Invalid URI: %s\n", uri.c_str());
        return nullptr;
    }

    struct sockaddr_storage addr;
    socklen_t addrLen;

    if (!resolveAddress(host, port, &addr, &addrLen)) {
        WppPlatform::print("Failed to resolve address: %s:%s\n",
                          host.c_str(), port.c_str());
        return nullptr;
    }

    // Check if connection already exists
    connection_t* conn = findConnection((struct sockaddr*)&addr, addrLen);
    if (conn) {
        return conn;
    }

    // Create new connection
    conn = createNewConn((struct sockaddr*)&addr, addrLen);
    if (!conn) {
        WppPlatform::print("Failed to create connection\n");
        return nullptr;
    }

    WppPlatform::print("Connected to %s:%s\n", host.c_str(), port.c_str());
    return conn;
}

void OpenWrtConnection::disconnect(SESSION_T session) {
    if (!session) {
        return;
    }

    connection_t* conn = static_cast<connection_t*>(session);
    connection_t* prev = nullptr;
    connection_t* current = m_connections;

    while (current) {
        if (current == conn) {
            if (prev) {
                prev->next = current->next;
            } else {
                m_connections = current->next;
            }
            delete current;
            WppPlatform::print("Connection disconnected\n");
            return;
        }
        prev = current;
        current = current->next;
    }
}

bool OpenWrtConnection::sessionCmp(SESSION_T session1, SESSION_T session2) {
    return session1 == session2;
}

bool OpenWrtConnection::sendPacket(const Packet& packet) {
    if (m_sockFd < 0) {
        WppPlatform::print("Socket not open\n");
        return false;
    }

    if (!packet.session) {
        WppPlatform::print("Invalid session\n");
        return false;
    }

    connection_t* conn = static_cast<connection_t*>(packet.session);

    ssize_t sent = sendto(m_sockFd, packet.buffer, packet.length, 0,
                         (struct sockaddr*)&conn->addr, conn->addrLen);

    if (sent < 0) {
        WppPlatform::print("Failed to send packet: %s\n", strerror(errno));
        return false;
    }

    if ((size_t)sent != packet.length) {
        WppPlatform::print("Partial send: %zd of %zu bytes\n", sent, packet.length);
        return false;
    }

    return true;
}

void OpenWrtConnection::loop() {
    if (m_sockFd < 0) {
        return;
    }

    uint8_t buffer[2048];
    struct sockaddr_storage addr;
    socklen_t addrLen = sizeof(addr);

    ssize_t received = recvfrom(m_sockFd, buffer, sizeof(buffer), 0,
                               (struct sockaddr*)&addr, &addrLen);

    if (received < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            WppPlatform::print("Receive error: %s\n", strerror(errno));
        }
        return;
    }

    if (received == 0) {
        return;
    }

    // Find or create connection for this peer
    connection_t* conn = findConnection((struct sockaddr*)&addr, addrLen);
    if (!conn) {
        conn = createNewConn((struct sockaddr*)&addr, addrLen);
        if (!conn) {
            WppPlatform::print("Failed to create connection for incoming packet\n");
            return;
        }
    }

    // Create packet and add to queue
    Packet packet;
    packet.session = conn;
    packet.length = received;
    packet.buffer = new uint8_t[received];
    std::memcpy(packet.buffer, buffer, received);

    if (!addPacketToQueue(packet)) {
        WppPlatform::print("Failed to add packet to queue (queue full)\n");
        delete[] packet.buffer;
    }
}

bool OpenWrtConnection::setInterface(const std::string& interfaceName) {
    m_interfaceName = interfaceName;
    if (m_sockFd >= 0) {
        return bindToInterface();
    }
    return true;
}

std::string OpenWrtConnection::getInterface() const {
    return m_interfaceName;
}

bool OpenWrtConnection::isReady() const {
    return m_sockFd >= 0;
}

std::string OpenWrtConnection::getLocalAddress() const {
    if (m_localAddrLen == 0) {
        return "";
    }

    char addrStr[INET6_ADDRSTRLEN];
    const void* addrPtr = nullptr;

    if (m_localAddr.ss_family == AF_INET) {
        addrPtr = &((struct sockaddr_in*)&m_localAddr)->sin_addr;
    } else if (m_localAddr.ss_family == AF_INET6) {
        addrPtr = &((struct sockaddr_in6*)&m_localAddr)->sin6_addr;
    }

    if (addrPtr && inet_ntop(m_localAddr.ss_family, addrPtr, addrStr, sizeof(addrStr))) {
        return std::string(addrStr);
    }

    return "";
}

int OpenWrtConnection::getLocalPort() const {
    if (m_localAddrLen == 0) {
        return 0;
    }

    if (m_localAddr.ss_family == AF_INET) {
        return ntohs(((struct sockaddr_in*)&m_localAddr)->sin_port);
    } else if (m_localAddr.ss_family == AF_INET6) {
        return ntohs(((struct sockaddr_in6*)&m_localAddr)->sin6_port);
    }

    return 0;
}

OpenWrtConnection::connection_t* OpenWrtConnection::createNewConn(struct sockaddr* addr,
                                                                  size_t addrLen) {
    connection_t* conn = new connection_t;
    if (!conn) {
        return nullptr;
    }

    conn->sock = m_sockFd;
    conn->addrLen = addrLen;
    std::memcpy(&conn->addr, addr, addrLen);
    conn->next = m_connections;
    m_connections = conn;

    return conn;
}

OpenWrtConnection::connection_t* OpenWrtConnection::findConnection(struct sockaddr* addr,
                                                                   size_t addrLen) {
    connection_t* current = m_connections;

    while (current) {
        if (current->addrLen == addrLen &&
            std::memcmp(&current->addr, addr, addrLen) == 0) {
            return current;
        }
        current = current->next;
    }

    return nullptr;
}

std::string OpenWrtConnection::uriToPort(const std::string& uri) {
    // Parse port from URI (e.g., "coap://example.com:5683" -> "5683")
    size_t portStart = uri.rfind(':');
    if (portStart == std::string::npos) {
        // Default CoAP port
        return uri.find("coaps://") == 0 ? "5684" : "5683";
    }

    return uri.substr(portStart + 1);
}

std::string OpenWrtConnection::uriToHost(const std::string& uri) {
    // Parse host from URI (e.g., "coap://example.com:5683" -> "example.com")
    size_t hostStart = uri.find("://");
    if (hostStart == std::string::npos) {
        return uri;
    }

    hostStart += 3; // Skip "://"
    size_t hostEnd = uri.find(':', hostStart);
    if (hostEnd == std::string::npos) {
        hostEnd = uri.find('/', hostStart);
    }
    if (hostEnd == std::string::npos) {
        hostEnd = uri.length();
    }

    return uri.substr(hostStart, hostEnd - hostStart);
}

bool OpenWrtConnection::resolveAddress(const std::string& host,
                                       const std::string& port,
                                       struct sockaddr_storage* addr,
                                       socklen_t* addrLen) {
    struct addrinfo hints;
    struct addrinfo* result = nullptr;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = m_addressFamily;
    hints.ai_socktype = SOCK_DGRAM;

    int ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
    if (ret != 0) {
        WppPlatform::print("getaddrinfo failed: %s\n", gai_strerror(ret));
        return false;
    }

    if (!result) {
        return false;
    }

    std::memcpy(addr, result->ai_addr, result->ai_addrlen);
    *addrLen = result->ai_addrlen;

    freeaddrinfo(result);
    return true;
}

std::string OpenWrtConnection::getInterfaceAddress(const std::string& interfaceName,
                                                   int addressFamily) {
    struct ifaddrs* ifaddr = nullptr;
    struct ifaddrs* ifa = nullptr;
    std::string address;

    if (getifaddrs(&ifaddr) == -1) {
        return "";
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }

        if (interfaceName == ifa->ifa_name &&
            ifa->ifa_addr->sa_family == addressFamily) {
            char addrStr[INET6_ADDRSTRLEN];
            const void* addrPtr = nullptr;

            if (addressFamily == AF_INET) {
                addrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
            } else if (addressFamily == AF_INET6) {
                addrPtr = &((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr;
            }

            if (addrPtr && inet_ntop(addressFamily, addrPtr, addrStr, sizeof(addrStr))) {
                address = addrStr;
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
    return address;
}

} // namespace wpp
