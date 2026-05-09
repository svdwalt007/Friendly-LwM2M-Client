/**
 * @file test_log_ring.cpp
 * @brief Unit tests for ftl::LogRing — ring-buffer logging system (P1-8)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "log_ring.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>

namespace ftl {
namespace test {

class LogRingTest : public ::testing::Test {
protected:
    void SetUp() override {
        ring_ = std::make_unique<LogRing>();
    }

    std::unique_ptr<LogRing> ring_;
};

// ============================================================================
// Construction / State
// ============================================================================

TEST_F(LogRingTest, NewRingIsEmpty) {
    EXPECT_EQ(ring_->count(), 0u);
    EXPECT_EQ(ring_->total_writes(), 0u);
    EXPECT_EQ(ring_->get_current_state(), 0u);
    EXPECT_EQ(ring_->get_lifetime(), 60u);
}

TEST_F(LogRingTest, MonotonicTimeIsNonZero) {
    uint64_t t1 = LogRing::get_monotonic_time_us();
    uint64_t t2 = LogRing::get_monotonic_time_us();
    EXPECT_GT(t1, 0u);
    EXPECT_GE(t2, t1);
}

TEST_F(LogRingTest, GetOnEmptyReturnsEmptyEntry) {
    const auto& entry = ring_->get(0);
    EXPECT_EQ(entry.t_mono_us, 0u);
    EXPECT_EQ(entry.sequence_number, 0u);
    EXPECT_EQ(entry.trigger, 0u);
}

// ============================================================================
// log_transition
// ============================================================================

TEST_F(LogRingTest, LogTransitionIncrementsCount) {
    ring_->log_transition(0, 1, TriggerEvent::REGISTRATION_STARTED);
    EXPECT_EQ(ring_->count(), 1u);
    EXPECT_EQ(ring_->total_writes(), 1u);
    EXPECT_EQ(ring_->get_current_state(), 1u);
}

TEST_F(LogRingTest, LogTransitionStoresFields) {
    ring_->log_transition(3, 4, TriggerEvent::REGISTRATION_UPDATE_SENT, -7);
    const auto& entry = ring_->get(0);
    EXPECT_EQ(entry.state_from, 3u);
    EXPECT_EQ(entry.state_to, 4u);
    EXPECT_EQ(entry.trigger, static_cast<uint16_t>(TriggerEvent::REGISTRATION_UPDATE_SENT));
    EXPECT_EQ(entry.error_code, -7);
    EXPECT_GT(entry.t_mono_us, 0u);
    EXPECT_EQ(entry.sequence_number, 0u);
}

TEST_F(LogRingTest, RawTriggerOverloadIsAccepted) {
    ring_->log_transition(static_cast<uint8_t>(0), static_cast<uint8_t>(1), uint16_t{0xABCD}, 0);
    const auto& entry = ring_->get(0);
    EXPECT_EQ(entry.trigger, 0xABCDu);
}

TEST_F(LogRingTest, SequenceNumbersAreOrdered) {
    ring_->log_transition(0, 1, TriggerEvent::REGISTRATION_STARTED);
    ring_->log_transition(1, 2, TriggerEvent::COAP_REQUEST_RECEIVED);
    ring_->log_transition(2, 3, TriggerEvent::COAP_RESPONSE_SENT);
    EXPECT_EQ(ring_->get(0).sequence_number, 0u);
    EXPECT_EQ(ring_->get(1).sequence_number, 1u);
    EXPECT_EQ(ring_->get(2).sequence_number, 2u);
}

TEST_F(LogRingTest, OutOfRangeIndexReturnsEmpty) {
    ring_->log_transition(0, 1, TriggerEvent::REGISTRATION_STARTED);
    const auto& entry = ring_->get(99);
    EXPECT_EQ(entry.sequence_number, 0u);
    EXPECT_EQ(entry.trigger, 0u);
    EXPECT_EQ(entry.t_mono_us, 0u);
}

TEST_F(LogRingTest, RingBufferWrapsAtSize) {
    constexpr size_t writes = LogRing::SIZE + 5;
    for (size_t i = 0; i < writes; ++i) {
        ring_->log_transition(0, 1, TriggerEvent::COAP_REQUEST_RECEIVED);
    }
    EXPECT_EQ(ring_->count(), LogRing::SIZE);
    EXPECT_EQ(ring_->total_writes(), writes);

    const auto& oldest = ring_->get(0);
    EXPECT_EQ(oldest.sequence_number, writes - LogRing::SIZE);
}

TEST_F(LogRingTest, CountersAreCapturedInEntry) {
    ring_->counters().transaction_count.store(7);
    ring_->counters().pending_token_count.store(3);
    ring_->counters().mid_alloc_count.store(11);
    ring_->counters().nvm_inflight_writes.store(2);

    ring_->log_transition(0, 1, TriggerEvent::TRANSACTION_CREATED);

    const auto& entry = ring_->get(0);
    EXPECT_EQ(entry.transaction_count, 7u);
    EXPECT_EQ(entry.pending_token_count, 3u);
    EXPECT_EQ(entry.mid_alloc_count, 11u);
    EXPECT_EQ(entry.nvm_inflight_writes, 2u);
}

// ============================================================================
// Sub-tracker reset / record
// ============================================================================

TEST_F(LogRingTest, IntrospectionCountersResetClearsAll) {
    auto& c = ring_->counters();
    c.transaction_count.store(5);
    c.pending_token_count.store(7);
    c.mid_alloc_count.store(9);
    c.nvm_inflight_writes.store(11);
    c.total_requests.store(20);
    c.total_2xx_responses.store(15);
    c.total_4xx_responses.store(2);

    c.reset();

    EXPECT_EQ(c.transaction_count.load(), 0u);
    EXPECT_EQ(c.pending_token_count.load(), 0u);
    EXPECT_EQ(c.mid_alloc_count.load(), 0u);
    EXPECT_EQ(c.nvm_inflight_writes.load(), 0u);
    EXPECT_EQ(c.total_requests.load(), 0u);
    EXPECT_EQ(c.total_2xx_responses.load(), 0u);
    EXPECT_EQ(c.total_4xx_responses.load(), 0u);
}

TEST_F(LogRingTest, UpdateTimestampsRecordAndCap) {
    auto& ts = ring_->update_timestamps();
    for (size_t i = 0; i < UpdateTimestamps::MAX_TIMESTAMPS + 5; ++i) {
        ts.record(static_cast<uint64_t>(i + 1));
    }
    EXPECT_EQ(ts.count.load(), UpdateTimestamps::MAX_TIMESTAMPS);

    ts.reset();
    EXPECT_EQ(ts.count.load(), 0u);
    EXPECT_EQ(ts.next_index.load(), 0u);
    EXPECT_EQ(ts.timestamps[0], 0u);
}

TEST_F(LogRingTest, MidTrackerWraps) {
    auto& mt = ring_->mid_tracker();
    for (size_t i = 0; i < MidTracker::MAX_MIDS * 2; ++i) {
        mt.record(static_cast<uint16_t>(i + 100));
    }
    EXPECT_EQ(mt.next_index.load(), MidTracker::MAX_MIDS * 2);

    mt.reset();
    EXPECT_EQ(mt.next_index.load(), 0u);
    EXPECT_EQ(mt.mids[0], 0u);
}

TEST_F(LogRingTest, DtlsOutcomesRecordAndReset) {
    auto& d = ring_->dtls_outcomes();
    d.record(1000, 0, true);
    d.record(2000, -1, false);
    d.record(3000, 0, true);

    EXPECT_EQ(d.next_index.load(), 3u);
    EXPECT_EQ(d.outcomes[0].timestamp_us, 1000u);
    EXPECT_TRUE(d.outcomes[0].success);
    EXPECT_EQ(d.outcomes[1].result_code, -1);
    EXPECT_FALSE(d.outcomes[1].success);

    d.reset();
    EXPECT_EQ(d.next_index.load(), 0u);
    EXPECT_EQ(d.outcomes[0].timestamp_us, 0u);
    EXPECT_FALSE(d.outcomes[0].success);
}

// ============================================================================
// State / Lifetime accessors
// ============================================================================

TEST_F(LogRingTest, SetGetCurrentState) {
    ring_->set_current_state(5);
    EXPECT_EQ(ring_->get_current_state(), 5u);
}

TEST_F(LogRingTest, SetGetLifetime) {
    ring_->set_lifetime(3600);
    EXPECT_EQ(ring_->get_lifetime(), 3600u);
}

// ============================================================================
// Reset
// ============================================================================

TEST_F(LogRingTest, ResetClearsAllState) {
    ring_->log_transition(0, 1, TriggerEvent::REGISTRATION_STARTED);
    ring_->log_transition(1, 2, TriggerEvent::REGISTRATION_UPDATE_SENT);
    ring_->set_current_state(3);
    ring_->set_lifetime(7200);
    ring_->counters().transaction_count.store(99);
    ring_->update_timestamps().record(123456);
    ring_->mid_tracker().record(0xABCD);
    ring_->dtls_outcomes().record(1000, 0, true);

    ring_->reset();

    EXPECT_EQ(ring_->count(), 0u);
    EXPECT_EQ(ring_->total_writes(), 0u);
    EXPECT_EQ(ring_->get_current_state(), 0u);
    EXPECT_EQ(ring_->get_lifetime(), 60u);
    EXPECT_EQ(ring_->counters().transaction_count.load(), 0u);
    EXPECT_EQ(ring_->update_timestamps().count.load(), 0u);
    EXPECT_EQ(ring_->mid_tracker().next_index.load(), 0u);
    EXPECT_EQ(ring_->dtls_outcomes().next_index.load(), 0u);
}

// ============================================================================
// Snapshot generation
// ============================================================================

TEST_F(LogRingTest, GenerateSnapshotProducesValidJsonShape) {
    ring_->log_transition(0, 1, TriggerEvent::REGISTRATION_STARTED, 0);
    ring_->log_transition(1, 4, TriggerEvent::REGISTRATION_UPDATE_SENT, 0);
    ring_->log_transition(4, 5, TriggerEvent::REGISTRATION_UPDATE_ACK, 0);
    ring_->set_lifetime(120);
    ring_->update_timestamps().record(123);
    ring_->mid_tracker().record(0x1234);
    ring_->dtls_outcomes().record(99999, 0, true);

    std::string json = ring_->generate_snapshot();

    EXPECT_NE(json.find("\"snapshot_version\""), std::string::npos);
    EXPECT_NE(json.find("\"client_state\""), std::string::npos);
    EXPECT_NE(json.find("\"client_state_name\""), std::string::npos);
    EXPECT_NE(json.find("\"ring_buffer\""), std::string::npos);
    EXPECT_NE(json.find("\"counters\""), std::string::npos);
    EXPECT_NE(json.find("\"last_update_timestamps_us\""), std::string::npos);
    EXPECT_NE(json.find("\"last_mids\""), std::string::npos);
    EXPECT_NE(json.find("\"last_dtls_outcomes\""), std::string::npos);
    EXPECT_NE(json.find("\"recent_entries\""), std::string::npos);
    EXPECT_NE(json.find("REGISTRATION_UPDATE_ACK"), std::string::npos);
    // Balanced braces sanity check
    size_t opens = std::count(json.begin(), json.end(), '{');
    size_t closes = std::count(json.begin(), json.end(), '}');
    EXPECT_EQ(opens, closes);
}

TEST_F(LogRingTest, GenerateSnapshotWithFullBufferShows10Recent) {
    for (size_t i = 0; i < 25; ++i) {
        ring_->log_transition(0, 1, TriggerEvent::COAP_REQUEST_RECEIVED);
    }
    std::string json = ring_->generate_snapshot();
    // Should contain at most 10 recent entries — count occurrences of "sequence":
    size_t count = 0;
    size_t pos = 0;
    while ((pos = json.find("\"sequence\"", pos)) != std::string::npos) {
        ++count;
        ++pos;
    }
    EXPECT_LE(count, 10u);
    EXPECT_GT(count, 0u);
}

TEST_F(LogRingTest, SnapshotToFileWritesFile) {
    ring_->log_transition(0, 1, TriggerEvent::REGISTRATION_STARTED);
    ring_->log_transition(1, 2, TriggerEvent::DTLS_HANDSHAKE_STARTED);

    const char* path = "/tmp/test_log_ring_snapshot.json";
    std::remove(path);

    ASSERT_TRUE(ring_->snapshot_to_file(path));

    std::ifstream f(path);
    ASSERT_TRUE(f.is_open());
    std::stringstream buffer;
    buffer << f.rdbuf();
    std::string content = buffer.str();
    EXPECT_NE(content.find("snapshot_version"), std::string::npos);
    EXPECT_NE(content.find("REGISTRATION_STARTED"), std::string::npos);
    f.close();
    std::remove(path);
}

TEST_F(LogRingTest, SnapshotToInvalidPathReturnsFalse) {
    EXPECT_FALSE(ring_->snapshot_to_file("/nonexistent_dir/cant_create_here.json"));
}

// ============================================================================
// trigger_event_to_string
// ============================================================================

TEST(LogRingHelpersTest, TriggerEventToStringKnownEvents) {
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::UNKNOWN), "UNKNOWN");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::REGISTRATION_STARTED), "REGISTRATION_STARTED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::REGISTRATION_UPDATE_SENT), "REGISTRATION_UPDATE_SENT");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::REGISTRATION_UPDATE_ACK), "REGISTRATION_UPDATE_ACK");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::REGISTRATION_FAILED), "REGISTRATION_FAILED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::REGISTRATION_TIMEOUT), "REGISTRATION_TIMEOUT");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::DEREGISTRATION_SENT), "DEREGISTRATION_SENT");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::DEREGISTRATION_ACK), "DEREGISTRATION_ACK");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::COAP_REQUEST_RECEIVED), "COAP_REQUEST_RECEIVED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::COAP_RESPONSE_SENT), "COAP_RESPONSE_SENT");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::COAP_TIMEOUT), "COAP_TIMEOUT");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::COAP_RETRANSMIT), "COAP_RETRANSMIT");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::COAP_ACK_RECEIVED), "COAP_ACK_RECEIVED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::DTLS_HANDSHAKE_STARTED), "DTLS_HANDSHAKE_STARTED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::DTLS_HANDSHAKE_COMPLETE), "DTLS_HANDSHAKE_COMPLETE");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::DTLS_HANDSHAKE_FAILED), "DTLS_HANDSHAKE_FAILED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::DTLS_SESSION_RESUMED), "DTLS_SESSION_RESUMED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::DTLS_ALERT_RECEIVED), "DTLS_ALERT_RECEIVED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::TRANSACTION_CREATED), "TRANSACTION_CREATED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::TRANSACTION_COMPLETED), "TRANSACTION_COMPLETED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::TRANSACTION_FAILED), "TRANSACTION_FAILED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::TRANSACTION_TIMEOUT), "TRANSACTION_TIMEOUT");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::LWM2M_STEP_CALLED), "LWM2M_STEP_CALLED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::SIGUSR1_RECEIVED), "SIGUSR1_RECEIVED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::SHUTDOWN_INITIATED), "SHUTDOWN_INITIATED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::ERROR_HANDLER_CALLED), "ERROR_HANDLER_CALLED");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::OBJECT_READ), "OBJECT_READ");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::OBJECT_WRITE), "OBJECT_WRITE");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::OBJECT_EXECUTE), "OBJECT_EXECUTE");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::OBJECT_CREATE), "OBJECT_CREATE");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::OBJECT_DELETE), "OBJECT_DELETE");
    EXPECT_STREQ(trigger_event_to_string(TriggerEvent::ACCESS_DENIED), "ACCESS_DENIED");
}

TEST(LogRingHelpersTest, TriggerEventToStringUnknown) {
    EXPECT_STREQ(trigger_event_to_string(static_cast<TriggerEvent>(0xFFFF)), "UNKNOWN");
}

// ============================================================================
// client_state_to_string
// ============================================================================

TEST(LogRingHelpersTest, ClientStateToStringAllKnown) {
    EXPECT_STREQ(client_state_to_string(0), "STATE_INITIAL");
    EXPECT_STREQ(client_state_to_string(1), "STATE_BOOTSTRAP_REQUIRED");
    EXPECT_STREQ(client_state_to_string(2), "STATE_BOOTSTRAPPING");
    EXPECT_STREQ(client_state_to_string(3), "STATE_REGISTER_REQUIRED");
    EXPECT_STREQ(client_state_to_string(4), "STATE_REGISTERING");
    EXPECT_STREQ(client_state_to_string(5), "STATE_READY");
    EXPECT_STREQ(client_state_to_string(6), "STATE_REG_UPDATE_PENDING");
    EXPECT_STREQ(client_state_to_string(7), "STATE_REG_UPDATE_NEEDED");
    EXPECT_STREQ(client_state_to_string(8), "STATE_DEREGISTERING");
    EXPECT_STREQ(client_state_to_string(255), "STATE_UNKNOWN");
}

// ============================================================================
// Singleton
// ============================================================================

TEST(LogRingSingletonTest, GlobalInstanceIsSame) {
    auto& a = get_log_ring();
    auto& b = get_log_ring();
    EXPECT_EQ(&a, &b);
}

// ============================================================================
// Concurrency
// ============================================================================

TEST_F(LogRingTest, MultiProducerWritesAreCounted) {
    constexpr int threadCount = 4;
    constexpr int perThread = 100;
    std::vector<std::thread> threads;

    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([this, t]() {
            for (int i = 0; i < perThread; ++i) {
                ring_->log_transition(static_cast<uint8_t>(t),
                                       static_cast<uint8_t>(t + 1),
                                       TriggerEvent::COAP_REQUEST_RECEIVED, 0);
            }
        });
    }
    for (auto& th : threads) th.join();

    EXPECT_EQ(ring_->total_writes(), static_cast<uint64_t>(threadCount * perThread));
}

} // namespace test
} // namespace ftl
