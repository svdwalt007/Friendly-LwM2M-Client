/*
 * RouterManagement (Object 10513)
 * Walt Technologies - OpenWRT One Router
 *
 * Full UCI integration for OpenWRT network configuration
 */

#include "w_10513_router_management/RouterManagement.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <array>
#include <memory>
#include <regex>
#include <map>
#include <algorithm>

#define TAG "RouterManagement"

namespace wpp {

// Helper function to execute shell commands and capture output
static std::string execCommand(const std::string& cmd) {
    std::array<char, 256> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    // Remove trailing newline
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

// Helper function to validate IPv4 address
static bool isValidIPv4(const std::string& ip) {
    std::regex ipv4_regex(
        "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    return std::regex_match(ip, ipv4_regex);
}

// Helper function to validate subnet mask
static bool isValidNetmask(const std::string& mask) {
    static const std::vector<std::string> validMasks = {
        "255.255.255.255", "255.255.255.254", "255.255.255.252",
        "255.255.255.248", "255.255.255.240", "255.255.255.224",
        "255.255.255.192", "255.255.255.128", "255.255.255.0",
        "255.255.254.0", "255.255.252.0", "255.255.248.0",
        "255.255.240.0", "255.255.224.0", "255.255.192.0",
        "255.255.128.0", "255.255.0.0", "255.254.0.0",
        "255.252.0.0", "255.248.0.0", "255.240.0.0",
        "255.224.0.0", "255.192.0.0", "255.128.0.0", "255.0.0.0"
    };
    return std::find(validMasks.begin(), validMasks.end(), mask) != validMasks.end();
}

RouterManagement::RouterManagement(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "Creating RouterManagement instance %d", instId);
}

RouterManagement::~RouterManagement() {
    WPP_LOGD(TAG, "Destroying RouterManagement instance");
}

Object& RouterManagement::object(WppClient& client) {
    return client.registry().routerManagement();
}

Instance* RouterManagement::instance(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().routerManagement().instance(instId);
    if (!inst) return NULL;
    return static_cast<RouterManagement*>(inst);
}

Instance* RouterManagement::createInst(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().routerManagement().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<RouterManagement*>(inst);
}

bool RouterManagement::remove(WppClient& client, INST_T instId) {
    return client.registry().routerManagement().remove(instId);
}

bool RouterManagement::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing resources");

    std::vector<Resource> resources = {
        {ROUTER_NAME_0,         ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {LAN_IP_ADDRESS_1,      ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {LAN_SUBNET_MASK_2,     ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {DHCP_ENABLED_3,        ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {DHCP_START_IP_4,       ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {DHCP_END_IP_5,         ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {DHCP_LEASE_TIME_6,     ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {DNS_SERVER_1_7,        ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {DNS_SERVER_2_8,        ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {WAN_CONNECTION_TYPE_9, ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {WAN_IP_ADDRESS_10,     ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {WAN_GATEWAY_11,        ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {FIREWALL_ENABLED_12,   ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {NAT_ENABLED_13,        ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {UPNP_ENABLED_14,       ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
        {APPLY_CONFIGURATION_15, ItemOp(ItemOp::EXECUTE),             IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::EXECUTE},
        {RESET_TO_DEFAULTS_16,  ItemOp(ItemOp::EXECUTE),              IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
    };

    setupResources(std::move(resources));

    // Read current configuration from UCI if available
    std::string routerName = execCommand("uci -q get system.@system[0].hostname 2>/dev/null");
    if (routerName.empty()) routerName = "OpenWRT-One";
    resource(ROUTER_NAME_0)->set<STRING_T>(routerName);

    std::string lanIp = execCommand("uci -q get network.lan.ipaddr 2>/dev/null");
    if (lanIp.empty()) lanIp = "192.168.1.1";
    resource(LAN_IP_ADDRESS_1)->set<STRING_T>(lanIp);

    std::string lanNetmask = execCommand("uci -q get network.lan.netmask 2>/dev/null");
    if (lanNetmask.empty()) lanNetmask = "255.255.255.0";
    resource(LAN_SUBNET_MASK_2)->set<STRING_T>(lanNetmask);

    // DHCP configuration
    std::string dhcpIgnore = execCommand("uci -q get dhcp.lan.ignore 2>/dev/null");
    bool dhcpEnabled = (dhcpIgnore != "1");
    resource(DHCP_ENABLED_3)->set<BOOL_T>(dhcpEnabled);

    std::string dhcpStart = execCommand("uci -q get dhcp.lan.start 2>/dev/null");
    if (dhcpStart.empty()) dhcpStart = "100";
    std::string baseIp = lanIp.substr(0, lanIp.rfind('.') + 1);
    resource(DHCP_START_IP_4)->set<STRING_T>(baseIp + dhcpStart);

    std::string dhcpLimit = execCommand("uci -q get dhcp.lan.limit 2>/dev/null");
    int startNum = std::atoi(dhcpStart.c_str());
    int limitNum = dhcpLimit.empty() ? 150 : std::atoi(dhcpLimit.c_str());
    resource(DHCP_END_IP_5)->set<STRING_T>(baseIp + std::to_string(startNum + limitNum - 1));

    std::string leaseTime = execCommand("uci -q get dhcp.lan.leasetime 2>/dev/null");
    INT_T leaseSeconds = 86400;
    if (!leaseTime.empty()) {
        char unit = leaseTime.back();
        int value = std::atoi(leaseTime.c_str());
        if (unit == 'h') leaseSeconds = value * 3600;
        else if (unit == 'd') leaseSeconds = value * 86400;
        else if (unit == 'm') leaseSeconds = value * 60;
        else leaseSeconds = value;
    }
    resource(DHCP_LEASE_TIME_6)->set<INT_T>(leaseSeconds);

    // DNS servers
    std::string dns1 = execCommand("uci -q get network.wan.dns 2>/dev/null | awk '{print $1}'");
    if (dns1.empty()) dns1 = "8.8.8.8";
    resource(DNS_SERVER_1_7)->set<STRING_T>(dns1);

    std::string dns2 = execCommand("uci -q get network.wan.dns 2>/dev/null | awk '{print $2}'");
    if (dns2.empty()) dns2 = "8.8.4.4";
    resource(DNS_SERVER_2_8)->set<STRING_T>(dns2);

    // WAN configuration
    std::string wanProto = execCommand("uci -q get network.wan.proto 2>/dev/null");
    INT_T wanType = WAN_DHCP;
    if (wanProto == "static") wanType = WAN_STATIC;
    else if (wanProto == "pppoe") wanType = WAN_PPPOE;
    resource(WAN_CONNECTION_TYPE_9)->set<INT_T>(wanType);

    // Get current WAN IP from interface
    std::string wanIp = execCommand("ip -4 addr show wan 2>/dev/null | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'");
    if (wanIp.empty()) wanIp = "0.0.0.0";
    resource(WAN_IP_ADDRESS_10)->set<STRING_T>(wanIp);

    std::string wanGateway = execCommand("ip route | grep default | grep wan | awk '{print $3}'");
    if (wanGateway.empty()) wanGateway = "0.0.0.0";
    resource(WAN_GATEWAY_11)->set<STRING_T>(wanGateway);

    // Firewall and services
    std::string fwEnabled = execCommand("uci -q get firewall.@defaults[0].syn_flood 2>/dev/null");
    resource(FIREWALL_ENABLED_12)->set<BOOL_T>(fwEnabled != "0");

    std::string natEnabled = execCommand("uci -q get firewall.@zone[1].masq 2>/dev/null");
    resource(NAT_ENABLED_13)->set<BOOL_T>(natEnabled != "0");

    std::string upnpEnabled = execCommand("uci -q get upnpd.config.enabled 2>/dev/null");
    resource(UPNP_ENABLED_14)->set<BOOL_T>(upnpEnabled == "1");

    // Set execute handlers
    resource(APPLY_CONFIGURATION_15)->set<EXECUTE_T>(applyConfiguration);
    resource(RESET_TO_DEFAULTS_16)->set<EXECUTE_T>(resetToDefaults);

    // Set validators
    resource(WAN_CONNECTION_TYPE_9)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= WAN_DHCP && value <= WAN_PPPOE;
        }
    );

    resource(DHCP_LEASE_TIME_6)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= 60 && value <= 604800;
        }
    );

    return true;
}

bool RouterManagement::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case LAN_IP_ADDRESS_1:
        case DHCP_START_IP_4:
        case DHCP_END_IP_5:
        case DNS_SERVER_1_7:
        case DNS_SERVER_2_8:
        case WAN_GATEWAY_11: {
            const STRING_T* ip = static_cast<const STRING_T*>(data);
            if (!ip->empty() && !isValidIPv4(*ip)) {
                WPP_LOGE(TAG, "Invalid IP address: %s", ip->c_str());
                return false;
            }
            break;
        }
        case LAN_SUBNET_MASK_2: {
            const STRING_T* mask = static_cast<const STRING_T*>(data);
            if (!isValidNetmask(*mask)) {
                WPP_LOGE(TAG, "Invalid subnet mask: %s", mask->c_str());
                return false;
            }
            break;
        }
        default:
            break;
    }
    return true;
}

bool RouterManagement::applyConfiguration(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Execute: Apply Configuration");

    RouterManagement& routerMgmt = static_cast<RouterManagement&>(inst);
    std::ostringstream uciCommands;
    bool success = true;

    // Get configuration values
    STRING_T routerName = routerMgmt.resource(ROUTER_NAME_0)->get<STRING_T>();
    STRING_T lanIp = routerMgmt.resource(LAN_IP_ADDRESS_1)->get<STRING_T>();
    STRING_T lanNetmask = routerMgmt.resource(LAN_SUBNET_MASK_2)->get<STRING_T>();
    BOOL_T dhcpEnabled = routerMgmt.resource(DHCP_ENABLED_3)->get<BOOL_T>();
    STRING_T dhcpStart = routerMgmt.resource(DHCP_START_IP_4)->get<STRING_T>();
    STRING_T dhcpEnd = routerMgmt.resource(DHCP_END_IP_5)->get<STRING_T>();
    INT_T leaseTime = routerMgmt.resource(DHCP_LEASE_TIME_6)->get<INT_T>();
    STRING_T dns1 = routerMgmt.resource(DNS_SERVER_1_7)->get<STRING_T>();
    STRING_T dns2 = routerMgmt.resource(DNS_SERVER_2_8)->get<STRING_T>();
    INT_T wanType = routerMgmt.resource(WAN_CONNECTION_TYPE_9)->get<INT_T>();
    STRING_T wanGateway = routerMgmt.resource(WAN_GATEWAY_11)->get<STRING_T>();
    BOOL_T firewallEnabled = routerMgmt.resource(FIREWALL_ENABLED_12)->get<BOOL_T>();
    BOOL_T natEnabled = routerMgmt.resource(NAT_ENABLED_13)->get<BOOL_T>();
    BOOL_T upnpEnabled = routerMgmt.resource(UPNP_ENABLED_14)->get<BOOL_T>();

    // Build UCI commands
    uciCommands << "uci set system.@system[0].hostname='" << routerName << "'; ";
    uciCommands << "uci set network.lan.ipaddr='" << lanIp << "'; ";
    uciCommands << "uci set network.lan.netmask='" << lanNetmask << "'; ";
    uciCommands << "uci set dhcp.lan.ignore='" << (dhcpEnabled ? "0" : "1") << "'; ";

    if (dhcpEnabled) {
        std::string baseIp = lanIp.substr(0, lanIp.rfind('.') + 1);
        int startOffset = 100;
        if (!dhcpStart.empty() && dhcpStart.find(baseIp) == 0) {
            startOffset = std::atoi(dhcpStart.substr(baseIp.length()).c_str());
        }
        int endOffset = 200;
        if (!dhcpEnd.empty() && dhcpEnd.find(baseIp) == 0) {
            endOffset = std::atoi(dhcpEnd.substr(baseIp.length()).c_str());
        }
        int limit = endOffset - startOffset + 1;
        if (limit < 1) limit = 150;

        uciCommands << "uci set dhcp.lan.start='" << startOffset << "'; ";
        uciCommands << "uci set dhcp.lan.limit='" << limit << "'; ";

        std::string leaseStr;
        if (leaseTime >= 86400 && leaseTime % 86400 == 0) {
            leaseStr = std::to_string(leaseTime / 86400) + "d";
        } else if (leaseTime >= 3600 && leaseTime % 3600 == 0) {
            leaseStr = std::to_string(leaseTime / 3600) + "h";
        } else {
            leaseStr = std::to_string(leaseTime);
        }
        uciCommands << "uci set dhcp.lan.leasetime='" << leaseStr << "'; ";
    }

    // DNS servers
    if (!dns1.empty() || !dns2.empty()) {
        std::string dnsServers;
        if (!dns1.empty()) dnsServers = dns1;
        if (!dns2.empty()) {
            if (!dnsServers.empty()) dnsServers += " ";
            dnsServers += dns2;
        }
        uciCommands << "uci set network.wan.dns='" << dnsServers << "'; ";
        uciCommands << "uci set network.wan.peerdns='0'; ";
    }

    // WAN connection type
    const char* wanProto;
    switch (wanType) {
        case WAN_STATIC: wanProto = "static"; break;
        case WAN_PPPOE: wanProto = "pppoe"; break;
        default: wanProto = "dhcp"; break;
    }
    uciCommands << "uci set network.wan.proto='" << wanProto << "'; ";

    if (wanType == WAN_STATIC && !wanGateway.empty() && wanGateway != "0.0.0.0") {
        uciCommands << "uci set network.wan.gateway='" << wanGateway << "'; ";
    }

    // Firewall configuration
    uciCommands << "uci set firewall.@defaults[0].syn_flood='" << (firewallEnabled ? "1" : "0") << "'; ";
    uciCommands << "uci set firewall.@defaults[0].forward='" << (firewallEnabled ? "REJECT" : "ACCEPT") << "'; ";
    uciCommands << "uci set firewall.@zone[1].masq='" << (natEnabled ? "1" : "0") << "'; ";
    uciCommands << "uci set upnpd.config.enabled='" << (upnpEnabled ? "1" : "0") << "'; ";

    // Commit UCI changes
    uciCommands << "uci commit system; uci commit network; uci commit dhcp; uci commit firewall; uci commit upnpd; ";

    std::string cmdStr = uciCommands.str();
    WPP_LOGD(TAG, "Executing UCI commands");

    int result = system(cmdStr.c_str());
    if (result != 0) {
        WPP_LOGE(TAG, "Failed to execute UCI commands (exit code: %d)", result);
        success = false;
    }

    // Reload services
    if (success) {
        WPP_LOGI(TAG, "Reloading network services...");
        system("/etc/init.d/network reload 2>/dev/null");
        system("/etc/init.d/dnsmasq reload 2>/dev/null");
        system("/etc/init.d/firewall reload 2>/dev/null");

        if (upnpEnabled) {
            system("/etc/init.d/miniupnpd enable 2>/dev/null; /etc/init.d/miniupnpd start 2>/dev/null");
        } else {
            system("/etc/init.d/miniupnpd stop 2>/dev/null; /etc/init.d/miniupnpd disable 2>/dev/null");
        }

        WPP_LOGI(TAG, "Configuration applied successfully");
    }

    return success;
}

bool RouterManagement::resetToDefaults(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Execute: Reset to Defaults");

    RouterManagement& routerMgmt = static_cast<RouterManagement&>(inst);

    routerMgmt.resource(ROUTER_NAME_0)->set<STRING_T>("OpenWRT-One");
    routerMgmt.resource(LAN_IP_ADDRESS_1)->set<STRING_T>("192.168.1.1");
    routerMgmt.resource(LAN_SUBNET_MASK_2)->set<STRING_T>("255.255.255.0");
    routerMgmt.resource(DHCP_ENABLED_3)->set<BOOL_T>(true);
    routerMgmt.resource(DHCP_START_IP_4)->set<STRING_T>("192.168.1.100");
    routerMgmt.resource(DHCP_END_IP_5)->set<STRING_T>("192.168.1.200");
    routerMgmt.resource(DHCP_LEASE_TIME_6)->set<INT_T>(86400);
    routerMgmt.resource(DNS_SERVER_1_7)->set<STRING_T>("8.8.8.8");
    routerMgmt.resource(DNS_SERVER_2_8)->set<STRING_T>("8.8.4.4");
    routerMgmt.resource(WAN_CONNECTION_TYPE_9)->set<INT_T>(WAN_DHCP);
    routerMgmt.resource(WAN_GATEWAY_11)->set<STRING_T>("0.0.0.0");
    routerMgmt.resource(FIREWALL_ENABLED_12)->set<BOOL_T>(true);
    routerMgmt.resource(NAT_ENABLED_13)->set<BOOL_T>(true);
    routerMgmt.resource(UPNP_ENABLED_14)->set<BOOL_T>(false);

    WPP_LOGI(TAG, "Configuration reset to defaults - call Apply Configuration to activate");
    return true;
}

} // namespace wpp
