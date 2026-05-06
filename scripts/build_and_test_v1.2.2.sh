#!/bin/bash
#
# Build and Test Script for Friendly LwM2M Client v1.2.2
#
# Author: Sean van der Walt
# Editor: Sean van der Walt
# Copyright (c) 2024-2026 Walt Technologies
# Version: 1.2.2
#

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored message
print_header() {
    echo -e "${BLUE}============================================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}============================================================${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Banner
clear
print_header "Friendly LwM2M Client v1.2.2"
echo ""
echo "Multi-DTLS Provider Build and Test Script"
echo "Author: Sean van der Walt"
echo ""

# Check prerequisites
print_header "Checking Prerequisites"

# Check for CMake
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1)
    print_success "CMake found: $CMAKE_VERSION"
else
    print_error "CMake not found. Please install CMake."
    exit 1
fi

# Check for Make
if command -v make &> /dev/null; then
    print_success "Make found"
else
    print_error "Make not found. Please install build-essential."
    exit 1
fi

# Check for DTLS libraries
echo ""
print_header "Checking DTLS Provider Libraries"

HAVE_MBEDTLS=false
HAVE_OPENSSL=false
HAVE_WOLFSSL=false

# Check for mbedTLS
if pkg-config --exists mbedtls 2>/dev/null; then
    MBEDTLS_VERSION=$(pkg-config --modversion mbedtls)
    print_success "mbedTLS found: $MBEDTLS_VERSION"
    HAVE_MBEDTLS=true
else
    print_warning "mbedTLS not found (optional, but recommended)"
    echo "           Install with: sudo apt-get install libmbedtls-dev"
fi

# Check for OpenSSL
if pkg-config --exists openssl 2>/dev/null; then
    OPENSSL_VERSION=$(pkg-config --modversion openssl)
    print_success "OpenSSL found: $OPENSSL_VERSION"
    HAVE_OPENSSL=true
else
    print_warning "OpenSSL not found (optional)"
    echo "           Install with: sudo apt-get install libssl-dev"
fi

# Check for wolfSSL
if pkg-config --exists wolfssl 2>/dev/null; then
    WOLFSSL_VERSION=$(pkg-config --modversion wolfssl)
    print_success "wolfSSL found: $WOLFSSL_VERSION"
    HAVE_WOLFSSL=true
else
    print_warning "wolfSSL not found (optional)"
    echo "           Install with: sudo apt-get install libwolfssl-dev"
fi

# TinyDTLS is always available (bundled)
print_success "TinyDTLS 0.8.6 (bundled)"

echo ""

# Build configuration
print_header "Build Configuration"

BUILD_DIR="build"
CMAKE_OPTIONS=""

# Ask user for build configuration
echo ""
echo "Select DTLS providers to build:"
echo ""

if [ "$HAVE_MBEDTLS" = true ]; then
    read -p "Include mbedTLS provider? (Y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Nn]$ ]]; then
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_PROVIDER_MBEDTLS=ON"
        echo "  ✓ mbedTLS will be included"
    else
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_PROVIDER_MBEDTLS=OFF"
    fi
fi

read -p "Include TinyDTLS provider? (Y/n): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Nn]$ ]]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_PROVIDER_TINYDTLS=ON"
    echo "  ✓ TinyDTLS will be included"
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_PROVIDER_TINYDTLS=OFF"
fi

if [ "$HAVE_OPENSSL" = true ]; then
    read -p "Include OpenSSL provider? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_PROVIDER_OPENSSL=ON"
        echo "  ✓ OpenSSL will be included"
    else
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_PROVIDER_OPENSSL=OFF"
    fi
fi

if [ "$HAVE_WOLFSSL" = true ]; then
    read -p "Include wolfSSL provider? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_PROVIDER_WOLFSSL=ON"
        echo "  ✓ wolfSSL will be included"
    else
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_PROVIDER_WOLFSSL=OFF"
    fi
fi

echo ""
read -p "Default DTLS provider (mbedtls/tinydtls/openssl/wolfssl): " DEFAULT_PROVIDER
DEFAULT_PROVIDER=${DEFAULT_PROVIDER:-mbedtls}
CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_DEFAULT_PROVIDER=$DEFAULT_PROVIDER"

echo ""
read -p "Enable static linking? (Y/n): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Nn]$ ]]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_STATIC_LINK=ON"
    echo "  ✓ Static linking enabled"
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DDTLS_STATIC_LINK=OFF"
    echo "  ✓ Dynamic linking enabled"
fi

echo ""
read -p "Enable tests? (Y/n): " -n 1 -r
echo
BUILD_TESTS=true
if [[ $REPLY =~ ^[Nn]$ ]]; then
    BUILD_TESTS=false
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_TESTING=OFF"
fi

echo ""
read -p "Enable code coverage? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DENABLE_COVERAGE=ON"
    echo "  ✓ Code coverage enabled"
fi

# Clean previous build
echo ""
read -p "Clean previous build? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    print_header "Cleaning Previous Build"
    rm -rf "$BUILD_DIR"
    print_success "Build directory cleaned"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo ""
print_header "Configuring Build"
echo "CMake options: $CMAKE_OPTIONS"
echo ""

cmake $CMAKE_OPTIONS .. || {
    print_error "CMake configuration failed"
    exit 1
}

print_success "Configuration complete"

# Build
echo ""
print_header "Building Friendly LwM2M Client v1.2.2"

make -j$(nproc) || {
    print_error "Build failed"
    exit 1
}

print_success "Build complete"

# Display built artifacts
echo ""
print_header "Built Artifacts"

if [ -f "friendly_example_client" ]; then
    SIZE=$(du -h friendly_example_client | cut -f1)
    print_success "friendly_example_client (${SIZE})"
fi

if [ -f "libfriendly_lwm2m.a" ]; then
    SIZE=$(du -h libfriendly_lwm2m.a | cut -f1)
    print_success "libfriendly_lwm2m.a (${SIZE})"
fi

# List available providers
echo ""
print_header "Testing DTLS Provider Detection"

if [ -f "friendly_example_client" ]; then
    ./friendly_example_client --list-dtls-providers || true
fi

# Run tests
if [ "$BUILD_TESTS" = true ]; then
    echo ""
    read -p "Run tests? (Y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Nn]$ ]]; then
        print_header "Running Tests"

        # Run CTest
        ctest --output-on-failure || {
            print_warning "Some tests failed"
        }

        echo ""
        print_header "Test Summary"

        # Show test results
        if [ -f "Testing/Temporary/LastTest.log" ]; then
            echo "Test log available at: build/Testing/Temporary/LastTest.log"
        fi

        # Run specific DTLS tests
        if [ -f "test_dtls_providers" ]; then
            echo ""
            read -p "Run detailed DTLS provider tests? (y/N): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                print_header "DTLS Provider Unit Tests"
                ./test_dtls_providers
            fi
        fi

        if [ -f "test_dtls_integration" ]; then
            echo ""
            read -p "Run DTLS integration tests? (y/N): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                print_header "DTLS Provider Integration Tests"
                ./test_dtls_integration
            fi
        fi

        if [ -f "benchmark_dtls_providers" ]; then
            echo ""
            read -p "Run performance benchmarks? (y/N): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                print_header "DTLS Provider Performance Benchmarks"
                ./benchmark_dtls_providers
            fi
        fi
    fi
fi

# Generate coverage report
if [[ $CMAKE_OPTIONS == *"ENABLE_COVERAGE=ON"* ]]; then
    echo ""
    read -p "Generate coverage report? (Y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Nn]$ ]]; then
        print_header "Generating Coverage Report"
        make coverage || {
            print_warning "Coverage report generation failed"
        }

        if [ -d "coverage" ]; then
            print_success "Coverage report generated in build/coverage/"
            echo "           Open build/coverage/index.html in your browser"
        fi
    fi
fi

# Success summary
echo ""
print_header "Build Complete! 🎉"
echo ""
echo "Friendly LwM2M Client v1.2.2 with Multi-DTLS Provider Support"
echo ""
echo "Next steps:"
echo "  1. Run the example client:"
echo "     ./build/friendly_example_client -h coaps://server:5684"
echo ""
echo "  2. List available providers:"
echo "     ./build/friendly_example_client --list-dtls-providers"
echo ""
echo "  3. Select a specific provider:"
echo "     ./build/friendly_example_client --dtls-provider=mbedtls -h coaps://server:5684"
echo ""
echo "  4. Read the documentation:"
echo "     - DTLS Provider Selection Guide: DTLS_PROVIDER_SELECTION_GUIDE.md"
echo "     - Migration Guide: MIGRATION_GUIDE_MULTI_DTLS.md"
echo "     - Release Notes: RELEASE_NOTES_v1.2.2.md"
echo ""
print_success "Build and test process complete!"
echo ""
