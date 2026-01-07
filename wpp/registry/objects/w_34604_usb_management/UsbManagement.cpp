/*
 * UsbManagement (Object 34604)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_34604_usb_management/UsbManagement.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

#define TAG "UsbManagement"

namespace wpp {

UsbManagement::UsbManagement(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "Creating UsbManagement instance %d", instId);
}

UsbManagement::~UsbManagement() {
    WPP_LOGD(TAG, "Destroying UsbManagement instance");
}

Object& UsbManagement::object(WppClient& client) {
    return client.registry().usbManagement();
}

Instance* UsbManagement::instance(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().usbManagement().instance(instId);
    if (!inst) return NULL;
    return static_cast<UsbManagement*>(inst);
}

Instance* UsbManagement::createInst(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().usbManagement().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<UsbManagement*>(inst);
}

bool UsbManagement::remove(WppClient& client, INST_T instId) {
    return client.registry().usbManagement().remove(instId);
}

bool UsbManagement::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing resources");

    std::vector<Resource> resources = {
        {PORT_NAME_0,        ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {PORT_TYPE_1,        ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {PORT_ENABLED_2,     ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {DEVICE_CONNECTED_3, ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {DEVICE_NAME_4,      ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {DEVICE_VENDOR_ID_5, ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {DEVICE_PRODUCT_ID_6,ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {DEVICE_CLASS_7,     ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {POWER_ENABLED_8,    ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
        {CURRENT_DRAW_9,     ItemOp(ItemOp::READ),                 IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
    };

    setupResources(std::move(resources));

    // Set default values
    resource(PORT_NAME_0)->set<STRING_T>("USB1");
    resource(PORT_TYPE_1)->set<INT_T>(USB_2_0_TYPE_A);
    resource(PORT_ENABLED_2)->set<BOOL_T>(true);
    resource(DEVICE_CONNECTED_3)->set<BOOL_T>(false);
    resource(DEVICE_NAME_4)->set<STRING_T>("");
    resource(DEVICE_VENDOR_ID_5)->set<STRING_T>("0000");
    resource(DEVICE_PRODUCT_ID_6)->set<STRING_T>("0000");
    resource(DEVICE_CLASS_7)->set<INT_T>(0);
    resource(POWER_ENABLED_8)->set<BOOL_T>(true);
    resource(CURRENT_DRAW_9)->set<INT_T>(0);

    // Set validators
    resource(PORT_TYPE_1)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value == USB_2_0_TYPE_A || value == USB_TYPE_C;
        }
    );

    resource(CURRENT_DRAW_9)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= 0 && value <= 3000; // 0 to 3000 mA
        }
    );

    return true;
}

bool UsbManagement::validate(ID_T resId, const void *data, size_t size) {
    return true;
}

} // namespace wpp
