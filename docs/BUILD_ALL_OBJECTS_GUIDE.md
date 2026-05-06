# Build Configuration for All LwM2M Objects

## Complete Build Command

To build the Friendly LwM2M Client with **ALL** available LwM2M objects (mandatory, optional, IPSO, and Walt Technologies custom objects), use the following CMake configuration:

### Clean Build Setup

```bash
# Clean previous build
rm -rf build
mkdir build
cd build

# Configure with all objects enabled
cmake .. \
  -DBUILD_ENV=DEV \
  -DBUILD_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  \
  -DWITH_DTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_OPENSSL=OFF \
  -DDTLS_PROVIDER_TINYDTLS=OFF \
  -DDTLS_PROVIDER_WOLFSSL=OFF \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  \
  -DWITH_DELTA_FOTA=ON \
  -DWITH_BSDIFF=ON \
  -DWITH_VCDIFF=ON \
  -DWITH_COURGETTE=ON \
  -DWITH_COMPRESSION=ON \
  -DWITH_ROLLBACK=ON \
  -DWITH_MQTT=ON \
  -DWITH_EDGE_AI=OFF \
  \
  -DWITH_OPENWRT_EXTENDED=ON \
  -DWITH_LUCI_MODULE=ON \
  -DWITH_HOTPLUG_HANDLERS=ON \
  \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON \
  \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  \
  -DWITH_STARLINK=OFF \
  -DWITH_STARLINK_GRPC=OFF \
  \
  -DWITH_MATTER_BRIDGE=ON \
  -DWITH_THREAD_BR=OFF \
  \
  -DWITH_ZIGBEE=ON \
  -DWITH_ZIGBEE_EZSP=OFF \
  -DWITH_ZIGBEE_ZSTACK=OFF

# Build
cmake --build . -j$(nproc)
```

### Single-Line Build Command (for scripting)

```bash
cmake -B build -DBUILD_ENV=DEV -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON -DWITH_DTLS=ON -DDTLS_PROVIDER_MBEDTLS=ON -DDTLS_DEFAULT_PROVIDER=mbedtls -DWITH_DELTA_FOTA=ON -DWITH_BSDIFF=ON -DWITH_VCDIFF=ON -DWITH_COURGETTE=ON -DWITH_COMPRESSION=ON -DWITH_ROLLBACK=ON -DWITH_MQTT=ON -DWITH_OPENWRT_EXTENDED=ON -DWITH_LUCI_MODULE=ON -DWITH_HOTPLUG_HANDLERS=ON -DWITH_LAN_CONFIG=ON -DWITH_ROUTING_TABLE=ON -DWITH_VPN_CONFIG=ON -DWITH_WAN_FAILOVER=ON -DWITH_MULTIWAN_HEALTH=ON -DWITH_WIFI_CLIENT_MGMT=ON -DWITH_WIFI_CHANNEL_OPT=ON -DWITH_MATTER_BRIDGE=ON -DWITH_ZIGBEE=ON && cmake --build build -j$(nproc)
```

## Object Breakdown

### Mandatory Objects (Always Included)
- **Object 0**: LwM2M Security
- **Object 1**: LwM2M Server
- **Object 3**: Device

### Standard Optional Objects
- **Object 2**: LwM2M Access Control
- **Object 4**: Connectivity Monitoring
- **Object 5**: Firmware Update
- **Object 6**: Location
- **Object 12**: WLAN Connectivity (disabled - incompatible)
- **Object 13**: Bearer Selection (disabled - incompatible)
- **Object 3339**: Audio Clip (IPSO)
- **Object 3331**: Memory (IPSO)

### Walt Technologies Custom Objects (10512-10537)

#### Always Available (no CMake flag required):
- **10512**: Starlink Terminal
- **10513**: Router Management
- **10514**: Ethernet Interface
- **10515**: GPIO Control
- **10516**: USB Management
- **10517**: Storage Management
- **10518**: System Monitor
- **10519**: Hardware Watchdog
- **10520**: Mikrobus
- **10521**: Firewall Configuration
- **10522**: PoE Management

#### Conditionally Enabled:
- **10525**: WAN Failover Policy (`-DWITH_WAN_FAILOVER=ON`)
- **10526**: Multi-WAN Health Check (`-DWITH_MULTIWAN_HEALTH=ON`)
- **10527**: WiFi Client Management (`-DWITH_WIFI_CLIENT_MGMT=ON`)
- **10528**: WiFi Channel Optimization (`-DWITH_WIFI_CHANNEL_OPT=ON`)
- **10529**: Matter Bridge (`-DWITH_MATTER_BRIDGE=ON`)
- **10530**: Matter Device (`-DWITH_MATTER_BRIDGE=ON`)
- **10531**: Thread Network (`-DWITH_MATTER_BRIDGE=ON`)
- **10532**: Zigbee Coordinator (`-DWITH_ZIGBEE=ON`)
- **10533**: Zigbee Device (`-DWITH_ZIGBEE=ON`)
- **10534**: Zigbee Group (`-DWITH_ZIGBEE=ON`)
- **10535**: LAN Configuration (`-DWITH_LAN_CONFIG=ON`)
- **10536**: Routing Table (`-DWITH_ROUTING_TABLE=ON`)
- **10537**: VPN Configuration (`-DWITH_VPN_CONFIG=ON`)

## Build Environments

### TEST Environment (Maximum Features, Debug)
```bash
cmake -B build -DBUILD_ENV=TEST [... all object flags ...]
```
- Full debug symbols
- All sanitizers enabled
- Code coverage enabled
- All DTLS providers enabled
- Logging level: TRACE

### DEV Environment (Development with Optimization)
```bash
cmake -B build -DBUILD_ENV=DEV [... all object flags ...]
```
- Debug symbols + optimization (-O2)
- Address sanitizer enabled
- Stack protection
- Logging level: DEBUG

### PREPROD Environment (Pre-Production Testing)
```bash
cmake -B build -DBUILD_ENV=PREPROD [... all object flags ...]
```
- Optimized build (-O3)
- Static DTLS linking
- Logging level: INFO

### PROD Environment (Production Deployment)
```bash
cmake -B build -DBUILD_ENV=PROD [... all object flags ...]
```
- Maximum optimization (-Os)
- Symbol stripping
- Link-time optimization (LTO)
- Static DTLS linking
- Minimal binary size
- Logging level: ERROR only

## Verification

After building, verify which objects are included:

```bash
# List all compiled object files
find build -name "*.o" | grep -E "objects|Object" | sort

# Check binary symbols
nm -C build/libfriendly_lwm2m.a | grep -i object

# Verify example client objects
./build/examples/friendly_example_client --list-objects
```

## Configuration File

The complete object configuration JSON file is provided in:
- `build/examples/lwm2m_objects_config_complete.json`

Use this file to configure all object instances at runtime.

## Notes

1. **Memory Requirements**: Enabling all objects requires significant RAM. Ensure your target platform has at least 512MB RAM.

2. **Build Time**: Full build with all objects takes ~5-10 minutes depending on CPU cores.

3. **Binary Size**:
   - TEST build: ~15-20 MB
   - DEV build: ~8-12 MB
   - PREPROD build: ~4-6 MB
   - PROD build: ~2-3 MB (with LTO and stripping)

4. **Dependencies**: Some objects require external libraries:
   - Starlink: gRPC, protobuf
   - Matter: Matter SDK
   - Zigbee: EZSP or Z-Stack SDK
   - MQTT: Paho MQTT

5. **Disabled Objects**: The following are currently disabled due to API incompatibilities:
   - Object 12 (WLAN Connectivity)
   - Object 13 (Bearer Selection)
   - These will be re-enabled in future versions with updated implementations.

## Troubleshooting

### Missing Dependencies
```bash
# Install required packages (Ubuntu/Debian)
sudo apt-get install -y \
  libmbedtls-dev \
  libssl-dev \
  libcurl4-openssl-dev \
  zlib1g-dev \
  libbz2-dev \
  liblzma-dev \
  libgtest-dev

# For MQTT support
sudo apt-get install -y libpaho-mqtt-dev libpaho-mqttpp-dev
```

### Build Failures
If build fails with specific object errors, you can selectively disable objects:
```bash
cmake -B build -DWITH_ZIGBEE=OFF -DWITH_MATTER_BRIDGE=OFF [...]
```

### Link Errors
Ensure all DTLS libraries are properly linked:
```bash
ldconfig -p | grep -E "mbedtls|ssl|crypto"
```
