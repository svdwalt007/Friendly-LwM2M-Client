# Building WppExample - Corrected Guide

## Prerequisites

Install dependencies:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git clang-14 clang++-14
sudo apt-get install libssl-dev libcurl4-openssl-dev zlib1g-dev
```

## Build Steps

### 1. Initialize Wakaama Submodule

The WppExample requires the Wakaama submodule:

```bash
# From project root
git submodule update --init --recursive
```

### 2. Build WppExample

```bash
# Create build directory for examples
mkdir _build
cd _build

# Configure (use clang)
cmake ../examples \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DCMAKE_C_COMPILER=/usr/bin/clang-14 \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++-14

# Build WppExample
cmake --build . --config MinSizeRel --target WppExample -j$(nproc)
```

### 3. Run WppExample

```bash
# Run with default settings
./WppExample

# Or run with custom server
./WppExample --uri coap://your-server.com:5683 --name my-device
```

## Alternative: Build Library Only

If you just want the library (not the full example):

```bash
# From project root
mkdir build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build library and tests
make -j$(nproc)

# This creates:
# - libfriendly_lwm2m.a (main library)
# - test_* executables
```

## Troubleshooting

**Wakaama submodule errors:**
- Make sure you ran `git submodule update --init --recursive`
- Check that `2305-Wakaama/` directory is not empty

**libcoap build errors:**
- The WppExample automatically builds libcoap via `examples/coap/libcoap_build.sh`
- Ensure you have autotools installed: `sudo apt-get install autoconf libtool`

**Missing clang:**
```bash
sudo apt-get install clang-14 clang++-14
```

## What Gets Built

### Root CMakeLists.txt (`make` in build/)
- `libfriendly_lwm2m.a` - Core library
- `test_blockwise_transfer` - Tests
- `test_delta_algorithms` - Tests
- `test_rollback_manager` - Tests  
- `test_firmware_update_object` - Tests

### Examples CMakeLists.txt (`make` in _build/)
- `WppExample` - Full LwM2M client application
- Links against Wakaama
- Includes firmware update support
