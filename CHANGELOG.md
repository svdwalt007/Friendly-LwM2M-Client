# Changelog

All notable changes to the Friendly LwM2M Client project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2026-01-08

### Added

#### OpenWRT Extended Integration
- **LUCI Web Module** (`openwrt/luci/`) - Complete web interface for LwM2M client management
  - Main dashboard with real-time status monitoring
  - Object configuration interface
  - Security settings management (DTLS, certificates)
  - Service control (start/stop/restart)
  - ACL-based access control
- **Hotplug Event Handlers** (`openwrt/files/etc/hotplug.d/`)
  - Network interface events (99-lwm2m)
  - USB device events (99-lwm2m)
  - Network device events (99-lwm2m)
  - Button press events (99-lwm2m)

#### Router Management LwM2M Objects
- **LAN Configuration Object (34621)** - Full LAN network management
  - Interface configuration (IP, DHCP, DNS)
  - DHCP server management
  - UCI configuration persistence
- **Routing Table Object (34622)** - Static and dynamic routing
  - IPv4/IPv6 dual-stack support
  - Policy-based routing
  - Multi-table routing (main, local, custom)
  - Route state monitoring
- **VPN Configuration Object (34623)** - Multi-protocol VPN management
  - OpenVPN client support
  - WireGuard interface management with key generation
  - IPsec/IKEv2 (strongSwan) integration
  - Kill switch and split tunneling
  - Traffic statistics

#### Multi-WAN and Failover
- **WAN Failover Policy Object (34611)** - Multi-WAN failover management
  - mwan3 integration for OpenWRT
  - Failover, load-balance, and round-robin modes
  - Weight-based traffic distribution
  - Policy-based routing rules
  - Automatic failover/failback
- **Multi-WAN Health Check Object (34612)** - WAN health monitoring
  - ICMP, HTTP, DNS, TCP probe types
  - Asynchronous probe execution
  - Latency, jitter, and packet loss tracking
  - State machine with configurable thresholds
  - mwan3 track integration

#### WiFi/WLAN Management
- **WiFi Client Management Object (34613)** - Connected client management
  - Client enumeration with MAC, IP, hostname
  - Real-time RSSI monitoring
  - TX/RX bandwidth tracking
  - Band steering (802.11v BSS-TM)
  - MAC-based ACL (whitelist/blacklist)
  - hostapd integration
- **WiFi Channel Optimization Object (34614)** - Channel management
  - Automatic Channel Selection (ACS)
  - DFS (Dynamic Frequency Selection) support
  - Channel survey and interference detection
  - Multi-band optimization (2.4/5/6 GHz)
  - Historical performance tracking

#### Starlink Integration
- **Starlink gRPC Client** (`src/starlink/`)
  - Connection to Starlink dish (192.168.100.1:9200)
  - Device info, status, history retrieval
  - Dish stow/unstow/reboot control
  - Obstruction map retrieval
  - WiFi configuration management
- **Starlink Data Collector** - Periodic metric collection
  - Configurable polling intervals
  - Metric aggregation and history
  - Observer pattern for LwM2M notifications
- **Enhanced Starlink Terminal Object (34600)** - Full gRPC integration

#### prpl Platform Integration
- **prpl Platform Abstraction** (`src/platform/prpl/`)
  - prplOS detection and initialization
  - Partition management (A/B slots)
  - Flash operations (MTD devices)
  - Bootloader control (u-boot environment)
- **Ambiorix Bus Client** - Full amxb/amxd integration
  - Connection management
  - Object CRUD operations
  - Event subscription
- **TR-181 Data Model Mapping**
  - Device.WiFi mapping
  - Device.Ethernet mapping
  - Device.IP mapping
  - Device.Hosts mapping
- **EasyMesh R2/R4 Integration**
  - Topology discovery
  - Client steering
  - Channel selection coordination
  - Backhaul optimization
- **prplMesh LwM2M Object (33440)** - Mesh network management

#### Matter/Thread Integration
- **Matter SDK Integration** (`src/matter/`)
  - Matter controller stack
  - Device commissioning (QR, BLE, WiFi, Thread)
  - Session management
  - Thread Border Router support
- **Matter Cluster Client** - 10 cluster implementations
  - OnOff, Level Control, Color Control
  - Temperature, Occupancy, Door Lock
  - Window Covering, Generic Switch
- **Matter Bridge Object (34615)** - Controller management
- **Matter Device Object (34616)** - Device instance management
- **Thread Network Object (34617)** - Thread BR configuration

#### Zigbee Integration
- **Zigbee Coordinator** (`src/zigbee/`)
  - EZSP (Silicon Labs) support
  - Z-Stack (Texas Instruments) support
  - Network formation and management
  - Device discovery and interviewing
- **ZCL Cluster Handlers** - 10 cluster implementations
  - Basic, OnOff, Level Control
  - Color Control, Temperature, Humidity
  - Occupancy, IAS Zone, Metering, OTA
- **Zigbee OTA Proxy** - Firmware upgrade distribution
  - Multi-device coordination
  - Block transfer management
- **Zigbee Coordinator Object (34618)** - Network management
- **Zigbee Device Object (34619)** - Device management
- **Zigbee Group Object (34620)** - Group control

#### OMA Registry XML Files
- 34611.xml - WAN Failover Policy
- 34612.xml - Multi-WAN Health Check
- 34613.xml - WiFi Client Management
- 34614.xml - WiFi Channel Optimization
- 34615.xml - Matter Bridge
- 34616.xml - Matter Device
- 34617.xml - Thread Network
- 34618.xml - Zigbee Coordinator
- 34619.xml - Zigbee Device
- 34620.xml - Zigbee Group
- 34621.xml - LAN Configuration
- 34622.xml - Routing Table
- 34623.xml - VPN Configuration

#### Documentation
- PRPL_INTEGRATION.md - prplOS/prplMesh integration guide
- MATTER_INTEGRATION.md - Matter/Thread integration guide
- ZIGBEE_INTEGRATION.md - Zigbee integration guide
- CHANGELOG.md - This file

### Changed

- **CMakeLists.txt** - Extended with 30+ new build options
  - OpenWRT extended integration options
  - Router management object options
  - Multi-WAN options
  - WiFi management options
  - Starlink integration options
  - prpl integration options
  - Matter integration options
  - Zigbee integration options
  - Enhanced configuration summary output

- **StarlinkTerminal Object (34600)** - Integrated gRPC client
  - Real-time telemetry from dish
  - Execute handlers use actual gRPC commands
  - Alert system integration

### Fixed

- All identified gaps from Gap Analysis document
- Build system integration for new components
- Conditional compilation for optional features

## [1.1.0] - 2026-01-08

### Added

- **MQTT Transport Binding** - Full OMA LwM2M v1.2.2 Section 8 compliance
  - MQTT 3.1.1 and 5.0 support
  - CBOR message encoding (RFC 8949)
  - QoS mapping (0, 1, 2)

- **Delta Firmware Update System**
  - BSDIFF algorithm implementation
  - VCDIFF algorithm (RFC 3284)
  - Courgette algorithm (Chromium-style)
  - Compression support (gzip, bzip2, lzma, zstd, brotli)
  - Block-wise transfer (RFC 7959)

- **A/B Partition & Rollback**
  - Snapshot management
  - Slot switching
  - Boot verification
  - Automatic rollback on failure

- **Edge AI Inference**
  - TensorFlow Lite backend
  - ONNX Runtime backend
  - Hardware acceleration (CPU, GPU, NPU, TPU)

- **New LwM2M Objects**
  - LwM2M COSE Object (ID: 23)
  - MQTT Server Object (ID: 24)
  - Edge AI Inference Object (ID: 33410)

### Changed

- Documentation updated to v1.1.0
- README.md with new features
- Build system with new options

## [1.0.0] - 2025-06-01

### Added

- Initial release
- Core LwM2M client implementation
- OMA standard objects (0-6, 12, 13)
- Walt Technologies custom objects (34600-34610)
- OpenWRT integration
- DTLS security support
- Multiple data format support

---

## Object ID Reference

| ID | Name | Version Added |
|----|------|---------------|
| 0 | LwM2M Security | 1.0.0 |
| 1 | LwM2M Server | 1.0.0 |
| 2 | LwM2M Access Control | 1.0.0 |
| 3 | Device | 1.0.0 |
| 4 | Connectivity Monitoring | 1.0.0 |
| 5 | Firmware Update | 1.0.0 |
| 6 | Location | 1.0.0 |
| 12 | WLAN Connectivity | 1.0.0 |
| 13 | Bearer Selection | 1.0.0 |
| 23 | LwM2M COSE | 1.1.0 |
| 24 | MQTT Server | 1.1.0 |
| 33410 | Edge AI Inference | 1.1.0 |
| 34611 | WAN Failover Policy | 1.2.0 |
| 34612 | Multi-WAN Health Check | 1.2.0 |
| 34613 | WiFi Client Management | 1.2.0 |
| 34614 | WiFi Channel Optimization | 1.2.0 |
| 33440 | prplMesh | 1.2.0 |
| 34615 | Matter Bridge | 1.2.0 |
| 34616 | Matter Device | 1.2.0 |
| 34617 | Thread Network | 1.2.0 |
| 34618 | Zigbee Coordinator | 1.2.0 |
| 34619 | Zigbee Device | 1.2.0 |
| 34620 | Zigbee Group | 1.2.0 |
| 34621 | LAN Configuration | 1.2.0 |
| 34622 | Routing Table | 1.2.0 |
| 34623 | VPN Configuration | 1.2.0 |
| 34600 | Starlink Terminal | 1.0.0 |
| 34601 | Router Management | 1.0.0 |
| 34602 | Ethernet Interface | 1.0.0 |
| 34603 | GPIO Control | 1.0.0 |
| 34604 | USB Management | 1.0.0 |
| 34605 | Storage Management | 1.0.0 |
| 34606 | System Monitor | 1.0.0 |
| 34607 | Hardware Watchdog | 1.0.0 |
| 34608 | MIKROBUS | 1.0.0 |
| 34609 | Firewall Config | 1.0.0 |
| 34610 | PoE Management | 1.0.0 |

---

## Build Options Reference

### Core Options (v1.0.0+)
| Option | Default | Description |
|--------|---------|-------------|
| WITH_DTLS | ON | Enable DTLS support |
| WITH_MBEDTLS | ON | Use mbedTLS |
| WITH_DELTA_FOTA | ON | Enable delta firmware updates |
| WITH_ROLLBACK | ON | Enable A/B partition rollback |
| WITH_MQTT | ON | Enable MQTT transport |
| WITH_EDGE_AI | ON | Enable Edge AI inference |

### Extended Options (v1.2.0+)
| Option | Default | Description |
|--------|---------|-------------|
| WITH_OPENWRT_EXTENDED | ON | Extended OpenWRT integration |
| WITH_LUCI_MODULE | ON | Build LuCI web interface |
| WITH_LAN_CONFIG | ON | LAN Configuration object |
| WITH_ROUTING_TABLE | ON | Routing Table object |
| WITH_VPN_CONFIG | ON | VPN Configuration object |
| WITH_WAN_FAILOVER | ON | WAN Failover Policy object |
| WITH_MULTIWAN_HEALTH | ON | Multi-WAN Health Check object |
| WITH_WIFI_CLIENT_MGMT | ON | WiFi Client Management |
| WITH_WIFI_CHANNEL_OPT | ON | WiFi Channel Optimization |
| WITH_STARLINK | ON | Starlink integration |
| WITH_STARLINK_GRPC | OFF | Starlink gRPC (requires gRPC) |
| WITH_PRPL | OFF | prplOS integration |
| WITH_MATTER | OFF | Matter/Thread integration |
| WITH_ZIGBEE | OFF | Zigbee integration |
