/*
 * RouterManagement (Object 34601)
 * Walt Technologies - OpenWRT One Router
 *
 * Core router configuration and management
 */

#ifndef WPP_W_34601_ROUTER_MANAGEMENT_H
#define WPP_W_34601_ROUTER_MANAGEMENT_H

#include "RouterManagementConfig.h"
#include "RouterManagementInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class RouterManagement : public Instance {
public:
    enum ID: ID_T {
        ROUTER_NAME_0 = 0,
        LAN_IP_ADDRESS_1 = 1,
        LAN_SUBNET_MASK_2 = 2,
        DHCP_ENABLED_3 = 3,
        DHCP_START_IP_4 = 4,
        DHCP_END_IP_5 = 5,
        DHCP_LEASE_TIME_6 = 6,
        DNS_SERVER_1_7 = 7,
        DNS_SERVER_2_8 = 8,
        WAN_CONNECTION_TYPE_9 = 9,
        WAN_IP_ADDRESS_10 = 10,
        WAN_GATEWAY_11 = 11,
        FIREWALL_ENABLED_12 = 12,
        NAT_ENABLED_13 = 13,
        UPNP_ENABLED_14 = 14,
        APPLY_CONFIGURATION_15 = 15,
        RESET_TO_DEFAULTS_16 = 16,
    };

    // WAN Connection Type Enumeration
    enum WanConnectionType: uint8_t {
        WAN_DHCP = 0,
        WAN_STATIC = 1,
        WAN_PPPOE = 2
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId = 0);
    static bool remove(WppClient& client, INST_T instId = 0);

    /* Instance lifecycle */
    RouterManagement(Object& object, INST_T instId);
    ~RouterManagement() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool applyConfiguration(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool resetToDefaults(Instance& inst, ID_T resId, const OPAQUE_T& data);
};

} // namespace wpp

#endif // WPP_W_34601_ROUTER_MANAGEMENT_H
