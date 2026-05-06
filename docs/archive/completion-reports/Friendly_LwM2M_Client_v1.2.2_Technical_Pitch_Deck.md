# Friendly LwM2M Client v1.2.2 - Technical Pitch Deck

**Target Audience:** R&D Teams
**Presentation Duration:** 20-40 minutes
**Total Slides:** 20
**Generated:** April 24, 2026

---

## Slide 1: Title Slide

**TITLE SLIDE - CENTER ALIGNED**

### Main Title
# Friendly LwM2M Client v1.2.2

### Subtitle
Production-Grade IoT Device Management Platform

### Secondary Subtitle
R&D Team Technical Presentation

### Key Metrics (4-column grid)

| v1.2.2 | OMA | MIT | C++17 |
|:------:|:---:|:---:|:-----:|
| **Release Version** | **LwM2M v1.2.2** | **Open Source** | **Modern Code** |

### Footer
Generated: April 24, 2026

**DESIGN NOTES:**
- Use gradient background (purple to blue)
- Large icons: 🌐📡
- Center all content
- Use bold, modern font

---

## Slide 2: Executive Summary

### Title
## Executive Summary

### Opening Statement
**Friendly LwM2M Client** is a **production-grade**, **open-source** Lightweight Machine-to-Machine (LwM2M) client implementation compliant with **OMA LwM2M v1.2.2** specification.

### Key Features (4-card grid layout)

#### 🔒 Enterprise Security
- Multi-DTLS provider support: mbedTLS, TinyDTLS, OpenSSL, wolfSSL
- PSK/RPK/X.509 authentication
- RFC 9146 Connection ID support
- DTLS 1.2 & 1.3 protocol support

#### 🤖 Edge AI Ready
- On-device ML inference with TensorFlow Lite and ONNX Runtime
- Hardware acceleration (CPU, GPU, NPU, TPU)
- FP32/FP16/INT8/INT4 quantization support
- Sync, async, streaming, batched inference modes

#### 📦 Smart Updates
- Delta firmware updates (BSDIFF/VCDIFF/Courgette)
- 50-70% size reduction
- A/B partition rollback for safe OTA
- Automatic health monitoring and rollback on failure

#### 🌍 Multi-Platform
- Linux, OpenWRT, Raspberry Pi, prplOS support
- Cross-platform abstractions
- Bootloader-agnostic design
- Easy platform porting

### Bottom Metrics (3-column)

| ~14K | 15+ | 4 |
|:----:|:---:|:-:|
| **Lines of Code** | **LwM2M Objects** | **DTLS Providers** |

**DESIGN NOTES:**
- Use feature cards with icons
- Color-code sections (blue, green, orange, purple)
- Large, readable metrics at bottom

---

## Slide 3: What's New in v1.2.2 ⭐

### Title
## What's New in v1.2.2 ⭐

### Major Features

#### 🔐 Multi-DTLS Provider Support (Highlighted Feature Box)

**Runtime-selectable DTLS implementations via factory pattern**

**Supported Providers:**
- **mbedTLS 3.6+** ⭐ (Production default - ~200KB)
- **TinyDTLS 0.8.6** (Constrained devices - ~50KB)
- **OpenSSL 3.x** (Desktop/server - ~500KB)
- **wolfSSL 5.x** (Performance-critical - ~250KB)

**Key Capabilities:**
✓ Factory pattern for clean abstraction
✓ Runtime provider selection via CLI
✓ RFC 9146 CID support (NAT resilience)
✓ DTLS 1.2 & 1.3 support

#### Additional v1.2.2 Features (4-card grid)

**📡 MQTT Transport**
- OMA Section 8 compliant
- MQTT 3.1.1/5.0 binding
- CBOR encoding (30-60% size reduction)

**🧠 Edge AI Inference**
- TensorFlow Lite & ONNX Runtime backends
- Hardware acceleration support
- Quantized models (FP32→INT4)

**⚡ Delta FOTA**
- BSDIFF, VCDIFF, Courgette algorithms
- 50-70% update size reduction
- Multiple compression formats

**🔄 A/B Rollback**
- Automatic partition rollback
- Health checks and boot counter
- Cross-platform support

**DESIGN NOTES:**
- Highlight Multi-DTLS in separate colored box
- Use checkmarks for bullet points
- Star icon (⭐) for emphasis on new features

---

## Slide 4: System Architecture

### Title
## System Architecture

### Architecture Diagram (Monospace font)

```
┌─────────────────────────────────────────────────────────────────────┐
│                   LwM2M Client Application v1.2.2                    │
├─────────────────────────────────────────────────────────────────────┤
│  Custom App  │  Service Daemon  │  CLI Interface  │  Web Dashboard   │
└────────────┬────────────────────────────────────────────────────────┘
             │
    ┌────────▼──────────────────────────────────────────────────┐
    │              WppClient Library (Core Engine)               │
    │  WppClient │ WppRegistry │ WppTaskQueue │ WppConnection   │
    └────────┬───────────────────────────────────────────────────┘
             │
    ┌────────▼─────────────────────────────────────────────┐
    │           Object Registry (15+ Objects)              │
    │ Device(3) │ MQTT(24) │ EdgeAI(33410) │ FW(5) │ Custom│
    └────────┬─────────────────────────────────────────────┘
             │
    ┌────────▼──────────────────────────────────────────┐
    │         Wakaama LwM2M Stack (Submodule)           │
    │ CoAP │ DTLS │ Observe │ Bootstrap │ FOTA │ Block  │
    └────────┬───────────────────────────────────────────┘
             │
    ┌────────┴────────────────────────────────────┐
    │  Advanced Features Layer (v1.2.2)           │
    ├─ Delta Firmware (BSDIFF/VCDIFF/Courgette)  │
    ├─ A/B Rollback Manager                      │
    ├─ Edge AI Inference Engine                  │
    ├─ MQTT Transport Binding (Section 8)        │
    └─────────────────────────────────────────────┘
             │
┌────────────┴──────────────────────────────────────────┐
│         Transport & Network Layer                     │
├───────────────────────────────────────────────────────┤
│  CoAP/DTLS (UDP)  │  MQTT (TCP)  │  HTTP (Future)    │
└───────────────────────────────────────────────────────┘
```

### Key Design Patterns

**Design Pattern Usage:**

- **Factory Pattern** - DTLS provider instantiation
- **Template Pattern** - ObjectImpl<T> base class for object implementations
- **Observer Pattern** - Resource change notifications to LwM2M server
- **Strategy Pattern** - Platform-specific implementations (Linux/OpenWRT/prplOS)
- **Producer-Consumer** - Task queue with worker thread

**Threading Model:**
- Single-threaded, event-driven main loop
- Non-blocking I/O operations
- Thread-safe operations with mutex protection
- Optional background threads for AI inference and delta patching

**DESIGN NOTES:**
- Use monospace font for diagram
- Light background box for code
- Bullet points for patterns

---

## Slide 5: Multi-DTLS Provider Deep Dive

### Title
## Multi-DTLS Provider Architecture

### Subtitle
Runtime-Selectable Security for IoT Devices

### Provider Comparison Table

| Provider | Version | Binary Size | Best For | Highlights |
|----------|---------|-------------|----------|------------|
| **mbedTLS** ⭐ | 3.6+ | ~200KB | Production deployments | Safe, audited, default choice |
| **TinyDTLS** | 0.8.6 | ~50KB | Constrained devices | Minimal footprint, PSK only |
| **OpenSSL** | 3.x | ~500KB | Desktop/server environments | DTLS 1.3, full X.509 support |
| **wolfSSL** | 5.x | ~250KB | Performance-critical applications | Hardware accel, DTLS 1.3 |

### Authentication Support Matrix

| Authentication | TinyDTLS | mbedTLS | OpenSSL | wolfSSL |
|----------------|----------|---------|---------|---------|
| PSK Mode | ✓ | ✓ | ✓ | ✓ |
| Certificate (X.509) | ✗ | ✓ | ✓ | ✓ |
| Raw Public Key (RPK) | ✗ | ✓ | ✓ | ✓ |
| DTLS 1.3 | ✗ | 🚧 | ✓ | ✓ |
| RFC 9146 CID | ✓ | ✓ | ⚠️ | ✓ |

**Legend:** ✓ = Supported | ✗ = Not supported | 🚧 = In progress | ⚠️ = Partial support

### Usage Example (Code Block)

```bash
# Runtime provider selection
./friendly_example_client --dtls-provider=mbedtls \
  --security cert \
  --cert device.pem \
  --key device-key.pem \
  --ca ca.pem \
  --uri coaps://server:5684

# List available providers
./friendly_example_client --list-dtls-providers
```

### Key Benefits

✓ **Choice** - Select the best DTLS library for your use case
✓ **Production-Ready** - mbedTLS provides production-grade security
✓ **Certificate Support** - X.509 certificates with mbedTLS/OpenSSL/wolfSSL
✓ **DTLS 1.3** - Latest protocol version with OpenSSL/wolfSSL
✓ **Runtime Selection** - Switch providers via command-line option

**DESIGN NOTES:**
- Two tables side-by-side if possible
- Code block with dark background
- Star (⭐) for default provider
- Use checkmarks and crosses in tables

---

## Slide 6: MQTT Transport Binding

### Title
## MQTT Transport Binding

### Subtitle
OMA LwM2M v1.2.2 Section 8 Compliant

### Key Features (4-card grid)

#### Protocol Support
- **MQTT 3.1.1** and **MQTT 5.0**
- Eclipse Paho C/C++ client integration
- Full specification compliance

#### CBOR Encoding
- **RFC 8949** compliant encoding
- **30-60% size reduction** vs JSON
- Fast parsing and serialization

#### QoS Support
- **QoS 0** - At most once
- **QoS 1** - At least once
- **QoS 2** - Exactly once

#### Message Queue
- Offline queue management
- **1000 max pending messages**
- Exponential backoff reconnection

### Topic Structure (Code Block)

```
Format: {prefix}/{direction}/{endpoint}[/{operation}]

Examples:
  lwm2m/req/device-001/register      # Registration request
  lwm2m/req/device-001/update        # Update registration
  lwm2m/req/device-001/notify        # Notification
  lwm2m/rsp/device-001/register      # Registration response
  lwm2m/req/device-001/op            # Composite operations
```

### CBOR Encoding Benefits Table

| Feature | JSON | CBOR | Savings |
|---------|------|------|---------|
| Payload Size | 1000 bytes | 400-700 bytes | **30-60%** |
| Parse Speed | Baseline | 2-3x faster | - |
| Binary Data | Base64 encoded | Native binary | **33% smaller** |
| Type Safety | String-based | Typed encoding | Better validation |

### Supported CBOR Types

- UINT variants (1, 2, 4, 8 bytes)
- Byte strings, text strings
- Arrays, maps (nested structures)
- Boolean and null values
- Compact integer representation

**DESIGN NOTES:**
- Feature cards in 2x2 grid
- Code block for topic examples
- Highlight savings percentages in bold

---

## Slide 7: Delta Firmware Update System

### Title
## Delta Firmware Update System

### Subtitle
Efficient FOTA with Multiple Algorithms

### Supported Algorithms Table

| Algorithm | Type | Compression | Speed | Best For | Size Reduction |
|-----------|------|-------------|-------|----------|----------------|
| **BSDIFF** | Binary diff | Yes | Medium | General binaries | **50-70%** |
| **VCDIFF** | RFC 3284 standard | Yes | Medium | Standard compliance | **~60%** |
| **Courgette** | Executable-aware | Yes | Fast | Executables (Google) | **~70%** |

### Compression Support

**Always Available:**
- gzip
- bzip2
- lzma

**Optional (Compile-time):**
- zstd
- brotli

### Update Size Reduction Metrics (3-column display)

| 50-70% | 60% | 70% |
|:------:|:---:|:---:|
| **BSDIFF Reduction** | **VCDIFF Reduction** | **Courgette Reduction** |

### Additional Compression (Stacked)

| 30-60% | 70-90% |
|:------:|:------:|
| **Further Compression** | **Combined Reduction** |

### Key Features Checklist

✓ **Incremental updates** - Delta payloads instead of full binaries
✓ **Block-wise transfer** - RFC 7959 for resumable downloads
✓ **SHA-256 verification** - Integrity checking on all payloads
✓ **Concurrent algorithms** - Support multiple algorithms simultaneously
✓ **Memory-efficient** - Streaming patch application
✓ **Network optimization** - Reduce bandwidth by 50-90%

### Use Case Example

```
Firmware Update: v1.0.0 → v1.1.0

Full Binary:        10 MB
BSDIFF Delta:        3 MB (70% reduction)
+ gzip compression:  1 MB (90% total reduction)

Network savings:     9 MB per device
Fleet of 10,000:    90 GB bandwidth saved
```

**DESIGN NOTES:**
- Use bold percentages for impact
- Show compression stacking visually
- Include practical example with calculations

---

## Slide 8: A/B Partition Rollback Manager

### Title
## A/B Partition Rollback Manager

### Subtitle
Production-Grade Firmware Update Safety

### Safety Highlight Box

🛡️ **Production-Grade Safety**

Automatic snapshot creation, boot counter tracking, health monitoring, and rollback on failure

### Platform Support Table

| Platform | Bootloader | Detection Method | Status |
|----------|------------|------------------|--------|
| Generic Linux | GRUB/U-Boot | MTD devices, /proc/cmdline | ✓ Full Support |
| OpenWRT | U-Boot | UCI bootloader config | ✓ Full Support |
| prplOS | U-Boot | U-Boot environment variables | ✓ Full Support |

### Rollback Workflow (Step-by-step diagram)

```
┌─────────────────────────────────────────────────────────────┐
│ 1. Pre-Update Snapshot                                      │
│    └─ Capture current system state                          │
│    └─ Store metadata (version, timestamp, checksum)         │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│ 2. Apply Firmware Update                                    │
│    └─ Download delta patch                                  │
│    └─ Verify SHA-256 integrity                              │
│    └─ Apply patch to inactive partition                     │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│ 3. Boot Counter Tracking                                    │
│    └─ Increment boot attempt counter                        │
│    └─ Max attempts: configurable (default 3)                │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│ 4. Health Check Post-Boot                                   │
│    └─ Verify services running                               │
│    └─ Check network connectivity                            │
│    └─ Validate system resources                             │
└─────────────────────────────────────────────────────────────┘
                            ↓
                ┌───────────┴──────────┐
                │                      │
         ✓ PASS                   ✗ FAIL
                │                      │
                ↓                      ↓
┌───────────────────────┐  ┌─────────────────────────────────┐
│ 6. Commit Update      │  │ 5. Auto-Rollback               │
│  └─ Mark stable       │  │  └─ Switch to previous snapshot │
│  └─ Reset counter     │  │  └─ Reset boot counter          │
│  └─ Create snapshot   │  │  └─ Reboot to last known good   │
└───────────────────────┘  └─────────────────────────────────┘
```

### Key Features Checklist

✓ **Thread-safe operations** - Mutex protection for concurrent access
✓ **Configurable max snapshots** - Default: 10, customizable
✓ **Multi-slot partition management** - Support for 2+ partitions
✓ **Prevents infinite boot loops** - Max boot attempt enforcement
✓ **Cross-platform abstractions** - Works on Linux, OpenWRT, prplOS
✓ **Automatic recovery** - No manual intervention required

**DESIGN NOTES:**
- Use flow diagram with boxes and arrows
- Highlight success/fail paths with colors (green/red)
- Safety icon in header box

---

## Slide 9: Edge AI Inference Engine

### Title
## Edge AI Inference Engine

### Subtitle
On-Device Machine Learning for IoT

### ML Backend Support Table

| Backend | Framework | Input Format | Hardware Acceleration |
|---------|-----------|--------------|----------------------|
| **TensorFlow Lite** | Keras/TensorFlow | .tflite | CPU, GPU, NPU, TPU |
| **ONNX Runtime** | ONNX (cross-framework) | .onnx | CPU, GPU, DirectML, NNAPI |

### Quantization Support (4-column metrics)

| FP32 | FP16 | INT8 | INT4 |
|:----:|:----:|:----:|:----:|
| **Full Precision** | **Half Precision** | **8-bit Quantized** | **Ultra-Lightweight** |
| Baseline accuracy | 2x faster, ~same accuracy | 4x faster, -1% accuracy | 8x faster, -3% accuracy |

### Inference Modes (4-card grid)

#### Synchronous
- Blocking inference
- Immediate results
- Simple API
- Best for: Single predictions

#### Asynchronous
- Non-blocking operation
- Future/promise pattern
- Parallel processing
- Best for: Background tasks

#### Streaming
- Continuous input processing
- Real-time data streams
- Low latency
- Best for: Video, audio, sensors

#### Batched
- Multiple samples per inference
- Throughput optimization
- Efficient GPU usage
- Best for: Bulk processing

### Inference Pipeline Diagram

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│ Preprocess   │────▶│  Inference   │────▶│ Postprocess  │
└──────────────┘     └──────────────┘     └──────────────┘
       │                    │                     │
   ├ Normalize          ├ Sync               ├ Softmax
   ├ Resize            ├ Async              ├ Top-K
   ├ Quantize          ├ Stream             ├ NMS
   └ Augment           └ Batch              └ Threshold
```

### Use Cases

**Industrial IoT:**
- Anomaly detection in machinery
- Predictive maintenance
- Quality inspection

**Smart Home:**
- Person detection
- Activity recognition
- Voice commands (wake word)

**Agriculture:**
- Crop disease detection
- Pest identification
- Yield prediction

**Automotive:**
- ADAS data processing
- Driver monitoring
- Parking assistance

**DESIGN NOTES:**
- Show quantization tradeoffs clearly
- Use 4-card grid for inference modes
- Include practical use cases

---

## Slide 10: LwM2M Object Implementations

### Title
## LwM2M Object Implementations

### Subtitle
15+ Objects for Comprehensive Device Management

### Mandatory Objects Table

| Object ID | Name | Purpose | Instances |
|-----------|------|---------|-----------|
| **0** | LwM2M Security | DTLS credentials, server URIs, security modes | 1-N |
| **1** | LwM2M Server | Server configuration, lifetime, binding | 1-N |
| **2** | Access Control | Resource permissions, ACL management | Variable |
| **3** | Device | Device information, reboot, factory reset | 1 (Singleton) |

### Standard Optional Objects (Implemented)

| Object ID | Name | Key Resources | Instances |
|-----------|------|---------------|-----------|
| **4** | Connectivity Monitoring | Signal strength, bearer type, IP addresses, link quality | 1 |
| **5** | Firmware Update | Package URI, update state, execute update | 1 |
| **6** | Location | Latitude, longitude, altitude, velocity, timestamp | 1 |
| **12** | WLAN Connectivity | SSID, signal strength, interface list, security mode | Variable |
| **13** | Bearer Selection | Bearer list, preferred bearer, acceptable RSSI | Variable |
| **23** | LwM2M COSE | Security credentials for MQTT transport | 1 |
| **24** | MQTT Server | Broker URI, port, client ID, QoS, topic prefix | 1 |

### Advanced Custom Objects (v1.2.2)

| Object ID | Name | Key Resources | Status |
|-----------|------|---------------|--------|
| **33410** | Edge AI Inference | Model name, inference request, results, quantization level | ✓ Full Implementation |
| **33405** | Advanced Firmware Update | Delta algorithm selection, compression type, rollback control | ✓ Full Implementation |

### Router Management Objects (Optional - Requires Wakaama Submodule)

| Object ID | Name | CMake Flag | Purpose |
|-----------|------|------------|---------|
| **10525** | WAN Failover Policy | `WITH_WAN_FAILOVER` | Multi-WAN failover configuration |
| **10526** | Multi-WAN Health Check | `WITH_MULTIWAN_HEALTH` | Health check parameters for WAN links |
| **10527** | WiFi Client Management | `WITH_WIFI_CLIENT_MGMT` | WiFi client connection management |
| **10528** | WiFi Channel Optimization | `WITH_WIFI_CHANNEL_OPT` | Automatic channel selection |
| **10535** | LAN Configuration | `WITH_LAN_CONFIG` | LAN interface configuration |
| **10536** | Routing Table | `WITH_ROUTING_TABLE` | Dynamic routing table management |
| **10537** | VPN Configuration | `WITH_VPN_CONFIG` | VPN tunnel configuration |

### Object Statistics

- **Mandatory Objects:** 4
- **Standard Optional:** 7
- **Custom Advanced:** 2
- **Router Management:** 7 (optional)
- **Total Available:** 15+

**DESIGN NOTES:**
- Use multiple tables to categorize objects
- Color-code mandatory vs optional vs custom
- Include implementation status

---

## Slide 11: Transport Protocols & Data Formats

### Title
## Transport Protocols & Data Formats

### Subtitle
Multi-Protocol Support for Diverse Deployments

### Transport Bindings (2-card grid)

#### CoAP/DTLS (Primary Transport)
**Standards:**
- RFC 7252 (CoAP)
- RFC 6347 (DTLS 1.2)
- RFC 9147 (DTLS 1.3)
- RFC 7959 (Block-wise transfer)

**Features:**
- Block-wise transfer for large payloads
- Observe/notify for resource subscriptions
- Configurable block size: 16-1024 bytes
- Request/response and pub/sub patterns

#### MQTT (OMA Section 8 Compliant)
**Standards:**
- MQTT 3.1.1
- MQTT 5.0
- RFC 8949 (CBOR encoding)

**Features:**
- QoS levels: 0, 1, 2
- Retained messages
- 1000 max pending messages
- Exponential backoff reconnection

### Supported Data Formats Table

| Format | Description | Use Case | Payload Efficiency |
|--------|-------------|----------|-------------------|
| **Plain Text** | UTF-8 text strings | Simple values, human-readable | Low |
| **Opaque** | Binary data | Firmware images, certificates | High |
| **CBOR** | RFC 8949 binary encoding | Compact, fast parsing | Very High |
| **TLV** | Type-Length-Value | Efficient multi-resource encoding | High |
| **JSON** | JavaScript Object Notation | Debug, web integration | Low |
| **SenML JSON** | Sensor Markup Language (JSON) | Sensor data, time-series | Medium |
| **SenML CBOR** | Sensor Markup Language (CBOR) | Compact sensor data | Very High |

### Security Modes

| Mode | Description | Use Case | Transport |
|------|-------------|----------|-----------|
| **No Security** | Plain CoAP | Development/testing only | CoAP (UDP) |
| **PSK** | Pre-Shared Key | Constrained devices, symmetric crypto | CoAPS (DTLS) |
| **RPK** | Raw Public Key | Public key crypto without PKI | CoAPS (DTLS) |
| **Certificate** | X.509 certificates | Enterprise deployments, full PKI | CoAPS (DTLS) |
| **TLS** | Transport Layer Security | MQTT secure transport | MQTTS (TLS) |

### Block-wise Transfer Performance

| Block Size | Throughput | Latency | Best For |
|------------|------------|---------|----------|
| 16 bytes | Low | High overhead | Very constrained networks |
| 128 bytes | Medium | Medium | NB-IoT, LoRaWAN |
| 512 bytes | Good | Low | LTE-M, 3G |
| **1024 bytes** ⭐ | **Best** | **Minimal** | **WiFi, Ethernet (default)** |

**DESIGN NOTES:**
- Split transport and formats into sections
- Table for data formats with efficiency ratings
- Highlight default block size

---

## Slide 12: Platform Support & Integrations

### Title
## Platform Support & Integrations

### Subtitle
Cross-Platform IoT Solution

### Operating System Support Table

| Platform | Support Level | Build Target | Notes |
|----------|---------------|--------------|-------|
| **Linux** | ✓ Full | Ubuntu 22.04 LTS | Primary development platform, full testing |
| **OpenWRT** | ✓ Full | Custom OpenWrt builds | Extended integration, LUCI dashboard, UCI config |
| **Raspberry Pi** | ✓ Full | ARM32/ARM64 | Tested on Raspberry Pi 4, GPIO support |
| **prplOS** | ✓ Full | TR-069/TR-181 | A/B bootloader support, TR-069 interop |
| **Windows** | ⚠️ Partial | MinGW/MSVC | CoAP only, no platform-specific objects |

### Optional Integration Ecosystems

| Integration | CMake Flag | Status | Description |
|-------------|------------|--------|-------------|
| **Starlink** | `WITH_STARLINK` | ✓ Complete | gRPC integration for Starlink satellite data collection |
| **Matter/Thread** | `WITH_MATTER` | ✓ Complete | Matter SDK integration, Thread border router support |
| **Zigbee** | `WITH_ZIGBEE` | ✓ Complete | Zigbee coordinator (EZSP & Z-Stack frameworks) |
| **prpl/EasyMesh** | `WITH_PRPL` | ✓ Complete | prplWRT integration, EasyMesh R2/R4 support |

### Cross-Platform Features

**Platform Abstraction Layer:**
- ✓ Bootloader abstractions (GRUB, U-Boot)
- ✓ Partition management (MTD, UCI, U-Boot env)
- ✓ Network interface detection
- ✓ System information collection
- ✓ Easy to port - clean separation of concerns

**Build System:**
- CMake 3.16+ for cross-platform builds
- Conditional compilation for platform features
- Toolchain files for cross-compilation
- Package generation (deb, ipk, rpm)

### Deployment Scenarios

**Edge Gateways:**
- Raspberry Pi gateway for home automation
- Industrial gateway for factory IoT
- Telecom CPE for network management

**Embedded Devices:**
- OpenWRT routers with LwM2M management
- prplOS gateways with TR-069 bridge
- Custom embedded Linux devices

**Protocol Bridges:**
- LwM2M ↔ MQTT bridge
- LwM2M ↔ Matter/Thread bridge
- LwM2M ↔ Zigbee coordinator

**DESIGN NOTES:**
- Use checkmarks and warning symbols in tables
- Highlight platform abstraction benefits
- Show deployment scenario variety

---

## Slide 13: Build System & Configuration

### Title
## Build System & Configuration

### Subtitle
Flexible CMake-Based Build System

### Core CMake Configuration

```cmake
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=17 \
  -DCMAKE_C_COMPILER=/usr/bin/clang \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++
```

### DTLS Provider Configuration

```cmake
# DTLS Providers (Enable/Disable)
-DDTLS_PROVIDER_MBEDTLS=ON          # Default: ON
-DDTLS_PROVIDER_TINYDTLS=ON         # Default: ON
-DDTLS_PROVIDER_OPENSSL=OFF         # Default: OFF
-DDTLS_PROVIDER_WOLFSSL=OFF         # Default: OFF
-DDTLS_DEFAULT_PROVIDER=mbedtls     # Runtime default
-DDTLS_STATIC_LINK=ON               # Static linking
```

### Advanced Features Configuration

```cmake
# Advanced Features
-DWITH_DELTA_FOTA=ON                # Delta firmware updates
-DWITH_MQTT=ON                      # MQTT transport binding
-DWITH_EDGE_AI=ON                   # Edge AI inference
-DWITH_TFLITE=OFF                   # TensorFlow Lite backend
-DWITH_ONNX=OFF                     # ONNX Runtime backend
-DWITH_ROLLBACK=ON                  # A/B partition rollback
```

### Optional Objects Configuration

```cmake
# Router Management Objects (Require Wakaama submodule)
-DWITH_WAN_FAILOVER=OFF
-DWITH_MULTIWAN_HEALTH=OFF
-DWITH_WIFI_CLIENT_MGMT=OFF
-DWITH_WIFI_CHANNEL_OPT=OFF
-DWITH_LAN_CONFIG=OFF
-DWITH_ROUTING_TABLE=OFF
-DWITH_VPN_CONFIG=OFF
```

### Quality & Testing Configuration

```cmake
# Build Targets
-DBUILD_TESTS=ON                    # Unit tests
-DBUILD_EXAMPLES=ON                 # Example applications
-DBUILD_DOCS=ON                     # Doxygen documentation

# Quality Assurance
-DENABLE_COVERAGE=ON                # Code coverage (gcov/lcov)
-DENABLE_SANITIZERS=ON              # Address & undefined behavior sanitizers
```

### Required Dependencies

| Dependency | Version | Purpose |
|------------|---------|---------|
| CMake | 3.16+ | Build system |
| C++ Compiler | clang-14+, gcc-9+ | Compilation |
| OpenSSL | Latest | Cryptography |
| ZLIB | Latest | Compression |
| libcurl | Latest | HTTP downloads |
| pthreads | - | Threading support |

### Optional Dependencies Table

| Dependency | Version | Purpose | CMake Flag |
|------------|---------|---------|------------|
| mbedTLS | 3.6+ | DTLS provider (default) | `DTLS_PROVIDER_MBEDTLS` |
| TinyDTLS | 0.8.6 | DTLS provider (lightweight) | `DTLS_PROVIDER_TINYDTLS` |
| OpenSSL | 3.x | DTLS provider (full-featured) | `DTLS_PROVIDER_OPENSSL` |
| wolfSSL | 5.x | DTLS provider (performance) | `DTLS_PROVIDER_WOLFSSL` |
| Eclipse Paho | Latest | MQTT transport | `WITH_MQTT` |
| TensorFlow Lite | Latest | ML inference | `WITH_TFLITE` |
| ONNX Runtime | Latest | ML inference | `WITH_ONNX` |
| BZip2, LZMA | Latest | Compression | `WITH_DELTA_FOTA` |
| zstd, Brotli | Latest | Compression (optional) | `WITH_DELTA_FOTA` |
| Wakaama | 2305 | LwM2M stack | Submodule (required) |

**DESIGN NOTES:**
- Use code blocks with syntax highlighting
- Group configuration by category
- Show dependencies in tables

---

## Slide 14: Testing & Quality Assurance

### Title
## Testing & Quality Assurance

### Subtitle
Production-Ready Code Quality

### Test Suite Coverage

| Test Suite | Purpose | Status |
|------------|---------|--------|
| `test_delta_algorithms` | BSDIFF, VCDIFF, Courgette delta patch generation/application | ✓ Complete |
| `test_blockwise_transfer` | RFC 7959 block-wise transfer protocol compliance | ✓ Complete |
| `test_rollback_manager` | A/B partition rollback logic and state management | ✓ Complete |
| `test_firmware_update_object` | FOTA state machine and resource operations | ✓ Complete |
| `test_dtls_providers` | Multi-provider DTLS functionality and API compatibility | ✓ Complete |
| `test_dtls_integration` | End-to-end DTLS handshakes across all providers | ✓ Complete |
| `test_cid_extensions` | RFC 9146 Connection ID support and encoding | ✓ Complete |
| `test_cid_record_format` | CID record encoding/decoding validation | ✓ Complete |
| `test_advanced_firmware_update` | Complete FOTA pipeline with delta updates | ✓ Complete |
| `test_edge_ai_inference` | Model loading, quantization, inference execution | ✓ Complete |

### Code Quality Measures

**Compiler Flags:**
```bash
-Wall                    # All warnings
-Wextra                  # Extra warnings
-Wpedantic               # Strict ISO C++ compliance
-fsanitize=address       # Address sanitizer (memory errors)
-fsanitize=undefined     # Undefined behavior sanitizer
--coverage               # Code coverage instrumentation
-O3                      # Optimization (Release builds)
-g                       # Debug symbols
```

**Static Analysis:**
- ✓ C++ Core Guidelines compliance
- ✓ No deprecated function usage
- ✓ Modern C++17 idioms throughout
- ✓ Smart pointers (unique_ptr, shared_ptr)
- ✓ RAII for resource management
- ✓ Const correctness enforcement

### Coverage Tools & Workflow

```bash
# Build with coverage
cmake .. -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run all tests
ctest --output-on-failure

# Generate coverage report
make coverage

# View HTML report
xdg-open coverage/index.html
```

**Tools Used:**
- **lcov** - Code coverage instrumentation
- **genhtml** - HTML report generation
- **gcov** - GNU coverage tool
- **CMake CTest** - Test execution framework

### Quality Metrics (3-column display)

| ~14K | 9+ | 100% |
|:----:|:--:|:----:|
| **Lines of Code** | **Test Suites** | **Critical Path Coverage** |

### Testing Best Practices

✓ **Unit Tests** - Individual component testing
✓ **Integration Tests** - End-to-end workflow testing
✓ **Protocol Compliance** - RFC and OMA spec validation
✓ **Memory Safety** - AddressSanitizer for leak detection
✓ **Undefined Behavior** - UBSanitizer for UB detection
✓ **Continuous Testing** - All tests run on every commit

### Code Review Standards

- All code reviewed before merge
- Automated CI/CD checks
- No warnings allowed in production builds
- Test coverage requirements enforced
- Documentation updated with code changes

**DESIGN NOTES:**
- Comprehensive test table
- Show compiler flags in code block
- Metrics prominently displayed

---

## Slide 15: Command-Line Interface

### Title
## Command-Line Interface

### Subtitle
Flexible Runtime Configuration

### Basic Usage

```bash
./friendly_example_client [options]
```

### Key Command-Line Options Table

| Option | Description | Default |
|--------|-------------|---------|
| `-n, --name=NAME` | LwM2M endpoint identifier | `walttech888` |
| `-u, --uri=URI` | LwM2M server URI (coap:// or coaps://) | `coap://demo-iot.friendly-tech.com:5680` |
| `-p, --port=PORT` | Local UDP port to bind | `56830` |
| `-s, --security=MODE` | Security mode: none/psk/rpk/cert | `none` |
| `--dtls-provider=NAME` | DTLS provider selection | `mbedtls` |
| `-l, --lifetime=SECONDS` | Registration lifetime | `25` |
| `-v, --verbose` | Increase verbosity (-v, -vv, -vvv) | `0` |
| `--block-size=SIZE` | CoAP block size (16-1024) | `1024` |
| `-4, --ipv4` | Use IPv4 address family | Enabled |
| `-6, --ipv6` | Use IPv6 address family | Disabled |
| `-b, --bootstrap` | Enable bootstrap mode | Enabled |

### Usage Examples

#### Default Usage (Backward Compatible)
```bash
./friendly_example_client
```
Connects to demo server with default settings.

#### Custom Endpoint and Server
```bash
./friendly_example_client \
  --name my-device-001 \
  --uri coap://server.example.com:5683
```

#### PSK Security Mode
```bash
./friendly_example_client \
  --security psk \
  --psk-identity mydevice \
  --psk-key 00112233445566778899aabbccddeeff \
  --uri coaps://secure.example.com:5684
```

#### Certificate-Based Security with mbedTLS
```bash
./friendly_example_client \
  --dtls-provider=mbedtls \
  --security cert \
  --cert /path/to/device-cert.pem \
  --key /path/to/device-key.pem \
  --ca /path/to/ca-cert.pem \
  --uri coaps://secure.example.com:5684
```

#### IPv6 with Custom Lifetime
```bash
./friendly_example_client \
  --ipv6 \
  --uri coap://[2001:db8::1]:5683 \
  --lifetime 3600
```

#### Verbose Debug Output
```bash
./friendly_example_client -vvv
```

**Verbosity Levels:**
- `-v` - Basic debug information
- `-vv` - Detailed debug information
- `-vvv` - Maximum debug output (all logs)

#### Production Deployment Example
```bash
./friendly_example_client \
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

### Additional Options

| Option | Description |
|--------|-------------|
| `--list-dtls-providers` | Display available DTLS providers and exit |
| `-h, --help` | Display help message and exit |
| `-V, --version` | Display version information and exit |
| `-q, --quiet` | Suppress non-error output |
| `-d, --daemon` | Run as daemon process |
| `-c, --config=FILE` | Load configuration from file |

**DESIGN NOTES:**
- Use code blocks for all examples
- Organize by use case
- Show progression from simple to complex

---

## Slide 16: Performance Characteristics

### Title
## Performance Characteristics

### Subtitle
Efficient Resource Utilization

### Resource Usage Table

| Component | Memory | CPU (Idle) | CPU (Active) |
|-----------|--------|------------|--------------|
| Base client | ~500 KB | 0.1% | 1-2% |
| Per object | ~200 bytes | - | - |
| Per instance | ~500 bytes | - | - |
| Per resource | ~50 bytes | - | - |
| Task queue | ~100 bytes/task | 0.01% | 0.1% |
| DTLS session (mbedTLS) | ~64 KB | 0.05% | 0.5% |
| MQTT connection | ~32 KB | 0.02% | 0.3% |

### Scalability Limits (Tested)

| 15+ | 100+ | 100+ | 50+ | 20+ |
|:---:|:----:|:----:|:---:|:---:|
| **Objects Simultaneously** | **Instances Per Object** | **Resources Per Instance** | **Concurrent Observations** | **Periodic Tasks** |

### Block-wise Transfer Performance

| Block Size | Throughput | Latency | Protocol Overhead | Use Case |
|------------|------------|---------|-------------------|----------|
| 16 bytes | Very Low | High | ~75% | Very constrained (satellite) |
| 64 bytes | Low | Medium-High | ~40% | NB-IoT, LoRaWAN |
| 128 bytes | Medium | Medium | ~25% | LTE-M, LoRa |
| 256 bytes | Good | Low-Medium | ~15% | 3G, WiFi (weak signal) |
| 512 bytes | Very Good | Low | ~10% | 4G, WiFi |
| **1024 bytes** ⭐ | **Excellent** | **Minimal** | **~5%** | **WiFi, Ethernet (default)** |

### Delta FOTA Performance Metrics

**Size Reduction:**

| Algorithm | Average Reduction | Best Case | Worst Case |
|-----------|-------------------|-----------|------------|
| BSDIFF | 50-70% | 85% | 30% |
| VCDIFF | ~60% | 80% | 40% |
| Courgette | ~70% | 90% | 50% |

**With Compression:**

| Compression | Additional Reduction | Combined Total |
|-------------|---------------------|----------------|
| gzip | 30-40% | 70-85% |
| bzip2 | 35-45% | 75-87% |
| lzma | 40-50% | 80-90% |
| zstd | 45-55% | 82-92% |

### Real-World Performance Example

**Scenario:** Firmware update for fleet of 10,000 devices

| Metric | Full Binary | Delta + Compression |
|--------|-------------|---------------------|
| Firmware Size | 10 MB | 1 MB |
| Per-Device Bandwidth | 10 MB | 1 MB |
| Fleet Bandwidth | 100 GB | 10 GB |
| Update Time (1 Mbps) | 80 seconds | 8 seconds |
| **Savings** | **-** | **90 GB, 72 sec/device** |

### Network Protocol Performance

| Protocol | Latency | Bandwidth Efficiency | Power Consumption |
|----------|---------|---------------------|-------------------|
| CoAP/UDP | Very Low (RTT ~20ms) | High (low overhead) | Very Low |
| CoAP/DTLS | Low (RTT ~100ms w/ handshake) | Medium-High | Low |
| MQTT/TCP | Medium (connection setup) | Medium | Medium |
| MQTT/TLS | Medium-High (TLS handshake) | Medium | Medium |

### Memory Footprint by Configuration

| Configuration | Binary Size | RAM Usage (Idle) | RAM Usage (Active) |
|---------------|-------------|------------------|-------------------|
| Minimal (TinyDTLS, no features) | ~250 KB | ~300 KB | ~500 KB |
| Standard (mbedTLS, basic objects) | ~600 KB | ~500 KB | ~1 MB |
| Full (all providers, all features) | ~2 MB | ~1 MB | ~3 MB |

**DESIGN NOTES:**
- Use tables for detailed breakdowns
- Highlight default/recommended options with star (⭐)
- Include real-world example with calculations

---

## Slide 17: Unique Technical Advantages

### Title
## Unique Technical Advantages

### Subtitle
Why Friendly LwM2M Client Stands Out

### 6 Key Differentiators (6-card grid)

#### 1. Multi-DTLS Provider Architecture ⭐

**Why It Matters:**
- ✓ Choose best provider per deployment scenario
- ✓ Easy migration between SSL libraries
- ✓ Optimize for binary size OR feature set
- ✓ RFC 9146 CID support across all providers
- ✓ Runtime selection without recompilation

**Competitive Advantage:**
- Most alternatives: Single hardcoded DTLS provider
- Friendly LwM2M: 4 providers with runtime selection

---

#### 2. Complete Firmware Update Pipeline

**Production-Tested Features:**
- ✓ Three delta algorithms (BSDIFF, VCDIFF, Courgette)
- ✓ Multiple compression formats (gzip, bzip2, lzma, zstd, brotli)
- ✓ Safe rollback mechanism with A/B partitions
- ✓ Automatic health monitoring and recovery
- ✓ Cross-platform bootloader support

**Competitive Advantage:**
- Most alternatives: Full binary updates only
- Friendly LwM2M: 50-90% size reduction with safety

---

#### 3. Edge AI Capabilities

**Future-Proof ML Integration:**
- ✓ Multiple ML backends (TensorFlow Lite, ONNX Runtime)
- ✓ Hardware acceleration (CPU, GPU, NPU, TPU)
- ✓ Quantized models (FP32, FP16, INT8, INT4)
- ✓ Four inference modes (sync, async, streaming, batched)
- ✓ On-device inference (no cloud dependency)

**Competitive Advantage:**
- Most alternatives: Cloud-only inference or no ML support
- Friendly LwM2M: Full edge AI with HW acceleration

---

#### 4. Multi-Protocol Gateway

**Ecosystem Bridge:**
- ✓ LwM2M primary protocol (OMA v1.2.2)
- ✓ MQTT transport binding (Section 8 compliant)
- ✓ Matter/Thread integration (border router)
- ✓ Zigbee coordination (EZSP & Z-Stack)
- ✓ Protocol translation capabilities

**Competitive Advantage:**
- Most alternatives: CoAP/DTLS only
- Friendly LwM2M: Multi-protocol IoT hub

---

#### 5. Production-Ready Quality

**Enterprise-Class Development:**
- ✓ Comprehensive test suite (9+ test executables)
- ✓ Code coverage instrumentation (lcov/genhtml)
- ✓ Sanitizers (address, undefined behavior)
- ✓ Thread-safe operations with mutex protection
- ✓ Modern C++17 with smart pointers and RAII
- ✓ C++ Core Guidelines compliance

**Competitive Advantage:**
- Most alternatives: Minimal testing, older C code
- Friendly LwM2M: Production-grade quality assurance

---

#### 6. Platform-Agnostic Design

**True Cross-Platform Portability:**
- ✓ Multiple OS support (Linux, OpenWRT, RPi, prplOS, Windows)
- ✓ Bootloader abstractions (GRUB, U-Boot)
- ✓ Partition management abstractions
- ✓ Clean separation of concerns
- ✓ Strategy pattern for platform-specific code

**Competitive Advantage:**
- Most alternatives: Linux-only or vendor-locked
- Friendly LwM2M: Easy platform porting

---

### Summary Comparison Table

| Feature | Friendly LwM2M | Typical Alternatives |
|---------|----------------|----------------------|
| **DTLS Providers** | 4 providers, runtime selection | Single hardcoded provider |
| **FOTA** | 3 delta algorithms + A/B rollback | Full binary updates only |
| **Edge AI** | TFLite + ONNX + HW acceleration | Cloud-only or none |
| **Transports** | CoAP + MQTT (Section 8) | CoAP only |
| **Testing** | 9+ test suites + sanitizers | Minimal or none |
| **License** | MIT (permissive) | GPL or proprietary |
| **Code Quality** | Modern C++17 + Core Guidelines | Legacy C code |
| **Platform Support** | 5+ OS with abstractions | Linux-only |

**DESIGN NOTES:**
- Use 6-card grid (2 rows × 3 columns)
- Color-code each card differently
- Include checkmarks for bullet points
- Summary table at bottom for quick comparison

---

## Slide 18: Why Friendly LwM2M Client for R&D?

### Title
## Why Friendly LwM2M Client for R&D?

### Subtitle
The Ideal Platform for IoT Research & Development

### For Research & Development Teams

#### ✅ Experimentation Platform

**Runtime Flexibility:**
- **Compare DTLS providers** without rebuilding - switch via CLI flag
- **Test ML backends** - Benchmark TFLite vs ONNX performance
- **Evaluate delta algorithms** - Compare BSDIFF, VCDIFF, Courgette efficiency
- **Transport flexibility** - CoAP vs MQTT performance testing
- **Quantization research** - Study accuracy vs performance tradeoffs (FP32→INT4)

**Example Research Questions:**
- Which DTLS provider offers best performance on ARM devices?
- What's the optimal delta algorithm for embedded Linux updates?
- How much can we reduce inference latency with INT8 quantization?
- What's the bandwidth savings of CBOR vs JSON over MQTT?

---

#### 🔬 Research-Friendly Architecture

**Modern C++17 Codebase:**
- **Template metaprogramming** - Type-safe compile-time optimizations
- **Smart pointers** - Automatic memory management (unique_ptr, shared_ptr)
- **RAII pattern** - Resource acquisition is initialization
- **Move semantics** - Efficient resource transfers
- **Lambda expressions** - Inline functional programming

**Design Patterns for Extension:**
- **Factory Pattern** - Easy addition of new DTLS providers
- **Template Pattern** - ObjectImpl<T> for new LwM2M objects
- **Observer Pattern** - Event-driven architecture
- **Strategy Pattern** - Platform-specific implementations
- **Dependency Injection** - Testable, modular code

**Extensibility:**
- Add custom LwM2M objects in <200 lines of code
- Implement new delta algorithms via interface
- Create platform ports with abstraction layer
- Integrate new ML backends through common API

**Documentation:**
- Doxygen-generated API documentation
- Architecture diagrams
- Code examples and tutorials
- Comprehensive README and guides

---

#### 🚀 Innovation Enabler

**Edge AI Research:**
- Prototype on-device ML applications
- Study model compression techniques
- Benchmark hardware accelerators
- Research federated learning approaches

**IoT Gateway Prototyping:**
- Bridge LwM2M, MQTT, Matter, Zigbee protocols
- Develop cross-protocol device management
- Research protocol translation efficiency
- Create unified IoT management platforms

**Advanced FOTA Research:**
- Study delta compression algorithms
- Research optimal block sizes for networks
- Develop new rollback strategies
- Investigate secure boot integration

**Platform Porting:**
- Port to new embedded platforms
- Research bootloader integration
- Develop custom partition schemes
- Create new platform abstractions

---

### Competitive Technical Comparison

| Feature | Friendly LwM2M Client | Alternative A | Alternative B | Alternative C |
|---------|----------------------|---------------|---------------|---------------|
| **DTLS Providers** | 4 (runtime selection) | 1 (OpenSSL) | 1 (mbedTLS) | 1 (TinyDTLS) |
| **Delta FOTA** | 3 algorithms + rollback | None | Full binary only | Proprietary |
| **Edge AI** | TFLite + ONNX + HW accel | None | Cloud-only | None |
| **MQTT Transport** | Section 8 + CBOR | Basic MQTT | None | Basic MQTT |
| **Code Quality** | C++17 + tests + sanitizers | C code, minimal tests | C++11, some tests | C code, no tests |
| **License** | MIT (permissive) | GPL v3 | Proprietary | Apache 2.0 |
| **Platform Support** | 5+ OS | Linux only | Linux, Windows | Linux, RTOS |
| **Active Development** | Yes (2026) | Limited | Yes | No (abandoned) |
| **Documentation** | Comprehensive | Basic | Good | Minimal |

### R&D Use Case Examples

**Academic Research:**
- IoT protocol efficiency studies
- Energy consumption analysis
- Security protocol comparison
- ML on edge devices research

**Corporate R&D:**
- Proof-of-concept IoT solutions
- Technology evaluation (DTLS providers, ML backends)
- Platform feasibility studies
- Product prototyping

**Standards Development:**
- LwM2M protocol extensions
- MQTT integration testing
- OMA compliance validation
- RFC implementation verification

**DESIGN NOTES:**
- Use three highlighted sections
- Include comparison table
- Show practical R&D applications
- Emphasize flexibility and extensibility

---

## Slide 19: Use Cases & Applications

### Title
## Use Cases & Applications

### Subtitle
Real-World Deployment Scenarios

### Target Deployment Scenarios (6-card grid)

#### 🏭 Industrial IoT

**Applications:**
- Factory automation sensors
- Predictive maintenance with Edge AI
- Machine condition monitoring
- Quality control systems

**Key Features Used:**
- Delta FOTA for minimal downtime
- Edge AI for anomaly detection
- Secure DTLS communication
- Multi-protocol integration

**Example:**
*Manufacturing plant with 500+ sensors using LwM2M for centralized management, Edge AI for predictive maintenance, and delta updates to minimize production disruptions.*

---

#### 🏠 Smart Home/Building

**Applications:**
- WiFi/Thread gateway devices
- Matter/Zigbee protocol bridge
- Building automation controller
- Energy management system

**Key Features Used:**
- Matter/Thread integration
- Zigbee coordinator
- Edge inference (occupancy, anomaly detection)
- Secure device management

**Example:**
*Raspberry Pi-based smart home hub managing 100+ devices across WiFi, Thread, and Zigbee networks with unified LwM2M management interface.*

---

#### 🌐 Telecom/Network Equipment

**Applications:**
- OpenWRT routers
- Multi-WAN management
- VPN gateway configuration
- Network traffic optimization

**Key Features Used:**
- OpenWRT extended integration
- Multi-WAN failover objects
- Remote configuration (TR-069 bridge)
- Secure firmware updates

**Example:**
*Telecom CPE devices with LwM2M management for remote configuration, multi-WAN health monitoring, and automated firmware updates across 10,000+ deployed units.*

---

#### 🚗 Connected Vehicles

**Applications:**
- Telematics gateway
- Fleet management system
- OTA firmware updates
- ADAS data processing

**Key Features Used:**
- Delta FOTA for bandwidth efficiency
- Edge AI for ADAS data preprocessing
- Multi-bearer selection (4G/5G/WiFi)
- Secure certificate-based authentication

**Example:**
*Vehicle telematics unit using LwM2M for fleet management, delta updates for 90% bandwidth savings, and edge AI for driver behavior analysis.*

---

#### 🌾 Agriculture/Environment

**Applications:**
- Soil moisture sensors
- Weather stations
- Crop monitoring systems
- Precision agriculture

**Key Features Used:**
- LPWAN connectivity (NB-IoT, LoRaWAN)
- Delta updates for bandwidth savings
- Low power operation
- Offline operation with queue

**Key Features Used:**
- LPWAN connectivity (NB-IoT)
- Delta updates (limited bandwidth)
- Power-efficient operation
- Offline queue for intermittent connectivity

**Example:**
*Agricultural sensor network with 1,000+ soil sensors using NB-IoT, delta updates to minimize cellular data costs, and offline queueing for unreliable connectivity.*

---

#### 📡 Satellite IoT

**Applications:**
- Starlink integration
- Remote asset tracking
- Maritime/aviation IoT
- Emergency communications

**Key Features Used:**
- Starlink gRPC integration
- MQTT transport for satellite
- Delta updates (expensive satellite bandwidth)
- Reliable delivery with QoS

**Example:**
*Remote monitoring stations in polar regions using Starlink satellite connectivity with LwM2M management, achieving 90% bandwidth savings through delta updates.*

---

### Reference Deployments

**OpenWRT Routers:**
- Multi-WAN failover and load balancing
- VPN configuration and management
- WiFi optimization and client management
- Remote diagnostics and updates

**Raspberry Pi Gateways:**
- Edge AI inference (image classification, anomaly detection)
- Protocol bridging (LwM2M ↔ MQTT ↔ Matter)
- Local data aggregation and preprocessing
- Smart home automation hub

**Industrial Gateways:**
- prplOS integration with TR-069/TR-181 interoperability
- Factory floor sensor aggregation
- Real-time analytics with edge AI
- Secure remote management

**IoT Development Kits:**
- Rapid prototyping of IoT solutions
- SDK integration for custom applications
- Educational and training platforms
- Technology evaluation and testing

### Deployment Statistics

| Use Case | Typical Devices | Update Frequency | Key Metric |
|----------|----------------|------------------|------------|
| Industrial IoT | 100-10,000+ | Monthly | 99.9% uptime required |
| Smart Home | 10-100 | Quarterly | Low power consumption |
| Telecom | 1,000-100,000+ | As needed | Remote management critical |
| Automotive | Fleet size | Security patches: weekly | Bandwidth efficiency crucial |
| Agriculture | 100-10,000+ | Seasonal | Battery life: 5+ years |
| Satellite | 1-1,000 | Rarely | Bandwidth costs: $10-50/MB |

**DESIGN NOTES:**
- 6-card grid (2 rows × 3 columns)
- Icons for each category
- Include real-world examples
- Show diversity of applications

---

## Slide 20: Get Started & Contact

### Title (Centered)
## Get Started with Friendly LwM2M Client

### Quick Start (Code Block)

```bash
# Clone repository with submodules
git clone --recurse-submodules \
  https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git

# Navigate to directory
cd Friendly-LwM2M-Client

# Run setup script (Ubuntu)
sh ./wpp_env_setup.sh

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run example client
./examples/friendly_example_client
```

### Key Metrics (4-column)

| v1.2.2 | MIT | 15+ | 4 |
|:------:|:---:|:---:|:-:|
| **Current Release** | **Open Source** | **LwM2M Objects** | **DTLS Providers** |

### Resources

**Documentation:**
- 📚 Online Documentation: https://friendly-technologies.github.io/Friendly-LwM2M-Client/
- 📖 Getting Started Guide
- 🏗️ Architecture Documentation
- 💻 API Reference
- 📝 Code Examples

**GitHub Repository:**
- ⭐ Star us on GitHub
- 🐛 Report issues
- 🔀 Submit pull requests
- 💬 Discussions and Q&A

**Build Guides:**
- Ubuntu/Linux build instructions
- OpenWRT integration guide
- Raspberry Pi setup
- Cross-compilation for embedded targets

### Contact Information

**Friendly Technologies**

🌐 **Website:** https://hubs.li/Q02D_GnZ0
💻 **GitHub:** https://github.com/Friendly-Technologies/Friendly-LwM2M-Client
📧 **Contact:** Visit website for contact information
📄 **License:** MIT (permissive open source)

### Features Recap

**What You Get:**
✓ Production-grade LwM2M client (OMA v1.2.2)
✓ 4 DTLS providers with runtime selection
✓ Edge AI inference (TFLite + ONNX)
✓ Delta firmware updates (50-90% reduction)
✓ MQTT transport binding
✓ A/B partition rollback
✓ 15+ LwM2M objects
✓ Cross-platform support (Linux, OpenWRT, RPi, prplOS)
✓ Modern C++17 codebase
✓ Comprehensive test suite
✓ MIT license (commercial-friendly)

### Next Steps

1. **Explore** - Clone repository and review documentation
2. **Build** - Compile for your target platform
3. **Test** - Run example client and test suites
4. **Integrate** - Incorporate into your IoT solution
5. **Contribute** - Join the community and contribute back

---

### Thank You! (Centered)

**Questions & Discussion**

*Friendly LwM2M Client v1.2.2 - Production-Grade IoT Device Management*

**Contact us for:**
- Technical support
- Custom development
- Enterprise licensing
- Training and consulting

---

**DESIGN NOTES FOR FINAL SLIDE:**
- Center all content
- Large, bold "Thank You"
- Include all contact methods
- Feature recap as checklist
- Professional footer with tagline
- Leave space for Q&A

---

## PRESENTATION FORMATTING GUIDELINES

### Color Scheme
- **Primary (Blue):** #2563eb - Core features, headers
- **Secondary (Purple):** #7c3aed - Advanced features
- **Accent (Cyan):** #06b6d4 - Technical details
- **Success (Green):** #10b981 - Quality, safety features
- **Warning (Orange):** #f59e0b - Important highlights
- **Danger (Red):** #ef4444 - Critical items

### Typography
- **Title Font:** Bold, 36-48pt
- **Subtitle Font:** Light, 24-28pt
- **Body Font:** Regular, 16-20pt
- **Code Font:** Monospace, 14-16pt
- **Caption Font:** Regular, 12-14pt

### Layout Guidelines
- **Slide Padding:** 60px all sides
- **Element Spacing:** 20-30px between sections
- **Card Layout:** 2×2 or 3×2 grids with 20px gap
- **Table Borders:** 1px solid, alternating row colors
- **Code Blocks:** Dark background (#1e293b), light text (#e2e8f0)

### Icons & Symbols
- ✓ - Checkmark (success, completed)
- ✗ - Cross (not supported)
- ⭐ - Star (featured, default, recommended)
- 🚧 - Under construction (in progress)
- ⚠️ - Warning (partial support, caution)
- 🔒 - Security
- 🤖 - AI/ML
- 📦 - Firmware/packages
- 🌍 - Cross-platform
- 🔬 - Research
- 🚀 - Innovation

### Table Formatting
- **Header Row:** Bold, colored background
- **Alternating Rows:** Light gray every other row
- **Alignment:** Left for text, center for numbers/icons
- **Border:** Subtle borders, avoid heavy lines

### Code Block Formatting
- **Background:** Dark (#1e293b)
- **Text Color:** Light cyan (#7dd3fc) for code
- **Comments:** Gray (#94a3b8)
- **Syntax Highlighting:** Different colors for keywords, strings, numbers
- **Line Numbers:** Optional, gray
- **Padding:** 20px all sides

---

## SPEAKER NOTES SUGGESTIONS

### General Tips
- **Timing:** ~2 minutes per slide for 40-minute presentation
- **Engagement:** Ask questions, pause for clarification
- **Technical Depth:** Adjust based on audience expertise
- **Demos:** Consider live demo of CLI or test execution
- **Q&A:** Reserve 10-15 minutes at end

### Key Emphasis Points
1. **Multi-DTLS is unique** - No other LwM2M client offers this
2. **Production-ready** - Emphasize quality and testing
3. **R&D-friendly** - Flexibility for experimentation
4. **Open source MIT** - Commercial-friendly license
5. **Active development** - v1.2.2 released January 2026

### Potential Questions to Prepare For
- How does performance compare to Wakaama/Anjay/Leshan?
- What's the migration path from other LwM2M clients?
- Can we add custom objects easily?
- What's the roadmap for future versions?
- Is commercial support available?
- What platforms have been tested in production?
- How do you handle security updates?
- What's the contribution process?

---

**END OF PITCH DECK MARKDOWN**

---

## CONVERSION TO PPTX NOTES

### Recommended Tools
1. **Pandoc** - Convert markdown to PPTX
   ```bash
   pandoc Friendly_LwM2M_Client_v1.2.2_Technical_Pitch_Deck.md \
     -o output.pptx \
     -t pptx
   ```

2. **Manual Conversion** - Copy/paste into PowerPoint
   - More control over layout
   - Better formatting options
   - Can add animations

3. **Marp** - Markdown presentation ecosystem
   ```bash
   marp Friendly_LwM2M_Client_v1.2.2_Technical_Pitch_Deck.md \
     --pptx
   ```

### Conversion Tips
- Tables may need manual adjustment in PPTX
- Code blocks should use monospace font
- Preserve color scheme from guidelines
- Add company logo to master slide
- Include slide numbers
- Use consistent transitions
- Add subtle animations for emphasis
- Export to PDF for distribution

### PowerPoint Specific Recommendations
- **Master Slide:** Create custom master with color scheme
- **Fonts:** Use company standard fonts if available
- **Animations:** Minimal, professional (fade, wipe)
- **Transitions:** Consistent, subtle (fade recommended)
- **Images:** Add architecture diagrams if available
- **Charts:** Convert metrics to pie/bar charts for visual impact
- **Icons:** Use professional icon set (Material Design, Font Awesome)

---

**FILE METADATA**
- **File Name:** Friendly_LwM2M_Client_v1.2.2_Technical_Pitch_Deck.md
- **Version:** 1.0
- **Date:** April 24, 2026
- **Author:** Technical Documentation Generator
- **Target Format:** PowerPoint (PPTX)
- **Slide Count:** 20
- **Estimated Duration:** 20-40 minutes
