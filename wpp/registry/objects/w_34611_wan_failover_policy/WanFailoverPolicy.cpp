/*
 * WanFailoverPolicy Implementation (Object 34611)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_34611_wan_failover_policy/WanFailoverPolicy.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"
#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34611_WAN_FAILOVER_POLICY

#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#include <regex>
#include <algorithm>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <unistd.h>
#include <net/if.h>

#define TAG "WanFailoverPolicy"

using namespace wpp;

/* Static object methods */
Object& WanFailoverPolicy::object(WppClient& client) {
    return client.registry().getObject(WAN_FAILOVER_POLICY_OBJECT_ID);
}

Instance* WanFailoverPolicy::createInst(WppClient& client, INST_T instId) {
    return object(client).createInstance(instId);
}

Instance* WanFailoverPolicy::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool WanFailoverPolicy::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

/* Instance lifecycle */
WanFailoverPolicy::WanFailoverPolicy(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "WanFailoverPolicy instance %d created", instId);
}

WanFailoverPolicy::~WanFailoverPolicy() {
    WPP_LOGD(TAG, "WanFailoverPolicy instance %d destroyed", instId());
}

/* Initialize resources */
bool WanFailoverPolicy::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing WanFailoverPolicy resources for instance %d", instId());

    // Default failover policy values
    set<STRING_T>(POLICY_NAME_0, "default_policy");
    set<STRING_T>(PRIMARY_WAN_1, "wan");
    set<STRING_T>(SECONDARY_WAN_2, "wan2");
    set<STRING_T>(TERTIARY_WAN_3, "");
    set<INT_T>(MODE_4, MODE_FAILOVER);
    set<INT_T>(PRIMARY_WEIGHT_5, 50);
    set<INT_T>(SECONDARY_WEIGHT_6, 30);
    set<INT_T>(TERTIARY_WEIGHT_7, 20);
    set<INT_T>(FAILOVER_THRESHOLD_8, 3);  // 3 missed pings
    set<INT_T>(FAILBACK_MODE_9, FAILBACK_DELAYED);
    set<INT_T>(FAILBACK_DELAY_10, 60);  // 60 seconds
    set<STRING_T>(CURRENT_ACTIVE_WAN_11, "");
    set<INT_T>(POLICY_STATE_12, STATE_INACTIVE);
    set<TIME_T>(LAST_FAILOVER_TIME_13, 0);
    set<INT_T>(FAILOVER_COUNT_14, 0);
    set<BOOL_T>(STICKY_CONNECTIONS_15, true);

    // Set execute handlers
    setExecute(APPLY_POLICY_17, applyPolicy);
    setExecute(FORCE_FAILOVER_18, forceFailover);
    setExecute(FORCE_FAILBACK_19, forceFailback);
    setExecute(RESET_COUNTERS_20, resetCounters);

#ifdef OPENWRT_BUILD
    // Load existing configuration from mwan3 if available
    loadFromMwan3();
    // Update interface status
    updateInterfaceStatus();
#endif

    return true;
}

/* Validation */
bool WanFailoverPolicy::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case PRIMARY_WAN_1:
        case SECONDARY_WAN_2:
        case TERTIARY_WAN_3: {
            const STRING_T& ifname = *(const STRING_T*)data;
            if (!ifname.empty() && !isValidInterface(ifname)) {
                WPP_LOGE(TAG, "Invalid interface name: %s", ifname.c_str());
                return false;
            }
            break;
        }
        case MODE_4: {
            INT_T mode = *(const INT_T*)data;
            if (mode < MODE_FAILOVER || mode > MODE_ROUNDROBIN) {
                WPP_LOGE(TAG, "Invalid failover mode: %lld", mode);
                return false;
            }
            break;
        }
        case PRIMARY_WEIGHT_5:
        case SECONDARY_WEIGHT_6:
        case TERTIARY_WEIGHT_7: {
            INT_T weight = *(const INT_T*)data;
            if (weight < 1 || weight > 100) {
                WPP_LOGE(TAG, "Invalid weight: %lld (must be 1-100)", weight);
                return false;
            }
            break;
        }
        case FAILOVER_THRESHOLD_8: {
            INT_T threshold = *(const INT_T*)data;
            if (threshold < 1 || threshold > 20) {
                WPP_LOGE(TAG, "Invalid failover threshold: %lld (must be 1-20)", threshold);
                return false;
            }
            break;
        }
        case FAILBACK_MODE_9: {
            INT_T mode = *(const INT_T*)data;
            if (mode < FAILBACK_IMMEDIATE || mode > FAILBACK_MANUAL) {
                WPP_LOGE(TAG, "Invalid failback mode: %lld", mode);
                return false;
            }
            break;
        }
        case FAILBACK_DELAY_10: {
            INT_T delay = *(const INT_T*)data;
            if (delay < 0 || delay > 3600) {
                WPP_LOGE(TAG, "Invalid failback delay: %lld (must be 0-3600 seconds)", delay);
                return false;
            }
            break;
        }
        case INTERFACE_RULES_16: {
            const STRING_T& rule = *(const STRING_T*)data;
            // Format: interface:protocol:port
            size_t colon1 = rule.find(':');
            size_t colon2 = rule.find(':', colon1 + 1);
            if (colon1 == std::string::npos || colon2 == std::string::npos) {
                WPP_LOGE(TAG, "Invalid interface rule format (expected interface:protocol:port): %s", rule.c_str());
                return false;
            }
            break;
        }
    }
    return true;
}

/* Execute handler: Apply Policy */
bool WanFailoverPolicy::applyPolicy(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WanFailoverPolicy& policy = static_cast<WanFailoverPolicy&>(inst);

    WPP_LOGI(TAG, "Applying WAN failover policy for instance %d", policy.instId());

    // Set state to inactive during configuration
    policy.set<INT_T>(POLICY_STATE_12, STATE_INACTIVE);

#ifdef OPENWRT_BUILD
    // Verify interfaces exist
    std::string primaryWan = policy.get<STRING_T>(PRIMARY_WAN_1);
    std::string secondaryWan = policy.get<STRING_T>(SECONDARY_WAN_2);

    if (!policy.isValidInterface(primaryWan)) {
        WPP_LOGE(TAG, "Primary WAN interface %s does not exist", primaryWan.c_str());
        policy.set<INT_T>(POLICY_STATE_12, STATE_ERROR);
        return false;
    }

    if (!secondaryWan.empty() && !policy.isValidInterface(secondaryWan)) {
        WPP_LOGE(TAG, "Secondary WAN interface %s does not exist", secondaryWan.c_str());
        policy.set<INT_T>(POLICY_STATE_12, STATE_ERROR);
        return false;
    }

    // Save to mwan3 configuration
    if (!policy.saveToMwan3()) {
        WPP_LOGE(TAG, "Failed to save WAN failover policy to mwan3");
        policy.set<INT_T>(POLICY_STATE_12, STATE_ERROR);
        return false;
    }

    // Setup health checks for interfaces
    if (!policy.setupHealthChecks()) {
        WPP_LOGE(TAG, "Failed to setup health checks");
        policy.set<INT_T>(POLICY_STATE_12, STATE_ERROR);
        return false;
    }

    // Configure policy-based routing
    if (!policy.configurePolicyRouting()) {
        WPP_LOGE(TAG, "Failed to configure policy routing");
        policy.set<INT_T>(POLICY_STATE_12, STATE_ERROR);
        return false;
    }

    // Apply load balancing weights if in load balance mode
    if (policy.get<INT_T>(MODE_4) == MODE_LOADBALANCE) {
        if (!policy.applyLoadBalancingWeights()) {
            WPP_LOGE(TAG, "Failed to apply load balancing weights");
            policy.set<INT_T>(POLICY_STATE_12, STATE_ERROR);
            return false;
        }
    }

    // Apply mwan3 configuration
    if (!policy.applyMwan3Config()) {
        WPP_LOGE(TAG, "Failed to apply mwan3 configuration");
        policy.set<INT_T>(POLICY_STATE_12, STATE_ERROR);
        return false;
    }

    // Update interface status and set active WAN
    policy.updateInterfaceStatus();

    // Set state to active
    policy.set<INT_T>(POLICY_STATE_12, STATE_ACTIVE);
    WPP_LOGI(TAG, "WAN failover policy applied successfully");

    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot apply WAN failover policy");
    policy.set<INT_T>(POLICY_STATE_12, STATE_ERROR);
    return false;
#endif
}

/* Execute handler: Force Failover */
bool WanFailoverPolicy::forceFailover(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WanFailoverPolicy& policy = static_cast<WanFailoverPolicy&>(inst);

    WPP_LOGI(TAG, "Forcing failover for instance %d", policy.instId());

#ifdef OPENWRT_BUILD
    std::string currentWan = policy.get<STRING_T>(CURRENT_ACTIVE_WAN_11);
    std::string primaryWan = policy.get<STRING_T>(PRIMARY_WAN_1);
    std::string secondaryWan = policy.get<STRING_T>(SECONDARY_WAN_2);
    std::string tertiaryWan = policy.get<STRING_T>(TERTIARY_WAN_3);

    // Determine next interface in failover order
    std::string nextWan;
    if (currentWan == primaryWan && !secondaryWan.empty()) {
        nextWan = secondaryWan;
    } else if (currentWan == secondaryWan && !tertiaryWan.empty()) {
        nextWan = tertiaryWan;
    } else if (!primaryWan.empty()) {
        nextWan = primaryWan;
    } else {
        WPP_LOGE(TAG, "No valid interface to failover to");
        return false;
    }

    if (!policy.triggerFailover(currentWan, nextWan)) {
        WPP_LOGE(TAG, "Failed to trigger failover from %s to %s", currentWan.c_str(), nextWan.c_str());
        return false;
    }

    WPP_LOGI(TAG, "Forced failover from %s to %s", currentWan.c_str(), nextWan.c_str());
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot force failover");
    return false;
#endif
}

/* Execute handler: Force Failback */
bool WanFailoverPolicy::forceFailback(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WanFailoverPolicy& policy = static_cast<WanFailoverPolicy&>(inst);

    WPP_LOGI(TAG, "Forcing failback for instance %d", policy.instId());

#ifdef OPENWRT_BUILD
    if (!policy.triggerFailback()) {
        WPP_LOGE(TAG, "Failed to trigger failback");
        return false;
    }

    WPP_LOGI(TAG, "Forced failback to primary WAN");
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot force failback");
    return false;
#endif
}

/* Execute handler: Reset Counters */
bool WanFailoverPolicy::resetCounters(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WanFailoverPolicy& policy = static_cast<WanFailoverPolicy&>(inst);

    WPP_LOGI(TAG, "Resetting counters for instance %d", policy.instId());

    policy.set<INT_T>(FAILOVER_COUNT_14, 0);
    policy.set<TIME_T>(LAST_FAILOVER_TIME_13, 0);

    WPP_LOGI(TAG, "Counters reset successfully");
    return true;
}

/* Load configuration from mwan3 */
bool WanFailoverPolicy::loadFromMwan3() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Loading WAN failover policy from mwan3");

    std::string policyName = getMwan3PolicyName();

    // Read policy members
    std::stringstream cmd;
    cmd << "uci get mwan3.policy_" << policyName << ".member 2>/dev/null";
    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            // Parse member list (space-separated)
            std::istringstream iss(buffer);
            std::string member;
            int idx = 0;
            while (iss >> member && idx < 3) {
                // Remove weight suffix if present (e.g., "wan_m1_w3" -> "wan")
                size_t pos = member.find("_m");
                if (pos != std::string::npos) {
                    member = member.substr(0, pos);
                }

                if (idx == 0) set<STRING_T>(PRIMARY_WAN_1, member);
                else if (idx == 1) set<STRING_T>(SECONDARY_WAN_2, member);
                else if (idx == 2) set<STRING_T>(TERTIARY_WAN_3, member);
                idx++;
            }
        }
        pclose(pipe);
    }

    // Update interface status
    updateInterfaceStatus();

    WPP_LOGD(TAG, "Loaded WAN failover policy from mwan3");
    return true;
#else
    return false;
#endif
}

/* Save configuration to mwan3 */
bool WanFailoverPolicy::saveToMwan3() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Saving WAN failover policy to mwan3");

    std::string policyName = getMwan3PolicyName();
    std::string primaryWan = get<STRING_T>(PRIMARY_WAN_1);
    std::string secondaryWan = get<STRING_T>(SECONDARY_WAN_2);
    std::string tertiaryWan = get<STRING_T>(TERTIARY_WAN_3);
    INT_T mode = get<INT_T>(MODE_4);
    INT_T threshold = get<INT_T>(FAILOVER_THRESHOLD_8);

    std::stringstream cmd;

    // Create or update interfaces in mwan3
    std::vector<std::string> interfaces = {primaryWan};
    if (!secondaryWan.empty()) interfaces.push_back(secondaryWan);
    if (!tertiaryWan.empty()) interfaces.push_back(tertiaryWan);

    for (const auto& ifname : interfaces) {
        // Create mwan3 interface section
        cmd.str("");
        cmd << "uci set mwan3." << ifname << "=interface";
        system(cmd.str().c_str());

        cmd.str("");
        cmd << "uci set mwan3." << ifname << ".enabled='1'";
        system(cmd.str().c_str());

        // Create member section for this interface
        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_m1=member";
        system(cmd.str().c_str());

        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_m1.interface='" << ifname << "'";
        system(cmd.str().c_str());

        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_m1.metric='1'";
        system(cmd.str().c_str());

        // Set weight based on mode and interface
        int weight = 1;
        if (mode == MODE_LOADBALANCE) {
            if (ifname == primaryWan) weight = get<INT_T>(PRIMARY_WEIGHT_5);
            else if (ifname == secondaryWan) weight = get<INT_T>(SECONDARY_WEIGHT_6);
            else if (ifname == tertiaryWan) weight = get<INT_T>(TERTIARY_WEIGHT_7);
        }

        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_m1.weight='" << weight << "'";
        system(cmd.str().c_str());
    }

    // Create policy
    cmd.str("");
    cmd << "uci set mwan3.policy_" << policyName << "=policy";
    system(cmd.str().c_str());

    // Set policy members
    std::string memberList;
    for (size_t i = 0; i < interfaces.size(); i++) {
        if (i > 0) memberList += " ";
        memberList += interfaces[i] + "_m1";
    }

    cmd.str("");
    cmd << "uci set mwan3.policy_" << policyName << ".member='" << memberList << "'";
    system(cmd.str().c_str());

    // Set last resort
    cmd.str("");
    cmd << "uci set mwan3.policy_" << policyName << ".last_resort='default'";
    system(cmd.str().c_str());

    // Create rule to use this policy
    cmd.str("");
    cmd << "uci set mwan3.rule_" << policyName << "=rule";
    system(cmd.str().c_str());

    cmd.str("");
    cmd << "uci set mwan3.rule_" << policyName << ".policy='policy_" << policyName << "'";
    system(cmd.str().c_str());

    cmd.str("");
    cmd << "uci set mwan3.rule_" << policyName << ".sticky='"
        << (get<BOOL_T>(STICKY_CONNECTIONS_15) ? "1" : "0") << "'";
    system(cmd.str().c_str());

    // Apply interface rules if any
    std::vector<STRING_T> rules;
    if (getMultiple(INTERFACE_RULES_16, rules)) {
        for (size_t i = 0; i < rules.size(); i++) {
            const auto& rule = rules[i];
            // Format: interface:protocol:port
            size_t colon1 = rule.find(':');
            size_t colon2 = rule.find(':', colon1 + 1);

            if (colon1 != std::string::npos && colon2 != std::string::npos) {
                std::string proto = rule.substr(colon1 + 1, colon2 - colon1 - 1);
                std::string port = rule.substr(colon2 + 1);

                cmd.str("");
                cmd << "uci set mwan3.rule_" << policyName << "_" << i << "=rule";
                system(cmd.str().c_str());

                cmd.str("");
                cmd << "uci set mwan3.rule_" << policyName << "_" << i << ".proto='" << proto << "'";
                system(cmd.str().c_str());

                cmd.str("");
                cmd << "uci set mwan3.rule_" << policyName << "_" << i << ".dest_port='" << port << "'";
                system(cmd.str().c_str());

                cmd.str("");
                cmd << "uci set mwan3.rule_" << policyName << "_" << i << ".policy='policy_" << policyName << "'";
                system(cmd.str().c_str());
            }
        }
    }

    // Commit mwan3 changes
    system("uci commit mwan3");

    WPP_LOGD(TAG, "Saved WAN failover policy to mwan3");
    return true;
#else
    return false;
#endif
}

/* Apply mwan3 configuration */
bool WanFailoverPolicy::applyMwan3Config() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Applying mwan3 configuration");

    // Restart mwan3 service
    int result = system("/etc/init.d/mwan3 restart");
    if (result != 0) {
        WPP_LOGE(TAG, "Failed to restart mwan3");
        return false;
    }

    return true;
#else
    return false;
#endif
}

/* Check interface status */
bool WanFailoverPolicy::checkInterfaceStatus(const std::string& ifname) {
#ifdef OPENWRT_BUILD
    // Check if interface is up using sysfs
    std::string path = "/sys/class/net/" + ifname + "/operstate";
    std::ifstream stateFile(path);

    if (!stateFile.is_open()) {
        return false;
    }

    std::string state;
    std::getline(stateFile, state);
    stateFile.close();

    return (state == "up" || state == "unknown");
#else
    return false;
#endif
}

/* Trigger failover */
bool WanFailoverPolicy::triggerFailover(const std::string& fromInterface, const std::string& toInterface) {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Triggering failover from %s to %s", fromInterface.c_str(), toInterface.c_str());

    // Update current active WAN
    set<STRING_T>(CURRENT_ACTIVE_WAN_11, toInterface);

    // Update failover time
    set<TIME_T>(LAST_FAILOVER_TIME_13, time(nullptr));

    // Increment failover count
    INT_T count = get<INT_T>(FAILOVER_COUNT_14);
    set<INT_T>(FAILOVER_COUNT_14, count + 1);

    // Use mwan3 to switch to backup interface
    std::stringstream cmd;
    cmd << "mwan3 use " << toInterface;
    int result = system(cmd.str().c_str());

    return (result == 0);
#else
    return false;
#endif
}

/* Trigger failback */
bool WanFailoverPolicy::triggerFailback() {
#ifdef OPENWRT_BUILD
    std::string primaryWan = get<STRING_T>(PRIMARY_WAN_1);

    WPP_LOGI(TAG, "Triggering failback to primary WAN: %s", primaryWan.c_str());

    // Check if primary interface is up
    if (!checkInterfaceStatus(primaryWan)) {
        WPP_LOGE(TAG, "Primary WAN %s is not available", primaryWan.c_str());
        return false;
    }

    return triggerFailover(get<STRING_T>(CURRENT_ACTIVE_WAN_11), primaryWan);
#else
    return false;
#endif
}

/* Update interface status */
void WanFailoverPolicy::updateInterfaceStatus() {
#ifdef OPENWRT_BUILD
    std::string activeWan = getActiveInterface();
    set<STRING_T>(CURRENT_ACTIVE_WAN_11, activeWan);
#endif
}

/* Get active interface */
std::string WanFailoverPolicy::getActiveInterface() {
#ifdef OPENWRT_BUILD
    std::string primaryWan = get<STRING_T>(PRIMARY_WAN_1);
    std::string secondaryWan = get<STRING_T>(SECONDARY_WAN_2);
    std::string tertiaryWan = get<STRING_T>(TERTIARY_WAN_3);

    // Check interfaces in priority order
    if (checkInterfaceStatus(primaryWan)) {
        return primaryWan;
    } else if (!secondaryWan.empty() && checkInterfaceStatus(secondaryWan)) {
        return secondaryWan;
    } else if (!tertiaryWan.empty() && checkInterfaceStatus(tertiaryWan)) {
        return tertiaryWan;
    }

    return "";
#else
    return "";
#endif
}

/* Validate interface name */
bool WanFailoverPolicy::isValidInterface(const std::string& ifname) {
    // Check if interface exists in /sys/class/net/
    std::string path = "/sys/class/net/" + ifname;
    std::ifstream test(path);
    return test.good();
}

/* Configure policy-based routing */
bool WanFailoverPolicy::configurePolicyRouting() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Configuring policy-based routing");

    std::string policyName = getMwan3PolicyName();
    std::stringstream cmd;

    // Create routing tables for each interface
    std::vector<std::string> interfaces = {get<STRING_T>(PRIMARY_WAN_1)};
    if (!get<STRING_T>(SECONDARY_WAN_2).empty())
        interfaces.push_back(get<STRING_T>(SECONDARY_WAN_2));
    if (!get<STRING_T>(TERTIARY_WAN_3).empty())
        interfaces.push_back(get<STRING_T>(TERTIARY_WAN_3));

    int tableId = 100;
    for (const auto& ifname : interfaces) {
        // Add routing table
        cmd.str("");
        cmd << "ip route add default dev " << ifname << " table " << tableId;
        system(cmd.str().c_str());

        // Add routing rule
        cmd.str("");
        cmd << "ip rule add from all iif " << ifname << " lookup " << tableId;
        system(cmd.str().c_str());

        tableId++;
    }

    return true;
#else
    return false;
#endif
}

/* Setup health checks */
bool WanFailoverPolicy::setupHealthChecks() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Setting up health checks");

    std::vector<std::string> interfaces = {get<STRING_T>(PRIMARY_WAN_1)};
    if (!get<STRING_T>(SECONDARY_WAN_2).empty())
        interfaces.push_back(get<STRING_T>(SECONDARY_WAN_2));
    if (!get<STRING_T>(TERTIARY_WAN_3).empty())
        interfaces.push_back(get<STRING_T>(TERTIARY_WAN_3));

    INT_T threshold = get<INT_T>(FAILOVER_THRESHOLD_8);

    std::stringstream cmd;
    for (const auto& ifname : interfaces) {
        // Create track_ip section for health monitoring
        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_track=track_ip";
        system(cmd.str().c_str());

        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_track.interface='" << ifname << "'";
        system(cmd.str().c_str());

        // Use Google DNS for health check
        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_track.reliability='1'";
        system(cmd.str().c_str());

        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_track.count='" << threshold << "'";
        system(cmd.str().c_str());

        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_track.timeout='4'";
        system(cmd.str().c_str());

        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_track.interval='10'";
        system(cmd.str().c_str());

        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_track.down='3'";
        system(cmd.str().c_str());

        cmd.str("");
        cmd << "uci set mwan3." << ifname << "_track.up='3'";
        system(cmd.str().c_str());
    }

    system("uci commit mwan3");
    return true;
#else
    return false;
#endif
}

/* Apply load balancing weights */
bool WanFailoverPolicy::applyLoadBalancingWeights() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Applying load balancing weights");

    int totalWeight = calculateTotalWeight();
    if (totalWeight == 0) {
        WPP_LOGE(TAG, "Total weight is zero");
        return false;
    }

    WPP_LOGD(TAG, "Total weight: %d", totalWeight);
    return true;
#else
    return false;
#endif
}

/* Calculate total weight */
int WanFailoverPolicy::calculateTotalWeight() {
    int total = get<INT_T>(PRIMARY_WEIGHT_5);

    if (!get<STRING_T>(SECONDARY_WAN_2).empty()) {
        total += get<INT_T>(SECONDARY_WEIGHT_6);
    }

    if (!get<STRING_T>(TERTIARY_WAN_3).empty()) {
        total += get<INT_T>(TERTIARY_WEIGHT_7);
    }

    return total;
}

/* Get mwan3 policy name */
std::string WanFailoverPolicy::getMwan3PolicyName() {
    std::string policyName = get<STRING_T>(POLICY_NAME_0);
    // Replace spaces and special characters with underscores
    std::replace(policyName.begin(), policyName.end(), ' ', '_');
    return policyName;
}

#endif // OBJ_W_34611_WAN_FAILOVER_POLICY
