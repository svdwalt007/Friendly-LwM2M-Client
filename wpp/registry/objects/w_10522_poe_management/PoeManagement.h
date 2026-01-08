/*
 * PoeManagement (Object 10522)
 * Walt Technologies - OpenWRT One Router
 *
 * Power over Ethernet (PoE) monitoring and control
 */

#ifndef WPP_W_10522_POE_MANAGEMENT_H
#define WPP_W_10522_POE_MANAGEMENT_H

#include "PoeManagementConfig.h"
#include "PoeManagementInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class PoeManagement : public Instance {
public:
    enum ID: ID_T {
        POE_SUPPORTED_0 = 0,        // PoE supported (R, Boolean)
        POE_ENABLED_1 = 1,          // PoE enabled (RW, Boolean)
        INPUT_VOLTAGE_2 = 2,        // Input voltage in volts (R, Float)
        INPUT_CURRENT_3 = 3,        // Input current in amperes (R, Float)
        INPUT_POWER_4 = 4,          // Input power in watts (R, Float)
        POE_STANDARD_5 = 5,         // PoE standard (R, String)
        POE_CLASS_6 = 6,            // PoE class 0-8 (R, Integer)
        MAX_POWER_7 = 7,            // Maximum power available (R, Float)
        POWER_ALLOCATED_8 = 8,      // Power currently allocated (R, Float)
        TEMPERATURE_9 = 9,          // PoE controller temperature (R, Float)
        STATUS_10 = 10,             // Status string (R, String)
        RESET_POE_11 = 11,          // Reset/power cycle PoE (E, Execute)
    };

    // PoE Class Enumeration (802.3af/at/bt)
    enum PoeClass: uint8_t {
        CLASS_0 = 0,  // Default, 15.4W (af) or 12.95W (at)
        CLASS_1 = 1,  // 4.0W (af) or 3.84W (at)
        CLASS_2 = 2,  // 7.0W (af) or 6.49W (at)
        CLASS_3 = 3,  // 15.4W (af) or 12.95W (at)
        CLASS_4 = 4,  // 30W (at)
        CLASS_5 = 5,  // 45W (bt Type 3)
        CLASS_6 = 6,  // 60W (bt Type 3)
        CLASS_7 = 7,  // 75W (bt Type 4)
        CLASS_8 = 8,  // 90W+ (bt Type 4)
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = 0);
    static Instance* instance(WppClient& client, INST_T instId = 0);
    static bool remove(WppClient& client, INST_T instId = 0);

    /* Instance lifecycle */
    PoeManagement(Object& object, INST_T instId);
    ~PoeManagement() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool resetPoe(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    void updatePoeStatus();
    bool detectPoeSupport();
    float readVoltage();
    float readCurrent();
    float readTemperature();
    std::string getPoeStandard();

    /* Task ID for periodic updates */
    void* _updateTaskId;
};

} // namespace wpp

#endif // WPP_W_10522_POE_MANAGEMENT_H
