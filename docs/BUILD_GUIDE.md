# Build Guide

Complete guide for building the Friendly LwM2M Client on various platforms and configurations.

## Table of Contents

1. [Requirements](#requirements)
2. [Standard Build](#standard-build)
3. [Build Options](#build-options)
4. [Platform-Specific Builds](#platform-specific-builds)
5. [Cross-Compilation](#cross-compilation)
6. [OpenWRT Integration](#openwrt-integration)
7. [Docker Build](#docker-build)
8. [Troubleshooting](#troubleshooting)

---

## Requirements

### Compiler Requirements

| Compiler | Minimum Version | Recommended |
|----------|-----------------|-------------|
| GCC | 8.0 | 11.0+ |
| Clang | 7.0 | 14.0+ |
| MSVC | 2019 (19.20) | 2022 |

### Build Tools

| Tool | Minimum Version |
|------|-----------------|
| CMake | 3.16 |
| Make | 4.0 |
| Ninja (optional) | 1.10 |

### Required Dependencies

```bash
# Ubuntu/Debian
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libssl-dev \
    libcurl4-openssl-dev \
    zlib1g-dev

# Fedora/RHEL
sudo dnf install -y \
    gcc-c++ \
    cmake \
    pkgconfig \
    openssl-devel \
    libcurl-devel \
    zlib-devel

# macOS
brew install cmake openssl curl zlib
```

### Optional Dependencies

```bash
# Compression libraries
sudo apt-get install -y liblzma-dev libzstd-dev libbrotli-dev libbz2-dev

# MQTT support
sudo apt-get install -y libmosquitto-dev

# TLS alternatives
sudo apt-get install -y libmbedtls-dev  # or tinydtls

# Edge AI (TensorFlow Lite)
# See Edge AI section below

# Testing
sudo apt-get install -y libgtest-dev

# Documentation
sudo apt-get install -y doxygen graphviz

# Code coverage
sudo apt-get install -y lcov
```

---

## Standard Build

### Basic Build

```bash
# Clone repository
git clone https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make -j$(nproc)
```

### Release Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Debug Build

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_SANITIZERS=ON

make -j$(nproc)
```

### Install

```bash
# Build first
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)

# Install (requires sudo)
sudo make install

# Verify installation
ls /usr/local/lib/libfriendly_lwm2m.*
ls /usr/local/include/lwm2m/
```

---

## Build Options

### Complete Options Reference

```bash
cmake .. \
    # Build type
    -DCMAKE_BUILD_TYPE=Release \          # Release, Debug, RelWithDebInfo, MinSizeRel
    
    # Security options
    -DWITH_DTLS=ON \                       # Enable DTLS support
    -DWITH_MBEDTLS=ON \                    # Use mbedTLS (default)
    -DWITH_TINYDTLS=OFF \                  # Use TinyDTLS (constrained)
    
    # Firmware update options
    -DWITH_DELTA_FOTA=ON \                 # Enable delta firmware
    -DWITH_BSDIFF=ON \                     # BSDIFF algorithm
    -DWITH_VCDIFF=ON \                     # VCDIFF algorithm
    -DWITH_COURGETTE=ON \                  # Courgette algorithm
    -DWITH_COMPRESSION=ON \                # Compression support
    -DWITH_ROLLBACK=ON \                   # A/B partition rollback
    
    # Transport options
    -DWITH_MQTT=ON \                       # MQTT transport binding
    
    # Edge AI options
    -DWITH_EDGE_AI=ON \                    # Edge AI object
    -DWITH_TFLITE=OFF \                    # TensorFlow Lite backend
    -DWITH_ONNX=OFF \                      # ONNX Runtime backend
    
    # Build options
    -DBUILD_TESTS=ON \                     # Build unit tests
    -DBUILD_EXAMPLES=ON \                  # Build examples
    -DBUILD_DOCS=OFF \                     # Build documentation
    -DBUILD_SHARED_LIBS=OFF \              # Build shared library
    
    # Development options
    -DENABLE_COVERAGE=OFF \                # Code coverage
    -DENABLE_SANITIZERS=OFF                # Address/UB sanitizers
```

### Feature Profiles

#### Minimal Build (Constrained Devices)

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DWITH_DTLS=ON \
    -DWITH_TINYDTLS=ON \
    -DWITH_MBEDTLS=OFF \
    -DWITH_DELTA_FOTA=OFF \
    -DWITH_MQTT=OFF \
    -DWITH_EDGE_AI=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF
```

#### Full-Featured Build

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_DTLS=ON \
    -DWITH_MBEDTLS=ON \
    -DWITH_DELTA_FOTA=ON \
    -DWITH_BSDIFF=ON \
    -DWITH_VCDIFF=ON \
    -DWITH_COURGETTE=ON \
    -DWITH_COMPRESSION=ON \
    -DWITH_ROLLBACK=ON \
    -DWITH_MQTT=ON \
    -DWITH_EDGE_AI=ON \
    -DWITH_TFLITE=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON
```

#### Development Build

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DENABLE_COVERAGE=ON \
    -DENABLE_SANITIZERS=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

---

## Platform-Specific Builds

### Ubuntu/Debian

```bash
# Install all dependencies
sudo apt-get install -y \
    build-essential cmake pkg-config \
    libssl-dev libcurl4-openssl-dev zlib1g-dev \
    liblzma-dev libzstd-dev libbrotli-dev \
    libmosquitto-dev libgtest-dev

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DWITH_MQTT=ON
make -j$(nproc)
```

### Fedora/RHEL/CentOS

```bash
# Install dependencies
sudo dnf install -y \
    gcc-c++ cmake pkgconfig \
    openssl-devel libcurl-devel zlib-devel \
    xz-devel libzstd-devel brotli-devel \
    mosquitto-devel gtest-devel

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### macOS

```bash
# Install dependencies
brew install cmake openssl curl zlib lzma zstd brotli mosquitto

# Configure with Homebrew paths
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENSSL_ROOT_DIR=$(brew --prefix openssl) \
    -DZLIB_ROOT=$(brew --prefix zlib)

make -j$(sysctl -n hw.ncpu)
```

### Raspberry Pi (Native)

```bash
# Install dependencies
sudo apt-get install -y \
    build-essential cmake \
    libssl-dev libcurl4-openssl-dev zlib1g-dev

# Build (may take a while on older Pi models)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

---

## Cross-Compilation

### ARM64 (aarch64)

```bash
# Install cross-compiler
sudo apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Create toolchain file
cat > arm64-toolchain.cmake << 'EOF'
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
cmake .. -DCMAKE_TOOLCHAIN_FILE=../arm64-toolchain.cmake
make -j$(nproc)
```

### ARM32 (armhf)

```bash
# Install cross-compiler
sudo apt-get install -y gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

# Create toolchain file
cat > armhf-toolchain.cmake << 'EOF'
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
mkdir build-armhf && cd build-armhf
cmake .. -DCMAKE_TOOLCHAIN_FILE=../armhf-toolchain.cmake
make -j$(nproc)
```

---

## OpenWRT Integration

### Using OpenWRT SDK

```bash
# Download OpenWRT SDK (example for 24.10.4)
wget https://downloads.openwrt.org/releases/24.10.4/targets/x86/64/openwrt-sdk-24.10.4-x86-64_gcc-13.3.0_musl.Linux-x86_64.tar.xz
tar xf openwrt-sdk-*.tar.xz
cd openwrt-sdk-*/

# Add package feed
echo "src-git friendly https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git" >> feeds.conf

# Update and install
./scripts/feeds update friendly
./scripts/feeds install friendly-lwm2m-client

# Configure
make menuconfig
# Navigate to: Utilities -> friendly-lwm2m-client

# Build package
make package/friendly-lwm2m-client/compile V=s
```

### Package Structure

```
openwrt/
├── Makefile              # OpenWRT package makefile
└── files/
    ├── lwm2m.init        # Init script
    └── lwm2m.config      # UCI configuration
```

### OpenWRT Makefile

```makefile
include $(TOPDIR)/rules.mk

PKG_NAME:=friendly-lwm2m-client
PKG_VERSION:=1.1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git
PKG_SOURCE_VERSION:=v$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/friendly-lwm2m-client
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Friendly LwM2M Client
  DEPENDS:=+libstdcpp +libopenssl +libcurl +zlib
endef

define Package/friendly-lwm2m-client/description
  Enhanced LwM2M client with delta FOTA, MQTT transport, and Edge AI support.
endef

CMAKE_OPTIONS += \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_MQTT=ON \
    -DWITH_EDGE_AI=OFF \
    -DBUILD_TESTS=OFF

define Package/friendly-lwm2m-client/install
    $(INSTALL_DIR) $(1)/usr/bin
    $(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
    $(INSTALL_DIR) $(1)/etc/init.d
    $(INSTALL_BIN) ./files/lwm2m.init $(1)/etc/init.d/lwm2m
    $(INSTALL_DIR) $(1)/etc/config
    $(INSTALL_CONF) ./files/lwm2m.config $(1)/etc/config/lwm2m
endef

$(eval $(call BuildPackage,friendly-lwm2m-client))
```

---

## Docker Build

### Dockerfile

```dockerfile
# Build stage
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential cmake git \
    libssl-dev libcurl4-openssl-dev zlib1g-dev \
    liblzma-dev libzstd-dev libmosquitto-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN mkdir build && cd build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DWITH_MQTT=ON \
        -DWITH_EDGE_AI=ON \
        -DBUILD_TESTS=OFF && \
    make -j$(nproc)

# Runtime stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libssl3 libcurl4 zlib1g liblzma5 libzstd1 libmosquitto1 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /src/build/libfriendly_lwm2m.* /usr/lib/
COPY --from=builder /src/build/examples/* /usr/bin/

ENTRYPOINT ["/usr/bin/firmware_client_example"]
```

### Build and Run

```bash
# Build image
docker build -t friendly-lwm2m:1.1.0 .

# Run container
docker run -it --rm \
    -e LWM2M_SERVER_URI=coaps://lwm2m.example.com:5684 \
    -e LWM2M_ENDPOINT=docker-device-001 \
    friendly-lwm2m:1.1.0
```

---

## Testing

### Run All Tests

```bash
# Build with tests
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)

# Run all tests
make run_tests

# Or individually
./test_delta_algorithms
./test_blockwise_transfer
./test_rollback_manager
./test_firmware_update_object
./test_mqtt_transport
./test_edge_ai_inference
```

### Code Coverage

```bash
# Build with coverage
cmake .. -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run tests
make run_tests

# Generate report
make coverage

# View report
firefox coverage/index.html
```

### Memory Analysis

```bash
# Build with sanitizers
cmake .. -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run tests (will report issues automatically)
./test_mqtt_transport

# Or use Valgrind
valgrind --leak-check=full ./test_mqtt_transport
```

---

## Troubleshooting

### Common Build Errors

#### "OpenSSL not found"

```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# macOS
brew install openssl
cmake .. -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
```

#### "Could not find mbedTLS"

```bash
# Install mbedTLS
sudo apt-get install libmbedtls-dev

# Or disable mbedTLS
cmake .. -DWITH_MBEDTLS=OFF
```

#### "mosquitto.h not found"

```bash
# Install Mosquitto dev package
sudo apt-get install libmosquitto-dev

# Or disable MQTT
cmake .. -DWITH_MQTT=OFF
```

#### Linker errors with std::filesystem

```bash
# GCC 8 requires explicit linking
cmake .. -DCMAKE_CXX_FLAGS="-lstdc++fs"
```

### Build Size Optimization

```bash
# Minimal size build
cmake .. \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DCMAKE_CXX_FLAGS="-Os -s" \
    -DWITH_DELTA_FOTA=OFF \
    -DWITH_MQTT=OFF \
    -DWITH_EDGE_AI=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF

make -j$(nproc)

# Check size
ls -lh libfriendly_lwm2m.a
```

---

## Generated Files

After successful build:

```
build/
├── libfriendly_lwm2m.a          # Static library
├── libfriendly_lwm2m.so         # Shared library (if enabled)
├── test_*                        # Test executables
├── examples/
│   ├── delta_update_example
│   └── firmware_client_example
└── FriendlyLwM2MConfig.cmake    # CMake config for find_package()
```

---

*For more information, see the [README](../README.md) or open an issue on GitHub.*
