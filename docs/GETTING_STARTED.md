# Getting Started with Friendly LwM2M Client

This guide will help you get up and running with the Friendly LwM2M Client in under 30 minutes.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Quick Installation](#quick-installation)
3. [Your First Client](#your-first-client)
4. [Connecting to a Server](#connecting-to-a-server)
5. [Using MQTT Transport](#using-mqtt-transport)
6. [Running Edge AI Inference](#running-edge-ai-inference)
7. [Next Steps](#next-steps)

---

## Prerequisites

### System Requirements

| Requirement | Minimum | Recommended |
|-------------|---------|-------------|
| OS | Ubuntu 18.04 / Debian 10 | Ubuntu 22.04 / Debian 12 |
| Architecture | x86_64 or ARM64 | x86_64 or ARM64 |
| RAM | 512 MB | 2 GB+ |
| Disk | 100 MB | 500 MB+ |
| C++ Compiler | GCC 8 / Clang 7 | GCC 11+ / Clang 14+ |
| CMake | 3.16 | 3.22+ |

### Install Dependencies

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libcurl4-openssl-dev \
    zlib1g-dev \
    liblzma-dev

# Optional: For MQTT support
sudo apt-get install -y libmosquitto-dev

# Optional: For compression
sudo apt-get install -y libzstd-dev libbrotli-dev
```

---

## Quick Installation

### Clone and Build

```bash
# Clone the repository
git clone https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client

# Create build directory
mkdir build && cd build

# Configure (basic)
cmake ..

# Build
make -j$(nproc)

# Verify build
./test_delta_algorithms  # Should show all tests passing
```

### Build with All Features

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_MQTT=ON \
    -DWITH_EDGE_AI=ON \
    -DWITH_ROLLBACK=ON \
    -DBUILD_TESTS=ON

make -j$(nproc)
```

---

## Your First Client

### Minimal Example

Create a file `my_client.cpp`:

```cpp
#include <iostream>
#include <csignal>
#include "platform/linux_platform.h"
#include "objects/advanced_firmware_update_object.h"

using namespace lwm2m;

// Global flag for clean shutdown
volatile bool running = true;

void signalHandler(int sig) {
    std::cout << "\nShutting down..." << std::endl;
    running = false;
}

int main(int argc, char* argv[]) {
    // Setup signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::cout << "Friendly LwM2M Client v1.1.0" << std::endl;
    std::cout << "============================" << std::endl;

    // Initialize platform
    platform::LinuxPlatform platform;
    if (!platform.initialize()) {
        std::cerr << "Failed to initialize platform" << std::endl;
        return 1;
    }

    // Get device info
    auto deviceInfo = platform.getDeviceInfo();
    std::cout << "Device: " << deviceInfo.manufacturer << " " 
              << deviceInfo.modelNumber << std::endl;
    std::cout << "Serial: " << deviceInfo.serialNumber << std::endl;
    std::cout << "Firmware: " << deviceInfo.firmwareVersion << std::endl;

    // Main loop
    std::cout << "\nClient running. Press Ctrl+C to exit.\n" << std::endl;
    
    while (running) {
        // Process LwM2M operations
        platform.sleep(1000);
    }

    std::cout << "Client stopped." << std::endl;
    return 0;
}
```

### Build Your Client

```bash
# From build directory
g++ -std=c++17 -I../include -L. -o my_client my_client.cpp -lfriendly_lwm2m -lpthread -lssl -lcrypto

# Run
./my_client
```

---

## Connecting to a Server

### Using Leshan Demo Server

[Eclipse Leshan](https://github.com/eclipse/leshan) provides a free demo server for testing.

1. **Start Leshan** (using Docker):
   ```bash
   docker run -p 5683:5683/udp -p 5684:5684/udp -p 8080:8080 \
       eclipsefoundation/leshan:latest
   ```

2. **Access Web UI**: Open http://localhost:8080

3. **Configure client** to connect:
   ```cpp
   // Server configuration
   ServerConfig serverConfig;
   serverConfig.uri = "coap://localhost:5683";
   serverConfig.securityMode = SecurityMode::NO_SEC;  // For testing only!
   serverConfig.lifetime = 300;  // 5 minutes
   ```

### Using Secure Connection (PSK)

```cpp
// PSK Security configuration
ServerConfig serverConfig;
serverConfig.uri = "coaps://lwm2m.example.com:5684";
serverConfig.securityMode = SecurityMode::PSK;
serverConfig.pskIdentity = "my-device-001";
serverConfig.pskKey = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
```

---

## Using MQTT Transport

### Why MQTT?

- Works through firewalls and NAT
- Integrates with existing MQTT infrastructure
- Supports persistent sessions
- Better for high-latency networks

### MQTT Quick Start

```cpp
#include "transport/mqtt_transport.h"

using namespace lwm2m::transport;

int main() {
    // Configure MQTT
    MqttTransport::Config config;
    config.server.brokerUri = "mqtt.example.com";
    config.server.brokerPort = 8883;
    config.server.clientId = "lwm2m-device-001";
    config.server.useTls = true;
    config.server.qos = static_cast<int>(MqttQoS::AT_LEAST_ONCE);
    
    // Security
    config.security.securityMode = MqttSecurityMode::PSK;
    config.security.pskIdentity = "device001";
    config.security.pskKey = "secretkey123";

    // Create transport
    MqttTransport mqtt(config);

    // Set up callbacks
    mqtt.onRead([](const LwM2MPayload& req, auto respond) {
        std::cout << "Read request for: " << req.path << std::endl;
        LwM2MPayload resp;
        resp.responseCode = LwM2MResponseCode::CONTENT;
        resp.data = {0x48, 0x65, 0x6C, 0x6C, 0x6F};  // "Hello"
        respond(resp);
    });

    mqtt.onWrite([](const LwM2MPayload& req, auto respond) {
        std::cout << "Write to: " << req.path << std::endl;
        LwM2MPayload resp;
        resp.responseCode = LwM2MResponseCode::CHANGED;
        respond(resp);
    });

    // Connect
    if (mqtt.connect() != MqttResult::SUCCESS) {
        std::cerr << "Connection failed!" << std::endl;
        return 1;
    }

    // Register with server
    MqttRegistrationParams params;
    params.endpointName = "my-mqtt-device";
    params.lifetime = 86400;
    params.lwm2mVersion = "1.2";
    params.objectLinks = {"</3/0>", "</5/0>"};

    mqtt.registerClient(params);

    // Process messages
    while (true) {
        mqtt.processMessages(1000);
    }

    return 0;
}
```

### Testing with Mosquitto

```bash
# Install Mosquitto
sudo apt-get install mosquitto mosquitto-clients

# Start broker (development mode)
mosquitto -v

# Subscribe to see client messages
mosquitto_sub -t 'lwm2m/#' -v
```

---

## Running Edge AI Inference

### Prerequisites

```bash
# Install TensorFlow Lite (Ubuntu)
sudo apt-get install libtensorflow-lite-dev

# Or build with TFLite support
cmake .. -DWITH_EDGE_AI=ON -DWITH_TFLITE=ON
make -j$(nproc)
```

### Load and Run a Model

```cpp
#include "objects/edge_ai_inference_object.h"

using namespace lwm2m::objects;

int main() {
    // Create Edge AI object
    EdgeAIInferenceObject::Config config;
    config.modelStoragePath = "/var/lib/lwm2m/models";
    config.enableGPU = false;  // CPU only for now
    config.defaultNumThreads = 4;

    EdgeAIInferenceObject ai(0, config);
    ai.initialize();

    // Load a TFLite model
    auto result = ai.loadModel("/path/to/mobilenet_v2.tflite", 
                               ModelFormat::TFLITE);
    
    if (result != ModelLoadResult::SUCCESS) {
        std::cerr << "Failed to load model" << std::endl;
        return 1;
    }

    // Get model info
    auto metadata = ai.getModelMetadata();
    std::cout << "Model: " << metadata.name << std::endl;
    std::cout << "Input shape: " << metadata.inputShapes[0].toString() << std::endl;

    // Prepare input (224x224 RGB image)
    Tensor input = Tensor::create({1, 224, 224, 3}, TensorDataType::FLOAT32);
    
    // Fill with image data...
    float* inputData = input.data<float>();
    // ... load and preprocess your image ...

    // Run inference
    std::vector<Tensor> outputs;
    auto inferResult = ai.runInference({input}, outputs);

    if (inferResult == InferenceResult::SUCCESS) {
        // Decode classification results
        auto classifications = ai.decodeClassification(outputs[0], 5);
        
        std::cout << "\nTop 5 predictions:" << std::endl;
        for (const auto& [label, confidence] : classifications) {
            std::cout << "  " << label << ": " 
                      << (confidence * 100) << "%" << std::endl;
        }
    }

    // Get statistics
    auto stats = ai.getStatistics();
    std::cout << "\nInference stats:" << std::endl;
    std::cout << "  Latency: " << stats.avgLatencyMs << " ms" << std::endl;
    std::cout << "  Memory: " << stats.peakMemoryBytes / 1024 << " KB" << std::endl;

    return 0;
}
```

### Download a Test Model

```bash
# Download MobileNet V2 (TFLite)
wget https://storage.googleapis.com/download.tensorflow.org/models/tflite/mobilenet_v2_1.0_224_quant.tflite

# Download labels
wget https://storage.googleapis.com/download.tensorflow.org/models/tflite/mobilenet_v1_1.0_224/labels.txt
```

---

## Next Steps

### Learn More

| Topic | Guide |
|-------|-------|
| Build options | [BUILD_GUIDE.md](BUILD_GUIDE.md) |
| Full API reference | [API_REFERENCE.md](API_REFERENCE.md) |
| MQTT transport details | [MQTT_TRANSPORT_GUIDE.md](MQTT_TRANSPORT_GUIDE.md) |
| Edge AI deep dive | [EDGE_AI_GUIDE.md](EDGE_AI_GUIDE.md) |
| OpenWRT integration | [OPENWRT_INTEGRATION.md](OPENWRT_INTEGRATION.md) |
| Delta firmware updates | [DELTA_FOTA_GUIDE.md](DELTA_FOTA_GUIDE.md) |

### Example Projects

- `examples/delta_update_example.cpp` - Delta firmware update demo
- `examples/firmware_client_example.cpp` - Full firmware client
- `examples/mqtt_client_example.cpp` - MQTT transport demo
- `examples/edge_ai_example.cpp` - Edge AI inference demo

### Community

- **GitHub Issues**: Report bugs or request features
- **GitHub Discussions**: Ask questions, share ideas
- **Slack**: Join #lwm2m-client for real-time help

---

## Troubleshooting

### Common Issues

#### Build fails with "libssl not found"
```bash
sudo apt-get install libssl-dev
```

#### DTLS handshake fails
- Check PSK key length (must be 16+ bytes)
- Verify server supports your cipher suite
- Try with `NO_SEC` first to isolate network issues

#### MQTT connection refused
- Verify broker is running: `netstat -tlnp | grep 1883`
- Check firewall: `sudo ufw allow 1883`
- Test with mosquitto_pub: `mosquitto_pub -t test -m hello`

#### Model loading fails
- Check file path is absolute
- Verify model format matches extension
- Check available memory: `free -m`

### Debug Logging

```bash
# Enable verbose logging
export LWM2M_LOG_LEVEL=DEBUG

# Run with logging
./my_client 2>&1 | tee client.log
```

---

**Need help?** Open an issue on GitHub or email support@friendly-tech.com

---

*Friendly Technologies - The IoT & Device Management Company*
