/*
 * UsbManagement (Object 34604)
 * Walt Technologies - OpenWRT One Router
 *
 * Linux sysfs integration for USB device monitoring and management
 */

#include "w_34604_usb_management/UsbManagement.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"
#include "WppTaskQueue.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>

#define TAG "UsbManagement"
#define USB_SYSFS_PATH "/sys/bus/usb/devices"
#define USB_UPDATE_INTERVAL_SEC 10

namespace wpp {

UsbManagement::UsbManagement(Object& object, INST_T instId)
    : Instance(object, instId), _portIndex(instId), _updateTaskId(WPP_ERR_TASK_ID) {
    WPP_LOGD(TAG, "Creating UsbManagement instance %d", instId);
}

UsbManagement::~UsbManagement() {
    WPP_LOGD(TAG, "Destroying UsbManagement instance %d", _portIndex);
    // Request removal of the periodic update task
    if (_updateTaskId != WPP_ERR_TASK_ID) {
        WppTaskQueue::requestToRemoveTask(_updateTaskId);
        _updateTaskId = WPP_ERR_TASK_ID;
    }
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
    WPP_LOGD(TAG, "Initializing resources for USB port %d", _portIndex);

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

    // Set default port name based on instance ID
    std::stringstream portName;
    portName << "USB" << (_portIndex + 1);
    resource(PORT_NAME_0)->set<STRING_T>(portName.str());

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

    // Set write handler for PORT_ENABLED
    resource(PORT_ENABLED_2)->setWriteVerifier(
        [this](const BOOL_T& value) {
            return handlePortEnable(value);
        }
    );

    // Set write handler for POWER_ENABLED
    resource(POWER_ENABLED_8)->setWriteVerifier(
        [this](const BOOL_T& value) {
            return handlePowerControl(value);
        }
    );

    // Perform initial USB device scan
    updateUsbDeviceInfo();

    // Set up periodic USB device monitoring (every 10 seconds)
    _updateTaskId = WppTaskQueue::addTask(USB_UPDATE_INTERVAL_SEC,
        [this](WppClient &client, void *ctx) -> bool {
            updateUsbDeviceInfo();
            return false; // Return false to keep the task running
        }
    );

    if (_updateTaskId == WPP_ERR_TASK_ID) {
        WPP_LOGE(TAG, "Failed to create USB monitoring task for port %d", _portIndex);
    } else {
        WPP_LOGD(TAG, "USB monitoring task created with ID %u for port %d",
                 _updateTaskId, _portIndex);
    }

    return true;
}

bool UsbManagement::validate(ID_T resId, const void *data, size_t size) {
    return true;
}

// ============================================================================
// Helper Functions for sysfs Integration
// ============================================================================

std::string UsbManagement::readSysfsFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::string content;
    std::getline(file, content);
    file.close();

    // Trim whitespace and newlines
    if (!content.empty()) {
        size_t end = content.find_last_not_of(" \n\r\t");
        if (end != std::string::npos) {
            content = content.substr(0, end + 1);
        }
    }

    return content;
}

bool UsbManagement::writeSysfsFile(const std::string& path, const std::string& value) {
    std::ofstream file(path);
    if (!file.is_open()) {
        WPP_LOGE(TAG, "Failed to open sysfs file for writing: %s", path.c_str());
        return false;
    }

    file << value;
    file.close();

    if (file.fail()) {
        WPP_LOGE(TAG, "Failed to write to sysfs file: %s", path.c_str());
        return false;
    }

    return true;
}

std::vector<std::string> UsbManagement::listUsbDevices() {
    std::vector<std::string> devices;

    DIR* dir = opendir(USB_SYSFS_PATH);
    if (!dir) {
        WPP_LOGE(TAG, "Failed to open USB sysfs directory: %s", USB_SYSFS_PATH);
        return devices;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string deviceName = entry->d_name;

        // Skip "." and ".."
        if (deviceName == "." || deviceName == "..") {
            continue;
        }

        // USB devices typically have format like "1-1", "2-1.1", etc.
        // Skip the root USB controllers (just numbers)
        if (deviceName.find('-') == std::string::npos) {
            continue;
        }

        // Check if idVendor exists (confirms it's a real device)
        std::string vendorPath = std::string(USB_SYSFS_PATH) + "/" + deviceName + "/idVendor";
        struct stat buffer;
        if (stat(vendorPath.c_str(), &buffer) == 0) {
            devices.push_back(deviceName);
        }
    }

    closedir(dir);
    return devices;
}

UsbManagement::UsbDeviceInfo UsbManagement::getDeviceInfo(const std::string& deviceName) {
    UsbDeviceInfo info;
    info.devicePath = std::string(USB_SYSFS_PATH) + "/" + deviceName;

    // Read vendor and product IDs
    info.vendorId = readSysfsFile(info.devicePath + "/idVendor");
    info.productId = readSysfsFile(info.devicePath + "/idProduct");

    // Read manufacturer and product strings
    info.manufacturer = readSysfsFile(info.devicePath + "/manufacturer");
    info.product = readSysfsFile(info.devicePath + "/product");

    // Read device class
    std::string deviceClass = readSysfsFile(info.devicePath + "/bDeviceClass");
    if (!deviceClass.empty()) {
        // Convert hex string to integer
        std::stringstream ss;
        ss << std::hex << deviceClass;
        ss >> info.deviceClass;
    }

    // Read max power (in mA)
    std::string maxPower = readSysfsFile(info.devicePath + "/bMaxPower");
    if (!maxPower.empty()) {
        // bMaxPower is in units of 2mA
        std::stringstream ss;
        ss << maxPower;
        int powerUnits;
        ss >> powerUnits;
        info.maxPowerMa = powerUnits * 2;
    }

    // Read current power state
    std::string powerControl = readSysfsFile(info.devicePath + "/power/control");
    info.powerEnabled = (powerControl == "on" || powerControl == "auto");

    // Check if device is authorized
    std::string authorized = readSysfsFile(info.devicePath + "/authorized");
    info.authorized = (authorized == "1");

    WPP_LOGD(TAG, "USB Device: %s, VID=%s, PID=%s, Class=%d, Power=%dmA, Product=%s",
             deviceName.c_str(), info.vendorId.c_str(), info.productId.c_str(),
             info.deviceClass, info.maxPowerMa, info.product.c_str());

    return info;
}

void UsbManagement::updateUsbDeviceInfo() {
    WPP_LOGD(TAG, "Updating USB device info for port %d", _portIndex);

    // List all USB devices
    std::vector<std::string> devices = listUsbDevices();

    // For this implementation, we map instance ID to USB port
    // Instance 0 = first USB device, Instance 1 = second USB device, etc.
    bool deviceFound = false;

    if (_portIndex < devices.size()) {
        const std::string& deviceName = devices[_portIndex];
        UsbDeviceInfo info = getDeviceInfo(deviceName);

        // Store device path for later use
        _currentDevicePath = info.devicePath;

        // Update resources
        resource(DEVICE_CONNECTED_3)->set<BOOL_T>(true);

        // Build device name from manufacturer and product
        std::string fullName;
        if (!info.manufacturer.empty() && !info.product.empty()) {
            fullName = info.manufacturer + " " + info.product;
        } else if (!info.product.empty()) {
            fullName = info.product;
        } else if (!info.manufacturer.empty()) {
            fullName = info.manufacturer;
        } else {
            fullName = "Unknown USB Device";
        }
        resource(DEVICE_NAME_4)->set<STRING_T>(fullName);

        resource(DEVICE_VENDOR_ID_5)->set<STRING_T>(info.vendorId);
        resource(DEVICE_PRODUCT_ID_6)->set<STRING_T>(info.productId);
        resource(DEVICE_CLASS_7)->set<INT_T>(info.deviceClass);
        resource(POWER_ENABLED_8)->set<BOOL_T>(info.powerEnabled);
        resource(CURRENT_DRAW_9)->set<INT_T>(info.maxPowerMa);

        deviceFound = true;

        // Notify resources changed
        notifyResChanged(DEVICE_CONNECTED_3);
        notifyResChanged(DEVICE_NAME_4);
        notifyResChanged(DEVICE_VENDOR_ID_5);
        notifyResChanged(DEVICE_PRODUCT_ID_6);
        notifyResChanged(DEVICE_CLASS_7);
        notifyResChanged(CURRENT_DRAW_9);
    }

    if (!deviceFound) {
        // No device connected to this port
        bool wasConnected = resource(DEVICE_CONNECTED_3)->get<BOOL_T>();

        if (wasConnected) {
            // Device was disconnected
            resource(DEVICE_CONNECTED_3)->set<BOOL_T>(false);
            resource(DEVICE_NAME_4)->set<STRING_T>("");
            resource(DEVICE_VENDOR_ID_5)->set<STRING_T>("0000");
            resource(DEVICE_PRODUCT_ID_6)->set<STRING_T>("0000");
            resource(DEVICE_CLASS_7)->set<INT_T>(0);
            resource(CURRENT_DRAW_9)->set<INT_T>(0);

            _currentDevicePath.clear();

            // Notify disconnection
            notifyResChanged(DEVICE_CONNECTED_3);
            notifyResChanged(DEVICE_NAME_4);

            WPP_LOGI(TAG, "USB device disconnected from port %d", _portIndex);
        }
    }
}

bool UsbManagement::handlePortEnable(bool enable) {
    WPP_LOGI(TAG, "Port %d enable request: %s", _portIndex, enable ? "true" : "false");

    if (_currentDevicePath.empty()) {
        WPP_LOGW(TAG, "No device connected to port %d", _portIndex);
        return true; // Allow the change even if no device is connected
    }

    // Enable/disable by authorizing/deauthorizing the device
    std::string authPath = _currentDevicePath + "/authorized";
    bool success = writeSysfsFile(authPath, enable ? "1" : "0");

    if (success) {
        WPP_LOGI(TAG, "Port %d %s successfully",
                 _portIndex, enable ? "enabled" : "disabled");
    } else {
        WPP_LOGE(TAG, "Failed to %s port %d",
                 enable ? "enable" : "disable", _portIndex);
    }

    return success;
}

bool UsbManagement::handlePowerControl(bool enable) {
    WPP_LOGI(TAG, "Power control request for port %d: %s",
             _portIndex, enable ? "on" : "off");

    if (_currentDevicePath.empty()) {
        WPP_LOGW(TAG, "No device connected to port %d", _portIndex);
        return true; // Allow the change even if no device is connected
    }

    // Control power through sysfs power management
    std::string powerControlPath = _currentDevicePath + "/power/control";
    std::string levelPath = _currentDevicePath + "/power/level";

    bool success = false;

    // Try modern "control" interface first
    if (enable) {
        success = writeSysfsFile(powerControlPath, "on");
    } else {
        success = writeSysfsFile(powerControlPath, "auto");
        if (!success) {
            // Try suspend
            success = writeSysfsFile(levelPath, "suspend");
        }
    }

    if (success) {
        WPP_LOGI(TAG, "Power %s for port %d",
                 enable ? "enabled" : "disabled", _portIndex);
    } else {
        WPP_LOGE(TAG, "Failed to control power for port %d", _portIndex);
    }

    return success;
}

} // namespace wpp
