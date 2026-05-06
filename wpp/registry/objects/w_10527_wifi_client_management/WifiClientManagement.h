/*
 * WifiClientManagement (Object 10527)
 * Walt Technologies - OpenWRT One Router
 *
 * WiFi Client Management for connected clients monitoring and control
 */

#ifndef WPP_W_10527_WIFI_CLIENT_MANAGEMENT_H
#define WPP_W_10527_WIFI_CLIENT_MANAGEMENT_H

#include "WifiClientManagementConfig.h"
#include "WifiClientManagementInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class WifiClientManagement : public Instance {
public:
    enum ID: ID_T {
        MAC_ADDRESS_0 = 0,           // MAC Address (R, String)
        IP_ADDRESS_1 = 1,            // IP Address (R, String)
        HOSTNAME_2 = 2,              // Hostname (R, String)
        INTERFACE_3 = 3,             // Interface (R, String)
        SIGNAL_STRENGTH_4 = 4,       // Signal Strength/RSSI (R, Integer) - dBm
        NOISE_LEVEL_5 = 5,           // Noise Level (R, Integer) - dBm
        SNR_6 = 6,                   // SNR (R, Integer) - dB
        TX_RATE_7 = 7,               // TX Rate (R, Integer) - Mbps
        RX_RATE_8 = 8,               // RX Rate (R, Integer) - Mbps
        TX_BYTES_9 = 9,              // TX Bytes (R, Integer)
        RX_BYTES_10 = 10,            // RX Bytes (R, Integer)
        CONNECTED_TIME_11 = 11,      // Connected Time (R, Integer) - seconds
        CAPABILITY_12 = 12,          // Capability (R, String) - 802.11 standard
        BAND_13 = 13,                // Band (R, String) - 2.4GHz/5GHz/6GHz
        BANDWIDTH_14 = 14,           // Bandwidth (R, Integer) - MHz
        ACL_STATE_15 = 15,           // ACL State (RW, String) - allowed/denied/none
        QOS_PRIORITY_16 = 16,        // QoS Priority (RW, Integer) - 0-7
        ISOLATED_17 = 17,            // Isolated (RW, Boolean)
        LAST_ACTIVITY_18 = 18,       // Last Activity (R, Integer) - seconds since last activity
        DISCONNECT_CLIENT_19 = 19,   // Disconnect Client (E, Execute)
        ADD_TO_BLACKLIST_20 = 20,    // Add to Blacklist (E, Execute)
        ADD_TO_WHITELIST_21 = 21,    // Add to Whitelist (E, Execute)
        STEER_TO_BAND_22 = 22        // Steer to Band (E, Execute) - parameter: "2.4" or "5" or "6"
    };

    // ACL State Enumeration
    enum ACLState {
        ACL_NONE,        // No ACL policy applied
        ACL_ALLOWED,     // Explicitly allowed
        ACL_DENIED       // Explicitly denied/blacklisted
    };

    /* Static object methods */
    static Object& object(WppClient& ctx);
    static WifiClientManagement* createInst(WppClient& ctx, ID_T instId = ID_T_MAX_VAL);
    static WifiClientManagement* instance(WppClient& ctx, ID_T instId);
    static bool removeInst(WppClient& ctx, ID_T instId);

    /* Instance lifecycle */
    WifiClientManagement(lwm2m_context_t& context, const OBJ_LINK_T& id);
    ~WifiClientManagement();

    /* Operation notification handlers */
    void serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) override;
    void userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) override;

    /* Public methods */
    void updateClientStats();
    static void enumerateClients(WppClient& ctx);

private:
    /* Resource setup methods */
    void resourcesCreate();
    void resourcesInit();

    /* Execute handlers */
    static bool disconnectClient(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool addToBlacklist(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool addToWhitelist(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool steerToBand(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool getClientInfoFromHostapd();
    bool getClientInfoFromIwinfo();
    bool getClientInfoFromUbus();
    bool getClientBandwidthStats();
    bool isValidMAC(const std::string& mac);
    std::string getClientIPFromMAC(const std::string& mac);
    std::string getClientHostnameFromMAC(const std::string& mac);
    std::string parseCapabilityString(const std::string& flags);
    std::string determineBand(int frequency);
    int getBandwidthFromFlags(const std::string& flags);
    bool sendBssTransitionRequest(const std::string& mac, const std::string& targetBand);
    bool executeHostapdCommand(const std::string& interface, const std::string& command);
    std::string getHostapdControlSocket(const std::string& interface);

    /* Member variables */
    std::string macAddress;
    std::string interface;
    time_t connectionStartTime;
    time_t lastActivityTime;
    uint64_t lastTxBytes;
    uint64_t lastRxBytes;
};

} // namespace wpp

#endif // WPP_W_10527_WIFI_CLIENT_MANAGEMENT_H
