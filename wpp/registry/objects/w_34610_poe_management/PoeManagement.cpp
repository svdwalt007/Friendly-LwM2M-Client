/*
 * PoeManagement Implementation (Object 34610)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_34610_poe_management/PoeManagement.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"
#include "WppTaskQueue.h"
#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34610_POE_MANAGEMENT

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>

#define TAG "PoeManagement"

using namespace wpp;

/* Static object methods */
Object& PoeManagement::object(WppClient& client) {
    return client.registry().getObject(POE_MANAGEMENT_OBJECT_ID);
}

Instance* PoeManagement::createInst(WppClient& client, INST_T instId) {
    return object(client).createInstance(instId);
}

Instance* PoeManagement::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool PoeManagement::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

/* Instance lifecycle */
PoeManagement::PoeManagement(Object& object, INST_T instId)
    : Instance(object, instId), _updateTaskId(nullptr) {
    WPP_LOGD(TAG, "PoeManagement instance %d created", instId);
}

PoeManagement::~PoeManagement() {
    // Remove periodic update task if it exists
    if (_updateTaskId) {
        WppTaskQueue::removeTask(_updateTaskId);
        _updateTaskId = nullptr;
    }
    WPP_LOGD(TAG, "PoeManagement instance %d destroyed", instId());
}

/* Initialize resources */
bool PoeManagement::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing PoeManagement resources");

    // Detect PoE support
    bool poeSupported = detectPoeSupport();
    set<BOOL_T>(POE_SUPPORTED_0, poeSupported);

    if (poeSupported) {
        // Initialize with default values
        set<BOOL_T>(POE_ENABLED_1, false);
        set<FLOAT_T>(INPUT_VOLTAGE_2, 0.0f);
        set<FLOAT_T>(INPUT_CURRENT_3, 0.0f);
        set<FLOAT_T>(INPUT_POWER_4, 0.0f);
        set<STRING_T>(POE_STANDARD_5, "");
        set<INT_T>(POE_CLASS_6, CLASS_0);
        set<FLOAT_T>(MAX_POWER_7, 0.0f);
        set<FLOAT_T>(POWER_ALLOCATED_8, 0.0f);
        set<FLOAT_T>(TEMPERATURE_9, 0.0f);
        set<STRING_T>(STATUS_10, "Disabled");

        // Set execute handler
        setExecute(RESET_POE_11, resetPoe);

#ifdef OPENWRT_BUILD
        // Initial status update
        updatePoeStatus();

        // Set up periodic status updates (every 10 seconds)
        _updateTaskId = (void*)WppTaskQueue::addTask(10, [this](WppClient& client, void* ctx) {
            updatePoeStatus();
            return false; // Keep running
        });
#endif
    } else {
        // PoE not supported
        set<BOOL_T>(POE_ENABLED_1, false);
        set<FLOAT_T>(INPUT_VOLTAGE_2, 0.0f);
        set<FLOAT_T>(INPUT_CURRENT_3, 0.0f);
        set<FLOAT_T>(INPUT_POWER_4, 0.0f);
        set<STRING_T>(POE_STANDARD_5, "Not Supported");
        set<INT_T>(POE_CLASS_6, 0);
        set<FLOAT_T>(MAX_POWER_7, 0.0f);
        set<FLOAT_T>(POWER_ALLOCATED_8, 0.0f);
        set<FLOAT_T>(TEMPERATURE_9, 0.0f);
        set<STRING_T>(STATUS_10, "Not Supported");

        WPP_LOGW(TAG, "PoE not supported on this device");
    }

    return true;
}

/* Validation */
bool PoeManagement::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case POE_ENABLED_1: {
            // Check if PoE is supported before allowing enable/disable
            if (!get<BOOL_T>(POE_SUPPORTED_0)) {
                WPP_LOGE(TAG, "Cannot enable PoE - not supported");
                return false;
            }
            break;
        }
    }
    return true;
}

/* Execute handler: Reset PoE */
bool PoeManagement::resetPoe(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    PoeManagement& poe = static_cast<PoeManagement&>(inst);

    if (!poe.get<BOOL_T>(POE_SUPPORTED_0)) {
        WPP_LOGE(TAG, "PoE not supported, cannot reset");
        return false;
    }

    WPP_LOGI(TAG, "Resetting PoE (power cycle)");

#ifdef OPENWRT_BUILD
    // Disable PoE
    system("echo 0 > /sys/class/power_supply/poe/online 2>/dev/null");
    poe.set<BOOL_T>(POE_ENABLED_1, false);
    poe.set<STRING_T>(STATUS_10, "Resetting");
    poe.notifyResChanged(POE_ENABLED_1);
    poe.notifyResChanged(STATUS_10);

    // Wait 2 seconds
    sleep(2);

    // Re-enable PoE
    system("echo 1 > /sys/class/power_supply/poe/online 2>/dev/null");
    poe.set<BOOL_T>(POE_ENABLED_1, true);
    poe.set<STRING_T>(STATUS_10, "Enabled");
    poe.notifyResChanged(POE_ENABLED_1);
    poe.notifyResChanged(STATUS_10);

    // Update status after reset
    poe.updatePoeStatus();

    WPP_LOGI(TAG, "PoE reset completed");
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot reset PoE");
    return false;
#endif
}

/* Detect PoE support */
bool PoeManagement::detectPoeSupport() {
#ifdef OPENWRT_BUILD
    // Check if PoE power supply exists in sysfs
    FILE* fp = fopen("/sys/class/power_supply/poe/type", "r");
    if (fp) {
        char buffer[64];
        if (fgets(buffer, sizeof(buffer), fp)) {
            fclose(fp);
            // Check if it's a PoE power supply
            if (strstr(buffer, "PoE") != nullptr || strstr(buffer, "USB_PD") != nullptr) {
                WPP_LOGI(TAG, "PoE support detected");
                return true;
            }
        }
        fclose(fp);
    }

    // Alternative: Check for PoE controller via I2C
    // Many PoE controllers are on I2C address 0x20-0x23
    FILE* pipe = popen("i2cdetect -y 0 2>/dev/null | grep '20\\|21\\|22\\|23'", "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            pclose(pipe);
            WPP_LOGI(TAG, "PoE controller detected on I2C");
            return true;
        }
        pclose(pipe);
    }

    // Check UCI configuration for PoE
    pipe = popen("uci get system.poe.enabled 2>/dev/null", "r");
    if (pipe) {
        char buffer[16];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            pclose(pipe);
            WPP_LOGI(TAG, "PoE configured in UCI");
            return true;
        }
        pclose(pipe);
    }

    WPP_LOGD(TAG, "No PoE support detected");
    return false;
#else
    // Assume PoE is supported in non-OpenWRT builds for testing
    return false;
#endif
}

/* Update PoE status */
void PoeManagement::updatePoeStatus() {
#ifdef OPENWRT_BUILD
    if (!get<BOOL_T>(POE_SUPPORTED_0)) {
        return;  // PoE not supported
    }

    // Read voltage
    float voltage = readVoltage();
    if (voltage > 0) {
        set<FLOAT_T>(INPUT_VOLTAGE_2, voltage);
        notifyResChanged(INPUT_VOLTAGE_2);
    }

    // Read current
    float current = readCurrent();
    if (current >= 0) {
        set<FLOAT_T>(INPUT_CURRENT_3, current);
        notifyResChanged(INPUT_CURRENT_3);
    }

    // Calculate power (P = V * I)
    if (voltage > 0 && current >= 0) {
        float power = voltage * current;
        set<FLOAT_T>(INPUT_POWER_4, power);
        notifyResChanged(INPUT_POWER_4);
    }

    // Read temperature
    float temp = readTemperature();
    if (temp > -273.0f) {  // Valid temperature
        set<FLOAT_T>(TEMPERATURE_9, temp);
        notifyResChanged(TEMPERATURE_9);
    }

    // Determine PoE standard and class
    std::string standard = getPoeStandard();
    set<STRING_T>(POE_STANDARD_5, standard);
    notifyResChanged(POE_STANDARD_5);

    // Determine PoE class based on voltage and power
    INT_T poeClass = CLASS_0;
    if (voltage >= 44.0f && voltage <= 57.0f) {
        float power = get<FLOAT_T>(INPUT_POWER_4);
        if (power < 4.0f) {
            poeClass = CLASS_1;
        } else if (power < 7.0f) {
            poeClass = CLASS_2;
        } else if (power < 15.4f) {
            poeClass = CLASS_3;
        } else if (power < 30.0f) {
            poeClass = CLASS_4;
        } else if (power < 45.0f) {
            poeClass = CLASS_5;
        } else if (power < 60.0f) {
            poeClass = CLASS_6;
        } else if (power < 75.0f) {
            poeClass = CLASS_7;
        } else {
            poeClass = CLASS_8;
        }
    }
    set<INT_T>(POE_CLASS_6, poeClass);
    notifyResChanged(POE_CLASS_6);

    // Set max power based on class
    float maxPower = 0.0f;
    switch (poeClass) {
        case CLASS_0: maxPower = 15.4f; break;
        case CLASS_1: maxPower = 4.0f; break;
        case CLASS_2: maxPower = 7.0f; break;
        case CLASS_3: maxPower = 15.4f; break;
        case CLASS_4: maxPower = 30.0f; break;
        case CLASS_5: maxPower = 45.0f; break;
        case CLASS_6: maxPower = 60.0f; break;
        case CLASS_7: maxPower = 75.0f; break;
        case CLASS_8: maxPower = 90.0f; break;
    }
    set<FLOAT_T>(MAX_POWER_7, maxPower);
    notifyResChanged(MAX_POWER_7);

    // Power allocated is same as input power for now
    set<FLOAT_T>(POWER_ALLOCATED_8, get<FLOAT_T>(INPUT_POWER_4));
    notifyResChanged(POWER_ALLOCATED_8);

    // Determine status
    bool enabled = get<BOOL_T>(POE_ENABLED_1);
    if (voltage > 40.0f) {
        set<STRING_T>(STATUS_10, "Enabled");
        if (!enabled) {
            set<BOOL_T>(POE_ENABLED_1, true);
            notifyResChanged(POE_ENABLED_1);
        }
    } else {
        set<STRING_T>(STATUS_10, "Disabled");
        if (enabled) {
            set<BOOL_T>(POE_ENABLED_1, false);
            notifyResChanged(POE_ENABLED_1);
        }
    }
    notifyResChanged(STATUS_10);

    WPP_LOGD(TAG, "PoE status updated: %.1fV, %.2fA, %.1fW, %s",
             voltage, current, get<FLOAT_T>(INPUT_POWER_4), standard.c_str());
#endif
}

/* Read voltage from sysfs */
float PoeManagement::readVoltage() {
#ifdef OPENWRT_BUILD
    // Try standard sysfs path
    FILE* fp = fopen("/sys/class/power_supply/poe/voltage_now", "r");
    if (fp) {
        int microvolts = 0;
        if (fscanf(fp, "%d", &microvolts) == 1) {
            fclose(fp);
            return microvolts / 1000000.0f;  // Convert µV to V
        }
        fclose(fp);
    }

    // Try alternative path
    fp = fopen("/sys/kernel/debug/poe/voltage", "r");
    if (fp) {
        float volts = 0.0f;
        if (fscanf(fp, "%f", &volts) == 1) {
            fclose(fp);
            return volts;
        }
        fclose(fp);
    }

    // Try reading from UCI
    FILE* pipe = popen("uci get system.poe.voltage 2>/dev/null", "r");
    if (pipe) {
        char buffer[32];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            pclose(pipe);
            return atof(buffer);
        }
        pclose(pipe);
    }
#endif
    return 0.0f;
}

/* Read current from sysfs */
float PoeManagement::readCurrent() {
#ifdef OPENWRT_BUILD
    // Try standard sysfs path
    FILE* fp = fopen("/sys/class/power_supply/poe/current_now", "r");
    if (fp) {
        int microamps = 0;
        if (fscanf(fp, "%d", &microamps) == 1) {
            fclose(fp);
            return microamps / 1000000.0f;  // Convert µA to A
        }
        fclose(fp);
    }

    // Try alternative path
    fp = fopen("/sys/kernel/debug/poe/current", "r");
    if (fp) {
        float amps = 0.0f;
        if (fscanf(fp, "%f", &amps) == 1) {
            fclose(fp);
            return amps;
        }
        fclose(fp);
    }

    // Try reading from UCI
    FILE* pipe = popen("uci get system.poe.current 2>/dev/null", "r");
    if (pipe) {
        char buffer[32];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            pclose(pipe);
            return atof(buffer);
        }
        pclose(pipe);
    }
#endif
    return 0.0f;
}

/* Read temperature */
float PoeManagement::readTemperature() {
#ifdef OPENWRT_BUILD
    // Try PoE-specific temperature sensor
    FILE* fp = fopen("/sys/class/power_supply/poe/temp", "r");
    if (fp) {
        int millidegrees = 0;
        if (fscanf(fp, "%d", &millidegrees) == 1) {
            fclose(fp);
            return millidegrees / 1000.0f;  // Convert milli-degrees to degrees
        }
        fclose(fp);
    }

    // Try thermal zone
    fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (fp) {
        int millidegrees = 0;
        if (fscanf(fp, "%d", &millidegrees) == 1) {
            fclose(fp);
            return millidegrees / 1000.0f;
        }
        fclose(fp);
    }
#endif
    return -273.15f;  // Invalid temperature
}

/* Get PoE standard */
std::string PoeManagement::getPoeStandard() {
#ifdef OPENWRT_BUILD
    float voltage = get<FLOAT_T>(INPUT_VOLTAGE_2);
    float power = get<FLOAT_T>(INPUT_POWER_4);

    // Determine standard based on voltage and power capabilities
    if (power > 60.0f) {
        return "802.3bt";  // PoE++ (Type 3/4, up to 90W+)
    } else if (power > 25.0f) {
        return "802.3at";  // PoE+ (Type 2, up to 30W)
    } else if (voltage >= 44.0f && voltage <= 57.0f) {
        return "802.3af";  // PoE (Type 1, up to 15.4W)
    } else {
        return "Unknown";
    }
#else
    return "Not Available";
#endif
}

#endif // OBJ_W_34610_POE_MANAGEMENT
