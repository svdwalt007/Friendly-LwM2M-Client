/*
 * SystemMonitor (Object 34606)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_34606_system_monitor/SystemMonitor.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

#define TAG "SystemMonitor"

namespace wpp {

SystemMonitor::SystemMonitor(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "Creating SystemMonitor instance %d", instId);
}

SystemMonitor::~SystemMonitor() {
    WPP_LOGD(TAG, "Destroying SystemMonitor instance");
}

Object& SystemMonitor::object(WppClient& client) {
    return client.registry().systemMonitor();
}

Instance* SystemMonitor::instance(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().systemMonitor().instance(instId);
    if (!inst) return NULL;
    return static_cast<SystemMonitor*>(inst);
}

Instance& SystemMonitor::createInst(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().systemMonitor().createInstance(instId);
    if (!inst) {
        WPP_LOGE(TAG, "Failed to create SystemMonitor instance");
        throw std::runtime_error("Failed to create SystemMonitor instance");
    }
    return *static_cast<SystemMonitor*>(inst);
}

bool SystemMonitor::remove(WppClient& client, INST_T instId) {
    return client.registry().systemMonitor().remove(instId);
}

bool SystemMonitor::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing resources");

    std::vector<Resource> resources = {
        {CPU_USAGE_PERCENTAGE_0, ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {CPU_FREQUENCY_1,        ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {CPU_TEMPERATURE_2,      ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
        {RAM_TOTAL_3,            ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {RAM_USED_4,             ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {RAM_FREE_5,             ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {RAM_CACHED_6,           ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {RAM_BUFFERS_7,          ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {SWAP_TOTAL_8,           ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {SWAP_USED_9,            ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {LOAD_AVERAGE_1MIN_10,   ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {LOAD_AVERAGE_5MIN_11,   ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {LOAD_AVERAGE_15MIN_12,  ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {UPTIME_13,              ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {PROCESS_COUNT_14,       ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
    };

    setupResources(std::move(resources));

    // Set default values (these would be populated from /proc in real implementation)
    resource(CPU_USAGE_PERCENTAGE_0)->set<INT_T>(0);
    resource(CPU_FREQUENCY_1)->set<INT_T>(1300); // 1.3 GHz
    resource(CPU_TEMPERATURE_2)->set<FLOAT_T>(45.0); // Celsius
    resource(RAM_TOTAL_3)->set<INT_T>(1024*1024*1024); // 1 GB
    resource(RAM_USED_4)->set<INT_T>(0);
    resource(RAM_FREE_5)->set<INT_T>(1024*1024*1024);
    resource(RAM_CACHED_6)->set<INT_T>(0);
    resource(RAM_BUFFERS_7)->set<INT_T>(0);
    resource(SWAP_TOTAL_8)->set<INT_T>(0);
    resource(SWAP_USED_9)->set<INT_T>(0);
    resource(LOAD_AVERAGE_1MIN_10)->set<FLOAT_T>(0.0);
    resource(LOAD_AVERAGE_5MIN_11)->set<FLOAT_T>(0.0);
    resource(LOAD_AVERAGE_15MIN_12)->set<FLOAT_T>(0.0);
    resource(UPTIME_13)->set<INT_T>(0);
    resource(PROCESS_COUNT_14)->set<INT_T>(0);

    // Set validators
    resource(CPU_USAGE_PERCENTAGE_0)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= 0 && value <= 100;
        }
    );

    resource(CPU_FREQUENCY_1)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= 100 && value <= 2000; // 100 MHz to 2 GHz
        }
    );

    resource(CPU_TEMPERATURE_2)->setDataVerifier(
        (VERIFY_FLOAT_T)[](const FLOAT_T& value) {
            return value >= -40.0 && value <= 125.0; // Reasonable temperature range
        }
    );

    return true;
}

bool SystemMonitor::validate(ID_T resId, const void *data, size_t size) {
    return true;
}

} // namespace wpp
