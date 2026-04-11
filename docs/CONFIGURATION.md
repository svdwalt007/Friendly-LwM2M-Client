# Configuration Guide

Complete guide to configuring the Friendly LwM2M Client v1.2.2.

**Version:** 1.2.2 | **Last Updated:** January 2026

---

## Table of Contents

1. [CLI Configuration](#cli-configuration)
2. [CMake Build Options](#v122-cmake-build-options)
3. [Build Configuration](#build-configuration)
4. [Resource Configuration](#resource-configuration)
5. [Runtime Configuration](#runtime-configuration)
6. [Advanced Configuration](#advanced-configuration)
7. [Security Hardening](#security-hardening)
8. [Quick Reference](#quick-reference)

---

## CLI Configuration

The Friendly LwM2M Client supports comprehensive command-line configuration, allowing runtime override of compile-time settings without rebuilding the application. This enables flexible deployment scenarios and easier testing.

### Overview

CLI configuration provides:
- **Runtime Flexibility**: Override compile-time defaults without rebuilding
- **Security Configuration**: Configure DTLS/TLS modes (PSK, RPK, Certificate)
- **Network Settings**: Customize ports, addressing, and server URIs
- **Validation**: Built-in validation with helpful error messages
- **Backward Compatibility**: All settings optional with sensible defaults

**Implementation:** The CLI parser is defined in `examples/cli_options.h` and integrated into `examples/main.cpp`.

### Complete CLI Options Reference

#### Core LwM2M Options

| Option | Short | Type | Default | Description |
|--------|-------|------|---------|-------------|
| `--name` | `-n` | STRING | `walttech888` | LwM2M endpoint identifier |
| `--uri` | `-u` | STRING | `coap://demo-iot.friendly-tech.com:5680` | LwM2M server URI (coap:// or coaps://) |
| `--port` | `-p` | STRING | `56830` | Local UDP port to bind |
| `--lifetime` | `-l` | INTEGER | `25` | Registration lifetime in seconds (must be positive) |
| `--ipv4` | `-4` | FLAG | ✓ | Use IPv4 addressing (default) |
| `--ipv6` | `-6` | FLAG | - | Use IPv6 addressing |
| `--bootstrap` | `-b` | FLAG | ✓ | Enable LwM2M bootstrap mode (default) |
| `--no-bootstrap` | `-B` | FLAG | - | Disable LwM2M bootstrap mode |

#### Security Options

| Option | Short | Type | Default | Description |
|--------|-------|------|---------|-------------|
| `--security` | `-s` | ENUM | `none` | Security mode: `none`\|`psk`\|`rpk`\|`cert` |
| `--psk-identity` | `-i` | STRING | - | PSK identity string (required for PSK mode) |
| `--psk-key` | `-k` | HEX | - | PSK key as hex string (required for PSK mode) |
| `--rpk-public` | - | HEX | - | RPK public key as hex string (required for RPK mode) |
| `--rpk-private` | - | HEX | - | RPK private key as hex string (required for RPK mode) |
| `--cert` | - | PATH | - | Certificate file path (required for cert mode) |
| `--key` | - | PATH | - | Private key file path (required for cert mode) |
| `--ca` | - | PATH | - | CA certificate file path (optional for cert mode) |

#### CoAP Protocol Options

| Option | Short | Type | Default | Description |
|--------|-------|------|---------|-------------|
| `--block-size` | - | INTEGER | `1024` | CoAP block size in bytes (16-1024, must be power of 2) |

#### Application Options

| Option | Short | Type | Default | Description |
|--------|-------|------|---------|-------------|
| `--verbose` | `-v` | FLAG | - | Increase verbosity (can be repeated: `-vvv`) |
| `--quiet` | `-q` | FLAG | - | Suppress non-error output |
| `--daemon` | `-d` | FLAG | - | Run as background daemon |
| `--config` | `-c` | PATH | - | Load configuration from file (future feature) |

#### Information Options

| Option | Short | Type | Description |
|--------|-------|------|-------------|
| `--help` | `-h` | FLAG | Display help message and exit |
| `--version` | `-V` | FLAG | Display version information and exit |

### CLI Override Behavior

CLI options override compile-time settings in the following priority order (highest to lowest):

1. **Command-line arguments** (highest priority)
2. **Environment variables** (if implemented)
3. **Configuration file** (`--config`, future feature)
4. **Compile-time defaults** in `wpp_config.cmake` (lowest priority)

**Example Override Flow:**

```cpp
// Compile-time default in wpp_config.cmake
#define LWM2M_COAP_DEFAULT_BLOCK_SIZE 1024

// Runtime CLI override
$ ./lwm2m_client --block-size=512
// Result: Uses 512 bytes (CLI wins)
```

**Important Notes:**
- CLI options are parsed at application startup (in `main()`)
- Changes require restarting the application
- Security credentials in CLI are visible in process list (use config file for production)
- The `CliOptions` struct is passed to initialization functions (`serverInit()`, `securityInit()`)

### Security Mode Configuration

The `--security` option configures DTLS/TLS security. Each mode has specific requirements:

#### Security Mode: none (No Security)

**Value:** `none` (default)
**LwM2M Code:** `3`
**Transport:** CoAP (no encryption)

```bash
# Basic connection with no security
./lwm2m_client -n mydevice -u coap://server.example.com:5683 -s none

# With bootstrap disabled
./lwm2m_client -n mydevice -u coap://server.example.com:5683 -s none -B
```

**Use Cases:**
- Development and testing
- Isolated private networks
- Performance benchmarking

**Security Notes:**
- ⚠️ **NOT recommended for production**
- All data transmitted in clear text
- No authentication or integrity protection

#### Security Mode: psk (Pre-Shared Key)

**Value:** `psk`
**LwM2M Code:** `0`
**Transport:** CoAPS (DTLS with PSK)

**Required Options:**
- `--psk-identity`: Identity string (e.g., device ID)
- `--psk-key`: Shared secret as hex string

```bash
# PSK authentication with 128-bit key
./lwm2m_client \
  -n mydevice \
  -u coaps://server.example.com:5684 \
  -s psk \
  -i mydevice-001 \
  -k 00112233445566778899aabbccddeeff

# PSK with 256-bit key
./lwm2m_client \
  -s psk \
  -i device-12345 \
  -k 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
```

**Key Format:**
- Hex string (0-9, a-f, A-F)
- Even number of characters
- Common lengths: 16 chars (64-bit), 32 chars (128-bit), 64 chars (256-bit)

**Validation:**
```cpp
// Automatic validation in parseCliOptions()
- Checks hex string format
- Requires both identity and key for PSK mode
- Warns if using coap:// with PSK (should be coaps://)
```

**Use Cases:**
- IoT devices with pre-provisioned credentials
- Constrained devices (low memory/CPU)
- Scenarios where PKI infrastructure is unavailable

#### Security Mode: rpk (Raw Public Key)

**Value:** `rpk`
**LwM2M Code:** `1`
**Transport:** CoAPS (DTLS with RPK)

**Required Options:**
- `--rpk-public`: Public key as hex string
- `--rpk-private`: Private key as hex string

```bash
# RPK authentication with ECC keys
./lwm2m_client \
  -n mydevice \
  -u coaps://server.example.com:5684 \
  -s rpk \
  --rpk-public=<public-key-hex> \
  --rpk-private=<private-key-hex>
```

**Use Cases:**
- Devices with public key crypto support
- Scenarios requiring stronger authentication than PSK
- When certificate infrastructure is too complex

**Future Enhancement:**
- Support loading keys from files
- Support for different key formats (PEM, DER)

#### Security Mode: cert (X.509 Certificate)

**Value:** `cert`
**LwM2M Code:** `2`
**Transport:** CoAPS (DTLS with X.509)

**Required Options:**
- `--cert`: Path to device certificate file
- `--key`: Path to private key file
- `--ca`: Path to CA certificate file (optional)

```bash
# Certificate-based authentication
./lwm2m_client \
  -n mydevice \
  -u coaps://server.example.com:5684 \
  -s cert \
  --cert=/etc/lwm2m/device.crt \
  --key=/etc/lwm2m/device.key \
  --ca=/etc/lwm2m/ca.crt

# Without CA (self-signed or pinned certificate)
./lwm2m_client \
  -s cert \
  --cert=/etc/lwm2m/device.crt \
  --key=/etc/lwm2m/device.key
```

**Certificate Requirements:**
- Certificates must be in PEM or DER format
- Private key must match certificate
- Certificate must be valid (not expired)
- Subject CN should match endpoint name

**Use Cases:**
- Enterprise deployments with PKI
- Highly secure environments
- Compliance requirements (e.g., regulatory)

### Validation Rules and Error Handling

The CLI parser includes comprehensive validation:

#### Input Validation

```cpp
// Block size validation
--block-size=512   ✓ Valid (power of 2, 16-1024)
--block-size=1000  ✗ Error: Must be power of 2
--block-size=2048  ✗ Error: Must be ≤ 1024
--block-size=8     ✗ Error: Must be ≥ 16

// Lifetime validation
--lifetime=300     ✓ Valid (positive integer)
--lifetime=0       ✗ Error: Must be positive
--lifetime=-1      ✗ Error: Must be positive

// Hex string validation
--psk-key=00112233         ✓ Valid (even length, hex chars)
--psk-key=0011223          ✗ Error: Must have even length
--psk-key=00112233gg       ✗ Error: Invalid hex character 'g'
```

#### Security Mode Validation

```cpp
// PSK mode validation
-s psk -i mydevice -k 00112233...   ✓ Valid
-s psk -i mydevice                  ✗ Error: PSK mode requires --psk-key
-s psk -k 00112233...               ✗ Error: PSK mode requires --psk-identity

// RPK mode validation
-s rpk --rpk-public=... --rpk-private=...   ✓ Valid
-s rpk --rpk-public=...                     ✗ Error: RPK mode requires both keys

// Certificate mode validation
-s cert --cert=device.crt --key=device.key  ✓ Valid
-s cert --cert=device.crt                   ✗ Error: Cert mode requires --key
```

#### URI and Security Consistency

The parser validates URI scheme matches security mode:

```bash
# Consistent configurations
coap://server:5683  + security=none    ✓ OK
coaps://server:5684 + security=psk     ✓ OK

# Potential mismatches (warnings, not errors)
coaps://server:5684 + security=none    ⚠ Warning: Using coaps:// with 'none'
coap://server:5683  + security=psk     ⚠ Warning: Using coap:// with DTLS
```

**Note:** Warnings allow advanced configurations but alert users to potential issues.

#### Error Reporting

All errors provide clear, actionable messages:

```bash
$ ./lwm2m_client -s psk -i mydevice
Error: PSK mode requires --psk-key
Try './lwm2m_client --help' for more information.

$ ./lwm2m_client --block-size=1000
Error: Block size must be a power of 2 between 16 and 1024

$ ./lwm2m_client unknown-arg
Error: Unexpected argument: unknown-arg
Try './lwm2m_client --help' for more information.
```

### Backward Compatibility

The CLI implementation maintains full backward compatibility:

#### Default Behavior

**Without CLI options** (original behavior):
```bash
# This still works exactly as before
./lwm2m_client
```

Uses compile-time defaults:
- Endpoint name: `walttech888`
- Server URI: `coap://demo-iot.friendly-tech.com:5680`
- Local port: `56830`
- Lifetime: `25` seconds
- Security: `none`
- Bootstrap: `enabled`

#### Compile-Time Defaults

All CLI options have defaults matching original hardcoded values in `examples/main.cpp`:

```cpp
// Original hardcoded values (still used as defaults)
CliOptions()
    : endpoint_name("walttech888")           // From main.cpp
    , server_uri("coap://demo-iot.friendly-tech.com:5680")
    , local_port("56830")
    , lifetime(25)
    , use_bootstrap(true)
    , security_mode(SecurityMode::NONE)
    , block_size(1024)                       // From LWM2M_COAP_DEFAULT_BLOCK_SIZE
    // ... other defaults
```

#### Migration Path

Existing applications can migrate incrementally:

**Phase 1: Use defaults**
```bash
./lwm2m_client  # Unchanged behavior
```

**Phase 2: Override specific settings**
```bash
./lwm2m_client -n production-device-001  # Change only endpoint
```

**Phase 3: Full CLI configuration**
```bash
./lwm2m_client -n device -u coaps://prod.example.com:5684 -s psk -i device -k 00112233...
```

#### API Compatibility

Object initialization functions support both old and new signatures:

```cpp
// Old API (still supported via default parameters)
void securityInit(WppClient &client) {
    // Uses compile-time defaults
}

// New API (CLI-aware)
void securityInit(WppClient &client, const CliOptions& options) {
    // Uses CLI options
}
```

**Implementation Detail:** The actual code uses function overloading or default parameters to maintain compatibility.

### Usage Examples

#### Basic Usage Examples

```bash
# 1. Default configuration (backward compatible)
./lwm2m_client

# 2. Custom endpoint name only
./lwm2m_client -n my-custom-device

# 3. Different server
./lwm2m_client -u coap://192.168.1.100:5683

# 4. IPv6 connection
./lwm2m_client -6 -u coap://[2001:db8::1]:5683

# 5. Custom local port
./lwm2m_client -p 12345

# 6. Disable bootstrap
./lwm2m_client -B

# 7. Extended lifetime
./lwm2m_client -l 3600  # 1 hour
```

#### Security Configuration Examples

```bash
# 8. PSK mode with 128-bit key
./lwm2m_client \
  -n secure-device \
  -u coaps://server.example.com:5684 \
  -s psk \
  -i device-12345 \
  -k 0123456789abcdef0123456789abcdef

# 9. PSK mode with verbose output
./lwm2m_client \
  -vvv \
  -s psk \
  -i mydevice \
  -k 00112233445566778899aabbccddeeff \
  -u coaps://192.168.1.100:5684

# 10. Certificate mode
./lwm2m_client \
  -n production-device-001 \
  -u coaps://prod-server.example.com:5684 \
  -s cert \
  --cert=/etc/lwm2m/certs/device.pem \
  --key=/etc/lwm2m/private/device-key.pem \
  --ca=/etc/lwm2m/certs/ca-bundle.pem

# 11. RPK mode
./lwm2m_client \
  -s rpk \
  --rpk-public=<public-key-hex> \
  --rpk-private=<private-key-hex>
```

#### Advanced Configuration Examples

```bash
# 12. Custom CoAP block size
./lwm2m_client --block-size=512

# 13. Quiet mode (minimal output)
./lwm2m_client -q -n device

# 14. Maximum verbosity
./lwm2m_client -vvv

# 15. Combined options
./lwm2m_client \
  -n iot-gateway-001 \
  -u coaps://server.example.com:5684 \
  -p 56830 \
  -l 300 \
  -6 \
  -s psk \
  -i gateway-001 \
  -k 0123456789abcdef0123456789abcdef \
  --block-size=512 \
  -vv

# 16. Development/testing with verbose logs
./lwm2m_client \
  -n test-device \
  -u coap://localhost:5683 \
  -B \
  -vvv

# 17. Production deployment
./lwm2m_client \
  -n prod-device-${DEVICE_ID} \
  -u coaps://prod.iot.example.com:5684 \
  -s psk \
  -i ${DEVICE_ID} \
  -k ${PSK_KEY} \
  -l 3600 \
  -q \
  -d
```

#### Script Integration Examples

```bash
# 18. Environment-driven configuration
#!/bin/bash
export DEVICE_NAME="sensor-$(hostname)"
export SERVER_URI="coaps://iot-hub.example.com:5684"
export PSK_ID="${DEVICE_NAME}"
export PSK_KEY="$(cat /etc/lwm2m/psk.key)"

./lwm2m_client \
  -n "${DEVICE_NAME}" \
  -u "${SERVER_URI}" \
  -s psk \
  -i "${PSK_ID}" \
  -k "${PSK_KEY}" \
  -d

# 19. Systemd service integration
[Service]
ExecStart=/usr/local/bin/lwm2m_client \
  -n %H \
  -u coaps://iot.example.com:5684 \
  -s psk \
  -i %H \
  -k $(cat /etc/lwm2m/device.key) \
  -d

# 20. Docker container
docker run -d \
  --name lwm2m-client \
  lwm2m-client:latest \
  -n "container-${HOSTNAME}" \
  -u "coaps://${IOT_SERVER}:5684" \
  -s psk \
  -i "${DEVICE_ID}" \
  -k "${PSK_KEY}"
```

### Getting Help

#### Help Command

```bash
$ ./lwm2m_client --help
Usage: lwm2m_client [OPTIONS]

Friendly LwM2M Client - Enhanced IoT device management client

Core Options:
  -n, --name=NAME              Endpoint name (default: walttech888)
  -u, --uri=URI                Server URI (default: coap://demo-iot.friendly-tech.com:5680)
  -p, --port=PORT              Local port (default: 56830)
  -l, --lifetime=SECONDS       Registration lifetime (default: 25)
  -4, --ipv4                   Use IPv4 (default)
  -6, --ipv6                   Use IPv6
  -b, --bootstrap              Enable bootstrap mode (default: true)
  -B, --no-bootstrap           Disable bootstrap mode

Security Options:
  -s, --security=MODE          Security mode: none|psk|rpk|cert (default: none)
  -i, --psk-identity=ID        PSK identity string
  -k, --psk-key=KEY            PSK key (hex string, e.g., 00112233...)
      --rpk-public=KEY         RPK public key (hex string)
      --rpk-private=KEY        RPK private key (hex string)
      --cert=FILE              Certificate file path
      --key=FILE               Private key file path
      --ca=FILE                CA certificate file path

CoAP Options:
      --block-size=SIZE        Block size in bytes (16-1024, power of 2, default: 1024)

Application Options:
  -v, --verbose                Increase verbosity (can be used multiple times)
  -q, --quiet                  Suppress non-error output
  -d, --daemon                 Run as daemon
  -c, --config=FILE            Load configuration from file

Information:
  -h, --help                   Show this help message
  -V, --version                Show version information

Examples:
  # Basic connection with no security
  lwm2m_client -n mydevice -u coap://server.example.com:5683

  # PSK security mode
  lwm2m_client -s psk -i mydevice -k 00112233445566778899aabbccddeeff

  # IPv6 with custom port
  lwm2m_client -6 -p 12345 -u coap://[::1]:5683

  # Verbose debug output
  lwm2m_client -vvv
```

#### Version Information

```bash
$ ./lwm2m_client --version
Friendly LwM2M Client v1.2.2
Enhanced LwM2M Client with OpenWRT integration
Copyright (c) 2024 Walt Technologies
Built with C++17, supports LwM2M 1.1 specification
```

### Troubleshooting

#### Common Issues and Solutions

**Issue: PSK connection fails**
```bash
# Check both identity and key are provided
./lwm2m_client -s psk -i device -k 00112233...

# Verify URI uses coaps:// for DTLS
./lwm2m_client -s psk ... -u coaps://server:5684
```

**Issue: Invalid block size error**
```bash
# Must be power of 2 between 16-1024
--block-size=512   ✓
--block-size=1000  ✗
```

**Issue: Verbosity not working**
```bash
# Use multiple -v flags for higher verbosity
./lwm2m_client -vvv  # Maximum verbosity
```

**Issue: Certificate files not found**
```bash
# Use absolute paths
--cert=/etc/lwm2m/device.crt ✓
--cert=device.crt            ✗ (relative path may fail)
```

---

## v1.2.2 CMake Build Options

### New Options in v1.2.2

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
# Build with all v1.2.2 features
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
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10519_HARDWARE_WATCHDOG)
# ---------- Optional objects config block end ----------

# ---------- Walt Technologies objects config block begin ----------
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10512_STARLINK_TERMINAL)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10513_ROUTER_MANAGEMENT)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10514_ETHERNET_INTERFACE)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10515_GPIO_CONTROL)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10516_USB_MANAGEMENT)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10517_STORAGE_MANAGEMENT)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10518_SYSTEM_MONITOR)
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
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10512_STARLINK_TERMINAL)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_W_10513_ROUTER_MANAGEMENT)
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
| Starlink | OBJ_O_10512_STARLINK_TERMINAL | ON |

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
- [CLI Options Header](../examples/cli_options.h) - Technical implementation details
- [CLI Example](../examples/cli_usage_example.cpp) - Integration example
