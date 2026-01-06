# Friendly LwM2M Client - Feature Specification v1.1

## OMA LwM2M v1.2.1 / v1.2.2 Compliance

**Document Version:** 1.1  
**Generated:** 2026-01-07  
**Friendly Technologies - The IoT & Device Management Company**

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Protocol Version Support](#2-protocol-version-support)
3. [Transport Layer](#3-transport-layer)
4. [MQTT Transport Binding (NEW)](#4-mqtt-transport-binding)
5. [Security](#5-security)
6. [LwM2M Operations](#6-lwm2m-operations)
7. [LwM2M Object Model](#7-lwm2m-object-model)
8. [Delta Firmware Update System](#8-delta-firmware-update-system)
9. [Edge AI Inference (NEW)](#9-edge-ai-inference-object)
10. [Version and Dependency Management](#10-version-and-dependency-management)
11. [Rollback and Recovery System](#11-rollback-and-recovery-system)
12. [Platform Support](#12-platform-support)
13. [Server Gap Analysis Checklist](#13-server-gap-analysis-checklist)

---

## 1. Executive Summary

This document provides a comprehensive specification of all features implemented in the Friendly LwM2M Client v1.1. This release adds two significant new capabilities:

- **MQTT Transport Binding** - Per OMA LwM2M v1.2.1 Section 8, providing an alternative to CoAP for environments where MQTT infrastructure exists
- **Edge AI Inference Object** - Custom object (33410) enabling on-device machine learning inference with TensorFlow Lite and ONNX Runtime support

The Friendly LwM2M Client is an enhanced open-source implementation supporting OMA LwM2M specifications v1.2.1 and v1.2.2, with advanced features including delta firmware updates, resumable block-wise transfers, multi-component version management, and A/B partition rollback support.

---

## 2. Protocol Version Support

### 2.1 Supported LwM2M Versions

| Version | Specification | Support Level |
|---------|---------------|---------------|
| LwM2M v1.0 | OMA-TS-LightweightM2M-V1_0 | Full |
| LwM2M v1.1 | OMA-TS-LightweightM2M-V1_1 | Full |
| LwM2M v1.2.1 | OMA-TS-LightweightM2M_Core-V1_2_1 | Full |
| LwM2M v1.2.2 | OMA-TS-LightweightM2M_Core-V1_2_2 | Full |

**Server Requirement:** Must support version negotiation and handle clients advertising LwM2M v1.2.1 or v1.2.2 in registration.

---

## 3. Transport Layer

### 3.1 Supported Transports

| Transport | Standard | Description |
|-----------|----------|-------------|
| CoAP over UDP | RFC 7252 | Primary constrained transport |
| CoAP over DTLS | RFC 6347 | Secured UDP transport |
| CoAP over TCP | RFC 8323 | Reliable transport for larger payloads |
| CoAP over TLS | RFC 8323 | Secured TCP transport |
| **MQTT over TLS** | **OMA LwM2M v1.2** | **NEW: Pub/sub transport binding** |

### 3.2 CoAP Block-wise Transfer (RFC 7959)

Full implementation of RFC 7959 block-wise transfers for handling large payloads.

#### 3.2.1 Block Size Options

| SZX Value | Block Size (bytes) | Description |
|-----------|-------------------|-------------|
| 0 | 16 | Minimum block size |
| 1 | 32 | Small constrained devices |
| 2 | 64 | Standard constrained |
| 3 | 128 | Default block size |
| 4 | 256 | Enhanced constrained |
| 5 | 512 | Standard IoT |
| 6 | 1024 | Maximum standard block |
| 7 (BERT) | 2048+ | Block-wise Extension for Robust Transport |

#### 3.2.2 Resumable Transfers

- ETag-based transfer identification
- SHA256 checksum verification
- Missing block detection and recovery
- Automatic retry with configurable retransmit count (default: 5)
- Configurable transfer timeout (default: 600 seconds)

---

## 4. MQTT Transport Binding

### 4.1 Overview

New in v1.1, the MQTT transport binding implements OMA-TS-LightweightM2M_Transport-V1_2_1 Section 8, providing MQTT as an alternative transport for LwM2M messaging.

### 4.2 MQTT Protocol Support

| Feature | Support |
|---------|---------|
| MQTT v3.1.1 | Full |
| MQTT v5.0 | Full |
| QoS 0 (At most once) | Full |
| QoS 1 (At least once) | Full |
| QoS 2 (Exactly once) | Full |
| Clean Session | Configurable |
| Session Persistence | Full |
| Last Will and Testament | Supported |

### 4.3 Topic Structure (Section 8.2)

The client uses the following topic structure per the LwM2M specification:

```
{prefix}/{direction}/{endpoint_name}/{operation}
```

| Direction | Purpose | Example Topics |
|-----------|---------|----------------|
| `rd` | Registration | `lwm2m/rd/endpoint123`, `lwm2m/rd/endpoint123/upd` |
| `bs` | Bootstrap | `lwm2m/bs/endpoint123`, `lwm2m/bs/endpoint123/req` |
| `dp` | Data/Commands | `lwm2m/dp/endpoint123/notify`, `lwm2m/dp/endpoint123/resp` |

### 4.4 LwM2M Objects for MQTT

#### MQTT Server Object (ID: 24)

| Resource ID | Resource Name | Operations | Type |
|-------------|---------------|------------|------|
| 0 | Short Server ID | R | Integer |
| 1 | MQTT Broker URI | RW | String |
| 2 | MQTT Broker Port | RW | Integer |
| 3 | Client Identifier | RW | String |
| 4 | Clean Session | RW | Boolean |
| 5 | Keep Alive | RW | Integer |
| 6 | QoS | RW | Integer |
| 7 | Protocol Version | RW | Integer |
| 8 | Username | RW | String |
| 9 | Password | W | String |
| 10 | Topic Prefix | RW | String |
| 11 | Retain | RW | Boolean |
| 12 | Session Expiry (v5.0) | RW | Integer |

#### LwM2M COSE Object (ID: 23)

| Resource ID | Resource Name | Operations | Type |
|-------------|---------------|------------|------|
| 0 | Short Server ID | R | Integer |
| 1 | Security Mode | RW | Integer |
| 2 | Public Key or Identity | RW | Opaque |
| 3 | Secret Key | W | Opaque |
| 4 | Server Public Key | RW | Opaque |
| 5 | Certificate Chain | RW | String |
| 6 | CA Certificate | RW | String |
| 7 | SNI | RW | String |
| 8 | Verify Peer | RW | Boolean |

### 4.5 Message Encoding (Section 8.7)

| Content Format | CoAP ID | Description |
|----------------|---------|-------------|
| CBOR | 60 | Primary encoding for MQTT |
| LwM2M JSON | 11543 | JSON fallback |
| SenML CBOR | 112 | Sensor data encoding |
| SenML JSON | 110 | Sensor data JSON |

### 4.6 Security Modes

| Mode | Description | Use Case |
|------|-------------|----------|
| No Security | Plain MQTT | Development only |
| PSK | Pre-Shared Key over TLS | Simple deployment |
| RPK | Raw Public Key over TLS | Enhanced security |
| Certificate | X.509 over TLS | Enterprise grade |
| Token | OAuth2/JWT | Cloud integration |

**Server Requirement:** Must support MQTT Server Object (24) and COSE Object (23), handle CBOR-encoded payloads, and implement the topic structure defined in Section 8.2.

---

## 5. Security

### 5.1 DTLS/TLS Security Modes

| Mode | Description | Use Case |
|------|-------------|----------|
| No Security | Plain CoAP/MQTT | Development only |
| PSK | DTLS/TLS with symmetric keys | Production - Simple |
| RPK | DTLS/TLS with asymmetric keys | Production - Enhanced |
| Certificate | DTLS/TLS with X.509 | Production - Enterprise |

### 5.2 Supported Cipher Suites

- TLS_PSK_WITH_AES_128_CCM_8
- TLS_PSK_WITH_AES_128_CBC_SHA256
- TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
- TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256

### 5.3 TLS Libraries

- mbedTLS (primary)
- TinyDTLS (constrained devices)
- OpenSSL (optional)

---

## 6. LwM2M Operations

### 6.1 Device Management Operations

| Operation | Direction | Description | Support |
|-----------|-----------|-------------|---------|
| Bootstrap | Server → Client | Initial device provisioning | Full |
| Register | Client → Server | Device registration | Full |
| Update | Client → Server | Registration update | Full |
| De-register | Client → Server | Graceful disconnection | Full |
| Read | Server → Client | Read resource value | Full |
| Discover | Server → Client | Discover object structure | Full |
| Write | Server → Client | Write resource value | Full |
| Write-Attributes | Server → Client | Set notification attributes | Full |
| Execute | Server → Client | Execute resource operation | Full |
| Create | Server → Client | Create object instance | Full |
| Delete | Server → Client | Delete object instance | Full |
| Observe | Server → Client | Subscribe to notifications | Full |
| Cancel Observe | Server → Client | Unsubscribe | Full |
| Send | Client → Server | Client-initiated push | Full (v1.1+) |
| Read-Composite | Server → Client | Multi-resource read | Full (v1.1+) |
| Write-Composite | Server → Client | Multi-resource write | Full (v1.1+) |
| Observe-Composite | Server → Client | Multi-resource observe | Full (v1.1+) |

---

## 7. LwM2M Object Model

### 7.1 Mandatory Objects

| Object ID | Object Name | Instances | Description |
|-----------|-------------|-----------|-------------|
| 0 | LwM2M Security | Multiple | Server credentials |
| 1 | LwM2M Server | Multiple | Server communication |
| 3 | Device | Single | Device information |

### 7.2 Optional Standard Objects

| Object ID | Object Name | Description |
|-----------|-------------|-------------|
| 2 | Access Control | Object access permissions |
| 4 | Connectivity Monitoring | Network status |
| 5 | Firmware Update | Standard firmware update |
| 6 | Location | Device location |
| 21 | OSCORE | Object security (v1.1+) |
| 23 | LwM2M COSE | **NEW: MQTT security** |
| 24 | MQTT Server | **NEW: MQTT configuration** |

### 7.3 Custom Objects

| Object ID | Object Name | Description |
|-----------|-------------|-------------|
| 33405 | Advanced Firmware Update | Delta updates, rollback |
| **33410** | **Edge AI Inference** | **NEW: On-device ML** |

---

## 8. Delta Firmware Update System

### 8.1 Delta Algorithms

| Algorithm | ID | Description | Best For |
|-----------|-----|-------------|----------|
| BSDIFF | 1 | Binary diff (RFC 3284) | General firmware |
| VCDIFF | 2 | Generic diff format | Text-heavy content |
| Courgette | 3 | Optimized for executables | x86/ARM binaries |

### 8.2 Compression Support

| Algorithm | Library | Compression Ratio |
|-----------|---------|-------------------|
| ZLIB | zlib | 60-70% |
| LZMA | liblzma | 70-80% |
| Brotli | brotli | 75-85% |
| Zstd | zstd | 80-90% |

### 8.3 Advanced Firmware Update Object (33405)

| Resource ID | Resource Name | Operations | Type |
|-------------|---------------|------------|------|
| 0 | Package | W | Opaque |
| 1 | Package URI | W | String |
| 2 | Update | E | N/A |
| 3 | State | R | Integer |
| 4 | Update Result | R | Integer |
| 10 | Delta Package | W | Opaque |
| 11 | Delta Package URI | W | String |
| 12 | Delta Algorithm | RW | Integer |
| 15 | Rollback Support | RW | Boolean |
| 16 | Update Progress | R | Integer |

---

## 9. Edge AI Inference Object

### 9.1 Overview

New in v1.1, the Edge AI Inference Object (33410) provides on-device machine learning inference capabilities. This enables IoT devices to run ML models locally for:

- Image classification
- Object detection
- Anomaly detection
- Predictive maintenance
- Natural language processing

### 9.2 Supported ML Frameworks

| Framework | Format | Support |
|-----------|--------|---------|
| TensorFlow Lite | .tflite | Full |
| ONNX Runtime | .onnx | Full |
| PyTorch Mobile | .pt, .ptl | Planned |
| Core ML | .mlmodel | Planned |
| TensorRT | .engine | Planned |

### 9.3 Hardware Acceleration

| Accelerator | Description | Support |
|-------------|-------------|---------|
| CPU | Multi-threaded inference | Full |
| GPU | OpenGL/Vulkan delegates | Full |
| NPU | Neural Processing Unit | Platform-dependent |
| DSP | Digital Signal Processor | Platform-dependent |
| TPU | Edge TPU (Coral) | Planned |

### 9.4 Edge AI Inference Object (33410) Resources

#### Model Management (Resources 0-19)

| Resource ID | Resource Name | Operations | Type | Description |
|-------------|---------------|------------|------|-------------|
| 0 | Model Name | R | String | Loaded model name |
| 1 | Model Version | R | String | Semantic version |
| 2 | Model URI | RW | String | Download URI |
| 3 | Model Format | R | Integer | TFLite=0, ONNX=1, etc. |
| 4 | Model State | R | Integer | IDLE=0, DOWNLOADING=1, READY=4, etc. |
| 5 | Model Size | R | Integer | Size in bytes |
| 6 | Model Checksum | RW | Opaque | SHA256 hash |
| 7 | Load Model | E | - | Execute to load |
| 8 | Unload Model | E | - | Execute to unload |
| 9 | Download Model | E | - | Execute to download |
| 10 | Download Progress | R | Integer | 0-100% |

#### Input/Output Configuration (Resources 20-39)

| Resource ID | Resource Name | Operations | Type | Description |
|-------------|---------------|------------|------|-------------|
| 20 | Input Tensor Info | R | Opaque | JSON tensor specs |
| 21 | Output Tensor Info | R | Opaque | JSON tensor specs |
| 22 | Input Data | W | Opaque | Inference input |
| 23 | Output Data | R | Opaque | Inference result |
| 24 | Labels | R | String | Class labels |

#### Inference Control (Resources 40-59)

| Resource ID | Resource Name | Operations | Type | Description |
|-------------|---------------|------------|------|-------------|
| 40 | Run Inference | E | - | Execute inference |
| 41 | Inference Mode | RW | Integer | SYNC=0, ASYNC=1, STREAM=2 |
| 42 | Accelerator Type | RW | Integer | CPU=0, GPU=1, NPU=2 |
| 43 | Num Threads | RW | Integer | Worker threads |
| 44 | Inference Timeout | RW | Integer | Timeout in ms |
| 45 | Quantization Type | R | Integer | NONE=0, INT8=2, etc. |

#### Statistics and Monitoring (Resources 60-79)

| Resource ID | Resource Name | Operations | Type | Description |
|-------------|---------------|------------|------|-------------|
| 60 | Inference Count | R | Integer | Total inferences |
| 61 | Inference Latency Avg | R | Float | Average latency (ms) |
| 62 | Inference Latency Min | R | Float | Minimum latency |
| 63 | Inference Latency Max | R | Float | Maximum latency |
| 64 | Inference Throughput | R | Float | Inferences/second |
| 65 | Memory Usage | R | Integer | Peak memory bytes |
| 66 | Power Consumption | R | Float | Average power (mW) |
| 67 | Error Count | R | Integer | Failed inferences |
| 68 | Last Error | R | String | Error message |
| 69 | Reset Statistics | E | - | Reset all stats |

#### Pre/Post Processing (Resources 80-99)

| Resource ID | Resource Name | Operations | Type | Description |
|-------------|---------------|------------|------|-------------|
| 80 | Normalize Input | RW | Boolean | Enable normalization |
| 81 | Input Mean | RW | Opaque | Per-channel mean |
| 82 | Input Stddev | RW | Opaque | Per-channel stddev |
| 83 | Apply Softmax | RW | Boolean | Softmax on output |
| 84 | Confidence Threshold | RW | Float | Detection threshold |
| 85 | Top K Results | RW | Integer | Number of results |
| 86 | NMS Enabled | RW | Boolean | Non-max suppression |
| 87 | NMS Threshold | RW | Float | IoU threshold |

#### Advanced Features (Resources 100-119)

| Resource ID | Resource Name | Operations | Type | Description |
|-------------|---------------|------------|------|-------------|
| 100 | Model Metadata | R | String | Full metadata JSON |
| 101 | Supported Formats | R | Opaque | Available formats |
| 102 | Supported Accelerators | R | Opaque | Available backends |
| 103 | Hardware Capabilities | R | String | Device capabilities |
| 104 | Benchmark Result | R | Opaque | Last benchmark data |
| 105 | Run Benchmark | E | - | Execute benchmark |
| 106 | Enable Profiling | RW | Boolean | Layer profiling |
| 107 | Profile Data | R | Opaque | Layer-by-layer timing |

### 9.5 Model State Machine

| State | Value | Description |
|-------|-------|-------------|
| IDLE | 0 | No model loaded |
| DOWNLOADING | 1 | Model being downloaded |
| VALIDATING | 2 | Validating checksum |
| LOADING | 3 | Loading into memory |
| READY | 4 | Ready for inference |
| RUNNING | 5 | Inference in progress |
| ERROR | 6 | Error state |
| UPDATING | 7 | Model being updated |

### 9.6 Inference Result Codes

| Result | Value | Description |
|--------|-------|-------------|
| SUCCESS | 0 | Inference completed |
| ERROR_MODEL_NOT_LOADED | -1 | No model loaded |
| ERROR_INVALID_INPUT | -2 | Bad input data |
| ERROR_INPUT_SHAPE_MISMATCH | -3 | Wrong input shape |
| ERROR_EXECUTION_FAILED | -5 | Inference failed |
| ERROR_TIMEOUT | -6 | Timeout exceeded |
| ERROR_OUT_OF_MEMORY | -7 | Insufficient memory |

**Server Requirement:** Must support custom object 33410 for Edge AI operations, handle model download coordination, and interpret inference results.

---

## 10. Version and Dependency Management

### 10.1 Semantic Versioning

Full semantic versioning support following semver.org specification.

- Format: MAJOR.MINOR.PATCH[-PRERELEASE][+METADATA]
- Example: 1.2.3-beta.1+build.456

### 10.2 Multi-Component Updates

- Dependency resolution between components
- Topological sorting for update order
- Supported components: bootloader, kernel, rootfs, application, config, **model**
- Update priority assignment per component

---

## 11. Rollback and Recovery System

### 11.1 A/B Partition Support

- Dual partition scheme (Slot A / Slot B)
- Active slot tracking
- Bootability verification per slot
- Atomic slot switching
- Write to inactive slot during update
- Automatic fallback on boot failure

### 11.2 Snapshot Management

- Pre-update snapshot creation
- Version-tagged snapshots with timestamps
- State checksum verification (SHA256)
- Automatic pruning of old snapshots

---

## 12. Platform Support

### 12.1 Target Platforms

| Platform | Architecture | Support Level |
|----------|--------------|---------------|
| OpenWRT v24.10.4 | ARM64 / x86_64 | Primary |
| OpenWRT One Router | ARM64 | Reference Target |
| Raspberry Pi 4 | ARM64 | Development |
| Linux (Generic) | x86_64 / ARM64 | Full |
| Custom Embedded | Various | Via SDK |

### 12.2 Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| libcoap | 4.3+ | CoAP protocol |
| mbedTLS | 3.0+ | TLS/DTLS security |
| Wakaama | 1.0+ | LwM2M core |
| ZLIB | 1.2+ | Compression |
| bsdiff | 4.3+ | Delta algorithm |
| **mosquitto** | 2.0+ | **MQTT client** |
| **TensorFlow Lite** | 2.x | **ML inference** |
| **ONNX Runtime** | 1.x | **ML inference** |
| GTest | 1.10+ | Unit testing |

---

## 13. Server Gap Analysis Checklist

### 13.1 MQTT Transport Support (NEW)

| Feature | Required | Priority |
|---------|----------|----------|
| MQTT Server Object (24) | Yes | High |
| LwM2M COSE Object (23) | Yes | High |
| CBOR payload encoding | Yes | Critical |
| Topic structure per Section 8.2 | Yes | Critical |
| MQTT v3.1.1 support | Yes | Critical |
| MQTT v5.0 support | Yes | Medium |
| TLS security modes | Yes | High |

### 13.2 Edge AI Support (NEW)

| Feature | Required | Priority |
|---------|----------|----------|
| Custom Object 33410 support | Yes | High |
| Model download coordination | Yes | High |
| Inference state tracking | Yes | High |
| Statistics reporting | Yes | Medium |
| Multiple model format support | Yes | Medium |

### 13.3 Protocol Support

| Feature | Required | Priority |
|---------|----------|----------|
| LwM2M v1.2.2 registration | Yes | Critical |
| Version negotiation | Yes | Critical |
| Composite operations | Yes | High |
| Send operation | Yes | High |
| Queue mode support | Yes | Medium |

### 13.4 Transport Support

| Feature | Required | Priority |
|---------|----------|----------|
| CoAP over UDP | Yes | Critical |
| CoAP over DTLS | Yes | Critical |
| CoAP over TCP/TLS | Yes | High |
| **MQTT over TLS** | **Yes** | **High** |
| Block-wise transfers | Yes | Critical |
| BERT support | Yes | High |
| Resumable transfers | Yes | High |

### 13.5 Firmware Update Support

| Feature | Required | Priority |
|---------|----------|----------|
| Standard FW Update (5) | Yes | Critical |
| Advanced FW Update (33405) | Yes | High |
| Delta package generation | Yes | High |
| Multi-component updates | Yes | Medium |
| Rollback command support | Yes | Medium |

---

## Appendix A: API Reference Summary

### A.1 MqttTransport Class (NEW)

**Key Methods:**

- `connect()` - Connect to MQTT broker
- `disconnect(graceful)` - Disconnect from broker
- `registerClient(params)` - Send registration
- `updateRegistration(lifetime, binding, objects)` - Update registration
- `deregister()` - Send de-registration
- `sendNotification(path, data, format)` - Send observation notification
- `sendData(paths, data, format)` - Client-initiated data push
- `onRead(callback)` - Handle Read requests
- `onWrite(callback)` - Handle Write requests
- `onExecute(callback)` - Handle Execute requests
- `onObserve(callback)` - Handle Observe requests
- `processMessages(timeout)` - Process incoming messages

### A.2 EdgeAIInferenceObject Class (NEW)

**Key Methods:**

- `loadModel(filePath, format)` - Load model from file
- `loadModelFromMemory(data, format)` - Load model from buffer
- `unloadModel()` - Unload current model
- `runInference(inputs, outputs, config)` - Synchronous inference
- `runInferenceAsync(inputs, callback, config)` - Async inference
- `startStreaming(callback, config)` - Start streaming inference
- `pushStreamingFrame(inputs, frameId)` - Push frame for streaming
- `decodeClassification(output, topK)` - Decode classification results
- `decodeDetection(outputs, threshold, nms)` - Decode detection boxes
- `runBenchmark(iterations, warmup)` - Run performance benchmark
- `getStatistics()` - Get inference statistics
- `getHardwareCapabilities()` - Get device capabilities

### A.3 BlockwiseTransfer Class

**Key Methods:**

- `startTransfer(resource, data, resumable)` - Initiate block transfer
- `resumeTransfer(transferId, etag)` - Resume interrupted transfer
- `receiveBlock(transferId, block, data)` - Process received block
- `getStats(transferId)` - Get transfer statistics
- `enableBERT(enabled)` - Enable/disable BERT

### A.4 RollbackManager Class

**Key Methods:**

- `createSnapshot(version)` - Create firmware snapshot
- `rollback(targetVersion)` - Rollback to version
- `getAvailableSnapshots()` - List available snapshots
- `switchSlot()` - Switch A/B partition
- `isSlotBootable(slot)` - Check partition validity

---

*--- End of Document ---*
