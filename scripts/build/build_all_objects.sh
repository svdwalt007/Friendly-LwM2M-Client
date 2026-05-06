#!/bin/bash
################################################################################
# Build Script - All LwM2M Objects Enabled
#
# This script builds the Friendly LwM2M Client with ALL available objects:
# - Mandatory objects (0, 1, 3)
# - Optional OMA objects (2, 4, 5, 6)
# - IPSO objects (3331, 3339)
# - Walt Technologies custom objects (10512-10537)
#
# Author: Sean van der Walt
# Version: 1.2.2
# Date: 2026-04-28
################################################################################

set -e  # Exit on error

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_DIR="build"
BUILD_ENV="DEV"
NUM_JOBS=$(nproc 2>/dev/null || echo 4)
CLEAN_BUILD=false

# Print banner
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Friendly LwM2M Client Build Script${NC}"
echo -e "${BLUE}  All Objects Configuration${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --env)
            BUILD_ENV="$2"
            shift 2
            ;;
        --jobs)
            NUM_JOBS="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --clean           Clean build directory before building"
            echo "  --env ENV         Build environment (TEST|DEV|PREPROD|PROD) [default: DEV]"
            echo "  --jobs N          Number of parallel jobs [default: nproc]"
            echo "  --help            Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0 --clean --env PROD"
            echo "  $0 --env TEST --jobs 8"
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Validate build environment
case $BUILD_ENV in
    TEST|DEV|PREPROD|PROD)
        ;;
    *)
        echo -e "${RED}Error: Invalid build environment: $BUILD_ENV${NC}"
        echo "Valid options: TEST, DEV, PREPROD, PROD"
        exit 1
        ;;
esac

echo -e "${GREEN}Build Configuration:${NC}"
echo "  Environment: $BUILD_ENV"
echo "  Jobs: $NUM_JOBS"
echo "  Clean build: $CLEAN_BUILD"
echo ""

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Configure CMake with all objects enabled
echo -e "${GREEN}Configuring CMake with all objects enabled...${NC}"
cmake -B "$BUILD_DIR" \
  -DBUILD_ENV="$BUILD_ENV" \
  -DBUILD_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  \
  -DWITH_DTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_OPENSSL=OFF \
  -DDTLS_PROVIDER_TINYDTLS=OFF \
  -DDTLS_PROVIDER_WOLFSSL=OFF \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  \
  -DWITH_DELTA_FOTA=ON \
  -DWITH_BSDIFF=ON \
  -DWITH_VCDIFF=ON \
  -DWITH_COURGETTE=ON \
  -DWITH_COMPRESSION=ON \
  -DWITH_ROLLBACK=ON \
  -DWITH_MQTT=ON \
  -DWITH_EDGE_AI=OFF \
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
  -DWITH_STARLINK=OFF \
  -DWITH_STARLINK_GRPC=OFF \
  \
  -DWITH_MATTER_BRIDGE=ON \
  -DWITH_THREAD_BR=OFF \
  \
  -DWITH_ZIGBEE=ON \
  -DWITH_ZIGBEE_EZSP=OFF \
  -DWITH_ZIGBEE_ZSTACK=OFF

if [ $? -ne 0 ]; then
    echo -e "${RED}CMake configuration failed!${NC}"
    exit 1
fi

# Build
echo ""
echo -e "${GREEN}Building with $NUM_JOBS parallel jobs...${NC}"
cmake --build "$BUILD_DIR" -j"$NUM_JOBS"

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

# Success
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  Build completed successfully!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# Show binary information
if [ -f "$BUILD_DIR/libfriendly_lwm2m.a" ]; then
    LIBSIZE=$(du -h "$BUILD_DIR/libfriendly_lwm2m.a" | cut -f1)
    echo -e "${BLUE}Library size: $LIBSIZE${NC}"
fi

if [ -f "$BUILD_DIR/examples/friendly_example_client" ]; then
    EXESIZE=$(du -h "$BUILD_DIR/examples/friendly_example_client" | cut -f1)
    echo -e "${BLUE}Example client size: $EXESIZE${NC}"
fi

echo ""
echo -e "${BLUE}Next steps:${NC}"
echo "  Run tests:     cd $BUILD_DIR && ctest --output-on-failure"
echo "  Run client:    ./$BUILD_DIR/examples/friendly_example_client"
echo "  List objects:  ./$BUILD_DIR/examples/friendly_example_client --list-objects"
echo ""
