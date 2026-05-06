# Building with Optional Features

This document explains how to enable optional features in the Friendly LwM2M Client.

## Two Build Systems

This project has **two separate build systems**:

### 1. WPP Example Application (Simple)
**Location:** `examples/CMakeLists.txt`
**What you've been building:** The basic WppExample executable
**Features:** Basic LwM2M client with core WPP objects

### 2. Full Library with Advanced Features (Complete)
**Location:** Root `CMakeLists.txt`
**What it builds:** `libfriendly_lwm2m` library with all optional features
**Features:** Starlink, MQTT, Edge AI, OpenWRT, Matter, Zigbee, etc.

---

## Currently Working Build: WPP Example

You're currently building from the **examples** directory, which provides a simple LwM2M client.

### Enabling More WPP Objects

To enable additional LwM2M objects in the WPP example, edit:
**File:** `wpp/configs/wpp_config.cmake`

#### Available Objects (Currently Disabled)

```cmake
# Optional objects already working
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_4_CONNECTIVITY_MONITORING)  # Currently enabled
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_6_LOCATION)                  # Currently enabled
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_2_LWM2M_ACCESS_CONTROL)      # Currently enabled
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_3339_AUDIO_CLIP)             # Currently enabled
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_5_FIRMWARE_UPDATE)           # Currently enabled
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_3331_MEMORY)                 # Currently enabled

# DISABLED objects (incompatible with current WPP framework)
# These were auto-generated and need API updates to work:
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_12_WLAN_CONNECTIVITY)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_13_BEARER_SELECTION)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10512_STARLINK_TERMINAL)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10519_HARDWARE_WATCHDOG)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10520_MIKROBUS)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10513_ROUTER_MANAGEMENT)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10514_ETHERNET_INTERFACE)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10515_GPIO_CONTROL)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10516_USB_MANAGEMENT)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10517_STORAGE_MANAGEMENT)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10518_SYSTEM_MONITOR)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10521_FIREWALL_CONFIG)
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10522_POE_MANAGEMENT)
```

**Why disabled?** These objects use an older API that's incompatible with the current WPP framework. They would need code updates to work.

### Enabling OpenWRT Firmware Updater

The WPP example supports choosing between two firmware updaters:

**Edit:** `examples/CMakeLists.txt` or pass CMake option

```bash
# Option 1: Edit CMakeLists.txt (line 83)
option(USE_OPENWRT_FW_UPDATER "Use OpenWRT firmware updater with sysupgrade integration" ON)

# Option 2: Pass to CMake
cd _build
cmake ../examples -DUSE_OPENWRT_FW_UPDATER=ON
make -j$(nproc)
```

**Note:** This requires the OpenWRT firmware updater implementation to exist in `examples/OpenWrtFirmwareUpdater/`.

---

## Building the Full Library with All Features

To access **Starlink, MQTT, Edge AI, and other advanced features**, you need to build from the **root directory** instead of the examples directory.

### Quick Start: Build Everything

```bash
# Clean previous build
rm -rf build
mkdir build
cd build

# Configure with all features enabled (defaults)
cmake ..

# Build
make -j$(nproc)

# Run tests
make test
```

### Default Enabled Features

By default (without any options), these features are **ON**:

```
Core Features:
✓ DTLS support
✓ mbedTLS (vs OpenSSL)
✓ Delta firmware updates (BSDIFF, VCDIFF, Courgette)
✓ Compression support
✓ A/B partition rollback
✓ MQTT transport binding
✓ Edge AI inference
✓ Unit tests
✓ Example applications
✓ Documentation
✓ Code coverage
✓ Address/undefined sanitizers

v1.2.0 Extended Features:
✓ OpenWRT extended integration
✓ LUCI web interface module
✓ Hotplug event handlers
✓ Starlink integration
✓ Starlink gRPC support

Disabled by Default:
✗ TinyDTLS (use mbedTLS instead)
✗ TensorFlow Lite backend
✗ ONNX Runtime backend
✗ Router Management Objects (10535-10537)
✗ Multi-WAN objects (10525-10526)
✗ WiFi Management objects (10527-10528)
✗ prpl Integration
✗ Matter/Thread integration
✗ Zigbee integration
```

### Customizing Features with CMake Options

You can enable/disable features by passing CMake options:

```bash
cmake .. \
  -DWITH_STARLINK=ON \
  -DWITH_STARLINK_GRPC=ON \
  -DWITH_MQTT=ON \
  -DWITH_EDGE_AI=ON \
  -DWITH_OPENWRT_EXTENDED=ON \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON
```

### Available Feature Flags

#### Core Features
```cmake
-DWITH_DTLS=ON|OFF               # Enable DTLS support
-DWITH_MBEDTLS=ON|OFF            # Use mbedTLS (vs OpenSSL)
-DWITH_TINYDTLS=ON|OFF           # Use TinyDTLS for constrained devices
-DWITH_DELTA_FOTA=ON|OFF         # Enable delta firmware updates
-DWITH_BSDIFF=ON|OFF             # Enable BSDIFF algorithm
-DWITH_VCDIFF=ON|OFF             # Enable VCDIFF algorithm
-DWITH_COURGETTE=ON|OFF          # Enable Courgette algorithm
-DWITH_COMPRESSION=ON|OFF        # Enable compression support
-DWITH_ROLLBACK=ON|OFF           # Enable A/B partition rollback
-DWITH_MQTT=ON|OFF               # Enable MQTT transport binding
-DWITH_EDGE_AI=ON|OFF            # Enable Edge AI inference
-DWITH_TFLITE=ON|OFF             # Enable TensorFlow Lite backend
-DWITH_ONNX=ON|OFF               # Enable ONNX Runtime backend
```

#### OpenWRT Features
```cmake
-DWITH_OPENWRT_EXTENDED=ON|OFF   # Extended OpenWRT integration
-DWITH_LUCI_MODULE=ON|OFF        # Build LuCI web interface module
-DWITH_HOTPLUG_HANDLERS=ON|OFF   # Install hotplug event handlers
```

#### Router Management Objects
```cmake
-DWITH_LAN_CONFIG=ON|OFF         # LAN Configuration object (10535)
-DWITH_ROUTING_TABLE=ON|OFF      # Routing Table object (10536)
-DWITH_VPN_CONFIG=ON|OFF         # VPN Configuration object (10537)
```

#### Multi-WAN and Failover
```cmake
-DWITH_WAN_FAILOVER=ON|OFF       # WAN Failover Policy object (10525)
-DWITH_MULTIWAN_HEALTH=ON|OFF    # Multi-WAN Health Check object (10526)
```

#### WiFi Management
```cmake
-DWITH_WIFI_CLIENT_MGMT=ON|OFF   # WiFi Client Management object (10527)
-DWITH_WIFI_CHANNEL_OPT=ON|OFF   # WiFi Channel Optimization object (10528)
```

#### Starlink Integration
```cmake
-DWITH_STARLINK=ON|OFF           # Enable Starlink integration
-DWITH_STARLINK_GRPC=ON|OFF      # Enable Starlink gRPC support
```

**Note:** Starlink features require:
- gRPC library installed
- Starlink Dishy terminal accessible on the network

#### prpl Integration
```cmake
-DWITH_PRPL=ON|OFF               # Enable prplOS/prplMesh integration
-DWITH_PRPL_EASYMESH=ON|OFF      # Enable EasyMesh R2/R4 support
-DWITH_PRPL_AMBIORIX=ON|OFF      # Enable Ambiorix bus support
```

#### Matter Integration
```cmake
-DWITH_MATTER=ON|OFF             # Enable Matter/Thread integration
-DWITH_MATTER_BRIDGE=ON|OFF      # Enable Matter Bridge object (33450)
-DWITH_THREAD_BR=ON|OFF          # Enable Thread Border Router support
```

#### Zigbee Integration
```cmake
-DWITH_ZIGBEE=ON|OFF             # Enable Zigbee integration
-DWITH_ZIGBEE_EZSP=ON|OFF        # Enable Silicon Labs EZSP support
-DWITH_ZIGBEE_ZSTACK=ON|OFF      # Enable TI Z-Stack support
```

#### Build Options
```cmake
-DBUILD_TESTS=ON|OFF             # Build unit tests
-DBUILD_EXAMPLES=ON|OFF          # Build example applications
-DBUILD_DOCS=ON|OFF              # Build documentation
-DBUILD_SHARED_LIBS=ON|OFF       # Build shared libraries
-DENABLE_COVERAGE=ON|OFF         # Enable code coverage
-DENABLE_SANITIZERS=ON|OFF       # Enable address/undefined sanitizers
```

---

## Example: Building with Starlink Support

```bash
# Install required dependencies
sudo apt-get install -y \
  libgrpc++-dev \
  protobuf-compiler-grpc \
  libprotobuf-dev

# Clean build directory
rm -rf build
mkdir build
cd build

# Configure with Starlink
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DWITH_STARLINK=ON \
  -DWITH_STARLINK_GRPC=ON \
  -DBUILD_TESTS=ON

# Build
make -j$(nproc)

# Run tests
make test
```

---

## Example: Building for OpenWRT Router

```bash
# Configure for OpenWRT with router management
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DWITH_OPENWRT_EXTENDED=ON \
  -DWITH_LUCI_MODULE=ON \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  -DBUILD_TESTS=OFF \
  -DENABLE_SANITIZERS=OFF

# Build
make -j$(nproc)
```

---

## Troubleshooting

### Missing Dependencies

If you enable a feature but the required library is missing, CMake will:
1. **Warn you** during configuration
2. **Continue** with a stub implementation (for MQTT)
3. **Fail** if the dependency is critical

Check the CMake output for messages like:
```
-- MQTT Transport:   ON
-- Could NOT find PAHO_MQTT (missing: PAHO_MQTT_LIB)
-- MQTT enabled but libraries not found - using stub implementation
```

### Installing Optional Dependencies

```bash
# MQTT (Paho)
sudo apt-get install libpaho-mqtt-dev libpaho-mqttpp-dev

# TensorFlow Lite
sudo apt-get install libtensorflowlite-dev

# Compression libraries
sudo apt-get install libbz2-dev liblzma-dev libzstd-dev libbrotli-dev

# gRPC (for Starlink)
sudo apt-get install libgrpc++-dev protobuf-compiler-grpc

# mbedTLS
sudo apt-get install libmbedtls-dev

# Partition management
sudo apt-get install libblkid-dev
```

---

## Difference Summary

### WPP Example Build (examples/)
- **Simple:** Basic LwM2M client
- **Fast:** Compiles quickly
- **Limited:** Only core WPP objects
- **Good for:** Learning, testing, basic deployments

### Full Library Build (root/)
- **Complete:** All features available
- **Advanced:** Starlink, MQTT, Edge AI, OpenWRT
- **Flexible:** Enable only what you need
- **Good for:** Production deployments, embedded systems

---

## Recommended Approach

1. **Start with WPP Example** (what you've been doing)
   - Get familiar with basic LwM2M operations
   - Test with LwM2M server
   - Simple and works now

2. **Move to Full Build** when you need:
   - Starlink integration
   - MQTT transport
   - Edge AI inference
   - Advanced router management
   - Multi-WAN failover
   - OpenWRT integration

3. **Build incrementally:**
   ```bash
   # Start minimal
   cmake .. -DWITH_STARLINK=OFF -DWITH_MQTT=OFF -DBUILD_TESTS=OFF

   # Add features one at a time
   cmake .. -DWITH_STARLINK=ON

   # Once working, enable more
   cmake .. -DWITH_STARLINK=ON -DWITH_MQTT=ON
   ```

---

## Current Status

✅ **WPP Example builds successfully**
✅ **All compilation fixes applied**
✅ **Ready to use basic LwM2M client**

Next steps:
- Test the WPP example with an LwM2M server
- When ready, switch to full build for advanced features
- Enable specific features based on your requirements
