/*
 * FirewallConfig (Object 10521)
 * Walt Technologies - OpenWRT One Router
 *
 * Firewall rule management for OpenWRT firewall/iptables
 */

#ifndef WPP_W_10521_FIREWALL_CONFIG_H
#define WPP_W_10521_FIREWALL_CONFIG_H

#include "FirewallConfigConfig.h"
#include "FirewallConfigInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class FirewallConfig : public Instance {
public:
    enum ID: ID_T {
        RULE_NAME_0 = 0,          // Rule name (RW, String)
        RULE_INDEX_1 = 1,         // Rule index/priority (R, Integer)
        ENABLED_2 = 2,            // Rule enabled (RW, Boolean)
        ACTION_3 = 3,             // Action (RW, Integer)
        PROTOCOL_4 = 4,           // Protocol (RW, Integer)
        SOURCE_IP_5 = 5,          // Source IP address (RW, String)
        SOURCE_PORT_6 = 6,        // Source port (RW, Integer)
        DEST_IP_7 = 7,            // Destination IP address (RW, String)
        DEST_PORT_8 = 8,          // Destination port (RW, Integer)
        INTERFACE_9 = 9,          // Network interface (RW, String)
        DIRECTION_10 = 10,        // Traffic direction (RW, Integer)
        ZONE_FROM_11 = 11,        // Source zone (RW, String)
        ZONE_TO_12 = 12,          // Destination zone (RW, String)
        COMMENT_13 = 13,          // Rule comment (RW, String)
        APPLY_RULE_14 = 14,       // Apply rule (E, Execute)
        DELETE_RULE_15 = 15,      // Delete rule (E, Execute)
    };

    // Firewall Action Enumeration
    enum FirewallAction: uint8_t {
        ACTION_ACCEPT = 0,   // Accept the packet
        ACTION_REJECT = 1,   // Reject with ICMP error
        ACTION_DROP = 2,     // Silently drop the packet
    };

    // Protocol Enumeration
    enum FirewallProtocol: uint8_t {
        PROTOCOL_ALL = 0,    // All protocols
        PROTOCOL_TCP = 1,    // TCP only
        PROTOCOL_UDP = 2,    // UDP only
        PROTOCOL_ICMP = 3,   // ICMP only
    };

    // Traffic Direction Enumeration
    enum FirewallDirection: uint8_t {
        DIRECTION_INPUT = 0,    // Incoming traffic
        DIRECTION_OUTPUT = 1,   // Outgoing traffic
        DIRECTION_FORWARD = 2,  // Forwarded traffic
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    FirewallConfig(Object& object, INST_T instId);
    ~FirewallConfig() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool applyRule(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool deleteRule(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool loadFromUCI();
    bool saveToUCI();
    bool applyFirewallRules();
    std::string buildIptablesRule();
};

} // namespace wpp

#endif // WPP_W_10521_FIREWALL_CONFIG_H
