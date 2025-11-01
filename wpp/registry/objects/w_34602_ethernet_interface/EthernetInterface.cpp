/*
 * EthernetInterface (Object 34602)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_34602_ethernet_interface/EthernetInterface.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

#define TAG "EthernetInterface"

namespace wpp {

EthernetInterface::EthernetInterface(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "Creating EthernetInterface instance %d", instId);
}

EthernetInterface::~EthernetInterface() {
    WPP_LOGD(TAG, "Destroying EthernetInterface instance");
}

Object& EthernetInterface::object(WppClient& client) {
    return client.registry().ethernetInterface();
}

Instance* EthernetInterface::instance(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().ethernetInterface().instance(instId);
    if (!inst) return NULL;
    return static_cast<EthernetInterface*>(inst);
}

Instance* EthernetInterface::createInst(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().ethernetInterface().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<EthernetInterface*>(inst);
}

bool EthernetInterface::remove(WppClient& client, INST_T instId) {
    return client.registry().ethernetInterface().remove(instId);
}

bool EthernetInterface::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing resources");

    std::vector<Resource> resources = {
        {INTERFACE_NAME_0,        ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {PORT_TYPE_1,             ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {SPEED_2,                 ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {DUPLEX_3,                ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {LINK_STATUS_4,           ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {MAC_ADDRESS_5,           ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {MTU_6,                   ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TOTAL_BYTES_SENT_7,      ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TOTAL_BYTES_RECEIVED_8,  ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TOTAL_PACKETS_SENT_9,    ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TOTAL_PACKETS_RECEIVED_10, ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TRANSMIT_ERRORS_11,      ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {RECEIVE_ERRORS_12,       ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {INTERFACE_ENABLED_13,    ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
    };

    setupResources(std::move(resources));

    // Set default values (these would be populated from system in real implementation)
    resource(INTERFACE_NAME_0)->set<STRING_T>("eth0");
    resource(PORT_TYPE_1)->set<INT_T>(PORT_WAN);
    resource(SPEED_2)->set<INT_T>(2500); // 2.5 Gbps for WAN
    resource(DUPLEX_3)->set<INT_T>(FULL_DUPLEX);
    resource(LINK_STATUS_4)->set<BOOL_T>(false);
    resource(MAC_ADDRESS_5)->set<STRING_T>("00:00:00:00:00:00");
    resource(MTU_6)->set<INT_T>(1500);
    resource(TOTAL_BYTES_SENT_7)->set<INT_T>(0);
    resource(TOTAL_BYTES_RECEIVED_8)->set<INT_T>(0);
    resource(TOTAL_PACKETS_SENT_9)->set<INT_T>(0);
    resource(TOTAL_PACKETS_RECEIVED_10)->set<INT_T>(0);
    resource(TRANSMIT_ERRORS_11)->set<INT_T>(0);
    resource(RECEIVE_ERRORS_12)->set<INT_T>(0);
    resource(INTERFACE_ENABLED_13)->set<BOOL_T>(true);

    // Set validators
    resource(PORT_TYPE_1)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value == PORT_LAN || value == PORT_WAN;
        }
    );

    resource(DUPLEX_3)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value == HALF_DUPLEX || value == FULL_DUPLEX;
        }
    );

    resource(MTU_6)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= 68 && value <= 9000; // Min IP MTU to jumbo frames
        }
    );

    return true;
}

bool EthernetInterface::validate(ID_T resId, const void *data, size_t size) {
    return true;
}

} // namespace wpp
