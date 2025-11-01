# Location Object (ID 6) Documentation

Complete documentation for the OMA LwM2M Location object implementation with OpenWRT and Starlink integration.

---

## Overview

The Location object provides GPS location telemetry that can be queried by an LwM2M Server. This implementation supports multiple GPS sources with automatic fallback and real-time updates.

**OMA Specification:** `urn:oma:lwm2m:oma:6:1.0`
**Object ID:** 6
**Instance Type:** Single
**Mandatory:** Optional

---

## Resource Definitions

| Resource ID | Name | Type | Units | Operations | Mandatory | Description |
|------------|------|------|-------|------------|-----------|-------------|
| 0 | Latitude | Float | lat (degrees) | R | Yes | Decimal notation of latitude (WGS84), e.g. -43.5723 |
| 1 | Longitude | Float | lon (degrees) | R | Yes | Decimal notation of longitude (WGS84), e.g. 153.21760 |
| 2 | Altitude | Float | m (meters) | R | No | Altitude in meters above sea level |
| 3 | Radius | Float | m (meters) | R | No | Circular area radius indicating coordinate uncertainty |
| 4 | Velocity | Opaque | - | R | No | Velocity as defined in 3GPP-TS_23.032 |
| 5 | Timestamp | Time | - | R | Yes | Unix timestamp of when the location was measured |
| 6 | Speed | Float | m/s | R | No | Scalar component of velocity (time rate of change in position) |

### Resource Details

#### Latitude (0)
- **Range:** -90.0 to +90.0 degrees
- **Format:** Decimal degrees (WGS84)
- **Positive:** North of equator
- **Negative:** South of equator
- **Example:** 40.7128 (New York City)

#### Longitude (1)
- **Range:** -180.0 to +180.0 degrees
- **Format:** Decimal degrees (WGS84)
- **Positive:** East of Prime Meridian
- **Negative:** West of Prime Meridian
- **Example:** -74.0060 (New York City)

#### Altitude (2)
- **Reference:** Mean sea level (MSL)
- **Positive:** Above sea level
- **Negative:** Below sea level (rare)
- **Typical Range:** -500m to 10,000m

#### Radius (3)
- **Purpose:** Indicates horizontal position accuracy
- **Interpretation:**
  - Smaller radius = More accurate position
  - Larger radius = Less certain position
- **Typical Values:**
  - Starlink GPS: ~10 meters
  - Consumer GPS: ~15 meters
  - Manual config: ~100 meters

#### Timestamp (5)
- **Format:** Unix epoch time (seconds since Jan 1, 1970 UTC)
- **Purpose:** Indicates when the GPS fix was obtained
- **Important:** Should be updated with each location update

#### Speed (6)
- **Calculation:** Haversine formula between consecutive positions
- **Update:** Calculated automatically on position changes
- **Zero Value:** Indicates stationary or first measurement

---

## Implementation Architecture

### File Structure

```
wpp/registry/objects/o_6_location/
├── Location.h                    # Object header
├── Location.cpp                  # Object implementation
├── LocationConfig.h              # Build configuration
├── LocationInfo.h                # Object metadata
├── OpenWrtLocationInfo.h         # OpenWRT helper class
└── CMakeLists.txt               # Build script
```

### Class Diagram

```
┌─────────────────┐
│   Instance      │
│  (Base Class)   │
└────────┬────────┘
         │ inherits
         │
┌────────▼────────┐
│    Location     │
│                 │
│  + resourcesCreate()
│  + resourcesInit()
│  + updateLocation()
│  - _locationUpdateTaskId
└─────────────────┘
         │ uses
         │
┌────────▼─────────────────┐
│  OpenWrtLocationInfo     │
│  (Helper Class)          │
│                          │
│  + getLatitude()         │
│  + getLongitude()        │
│  + getAltitude()         │
│  + getRadius()           │
│  + getTimestamp()        │
│  + getSpeed()            │
│  + getAllLocation()      │
└──────────────────────────┘
```

---

## GPS Data Sources

### Source Priority

The Location object queries GPS sources in the following order:

1. **Starlink Terminal GPS** (Highest Priority)
2. **gpsd Daemon**
3. **Manual UCI Configuration** (Lowest Priority)

### Source 1: Starlink Terminal GPS

**Requirements:**
- Starlink dish accessible at 192.168.100.1:9200
- Object OBJ_O_34600_STARLINK_TERMINAL enabled
- grpcurl installed (optional, for gRPC calls)

**Data Retrieved:**
```cpp
// From Starlink gRPC API
latitude   = dish_status.gps_stats().latitude()
longitude  = dish_status.gps_stats().longitude()
altitude   = dish_status.gps_stats().altitude_meters()
gps_valid  = dish_status.gps_stats().gps_valid()
gps_sats   = dish_status.gps_stats().gps_sats()
```

**Accuracy:** ~10 meters (when gps_valid == true)

**Example gRPC Call:**
```bash
grpcurl -plaintext -d '{"get_status":{}}' \
    192.168.100.1:9200 \
    SpaceX.API.Device.Device/Handle
```

### Source 2: gpsd Daemon

**Requirements:**
- gpsd installed and running
- GPS receiver connected (e.g., /dev/ttyUSB0)

**Installation (OpenWRT):**
```bash
opkg update
opkg install gpsd gpsd-clients

# Configure
uci set gpsd.core.enabled='1'
uci set gpsd.core.device='/dev/ttyUSB0'
uci set gpsd.core.port='2947'
uci commit gpsd

# Start
/etc/init.d/gpsd start
```

**Data Retrieved:**
```bash
# Via gpspipe
gpspipe -w -n 10 | grep -m 1 TPV

# Example JSON response:
{
  "class":"TPV",
  "device":"/dev/ttyUSB0",
  "mode":3,
  "time":"2025-11-01T12:00:00.000Z",
  "lat":40.7128,
  "lon":-74.0060,
  "alt":10.5,
  "speed":2.5
}
```

**Accuracy:** ~15 meters (mode 3 = 3D fix)

### Source 3: Manual UCI Configuration

**Use Cases:**
- Fixed installations with known coordinates
- Indoor deployments without GPS
- Testing and development

**Configuration:**
```bash
# Create UCI config
uci set lwm2m.location=location
uci set lwm2m.location.latitude='40.7128'
uci set lwm2m.location.longitude='-74.0060'
uci set lwm2m.location.altitude='10'
uci commit lwm2m
```

**Data Retrieved:**
```bash
uci get lwm2m.location.latitude
uci get lwm2m.location.longitude
uci get lwm2m.location.altitude
```

**Accuracy:** ~100 meters (assumed low accuracy)

---

## Code Examples

### Basic Usage

```cpp
#include "o_6_location/Location.h"

// Create instance
Location *loc = Location::createInst(client);

// Read latitude
float lat = loc->get<FLOAT_T>(Location::LATITUDE_0);

// Read longitude
float lon = loc->get<FLOAT_T>(Location::LONGITUDE_1);

// Read altitude
#if RES_6_2
float alt = loc->get<FLOAT_T>(Location::ALTITUDE_2);
#endif

// Read timestamp
time_t timestamp = loc->get<TIME_T>(Location::TIMESTAMP_5);

printf("Location: %f, %f (altitude: %fm) at %ld\n",
       lat, lon, alt, timestamp);
```

### Observing Location Changes

```cpp
// The server can observe location resources
// Updates are sent automatically every 60 seconds

// LwM2M Server OBSERVE request:
// COAP GET /6/0/0 with Observe option

// Client will notify server when:
// - Location changes significantly
// - Periodic update interval expires (60s)
// - notifyResChanged() is called
```

### Manual Location Update

```cpp
// Trigger immediate location update
Location *loc = Location::instance(client);
if (loc) {
    loc->updateLocation();
}
```

### Custom Update Interval

Modify in `Location.cpp`:

```cpp
void Location::resourcesInit() {
    // Change from 60 seconds to 30 seconds
    _locationUpdateTaskId = WppTaskQueue::addTask(30, [this](WppClient &client, void *ctx) {
        updateLocation();
        return false;
    });
}
```

---

## Speed Calculation

The Location object calculates speed using the Haversine formula to compute distance between consecutive GPS positions.

### Haversine Formula Implementation

```cpp
static double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000.0; // Earth radius in meters

    // Convert to radians
    double lat1Rad = lat1 * M_PI / 180.0;
    double lat2Rad = lat2 * M_PI / 180.0;
    double deltaLat = (lat2 - lat1) * M_PI / 180.0;
    double deltaLon = (lon2 - lon1) * M_PI / 180.0;

    // Haversine formula
    double a = sin(deltaLat / 2.0) * sin(deltaLat / 2.0) +
               cos(lat1Rad) * cos(lat2Rad) *
               sin(deltaLon / 2.0) * sin(deltaLon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return R * c; // Distance in meters
}
```

### Speed Calculation

```cpp
double getSpeed() {
    double lat = getLatitude();
    double lon = getLongitude();
    time_t now = getTimestamp();

    if (_prevTimestamp > 0 && (now > _prevTimestamp)) {
        // Calculate distance traveled
        double distance = calculateDistance(_prevLatitude, _prevLongitude, lat, lon);

        // Calculate time elapsed
        double timeDiff = static_cast<double>(now - _prevTimestamp);

        // Update previous position
        _prevLatitude = lat;
        _prevLongitude = lon;
        _prevTimestamp = now;

        // Return speed in m/s
        return distance / timeDiff;
    }

    // First measurement
    _prevLatitude = lat;
    _prevLongitude = lon;
    _prevTimestamp = now;

    return 0.0;
}
```

### Speed Examples

| Scenario | Distance | Time | Speed |
|----------|----------|------|-------|
| Stationary | 0 m | 60 s | 0.0 m/s |
| Walking | 75 m | 60 s | 1.25 m/s (4.5 km/h) |
| Driving | 1667 m | 60 s | 27.8 m/s (100 km/h) |
| Fast vessel | 514 m | 60 s | 8.6 m/s (31 km/h) |

---

## Configuration

### Enable Location Object

**File:** `wpp/configs/wpp_config.cmake`

```cmake
# Enable Location object (Object ID: 6)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_6_LOCATION)
```

### Configure Resources

**File:** `wpp/registry/objects/o_6_location/LocationConfig.h`

```cpp
// Enable/disable optional resources
#define RES_6_2 1  // Altitude
#define RES_6_3 1  // Radius (uncertainty)
#define RES_6_4 0  // Velocity (3GPP format) - not implemented
#define RES_6_6 1  // Speed
```

### OpenWRT Build Flag

```bash
cmake -DOPENWRT_BUILD=ON ..
```

When `OPENWRT_BUILD` is defined:
- Real GPS data is retrieved from available sources
- Location updates every 60 seconds
- Speed is calculated from position deltas

When `OPENWRT_BUILD` is NOT defined:
- Default values (0.0, 0.0) are used
- Manual setting of resources is required
- Suitable for testing without hardware

---

## Testing

### Test on Development Machine

```bash
# Build without OpenWRT support
mkdir build && cd build
cmake -DOPENWRT_BUILD=OFF ..
make

# Run
./examples/lwm2m_client

# Expected output:
# Location initialized from OpenWRT: lat=0.000000, lon=0.000000, alt=0.000000
# (or)
# No location source available, using defaults
```

### Test with Leshan Server

```bash
# Start Leshan
docker run -d -p 8080:8080 -p 5683:5683/udp eclipselwm2m/leshan

# Configure client to connect
# Edit examples/objects.cpp to set server URI to localhost:5683

# Run client
./lwm2m_client

# Open browser
open http://localhost:8080

# Navigate to your client -> Object 6 (Location)
# Observe resources 0 (Latitude) and 1 (Longitude)
```

### Test on OpenWRT with Starlink

```bash
# Prerequisites:
# - OpenWRT One device
# - Starlink dish at 192.168.100.1

# Cross-compile
mkdir build-openwrt && cd build-openwrt
cmake -DOPENWRT_BUILD=ON -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..
make

# Deploy
scp lwm2m_client root@192.168.1.1:/usr/bin/

# SSH and run
ssh root@192.168.1.1
/usr/bin/lwm2m_client

# Check logs for:
# Location initialized from OpenWRT: lat=XX.XXXX, lon=YY.YYYY, alt=ZZ.Z
# (coordinates from Starlink GPS)
```

---

## Troubleshooting

### No Location Data

**Symptom:** Location shows 0.0, 0.0

**Causes:**
1. No GPS source available
2. Starlink dish not reachable
3. gpsd not running
4. No manual configuration

**Solution:**
```bash
# Check Starlink connectivity
ping -c 1 192.168.100.1

# Check gpsd
ps | grep gpsd

# Check UCI config
uci show lwm2m.location

# Enable debug logging
# In wpp_config.cmake: set WPP_LOGS_LEVEL to 0
```

### Incorrect Location

**Symptom:** Location is wrong

**Causes:**
1. GPS not locked (insufficient satellites)
2. Indoor installation
3. Obstruction blocking GPS signal

**Solution:**
```bash
# Check Starlink GPS status
# Object 34600, Resource 82 (GPS_SATELLITES)
# Should be > 4 for good fix

# Check GPS valid flag
# Object 34600, Resource 80 (GPS_READY)
# Should be true
```

### Speed Always Zero

**Symptom:** Speed resource always shows 0.0

**Causes:**
1. Device is stationary
2. Update interval too short
3. GPS accuracy insufficient to detect movement

**Solution:**
- Increase update interval for better delta
- Move device to test speed calculation
- Check GPS accuracy (radius < 20m recommended)

---

## Integration with Other Objects

### Starlink Terminal (ID 34600)

The Location object can read GPS data directly from the Starlink Terminal object resources:

```cpp
// Starlink resources with GPS data:
// - GPS_READY_80 (Resource 80)
// - GPS_ENABLED_81 (Resource 81)
// - GPS_SATELLITES_82 (Resource 82)
// - LATITUDE_83 (Resource 83)
// - LONGITUDE_84 (Resource 84)
// - ALTITUDE_85 (Resource 85)
```

**Benefits:**
- Single GPS source for multiple objects
- Consistent location reporting
- Satellite tracking information
- GPS validity flags

### Device (ID 3)

Location can be combined with Device object for complete device telemetry:

```cpp
// Example: Send device info with location
DataLink deviceAndLocation[] = {
    {{OBJ_ID::DEVICE, 0}, {Device::SERIAL_NUMBER_2}},
    {{OBJ_ID::LOCATION, 0}, {Location::LATITUDE_0}},
    {{OBJ_ID::LOCATION, 0}, {Location::LONGITUDE_1}},
};
client.send(deviceAndLocation, 3);
```

---

## Performance Considerations

### Memory Usage

- **Object Instance:** ~500 bytes
- **Per Update:** ~100 bytes (temporary)
- **Helper Class:** 24 bytes (static members)

### CPU Usage

- **Update Task:** ~0.1% CPU (60s interval)
- **Haversine Calculation:** ~10 microseconds
- **GPS Query:** ~1-5 milliseconds

### Network Usage

- **Per Observation:** ~100 bytes (TLV format)
- **Update Frequency:** Configurable (default: 60s)
- **Bandwidth:** ~1.6 bytes/second (with updates)

### Optimization Tips

1. **Adjust Update Interval**
   - Longer intervals = Less CPU/network usage
   - Shorter intervals = Better speed accuracy

2. **Selective Resource Observation**
   - Only observe resources you need
   - Cancel observations when not required

3. **Batch Updates**
   - Use SENML formats for efficient encoding
   - Combine with other object updates

---

## API Reference

See [API_REFERENCE.md](API_REFERENCE.md) for complete API documentation.

---

## References

- [OMA LwM2M Location Specification](https://github.com/OpenMobileAlliance/lwm2m-registry/blob/prod/6.xml)
- [WGS84 Coordinate System](https://en.wikipedia.org/wiki/World_Geodetic_System)
- [Haversine Formula](https://en.wikipedia.org/wiki/Haversine_formula)
- [gpsd Documentation](https://gpsd.io/)
- [3GPP TS 23.032](https://www.3gpp.org/ftp/Specs/archive/23_series/23.032/)
