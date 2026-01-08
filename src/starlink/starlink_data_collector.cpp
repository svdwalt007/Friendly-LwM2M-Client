/*
 * Starlink Data Collector - Implementation
 *
 * Periodic data collection and aggregation for Starlink terminals
 */

#include "starlink_data_collector.h"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace starlink {

/* ============================================================================
 * Constructor / Destructor
 * ============================================================================ */

StarlinkDataCollector::StarlinkDataCollector(
    std::shared_ptr<StarlinkGrpcClient> client,
    const CollectionConfig& config)
    : client_(client)
    , config_(config)
    , running_(false)
    , shutdown_(false)
    , next_observer_id_(1) {

    statistics_.total_status_polls = 0;
    statistics_.total_history_polls = 0;
    statistics_.failed_status_polls = 0;
    statistics_.failed_history_polls = 0;
    statistics_.total_samples_collected = 0;
    statistics_.collection_start_time = std::chrono::system_clock::now();
    statistics_.last_successful_poll = std::chrono::system_clock::now();

    aggregation_start_ = std::chrono::system_clock::now();
}

StarlinkDataCollector::~StarlinkDataCollector() {
    stop();
}

/* ============================================================================
 * Start / Stop
 * ============================================================================ */

bool StarlinkDataCollector::start() {
    if (running_) {
        return true;
    }

    if (!client_) {
        return false;
    }

    // Ensure client is connected
    if (!client_->isConnected()) {
        if (!client_->connect()) {
            return false;
        }
    }

    shutdown_ = false;
    running_ = true;

    // Start collection threads
    status_thread_ = std::thread(&StarlinkDataCollector::statusCollectionThread, this);
    history_thread_ = std::thread(&StarlinkDataCollector::historyCollectionThread, this);

    if (config_.enable_aggregation) {
        aggregation_thread_ = std::thread(&StarlinkDataCollector::aggregationThread, this);
    }

    statistics_.collection_start_time = std::chrono::system_clock::now();

    return true;
}

void StarlinkDataCollector::stop() {
    if (!running_) {
        return;
    }

    shutdown_ = true;
    running_ = false;
    cv_.notify_all();

    if (status_thread_.joinable()) {
        status_thread_.join();
    }

    if (history_thread_.joinable()) {
        history_thread_.join();
    }

    if (aggregation_thread_.joinable()) {
        aggregation_thread_.join();
    }
}

bool StarlinkDataCollector::isRunning() const {
    return running_;
}

/* ============================================================================
 * Observer Registration
 * ============================================================================ */

uint32_t StarlinkDataCollector::registerStatusObserver(StatusUpdateCallback callback) {
    std::lock_guard<std::mutex> lock(observers_mutex_);

    uint32_t id = generateObserverId();
    Observer observer;
    observer.id = id;
    observer.status_callback = callback;
    observers_.push_back(observer);

    return id;
}

uint32_t StarlinkDataCollector::registerHistoryObserver(HistoryUpdateCallback callback) {
    std::lock_guard<std::mutex> lock(observers_mutex_);

    uint32_t id = generateObserverId();
    Observer observer;
    observer.id = id;
    observer.history_callback = callback;
    observers_.push_back(observer);

    return id;
}

uint32_t StarlinkDataCollector::registerMetricsObserver(MetricsUpdateCallback callback) {
    std::lock_guard<std::mutex> lock(observers_mutex_);

    uint32_t id = generateObserverId();
    Observer observer;
    observer.id = id;
    observer.metrics_callback = callback;
    observers_.push_back(observer);

    return id;
}

uint32_t StarlinkDataCollector::registerErrorObserver(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(observers_mutex_);

    uint32_t id = generateObserverId();
    Observer observer;
    observer.id = id;
    observer.error_callback = callback;
    observers_.push_back(observer);

    return id;
}

void StarlinkDataCollector::unregisterObserver(uint32_t observer_id) {
    std::lock_guard<std::mutex> lock(observers_mutex_);

    observers_.erase(
        std::remove_if(observers_.begin(), observers_.end(),
            [observer_id](const Observer& obs) { return obs.id == observer_id; }),
        observers_.end()
    );
}

/* ============================================================================
 * Data Access
 * ============================================================================ */

DishStatus StarlinkDataCollector::getLatestStatus() const {
    std::lock_guard<std::mutex> lock(data_mutex_);
    return latest_status_;
}

HistoryStats StarlinkDataCollector::getLatestHistory() const {
    std::lock_guard<std::mutex> lock(data_mutex_);
    return latest_history_;
}

AggregatedMetrics StarlinkDataCollector::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(data_mutex_);
    return current_metrics_;
}

std::vector<DishStatus> StarlinkDataCollector::getHistoricalStatus(uint32_t count) const {
    std::lock_guard<std::mutex> lock(history_mutex_);

    if (count == 0 || count >= status_history_.size()) {
        return std::vector<DishStatus>(status_history_.begin(), status_history_.end());
    }

    auto start = status_history_.end() - count;
    return std::vector<DishStatus>(start, status_history_.end());
}

std::vector<AggregatedMetrics> StarlinkDataCollector::getMetricsHistory(uint32_t count) const {
    std::lock_guard<std::mutex> lock(history_mutex_);

    if (count == 0 || count >= metrics_history_.size()) {
        return std::vector<AggregatedMetrics>(metrics_history_.begin(), metrics_history_.end());
    }

    auto start = metrics_history_.end() - count;
    return std::vector<AggregatedMetrics>(start, metrics_history_.end());
}

/* ============================================================================
 * Configuration
 * ============================================================================ */

void StarlinkDataCollector::updateConfig(const CollectionConfig& config) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    config_ = config;
    cv_.notify_all();
}

CollectionConfig StarlinkDataCollector::getConfig() const {
    std::lock_guard<std::mutex> lock(config_mutex_);
    return config_;
}

void StarlinkDataCollector::setStatusInterval(uint32_t interval_ms) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    config_.status_interval_ms = interval_ms;
    cv_.notify_all();
}

void StarlinkDataCollector::setHistoryInterval(uint32_t interval_ms) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    config_.history_interval_ms = interval_ms;
    cv_.notify_all();
}

void StarlinkDataCollector::setAggregationEnabled(bool enable) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    bool was_enabled = config_.enable_aggregation;
    config_.enable_aggregation = enable;

    if (enable && !was_enabled && running_ && !aggregation_thread_.joinable()) {
        aggregation_thread_ = std::thread(&StarlinkDataCollector::aggregationThread, this);
    }
}

void StarlinkDataCollector::clearHistory() {
    std::lock_guard<std::mutex> lock(history_mutex_);
    status_history_.clear();
    metrics_history_.clear();
}

/* ============================================================================
 * Statistics
 * ============================================================================ */

StarlinkDataCollector::Statistics StarlinkDataCollector::getStatistics() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return statistics_;
}

void StarlinkDataCollector::resetStatistics() {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    statistics_.total_status_polls = 0;
    statistics_.total_history_polls = 0;
    statistics_.failed_status_polls = 0;
    statistics_.failed_history_polls = 0;
    statistics_.total_samples_collected = 0;
    statistics_.collection_start_time = std::chrono::system_clock::now();
}

/* ============================================================================
 * Collection Threads
 * ============================================================================ */

void StarlinkDataCollector::statusCollectionThread() {
    while (!shutdown_) {
        uint32_t interval_ms;
        {
            std::lock_guard<std::mutex> lock(config_mutex_);
            interval_ms = config_.status_interval_ms;
        }

        // Poll status
        DishStatus status;
        std::string error_msg;

        {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            statistics_.total_status_polls++;
        }

        if (client_->getStatus(status, error_msg)) {
            // Update latest status
            {
                std::lock_guard<std::mutex> lock(data_mutex_);
                latest_status_ = status;
            }

            // Add to history
            addStatusSample(status);

            // Add to aggregation
            if (config_.enable_aggregation) {
                addToAggregation(status);
            }

            // Notify observers
            notifyStatusObservers(status);

            {
                std::lock_guard<std::mutex> lock(stats_mutex_);
                statistics_.last_successful_poll = std::chrono::system_clock::now();
                statistics_.total_samples_collected++;
            }
        } else {
            // Handle error
            {
                std::lock_guard<std::mutex> lock(stats_mutex_);
                statistics_.failed_status_polls++;
            }

            notifyErrorObservers("Status poll failed: " + error_msg);
        }

        // Sleep until next poll
        std::unique_lock<std::mutex> lock(config_mutex_);
        cv_.wait_for(lock, std::chrono::milliseconds(interval_ms), [this] {
            return shutdown_.load();
        });
    }
}

void StarlinkDataCollector::historyCollectionThread() {
    while (!shutdown_) {
        uint32_t interval_ms;
        {
            std::lock_guard<std::mutex> lock(config_mutex_);
            interval_ms = config_.history_interval_ms;
        }

        // Poll history
        HistoryStats history;
        std::string error_msg;

        {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            statistics_.total_history_polls++;
        }

        if (client_->getHistory(history, error_msg)) {
            // Update latest history
            {
                std::lock_guard<std::mutex> lock(data_mutex_);
                latest_history_ = history;
            }

            // Notify observers
            notifyHistoryObservers(history);

            {
                std::lock_guard<std::mutex> lock(stats_mutex_);
                statistics_.last_successful_poll = std::chrono::system_clock::now();
            }
        } else {
            // Handle error
            {
                std::lock_guard<std::mutex> lock(stats_mutex_);
                statistics_.failed_history_polls++;
            }

            notifyErrorObservers("History poll failed: " + error_msg);
        }

        // Sleep until next poll
        std::unique_lock<std::mutex> lock(config_mutex_);
        cv_.wait_for(lock, std::chrono::milliseconds(interval_ms), [this] {
            return shutdown_.load();
        });
    }
}

void StarlinkDataCollector::aggregationThread() {
    while (!shutdown_) {
        uint32_t window_s;
        {
            std::lock_guard<std::mutex> lock(config_mutex_);
            window_s = config_.aggregation_window_s;
        }

        // Wait for aggregation window
        std::unique_lock<std::mutex> lock(config_mutex_);
        cv_.wait_for(lock, std::chrono::seconds(window_s), [this] {
            return shutdown_.load();
        });
        lock.unlock();

        if (shutdown_) {
            break;
        }

        // Finalize aggregation
        AggregatedMetrics metrics = finalizeAggregation();

        // Update current metrics
        {
            std::lock_guard<std::mutex> data_lock(data_mutex_);
            current_metrics_ = metrics;
        }

        // Store in history
        {
            std::lock_guard<std::mutex> history_lock(history_mutex_);
            metrics_history_.push_back(metrics);

            // Limit history size
            if (metrics_history_.size() > config_.max_history_samples) {
                metrics_history_.pop_front();
            }
        }

        // Notify observers
        notifyMetricsObservers(metrics);

        // Reset aggregation window
        {
            std::lock_guard<std::mutex> agg_lock(aggregation_mutex_);
            aggregation_window_.clear();
            aggregation_start_ = std::chrono::system_clock::now();
        }
    }
}

/* ============================================================================
 * Observer Notification
 * ============================================================================ */

void StarlinkDataCollector::notifyStatusObservers(const DishStatus& status) {
    std::lock_guard<std::mutex> lock(observers_mutex_);

    for (const auto& observer : observers_) {
        if (observer.status_callback) {
            try {
                observer.status_callback(status);
            } catch (const std::exception& e) {
                // Log error but don't propagate
            }
        }
    }
}

void StarlinkDataCollector::notifyHistoryObservers(const HistoryStats& history) {
    std::lock_guard<std::mutex> lock(observers_mutex_);

    for (const auto& observer : observers_) {
        if (observer.history_callback) {
            try {
                observer.history_callback(history);
            } catch (const std::exception& e) {
                // Log error but don't propagate
            }
        }
    }
}

void StarlinkDataCollector::notifyMetricsObservers(const AggregatedMetrics& metrics) {
    std::lock_guard<std::mutex> lock(observers_mutex_);

    for (const auto& observer : observers_) {
        if (observer.metrics_callback) {
            try {
                observer.metrics_callback(metrics);
            } catch (const std::exception& e) {
                // Log error but don't propagate
            }
        }
    }
}

void StarlinkDataCollector::notifyErrorObservers(const std::string& error) {
    std::lock_guard<std::mutex> lock(observers_mutex_);

    for (const auto& observer : observers_) {
        if (observer.error_callback) {
            try {
                observer.error_callback(error);
            } catch (const std::exception& e) {
                // Log error but don't propagate
            }
        }
    }
}

/* ============================================================================
 * Data Management
 * ============================================================================ */

void StarlinkDataCollector::addStatusSample(const DishStatus& status) {
    std::lock_guard<std::mutex> lock(history_mutex_);

    status_history_.push_back(status);

    // Limit history size
    if (status_history_.size() > config_.max_history_samples) {
        status_history_.pop_front();
    }
}

void StarlinkDataCollector::addToAggregation(const DishStatus& status) {
    std::lock_guard<std::mutex> lock(aggregation_mutex_);
    aggregation_window_.push_back(status);
}

AggregatedMetrics StarlinkDataCollector::finalizeAggregation() {
    std::lock_guard<std::mutex> lock(aggregation_mutex_);

    AggregatedMetrics metrics;

    if (aggregation_window_.empty()) {
        return metrics;
    }

    metrics.sample_count = aggregation_window_.size();
    metrics.window_start = aggregation_start_;
    metrics.window_end = std::chrono::system_clock::now();

    // Calculate network performance averages
    double total_downlink = 0.0;
    double total_uplink = 0.0;
    float total_latency = 0.0f;
    float total_packet_loss = 0.0f;

    // Calculate signal quality averages
    float total_snr = 0.0f;

    // Calculate obstruction statistics
    float total_obstruction = 0.0f;
    uint32_t obstruction_count = 0;

    // Calculate power statistics
    float total_power = 0.0f;

    for (const auto& status : aggregation_window_) {
        // Network metrics
        total_downlink += status.network.downlink_throughput_bps;
        total_uplink += status.network.uplink_throughput_bps;
        total_latency += status.network.pop_ping_latency_ms;
        total_packet_loss += status.network.pop_ping_drop_rate;

        // Latency min/max
        if (status.network.pop_ping_latency_ms < metrics.min_latency_ms) {
            metrics.min_latency_ms = status.network.pop_ping_latency_ms;
        }
        if (status.network.pop_ping_latency_ms > metrics.max_latency_ms) {
            metrics.max_latency_ms = status.network.pop_ping_latency_ms;
        }

        // Signal quality
        total_snr += status.signal.snr;
        if (status.signal.snr < metrics.min_snr) {
            metrics.min_snr = status.signal.snr;
        }
        if (status.signal.snr > metrics.max_snr) {
            metrics.max_snr = status.signal.snr;
        }

        // Obstruction
        total_obstruction += status.obstruction.fraction_obstructed;
        if (status.obstruction.currently_obstructed) {
            obstruction_count++;
        }

        // Power
        total_power += status.power.power_w;
        if (status.power.power_w < metrics.min_power_w) {
            metrics.min_power_w = status.power.power_w;
        }
        if (status.power.power_w > metrics.max_power_w) {
            metrics.max_power_w = status.power.power_w;
        }
    }

    // Calculate averages
    metrics.avg_downlink_throughput_bps = total_downlink / metrics.sample_count;
    metrics.avg_uplink_throughput_bps = total_uplink / metrics.sample_count;
    metrics.avg_latency_ms = total_latency / metrics.sample_count;
    metrics.avg_packet_loss = total_packet_loss / metrics.sample_count;
    metrics.avg_snr = total_snr / metrics.sample_count;
    metrics.avg_obstruction_fraction = total_obstruction / metrics.sample_count;
    metrics.obstruction_count = obstruction_count;
    metrics.avg_power_w = total_power / metrics.sample_count;

    return metrics;
}

/* ============================================================================
 * Utilities
 * ============================================================================ */

uint32_t StarlinkDataCollector::generateObserverId() {
    return next_observer_id_++;
}

} // namespace starlink
