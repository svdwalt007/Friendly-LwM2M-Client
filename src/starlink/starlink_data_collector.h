/*
 * Starlink Data Collector
 *
 * Periodic data collection service for Starlink terminals
 * Integrates with LwM2M observe notifications and metric aggregation
 *
 * Features:
 * - Configurable polling intervals
 * - Metric aggregation and averaging
 * - Historical data storage with ring buffer
 * - LwM2M observer notification support
 * - Automatic error recovery
 */

#ifndef STARLINK_DATA_COLLECTOR_H
#define STARLINK_DATA_COLLECTOR_H

#include "starlink_grpc_client.h"
#include "starlink_proto_types.h"
#include <memory>
#include <functional>
#include <vector>
#include <deque>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

namespace starlink {

/* ============================================================================
 * Collection Configuration
 * ============================================================================ */

struct CollectionConfig {
    uint32_t status_interval_ms;        // Status polling interval (default: 5000 ms)
    uint32_t history_interval_ms;       // History polling interval (default: 60000 ms)
    uint32_t max_history_samples;       // Maximum history samples to store (default: 3600)
    bool enable_aggregation;            // Enable metric aggregation (default: true)
    uint32_t aggregation_window_s;      // Aggregation window in seconds (default: 60)

    CollectionConfig()
        : status_interval_ms(5000)
        , history_interval_ms(60000)
        , max_history_samples(3600)
        , enable_aggregation(true)
        , aggregation_window_s(60) {}
};

/* ============================================================================
 * Aggregated Metrics
 * ============================================================================ */

struct AggregatedMetrics {
    // Network performance averages
    double avg_downlink_throughput_bps;
    double avg_uplink_throughput_bps;
    float avg_latency_ms;
    float avg_packet_loss;
    float min_latency_ms;
    float max_latency_ms;

    // Signal quality averages
    float avg_snr;
    float min_snr;
    float max_snr;

    // Obstruction statistics
    float avg_obstruction_fraction;
    uint32_t obstruction_count;

    // Power statistics
    float avg_power_w;
    float min_power_w;
    float max_power_w;

    // Sample statistics
    uint32_t sample_count;
    std::chrono::system_clock::time_point window_start;
    std::chrono::system_clock::time_point window_end;

    AggregatedMetrics()
        : avg_downlink_throughput_bps(0.0)
        , avg_uplink_throughput_bps(0.0)
        , avg_latency_ms(0.0f)
        , avg_packet_loss(0.0f)
        , min_latency_ms(999999.0f)
        , max_latency_ms(0.0f)
        , avg_snr(0.0f)
        , min_snr(999999.0f)
        , max_snr(0.0f)
        , avg_obstruction_fraction(0.0f)
        , obstruction_count(0)
        , avg_power_w(0.0f)
        , min_power_w(999999.0f)
        , max_power_w(0.0f)
        , sample_count(0)
        , window_start(std::chrono::system_clock::now())
        , window_end(std::chrono::system_clock::now()) {}
};

/* ============================================================================
 * Observer Callback Types
 * ============================================================================ */

using StatusUpdateCallback = std::function<void(const DishStatus& status)>;
using HistoryUpdateCallback = std::function<void(const HistoryStats& history)>;
using MetricsUpdateCallback = std::function<void(const AggregatedMetrics& metrics)>;
using ErrorCallback = std::function<void(const std::string& error)>;

/* ============================================================================
 * StarlinkDataCollector Class
 * ============================================================================ */

class StarlinkDataCollector {
public:
    /**
     * @brief Constructor
     * @param client Shared pointer to gRPC client
     * @param config Collection configuration
     */
    explicit StarlinkDataCollector(
        std::shared_ptr<StarlinkGrpcClient> client,
        const CollectionConfig& config = CollectionConfig()
    );

    /**
     * @brief Destructor
     */
    ~StarlinkDataCollector();

    // Delete copy constructor and assignment operator
    StarlinkDataCollector(const StarlinkDataCollector&) = delete;
    StarlinkDataCollector& operator=(const StarlinkDataCollector&) = delete;

    /**
     * @brief Start data collection
     * @return true if started successfully
     */
    bool start();

    /**
     * @brief Stop data collection
     */
    void stop();

    /**
     * @brief Check if collector is running
     * @return true if running
     */
    bool isRunning() const;

    /* ========================================================================
     * Observer Registration
     * ======================================================================== */

    /**
     * @brief Register callback for status updates
     * @param callback Callback function
     * @return Observer ID (for unregistration)
     */
    uint32_t registerStatusObserver(StatusUpdateCallback callback);

    /**
     * @brief Register callback for history updates
     * @param callback Callback function
     * @return Observer ID
     */
    uint32_t registerHistoryObserver(HistoryUpdateCallback callback);

    /**
     * @brief Register callback for aggregated metrics updates
     * @param callback Callback function
     * @return Observer ID
     */
    uint32_t registerMetricsObserver(MetricsUpdateCallback callback);

    /**
     * @brief Register callback for errors
     * @param callback Callback function
     * @return Observer ID
     */
    uint32_t registerErrorObserver(ErrorCallback callback);

    /**
     * @brief Unregister an observer
     * @param observer_id Observer ID from registration
     */
    void unregisterObserver(uint32_t observer_id);

    /* ========================================================================
     * Data Access
     * ======================================================================== */

    /**
     * @brief Get latest status
     * @return Latest dish status
     */
    DishStatus getLatestStatus() const;

    /**
     * @brief Get latest history stats
     * @return Latest history statistics
     */
    HistoryStats getLatestHistory() const;

    /**
     * @brief Get current aggregated metrics
     * @return Current aggregated metrics
     */
    AggregatedMetrics getCurrentMetrics() const;

    /**
     * @brief Get historical status samples
     * @param count Number of samples to retrieve (0 = all)
     * @return Vector of historical status samples
     */
    std::vector<DishStatus> getHistoricalStatus(uint32_t count = 0) const;

    /**
     * @brief Get aggregated metrics history
     * @param count Number of aggregation windows to retrieve (0 = all)
     * @return Vector of aggregated metrics
     */
    std::vector<AggregatedMetrics> getMetricsHistory(uint32_t count = 0) const;

    /* ========================================================================
     * Configuration
     * ======================================================================== */

    /**
     * @brief Update collection configuration
     * @param config New configuration
     */
    void updateConfig(const CollectionConfig& config);

    /**
     * @brief Get current configuration
     * @return Current configuration
     */
    CollectionConfig getConfig() const;

    /**
     * @brief Set status polling interval
     * @param interval_ms Interval in milliseconds
     */
    void setStatusInterval(uint32_t interval_ms);

    /**
     * @brief Set history polling interval
     * @param interval_ms Interval in milliseconds
     */
    void setHistoryInterval(uint32_t interval_ms);

    /**
     * @brief Enable/disable metric aggregation
     * @param enable true to enable
     */
    void setAggregationEnabled(bool enable);

    /**
     * @brief Clear all stored historical data
     */
    void clearHistory();

    /* ========================================================================
     * Statistics
     * ======================================================================== */

    /**
     * @brief Get collection statistics
     */
    struct Statistics {
        uint64_t total_status_polls;
        uint64_t total_history_polls;
        uint64_t failed_status_polls;
        uint64_t failed_history_polls;
        uint64_t total_samples_collected;
        std::chrono::system_clock::time_point collection_start_time;
        std::chrono::system_clock::time_point last_successful_poll;
    };

    /**
     * @brief Get collection statistics
     * @return Statistics structure
     */
    Statistics getStatistics() const;

    /**
     * @brief Reset statistics
     */
    void resetStatistics();

private:
    /* ========================================================================
     * Private Methods
     * ======================================================================== */

    /**
     * @brief Status collection thread
     */
    void statusCollectionThread();

    /**
     * @brief History collection thread
     */
    void historyCollectionThread();

    /**
     * @brief Aggregation thread
     */
    void aggregationThread();

    /**
     * @brief Notify status observers
     * @param status Dish status to notify
     */
    void notifyStatusObservers(const DishStatus& status);

    /**
     * @brief Notify history observers
     * @param history History stats to notify
     */
    void notifyHistoryObservers(const HistoryStats& history);

    /**
     * @brief Notify metrics observers
     * @param metrics Aggregated metrics to notify
     */
    void notifyMetricsObservers(const AggregatedMetrics& metrics);

    /**
     * @brief Notify error observers
     * @param error Error message
     */
    void notifyErrorObservers(const std::string& error);

    /**
     * @brief Add status sample to history
     * @param status Status sample
     */
    void addStatusSample(const DishStatus& status);

    /**
     * @brief Add metrics to aggregation window
     * @param status Status containing metrics
     */
    void addToAggregation(const DishStatus& status);

    /**
     * @brief Finalize current aggregation window
     * @return Aggregated metrics
     */
    AggregatedMetrics finalizeAggregation();

    /**
     * @brief Generate next observer ID
     * @return Observer ID
     */
    uint32_t generateObserverId();

    /* ========================================================================
     * Member Variables
     * ======================================================================== */

    // gRPC client
    std::shared_ptr<StarlinkGrpcClient> client_;

    // Configuration
    CollectionConfig config_;
    mutable std::mutex config_mutex_;

    // Thread management
    std::atomic<bool> running_;
    std::atomic<bool> shutdown_;
    std::thread status_thread_;
    std::thread history_thread_;
    std::thread aggregation_thread_;
    std::condition_variable cv_;

    // Latest data
    DishStatus latest_status_;
    HistoryStats latest_history_;
    AggregatedMetrics current_metrics_;
    mutable std::mutex data_mutex_;

    // Historical data storage (ring buffers)
    std::deque<DishStatus> status_history_;
    std::deque<AggregatedMetrics> metrics_history_;
    mutable std::mutex history_mutex_;

    // Aggregation state
    std::vector<DishStatus> aggregation_window_;
    std::chrono::system_clock::time_point aggregation_start_;
    std::mutex aggregation_mutex_;

    // Observers
    struct Observer {
        uint32_t id;
        StatusUpdateCallback status_callback;
        HistoryUpdateCallback history_callback;
        MetricsUpdateCallback metrics_callback;
        ErrorCallback error_callback;
    };

    std::vector<Observer> observers_;
    mutable std::mutex observers_mutex_;
    std::atomic<uint32_t> next_observer_id_;

    // Statistics
    mutable Statistics statistics_;
    mutable std::mutex stats_mutex_;
};

} // namespace starlink

#endif // STARLINK_DATA_COLLECTOR_H
