# Build Examples

This document provides comprehensive examples for building the Friendly LwM2M Client in various configurations and environments.

## Table of Contents

- [Basic Build Configurations](#basic-build-configurations)
- [Platform-Specific Builds](#platform-specific-builds)
- [Cross-Compilation](#cross-compilation)
- [Feature Configurations](#feature-configurations)
- [Development Builds](#development-builds)
- [Production Builds](#production-builds)
- [Troubleshooting Common Build Issues](#troubleshooting-common-build-issues)

## Basic Build Configurations

### Standard Release Build

Optimized build for production use:

```bash
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

Expected output:
```
[  3%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/...
...
[ 61%] Linking CXX static library libfriendly_lwm2m.a
[ 61%] Built target friendly_lwm2m
[100%] Built target test_blockwise_transfer
```

### Debug Build

Build with debugging symbols and no optimization:

```bash
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

Useful for:
- GDB debugging
- Valgrind analysis
- Step-by-step code execution

### Debug with Sanitizers

Build with AddressSanitizer for memory error detection:

```bash
mkdir build-asan && cd build-asan
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" \
      -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address" ..
make -j$(nproc)
```

Build with ThreadSanitizer for race condition detection:

```bash
mkdir build-tsan && cd build-tsan
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-fsanitize=thread" \
      -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=thread" ..
make -j$(nproc)
```

### MinSizeRel Build

Minimal size optimized build:

```bash
mkdir build-minsizerel && cd build-minsizerel
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
make -j$(nproc)
```

Good for:
- Embedded systems with limited flash
- IoT devices with storage constraints

## Platform-Specific Builds

### Linux (Ubuntu/Debian)

```bash
# Install prerequisites
sudo apt-get update
sudo apt-get install build-essential cmake libssl-dev libcurl4-openssl-dev

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Linux (Fedora/RHEL/CentOS)

```bash
# Install prerequisites
sudo dnf install gcc gcc-c++ cmake openssl-devel libcurl-devel

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### OpenWrt

Build for OpenWrt routers:

```bash
# In OpenWrt SDK
cd package
git clone https://github.com/friendly-tech/lwm2m-client.git friendly-lwm2m

# Create Makefile for OpenWrt
cat > friendly-lwm2m/Makefile <<'EOF'
include $(TOPDIR)/rules.mk

PKG_NAME:=friendly-lwm2m
PKG_VERSION:=1.2.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/friendly-lwm2m
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Friendly LwM2M Client
  DEPENDS:=+libopenssl +libcurl +libstdcpp
endef

define Package/friendly-lwm2m/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lwm2m_client $(1)/usr/bin/
endef

$(eval $(call BuildPackage,friendly-lwm2m))
EOF

# Build package
cd ..
make package/friendly-lwm2m/compile V=s
```

### WSL2 (Windows Subsystem for Linux)

```bash
# Install WSL2 Ubuntu
wsl --install -d Ubuntu-22.04

# Inside WSL
sudo apt-get update
sudo apt-get install build-essential cmake libssl-dev libcurl4-openssl-dev

# Clone and build
git clone https://github.com/friendly-tech/lwm2m-client.git
cd lwm2m-client
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Cross-Compilation

### ARM32 (Raspberry Pi)

```bash
# Install cross-compiler
sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

# Create toolchain file
cat > toolchain-arm32.cmake <<'EOF'
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

# Build
mkdir build-arm32 && cd build-arm32
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm32.cmake \
      -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### ARM64 (aarch64)

```bash
# Install cross-compiler
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Create toolchain file
cat > toolchain-arm64.cmake <<'EOF'
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

# Build
mkdir build-arm64 && cd build-arm64
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm64.cmake \
      -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### MIPS (OpenWrt Routers)

```bash
# Using OpenWrt SDK
export STAGING_DIR=/path/to/openwrt/staging_dir
export PATH=$STAGING_DIR/toolchain-mips_24kc_gcc-11.2.0_musl/bin:$PATH

# Create toolchain file
cat > toolchain-mips.cmake <<'EOF'
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR mips)

set(CMAKE_C_COMPILER mips-openwrt-linux-gcc)
set(CMAKE_CXX_COMPILER mips-openwrt-linux-g++)

set(CMAKE_FIND_ROOT_PATH $ENV{STAGING_DIR})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

# Build
mkdir build-mips && cd build-mips
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-mips.cmake \
      -DCMAKE_BUILD_TYPE=MinSizeRel ..
make -j$(nproc)
```

## Feature Configurations

### All Features Enabled

Enable all optional features (requires submodules):

```bash
# Initialize submodules
git submodule update --init --recursive

# Configure with all features
mkdir build-full && cd build-full
cmake -DCMAKE_BUILD_TYPE=Release \
      -DWITH_WAN_FAILOVER=ON \
      -DWITH_MULTIWAN_HEALTH=ON \
      -DWITH_WIFI_CLIENT_MGMT=ON \
      -DWITH_WIFI_CHANNEL_OPT=ON \
      -DWITH_LAN_CONFIG=ON \
      -DWITH_ROUTING_TABLE=ON \
      -DWITH_VPN_CONFIG=ON \
      -DENABLE_MQTT=ON \
      -DENABLE_GRPC=ON \
      -DENABLE_STARLINK=ON ..
make -j$(nproc)
```

### Minimal Build

Smallest possible build with core features only:

```bash
mkdir build-minimal && cd build-minimal
cmake -DCMAKE_BUILD_TYPE=MinSizeRel \
      -DWITH_WAN_FAILOVER=OFF \
      -DWITH_MULTIWAN_HEALTH=OFF \
      -DWITH_WIFI_CLIENT_MGMT=OFF \
      -DWITH_WIFI_CHANNEL_OPT=OFF \
      -DWITH_LAN_CONFIG=OFF \
      -DWITH_ROUTING_TABLE=OFF \
      -DWITH_VPN_CONFIG=OFF \
      -DENABLE_MQTT=OFF \
      -DENABLE_GRPC=OFF \
      -DENABLE_STARLINK=OFF \
      -DBUILD_TESTING=OFF ..
make -j$(nproc)
```

### Firmware Update Only

Build focused on firmware update capabilities:

```bash
mkdir build-fwupdate && cd build-fwupdate
cmake -DCMAKE_BUILD_TYPE=Release \
      -DENABLE_FIRMWARE_UPDATE=ON \
      -DENABLE_DELTA_UPDATES=ON \
      -DENABLE_ROLLBACK=ON \
      -DWITH_WAN_FAILOVER=OFF \
      -DWITH_MULTIWAN_HEALTH=OFF \
      -DWITH_WIFI_CLIENT_MGMT=OFF \
      -DWITH_WIFI_CHANNEL_OPT=OFF \
      -DWITH_LAN_CONFIG=OFF \
      -DWITH_ROUTING_TABLE=OFF \
      -DWITH_VPN_CONFIG=OFF ..
make -j$(nproc)
```

### MQTT Transport Only

Build with MQTT transport instead of CoAP:

```bash
mkdir build-mqtt && cd build-mqtt
cmake -DCMAKE_BUILD_TYPE=Release \
      -DENABLE_MQTT=ON \
      -DENABLE_COAP=OFF \
      -DWITH_WAN_FAILOVER=OFF \
      -DWITH_MULTIWAN_HEALTH=OFF \
      -DWITH_WIFI_CLIENT_MGMT=OFF \
      -DWITH_WIFI_CHANNEL_OPT=OFF \
      -DWITH_LAN_CONFIG=OFF \
      -DWITH_ROUTING_TABLE=OFF \
      -DWITH_VPN_CONFIG=OFF ..
make -j$(nproc)
```

## Development Builds

### Build with Code Coverage

```bash
mkdir build-coverage && cd build-coverage
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DENABLE_COVERAGE=ON ..
make -j$(nproc)

# Run tests
ctest

# Generate coverage report
make coverage
# Open build-coverage/coverage/index.html in browser
```

### Build with Clang

```bash
mkdir build-clang && cd build-clang
cmake -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### Build with Ninja

Faster builds using Ninja instead of Make:

```bash
mkdir build-ninja && cd build-ninja
cmake -G Ninja \
      -DCMAKE_BUILD_TYPE=Release ..
ninja
```

### Static Analysis Build

Build with static analysis enabled:

```bash
mkdir build-analysis && cd build-analysis
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DENABLE_CLANG_TIDY=ON \
      -DENABLE_CPPCHECK=ON ..
make -j$(nproc)
```

## Production Builds

### Optimized Production Build

Maximum optimization for production deployment:

```bash
mkdir build-production && cd build-production
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3 -march=native -DNDEBUG" \
      -DBUILD_TESTING=OFF \
      -DENABLE_COVERAGE=OFF ..
make -j$(nproc)

# Strip debug symbols
strip libfriendly_lwm2m.a
```

### Hardened Security Build

Build with security hardening flags:

```bash
mkdir build-hardened && cd build-hardened
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIE" \
      -DCMAKE_EXE_LINKER_FLAGS="-Wl,-z,relro,-z,now -pie" ..
make -j$(nproc)
```

### Debian Package Build

Create a .deb package:

```bash
mkdir build-deb && cd build-deb
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/usr ..
make -j$(nproc)

# Create package
cpack -G DEB

# Install package
sudo dpkg -i friendly-lwm2m-1.2.0-Linux.deb
```

### RPM Package Build

Create an .rpm package:

```bash
mkdir build-rpm && cd build-rpm
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/usr ..
make -j$(nproc)

# Create package
cpack -G RPM

# Install package
sudo rpm -i friendly-lwm2m-1.2.0-Linux.rpm
```

## Troubleshooting Common Build Issues

### Missing OpenSSL

**Error:**
```
Could NOT find OpenSSL
```

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# Fedora/RHEL
sudo dnf install openssl-devel

# macOS
brew install openssl
cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl ..
```

### Missing libcurl

**Error:**
```
Could NOT find CURL
```

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev

# Fedora/RHEL
sudo dnf install libcurl-devel
```

### Google Test Not Found

**Error:**
```
Could NOT find GTest
```

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install libgtest-dev libgmock-dev

# Build from source if needed
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp lib/*.a /usr/lib/ || sudo cp *.a /usr/lib/
```

### Wakaama Submodule Missing

**Error:**
```
fatal error: liblwm2m.h: No such file or directory
```

**Solution Option 1** (Disable optional objects):
```bash
cmake -DWITH_WAN_FAILOVER=OFF \
      -DWITH_MULTIWAN_HEALTH=OFF \
      -DWITH_WIFI_CLIENT_MGMT=OFF \
      -DWITH_WIFI_CHANNEL_OPT=OFF \
      -DWITH_LAN_CONFIG=OFF \
      -DWITH_ROUTING_TABLE=OFF \
      -DWITH_VPN_CONFIG=OFF ..
```

**Solution Option 2** (Initialize submodule):
```bash
git submodule update --init --recursive
```

### Compiler Version Too Old

**Error:**
```
error: #error This file requires compiler and library support for C++17
```

**Solution:**
```bash
# Update compiler
# Ubuntu 20.04+
sudo apt-get install gcc-10 g++-10
cmake -DCMAKE_C_COMPILER=gcc-10 -DCMAKE_CXX_COMPILER=g++-10 ..

# Or use Clang
sudo apt-get install clang-12
cmake -DCMAKE_C_COMPILER=clang-12 -DCMAKE_CXX_COMPILER=clang++-12 ..
```

### Out of Memory During Build

**Error:**
```
c++: fatal error: Killed signal terminated program cc1plus
```

**Solution:**
```bash
# Use fewer parallel jobs
make -j2

# Or add swap space
sudo fallocate -l 4G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```

### Clean Build

When all else fails, clean and rebuild:

```bash
# Remove build directory
rm -rf build

# Recreate and rebuild
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Build Performance Tips

### Use ccache

Speed up rebuilds with compiler cache:

```bash
# Install ccache
sudo apt-get install ccache

# Configure CMake to use ccache
cmake -DCMAKE_CXX_COMPILER_LAUNCHER=ccache ..
make -j$(nproc)
```

### Use Ninja

Ninja is faster than Make:

```bash
sudo apt-get install ninja-build
cmake -G Ninja ..
ninja
```

### Parallel Builds

Use all CPU cores:

```bash
# Auto-detect cores
make -j$(nproc)

# Or specify manually
make -j8
```

### Link Time Optimization (LTO)

Enable for smaller, faster binaries:

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON ..
make -j$(nproc)
```

## Additional Resources

- **CMake Documentation**: https://cmake.org/documentation/
- **Cross-Compilation Guide**: See `docs/CROSS_COMPILE.md`
- **Platform Support**: See `docs/PLATFORM_GUIDE.md`
- **Quick Start**: See `QUICK_START.md`
- **Getting Started**: See `GETTING_STARTED.md`

## Build Matrix

Tested build configurations:

| Platform | Compiler | Build Type | Status |
|----------|----------|------------|--------|
| Ubuntu 22.04 | GCC 11 | Release | ✅ Pass |
| Ubuntu 22.04 | GCC 11 | Debug | ✅ Pass |
| Ubuntu 20.04 | GCC 9 | Release | ✅ Pass |
| Fedora 36 | GCC 12 | Release | ✅ Pass |
| WSL2 Ubuntu | GCC 11 | Release | ✅ Pass |
| ARM32 | GCC 10 | Release | ✅ Pass |
| ARM64 | GCC 11 | Release | ✅ Pass |
| OpenWrt 22.03 | GCC 11 | MinSizeRel | ✅ Pass |

Last Updated: 2026-04-11
