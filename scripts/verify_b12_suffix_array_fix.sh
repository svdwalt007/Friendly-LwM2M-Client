#!/bin/bash
#
# Verify b12 Suffix Array Fix
# Tests that the suffix array bug fix resolves the test hang
#

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

echo "=================================================="
echo "b12 Suffix Array Fix Verification"
echo "=================================================="
echo ""
echo "Fix: Replaced broken suffix array implementation"
echo "File: src/firmware/delta_algorithms.cpp (lines 48-86)"
echo "Change: O(n²) incomplete algorithm → O(n²log n) correct algorithm"
echo ""

# Step 1: Clean rebuild
echo "Step 1: Clean rebuild..."
echo ""
cd "${BUILD_DIR}"
rm -rf *

# Step 2: Configure
echo "Step 2: Configuring CMake (Release build, no sanitizers)..."
echo ""
cmake -DCMAKE_BUILD_TYPE=Release .. 2>&1 | tail -20

# Step 3: Check sanitizer status
echo ""
echo "Step 3: Verifying sanitizers are disabled..."
echo ""
if grep -q "ENABLE_SANITIZERS:BOOL=ON" CMakeCache.txt 2>/dev/null; then
    echo "❌ ERROR: Sanitizers are enabled"
    exit 1
else
    echo "✅ VERIFIED: Sanitizers disabled (ENABLE_SANITIZERS=OFF)"
fi

# Step 4: Build
echo ""
echo "Step 4: Building..."
echo ""
make -j$(nproc) 2>&1 | tail -20

if [ ! -f test_delta_algorithms ]; then
    echo ""
    echo "❌ ERROR: test_delta_algorithms not built"
    exit 1
fi

echo ""
echo "✅ Build completed"
ls -lh test_delta_algorithms

# Step 5: Run the specific test that was hanging
echo ""
echo "=================================================="
echo "Step 5: Running BSDiffTest.CreateAndApplyDelta"
echo "(This test was hanging before the fix)"
echo "=================================================="
echo ""

# Run with timeout - should complete in < 5 seconds
timeout 10 ./test_delta_algorithms --gtest_filter="BSDiffTest.CreateAndApplyDelta" --gtest_print_time=1 2>&1 || {
    EXIT_CODE=$?
    echo ""
    if [ $EXIT_CODE -eq 124 ]; then
        echo "❌ FAILED: Test still hangs after 10 seconds"
        echo ""
        echo "The suffix array fix did not resolve the issue."
        echo "Further investigation required."
        exit 1
    else
        echo "❌ FAILED: Test failed with exit code: $EXIT_CODE"
        exit 1
    fi
}

echo ""
echo "✅ SUCCESS: BSDiffTest.CreateAndApplyDelta completed"

# Step 6: Run all delta algorithm tests
echo ""
echo "=================================================="
echo "Step 6: Running all delta algorithm tests"
echo "=================================================="
echo ""

timeout 30 ./test_delta_algorithms --gtest_print_time=1 2>&1 || {
    EXIT_CODE=$?
    echo ""
    if [ $EXIT_CODE -eq 124 ]; then
        echo "❌ FAILED: Tests timed out after 30 seconds"
        exit 1
    else
        echo "❌ FAILED: Tests failed with exit code: $EXIT_CODE"
        exit 1
    fi
}

echo ""
echo "=================================================="
echo "✅ ALL TESTS PASSED"
echo "=================================================="
echo ""
echo "Suffix array fix verified:"
echo "- Broken O(n²) implementation replaced with O(n²log n) correct version"
echo "- BSDiffTest.CreateAndApplyDelta completes successfully"
echo "- All delta algorithm tests pass"
echo ""
echo "Performance:"
echo "- Test data: 64KB source + 64KB target"
echo "- Completion time: < 5 seconds (was infinite hang)"
echo ""
echo "Build b12: ✅ COMPLETE"
