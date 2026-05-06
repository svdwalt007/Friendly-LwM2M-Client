# Quick Build Reference

## Common Build Commands

### Test All Environments

```bash
./scripts/build/build_all_environments.sh
```

### Build for Specific Environment

```bash
# TEST (unit testing, coverage)
cmake -B build_test -DBUILD_ENV=TEST
cmake --build build_test
cd build_test && ctest --output-on-failure

# DEV (development, default)
cmake -B build_dev -DBUILD_ENV=DEV
cmake --build build_dev

# PREPROD (pre-production validation)
cmake -B build_preprod -DBUILD_ENV=PREPROD
cmake --build build_preprod

# PROD (production deployment)
cmake -B build_prod -DBUILD_ENV=PROD
cmake --build build_prod
```

---

## DTLS Provider Selection

### Single Provider (Recommended for Production)

```bash
# mbedTLS (default, 45 KB in PROD)
cmake -B build -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DDTLS_PROVIDER_TINYDTLS=OFF \
    -DDTLS_PROVIDER_OPENSSL=OFF \
    -DDTLS_PROVIDER_WOLFSSL=OFF

# TinyDTLS (smallest, 18 KB in PROD)
cmake -B build -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_MBEDTLS=OFF \
    -DDTLS_PROVIDER_TINYDTLS=ON

# OpenSSL (best performance on Linux)
cmake -B build -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_MBEDTLS=OFF \
    -DDTLS_PROVIDER_OPENSSL=ON

# wolfSSL (FIPS 140-2/3)
cmake -B build -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_MBEDTLS=OFF \
    -DDTLS_PROVIDER_WOLFSSL=ON
```

### Multiple Providers (Testing Only)

```bash
cmake -B build -DBUILD_ENV=TEST \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DDTLS_PROVIDER_TINYDTLS=ON \
    -DDTLS_PROVIDER_OPENSSL=ON \
    -DDTLS_DEFAULT_PROVIDER=mbedtls
```

---

## Common Build Configurations

### Minimal Production (NB-IoT Sensor)

**Target**: Ultra-low footprint (63 KB flash, 20 KB RAM)

```bash
cmake -B build_minimal -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_TINYDTLS=ON \
    -DDTLS_PROVIDER_MBEDTLS=OFF \
    -DWITH_DELTA_FOTA=OFF \
    -DWITH_MQTT=OFF \
    -DWITH_EDGE_AI=OFF \
    -DWITH_STARLINK=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF

cmake --build build_minimal
```

### Standard Production (LTE-M Device)

**Target**: General purpose (130 KB flash, 56 KB RAM)

```bash
cmake -B build_standard -DBUILD_ENV=PROD \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DWITH_DELTA_FOTA=ON \
    -DWITH_BSDIFF=ON \
    -DWITH_VCDIFF=ON \
    -DWITH_MQTT=ON

cmake --build build_standard
```

### OpenWRT Gateway

**Target**: Full-featured router gateway (2.5 MB flash, 10.5 MB RAM)

```bash
cmake -B build_gateway -DBUILD_ENV=PROD \
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

cmake --build build_gateway
```

---

## Feature Modules Quick Reference

```bash
# Delta FOTA
-DWITH_DELTA_FOTA=ON -DWITH_BSDIFF=ON -DWITH_VCDIFF=ON -DWITH_COURGETTE=ON

# Edge AI
-DWITH_EDGE_AI=ON -DWITH_TFLITE=ON -DWITH_ONNX=ON

# Connectivity
-DWITH_MQTT=ON -DWITH_STARLINK=ON

# Gateway
-DWITH_ZIGBEE=ON -DWITH_MATTER=ON -DWITH_PRPL=ON

# OpenWRT Extended
-DWITH_OPENWRT_EXTENDED=ON \
-DWITH_LAN_CONFIG=ON \
-DWITH_ROUTING_TABLE=ON \
-DWITH_VPN_CONFIG=ON \
-DWITH_WAN_FAILOVER=ON \
-DWITH_MULTIWAN_HEALTH=ON \
-DWITH_WIFI_CLIENT_MGMT=ON \
-DWITH_WIFI_CHANNEL_OPT=ON
```

---

## Testing & Coverage

```bash
# Run tests
cmake -B build -DBUILD_ENV=TEST
cmake --build build
cd build && ctest --output-on-failure

# Generate coverage report (requires lcov)
cd build
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage_filtered.info
genhtml coverage_filtered.info --output-directory coverage_html
# Open coverage_html/index.html in browser
```

---

## Clean Build

```bash
# Remove all build directories
rm -rf build build_* coverage_html *.log

# Or use the script
./scripts/build/build_all_environments.sh clean
```

---

## Checking Binary Size

```bash
# Static library size
ls -lh build_prod/libfriendly_lwm2m.a

# Example application size
ls -lh build_prod/examples/friendly_example_client

# Detailed size breakdown
size build_prod/libfriendly_lwm2m.a
size build_prod/examples/friendly_example_client

# Symbol analysis (find largest symbols)
nm --size-sort --radix=d build_prod/libfriendly_lwm2m.a | tail -20
```

---

## Cross-Compilation Examples

### ARM Cortex-M (STM32, Nordic nRF)

```bash
cmake -B build_arm -DBUILD_ENV=PROD \
    -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake \
    -DDTLS_PROVIDER_TINYDTLS=ON \
    -DDTLS_PROVIDER_MBEDTLS=OFF

cmake --build build_arm
```

### MIPS (OpenWRT)

```bash
cmake -B build_openwrt -DBUILD_ENV=PROD \
    -DCMAKE_TOOLCHAIN_FILE=openwrt/toolchain.cmake \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DWITH_OPENWRT_EXTENDED=ON

cmake --build build_openwrt
```

---

## Troubleshooting

### Error: No DTLS provider found

```bash
# Enable at least one DTLS provider
cmake -B build -DDTLS_PROVIDER_MBEDTLS=ON
```

### Error: Cannot find mbedTLS

```bash
# Install mbedTLS
sudo apt-get install libmbedtls-dev  # Debian/Ubuntu
brew install mbedtls                  # macOS

# Or point to custom location
cmake -B build -DMBEDTLS_INCLUDE=/path/to/include -DMBEDTLS_LIB=/path/to/lib
```

### Binary too large for flash

```bash
# Use PROD environment with TinyDTLS
cmake -B build -DBUILD_ENV=PROD -DDTLS_PROVIDER_TINYDTLS=ON -DDTLS_PROVIDER_MBEDTLS=OFF

# Disable unused features
-DWITH_DELTA_FOTA=OFF -DWITH_MQTT=OFF -DWITH_EDGE_AI=OFF
```

### Tests failing with sanitizer errors

```bash
# Use TEST environment (sanitizers enabled)
cmake -B build -DBUILD_ENV=TEST
cmake --build build
cd build && ctest --output-on-failure
```

---

## Environment Comparison

| Aspect | TEST | DEV | PREPROD | PROD |
|--------|------|-----|---------|------|
| **Optimization** | -O0 | -O2 | -O3 | -Os |
| **Debug Symbols** | Full | Full | Minimal | None |
| **Sanitizers** | Yes | Yes | No | No |
| **Coverage** | Yes | No | No | No |
| **Tests** | All | Unit | None | None |
| **Logging** | TRACE | DEBUG | WARN | ERROR |
| **Binary Size** | 100% | 85% | 60% | 45-55% |
| **Build Time** | Slow | Fast | Medium | Slow |

---

## Further Reading

- Full build documentation: [BUILD_ENVIRONMENTS.md](BUILD_ENVIRONMENTS.md)
- CMake configuration: [CMakeLists.txt](CMakeLists.txt)
- Build environment settings: [cmake/BuildEnvironment.cmake](cmake/BuildEnvironment.cmake)
- DTLS optimization: [cmake/DTLSOptimization.cmake](cmake/DTLSOptimization.cmake)
