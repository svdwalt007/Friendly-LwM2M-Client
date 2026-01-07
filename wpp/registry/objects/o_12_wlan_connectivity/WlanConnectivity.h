/*
 * WlanConnectivity (Object ID 12)
 * Generated for OpenWRT One Router
 * Walt Technologies
 *
 * OMA LwM2M Object: WLAN Connectivity
 * Manages WiFi interface configuration and statistics
 */

#ifndef WPP_O_12_WLAN_CONNECTIVITY_H
#define WPP_O_12_WLAN_CONNECTIVITY_H

#include "WlanConnectivityConfig.h"
#include "WlanConnectivityInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class WlanConnectivity : public Instance {
public:
    enum ID: ID_T {
        INTERFACE_NAME_0 = 0,
        ENABLE_1 = 1,
        RADIO_ENABLED_2 = 2,
        STATUS_3 = 3,
        BSSID_4 = 4,
        SSID_5 = 5,
        BROADCAST_SSID_6 = 6,
        BEACON_ENABLED_7 = 7,
        MODE_8 = 8,
        CHANNEL_9 = 9,
        AUTO_CHANNEL_10 = 10,
        SUPPORTED_CHANNELS_11 = 11,
        CHANNELS_IN_USE_12 = 12,
        REGULATORY_DOMAIN_13 = 13,
        STANDARD_14 = 14,
        AUTHENTICATION_MODE_15 = 15,
        ENCRYPTION_MODE_16 = 16,
        WPA_PRE_SHARED_KEY_17 = 17,
        WPA_KEY_PHRASE_18 = 18,
        WEP_ENCRYPTION_TYPE_19 = 19,
        WEP_KEY_INDEX_20 = 20,
        WEP_KEY_PHRASE_21 = 21,
        WEP_KEY_1_22 = 22,
        WEP_KEY_2_23 = 23,
        WEP_KEY_3_24 = 24,
        WEP_KEY_4_25 = 25,
        RADIUS_SERVER_26 = 26,
        RADIUS_SERVER_PORT_27 = 27,
        RADIUS_SECRET_28 = 28,
        WMM_SUPPORTED_29 = 29,
        WMM_ENABLED_30 = 30,
        MAC_CONTROL_ENABLED_31 = 31,
        MAC_ADDRESS_LIST_32 = 32,
        TOTAL_BYTES_SENT_33 = 33,
        TOTAL_BYTES_RECEIVED_34 = 34,
        TOTAL_PACKETS_SENT_35 = 35,
        TOTAL_PACKETS_RECEIVED_36 = 36,
        TRANSMIT_ERRORS_37 = 37,
        RECEIVE_ERRORS_38 = 38,
        UNICAST_PACKETS_SENT_39 = 39,
        UNICAST_PACKETS_RECEIVED_40 = 40,
        MULTICAST_PACKETS_SENT_41 = 41,
        MULTICAST_PACKETS_RECEIVED_42 = 42,
        BROADCAST_PACKETS_SENT_43 = 43,
        BROADCAST_PACKETS_RECEIVED_44 = 44,
        DISCARD_PACKETS_SENT_45 = 45,
        DISCARD_PACKETS_RECEIVED_46 = 46,
        UNKNOWN_PACKETS_RECEIVED_47 = 47,
        #if RES_12_48
        VENDOR_SPECIFIC_EXTENSIONS_48 = 48,
        #endif
    };

    // Radio Band Enumeration
    enum RadioBand: uint8_t {
        BAND_2_4_GHZ = 0,
        BAND_5_GHZ = 1,
        BAND_0_9_GHZ = 2,
        BAND_3_7_GHZ = 3,
        BAND_45_GHZ = 4,
        BAND_60_GHZ = 5
    };

    // Interface Status Enumeration
    enum InterfaceStatus: uint8_t {
        DISABLED = 0,
        UP = 1,
        ERROR = 2
    };

    // Operating Mode Enumeration
    enum OperatingMode: uint8_t {
        ACCESS_POINT = 0,
        CLIENT = 1,
        BRIDGE = 2,
        REPEATER = 3
    };

    // WiFi Standard Enumeration
    enum WifiStandard: uint8_t {
        IEEE_802_11A = 0,
        IEEE_802_11B = 1,
        IEEE_802_11BG = 2,
        IEEE_802_11G = 3,
        IEEE_802_11N = 4,
        IEEE_802_11BGN = 5,
        IEEE_802_11AC = 6,
        IEEE_802_11AH = 7,
        IEEE_802_11AX = 8  // WiFi 6
    };

    // Authentication Mode Enumeration
    enum AuthMode: uint8_t {
        AUTH_NONE = 0,
        AUTH_PSK = 1,
        AUTH_EAP = 2,
        AUTH_EAP_PSK = 3,
        AUTH_EAPSIM = 4
    };

    // Encryption Mode Enumeration
    enum EncryptMode: uint8_t {
        ENC_AES = 0,   // WPA2
        ENC_TKIP = 1,  // WPA
        ENC_WEP = 2
    };

    // WEP Encryption Type Enumeration
    enum WepEncType: uint8_t {
        WEP_NONE = 0,
        WEP_40_BIT = 1,
        WEP_104_BIT = 2
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId = 0);
    static bool remove(WppClient& client, INST_T instId = 0);

    /* Instance lifecycle */
    WlanConnectivity(Object& object, INST_T instId);
    ~WlanConnectivity() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

#ifdef OPENWRT_BUILD
    void loadFromUCI();
    void updateStatistics();
#endif
};

} // namespace wpp

#endif // WPP_O_12_WLAN_CONNECTIVITY_H
