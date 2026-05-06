# Friendly LwM2M Client - Comprehensive Knowledge Base

**Version:** 1.2.0
**LwM2M Specification:** OMA v1.2.2
**Last Updated:** January 2026
**Repository:** https://github.com/Friendly-Technologies/Friendly-LwM2M-Client

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Project Overview](#project-overview)
3. [Core Architecture](#core-architecture)
4. [LwM2M Protocol Compliance](#lwm2m-protocol-compliance)
5. [Supported Objects - Complete Reference](#supported-objects---complete-reference)
6. [Transport & Security](#transport--security)
7. [Platform Support](#platform-support)
8. [Advanced Features](#advanced-features)
9. [Build System](#build-system)
10. [API Reference](#api-reference)
11. [Integration Guides](#integration-guides)
12. [Use Cases & Examples](#use-cases--examples)
13. [Performance & Resource Usage](#performance--resource-usage)
14. [Troubleshooting](#troubleshooting)
15. [External References](#external-references)

---

## 1. Executive Summary

### What is Friendly LwM2M Client?

The Friendly LwM2M Client is a **production-ready, OMA LwM2M v1.2.2 compliant** implementation designed for IoT device management across resource-constrained and high-performance edge devices. It provides comprehensive remote management capabilities including device monitoring, firmware updates, configuration management, and telemetry collection.

### Key Highlights

- **Full OMA LwM2M v1.2.2 Compliance**: Implements all mandatory objects and extensive optional features
- **70+ LwM2M Objects**: 8 mandatory OMA objects + 60+ custom objects for specialized functionality
- **Multi-Transport**: UDP/DTLS (CoAP), MQTT 3.1.1/5.0 with CBOR encoding
- **Edge AI Integration**: On-device ML inference with TensorFlow Lite and ONNX Runtime
- **Advanced FOTA**: Delta firmware updates (BSDIFF, VCDIFF, Courgette) with A/B rollback
- **Multi-Platform**: Linux, OpenWRT, Raspberry Pi 4 (ARM64), prplOS/prplWrt
- **Enterprise Features**: Starlink satellite terminal management, MIKROBUS support, WiFi/networking management
- **Open Source**: Licensed under business-friendly terms

### Primary Use Cases

1. **IoT Gateway Management**: Router/gateway remote configuration and monitoring
2. **Satellite Communication**: Starlink terminal management and monitoring
3. **Smart Home/Building**: Multi-protocol device management (Matter, Zigbee, WiFi)
4. **Industrial IoT**: Edge computing with on-device ML inference
5. **Network Management**: Multi-WAN failover, VPN configuration, bearer selection

---

## 2. Project Overview

### Version History

#### v1.2.0 (January 2026) - Current Release
- Added extended OpenWRT integration (LUCI web module, hotplug handlers)
- Added router management objects (LAN config, routing, VPN)
- Added multi-WAN failover and health monitoring
- Added WiFi advanced management (client management, channel optimization)
- Added Matter/Thread integration
- Added Zigbee integration
- Added prplOS/prplMesh integration
- Starlink gRPC integration completed
- 13 new LwM2M objects

#### v1.1.0 (January 2026)
- MQTT transport binding (OMA Section 8)
- Edge AI inference (TensorFlow Lite, ONNX Runtime)
- Delta firmware updates (BSDIFF, VCDIFF, Courgette)
- A/B partition rollback management
- 3 new LwM2M objects (23, 24, 33410)

#### v1.0.0 (June 2025) - Initial Release
- Core LwM2M client implementation
- OMA standard objects (0-6, 12, 13)
- Walt Technologies custom objects (34600-34610)
- OpenWRT integration
- DTLS security support

### Project Structure

```
Friendly-LwM2M-Client/
├── wpp/                          # WPP Library (core client)
│   ├── registry/                 # Object registry system
│   │   ├── objects/              # LwM2M object implementations
│   │   │   ├── m_0_lwm2m_security/
│   │   │   ├── m_1_lwm2m_server/
│   │   │   ├── m_3_device/
│   │   │   ├── o_4_connectivity_monitoring/
│   │   │   ├── o_5_firmware_update/
│   │   │   ├── o_6_location/
│   │   │   ├── o_12_wlan_connectivity/
│   │   │   ├── o_13_bearer_selection/
│   │   │   ├── o_10512_starlink_terminal/   # Renumbered from 34600
│   │   │   ├── o_10519_hardware_watchdog/   # Renumbered from 34607
│   │   │   ├── o_10520_mikrobus/            # Renumbered from 34608
│   │   │   └── ... (60+ more objects)
│   │   ├── WppRegistry.h/cpp     # Object registry manager
│   │   └── ObjectID.h            # Object ID enumeration
│   ├── configs/                  # Build configuration
│   │   └── wpp_config.cmake      # Object enable/disable flags
│   ├── connection/               # Network connection management
│   ├── logs/                     # Logging system
│   ├── platform/                 # Platform abstraction layer
│   │   ├── openwrt/              # OpenWRT platform impl
│   │   ├── prpl/                 # prplOS platform impl
│   │   ├── linux/                # Generic Linux platform
│   │   └── rpi/                  # Raspberry Pi platform
│   └── CMakeLists.txt
├── examples/                     # Example applications
│   ├── main.cpp                  # Main entry point
│   ├── objects.cpp               # Object initialization
│   └── CMakeLists.txt
├── openwrt/                      # OpenWRT packaging
│   ├── Makefile                  # OpenWRT package makefile
│   ├── luci/                     # LUCI web interface module
│   ├── files/                    # Config files and init scripts
│   │   ├── etc/init.d/           # Service init scripts
│   │   ├── etc/config/           # UCI configuration files
│   │   └── etc/hotplug.d/        # Hotplug event handlers
│   └── README.md
├── src/                          # Additional source implementations
│   ├── transport/                # Transport layer implementations
│   │   ├── mqtt_transport.cpp    # MQTT transport binding
│   │   └── blockwise_transfer.cpp
│   ├── firmware/                 # Firmware update system
│   │   ├── delta_algorithms.cpp  # BSDIFF, VCDIFF, Courgette
│   │   └── rollback_manager.cpp  # A/B partition management
│   ├── objects/                  # Extended object implementations
│   │   ├── edge_ai_inference_object.cpp
│   │   ├── lwm2m_cose_object.cpp
│   │   └── mqtt_server_object.cpp
│   ├── starlink/                 # Starlink gRPC client
│   ├── matter/                   # Matter SDK integration
│   ├── zigbee/                   # Zigbee coordinator
│   └── platform/                 # Platform-specific code
├── utils/                        # Utility tools
│   └── object_maker/             # OMA XML to C++ code generator
│       ├── object_maker.py       # Generator script
│       └── *.xml                 # Object definitions (OMA format)
├── docs/                         # Comprehensive documentation
│   ├── API_REFERENCE.md
│   ├── ARCHITECTURE.md
│   ├── CONFIGURATION.md
│   ├── QUICKSTART.md
│   ├── IMPLEMENTATION_GUIDE.md
│   ├── WLAN_CONNECTIVITY.md
│   ├── BEARER_SELECTION.md
│   ├── LOCATION_OBJECT.md
│   ├── STARLINK_TERMINAL.md
│   ├── MIKROBUS_OBJECT.md
│   ├── OPENWRT_INTEGRATION.md
│   ├── MATTER_INTEGRATION.md
│   ├── ZIGBEE_INTEGRATION.md
│   ├── PRPL_INTEGRATION.md
│   └── TROUBLESHOOTING.md
├── cmake/                        # CMake configuration files
├── external/                     # External dependencies (Wakaama, etc.)
├── tests/                        # Unit and integration tests
├── CMakeLists.txt                # Root CMake configuration
├── README.md                     # Project README
├── CHANGELOG.md                  # Version history
└── LICENSE                       # Software license
```

### Technologies & Dependencies

**Core Stack:**
- **Language**: C++ (C++17)
- **LwM2M Core**: Eclipse Wakaama (embedded)
- **Build System**: CMake 3.10+
- **Compiler**: GCC 8+, Clang 10+

**Security:**
- OpenSSL 1.1+ (DTLS support)
- mbedTLS (alternative)

**Protocols:**
- CoAP (RFC 7252)
- DTLS 1.2 (RFC 6347)
- MQTT 3.1.1 (Eclipse Paho)
- MQTT 5.0 (Eclipse Paho)
- gRPC (Starlink integration)
- Protocol Buffers

**Data Formats:**
- CBOR (RFC 8949)
- TLV (OMA LwM2M)
- JSON
- SenML JSON
- SenML CBOR

**AI/ML:**
- TensorFlow Lite
- ONNX Runtime

**Smart Home:**
- Matter SDK (Connectivity Standards Alliance)
- Zigbee (EZSP, Z-Stack)
- Thread (OpenThread)

---

## 3. Core Architecture

### System Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                     LwM2M Client Application v1.2.0                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                       │
│  ┌──────────────┐  ┌──────────────┐  ┌─────────────────────┐       │
│  │   Examples   │  │  Custom App  │  │  Service Daemon     │       │
│  └──────┬───────┘  └──────┬───────┘  └──────────┬──────────┘       │
│         └──────────────────┴───────────────────┬─┘                  │
│                                                 │                    │
│  ┌──────────────────────────────────────────────▼──────────────┐   │
│  │                  WPP Client Library                          │   │
│  ├──────────────────────────────────────────────────────────────┤   │
│  │  WppClient  │  WppRegistry  │  WppTaskQueue  │ WppLogs      │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                 │                    │
│  ┌──────────────────────────────────────────────▼──────────────┐   │
│  │                   Object Registry                            │   │
│  ├──────────────────────────────────────────────────────────────┤   │
│  │  Device  │  MQTT Server  │  Edge AI  │  Firmware  │ Custom  │   │
│  │  (ID 3)  │    (ID 24)    │ (ID 33410)│   (ID 5)   │ Objects │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                 │                    │
│  ┌──────────────────────────────────────────────▼──────────────┐   │
│  │                 Wakaama LwM2M Stack                          │   │
│  ├──────────────────────────────────────────────────────────────┤   │
│  │  CoAP  │  DTLS  │  Observe  │  Bootstrap  │  FOTA  │ Block  │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                 │                    │
│  ┌─────────────────────┬────────────────────────┴──────────────┐   │
│  │   Delta Firmware    │        Transport Layer                 │   │
│  │   ┌─────────────┐   │   ┌───────────┬───────────────────┐   │   │
│  │   │ BSDIFF      │   │   │ CoAP/DTLS │  MQTT Transport   │   │   │
│  │   │ VCDIFF      │   │   │  (UDP)    │  (MQTT 3.1.1/5.0) │   │   │
│  │   │ Courgette   │   │   └───────────┴───────────────────┘   │   │
│  │   └─────────────┘   │                                        │   │
│  │   ┌─────────────┐   │   ┌───────────────────────────────┐   │   │
│  │   │ A/B Rollback│   │   │    Edge AI Inference Engine   │   │   │
│  │   │ Manager     │   │   │  TFLite  │  ONNX Runtime     │   │   │
│  │   └─────────────┘   │   └───────────────────────────────┘   │   │
│  └─────────────────────┴────────────────────────────────────────┘   │
│                                                                       │
└───────────────────────────────────────────────────────────────────────┘
                                    │
          ┌─────────────────────────┼─────────────────────────┐
          │                         │                         │
┌─────────▼─────────┐  ┌───────────▼───────────┐  ┌─────────▼─────────┐
│  UDP Transport    │  │   MQTT Broker         │  │  LwM2M Server     │
│  (CoAP/DTLS)      │  │   (MQTT 3.1.1/5.0)    │  │                   │
└───────────────────┘  └───────────────────────┘  └───────────────────┘
```

### Core Components

#### 1. WppClient
**Purpose**: Main client coordinator and lifecycle manager

**Key Responsibilities:**
- Initialize Wakaama LwM2M stack
- Manage object registry
- Handle server connections (registration, updates, de-registration)
- Coordinate client lifecycle

**API:**
```cpp
class WppClient {
public:
    // Connection management
    bool connect();
    bool disconnect();
    void process(int timeout_ms);

    // Registry access
    WppRegistry& registry();

    // Data operations
    bool send(const DataLink& data);
    bool send(const DataLink data[], size_t count);

    // Ownership transfer
    void giveOwnership();

    // Client state
    bool isConnected();
    const std::string& getEndpoint();
};
```

#### 2. WppRegistry
**Purpose**: Object management and discovery

**Key Responsibilities:**
- Register/deregister LwM2M objects
- Provide object accessors
- Manage object lifecycle
- Object instance enumeration

**API:**
```cpp
class WppRegistry {
public:
    // Object management
    bool registerObj(Object& object);
    bool deregisterObj(Object& object);
    bool isExist(OBJ_ID objId);
    Object* object(OBJ_ID objId);

    // Standard object accessors
    Object& device();
    Object& lwm2mServer();
    Object& lwm2mSecurity();
    Object& connectivityMonitoring();
    Object& location();
    Object& firmwareUpdate();

    // Custom object accessors (60+ objects)
    Object& starlinkTerminal();
    Object& mikrobus();
    Object& wlanConnectivity();
    // ... more
};
```

#### 3. WppTaskQueue
**Purpose**: Asynchronous task scheduling

**Key Responsibilities:**
- Schedule periodic tasks
- Execute callbacks at intervals
- Resource cleanup

**API:**
```cpp
class WppTaskQueue {
public:
    using TaskCallback = std::function<bool(WppClient&, void*)>;

    // Add task (returns task_id)
    static task_id_t addTask(int interval_sec, TaskCallback callback, void* context = nullptr);

    // Remove task
    static void requestToRemoveTask(task_id_t taskId);

    // Process tasks (called by WppClient::process)
    static void processTasks(WppClient& client);
};
```

### Object Hierarchy

```
┌─────────────────┐
│     Object      │  (Abstract base)
│                 │
│ + getObjectID() │
│ + instance()    │
│ + createInst()  │
└────────┬────────┘
         │
         ├── ObjectImpl<T>  (Template implementation)
         │
         └── Manages instances
                    │
         ┌──────────▼─────────┐
         │      Instance      │  (Abstract base)
         │                    │
         │ + resource()       │
         │ + set<T>()         │
         │ + get<T>()         │
         │ + notifyResChanged()│
         └──────────┬─────────┘
                    │
         ┌──────────▼────────┐
         │   Device          │
         │   Location        │
         │   StarlinkTerminal│
         │   Mikrobus        │
         │   (Concrete impls)│
         └───────────────────┘
```

### Data Flow

**Registration Flow:**
```
Client → Registry → Wakaama → Server
  │ registerObj()    │ lwm2m_add_object │ REGISTER (CoAP)
  ├──────────────────►│──────────────────►├───────────────►
  │                   │                   │ 2.01 Created
  │                   │                   │◄────────────────
  │  Connected!       │                   │
  │◄──────────────────────────────────────┤
```

**Resource Read Flow:**
```
Server → Wakaama → Instance → Resource
  │ READ /6/0/0      │ read_callback()  │ get<FLOAT_T>(0)
  ├─────────────────►├─────────────────►├────────────────►
  │                  │                  │  Returns value
  │                  │◄─────────────────┤
  │  2.05 Content    │                  │
  │◄─────────────────┤                  │
```

### Threading Model

**Single-Threaded, Event-Driven Design:**

```cpp
// Main loop
while (running) {
    // Process LwM2M events (non-blocking)
    client->process(1000);  // 1000ms timeout

    // Task queue runs in same thread
    // Tasks execute during process()
}
```

**Benefits:**
- Simple and predictable
- No race conditions
- Low overhead
- Suitable for embedded systems

**Limitations:**
- Long-running tasks block event loop
- No parallel processing

---

## 4. LwM2M Protocol Compliance

### OMA LwM2M v1.2.2 Specification

**Full compliance with:**
- Core Specification (OMA-TS-LightweightM2M_Core-V1_2_2)
- Transport Bindings (OMA-TS-LightweightM2M_Transport-V1_2_2)
- Object Registry (OMA-LwM2M-Registry)

### Supported Interfaces

#### Bootstrap Interface
- **Request**: Initiate bootstrap sequence
- **Finish**: Complete bootstrap
- **Discover**: Discover bootstrap server capabilities
- **Read**: Read bootstrap configuration
- **Write**: Write bootstrap configuration
- **Delete**: Delete bootstrap instances

#### Registration Interface
- **Register**: Register client with LwM2M server
- **Update**: Update registration
- **De-register**: Remove client registration

#### Device Management & Service Enablement Interface
- **Read**: Read resource values
- **Discover**: Discover object/resource structure
- **Write**: Write resource values
- **Write-Attributes**: Set observation parameters
- **Execute**: Execute resource actions
- **Create**: Create object instances
- **Delete**: Delete object instances
- **Send**: Send telemetry data (LwM2M 1.2+)

#### Information Reporting Interface
- **Observe**: Subscribe to resource changes
- **Notify**: Push notifications to server
- **Cancel Observation**: Unsubscribe from resources
- **Send Operation**: Push data without observation

### Supported Data Formats

| Format | Content-Type | RFC/Spec | Usage |
|--------|-------------|----------|-------|
| **Plain Text** | text/plain | - | Simple values |
| **Opaque** | application/octet-stream | - | Binary data |
| **CBOR** | application/cbor | RFC 8949 | Binary encoding |
| **TLV** | application/vnd.oma.lwm2m+tlv | OMA LwM2M | Legacy format |
| **JSON** | application/vnd.oma.lwm2m+json | OMA LwM2M | JSON format |
| **SenML JSON** | application/senml+json | RFC 8428 | Sensor data |
| **SenML CBOR** | application/senml+cbor | RFC 8428 | Binary sensor data |

### Security Modes

| Mode | Description | Use Case |
|------|-------------|----------|
| **PSK** (0) | Pre-Shared Key | Simple deployment, shared secrets |
| **Raw Public Key** (1) | Public key infrastructure | Asymmetric cryptography |
| **Certificate** (2) | X.509 certificates | Enterprise PKI |
| **NoSec** (3) | No security | Development, local networks |

### Block-Wise Transfer

**RFC 7959 Support:**
- Block size: 16, 32, 64, 128, 256, 512, 1024 bytes (configurable)
- Default: 1024 bytes
- Used for large resource transfers (firmware, telemetry)

---

## 5. Supported Objects - Complete Reference

### Object ID Mapping

The project uses a renumbering scheme where custom objects are mapped from the 34600+ range to the 105xx range:

| Original ID | New ID | Name |
|------------|--------|------|
| 34600 | 10512 | Starlink Terminal |
| 34607 | 10519 | Hardware Watchdog |
| 34608 | 10520 | MIKROBUS |

### Mandatory OMA Objects

#### Object 0: LwM2M Security
**URN**: urn:oma:lwm2m:oma:0
**Instances**: Multiple
**Purpose**: Security credentials and server URIs

**Key Resources:**
- Server URI (0): CoAP/CoAPS server address
- Bootstrap Server (1): Bootstrap flag
- Security Mode (2): PSK/RPK/Certificate/NoSec
- Public Key or Identity (3): Identity for PSK
- Secret Key (5): Pre-shared key
- Short Server ID (10): Server identifier

#### Object 1: LwM2M Server
**URN**: urn:oma:lwm2m:oma:1
**Instances**: Multiple
**Purpose**: LwM2M server configuration

**Key Resources:**
- Short Server ID (0): Server identifier (1-65534)
- Lifetime (1): Registration lifetime (seconds)
- Min Period (2): Minimum observation interval
- Max Period (3): Maximum observation interval
- Notification Storing (7): Store notifications when offline
- Binding (7): Transport binding (U=UDP, T=TCP, S=SMS, Q=Queue)

#### Object 3: Device
**URN**: urn:oma:lwm2m:oma:3
**Instances**: Single
**Purpose**: Device information and control

**Key Resources (60+ total):**
- Manufacturer (0): Device manufacturer name
- Model Number (1): Device model
- Serial Number (2): Unique serial number
- Firmware Version (3): Current firmware version
- Reboot (4): Execute to reboot device
- Factory Reset (5): Execute to factory reset
- Power Sources (6-9): Battery/power information
- Memory Free (10): Available memory (KB)
- Error Code (11): Device error codes
- Current Time (13): Unix timestamp
- Supported Bindings (16): Supported transports
- Device Type (17): Device category
- Hardware Version (18): Hardware revision
- Software Version (19): Software version

### Optional OMA Objects

#### Object 2: LwM2M Access Control
**URN**: urn:oma:lwm2m:oma:2
**Instances**: Multiple
**Purpose**: Access control lists for objects

#### Object 4: Connectivity Monitoring
**URN**: urn:oma:lwm2m:oma:4
**Instances**: Single
**Purpose**: Network connectivity monitoring

**Key Resources:**
- Network Bearer (0): Current network type (0=GSM, 21=WLAN, 41=Ethernet)
- Available Bearers (1): List of available networks
- Radio Signal Strength (2): dBm
- Link Quality (3): Percentage (0-100)
- IP Addresses (4): IPv4/IPv6 addresses
- Router IP (5): Gateway addresses
- Link Utilization (6): Bandwidth usage %
- APN (7): Cellular APN
- Cell ID (8): Cell tower identifier

#### Object 5: Firmware Update
**URN**: urn:oma:lwm2m:oma:5
**Instances**: Single
**Purpose**: Firmware Over-The-Air (FOTA) updates

**Key Resources:**
- Package (0): Firmware package download
- Package URI (1): URI for firmware download
- Update (2): Execute to apply firmware
- State (3): Update state (0=Idle, 1=Downloading, 2=Downloaded, 3=Updating)
- Update Result (5): Result code
- Package Name (6): Firmware package name
- Package Version (7): New firmware version

**Enhanced Features (v1.1.0+):**
- Delta update algorithms (BSDIFF, VCDIFF, Courgette)
- Compression support (gzip, bzip2, lzma, zstd, brotli)
- A/B partition management with automatic rollback
- Block-wise transfer support
- SHA-256 integrity verification

#### Object 6: Location
**URN**: urn:oma:lwm2m:oma:6
**Instances**: Single
**Purpose**: GPS location tracking

**Key Resources:**
- Latitude (0): Decimal degrees (-90 to +90)
- Longitude (1): Decimal degrees (-180 to +180)
- Altitude (2): Meters above sea level
- Radius (3): Uncertainty radius (meters)
- Velocity (4): 3GPP velocity format
- Timestamp (5): Unix timestamp
- Speed (6): m/s (calculated via Haversine formula)

**GPS Sources (Priority Order):**
1. Starlink Terminal GPS (Object 10512, Resources 83-85)
2. gpsd daemon
3. UCI manual configuration

#### Object 12: WLAN Connectivity
**URN**: urn:oma:lwm2m:oma:12
**Instances**: Multiple
**Purpose**: WiFi interface management

**48 Resources organized in groups:**

**Interface Control (0-3):**
- Interface Name (0): "wlan0", "wlan1"
- Enable (1): Enable/disable interface
- Radio Enabled (2): Radio hardware state
- Status (3): 0=Disabled, 1=Up, 2=Error

**SSID Configuration (4-7):**
- BSSID (4): MAC address of AP
- SSID (5): Network name (1-32 chars)
- Broadcast SSID (6): Show/hide SSID
- Beacon Enabled (7): Beacon transmission

**Channel Management (8-13):**
- Mode (8): 0=AP, 1=Client, 2=Bridge, 3=Repeater
- Channel (9): WiFi channel number
- Auto Channel (10): Automatic selection
- Supported Channels (11): Available channels
- Channels In Use (12): Nearby AP channels
- Regulatory Domain (13): Country code

**WiFi Standard (14):**
- Standard (14): 0=802.11a, 1=b, 2=bg, 3=g, 4=n, 5=bgn, 6=ac, 8=ax

**Security (15-28):**
- Authentication Mode (15): 0=None, 1=PSK, 2=EAP
- Encryption Mode (16): 0=AES(WPA2), 1=TKIP(WPA), 2=WEP
- WPA Pre Shared Key (17): WPA key
- WPA Key Phrase (18): Passphrase (8-63 chars)
- WEP Encryption (19-25): WEP configuration
- RADIUS Server (26-28): Enterprise authentication

**QoS (29-30):**
- WMM Supported (29): WiFi Multimedia support
- WMM Enabled (30): Enable QoS

**Access Control (31-32):**
- MAC Control (31): MAC filtering enabled
- MAC Address List (32): Allowed/blocked MACs

**Statistics (33-47):**
- Total Bytes Sent/Received (33-34)
- Total Packets Sent/Received (35-36)
- Transmit/Receive Errors (37-38)
- Unicast/Multicast/Broadcast counters (39-44)
- Discard Packets (45-46)
- Unknown Packets (47)

**OpenWRT Integration:**
- UCI configuration loading
- sysfs statistics (/sys/class/net/wlan*/statistics/)
- hostapd integration

#### Object 13: Bearer Selection
**URN**: urn:oma:lwm2m:oma:13
**Instances**: Single
**Purpose**: Network bearer preference management

**12 Resources:**
- Preferred Communications Bearer (0): Priority list (e.g., "21,41,6" = WiFi, Ethernet, LTE)
- Acceptable RSSI GSM (1): Minimum signal (-110 to -48 dBm)
- Acceptable RSCP UMTS (2): Minimum signal (-120 to -25 dBm)
- Acceptable RSRP LTE (3): Minimum signal (-140 to -44 dBm)
- Acceptable RSSI WLAN (4): Minimum signal (-100 to -30 dBm)
- Cell Lock List (5): Locked cells
- Operator List (6): PLMN codes (MCC-MNC)
- Operator List Mode (7): 0=Whitelist, 1=Blacklist
- Available Bearers (8): Currently available
- Signal Strength Variation (9): Hysteresis (dB)
- Higher Priority PLMN Search (10): Search interval (seconds)
- Attach Without PDN (11): Cellular attach mode

**Supported Bearers:**
- 0=GSM, 2=WCDMA, 6=LTE FDD, 7=LTE-M, 8=NB-IoT
- 21=WLAN, 41=Ethernet, 42=DSL

### v1.1.0 New Objects

#### Object 23: LwM2M COSE
**URN**: urn:oma:lwm2m:oma:23
**Purpose**: CBOR Object Signing and Encryption for MQTT

**Key Features:**
- Security credentials for MQTT transport
- COSE encryption and signing
- Key management

#### Object 24: MQTT Server
**URN**: urn:oma:lwm2m:oma:24
**Purpose**: MQTT broker configuration

**Key Resources:**
- MQTT Broker URI
- Client ID
- Port number
- TLS configuration
- QoS levels
- Keep-alive interval
- Clean session flag

#### Object 33410: Edge AI Inference
**URN**: urn:oma:lwm2m:x:33410
**Purpose**: On-device ML inference management

**Key Features:**
- Model Management (download, validate, load)
- TensorFlow Lite backend support
- ONNX Runtime backend support
- Hardware acceleration (CPU, GPU, NPU, TPU)
- Quantization support (FP32, FP16, INT8, INT4)
- Inference modes (Sync, Async, Streaming, Batched)
- Preprocessing/postprocessing pipelines

### Walt Technologies Custom Objects (34600-10537 / 105xx series)

#### Object 10512 (34600): Starlink Terminal
**URN**: urn:oma:lwm2m:x:10512
**Instances**: Single
**Purpose**: Starlink satellite terminal management via gRPC

**70+ Resources in 10 groups:**

**Device Info (0-9):**
- Device ID (0): Serial number
- Hardware Version (1): Terminal hardware version
- Software Version (2): Firmware version
- Connection State (3): CONNECTED, SEARCHING, BOOTING, STOWED, etc.
- Uptime (4): Seconds since reboot

**Network Performance (10-29):**
- Downlink Throughput (10): bps
- Uplink Throughput (11): bps
- POP Ping Latency (12): ms
- POP Ping Drop Rate (13): Packet loss (0.0-1.0)
- Seconds To First Slot (14): Next satellite availability
- SNR (15): Signal-to-noise ratio (dB)
- Download/Upload Usage (17-18): Total bytes

**Obstruction Monitoring (30-39):**
- Fraction Obstructed (30): 0.0-1.0
- Currently Obstructed (31): Boolean
- Obstruction Duration (32): Average duration (seconds)
- Obstruction Interval (33): Time between obstructions
- Wedges Fraction Obstructed (35): 12-wedge array

**Dish Alignment (40-49):**
- Direction Azimuth (40): 0-360° (0=North)
- Direction Elevation (41): 0-90°
- Dish Stowed (42): Get/set stow state
- Alignment Status (43): Human-readable message

**Alert System (50-79) - 16 alerts:**
- Motors Stuck (51)
- Thermal Shutdown (52)
- Thermal Throttle (53)
- Unexpected Location (54)
- Mast Not Vertical (55)
- Slow Ethernet (56)
- Roaming (57)
- Is Heating (59)
- Water Detected (65-66)
- And more...

**GPS Location (80-89):**
- GPS Ready (80): Fix available
- GPS Enabled (81): GPS functionality state
- GPS Satellites (82): Satellite count
- Latitude (83): Decimal degrees
- Longitude (84): Decimal degrees
- Altitude (85): Meters

**Power Management (90-99):**
- Latest Power (90): Watts (instantaneous)
- Mean Power (91): Watts (average)
- Min/Max Power (92-93): Watts
- Total Energy (94): kWh

**Control Actions (100-107):**
- Reboot Terminal (100): Execute
- Start Speedtest (101): Execute
- Factory Reset (103): Execute
- Get Obstruction Map (104): Execute
- Refresh Telemetry (106): Execute
- gRPC Endpoint (107): "192.168.100.1:9200"

**Historical Data (110-119):**
- History Samples (110): Sample count
- Mean Ping Latency (112-114)
- Total Ping Drop (115)
- Latency Deciles (118): JSON array
- Load Bucket Stats (119): JSON

**gRPC Integration:**
- SpaceX.API.Device.Device service
- Real-time telemetry updates
- Dish control commands
- Obstruction map retrieval

#### Object 10519 (34607): Hardware Watchdog
**URN**: urn:oma:lwm2m:x:10519
**Instances**: Single
**Purpose**: Hardware watchdog timer management

**Key Resources:**
- Watchdog Enabled
- Timeout Period
- Feed Interval
- Last Feed Time
- Reboot Count

#### Object 10520 (34608): MIKROBUS
**URN**: urn:oma:lwm2m:x:10520
**Instances**: Multiple
**Purpose**: MIKROBUS socket and Click board management

**50+ Resources in 8 groups:**

**Socket Information (0-6):**
- Socket ID (0): Socket number
- Socket Name (1): Human-readable name
- Socket Enabled (2): Enable/disable
- Click Board Present (3): Detection flag
- Click Board Type (4): Board category
- Power Voltage (5): 3300 or 5000 mV
- Power State (6): On/off

**Click Board Info (10-14):**
- Manufacturer (10)
- Model (11)
- Firmware Version (12)
- Serial Number (13)
- Description (14)

**Interface Configuration (20-27):**
- Active Interface (20): 0=None, 1=SPI, 2=I2C, 3=UART
- I2C Address (21): 7-bit address
- SPI Mode (22): 0-3 (CPOL/CPHA)
- SPI Speed (23): Hz
- UART Baud Rate (24): bps
- UART Data Bits (25): 7-8
- UART Parity (26): 0=None, 1=Odd, 2=Even
- UART Stop Bits (27): 1-2

**GPIO and Analog (31-36):**
- AN Value (31): 0-3.3V analog input
- PWM Duty Cycle (32): 0-100%
- PWM Frequency (33): Hz
- INT Pin State (34): Interrupt pin
- RST Pin State (35): Reset pin
- CS Pin State (36): Chip select

**Status & Diagnostics (40-44):**
- Communication Status (40): 0=OK, 1=Error
- Last Error (41): Error message
- Transaction Count (42)
- Error Count (43)
- Uptime (44)

**Control Actions (101-103, 110-111):**
- Init Click Board (101)
- Reset Click Board (102)
- Shutdown Click Board (103)
- Read Manifest (110)
- Manifest Data (111): JSON

**MIKROBUS Standard:**
- SPI: CS, SCK, MISO, MOSI
- I2C: SDA, SCL
- UART: RX, TX
- GPIO: AN, RST, INT, PWM
- Power: 3.3V or 5V selectable

#### Object 34601: Router Management
**Purpose**: Core router configuration

**17 Resources:**
- Router Name
- LAN IP Configuration
- WAN Configuration
- DHCP Server Settings
- DNS Configuration
- Firewall Status
- NAT Configuration

#### Object 34602: Ethernet Interface
**Purpose**: Ethernet port monitoring

**14 Resources (per instance):**
- Interface Name
- MAC Address
- Speed (Mbps)
- Duplex Mode
- Link Status
- TX/RX Statistics
- Error Counters

#### Object 34603: GPIO Control
**Purpose**: LED and GPIO management

**9 Resources (per instance):**
- GPIO Number
- Direction (Input/Output)
- Value (0/1)
- Active Low
- Debounce Time
- Trigger Type

#### Object 34604: USB Management
**Purpose**: USB port control

**10 Resources (per instance):**
- Port Number
- Device Connected
- Device Type
- Vendor ID / Product ID
- Power Control
- Device Path

#### Object 34605: Storage Management
**Purpose**: Storage device management

**13 Resources (per instance):**
- Device Name
- Mount Point
- Total Size
- Used Space
- Available Space
- Filesystem Type
- Health Status

#### Object 34606: System Monitor
**Purpose**: System health monitoring

**14 Resources:**
- CPU Usage (%)
- CPU Frequency (MHz)
- CPU Temperature (°C)
- RAM Total (MB)
- RAM Used (MB)
- RAM Free (MB)
- System Load (1/5/15 min)
- Uptime (seconds)
- Process Count

#### Object 34609: Firewall Config
**Purpose**: Firewall rule management

#### Object 34610: PoE Management
**Purpose**: Power over Ethernet control

### v1.2.0 Extended Objects

#### Object 10525: WAN Failover Policy
**Purpose**: Multi-WAN failover management with mwan3

**Key Features:**
- Failover, load-balance, round-robin modes
- Weight-based traffic distribution
- Policy-based routing rules
- Automatic failover/failback

#### Object 10526: Multi-WAN Health Check
**Purpose**: WAN connection health monitoring

**Probe Types:**
- ICMP ping
- HTTP/HTTPS
- DNS query
- TCP connection

**Metrics:**
- Latency
- Jitter
- Packet loss
- State machine (UP, DOWN, DEGRADED)

#### Object 10527: WiFi Client Management
**Purpose**: Connected client management

**Features:**
- Client enumeration
- RSSI monitoring
- TX/RX bandwidth tracking
- Band steering (802.11v)
- MAC-based ACL

#### Object 10528: WiFi Channel Optimization
**Purpose**: Automatic channel selection

**Features:**
- Automatic Channel Selection (ACS)
- DFS support
- Channel survey
- Interference detection
- Multi-band optimization

#### Object 10535: LAN Configuration
**Purpose**: LAN network management

**Features:**
- Interface configuration (IP, subnet mask, gateway)
- DHCP server management
- DNS settings
- UCI integration

#### Object 10536: Routing Table
**Purpose**: Static and dynamic routing

**Features:**
- IPv4/IPv6 dual-stack
- Policy-based routing
- Multi-table support (main, local, custom)
- Route state monitoring

#### Object 10537: VPN Configuration
**Purpose**: Multi-protocol VPN management

**Supported Protocols:**
- OpenVPN client
- WireGuard
- IPsec/IKEv2 (strongSwan)

**Features:**
- Kill switch
- Split tunneling
- Key generation (WireGuard)
- Traffic statistics

#### Object 33440: prplMesh
**Purpose**: Multi-AP mesh network management

**Features:**
- EasyMesh R2/R4 integration
- Topology discovery
- Client steering
- Channel selection coordination
- Backhaul optimization

#### Objects 10529-10531: Matter/Thread Integration

**Object 10529: Matter Bridge**
- Controller management
- Device commissioning
- Session management

**Object 10530: Matter Device**
- Device instance management
- 10 cluster implementations

**Object 10531: Thread Network**
- Thread Border Router
- Network credentials
- Topology management

#### Objects 10532-10534: Zigbee Integration

**Object 10532: Zigbee Coordinator**
- Network formation
- Device discovery
- EZSP/Z-Stack support

**Object 10533: Zigbee Device**
- Device management
- 10 ZCL cluster handlers

**Object 10534: Zigbee Group**
- Group control
- Scene management

---

## 6. Transport & Security

### Transport Bindings

#### UDP/DTLS (CoAP)
**Default Transport**

**Configuration:**
```cpp
// Server URI format
"coap://server.example.com:5683"     // No security
"coaps://server.example.com:5684"    // DTLS
```

**Characteristics:**
- Low overhead
- Best for resource-constrained devices
- NAT-friendly with keep-alive
- Block-wise transfer for large resources

#### MQTT Transport (v1.1.0+)
**OMA LwM2M v1.2.2 Section 8 Compliance**

**Supported Versions:**
- MQTT 3.1.1
- MQTT 5.0

**Message Encoding:**
- CBOR (RFC 8949)

**Topic Structure:**
```
{prefix}/{direction}/{endpoint}[/{operation}]

Examples:
lwm2m/c2s/device-001/register
lwm2m/s2c/device-001/read/3/0/0
```

**QoS Mapping:**
- QoS 0: At most once
- QoS 1: At least once (confirmable)
- QoS 2: Exactly once

**Configuration:**
```cpp
// Object 24: MQTT Server
mqttServer->set<STRING_T>(BROKER_URI, "mqtt://broker.example.com");
mqttServer->set<INT_T>(PORT, 1883);
mqttServer->set<STRING_T>(CLIENT_ID, "lwm2m-device-001");
mqttServer->set<INT_T>(QOS, 1);
mqttServer->set<INT_T>(KEEP_ALIVE, 60);
```

### Security Implementation

#### DTLS 1.2
**Cipher Suites:**
- TLS_PSK_WITH_AES_128_CCM_8
- TLS_PSK_WITH_AES_128_CBC_SHA256
- TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
- TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256

#### Pre-Shared Key (PSK) Mode
```cpp
security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, 0);  // PSK
security->set<OPAQUE_T>(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3,
                        "device-identity");
security->set<OPAQUE_T>(Lwm2mSecurity::SECRET_KEY_5,
                        "preshared-secret-key");
```

#### Certificate Mode
```cpp
security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, 2);  // Certificate
security->set<OPAQUE_T>(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3,
                        deviceCertificate);
security->set<OPAQUE_T>(Lwm2mSecurity::SECRET_KEY_5,
                        privateKey);
```

#### MQTT with TLS
```cpp
// Object 24: MQTT Server
mqttServer->set<STRING_T>(BROKER_URI, "mqtts://broker.example.com");
mqttServer->set<INT_T>(PORT, 8883);
mqttServer->set<BOOL_T>(TLS_ENABLED, true);

// Object 23: COSE for MQTT credentials
cose->set<OPAQUE_T>(CLIENT_CERTIFICATE, cert);
cose->set<OPAQUE_T>(CLIENT_PRIVATE_KEY, key);
```

---

## 7. Platform Support

### Supported Platforms

#### Linux (Native)
**Distributions:**
- Ubuntu 20.04+ (recommended)
- Debian 10+
- Fedora 33+
- CentOS 8+

**Build:**
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

#### OpenWRT
**Versions:**
- OpenWRT 23.05+
- OpenWRT 24.10+ (recommended)

**Targets:**
- MediaTek MT7981B (OpenWRT One)
- Raspberry Pi 4
- x86_64
- ARMv7/ARMv8

**Package Integration:**
```bash
# In OpenWRT SDK
cd package
git clone --recursive <repo-url> friendly-lwm2m-client
cd friendly-lwm2m-client
ln -s openwrt/Makefile ./Makefile

cd ~/openwrt-sdk
make package/friendly-lwm2m-client/compile V=s
```

**OpenWRT Extended Features:**
- LUCI web interface module
- Hotplug event handlers (network, USB, buttons)
- UCI configuration integration
- Service management via procd

#### Raspberry Pi 4 (ARM64)
**OS:**
- Raspberry Pi OS (64-bit)
- Ubuntu Server for Raspberry Pi

**Cross-Compilation:**
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-rpi4-arm64.cmake ..
make -j$(nproc)
```

#### prplOS / prplWrt
**Platform-Specific Features:**
- Ambiorix bus client (amxb/amxd)
- TR-181 data model mapping
- EasyMesh R2/R4 integration
- prplMesh coordination
- Partition management (A/B slots)
- MTD device access

**Build:**
```bash
cmake -DWITH_PRPL=ON ..
make -j$(nproc)
```

### Platform Abstraction Layer

**Directory Structure:**
```
wpp/platform/
├── platform_abstraction.h    # Abstract interface
├── openwrt/
│   ├── OpenWrtPlatform.cpp
│   ├── OpenWrtDeviceInfo.cpp
│   ├── OpenWrtLocationInfo.cpp
│   └── OpenWrtConnectivityInfo.cpp
├── prpl/
│   ├── PrplPlatform.cpp
│   ├── AmbiorixClient.cpp
│   └── TR181Mapper.cpp
├── linux/
│   └── LinuxPlatform.cpp
└── rpi/
    └── RaspberryPiPlatform.cpp
```

**Key Abstraction APIs:**
```cpp
class PlatformAbstraction {
public:
    // Device information
    virtual std::string getManufacturer() = 0;
    virtual std::string getModelNumber() = 0;
    virtual std::string getSerialNumber() = 0;

    // Network
    virtual std::vector<NetworkInterface> getNetworkInterfaces() = 0;
    virtual int getNetworkBearer() = 0;

    // Location
    virtual bool getLocation(double& lat, double& lon, double& alt) = 0;

    // System
    virtual int getMemoryFree() = 0;
    virtual int getCpuUsage() = 0;
    virtual bool reboot() = 0;
};
```

---

## 8. Advanced Features

### Edge AI Inference (v1.1.0+)

**Backends:**
- TensorFlow Lite
- ONNX Runtime

**Hardware Acceleration:**
- CPU
- GPU (OpenCL, Vulkan)
- NPU (Neural Processing Unit)
- TPU (Tensor Processing Unit)
- DSP (Digital Signal Processor)
- NNAPI (Android)
- CoreML (iOS/macOS)

**Quantization Support:**
- FP32 (32-bit float)
- FP16 (16-bit float)
- INT8 (8-bit integer)
- INT4 (4-bit integer)

**Inference Modes:**
- Synchronous
- Asynchronous
- Streaming
- Batched

**Workflow:**
```
1. Download Model (via Object 33410, Resource: Model URI)
   ↓
2. Validate Model (integrity check)
   ↓
3. Load Model (TFLite/ONNX)
   ↓
4. Configure Inference (quantization, acceleration)
   ↓
5. Run Inference (input → preprocess → infer → postprocess → output)
   ↓
6. Report Results (via LwM2M observations)
```

**Example:**
```cpp
EdgeAIInference* ai = EdgeAIInference::instance(client);

// Configure model
ai->set<STRING_T>(MODEL_URI, "https://server.com/model.tflite");
ai->set<INT_T>(BACKEND, 0);  // 0=TFLite, 1=ONNX
ai->set<INT_T>(QUANTIZATION, 2);  // INT8
ai->set<INT_T>(ACCELERATION, 1);  // GPU

// Execute: Download and load model
ai->execute(LOAD_MODEL, {});

// Run inference
OPAQUE_T inputData = getInputData();
ai->execute(RUN_INFERENCE, inputData);

// Get results
STRING_T results = ai->get<STRING_T>(INFERENCE_RESULTS);
```

### Delta Firmware Update (v1.1.0+)

**Algorithms:**

**1. BSDIFF (Binary Diff)**
- Use: General-purpose binary diffing
- Compression: gzip, bzip2
- Typical reduction: 60-80%

**2. VCDIFF (RFC 3284)**
- Use: Standardized delta encoding
- Compression: lzma, zstd
- Typical reduction: 70-85%

**3. Courgette (Chromium-style)**
- Use: Executable-aware diffing
- Best for: ARM/x86 binaries
- Typical reduction: 85-95%

**Compression Methods:**
- gzip (fast)
- bzip2 (balanced)
- lzma (high compression)
- zstd (fast + good compression)
- brotli (web-optimized)

**A/B Partition Management:**

**Partitioning Scheme:**
```
┌──────────┬──────────┬──────────┬──────────┐
│  Boot    │ Slot A   │ Slot B   │  Data    │
│          │ (Active) │(Standby) │          │
└──────────┴──────────┴──────────┴──────────┘
```

**Update Flow:**
```
1. System running on Slot A
   ↓
2. Download delta to Slot B
   ↓
3. Apply delta patch (old_image + delta → new_image)
   ↓
4. Verify integrity (SHA-256)
   ↓
5. Mark Slot B as active
   ↓
6. Reboot
   ↓
7. Boot from Slot B
   ↓
8. Health check (10 boots)
   ↓
9. If success: Commit Slot B
   If failure: Rollback to Slot A
```

**Rollback Manager:**
```cpp
class RollbackManager {
public:
    // Slot management
    Slot getActiveSlot();
    Slot getStandbySlot();
    bool switchSlot(Slot target);

    // Health monitoring
    void incrementBootCount();
    int getBootCount();
    void markBootSuccessful();

    // Rollback
    bool shouldRollback();
    bool performRollback();
};
```

### MQTT Transport Binding (v1.1.0+)

**Topic Mapping:**

**Registration:**
```
Publish: lwm2m/c2s/{endpoint}/register
Payload: CBOR({lifetime: 300, binding: "Q", objects: [...]})
```

**Read Request:**
```
Subscribe: lwm2m/s2c/{endpoint}/read
Receive: CBOR({msgId: 123, uri: "/3/0/0"})
Publish: lwm2m/c2s/{endpoint}/read/response
Payload: CBOR({msgId: 123, code: 205, value: "Manufacturer Name"})
```

**Observe:**
```
Subscribe: lwm2m/s2c/{endpoint}/observe
Receive: CBOR({msgId: 456, uri: "/6/0/0", pmin: 10, pmax: 60})

... time passes, value changes ...

Publish: lwm2m/c2s/{endpoint}/notify
Payload: CBOR({token: "abc123", uri: "/6/0/0", value: 40.7128})
```

**Send (LwM2M 1.2+):**
```
Publish: lwm2m/c2s/{endpoint}/send
Payload: CBOR({
  "bn": "/3/0/",
  "e": [
    {"n": "0", "vs": "Manufacturer"},
    {"n": "1", "vs": "Model-X"}
  ]
})
```

---

## 9. Build System

### CMake Options Reference

#### Core Options (v1.0.0+)

| Option | Default | Description |
|--------|---------|-------------|
| CMAKE_BUILD_TYPE | Release | Build type (Debug/Release/MinSizeRel) |
| WITH_DTLS | ON | Enable DTLS support |
| WITH_MBEDTLS | ON | Use mbedTLS (alternative: OpenSSL) |
| OPENWRT_BUILD | OFF | Build for OpenWRT platform |

#### v1.1.0 Options

| Option | Default | Description |
|--------|---------|-------------|
| WITH_MQTT | ON | MQTT transport binding |
| WITH_EDGE_AI | ON | Edge AI inference |
| WITH_TFLITE | OFF | TensorFlow Lite backend |
| WITH_ONNX | OFF | ONNX Runtime backend |
| WITH_DELTA_FOTA | ON | Delta firmware updates |
| WITH_BSDIFF | ON | BSDIFF algorithm |
| WITH_VCDIFF | ON | VCDIFF algorithm |
| WITH_COURGETTE | ON | Courgette algorithm |
| WITH_ROLLBACK | ON | A/B partition rollback |

#### v1.2.0 Extended Options

| Option | Default | Description |
|--------|---------|-------------|
| WITH_OPENWRT_EXTENDED | ON | Extended OpenWRT integration |
| WITH_LUCI_MODULE | ON | LUCI web interface |
| WITH_HOTPLUG_HANDLERS | ON | Hotplug event handlers |
| WITH_LAN_CONFIG | ON | LAN Configuration object |
| WITH_ROUTING_TABLE | ON | Routing Table object |
| WITH_VPN_CONFIG | ON | VPN Configuration object |
| WITH_WAN_FAILOVER | ON | WAN Failover Policy |
| WITH_MULTIWAN_HEALTH | ON | Multi-WAN Health Check |
| WITH_WIFI_CLIENT_MGMT | ON | WiFi Client Management |
| WITH_WIFI_CHANNEL_OPT | ON | WiFi Channel Optimization |
| WITH_STARLINK | ON | Starlink Terminal object |
| WITH_STARLINK_GRPC | OFF | Starlink gRPC client |
| WITH_PRPL | OFF | prplOS integration |
| WITH_MATTER | OFF | Matter/Thread integration |
| WITH_ZIGBEE | OFF | Zigbee integration |

### Build Profiles

**Minimal (Development):**
```bash
cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DWITH_MQTT=OFF \
  -DWITH_EDGE_AI=OFF \
  -DWITH_DELTA_FOTA=OFF \
  -DWITH_STARLINK=OFF \
  -DOPENWRT_BUILD=OFF \
  ..
```

**Standard (Production):**
```bash
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DWITH_DTLS=ON \
  -DWITH_MQTT=ON \
  -DWITH_DELTA_FOTA=ON \
  -DWITH_STARLINK=ON \
  ..
```

**Full Features (OpenWRT Extended):**
```bash
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DOPENWRT_BUILD=ON \
  -DWITH_OPENWRT_EXTENDED=ON \
  -DWITH_LUCI_MODULE=ON \
  -DWITH_MQTT=ON \
  -DWITH_EDGE_AI=ON \
  -DWITH_TFLITE=ON \
  -DWITH_DELTA_FOTA=ON \
  -DWITH_STARLINK_GRPC=ON \
  -DWITH_MATTER=ON \
  -DWITH_ZIGBEE=ON \
  ..
```

### Object Configuration (wpp_config.cmake)

**Enable/Disable Objects:**
```cmake
# Mandatory objects (always enabled)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_0_LWM2M_SECURITY)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_1_LWM2M_SERVER)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_M_3_DEVICE)

# Optional OMA objects
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_4_CONNECTIVITY_MONITORING)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_6_LOCATION)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_5_FIRMWARE_UPDATE)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_12_WLAN_CONNECTIVITY)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_13_BEARER_SELECTION)

# Walt Technologies objects
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10512_STARLINK_TERMINAL)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10519_HARDWARE_WATCHDOG)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10520_MIKROBUS)
```

**Wakaama Configuration:**
```cmake
# Client mode
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_CLIENT_MODE)

# Bootstrap support
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_BOOTSTRAP)

# Data formats
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_CBOR)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_SENML_CBOR)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_SENML_JSON)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_JSON)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_TLV)

# CoAP block size (bytes)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_COAP_DEFAULT_BLOCK_SIZE=1024)

# Logging
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_WITH_LOGS)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_ENABLE_LOGS)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=1)  # 0=Debug, 1=Info, 2=Warning, 3=Error
```

---

## 10. API Reference

### WppClient Class

**Constructor:**
```cpp
WppClient(const std::string& endpoint, uint16_t localPort = 0);
```

**Connection Management:**
```cpp
bool connect();                    // Connect to LwM2M server
bool disconnect();                 // Disconnect from server
void process(int timeout_ms);      // Process events (call in main loop)
bool isConnected();                // Check connection status
```

**Registry Access:**
```cpp
WppRegistry& registry();           // Get object registry
```

**Data Operations:**
```cpp
bool send(const DataLink& data);                    // Send single object data
bool send(const DataLink data[], size_t count);     // Send multiple objects
```

**Ownership:**
```cpp
void giveOwnership();              // Transfer ownership to registry
```

**Accessors:**
```cpp
const std::string& getEndpoint();  // Get client endpoint name
```

### WppRegistry Class

**Object Management:**
```cpp
bool registerObj(Object& object);          // Register object
bool deregisterObj(Object& object);        // Deregister object
bool isObjRegistered(Object& object);      // Check if registered
bool isExist(OBJ_ID objId);                // Check object existence
Object* object(OBJ_ID objId);              // Get object by ID
std::vector<Object*>& objects();           // Get all objects
```

**Standard Object Accessors:**
```cpp
Object& device();                          // Device (3)
Object& lwm2mServer();                     // LwM2M Server (1)
Object& lwm2mSecurity();                   // LwM2M Security (0)
Object& connectivityMonitoring();          // Connectivity Monitoring (4)
Object& location();                        // Location (6)
Object& firmwareUpdate();                  // Firmware Update (5)
Object& wlanConnectivity();                // WLAN Connectivity (12)
Object& bearerSelection();                 // Bearer Selection (13)
```

**Custom Object Accessors:**
```cpp
Object& starlinkTerminal();                // Starlink Terminal (10512)
Object& hardwareWatchdog();                // Hardware Watchdog (10519)
Object& mikrobus();                        // MIKROBUS (10520)
// ... 50+ more custom objects
```

### Instance Class

**Resource Access:**
```cpp
Resource* resource(ID_T resId);            // Get resource by ID
```

**Value Setters (Single Instance Resources):**
```cpp
template<typename T>
bool set(ID_T resId, const T& value);
```

**Value Setters (Multiple Instance Resources):**
```cpp
template<typename T>
bool set(ID_T resId, const T& value, ID_T resInstId);
```

**Value Getters (Single Instance Resources):**
```cpp
template<typename T>
T get(ID_T resId);
```

**Value Getters (Multiple Instance Resources):**
```cpp
template<typename T>
T get(ID_T resId, ID_T resInstId);
```

**Notifications:**
```cpp
void notifyResChanged(ID_T resId, ID_T resInstId = 0);
void notifyResChanged(const std::vector<ID_T>& resIds);
```

**Execute Handlers:**
```cpp
using ExecuteCallback = std::function<bool(Instance&, ID_T, const OPAQUE_T&)>;
void setExecute(ID_T resId, ExecuteCallback callback);
```

**Instance Info:**
```cpp
OBJ_ID getObjectID();                      // Get object ID
OBJ_INST_ID_T getInstanceID();             // Get instance ID
```

### WppTaskQueue Class

**Task Management:**
```cpp
using TaskCallback = std::function<bool(WppClient&, void*)>;

static task_id_t addTask(int interval_sec, TaskCallback callback, void* context = nullptr);
static void requestToRemoveTask(task_id_t taskId);
static void processTasks(WppClient& client);
```

### Data Types

```cpp
using ID_T = uint16_t;                     // Resource/Object ID
using OBJ_ID = ID_T;                       // Object ID
using OBJ_INST_ID_T = uint16_t;            // Instance ID
using STRING_T = std::string;              // String type
using INT_T = int64_t;                     // Integer type
using FLOAT_T = double;                    // Float type
using BOOL_T = bool;                       // Boolean type
using TIME_T = time_t;                     // Time type
using OPAQUE_T = std::vector<uint8_t>;     // Opaque data type
using EXECUTE_T = ExecuteCallback;         // Execute handler type

struct OBJ_LINK_T {
    OBJ_ID objId;
    OBJ_INST_ID_T instId;
};

struct DataLink {
    OBJ_LINK_T object;
    std::vector<ID_T> resources;
};
```

### Logging Macros

```cpp
#define WPP_LOGD(tag, fmt, ...)   // Debug
#define WPP_LOGI(tag, fmt, ...)   // Info
#define WPP_LOGW(tag, fmt, ...)   // Warning
#define WPP_LOGE(tag, fmt, ...)   // Error
```

**Usage:**
```cpp
#define TAG "MyObject"

WPP_LOGD(TAG, "Debug message: value=%d", value);
WPP_LOGI(TAG, "Info message: %s", str.c_str());
WPP_LOGW(TAG, "Warning: resource %d not found", resId);
WPP_LOGE(TAG, "Error occurred: %s", error.c_str());
```

---

## 11. Integration Guides

### Quick Start

**Prerequisites:**
- OS: Ubuntu 22.04+ or macOS
- CMake 3.10+
- GCC 8+ or Clang 10+
- OpenSSL 1.1+

**Build Steps:**
```bash
# 1. Clone repository
git clone --recurse-submodules https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client

# 2. Setup environment
sh ./wpp_env_setup.sh

# 3. Create build directory
mkdir build && cd build

# 4. Configure
cmake ..

# 5. Build
make -j$(nproc)

# 6. Run
./examples/WppExample
```

### OpenWRT Integration

**Package Build:**
```bash
# 1. Download OpenWRT SDK
wget https://downloads.openwrt.org/releases/24.10.0/targets/mediatek/filogic/openwrt-sdk-*.tar.xz
tar xf openwrt-sdk-*.tar.xz
cd openwrt-sdk-*/

# 2. Add package
cd package
git clone --recurse-submodules <repo-url> friendly-lwm2m-client
cd friendly-lwm2m-client
ln -s openwrt/Makefile ./Makefile
cd ../..

# 3. Configure
make menuconfig
# Navigate to: Network → IoT → friendly-lwm2m-client (select 'M')

# 4. Build
make package/friendly-lwm2m-client/compile V=s

# 5. Install on OpenWRT device
scp bin/packages/*/packages/friendly-lwm2m-client_*.ipk root@192.168.1.1:/tmp/
ssh root@192.168.1.1
opkg install /tmp/friendly-lwm2m-client_*.ipk

# 6. Start service
/etc/init.d/lwm2m-client start
```

**UCI Configuration:**
```bash
# Server configuration
uci set lwm2m.server=server
uci set lwm2m.server.uri='coaps://server.example.com:5684'
uci set lwm2m.server.identity='device-001'
uci set lwm2m.server.psk='preshared-key'
uci set lwm2m.server.lifetime='300'
uci commit lwm2m

# Location configuration (manual)
uci set lwm2m.location=location
uci set lwm2m.location.latitude='40.7128'
uci set lwm2m.location.longitude='-74.0060'
uci set lwm2m.location.altitude='10'
uci commit lwm2m

# WiFi configuration (loaded from wireless config)
uci set wireless.@wifi-iface[0].ssid='MyNetwork-2.4GHz'
uci set wireless.@wifi-iface[1].ssid='MyNetwork-5GHz'
uci commit wireless
wifi reload
```

**LUCI Web Interface:**
- Navigate to: http://192.168.1.1/cgi-bin/luci/admin/services/lwm2m
- Configure server URI, credentials
- View real-time status
- Start/stop service
- View object resources

### Starlink Integration

**Prerequisites:**
- Starlink dish accessible at 192.168.100.1:9200
- gRPC C++ library
- Starlink protobuf definitions

**Enable Starlink Object:**
```cmake
# wpp/configs/wpp_config.cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10512_STARLINK_TERMINAL)
```

**Build with gRPC:**
```bash
cmake -DWITH_STARLINK_GRPC=ON ..
make
```

**Initialize:**
```cpp
#ifdef OBJ_O_10512_STARLINK_TERMINAL
void starlinkTerminalInit(WppClient &client) {
    client.registry().registerObj(StarlinkTerminal::object(client));
    Instance* starlink = StarlinkTerminal::createInst(client);

    // Configure gRPC endpoint (optional)
    starlink->set<STRING_T>(StarlinkTerminal::GRPC_ENDPOINT_107,
                            "192.168.100.1:9200");
}
#endif
```

**Usage:**
```cpp
Instance* starlink = StarlinkTerminal::instance(client);

// Check connection
std::string state = starlink->get<STRING_T>(StarlinkTerminal::CONNECTION_STATE_3);
if (state == "CONNECTED") {
    // Get performance metrics
    float downlink = starlink->get<FLOAT_T>(StarlinkTerminal::DOWNLINK_THROUGHPUT_10);
    float latency = starlink->get<FLOAT_T>(StarlinkTerminal::POP_PING_LATENCY_12);

    // Get GPS location
    if (starlink->get<BOOL_T>(StarlinkTerminal::GPS_READY_80)) {
        double lat = starlink->get<FLOAT_T>(StarlinkTerminal::LATITUDE_83);
        double lon = starlink->get<FLOAT_T>(StarlinkTerminal::LONGITUDE_84);
    }

    // Check obstructions
    float obstructed = starlink->get<FLOAT_T>(StarlinkTerminal::FRACTION_OBSTRUCTED_30);
    if (obstructed > 0.1) {
        // Sky view obstructed
    }
}
```

### Matter/Thread Integration

**Enable Matter:**
```bash
cmake -DWITH_MATTER=ON ..
make
```

**Initialize:**
```cpp
#ifdef WITH_MATTER
void matterBridgeInit(WppClient &client) {
    client.registry().registerObj(MatterBridge::object(client));
    Instance* bridge = MatterBridge::createInst(client);

    // Initialize Matter controller
    bridge->execute(MatterBridge::INIT_CONTROLLER, {});

    // Commission device via QR code
    std::string qrCode = "MT:Y.K9042C00KA0648G00";
    OPAQUE_T data(qrCode.begin(), qrCode.end());
    bridge->execute(MatterBridge::COMMISSION_DEVICE, data);
}
#endif
```

**Supported Clusters:**
- OnOff (0x0006)
- Level Control (0x0008)
- Color Control (0x0300)
- Temperature Measurement (0x0402)
- Occupancy Sensing (0x0406)
- Door Lock (0x0101)
- Window Covering (0x0102)
- Generic Switch (0x003B)
- Thermostat (0x0201)
- Fan Control (0x0202)

### Zigbee Integration

**Enable Zigbee:**
```bash
cmake -DWITH_ZIGBEE=ON ..
make
```

**Initialize:**
```cpp
#ifdef WITH_ZIGBEE
void zigbeeCoordinatorInit(WppClient &client) {
    client.registry().registerObj(ZigbeeCoordinator::object(client));
    Instance* coordinator = ZigbeeCoordinator::createInst(client);

    // Form network
    coordinator->set<INT_T>(ZigbeeCoordinator::CHANNEL, 15);
    coordinator->set<STRING_T>(ZigbeeCoordinator::PAN_ID, "0x1234");
    coordinator->execute(ZigbeeCoordinator::FORM_NETWORK, {});

    // Permit joining
    coordinator->set<BOOL_T>(ZigbeeCoordinator::PERMIT_JOIN, true);
}
#endif
```

**Supported ZCL Clusters:**
- Basic (0x0000)
- OnOff (0x0006)
- Level Control (0x0008)
- Color Control (0x0300)
- Temperature Measurement (0x0402)
- Relative Humidity (0x0405)
- Occupancy Sensing (0x0406)
- IAS Zone (0x0500)
- Metering (0x0702)
- OTA Upgrade (0x0019)

---

## 12. Use Cases & Examples

### Use Case 1: IoT Gateway with Location Tracking

**Scenario:** Deploy routers with Starlink connectivity and GPS tracking

**Objects Used:**
- Object 3: Device (device info)
- Object 6: Location (GPS coordinates)
- Object 10512: Starlink Terminal (satellite connectivity)
- Object 4: Connectivity Monitoring (network status)

**Implementation:**
```cpp
// Initialize objects
deviceInit(client);
locationInit(client);
starlinkTerminalInit(client);
connectivityMonitoringInit(client);

// Location automatically uses Starlink GPS as primary source
// GPS update every 60 seconds

// Send location to server
DataLink locationData = {
    {OBJ_ID::LOCATION, 0},
    {Location::LATITUDE_0, Location::LONGITUDE_1, Location::ALTITUDE_2}
};
client.send(locationData);
```

**Benefits:**
- Real-time location tracking for mobile deployments
- Satellite connectivity in remote areas
- Automatic GPS from Starlink (no external GPS hardware)
- Centralized monitoring via LwM2M server

### Use Case 2: Multi-WAN Router with Automatic Failover

**Scenario:** Office router with WiFi primary, Ethernet backup, LTE failback

**Objects Used:**
- Object 13: Bearer Selection
- Object 10525: WAN Failover Policy
- Object 10526: Multi-WAN Health Check
- Object 12: WLAN Connectivity

**Implementation:**
```cpp
// Configure bearer preference
Instance* bearer = BearerSelection::instance(client);
bearer->set<STRING_T>(BearerSelection::PREFERRED_COMMS_BEARER_0, "21,41,6");  // WiFi, Eth, LTE
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSSI_WLAN_4, -75);
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_SIGNAL_STRENGTH_VAR_9, 8);  // Hysteresis

// Configure multi-WAN failover
Instance* failover = WanFailoverPolicy::instance(client);
failover->set<INT_T>(WanFailoverPolicy::MODE, 0);  // 0=Failover, 1=Load-balance
failover->set<STRING_T>(WanFailoverPolicy::WAN_INTERFACES, "wan1,wan2,wan3");
failover->set<STRING_T>(WanFailoverPolicy::PRIORITY, "1,2,3");

// Configure health checks
Instance* health1 = MultiWanHealthCheck::createInst(client, 0);  // WAN1 (WiFi)
health1->set<STRING_T>(MultiWanHealthCheck::INTERFACE, "wan1");
health1->set<INT_T>(MultiWanHealthCheck::PROBE_TYPE, 0);  // ICMP
health1->set<STRING_T>(MultiWanHealthCheck::TARGET, "8.8.8.8");
health1->set<INT_T>(MultiWanHealthCheck::INTERVAL, 5);  // 5 seconds
```

**Behavior:**
1. Primary WAN (WiFi) is active
2. Health check monitors WiFi every 5 seconds
3. If WiFi fails 3 consecutive probes:
   - Switch to Ethernet (WAN2)
4. If Ethernet fails:
   - Switch to LTE (WAN3)
5. When WiFi recovers (+ hysteresis):
   - Automatically failback to WiFi

### Use Case 3: Smart Home Hub with MIKROBUS Expansion

**Scenario:** Home automation hub with Click board sensors

**Objects Used:**
- Object 10520: MIKROBUS
- Object 3: Device
- Object 10530: Matter Device (for Matter accessories)
- Object 10533: Zigbee Device (for Zigbee sensors)

**Implementation:**
```cpp
// Initialize MIKROBUS socket
Instance* mikrobus = Mikrobus::createInst(client, 0);
mikrobus->set<STRING_T>(Mikrobus::SOCKET_NAME_1, "MIKROBUS-1");

// Configure I2C for temperature sensor (Click board)
mikrobus->set<INT_T>(Mikrobus::ACTIVE_INTERFACE_20, 2);  // I2C
mikrobus->set<INT_T>(Mikrobus::I2C_ADDRESS_21, 0x48);    // Temp sensor address

// Enable power at 3.3V
mikrobus->set<INT_T>(Mikrobus::POWER_VOLTAGE_5, 3300);
mikrobus->set<BOOL_T>(Mikrobus::POWER_STATE_6, true);

// Read temperature
// (Temperature reading via I2C would be in custom implementation)

// Initialize Click board
mikrobus->execute(Mikrobus::INIT_CLICK_BOARD_101, {});

// Periodic task to read sensor and report
WppTaskQueue::addTask(60, [mikrobus](WppClient& client, void* ctx) {
    // Read temperature from sensor via I2C
    // Update LwM2M resource
    // Notify server
    return false;  // Continue running
});
```

**Supported Click Boards:**
- Temperature & Humidity Click
- Ambient Light Click
- Pressure Click
- Motion Click
- OLED Display Click
- ...hundreds more

### Use Case 4: Edge AI Camera with Person Detection

**Scenario:** Security camera with on-device person detection

**Objects Used:**
- Object 33410: Edge AI Inference
- Object 3: Device
- Object 6: Location (camera location)

**Implementation:**
```cpp
// Initialize Edge AI
Instance* ai = EdgeAIInference::instance(client);

// Configure model
ai->set<STRING_T>(EdgeAIInference::MODEL_URI,
                  "https://server.com/person_detection.tflite");
ai->set<INT_T>(EdgeAIInference::BACKEND, 0);           // TensorFlow Lite
ai->set<INT_T>(EdgeAIInference::QUANTIZATION, 2);      // INT8
ai->set<INT_T>(EdgeAIInference::ACCELERATION, 2);      // NPU
ai->set<INT_T>(EdgeAIInference::INFERENCE_MODE, 0);    // Synchronous

// Download and load model
ai->execute(EdgeAIInference::LOAD_MODEL, {});

// Continuous inference task
WppTaskQueue::addTask(1, [ai](WppClient& client, void* ctx) {
    // Capture frame from camera
    OPAQUE_T frame = captureFrame();

    // Run inference
    ai->execute(EdgeAIInference::RUN_INFERENCE, frame);

    // Get results
    STRING_T results = ai->get<STRING_T>(EdgeAIInference::INFERENCE_RESULTS);

    // Parse JSON results
    // If person detected:
    //   - Trigger alert
    //   - Send notification to server

    return false;  // Continue
});
```

**Benefits:**
- On-device processing (privacy)
- Low latency
- Reduced bandwidth (only send alerts, not video)
- Hardware acceleration for real-time inference

### Use Case 5: WiFi Access Point with Client Management

**Scenario:** Enterprise AP with client monitoring and band steering

**Objects Used:**
- Object 12: WLAN Connectivity
- Object 10527: WiFi Client Management
- Object 10528: WiFi Channel Optimization

**Implementation:**
```cpp
// Configure 2.4GHz AP
Instance* wlan24 = WlanConnectivity::createInst(client, 0);
wlan24->set<STRING_T>(WlanConnectivity::SSID_5, "Enterprise-WiFi");
wlan24->set<INT_T>(WlanConnectivity::CHANNEL_9, 6);
wlan24->set<BOOL_T>(WlanConnectivity::AUTO_CHANNEL_10, true);
wlan24->set<INT_T>(WlanConnectivity::STANDARD_14, WlanConnectivity::IEEE_802_11BGN);

// Configure 5GHz AP
Instance* wlan5 = WlanConnectivity::createInst(client, 1);
wlan5->set<STRING_T>(WlanConnectivity::SSID_5, "Enterprise-WiFi");
wlan5->set<INT_T>(WlanConnectivity::CHANNEL_9, 36);
wlan5->set<BOOL_T>(WlanConnectivity::AUTO_CHANNEL_10, true);
wlan5->set<INT_T>(WlanConnectivity::STANDARD_14, WlanConnectivity::IEEE_802_11AC);

// Enable client management
Instance* clientMgmt = WifiClientManagement::instance(client);
clientMgmt->set<BOOL_T>(WifiClientManagement::BAND_STEERING_ENABLED, true);
clientMgmt->set<INT_T>(WifiClientManagement::RSSI_THRESHOLD, -70);  // Steer if RSSI < -70

// Enable channel optimization
Instance* channelOpt = WifiChannelOptimization::instance(client);
channelOpt->set<BOOL_T>(WifiChannelOptimization::ACS_ENABLED, true);
channelOpt->set<INT_T>(WifiChannelOptimization::SURVEY_INTERVAL, 3600);  // 1 hour

// Periodic client monitoring
WppTaskQueue::addTask(30, [clientMgmt](WppClient& client, void* ctx) {
    // Get connected clients
    INT_T clientCount = clientMgmt->get<INT_T>(WifiClientManagement::CLIENT_COUNT);

    // For each client, check RSSI and band steer if needed
    for (int i = 0; i < clientCount; i++) {
        INT_T rssi = clientMgmt->get<INT_T>(WifiClientManagement::CLIENT_RSSI, i);
        if (rssi < -70) {
            // Trigger band steering
            clientMgmt->execute(WifiClientManagement::STEER_CLIENT, {});
        }
    }

    return false;
});
```

**Features:**
- Automatic channel selection to avoid interference
- Band steering (2.4GHz → 5GHz for capable clients)
- Per-client RSSI monitoring
- Client blacklist/whitelist
- Real-time client enumeration

---

## 13. Performance & Resource Usage

### Memory Footprint

**Base Client:**
- Core library: ~500 KB
- Per object: ~200 bytes
- Per instance: ~500 bytes
- Per resource: ~50 bytes

**Example Configuration:**
```
Base: 500 KB
+ 10 objects × 200 bytes = 2 KB
+ 15 instances × 500 bytes = 7.5 KB
+ 150 resources × 50 bytes = 7.5 KB
----------------------------------------
Total: ~517 KB
```

### CPU Usage

**Idle State:**
- Background tasks: ~0.1% CPU
- Observations: ~0.01% per observation

**Active Operations:**
- Resource read: ~100 microseconds
- Resource write: ~200 microseconds
- Notification send: ~1-5 milliseconds
- Firmware update: ~10-50% (during download/apply)
- Edge AI inference: 5-30% (depends on model complexity)

### Network Bandwidth

**Registration:**
- Initial registration: ~1-2 KB
- Periodic update: ~200-500 bytes

**Observations:**
- Per notification: ~100-300 bytes (depends on format)
- CBOR (most efficient): ~100 bytes
- JSON (readable): ~250 bytes

**Firmware Update:**
- Full firmware: Depends on size (typically 5-50 MB)
- Delta firmware: 10-30% of full size
- Block-wise transfer: 1 KB blocks (configurable)

**Example Traffic (24 hours):**
```
Registration: 1 KB (initial) + 500 bytes × 24 (hourly updates) = 13 KB
Observations: 100 bytes × 10 resources × 60 (per minute) × 1440 (minutes) = 86 MB
Firmware update: 0 (no update) or 2-10 MB (delta update)
-----------------------------------------------------------------------
Total: ~86 MB/day (without firmware update)
```

### Scalability

**Tested Limits:**
- Objects: 70+ objects registered
- Instances: 100+ instances per object
- Resources: 150+ resources per instance
- Observations: 50+ concurrent observations
- Tasks: 20+ periodic tasks

**Recommended Limits (embedded):**
- Objects: 10-20
- Instances: 10-20 per object
- Concurrent observations: 10-15
- Task interval: ≥10 seconds

---

## 14. Troubleshooting

### Common Issues

#### Issue: Client Cannot Connect to Server

**Symptoms:**
- Registration fails
- Timeout errors
- No network activity

**Diagnosis:**
```bash
# Check network connectivity
ping server.example.com

# Check port accessibility
nc -zv server.example.com 5683  # CoAP
nc -zv server.example.com 5684  # CoAPS

# Check firewall rules
iptables -L -n | grep 5683

# Enable debug logging
# In wpp_config.cmake:
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=0)
```

**Solutions:**
1. Verify server URI in Security object (Resource 0)
2. Check security mode matches server expectations
3. Verify PSK/certificate credentials
4. Check NAT/firewall allows UDP 5683/5684
5. Test with NoSec mode first (mode 3)

#### Issue: DTLS Handshake Failure

**Symptoms:**
- Connection timeout during handshake
- "DTLS handshake failed" error

**Diagnosis:**
```bash
# Test DTLS connection
openssl s_client -dtls1_2 -connect server.example.com:5684

# Check certificate validity
openssl x509 -in client.crt -noout -dates
```

**Solutions:**
1. Verify PSK identity and key match server
2. Check certificate is valid (not expired)
3. Verify cipher suite compatibility
4. Ensure correct security mode (0=PSK, 2=Certificate)

#### Issue: No Location Data (0.0, 0.0)

**Symptoms:**
- Location object shows default values

**Diagnosis:**
```bash
# Check Starlink connectivity
ping 192.168.100.1
grpcurl -plaintext 192.168.100.1:9200 list

# Check gpsd
ps | grep gpsd
gpspipe -w -n 10 | grep TPV

# Check UCI config
uci show lwm2m.location
```

**Solutions:**
1. Starlink: Verify dish is reachable and GPS is enabled
2. gpsd: Install and configure GPS receiver
3. Manual: Set coordinates via UCI
4. Check GPS source priority in OpenWrtLocationInfo

#### Issue: Firmware Update Fails

**Symptoms:**
- Update state stuck in "Downloading"
- Update result shows error code

**Diagnosis:**
```bash
# Check firmware URI accessibility
wget -O /tmp/firmware.bin "http://server.com/firmware.bin"

# Check available space
df -h

# Check update log
logread | grep firmware
```

**Solutions:**
1. Verify firmware URI is accessible
2. Ensure sufficient storage space
3. Check firmware integrity (SHA-256)
4. Verify delta algorithm compatibility
5. Test with full firmware first (not delta)

#### Issue: MQTT Connection Fails

**Symptoms:**
- MQTT broker unreachable
- Authentication failed

**Diagnosis:**
```bash
# Test MQTT connection
mosquitto_sub -h broker.example.com -p 1883 -t "lwm2m/#" -v

# Check TLS connection
mosquitto_sub -h broker.example.com -p 8883 \
  --cafile ca.crt --cert client.crt --key client.key \
  -t "lwm2m/#" -v
```

**Solutions:**
1. Verify MQTT broker URI (Object 24)
2. Check client ID is unique
3. Verify TLS certificates (Object 23)
4. Test with QoS 0 first
5. Check broker allows topic structure

#### Issue: High CPU Usage

**Symptoms:**
- CPU constantly high
- System sluggish

**Diagnosis:**
```bash
# Monitor CPU
top -p $(pidof lwm2m_client)

# Check task queue
# Add debug logging to WppTaskQueue
```

**Solutions:**
1. Increase task intervals (60s → 120s)
2. Reduce number of concurrent observations
3. Disable unused objects
4. Check for infinite loops in execute handlers
5. Profile with gprof or perf

### Debug Logging

**Enable Verbose Logging:**
```cmake
# wpp/configs/wpp_config.cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=0)  # 0=Debug
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_ENABLE_LOGS)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_WITH_LOGS)
```

**Rebuild and Run:**
```bash
make clean && make
./examples/WppExample 2>&1 | tee lwm2m.log
```

**Log Output:**
```
[DEBUG] [Device] Manufacturer: OpenWrt/Walt Technologies
[DEBUG] [Location] Location initialized from Starlink GPS: lat=40.712800, lon=-74.006000
[INFO] [WppClient] Registration successful
[DEBUG] [StarlinkTerminal] Connection state: CONNECTED
[DEBUG] [WlanConnectivity] SSID: Enterprise-WiFi, Channel: 6
```

### System Diagnostics

**Check Service Status (OpenWRT):**
```bash
/etc/init.d/lwm2m-client status
ps | grep lwm2m
logread | grep lwm2m
```

**Check UCI Configuration:**
```bash
uci show lwm2m
uci show wireless
uci show network
```

**Network Capture:**
```bash
# Capture LwM2M traffic
tcpdump -i any -w lwm2m.pcap port 5683 or port 5684 or port 1883

# Analyze with Wireshark
wireshark lwm2m.pcap
```

---

## 15. External References

### Official Specifications

- [OMA LwM2M v1.2.2 Core Specification](https://www.openmobilealliance.org/release/LightweightM2M/V1_2_2-20230914-A/OMA-TS-LightweightM2M_Core-V1_2_2-20230914-A.pdf)
- [OMA LwM2M Transport Bindings](https://www.openmobilealliance.org/release/LightweightM2M/V1_2_2-20230914-A/OMA-TS-LightweightM2M_Transport-V1_2_2-20230914-A.pdf)
- [OMA LwM2M Registry](https://github.com/OpenMobileAlliance/lwm2m-registry)
- [CoAP RFC 7252](https://www.rfc-editor.org/rfc/rfc7252.html)
- [DTLS 1.2 RFC 6347](https://www.rfc-editor.org/rfc/rfc6347.html)
- [Block-Wise Transfer RFC 7959](https://www.rfc-editor.org/rfc/rfc7959.html)
- [CBOR RFC 8949](https://www.rfc-editor.org/rfc/rfc8949.html)
- [SenML RFC 8428](https://www.rfc-editor.org/rfc/rfc8428.html)

### Related Projects

- [Eclipse Wakaama](https://github.com/eclipse/wakaama) - LwM2M stack
- [Eclipse Leshan](https://github.com/eclipse/leshan) - LwM2M server (for testing)
- [Eclipse Paho](https://www.eclipse.org/paho/) - MQTT client libraries
- [Matter SDK](https://github.com/project-chip/connectedhomeip) - Matter/Thread
- [Starlink gRPC API](https://github.com/starlink-community/starlink-grpc-api)
- [OpenWRT](https://openwrt.org/) - Embedded Linux router OS
- [prplOS](https://prplfoundation.org/) - prpl Foundation operating system

### Documentation

- [Friendly LwM2M Client Documentation](https://friendly-technologies.github.io/Friendly-LwM2M-Client/)
- [Build Instructions](https://friendly-technologies.github.io/Friendly-LwM2M-Client/build_tag.html)
- [Architecture Guide](docs/ARCHITECTURE.md)
- [API Reference](docs/API_REFERENCE.md)
- [Quick Start](docs/QUICKSTART.md)
- [Implementation Guide](docs/IMPLEMENTATION_GUIDE.md)
- [OpenWRT Integration](docs/OPENWRT_INTEGRATION.md)
- [Troubleshooting](docs/TROUBLESHOOTING.md)

### Community & Support

- **GitHub Repository**: https://github.com/Friendly-Technologies/Friendly-LwM2M-Client
- **GitHub Issues**: https://github.com/Friendly-Technologies/Friendly-LwM2M-Client/issues
- **Friendly Technologies**: https://friendly-tech.com
- **Contact**: info@friendly-tech.com

### Tools & Utilities

- **Wireshark** - Packet analyzer with CoAP dissector
- **grpcurl** - Command-line gRPC client (for Starlink)
- **mosquitto_pub/sub** - MQTT command-line tools
- **OpenSSL** - Crypto toolkit and testing
- **CMake** - Build system
- **Doxygen** - Code documentation generator

---

## Appendix: Version Timeline

| Version | Release Date | Key Features | Objects Added |
|---------|-------------|--------------|---------------|
| 1.0.0 | June 2025 | Initial release, core LwM2M stack | 0-6, 12, 13, 34600-34610 |
| 1.1.0 | January 2026 | MQTT, Edge AI, Delta FOTA, A/B rollback | 23, 24, 33410 |
| 1.2.0 | January 2026 | Extended OpenWRT, Matter, Zigbee, prpl, router mgmt | 10525-10537, 33440 |

**Total Object Count: 70+ objects**

---

## Conclusion

The Friendly LwM2M Client is a comprehensive, production-ready implementation of the OMA LwM2M v1.2.2 specification, offering extensive features for IoT device management across multiple platforms. With support for 70+ LwM2M objects, advanced features like Edge AI and delta firmware updates, and seamless integration with OpenWRT, Matter, Zigbee, and prplOS, it provides a robust foundation for modern IoT deployments.

For the latest updates, visit the [GitHub repository](https://github.com/Friendly-Technologies/Friendly-LwM2M-Client) or contact [Friendly Technologies](https://friendly-tech.com).

---

**Document Version:** 1.0
**Generated:** January 2026
**For:** Friendly LwM2M Client v1.2.0
