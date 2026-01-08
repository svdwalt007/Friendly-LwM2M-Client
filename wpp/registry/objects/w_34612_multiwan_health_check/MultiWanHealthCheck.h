/*
 * MultiWanHealthCheck (Object 34612)
 * Walt Technologies - OpenWRT One Router
 *
 * Multi-WAN Health Check and Probing for WAN interfaces
 */

#ifndef WPP_W_34612_MULTIWAN_HEALTH_CHECK_H
#define WPP_W_34612_MULTIWAN_HEALTH_CHECK_H

#include "MultiWanHealthCheckConfig.h"
#include "MultiWanHealthCheckInfo.h"
#include "Instance.h"
#include "InstSubject.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <chrono>

namespace wpp {

class WppClient;
class Object;

class MultiWanHealthCheck : public Instance {
public:
    enum ID: ID_T {
        INTERFACE_NAME_0 = 0,           // Interface Name (RW, String)
        PROBE_TYPE_1 = 1,               // Probe Type (RW, Integer)
        TARGET_HOST_2 = 2,              // Target Host (RW, String)
        TARGET_PORT_3 = 3,              // Target Port (RW, Integer)
        PROBE_INTERVAL_4 = 4,           // Probe Interval (RW, Integer) - seconds
        PROBE_TIMEOUT_5 = 5,            // Probe Timeout (RW, Integer) - milliseconds
        FAILURE_THRESHOLD_6 = 6,        // Failure Threshold (RW, Integer)
        RECOVERY_THRESHOLD_7 = 7,       // Recovery Threshold (RW, Integer)
        CURRENT_STATE_8 = 8,            // Current State (R, Integer)
        LAST_PROBE_TIME_9 = 9,          // Last Probe Time (R, Time)
        LAST_PROBE_RESULT_10 = 10,      // Last Probe Result (R, Boolean)
        AVERAGE_LATENCY_11 = 11,        // Average Latency (R, Float) - ms
        MIN_LATENCY_12 = 12,            // Min Latency (R, Float) - ms
        MAX_LATENCY_13 = 13,            // Max Latency (R, Float) - ms
        JITTER_14 = 14,                 // Jitter (R, Float) - ms
        PACKET_LOSS_15 = 15,            // Packet Loss (R, Float) - percentage
        CONSECUTIVE_FAILURES_16 = 16,   // Consecutive Failures (R, Integer)
        CONSECUTIVE_SUCCESSES_17 = 17,  // Consecutive Successes (R, Integer)
        TOTAL_PROBES_18 = 18,           // Total Probes (R, Integer)
        FAILED_PROBES_19 = 19,          // Failed Probes (R, Integer)
        START_PROBING_20 = 20,          // Start Probing (E, Execute)
        STOP_PROBING_21 = 21,           // Stop Probing (E, Execute)
        FORCE_CHECK_22 = 22,            // Force Check (E, Execute)
        RESET_STATS_23 = 23             // Reset Stats (E, Execute)
    };

    // Probe Type Enumeration
    enum ProbeType: uint8_t {
        PROBE_ICMP = 0,   // ICMP ping
        PROBE_HTTP = 1,   // HTTP/HTTPS check
        PROBE_DNS = 2,    // DNS resolution
        PROBE_TCP = 3     // TCP connection
    };

    // Health State Enumeration
    enum HealthState: uint8_t {
        STATE_HEALTHY = 0,    // Interface is healthy
        STATE_DEGRADED = 1,   // Interface is degraded (intermittent failures)
        STATE_UNHEALTHY = 2   // Interface is unhealthy
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    MultiWanHealthCheck(Object& object, INST_T instId);
    ~MultiWanHealthCheck() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool startProbing(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool stopProbing(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool forceCheck(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool resetStats(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Probing worker thread */
    void probeWorker();

    /* Probe methods */
    bool executeProbe();
    bool probeICMP(const std::string& host, int timeout_ms, double& latency_ms);
    bool probeHTTP(const std::string& host, int port, int timeout_ms, double& latency_ms);
    bool probeDNS(const std::string& host, int timeout_ms, double& latency_ms);
    bool probeTCP(const std::string& host, int port, int timeout_ms, double& latency_ms);

    /* Statistics and health management */
    void updateStatistics(bool success, double latency_ms);
    void updateHealthState(bool probeSuccess);
    void resetStatistics();
    void calculateJitter();

    /* Helper methods */
    bool loadFromMwan3();
    bool saveToMwan3();
    std::string getInterfaceDevice();
    bool isValidHostname(const std::string& host);

    /* Latency history for jitter calculation */
    struct LatencyRecord {
        double latency_ms;
        std::chrono::steady_clock::time_point timestamp;
    };

    /* Private member variables */
    std::thread m_workerThread;
    std::atomic<bool> m_running;
    std::atomic<bool> m_probingActive;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::deque<LatencyRecord> m_latencyHistory;
    static constexpr size_t MAX_LATENCY_HISTORY = 100;

    // Exponential moving average alpha (0.2 = 20% weight to new values)
    static constexpr double EMA_ALPHA = 0.2;
};

} // namespace wpp

#endif // WPP_W_34612_MULTIWAN_HEALTH_CHECK_H
