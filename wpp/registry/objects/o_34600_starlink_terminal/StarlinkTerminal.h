/*
 * Walt Technologies Starlink Terminal Object
 *
 * Object ID: 34600 (Walt Technologies Custom Object Range)
 *
 * Description: Comprehensive LwM2M object for managing and monitoring Starlink Gen3
 *              satellite internet terminals. Provides access to telemetry, configuration,
 *              and control of Starlink dish, including obstruction monitoring, alignment,
 *              speed testing, and performance metrics.
 *
 * gRPC Interface: Connects to Starlink Dish at 192.168.100.1:9200
 *                 Connects to Starlink Router at 192.168.1.1:9000
 */

#ifndef STARLINK_TERMINAL_H
#define STARLINK_TERMINAL_H

#include "WppTypes.h"
#include "WppClient.h"
#include "data_model/Instance.h"
#include "data_model/ObjectImpl.h"

namespace wpp {

class StarlinkTerminal : public Instance {
public:
    /* Resource IDs - Organized by functional groups */
    enum ID: ID_T {
        /* Device Information (0-9) */
        DEVICE_ID_0 = 0,                         // R: String - Starlink serial number
        HARDWARE_VERSION_1 = 1,                  // R: String - Terminal hardware version
        SOFTWARE_VERSION_2 = 2,                  // R: String - Installed firmware version
        CONNECTION_STATE_3 = 3,                  // R: String - Current state (CONNECTED, SEARCHING, etc.)
        UPTIME_4 = 4,                           // R: Integer - Seconds since last reboot

        /* Network Performance & Status (10-29) */
        DOWNLINK_THROUGHPUT_10 = 10,            // R: Float - Download speed (bits/sec)
        UPLINK_THROUGHPUT_11 = 11,              // R: Float - Upload speed (bits/sec)
        POP_PING_LATENCY_12 = 12,               // R: Float - Round-trip latency (ms)
        POP_PING_DROP_RATE_13 = 13,             // R: Float - Packet loss fraction (0.0-1.0)
        SECONDS_TO_FIRST_SLOT_14 = 14,          // R: Float - Seconds until satellite available
        SNR_15 = 15,                            // R: Float - Signal-to-noise ratio
        IS_SNR_ABOVE_NOISE_FLOOR_16 = 16,       // R: Boolean - SNR > 3dB threshold
        DOWNLOAD_USAGE_17 = 17,                 // R: Integer - Total bytes downloaded
        UPLOAD_USAGE_18 = 18,                   // R: Integer - Total bytes uploaded

        /* Obstruction Monitoring (30-39) */
        FRACTION_OBSTRUCTED_30 = 30,            // R: Float - Obstruction area fraction (0.0-1.0)
        CURRENTLY_OBSTRUCTED_31 = 31,           // R: Boolean - Current obstruction status
        OBSTRUCTION_DURATION_32 = 32,           // R: Float - Avg prolonged obstruction duration (sec)
        OBSTRUCTION_INTERVAL_33 = 33,           // R: Float - Avg time between obstructions (sec)
        OBSTRUCTION_VALID_S_34 = 34,            // R: Float - Data completeness measure
        WEDGES_FRACTION_OBSTRUCTED_35 = 35,     // R: String - JSON array of 12 wedge obstructions

        /* Dish Alignment (40-49) */
        DIRECTION_AZIMUTH_40 = 40,              // R: Float - Dish pointing azimuth (degrees)
        DIRECTION_ELEVATION_41 = 41,            // R: Float - Dish pointing elevation (degrees)
        DISH_STOWED_42 = 42,                    // RW: Boolean - Stow/unstow dish
        DISH_ALIGNMENT_STATUS_43 = 43,          // R: String - Alignment status

        /* Alert System (50-79) - Individual alert flags */
        ALERTS_BITMAP_50 = 50,                  // R: Integer - Combined alerts bit field
        ALERT_MOTORS_STUCK_51 = 51,             // R: Boolean - Motors stuck alert
        ALERT_THERMAL_SHUTDOWN_52 = 52,         // R: Boolean - Thermal shutdown alert
        ALERT_THERMAL_THROTTLE_53 = 53,         // R: Boolean - Thermal throttling alert
        ALERT_UNEXPECTED_LOCATION_54 = 54,      // R: Boolean - Unexpected location alert
        ALERT_MAST_NOT_VERTICAL_55 = 55,        // R: Boolean - Mast not near vertical alert
        ALERT_SLOW_ETHERNET_56 = 56,            // R: Boolean - Slow Ethernet speeds alert
        ALERT_ROAMING_57 = 57,                  // R: Boolean - Roaming alert
        ALERT_INSTALL_PENDING_58 = 58,          // R: Boolean - Install pending alert
        ALERT_IS_HEATING_59 = 59,               // R: Boolean - Dish heating alert
        ALERT_POWER_SUPPLY_THROTTLE_60 = 60,    // R: Boolean - Power supply thermal throttle
        ALERT_IS_POWER_SAVE_IDLE_61 = 61,       // R: Boolean - Power save idle mode
        ALERT_LOW_MOTOR_CURRENT_62 = 62,        // R: Boolean - Low motor current alert
        ALERT_LOWER_SIGNAL_63 = 63,             // R: Boolean - Lower signal than predicted
        ALERT_OBSTRUCTION_MAP_RESET_64 = 64,    // R: Boolean - Obstruction map reset
        ALERT_DISH_WATER_DETECTED_65 = 65,      // R: Boolean - Water detected in dish
        ALERT_ROUTER_WATER_DETECTED_66 = 66,    // R: Boolean - Water detected in router

        /* GPS Location (80-89) */
        GPS_READY_80 = 80,                      // R: Boolean - GPS position ready
        GPS_ENABLED_81 = 81,                    // R: Boolean - GPS usage enabled
        GPS_SATELLITES_82 = 82,                 // R: Integer - Active GPS satellites count
        LATITUDE_83 = 83,                       // R: Float - Current latitude (degrees)
        LONGITUDE_84 = 84,                      // R: Float - Current longitude (degrees)
        ALTITUDE_85 = 85,                       // R: Float - Current altitude (meters)

        /* Power Management (90-99) */
        LATEST_POWER_90 = 90,                   // R: Float - Most recent power consumption (watts)
        MEAN_POWER_91 = 91,                     // R: Float - Average power consumption (watts)
        MIN_POWER_92 = 92,                      // R: Float - Minimum power (watts)
        MAX_POWER_93 = 93,                      // R: Float - Maximum power (watts)
        TOTAL_ENERGY_94 = 94,                   // R: Float - Total energy consumed (kWh)

        /* Control Actions (100-109) */
        REBOOT_TERMINAL_100 = 100,              // E: Execute - Reboot the Starlink terminal
        START_SPEEDTEST_101 = 101,              // E: Execute - Initiate speed test
        SPEEDTEST_STATUS_102 = 102,             // R: String - Speed test status/results (JSON)
        FACTORY_RESET_103 = 103,                // E: Execute - Factory reset terminal
        GET_OBSTRUCTION_MAP_104 = 104,          // E: Execute - Retrieve obstruction map
        OBSTRUCTION_MAP_DATA_105 = 105,         // R: Opaque - Obstruction map binary data
        REFRESH_TELEMETRY_106 = 106,            // E: Execute - Force telemetry refresh
        GRPC_ENDPOINT_107 = 107,                // RW: String - gRPC endpoint (default: 192.168.100.1:9200)

        /* Historical Data (110-129) */
        HISTORY_SAMPLES_110 = 110,              // R: Integer - Number of history samples available
        HISTORY_INTERVAL_111 = 111,             // R: Integer - History sample interval (seconds)
        MEAN_PING_LATENCY_112 = 112,            // R: Float - Mean latency (all samples)
        MEAN_FULL_PING_LATENCY_113 = 113,       // R: Float - Mean latency (0% loss only)
        STDEV_FULL_PING_LATENCY_114 = 114,      // R: Float - Latency std deviation
        TOTAL_PING_DROP_115 = 115,              // R: Float - Total sample intervals with loss
        COUNT_FULL_PING_DROP_116 = 116,         // R: Integer - Samples with 100% loss
        COUNT_OBSTRUCTED_117 = 117,             // R: Integer - Obstruction-marked samples
        LATENCY_DECILES_118 = 118,              // R: String - JSON array of latency deciles
        LOAD_BUCKET_STATS_119 = 119,            // R: String - JSON load bucket statistics
    };

    /* Connection State Enumeration */
    enum ConnState: INT_T {
        STATE_UNKNOWN = 0,
        STATE_CONNECTED = 1,
        STATE_BOOTING = 2,
        STATE_SEARCHING = 3,
        STATE_STOWED = 4,
        STATE_THERMAL_SHUTDOWN = 5,
        STATE_NO_SATS = 6,
        STATE_OBSTRUCTED = 7,
        STATE_NO_DOWNLINK = 8,
        STATE_NO_PINGS = 9
    };

    /* Speed Test Status */
    enum SpeedTestState: INT_T {
        SPEEDTEST_IDLE = 0,
        SPEEDTEST_RUNNING = 1,
        SPEEDTEST_COMPLETE = 2,
        SPEEDTEST_ERROR = 3
    };

    /**
     * @brief Create StarlinkTerminal object
     * @param client WppClient reference
     * @return StarlinkTerminal object
     */
    static Object& object(WppClient& client);

    /**
     * @brief Create instance of StarlinkTerminal
     * @param client WppClient reference
     * @param instanceId Instance ID (default: 0 for single instance)
     * @return Pointer to created instance
     */
    static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);

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
    ~StarlinkTerminal() override = default;

private:
    /**
     * @brief Constructor (private - use createInst)
     */
    StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);

    /**
     * @brief Execute callback for control actions
     */
    EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
    EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
    EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
    EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
    EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /**
     * @brief Update telemetry from Starlink gRPC interface
     * @return true if successful
     */
    bool updateTelemetry();

    /**
     * @brief Parse connection state string to enum
     */
    ConnState parseConnectionState(const std::string& state);

    /**
     * @brief Helper to extract alerts from bitmap
     */
    bool getAlertBit(INT_T bitmap, int bitPosition);
};

} // namespace wpp

#endif // STARLINK_TERMINAL_H
