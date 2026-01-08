# Walt Technologies Starlink Terminal Object (ID: 10512)

## Overview

The **Starlink Terminal** LwM2M object provides comprehensive management and monitoring capabilities for SpaceX Starlink Gen3 satellite internet terminals. This custom object enables remote device management platforms to monitor performance, track obstructions, manage dish alignment, and control Starlink functionality through the standard OMA LwM2M protocol.

**Object ID:** 10512 (Walt Technologies Custom Object Range)
**URN:** `urn:oma:lwm2m:x:10512:1.0`
**Version:** 1.0
**Instance Type:** Single Instance

## Features

### Core Capabilities

- **Device Information Monitoring**: Track hardware/software versions, uptime, and connection state
- **Network Performance Metrics**: Real-time monitoring of throughput, latency, and packet loss
- **Obstruction Detection**: Monitor dish obstructions by direction and severity
- **Dish Alignment**: Track and control dish pointing (azimuth/elevation)
- **Comprehensive Alert System**: 16+ distinct alert conditions with individual flags
- **GPS Location Tracking**: Monitor device location and GPS status
- **Power Management**: Track power consumption and energy usage
- **Remote Control**: Execute speed tests, reboots, and retrieve obstruction maps
- **Historical Statistics**: Access latency deciles, packet loss history, and load-based performance

## Resource Definitions

### Device Information (Resources 0-9)

| Resource ID | Name | Type | Operations | Description |
|-------------|------|------|------------|-------------|
| 0 | Device ID | String | R | Starlink serial number |
| 1 | Hardware Version | String | R | Terminal hardware version |
| 2 | Software Version | String | R | Installed firmware version |
| 3 | Connection State | String | R | Current state (CONNECTED, SEARCHING, etc.) |
| 4 | Uptime | Integer | R | Seconds since last reboot |

**Connection States:**
- `UNKNOWN` - State unknown
- `CONNECTED` - Connected to satellite network
- `BOOTING` - Terminal booting
- `SEARCHING` - Searching for satellites
- `STOWED` - Dish is stowed
- `THERMAL_SHUTDOWN` - Thermal protection shutdown
- `NO_SATS` - No satellites visible
- `OBSTRUCTED` - View obstructed
- `NO_DOWNLINK` - No downlink signal
- `NO_PINGS` - No connectivity to PoP

### Network Performance & Status (Resources 10-29)

| Resource ID | Name | Type | Operations | Mandatory | Description |
|-------------|------|------|------------|-----------|-------------|
| 10 | Downlink Throughput | Float | R | Yes | Download speed (bits/sec) |
| 11 | Uplink Throughput | Float | R | Yes | Upload speed (bits/sec) |
| 12 | PoP Ping Latency | Float | R | Yes | Round-trip latency (ms) |
| 13 | PoP Ping Drop Rate | Float | R | Yes | Packet loss fraction (0.0-1.0) |
| 14 | Seconds to First Slot | Float | R | Optional | Seconds until satellite available |
| 15 | SNR | Float | R | Optional | Signal-to-noise ratio |
| 16 | SNR Above Noise Floor | Boolean | R | Optional | SNR > 3dB threshold |
| 17 | Download Usage | Integer | R | Optional | Total bytes downloaded |
| 18 | Upload Usage | Integer | R | Optional | Total bytes uploaded |

### Obstruction Monitoring (Resources 30-39)

| Resource ID | Name | Type | Operations | Mandatory | Description |
|-------------|------|------|------------|-----------|-------------|
| 30 | Fraction Obstructed | Float | R | Yes | Obstruction area fraction (0.0-1.0) |
| 31 | Currently Obstructed | Boolean | R | Yes | Current obstruction status |
| 32 | Obstruction Duration | Float | R | Optional | Avg prolonged obstruction duration (sec) |
| 33 | Obstruction Interval | Float | R | Optional | Avg time between obstructions (sec) |
| 34 | Obstruction Valid S | Float | R | Optional | Data completeness measure |
| 35 | Wedges Fraction Obstructed | String | R | Optional | JSON array of 12 wedge obstructions (30° each) |

**Wedges Format (Resource 35):**
```json
[0.0, 0.12, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
```
12 values representing obstruction fraction for each 30° wedge, starting from North going clockwise.

### Dish Alignment (Resources 40-49)

| Resource ID | Name | Type | Operations | Mandatory | Description |
|-------------|------|------|------------|-----------|-------------|
| 40 | Direction Azimuth | Float | R | Yes | Dish pointing azimuth (degrees, 0-360) |
| 41 | Direction Elevation | Float | R | Yes | Dish pointing elevation (degrees, 0-90) |
| 42 | Dish Stowed | Boolean | RW | Optional | Stow/unstow dish control |
| 43 | Dish Alignment Status | String | R | Optional | Alignment status description |

### Alert System (Resources 50-79)

| Resource ID | Name | Type | Operations | Mandatory | Description |
|-------------|------|------|------------|-----------|-------------|
| 50 | Alerts Bitmap | Integer | R | Yes | Combined alerts bit field |
| 51 | Motors Stuck | Boolean | R | Yes | Bit 0: Motors stuck alert |
| 52 | Thermal Shutdown | Boolean | R | Yes | Bit 1: Thermal shutdown alert |
| 53 | Thermal Throttle | Boolean | R | Yes | Bit 2: Thermal throttling alert |
| 54 | Unexpected Location | Boolean | R | Yes | Bit 3: Unexpected location alert |
| 55 | Mast Not Vertical | Boolean | R | Yes | Bit 4: Mast not near vertical alert |
| 56 | Slow Ethernet | Boolean | R | Yes | Bit 5: Slow Ethernet speeds alert |
| 57 | Roaming | Boolean | R | Yes | Bit 6: Roaming alert |
| 58 | Install Pending | Boolean | R | Yes | Bit 7: Install pending alert |
| 59 | Is Heating | Boolean | R | Yes | Bit 8: Dish heating alert |
| 60 | Power Supply Throttle | Boolean | R | Yes | Bit 9: Power supply thermal throttle |
| 61 | Is Power Save Idle | Boolean | R | Yes | Bit 10: Power save idle mode |
| 62 | Low Motor Current | Boolean | R | Optional | Bit 15: Low motor current alert |
| 63 | Lower Signal | Boolean | R | Optional | Bit 16: Lower signal than predicted |
| 64 | Obstruction Map Reset | Boolean | R | Optional | Bit 18: Obstruction map reset |
| 65 | Dish Water Detected | Boolean | R | Optional | Bit 19: Water detected in dish |
| 66 | Router Water Detected | Boolean | R | Optional | Bit 20: Water detected in router |

### GPS Location (Resources 80-89)

| Resource ID | Name | Type | Operations | Mandatory | Description |
|-------------|------|------|------------|-----------|-------------|
| 80 | GPS Ready | Boolean | R | Yes | GPS position ready |
| 81 | GPS Enabled | Boolean | R | Yes | GPS usage enabled |
| 82 | GPS Satellites | Integer | R | Yes | Active GPS satellites count |
| 83 | Latitude | Float | R | Optional | Current latitude (degrees, -90 to 90) |
| 84 | Longitude | Float | R | Optional | Current longitude (degrees, -180 to 180) |
| 85 | Altitude | Float | R | Optional | Current altitude (meters) |

### Power Management (Resources 90-99)

| Resource ID | Name | Type | Operations | Mandatory | Description |
|-------------|------|------|------------|-----------|-------------|
| 90 | Latest Power | Float | R | Yes | Most recent power consumption (watts) |
| 91 | Mean Power | Float | R | Yes | Average power consumption (watts) |
| 92 | Min Power | Float | R | Optional | Minimum power (watts) |
| 93 | Max Power | Float | R | Optional | Maximum power (watts) |
| 94 | Total Energy | Float | R | Optional | Total energy consumed (kWh) |

### Control Actions (Resources 100-109)

| Resource ID | Name | Type | Operations | Mandatory | Description |
|-------------|------|------|------------|-----------|-------------|
| 100 | Reboot Terminal | - | E | Yes | Execute: Reboot the Starlink terminal |
| 101 | Start Speed Test | - | E | Yes | Execute: Initiate speed test |
| 102 | Speed Test Status | String | R | Yes | Speed test status/results (JSON) |
| 103 | Factory Reset | - | E | Optional | Execute: Factory reset terminal (DISABLED by default) |
| 104 | Get Obstruction Map | - | E | Optional | Execute: Retrieve obstruction map |
| 105 | Obstruction Map Data | Opaque | R | Optional | Obstruction map binary data |
| 106 | Refresh Telemetry | - | E | Yes | Execute: Force telemetry refresh |
| 107 | gRPC Endpoint | String | RW | Yes | gRPC endpoint (default: 192.168.100.1:9200) |

**Speed Test Status Format (Resource 102):**
```json
{
  "status": "idle|running|complete|error",
  "progress": 0-100,
  "download_mbps": 150.5,
  "upload_mbps": 20.3,
  "latency_ms": 35
}
```

### Historical Data (Resources 110-129)

| Resource ID | Name | Type | Operations | Mandatory | Description |
|-------------|------|------|------------|-----------|-------------|
| 110 | History Samples | Integer | R | Yes | Number of history samples available |
| 111 | History Interval | Integer | R | Yes | History sample interval (seconds, default: 1) |
| 112 | Mean Ping Latency | Float | R | Yes | Mean latency (all samples, ms) |
| 113 | Mean Full Ping Latency | Float | R | Yes | Mean latency (0% loss only, ms) |
| 114 | Stdev Full Ping Latency | Float | R | Optional | Latency std deviation (ms) |
| 115 | Total Ping Drop | Float | R | Yes | Total sample intervals with loss |
| 116 | Count Full Ping Drop | Integer | R | Yes | Samples with 100% loss |
| 117 | Count Obstructed | Integer | R | Yes | Obstruction-marked samples |
| 118 | Latency Deciles | String | R | Optional | JSON array of latency deciles [min, d1, ..., d9, max] |
| 119 | Load Bucket Stats | String | R | Optional | JSON load bucket statistics |

**Latency Deciles Format (Resource 118):**
```json
[20.5, 25.3, 28.1, 30.2, 32.5, 35.0, 37.8, 40.1, 45.2, 52.3, 78.9]
```
11 values: minimum, 9 deciles (10th, 20th, ..., 90th percentile), maximum

**Load Bucket Stats Format (Resource 119):**
```json
{
  "buckets": [
    {"min_bw": 0, "max_bw": 1048576, "samples": 120, "min_latency": 25, "median_latency": 35, "max_latency": 80},
    ...
  ]
}
```

## Integration Guide

### Prerequisites

The Starlink Terminal object requires:

1. **gRPC C++ Library** - For communication with Starlink hardware
2. **Protobuf Compiler** - To generate Starlink API bindings
3. **Network Access** - Connection to Starlink Dish (default: 192.168.100.1:9200)

### Building with Starlink Support

1. **Enable the object in CMake configuration:**

Edit `wpp/configs/wpp_config.cmake`:
```cmake
# Include Walt Technologies Starlink Terminal object
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10512_STARLINK_TERMINAL)
```

2. **Add gRPC dependencies** (when implementing full gRPC integration):

Edit `wpp/registry/objects/o_10512_starlink_terminal/CMakeLists.txt`:
```cmake
find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)

set(WPP_LINK_LIBRARIES ${WPP_LINK_LIBRARIES}
    gRPC::grpc++
    protobuf::libprotobuf
    PARENT_SCOPE
)
```

3. **Generate Starlink Protobuf Definitions:**

```bash
# Extract protobuf from Starlink dish using grpcurl
grpcurl -plaintext -protoset-out dish.protoset 192.168.100.1:9200 describe SpaceX.API.Device.Device

# Convert to .proto files
protoc --decode_raw < dish.protoset > dish_decoded.txt

# Generate C++ bindings
protoc --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` dish.proto
protoc --cpp_out=. dish.proto
```

### Implementing gRPC Integration

The object provides a framework with TODOs for gRPC integration. Key methods to implement:

**File:** `StarlinkTerminal.cpp`

1. **updateTelemetry()** - Main telemetry update function
```cpp
bool StarlinkTerminal::updateTelemetry() {
    // Create gRPC channel
    std::string endpoint = get<STRING_T>(GRPC_ENDPOINT_107);
    auto channel = grpc::CreateChannel(endpoint, grpc::InsecureChannelCredentials());
    auto stub = Device::NewStub(channel);

    // Call GetStatus()
    // Call GetHistory()
    // Parse and update resources

    return true;
}
```

2. **Execute Callbacks** - Implement control actions:
   - `executeReboot()` - Call gRPC Reboot
   - `executeSpeedTest()` - Call gRPC SpeedTest
   - `executeGetObstructionMap()` - Call gRPC DishGetObstructionMap

### Usage Example

```cpp
#include "WppClient.h"
#include "StarlinkTerminal.h"

// Initialize Starlink object
void starlinkTerminalInit(WppClient &client) {
    // Register object
    client.registry().registerObj(StarlinkTerminal::object(client));

    // Create instance
    Instance *starlink = StarlinkTerminal::createInst(client);

    // Optional: Configure gRPC endpoint
    starlink->set<STRING_T>(StarlinkTerminal::GRPC_ENDPOINT_107, "192.168.100.1:9200");

    // Access Control (optional)
    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(StarlinkTerminal::object(client),
                               Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    #endif
}

// In main loop - periodically update telemetry
void updateLoop() {
    WppClient *client = WppClient::takeOwnership();
    if (client) {
        // Get Starlink instance
        Instance *starlink = StarlinkTerminal::instance(*client);

        // Read current values
        float latency = starlink->get<FLOAT_T>(StarlinkTerminal::POP_PING_LATENCY_12);
        float downlink = starlink->get<FLOAT_T>(StarlinkTerminal::DOWNLINK_THROUGHPUT_10);
        bool obstructed = starlink->get<BOOL_T>(StarlinkTerminal::CURRENTLY_OBSTRUCTED_31);

        client->giveOwnership();
    }
}
```

## Configuration Options

### Optional Resources

Optional resources can be disabled to reduce memory footprint:

Edit `StarlinkTerminalConfig.h`:

```c
// Disable advanced obstruction monitoring
#define RES_10512_35 0  // Wedges Fraction Obstructed

// Disable power statistics
#define RES_10512_92 0  // Min Power
#define RES_10512_93 0  // Max Power

// Disable factory reset for safety
#define RES_10512_103 0  // Factory Reset
```

### Resource Configuration Summary

| Compile Flag | Resource | Default |
|--------------|----------|---------|
| RES_10512_14-18 | Network usage/SNR stats | Enabled |
| RES_10512_32-35 | Advanced obstruction data | Enabled |
| RES_10512_42-43 | Dish control/status | Enabled |
| RES_10512_62-66 | Extended alerts | Enabled |
| RES_10512_83-85 | GPS coordinates | Enabled |
| RES_10512_92-94 | Power statistics | Enabled |
| RES_10512_103 | Factory reset | **DISABLED** (safety) |
| RES_10512_104-105 | Obstruction map | Enabled |
| RES_10512_114 | Latency stdev | Enabled |
| RES_10512_118-119 | Advanced statistics | Enabled |

## OpenWRT Integration

For OpenWRT One router deployment:

1. **Create OpenWRT Package:**

```makefile
# Makefile
include $(TOPDIR)/rules.mk

PKG_NAME:=lwm2m-starlink-client
PKG_VERSION:=1.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/lwm2m-starlink-client
  SECTION:=net
  CATEGORY:=Network
  TITLE:=LwM2M Client with Starlink Support
  DEPENDS:=+libstdcpp +grpc +protobuf
endef

# Build, installation steps...
```

2. **Add to OpenWRT build system:**

```bash
cd openwrt
./scripts/feeds update packages
./scripts/feeds install lwm2m-starlink-client
make menuconfig  # Select package
make package/lwm2m-starlink-client/compile
```

3. **Configure for Starlink WAN:**

Ensure the OpenWRT router can reach:
- **Starlink Dish:** 192.168.100.1:9200 (gRPC)
- **Starlink Router:** 192.168.1.1:9000 (gRPC, for router stats)

## Monitoring & Diagnostics

### Key Metrics to Monitor

**Health Indicators:**
- Resource 3 (Connection State) - Should be "CONNECTED"
- Resource 31 (Currently Obstructed) - Should be `false`
- Resource 50 (Alerts Bitmap) - Should be `0` (no alerts)

**Performance Indicators:**
- Resource 12 (Latency) - Typical: 20-40ms
- Resource 13 (Packet Loss) - Should be < 0.01 (1%)
- Resource 10/11 (Throughput) - Compare against expected bandwidth

**Signal Quality:**
- Resource 30 (Fraction Obstructed) - Should be < 0.10 (10%)
- Resource 40/41 (Dish Direction) - Monitor for stability

### Alert Handling

When Alert Bitmap (Resource 50) is non-zero:

1. Check individual alert flags (Resources 51-66)
2. Common alerts and responses:
   - **Thermal Throttle (52):** Reduce power draw or improve ventilation
   - **Obstruction (via state):** Check Resource 35 for obstruction direction
   - **Slow Ethernet (56):** Verify cable quality
   - **Water Detected (65/66):** Inspect seals and connections

## Performance Characteristics

### Typical Starlink Gen3 Performance

| Metric | Typical Range |
|--------|---------------|
| Latency | 20-50ms |
| Download Speed | 50-200 Mbps |
| Upload Speed | 5-25 Mbps |
| Packet Loss | <1% |
| Power Consumption | 50-150W (varies with conditions) |
| Uptime | Days to weeks between reboots |

### Telemetry Update Frequency

- **Status Data:** Updated every 1-3 seconds
- **History Data:** 1-second samples, typically 900-sample buffer (15 minutes)
- **Obstruction Map:** Updated every 15-30 seconds

## Troubleshooting

### gRPC Connection Issues

**Problem:** Cannot connect to Starlink gRPC endpoint

**Solutions:**
1. Verify network connectivity: `ping 192.168.100.1`
2. Check gRPC port accessibility: `telnet 192.168.100.1 9200`
3. Ensure Starlink firmware supports gRPC (most Gen2/Gen3 do)
4. Try grpcurl test: `grpcurl -plaintext 192.168.100.1:9200 list`

### Missing Telemetry Data

**Problem:** Resources show default/zero values

**Solutions:**
1. Implement `updateTelemetry()` method with gRPC calls
2. Call `executeRefreshTelemetry()` to force update
3. Check for gRPC errors in logs
4. Verify Starlink dish is powered on and initialized

### High Obstruction Readings

**Problem:** Resource 30 shows high obstruction fraction

**Solutions:**
1. Check Resource 35 (wedges) to identify obstruction direction
2. Execute Resource 104 (Get Obstruction Map) for detailed view
3. Physically inspect dish surroundings for trees, buildings
4. Use Starlink app's AR viewer for precise obstruction identification

## API Reference

### Main Class

```cpp
class StarlinkTerminal : public Instance {
public:
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);

    // Resource IDs enum (see header file)
    enum ID: ID_T { ... };

    // Connection state enum
    enum ConnState: INT_T {
        STATE_UNKNOWN, STATE_CONNECTED, STATE_BOOTING,
        STATE_SEARCHING, STATE_STOWED, STATE_THERMAL_SHUTDOWN,
        STATE_NO_SATS, STATE_OBSTRUCTED, STATE_NO_DOWNLINK,
        STATE_NO_PINGS
    };
};
```

## License & Support

This object is part of the Friendly-LwM2M-Client project.

**Support Resources:**
- Project Repository: [Friendly-LwM2M-Client](https://github.com/Friendly-Technologies/Friendly-LwM2M-Client)
- Starlink Community: [starlink-grpc-tools](https://github.com/sparky8512/starlink-grpc-tools)
- LwM2M Registry: [OMA LwM2M Registry](https://technical.openmobilealliance.org/OMNA/LwM2M/LwM2MRegistry.html)

## Version History

- **v1.0** (2025-11) - Initial release
  - Full Starlink Gen3 telemetry support
  - 120 resources across 10 functional groups
  - Support for obstruction monitoring, alignment, alerts, GPS
  - Speed test and remote control capabilities
  - Historical statistics and performance analysis
