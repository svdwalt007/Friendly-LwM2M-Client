# OpenWRT One - Comprehensive LwM2M Client Design

## Executive Summary

This document outlines the comprehensive LwM2M implementation for the OpenWRT One router, mapping all hardware resources and interfaces to OMA LwM2M Registry Objects and creating custom Walt Technologies objects for router-specific functionality.

## OpenWRT One Hardware Specifications

### Processor & Memory
- **SoC**: MediaTek MT7981B (dual ARM Cortex-A53 @ 1.3 GHz)
- **RAM**: 1 GB DDR4
- **Storage**: 256 MB SPI NAND + 16 MB protected recovery flash
- **Expansion**: M.2 2230/2242 NVMe PCIe 2.0 x1 slot

### Network Interfaces
- **WiFi**: MediaTek MT7976C
  - 2x2 2.4 GHz (WiFi 6)
  - 3x3/2x2 5 GHz (WiFi 6) with zero-wait DFS
- **Ethernet**:
  - 1x 2.5 Gbps WAN port
  - 1x 1 Gbps LAN port
- **Support**: WPA, WPA2, WPA3

### Expansion Ports
- **USB**: 1x USB 2.0 Type-A host port
- **USB-C**: 1x USB Type-C port
- **MikroBUS**: Socket for expansion modules
- **PoE**: Power over Ethernet support

### Control Interfaces
- LEDs, GPIOs, buttons

---

## LwM2M Object Mapping

### 1. Core Mandatory Objects (Already Implemented)

| Object ID | Name | Purpose | Status |
|-----------|------|---------|--------|
| 0 | LwM2M Security | Bootstrap server credentials, PSK/DTLS configuration | ✓ Existing |
| 1 | LwM2M Server | Server registration, lifetime, binding mode | ✓ Existing |
| 3 | Device | Device info, manufacturer, model, reboot, factory reset | ✓ Existing |

### 2. Standard Network & Connectivity Objects

| Object ID | Name | Purpose | Implementation Status |
|-----------|------|---------|----------------------|
| 4 | Connectivity Monitoring | Network bearer, IP addresses, signal strength | ✓ Existing |
| 5 | Firmware Update | Remote firmware update capability | ✓ Existing |
| 12 | WLAN Connectivity | WiFi interface management (2.4GHz + 5GHz) | **NEW - To Implement** |
| 13 | Bearer Selection | Preferred network bearer selection | **NEW - To Implement** |

### 3. IPSO Smart Objects for Monitoring

| Object ID | Name | Purpose | Implementation Status |
|-----------|------|---------|----------------------|
| 3331 | Memory | RAM usage monitoring | **NEW - To Implement** |
| 3341 | Addressable Text Display | Status display | Optional |
| 3202 | Analogue Input | Voltage/power monitoring | Optional |
| 3300 | Generic Sensor | Temperature sensors | Optional |

### 4. Walt Technologies Custom Objects (Vendor Range: 10513-34649)

#### Object 10513: Router Management
**Purpose**: Core router configuration and management
**Resources**:
- 0: Router Name (RW, String)
- 1: LAN IP Address (RW, String)
- 2: LAN Subnet Mask (RW, String)
- 3: DHCP Enabled (RW, Boolean)
- 4: DHCP Start IP (RW, String)
- 5: DHCP End IP (RW, String)
- 6: DHCP Lease Time (RW, Integer) - seconds
- 7: DNS Server 1 (RW, String)
- 8: DNS Server 2 (RW, String)
- 9: WAN Connection Type (RW, Integer) - 0=DHCP, 1=Static, 2=PPPoE
- 10: WAN IP Address (R, String)
- 11: WAN Gateway (RW, String)
- 12: Firewall Enabled (RW, Boolean)
- 13: NAT Enabled (RW, Boolean)
- 14: UPnP Enabled (RW, Boolean)
- 15: Apply Configuration (E, Execute)
- 16: Reset to Defaults (E, Execute)

#### Object 10514: Ethernet Interface Management
**Purpose**: Manage LAN/WAN ethernet ports
**Multiple Instances**: Yes (one per physical port)
**Resources**:
- 0: Interface Name (R, String) - e.g., "eth0", "eth1"
- 1: Port Type (R, Integer) - 0=LAN, 1=WAN
- 2: Speed (R, Integer) - Mbps (10/100/1000/2500)
- 3: Duplex (R, Integer) - 0=Half, 1=Full
- 4: Link Status (R, Boolean) - Up/Down
- 5: MAC Address (R, String)
- 6: MTU (RW, Integer)
- 7: Total Bytes Sent (R, Integer)
- 8: Total Bytes Received (R, Integer)
- 9: Total Packets Sent (R, Integer)
- 10: Total Packets Received (R, Integer)
- 11: Transmit Errors (R, Integer)
- 12: Receive Errors (R, Integer)
- 13: Interface Enabled (RW, Boolean)

#### Object 10515: GPIO Control
**Purpose**: LED and GPIO pin management
**Multiple Instances**: Yes (one per GPIO/LED)
**Resources**:
- 0: GPIO Name (R, String) - e.g., "LED_STATUS", "LED_WLAN"
- 1: GPIO Number (R, Integer)
- 2: GPIO Type (R, Integer) - 0=LED, 1=Button, 2=General GPIO
- 3: Current State (RW, Boolean) - For LEDs/outputs
- 4: Blink Enabled (RW, Boolean)
- 5: Blink Interval (RW, Integer) - milliseconds
- 6: Trigger Mode (RW, Integer) - 0=None, 1=Netdev, 2=Timer, 3=Default-on
- 7: Button State (R, Boolean) - For buttons
- 8: Button Press Count (R, Integer)

#### Object 10516: USB Port Management
**Purpose**: USB port monitoring and control
**Multiple Instances**: Yes (one per USB port)
**Resources**:
- 0: Port Name (R, String) - e.g., "USB1", "USB2"
- 1: Port Type (R, Integer) - 0=USB-A 2.0, 1=USB-C
- 2: Port Enabled (RW, Boolean)
- 3: Device Connected (R, Boolean)
- 4: Device Name (R, String)
- 5: Device Vendor ID (R, String)
- 6: Device Product ID (R, String)
- 7: Device Class (R, Integer) - USB device class
- 8: Power Enabled (RW, Boolean)
- 9: Current Draw (R, Integer) - mA

#### Object 10517: Storage Management
**Purpose**: Monitor and manage storage devices (NAND, NVMe, USB)
**Multiple Instances**: Yes (one per storage device)
**Resources**:
- 0: Storage Name (R, String) - e.g., "NAND", "NVMe", "USB_Storage"
- 1: Storage Type (R, Integer) - 0=NAND, 1=NVMe, 2=USB, 3=SD
- 2: Mount Point (R, String)
- 3: Total Capacity (R, Integer) - bytes
- 4: Used Capacity (R, Integer) - bytes
- 5: Available Capacity (R, Integer) - bytes
- 6: Usage Percentage (R, Integer) - 0-100
- 7: Filesystem Type (R, String) - e.g., "ext4", "f2fs"
- 8: Is Mounted (R, Boolean)
- 9: Is Bootable (R, Boolean)
- 10: Mount (E, Execute)
- 11: Unmount (E, Execute)
- 12: Format (E, Execute)

#### Object 10518: System Resources Monitor
**Purpose**: CPU, memory, and thermal monitoring
**Resources**:
- 0: CPU Usage Percentage (R, Integer) - 0-100
- 1: CPU Frequency (R, Integer) - MHz
- 2: CPU Temperature (R, Float) - Celsius
- 3: RAM Total (R, Integer) - bytes
- 4: RAM Used (R, Integer) - bytes
- 5: RAM Free (R, Integer) - bytes
- 6: RAM Cached (R, Integer) - bytes
- 7: RAM Buffers (R, Integer) - bytes
- 8: Swap Total (R, Integer) - bytes
- 9: Swap Used (R, Integer) - bytes
- 10: Load Average 1min (R, Float)
- 11: Load Average 5min (R, Float)
- 12: Load Average 15min (R, Float)
- 13: Uptime (R, Integer) - seconds
- 14: Process Count (R, Integer)

#### Object 10521: Firewall Configuration
**Purpose**: Firewall rule management
**Multiple Instances**: Yes (one per rule)
**Resources**:
- 0: Rule Name (RW, String)
- 1: Rule Index (R, Integer)
- 2: Enabled (RW, Boolean)
- 3: Action (RW, Integer) - 0=Accept, 1=Reject, 2=Drop
- 4: Protocol (RW, Integer) - 0=All, 1=TCP, 2=UDP, 3=ICMP
- 5: Source IP (RW, String)
- 6: Source Port (RW, Integer)
- 7: Dest IP (RW, String)
- 8: Dest Port (RW, Integer)
- 9: Interface (RW, String) - e.g., "wan", "lan"
- 10: Direction (RW, Integer) - 0=Input, 1=Output, 2=Forward
- 11: Zone From (RW, String) - e.g., "wan", "lan"
- 12: Zone To (RW, String) - e.g., "lan", "wan"
- 13: Comment (RW, String)
- 14: Apply Rule (E, Execute)
- 15: Delete Rule (E, Execute)

#### Object 10522: PoE Management
**Purpose**: Power over Ethernet monitoring and control
**Resources**:
- 0: PoE Supported (R, Boolean)
- 1: PoE Enabled (RW, Boolean)
- 2: Input Voltage (R, Float) - volts
- 3: Input Current (R, Float) - amperes
- 4: Input Power (R, Float) - watts
- 5: PoE Standard (R, String) - e.g., "802.3af", "802.3at", "802.3bt"
- 6: PoE Class (R, Integer) - 0-8 (802.3bt Class 1-8)
- 7: Max Power Available (R, Float) - watts
- 8: Power Allocated (R, Float) - watts
- 9: Temperature (R, Float) - Celsius
- 10: Status (R, String) - "Enabled", "Disabled", "Error"
- 11: Reset PoE (E, Execute) - Power cycle the PoE

#### Object 10520: MIKROBUS
**Purpose**: MIKROBUS module detection and management
**Multiple Instances**: Yes (one per socket)
**Resources**: 50+ resources
- Socket Information (0-6): Socket ID, name, enabled state, Click board detection
- Power Management (5-6): Voltage selection (3.3V/5V), power state
- Interface Configuration (20-27): Active interface (SPI/I2C/UART), settings
- GPIO Control (31-36): PWM, INT, RST, CS, AN pin control
- Click Board Info (40-50): Vendor, product, serial number, capabilities
- Manifest Management (110-115): Manifest data, parsing, validation
**See**: docs/MIKROBUS_OBJECT.md for complete documentation

---

## Implementation Plan

### Phase 1: Core Configuration
1. Enable LwM2M Bootstrap support
2. Configure bootstrap server URI: `coap://demo-iot.friendly-tech.com:5680`
3. Set endpoint name: `walttech888`
4. Update security mode (NoSec for CoAP vs DTLS for CoAPS)

### Phase 2: Standard Object Implementation
1. Implement Object 12 (WLAN Connectivity) - Priority: HIGH
2. Implement Object 13 (Bearer Selection) - Priority: MEDIUM
3. Enhance Object 4 (Connectivity Monitoring) for ethernet
4. Add Object 3331 (Memory) for RAM monitoring

### Phase 3: Walt Technologies Custom Objects
1. Object 10512: Starlink Terminal (satellite connectivity) - ✅ Implemented
2. Object 10513: Router Management (core functionality) - ✅ Implemented
3. Object 10514: Ethernet Interface Management - ✅ Implemented
4. Object 10515: GPIO Control (LEDs, buttons) - ✅ Implemented
5. Object 10516: USB Port Management - ✅ Implemented
6. Object 10517: Storage Management - ✅ Implemented
7. Object 10518: System Resources Monitor - ✅ Implemented
8. Object 10519: Hardware Watchdog - ✅ Implemented
9. Object 10520: MIKROBUS Expansion - ✅ Implemented
10. Object 10521: Firewall Configuration - ✅ Implemented
11. Object 10522: PoE Management - ✅ Implemented

### Phase 4: OpenWRT Integration
1. Create OpenWRT package Makefile
2. Implement OpenWRT platform layer (read from UCI, ubus, procfs, sysfs)
3. Create init.d startup script
4. Add SDK build configuration for v24.10.4

### Phase 5: Testing & Validation
1. Test bootstrap connection
2. Validate all object read/write operations
3. Test firmware update flow
4. Verify configuration persistence

---

## OpenWRT Platform Integration

### Data Sources for Objects

| Object | Data Source | OpenWRT Tool/API |
|--------|-------------|------------------|
| Device Info | `/etc/openwrt_release`, `/proc/cpuinfo` | Direct file read |
| WLAN Connectivity | UCI wireless config | `uci get wireless`, `iw`, `iwinfo` |
| Network Interfaces | `/sys/class/net/*`, UCI network | `ip`, `ethtool`, `uci` |
| System Resources | `/proc/meminfo`, `/proc/stat`, `/proc/loadavg` | Direct file read |
| Storage | `/proc/mounts`, `df`, `/sys/block/*` | `df`, `mount` |
| Firewall | UCI firewall config | `uci get firewall`, `iptables` |
| USB | `/sys/bus/usb/devices/*` | `lsusb`, sysfs |
| GPIO/LED | `/sys/class/leds/*`, `/sys/class/gpio/*` | sysfs |

### Build System Integration

```cmake
# Add OpenWRT-specific definitions
set(WPP_DEFINITIONS ${WPP_DEFINITIONS}
    OPENWRT_BUILD
    OPENWRT_VERSION="24.10.4"
)

# Link OpenWRT libraries (uci, ubox, ubus)
target_link_libraries(WppExample uci ubox ubus)
```

---

## Bootstrap Configuration

### Security Object Instance (Object 0)

```cpp
security->set<STRING_T>(Lwm2mSecurity::LWM2M_SERVER_URI_0,
    "coap://demo-iot.friendly-tech.com:5680");
security->set<BOOL_T>(Lwm2mSecurity::BOOTSTRAP_SERVER_1, true);
security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2,
    LWM2M_SECURITY_MODE_NONE); // NoSec for CoAP
security->set<INT_T>(Lwm2mSecurity::CLIENT_HOLD_OFF_TIME_11, 10);
```

### Client Identity

```cpp
WppClient::create({"walttech888", "", ""}, connection, wppErrorHandler);
```

---

## File Structure

```
wpp/registry/objects/
├── o_12_wlan_connectivity/        # ✅ WLAN Connectivity (WiFi 2.4GHz + 5GHz)
├── o_13_bearer_selection/         # ✅ Bearer Selection (Network preferences)
├── o_10512_starlink_terminal/     # ✅ Starlink Terminal (Satellite)
├── o_10519_hardware_watchdog/     # ✅ Hardware Watchdog
├── o_10520_mikrobus/              # ✅ MIKROBUS (Click boards)
├── w_10513_router_management/     # ✅ Router Management
├── w_10514_ethernet_interface/    # ✅ Ethernet Interface
├── w_10515_gpio_control/          # ✅ GPIO Control (LEDs/buttons)
├── w_10516_usb_management/        # ✅ USB Management
├── w_10517_storage_management/    # ✅ Storage Management
├── w_10518_system_monitor/        # ✅ System Monitor (CPU/RAM)
├── w_10521_firewall_config/       # ✅ Firewall Configuration
└── w_10522_poe_management/        # ✅ PoE Management
```

---

## Build Targets

### Standard CMake Build
```bash
mkdir _build && cd _build
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
cmake --build . --target WppOpenWrtClient
```

### OpenWRT SDK Build
```bash
# In OpenWRT SDK
./scripts/feeds update -a
./scripts/feeds install -a
make package/friendly-lwm2m-client/compile V=s
```

---

## Implementation Status

1. ✅ Design document created
2. ✅ Standard objects implemented:
   - ✅ Object 12 (WLAN Connectivity) - WiFi management
   - ✅ Object 13 (Bearer Selection) - Network bearer preferences
3. ✅ Walt Technologies custom objects implemented:
   - ✅ Object 10512 (Starlink Terminal) - Satellite connectivity
   - ✅ Object 10513 (Router Management) - Core router configuration
   - ✅ Object 10514 (Ethernet Interface) - Ethernet port management
   - ✅ Object 10515 (GPIO Control) - LED and GPIO management
   - ✅ Object 10516 (USB Management) - USB port control
   - ✅ Object 10517 (Storage Management) - Storage device management
   - ✅ Object 10518 (System Monitor) - CPU/RAM/Load monitoring
   - ✅ Object 10519 (Hardware Watchdog) - Watchdog timer
   - ✅ Object 10520 (MIKROBUS) - Click board management
   - ✅ Object 10521 (Firewall Configuration) - Firewall rules
   - ✅ Object 10522 (PoE Management) - Power over Ethernet
4. ⏳ Create OpenWRT package integration
5. ⏳ Test with OpenWRT SDK v24.10.4

---

## References

- [OMA LwM2M Registry](https://github.com/OpenMobileAlliance/lwm2m-registry)
- [OpenWRT One Specifications](https://www.cnx-software.com/2024/10/02/buy-openwrt-one-wifi-6-router-filogic-820-soc/)
- [OpenWRT v24.10.4 SDK](https://openwrt.org/)
- [MediaTek MT7981B Datasheet](https://www.mediatek.com/)
- [Friendly LwM2M Client](https://github.com/Friendly-Technologies/Friendly-LwM2M-Client)
