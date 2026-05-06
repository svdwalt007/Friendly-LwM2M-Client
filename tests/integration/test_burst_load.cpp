/**
 * @file test_burst_load.cpp
 * @brief Integration tests for P0-5 burst-load stall detection
 *
 * This test suite validates that the LwM2M client can handle burst loads
 * without entering a stall state. It uses the Python burst_driver.py
 * reproducer to simulate the FT trace pattern.
 *
 * Test Scenario (from FT trace):
 * - 17 instance-level CON GET requests over 36 seconds
 * - 6 requests return 4.01 (Unauthorized)
 * - 11 requests return 2.05 (Content)
 * - Post-burst: client must send UPDATE within (lt * 0.75) + 5%
 *
 * Acceptance Criteria:
 * - UPDATE fires post-burst within expected window
 * - transaction_count returns to 0 within 5 seconds
 * - Memory and FD counts return to baseline within 10 seconds
 *
 * Reference: Issues/pcap-combo.md Section "P0-5 (NEW) - Burst-load stall"
 */

#include <gtest/gtest.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

// Forward declarations for client introspection
// These will be implemented as part of P1-8 observability work
namespace wpp {
class WppClient;
}

/**
 * @brief Helper to execute shell commands and capture output
 */
class CommandExecutor {
public:
    struct Result {
        int exit_code;
        std::string stdout_output;
        std::string stderr_output;
    };

    static Result execute(const std::string& command, int timeout_seconds = 300) {
        Result result;
        result.exit_code = -1;

        // Build command with timeout and output capture
        std::string full_command = command + " 2>&1";

        FILE* pipe = popen(full_command.c_str(), "r");
        if (!pipe) {
            result.stderr_output = "Failed to execute command";
            return result;
        }

        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result.stdout_output += buffer;
        }

        result.exit_code = pclose(pipe);

        // Convert to actual exit code (pclose returns status in specific format)
#ifdef _WIN32
        // Windows: exit code is direct
#else
        // POSIX: extract exit code from status
        if (WIFEXITED(result.exit_code)) {
            result.exit_code = WEXITSTATUS(result.exit_code);
        }
#endif

        return result;
    }
};

/**
 * @brief Resource baseline measurement
 */
struct ResourceBaseline {
    size_t memory_kb;
    size_t fd_count;
    size_t transaction_count;
    size_t token_count;
};

/**
 * @brief Test fixture for burst load tests
 */
class BurstLoadTest : public ::testing::Test {
protected:
    // Test configuration
    static constexpr const char* ENDPOINT = "TestClient";
    static constexpr const char* LESHAN_URL = "http://localhost:8080";
    static constexpr int LIFETIME_S = 60;
    static constexpr int BURST_DURATION_S = 36;

    // Paths
    static constexpr const char* BURST_DRIVER_PATH = "tools/burst-load-reproducer/burst_driver.py";
    static constexpr const char* RESULTS_PATH = "/tmp/burst-results.json";

    void SetUp() override {
        // TODO: Start client if not running
        // TODO: Verify Leshan is accessible
        // TODO: Capture resource baseline
    }

    void TearDown() override {
        // TODO: Cleanup test artifacts
    }

    /**
     * @brief Run the Python burst driver reproducer
     * @param extra_args Additional command line arguments
     * @return Command execution result
     */
    CommandExecutor::Result runBurstDriver(const std::string& extra_args = "") {
        std::ostringstream cmd;
        cmd << "python3 " << BURST_DRIVER_PATH
            << " --endpoint " << ENDPOINT
            << " --duration " << BURST_DURATION_S
            << " --lt " << LIFETIME_S
            << " --leshan-url " << LESHAN_URL
            << " --output " << RESULTS_PATH
            << " " << extra_args;

        return CommandExecutor::execute(cmd.str(), 600);  // 10 minute timeout
    }

    /**
     * @brief Get current resource usage (stub - requires P1-8 implementation)
     */
    ResourceBaseline getResourceBaseline() {
        ResourceBaseline baseline;
        baseline.memory_kb = 0;
        baseline.fd_count = 0;
        baseline.transaction_count = 0;
        baseline.token_count = 0;

        // TODO: Implement when P1-8 introspection is available
        // baseline.transaction_count = WppClient::getInstance().getTransactionCount();
        // baseline.token_count = WppClient::getInstance().getPendingTokenCount();

        return baseline;
    }

    /**
     * @brief Wait for resource counts to return to baseline
     * @param baseline Expected baseline values
     * @param timeout_s Maximum wait time
     * @return true if baseline restored within timeout
     */
    bool waitForBaselineRestore(const ResourceBaseline& baseline, int timeout_s) {
        auto start = std::chrono::steady_clock::now();
        auto timeout = std::chrono::seconds(timeout_s);

        while (std::chrono::steady_clock::now() - start < timeout) {
            auto current = getResourceBaseline();

            // Check if all values returned to baseline
            if (current.transaction_count <= baseline.transaction_count &&
                current.token_count <= baseline.token_count) {
                return true;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        return false;
    }

    /**
     * @brief Parse burst results JSON file
     * @return true if stall was detected
     */
    bool parseResultsForStall() {
        std::ifstream file(RESULTS_PATH);
        if (!file.is_open()) {
            return true;  // Assume stall if results not available
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

        // Simple JSON parsing - look for stall_detected field
        // In production, use a proper JSON library
        if (content.find("\"stall_detected\": true") != std::string::npos) {
            return true;
        }
        if (content.find("\"stall_detected\":true") != std::string::npos) {
            return true;
        }

        return false;
    }
};

/**
 * @test Verify client does not stall under FT trace burst pattern
 *
 * This is the primary P0-5 acceptance test. It runs the exact burst pattern
 * observed in the FT trace and verifies the client remains healthy.
 */
TEST_F(BurstLoadTest, NoStallUnder17Gets36Seconds) {
    // Skip if burst driver not available
    auto check = CommandExecutor::execute("python3 --version");
    if (check.exit_code != 0) {
        GTEST_SKIP() << "Python3 not available";
    }

    // Capture baseline before burst
    ResourceBaseline baseline = getResourceBaseline();

    // Run burst driver (the Python reproducer)
    auto result = runBurstDriver();

    // Assert: burst driver exits with code 0 (no stall)
    EXPECT_EQ(result.exit_code, 0)
        << "Burst driver detected stall. Output:\n" << result.stdout_output;

    // Assert: no stall detected in results file
    EXPECT_FALSE(parseResultsForStall())
        << "Results file indicates stall was detected";

    // Assert: transaction_count returns to baseline within 5 seconds
    // Note: This requires P1-8 observability implementation
    // EXPECT_TRUE(waitForBaselineRestore(baseline, 5))
    //     << "Resources did not return to baseline within 5 seconds";
}

/**
 * @test Verify UPDATE fires within expected window after burst
 *
 * Per LwM2M spec, client should send UPDATE at approximately lt * 0.75.
 * We allow 5% tolerance beyond that window.
 */
TEST_F(BurstLoadTest, UpdateFiresPostBurst) {
    auto check = CommandExecutor::execute("python3 --version");
    if (check.exit_code != 0) {
        GTEST_SKIP() << "Python3 not available";
    }

    // Run burst driver
    auto result = runBurstDriver();

    // The burst driver already checks for UPDATE timing
    // Exit code 0 means UPDATE was detected within window
    EXPECT_EQ(result.exit_code, 0)
        << "UPDATE not received within expected window. Output:\n"
        << result.stdout_output;
}

/**
 * @test Verify transaction count returns to zero after burst
 *
 * If transactions are leaking, this count will remain elevated after
 * the burst completes, indicating a resource leak in the 4.01 path.
 */
TEST_F(BurstLoadTest, TransactionCountReturnsToZero) {
    // TODO: Implement when P1-8 introspection is available
    GTEST_SKIP() << "Requires P1-8 introspection implementation";

    // Capture baseline
    ResourceBaseline baseline = getResourceBaseline();

    // Run burst (skip health check for faster test)
    auto result = runBurstDriver("--skip-health-check");
    ASSERT_EQ(result.exit_code, 0) << "Burst failed: " << result.stdout_output;

    // Wait for cleanup
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Check transaction count
    auto current = getResourceBaseline();
    EXPECT_EQ(current.transaction_count, baseline.transaction_count)
        << "Transaction count elevated after burst: "
        << current.transaction_count << " (expected " << baseline.transaction_count << ")";
}

/**
 * @test Verify memory returns to baseline after burst
 *
 * Memory growth during burst is expected, but it should return to
 * baseline within 10 seconds of burst completion.
 */
TEST_F(BurstLoadTest, MemoryReturnsToBaseline) {
    // TODO: Implement when P1-8 introspection is available
    GTEST_SKIP() << "Requires P1-8 introspection implementation";

    ResourceBaseline baseline = getResourceBaseline();

    auto result = runBurstDriver("--skip-health-check");
    ASSERT_EQ(result.exit_code, 0);

    // Allow time for cleanup
    std::this_thread::sleep_for(std::chrono::seconds(10));

    auto current = getResourceBaseline();

    // Allow 10% growth tolerance
    size_t tolerance = baseline.memory_kb / 10;
    EXPECT_LE(current.memory_kb, baseline.memory_kb + tolerance)
        << "Memory elevated after burst: " << current.memory_kb << " KB "
        << "(baseline: " << baseline.memory_kb << " KB)";
}

/**
 * @test Verify file descriptors return to baseline after burst
 *
 * FD leaks would indicate socket or file handle issues in the
 * request processing path.
 */
TEST_F(BurstLoadTest, FileDescriptorsReturnToBaseline) {
    // TODO: Implement when P1-8 introspection is available
    GTEST_SKIP() << "Requires P1-8 introspection implementation";

    ResourceBaseline baseline = getResourceBaseline();

    auto result = runBurstDriver("--skip-health-check");
    ASSERT_EQ(result.exit_code, 0);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    auto current = getResourceBaseline();
    EXPECT_LE(current.fd_count, baseline.fd_count + 2)  // Allow 2 FD tolerance
        << "FD count elevated after burst: " << current.fd_count
        << " (baseline: " << baseline.fd_count << ")";
}

/**
 * @test Sustained burst test (soak test)
 *
 * Run a longer burst (200 requests over 400 seconds) to verify
 * the client can handle extended management sessions.
 *
 * This test is marked as slow and may be skipped in CI.
 */
TEST_F(BurstLoadTest, DISABLED_SustainedBurstSoak) {
    auto check = CommandExecutor::execute("python3 --version");
    if (check.exit_code != 0) {
        GTEST_SKIP() << "Python3 not available";
    }

    // Extended burst parameters
    // Note: This requires modifying the pattern in burst_driver.py
    // or adding a --repeat option
    auto result = runBurstDriver("--health-timeout 600");

    EXPECT_EQ(result.exit_code, 0)
        << "Sustained burst test failed. Output:\n" << result.stdout_output;
}

/**
 * @test Rapid fire stress test
 *
 * Send requests as fast as possible (1 second total duration)
 * to stress test the request handling path.
 */
TEST_F(BurstLoadTest, RapidFireStress) {
    auto check = CommandExecutor::execute("python3 --version");
    if (check.exit_code != 0) {
        GTEST_SKIP() << "Python3 not available";
    }

    // Very short duration = rapid fire
    std::ostringstream cmd;
    cmd << "python3 " << BURST_DRIVER_PATH
        << " --endpoint " << ENDPOINT
        << " --duration 1"  // Rapid fire
        << " --lt " << LIFETIME_S
        << " --leshan-url " << LESHAN_URL
        << " --health-timeout 120";  // Shorter health timeout

    auto result = CommandExecutor::execute(cmd.str(), 180);

    EXPECT_EQ(result.exit_code, 0)
        << "Rapid fire stress test failed. Output:\n" << result.stdout_output;
}

/**
 * @brief Main entry point for test executable
 */
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
