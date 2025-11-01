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
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_34600_STARLINK_TERMINAL)
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

## ✅ Verify Installation

### Check Objects Are Running

```bash
# Run the client
./lwm2m_client

# You should see initialization messages for:
# - Device
# - Server
# - Security
# - Location (if enabled)
# - Connectivity Monitoring
# - Other enabled objects
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

- **[Implementation Guide](IMPLEMENTATION_GUIDE.md)** - Detailed build instructions
- **[Location Object Documentation](LOCATION_OBJECT.md)** - GPS/Location features
- **[OpenWRT Integration](OPENWRT_INTEGRATION.md)** - OpenWRT-specific features
- **[API Reference](API_REFERENCE.md)** - Code API documentation
- **[Troubleshooting](TROUBLESHOOTING.md)** - Common issues and solutions

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
