# Raspberry Pi 4 ARM64 Platform Porting Guide

## Executive Summary

This document provides a comprehensive guide for porting the **Friendly LwM2M Client** to **Raspberry Pi 4 ARM64** (aarch64) architecture. The client's modular platform abstraction layer makes porting straightforward, requiring primarily:

1. **Toolchain configuration** for ARM64 cross-compilation
2. **Platform implementation** for time and logging functions
3. **Connection implementation** for UDP/DTLS networking
4. **Hardware-specific adaptations** for optional Raspberry Pi features
5. **Build system integration** with CMake

**Estimated Effort**: 2-4 days for basic port, 1-2 weeks for full integration with RPi-specific features.

**Complexity**: **Medium** - Platform abstraction exists, but requires toolchain setup and testing.

---

## Table of Contents

1. [Platform Architecture Overview](#platform-architecture-overview)
2. [Prerequisites](#prerequisites)
3. [Porting Requirements](#porting-requirements)
4. [Step-by-Step Porting Guide](#step-by-step-porting-guide)
5. [Build System Configuration](#build-system-configuration)
6. [Platform-Specific Implementation](#platform-specific-implementation)
7. [Hardware Integration](#hardware-integration)
8. [Testing and Validation](#testing-and-validation)
9. [Known Challenges](#known-challenges)
10. [Optimization Opportunities](#optimization-opportunities)

---

## Platform Architecture Overview

### Current Platform Support

| Platform | Status | Location | Notes |
|----------|--------|----------|-------|
| **Linux x86_64** | ✅ Full | `examples/platform/` | Default platform |
| **OpenWRT (MIPS/ARM)** | ✅ Full | `openwrt/platform/` | Router-specific features |
| **Raspberry Pi 4 ARM64** | ⚠️ Pending | TBD | Target platform |

### Platform Abstraction Layers

The client uses a clean platform abstraction with two main interfaces:

#### 1. **WppPlatform** (`wpp/platform/WppPlatform.h`)

**Purpose**: System-level abstractions

**Required Methods**:
```cpp
class WppPlatform {
public:
    static time_t getTime(void);              // System time (seconds since epoch)
    static void print(const char* msg, va_list arg);  // Logging output
};
```

**Complexity**: ⭐ Low - Simple to implement

---

#### 2. **WppConnection** (`wpp/platform/connection/WppConnection.h`)

**Purpose**: Network connectivity (UDP/DTLS)

**Required Methods**:
```cpp
class WppConnection {
public:
    virtual SESSION_T connect(Lwm2mSecurity& security) = 0;  // Establish connection
    virtual void disconnect(SESSION_T session) = 0;          // Close connection
    virtual bool sessionCmp(SESSION_T s1, SESSION_T s2) = 0; // Compare sessions
    virtual bool sendPacket(const Packet& packet) = 0;       // Send CoAP packet
};
```

**Complexity**: ⭐⭐ Medium - Socket programming required

---

### Directory Structure

```
Friendly-LwM2M-Client/
├── wpp/                          # Core WPP library (platform-independent)
│   ├── platform/                 # Platform abstraction interfaces
│   │   ├── WppPlatform.h         # Time & logging interface
│   │   ├── WppPlatform.cpp       # Default implementation
│   │   └── connection/
│   │       └── WppConnection.h   # Network interface
│   ├── registry/                 # LwM2M objects (platform-independent)
│   └── client/                   # Client logic (platform-independent)
├── examples/                     # Example implementations
│   └── platform/                 # Linux x86_64 platform
│       ├── Platform.cpp          # Linux time/logging
│       ├── connection/           # UDP socket implementation
│       └── connection_dtls/      # DTLS socket implementation
├── openwrt/                      # OpenWRT platform (MIPS/ARM)
│   └── platform/                 # OpenWRT-specific features
│       ├── OpenWrtPlatform.cpp   # OpenWRT time/logging
│       └── OpenWrtConnection.*   # UCI-integrated networking
└── rpi4/                         # Raspberry Pi 4 platform (NEW)
    └── platform/                 # RPi4-specific features
        ├── RpiPlatform.cpp       # RPi4 time/logging
        └── RpiConnection.*       # RPi4 networking
```

---

## Prerequisites

### Hardware Requirements

| Component | Specification | Notes |
|-----------|--------------|-------|
| **Board** | Raspberry Pi 4 Model B | 2GB/4GB/8GB RAM |
| **Architecture** | ARM64 (aarch64) | 64-bit mode |
| **OS** | Raspberry Pi OS 64-bit | Debian-based |
| **Storage** | ≥8GB microSD | 16GB+ recommended |
| **Network** | Ethernet or WiFi | For LwM2M connectivity |

### Software Requirements

#### On Development Machine (x86_64 Linux)

```bash
# Cross-compilation toolchain
sudo apt-get install -y \
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    binutils-aarch64-linux-gnu

# Build tools
sudo apt-get install -y \
    cmake \
    ninja-build \
    pkg-config \
    git

# Dependencies (for cross-compilation)
# Will need ARM64 versions of libraries
```

#### On Raspberry Pi 4 (aarch64)

```bash
# Build essentials
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config

# LwM2M client dependencies
sudo apt-get install -y \
    libssl-dev \          # OpenSSL/mbedTLS for DTLS
    libcurl4-openssl-dev  # For firmware update (optional)

# Optional: Development tools
sudo apt-get install -y \
    clang \
    gdb \
    valgrind
```

### Toolchain Selection

**Option 1: Native Compilation on RPi4** ⭐ Recommended for development

- Build directly on Raspberry Pi 4
- Simpler setup, easier debugging
- Slower compilation

**Option 2: Cross-Compilation from x86_64** ⭐⭐ Recommended for production

- Build on fast development machine
- Faster compilation
- Requires toolchain setup

---

## Porting Requirements

### 1. Platform Abstraction Implementation

#### Required Files

```
rpi4/platform/
├── RpiPlatform.cpp        # Time & logging implementation
├── RpiConnection.h        # Network interface header
└── RpiConnection.cpp      # Network implementation
```

#### Minimal Implementation

**Complexity**: ⭐ Low (1-2 hours)

**Tasks**:
- Implement `WppPlatform::getTime()` - Use `std::time(NULL)`
- Implement `WppPlatform::print()` - Use `vfprintf(stderr, msg, arg)`
- Copy from `examples/platform/Platform.cpp`

---

### 2. Network Connection Implementation

#### Required Functionality

**Complexity**: ⭐⭐ Medium (4-8 hours)

**Tasks**:
- UDP socket creation and binding
- IPv4/IPv6 support
- DNS resolution
- Packet send/receive
- Session management
- Optional: DTLS support

**Base Implementation**: Copy from `examples/platform/connection/Connection.cpp`

**DTLS Support** (Optional): Copy from `examples/platform/connection_dtls/`

---

### 3. Build System Integration

#### CMake Configuration

**Complexity**: ⭐ Low (2-4 hours)

**Tasks**:
- Create `rpi4/CMakeLists.txt`
- Configure ARM64 compiler flags
- Set up dependency linking
- Add RPi4 build option

---

### 4. Hardware-Specific Features (Optional)

**Complexity**: ⭐⭐⭐ High (varies)

**Optional Integrations**:
- GPIO access via `/sys/class/gpio/` (similar to OpenWRT)
- Hardware watchdog (`/dev/watchdog`)
- Temperature monitoring (`/sys/class/thermal/`)
- Camera interface
- Display (HDMI)
- Bluetooth/WiFi configuration

---

## Step-by-Step Porting Guide

### Phase 1: Setup Development Environment

#### Step 1.1: Clone Repository

```bash
git clone --recurse-submodules git@github.com:Friendly-Technologies/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client
```

#### Step 1.2: Install Dependencies on RPi4

```bash
# Update system
sudo apt-get update
sudo apt-get upgrade -y

# Install build tools
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libssl-dev \
    libcurl4-openssl-dev

# Optional: Install clang (used by default config)
sudo apt-get install -y clang
```

#### Step 1.3: Verify Architecture

```bash
# Should output: aarch64
uname -m

# Should output: Linux rpi4 ... aarch64 GNU/Linux
uname -a

# Check GCC version
gcc --version
# Should be gcc (Debian/Raspbian) 10.x or newer
```

---

### Phase 2: Create Platform Implementation

#### Step 2.1: Create Directory Structure

```bash
cd /home/user/Friendly-LwM2M-Client
mkdir -p rpi4/platform/connection
```

#### Step 2.2: Implement RpiPlatform.cpp

**File**: `rpi4/platform/RpiPlatform.cpp`

```cpp
/*
 * RpiPlatform.cpp
 * Raspberry Pi 4 platform implementation
 */

#include "WppPlatform.h"
#include <ctime>
#include <cstdio>

namespace wpp {

/**
 * @brief Returns system time in seconds since Unix epoch
 * Uses standard POSIX time() function
 */
time_t WppPlatform::getTime(void) {
    return std::time(NULL);
}

/**
 * @brief Print formatted log message to stderr
 * @param msg Format string (printf-style)
 * @param arg Variable argument list
 */
void WppPlatform::print(const char* msg, va_list arg) {
    // Output to stderr for logging
    vfprintf(stderr, msg, arg);
    // Optional: Also write to syslog or file
    // vsyslog(LOG_INFO, msg, arg);
}

} // namespace wpp
```

**Complexity**: ⭐ Trivial - Direct copy from `examples/platform/Platform.cpp`

---

#### Step 2.3: Implement RpiConnection.h

**File**: `rpi4/platform/connection/RpiConnection.h`

```cpp
/*
 * RpiConnection.h
 * Raspberry Pi 4 network connection implementation
 */

#ifndef RPI_CONNECTION_H
#define RPI_CONNECTION_H

#include <netinet/in.h>
#include <string>
#include "WppConnection.h"
#include "WppRegistry.h"

namespace wpp {

/**
 * @brief Raspberry Pi 4 connection implementation
 *
 * Provides UDP/CoAP networking for Raspberry Pi 4.
 * Features:
 * - IPv4 and IPv6 support
 * - Non-blocking sockets
 * - Session management
 * - DNS resolution
 */
class RpiConnection : public WppConnection {
private:
    struct connection_t {
        struct connection_t* next;
        int sock;
        struct sockaddr_in6 addr;
        size_t addrLen;
    };

public:
    /**
     * @brief Constructor
     * @param port Local CoAP port (default: "5683")
     * @param addressFamily AF_INET or AF_INET6 (default: AF_INET6)
     */
    RpiConnection(const std::string& port = "5683",
                  int addressFamily = AF_INET6);

    /**
     * @brief Destructor - cleanup connections and sockets
     */
    ~RpiConnection();

    // WppConnection interface implementation
    SESSION_T connect(Lwm2mSecurity& security) override;
    void disconnect(SESSION_T session) override;
    bool sessionCmp(SESSION_T session1, SESSION_T session2) override;
    bool sendPacket(const Packet& packet) override;

    /**
     * @brief Main loop for receiving packets
     * Call regularly (e.g., every 100ms) to process incoming data
     */
    void loop();

private:
    bool openSocket();
    connection_t* createNewConn(struct sockaddr* addr, size_t addrLen);
    std::string uriToPort(const std::string& uri);
    std::string uriToHost(const std::string& uri);

private:
    std::string _port;
    int _addressFamily;
    int _connFd;
    connection_t* _connections;
};

} // namespace wpp

#endif // RPI_CONNECTION_H
```

---

#### Step 2.4: Implement RpiConnection.cpp

**File**: `rpi4/platform/connection/RpiConnection.cpp`

```cpp
/*
 * RpiConnection.cpp
 * Raspberry Pi 4 network connection implementation
 */

#include "RpiConnection.h"
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

namespace wpp {

RpiConnection::RpiConnection(const std::string& port, int addressFamily)
    : _port(port)
    , _addressFamily(addressFamily)
    , _connFd(-1)
    , _connections(nullptr)
{
    if (!openSocket()) {
        std::cerr << "Failed to open socket on port " << port << std::endl;
    }
}

RpiConnection::~RpiConnection() {
    // Close all connections
    while (_connections) {
        connection_t* conn = _connections;
        _connections = _connections->next;
        delete conn;
    }

    // Close listening socket
    if (_connFd >= 0) {
        close(_connFd);
    }
}

bool RpiConnection::openSocket() {
    struct addrinfo hints, *servinfo, *p;
    int yes = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = _addressFamily;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, _port.c_str(), &hints, &servinfo) != 0) {
        return false;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        _connFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (_connFd < 0) continue;

        // Allow address reuse
        setsockopt(_connFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

        // Set non-blocking
        int flags = fcntl(_connFd, F_GETFL, 0);
        fcntl(_connFd, F_SETFL, flags | O_NONBLOCK);

        if (bind(_connFd, p->ai_addr, p->ai_addrlen) < 0) {
            close(_connFd);
            _connFd = -1;
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);
    return (_connFd >= 0);
}

WppConnection::SESSION_T RpiConnection::connect(Lwm2mSecurity& security) {
    std::string uri = security.get<STRING_T>(Lwm2mSecurity::LWM2M_SERVER_URI_0);
    std::string host = uriToHost(uri);
    std::string port = uriToPort(uri);

    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = _addressFamily;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo) != 0) {
        return nullptr;
    }

    connection_t* conn = createNewConn(servinfo->ai_addr, servinfo->ai_addrlen);
    freeaddrinfo(servinfo);

    return conn;
}

void RpiConnection::disconnect(SESSION_T session) {
    connection_t* conn = (connection_t*)session;
    if (!conn) return;

    // Remove from linked list
    connection_t** p = &_connections;
    while (*p && *p != conn) {
        p = &((*p)->next);
    }
    if (*p) {
        *p = conn->next;
    }

    delete conn;
}

bool RpiConnection::sessionCmp(SESSION_T session1, SESSION_T session2) {
    if (!session1 || !session2) return false;

    connection_t* conn1 = (connection_t*)session1;
    connection_t* conn2 = (connection_t*)session2;

    if (conn1->addrLen != conn2->addrLen) return false;

    return (memcmp(&conn1->addr, &conn2->addr, conn1->addrLen) == 0);
}

bool RpiConnection::sendPacket(const Packet& packet) {
    if (!packet.session || _connFd < 0) return false;

    connection_t* conn = (connection_t*)packet.session;

    ssize_t sent = sendto(_connFd, packet.buffer, packet.length, 0,
                         (struct sockaddr*)&conn->addr, conn->addrLen);

    return (sent == (ssize_t)packet.length);
}

void RpiConnection::loop() {
    if (_connFd < 0) return;

    uint8_t buffer[4096];
    struct sockaddr_storage addr;
    socklen_t addrLen = sizeof(addr);

    ssize_t numBytes = recvfrom(_connFd, buffer, sizeof(buffer), 0,
                                (struct sockaddr*)&addr, &addrLen);

    if (numBytes > 0) {
        // Find existing connection or create new one
        connection_t* conn = createNewConn((struct sockaddr*)&addr, addrLen);

        // Add packet to queue for processing
        Packet packet;
        packet.session = conn;
        packet.length = numBytes;
        packet.buffer = new uint8_t[numBytes];
        memcpy(packet.buffer, buffer, numBytes);

        addPacketToQueue(packet);

        delete[] packet.buffer; // Queue makes internal copy
    }
}

RpiConnection::connection_t* RpiConnection::createNewConn(struct sockaddr* addr, size_t addrLen) {
    // Search for existing connection
    for (connection_t* conn = _connections; conn != nullptr; conn = conn->next) {
        if (conn->addrLen == addrLen &&
            memcmp(&conn->addr, addr, addrLen) == 0) {
            return conn;
        }
    }

    // Create new connection
    connection_t* conn = new connection_t;
    conn->sock = _connFd;
    conn->addrLen = addrLen;
    memcpy(&conn->addr, addr, addrLen);
    conn->next = _connections;
    _connections = conn;

    return conn;
}

std::string RpiConnection::uriToHost(const std::string& uri) {
    size_t start = uri.find("://");
    if (start == std::string::npos) return uri;
    start += 3;

    size_t end = uri.find(':', start);
    if (end == std::string::npos) end = uri.find('/', start);
    if (end == std::string::npos) end = uri.length();

    return uri.substr(start, end - start);
}

std::string RpiConnection::uriToPort(const std::string& uri) {
    size_t portStart = uri.rfind(':');
    if (portStart == std::string::npos || portStart < uri.find("://")) {
        // Default CoAP ports
        if (uri.find("coaps://") == 0) return "5684";
        return "5683";
    }

    size_t portEnd = uri.find('/', portStart);
    if (portEnd == std::string::npos) portEnd = uri.length();

    return uri.substr(portStart + 1, portEnd - portStart - 1);
}

} // namespace wpp
```

**Complexity**: ⭐⭐ Medium - Network programming

**Note**: This implementation can be directly copied and adapted from `examples/platform/connection/Connection.cpp`

---

### Phase 3: Build System Configuration

#### Step 3.1: Create rpi4/CMakeLists.txt

**File**: `rpi4/CMakeLists.txt`

```cmake
# Raspberry Pi 4 Platform Implementation
cmake_minimum_required(VERSION 3.13)

project(WppRpi4 CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Platform sources
set(RPI4_SOURCES
    platform/RpiPlatform.cpp
    platform/connection/RpiConnection.cpp
)

# Platform includes
set(RPI4_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/connection
)

# Export to parent scope
set(PLATFORM_SOURCES ${RPI4_SOURCES} PARENT_SCOPE)
set(PLATFORM_INCLUDES ${RPI4_INCLUDES} PARENT_SCOPE)

message(STATUS "Raspberry Pi 4 platform configured")
```

---

#### Step 3.2: Modify examples/CMakeLists.txt

Add option for RPi4 platform:

```cmake
# Platform selection
option(PLATFORM_RPI4 "Build for Raspberry Pi 4 ARM64" OFF)

if(PLATFORM_RPI4)
    message(STATUS "Building for Raspberry Pi 4 ARM64")
    add_subdirectory(../rpi4 ${CMAKE_BINARY_DIR}/rpi4)
    set(PLATFORM_SOURCES ${PLATFORM_SOURCES} ${CMAKE_SOURCE_DIR}/../rpi4/platform/RpiPlatform.cpp)
    set(INCLUDES ${INCLUDES} ${CMAKE_SOURCE_DIR}/../rpi4/platform)
else()
    # Default: Linux x86_64
    add_subdirectory(platform)
endif()
```

---

#### Step 3.3: Configure Toolchain (Cross-Compilation)

**File**: `rpi4/toolchain-rpi4.cmake`

```cmake
# Raspberry Pi 4 ARM64 Cross-Compilation Toolchain
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Specify cross-compiler
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Where to look for libraries
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)

# Adjust search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# ARM64-specific compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a+crc -mtune=cortex-a72")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a+crc -mtune=cortex-a72")
```

---

#### Step 3.4: Modify wpp/configs/compiler_config.cmake

Add RPi4 detection:

```cmake
# Detect Raspberry Pi 4
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    message(STATUS "Detected ARM64 architecture (Raspberry Pi 4)")
    # Force 64-bit build
    set(WPP_BUILD_FOR_64_BIT ON CACHE BOOL "Build for 64-bit system" FORCE)
    # Optimize for Cortex-A72
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a+crc -mtune=cortex-a72")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a+crc -mtune=cortex-a72")
endif()
```

---

### Phase 4: Build and Test

#### Step 4.1: Native Build on RPi4

```bash
cd /home/user/Friendly-LwM2M-Client

# Create build directory
mkdir -p build && cd build

# Configure
cmake .. -DPLATFORM_RPI4=ON -DCMAKE_BUILD_TYPE=MinSizeRel

# Build
make -j4

# Result
ls examples/WppExample
```

**Expected Output**:
- `WppExample` - Executable for RPi4 ARM64
- `WppExample.bin` - Binary file
- `WppExample.hex` - Hex file
- `output.map` - Linker map

---

#### Step 4.2: Cross-Compilation Build

```bash
# On development machine (x86_64)
cd /home/user/Friendly-LwM2M-Client
mkdir -p build-rpi4 && cd build-rpi4

# Configure with toolchain
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../rpi4/toolchain-rpi4.cmake \
    -DPLATFORM_RPI4=ON \
    -DCMAKE_BUILD_TYPE=MinSizeRel

# Build
make -j$(nproc)

# Copy to RPi4
scp examples/WppExample pi@rpi4.local:/home/pi/
```

---

#### Step 4.3: Run on RPi4

```bash
# On Raspberry Pi 4
cd /home/pi

# Make executable
chmod +x WppExample

# Run
./WppExample

# Expected output:
# ---- Initialization wpp Security ----
# ---- Initialization wpp Lwm2mServer ----
# ---- Initialization wpp Device ----
# ...
```

---

## Platform-Specific Implementation

### GPIO Integration (Optional)

**File**: `rpi4/platform/RpiGpio.h`

```cpp
/*
 * Raspberry Pi 4 GPIO interface
 * Access GPIO pins via sysfs (/sys/class/gpio)
 */

#ifndef RPI_GPIO_H
#define RPI_GPIO_H

#include <string>
#include <fstream>

namespace rpi {

class Gpio {
public:
    enum Direction { INPUT, OUTPUT };
    enum Value { LOW = 0, HIGH = 1 };

    Gpio(int pin);
    ~Gpio();

    bool setDirection(Direction dir);
    bool setValue(Value val);
    Value getValue();

private:
    int _pin;
    std::string _basePath;

    bool exportGpio();
    bool unexportGpio();
};

} // namespace rpi

#endif
```

**Usage in LwM2M Objects**:

```cpp
// In custom object (e.g., GPIO Control object)
#include "RpiGpio.h"

void GpioControl::executeToggle(Instance& instance, ID_T resId, const OPAQUE_T& args) {
    int pinNum = instance.get<INT_T>(PIN_NUMBER_0);
    rpi::Gpio gpio(pinNum);
    gpio.setDirection(rpi::Gpio::OUTPUT);

    // Toggle
    auto currentValue = gpio.getValue();
    gpio.setValue(currentValue == rpi::Gpio::HIGH ? rpi::Gpio::LOW : rpi::Gpio::HIGH);
}
```

---

### Temperature Monitoring (Optional)

**File**: `rpi4/platform/RpiTemp.h`

```cpp
/*
 * Raspberry Pi 4 temperature monitoring
 * Read from /sys/class/thermal/thermal_zone0/temp
 */

#ifndef RPI_TEMP_H
#define RPI_TEMP_H

#include <fstream>

namespace rpi {

class Temperature {
public:
    /**
     * @brief Get CPU temperature in Celsius
     * @return Temperature in °C (float)
     */
    static float getCpuTemp() {
        std::ifstream tempFile("/sys/class/thermal/thermal_zone0/temp");
        if (!tempFile.is_open()) return 0.0f;

        int milliTemp = 0;
        tempFile >> milliTemp;
        return milliTemp / 1000.0f;  // Convert millidegrees to degrees
    }
};

} // namespace rpi

#endif
```

**Usage in Device Object**:

```cpp
// In Device object serverOperationNotifier or periodic update
#ifdef __aarch64__
#include "RpiTemp.h"

void Device::updateBatteryLevel() {
    // Use CPU temperature as "battery level" proxy
    float temp = rpi::Temperature::getCpuTemp();
    int batteryLevel = std::max(0, std::min(100, (int)((85.0f - temp) * 2)));
    set<INT_T>(BATTERY_LEVEL_9, batteryLevel);
}
#endif
```

---

### WiFi Configuration (Optional)

**File**: `rpi4/platform/RpiWifi.h`

```cpp
/*
 * Raspberry Pi 4 WiFi configuration
 * Uses wpa_supplicant and NetworkManager
 */

#ifndef RPI_WIFI_H
#define RPI_WIFI_H

#include <string>
#include <vector>

namespace rpi {

struct WifiNetwork {
    std::string ssid;
    int signalStrength;  // dBm
    std::string security; // WPA2, WPA3, Open
};

class Wifi {
public:
    /**
     * @brief Scan for available WiFi networks
     * @return Vector of detected networks
     */
    static std::vector<WifiNetwork> scan();

    /**
     * @brief Connect to WiFi network
     * @param ssid Network SSID
     * @param password Network password (empty for open networks)
     * @return true if connection successful
     */
    static bool connect(const std::string& ssid, const std::string& password);

    /**
     * @brief Get current connection status
     * @return SSID of connected network, or empty string if disconnected
     */
    static std::string getCurrentSsid();

    /**
     * @brief Get signal strength of current connection
     * @return Signal strength in dBm, or 0 if not connected
     */
    static int getSignalStrength();
};

} // namespace rpi

#endif
```

**Implementation** uses:
- `iwlist wlan0 scan` - Scan networks
- `wpa_cli` - Control wpa_supplicant
- `nmcli` - NetworkManager CLI (if available)
- Parse `/proc/net/wireless` - Signal strength

---

## Build System Configuration

### CMake Options for RPi4

```cmake
# Configure for Raspberry Pi 4
cmake .. \
    -DPLATFORM_RPI4=ON \                    # Enable RPi4 platform
    -DCMAKE_BUILD_TYPE=MinSizeRel \         # Optimize for size
    -DWPP_BUILD_FOR_64_BIT=ON \             # 64-bit build
    -DWPP_BUILD_WITH_EXCEPTIONS=OFF \       # Disable exceptions (smaller binary)
    -DWPP_BUILD_WITH_RTTI=OFF \             # Disable RTTI (smaller binary)
    -DOPENWRT_BUILD=OFF \                   # Disable OpenWRT features
    -DWITH_UDP_DTLS=ON \                    # Enable DTLS security
    -DUSE_OPENWRT_FW_UPDATER=OFF           # Use example firmware updater
```

### Size Optimization

For constrained RPi4 deployments (e.g., minimal OS), enable aggressive optimization:

```cmake
# Minimal build
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG -flto")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -flto")

# Strip symbols
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--strip-all")

# Disable all optional features
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_VERSION_1_0)  # Only LwM2M 1.0
# Disable unused objects
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_6_LOCATION)
```

**Expected Binary Sizes**:
- **Full build**: 500-700 KB
- **Minimal build**: 300-400 KB
- **RAM usage**: 80-150 KB

---

## Hardware Integration

### Raspberry Pi 4 Hardware Features

| Feature | Interface | Implementation Effort |
|---------|-----------|----------------------|
| **GPIO** | `/sys/class/gpio/` | ⭐ Low (1-2 hours) |
| **Temperature** | `/sys/class/thermal/` | ⭐ Trivial (30 min) |
| **WiFi** | `wpa_supplicant`, `nmcli` | ⭐⭐ Medium (4-6 hours) |
| **Bluetooth** | `bluetoothctl`, D-Bus | ⭐⭐⭐ High (1-2 days) |
| **Camera** | V4L2, `raspistill` | ⭐⭐⭐ High (2-3 days) |
| **HDMI Display** | DRM/KMS | ⭐⭐⭐ High (not recommended) |
| **Ethernet** | Standard networking | ✅ Already supported |
| **USB** | Standard sysfs | ⭐ Low (similar to OpenWRT) |

### Example: GPIO LwM2M Object

**Object ID**: 10525 (Walt Technologies range)

**Purpose**: Control Raspberry Pi 4 GPIO pins via LwM2M

**Resources**:
- 0: Pin Number (R, Integer) - GPIO pin number (0-27)
- 1: Direction (RW, Integer) - 0=Input, 1=Output
- 2: Value (RW, Integer) - 0=Low, 1=High
- 3: Pull Mode (RW, Integer) - 0=None, 1=Pull-Up, 2=Pull-Down
- 4: Execute Set (E) - Set GPIO value
- 5: Execute Toggle (E) - Toggle GPIO value

**Implementation** (`rpi4/objects/RpiGpioControl.h`):

```cpp
class RpiGpioControl : public Instance {
public:
    enum ID: ID_T {
        PIN_NUMBER_0 = 0,
        DIRECTION_1 = 1,
        VALUE_2 = 2,
        PULL_MODE_3 = 3,
        SET_4 = 4,
        TOGGLE_5 = 5,
    };

    RpiGpioControl(lwm2m_context_t& context, const OBJ_LINK_T& id)
        : Instance(context, id) {

        // Initialize resources
        resource(PIN_NUMBER_0)->set<INT_T>(17);  // Default GPIO 17
        resource(DIRECTION_1)->set<INT_T>(0);    // Input
        resource(VALUE_2)->set<INT_T>(0);        // Low

        // Set execute handlers
        resource(SET_4)->setExecuteHandler([this](Instance& inst, ID_T resId, const OPAQUE_T& args) {
            executeSet(inst, resId, args);
            return true;
        });

        resource(TOGGLE_5)->setExecuteHandler([this](Instance& inst, ID_T resId, const OPAQUE_T& args) {
            executeToggle(inst, resId, args);
            return true;
        });
    }

private:
    void executeSet(Instance& inst, ID_T resId, const OPAQUE_T& args);
    void executeToggle(Instance& inst, ID_T resId, const OPAQUE_T& args);
};
```

---

## Testing and Validation

### Test Plan

#### Phase 1: Basic Functionality

**Test Case 1.1: Platform Initialization**

```bash
# Run client and verify startup
./WppExample

# Expected:
# - No compilation errors
# - Client initializes
# - Time function works
# - Logging works
```

**Test Case 1.2: Network Connectivity**

```bash
# Test UDP socket creation
netstat -uln | grep 5683

# Expected:
# udp6       0      0 :::5683                 :::*
```

**Test Case 1.3: LwM2M Server Connection**

```bash
# Configure server in examples/objects.cpp
# Set: string url = "coap://leshan.eclipseprojects.io:5683";

# Run client
./WppExample

# Check Leshan demo server
# https://leshan.eclipseprojects.io
# Should see client registered
```

---

#### Phase 2: Performance Testing

**Test Case 2.1: Memory Usage**

```bash
# Check memory consumption
ps aux | grep WppExample

# Expected:
# VSZ: 20-30 MB (virtual)
# RSS: 5-15 MB (resident)
```

**Test Case 2.2: CPU Usage**

```bash
# Monitor CPU during operation
top -p $(pidof WppExample)

# Expected:
# CPU: <5% during idle
# CPU: 10-30% during active communication
```

**Test Case 2.3: Network Bandwidth**

```bash
# Monitor network traffic
iftop -i eth0

# Expected:
# ~1-5 KB/s during registration/updates
# Minimal traffic during idle (queue mode)
```

---

#### Phase 3: Stress Testing

**Test Case 3.1: Long-Running Stability**

```bash
# Run for 24 hours
./WppExample &

# Monitor for memory leaks
while true; do
    ps aux | grep WppExample | awk '{print $6}'
    sleep 3600  # Check every hour
done

# Expected: No memory growth over time
```

**Test Case 3.2: Network Interruption Recovery**

```bash
# Disconnect network
sudo ifconfig eth0 down

# Wait 60 seconds

# Reconnect network
sudo ifconfig eth0 up

# Expected: Client reconnects automatically
```

**Test Case 3.3: High Frequency Updates**

```bash
# Configure aggressive update intervals
# In objects.cpp: set Lifetime to 60 seconds

# Run client and monitor
./WppExample

# Expected: Regular updates without crashes
```

---

## Known Challenges

### Challenge 1: Cross-Compilation Dependency Hell

**Problem**: Cross-compiling requires ARM64 versions of all libraries (OpenSSL, libcurl, etc.)

**Solution 1** (Recommended): Build natively on RPi4
- Slower compilation
- No dependency issues
- Easier debugging

**Solution 2**: Set up complete cross-compilation environment
```bash
# Install ARM64 libraries
sudo dpkg --add-architecture arm64
sudo apt-get update
sudo apt-get install libssl-dev:arm64 libcurl4-openssl-dev:arm64

# Configure pkg-config
export PKG_CONFIG_PATH=/usr/lib/aarch64-linux-gnu/pkgconfig
```

**Solution 3**: Use Docker with ARM64 emulation (QEMU)
```bash
docker run --platform linux/arm64 -it debian:bullseye
# Build inside ARM64 container
```

---

### Challenge 2: DTLS Performance on ARM

**Problem**: DTLS encryption may be slower on ARM compared to x86_64

**Metrics**:
- RPi4 (ARM Cortex-A72): ~5-10 MB/s AES throughput
- x86_64 (AES-NI): ~500 MB/s+ AES throughput

**Solutions**:
1. **Use ARM Crypto Extensions** (built-in to Cortex-A72):
```cmake
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a+crypto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a+crypto")
```

2. **Use mbedTLS** instead of OpenSSL (lighter, ARM-optimized):
```cmake
option(USE_MBEDTLS "Use mbedTLS instead of OpenSSL" ON)
```

3. **Disable DTLS for testing** (NoSec mode):
```cpp
security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_NONE);
```

---

### Challenge 3: Raspberry Pi OS Systemd Integration

**Problem**: Running client as systemd service requires proper configuration

**Solution**: Create systemd service file

**File**: `/etc/systemd/system/lwm2m-client.service`

```ini
[Unit]
Description=Friendly LwM2M Client
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
User=pi
WorkingDirectory=/home/pi/lwm2m
ExecStart=/home/pi/lwm2m/WppExample
Restart=on-failure
RestartSec=10

# Logging
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

**Enable and start**:
```bash
sudo systemctl daemon-reload
sudo systemctl enable lwm2m-client
sudo systemctl start lwm2m-client

# Check status
sudo systemctl status lwm2m-client

# View logs
sudo journalctl -u lwm2m-client -f
```

---

### Challenge 4: Endianness

**Problem**: ARM can be little-endian or big-endian

**Solution**: RPi4 is **little-endian**, same as x86_64 - no changes needed.

**Verification**:
```bash
lscpu | grep "Byte Order"
# Output: Byte Order:          Little Endian
```

---

## Optimization Opportunities

### 1. Hardware Acceleration

**ARM Crypto Extensions** (ARMv8-A):
- AES encryption/decryption
- SHA-1, SHA-256 hashing
- CRC32 checksum

**Enable in CMake**:
```cmake
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a+crypto+crc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a+crypto+crc")
```

**Expected Performance Improvement**:
- DTLS handshake: 30-50% faster
- Data encryption: 2-3x faster

---

### 2. NEON SIMD Optimizations

**NEON** (ARM SIMD):
- Available on Cortex-A72
- Can accelerate memory operations
- Useful for large data transfers

**Enable**:
```cmake
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon -ftree-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon -ftree-vectorize")
```

---

### 3. Link-Time Optimization (LTO)

**Already enabled** in `compiler_config.cmake`:
```cmake
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
```

**Benefits**:
- 10-20% smaller binary
- 5-10% faster execution
- Longer compilation time

---

### 4. Profile-Guided Optimization (PGO)

**Advanced optimization** technique:

**Step 1**: Build with instrumentation
```bash
cmake .. -DCMAKE_C_FLAGS="-fprofile-generate" -DCMAKE_CXX_FLAGS="-fprofile-generate"
make
```

**Step 2**: Run typical workload
```bash
./WppExample  # Run for ~5 minutes with typical operations
```

**Step 3**: Rebuild with profile data
```bash
cmake .. -DCMAKE_C_FLAGS="-fprofile-use" -DCMAKE_CXX_FLAGS="-fprofile-use"
make
```

**Expected Improvement**: 5-15% performance boost

---

## Summary

### Porting Effort Estimate

| Task | Complexity | Time Estimate |
|------|-----------|---------------|
| Platform abstraction (time, logging) | ⭐ Low | 1-2 hours |
| Network connection (UDP) | ⭐⭐ Medium | 4-8 hours |
| Build system integration | ⭐ Low | 2-4 hours |
| Testing and validation | ⭐⭐ Medium | 4-8 hours |
| **Basic Port Total** | | **11-22 hours (2-3 days)** |
| GPIO integration | ⭐ Low | 2-4 hours |
| Temperature monitoring | ⭐ Trivial | 1 hour |
| WiFi configuration | ⭐⭐ Medium | 4-6 hours |
| Bluetooth integration | ⭐⭐⭐ High | 1-2 days |
| **Full Integration Total** | | **1-2 weeks** |

---

### Recommended Approach

**Phase 1** (Day 1-2): Basic Port
1. Copy platform implementation from `examples/platform/`
2. Modify CMake build system
3. Build natively on RPi4
4. Test basic LwM2M connectivity

**Phase 2** (Day 3-4): Validation
1. Test with public LwM2M servers
2. Validate all operations (Read, Write, Execute, Observe)
3. Performance benchmarking
4. Memory leak testing

**Phase 3** (Week 2): Hardware Integration (Optional)
1. GPIO control object
2. Temperature monitoring
3. WiFi configuration
4. Custom RPi4-specific objects

---

### Success Criteria

✅ **Minimum Viable Port**:
- Compiles for ARM64
- Runs on Raspberry Pi 4
- Connects to LwM2M server
- All core operations work

✅ **Production-Ready Port**:
- Stable 24+ hour operation
- No memory leaks
- Proper error handling
- Systemd integration

✅ **Full Integration**:
- RPi4-specific objects (GPIO, temp, etc.)
- Performance optimized
- Documentation complete
- Example applications

---

## Next Steps

1. **Setup RPi4**: Install Raspberry Pi OS 64-bit
2. **Clone repository**: `git clone --recurse-submodules ...`
3. **Install dependencies**: `sudo apt-get install ...`
4. **Create platform files**: Copy from `examples/platform/`
5. **Configure CMake**: Add RPi4 option
6. **Build**: `cmake .. && make`
7. **Test**: Run against Leshan demo server

---

## Additional Resources

### Documentation
- **Raspberry Pi 4 Hardware**: https://www.raspberrypi.com/products/raspberry-pi-4-model-b/
- **Raspberry Pi OS**: https://www.raspberrypi.com/software/
- **ARM Architecture**: https://developer.arm.com/architectures/cpu-architecture/a-profile
- **GPIO Reference**: https://www.raspberrypi.com/documentation/computers/raspberry-pi.html

### LwM2M Resources
- **Eclipse Leshan**: https://github.com/eclipse-leshan/leshan
- **OMA LwM2M**: https://www.openmobilealliance.org/
- **Friendly Technologies**: https://hubs.li/Q02D_GnZ0

### Build Tools
- **CMake Documentation**: https://cmake.org/cmake/help/latest/
- **GCC ARM Options**: https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
- **Cross-Compilation Guide**: https://www.raspberrypi.com/documentation/computers/linux_kernel.html#cross-compiling-the-kernel

---

**Document Version**: 1.0
**Last Updated**: 2025-11-01
**Author**: Claude AI Assistant
**Target Platform**: Raspberry Pi 4 Model B (ARM64/aarch64)
**Base Branch**: `claude/merge-session-changes-011CUgtnPthsMQjuAoFu1d1J`
