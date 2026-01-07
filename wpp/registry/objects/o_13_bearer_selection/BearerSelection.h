/*
 * BearerSelection (Object ID 13)
 * OMA LwM2M Object: Bearer Selection
 * Manages preferred network bearer selection
 */

#ifndef WPP_O_13_BEARER_SELECTION_H
#define WPP_O_13_BEARER_SELECTION_H

#include "BearerSelectionConfig.h"
#include "BearerSelectionInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class BearerSelection : public Instance {
public:
    enum ID: ID_T {
        PREFERRED_COMMS_BEARER_0 = 0,           // Preferred bearer
        ACCEPTABLE_RSSI_GSM_1 = 1,              // GSM signal threshold (dBm)
        ACCEPTABLE_RSCP_UMTS_2 = 2,             // UMTS signal threshold (dBm)
        ACCEPTABLE_RSRP_LTE_3 = 3,              // LTE signal threshold (dBm)
        ACCEPTABLE_RSSI_WLAN_4 = 4,             // WLAN signal threshold (dBm)
        CELL_LOCK_LIST_5 = 5,                   // List of locked cells
        OPERATOR_LIST_6 = 6,                    // List of operators
        OPERATOR_LIST_MODE_7 = 7,               // Whitelist/Blacklist mode
        AVAILABLE_NETWORK_BEARERS_8 = 8,        // Available bearers (R)
        ACCEPTABLE_SIGNAL_STRENGTH_VAR_9 = 9,   // Signal strength variation (dB)
        HIGHER_PRIORITY_PLMN_SEARCH_TIMER_10 = 10, // Search timer (seconds)
        ATTACH_WITHOUT_PDN_11 = 11,             // Attach without PDN
    };

    // Network Bearer Enumeration (from Connectivity Monitoring Object)
    enum NetworkBearer: INT_T {
        GSM = 0,
        TD_SCDMA = 1,
        WCDMA = 2,
        CDMA2000 = 3,
        WIMAX = 4,
        LTE_TDD = 5,
        LTE_FDD = 6,
        LTE_M = 7,
        NB_IOT = 8,
        // Non-cellular bearers
        WLAN = 21,
        BLUETOOTH = 22,
        IEEE_802_15_4 = 23,
        ETHERNET = 41,
        DSL = 42,
        PLC = 43,
    };

    // Operator List Mode Enumeration
    enum OperatorListMode: INT_T {
        WHITELIST = 0,  // Only connect to listed operators
        BLACKLIST = 1,  // Avoid listed operators
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId = 0);
    static bool remove(WppClient& client, INST_T instId = 0);

    /* Instance lifecycle */
    BearerSelection(Object& object, INST_T instId);
    ~BearerSelection() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

#ifdef OPENWRT_BUILD
    void loadFromUCI();
    void updateAvailableBearers();
    NetworkBearer detectCurrentBearer();
#endif
};

} // namespace wpp

#endif // WPP_O_13_BEARER_SELECTION_H
