# Build Fix Summary for err12.txt Issues

## Problem Analysis

The build errors in `err12.txt` are identical to those in `build38.log` - GSSAPI/Kerberos undefined references when using static versions of `libcurl.a` and `libssh.a`.

### Root Cause

The initial fix added `find_library()` calls for GSSAPI libraries, but they were **NOT FOUND** during CMake configuration. This is because:

1. **GSSAPI libraries not installed** - The development packages were missing
2. **Library search paths** - `find_library()` couldn't locate them
3. **pkg-config not used** - More reliable method for finding Kerberos libraries

## Solution Overview

The fix has two parts:
1. **Install GSSAPI/Kerberos development libraries** (required)
2. **Improved CMake detection** using pkg-config (automatic fallback)

## Changes Made

### 1. Enhanced GSSAPI Detection in CMakeLists.txt

Both `CMakeLists.txt` and `examples/CMakeLists.txt` now use a two-stage detection:

```cmake
# Stage 1: Try pkg-config (most reliable)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(KRB5_GSSAPI QUIET krb5-gssapi)
    if(KRB5_GSSAPI_FOUND)
        set(GSSAPI_LIBRARIES ${KRB5_GSSAPI_LIBRARIES})
        set(GSSAPI_FOUND TRUE)
        message(STATUS "Found GSSAPI via pkg-config: ${KRB5_GSSAPI_LIBRARIES}")
    endif()
endif()

# Stage 2: Fall back to manual library search
if(NOT GSSAPI_FOUND)
    find_library(GSSAPI_KRB5_LIB gssapi_krb5)
    find_library(KRB5_LIB krb5)
    # ... additional libraries ...

    if(GSSAPI_KRB5_LIB AND KRB5_LIB)
        set(GSSAPI_LIBRARIES ${GSSAPI_KRB5_LIB} ${KRB5_LIB} ...)
        set(GSSAPI_FOUND TRUE)
    else()
        message(STATUS "GSSAPI not found - will link without GSSAPI support")
        message(STATUS "  If build fails, install: sudo apt-get install libkrb5-dev")
    endif()
endif()
```

### 2. Simplified Linking Logic

Instead of individual `if()` checks for each library, we now use a single variable:

**Before:**
```cmake
if(GSSAPI_KRB5_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${GSSAPI_KRB5_LIB})
endif()
if(KRB5_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${KRB5_LIB})
endif()
# ... 3 more libraries ...
```

**After:**
```cmake
if(GSSAPI_FOUND)
    target_link_libraries(friendly_lwm2m PRIVATE ${GSSAPI_LIBRARIES})
endif()
```

### 3. Installation Script

Created `scripts/install_gssapi_deps.sh` to automate package installation:

- **Auto-detects OS** (Ubuntu/Debian/RHEL/Fedora/Arch/Alpine)
- **Installs correct packages** for each distribution
- **Verifies installation** using pkg-config and ldconfig
- **Tests CMake detection** to ensure it will work

## Installation Instructions

### Quick Install (Ubuntu/Debian)

```bash
# Using the provided script (recommended)
./scripts/install_gssapi_deps.sh

# Or manually
sudo apt-get update
sudo apt-get install -y libkrb5-dev libkeyutils-dev pkg-config
```

### Quick Install (RHEL/CentOS/Fedora)

```bash
# Using the provided script (recommended)
./scripts/install_gssapi_deps.sh

# Or manually (Fedora 22+)
sudo dnf install -y krb5-devel keyutils-libs-devel pkgconfig

# Or manually (RHEL/CentOS 7)
sudo yum install -y krb5-devel keyutils-libs-devel pkgconfig
```

### Quick Install (Arch Linux)

```bash
sudo pacman -S krb5 keyutils pkgconf
```

### Quick Install (Alpine Linux)

```bash
apk add krb5-dev keyutils-dev e2fsprogs-dev pkgconfig
```

## Rebuild After Installation

After installing the GSSAPI libraries:

```bash
cd /path/to/Friendly-LwM2M-Client
rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

## Verification

### During CMake Configuration

You should now see these messages:

```
-- Found GSSAPI via pkg-config: gssapi_krb5;krb5;k5crypto;com_err
```

Or (fallback method):

```
-- Found GSSAPI for CURL/SSH: /usr/lib/x86_64-linux-gnu/libgssapi_krb5.so;...
```

### If GSSAPI Still Not Found

You'll see:

```
-- GSSAPI not found - will link without GSSAPI support
--   If build fails with GSSAPI errors, install: sudo apt-get install libkrb5-dev
```

This means you need to install the development packages before building.

## What Changed vs build38_fix_summary.md

The original fix (`build38_fix_summary.md`) added GSSAPI detection but:
- ❌ Libraries were not found on the system
- ❌ Build still failed with undefined references
- ❌ No clear installation instructions

This updated fix (`err12_fix_summary.md`):
- ✅ Uses pkg-config for reliable detection
- ✅ Provides clear error messages if libraries missing
- ✅ Includes automated installation script
- ✅ Verifies installation works with CMake
- ✅ Gracefully handles missing libraries (with warning)

## Testing the Fix

### Test 1: Check if GSSAPI is installed

```bash
pkg-config --exists krb5-gssapi && echo "GSSAPI installed" || echo "GSSAPI missing"
```

### Test 2: Check library paths

```bash
ldconfig -p | grep -E "gssapi|krb5"
```

### Test 3: Full build test

```bash
./test_build38_fix.sh
```

Or manually:

```bash
cd build
cmake .. 2>&1 | grep -i gssapi
# Should show "Found GSSAPI"
```

## Why pkg-config?

`pkg-config` is the preferred method for finding Kerberos libraries because:

1. **Cross-platform** - Works on all Linux distributions
2. **Handles dependencies** - Automatically includes all required libs
3. **Version-aware** - Can check for minimum versions
4. **Standard practice** - Used by most projects for Kerberos detection
5. **Reliable paths** - Always finds the correct library locations

The `find_library()` fallback still exists for systems without pkg-config.

## Files Modified

1. `CMakeLists.txt` - Enhanced GSSAPI detection and linking
2. `examples/CMakeLists.txt` - Same enhancements for example client
3. `scripts/install_gssapi_deps.sh` - NEW: Automated installation script

## Files Created

- `scripts/install_gssapi_deps.sh` - OS-agnostic GSSAPI installer

## Summary

**Problem:** GSSAPI libraries not found → build fails with undefined references

**Solution:**
1. Install GSSAPI development packages using the provided script
2. CMake now uses pkg-config for reliable detection
3. Clear error messages guide users to install missing packages

**Result:** Build succeeds with GSSAPI support, or fails early with helpful error message

## Quick Reference

| Error | Solution |
|-------|----------|
| `undefined reference to 'gss_init_sec_context'` | Install `libkrb5-dev` |
| `GSSAPI not found - will link without GSSAPI support` | Run `./scripts/install_gssapi_deps.sh` |
| `pkg-config: command not found` | Install `pkg-config` package |
| Build succeeds but no GSSAPI message | Libraries found and linked successfully ✓ |

## Next Steps

1. Install GSSAPI libraries (if not already installed):
   ```bash
   ./scripts/install_gssapi_deps.sh
   ```

2. Rebuild the project:
   ```bash
   cd build && cmake .. && make -j$(nproc)
   ```

3. Verify no GSSAPI errors in the build output

4. Run tests to ensure everything works:
   ```bash
   make test
   ```
