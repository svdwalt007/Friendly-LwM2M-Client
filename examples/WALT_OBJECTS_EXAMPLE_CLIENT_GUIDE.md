# Walt Technologies LwM2M Complete Object Example Guide

## Overview

This guide demonstrates how to use all **Walt Technologies LwM2M objects** (10512-10537) along with mandatory, optional, and IPSO objects in the Friendly LwM2M Client.

### Object Categories

| Category | Object IDs | Count | Purpose |
|----------|-----------|-------|---------|
| **Mandatory** | 0-3 | 3 | Security, Server, Device |
| **Optional** | 4-13 | 7 | Connectivity, Firmware, Location |
| **IPSO** | 3303-3345 | 18 | Sensors, Actuators, Controls |
| **Walt Objects** | 10512-10537 | 26 | Router, Smart Home, Network |
| **Total** | | **54** | Complete implementation |

---

## Walt Technologies Objects (10512-10537)

### Router & Network Objects (10512-10522)

#### 10512 - Starlink Terminal
**Purpose**: Monitor and control Starlink satellite dish

**Resources**:
- Device ID, Software Version, IP Address
- Uptime, Signal strength, Obstructions
- Downlink/Uplink speeds, Latency
- GPS coordinates, Connection status

**Example**:
```json
{
  "0": "STARLINK-TERM-001",
  "1": "1.0.42",
  "4": -45,
  "5": true,
  "8": 125000000,
  "9": 25000000
}
```

#### 10513 - Router Management
**Purpose**: Core router configuration and monitoring

**Resources**:
- Router name, Firmware version
- Uptime, Health status
- Memory/CPU usage, Temperature
- IP configuration, Mode (NAT/Bridge)

**Example**:
```json
{
  "0": "Walt-Router-X1",
  "1": "1.2.2",
  "6": 95,
  "7": 45.5,
  "12": "192.168.1.1"
}
```

#### 10514 - Ethernet Interface
**Purpose**: Ethernet port management (multi-instance)

**Resources**:
- Interface name, Link status, MAC address
- IP configuration, Speed, Duplex
- Traffic statistics, MTU

**Example** (2 instances):
```json
{
  "id": 0,
  "resources": {
    "0": "eth0",
    "1": true,
    "6": 1000,
    "8": 125000000
  }
}
```

#### 10515 - GPIO Control
**Purpose**: GPIO pin configuration (LEDs, buttons)

**Resources**:
- Pin number, Name, Direction
- Current state, Default state
- Pull resistor configuration

**Example**:
```json
{
  "0": 17,
  "1": "LED_STATUS",
  "2": "output",
  "3": true
}
```

#### 10516 - USB Management
**Purpose**: USB device management

**Resources**:
- Port number, Device name, USB version
- Vendor/Product ID, Connection state
- Speed, Power consumption, Device path

**Example**:
```json
{
  "0": 1,
  "1": "USB Storage",
  "2": "2.0",
  "5": true,
  "6": 480
}
```

#### 10517 - Storage Management
**Purpose**: Filesystem monitoring (multi-instance)

**Resources**:
- Device path, Type (eMMC/SD/USB)
- Total/Used/Free space, Mount point
- Filesystem type, Health percentage

**Example**:
```json
{
  "0": "/dev/mmcblk0p1",
  "1": "eMMC",
  "2": 32000000000,
  "4": 4000000000,
  "8": 85
}
```

#### 10518 - System Monitor
**Purpose**: System-wide resource monitoring

**Resources**:
- CPU temperature, Usage percentage
- Memory (total/used/free), Load average
- Uptime, Core count, Kernel version

**Example**:
```json
{
  "0": 35.5,
  "2": 85,
  "3": 2048,
  "4": 1536,
  "7": 3600
}
```

#### 10519 - Hardware Watchdog
**Purpose**: Watchdog timer management

**Resources**:
- Enabled state, Timeout value
- Reset count, Last reset time
- Auto-reboot enabled, Device path

**Example**:
```json
{
  "0": true,
  "1": 30,
  "5": true,
  "7": "/dev/watchdog"
}
```

#### 10520 - Mikrobus
**Purpose**: Mikrobus expansion socket management

**Resources**:
- Socket ID, Name, Occupied state
- Module type, Version, Protocol
- I2C address, Module name

**Example**:
```json
{
  "0": 1,
  "1": "Socket 1",
  "3": "TEMP_SENSOR",
  "5": "i2c",
  "6": "0x48"
}
```

#### 10521 - Firewall Configuration
**Purpose**: Firewall rules and policies

**Resources**:
- Enabled state, Default policies
- NAT enabled, Connection tracking
- Active connections, Rule count

**Example**:
```json
{
  "0": true,
  "1": "ACCEPT",
  "2": "DROP",
  "5": true,
  "6": 100
}
```

#### 10522 - PoE Management
**Purpose**: Power over Ethernet management (multi-instance)

**Resources**:
- Port number, Name, Enabled state
- Standard (802.3af/at), Power budget
- Voltage, Current, Class, Efficiency

**Example**:
```json
{
  "0": 1,
  "1": "PoE Port 1",
  "2": true,
  "3": "802.3at",
  "5": 25.5,
  "6": 12.5
}
```

---

### Network Management Objects (10525-10528)

#### 10525 - WAN Failover Policy
**Purpose**: Multi-WAN failover configuration

**Resources**:
- Policy name, Primary/Secondary/Tertiary WANs
- Mode (Load balance/Failover), Priority weights
- Failure threshold, Recovery delay
- Current active WAN, Failover count

**Example**:
```json
{
  "0": "Primary_Failover_Policy",
  "1": "wan0",
  "2": "wan1",
  "4": 0,
  "5": 70,
  "8": 3,
  "11": "wan0"
}
```

#### 10526 - Multi-WAN Health Check
**Purpose**: WAN interface health monitoring (multi-instance)

**Resources**:
- Interface name, Test method, Target host
- Interval, Timeout, Threshold
- Health status, Latency statistics
- Bandwidth estimation, Uptime

**Example**:
```json
{
  "0": "wan0",
  "2": "8.8.8.8",
  "4": 30,
  "10": true,
  "11": 15.5,
  "14": 2.3
}
```

#### 10527 - WiFi Client Management
**Purpose**: Connected WiFi client tracking (multi-instance)

**Resources**:
- MAC address, IP address, Hostname
- Signal strength, Link quality
- TX/RX rates, Bandwidth usage
- Connected time, Standard, Frequency band

**Example**:
```json
{
  "0": "AA:BB:CC:DD:EE:01",
  "1": "192.168.1.101",
  "2": "laptop-work",
  "4": -65,
  "7": 866,
  "12": "802.11ac"
}
```

#### 10528 - WiFi Channel Optimization
**Purpose**: WiFi channel selection and optimization (multi-instance)

**Resources**:
- Radio ID, Current channel, Bandwidth
- Auto-optimization enabled, Scan interval
- Channel utilization, Noise floor
- Neighboring APs count, DFS enabled

**Example**:
```json
{
  "0": "radio0",
  "1": 36,
  "2": 80,
  "3": true,
  "6": 35,
  "8": 25
}
```

---

### Smart Home Objects (10529-10534)

#### 10529 - Matter Bridge
**Purpose**: Matter smart home bridge status

**Resources**:
- Status, Device count, Network name
- Commissioning enabled, Fabric ID
- Vendor ID, Product ID, Version

**Example**:
```json
{
  "0": "Active",
  "1": 12,
  "2": "MyMatterNetwork",
  "3": true,
  "5": 65521
}
```

#### 10530 - Matter Device
**Purpose**: Connected Matter devices (multi-instance)

**Resources**:
- Device ID, Vendor/Product ID, Type
- Device name, Manufacturer, Online status
- Firmware version, Serial number
- IP address, Signal strength

**Example**:
```json
{
  "0": "0x0011223344556677",
  "3": 256,
  "4": "On/Off Light",
  "6": true,
  "9": "Smart Bulb Pro"
}
```

#### 10531 - Thread Network
**Purpose**: Thread mesh network management

**Resources**:
- Network name, PAN ID, Channel
- Network key, Commissioner credential
- Creation time, Role, Active routers

**Example**:
```json
{
  "0": "HomeThreadNetwork",
  "1": 4660,
  "2": 15,
  "6": 1,
  "8": 8
}
```

#### 10532 - Zigbee Coordinator
**Purpose**: Zigbee coordinator configuration

**Resources**:
- Network ID, PAN ID, Extended address
- Channel, Network key, Permit join
- Connected devices, Firmware version
- Serial port, Baud rate

**Example**:
```json
{
  "0": 1,
  "1": 6754,
  "3": 15,
  "6": 25,
  "12": "/dev/ttyUSB0",
  "13": 115200
}
```

#### 10533 - Zigbee Device
**Purpose**: Connected Zigbee devices (multi-instance)

**Resources**:
- IEEE address, Network address, Endpoint
- Manufacturer, Model, Firmware version
- Device type, Battery level, Signal strength
- Online status, Cluster IDs

**Example**:
```json
{
  "0": "00124B0022334455",
  "3": "Philips",
  "4": "Hue White A19",
  "6": 1,
  "7": 255,
  "12": true
}
```

#### 10534 - Zigbee Group
**Purpose**: Zigbee device groups (multi-instance)

**Resources**:
- Group ID, Name, Member device IEEEs
- Total devices, Active devices
- Scenes count, Brightness level

**Example**:
```json
{
  "0": 100,
  "1": "Living Room Lights",
  "3": 5,
  "4": 2
}
```

---

### Network Configuration Objects (10535-10537)

#### 10535 - LAN Configuration
**Purpose**: LAN network settings

**Resources**:
- IP address, Subnet mask, DHCP enabled
- DHCP range, Lease time
- DNS servers, Domain name, Hostname

**Example**:
```json
{
  "0": "192.168.1.1",
  "1": "255.255.255.0",
  "2": true,
  "3": "192.168.1.100",
  "4": "192.168.1.200",
  "5": 86400
}
```

#### 10536 - Routing Table
**Purpose**: Network routing entries (multi-instance)

**Resources**:
- Destination, Netmask, Gateway
- Interface, Metric, Flags
- Protocol, Source, MTU

**Example**:
```json
{
  "0": "0.0.0.0",
  "1": "0.0.0.0",
  "2": "192.168.1.1",
  "3": "eth0",
  "4": 0
}
```

#### 10537 - VPN Configuration
**Purpose**: VPN tunnel configuration (multi-instance)

**Resources**:
- Tunnel name, Protocol (OpenVPN/WireGuard)
- Server address, Port, Local/Remote IP
- Encryption, Authentication method
- Connection status, Traffic statistics

**Example**:
```json
{
  "0": "Corporate_VPN",
  "1": 0,
  "2": "vpn.company.com",
  "3": 1194,
  "13": 1,
  "14": 125000000
}
```

---

## Using the Configuration

### 1. Load Configuration JSON

```bash
# Using the complete Walt objects configuration
./friendly_lwm2m -c examples/lwm2m_walt_objects_config.json

# Using standard configuration
./friendly_lwm2m -c examples/lwm2m_objects_config.json
```

### 2. Connect to LwM2M Server

```bash
# Connect to Leshan demo server
./friendly_lwm2m \
  -c examples/lwm2m_walt_objects_config.json \
  -h coaps://leshan.eclipseprojects.io:5684 \
  -n walt-router-demo \
  -p 5683
```

### 3. Test with Leshan Server

1. **Start Leshan**:
   ```bash
   wget https://ci.eclipse.org/leshan/job/leshan/lastSuccessfulBuild/artifact/leshan-server-demo.jar
   java -jar leshan-server-demo.jar
   ```

2. **Access Web UI**: http://localhost:8080

3. **Connect Client**:
   ```bash
   ./friendly_lwm2m -c examples/lwm2m_walt_objects_config.json -h coap://localhost:5683 -n walt-demo
   ```

4. **View Objects**: All 54 objects visible in Leshan UI

---

## Object Instance Examples

### Multi-Instance Objects

Many Walt objects support multiple instances:

**Ethernet Interfaces** (10514):
```json
{
  "10514": {
    "instances": [
      {"id": 0, "resources": {"0": "eth0", "1": true}},
      {"id": 1, "resources": {"0": "eth1", "1": true}},
      {"id": 2, "resources": {"0": "eth2", "1": false}}
    ]
  }
}
```

**WiFi Clients** (10527):
```json
{
  "10527": {
    "instances": [
      {"id": 0, "resources": {"0": "AA:BB:CC:DD:EE:01"}},
      {"id": 1, "resources": {"0": "AA:BB:CC:DD:EE:02"}},
      {"id": 2, "resources": {"0": "AA:BB:CC:DD:EE:03"}}
    ]
  }
}
```

**Zigbee Devices** (10533):
```json
{
  "10533": {
    "instances": [
      {"id": 0, "resources": {"0": "00124B0022334455", "4": "Hue Light"}},
      {"id": 1, "resources": {"0": "00124B0066778899", "4": "Motion Sensor"}},
      {"id": 2, "resources": {"0": "00124B00AABBCCDD", "4": "Temperature"}}
    ]
  }
}
```

---

## Resource Operations

### Read Operations

```bash
# Read Starlink uptime
lwm2mclient read /10512/0/3

# Read router CPU usage
lwm2mclient read /10518/0/2

# Read WiFi client signal strength
lwm2mclient read /10527/0/4

# Read VPN connection status
lwm2mclient read /10537/0/13
```

### Write Operations

```bash
# Set GPIO pin state
lwm2mclient write /10515/0/3 1

# Enable DHCP
lwm2mclient write /10535/0/2 true

# Set WiFi channel
lwm2mclient write /10528/0/1 36

# Enable watchdog
lwm2mclient write /10519/0/0 true
```

### Execute Operations

```bash
# Reboot device
lwm2mclient execute /3/0/4

# Reset statistics
lwm2mclient execute /10518/0/execute

# Scan WiFi channels
lwm2mclient execute /10528/0/execute

# Trigger VPN reconnect
lwm2mclient execute /10537/0/execute
```

---

## Complete Object Summary

### Mandatory Objects (3)

| ID | Name | Purpose |
|----|------|---------|
| 0 | Security | Server credentials |
| 1 | Server | Server configuration |
| 3 | Device | Device information |

### Optional Objects (7)

| ID | Name | Purpose |
|----|------|---------|
| 2 | Access Control | ACL management |
| 4 | Connectivity Monitoring | Network status |
| 5 | Firmware Update | FOTA |
| 6 | Location | GPS coordinates |
| 7 | Connectivity Statistics | Traffic stats |
| 12 | WLAN Connectivity | WiFi status |
| 13 | Bearer Selection | Interface selection |

### IPSO Objects (18)

| ID | Name | Type |
|----|------|------|
| 3303 | Temperature | Sensor |
| 3305 | Power Measurement | Sensor |
| 3312 | Power Control | Actuator |
| 3313 | Accelerometer | Sensor |
| 3314 | Magnetometer | Sensor |
| 3315 | Barometer | Sensor |
| 3323 | Pressure | Sensor |
| 3324 | Humidity | Sensor |
| 3325 | Voltage | Sensor |
| 3326 | Current | Sensor |
| 3327 | Frequency | Sensor |
| 3336 | Positioner | Actuator |
| 3338 | Buzzer | Actuator |
| 3339 | Audio Clip | Media |
| 3340 | Timer | Control |
| 3341 | Text Display | Output |
| 3342 | On/Off Switch | Control |
| 3345 | Joystick | Input |

### Walt Objects (26)

| ID | Name | Category |
|----|------|----------|
| 10512 | Starlink Terminal | Network |
| 10513 | Router Management | Core |
| 10514 | Ethernet Interface | Network |
| 10515 | GPIO Control | Hardware |
| 10516 | USB Management | Hardware |
| 10517 | Storage Management | System |
| 10518 | System Monitor | System |
| 10519 | Hardware Watchdog | System |
| 10520 | Mikrobus | Hardware |
| 10521 | Firewall Configuration | Security |
| 10522 | PoE Management | Power |
| 10525 | WAN Failover Policy | Network |
| 10526 | Multi-WAN Health Check | Network |
| 10527 | WiFi Client Management | WiFi |
| 10528 | WiFi Channel Optimization | WiFi |
| 10529 | Matter Bridge | Smart Home |
| 10530 | Matter Device | Smart Home |
| 10531 | Thread Network | Smart Home |
| 10532 | Zigbee Coordinator | Smart Home |
| 10533 | Zigbee Device | Smart Home |
| 10534 | Zigbee Group | Smart Home |
| 10535 | LAN Configuration | Network |
| 10536 | Routing Table | Network |
| 10537 | VPN Configuration | Network |

---

## Testing Scenarios

### Scenario 1: Router Monitoring

Monitor all router statistics:
```bash
# System resources
read /10518/0/0  # CPU temp
read /10518/0/2  # CPU usage
read /10518/0/4  # Memory used

# Network interfaces
read /10514/0/8  # eth0 RX bytes
read /10514/0/9  # eth0 TX bytes

# WAN health
read /10526/0/10 # wan0 health
read /10526/0/11 # wan0 latency
```

### Scenario 2: Smart Home Control

Manage smart home devices:
```bash
# Matter devices
read /10529/0/1  # Device count
read /10530/0/6  # Device online

# Zigbee devices
read /10532/0/6  # Connected count
read /10533/0/7  # Battery level

# Groups
write /10534/0/6 100  # Set brightness
```

### Scenario 3: Network Failover

Test WAN failover:
```bash
# Check current WAN
read /10525/0/11

# Check health status
read /10526/0/10  # wan0
read /10526/1/10  # wan1

# Trigger failover
write /10525/0/14 10  # Reduce threshold
```

---

## Advanced Usage

### Observe Resources

```bash
# Observe CPU usage
lwm2mclient observe /10518/0/2

# Observe WAN health
lwm2mclient observe /10526/0/10

# Observe WiFi client count
lwm2mclient observe /10527
```

### Bootstrap

```bash
./friendly_lwm2m \
  -b \
  -h coaps://bootstrap.server.com:5684 \
  -c examples/lwm2m_walt_objects_config.json
```

### DTLS Security

```bash
./friendly_lwm2m \
  -c examples/lwm2m_walt_objects_config.json \
  -h coaps://leshan.eclipseprojects.io:5684 \
  -i client-identity \
  -s preshared-secret-key \
  --dtls mbedtls
```

---

## Configuration File Format

### Basic Structure

```json
{
  "lwm2m_objects": {
    "OBJECT_ID": {
      "name": "Object Name",
      "instances": [
        {
          "id": INSTANCE_ID,
          "resources": {
            "RESOURCE_ID": VALUE
          }
        }
      ]
    }
  },
  "metadata": {
    "total_objects": 54,
    "configuration_version": "1.0"
  }
}
```

### Resource Types

- **Integer**: `"5": 100`
- **Float**: `"7": 45.5`
- **String**: `"0": "router-name"`
- **Boolean**: `"2": true`
- **Array**: `"4": ["item1", "item2"]`
- **Object**: `"11": {"0": 0, "1": 5}`

---

## Troubleshooting

### Common Issues

1. **Object not registered**
   - Check if object is enabled in CMake build
   - Verify configuration JSON is valid

2. **Resource not readable**
   - Check resource permissions
   - Verify resource exists in object

3. **Connection timeout**
   - Check server address and port
   - Verify DTLS credentials
   - Check firewall rules

4. **Missing instances**
   - Verify multi-instance objects have unique IDs
   - Check instance creation limits

---

## References

- **Configuration Files**:
  - `examples/lwm2m_walt_objects_config.json` - Complete configuration
  - `examples/lwm2m_objects_config.json` - Standard configuration

- **Documentation**:
  - `docs/OBJECT_ID_QUICK_REFERENCE.md` - Object ID reference
  - `utils/object_maker/*.xml` - Object definitions
  - `wpp/registry/objects/w_105*/` - Object implementations

- **LwM2M Resources**:
  - OMA LwM2M Registry: https://technical.openmobilealliance.org/OMNA/LwM2M/LwM2MRegistry.html
  - IPSO Smart Objects: https://github.com/IPSO-Alliance/pub
  - Leshan Demo: https://leshan.eclipseprojects.io/

---

**Version**: 1.2.2
**Last Updated**: 2026-04-30
**Object Count**: 54 objects (3 mandatory + 7 optional + 18 IPSO + 26 Walt)
**Configuration File**: `examples/lwm2m_walt_objects_config.json`
