#!/bin/bash
# Friendly LwM2M Client - Full Feature Build Configuration
# This script configures CMake with all available features enabled

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=================================${NC}"
echo -e "${GREEN}Friendly LwM2M Client${NC}"
echo -e "${GREEN}Full Feature Build Configuration${NC}"
echo -e "${GREEN}=================================${NC}"
echo ""

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo -e "${YELLOW}Creating build directory...${NC}"
    mkdir build
fi

cd build

echo -e "${YELLOW}Configuring CMake with all features enabled...${NC}"
echo ""

# Run CMake with all features enabled
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    \
    `# Core DTLS Features` \
    -DDTLS=ON \
    -DMBEDTLS=ON \
    -DTINYDTLS=OFF \
    \
    `# DTLS Multi-Provider v1.2.2` \
    -DDTLS_TINYDTLS_PROVIDER=ON \
    -DDTLS_MBEDTLS_PROVIDER=ON \
    -DDTLS_OPENSSL_PROVIDER=OFF \
    -DDTLS_WOLFSSL_PROVIDER=OFF \
    -DDTLS_DEFAULT_PROVIDER=mbedtls \
    -DDTLS_STATIC_LINKING=ON \
    \
    `# Delta FOTA Features` \
    -DWITH_DELTA_FOTA=ON \
    -DWITH_BSDIFF=ON \
    -DWITH_VCDIFF=ON \
    -DWITH_COURGETTE=ON \
    -DWITH_COMPRESSION=ON \
    -DWITH_ROLLBACK=ON \
    \
    `# Transport and AI Features` \
    -DWITH_MQTT_TRANSPORT=ON \
    -DWITH_EDGE_AI=ON \
    -DWITH_TENSORFLOW_LITE=ON \
    -DWITH_ONNX_RUNTIME=ON \
    \
    `# v1.2.0 Extended Features - OpenWRT` \
    -DWITH_OPENWRT_EXTENDED=ON \
    -DWITH_LUCI_MODULE=ON \
    -DWITH_HOTPLUG_HANDLERS=ON \
    -DWITH_LAN_CONFIG=ON \
    -DWITH_ROUTING_TABLE=ON \
    -DWITH_VPN_CONFIG=ON \
    -DWITH_WAN_FAILOVER=ON \
    -DWITH_MULTIWAN_HEALTH=ON \
    -DWITH_WIFI_CLIENT_MGMT=ON \
    -DWITH_WIFI_CHANNEL_OPT=ON \
    \
    `# Starlink Features` \
    -DWITH_STARLINK=ON \
    -DWITH_STARLINK_GRPC=ON \
    \
    `# Platform Integrations - prpl` \
    -DWITH_PRPL_INTEGRATION=ON \
    -DWITH_PRPL_EASYMESH=ON \
    -DWITH_PRPL_AMBIORIX=ON \
    \
    `# Platform Integrations - Matter/Thread/Zigbee` \
    -DWITH_MATTER_THREAD=ON \
    -DWITH_MATTER_BRIDGE=ON \
    -DWITH_THREAD_BR=ON \
    -DWITH_ZIGBEE=ON \
    -DWITH_ZIGBEE_EZSP=ON \
    -DWITH_ZIGBEE_ZSTACK=ON \
    \
    `# Build Options` \
    -DWITH_TESTS=ON \
    -DWITH_EXAMPLES=ON \
    -DWITH_DOCUMENTATION=ON \
    -DWITH_COVERAGE=ON \
    -DWITH_SANITIZERS=ON

# Check if CMake succeeded
if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}=================================${NC}"
    echo -e "${GREEN}CMake configuration successful!${NC}"
    echo -e "${GREEN}=================================${NC}"
    echo ""
    echo -e "${YELLOW}Next steps:${NC}"
    echo -e "  1. Build the project:     ${GREEN}make${NC}"
    echo -e "  2. Build examples:        ${GREEN}make friendly_example_client${NC}"
    echo -e "  3. Run tests:             ${GREEN}make test${NC}"
    echo -e "  4. Generate docs:         ${GREEN}make doc${NC}"
    echo -e "  5. Install:               ${GREEN}sudo make install${NC}"
    echo ""
else
    echo ""
    echo -e "${RED}=================================${NC}"
    echo -e "${RED}CMake configuration FAILED!${NC}"
    echo -e "${RED}=================================${NC}"
    echo ""
    echo -e "${YELLOW}Common issues:${NC}"
    echo -e "  - Missing dependencies (check error messages above)"
    echo -e "  - Incompatible options (some features may conflict)"
    echo -e "  - Platform-specific features not available on this system"
    echo ""
    exit 1
fi
