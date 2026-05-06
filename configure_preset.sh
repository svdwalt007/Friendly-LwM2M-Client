#!/bin/bash
#
# Quick CMake Configuration with Presets
# For Friendly LwM2M Client
#

set -e

BUILD_DIR="build"

# Color output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  LwM2M Client - Quick Configuration${NC}"
echo -e "${BLUE}========================================${NC}"
echo
echo "Select a preset configuration:"
echo
echo "  1) Minimal      - Basic LwM2M client only"
echo "  2) Standard     - Core features + DTLS + MQTT"
echo "  3) OpenWRT      - OpenWRT router features"
echo "  4) Full         - All features enabled"
echo "  5) Development  - Full features + tests + coverage"
echo "  6) Custom       - Manual selection (runs interactive configure.sh)"
echo
read -p "Choice [2]: " PRESET
PRESET=${PRESET:-2}

mkdir -p "$BUILD_DIR"

case "$PRESET" in
    1)
        echo -e "${GREEN}Configuring: Minimal Build${NC}"
        cd "$BUILD_DIR"
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DWITH_DTLS=OFF \
            -DWITH_MQTT=OFF \
            -DWITH_DELTA_FOTA=OFF \
            -DWITH_EDGE_AI=OFF \
            -DWITH_STARLINK=OFF \
            -DWITH_OPENWRT_EXTENDED=OFF \
            -DBUILD_TESTS=OFF \
            -DENABLE_COVERAGE=OFF \
            -DENABLE_SANITIZERS=OFF
        ;;
    2)
        echo -e "${GREEN}Configuring: Standard Build${NC}"
        cd "$BUILD_DIR"
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DWITH_DTLS=ON \
            -DWITH_MBEDTLS=ON \
            -DWITH_MQTT=ON \
            -DWITH_DELTA_FOTA=ON \
            -DWITH_BSDIFF=ON \
            -DWITH_ROLLBACK=ON \
            -DWITH_EDGE_AI=OFF \
            -DWITH_STARLINK=ON \
            -DWITH_STARLINK_GRPC=ON \
            -DWITH_OPENWRT_EXTENDED=OFF \
            -DBUILD_TESTS=ON \
            -DENABLE_COVERAGE=OFF \
            -DENABLE_SANITIZERS=OFF
        ;;
    3)
        echo -e "${GREEN}Configuring: OpenWRT Build${NC}"
        cd "$BUILD_DIR"
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DWITH_DTLS=ON \
            -DWITH_MBEDTLS=ON \
            -DWITH_MQTT=ON \
            -DWITH_DELTA_FOTA=ON \
            -DWITH_BSDIFF=ON \
            -DWITH_ROLLBACK=ON \
            -DWITH_STARLINK=ON \
            -DWITH_STARLINK_GRPC=ON \
            -DWITH_OPENWRT_EXTENDED=ON \
            -DWITH_LUCI_MODULE=ON \
            -DWITH_HOTPLUG_HANDLERS=ON \
            -DWITH_WAN_FAILOVER=ON \
            -DWITH_MULTIWAN_HEALTH=ON \
            -DWITH_WIFI_CLIENT_MGMT=ON \
            -DWITH_WIFI_CHANNEL_OPT=ON \
            -DWITH_LAN_CONFIG=ON \
            -DWITH_ROUTING_TABLE=ON \
            -DWITH_VPN_CONFIG=ON \
            -DBUILD_TESTS=ON \
            -DENABLE_COVERAGE=OFF \
            -DENABLE_SANITIZERS=OFF
        ;;
    4)
        echo -e "${GREEN}Configuring: Full Build (All Features)${NC}"
        cd "$BUILD_DIR"
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DWITH_DTLS=ON \
            -DWITH_MBEDTLS=ON \
            -DDTLS_PROVIDER_TINYDTLS=ON \
            -DDTLS_PROVIDER_MBEDTLS=ON \
            -DWITH_MQTT=ON \
            -DWITH_DELTA_FOTA=ON \
            -DWITH_BSDIFF=ON \
            -DWITH_VCDIFF=ON \
            -DWITH_COURGETTE=ON \
            -DWITH_COMPRESSION=ON \
            -DWITH_ROLLBACK=ON \
            -DWITH_EDGE_AI=ON \
            -DWITH_STARLINK=ON \
            -DWITH_STARLINK_GRPC=ON \
            -DWITH_OPENWRT_EXTENDED=ON \
            -DWITH_LUCI_MODULE=ON \
            -DWITH_HOTPLUG_HANDLERS=ON \
            -DWITH_WAN_FAILOVER=ON \
            -DWITH_MULTIWAN_HEALTH=ON \
            -DWITH_WIFI_CLIENT_MGMT=ON \
            -DWITH_WIFI_CHANNEL_OPT=ON \
            -DWITH_LAN_CONFIG=ON \
            -DWITH_ROUTING_TABLE=ON \
            -DWITH_VPN_CONFIG=ON \
            -DWITH_MATTER=OFF \
            -DWITH_MATTER_BRIDGE=OFF \
            -DWITH_ZIGBEE=OFF \
            -DBUILD_TESTS=ON \
            -DBUILD_EXAMPLES=ON \
            -DENABLE_COVERAGE=OFF \
            -DENABLE_SANITIZERS=OFF
        ;;
    5)
        echo -e "${GREEN}Configuring: Development Build${NC}"
        cd "$BUILD_DIR"
        cmake .. \
            -DCMAKE_BUILD_TYPE=Debug \
            -DWITH_DTLS=ON \
            -DWITH_MBEDTLS=ON \
            -DDTLS_PROVIDER_TINYDTLS=ON \
            -DDTLS_PROVIDER_MBEDTLS=ON \
            -DWITH_MQTT=ON \
            -DWITH_DELTA_FOTA=ON \
            -DWITH_BSDIFF=ON \
            -DWITH_VCDIFF=ON \
            -DWITH_COURGETTE=ON \
            -DWITH_COMPRESSION=ON \
            -DWITH_ROLLBACK=ON \
            -DWITH_EDGE_AI=ON \
            -DWITH_STARLINK=ON \
            -DWITH_STARLINK_GRPC=ON \
            -DWITH_OPENWRT_EXTENDED=ON \
            -DWITH_LUCI_MODULE=ON \
            -DWITH_HOTPLUG_HANDLERS=ON \
            -DWITH_WAN_FAILOVER=ON \
            -DWITH_MULTIWAN_HEALTH=ON \
            -DWITH_WIFI_CLIENT_MGMT=ON \
            -DWITH_WIFI_CHANNEL_OPT=ON \
            -DWITH_LAN_CONFIG=ON \
            -DWITH_ROUTING_TABLE=ON \
            -DWITH_VPN_CONFIG=ON \
            -DWITH_MATTER=OFF \
            -DWITH_MATTER_BRIDGE=OFF \
            -DWITH_ZIGBEE=OFF \
            -DBUILD_TESTS=ON \
            -DBUILD_EXAMPLES=ON \
            -DENABLE_COVERAGE=ON \
            -DENABLE_SANITIZERS=ON
        ;;
    6)
        echo -e "${YELLOW}Running interactive configuration...${NC}"
        exec ./configure.sh
        ;;
    *)
        echo "Invalid choice. Defaulting to Standard build."
        exec "$0" <<< "2"
        ;;
esac

echo
echo -e "${GREEN}✓ Configuration complete!${NC}"
echo
echo "To build, run:"
echo -e "  ${BLUE}cd $BUILD_DIR && make -j\$(nproc)${NC}"
echo
echo "Build targets:"
echo "  make friendly_example_client  - Main client application"
echo "  make friendly_lwm2m           - Library only"
echo "  make test                      - Run all tests"
echo "  make all                       - Build everything"
echo
