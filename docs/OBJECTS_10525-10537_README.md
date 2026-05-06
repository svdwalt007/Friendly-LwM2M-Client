# LwM2M Objects 10525-10537 Configuration Guide

This document describes the Walt Technologies LwM2M objects in the range 10525-10537, how to enable them, and how to populate them with data from the JSON configuration file.

## Object Groups

### Group 1: Objects 10512-10522 (Always Available)
These objects are enabled by default in the build:

| Object ID | Object Name | CMake Flag | Status |
|-----------|-------------|------------|---------|
| 10512 | Starlink Terminal | `WITH_STARLINK` | ✅ Enabled |
| 10513 | Router Management | `WITH_ROUTER_MGMT` | ✅ Enabled |
| 10514 | Ethernet Interface | `WITH_ETHERNET_IFACE` | ✅ Enabled |
| 10515 | GPIO Control | `WITH_GPIO_CONTROL` | ✅ Enabled |
| 10516 | USB Management | `WITH_USB_MGMT` | ✅ Enabled |
| 10517 | Storage Management | `WITH_STORAGE_MGMT` | ✅ Enabled |
| 10518 | System Monitor | `WITH_SYSTEM_MONITOR` | ✅ Enabled |
| 10519 | Hardware Watchdog | Standard object | ✅ Enabled |
| 10520 | MIKROBUS | `WITH_MIKROBUS` | ✅ Enabled |
| 10521 | Firewall Configuration | `WITH_FIREWALL_CONFIG` | ✅ Enabled |
| 10522 | PoE Management | `WITH_POE_MGMT` | ✅ Enabled |

### Group 2: Objects 10525-10537 (Conditionally Available)
These objects require explicit CMake flags to be enabled:

| Object ID | Object Name | CMake Flag | Default |
|-----------|-------------|------------|---------|
| 10525 | WAN Failover Policy | `-DWITH_WAN_FAILOVER=ON` | ❌ Disabled |
| 10526 | Multi-WAN Health Check | `-DWITH_MULTIWAN_HEALTH=ON` | ❌ Disabled |
| 10527 | WiFi Client Management | `-DWITH_WIFI_CLIENT_MGMT=ON` | ❌ Disabled |
| 10528 | WiFi Channel Optimization | `-DWITH_WIFI_CHANNEL_OPT=ON` | ❌ Disabled |
| 10529 | Matter Bridge | `-DWITH_MATTER_BRIDGE=ON` | ❌ Disabled |
| 10530 | Matter Device | `-DWITH_MATTER_BRIDGE=ON` | ❌ Disabled |
| 10531 | Thread Network | `-DWITH_MATTER_BRIDGE=ON` | ❌ Disabled |
| 10532 | Zigbee Coordinator | `-DWITH_ZIGBEE=ON` | ❌ Disabled |
| 10533 | Zigbee Device | `-DWITH_ZIGBEE=ON` | ❌ Disabled |
| 10534 | Zigbee Group | `-DWITH_ZIGBEE=ON` | ❌ Disabled |
| 10535 | LAN Configuration | `-DWITH_LAN_CONFIG=ON` | ❌ Disabled |
| 10536 | Routing Table | `-DWITH_ROUTING_TABLE=ON` | ❌ Disabled |
| 10537 | VPN Configuration | `-DWITH_VPN_CONFIG=ON` | ❌ Disabled |

## Enabling Objects 10525-10537

### Option 1: Enable All Objects

```bash
cd build
cmake .. \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  -DWITH_MATTER_BRIDGE=ON \
  -DWITH_ZIGBEE=ON \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON
make
```

### Option 2: Enable Specific Object Groups

#### Network Management Objects
```bash
cmake .. \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON
```

#### WiFi Management Objects
```bash
cmake .. \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON
```

#### Matter/Thread Objects
```bash
cmake .. -DWITH_MATTER_BRIDGE=ON
```
Note: This enables objects 10529, 10530, and 10531.

#### Zigbee Objects
```bash
cmake .. -DWITH_ZIGBEE=ON
```
Note: This enables objects 10532, 10533, and 10534.

## Object Descriptions and Resources

### 10525 - WAN Failover Policy
Manages multi-WAN failover policies for load balancing and redundancy.

**Key Resources:**
- `0`: Policy Name (String)
- `1-3`: Primary/Secondary/Tertiary WAN interfaces (String)
- `4`: Mode (0=Failover, 1=LoadBalance, 2=RoundRobin)
- `5-7`: Interface weights for load balancing
- `11`: Current active WAN (Read-only)
- `12`: Policy state (Read-only)

**Example Configuration:**
```json
"10525": {
  "instances": [{
    "id": 0,
    "resources": {
      "0": "Primary_Failover_Policy",
      "1": "wan0",
      "2": "wan1",
      "4": 0,
      "5": 70,
      "6": 30
    }
  }]
}
```

### 10526 - Multi-WAN Health Check
Monitors WAN interface health using configurable probes.

**Key Resources:**
- `0`: Interface name (String)
- `1`: Probe type (0=Ping, 1=HTTP, 2=DNS)
- `2`: Target host (String)
- `4`: Probe interval (seconds)
- `8`: Current state (Read-only)
- `11-15`: Latency and packet loss metrics (Read-only)

**Example Configuration:**
```json
"10526": {
  "instances": [
    {
      "id": 0,
      "resources": {
        "0": "wan0",
        "1": 0,
        "2": "8.8.8.8",
        "4": 30,
        "6": 3,
        "7": 2
      }
    }
  ]
}
```

### 10527 - WiFi Client Management
Manages individual WiFi clients connected to access points.

**Key Resources:**
- `0`: MAC Address (Read-only)
- `1`: IP Address (Read-only)
- `2`: Hostname (Read-only)
- `4`: Signal strength/RSSI (dBm, Read-only)
- `7-8`: TX/RX rates (Mbps, Read-only)
- `12`: Capability (802.11 standard)
- `13`: Band (2.4GHz/5GHz/6GHz)
- `15`: ACL state (allowed/denied)

### 10528 - WiFi Channel Optimization
Optimizes WiFi channel selection to minimize interference.

**Key Resources:**
- `0`: Interface name
- `1`: Current channel (Read-only)
- `2`: Channel width (20/40/80/160 MHz)
- `3`: Auto channel enabled (Boolean)
- `6`: Channel utilization percentage (Read-only)
- `8`: Interference level (Read-only)
- `12-14`: Best channel recommendations (Read-only)

### 10529 - Matter Bridge
Matter controller/bridge for commissioning and managing Matter devices.

**Key Resources:**
- `0`: Controller status (String, Read-only)
- `1`: Commissioned devices count (Read-only)
- `2`: Thread network name
- `3`: Thread border router enabled
- `4`: Fabric ID (Read-only)

### 10530 - Matter Device
Represents individual Matter devices on the network.

**Key Resources:**
- `0`: Node ID (hex string, Read-only)
- `1-2`: Vendor/Product ID (Read-only)
- `3-4`: Device type and name (Read-only)
- `6`: Reachable status (Read-only)
- `11`: IP Address (Read-only)

### 10531 - Thread Network
Manages Thread mesh network configuration.

**Key Resources:**
- `0`: Network name
- `1`: PAN ID (0-65535)
- `2`: Channel (11-26)
- `3`: Extended PAN ID (hex string)
- `6`: Border router state (Read-only)
- `8`: Connected devices count (Read-only)

### 10532 - Zigbee Coordinator
Controls Zigbee coordinator/gateway functionality.

**Key Resources:**
- `0`: Network state (Read-only)
- `1`: PAN ID
- `2`: Extended PAN ID
- `3`: Channel (11-26)
- `5`: Permit join status (Read-only)
- `6`: Device count (Read-only)
- `12`: Serial port (/dev/ttyUSB0)
- `13`: Baud rate

### 10533 - Zigbee Device
Represents individual Zigbee devices on the network.

**Key Resources:**
- `0`: IEEE address (Read-only)
- `1`: Network address (Read-only)
- `3-4`: Manufacturer and model (Read-only)
- `7`: LQI (Link Quality, Read-only)
- `12`: Interviewed status (Read-only)
- `13`: Online status (Read-only)

### 10534 - Zigbee Group
Manages Zigbee lighting groups for synchronized control.

**Key Resources:**
- `0`: Group ID
- `1`: Group name
- `2`: Member devices (Read-only)
- `3`: Member count (Read-only)
- `4`: Scene count (Read-only)

### 10535 - LAN Configuration
Configures LAN network and DHCP settings.

**Key Resources:**
- `0`: LAN IP address
- `1`: Subnet mask
- `2`: DHCP enabled
- `3-4`: DHCP pool start/end
- `5`: DHCP lease time (seconds)
- `6-7`: Primary/Secondary DNS servers
- `10`: Active leases (Read-only)

### 10536 - Routing Table
Manages static routing table entries.

**Key Resources:**
- `0`: Destination network (IP/CIDR)
- `1`: Netmask/prefix length
- `2`: Gateway (next hop IP)
- `3`: Interface name
- `4`: Metric (route priority)
- `5`: Protocol (route origin, Read-only)

### 10537 - VPN Configuration
Configures VPN connections (OpenVPN, WireGuard, etc.).

**Key Resources:**
- `0`: VPN name
- `1`: Protocol type (0=OpenVPN, 1=WireGuard, 2=IPsec)
- `2-3`: Server address and port
- `5`: Remote network (CIDR)
- `13`: Connection state (Read-only)
- `14-15`: Bytes sent/received (Read-only)
- `16`: Uptime (seconds, Read-only)

## JSON Configuration Examples

### Complete Configuration Sample
See `examples/lwm2m_objects_config_SAMPLE.json` for a comprehensive example with all objects.

### Minimal Configuration
See `examples/lwm2m_objects_config.json` for the full working configuration.

### Custom Configuration Template

```json
{
  "lwm2m_objects": {
    "10525": {
      "name": "WAN Failover Policy",
      "instances": [{
        "id": 0,
        "resources": {
          "0": "MyFailoverPolicy",
          "1": "wan0",
          "2": "wan1"
        }
      }]
    },
    "10535": {
      "name": "LAN Configuration",
      "instances": [{
        "id": 0,
        "resources": {
          "0": "192.168.1.1",
          "1": "255.255.255.0",
          "2": true
        }
      }]
    }
  }
}
```

## Notes

### Current Limitations

1. **Objects 10525-10537 require manual registration** in main.cpp if you enable them, as they don't have initialization functions in objects.cpp yet.

2. **ConfigLoader Integration**: The current ConfigLoader in main.cpp only registers objects 10512-10522. To use objects 10525-10537, you need to:
   - Enable the CMake flags
   - Add initialization functions to objects.cpp (if needed)
   - Register them with ConfigLoader in main.cpp

### Future Enhancements

1. Auto-discovery of enabled objects
2. Dynamic object registration based on CMake flags
3. Initialization function generation for all objects
4. Validation of resource types and ranges

## Testing

After enabling objects, verify they're available:

```bash
# Build with objects enabled
cd build
cmake .. -DWITH_ZIGBEE=ON -DWITH_MATTER_BRIDGE=ON
make friendly_example_client

# Run client
cd examples
./friendly_example_client

# Check console output for:
# "Loaded configuration for X objects"
# Verify object count includes your enabled objects
```

## See Also

- `CONFIG_LOADER_README.md` - Configuration loader documentation
- `BUILD_AND_TEST.md` - Building and testing guide
- `examples/lwm2m_objects_config.json` - Full configuration file
- `examples/lwm2m_objects_config_SAMPLE.json` - Annotated sample
