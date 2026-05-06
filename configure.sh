#!/bin/bash
#
# Interactive CMake Configuration Script for Friendly LwM2M Client
# Allows you to select which objects and features to include in the build
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Build directory
BUILD_DIR="build"

echo -e "${BLUE}================================================${NC}"
echo -e "${BLUE}  Friendly LwM2M Client - Build Configuration${NC}"
echo -e "${BLUE}================================================${NC}"
echo

# Function to prompt yes/no
prompt_yn() {
    local prompt="$1"
    local default="${2:-n}"
    local response

    if [ "$default" = "y" ]; then
        read -p "$prompt [Y/n]: " response
        response=${response:-y}
    else
        read -p "$prompt [y/N]: " response
        response=${response:-n}
    fi

    case "$response" in
        [yY][eE][sS]|[yY]) return 0 ;;
        *) return 1 ;;
    esac
}

# Function to prompt for build type
prompt_build_type() {
    echo -e "${GREEN}Select build type:${NC}"
    echo "  1) Debug   - with debug symbols, no optimization"
    echo "  2) Release - optimized, no debug symbols"
    echo "  3) RelWithDebInfo - optimized with debug symbols"
    read -p "Choice [2]: " choice
    choice=${choice:-2}

    case "$choice" in
        1) echo "Debug" ;;
        3) echo "RelWithDebInfo" ;;
        *) echo "Release" ;;
    esac
}

# Start configuration
echo -e "${YELLOW}=== Basic Configuration ===${NC}"
BUILD_TYPE=$(prompt_build_type)

# Create build directory
mkdir -p "$BUILD_DIR"

# Initialize CMake options array
CMAKE_OPTS=()
CMAKE_OPTS+=("-DCMAKE_BUILD_TYPE=$BUILD_TYPE")

echo
echo -e "${YELLOW}=== Core Features ===${NC}"

# DTLS Support
if prompt_yn "Enable DTLS support?" "y"; then
    CMAKE_OPTS+=("-DWITH_DTLS=ON")

    if prompt_yn "  Use mbedTLS (recommended)?" "y"; then
        CMAKE_OPTS+=("-DWITH_MBEDTLS=ON")
    else
        CMAKE_OPTS+=("-DWITH_MBEDTLS=OFF")
    fi

    if prompt_yn "  Use TinyDTLS for constrained devices?" "n"; then
        CMAKE_OPTS+=("-DWITH_TINYDTLS=ON")
    fi
else
    CMAKE_OPTS+=("-DWITH_DTLS=OFF")
fi

# Delta Firmware Updates
if prompt_yn "Enable delta firmware updates (FOTA)?" "y"; then
    CMAKE_OPTS+=("-DWITH_DELTA_FOTA=ON")

    if prompt_yn "  Include BSDIFF algorithm?" "y"; then
        CMAKE_OPTS+=("-DWITH_BSDIFF=ON")
    fi

    if prompt_yn "  Include VCDIFF algorithm?" "y"; then
        CMAKE_OPTS+=("-DWITH_VCDIFF=ON")
    fi

    if prompt_yn "  Include Courgette algorithm?" "y"; then
        CMAKE_OPTS+=("-DWITH_COURGETTE=ON")
    fi

    if prompt_yn "  Enable A/B partition rollback?" "y"; then
        CMAKE_OPTS+=("-DWITH_ROLLBACK=ON")
    fi
else
    CMAKE_OPTS+=("-DWITH_DELTA_FOTA=OFF")
fi

# MQTT Transport
if prompt_yn "Enable MQTT transport binding?" "y"; then
    CMAKE_OPTS+=("-DWITH_MQTT=ON")
else
    CMAKE_OPTS+=("-DWITH_MQTT=OFF")
fi

# Edge AI
if prompt_yn "Enable Edge AI inference?" "y"; then
    CMAKE_OPTS+=("-DWITH_EDGE_AI=ON")

    if prompt_yn "  Enable TensorFlow Lite backend?" "n"; then
        CMAKE_OPTS+=("-DWITH_TFLITE=ON")
    fi

    if prompt_yn "  Enable ONNX Runtime backend?" "n"; then
        CMAKE_OPTS+=("-DWITH_ONNX=ON")
    fi
else
    CMAKE_OPTS+=("-DWITH_EDGE_AI=OFF")
fi

echo
echo -e "${YELLOW}=== OpenWRT Integration ===${NC}"

if prompt_yn "Enable extended OpenWRT integration?" "y"; then
    CMAKE_OPTS+=("-DWITH_OPENWRT_EXTENDED=ON")

    if prompt_yn "  Build LuCI web interface module?" "y"; then
        CMAKE_OPTS+=("-DWITH_LUCI_MODULE=ON")
    fi

    if prompt_yn "  Install hotplug event handlers?" "y"; then
        CMAKE_OPTS+=("-DWITH_HOTPLUG_HANDLERS=ON")
    fi
else
    CMAKE_OPTS+=("-DWITH_OPENWRT_EXTENDED=OFF")
fi

echo
echo -e "${YELLOW}=== LwM2M Objects (Standard) ===${NC}"
echo -e "${BLUE}Note: Objects 0-3 are mandatory and always included${NC}"
echo

# Optional standard objects are included by default, just inform user
echo "Optional standard objects (4-13) will be included automatically."

echo
echo -e "${YELLOW}=== LwM2M Objects (Walt Technologies Custom) ===${NC}"
echo -e "${BLUE}Note: These require Wakaama submodule headers${NC}"
echo

# Router Management Objects
echo -e "${GREEN}Router Management Objects:${NC}"
if prompt_yn "  Enable LAN Configuration (10535)?" "n"; then
    CMAKE_OPTS+=("-DWITH_LAN_CONFIG=ON")
else
    CMAKE_OPTS+=("-DWITH_LAN_CONFIG=OFF")
fi

if prompt_yn "  Enable Routing Table (10536)?" "n"; then
    CMAKE_OPTS+=("-DWITH_ROUTING_TABLE=ON")
else
    CMAKE_OPTS+=("-DWITH_ROUTING_TABLE=OFF")
fi

if prompt_yn "  Enable VPN Configuration (10537)?" "n"; then
    CMAKE_OPTS+=("-DWITH_VPN_CONFIG=ON")
else
    CMAKE_OPTS+=("-DWITH_VPN_CONFIG=OFF")
fi

# Multi-WAN and Failover
echo
echo -e "${GREEN}Multi-WAN and Failover:${NC}"
if prompt_yn "  Enable WAN Failover Policy (10525)?" "n"; then
    CMAKE_OPTS+=("-DWITH_WAN_FAILOVER=ON")
else
    CMAKE_OPTS+=("-DWITH_WAN_FAILOVER=OFF")
fi

if prompt_yn "  Enable Multi-WAN Health Check (10526)?" "n"; then
    CMAKE_OPTS+=("-DWITH_MULTIWAN_HEALTH=ON")
else
    CMAKE_OPTS+=("-DWITH_MULTIWAN_HEALTH=OFF")
fi

# WiFi Management
echo
echo -e "${GREEN}WiFi Management:${NC}"
if prompt_yn "  Enable WiFi Client Management (10527)?" "n"; then
    CMAKE_OPTS+=("-DWITH_WIFI_CLIENT_MGMT=ON")
else
    CMAKE_OPTS+=("-DWITH_WIFI_CLIENT_MGMT=OFF")
fi

if prompt_yn "  Enable WiFi Channel Optimization (10528)?" "n"; then
    CMAKE_OPTS+=("-DWITH_WIFI_CHANNEL_OPT=ON")
else
    CMAKE_OPTS+=("-DWITH_WIFI_CHANNEL_OPT=OFF")
fi

echo
echo -e "${YELLOW}=== Starlink Integration ===${NC}"

if prompt_yn "Enable Starlink integration?" "y"; then
    CMAKE_OPTS+=("-DWITH_STARLINK=ON")

    if prompt_yn "  Enable Starlink gRPC support (requires gRPC)?" "y"; then
        CMAKE_OPTS+=("-DWITH_STARLINK_GRPC=ON")
    else
        CMAKE_OPTS+=("-DWITH_STARLINK_GRPC=OFF")
    fi
else
    CMAKE_OPTS+=("-DWITH_STARLINK=OFF")
fi

echo
echo -e "${YELLOW}=== Matter Integration ===${NC}"

if prompt_yn "Enable Matter/Thread integration?" "n"; then
    CMAKE_OPTS+=("-DWITH_MATTER=ON")

    if prompt_yn "  Enable Matter Bridge object (33450)?" "n"; then
        CMAKE_OPTS+=("-DWITH_MATTER_BRIDGE=ON")
    else
        CMAKE_OPTS+=("-DWITH_MATTER_BRIDGE=OFF")
    fi

    if prompt_yn "  Enable Thread Border Router support?" "n"; then
        CMAKE_OPTS+=("-DWITH_THREAD_BR=ON")
    else
        CMAKE_OPTS+=("-DWITH_THREAD_BR=OFF")
    fi
else
    CMAKE_OPTS+=("-DWITH_MATTER=OFF")
fi

echo
echo -e "${YELLOW}=== Zigbee Integration ===${NC}"

if prompt_yn "Enable Zigbee integration?" "n"; then
    CMAKE_OPTS+=("-DWITH_ZIGBEE=ON")

    if prompt_yn "  Enable Silicon Labs EZSP support?" "n"; then
        CMAKE_OPTS+=("-DWITH_ZIGBEE_EZSP=ON")
    fi

    if prompt_yn "  Enable TI Z-Stack support?" "n"; then
        CMAKE_OPTS+=("-DWITH_ZIGBEE_ZSTACK=ON")
    fi
else
    CMAKE_OPTS+=("-DWITH_ZIGBEE=OFF")
fi

echo
echo -e "${YELLOW}=== Build Options ===${NC}"

if prompt_yn "Build unit tests?" "y"; then
    CMAKE_OPTS+=("-DBUILD_TESTS=ON")
else
    CMAKE_OPTS+=("-DBUILD_TESTS=OFF")
fi

if prompt_yn "Build example applications?" "y"; then
    CMAKE_OPTS+=("-DBUILD_EXAMPLES=ON")
else
    CMAKE_OPTS+=("-DBUILD_EXAMPLES=OFF")
fi

if prompt_yn "Enable code coverage?" "y"; then
    CMAKE_OPTS+=("-DENABLE_COVERAGE=ON")
else
    CMAKE_OPTS+=("-DENABLE_COVERAGE=OFF")
fi

if prompt_yn "Enable sanitizers (address/undefined)?" "y"; then
    CMAKE_OPTS+=("-DENABLE_SANITIZERS=ON")
else
    CMAKE_OPTS+=("-DENABLE_SANITIZERS=OFF")
fi

echo
echo -e "${BLUE}================================================${NC}"
echo -e "${GREEN}Configuration Summary${NC}"
echo -e "${BLUE}================================================${NC}"
echo "Build directory: $BUILD_DIR"
echo "Build type: $BUILD_TYPE"
echo
echo "CMake command:"
echo "  cmake .. ${CMAKE_OPTS[@]}"
echo

if prompt_yn "Proceed with configuration?" "y"; then
    echo
    echo -e "${GREEN}Configuring...${NC}"
    cd "$BUILD_DIR"
    cmake .. "${CMAKE_OPTS[@]}"

    echo
    echo -e "${GREEN}Configuration complete!${NC}"
    echo
    echo "To build, run:"
    echo -e "  ${BLUE}cd $BUILD_DIR && make -j\$(nproc)${NC}"
    echo
    echo "Or to build specific targets:"
    echo -e "  ${BLUE}make friendly_example_client${NC}  - Main example client"
    echo -e "  ${BLUE}make friendly_lwm2m${NC}           - Library only"
    echo -e "  ${BLUE}make test${NC}                      - Run tests"
else
    echo
    echo -e "${YELLOW}Configuration cancelled.${NC}"
    exit 0
fi
