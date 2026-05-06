/**
 * @file log_ring.cpp
 * @brief P1-8 Ring-Buffer Logging System Implementation
 *
 * @see log_ring.h for interface documentation
 *
 * @author Claude Code / Sean van der Walt
 * @date 2026-04-30
 */

#include "log_ring.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sstream>
#include <iomanip>

namespace ftl {

// Static member definitions
const LogEntry LogRing::empty_entry_{};

LogRing::LogRing() noexcept {
    // Initialize all entries to zero
    entries_.fill(LogEntry{});
}

void LogRing::log_transition(uint8_t from, uint8_t to, TriggerEvent trigger, int32_t err) noexcept {
    log_transition(from, to, static_cast<uint16_t>(trigger), err);
}

void LogRing::log_transition(uint8_t from, uint8_t to, uint16_t trigger, int32_t err) noexcept {
    // Create log entry
    LogEntry entry{};
    entry.t_mono_us = get_monotonic_time_us();
    entry.state_from = from;
    entry.state_to = to;
    entry.trigger = trigger;
    entry.error_code = err;

    // Capture current counter values atomically
    entry.transaction_count = counters_.transaction_count.load(std::memory_order_relaxed);
    entry.pending_token_count = counters_.pending_token_count.load(std::memory_order_relaxed);
    entry.mid_alloc_count = counters_.mid_alloc_count.load(std::memory_order_relaxed);
    entry.nvm_inflight_writes = counters_.nvm_inflight_writes.load(std::memory_order_relaxed);

    // Get sequence number
    entry.sequence_number = static_cast<uint32_t>(sequence_.fetch_add(1, std::memory_order_relaxed));

    // Write to buffer under lock (for multi-producer safety)
    {
        std::lock_guard<std::mutex> lock(write_mutex_);
        size_t idx = head_.load(std::memory_order_relaxed);
        entries_[idx] = entry;
        head_.store((idx + 1) % SIZE, std::memory_order_release);
        total_writes_.fetch_add(1, std::memory_order_relaxed);
    }

    // Update current state
    current_state_.store(to, std::memory_order_relaxed);
}

bool LogRing::snapshot_to_file(const char* path) const noexcept {
    // Generate snapshot string
    std::string json;
    try {
        json = generate_snapshot();
    } catch (...) {
        return false;
    }

    // Write to file using POSIX API (async-signal-safe)
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return false;
    }

    ssize_t written = 0;
    size_t total = json.size();
    const char* data = json.c_str();

    while (static_cast<size_t>(written) < total) {
        ssize_t result = write(fd, data + written, total - static_cast<size_t>(written));
        if (result < 0) {
            close(fd);
            return false;
        }
        written += result;
    }

    close(fd);
    return true;
}

std::string LogRing::generate_snapshot() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);

    // Get current time
    uint64_t now_us = get_monotonic_time_us();
    time_t wall_time = time(nullptr);

    oss << "{\n";
    oss << "  \"snapshot_version\": \"1.0\",\n";
    oss << "  \"wall_clock_time\": " << wall_time << ",\n";
    oss << "  \"monotonic_time_us\": " << now_us << ",\n";
    oss << "  \"client_state\": " << static_cast<int>(current_state_.load(std::memory_order_relaxed)) << ",\n";
    oss << "  \"client_state_name\": \"" << client_state_to_string(current_state_.load(std::memory_order_relaxed)) << "\",\n";
    oss << "  \"lifetime_seconds\": " << current_lt_.load(std::memory_order_relaxed) << ",\n";

    // Ring buffer stats
    oss << "  \"ring_buffer\": {\n";
    oss << "    \"entries_count\": " << count() << ",\n";
    oss << "    \"total_writes\": " << total_writes_.load(std::memory_order_relaxed) << ",\n";
    oss << "    \"capacity\": " << SIZE << "\n";
    oss << "  },\n";

    // Introspection counters
    oss << "  \"counters\": {\n";
    oss << "    \"transaction_count\": " << counters_.transaction_count.load(std::memory_order_relaxed) << ",\n";
    oss << "    \"pending_token_count\": " << counters_.pending_token_count.load(std::memory_order_relaxed) << ",\n";
    oss << "    \"mid_alloc_count\": " << counters_.mid_alloc_count.load(std::memory_order_relaxed) << ",\n";
    oss << "    \"nvm_inflight_writes\": " << counters_.nvm_inflight_writes.load(std::memory_order_relaxed) << ",\n";
    oss << "    \"total_requests\": " << counters_.total_requests.load(std::memory_order_relaxed) << ",\n";
    oss << "    \"total_2xx_responses\": " << counters_.total_2xx_responses.load(std::memory_order_relaxed) << ",\n";
    oss << "    \"total_4xx_responses\": " << counters_.total_4xx_responses.load(std::memory_order_relaxed) << "\n";
    oss << "  },\n";

    // Last 10 UPDATE timestamps
    oss << "  \"last_update_timestamps_us\": [";
    size_t update_count = update_timestamps_.count.load(std::memory_order_relaxed);
    size_t next_idx = update_timestamps_.next_index.load(std::memory_order_relaxed);
    for (size_t i = 0; i < update_count && i < UpdateTimestamps::MAX_TIMESTAMPS; ++i) {
        size_t idx;
        if (update_count >= UpdateTimestamps::MAX_TIMESTAMPS) {
            // Buffer is full, start from oldest
            idx = (next_idx + i) % UpdateTimestamps::MAX_TIMESTAMPS;
        } else {
            idx = i;
        }
        if (i > 0) oss << ", ";
        oss << update_timestamps_.timestamps[idx];
    }
    oss << "],\n";

    // Last 16 MIDs
    oss << "  \"last_mids\": [";
    size_t mid_next = mid_tracker_.next_index.load(std::memory_order_relaxed);
    size_t mid_count = std::min(mid_next, MidTracker::MAX_MIDS);
    for (size_t i = 0; i < mid_count; ++i) {
        size_t idx;
        if (mid_next > MidTracker::MAX_MIDS) {
            idx = (mid_next + i) % MidTracker::MAX_MIDS;
        } else {
            idx = i;
        }
        if (i > 0) oss << ", ";
        oss << mid_tracker_.mids[idx];
    }
    oss << "],\n";

    // Last 4 DTLS outcomes
    oss << "  \"last_dtls_outcomes\": [\n";
    size_t dtls_next = dtls_outcomes_.next_index.load(std::memory_order_relaxed);
    size_t dtls_count = std::min(dtls_next, DtlsOutcomes::MAX_OUTCOMES);
    for (size_t i = 0; i < dtls_count; ++i) {
        size_t idx;
        if (dtls_next > DtlsOutcomes::MAX_OUTCOMES) {
            idx = (dtls_next + i) % DtlsOutcomes::MAX_OUTCOMES;
        } else {
            idx = i;
        }
        const auto& outcome = dtls_outcomes_.outcomes[idx];
        oss << "    {\"timestamp_us\": " << outcome.timestamp_us
            << ", \"result_code\": " << outcome.result_code
            << ", \"success\": " << (outcome.success ? "true" : "false") << "}";
        if (i < dtls_count - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n";

    // Last 10 log entries (most recent first)
    oss << "  \"recent_entries\": [\n";
    size_t entry_count = count();
    size_t to_show = std::min(entry_count, static_cast<size_t>(10));
    for (size_t i = 0; i < to_show; ++i) {
        // Get from newest to oldest
        size_t idx = entry_count - 1 - i;
        const LogEntry& entry = get(idx);
        oss << "    {\n";
        oss << "      \"sequence\": " << entry.sequence_number << ",\n";
        oss << "      \"t_mono_us\": " << entry.t_mono_us << ",\n";
        oss << "      \"state_from\": " << static_cast<int>(entry.state_from) << ",\n";
        oss << "      \"state_from_name\": \"" << client_state_to_string(entry.state_from) << "\",\n";
        oss << "      \"state_to\": " << static_cast<int>(entry.state_to) << ",\n";
        oss << "      \"state_to_name\": \"" << client_state_to_string(entry.state_to) << "\",\n";
        oss << "      \"trigger\": " << entry.trigger << ",\n";
        oss << "      \"trigger_name\": \"" << trigger_event_to_string(static_cast<TriggerEvent>(entry.trigger)) << "\",\n";
        oss << "      \"error_code\": " << entry.error_code << ",\n";
        oss << "      \"transaction_count\": " << entry.transaction_count << ",\n";
        oss << "      \"pending_token_count\": " << entry.pending_token_count << ",\n";
        oss << "      \"mid_alloc_count\": " << entry.mid_alloc_count << ",\n";
        oss << "      \"nvm_inflight_writes\": " << entry.nvm_inflight_writes << "\n";
        oss << "    }";
        if (i < to_show - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ]\n";
    oss << "}\n";

    return oss.str();
}

const LogEntry& LogRing::get(size_t index) const noexcept {
    size_t total = total_writes_.load(std::memory_order_acquire);
    size_t current_count = std::min(total, SIZE);

    if (index >= current_count) {
        return empty_entry_;
    }

    // Calculate actual buffer index
    size_t head = head_.load(std::memory_order_acquire);
    size_t actual_idx;

    if (total <= SIZE) {
        // Buffer not yet full
        actual_idx = index;
    } else {
        // Buffer wrapped, oldest entry is at head
        actual_idx = (head + index) % SIZE;
    }

    return entries_[actual_idx];
}

size_t LogRing::count() const noexcept {
    uint64_t total = total_writes_.load(std::memory_order_acquire);
    return static_cast<size_t>(std::min(total, static_cast<uint64_t>(SIZE)));
}

uint64_t LogRing::total_writes() const noexcept {
    return total_writes_.load(std::memory_order_acquire);
}

uint64_t LogRing::get_monotonic_time_us() noexcept {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(duration).count()
    );
}

void LogRing::reset() noexcept {
    std::lock_guard<std::mutex> lock(write_mutex_);
    entries_.fill(LogEntry{});
    head_.store(0, std::memory_order_relaxed);
    total_writes_.store(0, std::memory_order_relaxed);
    sequence_.store(0, std::memory_order_relaxed);
    current_state_.store(0, std::memory_order_relaxed);
    current_lt_.store(60, std::memory_order_relaxed);
    counters_.reset();
    update_timestamps_.reset();
    mid_tracker_.reset();
    dtls_outcomes_.reset();
}

// Global singleton instance
LogRing& get_log_ring() noexcept {
    static LogRing instance;
    return instance;
}

const char* trigger_event_to_string(TriggerEvent event) noexcept {
    switch (event) {
        case TriggerEvent::UNKNOWN: return "UNKNOWN";

        // Registration events
        case TriggerEvent::REGISTRATION_STARTED: return "REGISTRATION_STARTED";
        case TriggerEvent::REGISTRATION_UPDATE_SENT: return "REGISTRATION_UPDATE_SENT";
        case TriggerEvent::REGISTRATION_UPDATE_ACK: return "REGISTRATION_UPDATE_ACK";
        case TriggerEvent::REGISTRATION_FAILED: return "REGISTRATION_FAILED";
        case TriggerEvent::REGISTRATION_TIMEOUT: return "REGISTRATION_TIMEOUT";
        case TriggerEvent::DEREGISTRATION_SENT: return "DEREGISTRATION_SENT";
        case TriggerEvent::DEREGISTRATION_ACK: return "DEREGISTRATION_ACK";

        // CoAP events
        case TriggerEvent::COAP_REQUEST_RECEIVED: return "COAP_REQUEST_RECEIVED";
        case TriggerEvent::COAP_RESPONSE_SENT: return "COAP_RESPONSE_SENT";
        case TriggerEvent::COAP_TIMEOUT: return "COAP_TIMEOUT";
        case TriggerEvent::COAP_RETRANSMIT: return "COAP_RETRANSMIT";
        case TriggerEvent::COAP_ACK_RECEIVED: return "COAP_ACK_RECEIVED";

        // DTLS events
        case TriggerEvent::DTLS_HANDSHAKE_STARTED: return "DTLS_HANDSHAKE_STARTED";
        case TriggerEvent::DTLS_HANDSHAKE_COMPLETE: return "DTLS_HANDSHAKE_COMPLETE";
        case TriggerEvent::DTLS_HANDSHAKE_FAILED: return "DTLS_HANDSHAKE_FAILED";
        case TriggerEvent::DTLS_SESSION_RESUMED: return "DTLS_SESSION_RESUMED";
        case TriggerEvent::DTLS_ALERT_RECEIVED: return "DTLS_ALERT_RECEIVED";

        // Transaction events
        case TriggerEvent::TRANSACTION_CREATED: return "TRANSACTION_CREATED";
        case TriggerEvent::TRANSACTION_COMPLETED: return "TRANSACTION_COMPLETED";
        case TriggerEvent::TRANSACTION_FAILED: return "TRANSACTION_FAILED";
        case TriggerEvent::TRANSACTION_TIMEOUT: return "TRANSACTION_TIMEOUT";

        // Application events
        case TriggerEvent::LWM2M_STEP_CALLED: return "LWM2M_STEP_CALLED";
        case TriggerEvent::SIGUSR1_RECEIVED: return "SIGUSR1_RECEIVED";
        case TriggerEvent::SHUTDOWN_INITIATED: return "SHUTDOWN_INITIATED";
        case TriggerEvent::ERROR_HANDLER_CALLED: return "ERROR_HANDLER_CALLED";

        // Object/Resource events
        case TriggerEvent::OBJECT_READ: return "OBJECT_READ";
        case TriggerEvent::OBJECT_WRITE: return "OBJECT_WRITE";
        case TriggerEvent::OBJECT_EXECUTE: return "OBJECT_EXECUTE";
        case TriggerEvent::OBJECT_CREATE: return "OBJECT_CREATE";
        case TriggerEvent::OBJECT_DELETE: return "OBJECT_DELETE";
        case TriggerEvent::ACCESS_DENIED: return "ACCESS_DENIED";

        default: return "UNKNOWN";
    }
}

const char* client_state_to_string(uint8_t state) noexcept {
    // These match the lwm2m_client_state_t enum from Wakaama
    switch (state) {
        case 0: return "STATE_INITIAL";
        case 1: return "STATE_BOOTSTRAP_REQUIRED";
        case 2: return "STATE_BOOTSTRAPPING";
        case 3: return "STATE_REGISTER_REQUIRED";
        case 4: return "STATE_REGISTERING";
        case 5: return "STATE_READY";
        case 6: return "STATE_REG_UPDATE_PENDING";
        case 7: return "STATE_REG_UPDATE_NEEDED";
        case 8: return "STATE_DEREGISTERING";
        default: return "STATE_UNKNOWN";
    }
}

} // namespace ftl
