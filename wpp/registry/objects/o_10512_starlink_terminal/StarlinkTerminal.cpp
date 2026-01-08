/*
 * Walt Technologies Starlink Terminal Object - Implementation
 *
 * Provides comprehensive management and monitoring of Starlink Gen3 terminals
 * via gRPC interface (default: 192.168.100.1:9200 for dish)
 *
 * Integration Note:
 * This implementation integrates with the Starlink gRPC client library
 * located in src/starlink/ for real-time communication with Starlink dishes.
 */

#include "StarlinkTerminal.h"
#include "StarlinkTerminalInfo.h"
#include "StarlinkTerminalConfig.h"
#include "WppClient.h"
#include "WppLogs.h"

// Include Starlink gRPC client headers
#ifdef WITH_STARLINK_GRPC_INTEGRATION
#include "starlink/starlink_grpc_client.h"
#include "starlink/starlink_data_collector.h"
#endif

#include <cmath>
#include <sstream>
#include <iomanip>
#include <memory>
#include <cstring>

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

#ifdef WITH_STARLINK_GRPC_INTEGRATION
    // Initialize gRPC client with default configuration
    initGrpcClient();
#endif
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
#if RES_10512_14
        {SECONDS_TO_FIRST_SLOT_14,  ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10512_15
        {SNR_15,                    ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10512_16
        {IS_SNR_ABOVE_NOISE_FLOOR_16, ItemOp(ItemOp::READ),        IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
#if RES_10512_17
        {DOWNLOAD_USAGE_17,         ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
#endif
#if RES_10512_18
        {UPLOAD_USAGE_18,           ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
#endif

        /* Obstruction Monitoring (30-39) */
        {FRACTION_OBSTRUCTED_30,    ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {CURRENTLY_OBSTRUCTED_31,   ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
#if RES_10512_32
        {OBSTRUCTION_DURATION_32,   ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10512_33
        {OBSTRUCTION_INTERVAL_33,   ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10512_34
        {OBSTRUCTION_VALID_S_34,    ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10512_35
        {WEDGES_FRACTION_OBSTRUCTED_35, ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif

        /* Dish Alignment (40-49) */
        {DIRECTION_AZIMUTH_40,      ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {DIRECTION_ELEVATION_41,    ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
#if RES_10512_42
        {DISH_STOWED_42,            ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL, TYPE_ID::BOOL},
#endif
#if RES_10512_43
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
#if RES_10512_62
        {ALERT_LOW_MOTOR_CURRENT_62,    ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
#if RES_10512_63
        {ALERT_LOWER_SIGNAL_63,         ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
#if RES_10512_64
        {ALERT_OBSTRUCTION_MAP_RESET_64, ItemOp(ItemOp::READ),      IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
#if RES_10512_65
        {ALERT_DISH_WATER_DETECTED_65,  ItemOp(ItemOp::READ),       IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif
#if RES_10512_66
        {ALERT_ROUTER_WATER_DETECTED_66, ItemOp(ItemOp::READ),      IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
#endif

        /* GPS Location (80-89) */
        {GPS_READY_80,              ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {GPS_ENABLED_81,            ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {GPS_SATELLITES_82,         ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
#if RES_10512_83
        {LATITUDE_83,               ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10512_84
        {LONGITUDE_84,              ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10512_85
        {ALTITUDE_85,               ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif

        /* Power Management (90-99) */
        {LATEST_POWER_90,           ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {MEAN_POWER_91,             ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
#if RES_10512_92
        {MIN_POWER_92,              ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10512_93
        {MAX_POWER_93,              ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
#if RES_10512_94
        {TOTAL_ENERGY_94,           ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif

        /* Control Actions (100-109) */
        {REBOOT_TERMINAL_100,       ItemOp(ItemOp::EXECUTE),        IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::EXECUTE},
        {START_SPEEDTEST_101,       ItemOp(ItemOp::EXECUTE),        IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::EXECUTE},
        {SPEEDTEST_STATUS_102,      ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
#if RES_10512_103
        {FACTORY_RESET_103,         ItemOp(ItemOp::EXECUTE),        IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
#endif
#if RES_10512_104
        {GET_OBSTRUCTION_MAP_104,   ItemOp(ItemOp::EXECUTE),        IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
#endif
#if RES_10512_105
        {OBSTRUCTION_MAP_DATA_105,  ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::OPAQUE},
#endif
        {REFRESH_TELEMETRY_106,     ItemOp(ItemOp::EXECUTE),        IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::EXECUTE},
        {GRPC_ENDPOINT_107,         ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},

        /* Historical Data (110-129) */
        {HISTORY_SAMPLES_110,       ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {HISTORY_INTERVAL_111,      ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {MEAN_PING_LATENCY_112,     ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {MEAN_FULL_PING_LATENCY_113, ItemOp(ItemOp::READ),          IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
#if RES_10512_114
        {STDEV_FULL_PING_LATENCY_114, ItemOp(ItemOp::READ),         IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
#endif
        {TOTAL_PING_DROP_115,       ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {COUNT_FULL_PING_DROP_116,  ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {COUNT_OBSTRUCTED_117,      ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT},
#if RES_10512_118
        {LATENCY_DECILES_118,       ItemOp(ItemOp::READ),           IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
#endif
#if RES_10512_119
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
#if RES_10512_14
    set<FLOAT_T>(SECONDS_TO_FIRST_SLOT_14, 0.0);
#endif
#if RES_10512_15
    set<FLOAT_T>(SNR_15, 0.0);
#endif
#if RES_10512_16
    set<BOOL_T>(IS_SNR_ABOVE_NOISE_FLOOR_16, false);
#endif
#if RES_10512_17
    set<INT_T>(DOWNLOAD_USAGE_17, 0);
#endif
#if RES_10512_18
    set<INT_T>(UPLOAD_USAGE_18, 0);
#endif

    /* Obstruction Monitoring */
    set<FLOAT_T>(FRACTION_OBSTRUCTED_30, 0.0);
    set<BOOL_T>(CURRENTLY_OBSTRUCTED_31, false);
#if RES_10512_32
    set<FLOAT_T>(OBSTRUCTION_DURATION_32, 0.0);
#endif
#if RES_10512_33
    set<FLOAT_T>(OBSTRUCTION_INTERVAL_33, 0.0);
#endif
#if RES_10512_34
    set<FLOAT_T>(OBSTRUCTION_VALID_S_34, 0.0);
#endif
#if RES_10512_35
    set<STRING_T>(WEDGES_FRACTION_OBSTRUCTED_35, "[]");
#endif

    /* Dish Alignment */
    set<FLOAT_T>(DIRECTION_AZIMUTH_40, 0.0);
    set<FLOAT_T>(DIRECTION_ELEVATION_41, 0.0);
#if RES_10512_42
    set<BOOL_T>(DISH_STOWED_42, false);
#endif
#if RES_10512_43
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
#if RES_10512_62
    set<BOOL_T>(ALERT_LOW_MOTOR_CURRENT_62, false);
#endif
#if RES_10512_63
    set<BOOL_T>(ALERT_LOWER_SIGNAL_63, false);
#endif
#if RES_10512_64
    set<BOOL_T>(ALERT_OBSTRUCTION_MAP_RESET_64, false);
#endif
#if RES_10512_65
    set<BOOL_T>(ALERT_DISH_WATER_DETECTED_65, false);
#endif
#if RES_10512_66
    set<BOOL_T>(ALERT_ROUTER_WATER_DETECTED_66, false);
#endif

    /* GPS Location */
    set<BOOL_T>(GPS_READY_80, false);
    set<BOOL_T>(GPS_ENABLED_81, false);
    set<INT_T>(GPS_SATELLITES_82, 0);
#if RES_10512_83
    set<FLOAT_T>(LATITUDE_83, 0.0);
#endif
#if RES_10512_84
    set<FLOAT_T>(LONGITUDE_84, 0.0);
#endif
#if RES_10512_85
    set<FLOAT_T>(ALTITUDE_85, 0.0);
#endif

    /* Power Management */
    set<FLOAT_T>(LATEST_POWER_90, 0.0);
    set<FLOAT_T>(MEAN_POWER_91, 0.0);
#if RES_10512_92
    set<FLOAT_T>(MIN_POWER_92, 0.0);
#endif
#if RES_10512_93
    set<FLOAT_T>(MAX_POWER_93, 0.0);
#endif
#if RES_10512_94
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

#if RES_10512_103
    setExecute(FACTORY_RESET_103,
               [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
                   return executeFactoryReset(inst, resId, data);
               });
#endif

#if RES_10512_104
    setExecute(GET_OBSTRUCTION_MAP_104,
               [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
                   return executeGetObstructionMap(inst, resId, data);
               });
#endif

#if RES_10512_105
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
#if RES_10512_114
    set<FLOAT_T>(STDEV_FULL_PING_LATENCY_114, 0.0);
#endif
    set<FLOAT_T>(TOTAL_PING_DROP_115, 0.0);
    set<INT_T>(COUNT_FULL_PING_DROP_116, 0);
    set<INT_T>(COUNT_OBSTRUCTED_117, 0);
#if RES_10512_118
    set<STRING_T>(LATENCY_DECILES_118, "[]");
#endif
#if RES_10512_119
    set<STRING_T>(LOAD_BUCKET_STATS_119, "{}");
#endif

    WPP_LOGD("StarlinkTerminal resources initialized");
}

/* Execute Callbacks */

EXECUTE_RESULT StarlinkTerminal::executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI("Starlink Terminal Reboot requested");

#ifdef WITH_STARLINK_GRPC_INTEGRATION
    if (!grpc_client_ || !grpc_client_->isConnected()) {
        WPP_LOGE("Starlink gRPC client not connected");
        return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
    }

    std::string error_msg;
    if (!grpc_client_->reboot(error_msg)) {
        WPP_LOGE("Failed to reboot Starlink dish: %s", error_msg.c_str());
        return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
    }

    WPP_LOGI("Starlink dish reboot command sent successfully");
    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
#else
    WPP_LOGW("Starlink gRPC integration not available");
    return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
#endif
}

EXECUTE_RESULT StarlinkTerminal::executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI("Starlink Speed Test initiated");

    // Update status to running
    set<STRING_T>(SPEEDTEST_STATUS_102, "{\"status\":\"running\",\"progress\":0}");

#ifdef WITH_STARLINK_GRPC_INTEGRATION
    if (!grpc_client_ || !grpc_client_->isConnected()) {
        WPP_LOGE("Starlink gRPC client not connected");
        set<STRING_T>(SPEEDTEST_STATUS_102, "{\"status\":\"error\",\"error\":\"Not connected\"}");
        return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
    }

    // Start speed test asynchronously
    grpc_client_->startSpeedTestAsync([this](bool success, const starlink::SpeedTestResult& result, const std::string& error) {
        if (success) {
            std::stringstream status_json;
            status_json << "{\"status\":\"complete\","
                       << "\"download_mbps\":" << result.download_mbps << ","
                       << "\"upload_mbps\":" << result.upload_mbps << ","
                       << "\"latency_ms\":" << result.latency_ms << "}";
            set<STRING_T>(SPEEDTEST_STATUS_102, status_json.str());
            WPP_LOGI("Speed test complete: %.2f Mbps down, %.2f Mbps up",
                    result.download_mbps, result.upload_mbps);
        } else {
            std::stringstream status_json;
            status_json << "{\"status\":\"error\",\"error\":\"" << error << "\"}";
            set<STRING_T>(SPEEDTEST_STATUS_102, status_json.str());
            WPP_LOGE("Speed test failed: %s", error.c_str());
        }
    });

    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
#else
    set<STRING_T>(SPEEDTEST_STATUS_102, "{\"status\":\"error\",\"error\":\"gRPC not available\"}");
    return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
#endif
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

#ifdef WITH_STARLINK_GRPC_INTEGRATION
    if (!grpc_client_ || !grpc_client_->isConnected()) {
        WPP_LOGE("Starlink gRPC client not connected");
        return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
    }

    starlink::ObstructionMap map;
    std::string error_msg;

    if (!grpc_client_->getObstructionMap(map, error_msg)) {
        WPP_LOGE("Failed to get obstruction map: %s", error_msg.c_str());
        return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
    }

#if RES_10512_105
    // Convert obstruction map to binary data
    // Format: [num_rows (4 bytes)][num_cols (4 bytes)][snr data][samples data]
    size_t data_size = 8 + (map.snr.size() * sizeof(float)) + (map.num_samples.size() * sizeof(uint32_t));
    OPAQUE_T map_data;
    map_data.resize(data_size);

    uint8_t* ptr = map_data.data();

    // Write dimensions
    *reinterpret_cast<uint32_t*>(ptr) = map.num_rows;
    ptr += 4;
    *reinterpret_cast<uint32_t*>(ptr) = map.num_cols;
    ptr += 4;

    // Write SNR data
    std::memcpy(ptr, map.snr.data(), map.snr.size() * sizeof(float));
    ptr += map.snr.size() * sizeof(float);

    // Write sample counts
    std::memcpy(ptr, map.num_samples.data(), map.num_samples.size() * sizeof(uint32_t));

    set<OPAQUE_T>(OBSTRUCTION_MAP_DATA_105, map_data);
    WPP_LOGI("Obstruction map retrieved: %ux%u cells", map.num_rows, map.num_cols);
#endif

    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
#else
    WPP_LOGW("Starlink gRPC integration not available");
    return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
#endif
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
#ifdef WITH_STARLINK_GRPC_INTEGRATION
    if (!grpc_client_ || !grpc_client_->isConnected()) {
        WPP_LOGW("Starlink gRPC client not connected, attempting to reconnect...");
        if (!initGrpcClient()) {
            return false;
        }
    }

    // Get current status from gRPC client
    starlink::DishStatus dish_status;
    std::string error_msg;

    if (!grpc_client_->getStatus(dish_status, error_msg)) {
        WPP_LOGE("Failed to get Starlink status: %s", error_msg.c_str());
        return false;
    }

    // Update device information
    set<STRING_T>(DEVICE_ID_0, dish_status.device_info.id);
    set<STRING_T>(HARDWARE_VERSION_1, dish_status.device_info.hardware_version);
    set<STRING_T>(SOFTWARE_VERSION_2, dish_status.device_info.software_version);
    set<STRING_T>(CONNECTION_STATE_3, dish_status.state_name);
    set<INT_T>(UPTIME_4, static_cast<INT_T>(dish_status.uptime_s));

    // Update network performance
    set<FLOAT_T>(DOWNLINK_THROUGHPUT_10, static_cast<FLOAT_T>(dish_status.network.downlink_throughput_bps));
    set<FLOAT_T>(UPLINK_THROUGHPUT_11, static_cast<FLOAT_T>(dish_status.network.uplink_throughput_bps));
    set<FLOAT_T>(POP_PING_LATENCY_12, dish_status.network.pop_ping_latency_ms);
    set<FLOAT_T>(POP_PING_DROP_RATE_13, dish_status.network.pop_ping_drop_rate);

#if RES_10512_14
    set<FLOAT_T>(SECONDS_TO_FIRST_SLOT_14, dish_status.network.seconds_to_first_nonempty_slot);
#endif
#if RES_10512_15
    set<FLOAT_T>(SNR_15, dish_status.signal.snr);
#endif
#if RES_10512_16
    set<BOOL_T>(IS_SNR_ABOVE_NOISE_FLOOR_16, dish_status.signal.snr_above_noise_floor);
#endif

    // Update obstruction monitoring
    set<FLOAT_T>(FRACTION_OBSTRUCTED_30, dish_status.obstruction.fraction_obstructed);
    set<BOOL_T>(CURRENTLY_OBSTRUCTED_31, dish_status.obstruction.currently_obstructed);

#if RES_10512_32
    set<FLOAT_T>(OBSTRUCTION_DURATION_32, dish_status.obstruction.avg_prolonged_obstruction_duration_s);
#endif
#if RES_10512_33
    set<FLOAT_T>(OBSTRUCTION_INTERVAL_33, dish_status.obstruction.avg_prolonged_obstruction_interval_s);
#endif
#if RES_10512_34
    set<FLOAT_T>(OBSTRUCTION_VALID_S_34, dish_status.obstruction.valid_s);
#endif
#if RES_10512_35
    // Convert wedge data to JSON array
    std::stringstream wedges_json;
    wedges_json << "[";
    for (size_t i = 0; i < dish_status.obstruction.wedge_fraction_obstructed.size(); i++) {
        if (i > 0) wedges_json << ",";
        wedges_json << dish_status.obstruction.wedge_fraction_obstructed[i];
    }
    wedges_json << "]";
    set<STRING_T>(WEDGES_FRACTION_OBSTRUCTED_35, wedges_json.str());
#endif

    // Update dish alignment
    set<FLOAT_T>(DIRECTION_AZIMUTH_40, dish_status.alignment.azimuth_deg);
    set<FLOAT_T>(DIRECTION_ELEVATION_41, dish_status.alignment.elevation_deg);
#if RES_10512_42
    set<BOOL_T>(DISH_STOWED_42, dish_status.alignment.is_stowed);
#endif
#if RES_10512_43
    set<STRING_T>(DISH_ALIGNMENT_STATUS_43, dish_status.alignment.alignment_status);
#endif

    // Update alert system
    set<INT_T>(ALERTS_BITMAP_50, static_cast<INT_T>(dish_status.alerts.alerts_bitmap));
    set<BOOL_T>(ALERT_MOTORS_STUCK_51, dish_status.alerts.motors_stuck);
    set<BOOL_T>(ALERT_THERMAL_SHUTDOWN_52, dish_status.alerts.thermal_shutdown);
    set<BOOL_T>(ALERT_THERMAL_THROTTLE_53, dish_status.alerts.thermal_throttle);
    set<BOOL_T>(ALERT_UNEXPECTED_LOCATION_54, dish_status.alerts.unexpected_location);
    set<BOOL_T>(ALERT_MAST_NOT_VERTICAL_55, dish_status.alerts.mast_not_vertical);
    set<BOOL_T>(ALERT_SLOW_ETHERNET_56, dish_status.alerts.slow_ethernet_speeds);
    set<BOOL_T>(ALERT_ROAMING_57, dish_status.alerts.roaming);
    set<BOOL_T>(ALERT_INSTALL_PENDING_58, dish_status.alerts.install_pending);
    set<BOOL_T>(ALERT_IS_HEATING_59, dish_status.alerts.is_heating);
    set<BOOL_T>(ALERT_POWER_SUPPLY_THROTTLE_60, dish_status.alerts.power_supply_thermal_throttle);
    set<BOOL_T>(ALERT_IS_POWER_SAVE_IDLE_61, dish_status.alerts.is_power_save_idle);

#if RES_10512_62
    set<BOOL_T>(ALERT_LOW_MOTOR_CURRENT_62, dish_status.alerts.low_motor_current);
#endif
#if RES_10512_63
    set<BOOL_T>(ALERT_LOWER_SIGNAL_63, dish_status.alerts.lower_signal_than_predicted);
#endif
#if RES_10512_64
    set<BOOL_T>(ALERT_OBSTRUCTION_MAP_RESET_64, dish_status.alerts.obstruction_map_reset);
#endif
#if RES_10512_65
    set<BOOL_T>(ALERT_DISH_WATER_DETECTED_65, dish_status.alerts.dish_water_detected);
#endif
#if RES_10512_66
    set<BOOL_T>(ALERT_ROUTER_WATER_DETECTED_66, dish_status.alerts.router_water_detected);
#endif

    // Update GPS location
    set<BOOL_T>(GPS_READY_80, dish_status.gps.gps_valid);
    set<BOOL_T>(GPS_ENABLED_81, dish_status.gps.gps_enabled);
    set<INT_T>(GPS_SATELLITES_82, static_cast<INT_T>(dish_status.gps.gps_sats));

#if RES_10512_83
    set<FLOAT_T>(LATITUDE_83, static_cast<FLOAT_T>(dish_status.gps.latitude));
#endif
#if RES_10512_84
    set<FLOAT_T>(LONGITUDE_84, static_cast<FLOAT_T>(dish_status.gps.longitude));
#endif
#if RES_10512_85
    set<FLOAT_T>(ALTITUDE_85, static_cast<FLOAT_T>(dish_status.gps.altitude_meters));
#endif

    // Update power management
    set<FLOAT_T>(LATEST_POWER_90, dish_status.power.power_w);
    set<FLOAT_T>(MEAN_POWER_91, dish_status.power.mean_power_w);

#if RES_10512_92
    set<FLOAT_T>(MIN_POWER_92, dish_status.power.min_power_w);
#endif
#if RES_10512_93
    set<FLOAT_T>(MAX_POWER_93, dish_status.power.max_power_w);
#endif
#if RES_10512_94
    set<FLOAT_T>(TOTAL_ENERGY_94, static_cast<FLOAT_T>(dish_status.power.total_energy_wh / 1000.0)); // Convert Wh to kWh
#endif

    // Get historical data
    starlink::HistoryStats history;
    if (grpc_client_->getHistory(history, error_msg)) {
        set<INT_T>(HISTORY_SAMPLES_110, static_cast<INT_T>(history.current_samples));
        set<INT_T>(HISTORY_INTERVAL_111, static_cast<INT_T>(history.history_period_s));
        set<FLOAT_T>(MEAN_PING_LATENCY_112, history.mean_ping_latency_ms);
        set<FLOAT_T>(MEAN_FULL_PING_LATENCY_113, history.mean_full_ping_latency_ms);

#if RES_10512_114
        set<FLOAT_T>(STDEV_FULL_PING_LATENCY_114, history.stdev_full_ping_latency_ms);
#endif

        set<FLOAT_T>(TOTAL_PING_DROP_115, history.total_ping_drop);
        set<INT_T>(COUNT_FULL_PING_DROP_116, static_cast<INT_T>(history.count_full_ping_drop));
        set<INT_T>(COUNT_OBSTRUCTED_117, static_cast<INT_T>(history.count_obstructed));

#if RES_10512_118
        // Convert latency deciles to JSON array
        std::stringstream deciles_json;
        deciles_json << "[";
        for (size_t i = 0; i < history.latency_deciles_ms.size(); i++) {
            if (i > 0) deciles_json << ",";
            deciles_json << history.latency_deciles_ms[i];
        }
        deciles_json << "]";
        set<STRING_T>(LATENCY_DECILES_118, deciles_json.str());
#endif
    }

    WPP_LOGD("Starlink telemetry updated successfully");
    return true;

#else
    // gRPC integration not compiled in
    WPP_LOGW("Starlink gRPC integration not compiled");
    WPP_LOGW("To enable real Starlink integration:");
    WPP_LOGW("1. Extract Starlink proto files to src/starlink/proto/");
    WPP_LOGW("2. Rebuild with: cmake -DWITH_STARLINK_GRPC=ON -DWITH_STARLINK_GRPC_INTEGRATION=ON");
    WPP_LOGW("3. See src/starlink/proto/README.md for details");

    return false;
#endif
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

/* gRPC Client Management */

bool StarlinkTerminal::initGrpcClient() {
#ifdef WITH_STARLINK_GRPC_INTEGRATION
    if (grpc_client_ && grpc_client_->isConnected()) {
        return true;
    }

    try {
        // Get endpoint from resource
        std::string endpoint = get<STRING_T>(GRPC_ENDPOINT_107);

        // Create connection configuration
        starlink::ConnectionConfig config;
        config.endpoint = endpoint;
        config.timeout_ms = 5000;
        config.retry_attempts = 3;
        config.retry_delay_ms = 1000;

        // Create gRPC client
        grpc_client_ = std::make_shared<starlink::StarlinkGrpcClient>(config);

        // Connect to Starlink dish
        if (!grpc_client_->connect()) {
            WPP_LOGE("Failed to connect to Starlink dish at %s", endpoint.c_str());
            grpc_client_.reset();
            return false;
        }

        // Create data collector with 5-second polling
        starlink::CollectionConfig collector_config;
        collector_config.status_interval_ms = 5000;   // 5 seconds
        collector_config.history_interval_ms = 60000; // 1 minute
        collector_config.enable_aggregation = true;

        data_collector_ = std::make_shared<starlink::StarlinkDataCollector>(grpc_client_, collector_config);

        // Start data collection
        if (!data_collector_->start()) {
            WPP_LOGW("Failed to start Starlink data collector");
        }

        WPP_LOGI("Starlink gRPC client connected to %s", endpoint.c_str());
        return true;

    } catch (const std::exception& e) {
        WPP_LOGE("Exception initializing Starlink gRPC client: %s", e.what());
        grpc_client_.reset();
        data_collector_.reset();
        return false;
    }
#else
    return false;
#endif
}

void StarlinkTerminal::shutdownGrpcClient() {
#ifdef WITH_STARLINK_GRPC_INTEGRATION
    if (data_collector_) {
        data_collector_->stop();
        data_collector_.reset();
    }

    if (grpc_client_) {
        grpc_client_->disconnect();
        grpc_client_.reset();
    }

    WPP_LOGD("Starlink gRPC client shutdown");
#endif
}

} // namespace wpp
