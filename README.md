# Friendly LwM2M Client v1.2.2 [<img align="right" height="100px" src="https://github.com/Friendly-Technologies/Friendly-LwM2M-Client/blob/main/logo.png">](https://hubs.li/Q02D_GnB0)

**Version:** 1.2.2 | **LwM2M Specification:** v1.2.2 | **Last Updated:** January 2026

This distribution contains **Friendly LwM2M client** as a reference for how to use Lightweight machine-to-machine implementations for IoT devices.
This code is provided under the associated
[LICENSE](LICENSE)

## Table of Contents

- [Friendly LwM2M Client v1.2.2](#friendly-lwm2m-client-v122)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [What's New in v1.2.2](#whats-new-in-v122)
  - [Documentation](#documentation)
  - [Features](#features)
    - [Supported features](#supported-features)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Setup build environment](#setup-build-environment)
    - [Command line](#command-line)
    - [VS Code](#vs-code)
    - [Configuration](#configuration)
  - [Command-Line Options](#command-line-options)
    - [Core Options](#core-options)
    - [Security Options](#security-options)
    - [CoAP Options](#coap-options)
    - [Application Options](#application-options)
    - [Information Options](#information-options)
    - [Usage Examples](#usage-examples)
    - [Security Mode Notes](#security-mode-notes)
  - [Contributing](#contributing)
  - [License](#license)

## Introduction

Friendly LwM2M Client is an open-source Lightweight machine-to-machine (LwM2M) client implementation designed for IoT devices. LwM2M is a protocol specified by the Open Mobile Alliance (OMA) for remote device management and service enablement for M2M and IoT devices and systems, intended for both resource-constrained and high-performance edge devices.

## What's New in v1.2.2

- **MQTT Transport Binding** - Full OMA LwM2M v1.2.2 Section 8 compliance with MQTT 3.1.1/5.0, CBOR encoding
- **Edge AI Inference** - On-device ML inference with TensorFlow Lite and ONNX Runtime backends
- **Delta Firmware Updates** - Efficient FOTA with BSDIFF, VCDIFF, and Courgette algorithms
- **A/B Partition Rollback** - Reliable firmware updates with automatic rollback on failure
- **New LwM2M Objects**:
  - LwM2M COSE Object (ID: 23) - Security credentials for MQTT
  - MQTT Server Object (ID: 24) - MQTT broker configuration
  - Edge AI Inference Object (ID: 33410) - ML model management and inference

## Documentation

### Getting Started Guides
- **[Quick Start Guide](QUICK_START.md)** - Get up and running in 5 minutes
- **[Getting Started](GETTING_STARTED.md)** - Comprehensive introduction and tutorials
- **[Build Examples](BUILD_EXAMPLES.md)** - Detailed build configurations and examples
- **[Installation Script](scripts/install-ubuntu-prerequisites.sh)** - Automated Ubuntu setup

### Online Documentation
[Friendly LWM2M Client](https://friendly-technologies.github.io/Friendly-LwM2M-Client/)
- [Build Instructions](https://friendly-technologies.github.io/Friendly-LwM2M-Client/build_tag.html)
- [Architecture](https://friendly-technologies.github.io/Friendly-LwM2M-Client/architecture_tag.html)
- [Examples of Usage](https://friendly-technologies.github.io/Friendly-LwM2M-Client/examples_tag.html)
- [Testing](https://friendly-technologies.github.io/Friendly-LwM2M-Client/testing_tag.html)
- [Code Documentation](https://friendly-technologies.github.io/Friendly-LwM2M-Client/code_tag.html)

### Feature Documentation
- [WLAN Connectivity (ID 12)](docs/WLAN_CONNECTIVITY.md) - WiFi interface management
- [Bearer Selection (ID 13)](docs/BEARER_SELECTION.md) - Network bearer selection
- [OpenWRT Integration Guide](docs/OPENWRT_INTEGRATION.md)
- [Implementation Guide](docs/IMPLEMENTATION_GUIDE.md)
- [Wakaama Submodule Setup](WAKAAMA_SUBMODULE_SETUP.md) - Optional WPP objects configuration

## Features

- Lightweight and efficient implementation
- Full LwM2M v1.2.2 specification compliance
- MQTT Transport Binding (OMA Section 8) with CBOR encoding
- Edge AI Inference with TensorFlow Lite and ONNX Runtime
- Delta Firmware Updates (BSDIFF, VCDIFF, Courgette)
- A/B Partition with automatic rollback
- Flexible configuration options
- Secure communication using DTLS and TLS
- Resource management and reporting
- Cross-platform support (Linux, OpenWRT, Raspberry Pi, prplOS)

### Supported features

**LwM2M OMA Objects**

Mandatory Objects:
- LWM2M Security (ID 0)
- LwM2M Server (ID 1)
- Device (ID 3)

Optional OMA Objects:
- LwM2M Access Control (ID 2)
- Connectivity Monitoring (ID 4)
- Firmware Update (ID 5) - Enhanced with delta updates
- Location (ID 6) - GPS with Starlink integration
- WLAN Connectivity (ID 12) - WiFi interface management
- Bearer Selection (ID 13) - Network bearer selection
- LwM2M COSE (ID 23) - MQTT security credentials
- MQTT Server (ID 24) - MQTT broker configuration

Custom Objects (v1.2.0 - Updated Object IDs):
- Advanced Firmware Update (ID 33405) - Delta updates with rollback
- MQTT Server (ID 10512) - MQTT broker configuration
- LwM2M COSE (ID 10513) - Security credentials for MQTT
- Edge AI Inference (ID 10514) - ML model management and inference

Optional WPP Objects (require Wakaama submodule):
- WAN Failover Policy (ID 10525)
- Multi-WAN Health Check (ID 10526)
- WiFi Client Management (ID 10527)
- WiFi Channel Optimization (ID 10528)
- LAN Configuration (ID 10535)
- Routing Table (ID 10536)
- VPN Configuration (ID 10537)

**Note:** See [WAKAAMA_SUBMODULE_SETUP.md](WAKAAMA_SUBMODULE_SETUP.md) for enabling optional WPP objects

**Transport Bindings**
- UDP/DTLS (CoAP)
- MQTT 3.1.1 and MQTT 5.0 with CBOR encoding

**Supported Interfaces & Features**

Bootstrap:
- Request, Finish, Discover, Read, Write, Delete

Registration:
- Register, Update, De-register

Device Management & Service Enablement:
- Read, Discover, Write, Execute, Create, Delete, Send

Information Reporting:
- Observe, Notify, Cancel Observation, Send Operation

Data Formats:
- Plain Text, Opaque, CBOR, TLV, JSON, SenML JSON, SenML CBOR

Security:
- PSK, Raw Public Key, Certificate, No-Sec mode

**Advanced Firmware Update (v1.2.2)**
- Delta algorithms: BSDIFF, VCDIFF, Courgette
- Compression: gzip, bzip2, lzma, zstd, brotli
- A/B partition management with automatic rollback
- Block-wise transfer (RFC 7959)
- Integrity verification with SHA-256

**Edge AI Inference (v1.2.2)**
- TensorFlow Lite backend
- ONNX Runtime backend
- Hardware acceleration: CPU, GPU, NPU, TPU
- Quantization: FP32, FP16, INT8, INT4
- Inference modes: Synchronous, Async, Streaming, Batched

For more information please contact [Friendly Technologies](https://hubs.li/Q02D_GnZ0)

## Getting Started 
This section provides instructions on how to build and run the Friendly LwM2M client. Also, this section will describe the environment requirements, and the setup process for the build, and explain the two methods for building and running the Friendly LwM2M client.
The Friendly LwM2M client can be built and run in two different ways: using VS Code and using the command line.
The method using VS Code is suitable for developing and modifying example parameters, while the command line is suitable for quick building and exploring the capabilities.

Before proceeding with the description, it should also be noted that this guide will cover running an example that demonstrates the Friendly LwM2M client functionality. The client itself is provided as a library that can be built separately.

### Prerequisites

- OS Ubuntu 22.04
- [CMake](https://cmake.org/) version 3.10 or higher
- [OpenSSL](https://www.openssl.org/) for DTLS support
- C/C++ compiler (clang-14, clang++-14)

### Setup build environment

1. Clone the repository:
    ```sh
    git clone --recurse-submodules git@github.com:Friendly-Technologies/Friendly-LwM2M-Client.git
    cd Friendly-LwM2M-Client
    ```

2. Setup environment:
    ```sh
    sh ./wpp_env_setup.sh
    ```

During step **2**, documentation will also be automatically generated and launched in the browser, so for more information on the structure and usage of the Friendly LwM2M client, you can refer to it.

After successfully completing these steps, we will have a fully configured environment with the loaded source files. The next steps are to build the example and run it. As mentioned earlier, this can be done using one of the two methods: via VS Code or the command line. Each method will be detailed further.

### Command line

1. Create build directory:
    ```sh
    mkdir _build
    cd _build
    ```

2. Generate build scripts with CMake:
    ```sh
    cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++
    ```

3. Building:
    ```sh
    cmake --build . --config MinSizeRel --target WppExample -j 14 --
    ```

4. Running:
    ```sh
    cd examples
    ./WppExample
    ```

    For advanced configuration, see [Command-Line Options](#command-line-options) below.

### VS Code

1. Download [Visual Studio Code](https://code.visualstudio.com/download).
2. Start VS Code and open workspace which is set up for this project `File → Open Workspace` from File.
3. In the open window, specify the path to the file `vs-code-wpp.code-workspace` located in the `Friendly-LwM2M-Client` repository.
4. After the workspace has loaded, go to `Extensions` (Ctrl+Shift+x), set `Recommended` (Text field: @recommended) in the extensions filter, and then install all recommended extensions.
5. Let’s move on to the `CMake Tool` extension, the open one contains the entire main interface.
6. Setup `Kit` for `Friendly-LwM2M-Client` project, to do this in the `Configure` section select `Wpp Linux Kit`.
7. Setup `Build Target`, to do this in the `Build` section select `WppExample`.
8. Press `Build` button
9. Launch the client, to do this in the `Launch` section select `WppExample`, and press `Run in Terminal` button.

### Configuration

The generated client example can be configured in two ways:

1. **Command-Line Options** (Runtime) - See [Command-Line Options](#command-line-options) for flexible runtime configuration
2. **Source Code** - Edit `Friendly-LwM2M-Client/examples/objects.cpp` and rebuild

By default, `WppExample` has the following configurations:
1. COAP server: coap://demo-iot.friendly-tech.com:5680
2. Endpoint name: walttech888
3. Bootstrap mode: enabled
4. Security: none (can be changed via CLI to PSK, RPK, or Certificate)
5. Data formats: CBOR, SENML CBOR, SENML JSON, JSON, TLV
6. Objects: DEVICE, LWM2M SERVER, LWM2M SECURITY, CONNECTIVITY MONITORING, LWM2M ACCESS CONTROL, FIRMWARE UPDATE, WLAN CONNECTIVITY, BEARER SELECTION

Note: The example can be configured to enable additional objects including Walt Technologies objects (34600-34608) for specialized OpenWRT router functionality.

## Command-Line Options

The Friendly LwM2M Client supports extensive command-line configuration, allowing runtime customization without rebuilding. All options are optional and have sensible defaults.

### Core Options

| Option | Description | Default |
|--------|-------------|---------|
| `-n, --name=NAME` | LwM2M endpoint identifier | `walttech888` |
| `-u, --uri=URI` | LwM2M server URI (coap:// or coaps://) | `coap://demo-iot.friendly-tech.com:5680` |
| `-p, --port=PORT` | Local UDP port to bind | `56830` |
| `-l, --lifetime=SECONDS` | Registration lifetime in seconds | `25` |
| `-4, --ipv4` | Use IPv4 address family | enabled |
| `-6, --ipv6` | Use IPv6 address family | disabled |
| `-b, --bootstrap` | Enable bootstrap mode | enabled |
| `-B, --no-bootstrap` | Disable bootstrap mode | - |

### Security Options

| Option | Description | Default |
|--------|-------------|---------|
| `-s, --security=MODE` | Security mode: `none`, `psk`, `rpk`, or `cert` | `none` |
| `-i, --psk-identity=ID` | PSK identity string (required for PSK mode) | - |
| `-k, --psk-key=KEY` | PSK key as hex string (required for PSK mode) | - |
| `--rpk-public=KEY` | RPK public key as hex string | - |
| `--rpk-private=KEY` | RPK private key as hex string | - |
| `--cert=FILE` | Certificate file path (PEM format) | - |
| `--key=FILE` | Private key file path (PEM format) | - |
| `--ca=FILE` | CA certificate file path (PEM format) | - |

### CoAP Options

| Option | Description | Default |
|--------|-------------|---------|
| `--block-size=SIZE` | CoAP block size in bytes (16-1024, must be power of 2) | `1024` |

### Application Options

| Option | Description | Default |
|--------|-------------|---------|
| `-v, --verbose` | Increase verbosity (can be repeated: -v, -vv, -vvv) | `0` |
| `-q, --quiet` | Suppress non-error output | disabled |
| `-d, --daemon` | Run as daemon process | disabled |
| `-c, --config=FILE` | Load configuration from file | - |

### Information Options

| Option | Description |
|--------|-------------|
| `-h, --help` | Display help message and exit |
| `-V, --version` | Display version information and exit |

### Usage Examples

#### Default Usage (Backward Compatible)

Run with default settings:

```sh
./WppExample
```

This connects to the default demo server with no security.

#### Custom Endpoint and Server

Connect to a custom server with custom endpoint name:

```sh
./WppExample --name my-device-001 --uri coap://server.example.com:5683
```

#### PSK Security Mode

Use Pre-Shared Key authentication:

```sh
./WppExample --security psk \
  --psk-identity mydevice \
  --psk-key 00112233445566778899aabbccddeeff \
  --uri coaps://secure.example.com:5684
```

Note: PSK mode requires both `--psk-identity` and `--psk-key`. The key must be a valid hex string.

#### Certificate-Based Security

Use X.509 certificate authentication:

```sh
./WppExample --security cert \
  --cert /path/to/device-cert.pem \
  --key /path/to/device-key.pem \
  --ca /path/to/ca-cert.pem \
  --uri coaps://secure.example.com:5684
```

#### IPv6 with Custom Lifetime

Use IPv6 with custom registration lifetime:

```sh
./WppExample --ipv6 \
  --uri coap://[2001:db8::1]:5683 \
  --lifetime 300
```

#### Disable Bootstrap

Connect directly without bootstrap:

```sh
./WppExample --no-bootstrap --uri coap://server.example.com:5683
```

#### Verbose Debug Output

Enable maximum verbosity for troubleshooting:

```sh
./WppExample -vvv
```

Verbosity levels:
- `-v`: Basic debug information
- `-vv`: Detailed debug information
- `-vvv`: Maximum debug output

#### Custom Block Size

Adjust CoAP block transfer size:

```sh
./WppExample --block-size 512
```

Valid block sizes: 16, 32, 64, 128, 256, 512, 1024 bytes

#### Combined Options

Production deployment with PSK, IPv4, custom lifetime:

```sh
./WppExample \
  --name production-sensor-42 \
  --uri coaps://lwm2m.company.com:5684 \
  --security psk \
  --psk-identity sensor-42 \
  --psk-key 0123456789abcdef0123456789abcdef \
  --lifetime 3600 \
  --ipv4 \
  --no-bootstrap \
  --quiet
```

### Security Mode Notes

- **none**: No security, uses plain CoAP (not recommended for production)
- **psk**: Pre-Shared Key mode, requires both identity and key
- **rpk**: Raw Public Key mode, requires public and private keys
- **cert**: Certificate mode, requires certificate, private key, and CA certificate

When using secure modes (`psk`, `rpk`, `cert`), ensure the URI scheme matches:
- Use `coaps://` for secure connections
- Use `coap://` for non-secure connections

The client will warn about scheme/security mode mismatches but will attempt to proceed.

## Contributing

We welcome contributions from the community! Please follow these steps to contribute:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make your changes.
4. Commit your changes (`git commit -m 'Add new feature'`).
5. Push to the branch (`git push origin feature-branch`).
6. Open a pull request.

Please ensure your code adheres to our [coding standards](CODE_OF_CONDUCT.md).

## License

This project is licensed under the following license- see the [LICENSE](LICENSE) file for details.
