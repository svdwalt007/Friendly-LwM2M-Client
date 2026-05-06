#!/bin/bash
#
# Diagnose Test Hang
# Step-by-step diagnosis of what's causing tests to hang
#

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
TEST_NAME="test_delta_algorithms"
TEST_PATH="${BUILD_DIR}/${TEST_NAME}"

echo "=================================================="
echo "Test Hang Diagnostic Tool"
echo "=================================================="
echo ""
echo "Test: ${TEST_NAME}"
echo "Build: ${BUILD_DIR}"
echo ""

cd "${BUILD_DIR}"

# Check if test executable exists
echo "Step 1: Checking test executable..."
if [ ! -f "${TEST_PATH}" ]; then
    echo "✗ Test executable not found: ${TEST_PATH}"
    exit 1
fi
echo "✓ Found: ${TEST_PATH}"
ls -lh "${TEST_PATH}"
echo ""

# Check library dependencies
echo "Step 2: Checking library dependencies..."
echo ""
ldd "${TEST_PATH}" | grep -E "(brotli|bsdiff|vcdiff|zstd|bzip2|lzma)" || {
    echo "No compression library dependencies found"
}
echo ""

# Check for undefined symbols
echo "Step 3: Checking for undefined symbols..."
nm -u "${TEST_PATH}" | head -20 || {
    echo "Could not check symbols"
}
echo ""

# Try to run with strace to see system calls
echo "Step 4: Attempting to trace first few system calls..."
echo "(This will show what the test is doing when it starts)"
echo ""
timeout 5 strace -e trace=open,openat,read,write,mmap,futex "${TEST_PATH}" 2>&1 | head -100 || {
    echo ""
    echo "Trace ended (timeout or completion)"
}

echo ""
echo "Step 5: Attempting to list tests (this may hang)..."
echo ""
timeout 10 "${TEST_PATH}" --gtest_list_tests 2>&1 || {
    EXIT_CODE=$?
    echo ""
    if [ $EXIT_CODE -eq 124 ]; then
        echo "✗ HANG DETECTED: Test hangs during initialization"
        echo ""
        echo "The test cannot even list its test cases, which means:"
        echo "  - Hang occurs before main() runs test discovery"
        echo "  - Likely in static initialization"
        echo "  - Could be global object constructor"
        echo "  - Could be library initialization"
        echo ""
        echo "Common causes:"
        echo "  1. Static object with platform init (PlatformFactory)"
        echo "  2. Library trying to access hardware/files"
        echo "  3. Mutex/lock contention in static init"
        echo ""
        echo "Recommendation: Add debug printf to test constructors"
    else
        echo "✗ Test failed with exit code: $EXIT_CODE"
    fi
    exit 1
}

echo ""
echo "✓ Test can list its test cases (no hang in initialization)"
echo ""

echo "Step 6: Attempting to run first test only..."
echo ""
FIRST_TEST=$("${TEST_PATH}" --gtest_list_tests 2>&1 | grep -A1 "^[A-Z]" | head -2 | tail -1 | sed 's/^  //')
if [ -n "$FIRST_TEST" ]; then
    echo "Running first test: ${FIRST_TEST}"
    timeout 30 "${TEST_PATH}" --gtest_filter="*${FIRST_TEST}*" --gtest_print_time=1 2>&1 || {
        EXIT_CODE=$?
        echo ""
        if [ $EXIT_CODE -eq 124 ]; then
            echo "✗ HANG DETECTED: Test hangs during execution of first test"
            echo ""
            echo "Test case: ${FIRST_TEST}"
            echo ""
            echo "The hang occurs during test execution, which means:"
            echo "  - Initialization completed successfully"
            echo "  - Specific test code is blocking"
            echo ""
            echo "Recommendation: Add debug printf to test case"
        fi
        exit 1
    }
    echo "✓ First test completed successfully"
fi

echo ""
echo "=================================================="
echo "Diagnosis Complete"
echo "=================================================="
echo ""
echo "If you reached here, the test should run without hanging."
echo "Try running the full test suite:"
echo "  ${TEST_PATH}"
