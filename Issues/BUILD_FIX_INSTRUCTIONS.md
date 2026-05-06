# Build Fix Instructions

## Problem

The build failed with two types of errors:

1. **Missing WppLogs.h** - Objects 10525-10537 (WAN/WiFi/LAN/Routing/VPN) couldn't find headers
2. **Missing TinyDTLS files** - Wakaama submodule TinyDTLS sources not found

## Root Cause

The CMake cache from a previous build had these objects **enabled** (ON), but:
- The WppLogs.h include path wasn't properly configured
- The Wakaama submodule's TinyDTLS files are missing

## Solution

**Disable the optional WPP objects** (10525-10537) that require Wakaama headers, since:
1. They're not needed for basic LwM2M client functionality
2. Your current build configuration doesn't include them
3. They were accidentally left enabled in the CMake cache

## Quick Fix - Run the Rebuild Script

### On Linux/WSL:

```bash
cd /mnt/d/Dev/Friendly-LwM2M-Client
chmod +x rebuild_fixed.sh
./rebuild_fixed.sh
```

### On Windows:

```cmd
cd D:\Dev\Friendly-LwM2M-Client
rebuild_fixed.bat
```

## Manual Fix

If you prefer to run commands manually:

```bash
cd /mnt/d/Dev/Friendly-LwM2M-Client/build

# Step 1: Clean cache
rm -f CMakeCache.txt
rm -rf CMakeFiles/

# Step 2: Reconfigure
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_WAN_FAILOVER=OFF \
    -DWITH_MULTIWAN_HEALTH=OFF \
    -DWITH_WIFI_CLIENT_MGMT=OFF \
    -DWITH_WIFI_CHANNEL_OPT=OFF \
    -DWITH_LAN_CONFIG=OFF \
    -DWITH_ROUTING_TABLE=OFF \
    -DWITH_VPN_CONFIG=OFF

# Step 3: Build
make -j$(nproc)
```

## What Gets Built

After this fix, your build will include:

**✅ Core LwM2M Objects:**
- Object 0: LwM2M Security
- Object 1: LwM2M Server
- Object 2: Access Control (with fixes)
- Object 3: Device
- Object 4: Connectivity Monitoring
- Object 5: Firmware Update
- Object 6: Location

**✅ Fixed Issues:**
- No more duplicate ACK responses
- Proper ACL configuration for server access

**❌ Disabled Objects (not needed for basic functionality):**
- Object 10525: WAN Failover Policy
- Object 10526: Multi-WAN Health Check
- Object 10527: WiFi Client Management
- Object 10528: WiFi Channel Optimization
- Object 10535: LAN Configuration
- Object 10536: Routing Table
- Object 10537: VPN Configuration

## If You Need WPP Objects 10525-10537

If you really need these objects, you must:

1. **Initialize Wakaama submodule properly:**
   ```bash
   git submodule update --init --recursive
   ```

2. **Ensure WppLogs.h path is included:**
   The CMakeLists.txt should already handle this, but verify the include path is set.

3. **Re-enable in CMake:**
   ```bash
   cmake .. \
       -DWITH_WAN_FAILOVER=ON \
       -DWITH_MULTIWAN_HEALTH=ON \
       -DWITH_WIFI_CLIENT_MGMT=ON \
       -DWITH_WIFI_CHANNEL_OPT=ON \
       -DWITH_LAN_CONFIG=ON \
       -DWITH_ROUTING_TABLE=ON \
       -DWITH_VPN_CONFIG=ON
   ```

## After Successful Build

Test the fixes:

```bash
cd build/examples

./friendly_example_client \
    --config ../../examples/lwm2m_minimal_config.json \
    -B \
    -x Svdw007-777 \
    -n svdwat007-777 \
    -s none \
    --disable-cid \
    --dtls-provider=tinydtls \
    -v \
    -u coap://dem-iot.friendly-tech.com:5683
```

Expected results:
- ✅ Client registers successfully
- ✅ Only ONE ACK per server request (no duplicates)
- ✅ No `5.00 Internal Server Error` duplicates
- ✅ No `4.01 Unauthorized` errors
- ✅ Server can read all objects

## Verification

Capture new traffic and compare with `Issues/pcap01.csv`:
```bash
sudo tcpdump -i any -w test_fixed.pcap port 5683
```

Check for:
- Each GET request receives exactly ONE ACK (not two)
- No duplicate `5.00 Internal Server Error` responses
- All GET requests for `/2/*` and `/6/0` return `2.05 Content`

---

**Generated:** 2026-04-15
**Related Files:**
- `Issues/PCAP_ANALYSIS_REPORT.md` - Original issue analysis
- `Issues/FIXES_APPLIED.md` - Detailed fix documentation
- `rebuild_fixed.sh` / `rebuild_fixed.bat` - Rebuild scripts
