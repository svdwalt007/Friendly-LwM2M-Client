#!/bin/bash
# Build script without advanced WPP objects (10525-10537)
# These objects have code errors and need fixing before they can be enabled

set -e

BUILD_DIR="build"
SOURCE_DIR="."

echo "========================================="
echo "Building Friendly LwM2M Client"
echo "Without Advanced Objects (10525-10537)"
echo "========================================="
echo ""

# Change to source directory if we're already in build/
if [ "$(basename "$PWD")" = "build" ]; then
    cd ..
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

echo "Removing old CMake cache..."
rm -f CMakeCache.txt

echo ""
echo "Configuring with CMake..."
echo ""

# Configure with advanced objects disabled
cmake \
    -DBUILD_ENV=DEV \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DDTLS_DEFAULT_PROVIDER=mbedtls \
    -DWITH_DTLS=ON \
    -DWITH_DELTA_FOTA=ON \
    -DWITH_BSDIFF=ON \
    -DWITH_VCDIFF=ON \
    -DWITH_COURGETTE=ON \
    -DWITH_COMPRESSION=ON \
    -DWITH_ROLLBACK=ON \
    -DWITH_MQTT=ON \
    -DWITH_OPENWRT_EXTENDED=ON \
    -DWITH_LUCI_MODULE=ON \
    -DWITH_HOTPLUG_HANDLERS=ON \
    -DWITH_WAN_FAILOVER=OFF \
    -DWITH_MULTIWAN_HEALTH=OFF \
    -DWITH_WIFI_CLIENT_MGMT=OFF \
    -DWITH_WIFI_CHANNEL_OPT=OFF \
    -DWITH_MATTER_BRIDGE=OFF \
    -DWITH_LAN_CONFIG=OFF \
    -DWITH_ROUTING_TABLE=OFF \
    -DWITH_VPN_CONFIG=OFF \
    ..

if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: CMake configuration failed!"
    exit 1
fi

echo ""
echo "========================================="
echo "Configuration Summary:"
echo "========================================="
echo "Build Environment: DEV"
echo "DTLS Provider: mbedTLS"
echo "Tests: Enabled"
echo "Examples: Enabled"
echo ""
echo "Objects Built:"
echo "  ✅ Mandatory objects (0, 1, 3)"
echo "  ✅ Optional OMA objects (2, 4, 5, 6)"
echo "  ✅ IPSO objects (3331, 3339)"
echo "  ✅ Platform objects (10512-10522)"
echo "  ❌ Advanced objects (10525-10537) - DISABLED"
echo ""
echo "Total: ~23 objects"
echo "========================================="
echo ""

# Detect number of CPU cores
if command -v nproc &> /dev/null; then
    JOBS=$(nproc)
elif [ -f /proc/cpuinfo ]; then
    JOBS=$(grep -c ^processor /proc/cpuinfo)
else
    JOBS=4
fi

echo "Building with $JOBS parallel jobs..."
echo ""

# Build
make -j$JOBS

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================="
    echo "✅ Build Successful!"
    echo "========================================="
    echo ""
    echo "Binary location: build/examples/friendly_example_client"
    echo "Library location: build/libfriendly_lwm2m.a"
    echo ""
    echo "To run tests:"
    echo "  cd build && ctest"
    echo ""
    echo "To run example client:"
    echo "  ./build/examples/friendly_example_client"
    echo ""
else
    echo ""
    echo "========================================="
    echo "❌ Build Failed!"
    echo "========================================="
    echo ""
    echo "Check build errors above"
    exit 1
fi
