#!/bin/bash
#
# Full Development Build with All Features
# RelWithDebInfo build type with coverage and sanitizers
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

echo "=================================================="
echo "Full Development Build Configuration"
echo "=================================================="
echo ""
echo "Build Type: RelWithDebInfo (optimized + debug symbols)"
echo "Coverage:   ON"
echo "Sanitizers: ON (Address + Undefined Behavior)"
echo "Tests:      ON"
echo ""
echo "Features: ALL ENABLED"
echo "  - DTLS (mbedTLS only)"
echo "  - Delta FOTA (BSDIFF, VCDIFF, Courgette)"
echo "  - MQTT Transport"
echo "  - Edge AI (TFLite, ONNX)"
echo "  - OpenWRT Extended"
echo "  - Router Management (LAN, Routing, VPN)"
echo "  - Multi-WAN & Failover"
echo "  - WiFi Management"
echo "  - Starlink Integration (gRPC)"
echo "  - prpl Integration (EasyMesh, Ambiorix)"
echo "  - Matter/Thread"
echo "  - Zigbee (EZSP, Z-Stack)"
echo ""

# Check Wakaama submodule
if [ ! -f "${PROJECT_ROOT}/2305-Wakaama/CMakeLists.txt" ]; then
    echo "⚠️  WARNING: Wakaama submodule not initialized"
    echo "    Router management objects require Wakaama"
    echo ""
    echo "    To initialize: git submodule update --init --recursive"
    echo ""
    read -p "Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

echo "Step 1: Configuring CMake..."
echo ""

# NOTE: Using DEV build environment which enables Edge AI and Starlink
# BuildEnvironment.cmake is included first and sets baseline defaults
# Command-line options below can override if needed

cmake .. \
  -DBUILD_ENV=DEV \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  \
  -DWITH_DTLS=ON \
  -DWITH_MBEDTLS=ON \
  -DWITH_TINYDTLS=OFF \
  \
  -DDTLS_PROVIDER_TINYDTLS=OFF \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_OPENSSL=OFF \
  -DDTLS_PROVIDER_WOLFSSL=OFF \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  -DDTLS_STATIC_LINK=ON \
  \
  -DWITH_DELTA_FOTA=ON \
  -DWITH_BSDIFF=ON \
  -DWITH_VCDIFF=ON \
  -DWITH_COURGETTE=ON \
  -DWITH_COMPRESSION=ON \
  -DWITH_ROLLBACK=ON \
  \
  -DWITH_MQTT=ON \
  -DWITH_EDGE_AI=ON \
  -DWITH_TFLITE=ON \
  -DWITH_ONNX=ON \
  \
  -DWITH_OPENWRT_EXTENDED=ON \
  -DWITH_LUCI_MODULE=ON \
  -DWITH_HOTPLUG_HANDLERS=ON \
  \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON \
  \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  \
  -DWITH_STARLINK=ON \
  -DWITH_STARLINK_GRPC=ON \
  \
  -DWITH_PRPL=ON \
  -DWITH_PRPL_EASYMESH=ON \
  -DWITH_PRPL_AMBIORIX=ON \
  \
  -DWITH_MATTER=ON \
  -DWITH_MATTER_BRIDGE=ON \
  -DWITH_THREAD_BR=ON \
  \
  -DWITH_ZIGBEE=ON \
  -DWITH_ZIGBEE_EZSP=ON \
  -DWITH_ZIGBEE_ZSTACK=ON \
  \
  -DBUILD_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  -DBUILD_DOCS=ON \
  \
  -DENABLE_COVERAGE=ON \
  -DENABLE_SANITIZERS=ON

if [ $? -ne 0 ]; then
    echo ""
    echo "❌ CMake configuration failed"
    echo ""
    echo "Common issues:"
    echo "  - Missing Wakaama submodule: git submodule update --init --recursive"
    echo "  - Missing dependencies: see docs/CMAKE_BUILD_OPTIONS.md"
    echo ""
    exit 1
fi

echo ""
echo "Step 2: Building..."
echo ""

make -j$(nproc)

if [ $? -ne 0 ]; then
    echo ""
    echo "❌ Build failed"
    exit 1
fi

echo ""
echo "=================================================="
echo "✅ Build completed successfully"
echo "=================================================="
echo ""
echo "Build directory: ${BUILD_DIR}"
echo "Executables:"
ls -lh friendly_lwm2m test_* 2>/dev/null | grep -v ".o$" || echo "  (none built)"
echo ""
echo "Next steps:"
echo "  - Run tests:     cd build && ctest --output-on-failure"
echo "  - Run coverage:  cd build && make test && gcovr -r .. --html -o coverage.html"
echo "  - Run client:    cd build && ./friendly_lwm2m"
echo ""
echo "Notes:"
echo "  - Sanitizers enabled: Binary will be slower (4-8x)"
echo "  - Coverage enabled: Run tests to generate coverage data"
echo "  - RelWithDebInfo: Optimized (-O2) with debug symbols (-g)"
echo ""
