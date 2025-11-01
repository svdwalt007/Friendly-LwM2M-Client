/*
 * SystemMonitor (Object 34606)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_W_34606_SYSTEM_MONITOR_H
#define WPP_W_34606_SYSTEM_MONITOR_H

#include "SystemMonitorConfig.h"
#include "SystemMonitorInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class SystemMonitor : public Instance {
public:
    enum ID: ID_T {
        CPU_USAGE_PERCENTAGE_0 = 0,
        CPU_FREQUENCY_1 = 1,
        CPU_TEMPERATURE_2 = 2,
        RAM_TOTAL_3 = 3,
        RAM_USED_4 = 4,
        RAM_FREE_5 = 5,
        RAM_CACHED_6 = 6,
        RAM_BUFFERS_7 = 7,
        SWAP_TOTAL_8 = 8,
        SWAP_USED_9 = 9,
        LOAD_AVERAGE_1MIN_10 = 10,
        LOAD_AVERAGE_5MIN_11 = 11,
        LOAD_AVERAGE_15MIN_12 = 12,
        UPTIME_13 = 13,
        PROCESS_COUNT_14 = 14,
    };

    static Object& object(WppClient& client);
    static Instance& createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId = 0);
    static bool remove(WppClient& client, INST_T instId = 0);

    SystemMonitor(Object& object, INST_T instId);
    ~SystemMonitor() override;

protected:
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    bool initResources(ItemOp *) override;
};

} // namespace wpp

#endif // WPP_W_34606_SYSTEM_MONITOR_H
