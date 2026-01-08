/*
 * GpioControl (Object 10515)
 * Walt Technologies - OpenWRT One Router
 * Linux sysfs GPIO and LED Control Integration
 */

#include "w_10515_gpio_control/GpioControl.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>

#define TAG "GpioControl"

namespace wpp {

// Helper function to write to sysfs files
static bool writeSysfsFile(const std::string& path, const std::string& value) {
    std::ofstream file(path);
    if (!file.is_open()) {
        WPP_LOGE(TAG, "Failed to open file for writing: %s", path.c_str());
        return false;
    }

    file << value;
    file.close();

    if (file.fail()) {
        WPP_LOGE(TAG, "Failed to write to file: %s", path.c_str());
        return false;
    }

    WPP_LOGD(TAG, "Wrote '%s' to %s", value.c_str(), path.c_str());
    return true;
}

// Helper function to read from sysfs files
static bool readSysfsFile(const std::string& path, std::string& value) {
    std::ifstream file(path);
    if (!file.is_open()) {
        WPP_LOGE(TAG, "Failed to open file for reading: %s", path.c_str());
        return false;
    }

    std::getline(file, value);
    file.close();

    if (file.fail() && !file.eof()) {
        WPP_LOGE(TAG, "Failed to read from file: %s", path.c_str());
        return false;
    }

    // Trim whitespace
    value.erase(0, value.find_first_not_of(" \t\n\r"));
    value.erase(value.find_last_not_of(" \t\n\r") + 1);

    WPP_LOGD(TAG, "Read '%s' from %s", value.c_str(), path.c_str());
    return true;
}

// Helper function to check if a file exists
static bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

GpioControl::GpioControl(Object& object, INST_T instId)
    : Instance(object, instId), _gpioExported(false), _lastButtonState(false), _buttonPressCount(0) {
    WPP_LOGD(TAG, "Creating GpioControl instance %d", instId);
}

GpioControl::~GpioControl() {
    WPP_LOGD(TAG, "Destroying GpioControl instance");

    // Cleanup: unexport GPIO if it was exported
    if (_gpioExported) {
        INT_T gpioNum = 0;
        INT_T gpioType = TYPE_LED;

        if (resource(GPIO_NUMBER_1)) {
            gpioNum = resource(GPIO_NUMBER_1)->get<INT_T>();
        }
        if (resource(GPIO_TYPE_2)) {
            gpioType = resource(GPIO_TYPE_2)->get<INT_T>();
        }

        if (gpioType == TYPE_GENERAL_GPIO || gpioType == TYPE_BUTTON) {
            std::string unexportPath = "/sys/class/gpio/unexport";
            writeSysfsFile(unexportPath, std::to_string(gpioNum));
        }
    }
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

bool GpioControl::exportGpio(INT_T gpioNum) {
    std::string gpioPath = "/sys/class/gpio/gpio" + std::to_string(gpioNum);

    // Check if already exported
    if (fileExists(gpioPath)) {
        WPP_LOGD(TAG, "GPIO %d already exported", gpioNum);
        _gpioExported = true;
        return true;
    }

    // Export the GPIO
    std::string exportPath = "/sys/class/gpio/export";
    if (writeSysfsFile(exportPath, std::to_string(gpioNum))) {
        // Wait a bit for the system to create the GPIO files
        usleep(100000); // 100ms
        _gpioExported = true;
        WPP_LOGD(TAG, "Exported GPIO %d", gpioNum);
        return true;
    }

    return false;
}

bool GpioControl::setGpioDirection(INT_T gpioNum, bool isOutput) {
    std::string directionPath = "/sys/class/gpio/gpio" + std::to_string(gpioNum) + "/direction";
    std::string direction = isOutput ? "out" : "in";

    if (writeSysfsFile(directionPath, direction)) {
        WPP_LOGD(TAG, "Set GPIO %d direction to %s", gpioNum, direction.c_str());
        return true;
    }

    return false;
}

bool GpioControl::readGpioValue(INT_T gpioNum, bool& value) {
    std::string valuePath = "/sys/class/gpio/gpio" + std::to_string(gpioNum) + "/value";
    std::string strValue;

    if (readSysfsFile(valuePath, strValue)) {
        value = (strValue == "1");
        WPP_LOGD(TAG, "Read GPIO %d value: %d", gpioNum, value);
        return true;
    }

    return false;
}

bool GpioControl::writeGpioValue(INT_T gpioNum, bool value) {
    std::string valuePath = "/sys/class/gpio/gpio" + std::to_string(gpioNum) + "/value";
    std::string strValue = value ? "1" : "0";

    if (writeSysfsFile(valuePath, strValue)) {
        WPP_LOGD(TAG, "Wrote GPIO %d value: %d", gpioNum, value);
        return true;
    }

    return false;
}

bool GpioControl::readLedBrightness(const std::string& ledName, INT_T& brightness) {
    std::string brightnessPath = "/sys/class/leds/" + ledName + "/brightness";
    std::string strValue;

    if (readSysfsFile(brightnessPath, strValue)) {
        try {
            brightness = std::stoi(strValue);
            WPP_LOGD(TAG, "Read LED '%s' brightness: %d", ledName.c_str(), brightness);
            return true;
        } catch (const std::exception& e) {
            WPP_LOGE(TAG, "Failed to parse brightness value: %s", e.what());
        }
    }

    return false;
}

bool GpioControl::writeLedBrightness(const std::string& ledName, INT_T brightness) {
    std::string brightnessPath = "/sys/class/leds/" + ledName + "/brightness";

    if (writeSysfsFile(brightnessPath, std::to_string(brightness))) {
        WPP_LOGD(TAG, "Wrote LED '%s' brightness: %d", ledName.c_str(), brightness);
        return true;
    }

    return false;
}

bool GpioControl::readLedTrigger(const std::string& ledName, std::string& trigger) {
    std::string triggerPath = "/sys/class/leds/" + ledName + "/trigger";
    std::string triggerList;

    if (readSysfsFile(triggerPath, triggerList)) {
        // Parse the trigger list to find the active one (surrounded by brackets)
        size_t start = triggerList.find('[');
        size_t end = triggerList.find(']');

        if (start != std::string::npos && end != std::string::npos && end > start) {
            trigger = triggerList.substr(start + 1, end - start - 1);
            WPP_LOGD(TAG, "Read LED '%s' trigger: %s", ledName.c_str(), trigger.c_str());
            return true;
        }
    }

    return false;
}

bool GpioControl::writeLedTrigger(const std::string& ledName, const std::string& trigger) {
    std::string triggerPath = "/sys/class/leds/" + ledName + "/trigger";

    if (writeSysfsFile(triggerPath, trigger)) {
        WPP_LOGD(TAG, "Wrote LED '%s' trigger: %s", ledName.c_str(), trigger.c_str());
        return true;
    }

    return false;
}

bool GpioControl::setLedState(const std::string& ledName, bool state) {
    // Set brightness to max (255) for ON, 0 for OFF
    INT_T brightness = state ? 255 : 0;
    return writeLedBrightness(ledName, brightness);
}

bool GpioControl::setupBlinkTimer(const std::string& ledName, INT_T intervalMs) {
    // Set trigger to timer
    if (!writeLedTrigger(ledName, "timer")) {
        return false;
    }

    // Set delay_on and delay_off (in milliseconds)
    std::string delayOnPath = "/sys/class/leds/" + ledName + "/delay_on";
    std::string delayOffPath = "/sys/class/leds/" + ledName + "/delay_off";

    std::string delayValue = std::to_string(intervalMs);

    bool success = writeSysfsFile(delayOnPath, delayValue);
    success = success && writeSysfsFile(delayOffPath, delayValue);

    if (success) {
        WPP_LOGD(TAG, "Set LED '%s' blink interval to %d ms", ledName.c_str(), intervalMs);
    }

    return success;
}

std::string GpioControl::getTriggerName(INT_T triggerMode) {
    switch (triggerMode) {
        case TRIGGER_NONE:
            return "none";
        case TRIGGER_NETDEV:
            return "netdev";
        case TRIGGER_TIMER:
            return "timer";
        case TRIGGER_DEFAULT_ON:
            return "default-on";
        default:
            return "none";
    }
}

INT_T GpioControl::getTriggerMode(const std::string& triggerName) {
    if (triggerName == "netdev") return TRIGGER_NETDEV;
    if (triggerName == "timer") return TRIGGER_TIMER;
    if (triggerName == "default-on") return TRIGGER_DEFAULT_ON;
    return TRIGGER_NONE;
}

void GpioControl::updateButtonState() {
    INT_T gpioType = resource(GPIO_TYPE_2)->get<INT_T>();

    if (gpioType != TYPE_BUTTON) {
        return; // Not a button
    }

    INT_T gpioNum = resource(GPIO_NUMBER_1)->get<INT_T>();
    bool currentState = false;

    if (readGpioValue(gpioNum, currentState)) {
        // Update button state resource
        resource(BUTTON_STATE_7)->set<BOOL_T>(currentState);

        // Detect button press (transition from released to pressed)
        // Assuming active low (pressed = 0, released = 1)
        if (_lastButtonState && !currentState) {
            _buttonPressCount++;
            resource(BUTTON_PRESS_COUNT_8)->set<INT_T>(_buttonPressCount);
            WPP_LOGD(TAG, "Button pressed! Count: %d", _buttonPressCount);
        }

        _lastButtonState = currentState;
    }
}

void GpioControl::updateHardwareState() {
    INT_T gpioType = resource(GPIO_TYPE_2)->get<INT_T>();
    STRING_T gpioName = resource(GPIO_NAME_0)->get<STRING_T>();
    INT_T gpioNum = resource(GPIO_NUMBER_1)->get<INT_T>();

    if (gpioType == TYPE_LED) {
        // LED Control
        bool currentState = resource(CURRENT_STATE_3)->get<BOOL_T>();
        bool blinkEnabled = resource(BLINK_ENABLED_4)->get<BOOL_T>();
        INT_T blinkInterval = resource(BLINK_INTERVAL_5)->get<INT_T>();
        INT_T triggerMode = resource(TRIGGER_MODE_6)->get<INT_T>();

        std::string ledName = gpioName;

        // Check if LED exists
        std::string ledPath = "/sys/class/leds/" + ledName;
        if (!fileExists(ledPath)) {
            WPP_LOGW(TAG, "LED '%s' not found in sysfs", ledName.c_str());
            return;
        }

        if (blinkEnabled) {
            // Enable blinking
            setupBlinkTimer(ledName, blinkInterval);
        } else {
            // Disable blinking - set trigger based on triggerMode
            std::string trigger = getTriggerName(triggerMode);
            writeLedTrigger(ledName, trigger);

            // If trigger is none, set the LED state directly
            if (triggerMode == TRIGGER_NONE) {
                setLedState(ledName, currentState);
            }
        }
    } else if (gpioType == TYPE_GENERAL_GPIO) {
        // General GPIO Control
        if (!_gpioExported) {
            if (!exportGpio(gpioNum)) {
                WPP_LOGE(TAG, "Failed to export GPIO %d", gpioNum);
                return;
            }
        }

        // Set as output
        if (!setGpioDirection(gpioNum, true)) {
            WPP_LOGE(TAG, "Failed to set GPIO %d direction", gpioNum);
            return;
        }

        // Write the state
        bool currentState = resource(CURRENT_STATE_3)->get<BOOL_T>();
        writeGpioValue(gpioNum, currentState);
    } else if (gpioType == TYPE_BUTTON) {
        // Button - input only
        if (!_gpioExported) {
            if (!exportGpio(gpioNum)) {
                WPP_LOGE(TAG, "Failed to export GPIO %d", gpioNum);
                return;
            }
        }

        // Set as input
        if (!setGpioDirection(gpioNum, false)) {
            WPP_LOGE(TAG, "Failed to set GPIO %d direction", gpioNum);
            return;
        }

        // Read initial button state
        updateButtonState();
    }
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
    resource(GPIO_NAME_0)->set<STRING_T>("green:status");
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

    // Set write handlers
    resource(CURRENT_STATE_3)->setDataWriter(
        (WRITE_BOOL_T)[this](const BOOL_T& value) {
            WPP_LOGD(TAG, "CURRENT_STATE write handler: %d", value);
            updateHardwareState();
            return DATA_WRITE_RESULT::WRITE_SUCCESS;
        }
    );

    resource(BLINK_ENABLED_4)->setDataWriter(
        (WRITE_BOOL_T)[this](const BOOL_T& value) {
            WPP_LOGD(TAG, "BLINK_ENABLED write handler: %d", value);
            updateHardwareState();
            return DATA_WRITE_RESULT::WRITE_SUCCESS;
        }
    );

    resource(BLINK_INTERVAL_5)->setDataWriter(
        (WRITE_INT_T)[this](const INT_T& value) {
            WPP_LOGD(TAG, "BLINK_INTERVAL write handler: %d", value);
            updateHardwareState();
            return DATA_WRITE_RESULT::WRITE_SUCCESS;
        }
    );

    resource(TRIGGER_MODE_6)->setDataWriter(
        (WRITE_INT_T)[this](const INT_T& value) {
            WPP_LOGD(TAG, "TRIGGER_MODE write handler: %d", value);
            updateHardwareState();
            return DATA_WRITE_RESULT::WRITE_SUCCESS;
        }
    );

    // Initialize hardware state
    updateHardwareState();

    return true;
}

bool GpioControl::validate(ID_T resId, const void *data, size_t size) {
    return true;
}

} // namespace wpp
