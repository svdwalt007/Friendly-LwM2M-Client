/*
 * Walt Technologies MIKROBUS Object
 *
 * Object ID: 10520 (Walt Technologies Custom Object Range)
 *
 * Description: Comprehensive LwM2M object for managing and monitoring MIKROBUS sockets
 *              and Click boards. Provides access to interface configuration (SPI, I2C, UART),
 *              GPIO control, PWM, analog input, and Click board identification. Supports
 *              multiple instances for managing multiple MIKROBUS sockets on a device.
 *
 * MIKROBUS Standard: https://www.mikroe.com/mikrobus
 */

#ifndef MIKROBUS_H
#define MIKROBUS_H

#include "WppTypes.h"
#include "WppClient.h"
#include "data_model/Instance.h"
#include "data_model/ObjectImpl.h"

namespace wpp {

class Mikrobus : public Instance {
public:
    /* Resource IDs - Organized by functional groups */
    enum ID: ID_T {
        /* Socket Information (0-9) */
        SOCKET_ID_0 = 0,                     // R: Integer - Socket number (1-N)
        SOCKET_NAME_1 = 1,                   // RW: String - Human-readable socket name
        SOCKET_ENABLED_2 = 2,                // RW: Boolean - Socket enabled state
        CLICK_BOARD_PRESENT_3 = 3,           // R: Boolean - Click board detected
        CLICK_BOARD_TYPE_4 = 4,              // R: String - Click board type/category
        POWER_VOLTAGE_5 = 5,                 // RW: Integer - Power voltage (3300=3.3V, 5000=5V)
        POWER_STATE_6 = 6,                   // RW: Boolean - Power enabled/disabled

        /* Click Board Information (10-19) */
        BOARD_MANUFACTURER_10 = 10,          // R: String - Manufacturer name
        BOARD_MODEL_11 = 11,                 // R: String - Board model number
        BOARD_FIRMWARE_VERSION_12 = 12,      // R: String - Firmware/driver version
        BOARD_SERIAL_NUMBER_13 = 13,         // R: String - Unique serial number
        BOARD_DESCRIPTION_14 = 14,           // R: String - Board description

        /* Interface Configuration (20-29) */
        ACTIVE_INTERFACE_20 = 20,            // RW: Integer - Active interface (0=None, 1=SPI, 2=I2C, 3=UART)
        I2C_ADDRESS_21 = 21,                 // RW: Integer - I2C slave address (7-bit)
        SPI_MODE_22 = 22,                    // RW: Integer - SPI mode (0-3)
        SPI_SPEED_23 = 23,                   // RW: Integer - SPI clock speed (Hz)
        UART_BAUD_RATE_24 = 24,              // RW: Integer - UART baud rate (bps)
        UART_DATA_BITS_25 = 25,              // RW: Integer - UART data bits (7, 8)
        UART_PARITY_26 = 26,                 // RW: Integer - UART parity (0=None, 1=Odd, 2=Even)
        UART_STOP_BITS_27 = 27,              // RW: Integer - UART stop bits (1, 2)

        /* GPIO and Analog (30-39) */
        AN_VALUE_31 = 31,                    // R: Float - Analog input value (volts)
        PWM_DUTY_CYCLE_32 = 32,              // RW: Float - PWM duty cycle (0.0-100.0 %)
        PWM_FREQUENCY_33 = 33,               // RW: Integer - PWM frequency (Hz)
        INT_PIN_STATE_34 = 34,               // R: Boolean - Interrupt pin state
        RST_PIN_STATE_35 = 35,               // RW: Boolean - Reset pin state
        CS_PIN_STATE_36 = 36,                // RW: Boolean - Chip select pin state

        /* Status and Diagnostics (40-49) */
        COMMUNICATION_STATUS_40 = 40,        // R: Integer - Comm status (0=OK, 1=Error, 2=Timeout)
        LAST_ERROR_41 = 41,                  // R: String - Last error message
        TRANSACTION_COUNT_42 = 42,           // R: Integer - Total I/O transactions
        ERROR_COUNT_43 = 43,                 // R: Integer - Total error count
        UPTIME_44 = 44,                      // R: Integer - Socket uptime (seconds)

        /* Control Actions (100-109) */
        INIT_CLICK_BOARD_101 = 101,          // E: Execute - Initialize Click board
        RESET_CLICK_BOARD_102 = 102,         // E: Execute - Reset Click board
        SHUTDOWN_CLICK_BOARD_103 = 103,      // E: Execute - Shutdown Click board
        READ_MANIFEST_110 = 110,             // E: Execute - Read Click board manifest
        MANIFEST_DATA_111 = 111,             // R: String - Manifest data (JSON)
    };

    /* Interface Type Enumeration */
    enum InterfaceType: INT_T {
        INTERFACE_NONE = 0,
        INTERFACE_SPI = 1,
        INTERFACE_I2C = 2,
        INTERFACE_UART = 3,
    };

    /* Communication Status Enumeration */
    enum CommStatus: INT_T {
        COMM_OK = 0,
        COMM_ERROR = 1,
        COMM_TIMEOUT = 2,
        COMM_NOT_CONFIGURED = 3,
    };

    /**
     * @brief Create Mikrobus object
     * @param client WppClient reference
     * @return Mikrobus object
     */
    static Object& object(WppClient& client);

    /**
     * @brief Create instance of Mikrobus
     * @param client WppClient reference
     * @param instanceId Instance ID (socket number: 0, 1, 2, ...)
     * @return Pointer to created instance
     */
    static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);

    /**
     * @brief Create resources for this object
     */
    void resourcesCreate() override;

    /**
     * @brief Initialize resources with default values
     */
    void resourcesInit() override;

    /**
     * @brief Get URN for this object
     * @return URN string
     */
    static const std::string urn();

    /**
     * @brief Destructor
     */
    ~Mikrobus() override = default;

private:
    /**
     * @brief Constructor (private - use createInst)
     */
    Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);

    /**
     * @brief Execute callback for control actions
     */
    EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
    EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
    EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
    EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /**
     * @brief Update socket status and telemetry
     * @return true if successful
     */
    bool updateSocketStatus();

    /**
     * @brief Detect and identify Click board
     * @return true if board detected
     */
    bool detectClickBoard();

    /**
     * @brief Configure active interface
     */
    bool configureInterface();

    /**
     * @brief Task ID for periodic updates
     */
    WppTaskQueue::task_id_t _updateTaskId;

    /**
     * @brief Socket instance number
     */
    OBJ_INST_ID_T _socketId;
};

} // namespace wpp

#endif // MIKROBUS_H
