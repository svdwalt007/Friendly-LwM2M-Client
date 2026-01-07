# Configuration Guide

Complete guide to configuring the Friendly LwM2M Client v1.1.0.

**Version:** 1.1.0 | **Last Updated:** January 2026

---

## v1.1.0 CMake Build Options

### New Options in v1.1.0

```cmake
# MQTT Transport Binding
option(WITH_MQTT "Enable MQTT transport binding" ON)

# Edge AI Inference
option(WITH_EDGE_AI "Enable Edge AI inference" ON)
option(WITH_TFLITE "Enable TensorFlow Lite backend" OFF)
option(WITH_ONNX "Enable ONNX Runtime backend" OFF)

# Delta Firmware Updates (already available)
option(WITH_DELTA_FOTA "Enable delta firmware updates" ON)
option(WITH_BSDIFF "Enable BSDIFF algorithm" ON)
option(WITH_VCDIFF "Enable VCDIFF algorithm" ON)
option(WITH_COURGETTE "Enable Courgette algorithm" ON)
option(WITH_ROLLBACK "Enable A/B partition rollback" ON)
```

### Example Build Commands

```bash
# Build with all v1.1.0 features
cmake -DWITH_MQTT=ON -DWITH_EDGE_AI=ON -DWITH_DELTA_FOTA=ON ..

# Build with MQTT and TensorFlow Lite
cmake -DWITH_MQTT=ON -DWITH_EDGE_AI=ON -DWITH_TFLITE=ON ..

# Build with ONNX Runtime
cmake -DWITH_MQTT=ON -DWITH_EDGE_AI=ON -DWITH_ONNX=ON ..

# Minimal build (no new features)
cmake -DWITH_MQTT=OFF -DWITH_EDGE_AI=OFF -DWITH_DELTA_FOTA=OFF ..
```

---

## Build Configuration

### wpp_config.cmake

**Location:** `wpp/configs/wpp_config.cmake`

This is the main configuration file for enabling/disabling LwM2M objects and features.

### Object Configuration

```cmake
####################################################################
############## Mandatory and Optional object configs ###############
####################################################################

# ---------- Mandatory objects config block begin ----------
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_3_DEVICE)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_1_LWM2M_SERVER)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_0_LWM2M_SECURITY)
# ---------- Mandatory objects config block end ----------

# ---------- Optional objects config block begin ----------
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_4_CONNECTIVITY_MONITORING)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_6_LOCATION)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_5_FIRMWARE_UPDATE)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_34607_HARDWARE_WATCHDOG)
# ---------- Optional objects config block end ----------

# ---------- Walt Technologies objects config block begin ----------
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_34600_STARLINK_TERMINAL)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_34601_ROUTER_MANAGEMENT)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_34602_ETHERNET_INTERFACE)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_34603_GPIO_CONTROL)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_34604_USB_MANAGEMENT)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_34605_STORAGE_MANAGEMENT)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_34606_SYSTEM_MONITOR)
# ---------- Walt Technologies objects config block end ----------
```

### Wakaama LwM2M Stack Configuration

```cmake
####################################################################
######################### Wakaama configs ##########################
####################################################################

# Client mode (required)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_CLIENT_MODE)

# Bootstrap support
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_BOOTSTRAP)

# Content format support
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_CBOR)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_SENML_CBOR)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_SENML_JSON)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_JSON)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_TLV)

# CoAP block size (16, 32, 64, 128, 256, 512, or 1024)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_COAP_DEFAULT_BLOCK_SIZE=1024)

# Logging
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_WITH_LOGS)
```

### WPP Library Configuration

```cmake
####################################################################
########################### Wpp configs ############################
####################################################################

# Enable logs
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_ENABLE_LOGS)

# Log level (0=Debug, 1=Info, 2=Warning, 3=Error)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=0)
```

---

## Resource Configuration

### Per-Object Resource Configuration

Each object has a `<ObjectName>Config.h` file that controls optional resources.

**Example: LocationConfig.h**

```cpp
#ifndef WPP_LOCATION_CONFIG_H
#define WPP_LOCATION_CONFIG_H

#ifdef OBJ_O_6_LOCATION

// Mandatory resources are always enabled
// RES_6_0 - Latitude (always enabled)
// RES_6_1 - Longitude (always enabled)
// RES_6_5 - Timestamp (always enabled)

// Optional resources
#define RES_6_2 1  // Altitude (Float, meters)
#define RES_6_3 1  // Radius (Float, meters - uncertainty)
#define RES_6_4 0  // Velocity (Opaque - 3GPP format)
#define RES_6_6 1  // Speed (Float, m/s)

#endif
#endif
```

**Example: DeviceConfig.h**

```cpp
#define RES_3_0 1   // Manufacturer
#define RES_3_1 1   // Model Number
#define RES_3_2 1   // Serial Number
#define RES_3_3 1   // Firmware Version
#define RES_3_5 0   // Factory Reset (disabled for safety)
#define RES_3_6 0   // Available Power Sources
#define RES_3_10 0  // Memory Free
#define RES_3_13 1  // Current Time
```

---

## Runtime Configuration

### Server Configuration

**File:** `examples/objects.cpp` - `serverInit()`

```cpp
void serverInit(WppClient &client) {
    Instance *server = Lwm2mServer::createInst(client);

    // Server ID
    server->set<INT_T>(Lwm2mServer::SHORT_SERVER_ID_0, 123);

    // Lifetime (seconds)
    server->set<INT_T>(Lwm2mServer::LIFETIME_1, 300);

    // Minimum observation period (seconds)
    server->set<INT_T>(Lwm2mServer::MIN_PERIOD_2, 10);

    // Maximum observation period (seconds)
    server->set<INT_T>(Lwm2mServer::MAX_PERIOD_3, 60);

    // Notification storing when disabled/offline
    server->set<BOOL_T>(Lwm2mServer::NOTIFICATION_STORING_7, true);

    // Binding mode
    server->set<STRING_T>(Lwm2mServer::BINDING_7, "U");  // UDP
}
```

### Security Configuration

**File:** `examples/objects.cpp` - `securityInit()`

```cpp
void securityInit(WppClient &client) {
    Instance *security = Lwm2mSecurity::createInst(client);

    // Server URI
    security->set<STRING_T>(Lwm2mSecurity::SERVER_URI_0,
                            "coap://server.example.com:5683");

    // Bootstrap server flag
    security->set<BOOL_T>(Lwm2mSecurity::BOOTSTRAP_SERVER_1, false);

    // Security mode
    // 0 = PSK, 1 = Raw Public Key, 2 = Certificate, 3 = NoSec
    security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, 3);

    // Public Key or Identity (for PSK)
    if (securityMode == 0) {  // PSK
        security->set<OPAQUE_T>(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3,
                                "device-identity");
    }

    // Secret Key (for PSK)
    if (securityMode == 0) {  // PSK
        security->set<OPAQUE_T>(Lwm2mSecurity::SECRET_KEY_5,
                                "secret-key-here");
    }

    // Short Server ID (must match server config)
    security->set<INT_T>(Lwm2mSecurity::SHORT_SERVER_ID_10, 123);

    // Client Hold Off Time
    security->set<INT_T>(Lwm2mSecurity::CLIENT_HOLD_OFF_TIME_11, 0);
}
```

---

## CMake Build Options

### Standard Options

```bash
# Build type
cmake -DCMAKE_BUILD_TYPE=Debug ..      # Debug build
cmake -DCMAKE_BUILD_TYPE=Release ..    # Release (optimized)

# OpenWRT build
cmake -DOPENWRT_BUILD=ON ..

# Custom install prefix
cmake -DCMAKE_INSTALL_PREFIX=/opt/lwm2m ..

# Verbose output
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
```

### Toolchain Configuration

```bash
# Cross-compilation
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-openwrt.cmake ..

# Custom compiler
cmake -DCMAKE_C_COMPILER=gcc-12 -DCMAKE_CXX_COMPILER=g++-12 ..
```

---

## Environment Variables

### Development

```bash
# Staging directory for OpenWRT SDK
export STAGING_DIR=/path/to/openwrt-sdk/staging_dir

# Add toolchain to PATH
export PATH=$STAGING_DIR/toolchain-*/bin:$PATH

# Library path
export LD_LIBRARY_PATH=/path/to/libs:$LD_LIBRARY_PATH
```

### Runtime

```bash
# Log level override
export WPP_LOG_LEVEL=0  # 0=Debug, 1=Info, 2=Warning, 3=Error

# Configuration file
export LWM2M_CONFIG=/etc/lwm2m/config.ini
```

---

## Configuration Profiles

### Minimal Profile (Development)

```cmake
# Minimal object set for testing
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_3_DEVICE)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_1_LWM2M_SERVER)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_0_LWM2M_SECURITY)
# No optional objects

# Debug logging
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=0)
```

### Standard Profile (Production)

```cmake
# Standard object set
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_3_DEVICE)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_1_LWM2M_SERVER)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_0_LWM2M_SECURITY)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_4_CONNECTIVITY_MONITORING)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_6_LOCATION)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_5_FIRMWARE_UPDATE)

# Warning logging
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=2)
```

### Full Profile (All Features)

```cmake
# All objects enabled
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_3_DEVICE)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_1_LWM2M_SERVER)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_0_LWM2M_SECURITY)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_4_CONNECTIVITY_MONITORING)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_6_LOCATION)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_5_FIRMWARE_UPDATE)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_34600_STARLINK_TERMINAL)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_34601_ROUTER_MANAGEMENT)
# ... all Walt Tech objects

# Info logging
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=1)
```

---

## Advanced Configuration

### Custom Update Intervals

**Location updates:**
```cpp
// In Location.cpp - resourcesInit()
_locationUpdateTaskId = WppTaskQueue::addTask(30, [this](...) {
    updateLocation();
    return false;
});  // Update every 30 seconds (default: 60)
```

**Device memory updates:**
```cpp
// In Device.cpp - resourcesInit()
_currentTimeTaskId = WppTaskQueue::addTask(5, [this](...) {
    resource(CURRENT_TIME_13)->set<TIME_T>(std::time(nullptr));
    resource(MEMORY_FREE_10)->set<INT_T>(OpenWrtDeviceInfo::getMemoryFree());
    notifyResChanged(MEMORY_FREE_10);
    return false;
});  // Update every 5 seconds (default: 1)
```

### Custom Notification Behavior

```cpp
// Enable automatic notifications
client.setNotificationMode(WppClient::AUTO_NOTIFY);

// Manual notification control
resource(RESOURCE_ID)->set<TYPE>(value);
notifyResChanged(RESOURCE_ID);  // Explicit notify

// Batch notifications
std::vector<ID_T> resources = {RESOURCE_1, RESOURCE_2, RESOURCE_3};
notifyResChanged(resources);
```

---

## Security Hardening

### 1. Disable Unused Objects

```cmake
# Comment out or remove unused objects
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_3339_AUDIO_CLIP)
```

### 2. Use Secure Communication

```cpp
// Use CoAPS (DTLS) instead of CoAP
security->set<STRING_T>(Lwm2mSecurity::SERVER_URI_0,
                        "coaps://server.example.com:5684");

// Use PSK mode
security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, 0);  // PSK
security->set<OPAQUE_T>(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3, "device-id");
security->set<OPAQUE_T>(Lwm2mSecurity::SECRET_KEY_5, generateSecureKey());
```

### 3. Disable Execute Resources

```cpp
// In DeviceConfig.h
#define RES_3_4 1   // Reboot (keep enabled)
#define RES_3_5 0   // Factory Reset (DISABLE for security)
#define RES_3_12 0  // Reset Error Code (DISABLE for security)
```

---

## Quick Reference

### Enable/Disable Objects

| Object | Configuration Define | Default |
|--------|---------------------|---------|
| Device | OBJ_M_3_DEVICE | ON |
| Server | OBJ_M_1_LWM2M_SERVER | ON |
| Security | OBJ_M_0_LWM2M_SECURITY | ON |
| Connectivity | OBJ_O_4_CONNECTIVITY_MONITORING | ON |
| Location | OBJ_O_6_LOCATION | ON |
| Firmware Update | OBJ_O_5_FIRMWARE_UPDATE | ON |
| Starlink | OBJ_O_34600_STARLINK_TERMINAL | ON |

### Log Levels

| Level | Value | Usage |
|-------|-------|-------|
| Debug | 0 | Development, verbose output |
| Info | 1 | Production, normal operation |
| Warning | 2 | Production, important messages only |
| Error | 3 | Production, errors only |

---

## See Also

- [Quick Start Guide](QUICKSTART.md) - Getting started
- [Implementation Guide](IMPLEMENTATION_GUIDE.md) - Advanced development
- [API Reference](API_REFERENCE.md) - Code documentation
