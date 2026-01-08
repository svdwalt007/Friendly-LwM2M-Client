/*
 * UsbManagement (Object 34604)
 * Walt Technologies - OpenWRT One Router
 *
 * USB port monitoring and control with Linux sysfs integration
 */

#ifndef WPP_W_34604_USB_MANAGEMENT_H
#define WPP_W_34604_USB_MANAGEMENT_H

#include "UsbManagementConfig.h"
#include "UsbManagementInfo.h"
#include "Instance.h"
#include "InstSubject.h"
#include "WppTaskQueue.h"

#include <string>
#include <vector>

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

    // USB device information structure
    struct UsbDeviceInfo {
        std::string devicePath;
        std::string vendorId;
        std::string productId;
        std::string manufacturer;
        std::string product;
        int deviceClass = 0;
        int maxPowerMa = 0;
        bool powerEnabled = true;
        bool authorized = true;
    };

    // sysfs helper functions
    std::string readSysfsFile(const std::string& path);
    bool writeSysfsFile(const std::string& path, const std::string& value);
    std::vector<std::string> listUsbDevices();
    UsbDeviceInfo getDeviceInfo(const std::string& deviceName);

    // USB monitoring and control
    void updateUsbDeviceInfo();
    bool handlePortEnable(bool enable);
    bool handlePowerControl(bool enable);

    // Member variables
    INST_T _portIndex;                       // USB port index (instance ID)
    WppTaskQueue::task_id_t _updateTaskId;   // Periodic update task ID
    std::string _currentDevicePath;          // Current connected device path in sysfs
};

} // namespace wpp

#endif // WPP_W_34604_USB_MANAGEMENT_H
