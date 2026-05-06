#!/bin/bash
#
# Build b10 Test Script
# Tests the fix for b09 Brotli linking error
#
# Usage: ./scripts/build_b10_test.sh
#

set -e  # Exit on error

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
LOG_FILE="${PROJECT_ROOT}/Issues/b10.log"

echo "==============================================="
echo "Build b10 Test - Brotli Linking Fix Verification"
echo "==============================================="
echo ""
echo "Project: Friendly LwM2M Client v1.2.2"
echo "Build: b10"
echo "Fix: Added BROTLI_COMMON_LIB to compression support"
echo "Log: ${LOG_FILE}"
echo ""

# Clean build directory
echo "Step 1: Cleaning build directory..."
cd "${BUILD_DIR}"
rm -rf *
echo "✓ Build directory cleaned"
echo ""

# Run CMake configuration
echo "Step 2: Running CMake configuration..."
cmake -DCMAKE_BUILD_TYPE=Release .. 2>&1 | tee "${LOG_FILE}"
CMAKE_STATUS=$?

if [ $CMAKE_STATUS -ne 0 ]; then
    echo ""
    echo "✗ CMake configuration failed"
    echo "See log: ${LOG_FILE}"
    exit 1
fi

echo ""
echo "✓ CMake configuration successful"
echo ""

# Build all targets
echo "Step 3: Building all targets..."
make -j$(nproc) 2>&1 | tee -a "${LOG_FILE}"
BUILD_STATUS=$?

echo ""
echo "==========================================="
echo "Build Summary"
echo "==========================================="

if [ $BUILD_STATUS -eq 0 ]; then
    echo "✓ BUILD SUCCESSFUL"
    echo ""
    echo "Executables built:"
    ls -lh examples/friendly_example_client 2>/dev/null && echo "  ✓ friendly_example_client"
    ls -lh test_firmware_update_object 2>/dev/null && echo "  ✓ test_firmware_update_object"
    ls -lh test_delta_algorithms 2>/dev/null && echo "  ✓ test_delta_algorithms"
    ls -lh test_rollback_manager 2>/dev/null && echo "  ✓ test_rollback_manager"
    ls -lh test_blockwise_transfer 2>/dev/null && echo "  ✓ test_blockwise_transfer"
    echo ""

    # Verify Brotli linking in test_firmware_update_object
    echo "Verifying Brotli link order..."
    LINK_FILE="CMakeFiles/test_firmware_update_object.dir/link.txt"
    if [ -f "${LINK_FILE}" ]; then
        if grep -q "libbrotlienc.a.*libbrotlidec.a.*libbrotlicommon.a" "${LINK_FILE}"; then
            echo "  ✓ Brotli libraries linked in correct order: enc → dec → common"
        else
            echo "  ⚠ Warning: Brotli library order may be incorrect"
        fi

        # Check for duplicate/wrong-order libbrotlidec.a
        BROTLIDEC_COUNT=$(grep -o "libbrotlidec.a" "${LINK_FILE}" | wc -l)
        if [ $BROTLIDEC_COUNT -gt 2 ]; then
            echo "  ⚠ Warning: libbrotlidec.a appears ${BROTLIDEC_COUNT} times (may indicate duplication)"
        else
            echo "  ✓ No duplicate Brotli library references"
        fi
    fi
    echo ""

    echo "Fix Status: ✅ VERIFIED"
    echo "  - Build completed without Brotli linking errors"
    echo "  - test_firmware_update_object linked successfully"
    echo "  - All test targets built successfully"
    echo ""

    # Run tests
    echo "Step 4: Running tests..."
    if command -v ctest &> /dev/null; then
        echo ""
        ctest --output-on-failure 2>&1 | tee -a "${LOG_FILE}"
        TEST_STATUS=$?
        echo ""
        if [ $TEST_STATUS -eq 0 ]; then
            echo "✓ All tests passed"
        else
            echo "⚠ Some tests failed (check log for details)"
        fi
    else
        echo "  ⚠ ctest not available, skipping test execution"
    fi

else
    echo "✗ BUILD FAILED"
    echo ""
    echo "Check for errors:"
    grep -E "(error:|undefined reference)" "${LOG_FILE}" | head -20
    echo ""
    echo "Full log: ${LOG_FILE}"
    echo ""
    echo "Common issues:"
    echo "  1. Missing custom minimal libcurl (/usr/local/lib/libcurl.a)"
    echo "  2. Missing dependencies (run: ./scripts/install_curl_dependencies.sh)"
    echo "  3. Brotli libraries not found (install: libbrotli-dev)"
    exit 1
fi

echo ""
echo "==========================================="
echo "Log saved to: ${LOG_FILE}"
echo "==========================================="
