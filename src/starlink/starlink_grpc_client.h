/*
 * Starlink gRPC Client
 *
 * Async gRPC client for Starlink Dish communication
 * Connects to SpaceX.API.Device.Device service at 192.168.100.1:9200
 *
 * Features:
 * - Async request handling
 * - Connection pooling
 * - Automatic retry with exponential backoff
 * - Connection state tracking
 * - Support for Gen2 and Gen3 dishes
 */

#ifndef STARLINK_GRPC_CLIENT_H
#define STARLINK_GRPC_CLIENT_H

#include "starlink_proto_types.h"
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

// Forward declarations for gRPC types (to avoid including gRPC headers here)
namespace grpc {
    class Channel;
    class CompletionQueue;
}

namespace starlink {

/* ============================================================================
 * Connection State
 * ============================================================================ */

enum class ConnectionState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    RECONNECTING,
    ERROR
};

/* ============================================================================
 * Callback Types
 * ============================================================================ */

using StatusCallback = std::function<void(bool success, const DishStatus& status, const std::string& error)>;
using HistoryCallback = std::function<void(bool success, const HistoryStats& history, const std::string& error)>;
using ObstructionMapCallback = std::function<void(bool success, const ObstructionMap& map, const std::string& error)>;
using DeviceInfoCallback = std::function<void(bool success, const DeviceInfo& info, const std::string& error)>;
using CommandCallback = std::function<void(bool success, const std::string& error)>;
using SpeedTestCallback = std::function<void(bool success, const SpeedTestResult& result, const std::string& error)>;

/* ============================================================================
 * StarlinkGrpcClient Class
 * ============================================================================ */

class StarlinkGrpcClient {
public:
    /**
     * @brief Constructor
     * @param config Connection configuration
     */
    explicit StarlinkGrpcClient(const ConnectionConfig& config = ConnectionConfig());

    /**
     * @brief Destructor - ensures clean shutdown
     */
    ~StarlinkGrpcClient();

    // Delete copy constructor and assignment operator
    StarlinkGrpcClient(const StarlinkGrpcClient&) = delete;
    StarlinkGrpcClient& operator=(const StarlinkGrpcClient&) = delete;

    /**
     * @brief Initialize connection to Starlink dish
     * @return true if connection successful
     */
    bool connect();

    /**
     * @brief Close connection to Starlink dish
     */
    void disconnect();

    /**
     * @brief Check if client is connected
     * @return true if connected
     */
    bool isConnected() const;

    /**
     * @brief Get current connection state
     * @return Connection state
     */
    ConnectionState getConnectionState() const;

    /**
     * @brief Update connection endpoint
     * @param endpoint New endpoint (e.g., "192.168.100.1:9200")
     */
    void setEndpoint(const std::string& endpoint);

    /**
     * @brief Get current endpoint
     * @return Endpoint string
     */
    std::string getEndpoint() const;

    /* ========================================================================
     * Synchronous API (Blocking)
     * ======================================================================== */

    /**
     * @brief Get device information (blocking)
     * @param info Output device info
     * @param error_msg Output error message
     * @return true if successful
     */
    bool getDeviceInfo(DeviceInfo& info, std::string& error_msg);

    /**
     * @brief Get current dish status (blocking)
     * @param status Output dish status
     * @param error_msg Output error message
     * @return true if successful
     */
    bool getStatus(DishStatus& status, std::string& error_msg);

    /**
     * @brief Get historical performance data (blocking)
     * @param history Output history stats
     * @param error_msg Output error message
     * @return true if successful
     */
    bool getHistory(HistoryStats& history, std::string& error_msg);

    /**
     * @brief Get obstruction map (blocking)
     * @param map Output obstruction map
     * @param error_msg Output error message
     * @return true if successful
     */
    bool getObstructionMap(ObstructionMap& map, std::string& error_msg);

    /**
     * @brief Stow the dish (blocking)
     * @param error_msg Output error message
     * @return true if successful
     */
    bool dishStow(std::string& error_msg);

    /**
     * @brief Unstow the dish (blocking)
     * @param error_msg Output error message
     * @return true if successful
     */
    bool dishUnstow(std::string& error_msg);

    /**
     * @brief Reboot the dish (blocking)
     * @param error_msg Output error message
     * @return true if successful
     */
    bool reboot(std::string& error_msg);

    /**
     * @brief Factory reset the dish (blocking)
     * WARNING: This erases all settings!
     * @param error_msg Output error message
     * @return true if successful
     */
    bool factoryReset(std::string& error_msg);

    /**
     * @brief Start speed test (blocking)
     * @param result Output speed test result
     * @param error_msg Output error message
     * @return true if successful
     */
    bool startSpeedTest(SpeedTestResult& result, std::string& error_msg);

    /**
     * @brief Get WiFi configuration (blocking)
     * @param config Output WiFi configuration
     * @param error_msg Output error message
     * @return true if successful
     */
    bool getWiFiConfig(WiFiConfig& config, std::string& error_msg);

    /**
     * @brief Set WiFi configuration (blocking)
     * @param config WiFi configuration to apply
     * @param error_msg Output error message
     * @return true if successful
     */
    bool setWiFiConfig(const WiFiConfig& config, std::string& error_msg);

    /* ========================================================================
     * Asynchronous API (Non-blocking)
     * ======================================================================== */

    /**
     * @brief Get device information (async)
     * @param callback Callback function
     */
    void getDeviceInfoAsync(DeviceInfoCallback callback);

    /**
     * @brief Get current dish status (async)
     * @param callback Callback function
     */
    void getStatusAsync(StatusCallback callback);

    /**
     * @brief Get historical performance data (async)
     * @param callback Callback function
     */
    void getHistoryAsync(HistoryCallback callback);

    /**
     * @brief Get obstruction map (async)
     * @param callback Callback function
     */
    void getObstructionMapAsync(ObstructionMapCallback callback);

    /**
     * @brief Stow the dish (async)
     * @param callback Callback function
     */
    void dishStowAsync(CommandCallback callback);

    /**
     * @brief Unstow the dish (async)
     * @param callback Callback function
     */
    void dishUnstowAsync(CommandCallback callback);

    /**
     * @brief Reboot the dish (async)
     * @param callback Callback function
     */
    void rebootAsync(CommandCallback callback);

    /**
     * @brief Start speed test (async)
     * @param callback Callback function
     */
    void startSpeedTestAsync(SpeedTestCallback callback);

    /* ========================================================================
     * Connection Management
     * ======================================================================== */

    /**
     * @brief Enable/disable automatic reconnection
     * @param enable true to enable auto-reconnect
     */
    void setAutoReconnect(bool enable);

    /**
     * @brief Get auto-reconnect status
     * @return true if auto-reconnect enabled
     */
    bool getAutoReconnect() const;

    /**
     * @brief Set connection timeout
     * @param timeout_ms Timeout in milliseconds
     */
    void setTimeout(uint32_t timeout_ms);

    /**
     * @brief Get connection timeout
     * @return Timeout in milliseconds
     */
    uint32_t getTimeout() const;

    /**
     * @brief Get last error message
     * @return Error message string
     */
    std::string getLastError() const;

    /**
     * @brief Clear cached data
     */
    void clearCache();

private:
    /* ========================================================================
     * Private Implementation
     * ======================================================================== */

    class Impl;
    std::unique_ptr<Impl> pImpl;

    /**
     * @brief Initialize gRPC channel
     * @return true if successful
     */
    bool initChannel();

    /**
     * @brief Close gRPC channel
     */
    void closeChannel();

    /**
     * @brief Execute RPC with retry logic
     * @param rpc_func RPC function to execute
     * @param max_retries Maximum retry attempts
     * @return true if successful
     */
    template<typename Func>
    bool executeWithRetry(Func rpc_func, int max_retries = 3);

    /**
     * @brief Calculate exponential backoff delay
     * @param attempt Current attempt number
     * @return Delay in milliseconds
     */
    uint32_t calculateBackoff(int attempt) const;

    /**
     * @brief Update connection state
     * @param state New connection state
     */
    void updateConnectionState(ConnectionState state);

    /**
     * @brief Background reconnection thread
     */
    void reconnectionThread();

    /**
     * @brief Async completion queue processor
     */
    void processCompletionQueue();

    /* ========================================================================
     * Member Variables
     * ======================================================================== */

    ConnectionConfig config_;
    mutable std::mutex mutex_;
    std::atomic<ConnectionState> connection_state_;
    std::atomic<bool> auto_reconnect_;
    std::atomic<bool> shutdown_;
    std::string last_error_;

    // gRPC channel and completion queue
    std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<grpc::CompletionQueue> cq_;

    // Background threads
    std::thread reconnection_thread_;
    std::thread cq_thread_;
    std::condition_variable cv_;

    // Caching for frequently accessed data
    std::atomic<bool> cache_valid_;
    DishStatus cached_status_;
    std::chrono::system_clock::time_point cache_time_;
    static constexpr int CACHE_DURATION_MS = 1000; // 1 second cache
};

} // namespace starlink

#endif // STARLINK_GRPC_CLIENT_H
