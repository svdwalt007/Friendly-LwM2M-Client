# CMake Configuration Guide - Full Feature Build

This guide explains how to configure CMake with all available features for the Friendly LwM2M Client.

## Quick Start

### Linux/macOS

```bash
# Make script executable
chmod +x configure_full_build.sh

# Run configuration script
./configure_full_build.sh

# Build the project
cd build
make -j$(nproc)
```

### Windows

```cmd
# Run configuration script
configure_full_build.bat

# Build the project
cd build
cmake --build . --config Release
```

## Manual CMake Configuration

If you prefer to run CMake manually or need to customize options:

```bash
cd build
cmake .. [OPTIONS]
```

## All Available CMake Options

### Core DTLS Features

```bash
-DDTLS=ON                          # Enable DTLS support
-DMBEDTLS=ON                       # Use mbedTLS library
-DTINYDTLS=OFF                     # Disable TinyDTLS (using multi-provider)
```

### DTLS Multi-Provider (v1.2.2)

```bash
-DDTLS_TINYDTLS_PROVIDER=ON        # Enable TinyDTLS provider
-DDTLS_MBEDTLS_PROVIDER=ON         # Enable mbedTLS provider (default)
-DDTLS_OPENSSL_PROVIDER=OFF        # OpenSSL provider (optional)
-DDTLS_WOLFSSL_PROVIDER=OFF        # wolfSSL provider (optional)
-DDTLS_DEFAULT_PROVIDER=mbedtls    # Set default DTLS provider
-DDTLS_STATIC_LINKING=ON           # Statically link DTLS libraries
```

**Provider Options:**
- `mbedtls` - Recommended for embedded systems
- `tinydtls` - Lightweight option
- `openssl` - Full-featured, requires OpenSSL
- `wolfssl` - Security-focused, requires wolfSSL

### Delta FOTA Features

```bash
-DWITH_DELTA_FOTA=ON               # Enable delta firmware updates
-DWITH_BSDIFF=ON                   # Binary diff algorithm
-DWITH_VCDIFF=ON                   # Version control diff
-DWITH_COURGETTE=ON                # Chromium's diff algorithm
-DWITH_COMPRESSION=ON              # Compression support
-DWITH_ROLLBACK=ON                 # Firmware rollback protection
```

### Transport and AI Features

```bash
-DWITH_MQTT_TRANSPORT=ON           # Enable MQTT transport layer
-DWITH_EDGE_AI=ON                  # Edge AI inference support
-DWITH_TENSORFLOW_LITE=ON          # TensorFlow Lite integration
-DWITH_ONNX_RUNTIME=ON             # ONNX Runtime integration
```

### OpenWRT Extended Features (v1.2.0)

```bash
-DWITH_OPENWRT_EXTENDED=ON         # Enable OpenWRT extensions
-DWITH_LUCI_MODULE=ON              # LuCI web interface module
-DWITH_HOTPLUG_HANDLERS=ON         # Network hotplug handlers
-DWITH_LAN_CONFIG=ON               # LAN configuration (Object 10535)
-DWITH_ROUTING_TABLE=ON            # Routing table management (Object 10536)
-DWITH_VPN_CONFIG=ON               # VPN configuration (Object 10537)
-DWITH_WAN_FAILOVER=ON             # WAN failover policy (Object 10525)
-DWITH_MULTIWAN_HEALTH=ON          # Multi-WAN health check (Object 10526)
-DWITH_WIFI_CLIENT_MGMT=ON         # WiFi client management (Object 10527)
-DWITH_WIFI_CHANNEL_OPT=ON         # WiFi channel optimization (Object 10528)
```

### Starlink Integration

```bash
-DWITH_STARLINK=ON                 # Starlink terminal support (Object 10512)
-DWITH_STARLINK_GRPC=ON            # Starlink gRPC interface
```

### prpl Foundation Platform Integrations

```bash
-DWITH_PRPL_INTEGRATION=ON         # Enable prpl integration
-DWITH_PRPL_EASYMESH=ON            # EasyMesh WiFi management
-DWITH_PRPL_AMBIORIX=ON            # Ambiorix data model framework
```

### Matter/Thread/Zigbee Platform Integrations

```bash
-DWITH_MATTER_THREAD=ON            # Matter over Thread support
-DWITH_MATTER_BRIDGE=ON            # Matter bridge (Objects 10529-10531)
-DWITH_THREAD_BR=ON                # Thread border router
-DWITH_ZIGBEE=ON                   # Zigbee support (Objects 10532-10534)
-DWITH_ZIGBEE_EZSP=ON              # Zigbee EZSP protocol
-DWITH_ZIGBEE_ZSTACK=ON            # Zigbee Z-Stack protocol
```

### Build Options

```bash
-DWITH_TESTS=ON                    # Build unit tests
-DWITH_EXAMPLES=ON                 # Build example applications
-DWITH_DOCUMENTATION=ON            # Generate documentation (Doxygen)
-DWITH_COVERAGE=ON                 # Enable code coverage
-DWITH_SANITIZERS=ON               # Enable sanitizers (AddressSanitizer, etc.)
```

### Build Type

```bash
-DCMAKE_BUILD_TYPE=Release         # Release build (optimized)
-DCMAKE_BUILD_TYPE=Debug           # Debug build (with symbols)
-DCMAKE_BUILD_TYPE=RelWithDebInfo  # Release with debug info
-DCMAKE_BUILD_TYPE=MinSizeRel      # Minimum size release
```

## Preset Configurations

### Minimal Build (Core Features Only)

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DDTLS=ON \
    -DMBEDTLS=ON \
    -DWITH_EXAMPLES=ON
```

### Standard Build (Common Features)

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DDTLS=ON \
    -DMBEDTLS=ON \
    -DWITH_DELTA_FOTA=ON \
    -DWITH_MQTT_TRANSPORT=ON \
    -DWITH_EXAMPLES=ON \
    -DWITH_TESTS=ON
```

### OpenWRT Router Build

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DDTLS=ON \
    -DMBEDTLS=ON \
    -DWITH_OPENWRT_EXTENDED=ON \
    -DWITH_LUCI_MODULE=ON \
    -DWITH_LAN_CONFIG=ON \
    -DWITH_ROUTING_TABLE=ON \
    -DWITH_VPN_CONFIG=ON \
    -DWITH_WAN_FAILOVER=ON \
    -DWITH_MULTIWAN_HEALTH=ON \
    -DWITH_WIFI_CLIENT_MGMT=ON \
    -DWITH_WIFI_CHANNEL_OPT=ON \
    -DWITH_EXAMPLES=ON
```

### Smart Home Hub Build (Matter + Zigbee)

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DDTLS=ON \
    -DMBEDTLS=ON \
    -DWITH_MATTER_BRIDGE=ON \
    -DWITH_THREAD_BR=ON \
    -DWITH_ZIGBEE=ON \
    -DWITH_ZIGBEE_EZSP=ON \
    -DWITH_EXAMPLES=ON
```

### Edge AI Gateway Build

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DDTLS=ON \
    -DMBEDTLS=ON \
    -DWITH_EDGE_AI=ON \
    -DWITH_TENSORFLOW_LITE=ON \
    -DWITH_MQTT_TRANSPORT=ON \
    -DWITH_EXAMPLES=ON
```

### Development Build (All Features + Testing)

```bash
./configure_full_build.sh
```

## Feature Dependencies

Some features require specific dependencies to be installed:

### DTLS Providers
- **mbedTLS**: `libmbedtls-dev` (Debian/Ubuntu)
- **OpenSSL**: `libssl-dev`
- **wolfSSL**: `libwolfssl-dev`

### Compression Libraries
- **BZip2**: `libbz2-dev`
- **LZMA**: `liblzma-dev`
- **Zstandard**: `libzstd-dev`
- **Brotli**: `libbrotli-dev`

### AI Frameworks
- **TensorFlow Lite**: Build from source or use prebuilt binaries
- **ONNX Runtime**: `libonnxruntime-dev`

### Platform Integration
- **gRPC** (for Starlink): `libgrpc++-dev`
- **Ambiorix**: prpl SDK
- **Matter SDK**: Build from source

### Build Tools
- **Doxygen**: `doxygen` (for documentation)
- **gcov/lcov**: `gcov lcov` (for coverage)
- **Sanitizers**: Included with GCC/Clang

## Verifying Configuration

After running CMake, check the output for enabled features:

```
-- Core Features:
--   DTLS:             ON
--   mbedTLS:          ON
--
-- v1.2.0 Extended Features:
--   OpenWRT Extended: ON
--   LAN Config:       ON
--   ...
--
-- Platform Integrations:
--   Matter/Thread:    ON
--   Zigbee:           ON
--   ...
```

## Building After Configuration

### Using Make (Linux/macOS)

```bash
cd build

# Build all targets
make -j$(nproc)

# Build specific target
make friendly_example_client

# Run tests
make test

# Generate documentation
make doc

# Install
sudo make install
```

### Using CMake Build Command (Cross-platform)

```bash
cd build

# Build all targets
cmake --build . --parallel

# Build specific target
cmake --build . --target friendly_example_client

# Run tests
ctest

# Install
cmake --install .
```

## Troubleshooting

### Missing Dependencies

**Error:** `Could not find X library`

**Solution:** Install the required dependency:
```bash
# Debian/Ubuntu
sudo apt-get install <package-name>

# Fedora/RHEL
sudo dnf install <package-name>

# macOS
brew install <package-name>
```

### Conflicting Options

Some options may conflict. Common conflicts:
- TinyDTLS standalone vs. Multi-provider mode
- Multiple DTLS providers as default

**Solution:** Use the provided configuration scripts or presets.

### Platform-Specific Features

Some features are only available on specific platforms:
- **OpenWRT features**: Linux only
- **LuCI module**: OpenWRT only
- **prpl Ambiorix**: Requires prpl SDK

### Out of Memory During Build

**Solution:** Reduce parallel jobs:
```bash
make -j2  # Use 2 jobs instead of all cores
```

## Configuration File

CMake stores configuration in `build/CMakeCache.txt`. To reconfigure:

```bash
# Clean configuration
rm -rf build/*

# Or delete cache only
rm build/CMakeCache.txt

# Then reconfigure
cmake .. [OPTIONS]
```

## Environment Variables

Override default paths:

```bash
# Custom install prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/lwm2m

# Custom library path
cmake .. -DCMAKE_PREFIX_PATH=/usr/local

# Compiler selection
cmake .. -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_CXX_COMPILER=g++-11
```

## Integration with ConfigLoader

After enabling objects (10525-10537), ensure they're loaded from JSON:

1. Objects are automatically included if CMake flags are set
2. JSON configuration in `examples/lwm2m_objects_config.json` includes them
3. ConfigLoader will populate them at startup (if registered in main.cpp)

See `OBJECTS_10525-10537_README.md` for details on registering objects.

## Summary of Object Enablement

| Object Range | Features | CMake Flags Required |
|--------------|----------|---------------------|
| 10512 | Starlink Terminal | `-DWITH_STARLINK=ON` |
| 10513-10522 | Router Management | Various router flags |
| 10525-10528 | Network Management | `-DWITH_WAN_FAILOVER=ON`, etc. |
| 10529-10531 | Matter/Thread | `-DWITH_MATTER_BRIDGE=ON` |
| 10532-10534 | Zigbee | `-DWITH_ZIGBEE=ON` |
| 10535-10537 | Router Config | `-DWITH_LAN_CONFIG=ON`, etc. |

## See Also

- `CONFIGURATION_SUMMARY.md` - Overview of configuration system
- `OBJECTS_10525-10537_README.md` - Detailed object documentation
- `CONFIG_LOADER_README.md` - ConfigLoader documentation
- `BUILD_AND_TEST.md` - Building and testing guide
