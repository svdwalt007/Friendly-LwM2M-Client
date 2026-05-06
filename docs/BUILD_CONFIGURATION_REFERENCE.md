# Build Configuration Reference

## Matching CMake Output to Options

This document shows how each line in the cmake configuration output maps to cmake options.

---

## Configuration Output → CMake Options

### Build Type

```
Build type: RelWithDebInfo
```
↓
```bash
-DCMAKE_BUILD_TYPE=RelWithDebInfo
```

**Options**:
- `Debug` - No optimization, full debug symbols (-g -O0)
- `Release` - Full optimization, no debug symbols (-O3 -DNDEBUG)
- `RelWithDebInfo` - Optimized with debug symbols (-O2 -g) **← Recommended for testing**
- `MinSizeRel` - Optimize for size (-Os)

---

### Core Features

```
DTLS:             ON
mbedTLS:          ON
TinyDTLS:         OFF
```
↓
```bash
-DWITH_DTLS=ON
-DWITH_MBEDTLS=ON
-DWITH_TINYDTLS=OFF
```

---

### DTLS Multi-Provider (v1.2.2)

```
TinyDTLS Provider: OFF
mbedTLS Provider:  ON (DEFAULT)
OpenSSL Provider:  OFF
wolfSSL Provider:  OFF
Default Provider:  mbedtls
Static Linking:    ON
```
↓
```bash
-DDTLS_PROVIDER_TINYDTLS=OFF
-DDTLS_PROVIDER_MBEDTLS=ON
-DDTLS_PROVIDER_OPENSSL=OFF
-DDTLS_PROVIDER_WOLFSSL=OFF
-DDTLS_DEFAULT_PROVIDER=mbedtls
-DDTLS_STATIC_LINK=ON
```

**Note**: You can enable multiple providers and switch at runtime. Default provider is used when not specified in configuration.

---

### Delta FOTA

```
Delta FOTA:       ON
BSDIFF:           ON
VCDIFF:           ON
Courgette:        ON
Compression:      ON
Rollback:         ON
```
↓
```bash
-DWITH_DELTA_FOTA=ON
-DWITH_BSDIFF=ON
-DWITH_VCDIFF=ON
-DWITH_COURGETTE=ON
-DWITH_COMPRESSION=ON
-DWITH_ROLLBACK=ON
```

**Algorithms**:
- **BSDIFF**: Binary diff using suffix arrays (good for firmware)
- **VCDIFF**: RFC 3284 delta encoding (good for large files)
- **Courgette**: Chrome update algorithm (excellent for executables)

---

### MQTT & Edge AI

```
MQTT Transport:   ON
Edge AI:          ON
TensorFlow Lite:  ON
ONNX Runtime:     ON
```
↓
```bash
-DWITH_MQTT=ON
-DWITH_EDGE_AI=ON
-DWITH_TFLITE=ON
-DWITH_ONNX=ON
```

**Dependencies**:
- MQTT: Requires `libpaho-mqtt-dev`
- TFLite: Requires `libtensorflow-lite-dev`
- ONNX: Requires `libonnxruntime-dev`

---

### v1.2.0 Extended Features - OpenWRT

```
OpenWRT Extended: ON
LUCI Module:      ON
Hotplug Handlers: ON
```
↓
```bash
-DWITH_OPENWRT_EXTENDED=ON
-DWITH_LUCI_MODULE=ON
-DWITH_HOTPLUG_HANDLERS=ON
```

**Platform**: OpenWRT only. Disable for other platforms.

---

### Router Management Objects

```
LAN Config:       ON
Routing Table:    ON
VPN Config:       ON
```
↓
```bash
-DWITH_LAN_CONFIG=ON
-DWITH_ROUTING_TABLE=ON
-DWITH_VPN_CONFIG=ON
```

**Requires**: Wakaama submodule
```bash
git submodule update --init --recursive
```

**LwM2M Objects**:
- **10535**: LAN Configuration
- **10536**: Routing Table
- **10537**: VPN Configuration

---

### Multi-WAN & Failover

```
WAN Failover:     ON
Multi-WAN Health: ON
```
↓
```bash
-DWITH_WAN_FAILOVER=ON
-DWITH_MULTIWAN_HEALTH=ON
```

**Requires**: Wakaama submodule

**LwM2M Objects**:
- **10525**: WAN Failover Policy
- **10526**: Multi-WAN Health Check

---

### WiFi Management

```
WiFi Client Mgmt: ON
WiFi Channel Opt: ON
```
↓
```bash
-DWITH_WIFI_CLIENT_MGMT=ON
-DWITH_WIFI_CHANNEL_OPT=ON
```

**Requires**: Wakaama submodule

**LwM2M Objects**:
- **10527**: WiFi Client Management
- **10528**: WiFi Channel Optimization

---

### Starlink Integration

```
Starlink:         ON
Starlink gRPC:    ON
```
↓
```bash
-DWITH_STARLINK=ON
-DWITH_STARLINK_GRPC=ON
```

**Dependencies**: Requires `libgrpc++-dev` and `protobuf-compiler-grpc`

**Use case**: Monitor and control Starlink dish

---

### Platform Integrations - prpl

```
prpl Integration: ON
prpl EasyMesh:    ON
prpl Ambiorix:    ON
```
↓
```bash
-DWITH_PRPL=ON
-DWITH_PRPL_EASYMESH=ON
-DWITH_PRPL_AMBIORIX=ON
```

**Default**: OFF (enable only for prplOS-based systems)

**Platform**: prplOS/prplWRT only

---

### Platform Integrations - Matter/Thread

```
Matter/Thread:    ON
Matter Bridge:    ON
Thread BR:        ON
```
↓
```bash
-DWITH_MATTER=ON
-DWITH_MATTER_BRIDGE=ON
-DWITH_THREAD_BR=ON
```

**Dependencies**: Requires Matter SDK and Thread libraries

**LwM2M Object**:
- **33450**: Matter Bridge

---

### Platform Integrations - Zigbee

```
Zigbee:           ON
Zigbee EZSP:      ON
Zigbee Z-Stack:   ON
```
↓
```bash
-DWITH_ZIGBEE=ON
-DWITH_ZIGBEE_EZSP=ON
-DWITH_ZIGBEE_ZSTACK=ON
```

**Hardware**:
- **EZSP**: Silicon Labs Zigbee modules
- **Z-Stack**: Texas Instruments Zigbee modules

---

### Build Options

```
Tests:            ON
Examples:         ON
Documentation:    ON
```
↓
```bash
-DBUILD_TESTS=ON
-DBUILD_EXAMPLES=ON
-DBUILD_DOCS=ON
```

**Default**:
- Tests: OFF (enable for development)
- Examples: ON
- Documentation: ON

---

### Debugging & Profiling

```
Coverage:         ON
Sanitizers:       ON
```
↓
```bash
-DENABLE_COVERAGE=ON
-DENABLE_SANITIZERS=ON
```

**WARNING**:
- **Sanitizers**: Add 4-8x performance overhead
- **Coverage**: Requires `gcov`/`lcov` for reports
- Both are for **Debug/RelWithDebInfo** builds only

**Generate coverage report**:
```bash
cd build
make test
gcovr -r .. --html --html-details -o coverage.html
```

---

### Optional Libraries

```
Optional libraries found:
  BZip2:            TRUE
  LZMA:             TRUE
  Zstd:             TRUE
  Brotli:           TRUE
```

These are **detected automatically** by CMake. No options needed.

**Install on Debian/Ubuntu**:
```bash
sudo apt-get install \
  libbz2-dev \
  liblzma-dev \
  libzstd-dev \
  libbrotli-dev
```

These libraries improve compression performance for delta firmware updates.

---

## Complete Command for Your Configuration

### Full Command (All Options)

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DWITH_DTLS=ON \
  -DWITH_MBEDTLS=ON \
  -DWITH_TINYDTLS=OFF \
  -DDTLS_PROVIDER_TINYDTLS=OFF \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_OPENSSL=OFF \
  -DDTLS_PROVIDER_WOLFSSL=OFF \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  -DDTLS_STATIC_LINK=ON \
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
  -DWITH_LUCI_MODULE=ON \
  -DWITH_HOTPLUG_HANDLERS=ON \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  -DWITH_STARLINK=ON \
  -DWITH_STARLINK_GRPC=ON \
  -DWITH_PRPL=ON \
  -DWITH_PRPL_EASYMESH=ON \
  -DWITH_PRPL_AMBIORIX=ON \
  -DWITH_MATTER=ON \
  -DWITH_MATTER_BRIDGE=ON \
  -DWITH_THREAD_BR=ON \
  -DWITH_ZIGBEE=ON \
  -DWITH_ZIGBEE_EZSP=ON \
  -DWITH_ZIGBEE_ZSTACK=ON \
  -DBUILD_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  -DBUILD_DOCS=ON \
  -DENABLE_COVERAGE=ON \
  -DENABLE_SANITIZERS=ON
```

### Minimal Command (Only Non-Defaults)

Since most options are ON by default, you only need to change:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DWITH_TINYDTLS=OFF \
  -DDTLS_PROVIDER_TINYDTLS=OFF \
  -DWITH_PRPL=ON \
  -DWITH_PRPL_EASYMESH=ON \
  -DWITH_PRPL_AMBIORIX=ON \
  -DBUILD_TESTS=ON \
  -DENABLE_COVERAGE=ON \
  -DENABLE_SANITIZERS=ON
```

---

## Using the Build Script

We've created a build script with this exact configuration:

```bash
./scripts/build_full_dev.sh
```

This script:
- ✅ Checks for Wakaama submodule
- ✅ Configures with all options shown above
- ✅ Builds with parallel jobs
- ✅ Reports executable sizes
- ✅ Shows next steps (run tests, coverage, etc.)

---

## Quick Reference: Defaults

### Options ON by Default

You **don't need to specify** these (already ON):
- All DTLS options (except OpenSSL, wolfSSL)
- All Delta FOTA algorithms
- MQTT, Edge AI, TFLite, ONNX
- OpenWRT Extended features
- All router management objects
- All WiFi management objects
- Starlink integration
- Matter/Thread integration
- Zigbee integration
- BUILD_EXAMPLES, BUILD_DOCS

### Options OFF by Default

You **must enable** these:
- `DTLS_PROVIDER_TINYDTLS=OFF` (default ON, disable it)
- `WITH_TINYDTLS=OFF` (default OFF, keep it off)
- `WITH_PRPL=ON` (default OFF)
- `WITH_PRPL_EASYMESH=ON` (default OFF)
- `WITH_PRPL_AMBIORIX=ON` (default OFF)
- `BUILD_TESTS=ON` (default OFF)
- `ENABLE_COVERAGE=ON` (default OFF)
- `ENABLE_SANITIZERS=ON` (default OFF)

---

## Verification

After running cmake, verify your configuration:

```bash
# Show all enabled options
cmake -L | grep "WITH_\|DTLS_\|BUILD_\|ENABLE_"

# Or read the configuration summary
cmake .. | grep "ON\|OFF"

# Or examine cache
cat build/CMakeCache.txt | grep "WITH_\|DTLS_"
```

Expected output should match the configuration shown at the top of this document.

---

## Common Build Type Comparisons

| Build Type | Optimization | Debug Symbols | Use Case |
|------------|--------------|---------------|----------|
| **Debug** | -O0 (none) | -g (full) | Active development, debugging |
| **RelWithDebInfo** | -O2 (good) | -g (full) | **Testing, profiling** ← You want this |
| **Release** | -O3 (max) | None | Production deployment |
| **MinSizeRel** | -Os (size) | None | Embedded, size-constrained |

**RelWithDebInfo** is the best choice for comprehensive testing because:
- ✅ Good performance (optimized)
- ✅ Debuggable (symbols included)
- ✅ Representative of production behavior
- ✅ Works well with sanitizers
- ✅ Works well with coverage tools

---

## Dependency Installation

To build with all features enabled:

```bash
# Debian/Ubuntu
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  git \
  libmbedtls-dev \
  libpaho-mqtt-dev \
  libtensorflow-lite-dev \
  libonnxruntime-dev \
  libgrpc++-dev \
  protobuf-compiler-grpc \
  libbz2-dev \
  liblzma-dev \
  libzstd-dev \
  libbrotli-dev \
  libvcdiff-dev \
  zlib1g-dev \
  libgtest-dev \
  gcovr \
  lcov

# Initialize Wakaama submodule
git submodule update --init --recursive
```

---

## Next Steps After Build

### 1. Run Tests

```bash
cd build
ctest --output-on-failure
```

### 2. Generate Coverage Report

```bash
cd build
make test
gcovr -r .. --html --html-details -o coverage.html
xdg-open coverage.html  # Or open in browser
```

### 3. Run Specific Test

```bash
cd build
./test_delta_algorithms --gtest_filter="BSDiffTest.*"
```

### 4. Check Sanitizer Output

Sanitizers will print warnings/errors if they detect issues:
```bash
cd build
./test_delta_algorithms 2>&1 | tee sanitizer.log
```

### 5. Run Example Client

```bash
cd build
./friendly_lwm2m --help
```

---

## See Also

- **docs/CMAKE_BUILD_OPTIONS.md** - Complete option reference
- **scripts/build_full_dev.sh** - Automated build script
- **CMakeLists.txt** - Source of truth for all options
- **Issues/b11_sanitizer_hang_fix.md** - Sanitizer performance notes
- **Issues/b12_COMPLETE.md** - BSDIFF fix (affects delta FOTA)

---

**Last Updated**: 2026-04-30 (v1.2.2)
