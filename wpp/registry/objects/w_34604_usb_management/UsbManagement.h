/*
 * UsbManagement (Object 34604)
 * Walt Technologies - OpenWRT One Router
 *
 * USB port monitoring and control
 */

#ifndef WPP_W_34604_USB_MANAGEMENT_H
#define WPP_W_34604_USB_MANAGEMENT_H

#include "UsbManagementConfig.h"
#include "UsbManagementInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class UsbManagement : public Instance {
public:
    enum ID: ID_T {
        PORT_NAME_0 = 0,
        PORT_TYPE_1 = 1,
        PORT_ENABLED_2 = 2,
        DEVICE_CONNECTED_3 = 3,
        DEVICE_NAME_4 = 4,
        DEVICE_VENDOR_ID_5 = 5,
        DEVICE_PRODUCT_ID_6 = 6,
        DEVICE_CLASS_7 = 7,
        POWER_ENABLED_8 = 8,
        CURRENT_DRAW_9 = 9,
    };

    // Port Type Enumeration
    enum PortType: uint8_t {
        USB_2_0_TYPE_A = 0,
        USB_TYPE_C = 1
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId = 0);
    static bool remove(WppClient& client, INST_T instId = 0);

    /* Instance lifecycle */
    UsbManagement(Object& object, INST_T instId);
    ~UsbManagement() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;
};

} // namespace wpp

#endif // WPP_W_34604_USB_MANAGEMENT_H
