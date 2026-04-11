# Quick Start Guide

Welcome to the Friendly LwM2M Client! This guide will get you up and running in minutes.

## Prerequisites

### Ubuntu 20.04/22.04/24.04

Run our automated installation script:

```bash
# From the project root directory
bash scripts/install-ubuntu-prerequisites.sh
```

This installs all required dependencies including:
- Build tools (gcc, g++, cmake, ninja)
- Libraries (OpenSSL, libcurl, zlib)
- Optional features (Mosquitto, gRPC, Protobuf)
- Testing frameworks (Google Test, gcovr)
- Code quality tools (clang-format, cppcheck, valgrind)

### Manual Installation

If you prefer manual installation or are using another Linux distribution:

```bash
# Essential build tools
sudo apt-get install build-essential cmake git

# Required libraries
sudo apt-get install libssl-dev libcurl4-openssl-dev zlib1g-dev

# Testing framework
sudo apt-get install libgtest-dev libgmock-dev

# Optional features
sudo apt-get install libmosquitto-dev libprotobuf-dev protobuf-compiler
```

### Windows (WSL2)

1. Install WSL2 with Ubuntu 22.04:
   ```powershell
   wsl --install -d Ubuntu-22.04
   ```

2. Inside WSL, run the installation script:
   ```bash
   bash scripts/install-ubuntu-prerequisites.sh
   ```

## Building the Project

### Standard Build

```bash
# Clone the repository
git clone https://github.com/friendly-tech/lwm2m-client.git
cd lwm2m-client

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build (using all CPU cores)
make -j$(nproc)
```

Build output:
- `libfriendly_lwm2m.a` - Main library (~47MB)
- `test_*` - Unit test executables
- `examples/*` - Example applications

### Quick Build Commands

```bash
# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Debug build (with symbols)
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)

# Build with Ninja (faster)
cmake -G Ninja ..
ninja
```

### Build Options

Enable/disable optional features:

```bash
# Disable optional WPP objects (requires Wakaama submodule)
cmake -DWITH_WAN_FAILOVER=OFF \
      -DWITH_MULTIWAN_HEALTH=OFF \
      -DWITH_WIFI_CLIENT_MGMT=OFF \
      -DWITH_WIFI_CHANNEL_OPT=OFF \
      -DWITH_LAN_CONFIG=OFF \
      -DWITH_ROUTING_TABLE=OFF \
      -DWITH_VPN_CONFIG=OFF ..

# Enable code coverage
cmake -DENABLE_COVERAGE=ON ..

# Enable all optional objects (requires submodule initialization)
git submodule update --init --recursive
cmake -DWITH_WAN_FAILOVER=ON \
      -DWITH_MULTIWAN_HEALTH=ON \
      -DWITH_WIFI_CLIENT_MGMT=ON ..
```

## Running Tests

### Run All Tests

```bash
cd build
ctest -V
```

### Run Specific Tests

```bash
# Blockwise transfer tests
./test_blockwise_transfer

# Firmware update tests
./test_firmware_update_object

# Delta algorithm tests
./test_delta_algorithms

# Rollback manager tests
./test_rollback_manager
```

### Test with Coverage

```bash
# Configure with coverage enabled
cmake -DENABLE_COVERAGE=ON ..
make -j$(nproc)

# Run tests
ctest

# Generate coverage report
make coverage
# Open build/coverage/index.html in browser
```

## Basic Usage Example

### Simple LwM2M Client

```cpp
#include "lwm2m_client.h"
#include "objects/device_object.h"
#include "objects/firmware_update_object.h"

int main() {
    // Create LwM2M client
    lwm2m::LwM2MClient client("my-device", "coap://server.example.com:5683");

    // Register standard objects
    client.registerObject(std::make_unique<lwm2m::objects::DeviceObject>());
    client.registerObject(std::make_unique<lwm2m::objects::FirmwareUpdateObject>());

    // Connect to server
    if (client.connect()) {
        std::cout << "Connected to LwM2M server" << std::endl;

        // Run client event loop
        client.run();
    }

    return 0;
}
```

### Compile and Run

```bash
# Compile your application
g++ -std=c++17 my_client.cpp -I../include \
    -L. -lfriendly_lwm2m \
    -lssl -lcrypto -lcurl -lpthread \
    -o my_client

# Run
./my_client
```

## CLI Client Test Tool

The project includes a CLI test tool for quick testing and development.

### Basic Usage

```bash
# Connect to LwM2M server
./lwm2m_client_test --server-url coap://localhost:5683 \
                     --endpoint-name test-device

# With authentication
./lwm2m_client_test --server-url coaps://server.com:5684 \
                     --endpoint-name my-device \
                     --psk-identity device123 \
                     --psk-key 0123456789abcdef

# Enable firmware updates
./lwm2m_client_test --server-url coap://localhost:5683 \
                     --endpoint-name test-device \
                     --enable-firmware-update \
                     --firmware-version 1.0.0
```

### CLI Arguments

| Argument | Description | Default |
|----------|-------------|---------|
| `--server-url` | LwM2M server URL (coap:// or coaps://) | Required |
| `--endpoint-name` | Client endpoint name | Required |
| `--lifetime` | Registration lifetime (seconds) | 300 |
| `--psk-identity` | PSK identity for DTLS | None |
| `--psk-key` | PSK key (hex string) | None |
| `--enable-firmware-update` | Enable firmware update object | Disabled |
| `--firmware-version` | Current firmware version | 1.0.0 |
| `--enable-location` | Enable location object | Disabled |
| `--latitude` | Device latitude | 0.0 |
| `--longitude` | Device longitude | 0.0 |
| `--log-level` | Logging level (debug/info/warn/error) | info |
| `--bootstrap` | Use bootstrap server | false |

### Example Scenarios

**Basic connectivity test:**
```bash
./lwm2m_client_test --server-url coap://localhost:5683 \
                     --endpoint-name test-device \
                     --log-level debug
```

**Firmware update testing:**
```bash
./lwm2m_client_test --server-url coap://localhost:5683 \
                     --endpoint-name fw-test \
                     --enable-firmware-update \
                     --firmware-version 2.1.0 \
                     --log-level info
```

**Secure connection with PSK:**
```bash
./lwm2m_client_test --server-url coaps://server.example.com:5684 \
                     --endpoint-name secure-device \
                     --psk-identity device001 \
                     --psk-key 0123456789abcdef0123456789abcdef \
                     --lifetime 600
```

**Location tracking:**
```bash
./lwm2m_client_test --server-url coap://localhost:5683 \
                     --endpoint-name gps-tracker \
                     --enable-location \
                     --latitude 37.7749 \
                     --longitude -122.4194
```

## Project Structure

```
lwm2m-client/
├── include/              # Public header files
│   ├── objects/         # LwM2M object implementations
│   ├── platform/        # Platform abstraction layer
│   ├── transport/       # Network transport (CoAP, MQTT)
│   └── firmware/        # Firmware update & delta algorithms
├── src/                 # Implementation files
├── test/                # Unit tests
├── examples/            # Example applications
├── docs/                # Documentation
├── scripts/             # Utility scripts
└── wpp/                 # WPP registry objects (10512-10537)
```

## Key Features

### Object ID Mapping (v1.2.0)

| Object Name | Object ID | Status |
|------------|-----------|--------|
| Device | 3 | Standard |
| Firmware Update | 5 | Standard |
| Advanced Firmware Update | 33405 | Custom |
| MQTT Server | 10512 | WPP |
| COSE | 10513 | WPP |
| Edge AI Inference | 10514 | WPP |
| WAN Failover Policy | 10525 | Optional* |
| Multi-WAN Health Check | 10526 | Optional* |
| WiFi Client Management | 10527 | Optional* |
| WiFi Channel Optimization | 10528 | Optional* |
| LAN Configuration | 10535 | Optional* |
| Routing Table | 10536 | Optional* |
| VPN Configuration | 10537 | Optional* |

*Optional objects require Wakaama submodule initialization

### Advanced Features

- **Delta Updates**: bsdiff, vcdiff, Courgette algorithms
- **Rollback Management**: Atomic updates with automatic rollback
- **Blockwise Transfer**: RFC 7959 with BERT support
- **MQTT Transport**: Alternative to CoAP
- **Platform Abstraction**: Linux, OpenWrt support
- **Starlink Integration**: Direct gRPC client for SpaceX Starlink

## Troubleshooting

### Build Errors

**Missing headers:**
```bash
# Ensure all prerequisites are installed
bash scripts/install-ubuntu-prerequisites.sh
```

**Wakaama submodule errors:**
```bash
# Disable optional objects or initialize submodule
cmake -DWITH_WAN_FAILOVER=OFF -DWITH_MULTIWAN_HEALTH=OFF \
      -DWITH_WIFI_CLIENT_MGMT=OFF -DWITH_WIFI_CHANNEL_OPT=OFF \
      -DWITH_LAN_CONFIG=OFF -DWITH_ROUTING_TABLE=OFF \
      -DWITH_VPN_CONFIG=OFF ..
```

**Clean build:**
```bash
rm -rf build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Runtime Issues

**Connection refused:**
- Verify server URL is correct
- Check firewall settings
- Test with `ping` and `netcat`

**DTLS/PSK errors:**
- Verify PSK key is valid hex string
- Check PSK identity matches server configuration
- Ensure OpenSSL supports PSK ciphersuites

**Firmware update fails:**
- Check available disk space
- Verify firmware package integrity
- Review logs with `--log-level debug`

## Next Steps

1. **Read the API Documentation**
   ```bash
   # Generate Doxygen docs
   cd build
   make docs
   # Open build/docs/html/index.html
   ```

2. **Explore Examples**
   - `examples/basic_client.cpp` - Simple client
   - `examples/firmware_update.cpp` - Firmware update
   - `examples/mqtt_transport.cpp` - MQTT usage

3. **Join the Community**
   - GitHub Issues: Report bugs and request features
   - Discussions: Ask questions and share implementations
   - Contributing: See CONTRIBUTING.md

## Additional Resources

- **Full Documentation**: See `docs/` directory
- **API Reference**: Generated with Doxygen
- **Build Examples**: See `BUILD_EXAMPLES.md`
- **Getting Started**: See `GETTING_STARTED.md`
- **Object Reference**: See `docs/OBJECT_REFERENCE.md`
- **Platform Guide**: See `docs/PLATFORM_GUIDE.md`

## License

MIT License - See LICENSE file for details

## Support

- **Email**: support@friendly-tech.com
- **GitHub Issues**: https://github.com/friendly-tech/lwm2m-client/issues
- **Documentation**: https://lwm2m-client.readthedocs.io
