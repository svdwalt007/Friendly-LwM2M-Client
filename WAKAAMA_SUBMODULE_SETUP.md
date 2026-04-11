# Wakaama Submodule Setup

## Issue

The following LwM2M objects require the Wakaama submodule to be initialized:

- **Object 10525**: WAN Failover Policy
- **Object 10526**: Multi-WAN Health Check
- **Object 10527**: WiFi Client Management
- **Object 10528**: WiFi Channel Optimization
- **Object 10535**: LAN Configuration
- **Object 10536**: Routing Table
- **Object 10537**: VPN Configuration

These objects are currently **DISABLED by default** in CMakeLists.txt because the Wakaama submodule is not initialized.

## Solution

To enable these objects, initialize the Wakaama submodule:

### If you just cloned the repository:
```bash
# Clone with submodules from scratch
git clone --recurse-submodules git@github.com:Friendly-Technologies/Friendly-LwM2M-Client.git
```

### If you already have the repository:
```bash
# Initialize the Wakaama submodule
git submodule update --init --recursive

# Verify the submodule is initialized
ls 2305-Wakaama/
```

### Enable the objects in CMake:

Edit `CMakeLists.txt` or pass options to cmake:

```bash
cd build
cmake .. \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  -DWITH_LAN_CONFIG=ON \
  -DWITH_ROUTING_TABLE=ON \
  -DWITH_VPN_CONFIG=ON

make -j$(nproc)
```

## Current Status

✅ Core LwM2M client compiles without Wakaama submodule
✅ Objects 10512-10522 work without submodule
❌ Objects 10525-10537 require Wakaama submodule (currently disabled)

To use these advanced router management objects, initialize the submodule as shown above.
