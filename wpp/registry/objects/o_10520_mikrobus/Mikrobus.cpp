/*
 * Walt Technologies MIKROBUS Object - Implementation
 *
 * Provides comprehensive management and monitoring of MIKROBUS sockets and Click boards.
 *
 * Integration Note:
 * This implementation provides the LwM2M object structure and resource definitions.
 * The hardware integration requires platform-specific drivers for:
 * - SPI, I2C, UART bus access
 * - GPIO control
 * - ADC (analog input)
 * - PWM output
 * - Click board manifest reading (EEPROM/I2C)
 *
 * For OpenWRT, use sysfs interfaces or direct /dev access.
 */

#include "Mikrobus.h"
#include "MikrobusInfo.h"
#include "MikrobusConfig.h"
#include "WppClient.h"
#include "WppLogs.h"
#include <cmath>
#include <sstream>
#include <iomanip>

#ifdef OPENWRT_BUILD
#include <fstream>
#include <cstring>
#endif

namespace wpp {

/* Static object instance */
static ObjectImpl<Mikrobus> mikrobusObj(MIKROBUS_OBJ_ID,
                                         MIKROBUS_MULTIPLE_INSTANCES);

Object& Mikrobus::object(WppClient& client) {
    return mikrobusObj;
}

Instance* Mikrobus::createInst(WppClient& client, OBJ_INST_ID_T instanceId) {
    return mikrobusObj.createInst(instanceId, client, instanceId);
}

const std::string Mikrobus::urn() {
    return MIKROBUS_URN;
}

Mikrobus::Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId)
    : Instance(MIKROBUS_OBJ_ID, instanceId), _socketId(instanceId) {
    WPP_LOGD("Mikrobus instance created for socket: %d", instanceId);
}

void Mikrobus::resourcesCreate() {
    std::vector<Resource> resources = {
        /* Socket Information (0-9) */
        {SOCKET_ID_0,           ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {SOCKET_NAME_1,         ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {SOCKET_ENABLED_2,      ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {CLICK_BOARD_PRESENT_3, ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {CLICK_BOARD_TYPE_4,    ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {POWER_VOLTAGE_5,       ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {POWER_STATE_6,         ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},

        /* Click Board Information (10-19) */
#if RES_10520_10
        {BOARD_MANUFACTURER_10, ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif
#if RES_10520_11
        {BOARD_MODEL_11,        ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif
#if RES_10520_12
        {BOARD_FIRMWARE_VERSION_12, ItemOp(ItemOp::READ),          IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif
#if RES_10520_13
        {BOARD_SERIAL_NUMBER_13, ItemOp(ItemOp::READ),             IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif
        {BOARD_DESCRIPTION_14,  ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},

        /* Interface Configuration (20-29) */
        {ACTIVE_INTERFACE_20,   ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
#if RES_10520_21
        {I2C_ADDRESS_21,        ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
#endif
#if RES_10520_22
        {SPI_MODE_22,           ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
#endif
#if RES_10520_23
        {SPI_SPEED_23,          ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
#endif
#if RES_10520_24
        {UART_BAUD_RATE_24,     ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
#endif
        {UART_DATA_BITS_25,     ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {UART_PARITY_26,        ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {UART_STOP_BITS_27,     ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},

        /* GPIO and Analog (30-39) */
#if RES_10520_31
        {AN_VALUE_31,           ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10520_32
        {PWM_DUTY_CYCLE_32,     ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10520_33
        {PWM_FREQUENCY_33,      ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
#endif
#if RES_10520_34
        {INT_PIN_STATE_34,      ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
        {RST_PIN_STATE_35,      ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
        {CS_PIN_STATE_36,       ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},

        /* Status and Diagnostics (40-49) */
        {COMMUNICATION_STATUS_40, ItemOp(ItemOp::READ),             IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {LAST_ERROR_41,         ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {TRANSACTION_COUNT_42,  ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {ERROR_COUNT_43,        ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {UPTIME_44,             ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},

        /* Control Actions (100-109) */
#if RES_10520_101
        {INIT_CLICK_BOARD_101,  ItemOp(ItemOp::EXECUTE),            IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::NONE},
#endif
#if RES_10520_102
        {RESET_CLICK_BOARD_102, ItemOp(ItemOp::EXECUTE),            IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::NONE},
#endif
#if RES_10520_103
        {SHUTDOWN_CLICK_BOARD_103, ItemOp(ItemOp::EXECUTE),         IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::NONE},
#endif
#if RES_10520_110
        {READ_MANIFEST_110,     ItemOp(ItemOp::EXECUTE),            IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::NONE},
#endif
#if RES_10520_111
        {MANIFEST_DATA_111,     ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif
    };
    setupResources(std::move(resources));
}

void Mikrobus::resourcesInit() {
    // Initialize socket information
    resource(SOCKET_ID_0)->set<INT_T>(_socketId);

    std::stringstream ss;
    ss << "MIKROBUS-" << _socketId;
    resource(SOCKET_NAME_1)->set<STRING_T>(ss.str());

    resource(SOCKET_ENABLED_2)->set<BOOL_T>(false);
    resource(CLICK_BOARD_PRESENT_3)->set<BOOL_T>(false);
    resource(CLICK_BOARD_TYPE_4)->set<STRING_T>("Unknown");

    // Default to 3.3V
    resource(POWER_VOLTAGE_5)->set<INT_T>(3300);
    resource(POWER_STATE_6)->set<BOOL_T>(false);

    // Initialize Click board information
#if RES_10520_10
    resource(BOARD_MANUFACTURER_10)->set<STRING_T>("Unknown");
#endif
#if RES_10520_11
    resource(BOARD_MODEL_11)->set<STRING_T>("Unknown");
#endif
#if RES_10520_12
    resource(BOARD_FIRMWARE_VERSION_12)->set<STRING_T>("0.0.0");
#endif
#if RES_10520_13
    resource(BOARD_SERIAL_NUMBER_13)->set<STRING_T>("N/A");
#endif
    resource(BOARD_DESCRIPTION_14)->set<STRING_T>("No Click board detected");

    // Initialize interface configuration
    resource(ACTIVE_INTERFACE_20)->set<INT_T>(INTERFACE_NONE);
#if RES_10520_21
    resource(I2C_ADDRESS_21)->set<INT_T>(0x00);
#endif
#if RES_10520_22
    resource(SPI_MODE_22)->set<INT_T>(0);
#endif
#if RES_10520_23
    resource(SPI_SPEED_23)->set<INT_T>(1000000);  // 1 MHz default
#endif
#if RES_10520_24
    resource(UART_BAUD_RATE_24)->set<INT_T>(115200);
#endif
    resource(UART_DATA_BITS_25)->set<INT_T>(8);
    resource(UART_PARITY_26)->set<INT_T>(0);  // None
    resource(UART_STOP_BITS_27)->set<INT_T>(1);

    // Initialize GPIO and analog
#if RES_10520_31
    resource(AN_VALUE_31)->set<FLOAT_T>(0.0f);
#endif
#if RES_10520_32
    resource(PWM_DUTY_CYCLE_32)->set<FLOAT_T>(0.0f);
#endif
#if RES_10520_33
    resource(PWM_FREQUENCY_33)->set<INT_T>(1000);  // 1 kHz default
#endif
#if RES_10520_34
    resource(INT_PIN_STATE_34)->set<BOOL_T>(false);
#endif
    resource(RST_PIN_STATE_35)->set<BOOL_T>(true);  // Not in reset
    resource(CS_PIN_STATE_36)->set<BOOL_T>(true);   // Chip select inactive (high)

    // Initialize status
    resource(COMMUNICATION_STATUS_40)->set<INT_T>(COMM_NOT_CONFIGURED);
    resource(LAST_ERROR_41)->set<STRING_T>("Not configured");
    resource(TRANSACTION_COUNT_42)->set<INT_T>(0);
    resource(ERROR_COUNT_43)->set<INT_T>(0);
    resource(UPTIME_44)->set<INT_T>(0);

    // Initialize manifest data
#if RES_10520_111
    resource(MANIFEST_DATA_111)->set<STRING_T>("{}");
#endif

    // Set execute handlers
#if RES_10520_101
    resource(INIT_CLICK_BOARD_101)->set<EXECUTE_T>(
        [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
            return executeInitClickBoard(inst, resId, data);
        }
    );
#endif

#if RES_10520_102
    resource(RESET_CLICK_BOARD_102)->set<EXECUTE_T>(
        [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
            return executeResetClickBoard(inst, resId, data);
        }
    );
#endif

#if RES_10520_103
    resource(SHUTDOWN_CLICK_BOARD_103)->set<EXECUTE_T>(
        [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
            return executeShutdownClickBoard(inst, resId, data);
        }
    );
#endif

#if RES_10520_110
    resource(READ_MANIFEST_110)->set<EXECUTE_T>(
        [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
            return executeReadManifest(inst, resId, data);
        }
    );
#endif

    // Set up periodic status updates (every 30 seconds)
    _updateTaskId = WppTaskQueue::addTask(30, [this](WppClient &client, void *ctx) {
        updateSocketStatus();
        return false; // Keep running
    });

    // Try to detect Click board
    detectClickBoard();
}

bool Mikrobus::updateSocketStatus() {
    WPP_LOGD("Mikrobus", "Updating socket %d status", _socketId);

    #ifdef OPENWRT_BUILD
    // Update analog input if available
    #if RES_10520_31
    std::stringstream anPath;
    anPath << "/sys/bus/iio/devices/iio:device0/in_voltage" << _socketId << "_raw";
    std::ifstream anFile(anPath.str());
    if (anFile.is_open()) {
        int rawValue;
        anFile >> rawValue;
        anFile.close();

        // Convert to volts (assuming 12-bit ADC, 3.3V reference)
        float voltage = (rawValue / 4095.0f) * 3.3f;
        resource(AN_VALUE_31)->set<FLOAT_T>(voltage);
        notifyResChanged(AN_VALUE_31);
    }
    #endif

    // Update INT pin state if available
    #if RES_10520_34
    std::stringstream intPath;
    intPath << "/sys/class/gpio/mikrobus" << _socketId << "_int/value";
    std::ifstream intFile(intPath.str());
    if (intFile.is_open()) {
        int value;
        intFile >> value;
        intFile.close();
        resource(INT_PIN_STATE_34)->set<BOOL_T>(value != 0);
        notifyResChanged(INT_PIN_STATE_34);
    }
    #endif

    // Update uptime
    INT_T uptime = resource(UPTIME_44)->get<INT_T>();
    resource(UPTIME_44)->set<INT_T>(uptime + 30);
    #endif

    return true;
}

bool Mikrobus::detectClickBoard() {
    WPP_LOGD("Mikrobus", "Detecting Click board on socket %d", _socketId);

    #ifdef OPENWRT_BUILD
    // Try to read Click board manifest from I2C EEPROM (typically at 0x50)
    // Click boards with manifest support store identification data

    std::stringstream cmd;
    cmd << "i2cget -y " << _socketId << " 0x50 0x00 2>/dev/null";

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            // Board detected
            resource(CLICK_BOARD_PRESENT_3)->set<BOOL_T>(true);
            notifyResChanged(CLICK_BOARD_PRESENT_3);

            // Try to read more manifest data
            // This is a simplified example - real implementation would parse full manifest
            pclose(pipe);
            return true;
        }
        pclose(pipe);
    }
    #endif

    resource(CLICK_BOARD_PRESENT_3)->set<BOOL_T>(false);
    return false;
}

bool Mikrobus::configureInterface() {
    INT_T interfaceType = resource(ACTIVE_INTERFACE_20)->get<INT_T>();

    WPP_LOGD("Mikrobus", "Configuring interface %d for socket %d",
             interfaceType, _socketId);

    #ifdef OPENWRT_BUILD
    switch (interfaceType) {
        case INTERFACE_SPI:
            // Configure SPI interface
            // Load spidev module and configure parameters
            break;

        case INTERFACE_I2C:
            // Configure I2C interface
            // I2C bus should already be available
            break;

        case INTERFACE_UART:
            // Configure UART interface
            // Set baud rate, parity, stop bits
            break;

        default:
            resource(COMMUNICATION_STATUS_40)->set<INT_T>(COMM_NOT_CONFIGURED);
            return false;
    }

    resource(COMMUNICATION_STATUS_40)->set<INT_T>(COMM_OK);
    notifyResChanged(COMMUNICATION_STATUS_40);
    return true;
    #else
    return false;
    #endif
}

EXECUTE_RESULT Mikrobus::executeInitClickBoard(Instance& inst, ID_T resId,
                                                 const OPAQUE_T& data) {
    WPP_LOGI("Mikrobus", "Initializing Click board on socket %d", _socketId);

    // Enable power
    resource(POWER_STATE_6)->set<BOOL_T>(true);
    notifyResChanged(POWER_STATE_6);

    // Configure active interface
    if (configureInterface()) {
        // Detect Click board
        if (detectClickBoard()) {
            resource(LAST_ERROR_41)->set<STRING_T>("Initialized successfully");
            return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
        }
    }

    resource(LAST_ERROR_41)->set<STRING_T>("Initialization failed");
    return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
}

EXECUTE_RESULT Mikrobus::executeResetClickBoard(Instance& inst, ID_T resId,
                                                  const OPAQUE_T& data) {
    WPP_LOGI("Mikrobus", "Resetting Click board on socket %d", _socketId);

    // Assert reset (active low)
    resource(RST_PIN_STATE_35)->set<BOOL_T>(false);
    notifyResChanged(RST_PIN_STATE_35);

    #ifdef OPENWRT_BUILD
    // Hold reset for 100ms
    usleep(100000);
    #endif

    // Deassert reset
    resource(RST_PIN_STATE_35)->set<BOOL_T>(true);
    notifyResChanged(RST_PIN_STATE_35);

    resource(LAST_ERROR_41)->set<STRING_T>("Reset completed");
    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
}

EXECUTE_RESULT Mikrobus::executeShutdownClickBoard(Instance& inst, ID_T resId,
                                                     const OPAQUE_T& data) {
    WPP_LOGI("Mikrobus", "Shutting down Click board on socket %d", _socketId);

    // Disable power
    resource(POWER_STATE_6)->set<BOOL_T>(false);
    notifyResChanged(POWER_STATE_6);

    // Set communication status to not configured
    resource(COMMUNICATION_STATUS_40)->set<INT_T>(COMM_NOT_CONFIGURED);
    notifyResChanged(COMMUNICATION_STATUS_40);

    resource(LAST_ERROR_41)->set<STRING_T>("Shut down");
    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
}

EXECUTE_RESULT Mikrobus::executeReadManifest(Instance& inst, ID_T resId,
                                               const OPAQUE_T& data) {
    WPP_LOGI("Mikrobus", "Reading Click board manifest from socket %d", _socketId);

    #ifdef OPENWRT_BUILD
    // Read manifest from I2C EEPROM
    // Manifest format is vendor-specific but typically includes:
    // - Manufacturer
    // - Model
    // - Serial number
    // - Capabilities

    // Simplified example - real implementation would parse full structure
    std::string manifestJson = R"({
        "manufacturer": "MikroElektronika",
        "model": "Unknown",
        "serial": "N/A",
        "capabilities": ["SPI", "I2C"]
    })";

    #if RES_10520_111
    resource(MANIFEST_DATA_111)->set<STRING_T>(manifestJson);
    notifyResChanged(MANIFEST_DATA_111);
    #endif

    resource(LAST_ERROR_41)->set<STRING_T>("Manifest read successfully");
    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
    #else
    resource(LAST_ERROR_41)->set<STRING_T>("Manifest reading not supported");
    return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
    #endif
}

} // namespace wpp
