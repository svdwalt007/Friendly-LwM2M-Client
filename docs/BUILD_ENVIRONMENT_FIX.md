# Build Environment Edge AI & Starlink Fix

## Issue

When running `./scripts/build_full_dev.sh`, Edge AI and Starlink were showing as OFF in the cmake configuration output, despite being set to ON in the script.

```
Edge AI:          OFF  # Expected ON
Starlink:         OFF  # Expected ON
```

## Root Cause

The `cmake/BuildEnvironment.cmake` file is included **first** in CMakeLists.txt (line 14), before any command-line options are processed.

The DEV environment (default) was **forcing** Edge AI and Starlink to OFF:

```cmake
# cmake/BuildEnvironment.cmake - DEV environment (line 77-78)
set(WITH_EDGE_AI OFF CACHE BOOL "" FORCE)
set(WITH_STARLINK OFF CACHE BOOL "" FORCE)
```

The `FORCE` keyword means these settings **cannot be overridden** by command-line options in the build script.

## Solution

Modified `cmake/BuildEnvironment.cmake` line 77-78 to enable Edge AI and Starlink in DEV environment:

```cmake
# Before (line 77-78)
set(WITH_EDGE_AI OFF CACHE BOOL "" FORCE)
set(WITH_STARLINK OFF CACHE BOOL "" FORCE)

# After
set(WITH_EDGE_AI ON CACHE BOOL "" FORCE)
set(WITH_STARLINK ON CACHE BOOL "" FORCE)
```

Updated comment on line 74 to reflect the change:

```cmake
# Before
# Enable core features, disable experimental

# After
# Enable core features (Edge AI and Starlink now enabled by default)
```

## Build Script Update

Updated `scripts/build_full_dev.sh` to explicitly use DEV environment with clear comments:

```bash
# NOTE: Using DEV build environment which enables Edge AI and Starlink
# BuildEnvironment.cmake is included first and sets baseline defaults
# Command-line options below can override if needed

cmake .. \
  -DBUILD_ENV=DEV \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  ...
```

## Verification

After the fix, running `./scripts/build_full_dev.sh` should show:

```
Build Environment: DEV
Build Type: RelWithDebInfo

Core Features:
  DTLS:             ON
  mbedTLS:          ON

DTLS Multi-Provider (v1.2.2):
  mbedTLS Provider:  ON (DEFAULT)

  Delta FOTA:       ON
  MQTT Transport:   ON
  Edge AI:          ON     # ✅ Now ON
  TensorFlow Lite:  ON
  ONNX Runtime:     ON

v1.2.0 Extended Features:
  Starlink:         ON     # ✅ Now ON
  Starlink gRPC:    ON
```

## Build Environment Configurations

For reference, here's what each environment enables:

### TEST Environment
```bash
cmake .. -DBUILD_ENV=TEST
```
- Build Type: Debug (-g -O0)
- Tests: ON
- Coverage: ON
- Sanitizers: ON
- Edge AI: ON
- Starlink: ON
- All DTLS providers: ON

**Use for**: Comprehensive testing with full debugging

### DEV Environment (Default)
```bash
cmake .. -DBUILD_ENV=DEV  # or omit, it's the default
```
- Build Type: RelWithDebInfo (-g -O2)
- Tests: ON
- Coverage: OFF
- Sanitizers: ON
- Edge AI: ON (after fix)
- Starlink: ON (after fix)
- DTLS: mbedTLS only

**Use for**: Development and testing with optimized code

### PREPROD Environment
```bash
cmake .. -DBUILD_ENV=PREPROD
```
- Build Type: Release (-O3 -g1)
- Tests: OFF
- Coverage: OFF
- Sanitizers: OFF
- Edge AI: User-configured (not forced)
- Starlink: User-configured (not forced)
- LTO: ON

**Use for**: Pre-production validation

### PROD Environment
```bash
cmake .. -DBUILD_ENV=PROD
```
- Build Type: MinSizeRel (-Os)
- Tests: OFF
- Coverage: OFF
- Sanitizers: OFF
- Examples: OFF
- Docs: OFF
- Edge AI: User-configured (not forced)
- Starlink: User-configured (not forced)
- LTO: ON
- Stripped: ON

**Use for**: Production deployment

## How Build Environments Work

1. **CMakeLists.txt line 14**: Includes `cmake/BuildEnvironment.cmake` FIRST
2. **BuildEnvironment.cmake**: Reads `BUILD_ENV` variable (default: "DEV")
3. **Sets defaults with FORCE**: Cannot be overridden by command-line options
4. **Command-line options**: Can override non-FORCED settings

This is why we needed to modify BuildEnvironment.cmake directly rather than just adding options to the build script.

## Alternative Approach

If you want full control without modifying BuildEnvironment.cmake, you can skip it entirely by using PREPROD or PROD environments (which don't force feature flags), then set everything manually:

```bash
cmake .. \
  -DBUILD_ENV=PREPROD \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DBUILD_TESTS=ON \
  -DENABLE_COVERAGE=ON \
  -DENABLE_SANITIZERS=ON \
  -DWITH_EDGE_AI=ON \
  -DWITH_STARLINK=ON \
  ...
```

But this is more verbose and defeats the purpose of build environments.

## Files Modified

1. **cmake/BuildEnvironment.cmake**
   - Line 74: Updated comment
   - Line 77: Changed `WITH_EDGE_AI` from OFF to ON
   - Line 78: Changed `WITH_STARLINK` from OFF to ON

2. **scripts/build_full_dev.sh**
   - Line 58-63: Updated comments explaining DEV environment
   - Line 65: Explicitly set `-DBUILD_ENV=DEV` for clarity

## Testing

To verify the fix works:

```bash
# Clean rebuild
rm -rf build/*

# Run build script
./scripts/build_full_dev.sh

# Check output for:
#   Edge AI:          ON
#   Starlink:         ON
```

Or run cmake directly:

```bash
cd build
cmake .. -DBUILD_ENV=DEV
cmake -L | grep "WITH_EDGE_AI\|WITH_STARLINK"
```

Expected output:
```
WITH_EDGE_AI:BOOL=ON
WITH_STARLINK:BOOL=ON
```

## Why This Matters

**Edge AI Integration**: Enables TensorFlow Lite and ONNX Runtime for on-device AI inference
- LwM2M Object for Edge AI model deployment and execution
- Critical for IoT devices with AI capabilities

**Starlink Integration**: Enables Starlink dish monitoring and control via gRPC
- LwM2M objects for Starlink metrics and configuration
- Important for remote deployments using Starlink connectivity

Both features are production-ready and should be enabled by default in development builds.

## Related Documentation

- **docs/CMAKE_BUILD_OPTIONS.md** - Complete option reference
- **docs/BUILD_CONFIGURATION_REFERENCE.md** - Configuration mapping guide
- **CMakeLists.txt** - Main build configuration
- **cmake/BuildEnvironment.cmake** - Environment presets

---

**Fixed**: 2026-04-30
**Version**: 1.2.2
**Build**: scripts/build_full_dev.sh
