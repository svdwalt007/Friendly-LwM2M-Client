# Quick Start Guide

Get your Friendly LwM2M Client up and running in minutes!

---

## 🎯 Prerequisites

### Development Environment

**Operating System:**
- Linux (Ubuntu 20.04+ recommended)
- macOS (with Homebrew)
- Windows (WSL2)

**Required Tools:**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    git \
    cmake \
    build-essential \
    gcc \
    g++ \
    make

# macOS
brew install cmake git

# Fedora/RHEL
sudo dnf install -y git cmake gcc gcc-c++ make
```

### For OpenWRT Builds

**OpenWRT SDK:**
```bash
# Download OpenWRT SDK for your target
wget https://downloads.openwrt.org/releases/23.05.0/targets/mediatek/filogic/openwrt-sdk-23.05.0-mediatek-filogic_gcc-12.3.0_musl.Linux-x86_64.tar.xz

# Extract
tar xf openwrt-sdk-*.tar.xz
cd openwrt-sdk-*/
```

**Cross-Compilation Tools:**
```bash
# Install OpenWRT toolchain
./scripts/feeds update -a
./scripts/feeds install -a
```

---

## 📦 Installation

### Step 1: Clone the Repository

```bash
git clone https://github.com/your-org/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client
```

### Step 2: Standard Build (Linux/macOS)

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
make -j$(nproc)

# The binary will be at: build/examples/lwm2m_client
```

### Step 3: Run the Client

```bash
# Run with default configuration
./examples/lwm2m_client

# You should see output like:
# ---- Initialization wpp Server ----
# ---- Initialization wpp Security ----
# ---- Initialization wpp Device ----
# ---- Initialization wpp Location ----
# ...
```

---

## 🔧 Quick Configuration

### Configure LwM2M Server Connection

Edit the server configuration in `examples/objects.cpp`:

```cpp
void serverInit(WppClient &client) {
    Instance *server = Lwm2mServer::createInst(client);

    // Set your LwM2M server details
    server->set<INT_T>(Lwm2mServer::SHORT_SERVER_ID_0, 123);
    server->set<INT_T>(Lwm2mServer::LIFETIME_1, 300);  // 5 minutes
    server->set<INT_T>(Lwm2mServer::MIN_PERIOD_2, 10);
    server->set<INT_T>(Lwm2mServer::MAX_PERIOD_3, 60);
    server->set<BOOL_T>(Lwm2mServer::BINDING_7, true);
}
```

### Configure Security Credentials

Edit security configuration in `examples/objects.cpp`:

```cpp
void securityInit(WppClient &client) {
    Instance *security = Lwm2mSecurity::createInst(client);

    // CoAP server URI
    security->set<STRING_T>(Lwm2mSecurity::SERVER_URI_0,
                            "coap://your-server.example.com:5683");

    // Bootstrap server (0 = false, 1 = true)
    security->set<BOOL_T>(Lwm2mSecurity::BOOTSTRAP_SERVER_1, false);

    // Security mode (0 = PSK, 2 = Certificate, 3 = NoSec)
    security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, 3);  // NoSec for testing

    // Server ID
    security->set<INT_T>(Lwm2mSecurity::SHORT_SERVER_ID_10, 123);
}
```

---

## 🔧 Configuring the Client via Command-Line

The LwM2M client supports extensive command-line configuration, allowing you to customize connection parameters, security settings, and behavior without modifying code.

### Available Command-Line Options

#### Core Options
- `-n, --name=NAME` - Endpoint name (default: walttech888)
- `-u, --uri=URI` - Server URI (default: coap://demo-iot.friendly-tech.com:5680)
- `-p, --port=PORT` - Local port (default: 56830)
- `-l, --lifetime=SECONDS` - Registration lifetime (default: 25)
- `-4, --ipv4` - Use IPv4 (default)
- `-6, --ipv6` - Use IPv6
- `-b, --bootstrap` - Enable bootstrap mode (default: true)
- `-B, --no-bootstrap` - Disable bootstrap mode

#### Security Options
- `-s, --security=MODE` - Security mode: none|psk|rpk|cert (default: none)
- `-i, --psk-identity=ID` - PSK identity string
- `-k, --psk-key=KEY` - PSK key (hex string)
- `--rpk-public=KEY` - RPK public key (hex string)
- `--rpk-private=KEY` - RPK private key (hex string)
- `--cert=FILE` - Certificate file path
- `--key=FILE` - Private key file path
- `--ca=FILE` - CA certificate file path

#### CoAP Options
- `--block-size=SIZE` - Block size in bytes (16-1024, power of 2, default: 1024)

#### Application Options
- `-v, --verbose` - Increase verbosity (can be used multiple times)
- `-q, --quiet` - Suppress non-error output
- `-d, --daemon` - Run as daemon
- `-c, --config=FILE` - Load configuration from file

#### Information
- `-h, --help` - Show help message
- `-V, --version` - Show version information

### Common Use Cases

#### 1. Running with Default Settings

The simplest way to start the client with default configuration:

```bash
./lwm2m_client
```

This connects to the default demo server (coap://demo-iot.friendly-tech.com:5680) with endpoint name "walttech888".

#### 2. Connecting to a Custom Server

Connect to your own LwM2M server with a custom endpoint name:

```bash
./lwm2m_client -n mydevice -u coap://your-server.example.com:5683
```

For Leshan demo server testing:

```bash
./lwm2m_client -n testdevice123 -u coap://leshan.eclipseprojects.io:5683

# Then visit: https://leshan.eclipseprojects.io/
# Look for "testdevice123" in the client list
```

#### 3. Using PSK Security

Connect with Pre-Shared Key (PSK) security for encrypted communication:

```bash
./lwm2m_client \
  -n secure-device \
  -u coaps://secure-server.example.com:5684 \
  -s psk \
  -i mydevice-identity \
  -k 0123456789abcdef0123456789abcdef
```

Important PSK notes:
- Use `coaps://` (not `coap://`) for secure connections
- PSK key must be a hex string (32 hex characters = 16 bytes for 128-bit key)
- PSK identity must match the server configuration
- Common key lengths: 128-bit (32 hex chars), 256-bit (64 hex chars)

Example with 256-bit key:

```bash
./lwm2m_client \
  -s psk \
  -i production-device-001 \
  -k 00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff \
  -u coaps://prod-server.example.com:5684
```

#### 4. Changing the Endpoint Name

The endpoint name identifies your device on the LwM2M server:

```bash
# Simple name change
./lwm2m_client -n office-gateway-01

# With custom server
./lwm2m_client -n warehouse-sensor-42 -u coap://10.0.1.50:5683

# With full configuration
./lwm2m_client \
  -n "BuildingA-Floor2-Room203" \
  -u coap://building-mgmt.example.com:5683 \
  -l 300
```

#### 5. Custom Port Configuration

Specify a custom local UDP port for the client:

```bash
# Use port 12345 instead of default 56830
./lwm2m_client -p 12345

# Useful when running multiple clients on same host
./lwm2m_client -n device1 -p 56831
./lwm2m_client -n device2 -p 56832
```

#### 6. IPv6 Configuration

Use IPv6 instead of IPv4:

```bash
# Connect to IPv6 server
./lwm2m_client -6 -u coap://[2001:db8::1]:5683

# IPv6 localhost for testing
./lwm2m_client -6 -u coap://[::1]:5683
```

#### 7. Adjusting Registration Lifetime

Set how often the client re-registers with the server (in seconds):

```bash
# Short lifetime (60 seconds) - for testing
./lwm2m_client -l 60

# Long lifetime (1 hour = 3600 seconds) - for production
./lwm2m_client -l 3600

# Very short for debugging (10 seconds)
./lwm2m_client -l 10 -v
```

#### 8. Disable Bootstrap Mode

By default, bootstrap is enabled. To disable:

```bash
# Connect directly without bootstrap
./lwm2m_client -B -u coap://server.example.com:5683
```

#### 9. Verbose Debug Output

Increase verbosity for troubleshooting:

```bash
# Level 1 verbosity
./lwm2m_client -v

# Level 2 verbosity (more detailed)
./lwm2m_client -vv

# Maximum verbosity (level 3)
./lwm2m_client -vvv
```

This shows:
- Configuration summary at startup
- Connection status
- Resource updates
- CoAP message details

#### 10. Quiet Mode

Suppress all non-error output:

```bash
./lwm2m_client -q
```

Useful for:
- Production deployments
- Running as a service
- Log file management

#### 11. Running as a Daemon

Run the client in background (daemon mode):

```bash
./lwm2m_client -d

# With custom configuration
./lwm2m_client -d -n production-gateway -u coaps://mgmt.example.com:5684
```

### Complete Configuration Examples

#### Example 1: Development Testing

```bash
# Local Leshan server, verbose output, short lifetime
./lwm2m_client \
  -n dev-test-device \
  -u coap://localhost:5683 \
  -l 30 \
  -B \
  -vv
```

#### Example 2: Production Deployment with PSK

```bash
# Secure production configuration
./lwm2m_client \
  --name=prod-gateway-001 \
  --uri=coaps://mgmt.company.com:5684 \
  --port=56830 \
  --lifetime=3600 \
  --security=psk \
  --psk-identity=gateway-001 \
  --psk-key=00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff \
  --daemon \
  --quiet
```

#### Example 3: IPv6 Testing Environment

```bash
# IPv6 with bootstrap and verbose logging
./lwm2m_client \
  -6 \
  -n ipv6-test-client \
  -u coap://[2001:db8:1::10]:5683 \
  -p 12345 \
  -l 120 \
  -vvv
```

#### Example 4: Multiple Clients on Same Host

```bash
# Terminal 1
./lwm2m_client -n client-alpha -p 56831 -u coap://server:5683

# Terminal 2
./lwm2m_client -n client-beta -p 56832 -u coap://server:5683

# Terminal 3
./lwm2m_client -n client-gamma -p 56833 -u coap://server:5683
```

#### Example 5: Certificate-Based Security

```bash
# X.509 certificate authentication
./lwm2m_client \
  --name=secure-device \
  --uri=coaps://enterprise-server.com:5684 \
  --security=cert \
  --cert=/etc/lwm2m/device-cert.pem \
  --key=/etc/lwm2m/device-key.pem \
  --ca=/etc/lwm2m/ca-cert.pem \
  --lifetime=7200
```

### Tips for Command-Line Usage

1. **Use Long Options for Scripts**

   Long options are more readable in scripts:
   ```bash
   #!/bin/bash
   ./lwm2m_client \
     --name="${DEVICE_NAME}" \
     --uri="${LWM2M_SERVER}" \
     --lifetime=3600 \
     --daemon
   ```

2. **Combine Short Options**

   Multiple single-letter options can be combined:
   ```bash
   # These are equivalent:
   ./lwm2m_client -v -v -v -q
   ./lwm2m_client -vvvq
   ```

3. **View Current Configuration**

   Use verbose mode to see all active settings:
   ```bash
   ./lwm2m_client -v | head -20
   ```

4. **Environment Variables**

   Create a wrapper script for consistent configuration:
   ```bash
   #!/bin/bash
   # lwm2m-wrapper.sh
   export LWM2M_SERVER="${LWM2M_SERVER:-coap://localhost:5683}"
   export LWM2M_NAME="${LWM2M_NAME:-default-device}"

   ./lwm2m_client -n "$LWM2M_NAME" -u "$LWM2M_SERVER" "$@"
   ```

5. **Testing Security Modes**

   Always verify URI scheme matches security mode:
   - `coap://` for `-s none`
   - `coaps://` for `-s psk`, `-s rpk`, `-s cert`

6. **Debugging Connection Issues**

   Use maximum verbosity and disable quiet mode:
   ```bash
   ./lwm2m_client -vvv -u coap://your-server:5683
   ```

### Getting Help

View all available options:

```bash
./lwm2m_client --help
```

Check version information:

```bash
./lwm2m_client --version
```

---

## 🌐 OpenWRT Build

### For OpenWRT One (Recommended)

```bash
# Set up environment
export STAGING_DIR=/path/to/openwrt-sdk/staging_dir
export PATH=$STAGING_DIR/toolchain-*/bin:$PATH

# Create build directory
mkdir build-openwrt
cd build-openwrt

# Configure for OpenWRT with Location support
cmake \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-openwrt.cmake \
    -DOPENWRT_BUILD=ON \
    -DCMAKE_BUILD_TYPE=Release \
    ..

# Build
make -j$(nproc)

# Package will be in: build-openwrt/examples/lwm2m_client
```

### Deploy to OpenWRT Device

```bash
# Copy to OpenWRT device
scp lwm2m_client root@192.168.1.1:/usr/bin/

# SSH into device
ssh root@192.168.1.1

# Run on OpenWRT
/usr/bin/lwm2m_client
```

---

## 📍 Location Object Quick Setup

### Enable Location Support

1. **Edit Configuration**

   File: `wpp/configs/wpp_config.cmake`

   ```cmake
   # Enable Location object
   set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_6_LOCATION)
   ```

2. **Rebuild**

   ```bash
   cd build
   cmake ..
   make
   ```

### Configure GPS Source

#### Option 1: Starlink Terminal (Automatic)

If you have a Starlink dish at 192.168.100.1, location data will be automatically retrieved via gRPC.

**Enable Starlink Object:**
```cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10512_STARLINK_TERMINAL)
```

#### Option 2: gpsd Daemon

```bash
# On OpenWRT, install gpsd
opkg update
opkg install gpsd gpsd-clients

# Configure gpsd
uci set gpsd.core.enabled='1'
uci set gpsd.core.device='/dev/ttyUSB0'
uci commit gpsd
/etc/init.d/gpsd start
```

#### Option 3: Manual Configuration (UCI)

```bash
# On OpenWRT, set manual location
uci set lwm2m.location=location
uci set lwm2m.location.latitude='40.7128'
uci set lwm2m.location.longitude='-74.0060'
uci set lwm2m.location.altitude='10'
uci commit lwm2m
```

---

## 🛰️ Starlink Terminal Quick Setup

### Enable Starlink Terminal Support

1. **Edit Configuration**

   File: `wpp/configs/wpp_config.cmake`

   ```cmake
   # Enable Starlink Terminal object
   set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10512_STARLINK_TERMINAL)
   ```

2. **Rebuild**

   ```bash
   cd build
   cmake ..
   make
   ```

### Configure Starlink Connection

#### Automatic Configuration (Default)

The Starlink Terminal object connects to:
- **Dish gRPC endpoint:** 192.168.100.1:9200 (default)
- **Router gRPC endpoint:** 192.168.1.1:9000 (optional)

No configuration needed if using standard Starlink network layout.

#### Custom gRPC Endpoint

If your Starlink dish is at a different IP:

```cpp
// In examples/objects.cpp - starlinkTerminalInit()
Instance *starlink = StarlinkTerminal::createInst(client);

// Set custom gRPC endpoint
starlink->set<STRING_T>(StarlinkTerminal::GRPC_ENDPOINT_107, "192.168.50.1:9200");
```

### Verify Starlink Integration

```bash
# Run the client
./lwm2m_client

# You should see:
# ---- Initialization wpp Starlink Terminal ----
# StarlinkTerminal instance created: 0
# Connected to Starlink at 192.168.100.1:9200
```

### Test Starlink Resources

The Starlink Terminal object provides 70+ resources organized into:

#### Network Performance (Resources 10-18)
- Downlink/Uplink throughput
- Latency and packet loss
- SNR and signal quality
- Data usage counters

#### Obstruction Monitoring (Resources 30-35)
- Sky view obstruction percentage
- 12-wedge analysis
- Obstruction duration/interval statistics

#### Dish Status (Resources 40-43)
- Azimuth and elevation angles
- Stowed state (read/write)
- Alignment status

#### GPS Location (Resources 80-85)
- Latitude, longitude, altitude
- GPS satellite count
- GPS ready status

#### Control Actions (Resources 100-107)
- Execute reboot
- Run speed test
- Get obstruction map
- Refresh telemetry

### Quick Starlink Commands

```bash
# Monitor Starlink connection state
# Resource 3: CONNECTION_STATE_3
# Values: CONNECTED, SEARCHING, BOOTING, STOWED, etc.

# Check obstruction status
# Resource 30: FRACTION_OBSTRUCTED_30 (0.0 = clear, 1.0 = fully blocked)
# Resource 31: CURRENTLY_OBSTRUCTED_31 (true/false)

# View dish alignment
# Resource 40: DIRECTION_AZIMUTH_40 (0-360 degrees)
# Resource 41: DIRECTION_ELEVATION_41 (0-90 degrees)

# Monitor power consumption
# Resource 90: LATEST_POWER_90 (watts)
# Resource 91: MEAN_POWER_91 (watts)
```

### Integration with Location Object

When both Location (ID 6) and Starlink Terminal (ID 10512) are enabled, the Location object automatically uses Starlink GPS as its primary source:

```
GPS Source Priority:
1. Starlink Terminal GPS (Resources 83-85)  ← Highest priority
2. gpsd daemon
3. UCI manual configuration
```

This provides seamless location tracking for mobile Starlink installations.

---

## 🔧 Walt Technologies Objects Quick Setup

The Friendly LwM2M Client includes **9 custom Walt Technologies objects** (IDs 10512-10520) for comprehensive OpenWRT router and peripheral management.

### Available Walt Technologies Objects

| Object ID | Name | Purpose | Instances |
|-----------|------|---------|-----------|
| 10512 | Starlink Terminal | Satellite terminal management | Single |
| 10513 | Router Management | Core router configuration | Single |
| 10514 | Ethernet Interface | Ethernet port monitoring | Multiple |
| 10515 | GPIO Control | GPIO/LED/Button control | Multiple |
| 10516 | USB Management | USB port management | Multiple |
| 10517 | Storage Management | Storage device management | Multiple |
| 10518 | System Monitor | System health monitoring | Single |
| 10519 | Hardware Watchdog | Watchdog timer management | Single |
| 10520 | MIKROBUS | MIKROBUS socket and Click boards | Multiple |

### Enable System Monitor

**Purpose:** Monitor CPU, RAM, load, uptime, and temperature

File: `wpp/configs/wpp_config.cmake`

```cmake
# Enable System Monitor object
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10518_SYSTEM_MONITOR)
```

**What you get:**
- CPU usage percentage and frequency
- CPU temperature monitoring
- RAM statistics (total, used, free, cached)
- System load averages (1, 5, 15 minutes)
- Uptime and process count

### Enable MIKROBUS Object

**Purpose:** Manage MIKROBUS sockets and MikroElektronika Click boards

File: `wpp/configs/wpp_config.cmake`

```cmake
# Enable MIKROBUS object
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10520_MIKROBUS)
```

**What you get:**
- 50+ resources for socket management
- Automatic Click board detection via I2C EEPROM
- Interface configuration (SPI, I2C, UART)
- GPIO control (AN, PWM, INT, RST, CS pins)
- Power management (3.3V/5V selection)
- Click board manifest reading
- Multiple instance support (one per socket)

**Example Usage:**

```cpp
// In examples/objects.cpp
#ifdef OBJ_O_10520_MIKROBUS
void mikrobusInit(WppClient &client) {
    client.registry().registerObj(Mikrobus::object(client));

    // Create instance for MIKROBUS socket 1
    Instance *mikrobus1 = Mikrobus::createInst(client, 0);
    mikrobus1->set<STRING_T>(Mikrobus::SOCKET_NAME_1, "MIKROBUS-1");

    // Configure for I2C
    mikrobus1->set<INT_T>(Mikrobus::ACTIVE_INTERFACE_20, 2);  // I2C
    mikrobus1->set<INT_T>(Mikrobus::I2C_ADDRESS_21, 0x48);

    // Enable power at 3.3V
    mikrobus1->set<INT_T>(Mikrobus::POWER_VOLTAGE_5, 3300);
    mikrobus1->set<BOOL_T>(Mikrobus::POWER_STATE_6, true);
}
#endif
```

### Enable Other Walt Objects

**Router Management (10513):** LAN/WAN configuration, DHCP, DNS, firewall
```cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10513_ROUTER_MANAGEMENT)
```

**Ethernet Interface (10514):** Monitor Ethernet ports, link status, traffic stats
```cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10514_ETHERNET_INTERFACE)
```

**GPIO Control (10515):** LED control, button monitoring
```cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10515_GPIO_CONTROL)
```

**USB Management (10516):** USB port control and device detection
```cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10516_USB_MANAGEMENT)
```

**Storage Management (10517):** NAND/NVMe/USB/SD card management
```cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10517_STORAGE_MANAGEMENT)
```

**Hardware Watchdog (10519):** Watchdog timer configuration
```cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_10519_HARDWARE_WATCHDOG)
```

### Rebuild After Enabling Objects

```bash
cd build
cmake ..
make -j$(nproc)
```

---

## 📡 WLAN Connectivity Quick Setup

**Purpose:** Manage WiFi interfaces (2.4GHz and 5GHz) with comprehensive monitoring and configuration.

### Enable WLAN Connectivity

File: `wpp/configs/wpp_config.cmake`

```cmake
# Enable WLAN Connectivity object (ID 12)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_12_WLAN_CONNECTIVITY)
```

### What You Get

The WLAN Connectivity object provides:
- **Multi-instance support:** Instance 0 = wlan0 (2.4GHz), Instance 1 = wlan1 (5GHz)
- **48 resources** covering:
  - Interface control (enable/disable, radio state)
  - WiFi configuration (SSID, channel, band, standard)
  - Security (WPA/WPA2, WEP, RADIUS authentication)
  - Statistics (bytes sent/received, packet counters, errors)
  - WiFi standards: 802.11a/b/g/n/ac/ax (WiFi 6)
  - Operating modes: Access Point, Client, Bridge, Repeater

### Quick Configuration Examples

#### Example 1: Configure 2.4GHz Access Point

```cpp
// In examples/objects.cpp - wlanConnectivityInit()
Instance *wlan0 = WlanConnectivity::instance(client, 0);

// Enable interface
wlan0->set<BOOL_T>(WlanConnectivity::ENABLE_1, true);
wlan0->set<BOOL_T>(WlanConnectivity::RADIO_ENABLED_2, true);

// Configure as Access Point
wlan0->set<INT_T>(WlanConnectivity::MODE_8, WlanConnectivity::ACCESS_POINT);
wlan0->set<STRING_T>(WlanConnectivity::SSID_5, "MyNetwork-2.4GHz");
wlan0->set<INT_T>(WlanConnectivity::CHANNEL_9, 6);

// Security: WPA2-PSK
wlan0->set<INT_T>(WlanConnectivity::AUTHENTICATION_MODE_15, WlanConnectivity::AUTH_PSK);
wlan0->set<INT_T>(WlanConnectivity::ENCRYPTION_MODE_16, WlanConnectivity::ENC_AES);
wlan0->set<STRING_T>(WlanConnectivity::WPA_PRE_SHARED_KEY_17, "YourSecurePassword");
```

#### Example 2: Configure 5GHz Access Point

```cpp
Instance *wlan1 = WlanConnectivity::instance(client, 1);

// Enable interface
wlan1->set<BOOL_T>(WlanConnectivity::ENABLE_1, true);
wlan1->set<BOOL_T>(WlanConnectivity::RADIO_ENABLED_2, true);

// Configure as Access Point (5GHz, 802.11ac)
wlan1->set<INT_T>(WlanConnectivity::MODE_8, WlanConnectivity::ACCESS_POINT);
wlan1->set<STRING_T>(WlanConnectivity::SSID_5, "MyNetwork-5GHz");
wlan1->set<INT_T>(WlanConnectivity::CHANNEL_9, 36);
wlan1->set<INT_T>(WlanConnectivity::STANDARD_14, WlanConnectivity::IEEE_802_11AC);

// Security
wlan1->set<INT_T>(WlanConnectivity::AUTHENTICATION_MODE_15, WlanConnectivity::AUTH_PSK);
wlan1->set<INT_T>(WlanConnectivity::ENCRYPTION_MODE_16, WlanConnectivity::ENC_AES);
wlan1->set<STRING_T>(WlanConnectivity::WPA_PRE_SHARED_KEY_17, "YourSecurePassword");
```

### OpenWRT Integration

The object automatically loads configuration from UCI on OpenWRT:

```bash
# View WiFi configuration
uci show wireless

# The client will read from:
# - wireless.@wifi-iface[0] for wlan0 (2.4GHz)
# - wireless.@wifi-iface[1] for wlan1 (5GHz)
```

Statistics are updated automatically every 30 seconds from `/sys/class/net/wlan*/statistics/`

### Monitor WiFi Statistics

```cpp
// Read statistics
INT_T bytesSent = wlan0->get<INT_T>(WlanConnectivity::TOTAL_BYTES_SENT_33);
INT_T bytesReceived = wlan0->get<INT_T>(WlanConnectivity::TOTAL_BYTES_RECEIVED_34);
INT_T errors = wlan0->get<INT_T>(WlanConnectivity::TRANSMIT_ERRORS_37);

// Check interface status
INT_T status = wlan0->get<INT_T>(WlanConnectivity::STATUS_3);
// 0 = DISABLED, 1 = UP, 2 = ERROR
```

---

## 🔀 Bearer Selection Quick Setup

**Purpose:** Manage network bearer preferences and enable automatic bearer switching based on availability and signal strength.

### Enable Bearer Selection

File: `wpp/configs/wpp_config.cmake`

```cmake
# Enable Bearer Selection object (ID 13)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_13_BEARER_SELECTION)
```

### What You Get

The Bearer Selection object provides:
- **Single instance** for managing all network bearers
- **12 resources** covering:
  - Bearer preference list (WiFi, Ethernet, LTE, WCDMA, GSM, etc.)
  - Signal strength thresholds for GSM/UMTS/LTE/WiFi
  - Operator management (PLMN whitelist/blacklist)
  - Automatic bearer switching with hysteresis
  - Available bearer detection

### Supported Network Bearers

| Bearer | ID | Description |
|--------|----|----- |
| GSM | 0 | 2G GSM/GPRS/EDGE |
| WCDMA | 2 | 3G UMTS/HSPA |
| LTE FDD | 6 | 4G LTE |
| LTE-M | 7 | LTE Machine Type |
| NB-IoT | 8 | Narrowband IoT |
| WiFi | 21 | 802.11 WiFi |
| Ethernet | 41 | Wired Ethernet |
| DSL | 42 | Digital Subscriber Line |

### Quick Configuration Examples

#### Example 1: WiFi Preferred, Cellular Fallback

```cpp
Instance *bearer = BearerSelection::instance(client);

// Prefer WiFi, then Ethernet, then LTE
bearer->set<STRING_T>(BearerSelection::PREFERRED_COMMS_BEARER_0, "21,41,6");

// Set minimum WiFi signal strength (-70 dBm)
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSSI_WLAN_4, -70);

// Set minimum LTE signal strength (-95 dBm)
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSRP_LTE_3, -95);

// Set hysteresis to prevent frequent switching (5 dB)
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_SIGNAL_STRENGTH_VAR_9, 5);
```

#### Example 2: Ethernet Only (Fixed Installation)

```cpp
Instance *bearer = BearerSelection::instance(client);

// Use only Ethernet
bearer->set<STRING_T>(BearerSelection::PREFERRED_COMMS_BEARER_0, "41");

// Set very high thresholds for other bearers to effectively disable them
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSSI_WLAN_4, -30);
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSRP_LTE_3, -50);
```

#### Example 3: Operator Whitelist (Roaming Control)

```cpp
Instance *bearer = BearerSelection::instance(client);

// Only connect to specific operators (AT&T and T-Mobile US)
bearer->set<STRING_T>(BearerSelection::OPERATOR_LIST_6, "310-410,310-260");
bearer->set<INT_T>(BearerSelection::OPERATOR_LIST_MODE_7, BearerSelection::WHITELIST);
```

#### Example 4: LTE Preferred with Automatic Fallback

```cpp
Instance *bearer = BearerSelection::instance(client);

// Prefer LTE, then WCDMA, then GSM
bearer->set<STRING_T>(BearerSelection::PREFERRED_COMMS_BEARER_0, "6,2,0");

// Signal thresholds
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSRP_LTE_3, -110);  // Aggressive
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSCP_UMTS_2, -95);  // Conservative
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSSI_GSM_1, -75);

// Hysteresis to prevent ping-pong
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_SIGNAL_STRENGTH_VAR_9, 8);
```

### OpenWRT Integration

The object automatically detects available bearers on OpenWRT:

```bash
# Configure bearer preferences via UCI
uci set network.bearer=bearer
uci set network.bearer.preference='21,41,6'
uci set network.bearer.wlan_rssi='-70'
uci set network.bearer.lte_rsrp='-95'
uci commit network
```

### Monitor Available Bearers

```cpp
// Check which bearers are currently available
STRING_T available = bearer->get<STRING_T>(BearerSelection::AVAILABLE_NETWORK_BEARERS_8);
// Example: "21,41,6" means WiFi, Ethernet, and LTE are available
```

### Bearer Selection Logic

The device automatically selects the best bearer using this algorithm:

1. Check bearer preference list in order
2. Verify bearer is available
3. Check signal strength meets minimum threshold
4. Verify operator is allowed (whitelist/blacklist)
5. Apply hysteresis to prevent frequent switching
6. Select highest priority bearer that meets all criteria

### Hysteresis Prevents Ping-Pong

Hysteresis prevents frequent bearer switching when signal strengths are similar:

```
Current: LTE at -95 dBm
Hysteresis: 5 dB
WiFi Available: -85 dBm

Decision: Only switch to WiFi if it's 5 dB better than LTE
Result: WiFi signal is better, so switch
```

---

## ✅ Verify Installation

### Check Objects Are Running

```bash
# Run the client
./lwm2m_client

# You should see initialization messages for:
# - Device
# - Server
# - Security
# - WLAN Connectivity (if enabled)
# - Bearer Selection (if enabled)
# - Location (if enabled)
# - Starlink Terminal (if enabled)
# - MIKROBUS (if enabled)
# - System Monitor (if enabled)
# - Connectivity Monitoring
# - Other Walt Technologies objects (if enabled)
```

### Test Location Data

```bash
# The Location object will log:
# Location initialized from OpenWRT: lat=X, lon=Y, alt=Z
# OR
# No location source available, using defaults
```

### Connect to LwM2M Server

```bash
# If configured correctly, you should see:
# [INFO] Registration successful
# [INFO] Server responded with 2.01 Created
```

---

## 🚦 Common Scenarios

### Scenario 1: Development Testing (No Server)

```bash
# Build and run locally
mkdir build && cd build
cmake ..
make
./examples/lwm2m_client

# Client will run but won't connect (no server configured)
# Good for testing object initialization
```

### Scenario 2: Connect to Leshan Demo Server

```bash
# Use public Leshan server for testing
# Edit examples/objects.cpp:

security->set<STRING_T>(Lwm2mSecurity::SERVER_URI_0,
                        "coap://leshan.eclipseprojects.io:5683");
security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, 3);  // NoSec

# Rebuild and run
make
./examples/lwm2m_client

# View your device at: https://leshan.eclipseprojects.io/
# Look for your device's endpoint name
```

### Scenario 3: Production OpenWRT Deployment

```bash
# Cross-compile for OpenWRT
cmake -DOPENWRT_BUILD=ON -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-openwrt.cmake ..
make

# Create IPK package
./scripts/create-ipk.sh

# Install on OpenWRT
opkg install lwm2m-client_*.ipk

# Configure via UCI
uci set lwm2m.server.uri='coaps://your-server.com:5684'
uci set lwm2m.server.identity='device-001'
uci commit lwm2m

# Start service
/etc/init.d/lwm2m start
```

---

## 🔍 Next Steps

### For Developers

1. **Explore the codebase**
   - Review `examples/main.cpp` for initialization flow
   - Check `wpp/registry/objects/` for object implementations
   - Read `docs/ARCHITECTURE.md` for system design

2. **Create custom objects**
   - See `docs/IMPLEMENTATION_GUIDE.md`
   - Use the object_maker utility
   - Follow existing object patterns

3. **Test integration**
   - Set up a local LwM2M server (Leshan)
   - Monitor object resources
   - Test firmware updates (FOTA)

### For Operators

1. **Deploy to production**
   - Follow `docs/OPENWRT_INTEGRATION.md`
   - Configure monitoring
   - Set up automatic updates

2. **Monitor devices**
   - Use LwM2M server dashboard
   - Set up alerts for device health
   - Monitor location updates

3. **Troubleshooting**
   - Check `docs/TROUBLESHOOTING.md`
   - Enable debug logging
   - Review system logs

---

## 📚 Additional Resources

### Core Documentation
- **[Implementation Guide](IMPLEMENTATION_GUIDE.md)** - Detailed build instructions
- **[OpenWRT Integration](OPENWRT_INTEGRATION.md)** - OpenWRT-specific features
- **[API Reference](API_REFERENCE.md)** - Code API documentation
- **[Troubleshooting](TROUBLESHOOTING.md)** - Common issues and solutions

### Object Documentation
- **[WLAN Connectivity (ID 12)](WLAN_CONNECTIVITY.md)** - WiFi interface management (2.4GHz + 5GHz)
- **[Bearer Selection (ID 13)](BEARER_SELECTION.md)** - Network bearer preference and automatic selection
- **[Location Object (ID 6)](LOCATION_OBJECT.md)** - GPS/Location features
- **[Starlink Terminal (ID 10512)](STARLINK_TERMINAL.md)** - Starlink satellite terminal management
- **[MIKROBUS Object (ID 10520)](MIKROBUS_OBJECT.md)** - MIKROBUS socket and Click board management

---

## 💡 Pro Tips

1. **Use Debug Logging**
   ```cmake
   # In wpp_config.cmake
   set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=0)  # 0=Debug
   ```

2. **Quick Rebuild**
   ```bash
   # Only rebuild changed files
   make -j$(nproc)

   # Force rebuild of specific target
   make clean && make lwm2m_client
   ```

3. **Test Without Hardware**
   ```bash
   # Build without OPENWRT_BUILD flag
   cmake -DOPENWRT_BUILD=OFF ..
   make
   # Location will use default values
   ```

4. **Monitor Network Traffic**
   ```bash
   # On OpenWRT, capture LwM2M packets
   tcpdump -i any -w lwm2m.pcap port 5683
   ```

---

**You're all set!** 🎉

Your LwM2M client is now ready. Proceed to the [Implementation Guide](IMPLEMENTATION_GUIDE.md) for advanced configuration and custom development.
