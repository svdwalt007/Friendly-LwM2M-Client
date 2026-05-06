#!/bin/bash
# Test script for build38 GSSAPI/Kerberos linking fix
# Run this on your Linux build system (WSL/Docker/remote)

set -e  # Exit on error

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"

echo "=========================================="
echo "Testing build38 GSSAPI Linking Fix"
echo "=========================================="
echo ""

# Check for required GSSAPI libraries
echo "Checking for GSSAPI/Kerberos libraries..."
MISSING_LIBS=0

for lib in gssapi_krb5 krb5 k5crypto com_err keyutils; do
    if ldconfig -p | grep -q "lib${lib}"; then
        echo "  ✓ Found lib${lib}"
    else
        echo "  ✗ Missing lib${lib}"
        MISSING_LIBS=$((MISSING_LIBS + 1))
    fi
done

if [ $MISSING_LIBS -gt 0 ]; then
    echo ""
    echo "WARNING: Some GSSAPI libraries are missing."
    echo "Install with: sudo apt-get install libkrb5-dev libkeyutils-dev"
    echo "Build will continue but may fail if static libcurl/libssh require GSSAPI."
    echo ""
fi

# Clean build directory
echo ""
echo "Cleaning build directory..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Configure with CMake
echo ""
echo "Configuring with CMake..."
cd "$BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_TESTS=ON \
      -DBUILD_EXAMPLES=ON \
      -DDTLS_STATIC_LINK=ON \
      ..

# Build
echo ""
echo "Building project..."
make -j$(nproc) 2>&1 | tee build_output.log

# Check for specific errors from build38.log
echo ""
echo "Checking for previous build errors..."
ERRORS_FOUND=0

if grep -q "undefined reference to.*gss" build_output.log; then
    echo "  ✗ GSSAPI linking errors still present"
    ERRORS_FOUND=$((ERRORS_FOUND + 1))
else
    echo "  ✓ No GSSAPI linking errors"
fi

if grep -q "undefined reference to.*SSL_get_error" build_output.log; then
    echo "  ✗ OpenSSL linking errors still present"
    ERRORS_FOUND=$((ERRORS_FOUND + 1))
else
    echo "  ✓ No OpenSSL linking errors"
fi

# Verify executables were built
echo ""
echo "Checking built executables..."
MISSING_EXES=0

if [ -f "examples/friendly_example_client" ]; then
    echo "  ✓ friendly_example_client built successfully"
else
    echo "  ✗ friendly_example_client not found"
    MISSING_EXES=$((MISSING_EXES + 1))
fi

if [ -f "test_firmware_update_object" ]; then
    echo "  ✓ test_firmware_update_object built successfully"
else
    echo "  ✗ test_firmware_update_object not found"
    MISSING_EXES=$((MISSING_EXES + 1))
fi

# Check what libraries the executables link to
echo ""
echo "Verifying library linkage..."
if [ -f "examples/friendly_example_client" ]; then
    echo ""
    echo "Libraries linked by friendly_example_client:"
    ldd examples/friendly_example_client | grep -E "(gssapi|krb5|ssl|crypto|ssh|curl)" || echo "  No dynamic GSSAPI/SSL libs (using static linking)"
fi

# Run tests if built
if [ -f "test_firmware_update_object" ]; then
    echo ""
    echo "Running test_firmware_update_object..."
    if ./test_firmware_update_object --gtest_brief=1; then
        echo "  ✓ Tests passed"
    else
        echo "  ✗ Tests failed (but executable built successfully)"
    fi
fi

# Summary
echo ""
echo "=========================================="
echo "Summary"
echo "=========================================="

if [ $ERRORS_FOUND -eq 0 ] && [ $MISSING_EXES -eq 0 ]; then
    echo "✓ BUILD SUCCESSFUL - All build38 issues resolved!"
    echo ""
    echo "The following fixes were applied:"
    echo "  - Added GSSAPI/Kerberos library detection"
    echo "  - Linked gssapi_krb5, krb5, k5crypto, com_err, keyutils"
    echo "  - Fixed static linking order for OpenSSL"
    echo ""
    exit 0
else
    echo "✗ BUILD HAD ISSUES"
    if [ $ERRORS_FOUND -gt 0 ]; then
        echo "  - Linking errors still present"
    fi
    if [ $MISSING_EXES -gt 0 ]; then
        echo "  - Some executables failed to build"
    fi
    echo ""
    echo "See build_output.log for details"
    exit 1
fi
