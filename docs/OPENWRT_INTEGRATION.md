# OpenWRT Integration Guide

Comprehensive guide for integrating LwM2M Client with OpenWRT platform features.

---

## Overview

This guide covers OpenWRT-specific implementations for:
- Device information gathering
- Network connectivity monitoring
- GPS/Location services
- System monitoring
- Hardware watchdog integration

---

## Hardware Information (Device Object)

### OpenWrtDeviceInfo Helper

**File:** `wpp/registry/objects/m_3_device/OpenWrtDeviceInfo.h`

### Available Methods

```cpp
// Device identification
static std::string getManufacturer();      // "Banana Pi (OpenWRT One)"
static std::string getModelNumber();       // "OpenWRT One (BPI-R4)"
static std::string getSerialNumber();      // "OWO-<MAC_ADDRESS>"
static std::string getFirmwareVersion();   // From /etc/openwrt_release
static std::string getHardwareVersion();   // "MT7981B"

// Power information
static std::vector<int> getAvailablePowerSources();
static int getPowerSourceVoltage(int source);
static int getPowerSourceCurrent(int source);

// Memory information
static int getMemoryFree();                // From /proc/meminfo
static int getMemoryTotal();               // From /proc/meminfo

// System operations
static bool performReboot();               // Executes 'reboot' command
```

### Data Sources

| Information | Source | Path |
|------------|--------|------|
| Model | Device tree | `/proc/device-tree/model` |
| Serial (MAC) | Network interface | `/sys/class/net/eth1/address` |
| Firmware version | OpenWRT release | `/etc/openwrt_release` |
| Free memory | Proc filesystem | `/proc/meminfo` |
| Uptime | Proc filesystem | `/proc/uptime` |

### Implementation Example

```cpp
#ifdef OPENWRT_BUILD
resource(MANUFACTURER_0)->set<STRING_T>(OpenWrtDeviceInfo::getManufacturer());
resource(MODEL_NUMBER_1)->set<STRING_T>(OpenWrtDeviceInfo::getModelNumber());
resource(SERIAL_NUMBER_2)->set<STRING_T>(OpenWrtDeviceInfo::getSerialNumber());
#endif
```

---

## Network Connectivity Monitoring

### OpenWrtConnectivityInfo Helper

**File:** `wpp/registry/objects/o_4_connectivity_monitoring/OpenWrtConnectivityInfo.h`

### Available Methods

```cpp
// Network bearer detection
static int getNetworkBearer();                    // Auto-detect primary bearer
static std::vector<int> getAvailableNetworkBearers();

// Signal quality
static int getRadioSignalStrength();              // dBm for wireless
static int getLinkQuality();                      // 0-100%
static int getLinkUtilization();                  // Bandwidth usage %

// Network configuration
static std::vector<std::string> getIpAddresses(); // IPv4/IPv6 addresses
static std::vector<std::string> getRouterIpAddresses(); // Gateway IPs
static std::vector<std::string> getAPN();         // Cellular APN

// Cellular information (if available)
static int getCellId();                           // Cell tower ID
static int getSMNC();                             // Mobile network code
static int getSMCC();                             // Mobile country code
```

### Bearer Types

```cpp
enum NtwrkBrr: INT_T {
    GSM = 0,
    TD_SCDMA = 1,
    WCDMA = 2,
    CDMA2000 = 3,
    WIMAX = 4,
    LTE_TDD = 5,
    LTE_FDD = 6,
    LTE_M = 7,
    NBIOT = 8,
    ETHERNET = 41,
    WLAN = 21,
};
```

### Network Detection Logic

```cpp
static int getNetworkBearer() {
    // Check for wireless WAN
    if (isWirelessInterface("wan")) {
        return NtwrkBrr::WLAN;  // 21
    }

    // Check for cellular modem
    if (hasCellularModem()) {
        return NtwrkBrr::LTE_FDD;  // 6
    }

    // Default to Ethernet
    return NtwrkBrr::ETHERNET;  // 41
}
```

---

## GPS and Location Services

### Location Data Sources

See [LOCATION_OBJECT.md](LOCATION_OBJECT.md) for complete details.

#### 1. Starlink Terminal GPS

**Prerequisites:**
- Starlink dish at 192.168.100.1:9200
- grpcurl installed (optional)

**Access Method:**
```cpp
bool getStarlinkLocation(double& lat, double& lon, double& alt) {
    std::string cmd = "grpcurl -plaintext -d '{\"get_status\":{}}' "
                     "192.168.100.1:9200 SpaceX.API.Device.Device/Handle";
    std::string result = executeCommand(cmd);
    // Parse JSON for latitude, longitude, altitude
    return parseStarlinkGPS(result, lat, lon, alt);
}
```

#### 2. gpsd Integration

**Installation:**
```bash
opkg update
opkg install gpsd gpsd-clients

uci set gpsd.core.enabled='1'
uci set gpsd.core.device='/dev/ttyUSB0'
uci commit gpsd
/etc/init.d/gpsd start
```

**Access Method:**
```cpp
bool getGpsdLocation(double& lat, double& lon, double& alt) {
    std::string cmd = "gpspipe -w -n 10 | grep -m 1 TPV";
    std::string result = executeCommand(cmd);
    // Parse JSON TPV record
    return parseGpsdTPV(result, lat, lon, alt);
}
```

#### 3. UCI Manual Configuration

**Configuration:**
```bash
uci set lwm2m.location=location
uci set lwm2m.location.latitude='40.7128'
uci set lwm2m.location.longitude='-74.0060'
uci set lwm2m.location.altitude='10'
uci commit lwm2m
```

**Access Method:**
```cpp
bool getManualLocation(double& lat, double& lon, double& alt) {
    lat = std::stod(executeCommand("uci get lwm2m.location.latitude"));
    lon = std::stod(executeCommand("uci get lwm2m.location.longitude"));
    alt = std::stod(executeCommand("uci get lwm2m.location.altitude"));
    return true;
}
```

---

## Starlink Terminal Integration

### Starlink Terminal Object (ID 34600)

Comprehensive management and monitoring of Starlink Gen3 satellite terminals via gRPC interface.

#### Prerequisites

- Starlink dish accessible on network
- Default gRPC endpoint: `192.168.100.1:9200`
- Network routing configured to reach Starlink dish

#### Verify Connectivity

```bash
# Ping Starlink dish
ping 192.168.100.1

# Test gRPC endpoint (requires grpcurl)
grpcurl -plaintext 192.168.100.1:9200 list

# Expected output:
# SpaceX.API.Device.Device
```

#### Enable in Configuration

**wpp/configs/wpp_config.cmake:**
```cmake
# Enable Starlink Terminal object
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_34600_STARLINK_TERMINAL)
```

#### Initialize in Application

**examples/objects.cpp:**
```cpp
#ifdef OBJ_O_34600_STARLINK_TERMINAL
void starlinkTerminalInit(WppClient &client) {
    client.registry().registerObj(StarlinkTerminal::object(client));
    StarlinkTerminal::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(StarlinkTerminal::object(client),
                               Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    #endif
}
#endif
```

#### gRPC Communication

The Starlink Terminal object uses gRPC to communicate with the dish:

```cpp
// Example: Reading Starlink status via gRPC
#include <grpcpp/grpcpp.h>
#include "spacex/api/device/device.grpc.pb.h"

bool updateStarlinkTelemetry() {
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

        // Access telemetry data
        float downlink = dishStatus.downlink_throughput_bps();
        float uplink = dishStatus.uplink_throughput_bps();
        float latency = dishStatus.pop_ping_latency_ms();
        bool obstructed = dishStatus.obstruction_stats().currently_obstructed();

        return true;
    }

    return false;
}
```

#### OpenWRT Network Configuration

For Starlink integration on OpenWRT, ensure proper network routing:

```bash
# Add route to Starlink network (if not on same subnet)
ip route add 192.168.100.0/24 via <gateway_ip>

# Or configure via UCI
uci set network.starlink=interface
uci set network.starlink.proto='static'
uci set network.starlink.ipaddr='192.168.100.2'
uci set network.starlink.netmask='255.255.255.0'
uci commit network
/etc/init.d/network restart
```

#### Firewall Configuration

```bash
# Allow traffic to Starlink dish gRPC port
uci add firewall rule
uci set firewall.@rule[-1].name='Allow-Starlink-gRPC'
uci set firewall.@rule[-1].src='lan'
uci set firewall.@rule[-1].dest_ip='192.168.100.1'
uci set firewall.@rule[-1].dest_port='9200'
uci set firewall.@rule[-1].proto='tcp'
uci set firewall.@rule[-1].target='ACCEPT'
uci commit firewall
/etc/init.d/firewall restart
```

#### Resource Groups

The Starlink Terminal object provides 70+ resources organized into:

1. **Device Information (0-9)**
   - Serial number, hardware/software versions
   - Connection state, uptime

2. **Network Performance (10-29)**
   - Throughput (downlink/uplink)
   - Latency, packet loss, SNR
   - Data usage counters

3. **Obstruction Monitoring (30-39)**
   - Obstruction fraction
   - 12-wedge sky view analysis
   - Obstruction duration/interval stats

4. **Dish Alignment (40-49)**
   - Azimuth (0-360°) and elevation (0-90°)
   - Stowed state (read/write)

5. **Alert System (50-79)**
   - 16 different alert conditions
   - Thermal, motor, water detection alerts

6. **GPS Location (80-89)**
   - Latitude, longitude, altitude
   - GPS ready/enabled status
   - Satellite count

7. **Power Management (90-99)**
   - Current, mean, min/max power
   - Total energy consumption

8. **Control Actions (100-107)**
   - Reboot terminal
   - Run speed test
   - Get obstruction map
   - Configure gRPC endpoint

9. **Historical Data (110-119)**
   - Performance statistics
   - Latency deciles
   - Load bucket stats

#### Monitoring on OpenWRT

```bash
# Monitor Starlink connection state via LwM2M
# Object 34600, Resource 3: CONNECTION_STATE
# Values: CONNECTED, SEARCHING, BOOTING, STOWED, etc.

# Check obstruction status
# Object 34600, Resource 30: FRACTION_OBSTRUCTED (0.0-1.0)
# Object 34600, Resource 31: CURRENTLY_OBSTRUCTED (true/false)

# Monitor network performance
# Object 34600, Resource 10: DOWNLINK_THROUGHPUT (bps)
# Object 34600, Resource 11: UPLINK_THROUGHPUT (bps)
# Object 34600, Resource 12: POP_PING_LATENCY (ms)
```

#### Integration with Location Object

When both Starlink Terminal (34600) and Location (6) objects are enabled, the Location object automatically uses Starlink GPS as its primary source:

```cpp
// Automatic GPS source priority:
// 1. Starlink Terminal GPS (Resources 83-85)
// 2. gpsd daemon
// 3. UCI manual configuration

// No additional configuration needed - works automatically!
```

**See Also:** [Starlink Terminal Documentation](STARLINK_TERMINAL.md) for complete details

---

## System Monitoring

### System Monitor Object (ID 34606)

Monitors OpenWRT system resources:

**Resources:**
- CPU usage percentage
- RAM usage (free/total)
- Load average (1/5/15 min)
- System uptime
- Process count
- Disk usage

**Data Sources:**
```bash
# CPU usage
cat /proc/stat | grep "cpu "

# Memory
cat /proc/meminfo

# Load average
cat /proc/loadavg

# Uptime
cat /proc/uptime

# Processes
ps | wc -l

# Disk
df -h /
```

---

## Hardware Watchdog

### Watchdog Configuration

**OpenWRT Watchdog Support:**
```bash
# Install watchdog package
opkg install watchdog

# Configure
uci set system.@watchdog[0].enabled='1'
uci commit system
/etc/init.d/system reload
```

**LwM2M Watchdog Object (ID 34607):**

Provides:
- Watchdog enable/disable
- Timeout configuration
- Manual trigger
- Status monitoring

---

## UCI Configuration System

### LwM2M UCI Schema

Create `/etc/config/lwm2m`:

```bash
config lwm2m 'server'
    option uri 'coap://server.example.com:5683'
    option endpoint 'openwrt-device-001'
    option lifetime '300'
    option enabled '1'

config lwm2m 'security'
    option mode 'psk'
    option identity 'device-001'
    option key 'secret-key'

config location
    option latitude '0.0'
    option longitude '0.0'
    option altitude '0.0'
```

### UCI Access in C++

```cpp
std::string getUciValue(const std::string& option) {
    std::string cmd = "uci get lwm2m." + option;
    return executeCommand(cmd);
}

void setUciValue(const std::string& option, const std::string& value) {
    std::string cmd = "uci set lwm2m." + option + "='" + value + "'";
    executeCommand(cmd);
    executeCommand("uci commit lwm2m");
}
```

---

## Filesystem Access

### Reading System Information

```cpp
static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Trim whitespace
    size_t start = content.find_first_not_of(" \t\n\r");
    size_t end = content.find_last_not_of(" \t\n\r");
    return (start != std::string::npos) ? content.substr(start, end - start + 1) : "";
}
```

### Executing Commands

```cpp
static std::string executeCommand(const std::string& cmd) {
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    // Trim newline
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}
```

---

## Performance Optimization

### 1. Minimize System Calls

**Bad:**
```cpp
for (int i = 0; i < 100; i++) {
    executeCommand("uci get lwm2m.resource" + std::to_string(i));
}
```

**Good:**
```cpp
std::string all = executeCommand("uci show lwm2m");
// Parse all values at once
```

### 2. Cache Static Values

```cpp
class OpenWrtDeviceInfo {
    static std::string _cachedModel;
    static bool _modelCached;

public:
    static std::string getModelNumber() {
        if (!_modelCached) {
            _cachedModel = readFile("/proc/device-tree/model");
            _modelCached = true;
        }
        return _cachedModel;
    }
};
```

### 3. Use Task Queue for Periodic Updates

```cpp
// Update memory every 60 seconds instead of every read
WppTaskQueue::addTask(60, [this](WppClient &client, void *ctx) {
    int memFree = OpenWrtDeviceInfo::getMemoryFree();
    resource(MEMORY_FREE_10)->set<INT_T>(memFree);
    notifyResChanged(MEMORY_FREE_10);
    return false;
});
```

---

## Security Considerations

### 1. Command Injection Prevention

**Bad:**
```cpp
std::string cmd = "uci get " + userInput;  // DANGEROUS!
executeCommand(cmd);
```

**Good:**
```cpp
std::string sanitize(const std::string& input) {
    // Only allow alphanumeric and dots
    std::string safe;
    for (char c : input) {
        if (std::isalnum(c) || c == '.') {
            safe += c;
        }
    }
    return safe;
}

std::string cmd = "uci get lwm2m." + sanitize(userInput);
executeCommand(cmd);
```

### 2. File Access Restrictions

```cpp
bool isPathSafe(const std::string& path) {
    // Only allow specific directories
    return (path.find("/proc/") == 0 ||
            path.find("/sys/") == 0 ||
            path.find("/etc/openwrt_") == 0);
}

std::string readFile(const std::string& path) {
    if (!isPathSafe(path)) {
        return "";
    }
    // ... proceed with read
}
```

### 3. Privilege Separation

Run LwM2M client with minimal privileges:

```bash
# Create dedicated user
useradd -r -s /bin/false lwm2m

# Set capabilities instead of running as root
setcap cap_net_bind_service=+ep /usr/bin/lwm2m_client
```

---

## Deployment Best Practices

### 1. Init Script

Create `/etc/init.d/lwm2m`:

```bash
#!/bin/sh /etc/rc.common

START=99
STOP=10

USE_PROCD=1

start_service() {
    procd_open_instance
    procd_set_param command /usr/bin/lwm2m_client
    procd_set_param respawn
    procd_set_param stdout 1
    procd_set_param stderr 1
    procd_close_instance
}
```

### 2. Log Rotation

Create `/etc/logrotate.d/lwm2m`:

```
/var/log/lwm2m.log {
    daily
    rotate 7
    compress
    delaycompress
    missingok
    notifempty
}
```

### 3. Monitoring

```bash
# Check if running
/etc/init.d/lwm2m status

# View logs
logread | grep lwm2m

# Monitor resources
top -b -n 1 | grep lwm2m_client
```

---

## Troubleshooting

### Common Issues

1. **Permission Denied**
   - Solution: Run with appropriate capabilities or as root
   - Check: `ls -la /usr/bin/lwm2m_client`

2. **UCI Not Found**
   - Solution: Install UCI: `opkg install uci`
   - Verify: `which uci`

3. **GPS Not Working**
   - Check Starlink: `ping 192.168.100.1`
   - Check gpsd: `ps | grep gpsd`
   - Check config: `uci show lwm2m.location`

4. **High CPU Usage**
   - Reduce update frequency
   - Cache values
   - Use task queue instead of polling

---

## References

- [OpenWRT Developer Guide](https://openwrt.org/docs/guide-developer/start)
- [UCI Configuration](https://openwrt.org/docs/guide-user/base-system/uci)
- [OpenWRT One Hardware](https://openwrt.org/toh/openwrt/one)
- [procd Init System](https://openwrt.org/docs/guide-developer/procd-init-scripts)
