# Build Instructions - CORRECTED

This guide provides the correct build instructions for the WppExample executable, with all issues fixed.

## Issues That Were Fixed

1. **Missing cmake_minimum_required** - Added to examples/CMakeLists.txt
2. **Windows line endings** - Fixed libcoap_build.sh script
3. **WPP object CMakeLists errors** - Fixed 6 object CMakeLists.txt files to use proper WPP pattern
4. **Missing autotools** - Need to install autoconf/automake/libtool

## Prerequisites

### Install Dependencies (Ubuntu/WSL)

Run the provided installation script:

```bash
# From project root
bash install_build_deps.sh
```

Or manually install:

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git pkg-config
sudo apt-get install -y autoconf automake libtool
sudo apt-get install -y libssl-dev libcurl4-openssl-dev zlib1g-dev
```

## Build Steps

### 1. Initialize Wakaama Submodule

The WppExample requires the Wakaama submodule for LwM2M core functionality:

```bash
# From project root
git submodule update --init --recursive
```

Verify the submodule is initialized:
```bash
ls 2305-Wakaama/
# Should show files, not an empty directory
```

### 2. Build WppExample

```bash
# Create build directory
mkdir -p _build
cd _build

# Configure CMake
cmake ../examples -DCMAKE_BUILD_TYPE=MinSizeRel

# Build (use all CPU cores)
make -j$(nproc)
```

### 3. Run WppExample

```bash
# Run with default settings
./WppExample

# Or run with custom server
./WppExample --uri coap://leshan.eclipseprojects.io:5683 --name my-device-001
```

## Build Outputs

The build will create:
- `WppExample` - Main executable
- `WppExample.bin` - Binary firmware image
- `WppExample.hex` - Intel HEX firmware image
- `output.map` - Linker map file

## Testing with Leshan Demo Server

You can test your client with the public Leshan demo server:

```bash
# Start the client
./WppExample --uri coap://leshan.eclipseprojects.io:5683 --name test-device-$(date +%s)

# Then visit in your browser:
# https://leshan.eclipseprojects.io
```

You should see your device appear in the client list.

## Troubleshooting

### Error: "autoreconf: not found"
**Solution**: Install autotools:
```bash
sudo apt-get install autoconf automake libtool
```

### Error: "Wakaama submodule not found"
**Solution**: Initialize the submodule:
```bash
git submodule update --init --recursive
```

### Error: "libcoap build failed"
**Solution**:
1. Check that `examples/coap/libcoap_build.sh` has Unix line endings
2. Run: `dos2unix examples/coap/libcoap_build.sh`
3. Make sure autotools are installed

### Error: "Cannot specify sources for target 'Wpp'"
**Solution**: This has been fixed in the following files:
- `wpp/registry/objects/w_10513_router_management/CMakeLists.txt`
- `wpp/registry/objects/w_10514_ethernet_interface/CMakeLists.txt`
- `wpp/registry/objects/w_10515_gpio_control/CMakeLists.txt`
- `wpp/registry/objects/w_10516_usb_management/CMakeLists.txt`
- `wpp/registry/objects/w_10517_storage_management/CMakeLists.txt`
- `wpp/registry/objects/w_10518_system_monitor/CMakeLists.txt`

If you still see this error, make sure you have the latest code.

## Building with Different Options

### Debug Build
```bash
cmake ../examples -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### Release Build
```bash
cmake ../examples -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Without DTLS (plain CoAP)
```bash
cmake ../examples -DCMAKE_BUILD_TYPE=MinSizeRel -DWITH_UDP_DTLS=OFF
make -j$(nproc)
```

### Using OpenWRT Firmware Updater
```bash
cmake ../examples -DCMAKE_BUILD_TYPE=MinSizeRel -DUSE_OPENWRT_FW_UPDATER=ON
make -j$(nproc)
```

## Clean Build

If you need to start from scratch:

```bash
cd _build
rm -rf *
cmake ../examples -DCMAKE_BUILD_TYPE=MinSizeRel
make -j$(nproc)
```

## Alternative: Build Just the Library

If you only want to build the friendly_lwm2m library (not the full WppExample):

```bash
# From project root
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# This creates:
# - libfriendly_lwm2m.a
# - test_* executables
```

## Environment Note

**IMPORTANT**: You must build in WSL/Linux, not in Git Bash/MINGW64. The project uses Linux-specific build tools and libraries.

If you're on Windows:
1. Install WSL2 with Ubuntu
2. Navigate to project: `cd /mnt/d/Dev/Friendly-LwM2M-Client`
3. Follow the build steps above

## Command Summary

Quick reference for a full build from scratch:

```bash
# In WSL/Linux terminal
cd /mnt/d/Dev/Friendly-LwM2M-Client

# Install dependencies (first time only)
bash install_build_deps.sh

# Initialize submodule (first time only)
git submodule update --init --recursive

# Build
mkdir -p _build && cd _build
cmake ../examples -DCMAKE_BUILD_TYPE=MinSizeRel
make -j$(nproc)

# Run
./WppExample --uri coap://leshan.eclipseprojects.io:5683
```
