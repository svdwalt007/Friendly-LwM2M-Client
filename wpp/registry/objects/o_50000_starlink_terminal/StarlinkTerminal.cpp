/*
 * Walt Technologies Starlink Terminal Object - Implementation
 *
 * Provides comprehensive management and monitoring of Starlink Gen3 terminals
 * via gRPC interface (default: 192.168.100.1:9200 for dish)
 *
 * Integration Note:
 * This implementation provides the LwM2M object structure and resource definitions.
 * The gRPC integration with Starlink requires additional dependencies:
 * - gRPC C++ library
 * - Starlink protobuf definitions
 * - Platform-specific networking
 *
 * The updateTelemetry() method should be extended to call actual gRPC endpoints.
 */

#include "StarlinkTerminal.h"
#include "StarlinkTerminalInfo.h"
#include "StarlinkTerminalConfig.h"
#include "WppClient.h"
#include "WppLogs.h"
#include <cmath>
#include <sstream>
#include <iomanip>

namespace wpp {

/* Static object instance */
static ObjectImpl<StarlinkTerminal> starlinkTerminalObj(STARLINK_TERMINAL_OBJ_ID,
                                                         !STARLINK_TERMINAL_MULTIPLE_INSTANCES);

Object& StarlinkTerminal::object(WppClient& client) {
    return starlinkTerminalObj;
}

Instance* StarlinkTerminal::createInst(WppClient& client, OBJ_INST_ID_T instanceId) {
    return starlinkTerminalObj.createInst(instanceId, client, instanceId);
}

const std::string StarlinkTerminal::urn() {
    return STARLINK_TERMINAL_URN;
}

StarlinkTerminal::StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId)
    : Instance(STARLINK_TERMINAL_OBJ_ID, instanceId) {
    WPP_LOGD("StarlinkTerminal instance created: %d", instanceId);
}

void StarlinkTerminal::resourcesCreate() {
    std::vector<Resource> resources = {
        /* Device Information (0-9) */
        {DEVICE_ID_0,           ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {HARDWARE_VERSION_1,    ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {SOFTWARE_VERSION_2,    ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {CONNECTION_STATE_3,    ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {UPTIME_4,              ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},

        /* Network Performance & Status (10-29) */
        {DOWNLINK_THROUGHPUT_10,    ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {UPLINK_THROUGHPUT_11,      ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {POP_PING_LATENCY_12,       ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {POP_PING_DROP_RATE_13,     ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
#if RES_50000_14
        {SECONDS_TO_FIRST_SLOT_14,  ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_50000_15
        {SNR_15,                    ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_50000_16
        {IS_SNR_ABOVE_NOISE_FLOOR_16, ItemOp(ItemOp::READ),        IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
#if RES_50000_17
        {DOWNLOAD_USAGE_17,         ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
#endif
#if RES_50000_18
        {UPLOAD_USAGE_18,           ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
#endif

        /* Obstruction Monitoring (30-39) */
        {FRACTION_OBSTRUCTED_30,    ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {CURRENTLY_OBSTRUCTED_31,   ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
#if RES_50000_32
        {OBSTRUCTION_DURATION_32,   ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_50000_33
        {OBSTRUCTION_INTERVAL_33,   ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_50000_34
        {OBSTRUCTION_VALID_S_34,    ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_50000_35
        {WEDGES_FRACTION_OBSTRUCTED_35, ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif

        /* Dish Alignment (40-49) */
        {DIRECTION_AZIMUTH_40,      ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {DIRECTION_ELEVATION_41,    ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
#if RES_50000_42
        {DISH_STOWED_42,            ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL, TYPE_ID::BOOL},
#endif
#if RES_50000_43
        {DISH_ALIGNMENT_STATUS_43,  ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif

        /* Alert System (50-79) */
        {ALERTS_BITMAP_50,              ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {ALERT_MOTORS_STUCK_51,         ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {ALERT_THERMAL_SHUTDOWN_52,     ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {ALERT_THERMAL_THROTTLE_53,     ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {ALERT_UNEXPECTED_LOCATION_54,  ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {ALERT_MAST_NOT_VERTICAL_55,    ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {ALERT_SLOW_ETHERNET_56,        ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {ALERT_ROAMING_57,              ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {ALERT_INSTALL_PENDING_58,      ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {ALERT_IS_HEATING_59,           ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {ALERT_POWER_SUPPLY_THROTTLE_60, ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {ALERT_IS_POWER_SAVE_IDLE_61,   ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
#if RES_50000_62
        {ALERT_LOW_MOTOR_CURRENT_62,    ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
#if RES_50000_63
        {ALERT_LOWER_SIGNAL_63,         ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
#if RES_50000_64
        {ALERT_OBSTRUCTION_MAP_RESET_64, ItemOp(ItemOp::READ),      IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
#if RES_50000_65
        {ALERT_DISH_WATER_DETECTED_65,  ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
#if RES_50000_66
        {ALERT_ROUTER_WATER_DETECTED_66, ItemOp(ItemOp::READ),      IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif

        /* GPS Location (80-89) */
        {GPS_READY_80,              ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {GPS_ENABLED_81,            ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {GPS_SATELLITES_82,         ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
#if RES_50000_83
        {LATITUDE_83,               ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_50000_84
        {LONGITUDE_84,              ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_50000_85
        {ALTITUDE_85,               ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif

        /* Power Management (90-99) */
        {LATEST_POWER_90,           ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {MEAN_POWER_91,             ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
#if RES_50000_92
        {MIN_POWER_92,              ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_50000_93
        {MAX_POWER_93,              ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_50000_94
        {TOTAL_ENERGY_94,           ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif

        /* Control Actions (100-109) */
        {REBOOT_TERMINAL_100,       ItemOp(ItemOp::EXECUTE),        IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::EXECUTE},
        {START_SPEEDTEST_101,       ItemOp(ItemOp::EXECUTE),        IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::EXECUTE},
        {SPEEDTEST_STATUS_102,      ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
#if RES_50000_103
        {FACTORY_RESET_103,         ItemOp(ItemOp::EXECUTE),        IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
#endif
#if RES_50000_104
        {GET_OBSTRUCTION_MAP_104,   ItemOp(ItemOp::EXECUTE),        IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
#endif
#if RES_50000_105
        {OBSTRUCTION_MAP_DATA_105,  ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::OPAQUE},
#endif
        {REFRESH_TELEMETRY_106,     ItemOp(ItemOp::EXECUTE),        IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::EXECUTE},
        {GRPC_ENDPOINT_107,         ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},

        /* Historical Data (110-129) */
        {HISTORY_SAMPLES_110,       ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {HISTORY_INTERVAL_111,      ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {MEAN_PING_LATENCY_112,     ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {MEAN_FULL_PING_LATENCY_113, ItemOp(ItemOp::READ),          IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
#if RES_50000_114
        {STDEV_FULL_PING_LATENCY_114, ItemOp(ItemOp::READ),         IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
        {TOTAL_PING_DROP_115,       ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {COUNT_FULL_PING_DROP_116,  ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {COUNT_OBSTRUCTED_117,      ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
#if RES_50000_118
        {LATENCY_DECILES_118,       ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif
#if RES_50000_119
        {LOAD_BUCKET_STATS_119,     ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif
    };

    setupResources(std::move(resources));
}

void StarlinkTerminal::resourcesInit() {
    /* Device Information */
    set<STRING_T>(DEVICE_ID_0, "STARLINK-UNKNOWN");
    set<STRING_T>(HARDWARE_VERSION_1, "Unknown");
    set<STRING_T>(SOFTWARE_VERSION_2, "0.0.0");
    set<STRING_T>(CONNECTION_STATE_3, "UNKNOWN");
    set<INT_T>(UPTIME_4, 0);

    /* Network Performance */
    set<FLOAT_T>(DOWNLINK_THROUGHPUT_10, 0.0);
    set<FLOAT_T>(UPLINK_THROUGHPUT_11, 0.0);
    set<FLOAT_T>(POP_PING_LATENCY_12, 0.0);
    set<FLOAT_T>(POP_PING_DROP_RATE_13, 0.0);
#if RES_50000_14
    set<FLOAT_T>(SECONDS_TO_FIRST_SLOT_14, 0.0);
#endif
#if RES_50000_15
    set<FLOAT_T>(SNR_15, 0.0);
#endif
#if RES_50000_16
    set<BOOL_T>(IS_SNR_ABOVE_NOISE_FLOOR_16, false);
#endif
#if RES_50000_17
    set<INT_T>(DOWNLOAD_USAGE_17, 0);
#endif
#if RES_50000_18
    set<INT_T>(UPLOAD_USAGE_18, 0);
#endif

    /* Obstruction Monitoring */
    set<FLOAT_T>(FRACTION_OBSTRUCTED_30, 0.0);
    set<BOOL_T>(CURRENTLY_OBSTRUCTED_31, false);
#if RES_50000_32
    set<FLOAT_T>(OBSTRUCTION_DURATION_32, 0.0);
#endif
#if RES_50000_33
    set<FLOAT_T>(OBSTRUCTION_INTERVAL_33, 0.0);
#endif
#if RES_50000_34
    set<FLOAT_T>(OBSTRUCTION_VALID_S_34, 0.0);
#endif
#if RES_50000_35
    set<STRING_T>(WEDGES_FRACTION_OBSTRUCTED_35, "[]");
#endif

    /* Dish Alignment */
    set<FLOAT_T>(DIRECTION_AZIMUTH_40, 0.0);
    set<FLOAT_T>(DIRECTION_ELEVATION_41, 0.0);
#if RES_50000_42
    set<BOOL_T>(DISH_STOWED_42, false);
#endif
#if RES_50000_43
    set<STRING_T>(DISH_ALIGNMENT_STATUS_43, "Unknown");
#endif

    /* Alert System */
    set<INT_T>(ALERTS_BITMAP_50, 0);
    set<BOOL_T>(ALERT_MOTORS_STUCK_51, false);
    set<BOOL_T>(ALERT_THERMAL_SHUTDOWN_52, false);
    set<BOOL_T>(ALERT_THERMAL_THROTTLE_53, false);
    set<BOOL_T>(ALERT_UNEXPECTED_LOCATION_54, false);
    set<BOOL_T>(ALERT_MAST_NOT_VERTICAL_55, false);
    set<BOOL_T>(ALERT_SLOW_ETHERNET_56, false);
    set<BOOL_T>(ALERT_ROAMING_57, false);
    set<BOOL_T>(ALERT_INSTALL_PENDING_58, false);
    set<BOOL_T>(ALERT_IS_HEATING_59, false);
    set<BOOL_T>(ALERT_POWER_SUPPLY_THROTTLE_60, false);
    set<BOOL_T>(ALERT_IS_POWER_SAVE_IDLE_61, false);
#if RES_50000_62
    set<BOOL_T>(ALERT_LOW_MOTOR_CURRENT_62, false);
#endif
#if RES_50000_63
    set<BOOL_T>(ALERT_LOWER_SIGNAL_63, false);
#endif
#if RES_50000_64
    set<BOOL_T>(ALERT_OBSTRUCTION_MAP_RESET_64, false);
#endif
#if RES_50000_65
    set<BOOL_T>(ALERT_DISH_WATER_DETECTED_65, false);
#endif
#if RES_50000_66
    set<BOOL_T>(ALERT_ROUTER_WATER_DETECTED_66, false);
#endif

    /* GPS Location */
    set<BOOL_T>(GPS_READY_80, false);
    set<BOOL_T>(GPS_ENABLED_81, false);
    set<INT_T>(GPS_SATELLITES_82, 0);
#if RES_50000_83
    set<FLOAT_T>(LATITUDE_83, 0.0);
#endif
#if RES_50000_84
    set<FLOAT_T>(LONGITUDE_84, 0.0);
#endif
#if RES_50000_85
    set<FLOAT_T>(ALTITUDE_85, 0.0);
#endif

    /* Power Management */
    set<FLOAT_T>(LATEST_POWER_90, 0.0);
    set<FLOAT_T>(MEAN_POWER_91, 0.0);
#if RES_50000_92
    set<FLOAT_T>(MIN_POWER_92, 0.0);
#endif
#if RES_50000_93
    set<FLOAT_T>(MAX_POWER_93, 0.0);
#endif
#if RES_50000_94
    set<FLOAT_T>(TOTAL_ENERGY_94, 0.0);
#endif

    /* Control Actions - Set execute callbacks */
    setExecute(REBOOT_TERMINAL_100,
               [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
                   return executeReboot(inst, resId, data);
               });

    setExecute(START_SPEEDTEST_101,
               [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
                   return executeSpeedTest(inst, resId, data);
               });

    set<STRING_T>(SPEEDTEST_STATUS_102, "{\"status\":\"idle\"}");

#if RES_50000_103
    setExecute(FACTORY_RESET_103,
               [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
                   return executeFactoryReset(inst, resId, data);
               });
#endif

#if RES_50000_104
    setExecute(GET_OBSTRUCTION_MAP_104,
               [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
                   return executeGetObstructionMap(inst, resId, data);
               });
#endif

#if RES_50000_105
    set<OPAQUE_T>(OBSTRUCTION_MAP_DATA_105, OPAQUE_T{});
#endif

    setExecute(REFRESH_TELEMETRY_106,
               [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
                   return executeRefreshTelemetry(inst, resId, data);
               });

    set<STRING_T>(GRPC_ENDPOINT_107, "192.168.100.1:9200");

    /* Historical Data */
    set<INT_T>(HISTORY_SAMPLES_110, 0);
    set<INT_T>(HISTORY_INTERVAL_111, 1);  // 1 second default
    set<FLOAT_T>(MEAN_PING_LATENCY_112, 0.0);
    set<FLOAT_T>(MEAN_FULL_PING_LATENCY_113, 0.0);
#if RES_50000_114
    set<FLOAT_T>(STDEV_FULL_PING_LATENCY_114, 0.0);
#endif
    set<FLOAT_T>(TOTAL_PING_DROP_115, 0.0);
    set<INT_T>(COUNT_FULL_PING_DROP_116, 0);
    set<INT_T>(COUNT_OBSTRUCTED_117, 0);
#if RES_50000_118
    set<STRING_T>(LATENCY_DECILES_118, "[]");
#endif
#if RES_50000_119
    set<STRING_T>(LOAD_BUCKET_STATS_119, "{}");
#endif

    WPP_LOGD("StarlinkTerminal resources initialized");
}

/* Execute Callbacks */

EXECUTE_RESULT StarlinkTerminal::executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI("Starlink Terminal Reboot requested");

    /*
     * TODO: Implement gRPC call to Starlink
     * grpc::ClientContext context;
     * Request request;
     * request.mutable_reboot()->set_reboot(true);
     * Response response;
     * Status status = stub_->Handle(&context, request, &response);
     */

    // For now, return success - implement actual gRPC call when integrated
    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
}

EXECUTE_RESULT StarlinkTerminal::executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI("Starlink Speed Test initiated");

    // Update status to running
    set<STRING_T>(SPEEDTEST_STATUS_102, "{\"status\":\"running\",\"progress\":0}");

    /*
     * TODO: Implement gRPC call for speed test
     * grpc::ClientContext context;
     * Request request;
     * request.mutable_speed_test();
     * Response response;
     * Status status = stub_->Handle(&context, request, &response);
     *
     * Parse response and update SPEEDTEST_STATUS_102 with results
     */

    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
}

EXECUTE_RESULT StarlinkTerminal::executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGW("Starlink Terminal Factory Reset requested - this will erase all settings!");

    /*
     * TODO: Implement gRPC call for factory reset
     * WARNING: This is a destructive operation
     * grpc::ClientContext context;
     * Request request;
     * request.mutable_factory_reset();
     * Response response;
     * Status status = stub_->Handle(&context, request, &response);
     */

    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
}

EXECUTE_RESULT StarlinkTerminal::executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI("Starlink Obstruction Map retrieval requested");

    /*
     * TODO: Implement gRPC call to get obstruction map
     * grpc::ClientContext context;
     * Request request;
     * request.mutable_dish_get_obstruction_map();
     * Response response;
     * Status status = stub_->Handle(&context, request, &response);
     *
     * if (status.ok() && response.has_dish_get_obstruction_map()) {
     *     auto& map = response.dish_get_obstruction_map();
     *     // Convert to OPAQUE_T and store in OBSTRUCTION_MAP_DATA_105
     * }
     */

    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
}

EXECUTE_RESULT StarlinkTerminal::executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI("Starlink Telemetry refresh requested");

    if (updateTelemetry()) {
        return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
    } else {
        return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
    }
}

/* Telemetry Update */

bool StarlinkTerminal::updateTelemetry() {
    /*
     * TODO: Implement complete gRPC integration with Starlink
     *
     * This method should:
     * 1. Create gRPC channel to endpoint (get from GRPC_ENDPOINT_107)
     * 2. Call GetStatus() to retrieve current status
     * 3. Call GetHistory() to retrieve historical data
     * 4. Parse responses and update all resources
     *
     * Example integration:
     *
     * std::string endpoint = get<STRING_T>(GRPC_ENDPOINT_107);
     * auto channel = grpc::CreateChannel(endpoint, grpc::InsecureChannelCredentials());
     * auto stub = Device::NewStub(channel);
     *
     * // Get Status
     * grpc::ClientContext status_context;
     * Request status_request;
     * status_request.mutable_get_status();
     * Response status_response;
     * Status status = stub->Handle(&status_context, status_request, &status_response);
     *
     * if (status.ok() && status_response.has_dish_get_status()) {
     *     auto& dish_status = status_response.dish_get_status();
     *
     *     // Update device info
     *     if (dish_status.has_device_info()) {
     *         set<STRING_T>(DEVICE_ID_0, dish_status.device_info().id());
     *         set<STRING_T>(HARDWARE_VERSION_1, dish_status.device_info().hardware_version());
     *         set<STRING_T>(SOFTWARE_VERSION_2, dish_status.device_info().software_version());
     *     }
     *
     *     // Update state
     *     set<STRING_T>(CONNECTION_STATE_3, dish_status.state_name());
     *     set<INT_T>(UPTIME_4, dish_status.device_state().uptime_s());
     *
     *     // Update network performance
     *     set<FLOAT_T>(DOWNLINK_THROUGHPUT_10, dish_status.downlink_throughput_bps());
     *     set<FLOAT_T>(UPLINK_THROUGHPUT_11, dish_status.uplink_throughput_bps());
     *     set<FLOAT_T>(POP_PING_LATENCY_12, dish_status.pop_ping_latency_ms());
     *     set<FLOAT_T>(POP_PING_DROP_RATE_13, dish_status.pop_ping_drop_rate());
     *
     *     // Update obstruction data
     *     if (dish_status.has_obstruction_stats()) {
     *         set<FLOAT_T>(FRACTION_OBSTRUCTED_30, dish_status.obstruction_stats().fraction_obstructed());
     *         set<BOOL_T>(CURRENTLY_OBSTRUCTED_31, dish_status.obstruction_stats().currently_obstructed());
     *     }
     *
     *     // Update dish alignment
     *     if (dish_status.has_boresight()) {
     *         set<FLOAT_T>(DIRECTION_AZIMUTH_40, dish_status.boresight().azimuth_deg());
     *         set<FLOAT_T>(DIRECTION_ELEVATION_41, dish_status.boresight().elevation_deg());
     *     }
     *
     *     // Update alerts
     *     INT_T alerts = dish_status.alerts();
     *     set<INT_T>(ALERTS_BITMAP_50, alerts);
     *     set<BOOL_T>(ALERT_MOTORS_STUCK_51, getAlertBit(alerts, 0));
     *     set<BOOL_T>(ALERT_THERMAL_SHUTDOWN_52, getAlertBit(alerts, 1));
     *     // ... update all alert flags
     *
     *     // Update GPS
     *     if (dish_status.has_gps_stats()) {
     *         set<BOOL_T>(GPS_READY_80, dish_status.gps_stats().gps_valid());
     *         set<INT_T>(GPS_SATELLITES_82, dish_status.gps_stats().gps_sats());
     *         set<FLOAT_T>(LATITUDE_83, dish_status.gps_stats().latitude());
     *         set<FLOAT_T>(LONGITUDE_84, dish_status.gps_stats().longitude());
     *         set<FLOAT_T>(ALTITUDE_85, dish_status.gps_stats().altitude_meters());
     *     }
     *
     *     // Update power
     *     set<FLOAT_T>(LATEST_POWER_90, dish_status.power_w());
     * }
     *
     * // Get History
     * grpc::ClientContext history_context;
     * Request history_request;
     * history_request.mutable_get_history();
     * Response history_response;
     * status = stub->Handle(&history_context, history_request, &history_response);
     *
     * if (status.ok() && history_response.has_dish_get_history()) {
     *     auto& history = history_response.dish_get_history();
     *
     *     // Update historical statistics
     *     set<INT_T>(HISTORY_SAMPLES_110, history.current());
     *     // Calculate and set mean latency, packet loss statistics, etc.
     * }
     *
     * return true;
     */

    // Placeholder implementation - return false until gRPC is integrated
    WPP_LOGW("updateTelemetry() called but gRPC integration not yet implemented");
    WPP_LOGW("To enable Starlink integration:");
    WPP_LOGW("1. Add gRPC C++ dependencies to CMakeLists.txt");
    WPP_LOGW("2. Generate Starlink protobuf files from dish protoset");
    WPP_LOGW("3. Implement gRPC calls in updateTelemetry() method");
    WPP_LOGW("4. Add periodic telemetry updates in main loop");

    return false;
}

/* Helper Methods */

StarlinkTerminal::ConnState StarlinkTerminal::parseConnectionState(const std::string& state) {
    if (state == "CONNECTED") return STATE_CONNECTED;
    if (state == "BOOTING") return STATE_BOOTING;
    if (state == "SEARCHING") return STATE_SEARCHING;
    if (state == "STOWED") return STATE_STOWED;
    if (state == "THERMAL_SHUTDOWN") return STATE_THERMAL_SHUTDOWN;
    if (state == "NO_SATS") return STATE_NO_SATS;
    if (state == "OBSTRUCTED") return STATE_OBSTRUCTED;
    if (state == "NO_DOWNLINK") return STATE_NO_DOWNLINK;
    if (state == "NO_PINGS") return STATE_NO_PINGS;
    return STATE_UNKNOWN;
}

bool StarlinkTerminal::getAlertBit(INT_T bitmap, int bitPosition) {
    return (bitmap & (1 << bitPosition)) != 0;
}

} // namespace wpp
