# Starlink Terminal Object (ID 10512)

Complete documentation for the Walt Technologies Starlink Terminal LwM2M object.

---

## Overview

The Starlink Terminal object provides comprehensive management and monitoring of Starlink Gen3 satellite internet terminals via the gRPC protocol. This custom LwM2M object enables remote access to telemetry, configuration, and control of Starlink dishes, including obstruction monitoring, alignment tracking, speed testing, and performance metrics.

**Object ID:** 10512 (Walt Technologies Custom Object Range)
**URN:** `urn:oma:lwm2m:x:10512:1.0`
**Version:** 1.0
**Instance Type:** Single Instance
**Mandatory:** Optional

---

## Key Features

- **70+ Resources** organized into 10 functional groups
- **Real-time Telemetry**: Network throughput, latency, SNR, packet loss
- **Obstruction Monitoring**: 12-wedge sky view analysis and obstruction tracking
- **Dish Alignment**: Azimuth/elevation tracking and stow control
- **16 Alert Conditions**: Thermal, motors, obstructions, water detection, and more
- **Integrated GPS**: Full GPS capabilities with satellite tracking
- **Power Management**: Current, mean, min/max power, and energy consumption
- **Control Actions**: Reboot, speed test, factory reset, obstruction map retrieval
- **Historical Statistics**: Performance analytics and latency deciles
- **gRPC Interface**: Direct communication with Starlink dish at 192.168.100.1:9200

---

## Resource Overview

### Device Information Resources (0-9)

| Resource ID | Name | Type | Operations | Description |
|------------|------|------|------------|-------------|
| 0 | Device ID | String | R | Starlink terminal serial number |
| 1 | Hardware Version | String | R | Terminal hardware version (e.g., "rev3_proto2") |
| 2 | Software Version | String | R | Currently installed firmware version |
| 3 | Connection State | String | R | Current state: CONNECTED, SEARCHING, BOOTING, etc. |
| 4 | Uptime | Integer | R | Seconds since last reboot |

**Connection States:**
- `CONNECTED` - Terminal connected to satellites
- `SEARCHING` - Searching for satellites
- `BOOTING` - Terminal starting up
- `STOWED` - Dish in stowed position
- `THERMAL_SHUTDOWN` - Thermal shutdown active
- `NO_SATS` - No satellites available
- `OBSTRUCTED` - View obstructed
- `NO_DOWNLINK` - No downlink from satellites
- `NO_PINGS` - No ping responses
- `UNKNOWN` - State unknown

### Network Performance Resources (10-29)

| Resource ID | Name | Type | Operations | Units | Description |
|------------|------|------|------------|-------|-------------|
| 10 | Downlink Throughput | Float | R | bps | Current download speed |
| 11 | Uplink Throughput | Float | R | bps | Current upload speed |
| 12 | POP Ping Latency | Float | R | ms | Round-trip latency to Point of Presence |
| 13 | POP Ping Drop Rate | Float | R | - | Packet loss fraction (0.0-1.0) |
| 14 | Seconds To First Slot | Float | R | s | Seconds until next satellite available |
| 15 | SNR | Float | R | dB | Signal-to-noise ratio |
| 16 | Is SNR Above Noise Floor | Boolean | R | - | True if SNR > 3dB threshold |
| 17 | Download Usage | Integer | R | bytes | Total bytes downloaded |
| 18 | Upload Usage | Integer | R | bytes | Total bytes uploaded |

### Obstruction Monitoring Resources (30-39)

| Resource ID | Name | Type | Operations | Description |
|------------|------|------|------------|-------------|
| 30 | Fraction Obstructed | Float | R | Obstruction area fraction (0.0 = clear, 1.0 = fully blocked) |
| 31 | Currently Obstructed | Boolean | R | Current obstruction status |
| 32 | Obstruction Duration | Float | R | Average prolonged obstruction duration (seconds) |
| 33 | Obstruction Interval | Float | R | Average time between obstructions (seconds) |
| 34 | Obstruction Valid S | Float | R | Data completeness measure (seconds) |
| 35 | Wedges Fraction Obstructed | String | R | JSON array of 12 wedge obstructions (30° each) |

**Wedges Array Format:**
```json
[0.0, 0.1, 0.0, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
```
Each value represents obstruction fraction for a 30-degree sky wedge (0-11).

### Dish Alignment Resources (40-49)

| Resource ID | Name | Type | Operations | Range | Description |
|------------|------|------|------------|-------|-------------|
| 40 | Direction Azimuth | Float | R | 0-360° | Dish pointing azimuth (0=North, 90=East, 180=South, 270=West) |
| 41 | Direction Elevation | Float | R | 0-90° | Dish pointing elevation (0=horizontal, 90=vertical) |
| 42 | Dish Stowed | Boolean | RW | - | Get/set stow state (true=stowed for transport) |
| 43 | Dish Alignment Status | String | R | - | Human-readable alignment status message |

### Alert System Resources (50-79)

| Resource ID | Name | Type | Description |
|------------|------|------|-------------|
| 50 | Alerts Bitmap | Integer | Combined alert flags as bit field |
| 51 | Alert Motors Stuck | Boolean | Motors stuck and cannot move |
| 52 | Alert Thermal Shutdown | Boolean | Thermal shutdown triggered |
| 53 | Alert Thermal Throttle | Boolean | Performance throttled due to heat |
| 54 | Alert Unexpected Location | Boolean | Dish detected at unexpected location |
| 55 | Alert Mast Not Vertical | Boolean | Mounting mast not near vertical |
| 56 | Alert Slow Ethernet | Boolean | Ethernet connection speed below expected |
| 57 | Alert Roaming | Boolean | Terminal roaming outside home service area |
| 58 | Alert Install Pending | Boolean | Installation pending completion |
| 59 | Alert Is Heating | Boolean | Dish heater actively running (cold weather) |
| 60 | Alert Power Supply Throttle | Boolean | Power supply thermal throttling active |
| 61 | Alert Is Power Save Idle | Boolean | Terminal in power saving idle mode |
| 62 | Alert Low Motor Current | Boolean | Motor current below expected levels |
| 63 | Alert Lower Signal | Boolean | Signal strength lower than predicted |
| 64 | Alert Obstruction Map Reset | Boolean | Obstruction map has been reset |
| 65 | Alert Dish Water Detected | Boolean | Water detected in dish enclosure |
| 66 | Alert Router Water Detected | Boolean | Water detected in router enclosure |

### GPS Location Resources (80-89)

| Resource ID | Name | Type | Operations | Range | Description |
|------------|------|------|------------|-------|-------------|
| 80 | GPS Ready | Boolean | R | - | GPS position fix available and valid |
| 81 | GPS Enabled | Boolean | R | - | GPS functionality enabled on terminal |
| 82 | GPS Satellites | Integer | R | - | Number of GPS satellites in use |
| 83 | Latitude | Float | R | -90 to 90° | Current latitude (WGS84) |
| 84 | Longitude | Float | R | -180 to 180° | Current longitude (WGS84) |
| 85 | Altitude | Float | R | meters | Current altitude above sea level |

### Power Management Resources (90-99)

| Resource ID | Name | Type | Operations | Units | Description |
|------------|------|------|------------|-------|-------------|
| 90 | Latest Power | Float | R | W | Most recent instantaneous power consumption |
| 91 | Mean Power | Float | R | W | Average power consumption over monitoring period |
| 92 | Min Power | Float | R | W | Minimum power observed during period |
| 93 | Max Power | Float | R | W | Maximum power observed during period |
| 94 | Total Energy | Float | R | kWh | Cumulative energy consumed since counter reset |

### Control Action Resources (100-107)

| Resource ID | Name | Type | Operations | Description |
|------------|------|------|------------|-------------|
| 100 | Reboot Terminal | - | E | Execute to reboot the Starlink terminal |
| 101 | Start Speedtest | - | E | Initiate a speed test |
| 102 | Speedtest Status | String | R | Speed test status and results (JSON) |
| 103 | Factory Reset | - | E | Perform factory reset (disabled by default for safety) |
| 104 | Get Obstruction Map | - | E | Retrieve current obstruction map |
| 105 | Obstruction Map Data | Opaque | R | Binary obstruction map data |
| 106 | Refresh Telemetry | - | E | Force immediate telemetry refresh |
| 107 | gRPC Endpoint | String | RW | - | gRPC endpoint address (default: "192.168.100.1:9200") |

### Historical Data Resources (110-119)

| Resource ID | Name | Type | Operations | Description |
|------------|------|------|------------|-------------|
| 110 | History Samples | Integer | R | Number of historical samples available |
| 111 | History Interval | Integer | R | Time interval between samples (seconds) |
| 112 | Mean Ping Latency | Float | R | Mean latency across all historical samples |
| 113 | Mean Full Ping Latency | Float | R | Mean latency (0% loss samples only) |
| 114 | Stdev Full Ping Latency | Float | R | Latency standard deviation |
| 115 | Total Ping Drop | Float | R | Total fraction of samples with packet loss |
| 116 | Count Full Ping Drop | Integer | R | Samples with 100% packet loss |
| 117 | Count Obstructed | Integer | R | Obstruction-marked samples |
| 118 | Latency Deciles | String | R | JSON array of latency deciles (10th-90th percentile) |
| 119 | Load Bucket Stats | String | R | JSON formatted network load distribution statistics |

---

## gRPC Integration

### Prerequisites

**gRPC Dependencies:**
- gRPC C++ library (`libgrpc++`)
- Protocol Buffers (`protobuf`)
- Starlink protobuf definitions

**Network Requirements:**
- Starlink dish accessible at 192.168.100.1:9200 (default)
- TCP port 9200 open for gRPC communication
- Network routing configured correctly

### gRPC Implementation Example

```cpp
#include <grpcpp/grpcpp.h>
#include "spacex/api/device/device.grpc.pb.h"

bool StarlinkTerminal::updateTelemetry() {
    // Create gRPC channel
    auto channel = grpc::CreateChannel("192.168.100.1:9200",
                                       grpc::InsecureChannelCredentials());
    auto stub = SpaceX::API::Device::Device::NewStub(channel);

    // Prepare request
    grpc::ClientContext context;
    SpaceX::API::Device::Request request;
    request.mutable_get_status();
    SpaceX::API::Device::Response response;

    // Call gRPC method
    grpc::Status status = stub->Handle(&context, request, &response);

    if (status.ok() && response.has_dish_get_status()) {
        auto dishStatus = response.dish_get_status();

        // Update resources
        resource(CONNECTION_STATE_3)->set<STRING_T>(
            parseState(dishStatus.state()));
        resource(DOWNLINK_THROUGHPUT_10)->set<FLOAT_T>(
            dishStatus.downlink_throughput_bps());
        resource(UPLINK_THROUGHPUT_11)->set<FLOAT_T>(
            dishStatus.uplink_throughput_bps());
        resource(POP_PING_LATENCY_12)->set<FLOAT_T>(
            dishStatus.pop_ping_latency_ms());

        // Update obstruction data
        resource(FRACTION_OBSTRUCTED_30)->set<FLOAT_T>(
            dishStatus.obstruction_stats().fraction_obstructed());
        resource(CURRENTLY_OBSTRUCTED_31)->set<BOOL_T>(
            dishStatus.obstruction_stats().currently_obstructed());

        // Update GPS (if available)
        if (dishStatus.has_gps_stats()) {
            resource(GPS_READY_80)->set<BOOL_T>(
                dishStatus.gps_stats().gps_valid());
            resource(LATITUDE_83)->set<FLOAT_T>(
                dishStatus.gps_stats().latitude());
            resource(LONGITUDE_84)->set<FLOAT_T>(
                dishStatus.gps_stats().longitude());
            resource(ALTITUDE_85)->set<FLOAT_T>(
                dishStatus.gps_stats().altitude());
        }

        // Notify observers
        notifyResChanged(CONNECTION_STATE_3);
        notifyResChanged(DOWNLINK_THROUGHPUT_10);

        return true;
    }

    return false;
}
```

### Testing gRPC Connectivity

```bash
# Install grpcurl for testing
opkg install grpcurl  # On OpenWRT

# List available services
grpcurl -plaintext 192.168.100.1:9200 list

# Expected output:
# SpaceX.API.Device.Device

# Get dish status
grpcurl -plaintext -d '{"get_status":{}}' \
    192.168.100.1:9200 \
    SpaceX.API.Device.Device/Handle

# This returns JSON with all telemetry data
```

---

## Configuration

### Enable Starlink Terminal Object

**wpp/configs/wpp_config.cmake:**
```cmake
# Enable Starlink Terminal object
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10512_STARLINK_TERMINAL)
```

### Optional Resource Configuration

**StarlinkTerminalConfig.h:**
```cpp
// Disable factory reset for safety
#define RES_10512_103 0  // Factory Reset

// Enable all other optional resources
#define RES_10512_14 1   // Seconds To First Slot
#define RES_10512_15 1   // SNR
#define RES_10512_104 1  // Get Obstruction Map
#define RES_10512_118 1  // Latency Deciles
```

### Initialize in Application

**examples/objects.cpp:**
```cpp
#ifdef OBJ_O_10512_STARLINK_TERMINAL
void starlinkTerminalInit(WppClient &client) {
    client.registry().registerObj(StarlinkTerminal::object(client));
    Instance* starlink = StarlinkTerminal::createInst(client);

    // Optionally configure custom gRPC endpoint
    if (starlink) {
        starlink->set<STRING_T>(StarlinkTerminal::GRPC_ENDPOINT_107,
                                "192.168.100.1:9200");
    }

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(StarlinkTerminal::object(client),
                               Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    #endif
}
#endif
```

---

## Usage Examples

### Monitor Connection Status

```cpp
Instance* starlink = StarlinkTerminal::instance(client);
if (starlink) {
    // Get connection state
    std::string state = starlink->get<STRING_T>(
        StarlinkTerminal::CONNECTION_STATE_3);

    if (state == "CONNECTED") {
        // Get network performance
        float downlink = starlink->get<FLOAT_T>(
            StarlinkTerminal::DOWNLINK_THROUGHPUT_10);
        float uplink = starlink->get<FLOAT_T>(
            StarlinkTerminal::UPLINK_THROUGHPUT_11);
        float latency = starlink->get<FLOAT_T>(
            StarlinkTerminal::POP_PING_LATENCY_12);

        std::cout << "Downlink: " << (downlink / 1e6) << " Mbps\n";
        std::cout << "Uplink: " << (uplink / 1e6) << " Mbps\n";
        std::cout << "Latency: " << latency << " ms\n";
    }
}
```

### Check Obstruction Status

```cpp
Instance* starlink = StarlinkTerminal::instance(client);
if (starlink) {
    float obstructed = starlink->get<FLOAT_T>(
        StarlinkTerminal::FRACTION_OBSTRUCTED_30);
    bool currently_obstructed = starlink->get<BOOL_T>(
        StarlinkTerminal::CURRENTLY_OBSTRUCTED_31);

    if (obstructed > 0.1) {
        std::cout << "Warning: " << (obstructed * 100.0)
                  << "% sky view obstructed\n";
    }

    if (currently_obstructed) {
        std::cout << "Alert: Dish currently obstructed!\n";
    }
}
```

### Get GPS Location

```cpp
Instance* starlink = StarlinkTerminal::instance(client);
if (starlink) {
    bool gps_ready = starlink->get<BOOL_T>(
        StarlinkTerminal::GPS_READY_80);

    if (gps_ready) {
        double lat = starlink->get<FLOAT_T>(
            StarlinkTerminal::LATITUDE_83);
        double lon = starlink->get<FLOAT_T>(
            StarlinkTerminal::LONGITUDE_84);
        double alt = starlink->get<FLOAT_T>(
            StarlinkTerminal::ALTITUDE_85);
        int sats = starlink->get<INT_T>(
            StarlinkTerminal::GPS_SATELLITES_82);

        std::cout << "Location: " << lat << ", " << lon
                  << " at " << alt << "m\n";
        std::cout << "Satellites: " << sats << "\n";
    }
}
```

### Execute Speed Test

```cpp
Instance* starlink = StarlinkTerminal::instance(client);
if (starlink) {
    // Start speed test
    OPAQUE_T data;
    EXECUTE_RESULT result = starlink->execute(
        StarlinkTerminal::START_SPEEDTEST_101, data);

    if (result == EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS) {
        std::cout << "Speed test started\n";

        // Poll for results
        std::string status = starlink->get<STRING_T>(
            StarlinkTerminal::SPEEDTEST_STATUS_102);
        // Parse JSON status for results
    }
}
```

### Monitor Alerts

```cpp
Instance* starlink = StarlinkTerminal::instance(client);
if (starlink) {
    // Check individual alerts
    if (starlink->get<BOOL_T>(StarlinkTerminal::ALERT_THERMAL_SHUTDOWN_52)) {
        std::cout << "CRITICAL: Thermal shutdown!\n";
    }

    if (starlink->get<BOOL_T>(StarlinkTerminal::ALERT_MOTORS_STUCK_51)) {
        std::cout << "ERROR: Motors stuck!\n";
    }

    if (starlink->get<BOOL_T>(StarlinkTerminal::ALERT_IS_HEATING_59)) {
        std::cout << "INFO: Dish heater active\n";
    }

    if (starlink->get<BOOL_T>(StarlinkTerminal::ALERT_DISH_WATER_DETECTED_65)) {
        std::cout << "WARNING: Water detected in dish!\n";
    }
}
```

---

## Integration with Location Object

The Starlink Terminal object GPS resources (80-89) can be used as the primary GPS source for the Location object (ID 6).

### Automatic Integration

When both objects are enabled, the Location object automatically prioritizes Starlink GPS:

```cpp
// GPS source priority (automatic):
// 1. Starlink Terminal GPS (Resources 83-85)
// 2. gpsd daemon
// 3. UCI manual configuration
```

### Benefits

- **Single GPS Source**: No need for external GPS hardware
- **High Accuracy**: ~10 meter accuracy from Starlink constellation
- **Satellite Count**: Real-time satellite tracking information
- **Validity Flags**: GPS_READY and GPS_ENABLED status
- **Automatic Fallback**: If Starlink unavailable, falls back to other sources

**See Also:** [Location Object Documentation](LOCATION_OBJECT.md)

---

## Performance Considerations

### Memory Usage

- **Object Instance**: ~2KB
- **Per Telemetry Update**: ~500 bytes (temporary)
- **gRPC Overhead**: ~10KB (channels, stubs)

### CPU Usage

- **Telemetry Update**: ~0.5% CPU (10-second interval)
- **gRPC Call**: ~10-50 milliseconds
- **Resource Updates**: ~100 microseconds

### Network Usage

- **Per Observation**: ~200 bytes (TLV format, 70+ resources)
- **Update Frequency**: Configurable (default: 10 seconds)
- **gRPC Traffic**: ~1-5 KB per telemetry request
- **Bandwidth**: ~5-10 bytes/second (with observations)

### Optimization Tips

1. **Adjust Update Interval**
   - Longer intervals = Less CPU/network usage
   - Shorter intervals = More real-time monitoring

2. **Selective Resource Observation**
   - Only observe critical resources
   - Cancel observations when not needed

3. **Disable Optional Resources**
   - Disable historical statistics if not needed
   - Disable advanced alerts to reduce memory

---

## Troubleshooting

### Starlink Dish Not Reachable

```bash
# Check network connectivity
ping 192.168.100.1

# Check routing
ip route | grep 192.168.100

# Add route if needed
ip route add 192.168.100.0/24 via <gateway_ip>
```

### gRPC Connection Failures

```bash
# Test gRPC endpoint
grpcurl -plaintext 192.168.100.1:9200 list

# Check firewall rules
iptables -L -n | grep 9200

# Allow gRPC port
iptables -A OUTPUT -p tcp --dport 9200 -j ACCEPT
```

### No GPS Data

```bash
# Check GPS status via gRPC
grpcurl -plaintext -d '{"get_status":{}}' \
    192.168.100.1:9200 \
    SpaceX.API.Device.Device/Handle | grep -i gps

# Verify GPS is enabled on terminal
# Resource 81: GPS_ENABLED should be true
# Resource 80: GPS_READY should become true when fix acquired
```

### Connection State Stuck

```bash
# Common states and causes:
# SEARCHING - Normal during startup or after obstruction
# OBSTRUCTED - Check Resource 30 (FRACTION_OBSTRUCTED)
# NO_SATS - Check for nearby obstructions
# STOWED - Dish is stowed, write false to Resource 42 to unstow

# Force telemetry refresh via LwM2M
# Execute Resource 106: REFRESH_TELEMETRY
```

---

## API Reference

### StarlinkTerminal Class

```cpp
class StarlinkTerminal : public Instance {
public:
    // Resource ID enumeration
    enum ID: ID_T {
        DEVICE_ID_0 = 0,
        // ... (see Resource Overview above)
    };

    // Static factory methods
    static Object& object(WppClient& ctx);
    static Instance* createInst(WppClient& ctx, OBJ_INST_ID_T instId = 0);

    // Resource management
    void resourcesCreate() override;
    void resourcesInit() override;

private:
    // Telemetry update
    bool updateTelemetry();

    // Execute callbacks
    EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId,
                                  const OPAQUE_T& data);
    EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId,
                                     const OPAQUE_T& data);
    // ... more execute methods
};
```

---

## References

- [OMA LwM2M Specification](https://www.openmobilealliance.org/release/LightweightM2M/)
- [Starlink gRPC API](https://github.com/starlink-community/starlink-grpc-api)
- [Starlink Community Tools](https://github.com/sparky8512/starlink-grpc-tools)
- [gRPC C++ Documentation](https://grpc.io/docs/languages/cpp/)
- [Protocol Buffers](https://developers.google.com/protocol-buffers)
- [OpenWRT Integration Guide](OPENWRT_INTEGRATION.md)
- [Location Object Documentation](LOCATION_OBJECT.md)

---

**Next Steps:**
- Review [Quick Start Guide](QUICKSTART.md) for setup instructions
- See [Implementation Guide](IMPLEMENTATION_GUIDE.md) for development examples
- Check [Troubleshooting Guide](TROUBLESHOOTING.md) for common issues
