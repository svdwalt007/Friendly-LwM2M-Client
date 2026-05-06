# Build Environment Configuration Guide

## Overview

The Friendly LwM2M Client build system supports **four distinct build environments**, each optimized for different stages of development and deployment:

1. **TEST** - Unit testing with maximum debugging
2. **DEV** - Development builds with moderate optimization
3. **PREPROD** - Pre-production validation builds
4. **PROD** - Production builds with maximum optimization

Additionally, the build system implements **optimized DTLS library linking** to minimize binary size by including only the symbols actually used.

---

## Build Environments

### 1. TEST Environment

**Purpose**: Comprehensive unit testing, integration testing, code coverage analysis

**Configuration**:
```bash
cmake -B build -DBUILD_ENV=TEST
cmake --build build
```

**Features**:
- ✅ All tests enabled (unit, integration, E2E)
- ✅ Code coverage enabled (--coverage, gcov)
- ✅ Address/undefined sanitizers enabled
- ✅ Debug symbols (g -O0)
- ✅ Verbose logging (TRACE level)
- ✅ All optional features enabled for testing
- ✅ All DTLS providers enabled
- ✅ Static linking for reproducibility

**Compiler Flags**:
```
-g -O0 -DDEBUG -D_DEBUG
--coverage -fprofile-arcs -ftest-coverage
-fsanitize=address,undefined
-Wall -Wextra -Wpedantic
```

**When to Use**:
- Running unit tests (`ctest`)
- Generating code coverage reports
- Debugging test failures
- CI/CD automated testing

---

### 2. DEV Environment (Default)

**Purpose**: Day-to-day development with fast iteration

**Configuration**:
```bash
cmake -B build -DBUILD_ENV=DEV
cmake --build build
```

**Features**:
- ✅ Debug symbols with moderate optimization (g -O2)
- ✅ Sanitizers enabled for catching bugs
- ✅ Tests enabled
- ✅ Examples enabled
- ✅ Standard logging (DEBUG level)
- ✅ Core features enabled, experimental disabled
- ✅ Single DTLS provider (mbedTLS by default)

**Compiler Flags**:
```
-g -O2 -DNDEBUG
-fsanitize=address,undefined
-Wall -Wextra -Wpedantic
```

**When to Use**:
- Daily development
- Debugging application logic
- Quick iterations
- Local testing

---

### 3. PREPROD Environment

**Purpose**: Pre-production validation, performance testing, profiling

**Configuration**:
```bash
cmake -B build -DBUILD_ENV=PREPROD
cmake --build build
```

**Features**:
- ✅ Full optimization (-O3)
- ✅ Minimal debug symbols (g1) for profiling
- ✅ No sanitizers (production-like performance)
- ✅ Production logging (WARN level)
- ✅ Link-time optimization (LTO)
- ✅ Static linking
- ✅ Examples enabled for validation

**Compiler Flags**:
```
-O3 -DNDEBUG -g1
-ffunction-sections -fdata-sections
-flto
```

**Linker Flags**:
```
-Wl,--gc-sections
-Wl,--as-needed
-Wl,-O1
```

**When to Use**:
- Performance testing
- Profiling (perf, gprof)
- Load testing
- Staging environment validation
- Pre-deployment verification

---

### 4. PROD Environment

**Purpose**: Production deployment with minimal binary size and maximum performance

**Configuration**:
```bash
cmake -B build -DBUILD_ENV=PROD
cmake --build build
```

**Features**:
- ✅ Size optimization (-Os)
- ✅ No debug symbols (stripped)
- ✅ No tests, examples, docs
- ✅ Minimal logging (ERROR level only)
- ✅ Aggressive optimization (LTO, ICF, gc-sections)
- ✅ Static linking
- ✅ Function/data section optimization

**Compiler Flags**:
```
-Os -DNDEBUG -s
-ffunction-sections -fdata-sections
-flto
```

**Linker Flags**:
```
-Wl,--gc-sections
-Wl,--as-needed
-Wl,--strip-all
-Wl,--icf=all
-Wl,--compress-debug-sections=zlib
-static-libgcc -static-libstdc++
```

**Binary Size Reduction**:
- Typical reduction: 40-60% vs DEV build
- mbedTLS footprint: 77 KB → 45 KB (unused ciphers removed)
- TinyDTLS footprint: 22 KB → 18 KB

**When to Use**:
- Production firmware releases
- Embedded devices with limited flash
- NB-IoT/LTE-M devices (bandwidth-constrained OTA)
- Final distribution binaries

---

## DTLS Library Optimization

### Supported DTLS Providers

| Provider | Footprint | Performance | Use Case |
|----------|-----------|-------------|----------|
| **mbedTLS** | 77 KB → 45 KB (PROD) | Good | General purpose, default |
| **TinyDTLS** | 22 KB → 18 KB (PROD) | Fast | Memory-constrained MCUs |
| **OpenSSL** | 350 KB → 280 KB (PROD) | Excellent | Linux, high-performance |
| **wolfSSL** | 120 KB → 95 KB (PROD) | Excellent | FIPS, automotive, medical |

### Optimization Techniques

1. **Symbol Garbage Collection** (`--gc-sections`)
   - Removes unused functions and data
   - Requires `-ffunction-sections -fdata-sections`
   - Typical savings: 20-30%

2. **As-Needed Linking** (`--as-needed`)
   - Only links libraries actually used
   - Removes unnecessary dependencies
   - Typical savings: 10-15%

3. **Identical Code Folding** (`--icf=all`, PROD only)
   - Merges identical functions
   - Typical savings: 5-10%

4. **Static Linking** (configurable)
   - Embeds libraries in binary
   - Enables aggressive LTO
   - Typical savings: 15-25% (with LTO)

### Selecting DTLS Providers

**Enable specific providers**:
```bash
# mbedTLS only (default, smallest for production)
cmake -B build -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DDTLS_PROVIDER_TINYDTLS=OFF \
    -DDTLS_PROVIDER_OPENSSL=OFF \
    -DDTLS_PROVIDER_WOLFSSL=OFF

# TinyDTLS only (smallest footprint, 18 KB)
cmake -B build -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_MBEDTLS=OFF \
    -DDTLS_PROVIDER_TINYDTLS=ON \
    -DDTLS_PROVIDER_OPENSSL=OFF \
    -DDTLS_PROVIDER_WOLFSSL=OFF

# Multiple providers (TEST environment)
cmake -B build -DBUILD_ENV=TEST \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DDTLS_PROVIDER_TINYDTLS=ON \
    -DDTLS_PROVIDER_OPENSSL=ON

# wolfSSL for FIPS compliance
cmake -B build -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_WOLFSSL=ON \
    -DDTLS_PROVIDER_MBEDTLS=OFF
```

**Set default provider**:
```bash
cmake -B build -DDTLS_DEFAULT_PROVIDER=tinydtls
```

**Static vs Dynamic Linking**:
```bash
# Static (default for PROD, recommended)
cmake -B build -DBUILD_ENV=PROD -DDTLS_STATIC_LINK=ON

# Dynamic (faster iteration in DEV)
cmake -B build -DBUILD_ENV=DEV -DDTLS_STATIC_LINK=OFF
```

---

## Feature Modules

### Core Features (always enabled)
- LwM2M v1.0 (100% compliance)
- Mandatory objects (0, 1, 3)
- CoAP/UDP transport

### Optional Features (configurable)

**Delta FOTA**:
```bash
-DWITH_DELTA_FOTA=ON     # Enable delta firmware updates
-DWITH_BSDIFF=ON          # BSDIFF algorithm
-DWITH_VCDIFF=ON          # VCDIFF algorithm (RFC 3284)
-DWITH_COURGETTE=ON       # Courgette algorithm (best for ARM/x86)
-DWITH_COMPRESSION=ON     # Enable compression libs
-DWITH_ROLLBACK=ON        # A/B partition rollback
```

**Edge AI/ML**:
```bash
-DWITH_EDGE_AI=ON         # Enable Edge AI object (33410)
-DWITH_TFLITE=ON          # TensorFlow Lite backend
-DWITH_ONNX=ON            # ONNX Runtime backend
```

**Connectivity**:
```bash
-DWITH_MQTT=ON            # MQTT transport binding
-DWITH_STARLINK=ON        # Starlink terminal integration
```

**Gateway Features**:
```bash
-DWITH_ZIGBEE=ON          # Zigbee gateway
-DWITH_MATTER=ON          # Matter/Thread gateway
-DWITH_PRPL=ON            # prplOS integration
```

**OpenWRT/prplOS Features**:
```bash
-DWITH_OPENWRT_EXTENDED=ON    # Extended OpenWRT integration
-DWITH_LAN_CONFIG=ON          # LAN Configuration object (10535)
-DWITH_ROUTING_TABLE=ON       # Routing Table object (10536)
-DWITH_VPN_CONFIG=ON          # VPN Configuration object (10537)
-DWITH_WAN_FAILOVER=ON        # WAN Failover object (10525)
-DWITH_MULTIWAN_HEALTH=ON     # Multi-WAN Health Check (10526)
-DWITH_WIFI_CLIENT_MGMT=ON    # WiFi Client Management (10527)
-DWITH_WIFI_CHANNEL_OPT=ON    # WiFi Channel Optimization (10528)
```

---

## Example Build Configurations

### Minimal Production Build (NB-IoT Sensor)

**Target**: Ultra-low footprint for constrained MCU (STM32L4, nRF9160)

```bash
cmake -B build -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_TINYDTLS=ON \
    -DDTLS_PROVIDER_MBEDTLS=OFF \
    -DWITH_DELTA_FOTA=OFF \
    -DWITH_MQTT=OFF \
    -DWITH_EDGE_AI=OFF \
    -DWITH_STARLINK=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF

cmake --build build --target friendly_lwm2m
```

**Expected Footprint**:
- Code: 45 KB (core) + 18 KB (TinyDTLS) = **63 KB flash**
- RAM: 12 KB (buffers) + 8 KB (TinyDTLS) = **20 KB RAM**

---

### Standard Production Build (LTE-M Device)

**Target**: General-purpose IoT device (Quectel BG95, u-blox SARA-R5)

```bash
cmake -B build -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DWITH_DELTA_FOTA=ON \
    -DWITH_BSDIFF=ON \
    -DWITH_VCDIFF=ON \
    -DWITH_MQTT=ON \
    -DBUILD_EXAMPLES=OFF

cmake --build build
```

**Expected Footprint**:
- Code: 85 KB (core + delta) + 45 KB (mbedTLS) = **130 KB flash**
- RAM: 24 KB (buffers) + 32 KB (mbedTLS) = **56 KB RAM**

---

### Full-Featured Gateway Build (OpenWRT Router)

**Target**: OpenWRT/prplOS router with gateway functionality

```bash
cmake -B build -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DWITH_DELTA_FOTA=ON \
    -DWITH_MQTT=ON \
    -DWITH_EDGE_AI=ON \
    -DWITH_TFLITE=ON \
    -DWITH_STARLINK=ON \
    -DWITH_OPENWRT_EXTENDED=ON \
    -DWITH_LAN_CONFIG=ON \
    -DWITH_ROUTING_TABLE=ON \
    -DWITH_VPN_CONFIG=ON \
    -DWITH_WAN_FAILOVER=ON \
    -DWITH_MULTIWAN_HEALTH=ON \
    -DWITH_WIFI_CLIENT_MGMT=ON \
    -DWITH_WIFI_CHANNEL_OPT=ON \
    -DWITH_ZIGBEE=ON \
    -DWITH_MATTER=ON

cmake --build build
```

**Expected Footprint**:
- Code: 450 KB (all objects + gateway) + 45 KB (mbedTLS) + 2 MB (TFLite) = **2.5 MB flash**
- RAM: 2 MB (object instances) + 512 KB (mbedTLS) + 8 MB (TFLite) = **10.5 MB RAM**

---

### Development Build with All Features (Testing)

**Target**: Comprehensive testing on development machine

```bash
cmake -B build -DBUILD_ENV=TEST \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DDTLS_PROVIDER_TINYDTLS=ON \
    -DDTLS_PROVIDER_OPENSSL=ON \
    -DWITH_DELTA_FOTA=ON \
    -DWITH_MQTT=ON \
    -DWITH_EDGE_AI=ON \
    -DWITH_TFLITE=ON \
    -DWITH_ONNX=ON \
    -DWITH_STARLINK=ON

cmake --build build
ctest --output-on-failure
```

**Features**:
- All DTLS providers
- All optional features
- Code coverage enabled
- Sanitizers enabled
- Comprehensive tests

---

## Build System Summary

| Environment | Optimization | Symbols | Sanitizers | Coverage | Tests | Logging | Binary Size |
|-------------|--------------|---------|------------|----------|-------|---------|-------------|
| **TEST** | -O0 | Full (g) | Yes | Yes | All | TRACE | 100% |
| **DEV** | -O2 | Full (g) | Yes | No | Unit | DEBUG | 85% |
| **PREPROD** | -O3 | Minimal (g1) | No | No | None | WARN | 60% |
| **PROD** | -Os | None (stripped) | No | No | None | ERROR | 45-55% |

**Size Comparison** (example application with mbedTLS):
```
TEST:     2.8 MB (unoptimized, symbols, coverage)
DEV:      1.9 MB (moderate optimization, symbols)
PREPROD:  1.2 MB (optimized, minimal symbols)
PROD:     850 KB (fully optimized, stripped, gc-sections)
```

---

## Troubleshooting

**Problem**: `No DTLS provider found!`
**Solution**: Enable at least one DTLS provider:
```bash
cmake -B build -DDTLS_PROVIDER_MBEDTLS=ON
```

**Problem**: Binary size too large for MCU flash
**Solution**: Use PROD environment with TinyDTLS:
```bash
cmake -B build -DBUILD_ENV=PROD -DDTLS_PROVIDER_TINYDTLS=ON -DDTLS_PROVIDER_MBEDTLS=OFF
```

**Problem**: Tests failing with sanitizer errors
**Solution**: Use TEST environment with sanitizers:
```bash
cmake -B build -DBUILD_ENV=TEST
```

**Problem**: Slow iteration time during development
**Solution**: Use DEV environment with dynamic linking:
```bash
cmake -B build -DBUILD_ENV=DEV -DDTLS_STATIC_LINK=OFF
```

---

## References

- CMake Documentation: https://cmake.org/documentation/
- GCC Optimization Options: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
- Linker Scripts: https://sourceware.org/binutils/docs/ld/
- mbedTLS Configuration: https://tls.mbed.org/kb/compiling-and-building
- TinyDTLS: https://projects.eclipse.org/projects/iot.tinydtls
