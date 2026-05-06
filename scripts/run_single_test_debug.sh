#!/bin/bash
#
# Run Single Test with Debug Output
# Helps diagnose test hangs by showing what GoogleTest is doing
#
# Usage: ./scripts/run_single_test_debug.sh [test_name]
#        ./scripts/run_single_test_debug.sh test_delta_algorithms
#

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

if [ $# -eq 0 ]; then
    echo "Usage: $0 <test_executable>"
    echo ""
    echo "Available tests:"
    echo "  test_delta_algorithms"
    echo "  test_blockwise_transfer"
    echo "  test_rollback_manager"
    echo "  test_firmware_update_object"
    exit 1
fi

TEST_NAME="$1"
TEST_PATH="${BUILD_DIR}/${TEST_NAME}"

if [ ! -f "${TEST_PATH}" ]; then
    echo "Error: Test executable not found: ${TEST_PATH}"
    exit 1
fi

echo "=================================================="
echo "Running Test with Debug Output"
echo "=================================================="
echo ""
echo "Test: ${TEST_NAME}"
echo "Path: ${TEST_PATH}"
echo ""

# First, list all tests to see what will run
echo "Step 1: Listing all test cases..."
echo ""
"${TEST_PATH}" --gtest_list_tests 2>&1 || {
    echo "ERROR: Failed to list tests"
    echo "This may indicate the test executable has issues"
    exit 1
}

echo ""
echo "=================================================="
echo "Step 2: Running tests with verbose output..."
echo "=================================================="
echo ""

# Run with:
# - Verbose output (--gtest_print_time=1)
# - Color output (--gtest_color=yes)
# - Shuffle off for reproducibility (--gtest_shuffle=0)
# - With timeout
timeout 60 "${TEST_PATH}" \
    --gtest_print_time=1 \
    --gtest_color=yes \
    --gtest_shuffle=0 \
    2>&1 || {
    EXIT_CODE=$?
    echo ""
    echo "=================================================="
    echo "Test Failed or Timed Out"
    echo "=================================================="
    echo ""
    if [ $EXIT_CODE -eq 124 ]; then
        echo "✗ Test timed out after 60 seconds"
        echo ""
        echo "This indicates:"
        echo "  - Test is hanging in a specific operation"
        echo "  - Likely infinite loop or blocking call"
        echo "  - Need to identify which test case is hanging"
        echo ""
        echo "Try running individual test cases:"
        echo "  ${TEST_PATH} --gtest_filter='TestFixture.TestName'"
    else
        echo "✗ Test failed with exit code: $EXIT_CODE"
    fi
    exit $EXIT_CODE
}

echo ""
echo "=================================================="
echo "✓ Test Completed Successfully"
echo "=================================================="
