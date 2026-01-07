/*
 * GpioControl (Object 34603)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_34603_gpio_control/GpioControl.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

#define TAG "GpioControl"

namespace wpp {

GpioControl::GpioControl(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "Creating GpioControl instance %d", instId);
}

GpioControl::~GpioControl() {
    WPP_LOGD(TAG, "Destroying GpioControl instance");
}

Object& GpioControl::object(WppClient& client) {
    return client.registry().gpioControl();
}

Instance* GpioControl::instance(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().gpioControl().instance(instId);
    if (!inst) return NULL;
    return static_cast<GpioControl*>(inst);
}

Instance* GpioControl::createInst(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().gpioControl().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<GpioControl*>(inst);
}

bool GpioControl::remove(WppClient& client, INST_T instId) {
    return client.registry().gpioControl().remove(instId);
}

bool GpioControl::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing resources");

    std::vector<Resource> resources = {
        {GPIO_NAME_0,           ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {GPIO_NUMBER_1,         ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {GPIO_TYPE_2,           ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {CURRENT_STATE_3,       ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
        {BLINK_ENABLED_4,       ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
        {BLINK_INTERVAL_5,      ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {TRIGGER_MODE_6,        ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {BUTTON_STATE_7,        ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
        {BUTTON_PRESS_COUNT_8,  ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
    };

    setupResources(std::move(resources));

    // Set default values
    resource(GPIO_NAME_0)->set<STRING_T>("LED_STATUS");
    resource(GPIO_NUMBER_1)->set<INT_T>(0);
    resource(GPIO_TYPE_2)->set<INT_T>(TYPE_LED);
    resource(CURRENT_STATE_3)->set<BOOL_T>(false);
    resource(BLINK_ENABLED_4)->set<BOOL_T>(false);
    resource(BLINK_INTERVAL_5)->set<INT_T>(500); // milliseconds
    resource(TRIGGER_MODE_6)->set<INT_T>(TRIGGER_NONE);
    resource(BUTTON_STATE_7)->set<BOOL_T>(false);
    resource(BUTTON_PRESS_COUNT_8)->set<INT_T>(0);

    // Set validators
    resource(GPIO_TYPE_2)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= TYPE_LED && value <= TYPE_GENERAL_GPIO;
        }
    );

    resource(TRIGGER_MODE_6)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= TRIGGER_NONE && value <= TRIGGER_DEFAULT_ON;
        }
    );

    resource(BLINK_INTERVAL_5)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= 50 && value <= 10000; // 50ms to 10 seconds
        }
    );

    return true;
}

bool GpioControl::validate(ID_T resId, const void *data, size_t size) {
    return true;
}

} // namespace wpp
