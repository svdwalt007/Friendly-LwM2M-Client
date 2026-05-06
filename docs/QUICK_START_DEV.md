# Quick Start - Development Setup from Scratch

This guide helps you set up a complete development and test environment for the Friendly LwM2M Client v1.2.2 with ALL features enabled.

## TL;DR - One-Command Setup

```bash
./scripts/dev_setup_complete.sh
```

This will:
- ✅ Check all prerequisites
- ✅ Initialize git submodules
- ✅ Configure CMake with ALL features (Walt Tech objects, Starlink, OpenWRT, etc.)
- ✅ Build the project (~15-30 min)
- ✅ Run all tests (unit, integration, benchmarks)
- ✅ Generate usage instructions
- ✅ Create example applications

**Estimated time:** 20-40 minutes (depending on your system)

## Prerequisites

### Ubuntu/Debian

**Essential dependencies:**
```bash
sudo apt-get update
sudo apt-get install -y \
    cmake \
    build-essential \
    git \
    pkg-config \
    python3 \
    libmbedtls-dev \
    libssl-dev \
    zlib1g-dev \
    libcurl4-openssl-dev \
    lcov \
    genhtml \
    doxygen \
    graphviz
```

**Optional - For static linking (recommended for production):**
```bash
# Option 1: Build mbedTLS from source with static libs
git clone https://github.com/Mbed-TLS/mbedtls.git
cd mbedtls
git checkout v3.6.0
mkdir build && cd build
cmake .. -DENABLE_PROGRAMS=OFF -DENABLE_TESTING=OFF
make -j$(nproc)
sudo make install
cd ../..

# Option 2: Use dynamic linking (default fallback, works fine for dev)
# No action needed - script will use dynamic libs automatically
```

### Fedora/RedHat

```bash
sudo dnf install -y \
    cmake \
    gcc-c++ \
    git \
    pkg-config \
    python3 \
    mbedtls-devel \
    openssl-devel \
    zlib-devel \
    libcurl-devel \
    lcov
```

### macOS

```bash
brew install cmake git pkg-config mbedtls openssl zlib curl lcov
```

## What Gets Enabled

### Walt Tech Objects (10525-10537)
- ✅ **WAN Failover Policy (10525)** - Multi-WAN failover configuration
- ✅ **Multi-WAN Health Check (10526)** - Connection monitoring with ICMP/HTTP/TCP probes
- ✅ **WiFi Client Management (10527)** - Client tracking, QoS, blocking
- ✅ **WiFi Channel Optimization (10528)** - Auto-channel selection
- ✅ **Matter Bridge (10529)** - Matter protocol bridging
- ✅ **Matter Device (10530)** - Matter device integration
- ✅ **Thread Network (10531)** - Thread border router
- ✅ **LAN Configuration (10535)** - LAN interface settings
- ✅ **Routing Table (10536)** - Static route management
- ✅ **VPN Configuration (10537)** - VPN tunnel setup

**Note:** Zigbee objects (10532-10534) are disabled due to old framework patterns.

### Core Features
- ✅ **Multi-DTLS Providers** - TinyDTLS, mbedTLS (default), OpenSSL
- ✅ **MQTT Transport Binding** - OMA LwM2M v1.2.2 Section 8 compliance
- ✅ **Delta FOTA** - BSDIFF, VCDIFF, Courgette algorithms
- ✅ **A/B Partition Rollback** - Reliable firmware updates
- ✅ **Edge AI Inference** - TensorFlow Lite, ONNX Runtime (if available)
- ✅ **Starlink Integration** - Metrics collection (gRPC if available)
- ✅ **OpenWRT Extended** - LuCI module, hotplug handlers
- ✅ **Complete Test Suite** - Unit, integration, benchmark tests

### Platform Support
- ✅ **Linux** - Full support
- ✅ **OpenWRT** - Package build scripts included
- ⚠️ **prplOS** - Platform-specific, requires prpl environment
- ⚠️ **RTOS** - FreeRTOS, Zephyr, mbed OS (partial support)

## Step-by-Step Manual Setup

If you prefer manual setup or want to customize the build:

### 1. Clone Repository

```bash
git clone https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client
```

### 2. Initialize Submodules

```bash
git submodule update --init --recursive
```

This initializes:
- `2305-Wakaama` - LwM2M implementation
- `examples/coap/libcoap` - CoAP library

### 3. Configure Build (All Features)

```bash
mkdir build && cd build

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_OPENSSL=ON \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  -DWITH_DELTA_FOTA=ON \
  -DWITH_MQTT=ON \
  -DWITH_EDGE_AI=ON \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  -DWITH_MATTER_BRIDGE=ON \
  -DWITH_MATTER_DEVICE=ON \
  -DWITH_THREAD_NETWORK=ON \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON \
  -DWITH_STARLINK=ON \
  -DWITH_OPENWRT_EXTENDED=ON \
  -DBUILD_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  -DENABLE_COVERAGE=ON
```

### 4. Build

```bash
make -j$(nproc)
```

**Estimated time:** 10-20 minutes

### 5. Run Tests

```bash
# All tests
ctest --output-on-failure -j$(nproc)

# Specific test suites
./test_delta_algorithms
./test_blockwise_transfer
./test_firmware_update_object
./test_dtls_integration

# Benchmarks
./benchmark_dtls_providers
```

### 6. Run Example Client

```bash
cd examples
./friendly_example_client -h coaps://leshan.eclipseprojects.io:5684 -n TestDevice
```

## Quick Test Commands

### Test DTLS Providers

```bash
cd build/examples

# List available providers
./friendly_example_client --list-dtls-providers

# Test mbedTLS (default)
./friendly_example_client --dtls-provider=mbedtls -h coaps://leshan.eclipseprojects.io:5684

# Test TinyDTLS
./friendly_example_client --dtls-provider=tinydtls -h coaps://leshan.eclipseprojects.io:5684

# Test OpenSSL
./friendly_example_client --dtls-provider=openssl -h coaps://leshan.eclipseprojects.io:5684
```

### Test Walt Tech Objects

Connect to Leshan demo server and explore objects 10525-10537:

1. **Start client:**
```bash
./friendly_example_client -h coaps://leshan.eclipseprojects.io:5684 -n WaltTechDemo
```

2. **Open Leshan UI:** https://leshan.eclipseprojects.io/

3. **Find your device:** Look for "WaltTechDemo" in client list

4. **Explore objects:** Expand object tree and navigate to:
   - Object 10525: WAN Failover Policy
   - Object 10526: Multi-WAN Health Check
   - Object 10527: WiFi Client Management
   - Object 10528: WiFi Channel Optimization
   - ... (all objects 10525-10537)

### Test MQTT Transport

```bash
# Requires MQTT broker (e.g., Mosquitto)
./friendly_example_client \
  -h coaps://server:5684 \
  --mqtt-broker=mqtt://localhost:1883 \
  -n MqttDevice
```

### Test Delta FOTA

```bash
# Requires firmware update file
./friendly_example_client \
  -h coaps://server:5684 \
  --fota-url=http://server/firmware-delta.patch \
  -n FotaDevice
```

## Build Variants

### Minimal Build (Constrained Devices)

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=OFF \
  -DDTLS_PROVIDER_OPENSSL=OFF \
  -DWITH_DELTA_FOTA=OFF \
  -DWITH_MQTT=OFF \
  -DWITH_EDGE_AI=OFF \
  -DBUILD_TESTS=OFF
```

### Development Build (All Features + Debug)

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_OPENSSL=ON \
  -DWITH_ALL_FEATURES=ON \
  -DBUILD_TESTS=ON \
  -DENABLE_COVERAGE=ON \
  -DENABLE_SANITIZERS=ON
```

### Production Build (Optimized)

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  -DDTLS_STATIC_LINK=ON \
  -DWITH_WALT_TECH_OBJECTS=ON \
  -DBUILD_TESTS=OFF \
  -DENABLE_SANITIZERS=OFF
```

## OpenWRT Deployment

### Build Package

```bash
# Requires OpenWRT SDK
cd openwrt
make package/friendly-lwm2m/compile V=s
```

### Install Package

```bash
# On OpenWRT device
opkg update
opkg install friendly-lwm2m_1.2.2-1_*.ipk
```

### Configure via LuCI

1. Navigate to **Services → LwM2M Client**
2. Configure server URI and credentials
3. Enable Walt Tech objects (10525-10537)
4. Save and apply

### Command Line Configuration

```bash
# Edit config
vi /etc/config/lwm2m_client

# Start service
/etc/init.d/friendly-lwm2m start

# Check logs
logread -f | grep lwm2m
```

## prpl Deployment

### Requirements
- prplOS development environment
- Ambiorix data bus
- EasyMesh R2/R4 support (optional)

### Build

```bash
# In prplOS environment
cmake .. \
  -DWITH_PRPL=ON \
  -DWITH_PRPL_AMBIORIX=ON \
  -DWITH_PRPL_EASYMESH=ON
make
```

### Integration Points

- **Ambiorix Bus** - Data model integration
- **EasyMesh** - Topology management via LwM2M objects
- **prplMesh** - Backhaul optimization

See `src/platform/prpl/README.md` for detailed instructions.

## Troubleshooting

### Build Fails with "Wakaama submodule not found"

```bash
git submodule update --init --recursive
```

### CMake can't find mbedTLS

```bash
# Ubuntu/Debian
sudo apt-get install libmbedtls-dev

# Fedora
sudo dnf install mbedtls-devel

# macOS
brew install mbedtls
```

### Tests fail with "Permission denied"

```bash
# Make test executables executable
chmod +x build/test_*
```

### Client can't connect to server

**Check network:**
```bash
ping leshan.eclipseprojects.io
nc -zv leshan.eclipseprojects.io 5684
```

**Check DTLS provider:**
```bash
./friendly_example_client --list-dtls-providers
```

**Check server logs:** Look for connection errors in Leshan logs

### Clean build needed

```bash
rm -rf build
mkdir build
cd build
cmake ..
make
```

## Performance Benchmarks

### Run All Benchmarks

```bash
cd build

# DTLS provider performance
./benchmark_dtls_providers

# Memory usage
valgrind --tool=massif ./examples/friendly_example_client -h coaps://server:5684

# CPU profiling
perf record -g ./examples/friendly_example_client -h coaps://server:5684
perf report
```

### Expected Performance

| Metric | TinyDTLS | mbedTLS | OpenSSL |
|--------|----------|---------|---------|
| Binary size | ~50KB | ~200KB | ~500KB |
| RAM usage | ~20KB | ~50KB | ~100KB |
| Handshake time | ~200ms | ~150ms | ~100ms |
| Throughput | ~100KB/s | ~500KB/s | ~1MB/s |

## Coverage Report

```bash
cd build
make coverage
firefox coverage/index.html
```

**Target coverage:** >80% for core modules

## Next Steps

1. **Read the documentation:**
   - `README.md` - Project overview
   - `RELEASE_NOTES_v1.2.2.md` - What's new
   - `DTLS_PROVIDER_SELECTION_GUIDE.md` - Provider comparison
   - `FIRST_TIME_USAGE.md` - Detailed usage guide (auto-generated)

2. **Explore the code:**
   - `src/` - Core implementation
   - `wpp/` - WPP framework
   - `wpp/registry/objects/w_10525_*` - Walt Tech objects
   - `examples/` - Example applications

3. **Test platform-specific builds:**
   - OpenWRT: See `openwrt/README.md`
   - prpl: See `src/platform/prpl/README.md`
   - RTOS: See `tests/platform/rtos/`

4. **Join the community:**
   - Report issues: GitHub Issues
   - Contribute: See CONTRIBUTING.md

## Support

**Questions?**
- GitHub Issues: https://github.com/Friendly-Technologies/Friendly-LwM2M-Client/issues
- Documentation: Check `docs/` directory
- Examples: See `examples/` directory

**Found a bug?**
- Report it: GitHub Issues with build logs and error messages
- Include: OS, CMake version, gcc version, enabled features

**Want to contribute?**
- Fork, code, test, submit PR
- Follow coding standards
- Include tests for new features
- Update documentation

---

**Happy developing! 🚀**

*This guide was created for Friendly LwM2M Client v1.2.2*
