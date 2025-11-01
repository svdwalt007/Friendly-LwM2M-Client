/*
 * GpioControl (Object 34603)
 * Walt Technologies - OpenWRT One Router
 *
 * LED and GPIO pin management
 */

#ifndef WPP_W_34603_GPIO_CONTROL_H
#define WPP_W_34603_GPIO_CONTROL_H

#include "GpioControlConfig.h"
#include "GpioControlInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class GpioControl : public Instance {
public:
    enum ID: ID_T {
        GPIO_NAME_0 = 0,
        GPIO_NUMBER_1 = 1,
        GPIO_TYPE_2 = 2,
        CURRENT_STATE_3 = 3,
        BLINK_ENABLED_4 = 4,
        BLINK_INTERVAL_5 = 5,
        TRIGGER_MODE_6 = 6,
        BUTTON_STATE_7 = 7,
        BUTTON_PRESS_COUNT_8 = 8,
    };

    // GPIO Type Enumeration
    enum GpioType: uint8_t {
        TYPE_LED = 0,
        TYPE_BUTTON = 1,
        TYPE_GENERAL_GPIO = 2
    };

    // Trigger Mode Enumeration
    enum TriggerMode: uint8_t {
        TRIGGER_NONE = 0,
        TRIGGER_NETDEV = 1,
        TRIGGER_TIMER = 2,
        TRIGGER_DEFAULT_ON = 3
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId = 0);
    static bool remove(WppClient& client, INST_T instId = 0);

    /* Instance lifecycle */
    GpioControl(Object& object, INST_T instId);
    ~GpioControl() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;
};

} // namespace wpp

#endif // WPP_W_34603_GPIO_CONTROL_H
