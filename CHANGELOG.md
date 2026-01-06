# Changelog

All notable changes to the Friendly LwM2M Client will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- PyTorch Mobile backend support
- Core ML backend for Apple devices
- TensorRT optimization for NVIDIA platforms
- FreeRTOS port for MCU targets
- WebSocket transport binding

---

## [1.1.0] - 2026-01-07

### Added

#### MQTT Transport Binding (OMA LwM2M v1.2.1 Section 8)
- Full MQTT 3.1.1 and MQTT 5.0 protocol support
- Topic structure per LwM2M specification: `{prefix}/{direction}/{endpoint}/{operation}`
- CBOR and JSON payload encoding with automatic serialization/deserialization
- LwM2M MQTT Server Object (ID: 24) implementation
- LwM2M COSE Object (ID: 23) for MQTT security
- All security modes: PSK, RPK, Certificate, and Token-based authentication
- Complete Bootstrap, Registration, Device Management, and Information Reporting interfaces
- Automatic reconnection with exponential backoff
- QoS 0/1/2 support with message correlation via tokens
- Session persistence and Last Will Testament support
- Statistics and monitoring (messages sent/received, bytes, errors)

#### Edge AI Inference Object (Custom Object 33410)
- TensorFlow Lite backend integration
- ONNX Runtime backend integration
- Hardware acceleration support: CPU, GPU, NPU, DSP delegates
- Complete model lifecycle management: Download → Validate → Load → Inference → Unload
- Synchronous, asynchronous, and streaming inference modes
- Pre-processing pipeline: Normalization, mean subtraction, stddev division
- Post-processing: Softmax, top-K selection, confidence thresholding
- Object detection support with Non-Maximum Suppression (NMS)
- Classification decoding with label mapping
- Comprehensive statistics: latency (avg/min/max), throughput, memory, power
- Benchmarking with warmup and percentile calculations (p50, p95, p99)
- Layer-by-layer profiling support
- Hardware capability detection
- Model format auto-detection from magic bytes and file extension

#### New Unit Tests
- 28 new tests for MQTT Transport
- 41 new tests for Edge AI Inference
- Total test count: 164 unit tests

#### Documentation
- Feature Specification v1.1 with server gap analysis
- MQTT Transport Guide
- Edge AI Integration Guide
- Updated API Reference

### Changed
- CMakeLists.txt updated with new build options: `WITH_MQTT`, `WITH_EDGE_AI`, `WITH_TFLITE`, `WITH_ONNX`
- Dependency detection for Mosquitto, TensorFlow Lite, and ONNX Runtime
- Build summary now displays MQTT and Edge AI feature status

### Fixed
- N/A (new features only)

### Security
- MQTT TLS security with certificate verification
- Model checksum validation (SHA256) before loading

---

## [1.0.0] - 2025-12-15

### Added

#### Core LwM2M Implementation
- OMA LwM2M v1.0, v1.1, v1.2.1, v1.2.2 compliance
- Bootstrap Interface (Client-initiated, Server-initiated)
- Registration Interface (Register, Update, De-register)
- Device Management Interface (Read, Write, Execute, Create, Delete, Discover)
- Information Reporting Interface (Observe, Cancel Observe, Notify)
- Composite operations (Read-Composite, Write-Composite, Observe-Composite)
- Send operation for client-initiated data push

#### Transport Layer
- CoAP over UDP (RFC 7252)
- CoAP over DTLS (RFC 6347)
- CoAP over TCP (RFC 8323)
- CoAP over TLS (RFC 8323)
- Block-wise transfers (RFC 7959) with BERT support
- Resumable transfers with ETag-based recovery
- Configurable block sizes (16 bytes to 8KB with BERT)

#### Security
- DTLS 1.2 with PSK, RPK, and X.509 certificate modes
- mbedTLS integration (primary)
- TinyDTLS integration (constrained devices)
- OpenSSL integration (optional)
- OSCORE support (Object 21)

#### Delta Firmware Updates (Object 33405)
- BSDIFF algorithm implementation
- VCDIFF algorithm implementation
- Courgette algorithm (optimized for executables)
- Compression: ZLIB, LZMA, Brotli, Zstd
- Delta package validation with SHA256
- Progress tracking and state machine
- Multi-component update support

#### A/B Partition Rollback
- Dual partition scheme management
- Active slot tracking
- Bootability verification
- Atomic slot switching
- Automatic fallback on boot failure
- Snapshot management with pruning

#### Platform Support
- Linux (Ubuntu/Debian) support
- OpenWRT 24.10.4 integration
- OpenWRT One Router reference target
- Raspberry Pi 4 support
- Platform abstraction layer

#### LwM2M Objects
- Object 0: Security
- Object 1: Server
- Object 2: Access Control
- Object 3: Device
- Object 4: Connectivity Monitoring
- Object 5: Firmware Update
- Object 6: Location
- Object 21: OSCORE
- Object 33405: Advanced Firmware Update (custom)

#### Build System
- CMake-based build system
- OpenWRT SDK integration
- Cross-compilation support
- IPK package generation
- GTest integration for unit testing

#### Testing
- 95+ unit tests
- Delta algorithm tests
- Blockwise transfer tests
- Rollback manager tests
- Firmware update object tests

### Dependencies
- libcoap 4.3+
- mbedTLS 3.0+ (or TinyDTLS/OpenSSL)
- Wakaama 1.0+
- ZLIB 1.2+
- liblzma 5.2+
- bsdiff 4.3+
- GTest 1.10+

---

## [0.9.0] - 2025-11-01 (Beta)

### Added
- Initial beta release
- Basic LwM2M v1.1 support
- CoAP over UDP/DTLS
- Standard firmware update (Object 5)
- Linux platform support

### Known Issues
- Block-wise transfers not fully implemented
- No rollback support
- Limited error handling

---

## Version History Summary

| Version | Date | Highlights |
|---------|------|------------|
| 1.1.0 | 2026-01-07 | MQTT Transport, Edge AI Inference |
| 1.0.0 | 2025-12-15 | Delta FOTA, A/B Rollback, Full LwM2M v1.2.2 |
| 0.9.0 | 2025-11-01 | Initial beta release |

---

## Upgrade Guide

### From 1.0.x to 1.1.x

1. **Update CMakeLists.txt options** (if using custom build):
   ```cmake
   # New options available
   option(WITH_MQTT "Enable MQTT transport" ON)
   option(WITH_EDGE_AI "Enable Edge AI" ON)
   option(WITH_TFLITE "Enable TensorFlow Lite" OFF)
   option(WITH_ONNX "Enable ONNX Runtime" OFF)
   ```

2. **Install new dependencies** (if using new features):
   ```bash
   # For MQTT
   sudo apt-get install libmosquitto-dev
   
   # For Edge AI
   sudo apt-get install libtensorflow-lite-dev
   ```

3. **Update includes** (if using new features):
   ```cpp
   // MQTT Transport
   #include "transport/mqtt_transport.h"
   
   // Edge AI
   #include "objects/edge_ai_inference_object.h"
   ```

4. **API changes**: None (backward compatible)

5. **Configuration changes**: 
   - New `mqtt` section in config file
   - New `edge_ai` section in config file

---

[Unreleased]: https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client/compare/v1.1.0...HEAD
[1.1.0]: https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client/compare/v0.9.0...v1.0.0
[0.9.0]: https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client/releases/tag/v0.9.0
