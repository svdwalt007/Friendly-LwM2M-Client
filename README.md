# Friendly LwM2M Client

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![OMA LwM2M](https://img.shields.io/badge/OMA%20LwM2M-v1.2.2-blue.svg)](https://www.openmobilealliance.org/release/LightweightM2M/)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20OpenWRT-green.svg)](#platform-support)

**The IoT & Device Management Company**

An enhanced, production-ready LwM2M client implementation supporting OMA LwM2M v1.2.1/v1.2.2 specifications with advanced features including delta firmware updates, MQTT transport binding, and on-device Edge AI inference.

---

## 🚀 Key Features

### Protocol Compliance
- ✅ **OMA LwM2M v1.0 - v1.2.2** full compliance
- ✅ **Bootstrap, Registration, Device Management, Information Reporting** interfaces
- ✅ **Composite operations** (Read/Write/Observe-Composite)
- ✅ **Send operation** for client-initiated data push

### Transport Options
- ✅ **CoAP over UDP/DTLS** - Primary constrained transport
- ✅ **CoAP over TCP/TLS** - Reliable transport (RFC 8323)
- ✅ **MQTT over TLS** - NEW! Pub/sub transport binding (LwM2M v1.2)
- ✅ **Block-wise transfers** with BERT support (RFC 7959)
- ✅ **Resumable transfers** with ETag-based recovery

### Advanced Firmware Updates
- ✅ **Delta updates** - BSDIFF, VCDIFF, Courgette algorithms
- ✅ **Compression** - ZLIB, LZMA, Brotli, Zstd
- ✅ **A/B partition rollback** - Safe updates with automatic recovery
- ✅ **Multi-component updates** - Dependency resolution and ordering
- ✅ **Progress tracking** - Real-time update status

### Edge AI Inference (NEW!)
- ✅ **TensorFlow Lite** - On-device ML inference
- ✅ **ONNX Runtime** - Cross-platform model support
- ✅ **Hardware acceleration** - GPU, NPU, DSP delegates
- ✅ **Model management** - Download, validate, hot-swap
- ✅ **Streaming inference** - Real-time video/audio processing

### Security
- ✅ **DTLS 1.2/1.3** with PSK, RPK, X.509 certificates
- ✅ **OSCORE** application-layer security
- ✅ **Secure boot** integration support
- ✅ **Encrypted storage** for credentials

---

## 📋 Table of Contents

- [Quick Start](#-quick-start)
- [Installation](#-installation)
- [Configuration](#-configuration)
- [Usage Examples](#-usage-examples)
- [Architecture](#-architecture)
- [API Reference](#-api-reference)
- [Platform Support](#-platform-support)
- [Contributing](#-contributing)
- [License](#-license)

---

## 🏃 Quick Start

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libssl-dev libcurl4-openssl-dev \
    zlib1g-dev liblzma-dev libzstd-dev libbrotli-dev libmosquitto-dev

# For Edge AI (optional)
sudo apt-get install libtensorflow-lite-dev  # TensorFlow Lite
# or build ONNX Runtime from source
```

### Build

```bash
git clone https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run tests
make run_tests
```

### Basic Usage

```cpp
#include "lwm2m_client.h"
#include "transport/mqtt_transport.h"
#include "objects/edge_ai_inference_object.h"

int main() {
    // Configure client
    lwm2m::ClientConfig config;
    config.endpointName = "friendly-device-001";
    config.serverUri = "coap://lwm2m.example.com:5684";
    config.securityMode = lwm2m::SecurityMode::PSK;
    
    // Create and start client
    lwm2m::Client client(config);
    client.start();
    
    // Run event loop
    while (running) {
        client.process(1000);
    }
    
    return 0;
}
```

---

## 📦 Installation

### From Source

```bash
# Clone repository
git clone https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client

# Configure with all features
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_MQTT=ON \
    -DWITH_EDGE_AI=ON \
    -DWITH_TFLITE=ON \
    -DWITH_ROLLBACK=ON

# Build
make -j$(nproc)

# Install
sudo make install
```

### OpenWRT Package

```bash
# Add to your OpenWRT feeds
echo "src-git friendly https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git" \
    >> feeds.conf

./scripts/feeds update friendly
./scripts/feeds install friendly-lwm2m-client

# Build package
make package/friendly-lwm2m-client/compile
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `WITH_DTLS` | ON | Enable DTLS support |
| `WITH_MBEDTLS` | ON | Use mbedTLS (vs OpenSSL) |
| `WITH_TINYDTLS` | OFF | Use TinyDTLS for constrained devices |
| `WITH_DELTA_FOTA` | ON | Enable delta firmware updates |
| `WITH_BSDIFF` | ON | Enable BSDIFF algorithm |
| `WITH_VCDIFF` | ON | Enable VCDIFF algorithm |
| `WITH_COURGETTE` | ON | Enable Courgette algorithm |
| `WITH_COMPRESSION` | ON | Enable compression support |
| `WITH_ROLLBACK` | ON | Enable A/B partition rollback |
| `WITH_MQTT` | ON | Enable MQTT transport binding |
| `WITH_EDGE_AI` | ON | Enable Edge AI inference |
| `WITH_TFLITE` | OFF | Enable TensorFlow Lite backend |
| `WITH_ONNX` | OFF | Enable ONNX Runtime backend |
| `BUILD_TESTS` | ON | Build unit tests |
| `BUILD_EXAMPLES` | ON | Build example applications |

---

## ⚙️ Configuration

### Client Configuration File

```json
{
    "endpoint": {
        "name": "friendly-device-001",
        "lifetime": 86400,
        "binding": "UQM"
    },
    "server": {
        "uri": "coaps://lwm2m.example.com:5684",
        "security_mode": "psk",
        "psk_identity": "device001",
        "psk_key": "secret123"
    },
    "mqtt": {
        "enabled": true,
        "broker_uri": "mqtts://mqtt.example.com:8883",
        "client_id": "lwm2m-device-001",
        "topic_prefix": "lwm2m",
        "qos": 1
    },
    "firmware": {
        "delta_enabled": true,
        "algorithms": ["bsdiff", "vcdiff"],
        "compression": ["zstd", "lzma"],
        "rollback_enabled": true,
        "max_retries": 3
    },
    "edge_ai": {
        "enabled": true,
        "model_storage": "/var/lib/lwm2m/models",
        "max_model_size": 104857600,
        "default_accelerator": "auto",
        "num_threads": 4
    }
}
```

### Environment Variables

| Variable | Description |
|----------|-------------|
| `LWM2M_ENDPOINT` | Override endpoint name |
| `LWM2M_SERVER_URI` | Override server URI |
| `LWM2M_PSK_ID` | PSK identity |
| `LWM2M_PSK_KEY` | PSK key (hex-encoded) |
| `LWM2M_LOG_LEVEL` | Log level (DEBUG, INFO, WARN, ERROR) |
| `LWM2M_CONFIG_PATH` | Path to config file |

---

## 💡 Usage Examples

### MQTT Transport

```cpp
#include "transport/mqtt_transport.h"

using namespace lwm2m::transport;

// Configure MQTT
MqttTransport::Config config;
config.server.brokerUri = "mqtt.example.com";
config.server.brokerPort = 8883;
config.server.clientId = "lwm2m-client-001";
config.server.topicPrefix = "lwm2m";
config.security.securityMode = MqttSecurityMode::CERTIFICATE;
config.security.certificateChain = "/etc/lwm2m/client.pem";
config.security.privateKey = "/etc/lwm2m/client.key";

// Create transport
MqttTransport mqtt(config);

// Set callbacks
mqtt.onRead([](const LwM2MPayload& request, auto respond) {
    // Handle read request
    LwM2MPayload response;
    response.responseCode = LwM2MResponseCode::CONTENT;
    response.data = readResource(request.path);
    respond(response);
});

// Connect and register
mqtt.connect();

MqttRegistrationParams params;
params.endpointName = "my-device";
params.lifetime = 86400;
params.objectLinks = {"</3/0>", "</5/0>", "</33410/0>"};

mqtt.registerClient(params);

// Process messages
while (running) {
    mqtt.processMessages(1000);
}
```

### Edge AI Inference

```cpp
#include "objects/edge_ai_inference_object.h"

using namespace lwm2m::objects;

// Configure Edge AI
EdgeAIInferenceObject::Config config;
config.modelStoragePath = "/var/lib/models";
config.enableGPU = true;
config.defaultNumThreads = 4;

// Create object
EdgeAIInferenceObject ai(0, config);
ai.initialize();

// Load model
ai.loadModel("/path/to/model.tflite", ModelFormat::TFLITE);

// Configure preprocessing
PrePostProcessConfig preprocess;
preprocess.normalizeInput = true;
preprocess.mean = {0.485f, 0.456f, 0.406f};
preprocess.stddev = {0.229f, 0.224f, 0.225f};
preprocess.applySoftmax = true;
ai.setPreprocessConfig(preprocess);

// Run inference
TensorShape inputShape;
inputShape.dimensions = {1, 224, 224, 3};
inputShape.dataType = TensorDataType::FLOAT32;

Tensor input = Tensor::create(inputShape);
// ... fill input data ...

std::vector<Tensor> outputs;
InferenceResult result = ai.runInference({input}, outputs);

if (result == InferenceResult::SUCCESS) {
    auto classifications = ai.decodeClassification(outputs[0], 5);
    for (const auto& [label, confidence] : classifications) {
        std::cout << label << ": " << confidence * 100 << "%" << std::endl;
    }
}
```

### Delta Firmware Update

```cpp
#include "firmware/delta_algorithms.h"
#include "firmware/rollback_manager.h"

using namespace lwm2m::firmware;

// Create delta updater
DeltaAlgorithms delta;

// Apply delta patch
auto result = delta.applyPatch(
    "/firmware/current.bin",
    "/tmp/delta.patch",
    "/firmware/new.bin",
    DeltaAlgorithm::BSDIFF
);

if (result == DeltaResult::SUCCESS) {
    // Create rollback snapshot
    RollbackManager rollback;
    rollback.createSnapshot("1.0.0");
    
    // Switch to new firmware
    rollback.switchSlot();
    
    // Reboot
    system("reboot");
}
```

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Application Layer                       │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────────────────┐│
│  │   Objects   │ │  Firmware   │ │      Edge AI            ││
│  │  (3,5,etc)  │ │   Update    │ │  (TFLite, ONNX)         ││
│  │             │ │  (33405)    │ │     (33410)             ││
│  └─────────────┘ └─────────────┘ └─────────────────────────┘│
├─────────────────────────────────────────────────────────────┤
│                     LwM2M Core Engine                        │
│  ┌─────────────────────────────────────────────────────────┐│
│  │  Bootstrap │ Registration │ DevMgmt │ Reporting │ Send  ││
│  └─────────────────────────────────────────────────────────┘│
├─────────────────────────────────────────────────────────────┤
│                    Transport Abstraction                     │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────────────────┐│
│  │    CoAP     │ │    MQTT     │ │     Block-wise          ││
│  │  UDP/DTLS   │ │   over TLS  │ │     Transfers           ││
│  │  TCP/TLS    │ │             │ │                         ││
│  └─────────────┘ └─────────────┘ └─────────────────────────┘│
├─────────────────────────────────────────────────────────────┤
│                      Security Layer                          │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────────────────┐│
│  │   mbedTLS   │ │  TinyDTLS   │ │       OSCORE            ││
│  │   OpenSSL   │ │             │ │                         ││
│  └─────────────┘ └─────────────┘ └─────────────────────────┘│
├─────────────────────────────────────────────────────────────┤
│                    Platform Abstraction                      │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────────────────┐│
│  │   Linux     │ │   OpenWRT   │ │    Embedded RTOS        ││
│  └─────────────┘ └─────────────┘ └─────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
```

---

## 📚 API Reference

### Core Classes

| Class | Description |
|-------|-------------|
| `lwm2m::Client` | Main LwM2M client class |
| `lwm2m::transport::MqttTransport` | MQTT transport binding |
| `lwm2m::transport::BlockwiseTransfer` | Block-wise transfer handler |
| `lwm2m::objects::EdgeAIInferenceObject` | Edge AI inference (33410) |
| `lwm2m::firmware::DeltaAlgorithms` | Delta update algorithms |
| `lwm2m::firmware::RollbackManager` | A/B partition management |

### LwM2M Objects Supported

| Object ID | Name | Description |
|-----------|------|-------------|
| 0 | Security | Server credentials |
| 1 | Server | Server configuration |
| 2 | Access Control | ACL management |
| 3 | Device | Device information |
| 4 | Connectivity Monitoring | Network status |
| 5 | Firmware Update | Standard FOTA |
| 6 | Location | GPS/Location |
| 21 | OSCORE | Object security |
| 23 | LwM2M COSE | MQTT security |
| 24 | MQTT Server | MQTT configuration |
| 33405 | Advanced Firmware | Delta updates |
| 33410 | Edge AI Inference | On-device ML |

For detailed API documentation, see [docs/API_REFERENCE.md](docs/API_REFERENCE.md).

---

## 🖥️ Platform Support

| Platform | Architecture | Status |
|----------|--------------|--------|
| Ubuntu 20.04+ | x86_64, ARM64 | ✅ Fully Supported |
| Debian 11+ | x86_64, ARM64 | ✅ Fully Supported |
| OpenWRT 24.10+ | ARM64, x86_64 | ✅ Fully Supported |
| OpenWRT One Router | ARM64 | ✅ Reference Target |
| Raspberry Pi 4 | ARM64 | ✅ Tested |
| Yocto/Buildroot | Various | 🔧 Community |
| FreeRTOS | ARM Cortex-M | 📋 Planned |

---

## 🧪 Testing

```bash
# Build with tests
cmake .. -DBUILD_TESTS=ON
make

# Run all tests
make run_tests

# Run specific test suite
./test_mqtt_transport
./test_edge_ai_inference
./test_delta_algorithms

# Generate coverage report
cmake .. -DENABLE_COVERAGE=ON
make
make run_tests
make coverage
```

### Test Coverage

| Module | Tests | Coverage |
|--------|-------|----------|
| MQTT Transport | 28 | 85% |
| Edge AI Inference | 41 | 82% |
| Delta Algorithms | 35 | 90% |
| Blockwise Transfer | 25 | 88% |
| Rollback Manager | 20 | 85% |
| Firmware Update | 15 | 80% |
| **Total** | **164** | **85%** |

---

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup

```bash
# Clone with submodules
git clone --recursive https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git

# Install development dependencies
sudo apt-get install clang-format cppcheck doxygen

# Build with sanitizers
cmake .. -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
make

# Format code
make format

# Run static analysis
make lint
```

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- [OMA SpecWorks](https://www.omaspecworks.org/) for the LwM2M specification
- [Eclipse Wakaama](https://github.com/eclipse/wakaama) for LwM2M reference implementation
- [TensorFlow Lite](https://www.tensorflow.org/lite) team for embedded ML
- [ONNX Runtime](https://onnxruntime.ai/) team for cross-platform inference

---

## 📞 Support

- 📧 Email: support@friendly-tech.com
- 🐛 Issues: [GitHub Issues](https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client/issues)
- 💬 Discussions: [GitHub Discussions](https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client/discussions)
- 📖 Documentation: [docs/](docs/)

---

**Friendly Technologies** - *The IoT & Device Management Company*
