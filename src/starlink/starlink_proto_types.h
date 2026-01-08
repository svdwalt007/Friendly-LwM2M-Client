/*
 * Starlink Protocol Buffer Type Definitions
 *
 * Type definitions for Starlink gRPC API
 * Based on SpaceX.API.Device service protobuf definitions
 *
 * Supports Gen2 and Gen3 Starlink dishes
 */

#ifndef STARLINK_PROTO_TYPES_H
#define STARLINK_PROTO_TYPES_H

#include <string>
#include <vector>
#include <cstdint>
#include <map>
#include <chrono>

namespace starlink {

/* ============================================================================
 * Connection States
 * ============================================================================ */

enum class DishState {
    UNKNOWN = 0,
    CONNECTED = 1,
    BOOTING = 2,
    SEARCHING = 3,
    STOWED = 4,
    THERMAL_SHUTDOWN = 5,
    NO_SATS = 6,
    OBSTRUCTED = 7,
    NO_DOWNLINK = 8,
    NO_PINGS = 9
};

/* ============================================================================
 * Device Information
 * ============================================================================ */

struct DeviceInfo {
    std::string id;                      // Serial number
    std::string hardware_version;        // Hardware revision
    std::string software_version;        // Firmware version
    std::string country_code;            // Country code
    uint64_t utc_offset_s;              // UTC offset in seconds
    bool bootcount_valid;               // Boot count validity
    uint32_t bootcount;                 // Boot count

    DeviceInfo() : utc_offset_s(0), bootcount_valid(false), bootcount(0) {}
};

/* ============================================================================
 * Network Performance Metrics
 * ============================================================================ */

struct NetworkMetrics {
    double downlink_throughput_bps;     // Download speed (bits/sec)
    double uplink_throughput_bps;       // Upload speed (bits/sec)
    float pop_ping_latency_ms;          // Round-trip latency (ms)
    float pop_ping_drop_rate;           // Packet loss fraction (0.0-1.0)
    float seconds_to_first_nonempty_slot; // Time to next satellite

    NetworkMetrics() : downlink_throughput_bps(0.0), uplink_throughput_bps(0.0),
                       pop_ping_latency_ms(0.0f), pop_ping_drop_rate(0.0f),
                       seconds_to_first_nonempty_slot(0.0f) {}
};

/* ============================================================================
 * Signal Quality
 * ============================================================================ */

struct SignalQuality {
    float snr;                          // Signal-to-noise ratio
    bool snr_above_noise_floor;         // SNR > 3dB threshold
    float signal_quality_pct;           // Signal quality percentage
    float eirp_exceeded_band_mask;      // EIRP band mask

    SignalQuality() : snr(0.0f), snr_above_noise_floor(false),
                      signal_quality_pct(0.0f), eirp_exceeded_band_mask(0.0f) {}
};

/* ============================================================================
 * Obstruction Statistics
 * ============================================================================ */

struct ObstructionStats {
    float fraction_obstructed;          // Overall obstruction fraction (0.0-1.0)
    bool currently_obstructed;          // Current obstruction state
    float avg_prolonged_obstruction_duration_s; // Avg obstruction duration
    float avg_prolonged_obstruction_interval_s; // Avg time between obstructions
    bool avg_prolonged_obstruction_valid;       // Stats validity
    float valid_s;                      // Time period of valid data

    // 12 wedge obstruction values (30-degree sectors)
    std::vector<float> wedge_abs_fraction_obstructed;
    std::vector<float> wedge_fraction_obstructed;

    ObstructionStats() : fraction_obstructed(0.0f), currently_obstructed(false),
                         avg_prolonged_obstruction_duration_s(0.0f),
                         avg_prolonged_obstruction_interval_s(0.0f),
                         avg_prolonged_obstruction_valid(false),
                         valid_s(0.0f) {
        wedge_abs_fraction_obstructed.resize(12, 0.0f);
        wedge_fraction_obstructed.resize(12, 0.0f);
    }
};

/* ============================================================================
 * Obstruction Map
 * ============================================================================ */

struct ObstructionMap {
    uint32_t num_rows;                  // Number of rows in map
    uint32_t num_cols;                  // Number of columns in map
    std::vector<float> snr;             // SNR values per cell
    std::vector<uint32_t> num_samples;  // Sample count per cell

    ObstructionMap() : num_rows(0), num_cols(0) {}
};

/* ============================================================================
 * Dish Alignment
 * ============================================================================ */

struct DishAlignment {
    float azimuth_deg;                  // Azimuth angle (degrees)
    float elevation_deg;                // Elevation angle (degrees)
    float tilt_deg;                     // Tilt angle (degrees)
    bool is_stowed;                     // Stow state
    std::string alignment_status;       // Alignment description

    DishAlignment() : azimuth_deg(0.0f), elevation_deg(0.0f),
                      tilt_deg(0.0f), is_stowed(false),
                      alignment_status("Unknown") {}
};

/* ============================================================================
 * GPS Statistics
 * ============================================================================ */

struct GpsStats {
    bool gps_valid;                     // GPS position valid
    bool gps_enabled;                   // GPS enabled
    uint32_t gps_sats;                  // Number of GPS satellites
    double latitude;                    // Latitude (degrees)
    double longitude;                   // Longitude (degrees)
    double altitude_meters;             // Altitude (meters)

    GpsStats() : gps_valid(false), gps_enabled(false), gps_sats(0),
                 latitude(0.0), longitude(0.0), altitude_meters(0.0) {}
};

/* ============================================================================
 * Power Metrics
 * ============================================================================ */

struct PowerMetrics {
    float power_w;                      // Current power consumption (watts)
    float poe_power_w;                  // PoE power (watts)
    float mean_power_w;                 // Average power (watts)
    float min_power_w;                  // Minimum power (watts)
    float max_power_w;                  // Maximum power (watts)
    double total_energy_wh;             // Total energy consumed (watt-hours)

    PowerMetrics() : power_w(0.0f), poe_power_w(0.0f), mean_power_w(0.0f),
                     min_power_w(0.0f), max_power_w(0.0f), total_energy_wh(0.0) {}
};

/* ============================================================================
 * Alert Flags
 * ============================================================================ */

struct AlertFlags {
    uint64_t alerts_bitmap;             // Combined alerts bit field
    bool motors_stuck;
    bool thermal_shutdown;
    bool thermal_throttle;
    bool unexpected_location;
    bool mast_not_vertical;
    bool slow_ethernet_speeds;
    bool roaming;
    bool install_pending;
    bool is_heating;
    bool power_supply_thermal_throttle;
    bool is_power_save_idle;
    bool moving_while_not_mobile;
    bool moving_fast_while_not_aviation;
    bool low_motor_current;
    bool lower_signal_than_predicted;
    bool obstruction_map_reset;
    bool dish_water_detected;
    bool router_water_detected;

    AlertFlags() : alerts_bitmap(0), motors_stuck(false), thermal_shutdown(false),
                   thermal_throttle(false), unexpected_location(false),
                   mast_not_vertical(false), slow_ethernet_speeds(false),
                   roaming(false), install_pending(false), is_heating(false),
                   power_supply_thermal_throttle(false), is_power_save_idle(false),
                   moving_while_not_mobile(false), moving_fast_while_not_aviation(false),
                   low_motor_current(false), lower_signal_than_predicted(false),
                   obstruction_map_reset(false), dish_water_detected(false),
                   router_water_detected(false) {}
};

/* ============================================================================
 * Historical Statistics
 * ============================================================================ */

struct HistoryStats {
    uint32_t current_samples;           // Number of samples in current period
    uint32_t history_period_s;          // History period (seconds)

    // Latency statistics
    float mean_ping_latency_ms;         // Mean latency (all samples)
    float mean_full_ping_latency_ms;    // Mean latency (0% loss only)
    float stdev_full_ping_latency_ms;   // Latency standard deviation
    std::vector<float> latency_deciles_ms; // 10 decile values

    // Packet loss statistics
    float total_ping_drop;              // Total sample intervals with loss
    uint32_t count_full_ping_drop;      // Samples with 100% loss
    uint32_t count_obstructed;          // Obstruction-marked samples

    // Load bucket statistics (usage intensity buckets)
    std::map<uint32_t, uint32_t> load_bucket_samples;

    // Historical arrays (time series data)
    std::vector<float> pop_ping_latency_ms;
    std::vector<float> pop_ping_drop_rate;
    std::vector<float> downlink_throughput_bps;
    std::vector<float> uplink_throughput_bps;
    std::vector<float> snr;
    std::vector<bool> scheduled;
    std::vector<bool> obstructed;

    HistoryStats() : current_samples(0), history_period_s(0),
                     mean_ping_latency_ms(0.0f), mean_full_ping_latency_ms(0.0f),
                     stdev_full_ping_latency_ms(0.0f), total_ping_drop(0.0f),
                     count_full_ping_drop(0), count_obstructed(0) {
        latency_deciles_ms.resize(10, 0.0f);
    }
};

/* ============================================================================
 * Speed Test Results
 * ============================================================================ */

enum class SpeedTestStatus {
    IDLE = 0,
    RUNNING = 1,
    COMPLETE = 2,
    ERROR = 3
};

struct SpeedTestResult {
    SpeedTestStatus status;
    float download_mbps;                // Download speed (Mbps)
    float upload_mbps;                  // Upload speed (Mbps)
    float latency_ms;                   // Latency (ms)
    std::string error_message;
    uint32_t progress_pct;              // Progress percentage

    SpeedTestResult() : status(SpeedTestStatus::IDLE), download_mbps(0.0f),
                        upload_mbps(0.0f), latency_ms(0.0f), progress_pct(0) {}
};

/* ============================================================================
 * WiFi Configuration (for Starlink Router)
 * ============================================================================ */

struct WiFiConfig {
    std::string ssid;
    std::string password;
    bool is_guest;
    bool is_enabled;
    std::string channel;
    std::string bandwidth;

    WiFiConfig() : is_guest(false), is_enabled(true) {}
};

/* ============================================================================
 * Complete Dish Status
 * ============================================================================ */

struct DishStatus {
    DeviceInfo device_info;
    DishState state;
    std::string state_name;
    uint64_t uptime_s;                  // Uptime in seconds

    NetworkMetrics network;
    SignalQuality signal;
    ObstructionStats obstruction;
    DishAlignment alignment;
    GpsStats gps;
    PowerMetrics power;
    AlertFlags alerts;

    bool is_dev_build;                  // Development firmware flag
    std::chrono::system_clock::time_point timestamp;

    DishStatus() : state(DishState::UNKNOWN), state_name("UNKNOWN"),
                   uptime_s(0), is_dev_build(false),
                   timestamp(std::chrono::system_clock::now()) {}
};

/* ============================================================================
 * Complete Dish Configuration
 * ============================================================================ */

struct DishConfig {
    std::string dish_mode;              // Operational mode
    bool apply_dish_mode;               // Apply mode change
    bool dish_power_save;               // Power save mode
    bool power_save_start_minutes;      // Power save schedule start
    bool power_save_duration_minutes;   // Power save duration
    bool power_save_mode;               // Power save enabled

    DishConfig() : apply_dish_mode(false), dish_power_save(false),
                   power_save_start_minutes(false), power_save_duration_minutes(false),
                   power_save_mode(false) {}
};

/* ============================================================================
 * Connection Configuration
 * ============================================================================ */

struct ConnectionConfig {
    std::string endpoint;               // gRPC endpoint (default: 192.168.100.1:9200)
    uint32_t timeout_ms;                // Request timeout (milliseconds)
    uint32_t retry_attempts;            // Number of retry attempts
    uint32_t retry_delay_ms;            // Delay between retries (milliseconds)
    uint32_t max_backoff_ms;            // Maximum backoff delay

    ConnectionConfig() : endpoint("192.168.100.1:9200"), timeout_ms(5000),
                         retry_attempts(3), retry_delay_ms(1000), max_backoff_ms(30000) {}
};

} // namespace starlink

#endif // STARLINK_PROTO_TYPES_H
