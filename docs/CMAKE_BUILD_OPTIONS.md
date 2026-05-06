# CMake Build Options Reference

## Complete Dev/Test Build Command

For a comprehensive development/test build with all features enabled:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  \
  # DTLS Security Options
  -DWITH_DTLS=ON \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_OPENSSL=ON \
  -DDTLS_PROVIDER_WOLFSSL=ON \
  -DDTLS_STATIC_LINK=ON \
  \
  # Firmware Update Features
  -DWITH_DELTA_FOTA=ON \
  -DWITH_BSDIFF=ON \
  -DWITH_VCDIFF=ON \
  -DWITH_COURGETTE=ON \
  -DWITH_COMPRESSION=ON \
  -DWITH_ROLLBACK=ON \
  \
  # Transport & AI Features
  -DWITH_MQTT=ON \
  -DWITH_EDGE_AI=ON \
  -DWITH_TFLITE=ON \
  -DWITH_ONNX=ON \
  \
  # OpenWRT Integration
  -DWITH_OPENWRT_EXTENDED=ON \
  -DWITH_LUCI_MODULE=ON \
  -DWITH_HOTPLUG_HANDLERS=ON \
  \
  # Router Management Objects (require Wakaama)
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON \
  \
  # Multi-WAN & Failover (require Wakaama)
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  \
  # WiFi Management (require Wakaama)
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  \
  # Starlink Integration
  -DWITH_STARLINK=ON \
  -DWITH_STARLINK_GRPC=ON \
  \
  # prpl Integration (disabled by default)
  -DWITH_PRPL=OFF \
  -DWITH_PRPL_EASYMESH=OFF \
  -DWITH_PRPL_AMBIORIX=OFF \
  \
  # Matter/Thread Integration
  -DWITH_MATTER=ON \
  -DWITH_MATTER_BRIDGE=ON \
  -DWITH_THREAD_BR=ON \
  \
  # Zigbee Integration
  -DWITH_ZIGBEE=ON \
  -DWITH_ZIGBEE_EZSP=ON \
  -DWITH_ZIGBEE_ZSTACK=ON \
  \
  # Build Options
  -DBUILD_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  -DBUILD_DOCS=ON \
  -DBUILD_SHARED_LIBS=OFF \
  \
  # Debugging & Profiling (Debug build only)
  -DENABLE_COVERAGE=OFF \
  -DENABLE_SANITIZERS=ON
```

---

## Quick Reference: Option Categories

### 1. DTLS Security Options

Control which DTLS providers are compiled in:

```bash
-DWITH_DTLS=ON                    # Enable DTLS support
-DDTLS_PROVIDER_TINYDTLS=ON      # Include TinyDTLS (lightweight)
-DDTLS_PROVIDER_MBEDTLS=ON       # Include mbedTLS (recommended)
-DDTLS_PROVIDER_OPENSSL=ON       # Include OpenSSL (full-featured)
-DDTLS_PROVIDER_WOLFSSL=ON       # Include wolfSSL (commercial)
-DDTLS_STATIC_LINK=ON            # Link DTLS libraries statically
```

**Default provider**: Set with `-DDTLS_DEFAULT_PROVIDER=mbedtls` (options: tinydtls, mbedtls, openssl, wolfssl)

**Note**: You can include multiple providers and switch at runtime.

---

### 2. Firmware Update Features

Delta firmware update algorithms and rollback support:

```bash
-DWITH_DELTA_FOTA=ON    # Enable delta firmware updates
-DWITH_BSDIFF=ON        # BSDIFF algorithm (binary diff)
-DWITH_VCDIFF=ON        # VCDIFF algorithm (RFC 3284)
-DWITH_COURGETTE=ON     # Courgette algorithm (Chrome updates)
-DWITH_COMPRESSION=ON   # Compression support (zlib, brotli, zstd)
-DWITH_ROLLBACK=ON      # A/B partition rollback manager
```

**Dependencies**:
- BSDIFF: Uses suffix arrays (fixed in b12)
- VCDIFF: Requires open-vcdiff library
- Courgette: Requires disassembler library
- Compression: Requires zlib, brotli, zstd

---

### 3. Transport & AI Features

MQTT transport and Edge AI inference:

```bash
-DWITH_MQTT=ON       # MQTT transport binding
-DWITH_EDGE_AI=ON    # Edge AI inference support
-DWITH_TFLITE=ON     # TensorFlow Lite backend
-DWITH_ONNX=ON       # ONNX Runtime backend
```

**Dependencies**:
- MQTT: Requires Paho MQTT C library
- TFLite: Requires TensorFlow Lite library
- ONNX: Requires ONNX Runtime library

---

### 4. OpenWRT Integration

Extended OpenWRT platform features:

```bash
-DWITH_OPENWRT_EXTENDED=ON   # Extended OpenWRT integration
-DWITH_LUCI_MODULE=ON         # LuCI web interface module
-DWITH_HOTPLUG_HANDLERS=ON    # Hotplug event handlers
```

**Platform**: Only relevant for OpenWRT builds

---

### 5. Router Management Objects

Requires Wakaama submodule to be initialized:

```bash
-DWITH_LAN_CONFIG=ON        # LAN Configuration object (10535)
-DWITH_ROUTING_TABLE=ON     # Routing Table object (10536)
-DWITH_VPN_CONFIG=ON        # VPN Configuration object (10537)
```

**Setup Wakaama**:
```bash
git submodule update --init --recursive
```

---

### 6. Multi-WAN & Failover

Requires Wakaama submodule:

```bash
-DWITH_WAN_FAILOVER=ON      # WAN Failover Policy object (10525)
-DWITH_MULTIWAN_HEALTH=ON   # Multi-WAN Health Check object (10526)
```

**Use case**: Multi-WAN routers with automatic failover

---

### 7. WiFi Management

Requires Wakaama submodule:

```bash
-DWITH_WIFI_CLIENT_MGMT=ON    # WiFi Client Management object (10527)
-DWITH_WIFI_CHANNEL_OPT=ON    # WiFi Channel Optimization object (10528)
```

**Use case**: WiFi routers and access points

---

### 8. Starlink Integration

Starlink dish monitoring and control:

```bash
-DWITH_STARLINK=ON       # Starlink integration
-DWITH_STARLINK_GRPC=ON  # Starlink gRPC support
```

**Dependencies**: Requires gRPC library for dish communication

---

### 9. prpl Integration

prplOS/prplMesh support (disabled by default):

```bash
-DWITH_PRPL=ON              # prplOS/prplMesh integration
-DWITH_PRPL_EASYMESH=ON     # EasyMesh R2/R4 support
-DWITH_PRPL_AMBIORIX=ON     # Ambiorix bus support
```

**Platform**: Specific to prplOS-based systems

---

### 10. Matter/Thread Integration

Matter smart home protocol with Thread networking:

```bash
-DWITH_MATTER=ON           # Matter/Thread integration
-DWITH_MATTER_BRIDGE=ON    # Matter Bridge object (33450)
-DWITH_THREAD_BR=ON        # Thread Border Router support
```

**Dependencies**: Requires Matter SDK and Thread libraries

---

### 11. Zigbee Integration

Zigbee coordinator and gateway functionality:

```bash
-DWITH_ZIGBEE=ON          # Zigbee integration
-DWITH_ZIGBEE_EZSP=ON     # Silicon Labs EZSP support
-DWITH_ZIGBEE_ZSTACK=ON   # TI Z-Stack support
```

**Hardware**: Requires Zigbee radio module (EZSP or Z-Stack)

---

### 12. Build Options

Control what gets built:

```bash
-DBUILD_TESTS=ON          # Build unit tests (GoogleTest)
-DBUILD_EXAMPLES=ON       # Build example applications
-DBUILD_DOCS=ON           # Build documentation (Doxygen)
-DBUILD_SHARED_LIBS=OFF   # Build as shared libraries
```

**Note**: For development, set `BUILD_TESTS=ON`. For production, set `BUILD_TESTS=OFF`.

---

### 13. Debugging & Profiling

Only use in Debug builds:

```bash
-DENABLE_COVERAGE=ON      # Code coverage (--coverage flag)
-DENABLE_SANITIZERS=ON    # Address/Undefined sanitizers
```

**WARNING**:
- Sanitizers cause 4-8x slowdown
- Only enable for Debug builds
- Can cause test hangs in Release builds
- See Issues/b11_sanitizer_hang_fix.md

---

## Build Type Presets

### Minimal Test Build (Your Original Command)

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  -DBUILD_TESTS=OFF
```

**Missing options** you might want:
- `-DBUILD_TESTS=ON` - Enable tests
- `-DWITH_DELTA_FOTA=ON` - Firmware updates
- `-DWITH_MQTT=ON` - MQTT transport
- `-DWITH_EDGE_AI=ON` - AI inference
- `-DWITH_STARLINK=ON` - Starlink integration
- `-DWITH_MATTER=ON` - Matter/Thread
- `-DWITH_ZIGBEE=ON` - Zigbee support

---

### Full-Featured Dev/Test Build

All features enabled for comprehensive testing:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DWITH_DELTA_FOTA=ON \
  -DWITH_BSDIFF=ON \
  -DWITH_VCDIFF=ON \
  -DWITH_COURGETTE=ON \
  -DWITH_COMPRESSION=ON \
  -DWITH_ROLLBACK=ON \
  -DWITH_MQTT=ON \
  -DWITH_EDGE_AI=ON \
  -DWITH_TFLITE=ON \
  -DWITH_ONNX=ON \
  -DWITH_OPENWRT_EXTENDED=ON \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  -DWITH_STARLINK=ON \
  -DWITH_MATTER=ON \
  -DWITH_MATTER_BRIDGE=ON \
  -DWITH_THREAD_BR=ON \
  -DWITH_ZIGBEE=ON \
  -DWITH_ZIGBEE_EZSP=ON \
  -DWITH_ZIGBEE_ZSTACK=ON \
  -DBUILD_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  -DENABLE_SANITIZERS=ON
```

---

### Production Release Build

Optimized, minimal dependencies:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DWITH_DELTA_FOTA=ON \
  -DWITH_MQTT=ON \
  -DBUILD_TESTS=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_DOCS=OFF \
  -DENABLE_SANITIZERS=OFF
```

Add only the features you actually need in production.

---

### Coverage Build

For code coverage analysis:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DBUILD_TESTS=ON \
  -DENABLE_COVERAGE=ON \
  -DENABLE_SANITIZERS=OFF
```

Then run:
```bash
make -j$(nproc)
make test
gcovr -r .. --html --html-details -o coverage.html
```

---

## Feature Dependencies

Some features require external libraries or submodules:

### Requires Wakaama Submodule

Initialize first:
```bash
git submodule update --init --recursive
```

Required for:
- `WITH_LAN_CONFIG`
- `WITH_ROUTING_TABLE`
- `WITH_VPN_CONFIG`
- `WITH_WAN_FAILOVER`
- `WITH_MULTIWAN_HEALTH`
- `WITH_WIFI_CLIENT_MGMT`
- `WITH_WIFI_CHANNEL_OPT`

### Requires External Libraries

| Feature | Required Library | Package Name (Debian/Ubuntu) |
|---------|------------------|------------------------------|
| WITH_MQTT | Paho MQTT C | libpaho-mqtt-dev |
| WITH_EDGE_AI | TensorFlow Lite | libtensorflow-lite-dev |
| WITH_ONNX | ONNX Runtime | libonnxruntime-dev |
| WITH_STARLINK_GRPC | gRPC | libgrpc++-dev, protobuf-compiler-grpc |
| WITH_VCDIFF | open-vcdiff | libvcdiff-dev |
| WITH_MATTER | Matter SDK | (build from source) |
| WITH_ZIGBEE | Zigbee libraries | (vendor-specific) |

### Install Common Dependencies

```bash
# Debian/Ubuntu
sudo apt-get install \
  libpaho-mqtt-dev \
  libtensorflow-lite-dev \
  libgrpc++-dev \
  protobuf-compiler-grpc \
  libvcdiff-dev \
  zlib1g-dev \
  libbrotli-dev \
  libzstd-dev
```

---

## Default Values

Options enabled by default (ON):

**Security**:
- WITH_DTLS
- DTLS_PROVIDER_TINYDTLS
- DTLS_PROVIDER_MBEDTLS
- DTLS_STATIC_LINK

**Firmware Updates**:
- WITH_DELTA_FOTA
- WITH_BSDIFF
- WITH_VCDIFF
- WITH_COURGETTE
- WITH_COMPRESSION
- WITH_ROLLBACK

**Transports & AI**:
- WITH_MQTT
- WITH_EDGE_AI
- WITH_TFLITE
- WITH_ONNX

**OpenWRT**:
- WITH_OPENWRT_EXTENDED
- WITH_LUCI_MODULE
- WITH_HOTPLUG_HANDLERS

**Router Management**:
- WITH_LAN_CONFIG
- WITH_ROUTING_TABLE
- WITH_VPN_CONFIG
- WITH_WAN_FAILOVER
- WITH_MULTIWAN_HEALTH
- WITH_WIFI_CLIENT_MGMT
- WITH_WIFI_CHANNEL_OPT

**Starlink**:
- WITH_STARLINK
- WITH_STARLINK_GRPC

**Matter**:
- WITH_MATTER
- WITH_MATTER_BRIDGE
- WITH_THREAD_BR

**Zigbee**:
- WITH_ZIGBEE
- WITH_ZIGBEE_EZSP
- WITH_ZIGBEE_ZSTACK

**Build**:
- BUILD_EXAMPLES
- BUILD_DOCS

Options disabled by default (OFF):

**Security**:
- DTLS_PROVIDER_OPENSSL (use mbedTLS instead)
- DTLS_PROVIDER_WOLFSSL (commercial)

**prpl**:
- WITH_PRPL
- WITH_PRPL_EASYMESH
- WITH_PRPL_AMBIORIX

**Build**:
- BUILD_TESTS (enable for development)
- BUILD_SHARED_LIBS (use static)

**Debug**:
- ENABLE_COVERAGE
- ENABLE_SANITIZERS

---

## Checking Current Configuration

After running cmake, check what's enabled:

```bash
# View all cache variables
cmake -L

# View with descriptions
cmake -LA

# View with advanced options
cmake -LAH

# Search for specific options
cmake -L | grep WITH_
```

Or examine CMakeCache.txt:
```bash
grep "WITH_" build/CMakeCache.txt
```

---

## Common Scenarios

### I Want to Test Everything

Use the full-featured dev/test build above, or:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTS=ON \
  -DENABLE_SANITIZERS=ON
```

Most features are ON by default, so this enables nearly everything.

### I Want Just LwM2M + DTLS

Minimal build:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DWITH_DELTA_FOTA=OFF \
  -DWITH_MQTT=OFF \
  -DWITH_EDGE_AI=OFF \
  -DWITH_STARLINK=OFF \
  -DWITH_MATTER=OFF \
  -DWITH_ZIGBEE=OFF \
  -DBUILD_TESTS=OFF
```

### I Want Router Management Only

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  -DWITH_EDGE_AI=OFF \
  -DWITH_STARLINK=OFF \
  -DWITH_MATTER=OFF \
  -DWITH_ZIGBEE=OFF
```

Don't forget to initialize Wakaama submodule first.

### I Want Smart Home (Matter + Zigbee)

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DWITH_MATTER=ON \
  -DWITH_MATTER_BRIDGE=ON \
  -DWITH_THREAD_BR=ON \
  -DWITH_ZIGBEE=ON \
  -DWITH_ZIGBEE_EZSP=ON \
  -DWITH_ZIGBEE_ZSTACK=ON \
  -DWITH_OPENWRT_EXTENDED=OFF
```

---

## Build Environment Presets

The project uses `cmake/BuildEnvironment.cmake` for preset configurations.

You can set the environment type:

```bash
-DBUILD_ENVIRONMENT=TEST      # Full debugging, all features
-DBUILD_ENVIRONMENT=DEV       # Development with sanitizers
-DBUILD_ENVIRONMENT=PREPROD   # Pre-production testing
-DBUILD_ENVIRONMENT=PROD      # Production optimized
```

This sets sensible defaults for each environment. You can still override individual options.

---

## Tips

1. **Start with defaults**: Most features are ON by default, so you can just run `cmake ..` and it works.

2. **Disable what you don't need**: It's easier to turn OFF unused features than turn ON everything.

3. **Debug vs Release**: Always use Debug for development/testing, Release for production.

4. **Sanitizers only in Debug**: ENABLE_SANITIZERS causes 4-8x slowdown and test hangs in Release.

5. **Check dependencies**: Many features require external libraries. Install them first or disable the feature.

6. **Wakaama submodule**: Initialize if using router management objects: `git submodule update --init --recursive`

7. **Clean rebuild**: After changing many options, clean rebuild:
   ```bash
   rm -rf build/*
   cd build
   cmake ..
   make -j$(nproc)
   ```

8. **Check configuration**: After cmake, review what's enabled with `cmake -LA`

---

## Option Summary Table

| Option | Default | Purpose | Dependencies |
|--------|---------|---------|--------------|
| **DTLS Security** |
| WITH_DTLS | ON | Enable DTLS support | mbedTLS/OpenSSL |
| DTLS_PROVIDER_TINYDTLS | ON | Include TinyDTLS provider | TinyDTLS library |
| DTLS_PROVIDER_MBEDTLS | ON | Include mbedTLS provider | mbedTLS library |
| DTLS_PROVIDER_OPENSSL | OFF | Include OpenSSL provider | OpenSSL library |
| DTLS_PROVIDER_WOLFSSL | OFF | Include wolfSSL provider | wolfSSL library |
| DTLS_STATIC_LINK | ON | Link DTLS statically | - |
| **Firmware Updates** |
| WITH_DELTA_FOTA | ON | Delta firmware updates | - |
| WITH_BSDIFF | ON | BSDIFF algorithm | - |
| WITH_VCDIFF | ON | VCDIFF algorithm | open-vcdiff |
| WITH_COURGETTE | ON | Courgette algorithm | disassembler |
| WITH_COMPRESSION | ON | Compression support | zlib, brotli, zstd |
| WITH_ROLLBACK | ON | A/B partition rollback | - |
| **Transport & AI** |
| WITH_MQTT | ON | MQTT transport | Paho MQTT C |
| WITH_EDGE_AI | ON | Edge AI inference | - |
| WITH_TFLITE | ON | TensorFlow Lite | TFLite library |
| WITH_ONNX | ON | ONNX Runtime | ONNX library |
| **OpenWRT** |
| WITH_OPENWRT_EXTENDED | ON | Extended OpenWRT features | - |
| WITH_LUCI_MODULE | ON | LuCI web interface | LuCI |
| WITH_HOTPLUG_HANDLERS | ON | Hotplug event handlers | - |
| **Router Management** (requires Wakaama) |
| WITH_LAN_CONFIG | ON | LAN Configuration (10535) | Wakaama |
| WITH_ROUTING_TABLE | ON | Routing Table (10536) | Wakaama |
| WITH_VPN_CONFIG | ON | VPN Configuration (10537) | Wakaama |
| WITH_WAN_FAILOVER | ON | WAN Failover (10525) | Wakaama |
| WITH_MULTIWAN_HEALTH | ON | Multi-WAN Health (10526) | Wakaama |
| WITH_WIFI_CLIENT_MGMT | ON | WiFi Client Mgmt (10527) | Wakaama |
| WITH_WIFI_CHANNEL_OPT | ON | WiFi Channel Opt (10528) | Wakaama |
| **Starlink** |
| WITH_STARLINK | ON | Starlink integration | - |
| WITH_STARLINK_GRPC | ON | Starlink gRPC support | gRPC, protobuf |
| **prpl** |
| WITH_PRPL | OFF | prplOS/prplMesh | prpl SDK |
| WITH_PRPL_EASYMESH | OFF | EasyMesh R2/R4 | prpl SDK |
| WITH_PRPL_AMBIORIX | OFF | Ambiorix bus | prpl SDK |
| **Matter/Thread** |
| WITH_MATTER | ON | Matter/Thread integration | Matter SDK |
| WITH_MATTER_BRIDGE | ON | Matter Bridge (33450) | Matter SDK |
| WITH_THREAD_BR | ON | Thread Border Router | Thread SDK |
| **Zigbee** |
| WITH_ZIGBEE | ON | Zigbee integration | - |
| WITH_ZIGBEE_EZSP | ON | Silicon Labs EZSP | EZSP library |
| WITH_ZIGBEE_ZSTACK | ON | TI Z-Stack | Z-Stack library |
| **Build Options** |
| BUILD_TESTS | OFF | Build unit tests | GoogleTest |
| BUILD_EXAMPLES | ON | Build examples | - |
| BUILD_DOCS | ON | Build documentation | Doxygen |
| BUILD_SHARED_LIBS | OFF | Build shared libraries | - |
| ENABLE_COVERAGE | OFF | Code coverage | gcov/lcov |
| ENABLE_SANITIZERS | OFF | Address/UB sanitizers | - |

---

**See also**:
- CMakeLists.txt - Complete option definitions
- cmake/BuildEnvironment.cmake - Environment presets
- Issues/b11_sanitizer_hang_fix.md - Sanitizer performance impact
- Issues/b12_COMPLETE.md - BSDIFF suffix array fix
