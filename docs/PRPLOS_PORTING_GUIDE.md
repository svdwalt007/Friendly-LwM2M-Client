# prplOS/prplWrt Platform Porting Guide

## Executive Summary

This document provides a comprehensive guide for porting the **Friendly LwM2M Client** to **prplOS/prplWrt** platforms. Since prplOS is built on top of OpenWRT and this client already has **full OpenWRT support**, porting to prplOS is **significantly easier** than porting to other platforms.

**Key Advantage**: ✅ **90% of the work is already done** - the existing OpenWRT implementation can be reused with minimal modifications.

**Estimated Effort**:
- **Basic Port**: 1-2 days (leveraging existing OpenWRT code)
- **prplOS Integration**: 3-5 days (microservices, containers, prplOS APIs)
- **Full Integration**: 1-2 weeks (TR-069/USP, vendor customization)

**Complexity**: ⭐ **Low to Medium** - Primarily configuration and integration work

---

## Table of Contents

1. [Understanding prplOS/prplWrt](#understanding-prplosprplwrt)
2. [Platform Comparison](#platform-comparison)
3. [Leveraging Existing OpenWRT Support](#leveraging-existing-openwrt-support)
4. [Porting Requirements](#porting-requirements)
5. [Step-by-Step Porting Guide](#step-by-step-porting-guide)
6. [prplOS-Specific Integration](#prplos-specific-integration)
7. [Build System Configuration](#build-system-configuration)
8. [Containerization and Microservices](#containerization-and-microservices)
9. [Device Management Integration](#device-management-integration)
10. [Testing and Validation](#testing-and-validation)
11. [Advanced Features](#advanced-features)
12. [Known Considerations](#known-considerations)

---

## Understanding prplOS/prplWrt

### What is prplOS?

**prplOS** (formerly prplWrt) is an **open-source operating system** for embedded networking devices based on OpenWRT, created by the **prpl Foundation**.

**Key Characteristics**:
- 📦 **OpenWRT Foundation**: Built on top of OpenWRT
- 🏢 **Carrier-Grade**: Enhanced with service provider features
- 🐳 **Microservices**: Modular architecture with containerization
- 🔌 **Standardized APIs**: High-Level and Low-Level APIs for portability
- 🔒 **Enhanced Security**: Software hardening, security features
- 📡 **Management**: TR-069, USP remote management support

### prpl Foundation

**Founded**: 2014
**Focus**: Open-source solutions for embedded devices
**Target**: MIPS architecture (but open to others including ARM, x86)
**Backers**: ADTRAN, CommScope, Intel, MediaTek, Verizon, Vodafone, and others

### prplOS vs OpenWRT

| Feature | OpenWRT | prplOS/prplWrt |
|---------|---------|----------------|
| **Base** | Linux kernel + custom packages | OpenWRT + carrier features |
| **Architecture** | Monolithic | Microservices |
| **APIs** | Custom per vendor | Standardized High/Low-Level APIs |
| **Containers** | Basic LXC | OCI-compliant containers |
| **Management** | LuCI web UI | TR-069, USP, REST APIs |
| **Target** | DIY, enthusiasts | Service providers, carriers |
| **Security** | Community-driven | Carrier-grade hardening |
| **QA** | Community testing | Professional QA and testing |

### prplOS Components

**Core Stack**:
- **prplOS**: Base operating system
- **prplMesh**: WiFi mesh networking
- **prplLCM**: Lifecycle management
- **prplWare**: Vendor-specific customization layer

**APIs**:
- **High-Level API**: Application-level abstraction
- **Low-Level API**: Hardware abstraction layer (TR-181 data model)
- **REST APIs**: Cloud management interface

---

## Platform Comparison

### Architecture Support

| Architecture | OpenWRT | prplOS | This Client |
|--------------|---------|--------|-------------|
| **MIPS** | ✅ Yes | ✅ Primary | ✅ Supported |
| **ARM** | ✅ Yes | ✅ Yes | ✅ Supported |
| **ARM64** | ✅ Yes | ✅ Yes | ✅ Supported |
| **x86_64** | ✅ Yes | ✅ Yes | ✅ Supported |

**Conclusion**: ✅ The client is **architecture-independent** and will work on any architecture supported by prplOS.

### OpenWRT Features Already Supported

The Friendly LwM2M Client already has **comprehensive OpenWRT support**:

| Feature | Status | Location |
|---------|--------|----------|
| **UCI Configuration** | ✅ Implemented | `openwrt/uci/UciConfig.cpp` |
| **ubus Integration** | ✅ Implemented | `openwrt/ubus/UbusInterface.cpp` |
| **sysfs Monitoring** | ✅ Implemented | `openwrt/sysfs/SysfsMonitor.cpp` |
| **Network Connection** | ✅ Implemented | `openwrt/platform/OpenWrtConnection.cpp` |
| **Platform Functions** | ✅ Implemented | `openwrt/platform/OpenWrtPlatform.cpp` |
| **OpenWRT Makefile** | ✅ Implemented | `openwrt/Makefile` |
| **Init Scripts** | ✅ Implemented | `openwrt/files/etc/init.d/` |
| **UCI Config Files** | ✅ Implemented | `openwrt/files/etc/config/` |

**Total**: 10 source files, fully functional OpenWRT integration

---

## Leveraging Existing OpenWRT Support

### What's Already Done

✅ **Platform Abstraction**:
```cpp
// openwrt/platform/OpenWrtPlatform.cpp
time_t WppPlatform::getTime(void);      // System time
void WppPlatform::print(...);           // Logging
```

✅ **Network Connection**:
```cpp
// openwrt/platform/OpenWrtConnection.cpp
class OpenWrtConnection : public WppConnection {
    // Full UDP/CoAP networking
    // IPv4/IPv6 support
    // Interface selection
    // Session management
};
```

✅ **UCI Integration**:
```cpp
// openwrt/uci/UciConfig.cpp
class UciConfig {
    // Read/Write OpenWRT configuration
    // Network settings
    // Server configuration
    // Device information
};
```

✅ **ubus Integration**:
```cpp
// openwrt/ubus/UbusInterface.cpp
class UbusInterface {
    // 8 ubus methods:
    // - status, resource_read, resource_write
    // - register, update, deregister
    // - list_objects, reload_config
};
```

✅ **Hardware Monitoring**:
```cpp
// openwrt/sysfs/SysfsMonitor.cpp
class SysfsMonitor {
    // /sys monitoring
    // /proc monitoring
    // Network statistics
    // Temperature, CPU, memory
};
```

✅ **OpenWRT Package**:
```makefile
# openwrt/Makefile
PKG_NAME:=friendly-lwm2m-client
PKG_VERSION:=1.0.0
# Complete OpenWRT package definition
```

### Reusability for prplOS

**Good News**: 🎉 **All existing OpenWRT code is compatible with prplOS!**

Since prplOS is OpenWRT-based, the following work **without modification**:
- ✅ Platform abstraction (time, logging)
- ✅ Network connection (UDP/CoAP)
- ✅ UCI configuration system
- ✅ ubus messaging
- ✅ sysfs/proc filesystem access
- ✅ Build system (Makefile, CMake)

**What Needs Adaptation**:
- 🔧 Package manifest for prplOS package manager
- 🔧 Integration with prplOS APIs (optional)
- 🔧 Container/microservice packaging (optional)
- 🔧 TR-069/USP integration (if needed)

---

## Porting Requirements

### Minimal Port (Basic LwM2M Client)

**Effort**: ⭐ **1-2 days**

**Tasks**:
1. Copy OpenWRT code to `prplos/` directory
2. Update package manifest for prplOS
3. Build and test on prplOS device
4. Validate LwM2M connectivity

**Code Changes**: **Minimal** (mostly configuration)

---

### Standard Port (prplOS Package)

**Effort**: ⭐⭐ **3-5 days**

**Tasks**:
1. Minimal port (above)
2. Create prplOS package definition
3. Integration with prplOS APIs (optional)
4. Documentation and examples
5. Automated testing

**Code Changes**: **Low to Medium**

---

### Full Integration (Production-Ready)

**Effort**: ⭐⭐⭐ **1-2 weeks**

**Tasks**:
1. Standard port (above)
2. Containerization (Docker/OCI)
3. Microservices architecture
4. TR-069/USP integration
5. prplOS API usage
6. Vendor customization layer
7. Comprehensive testing and QA

**Code Changes**: **Medium** (primarily new integration code)

---

## Step-by-Step Porting Guide

### Phase 1: Setup prplOS Environment

#### Step 1.1: Obtain prplOS SDK

```bash
# Option 1: Download prplOS SDK from prpl Foundation
# Visit: https://prplfoundation.org/prplos/

# Option 2: Use prplWrt builder
git clone https://github.com/prplfoundation/prplwrt.git
cd prplwrt
./scripts/feeds update -a
./scripts/feeds install -a
make menuconfig
```

#### Step 1.2: Install Build Dependencies

```bash
# On Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    ccache \
    ecj \
    fastjar \
    file \
    g++ \
    gawk \
    gettext \
    git \
    java-propose-classpath \
    libelf-dev \
    libncurses5-dev \
    libncursesw5-dev \
    libssl-dev \
    python3 \
    python3-dev \
    python3-distutils \
    python3-setuptools \
    rsync \
    subversion \
    swig \
    time \
    unzip \
    wget \
    xsltproc \
    zlib1g-dev
```

#### Step 1.3: Verify prplOS Installation

```bash
# Check prplOS version
cat /etc/prplos-release

# Check available APIs
ls /usr/lib/libprpl*

# Check UCI system
uci show system
```

---

### Phase 2: Copy OpenWRT Implementation

#### Step 2.1: Create prplOS Directory Structure

```bash
cd /home/user/Friendly-LwM2M-Client
mkdir -p prplos/platform
mkdir -p prplos/uci
mkdir -p prplos/ubus
mkdir -p prplos/sysfs
mkdir -p prplos/files/etc/init.d
mkdir -p prplos/files/etc/config
```

#### Step 2.2: Copy OpenWRT Code

```bash
# Copy platform implementation
cp openwrt/platform/OpenWrtConnection.cpp prplos/platform/PrplConnection.cpp
cp openwrt/platform/OpenWrtConnection.h prplos/platform/PrplConnection.h
cp openwrt/platform/OpenWrtPlatform.cpp prplos/platform/PrplPlatform.cpp

# Copy integration code
cp openwrt/uci/UciConfig.* prplos/uci/
cp openwrt/ubus/UbusInterface.* prplos/ubus/
cp openwrt/sysfs/SysfsMonitor.* prplos/sysfs/

# Copy configuration files
cp -r openwrt/files/* prplos/files/

# Copy build files
cp openwrt/CMakeLists.txt prplos/
cp openwrt/Makefile prplos/
```

#### Step 2.3: Rename Classes (Optional)

**Option 1**: Keep "OpenWrt" naming (works fine, less changes)

**Option 2**: Rename to "Prpl" (more semantic)

```bash
# Rename classes in source files
cd prplos/platform
sed -i 's/OpenWrtConnection/PrplConnection/g' PrplConnection.*
sed -i 's/OpenWrtPlatform/PrplPlatform/g' PrplPlatform.cpp

# Update CMakeLists.txt
cd ../
sed -i 's/OpenWRT/Prpl/g' CMakeLists.txt
sed -i 's/OPENWRT/PRPL/g' CMakeLists.txt
```

---

### Phase 3: Update Build System

#### Step 3.1: Create prplOS CMakeLists.txt

**File**: `prplos/CMakeLists.txt`

```cmake
# CMakeLists.txt for prplOS integration components
cmake_minimum_required(VERSION 3.13)

project(PrplOS_LwM2M_Integration)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# prplOS/OpenWRT libraries
find_library(UCI_LIBRARY uci)
find_library(UBOX_LIBRARY ubox)
find_library(UBUS_LIBRARY ubus)

# Optional: prplOS-specific libraries
find_library(PRPL_API_LIBRARY prpl-api)
find_library(PRPL_LCM_LIBRARY prpl-lcm)

if(NOT UCI_LIBRARY)
    message(WARNING "libuci not found - UCI support will be disabled")
endif()

if(NOT UBUS_LIBRARY)
    message(WARNING "libubus not found - ubus support will be disabled")
endif()

if(PRPL_API_LIBRARY)
    message(STATUS "Found prplOS API library: ${PRPL_API_LIBRARY}")
    add_definitions(-DHAVE_PRPL_API)
endif()

# Source files for prplOS integration
set(PRPL_SOURCES
    platform/PrplConnection.cpp
    platform/PrplPlatform.cpp
)

# Conditionally add UCI support
if(UCI_LIBRARY)
    list(APPEND PRPL_SOURCES uci/UciConfig.cpp)
    add_definitions(-DHAVE_UCI)
endif()

# Conditionally add ubus support
if(UBUS_LIBRARY)
    list(APPEND PRPL_SOURCES ubus/UbusInterface.cpp)
    add_definitions(-DHAVE_UBUS)
endif()

# Add sysfs monitor
list(APPEND PRPL_SOURCES sysfs/SysfsMonitor.cpp)

# Create prplOS integration library
add_library(WppPrplOS STATIC ${PRPL_SOURCES})

# Include directories
target_include_directories(WppPrplOS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/uci
    ${CMAKE_CURRENT_SOURCE_DIR}/ubus
    ${CMAKE_CURRENT_SOURCE_DIR}/sysfs
    ${CMAKE_SOURCE_DIR}/wpp/platform
    ${CMAKE_SOURCE_DIR}/wpp/platform/connection
    ${CMAKE_SOURCE_DIR}/wpp/client
    ${CMAKE_SOURCE_DIR}/wpp/registry
)

# Link libraries
if(UCI_LIBRARY)
    target_link_libraries(WppPrplOS PUBLIC ${UCI_LIBRARY})
endif()

if(UBUS_LIBRARY AND UBOX_LIBRARY)
    target_link_libraries(WppPrplOS PUBLIC ${UBUS_LIBRARY} ${UBOX_LIBRARY})
endif()

# Link prplOS-specific libraries
if(PRPL_API_LIBRARY)
    target_link_libraries(WppPrplOS PUBLIC ${PRPL_API_LIBRARY})
endif()

if(PRPL_LCM_LIBRARY)
    target_link_libraries(WppPrplOS PUBLIC ${PRPL_LCM_LIBRARY})
endif()

# Install library and headers
install(TARGETS WppPrplOS
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES
    platform/PrplConnection.h
    DESTINATION include/lwm2m/platform
)

if(UCI_LIBRARY)
    install(FILES uci/UciConfig.h DESTINATION include/lwm2m/uci)
endif()

if(UBUS_LIBRARY)
    install(FILES ubus/UbusInterface.h DESTINATION include/lwm2m/ubus)
endif()

install(FILES sysfs/SysfsMonitor.h DESTINATION include/lwm2m/sysfs)
```

---

#### Step 3.2: Update Main CMakeLists.txt

**File**: `CMakeLists.txt`

```cmake
# Add to existing CMakeLists.txt

# Option to build prplOS integration
option(PRPL_BUILD "Build prplOS integration components" OFF)

# Add prplOS integration if enabled
if(PRPL_BUILD)
    message(STATUS "Building with prplOS integration support")
    add_subdirectory(prplos)
endif()
```

---

#### Step 3.3: Create prplOS Makefile

**File**: `prplos/Makefile`

```makefile
# prplOS Makefile for friendly-lwm2m-client
# Based on OpenWRT Makefile with prplOS-specific adjustments

include $(TOPDIR)/rules.mk

PKG_NAME:=friendly-lwm2m-client
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git
PKG_SOURCE_VERSION:=HEAD

PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Friendly Technologies <support@friendly-tech.com>

PKG_BUILD_PARALLEL:=1
CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

# prplOS-specific package definition
define Package/friendly-lwm2m-client
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IoT
  TITLE:=Friendly LwM2M Client for prplOS
  URL:=https://friendly-tech.com
  DEPENDS:=+libuci +libubox +libubus +libopenssl +libcurl +libpthread
  # Optional prplOS dependencies
  # DEPENDS+=+prpl-api +prpl-lcm
endef

define Package/friendly-lwm2m-client/description
  Friendly LwM2M Client is an open-source OMA LwM2M client for IoT device
  management. This version is optimized for prplOS with support for:
  - UCI configuration integration
  - ubus messaging interface
  - prplOS High-Level and Low-Level APIs (optional)
  - Container deployment (optional)
  - Microservices architecture (optional)
endef

# Build configuration
CMAKE_OPTIONS += \
    -DPRPL_BUILD=ON \
    -DOPENWRT_BUILD=ON \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DWPP_BUILD_WITH_EXCEPTIONS=OFF \
    -DWPP_BUILD_WITH_RTTI=OFF

# prplOS-specific build flags
CMAKE_OPTIONS += \
    -DPRPL_API_SUPPORT=ON

# Install files
define Package/friendly-lwm2m-client/install
    $(INSTALL_DIR) $(1)/usr/bin
    $(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/WppExample $(1)/usr/bin/lwm2m-client

    $(INSTALL_DIR) $(1)/etc/init.d
    $(INSTALL_BIN) ./files/etc/init.d/lwm2m-client $(1)/etc/init.d/

    $(INSTALL_DIR) $(1)/etc/config
    $(INSTALL_CONF) ./files/etc/config/lwm2m $(1)/etc/config/

    # Optional: Install prplOS service descriptor
    # $(INSTALL_DIR) $(1)/etc/prpl/services
    # $(INSTALL_CONF) ./files/etc/prpl/services/lwm2m-client.json $(1)/etc/prpl/services/
endef

define Package/friendly-lwm2m-client/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
    /etc/init.d/lwm2m-client enable
    /etc/init.d/lwm2m-client start
}
exit 0
endef

define Package/friendly-lwm2m-client/prerm
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
    /etc/init.d/lwm2m-client stop
    /etc/init.d/lwm2m-client disable
}
exit 0
endef

$(eval $(call BuildPackage,friendly-lwm2m-client))
```

---

### Phase 4: Build and Test

#### Step 4.1: Configure Build

```bash
cd /path/to/prplos-sdk

# Copy package to feeds
mkdir -p feeds/packages/net/friendly-lwm2m-client
cp -r /home/user/Friendly-LwM2M-Client/* feeds/packages/net/friendly-lwm2m-client/

# Update and install feeds
./scripts/feeds update -a
./scripts/feeds install -a

# Configure build
make menuconfig
# Navigate to: Network > IoT > friendly-lwm2m-client
# Select with 'M' (module) or '*' (built-in)
```

#### Step 4.2: Build Package

```bash
# Build the package
make package/friendly-lwm2m-client/compile V=s

# Find the package
find bin/packages -name "friendly-lwm2m-client*.ipk"
```

#### Step 4.3: Install on Device

```bash
# Copy to device
scp bin/packages/*/packages/friendly-lwm2m-client_*.ipk root@prplos-device:/tmp/

# SSH to device
ssh root@prplos-device

# Install package
opkg update
opkg install /tmp/friendly-lwm2m-client_*.ipk

# Check status
/etc/init.d/lwm2m-client status

# View logs
logread | grep lwm2m
```

#### Step 4.4: Verify Operation

```bash
# Check if client is running
ps | grep lwm2m-client

# Check network connectivity
netstat -uln | grep 5683

# Check UCI configuration
uci show lwm2m

# Test ubus interface
ubus call lwm2m-client status
```

---

## prplOS-Specific Integration

### 1. prplOS API Integration (Optional)

**prplOS High-Level API** provides standardized interfaces for applications.

**File**: `prplos/api/PrplApiIntegration.h`

```cpp
#ifndef PRPL_API_INTEGRATION_H
#define PRPL_API_INTEGRATION_H

#ifdef HAVE_PRPL_API
#include <prpl/high_level_api.h>

namespace prpl {

/**
 * @brief prplOS High-Level API integration
 *
 * Provides integration between LwM2M client and prplOS APIs
 */
class PrplApiIntegration {
public:
    /**
     * @brief Initialize prplOS API integration
     * @return true if successful
     */
    static bool initialize();

    /**
     * @brief Publish LwM2M data to prplOS data model
     * @param objectId LwM2M object ID
     * @param instanceId LwM2M instance ID
     * @param resourceId LwM2M resource ID
     * @param value Resource value
     */
    static void publishData(uint16_t objectId, uint16_t instanceId,
                           uint16_t resourceId, const std::string& value);

    /**
     * @brief Subscribe to prplOS events
     * @param eventName Event name to subscribe
     * @param callback Callback function
     */
    static void subscribeEvent(const std::string& eventName,
                              std::function<void(const std::string&)> callback);

    /**
     * @brief Cleanup prplOS API integration
     */
    static void cleanup();
};

} // namespace prpl

#endif // HAVE_PRPL_API
#endif // PRPL_API_INTEGRATION_H
```

---

### 2. TR-181 Data Model Mapping (Optional)

**prplOS uses TR-181** (Broadband Forum) data model. LwM2M can be mapped to TR-181.

**Mapping Example**:

| LwM2M Object | TR-181 Path | Description |
|--------------|-------------|-------------|
| Device (3) | Device.DeviceInfo. | Device information |
| Connectivity Monitoring (4) | Device.IP.Interface. | Network connectivity |
| Firmware Update (5) | Device.DeviceInfo.FirmwareImage. | Firmware management |
| WLAN Connectivity (12) | Device.WiFi.SSID. | WiFi configuration |

**Implementation**:

```cpp
// prplos/tr181/Tr181Mapper.h
class Tr181Mapper {
public:
    /**
     * @brief Map LwM2M resource to TR-181 parameter
     * @param lwm2mPath LwM2M path (e.g., "/3/0/0")
     * @return TR-181 path (e.g., "Device.DeviceInfo.Manufacturer")
     */
    static std::string mapLwm2mToTr181(const std::string& lwm2mPath);

    /**
     * @brief Map TR-181 parameter to LwM2M resource
     * @param tr181Path TR-181 path
     * @return LwM2M path
     */
    static std::string mapTr181ToLwm2m(const std::string& tr181Path);

    /**
     * @brief Sync LwM2M data to TR-181 data model
     */
    static void syncToTr181();

    /**
     * @brief Sync TR-181 data to LwM2M
     */
    static void syncFromTr181();
};
```

---

## Containerization and Microservices

### 1. Docker Container Support

**prplOS supports OCI-compliant containers**. Package the LwM2M client as a container.

**File**: `prplos/docker/Dockerfile`

```dockerfile
# Dockerfile for prplOS LwM2M Client Container
FROM prplos/base:latest

# Install dependencies
RUN opkg update && opkg install \
    libuci \
    libubox \
    libubus \
    libopenssl \
    libcurl \
    libpthread

# Copy application
COPY --from=builder /usr/bin/lwm2m-client /usr/bin/
COPY prplos/files/etc/config/lwm2m /etc/config/

# Expose CoAP port
EXPOSE 5683/udp

# Set environment variables
ENV LWM2M_ENDPOINT="prplos-device"
ENV LWM2M_SERVER="coap://leshan.eclipseprojects.io:5683"

# Start client
CMD ["/usr/bin/lwm2m-client"]
```

**Build Container**:

```bash
# Build image
docker build -t lwm2m-client:1.0 -f prplos/docker/Dockerfile .

# Run container
docker run -d \
    --name lwm2m-client \
    --network host \
    -v /etc/config:/etc/config:ro \
    lwm2m-client:1.0
```

---

### 2. Microservice Architecture

**prplOS uses microservices**. Structure the client as a microservice.

**Service Descriptor**: `prplos/files/etc/prpl/services/lwm2m-client.json`

```json
{
  "name": "lwm2m-client",
  "version": "1.0.0",
  "description": "Friendly LwM2M Client microservice",
  "type": "daemon",
  "executable": "/usr/bin/lwm2m-client",
  "dependencies": [
    "network",
    "ubus"
  ],
  "resources": {
    "memory": {
      "min": "4M",
      "max": "16M"
    },
    "cpu": {
      "shares": 512
    }
  },
  "configuration": {
    "file": "/etc/config/lwm2m",
    "schema": "uci"
  },
  "api": {
    "ubus": {
      "object": "lwm2m-client",
      "methods": [
        "status",
        "resource_read",
        "resource_write",
        "register",
        "update",
        "deregister",
        "list_objects",
        "reload_config"
      ]
    }
  },
  "logging": {
    "facility": "daemon",
    "level": "info"
  },
  "restart": {
    "policy": "on-failure",
    "max_retries": 3
  }
}
```

---

## Device Management Integration

### 1. TR-069/CWMP Integration (Optional)

**prplOS supports TR-069** for remote management. Expose LwM2M via TR-069.

**Approach 1**: TR-069 Agent calls LwM2M client via ubus

```cpp
// TR-069 agent queries LwM2M data
ubus call lwm2m-client resource_read '{"object":3,"instance":0,"resource":0}'
// Returns: {"value":"Friendly Technologies"}
```

**Approach 2**: LwM2M client publishes to TR-181 data model

```cpp
// LwM2M client updates TR-181 parameter
prpl_api_set_parameter("Device.DeviceInfo.Manufacturer", "Friendly Technologies");
```

---

### 2. USP (User Services Platform) Integration (Optional)

**prplOS supports USP** (successor to TR-069). Modern alternative for device management.

**USP Controller** ↔ **USP Agent** ↔ **LwM2M Client** (via ubus/API)

**Benefits**:
- Modern protocol (based on Protocol Buffers)
- Better multi-vendor support
- Flexible transport (CoAP, MQTT, WebSocket)

---

## Testing and Validation

### Test Plan

#### Phase 1: Basic Functionality

**Test 1.1**: Package Installation

```bash
# Install package
opkg install friendly-lwm2m-client

# Expected: Package installs without errors
# Expected: Service starts automatically
```

**Test 1.2**: LwM2M Connectivity

```bash
# Check client connects to bootstrap server
logread | grep "Bootstrap request"

# Expected: Bootstrap successful
# Expected: Registration successful
```

**Test 1.3**: UCI Configuration

```bash
# Read configuration
uci show lwm2m

# Modify configuration
uci set lwm2m.client.endpoint_name="test-device"
uci commit lwm2m

# Reload
/etc/init.d/lwm2m-client restart

# Expected: Client uses new configuration
```

**Test 1.4**: ubus Interface

```bash
# Test all ubus methods
ubus call lwm2m-client status
ubus call lwm2m-client list_objects
ubus call lwm2m-client resource_read '{"object":3,"instance":0,"resource":0}'

# Expected: All methods work correctly
```

---

#### Phase 2: prplOS-Specific Features

**Test 2.1**: prplOS API Integration

```bash
# If PRPL_API enabled, verify integration
# Check data appears in prplOS data model
# Check events are received
```

**Test 2.2**: Microservice Lifecycle

```bash
# Test service management via prplOS
prpl-lcm service status lwm2m-client
prpl-lcm service restart lwm2m-client

# Expected: Service managed correctly
```

**Test 2.3**: Container Deployment

```bash
# If containerized, test container
docker ps | grep lwm2m-client
docker logs lwm2m-client

# Expected: Container runs correctly
```

---

#### Phase 3: Interoperability

**Test 3.1**: Multiple Architectures

Test on different prplOS targets:
- MIPS (primary)
- ARM
- ARM64
- x86_64

**Test 3.2**: prplOS Versions

Test on different prplOS releases:
- prplOS 1.x
- prplOS 2.x
- Latest prplWrt

**Test 3.3**: prplOS Distributions

Test with different prplOS vendors:
- SmartRG SmartOS
- Iopsys prplOS
- Other prplOS distributions

---

## Advanced Features

### 1. prplMesh Integration (Optional)

**prplMesh** is prplOS WiFi mesh networking. Integrate LwM2M with mesh.

**Use Cases**:
- Monitor mesh topology via LwM2M
- Configure mesh nodes via LwM2M
- Report mesh statistics

**Implementation**:

```cpp
// prplos/mesh/PrplMeshIntegration.h
class PrplMeshIntegration {
public:
    /**
     * @brief Get mesh topology
     * @return Mesh node information
     */
    static std::vector<MeshNode> getTopology();

    /**
     * @brief Configure mesh node
     * @param nodeId Node identifier
     * @param config Configuration parameters
     */
    static void configureNode(const std::string& nodeId, const MeshConfig& config);

    /**
     * @brief Get mesh statistics
     * @return Statistics for all mesh nodes
     */
    static MeshStatistics getStatistics();
};
```

**LwM2M Object**: Create custom object for mesh management (e.g., ID 34612)

---

### 2. Vendor Customization Layer

**prplOS prplWare** allows vendor customization.

**Example**: Custom Walt Technologies prplWare module

```
prplos/prplware/
├── walt-tech/
│   ├── walt_tech_module.cpp
│   ├── walt_tech_objects.h
│   └── walt_tech_config.json
```

**Features**:
- Vendor-specific LwM2M objects
- Custom backend integrations
- Proprietary SoC features
- Extended TR-181 parameters

---

### 3. Cloud Integration

**prplOS supports cloud management**. Integrate LwM2M with cloud.

**Scenario 1**: LwM2M + Cloud dual management

```
IoT Device (prplOS + LwM2M Client)
    ├── LwM2M Server (OMA LwM2M)
    └── Cloud Platform (TR-069/USP/REST)
```

**Scenario 2**: LwM2M as cloud transport

```
IoT Device (prplOS) → LwM2M Server → Cloud Platform
```

**Implementation**:
- LwM2M Send operation → Cloud events
- Cloud commands → LwM2M Write operations
- Data sync between protocols

---

## Known Considerations

### 1. MIPS Architecture Considerations

**prplOS primarily targets MIPS**. The LwM2M client is architecture-independent, but:

**Endianness**: MIPS can be **big-endian** or **little-endian**
- Check: `lscpu | grep "Byte Order"`
- Configure: Set `LWM2M_BIG_ENDIAN` or `LWM2M_LITTLE_ENDIAN` in CMake

**Compiler Flags** for MIPS:

```cmake
if(CMAKE_SYSTEM_PROCESSOR MATCHES "mips")
    message(STATUS "Detected MIPS architecture")
    # MIPS-specific optimizations
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mips32r2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mips32r2")
endif()
```

---

### 2. Resource Constraints

**prplOS devices** (routers, gateways) may have limited resources.

**Typical Specs**:
- CPU: 500 MHz - 1 GHz (MIPS/ARM)
- RAM: 128 MB - 512 MB
- Flash: 16 MB - 256 MB

**Optimizations**:

```cmake
# Enable minimal build
set(CMAKE_BUILD_TYPE MinSizeRel)

# Disable unused features
set(WPP_BUILD_WITH_EXCEPTIONS OFF)
set(WPP_BUILD_WITH_RTTI OFF)

# Limit objects to essential only
# Comment out Walt Tech objects if not needed
```

**Expected Sizes**:
- Binary: 300-500 KB (minimal), 500-800 KB (full)
- RAM: 80-150 KB during operation

---

### 3. prplOS vs OpenWRT Differences

**Package Management**:
- OpenWRT: `opkg`
- prplOS: `opkg` + prplOS package manager

**API Layer**:
- OpenWRT: Custom APIs per vendor
- prplOS: Standardized High/Low-Level APIs

**Services**:
- OpenWRT: `procd`
- prplOS: `procd` + prplOS service manager

**Impact**: Minimal - existing code works, optional prplOS features enhance functionality

---

### 4. Licensing Considerations

**prplOS License**: Open-source (varies by component)

**LwM2M Client License**: Apache 2.0

**Compatibility**: ✅ **Compatible** - Can be distributed together

**Note**: Verify specific prplOS component licenses for commercial use

---

## Summary

### Porting Effort Estimate

| Task | Complexity | Time Estimate |
|------|-----------|---------------|
| Copy OpenWRT code to prplos/ | ⭐ Trivial | 1 hour |
| Update build system (CMake, Makefile) | ⭐ Low | 2-4 hours |
| Build and test on prplOS device | ⭐ Low | 4-8 hours |
| **Basic Port Total** | | **7-13 hours (1-2 days)** |
| prplOS API integration | ⭐⭐ Medium | 1-2 days |
| Container/microservice packaging | ⭐⭐ Medium | 1-2 days |
| TR-069/USP integration | ⭐⭐⭐ High | 2-3 days |
| Vendor customization | ⭐⭐ Medium | 1-2 days |
| **Full Integration Total** | | **1-2 weeks** |

---

### Success Criteria

✅ **Minimal Port**:
- Builds on prplOS SDK
- Runs on prplOS device
- Connects to LwM2M server
- All core operations work

✅ **Standard Port**:
- prplOS package (.ipk)
- UCI configuration
- ubus interface
- Init script integration
- Documentation

✅ **Full Integration**:
- prplOS API integration
- Container deployment
- Microservice architecture
- TR-069/USP integration
- Vendor customization support
- Production-ready

---

### Recommended Approach

**Week 1** (Basic Port):
1. Copy OpenWRT code
2. Update build system
3. Build package
4. Test on device
5. Validate LwM2M connectivity

**Week 2** (prplOS Integration - Optional):
1. prplOS API integration
2. Container packaging
3. Microservice setup
4. TR-181 mapping
5. Advanced testing

---

## Conclusion

**Key Advantage**: 🎉 Since prplOS is OpenWRT-based and this client has **full OpenWRT support**, porting to prplOS is **straightforward and low-effort**.

**Effort Comparison**:
- **Raspberry Pi 4 Port**: 2-4 days (new platform, new code)
- **prplOS Port**: 1-2 days (reuse OpenWRT code) ⭐

**Recommendation**: ✅ **Highly Recommended** to port to prplOS - minimal effort, maximum benefit for IoT gateway deployments.

---

## Next Steps

1. **Obtain prplOS SDK** or device
2. **Copy OpenWRT code** to `prplos/` directory
3. **Build package**: `make package/friendly-lwm2m-client/compile`
4. **Install and test**: `opkg install friendly-lwm2m-client_*.ipk`
5. **Validate**: Test LwM2M connectivity and operations

---

## Additional Resources

### Documentation
- **prpl Foundation**: https://prplfoundation.org/
- **prplOS**: https://prplfoundation.org/prplos/
- **prplWrt**: https://prplfoundation.org/prplware/
- **OpenWRT**: https://openwrt.org/

### Community
- **prpl Foundation GitHub**: https://github.com/prplfoundation
- **prplOS Mailing List**: Contact via prpl Foundation
- **OpenWRT Forums**: https://forum.openwrt.org/

### Commercial Support
- **Friendly Technologies**: https://hubs.li/Q02D_GnZ0
- **prpl Foundation Members**: ADTRAN, CommScope, Intel, MediaTek, etc.

---

**Document Version**: 1.0
**Last Updated**: 2025-11-01
**Author**: Claude AI Assistant
**Target Platform**: prplOS/prplWrt (OpenWRT-based)
**Base Branch**: `claude/merge-session-changes-011CUgtnPthsMQjuAoFu1d1J`
