# Build Fix Instructions

## Issue Fixed

The CMake configuration was looking for a missing file: `cmake/FriendlyLwM2MConfig.cmake.in`

## Solution Applied

Created the missing cmake infrastructure:

1. **Created directory**: `cmake/`
2. **Created file**: `cmake/FriendlyLwM2MConfig.cmake.in`

This file is a CMake package configuration template that gets processed during build to help other projects find and use FriendlyLwM2M as a dependency.

## Build Instructions

### Clean Build (Recommended)

```bash
cd /mnt/d/Dev/Friendly-LwM2M-Client

# Remove old build directory
rm -rf build

# Create fresh build directory
mkdir build
cd build

# Configure with CMake (choose options as needed)
cmake .. -DBUILD_EXAMPLES=ON -DWITH_DTLS=OFF

# Build
make -j$(nproc)

# The executable will be at:
# ./examples/lwm2m_client
```

### Build Options

You can customize the build with these CMake options:

```bash
# Basic build (no DTLS)
cmake .. -DBUILD_EXAMPLES=ON -DWITH_DTLS=OFF

# With DTLS/mbedTLS support
cmake .. -DBUILD_EXAMPLES=ON -DWITH_DTLS=ON -DWITH_MBEDTLS=ON

# With MQTT transport
cmake .. -DBUILD_EXAMPLES=ON -DWITH_MQTT=ON

# Debug build with verbose output
cmake .. -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug -DWITH_DTLS=OFF

# Release build optimized
cmake .. -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release -DWITH_DTLS=OFF
```

### If Build Still Fails

If you encounter other missing cmake files or dependencies, try:

1. **Check CMake version**:
   ```bash
   cmake --version
   # Requires CMake 3.16 or newer
   ```

2. **Install missing dependencies** (if needed):
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake libmbedtls-dev

   # Fedora/RHEL
   sudo dnf install gcc-c++ cmake mbedtls-devel

   # macOS
   brew install cmake mbedtls
   ```

3. **Check for other missing .cmake.in files**:
   ```bash
   cd /mnt/d/Dev/Friendly-LwM2M-Client
   grep -r "\.cmake\.in" CMakeLists.txt
   ```

### Testing the CLI Implementation

Once built successfully:

```bash
cd /mnt/d/Dev/Friendly-LwM2M-Client/build/examples

# Test help
./lwm2m_client --help

# Test version
./lwm2m_client --version

# Test default run (backward compatible)
./lwm2m_client

# Test with custom options
./lwm2m_client -n testdevice -vvv
```

### Common Build Issues

#### Issue: "command not found: cmake"
**Solution**: Install cmake
```bash
sudo apt-get install cmake  # Ubuntu/Debian
```

#### Issue: "Could NOT find MbedTLS"
**Solution**: Either install mbedtls or disable DTLS
```bash
# Option 1: Install mbedtls
sudo apt-get install libmbedtls-dev

# Option 2: Build without DTLS
cmake .. -DBUILD_EXAMPLES=ON -DWITH_DTLS=OFF
```

#### Issue: "Target 'xyz' not found"
**Solution**: Make sure you're in the build directory and ran cmake first
```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON
make
```

## What Was Created

### File: cmake/FriendlyLwM2MConfig.cmake.in

This is a CMake package configuration template that:

- Helps other CMake projects find FriendlyLwM2M with `find_package(FriendlyLwM2M)`
- Exports version information
- Lists required dependencies based on build options
- Provides library and include directory paths
- Declares feature flags (DTLS, MQTT, Edge AI, etc.)

The template uses CMake variables (like `@PROJECT_VERSION@`) that get substituted during the build process.

## Verification

After a successful build, verify:

```bash
# Check the binary exists
ls -lh build/examples/lwm2m_client

# Check CLI help works
./build/examples/lwm2m_client --help

# Check backward compatibility (should run with default settings)
./build/examples/lwm2m_client
```

## Next Steps After Successful Build

1. **Test backward compatibility**: Run with no arguments
2. **Test CLI options**: Try various command-line options from CLI_USAGE.md
3. **Update deployment scripts**: Start using CLI options instead of hardcoded values
4. **Read documentation**:
   - `CLI_IMPLEMENTATION_SUMMARY.md` - Implementation overview
   - `examples/CLI_USAGE.md` - Complete CLI reference
   - `docs/QUICKSTART.md` - Getting started guide

---

If you continue to have issues, please share the full error output and I'll help resolve it.
