/**
 * @file log_ring.h
 * @brief P1-8 Ring-Buffer Logging System for LwM2M Client Observability
 *
 * Provides a thread-safe, persistent ring-buffer for logging state transitions,
 * introspection counters, and diagnostic information critical for debugging
 * burst-load stalls and other protocol-level issues.
 *
 * @see Issues/pcap-combo.md Section P1-8
 * @see docs/IMMEDIATE_ACTION_PLAN.md
 *
 * @author Claude Code / Sean van der Walt
 * @date 2026-04-30
 */

#ifndef FTL_LOG_RING_H
#define FTL_LOG_RING_H

#include <cstdint>
#include <array>
#include <atomic>
#include <mutex>
#include <string>
#include <chrono>
#include <functional>
#include <memory>

namespace ftl {

/**
 * @brief Trigger event codes for state transitions
 */
enum class TriggerEvent : uint16_t {
    UNKNOWN = 0,

    // Registration events (0x01xx)
    REGISTRATION_STARTED = 0x0100,
    REGISTRATION_UPDATE_SENT = 0x0101,
    REGISTRATION_UPDATE_ACK = 0x0102,
    REGISTRATION_FAILED = 0x0103,
    REGISTRATION_TIMEOUT = 0x0104,
    DEREGISTRATION_SENT = 0x0105,
    DEREGISTRATION_ACK = 0x0106,

    // CoAP events (0x02xx)
    COAP_REQUEST_RECEIVED = 0x0200,
    COAP_RESPONSE_SENT = 0x0201,
    COAP_TIMEOUT = 0x0202,
    COAP_RETRANSMIT = 0x0203,
    COAP_ACK_RECEIVED = 0x0204,

    // DTLS events (0x03xx)
    DTLS_HANDSHAKE_STARTED = 0x0300,
    DTLS_HANDSHAKE_COMPLETE = 0x0301,
    DTLS_HANDSHAKE_FAILED = 0x0302,
    DTLS_SESSION_RESUMED = 0x0303,
    DTLS_ALERT_RECEIVED = 0x0304,

    // Transaction events (0x04xx)
    TRANSACTION_CREATED = 0x0400,
    TRANSACTION_COMPLETED = 0x0401,
    TRANSACTION_FAILED = 0x0402,
    TRANSACTION_TIMEOUT = 0x0403,

    // Application events (0x05xx)
    LWM2M_STEP_CALLED = 0x0500,
    SIGUSR1_RECEIVED = 0x0501,
    SHUTDOWN_INITIATED = 0x0502,
    ERROR_HANDLER_CALLED = 0x0503,

    // Object/Resource events (0x06xx)
    OBJECT_READ = 0x0600,
    OBJECT_WRITE = 0x0601,
    OBJECT_EXECUTE = 0x0602,
    OBJECT_CREATE = 0x0603,
    OBJECT_DELETE = 0x0604,
    ACCESS_DENIED = 0x0605,  // 4.01 Unauthorized
};

/**
 * @brief Structured log entry with all diagnostic fields
 *
 * Size: 40 bytes (padded to 64 bytes for cache alignment)
 */
struct LogEntry {
    uint64_t t_mono_us;          ///< Monotonic timestamp in microseconds (same clock as lwm2m_step)
    uint8_t  state_from;         ///< lwm2m_client_state_t before transition
    uint8_t  state_to;           ///< lwm2m_client_state_t after transition
    uint16_t trigger;            ///< TriggerEvent code
    int32_t  error_code;         ///< Error code (0 = success)
    uint32_t transaction_count;  ///< Current transaction list size
    uint32_t pending_token_count;///< Pending CoAP tokens
    uint32_t mid_alloc_count;    ///< Allocated Message IDs
    uint32_t nvm_inflight_writes;///< NVM writes in progress
    uint32_t sequence_number;    ///< Global sequence number for ordering
    uint32_t _reserved;          ///< Reserved for future use (alignment)

    // Default constructor
    LogEntry() noexcept
        : t_mono_us(0), state_from(0), state_to(0), trigger(0), error_code(0),
          transaction_count(0), pending_token_count(0), mid_alloc_count(0),
          nvm_inflight_writes(0), sequence_number(0), _reserved(0) {}
};

static_assert(sizeof(LogEntry) == 40, "LogEntry size must be 40 bytes");

/**
 * @brief Introspection counters for resource tracking
 *
 * These counters are updated atomically and can be read without locking.
 * They provide visibility into internal state that would otherwise require
 * walking the Wakaama transaction list.
 */
struct IntrospectionCounters {
    std::atomic<uint32_t> transaction_count{0};     ///< Active transactions
    std::atomic<uint32_t> pending_token_count{0};   ///< Pending CoAP tokens
    std::atomic<uint32_t> mid_alloc_count{0};       ///< Allocated MIDs
    std::atomic<uint32_t> nvm_inflight_writes{0};   ///< NVM writes in flight
    std::atomic<uint64_t> total_requests{0};        ///< Total requests processed
    std::atomic<uint64_t> total_4xx_responses{0};   ///< Total 4xx responses sent
    std::atomic<uint64_t> total_2xx_responses{0};   ///< Total 2xx responses sent

    // Reset all counters (for testing)
    void reset() noexcept {
        transaction_count.store(0, std::memory_order_relaxed);
        pending_token_count.store(0, std::memory_order_relaxed);
        mid_alloc_count.store(0, std::memory_order_relaxed);
        nvm_inflight_writes.store(0, std::memory_order_relaxed);
        total_requests.store(0, std::memory_order_relaxed);
        total_4xx_responses.store(0, std::memory_order_relaxed);
        total_2xx_responses.store(0, std::memory_order_relaxed);
    }
};

/**
 * @brief UPDATE timestamp tracking for diagnostics
 */
struct UpdateTimestamps {
    static constexpr size_t MAX_TIMESTAMPS = 10;
    std::array<uint64_t, MAX_TIMESTAMPS> timestamps{};  ///< Last 10 UPDATE timestamps (monotonic us)
    std::atomic<size_t> count{0};                        ///< Number of valid timestamps
    std::atomic<size_t> next_index{0};                   ///< Next write position (circular)

    void record(uint64_t t_mono_us) noexcept {
        size_t idx = next_index.fetch_add(1, std::memory_order_relaxed) % MAX_TIMESTAMPS;
        timestamps[idx] = t_mono_us;
        size_t current_count = count.load(std::memory_order_relaxed);
        if (current_count < MAX_TIMESTAMPS) {
            count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void reset() noexcept {
        timestamps.fill(0);
        count.store(0, std::memory_order_relaxed);
        next_index.store(0, std::memory_order_relaxed);
    }
};

/**
 * @brief MID tracking for recent CoAP messages
 */
struct MidTracker {
    static constexpr size_t MAX_MIDS = 16;
    std::array<uint16_t, MAX_MIDS> mids{};  ///< Last 16 MIDs
    std::atomic<size_t> next_index{0};       ///< Next write position (circular)

    void record(uint16_t mid) noexcept {
        size_t idx = next_index.fetch_add(1, std::memory_order_relaxed) % MAX_MIDS;
        mids[idx] = mid;
    }

    void reset() noexcept {
        mids.fill(0);
        next_index.store(0, std::memory_order_relaxed);
    }
};

/**
 * @brief DTLS handshake outcome tracking
 */
struct DtlsOutcomes {
    static constexpr size_t MAX_OUTCOMES = 4;

    struct Outcome {
        uint64_t timestamp_us{0};  ///< When the handshake completed
        int32_t result_code{0};    ///< Result code (0 = success)
        bool success{false};       ///< Quick success flag
        uint8_t _padding[3]{};     ///< Alignment padding
    };

    std::array<Outcome, MAX_OUTCOMES> outcomes{};
    std::atomic<size_t> next_index{0};

    void record(uint64_t t_mono_us, int32_t result, bool success) noexcept {
        size_t idx = next_index.fetch_add(1, std::memory_order_relaxed) % MAX_OUTCOMES;
        outcomes[idx] = {t_mono_us, result, success, {}};
    }

    void reset() noexcept {
        outcomes.fill({});
        next_index.store(0, std::memory_order_relaxed);
    }
};

/**
 * @brief Thread-safe ring buffer for log entries
 *
 * The ring buffer uses a lock-free design for single-producer scenarios
 * and falls back to mutex-based locking for multi-producer safety.
 * The buffer is sized for 64 KiB on Linux targets (1024 entries).
 *
 * @note All timestamps use the same monotonic clock as lwm2m_step()
 */
class LogRing {
public:
    /// Buffer size for Linux targets: 64 KiB / ~64 bytes per entry = ~1000 entries
    static constexpr size_t SIZE = 1024;

    /// Default snapshot output path
    static constexpr const char* DEFAULT_SNAPSHOT_PATH = "/tmp/ftl-lwm2m-snapshot.json";

    LogRing() noexcept;
    ~LogRing() = default;

    // Non-copyable, non-movable (contains atomic members)
    LogRing(const LogRing&) = delete;
    LogRing& operator=(const LogRing&) = delete;
    LogRing(LogRing&&) = delete;
    LogRing& operator=(LogRing&&) = delete;

    /**
     * @brief Log a state transition with full diagnostic context
     *
     * @param from      Previous state (lwm2m_client_state_t)
     * @param to        New state (lwm2m_client_state_t)
     * @param trigger   Event that triggered the transition
     * @param err       Error code (0 = success)
     */
    void log_transition(uint8_t from, uint8_t to, TriggerEvent trigger, int32_t err = 0) noexcept;

    /**
     * @brief Log a state transition with raw trigger code
     *
     * @param from      Previous state
     * @param to        New state
     * @param trigger   Raw trigger code (for backward compatibility)
     * @param err       Error code
     */
    void log_transition(uint8_t from, uint8_t to, uint16_t trigger, int32_t err = 0) noexcept;

    /**
     * @brief Write a JSON snapshot to file (signal-safe)
     *
     * This function is designed to be safely callable from a signal handler.
     * It uses only async-signal-safe functions and does not allocate memory.
     *
     * @param path Output file path (defaults to /tmp/ftl-lwm2m-snapshot.json)
     * @return true if snapshot was written successfully
     */
    bool snapshot_to_file(const char* path = DEFAULT_SNAPSHOT_PATH) const noexcept;

    /**
     * @brief Generate JSON snapshot as a string
     *
     * @return JSON string representation of current state
     */
    std::string generate_snapshot() const;

    /**
     * @brief Get entry at logical index (0 = oldest)
     *
     * @param index Logical index from oldest entry
     * @return Reference to log entry
     */
    const LogEntry& get(size_t index) const noexcept;

    /**
     * @brief Get number of valid entries in the buffer
     */
    size_t count() const noexcept;

    /**
     * @brief Get total number of entries ever written
     */
    uint64_t total_writes() const noexcept;

    /**
     * @brief Get monotonic timestamp in microseconds
     *
     * Uses the same clock source as lwm2m_step() for consistency.
     */
    static uint64_t get_monotonic_time_us() noexcept;

    /**
     * @brief Access introspection counters (read-only)
     */
    const IntrospectionCounters& counters() const noexcept { return counters_; }

    /**
     * @brief Access introspection counters (read-write)
     */
    IntrospectionCounters& counters() noexcept { return counters_; }

    /**
     * @brief Access UPDATE timestamps
     */
    UpdateTimestamps& update_timestamps() noexcept { return update_timestamps_; }
    const UpdateTimestamps& update_timestamps() const noexcept { return update_timestamps_; }

    /**
     * @brief Access MID tracker
     */
    MidTracker& mid_tracker() noexcept { return mid_tracker_; }
    const MidTracker& mid_tracker() const noexcept { return mid_tracker_; }

    /**
     * @brief Access DTLS outcomes
     */
    DtlsOutcomes& dtls_outcomes() noexcept { return dtls_outcomes_; }
    const DtlsOutcomes& dtls_outcomes() const noexcept { return dtls_outcomes_; }

    /**
     * @brief Set current client state (for snapshot)
     */
    void set_current_state(uint8_t state) noexcept { current_state_.store(state, std::memory_order_relaxed); }
    uint8_t get_current_state() const noexcept { return current_state_.load(std::memory_order_relaxed); }

    /**
     * @brief Set current lifetime (lt) value
     */
    void set_lifetime(uint32_t lt) noexcept { current_lt_.store(lt, std::memory_order_relaxed); }
    uint32_t get_lifetime() const noexcept { return current_lt_.load(std::memory_order_relaxed); }

    /**
     * @brief Reset all state (for testing)
     */
    void reset() noexcept;

private:
    // Ring buffer storage
    std::array<LogEntry, SIZE> entries_{};

    // Buffer state (atomic for lock-free reads)
    std::atomic<size_t> head_{0};           ///< Next write position
    std::atomic<uint64_t> total_writes_{0}; ///< Total entries written
    std::atomic<uint64_t> sequence_{0};     ///< Global sequence number

    // Write mutex for multi-producer safety
    mutable std::mutex write_mutex_;

    // Introspection state
    IntrospectionCounters counters_;
    UpdateTimestamps update_timestamps_;
    MidTracker mid_tracker_;
    DtlsOutcomes dtls_outcomes_;

    // Current client state
    std::atomic<uint8_t> current_state_{0};
    std::atomic<uint32_t> current_lt_{60};  // Default lifetime

    // Scratch entry for out-of-bounds access
    static const LogEntry empty_entry_;
};

/**
 * @brief Global log ring instance
 *
 * Singleton pattern ensures consistent logging across the application.
 * Thread-safe initialization via C++11 magic statics.
 */
LogRing& get_log_ring() noexcept;

/**
 * @brief Helper to convert trigger event to string
 */
const char* trigger_event_to_string(TriggerEvent event) noexcept;

/**
 * @brief Helper to convert lwm2m_client_state_t to string
 */
const char* client_state_to_string(uint8_t state) noexcept;

} // namespace ftl

#endif // FTL_LOG_RING_H
