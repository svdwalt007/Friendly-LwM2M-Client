/*
 * WanFailoverPolicy (Object 33420)
 * Walt Technologies - OpenWRT One Router
 *
 * Multi-WAN Failover Policy management for OpenWRT
 */

#ifndef WPP_W_33420_WAN_FAILOVER_POLICY_H
#define WPP_W_33420_WAN_FAILOVER_POLICY_H

#include "WanFailoverPolicyConfig.h"
#include "WanFailoverPolicyInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class WanFailoverPolicy : public Instance {
public:
    enum ID: ID_T {
        POLICY_NAME_0 = 0,            // Policy Name (RW, String)
        PRIMARY_WAN_1 = 1,            // Primary WAN Interface (RW, String)
        SECONDARY_WAN_2 = 2,          // Secondary WAN Interface (RW, String)
        TERTIARY_WAN_3 = 3,           // Tertiary WAN Interface (RW, String)
        MODE_4 = 4,                   // Mode (RW, Integer)
        PRIMARY_WEIGHT_5 = 5,         // Primary Weight (RW, Integer)
        SECONDARY_WEIGHT_6 = 6,       // Secondary Weight (RW, Integer)
        TERTIARY_WEIGHT_7 = 7,        // Tertiary Weight (RW, Integer)
        FAILOVER_THRESHOLD_8 = 8,     // Failover Threshold (RW, Integer)
        FAILBACK_MODE_9 = 9,          // Failback Mode (RW, Integer)
        FAILBACK_DELAY_10 = 10,       // Failback Delay (RW, Integer, seconds)
        CURRENT_ACTIVE_WAN_11 = 11,   // Current Active WAN (R, String)
        POLICY_STATE_12 = 12,         // Policy State (R, Integer)
        LAST_FAILOVER_TIME_13 = 13,   // Last Failover Time (R, Time)
        FAILOVER_COUNT_14 = 14,       // Failover Count (R, Integer)
        STICKY_CONNECTIONS_15 = 15,   // Sticky Connections (RW, Boolean)
        INTERFACE_RULES_16 = 16,      // Interface Rules (RW, Multiple, String)
        APPLY_POLICY_17 = 17,         // Apply Policy (E, Execute)
        FORCE_FAILOVER_18 = 18,       // Force Failover (E, Execute)
        FORCE_FAILBACK_19 = 19,       // Force Failback (E, Execute)
        RESET_COUNTERS_20 = 20        // Reset Counters (E, Execute)
    };

    // Failover Mode Enumeration
    enum FailoverMode: uint8_t {
        MODE_FAILOVER = 0,      // Pure failover - use primary, fallback to secondary
        MODE_LOADBALANCE = 1,   // Load balancing based on weights
        MODE_ROUNDROBIN = 2     // Round-robin distribution
    };

    // Failback Mode Enumeration
    enum FailbackMode: uint8_t {
        FAILBACK_IMMEDIATE = 0,  // Immediately failback when primary is restored
        FAILBACK_DELAYED = 1,    // Wait for delay period before failback
        FAILBACK_MANUAL = 2      // Only failback on manual command
    };

    // Policy State Enumeration
    enum PolicyState: uint8_t {
        STATE_INACTIVE = 0,      // Policy not applied
        STATE_ACTIVE = 1,        // Policy active and working
        STATE_ERROR = 2          // Policy error state
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    WanFailoverPolicy(Object& object, INST_T instId);
    ~WanFailoverPolicy() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool applyPolicy(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool forceFailover(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool forceFailback(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool resetCounters(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool loadFromMwan3();
    bool saveToMwan3();
    bool applyMwan3Config();
    bool checkInterfaceStatus(const std::string& ifname);
    bool triggerFailover(const std::string& fromInterface, const std::string& toInterface);
    bool triggerFailback();
    void updateInterfaceStatus();
    std::string getActiveInterface();
    bool isValidInterface(const std::string& ifname);
    bool configurePolicyRouting();
    bool setupHealthChecks();
    bool applyLoadBalancingWeights();
    int calculateTotalWeight();
    std::string getMwan3PolicyName();
};

} // namespace wpp

#endif // WPP_W_33420_WAN_FAILOVER_POLICY_H
