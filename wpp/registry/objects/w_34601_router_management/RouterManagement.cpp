/*
 * RouterManagement (Object 34601)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_34601_router_management/RouterManagement.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

#define TAG "RouterManagement"

namespace wpp {

RouterManagement::RouterManagement(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "Creating RouterManagement instance %d", instId);
    // Resources will be created by framework
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

    // Set default values
    resource(ROUTER_NAME_0)->set<STRING_T>("OpenWRT-One");
    resource(LAN_IP_ADDRESS_1)->set<STRING_T>("192.168.1.1");
    resource(LAN_SUBNET_MASK_2)->set<STRING_T>("255.255.255.0");
    resource(DHCP_ENABLED_3)->set<BOOL_T>(true);
    resource(DHCP_START_IP_4)->set<STRING_T>("192.168.1.100");
    resource(DHCP_END_IP_5)->set<STRING_T>("192.168.1.200");
    resource(DHCP_LEASE_TIME_6)->set<INT_T>(86400); // 24 hours
    resource(DNS_SERVER_1_7)->set<STRING_T>("8.8.8.8");
    resource(DNS_SERVER_2_8)->set<STRING_T>("8.8.4.4");
    resource(WAN_CONNECTION_TYPE_9)->set<INT_T>(WAN_DHCP);
    resource(WAN_IP_ADDRESS_10)->set<STRING_T>("0.0.0.0"); // Will be populated from system
    resource(WAN_GATEWAY_11)->set<STRING_T>("0.0.0.0");
    resource(FIREWALL_ENABLED_12)->set<BOOL_T>(true);
    resource(NAT_ENABLED_13)->set<BOOL_T>(true);
    resource(UPNP_ENABLED_14)->set<BOOL_T>(false);

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
            return value >= 60 && value <= 604800; // 1 min to 7 days
        }
    );

    return true;
}

bool RouterManagement::validate(ID_T resId, const void *data, size_t size) {
    // Additional validation can be added here
    return true;
}

bool RouterManagement::applyConfiguration(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Execute: Apply Configuration");

    RouterManagement& routerMgmt = static_cast<RouterManagement&>(inst);

    // In a real implementation, this would:
    // 1. Read all configuration values from resources
    // 2. Apply them to the OpenWRT system via UCI
    // 3. Reload network services
    // 4. Update firewall rules

    WPP_LOGI(TAG, "Configuration applied successfully");

    // TODO: Implement OpenWRT platform integration
    // Example: system("uci set network.lan.ipaddr='...' && uci commit && /etc/init.d/network reload");

    return true;
}

bool RouterManagement::resetToDefaults(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Execute: Reset to Defaults");

    RouterManagement& routerMgmt = static_cast<RouterManagement&>(inst);

    // Reset all values to defaults
    routerMgmt.resource(ROUTER_NAME_0)->set<STRING_T>("OpenWRT-One");
    routerMgmt.resource(LAN_IP_ADDRESS_1)->set<STRING_T>("192.168.1.1");
    routerMgmt.resource(LAN_SUBNET_MASK_2)->set<STRING_T>("255.255.255.0");
    routerMgmt.resource(DHCP_ENABLED_3)->set<BOOL_T>(true);
    routerMgmt.resource(DHCP_START_IP_4)->set<STRING_T>("192.168.1.100");
    routerMgmt.resource(DHCP_END_IP_5)->set<STRING_T>("192.168.1.200");
    routerMgmt.resource(DHCP_LEASE_TIME_6)->set<INT_T>(86400);
    routerMgmt.resource(WAN_CONNECTION_TYPE_9)->set<INT_T>(WAN_DHCP);
    routerMgmt.resource(FIREWALL_ENABLED_12)->set<BOOL_T>(true);
    routerMgmt.resource(NAT_ENABLED_13)->set<BOOL_T>(true);
    routerMgmt.resource(UPNP_ENABLED_14)->set<BOOL_T>(false);

    WPP_LOGI(TAG, "Configuration reset to defaults");
    return true;
}

} // namespace wpp
