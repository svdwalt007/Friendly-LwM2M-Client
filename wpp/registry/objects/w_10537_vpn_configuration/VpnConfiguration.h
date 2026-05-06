/*
 * VpnConfiguration (Object 10537)
 * Walt Technologies - OpenWRT One Router
 *
 * VPN Configuration and management for OpenWRT
 * Supports OpenVPN, WireGuard, and IPsec/IKEv2 protocols
 */

#ifndef WPP_W_10537_VPN_CONFIGURATION_H
#define WPP_W_10537_VPN_CONFIGURATION_H

#include "VpnConfigurationConfig.h"
#include "VpnConfigurationInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class VpnConfiguration : public Instance {
public:
    enum ID: ID_T {
        VPN_NAME_0 = 0,                  // VPN Name (RW, String)
        PROTOCOL_TYPE_1 = 1,             // Protocol Type (RW, Integer)
        SERVER_ADDRESS_2 = 2,            // Server Address (RW, String)
        SERVER_PORT_3 = 3,               // Server Port (RW, Integer)
        LOCAL_ADDRESS_4 = 4,             // Local Address (R, String)
        REMOTE_NETWORK_5 = 5,            // Remote Network (RW, String)
        AUTHENTICATION_TYPE_6 = 6,       // Authentication Type (RW, Integer)
        USERNAME_7 = 7,                  // Username (RW, String)
        PASSWORD_8 = 8,                  // Password (RW, String)
        CERTIFICATE_9 = 9,               // Certificate (RW, String)
        PRIVATE_KEY_10 = 10,             // Private Key (RW, String)
        PUBLIC_KEY_11 = 11,              // Public Key (R, String)
        PRE_SHARED_KEY_12 = 12,          // Pre-Shared Key (RW, String)
        CONNECTION_STATE_13 = 13,        // Connection State (R, Integer)
        BYTES_SENT_14 = 14,              // Bytes Sent (R, Integer)
        BYTES_RECEIVED_15 = 15,          // Bytes Received (R, Integer)
        UPTIME_16 = 16,                  // Uptime (R, Integer, seconds)
        LAST_ERROR_17 = 17,              // Last Error (R, String)
        KEEPALIVE_INTERVAL_18 = 18,      // Keepalive Interval (RW, Integer, seconds)
        RECONNECT_DELAY_19 = 19,         // Reconnect Delay (RW, Integer, seconds)
        SPLIT_TUNNEL_20 = 20,            // Split Tunnel (RW, Boolean)
        KILL_SWITCH_21 = 21,             // Kill Switch (RW, Boolean)
        CONNECT_22 = 22,                 // Connect (E, Execute)
        DISCONNECT_23 = 23,              // Disconnect (E, Execute)
        RESTART_24 = 24,                 // Restart (E, Execute)
        REGENERATE_KEYS_25 = 25          // Regenerate Keys (E, Execute)
    };

    // Protocol Type Enumeration
    enum ProtocolType: uint8_t {
        PROTOCOL_OPENVPN = 0,     // OpenVPN protocol
        PROTOCOL_WIREGUARD = 1,   // WireGuard protocol
        PROTOCOL_IPSEC = 2,       // IPsec/IKEv2 protocol
    };

    // Connection State Enumeration
    enum ConnectionState: uint8_t {
        STATE_DISCONNECTED = 0,   // VPN is disconnected
        STATE_CONNECTING = 1,     // VPN is connecting
        STATE_CONNECTED = 2,      // VPN is connected
        STATE_DISCONNECTING = 3,  // VPN is disconnecting
        STATE_ERROR = 4,          // VPN is in error state
        STATE_RECONNECTING = 5    // VPN is reconnecting
    };

    // Authentication Type Enumeration
    enum AuthType: uint8_t {
        AUTH_PASSWORD = 0,        // Username/Password authentication
        AUTH_CERTIFICATE = 1,     // Certificate-based authentication
        AUTH_PSK = 2,            // Pre-Shared Key authentication
        AUTH_PUBKEY = 3          // Public Key authentication (WireGuard)
    };

    /* Static object methods */
    static Object& object(WppClient& ctx);
    static VpnConfiguration* createInst(WppClient& ctx, ID_T instId = ID_T_MAX_VAL);
    static VpnConfiguration* instance(WppClient& ctx, ID_T instId);
    static bool removeInst(WppClient& ctx, ID_T instId);

    /* Instance lifecycle */
    VpnConfiguration(lwm2m_context_t& context, const OBJ_LINK_T& id);
    ~VpnConfiguration();

    /* Operation notification handlers */
    void serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) override;
    void userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) override;

private:
    /* Resource setup methods */
    void resourcesCreate();
    void resourcesInit();

    /* Execute handlers */
    static bool executeConnect(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool executeDisconnect(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool executeRestart(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool executeRegenerateKeys(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool loadFromUCI();
    bool saveToUCI();
    bool updateConnectionStatus();
    bool updateTrafficStats();
    std::string getInterfaceName();

    /* Protocol-specific methods */
    bool connectOpenVPN();
    bool connectWireGuard();
    bool connectIPsec();
    bool disconnectOpenVPN();
    bool disconnectWireGuard();
    bool disconnectIPsec();

    /* Configuration generation */
    bool generateOpenVPNConfig();
    bool generateWireGuardConfig();
    bool generateIPsecConfig();

    /* Key management */
    bool generateWireGuardKeys();
    bool validateCertificate(const std::string& cert);
    bool validatePrivateKey(const std::string& key);

    /* Network management */
    bool setupKillSwitch();
    bool removeKillSwitch();
    bool setupSplitTunnel();
    bool setupRouting();
    bool setupDNS();

    /* Monitoring */
    bool startMonitoring();
    bool stopMonitoring();

    /* Validation helpers */
    bool isValidIPAddress(const std::string& ip);
    bool isValidPort(int port);
    bool isValidCIDR(const std::string& cidr);

    /* Member variables */
    time_t connectionStartTime;
    bool isMonitoring;
};

} // namespace wpp

#endif // WPP_W_10537_VPN_CONFIGURATION_H
