# Getting Started with Friendly LwM2M Client

Welcome to the Friendly LwM2M Client! This guide will help you understand, build, and integrate the LwM2M client into your IoT projects.

## Table of Contents

- [What is LwM2M?](#what-is-lwm2m)
- [Architecture Overview](#architecture-overview)
- [Installation](#installation)
- [Your First LwM2M Client](#your-first-lwm2m-client)
- [Understanding LwM2M Objects](#understanding-lwm2m-objects)
- [Working with Firmware Updates](#working-with-firmware-updates)
- [Advanced Features](#advanced-features)
- [Integration Guide](#integration-guide)
- [Best Practices](#best-practices)

## What is LwM2M?

LwM2M (Lightweight M2M) is an OMA SpecWorks protocol designed for IoT device management. It provides:

- **Device Management**: Remote configuration and control
- **Firmware Updates**: Over-the-air (OTA) updates with delta support
- **Data Reporting**: Sensor data collection and monitoring
- **Resource Control**: Efficient resource usage for constrained devices

### Key Concepts

**Objects**: Logical groupings of resources (e.g., Device, Firmware Update)
- Each object has a unique ID (e.g., Device = 3, Firmware Update = 5)

**Resources**: Individual data points within an object
- Read/Write operations
- Execute operations for actions
- Observe for notifications

**Instances**: Multiple copies of the same object
- Example: Multiple sensors of the same type

## Architecture Overview

```
┌─────────────────────────────────────────────────┐
│         Application Layer                       │
│  (Your Custom Logic & Business Rules)           │
└─────────────────────────────────────────────────┘
                    ↕
┌─────────────────────────────────────────────────┐
│       LwM2M Client Core                         │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │ Objects  │  │Transport │  │ Security │     │
│  │ Registry │  │ (CoAP)   │  │ (DTLS)   │     │
│  └──────────┘  └──────────┘  └──────────┘     │
└─────────────────────────────────────────────────┘
                    ↕
┌─────────────────────────────────────────────────┐
│      Platform Abstraction Layer                 │
│  (Linux, OpenWrt, Custom Platforms)             │
└─────────────────────────────────────────────────┘
                    ↕
┌─────────────────────────────────────────────────┐
│         Operating System / Hardware             │
└─────────────────────────────────────────────────┘
```

### Component Overview

**Core Objects** (Standard LwM2M):
- Device (3): Device information and control
- Firmware Update (5): Basic firmware management
- Connectivity Monitoring (4): Network status

**Custom Objects**:
- Advanced Firmware Update (33405): Delta updates, rollback
- MQTT Server (10512): MQTT broker configuration
- COSE (10513): Cryptographic operations
- Edge AI Inference (10514): On-device ML inference

**Transport Layer**:
- CoAP over UDP/DTLS
- MQTT (alternative transport)
- Blockwise transfer for large payloads

**Platform Layer**:
- Linux: Full-featured implementation
- OpenWrt: Router-optimized build
- Custom: Extensible platform abstraction

## Installation

### Quick Install (Ubuntu)

```bash
# Run automated installation script
bash scripts/install-ubuntu-prerequisites.sh
```

### Manual Installation

#### Prerequisites

**Required**:
- C++17 compatible compiler (GCC 9+, Clang 10+)
- CMake 3.14+
- OpenSSL 1.1.1+
- libcurl 7.68+
- zlib 1.2.11+

**Optional**:
- Mosquitto (for MQTT support)
- gRPC/Protobuf (for Starlink integration)
- Google Test (for unit tests)

#### Ubuntu/Debian

```bash
# Update package lists
sudo apt-get update

# Install build tools
sudo apt-get install build-essential cmake git

# Install required libraries
sudo apt-get install libssl-dev libcurl4-openssl-dev zlib1g-dev

# Install optional libraries
sudo apt-get install libmosquitto-dev libprotobuf-dev \
                     protobuf-compiler libgrpc++-dev

# Install testing framework
sudo apt-get install libgtest-dev libgmock-dev
```

#### Fedora/RHEL/CentOS

```bash
sudo dnf install gcc gcc-c++ cmake git
sudo dnf install openssl-devel libcurl-devel zlib-devel
sudo dnf install mosquitto-devel protobuf-devel grpc-devel
```

### Clone and Build

```bash
# Clone repository
git clone https://github.com/friendly-tech/lwm2m-client.git
cd lwm2m-client

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build (use all CPU cores)
make -j$(nproc)

# Run tests (optional)
ctest -V
```

## Your First LwM2M Client

### Minimal Example

Create a file `minimal_client.cpp`:

```cpp
#include "lwm2m_client.h"
#include "objects/device_object.h"
#include <iostream>

int main() {
    // Create LwM2M client instance
    lwm2m::LwM2MClient client(
        "my-device-001",                    // Endpoint name
        "coap://lwm2m.server.com:5683"      // Server URL
    );

    // Register the Device object
    auto deviceObj = std::make_unique<lwm2m::objects::DeviceObject>();
    deviceObj->setManufacturer("Friendly Technologies");
    deviceObj->setModelNumber("FLW-001");
    deviceObj->setSerialNumber("SN-12345");
    deviceObj->setFirmwareVersion("1.0.0");

    client.registerObject(std::move(deviceObj));

    // Connect to LwM2M server
    if (!client.connect()) {
        std::cerr << "Failed to connect to LwM2M server" << std::endl;
        return 1;
    }

    std::cout << "Connected to LwM2M server" << std::endl;

    // Run client event loop
    client.run();

    return 0;
}
```

### Compile and Run

```bash
# Compile
g++ -std=c++17 minimal_client.cpp \
    -I../include \
    -L../build -lfriendly_lwm2m \
    -lssl -lcrypto -lcurl -lpthread \
    -o minimal_client

# Run
./minimal_client
```

### Testing with Leshan Demo Server

Use the public Leshan demo server for testing:

```bash
./minimal_client --server-url coap://leshan.eclipseprojects.io:5683
```

Then visit https://leshan.eclipseprojects.io to see your device!

## Understanding LwM2M Objects

### Standard LwM2M Objects

#### Device Object (3)

Provides device information:

```cpp
#include "objects/device_object.h"

auto deviceObj = std::make_unique<lwm2m::objects::DeviceObject>();

// Set device information
deviceObj->setManufacturer("Friendly Tech");
deviceObj->setModelNumber("FLW-001");
deviceObj->setSerialNumber("SN-12345");
deviceObj->setFirmwareVersion("1.2.0");
deviceObj->setHardwareVersion("HW-REV-A");
deviceObj->setDeviceType("IoT Gateway");

// Set power sources
deviceObj->setPowerSource(
    lwm2m::objects::PowerSource::INTERNAL_BATTERY
);
deviceObj->setBatteryLevel(85); // 85%
deviceObj->setBatteryStatus(
    lwm2m::objects::BatteryStatus::NORMAL
);

// Set time
deviceObj->setCurrentTime(std::time(nullptr));
deviceObj->setTimezone("America/Los_Angeles");

// Register reboot callback
deviceObj->setRebootCallback([]() {
    std::cout << "Reboot requested by server" << std::endl;
    // Perform reboot logic
    system("reboot");
});

client.registerObject(std::move(deviceObj));
```

#### Connectivity Monitoring (4)

Monitor network connectivity:

```cpp
#include "objects/connectivity_monitoring_object.h"

auto connObj = std::make_unique<lwm2m::objects::ConnectivityMonitoringObject>();

// Set network information
connObj->setNetworkBearer(
    lwm2m::objects::NetworkBearer::ETHERNET
);
connObj->setIPAddresses({"192.168.1.100", "2001:db8::1"});
connObj->setSignalStrength(-65); // dBm
connObj->setLinkQuality(4);      // 0-5 scale

client.registerObject(std::move(connObj));
```

### Custom Objects

#### Advanced Firmware Update (33405)

Enhanced firmware update with delta support:

```cpp
#include "objects/advanced_firmware_update_object.h"
#include "platform/linux_platform.h"

// Create platform manager
auto platform = std::make_shared<lwm2m::platform::LinuxPlatformFactory>();

// Create firmware update object
auto fwObj = std::make_unique<lwm2m::objects::AdvancedFirmwareUpdateObject>(
    platform
);

// Configure firmware update
fwObj->setCurrentVersion("1.0.0");
fwObj->setBlockSize(1024);  // CoAP block size
fwObj->enableRollback(true); // Enable automatic rollback

// Set callbacks
fwObj->setStateChangeCallback(
    [](lwm2m::objects::FirmwareState oldState,
       lwm2m::objects::FirmwareState newState) {
        std::cout << "Firmware state: "
                  << static_cast<int>(oldState) << " -> "
                  << static_cast<int>(newState) << std::endl;
    }
);

fwObj->setProgressCallback(
    [](int progress) {
        std::cout << "Download progress: " << progress << "%" << std::endl;
    }
);

fwObj->setUpdateCompleteCallback(
    [](lwm2m::objects::UpdateResult result) {
        if (result == lwm2m::objects::UpdateResult::SUCCESS) {
            std::cout << "Firmware update successful!" << std::endl;
        } else {
            std::cerr << "Firmware update failed: "
                      << static_cast<int>(result) << std::endl;
        }
    }
);

client.registerObject(std::move(fwObj));
```

#### MQTT Server Object (10512)

Configure MQTT broker connection:

```cpp
#include "objects/mqtt_server_object.h"

auto mqttObj = std::make_unique<lwm2m::objects::MqttServerObject>();

// Configure MQTT broker
mqttObj->setBrokerAddress("mqtt.example.com");
mqttObj->setBrokerPort(1883);
mqttObj->setClientId("device-001");
mqttObj->setUsername("device");
mqttObj->setPassword("secret123");
mqttObj->setKeepAlive(60);
mqttObj->setCleanSession(true);

// Set TLS configuration
mqttObj->enableTLS(true);
mqttObj->setCACertificate("/path/to/ca.crt");

client.registerObject(std::move(mqttObj));
```

## Working with Firmware Updates

### Basic Firmware Update Flow

```cpp
#include "lwm2m_client.h"
#include "objects/advanced_firmware_update_object.h"
#include "platform/linux_platform.h"

int main() {
    lwm2m::LwM2MClient client("fw-device", "coap://server.com:5683");

    auto platform = std::make_shared<lwm2m::platform::LinuxPlatformFactory>();
    auto fwObj = std::make_unique<lwm2m::objects::AdvancedFirmwareUpdateObject>(
        platform
    );

    // Configure
    fwObj->setCurrentVersion("1.0.0");
    fwObj->enableRollback(true);

    // Monitor state changes
    fwObj->setStateChangeCallback(
        [](auto oldState, auto newState) {
            using State = lwm2m::objects::FirmwareState;
            switch (newState) {
                case State::DOWNLOADING:
                    std::cout << "Starting download..." << std::endl;
                    break;
                case State::DOWNLOADED:
                    std::cout << "Download complete, ready to update" << std::endl;
                    break;
                case State::UPDATING:
                    std::cout << "Installing firmware..." << std::endl;
                    break;
                case State::IDLE:
                    std::cout << "Update complete or idle" << std::endl;
                    break;
            }
        }
    );

    // Monitor progress
    fwObj->setProgressCallback([](int progress) {
        static int last = -1;
        if (progress != last) {
            std::cout << "\rProgress: " << progress << "%     " << std::flush;
            last = progress;
        }
    });

    // Handle completion
    fwObj->setUpdateCompleteCallback(
        [](lwm2m::objects::UpdateResult result) {
            std::cout << std::endl;
            if (result == lwm2m::objects::UpdateResult::SUCCESS) {
                std::cout << "✓ Firmware update successful!" << std::endl;
            } else {
                std::cerr << "✗ Update failed: "
                          << static_cast<int>(result) << std::endl;
            }
        }
    );

    client.registerObject(std::move(fwObj));
    client.connect();
    client.run();

    return 0;
}
```

### Delta Updates

Use delta updates to minimize bandwidth:

```cpp
// Enable delta algorithm
fwObj->setDeltaAlgorithm(lwm2m::firmware::DeltaAlgorithm::BSDIFF);

// Or use vcdiff
fwObj->setDeltaAlgorithm(lwm2m::firmware::DeltaAlgorithm::VCDIFF);

// Or Courgette (for executables)
fwObj->setDeltaAlgorithm(lwm2m::firmware::DeltaAlgorithm::COURGETTE);

// Server will send delta package instead of full firmware
```

### Rollback Support

Automatic rollback on update failure:

```cpp
// Enable rollback
fwObj->enableRollback(true);

// Set rollback timeout (seconds)
fwObj->setRollbackTimeout(300);  // 5 minutes

// If device doesn't confirm successful boot within timeout,
// it automatically rolls back to previous firmware
```

## Advanced Features

### MQTT Transport

Use MQTT instead of CoAP:

```cpp
#include "transport/mqtt_transport.h"

lwm2m::transport::MqttTransport::Config mqttConfig;
mqttConfig.brokerAddress = "mqtt://broker.example.com:1883";
mqttConfig.clientId = "device-001";
mqttConfig.username = "user";
mqttConfig.password = "pass";
mqttConfig.keepAlive = 60;

auto transport = std::make_shared<lwm2m::transport::MqttTransport>(mqttConfig);
client.setTransport(transport);
```

### Blockwise Transfer

For large firmware files:

```cpp
#include "transport/blockwise_transfer.h"

lwm2m::transport::BlockwiseTransfer::Config blockConfig;
blockConfig.preferredBlockSize = lwm2m::transport::BlockSize::BLOCK_1024;
blockConfig.bertEnabled = true;        // Enable BERT for large transfers
blockConfig.bertBlockSize = 8192;      // 8KB blocks
blockConfig.verifyChecksum = true;     // Verify integrity

auto blockTransfer = std::make_shared<lwm2m::transport::BlockwiseTransfer>(
    blockConfig
);
```

### Security (DTLS/PSK)

Secure communication with pre-shared keys:

```cpp
lwm2m::LwM2MClient::SecurityConfig secConfig;
secConfig.useDTLS = true;
secConfig.pskIdentity = "device-001";
secConfig.pskKey = "0123456789abcdef0123456789abcdef";  // Hex string

client.setSecurityConfig(secConfig);
```

## Integration Guide

### Integrating into Existing Projects

#### CMake Integration

Add to your `CMakeLists.txt`:

```cmake
# Add Friendly LwM2M as subdirectory
add_subdirectory(external/lwm2m-client)

# Link to your target
target_link_libraries(your_app
    PRIVATE
        friendly_lwm2m
        OpenSSL::SSL
        CURL::libcurl
)

# Add include directories
target_include_directories(your_app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/lwm2m-client/include
)
```

#### As External Package

Install system-wide:

```bash
cd lwm2m-client/build
sudo make install
```

Then in your project:

```cmake
find_package(FriendlyLwM2M REQUIRED)
target_link_libraries(your_app PRIVATE FriendlyLwM2M::friendly_lwm2m)
```

### Custom Platform Implementation

Implement platform abstraction for your hardware:

```cpp
#include "platform/platform_interface.h"

class MyPlatform : public lwm2m::platform::PlatformFactory {
public:
    std::unique_ptr<PartitionManager> createPartitionManager() override {
        return std::make_unique<MyPartitionManager>();
    }

    std::unique_ptr<FlashManager> createFlashManager() override {
        return std::make_unique<MyFlashManager>();
    }

    std::unique_ptr<BootloaderControl> createBootloaderControl() override {
        return std::make_unique<MyBootloaderControl>();
    }
};
```

See `include/platform/platform_interface.h` for full API.

## Best Practices

### Resource Management

```cpp
// Use smart pointers
auto obj = std::make_unique<lwm2m::objects::DeviceObject>();

// Don't store raw pointers after registration
client.registerObject(std::move(obj));
// obj is now nullptr, client owns it
```

### Error Handling

```cpp
// Check connection status
if (!client.connect()) {
    std::cerr << "Connection failed: " << client.getLastError() << std::endl;
    return 1;
}

// Handle firmware update errors
fwObj->setUpdateCompleteCallback(
    [](lwm2m::objects::UpdateResult result) {
        using Result = lwm2m::objects::UpdateResult;
        switch (result) {
            case Result::SUCCESS:
                // Success path
                break;
            case Result::NOT_ENOUGH_STORAGE:
                // Handle storage error
                break;
            case Result::INTEGRITY_CHECK_FAILED:
                // Handle integrity error
                break;
            default:
                // Handle other errors
                break;
        }
    }
);
```

### Logging

```cpp
// Enable debug logging
client.setLogLevel(lwm2m::LogLevel::DEBUG);

// Custom log handler
client.setLogHandler(
    [](lwm2m::LogLevel level, const std::string& message) {
        std::cout << "[" << levelToString(level) << "] "
                  << message << std::endl;
    }
);
```

### Thread Safety

```cpp
// Client is thread-safe, but objects should be registered before connect()
auto obj = std::make_unique<lwm2m::objects::DeviceObject>();
client.registerObject(std::move(obj));  // Do this before connect()

// Callbacks are called from client thread
fwObj->setProgressCallback([](int progress) {
    // This runs in client thread
    // Use mutex if accessing shared data
});
```

## Next Steps

- **Explore Examples**: Check `examples/` directory for complete applications
- **API Documentation**: Generate with `make docs` in build directory
- **Build Options**: See `BUILD_EXAMPLES.md` for advanced build configurations
- **Platform Guide**: See `docs/PLATFORM_GUIDE.md` for platform-specific details
- **Object Reference**: See `docs/OBJECT_REFERENCE.md` for all supported objects

## Support and Community

- **GitHub Issues**: https://github.com/friendly-tech/lwm2m-client/issues
- **Documentation**: https://lwm2m-client.readthedocs.io
- **Email Support**: support@friendly-tech.com
- **Contributing**: See `CONTRIBUTING.md`

## License

MIT License - See LICENSE file for details
