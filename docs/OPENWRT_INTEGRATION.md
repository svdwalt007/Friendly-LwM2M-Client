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

## WLAN Connectivity Management (Object ID 12)

### Overview

The WLAN Connectivity object provides comprehensive WiFi interface management for OpenWRT, supporting both 2.4GHz and 5GHz bands through multiple instances.

**Object ID:** 12
**URN:** urn:oma:lwm2m:oma:12
**Multiple Instances:** Yes (Instance 0 = wlan0 @ 2.4GHz, Instance 1 = wlan1 @ 5GHz)
**Documentation:** [WLAN_CONNECTIVITY.md](WLAN_CONNECTIVITY.md)

### OpenWRT Integration Features

1. **UCI Configuration Loading**
   - Automatically loads WiFi settings from UCI
   - Supports wireless.@wifi-iface configuration
   - Reads SSID, encryption, channel settings

2. **sysfs Statistics Monitoring**
   - Real-time statistics from `/sys/class/net/wlan*/statistics/`
   - TX/RX bytes, packets, errors
   - Automatic updates every 30 seconds

3. **Multi-Band Support**
   - Instance 0: 2.4GHz (wlan0) - Channels 1-14
   - Instance 1: 5GHz (wlan1) - Channels 36-165
   - Automatic band detection

### UCI Configuration

```bash
# Configure 2.4GHz WiFi (wlan0)
uci set wireless.@wifi-iface[0].device='radio0'
uci set wireless.@wifi-iface[0].mode='ap'
uci set wireless.@wifi-iface[0].ssid='MyNetwork-2.4GHz'
uci set wireless.@wifi-iface[0].encryption='psk2'
uci set wireless.@wifi-iface[0].key='SecurePassword'
uci set wireless.@wifi-iface[0].network='lan'
uci commit wireless

# Configure 5GHz WiFi (wlan1)
uci set wireless.@wifi-iface[1].device='radio1'
uci set wireless.@wifi-iface[1].mode='ap'
uci set wireless.@wifi-iface[1].ssid='MyNetwork-5GHz'
uci set wireless.@wifi-iface[1].encryption='psk2'
uci set wireless.@wifi-iface[1].key='SecurePassword'
uci set wireless.@wifi-iface[1].network='lan'
uci commit wireless
wifi reload
```

### Implementation Example

```cpp
#ifdef OPENWRT_BUILD
void WlanConnectivity::loadFromUCI() {
    // Read SSID from UCI
    std::string cmd = "uci get wireless.@wifi-iface[" +
                      std::to_string(instId()) + "].ssid 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            set<STRING_T>(SSID_5, buffer);
        }
        pclose(pipe);
    }

    // Read encryption settings
    cmd = "uci get wireless.@wifi-iface[" +
          std::to_string(instId()) + "].encryption 2>/dev/null";
    pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            if (strstr(buffer, "psk2")) {
                set<INT_T>(AUTHENTICATION_MODE_15, 1);  // PSK
                set<INT_T>(ENCRYPTION_MODE_16, 0);      // AES
            }
        }
        pclose(pipe);
    }
}

void WlanConnectivity::updateStatistics() {
    std::string statsPath = "/sys/class/net/" + _interfaceName + "/statistics/";

    // Read TX/RX statistics from sysfs
    INT_T bytesSent = readSysfsInt(statsPath + "tx_bytes");
    INT_T bytesReceived = readSysfsInt(statsPath + "rx_bytes");
    INT_T packetsSent = readSysfsInt(statsPath + "tx_packets");
    INT_T packetsReceived = readSysfsInt(statsPath + "rx_packets");

    // Update resources
    set<INT_T>(TOTAL_BYTES_SENT_33, bytesSent);
    set<INT_T>(TOTAL_BYTES_RECEIVED_34, bytesReceived);
    set<INT_T>(TOTAL_PACKETS_SENT_35, packetsSent);
    set<INT_T>(TOTAL_PACKETS_RECEIVED_36, packetsReceived);

    // Notify observers
    notifyResChanged(TOTAL_BYTES_SENT_33);
    notifyResChanged(TOTAL_BYTES_RECEIVED_34);
}
#endif
```

### Data Sources

| Information | Source | Path |
|------------|--------|------|
| Interface status | sysfs | `/sys/class/net/wlan*/operstate` |
| TX/RX bytes | sysfs | `/sys/class/net/wlan*/statistics/tx_bytes` |
| TX/RX packets | sysfs | `/sys/class/net/wlan*/statistics/tx_packets` |
| Errors | sysfs | `/sys/class/net/wlan*/statistics/*_errors` |
| SSID | UCI | `wireless.@wifi-iface[N].ssid` |
| Encryption | UCI | `wireless.@wifi-iface[N].encryption` |
| Channel | UCI | `wireless.@wifi-device[N].channel` |

### Key Features

- **48 Resources**: Complete WiFi management
- **Security Modes**: WPA/WPA2-PSK, WPA/WPA2-EAP, WEP, Open
- **Operating Modes**: Access Point, Client, Bridge, Repeater
- **WiFi Standards**: 802.11a/b/g/n/ac/ax (WiFi 6)
- **Channel Management**: Auto-selection or manual configuration
- **Statistics**: Real-time TX/RX monitoring

---

## Bearer Selection Management (Object ID 13)

### Overview

The Bearer Selection object manages network bearer preferences and enables automatic failover between WiFi, Ethernet, and cellular connections.

**Object ID:** 13
**URN:** urn:oma:lwm2m:oma:13
**Multiple Instances:** No (Single instance)
**Documentation:** [BEARER_SELECTION.md](BEARER_SELECTION.md)

### OpenWRT Integration Features

1. **Automatic Bearer Detection**
   - Checks Ethernet operstate via sysfs
   - Monitors WiFi interface status
   - Queries cellular modem via ModemManager

2. **UCI Configuration Support**
   - Load bearer preferences from UCI
   - Configure signal thresholds
   - Operator whitelist/blacklist

3. **Intelligent Failover**
   - Hysteresis control (prevents ping-pong switching)
   - Signal strength monitoring
   - Priority-based bearer selection

### Supported Network Bearers

| Bearer | ID | OpenWRT Detection Method |
|--------|----|--------------------------|
| GSM | 0 | `mmcli -m 0` (gsm/gprs) |
| WCDMA | 2 | `mmcli -m 0` (umts/hspa) |
| LTE FDD | 6 | `mmcli -m 0` (lte) |
| LTE-M | 7 | `mmcli -m 0` (lte-m) |
| NB-IoT | 8 | `mmcli -m 0` (nb-iot) |
| WiFi | 21 | `/sys/class/net/wlan0/operstate` |
| Ethernet | 41 | `/sys/class/net/eth0/operstate` |
| DSL | 42 | `/sys/class/net/dsl0/operstate` |

### UCI Configuration

```bash
# Create bearer configuration
uci set network.bearer=bearer
uci set network.bearer.preference='21,41,6,2,0'
uci set network.bearer.wlan_rssi='-70'
uci set network.bearer.lte_rsrp='-95'
uci set network.bearer.umts_rscp='-85'
uci set network.bearer.gsm_rssi='-75'
uci set network.bearer.hysteresis='5'
uci commit network
```

### Implementation Example

```cpp
#ifdef OPENWRT_BUILD
void BearerSelection::updateAvailableBearers() {
    _availableBearers.clear();

    // Check Ethernet via sysfs
    FILE* fp = fopen("/sys/class/net/eth0/operstate", "r");
    if (fp) {
        char state[16];
        if (fgets(state, sizeof(state), fp) && strncmp(state, "up", 2) == 0) {
            _availableBearers.push_back(ETHERNET);
        }
        fclose(fp);
    }

    // Check WLAN via sysfs
    fp = fopen("/sys/class/net/wlan0/operstate", "r");
    if (fp) {
        char state[16];
        if (fgets(state, sizeof(state), fp) && strncmp(state, "up", 2) == 0) {
            _availableBearers.push_back(WLAN);
        }
        fclose(fp);
    }

    // Check cellular modem via ModemManager
    FILE* pipe = popen("mmcli -m 0 --output-keyvalue 2>/dev/null | "
                       "grep access-technologies", "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            if (strstr(buffer, "lte")) {
                _availableBearers.push_back(LTE_FDD);
            } else if (strstr(buffer, "umts") || strstr(buffer, "hspa")) {
                _availableBearers.push_back(WCDMA);
            } else if (strstr(buffer, "gsm") || strstr(buffer, "gprs")) {
                _availableBearers.push_back(GSM);
            }
        }
        pclose(pipe);
    }

    // Build available bearers list
    std::string available;
    for (size_t i = 0; i < _availableBearers.size(); i++) {
        if (i > 0) available += ",";
        available += std::to_string(_availableBearers[i]);
    }

    set<STRING_T>(AVAILABLE_NETWORK_BEARERS_8, available);
    notifyResChanged(AVAILABLE_NETWORK_BEARERS_8);
}
#endif
```

### Bearer Selection Algorithm

The device selects the best bearer using this process:

1. **Iterate preference list** (e.g., "21,41,6" = WiFi > Ethernet > LTE)
2. **Check availability** (network interface up)
3. **Verify signal strength** (meets minimum threshold)
4. **Check operator** (whitelist/blacklist for cellular)
5. **Apply hysteresis** (prevent frequent switching)
6. **Select bearer** (highest priority that meets criteria)

### Signal Strength Monitoring

#### ModemManager Integration (Cellular)

```bash
# Get signal strength for LTE
mmcli -m 0 --output-keyvalue | grep signal.lte.rsrp
# Output: modem.generic.signal-quality.lte.rsrp: -95

# Get signal strength for UMTS
mmcli -m 0 --output-keyvalue | grep signal.umts.rscp
# Output: modem.generic.signal-quality.umts.rscp: -85
```

#### WiFi Signal Strength (iw/iwinfo)

```bash
# Get WiFi signal strength
iw dev wlan0 link | grep signal
# Output: signal: -70 dBm

# Or using iwinfo
iwinfo wlan0 info | grep Signal
# Output: Signal: -70 dBm
```

### Data Sources

| Information | Source | Command/Path |
|------------|--------|--------------|
| Ethernet status | sysfs | `/sys/class/net/eth0/operstate` |
| WiFi status | sysfs | `/sys/class/net/wlan0/operstate` |
| WiFi signal | iw/iwinfo | `iw dev wlan0 link` |
| Cellular technology | ModemManager | `mmcli -m 0` |
| LTE signal (RSRP) | ModemManager | `mmcli -m 0 --output-keyvalue \| grep rsrp` |
| UMTS signal (RSCP) | ModemManager | `mmcli -m 0 --output-keyvalue \| grep rscp` |
| GSM signal (RSSI) | ModemManager | `mmcli -m 0 --output-keyvalue \| grep rssi` |
| Bearer preference | UCI | `network.bearer.preference` |

### Use Cases

#### Example 1: WiFi Preferred with Cellular Fallback

```bash
# Prefer WiFi (21), fallback to LTE (6), then GSM (0)
uci set network.bearer.preference='21,6,0'
uci set network.bearer.wlan_rssi='-70'
uci set network.bearer.lte_rsrp='-95'
uci set network.bearer.hysteresis='5'
uci commit network
```

#### Example 2: Ethernet Only (Fixed Installation)

```bash
# Use only Ethernet, disable other bearers
uci set network.bearer.preference='41'
uci set network.bearer.wlan_rssi='-30'   # Very high threshold
uci set network.bearer.lte_rsrp='-50'    # Very high threshold
uci commit network
```

#### Example 3: Operator Whitelist (Roaming Control)

```bash
# Only connect to specific operators (AT&T and T-Mobile US)
uci set network.bearer.operators='310-410,310-260'
uci set network.bearer.operator_mode='0'  # 0=whitelist, 1=blacklist
uci commit network
```

### Key Features

- **12 Resources**: Complete bearer management
- **Hysteresis Control**: Prevents ping-pong switching (typically 5-10 dB)
- **Multi-Technology**: Cellular (GSM/UMTS/LTE), WiFi, Ethernet, DSL
- **Operator Management**: PLMN whitelist/blacklist for roaming
- **Automatic Detection**: Periodic bearer availability checks
- **UCI Integration**: Persistent configuration storage

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

### Starlink Terminal Object (ID 10512)

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
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10512_STARLINK_TERMINAL)
```

#### Initialize in Application

**examples/objects.cpp:**
```cpp
#ifdef OBJ_O_10512_STARLINK_TERMINAL
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
# Object 10512, Resource 3: CONNECTION_STATE
# Values: CONNECTED, SEARCHING, BOOTING, STOWED, etc.

# Check obstruction status
# Object 10512, Resource 30: FRACTION_OBSTRUCTED (0.0-1.0)
# Object 10512, Resource 31: CURRENTLY_OBSTRUCTED (true/false)

# Monitor network performance
# Object 10512, Resource 10: DOWNLINK_THROUGHPUT (bps)
# Object 10512, Resource 11: UPLINK_THROUGHPUT (bps)
# Object 10512, Resource 12: POP_PING_LATENCY (ms)
```

#### Integration with Location Object

When both Starlink Terminal (10512) and Location (6) objects are enabled, the Location object automatically uses Starlink GPS as its primary source:

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

### System Monitor Object (ID 10518)

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

**LwM2M Watchdog Object (ID 10519):**

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

## Walt Technologies Objects Integration (10512-10520)

The Friendly LwM2M Client includes **9 custom Walt Technologies objects** designed specifically for OpenWRT router and IoT gateway management. These objects provide comprehensive monitoring and control capabilities.

### Available Objects Overview

| Object ID | Name | Purpose | Instances | OpenWRT Integration |
|-----------|------|---------|-----------|---------------------|
| 10512 | Starlink Terminal | Satellite terminal management | Single | gRPC |
| 10513 | Router Management | Core router configuration | Single | UCI, network config |
| 10514 | Ethernet Interface | Ethernet port monitoring | Multiple | sysfs (/sys/class/net) |
| 10515 | GPIO Control | GPIO/LED/Button control | Multiple | sysfs (/sys/class/gpio) |
| 10516 | USB Management | USB port management | Multiple | sysfs (/sys/bus/usb) |
| 10517 | Storage Management | Storage device management | Multiple | mount, df, filesystem |
| 10518 | System Monitor | System health monitoring | Single | /proc (stat, meminfo) |
| 10519 | Hardware Watchdog | Watchdog timer management | Single | /dev/watchdog |
| 10520 | MIKROBUS | MIKROBUS socket and Click boards | Multiple | I2C, SPI, GPIO |

### System Monitor (ID 10518) - OpenWRT Integration

**Purpose:** Monitor CPU, RAM, load, uptime, and temperature

**Data Sources:**

```bash
# CPU usage from /proc/stat
cat /proc/stat
# Output: cpu user nice system idle iowait irq softirq

# CPU temperature from thermal zone
cat /sys/class/thermal/thermal_zone0/temp
# Output: 45000 (45°C in millidegrees)

# Memory statistics from /proc/meminfo
cat /proc/meminfo
# Output: MemTotal, MemFree, MemAvailable, Cached, Buffers

# Load averages from /proc/loadavg
cat /proc/loadavg
# Output: 0.15 0.10 0.08 1/42 1234

# Uptime from /proc/uptime
cat /proc/uptime
# Output: 12345.67 98765.43
```

**Enable System Monitor:**

```cmake
# In wpp/configs/wpp_config.cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10518_SYSTEM_MONITOR)
```

**Key Resources:**
- CPU Usage (0): 0-100%
- RAM Total/Used/Free (3-5): MB
- Load Averages (10-12): 1, 5, 15 minute
- Uptime (13): seconds

### MIKROBUS Object (ID 10520) - OpenWRT Integration

**Purpose:** Manage MIKROBUS sockets and MikroElektronika Click boards

**OpenWRT Prerequisites:**

```bash
# Install I2C tools
opkg update
opkg install i2c-tools

# Install SPI support (if needed)
opkg install kmod-spi-dev

# Verify I2C buses
i2cdetect -l
# Output: i2c-0, i2c-1, etc.
```

**GPIO Access:**

```bash
# Export GPIO for RST pin
echo mikrobus0_rst > /sys/class/gpio/export
echo out > /sys/class/gpio/mikrobus0_rst/direction
echo 1 > /sys/class/gpio/mikrobus0_rst/value

# Export GPIO for CS pin
echo mikrobus0_cs > /sys/class/gpio/export
echo out > /sys/class/gpio/mikrobus0_cs/direction
```

**Click Board Detection:**

```bash
# Detect Click board via I2C EEPROM at address 0x50
i2cdetect -y 0

# Read manifest from EEPROM
i2cdump -y 0 0x50
```

**Enable MIKROBUS:**

```cmake
# In wpp/configs/wpp_config.cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10520_MIKROBUS)
```

**Example Configuration:**

```cpp
// Configure MIKROBUS socket 0 for I2C Temperature sensor
Instance *mikrobus0 = Mikrobus::createInst(client, 0);
mikrobus0->set<STRING_T>(Mikrobus::SOCKET_NAME_1, "TEMP-SENSOR");
mikrobus0->set<INT_T>(Mikrobus::ACTIVE_INTERFACE_20, 2);  // I2C
mikrobus0->set<INT_T>(Mikrobus::I2C_ADDRESS_21, 0x48);
mikrobus0->set<INT_T>(Mikrobus::POWER_VOLTAGE_5, 3300);   // 3.3V
mikrobus0->set<BOOL_T>(Mikrobus::POWER_STATE_6, true);
```

### Router Management (ID 10513) - OpenWRT Integration

**Purpose:** Core router configuration and management

**UCI Integration:**

```bash
# LAN configuration
uci set network.lan.ipaddr='192.168.1.1'
uci set network.lan.netmask='255.255.255.0'
uci commit network

# DHCP configuration
uci set dhcp.lan.start='100'
uci set dhcp.lan.limit='150'
uci set dhcp.lan.leasetime='12h'
uci commit dhcp

# Firewall configuration
uci set firewall.@defaults[0].input='ACCEPT'
uci set firewall.@defaults[0].forward='ACCEPT'
uci commit firewall

# Apply changes
/etc/init.d/network restart
/etc/init.d/dnsmasq restart
/etc/init.d/firewall restart
```

### Ethernet Interface (ID 10514) - OpenWRT Integration

**Purpose:** Monitor Ethernet ports (WAN/LAN)

**Data Sources:**

```bash
# Link status
cat /sys/class/net/eth0/carrier
# Output: 1 (link up) or 0 (link down)

# Link speed
cat /sys/class/net/eth0/speed
# Output: 1000 (1000 Mbps)

# Duplex mode
cat /sys/class/net/eth0/duplex
# Output: full or half

# MAC address
cat /sys/class/net/eth0/address
# Output: 00:11:22:33:44:55

# Traffic statistics
cat /sys/class/net/eth0/statistics/rx_bytes
cat /sys/class/net/eth0/statistics/tx_bytes
cat /sys/class/net/eth0/statistics/rx_errors
cat /sys/class/net/eth0/statistics/tx_errors
```

**Multi-Instance Configuration:**

```cpp
// Create instance for WAN port
Instance *wan = EthernetInterface::createInst(client, 0);
wan->set<STRING_T>(EthernetInterface::INTERFACE_NAME_0, "eth1");
wan->set<INT_T>(EthernetInterface::PORT_TYPE_1, 1);  // WAN

// Create instance for LAN port
Instance *lan = EthernetInterface::createInst(client, 1);
lan->set<STRING_T>(EthernetInterface::INTERFACE_NAME_0, "eth0");
lan->set<INT_T>(EthernetInterface::PORT_TYPE_1, 0);  // LAN
```

### GPIO Control (ID 10515) - OpenWRT Integration

**Purpose:** LED and button management

**LED Control:**

```bash
# Available LEDs
ls /sys/class/leds/
# Output: led0, led1, wan-green, wan-red, etc.

# Set LED state
echo 1 > /sys/class/leds/led0/brightness  # On
echo 0 > /sys/class/leds/led0/brightness  # Off

# Set LED trigger
echo timer > /sys/class/leds/led0/trigger
echo 500 > /sys/class/leds/led0/delay_on   # 500ms on
echo 500 > /sys/class/leds/led0/delay_off  # 500ms off
```

**Button Monitoring:**

```bash
# GPIO button input
cat /sys/class/gpio/gpio12/value
# Output: 1 (pressed) or 0 (released)
```

### Storage Management (ID 10517) - OpenWRT Integration

**Purpose:** Manage NAND, NVMe, USB, SD card storage

**Data Sources:**

```bash
# List block devices
block info
# Output: /dev/mmcblk0p1, /dev/sda1, etc.

# Check filesystem usage
df -h
# Output: Filesystem, Size, Used, Available, Use%

# Mount information
mount | grep -E 'sd|mmc|nvme'
# Output: /dev/sda1 on /mnt/usb type ext4

# Storage device type detection
ls /sys/block/
# Output: mmcblk0 (SD), sda (USB), nvme0n1 (NVMe)
```

**Mount/Unmount Operations:**

```bash
# Mount USB storage
mount /dev/sda1 /mnt/usb

# Unmount safely
umount /mnt/usb

# Format storage
mkfs.ext4 /dev/sda1
```

### Hardware Watchdog (ID 10519) - OpenWRT Integration

**Purpose:** Watchdog timer management

**Watchdog Device Access:**

```bash
# Watchdog device
ls /dev/watchdog*
# Output: /dev/watchdog, /dev/watchdog0

# Check watchdog support
cat /sys/class/watchdog/watchdog0/info
# Output: identity, firmware_version, options

# Configure watchdog timeout
echo 30 > /sys/class/watchdog/watchdog0/timeout
cat /sys/class/watchdog/watchdog0/timeout
# Output: 30 (seconds)
```

**procd Integration:**

OpenWRT's procd already manages the watchdog. The LwM2M object provides monitoring and configuration:

```bash
# Check procd watchdog status
ubus call system watchdog
# Output: {"frequency":5,"timeout":60,"magicclose":false}
```

### Complete Integration Example

**Enable All Walt Technologies Objects:**

```cmake
# In wpp/configs/wpp_config.cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS}
    OBJ_O_10512_STARLINK_TERMINAL
    OBJ_O_10513_ROUTER_MANAGEMENT
    OBJ_O_10514_ETHERNET_INTERFACE
    OBJ_O_10515_GPIO_CONTROL
    OBJ_O_10516_USB_MANAGEMENT
    OBJ_O_10517_STORAGE_MANAGEMENT
    OBJ_O_10518_SYSTEM_MONITOR
    OBJ_O_10519_HARDWARE_WATCHDOG
    OBJ_O_10520_MIKROBUS
)
```

**Initialize All Objects:**

```cpp
// In examples/main.cpp
#ifdef OBJ_O_10512_STARLINK_TERMINAL
starlinkTerminalInit(*client);
#endif

#ifdef OBJ_O_10518_SYSTEM_MONITOR
systemMonitorInit(*client);
#endif

#ifdef OBJ_O_10520_MIKROBUS
mikrobusInit(*client);
#endif

// ... initialize other Walt objects
```

### Monitoring All Objects

```bash
# View all object instances
cat /var/log/lwm2m.log | grep "Initialization"

# Expected output:
# ---- Initialization wpp Starlink Terminal ----
# ---- Initialization wpp System Monitor ----
# ---- Initialization wpp MIKROBUS ----
# ---- Initialization wpp Router Management ----
# ... etc.
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
