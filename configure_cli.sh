#!/bin/bash
#
# Direct CMake CLI Configuration
# Pass options directly as command-line arguments
#
# Usage:
#   ./configure_cli.sh [options]
#
# Examples:
#   ./configure_cli.sh --preset=full
#   ./configure_cli.sh --dtls --mqtt --wan-failover --lan-config
#   ./configure_cli.sh --all-objects
#   ./configure_cli.sh --help
#

set -e

BUILD_DIR="build"
BUILD_TYPE="Release"
CMAKE_OPTS=()

# Display help
show_help() {
    cat << EOF
Usage: ./configure_cli.sh [OPTIONS]

Preset Configurations:
  --preset=minimal        Minimal LwM2M client only
  --preset=standard       Core features (default if no options given)
  --preset=openwrt        OpenWRT router configuration
  --preset=full           All features enabled
  --preset=dev            Development build (debug + coverage)

Core Features:
  --dtls                  Enable DTLS support
  --no-dtls               Disable DTLS
  --mbedtls               Use mbedTLS (with --dtls)
  --tinydtls              Use TinyDTLS (with --dtls)
  --mqtt                  Enable MQTT transport
  --no-mqtt               Disable MQTT
  --delta-fota            Enable delta firmware updates
  --edge-ai               Enable Edge AI inference
  --compression           Enable compression support
  --rollback              Enable A/B partition rollback

Starlink:
  --starlink              Enable Starlink integration
  --starlink-grpc         Enable Starlink gRPC support

OpenWRT:
  --openwrt               Enable OpenWRT extended integration
  --luci                  Build LuCI web interface
  --hotplug               Install hotplug handlers

Router Management Objects (require Wakaama):
  --lan-config            Enable LAN Configuration (10535)
  --routing-table         Enable Routing Table (10536)
  --vpn-config            Enable VPN Configuration (10537)

Multi-WAN Objects (require Wakaama):
  --wan-failover          Enable WAN Failover Policy (10525)
  --multiwan-health       Enable Multi-WAN Health Check (10526)

WiFi Objects (require Wakaama):
  --wifi-client-mgmt      Enable WiFi Client Management (10527)
  --wifi-channel-opt      Enable WiFi Channel Optimization (10528)

Matter Integration:
  --matter                Enable Matter/Thread integration
  --matter-bridge         Enable Matter Bridge object (33450)
  --thread-br             Enable Thread Border Router

Zigbee Integration:
  --zigbee                Enable Zigbee integration
  --zigbee-ezsp           Enable Silicon Labs EZSP support
  --zigbee-zstack         Enable TI Z-Stack support

Convenience Options:
  --all-wan-objects       Enable all WAN/MultiWAN objects
  --all-wifi-objects      Enable all WiFi objects
  --all-router-objects    Enable all router management objects
  --all-custom-objects    Enable all Walt Technologies custom objects
  --all-objects           Enable ALL custom objects (requires Wakaama)

Build Options:
  --debug                 Debug build (default: Release)
  --release               Release build
  --tests                 Build unit tests
  --no-tests              Don't build tests
  --coverage              Enable code coverage
  --sanitizers            Enable address/undefined sanitizers
  --no-sanitizers         Disable sanitizers

Other:
  --build-dir=DIR         Set build directory (default: build)
  --clean                 Clean build directory before configuring
  --help                  Show this help message

Examples:
  # Standard build with custom objects
  ./configure_cli.sh --dtls --mqtt --wan-failover --lan-config

  # Full OpenWRT build
  ./configure_cli.sh --preset=openwrt

  # Development build with all objects
  ./configure_cli.sh --debug --all-objects --coverage --tests

  # Minimal build
  ./configure_cli.sh --preset=minimal --no-tests

EOF
}

# Parse command line arguments
CLEAN_BUILD=false
ALL_WAN=false
ALL_WIFI=false
ALL_ROUTER=false
ALL_CUSTOM=false
PRESET=""

# Default to standard preset if no args
if [ $# -eq 0 ]; then
    PRESET="standard"
fi

while [[ $# -gt 0 ]]; do
    case $1 in
        --help|-h)
            show_help
            exit 0
            ;;
        --preset=*)
            PRESET="${1#*=}"
            ;;
        --build-dir=*)
            BUILD_DIR="${1#*=}"
            ;;
        --clean)
            CLEAN_BUILD=true
            ;;
        --debug)
            BUILD_TYPE="Debug"
            ;;
        --release)
            BUILD_TYPE="Release"
            ;;
        --dtls)
            CMAKE_OPTS+=("-DWITH_DTLS=ON")
            ;;
        --no-dtls)
            CMAKE_OPTS+=("-DWITH_DTLS=OFF")
            ;;
        --mbedtls)
            CMAKE_OPTS+=("-DWITH_MBEDTLS=ON")
            ;;
        --tinydtls)
            CMAKE_OPTS+=("-DWITH_TINYDTLS=ON")
            ;;
        --mqtt)
            CMAKE_OPTS+=("-DWITH_MQTT=ON")
            ;;
        --no-mqtt)
            CMAKE_OPTS+=("-DWITH_MQTT=OFF")
            ;;
        --delta-fota)
            CMAKE_OPTS+=("-DWITH_DELTA_FOTA=ON" "-DWITH_BSDIFF=ON")
            ;;
        --edge-ai)
            CMAKE_OPTS+=("-DWITH_EDGE_AI=ON")
            ;;
        --compression)
            CMAKE_OPTS+=("-DWITH_COMPRESSION=ON")
            ;;
        --rollback)
            CMAKE_OPTS+=("-DWITH_ROLLBACK=ON")
            ;;
        --starlink)
            CMAKE_OPTS+=("-DWITH_STARLINK=ON")
            ;;
        --starlink-grpc)
            CMAKE_OPTS+=("-DWITH_STARLINK_GRPC=ON")
            ;;
        --openwrt)
            CMAKE_OPTS+=("-DWITH_OPENWRT_EXTENDED=ON")
            ;;
        --luci)
            CMAKE_OPTS+=("-DWITH_LUCI_MODULE=ON")
            ;;
        --hotplug)
            CMAKE_OPTS+=("-DWITH_HOTPLUG_HANDLERS=ON")
            ;;
        --lan-config)
            CMAKE_OPTS+=("-DWITH_LAN_CONFIG=ON")
            ;;
        --routing-table)
            CMAKE_OPTS+=("-DWITH_ROUTING_TABLE=ON")
            ;;
        --vpn-config)
            CMAKE_OPTS+=("-DWITH_VPN_CONFIG=ON")
            ;;
        --wan-failover)
            CMAKE_OPTS+=("-DWITH_WAN_FAILOVER=ON")
            ;;
        --multiwan-health)
            CMAKE_OPTS+=("-DWITH_MULTIWAN_HEALTH=ON")
            ;;
        --wifi-client-mgmt)
            CMAKE_OPTS+=("-DWITH_WIFI_CLIENT_MGMT=ON")
            ;;
        --wifi-channel-opt)
            CMAKE_OPTS+=("-DWITH_WIFI_CHANNEL_OPT=ON")
            ;;
        --matter)
            CMAKE_OPTS+=("-DWITH_MATTER=ON")
            ;;
        --matter-bridge)
            CMAKE_OPTS+=("-DWITH_MATTER_BRIDGE=ON")
            ;;
        --thread-br)
            CMAKE_OPTS+=("-DWITH_THREAD_BR=ON")
            ;;
        --zigbee)
            CMAKE_OPTS+=("-DWITH_ZIGBEE=ON")
            ;;
        --zigbee-ezsp)
            CMAKE_OPTS+=("-DWITH_ZIGBEE_EZSP=ON")
            ;;
        --zigbee-zstack)
            CMAKE_OPTS+=("-DWITH_ZIGBEE_ZSTACK=ON")
            ;;
        --all-wan-objects)
            ALL_WAN=true
            ;;
        --all-wifi-objects)
            ALL_WIFI=true
            ;;
        --all-router-objects)
            ALL_ROUTER=true
            ;;
        --all-custom-objects)
            ALL_CUSTOM=true
            ;;
        --all-objects)
            ALL_CUSTOM=true
            ALL_WAN=true
            ALL_WIFI=true
            ALL_ROUTER=true
            ;;
        --tests)
            CMAKE_OPTS+=("-DBUILD_TESTS=ON")
            ;;
        --no-tests)
            CMAKE_OPTS+=("-DBUILD_TESTS=OFF")
            ;;
        --coverage)
            CMAKE_OPTS+=("-DENABLE_COVERAGE=ON")
            ;;
        --sanitizers)
            CMAKE_OPTS+=("-DENABLE_SANITIZERS=ON")
            ;;
        --no-sanitizers)
            CMAKE_OPTS+=("-DENABLE_SANITIZERS=OFF")
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
    shift
done

# Handle convenience flags
if [ "$ALL_WAN" = true ]; then
    CMAKE_OPTS+=("-DWITH_WAN_FAILOVER=ON" "-DWITH_MULTIWAN_HEALTH=ON")
fi

if [ "$ALL_WIFI" = true ]; then
    CMAKE_OPTS+=("-DWITH_WIFI_CLIENT_MGMT=ON" "-DWITH_WIFI_CHANNEL_OPT=ON")
fi

if [ "$ALL_ROUTER" = true ]; then
    CMAKE_OPTS+=("-DWITH_LAN_CONFIG=ON" "-DWITH_ROUTING_TABLE=ON" "-DWITH_VPN_CONFIG=ON")
fi

# Apply preset if specified
if [ -n "$PRESET" ]; then
    case "$PRESET" in
        minimal)
            CMAKE_OPTS=(
                "-DCMAKE_BUILD_TYPE=Release"
                "-DWITH_DTLS=OFF"
                "-DWITH_MQTT=OFF"
                "-DWITH_DELTA_FOTA=OFF"
                "-DWITH_EDGE_AI=OFF"
                "-DWITH_STARLINK=OFF"
                "-DBUILD_TESTS=OFF"
            )
            ;;
        standard)
            CMAKE_OPTS=(
                "-DCMAKE_BUILD_TYPE=Release"
                "-DWITH_DTLS=ON"
                "-DWITH_MBEDTLS=ON"
                "-DWITH_MQTT=ON"
                "-DWITH_DELTA_FOTA=ON"
                "-DWITH_BSDIFF=ON"
                "-DWITH_ROLLBACK=ON"
                "-DWITH_STARLINK=ON"
                "-DBUILD_TESTS=ON"
            )
            ;;
        openwrt)
            CMAKE_OPTS=(
                "-DCMAKE_BUILD_TYPE=Release"
                "-DWITH_DTLS=ON"
                "-DWITH_MBEDTLS=ON"
                "-DWITH_MQTT=ON"
                "-DWITH_DELTA_FOTA=ON"
                "-DWITH_ROLLBACK=ON"
                "-DWITH_STARLINK=ON"
                "-DWITH_OPENWRT_EXTENDED=ON"
                "-DWITH_LUCI_MODULE=ON"
                "-DWITH_WAN_FAILOVER=ON"
                "-DWITH_MULTIWAN_HEALTH=ON"
                "-DWITH_WIFI_CLIENT_MGMT=ON"
                "-DWITH_WIFI_CHANNEL_OPT=ON"
                "-DWITH_LAN_CONFIG=ON"
                "-DWITH_ROUTING_TABLE=ON"
                "-DWITH_VPN_CONFIG=ON"
                "-DBUILD_TESTS=ON"
            )
            ;;
        full)
            CMAKE_OPTS=(
                "-DCMAKE_BUILD_TYPE=Release"
                "-DWITH_DTLS=ON"
                "-DWITH_MBEDTLS=ON"
                "-DWITH_MQTT=ON"
                "-DWITH_DELTA_FOTA=ON"
                "-DWITH_BSDIFF=ON"
                "-DWITH_VCDIFF=ON"
                "-DWITH_COURGETTE=ON"
                "-DWITH_ROLLBACK=ON"
                "-DWITH_EDGE_AI=ON"
                "-DWITH_STARLINK=ON"
                "-DWITH_OPENWRT_EXTENDED=ON"
                "-DWITH_WAN_FAILOVER=ON"
                "-DWITH_MULTIWAN_HEALTH=ON"
                "-DWITH_WIFI_CLIENT_MGMT=ON"
                "-DWITH_WIFI_CHANNEL_OPT=ON"
                "-DWITH_LAN_CONFIG=ON"
                "-DWITH_ROUTING_TABLE=ON"
                "-DWITH_VPN_CONFIG=ON"
                "-DBUILD_TESTS=ON"
            )
            ;;
        dev)
            BUILD_TYPE="Debug"
            CMAKE_OPTS=(
                "-DCMAKE_BUILD_TYPE=Debug"
                "-DWITH_DTLS=ON"
                "-DWITH_MBEDTLS=ON"
                "-DWITH_MQTT=ON"
                "-DWITH_DELTA_FOTA=ON"
                "-DWITH_ROLLBACK=ON"
                "-DWITH_STARLINK=ON"
                "-DWITH_OPENWRT_EXTENDED=ON"
                "-DWITH_WAN_FAILOVER=ON"
                "-DWITH_MULTIWAN_HEALTH=ON"
                "-DWITH_WIFI_CLIENT_MGMT=ON"
                "-DWITH_WIFI_CHANNEL_OPT=ON"
                "-DWITH_LAN_CONFIG=ON"
                "-DWITH_ROUTING_TABLE=ON"
                "-DWITH_VPN_CONFIG=ON"
                "-DBUILD_TESTS=ON"
                "-DENABLE_COVERAGE=ON"
                "-DENABLE_SANITIZERS=ON"
            )
            ;;
        *)
            echo "Unknown preset: $PRESET"
            exit 1
            ;;
    esac
fi

# Add build type if not already set by preset
if ! [[ "${CMAKE_OPTS[@]}" =~ "CMAKE_BUILD_TYPE" ]]; then
    CMAKE_OPTS=("-DCMAKE_BUILD_TYPE=$BUILD_TYPE" "${CMAKE_OPTS[@]}")
fi

# Create and clean build directory if requested
mkdir -p "$BUILD_DIR"

if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"/*
fi

# Configure
echo "Configuring build in: $BUILD_DIR"
echo "Build type: $BUILD_TYPE"
echo
echo "CMake options:"
printf '  %s\n' "${CMAKE_OPTS[@]}"
echo

cd "$BUILD_DIR"
cmake .. "${CMAKE_OPTS[@]}"

echo
echo "✓ Configuration complete!"
echo
echo "To build, run:"
echo "  cd $BUILD_DIR && make -j\$(nproc)"
echo
