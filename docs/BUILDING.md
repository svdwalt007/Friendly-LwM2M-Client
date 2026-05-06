# Building Friendly LwM2M Client v1.2.2

**Complete build guide for all platforms and configurations**

---

## Table of Contents

- [Quick Start](#quick-start)
- [Prerequisites](#prerequisites)
- [Build Dependencies](#build-dependencies)
- [Build Steps](#build-steps)
- [Build Options](#build-options)
- [Platform-Specific Builds](#platform-specific-builds)
- [Troubleshooting](#troubleshooting)

---

## Quick Start

**For the impatient** (Ubuntu 22.04):

```bash
# 1. Install minimal dependencies
./scripts/install_curl_dependencies.sh

# 2. Build custom minimal libcurl (REQUIRED)
# See docs/MINIMAL_CURL_BUILD.md for detailed instructions
# Quick version:
wget https://curl.se/download/curl-8.7.1.tar.gz
tar -xzf curl-8.7.1.tar.gz
cd curl-8.7.1
./configure --prefix=/usr/local --enable-static --disable-shared \
    --with-openssl --with-zlib --with-brotli --with-nghttp2 --with-zstd \
    --disable-ldap --disable-ldaps --without-libssh --without-libssh2 \
    --without-libidn2 --without-librtmp --without-libpsl
make -j$(nproc)
sudo make install

# 3. Clone and build
git clone --recurse-submodules https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# 4. Run
cd examples
./friendly_example_client --help
```

---

## Prerequisites

### Required

- **Operating System**: Ubuntu 22.04 (or compatible Linux distribution)
- **CMake**: version 3.10 or higher
- **Compiler**: clang-14/clang++-14 or gcc-11/g++-11
- **Git**: for cloning repository and submodules

### DTLS Provider (choose one or more)

| Provider | Version | Install |
|----------|---------|---------|
| **mbedTLS** (default) | 3.6+ | `sudo apt install libmbedtls-dev` |
| **TinyDTLS** | 0.8.6 | Included as submodule |
| **OpenSSL** | 3.x | `sudo apt install libssl-dev` |
| **wolfSSL** | 5.x | Build from source |

### Custom Minimal Libcurl (REQUIRED ⚠️)

**Why custom libcurl?**

The system libcurl has 28+ dependencies (PostgreSQL, MySQL, SQLite, LDAP, SSH, etc.) that aren't needed for IoT firmware updates. Our minimal build reduces this to ~8 libraries (71% fewer dependencies).

**See**: [docs/MINIMAL_CURL_BUILD.md](MINIMAL_CURL_BUILD.md) for complete instructions.

---

## Build Dependencies

### Option 1: Automated Installation (Recommended)

Install minimal dependencies for custom libcurl:

```bash
./scripts/install_curl_dependencies.sh
```

This installs **only 6 packages** (vs 30+ for system libcurl):
- libssl-dev (OpenSSL for HTTPS/TLS)
- zlib1g-dev (compression)
- libbrotli-dev (brotli compression - optional)
- libnghttp2-dev (HTTP/2 - optional)
- libzstd-dev (zstandard compression - optional)
- libkrb5-dev (Kerberos/GSSAPI - optional)

### Option 2: Manual Installation

**Ubuntu/Debian**:
```bash
sudo apt-get update
sudo apt-get install -y \
    cmake \
    build-essential \
    clang-14 \
    clang++-14 \
    libssl-dev \
    zlib1g-dev \
    libbrotli-dev \
    libnghttp2-dev \
    libzstd-dev \
    libkrb5-dev \
    libmbedtls-dev
```

**RHEL/Fedora**:
```bash
sudo dnf install -y \
    cmake \
    gcc gcc-c++ \
    clang \
    openssl-devel \
    zlib-devel \
    brotli-devel \
    libnghttp2-devel \
    libzstd-devel \
    krb5-devel \
    mbedtls-devel
```

---

## Build Steps

### Step 1: Build Custom Minimal Libcurl

**IMPORTANT**: This step is REQUIRED. Do not skip.

See [docs/MINIMAL_CURL_BUILD.md](MINIMAL_CURL_BUILD.md) for detailed instructions.

**Quick version**:

```bash
# Download libcurl 8.7.1 (or latest stable)
wget https://curl.se/download/curl-8.7.1.tar.gz
tar -xzf curl-8.7.1.tar.gz
cd curl-8.7.1

# Configure with minimal features
./configure \
    --prefix=/usr/local \
    --enable-static \
    --disable-shared \
    --with-openssl \
    --with-zlib \
    --with-brotli \
    --with-nghttp2 \
    --with-zstd \
    --disable-ldap \
    --disable-ldaps \
    --disable-rtsp \
    --disable-dict \
    --disable-telnet \
    --disable-tftp \
    --disable-pop3 \
    --disable-imap \
    --disable-smtp \
    --disable-gopher \
    --disable-mqtt \
    --without-libidn2 \
    --without-librtmp \
    --without-libssh \
    --without-libssh2 \
    --without-libpsl

# Build and install
make -j$(nproc)
sudo make install

# Verify
/usr/local/bin/curl-config --version
# Expected: libcurl 8.7.1
```

### Step 2: Clone Repository

```bash
git clone --recurse-submodules https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client
```

**If you forgot `--recurse-submodules`**:
```bash
git submodule update --init --recursive
```

### Step 3: Configure Build

```bash
mkdir build
cd build

# Basic build (mbedTLS default)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Or with custom options (see Build Options below)
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DDTLS_PROVIDER_TINYDTLS=OFF \
    -DDTLS_DEFAULT_PROVIDER=mbedtls \
    ..
```

### Step 4: Build

```bash
# Build with all CPU cores
make -j$(nproc)

# Or build specific targets
make friendly_lwm2m              # Library only
make friendly_example_client     # Example client
make test_firmware_update_object # Tests
```

### Step 5: Verify Build

```bash
# Check binary exists
ls -lh examples/friendly_example_client

# Check dependencies (should NOT see PostgreSQL, MySQL, LDAP, SSH)
ldd examples/friendly_example_client | grep -E "(curl|ssl|ldap|ssh|pq|mysql)"

# Expected output (only OpenSSL, no databases/LDAP/SSH):
# libssl.so.3 => /usr/lib/x86_64-linux-gnu/libssl.so.3
# libcrypto.so.3 => /usr/lib/x86_64-linux-gnu/libcrypto.so.3

# Run help to verify
./examples/friendly_example_client --help
```

---

## Build Options

### CMake Configuration Options

#### DTLS Provider Selection

```bash
-DDTLS_PROVIDER_TINYDTLS=ON/OFF    # Enable TinyDTLS (default: ON)
-DDTLS_PROVIDER_MBEDTLS=ON/OFF     # Enable mbedTLS (default: ON)
-DDTLS_PROVIDER_OPENSSL=ON/OFF     # Enable OpenSSL (default: OFF)
-DDTLS_PROVIDER_WOLFSSL=ON/OFF     # Enable wolfSSL (default: OFF)
-DDTLS_DEFAULT_PROVIDER=<name>     # Default provider (default: mbedtls)
```

**Example**: Build with all providers:
```bash
cmake \
    -DDTLS_PROVIDER_TINYDTLS=ON \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DDTLS_PROVIDER_OPENSSL=ON \
    -DDTLS_PROVIDER_WOLFSSL=OFF \
    -DDTLS_DEFAULT_PROVIDER=mbedtls \
    ..
```

#### Build Types

```bash
-DCMAKE_BUILD_TYPE=Release         # Optimized build (recommended)
-DCMAKE_BUILD_TYPE=Debug           # Debug symbols, no optimization
-DCMAKE_BUILD_TYPE=MinSizeRel      # Optimize for size
-DCMAKE_BUILD_TYPE=RelWithDebInfo  # Optimized + debug symbols
```

#### Optional Features

```bash
-DWITH_COMPRESSION=ON              # Enable compression (bzip2, lzma, zstd, brotli)
-DWITH_DELTA_FOTA=ON               # Enable delta firmware updates
-DWITH_ROLLBACK=ON                 # Enable A/B partition rollback
-DWITH_ML_INFERENCE=ON             # Enable ML inference (TensorFlow Lite, ONNX)
-DWITH_MQTT=ON                     # Enable MQTT transport binding
-DBUILD_TESTING=ON                 # Build unit tests
```

#### Compiler Selection

```bash
-DCMAKE_C_COMPILER=/usr/bin/clang-14
-DCMAKE_CXX_COMPILER=/usr/bin/clang++-14
```

Or:
```bash
-DCMAKE_C_COMPILER=/usr/bin/gcc-11
-DCMAKE_CXX_COMPILER=/usr/bin/g++-11
```

### Complete Configuration Example

```bash
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=/usr/bin/clang-14 \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++-14 \
    -DDTLS_PROVIDER_MBEDTLS=ON \
    -DDTLS_PROVIDER_TINYDTLS=ON \
    -DDTLS_PROVIDER_OPENSSL=OFF \
    -DDTLS_DEFAULT_PROVIDER=mbedtls \
    -DWITH_COMPRESSION=ON \
    -DWITH_DELTA_FOTA=ON \
    -DWITH_ROLLBACK=ON \
    -DBUILD_TESTING=ON \
    ..
```

---

## Platform-Specific Builds

### OpenWrt

See [docs/OPENWRT_INTEGRATION.md](OPENWRT_INTEGRATION.md)

**Quick version**:
```bash
# In OpenWrt SDK
cd package
git clone https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git friendly-lwm2m
cd ..
make package/friendly-lwm2m/compile V=s
```

### Raspberry Pi 4 (ARM64)

See [docs/RPI4_ARM64_PORTING_GUIDE.md](RPI4_ARM64_PORTING_GUIDE.md)

**Quick version**:
```bash
# On RPi4 running Ubuntu 22.04 ARM64
sudo apt-get install cmake build-essential libssl-dev libmbedtls-dev
# Then follow standard build steps above
```

### prplOS

See [docs/PRPLOS_PORTING_GUIDE.md](PRPLOS_PORTING_GUIDE.md)

---

## Troubleshooting

### Issue 1: CMake can't find custom libcurl

**Symptom**:
```
-- Found CURL: /usr/lib/x86_64-linux-gnu/libcurl.a (NOT /usr/local/lib/libcurl.a)
```

**Solution**:
The CMakeLists.txt forces use of `/usr/local/lib/libcurl.a`. If it's not found:
```bash
# Verify custom libcurl exists
ls -lh /usr/local/lib/libcurl.a

# If missing, rebuild and install:
cd curl-8.7.1
sudo make install

# Clean CMake cache
cd /path/to/Friendly-LwM2M-Client/build
rm -rf *
cmake ..
```

### Issue 2: Build fails with database/LDAP undefined references

**Symptom**:
```
undefined reference to `ldap_initialize'
undefined reference to `PQconnectdb'
```

**Cause**: You're using system libcurl instead of custom minimal libcurl

**Solution**:
1. Verify custom libcurl installation: `ls -lh /usr/local/lib/libcurl.a`
2. Clean build: `cd build && rm -rf * && cmake ..`
3. Check CMake finds correct libcurl: `grep CURL_LIBRARY CMakeCache.txt`
   - Should show: `CURL_LIBRARY:FILEPATH=/usr/local/lib/libcurl.a`

### Issue 3: Missing DTLS provider

**Symptom**:
```
CMake Error: DTLS provider 'mbedtls' requested but not found
```

**Solution**:
```bash
# Install mbedTLS
sudo apt-get install libmbedtls-dev

# Or disable that provider
cmake -DDTLS_PROVIDER_MBEDTLS=OFF ..
```

### Issue 4: Submodule not initialized

**Symptom**:
```
CMake Error: Could not find 2305-Wakaama/core/liblwm2m.c
```

**Solution**:
```bash
git submodule update --init --recursive
```

### Issue 5: Tests fail to build

**Symptom**:
```
GoogleTest not found
```

**Solution**:
```bash
# Install GoogleTest
sudo apt-get install libgtest-dev

# Or disable tests
cmake -DBUILD_TESTING=OFF ..
```

---

## Build Artifacts

After successful build, you'll find:

```
build/
├── friendly_lwm2m                    # Static library
├── libfriendly_lwm2m.a              # Static library file
├── examples/
│   └── friendly_example_client       # Example client executable
└── test/
    ├── test_firmware_update_object   # Firmware update tests
    ├── test_delta_algorithms         # Delta algorithm tests
    ├── test_rollback_manager         # Rollback manager tests
    └── test_blockwise_transfer       # Blockwise transfer tests
```

---

## Next Steps

After building:

1. **Configure the client**: See [docs/CONFIGURATION.md](CONFIGURATION.md)
2. **Run the example**: `./examples/friendly_example_client --help`
3. **Run tests**: `cd build && ctest -V`
4. **Connect to LwM2M server**: See [docs/QUICK_START.md](QUICK_START.md)
5. **Deploy to production**: See platform-specific guides

---

## Related Documentation

- **[Minimal Libcurl Build](MINIMAL_CURL_BUILD.md)** - REQUIRED custom libcurl build
- **[Minimal Libcurl Integration](MINIMAL_CURL_INTEGRATION.md)** - Integration verification
- **[Database References Removed](DATABASE_REFERENCES_REMOVED.md)** - Dependency cleanup summary
- **[DTLS Provider Selection](DTLS_PROVIDER_SELECTION_GUIDE.md)** - Choose DTLS implementation
- **[Configuration Guide](CONFIGURATION.md)** - CMake and runtime options
- **[Troubleshooting](TROUBLESHOOTING.md)** - Common issues and solutions

---

**Build Status**: ✅ All builds passing with minimal libcurl
**Dependencies**: 8 libraries (reduced from 28)
**Build Time**: ~2-3 minutes on 8-core CPU
**Binary Size**: ~15-20 MB (static build)
