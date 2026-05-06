/*
 * FirewallConfig Implementation (Object 10521)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_10521_firewall_config/FirewallConfig.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"

#ifdef OBJ_W_10521_FIREWALL_CONFIG

#include <cstdio>
#include <cstring>
#include <sstream>

#define TAG "FirewallConfig"

using namespace wpp;

/* Static object methods */
Object& FirewallConfig::object(WppClient& client) {
    return client.registry().getObject(FIREWALL_CONFIG_OBJECT_ID);
}

Instance* FirewallConfig::createInst(WppClient& client, INST_T instId) {
    return object(client).createInstance(instId);
}

Instance* FirewallConfig::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool FirewallConfig::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

/* Instance lifecycle */
FirewallConfig::FirewallConfig(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "FirewallConfig instance %d created", instId);
}

FirewallConfig::~FirewallConfig() {
    WPP_LOGD(TAG, "FirewallConfig instance %d destroyed", instId());
}

/* Initialize resources */
bool FirewallConfig::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing FirewallConfig resources for instance %d", instId());

    // Default values for a firewall rule
    set<STRING_T>(RULE_NAME_0, "rule_" + std::to_string(instId()));
    set<INT_T>(RULE_INDEX_1, static_cast<INT_T>(instId()));
    set<BOOL_T>(ENABLED_2, true);
    set<INT_T>(ACTION_3, ACTION_ACCEPT);
    set<INT_T>(PROTOCOL_4, PROTOCOL_ALL);
    set<STRING_T>(SOURCE_IP_5, "0.0.0.0/0");
    set<INT_T>(SOURCE_PORT_6, 0);  // 0 = any port
    set<STRING_T>(DEST_IP_7, "0.0.0.0/0");
    set<INT_T>(DEST_PORT_8, 0);    // 0 = any port
    set<STRING_T>(INTERFACE_9, "");
    set<INT_T>(DIRECTION_10, DIRECTION_FORWARD);
    set<STRING_T>(ZONE_FROM_11, "wan");
    set<STRING_T>(ZONE_TO_12, "lan");
    set<STRING_T>(COMMENT_13, "");

    // Set execute handlers
    setExecute(APPLY_RULE_14, applyRule);
    setExecute(DELETE_RULE_15, deleteRule);

#ifdef OPENWRT_BUILD
    // Load existing configuration from UCI if available
    loadFromUCI();
#endif

    return true;
}

/* Validation */
bool FirewallConfig::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case ACTION_3: {
            INT_T action = *(const INT_T*)data;
            if (action < ACTION_ACCEPT || action > ACTION_DROP) {
                WPP_LOGE(TAG, "Invalid firewall action: %lld", action);
                return false;
            }
            break;
        }
        case PROTOCOL_4: {
            INT_T protocol = *(const INT_T*)data;
            if (protocol < PROTOCOL_ALL || protocol > PROTOCOL_ICMP) {
                WPP_LOGE(TAG, "Invalid protocol: %lld", protocol);
                return false;
            }
            break;
        }
        case DIRECTION_10: {
            INT_T direction = *(const INT_T*)data;
            if (direction < DIRECTION_INPUT || direction > DIRECTION_FORWARD) {
                WPP_LOGE(TAG, "Invalid direction: %lld", direction);
                return false;
            }
            break;
        }
    }
    return true;
}

/* Execute handler: Apply Rule */
bool FirewallConfig::applyRule(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    FirewallConfig& fw = static_cast<FirewallConfig&>(inst);

    WPP_LOGI(TAG, "Applying firewall rule for instance %d", fw.instId());

#ifdef OPENWRT_BUILD
    // Save to UCI configuration
    if (!fw.saveToUCI()) {
        WPP_LOGE(TAG, "Failed to save firewall rule to UCI");
        return false;
    }

    // Apply firewall rules
    if (!fw.applyFirewallRules()) {
        WPP_LOGE(TAG, "Failed to apply firewall rules");
        return false;
    }

    WPP_LOGI(TAG, "Firewall rule applied successfully");
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot apply firewall rule");
    return false;
#endif
}

/* Execute handler: Delete Rule */
bool FirewallConfig::deleteRule(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    FirewallConfig& fw = static_cast<FirewallConfig&>(inst);

    WPP_LOGI(TAG, "Deleting firewall rule for instance %d", fw.instId());

#ifdef OPENWRT_BUILD
    // Build UCI delete command
    std::stringstream cmd;
    cmd << "uci delete firewall.@rule[" << fw.instId() << "] 2>/dev/null && "
        << "uci commit firewall && "
        << "/etc/init.d/firewall reload";

    int result = system(cmd.str().c_str());
    if (result == 0) {
        WPP_LOGI(TAG, "Firewall rule deleted successfully");
        return true;
    } else {
        WPP_LOGE(TAG, "Failed to delete firewall rule");
        return false;
    }
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot delete firewall rule");
    return false;
#endif
}

/* Load configuration from UCI */
bool FirewallConfig::loadFromUCI() {
#ifdef OPENWRT_BUILD
    std::stringstream cmd;
    std::string ruleName = "rule_" + std::to_string(instId());

    // Try to read rule name from UCI
    cmd << "uci get firewall.@rule[" << instId() << "].name 2>/dev/null";
    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            set<STRING_T>(RULE_NAME_0, buffer);
        }
        pclose(pipe);
    }

    // Read enabled status
    cmd.str("");
    cmd << "uci get firewall.@rule[" << instId() << "].enabled 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[16];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            set<BOOL_T>(ENABLED_2, (buffer[0] == '1'));
        }
        pclose(pipe);
    }

    // Read target (action)
    cmd.str("");
    cmd << "uci get firewall.@rule[" << instId() << "].target 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[32];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strcmp(buffer, "ACCEPT") == 0) {
                set<INT_T>(ACTION_3, ACTION_ACCEPT);
            } else if (strcmp(buffer, "REJECT") == 0) {
                set<INT_T>(ACTION_3, ACTION_REJECT);
            } else if (strcmp(buffer, "DROP") == 0) {
                set<INT_T>(ACTION_3, ACTION_DROP);
            }
        }
        pclose(pipe);
    }

    WPP_LOGD(TAG, "Loaded firewall rule from UCI for instance %d", instId());
    return true;
#else
    return false;
#endif
}

/* Save configuration to UCI */
bool FirewallConfig::saveToUCI() {
#ifdef OPENWRT_BUILD
    std::stringstream cmd;
    INST_T ruleIdx = instId();

    // Check if rule exists, if not create it
    cmd << "uci get firewall.@rule[" << ruleIdx << "] 2>/dev/null || uci add firewall rule";
    system(cmd.str().c_str());

    // Set rule name
    cmd.str("");
    cmd << "uci set firewall.@rule[" << ruleIdx << "].name='"
        << get<STRING_T>(RULE_NAME_0) << "'";
    system(cmd.str().c_str());

    // Set enabled status
    cmd.str("");
    cmd << "uci set firewall.@rule[" << ruleIdx << "].enabled='"
        << (get<BOOL_T>(ENABLED_2) ? "1" : "0") << "'";
    system(cmd.str().c_str());

    // Set action/target
    cmd.str("");
    cmd << "uci set firewall.@rule[" << ruleIdx << "].target='";
    switch (get<INT_T>(ACTION_3)) {
        case ACTION_ACCEPT: cmd << "ACCEPT"; break;
        case ACTION_REJECT: cmd << "REJECT"; break;
        case ACTION_DROP: cmd << "DROP"; break;
    }
    cmd << "'";
    system(cmd.str().c_str());

    // Set protocol
    if (get<INT_T>(PROTOCOL_4) != PROTOCOL_ALL) {
        cmd.str("");
        cmd << "uci set firewall.@rule[" << ruleIdx << "].proto='";
        switch (get<INT_T>(PROTOCOL_4)) {
            case PROTOCOL_TCP: cmd << "tcp"; break;
            case PROTOCOL_UDP: cmd << "udp"; break;
            case PROTOCOL_ICMP: cmd << "icmp"; break;
        }
        cmd << "'";
        system(cmd.str().c_str());
    }

    // Set source IP
    std::string srcIp = get<STRING_T>(SOURCE_IP_5);
    if (!srcIp.empty() && srcIp != "0.0.0.0/0") {
        cmd.str("");
        cmd << "uci set firewall.@rule[" << ruleIdx << "].src_ip='" << srcIp << "'";
        system(cmd.str().c_str());
    }

    // Set destination IP
    std::string destIp = get<STRING_T>(DEST_IP_7);
    if (!destIp.empty() && destIp != "0.0.0.0/0") {
        cmd.str("");
        cmd << "uci set firewall.@rule[" << ruleIdx << "].dest_ip='" << destIp << "'";
        system(cmd.str().c_str());
    }

    // Set source port
    INT_T srcPort = get<INT_T>(SOURCE_PORT_6);
    if (srcPort > 0) {
        cmd.str("");
        cmd << "uci set firewall.@rule[" << ruleIdx << "].src_port='" << srcPort << "'";
        system(cmd.str().c_str());
    }

    // Set destination port
    INT_T destPort = get<INT_T>(DEST_PORT_8);
    if (destPort > 0) {
        cmd.str("");
        cmd << "uci set firewall.@rule[" << ruleIdx << "].dest_port='" << destPort << "'";
        system(cmd.str().c_str());
    }

    // Set source zone
    cmd.str("");
    cmd << "uci set firewall.@rule[" << ruleIdx << "].src='"
        << get<STRING_T>(ZONE_FROM_11) << "'";
    system(cmd.str().c_str());

    // Set destination zone
    cmd.str("");
    cmd << "uci set firewall.@rule[" << ruleIdx << "].dest='"
        << get<STRING_T>(ZONE_TO_12) << "'";
    system(cmd.str().c_str());

    // Commit changes
    system("uci commit firewall");

    WPP_LOGD(TAG, "Saved firewall rule to UCI for instance %d", instId());
    return true;
#else
    return false;
#endif
}

/* Apply firewall rules */
bool FirewallConfig::applyFirewallRules() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Reloading firewall to apply rules");
    int result = system("/etc/init.d/firewall reload");
    return (result == 0);
#else
    return false;
#endif
}

/* Build iptables rule string (for logging/debugging) */
std::string FirewallConfig::buildIptablesRule() {
    std::stringstream rule;

    rule << "iptables -A ";

    // Chain based on direction
    switch (get<INT_T>(DIRECTION_10)) {
        case DIRECTION_INPUT:   rule << "INPUT"; break;
        case DIRECTION_OUTPUT:  rule << "OUTPUT"; break;
        case DIRECTION_FORWARD: rule << "FORWARD"; break;
    }

    // Protocol
    if (get<INT_T>(PROTOCOL_4) != PROTOCOL_ALL) {
        rule << " -p ";
        switch (get<INT_T>(PROTOCOL_4)) {
            case PROTOCOL_TCP: rule << "tcp"; break;
            case PROTOCOL_UDP: rule << "udp"; break;
            case PROTOCOL_ICMP: rule << "icmp"; break;
        }
    }

    // Source IP
    std::string srcIp = get<STRING_T>(SOURCE_IP_5);
    if (!srcIp.empty() && srcIp != "0.0.0.0/0") {
        rule << " -s " << srcIp;
    }

    // Destination IP
    std::string destIp = get<STRING_T>(DEST_IP_7);
    if (!destIp.empty() && destIp != "0.0.0.0/0") {
        rule << " -d " << destIp;
    }

    // Source port
    INT_T srcPort = get<INT_T>(SOURCE_PORT_6);
    if (srcPort > 0) {
        rule << " --sport " << srcPort;
    }

    // Destination port
    INT_T destPort = get<INT_T>(DEST_PORT_8);
    if (destPort > 0) {
        rule << " --dport " << destPort;
    }

    // Interface
    std::string iface = get<STRING_T>(INTERFACE_9);
    if (!iface.empty()) {
        rule << " -i " << iface;
    }

    // Action
    rule << " -j ";
    switch (get<INT_T>(ACTION_3)) {
        case ACTION_ACCEPT: rule << "ACCEPT"; break;
        case ACTION_REJECT: rule << "REJECT"; break;
        case ACTION_DROP: rule << "DROP"; break;
    }

    return rule.str();
}

#endif // OBJ_W_10521_FIREWALL_CONFIG
