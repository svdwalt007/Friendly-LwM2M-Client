# b02.log Build Error Analysis

**Date**: 2026-04-30
**Status**: ❌ Build Failed - Missing required libraries
**Root Cause**: Required libcurl dependency libraries are not installed on the system

## Problem Summary

The build in b02.log shows the **SAME errors** as b01.log, which means:
1. ✅ The CMake changes were applied successfully
2. ❌ The required libraries are **NOT installed** on the system
3. ❌ CMake's `find_library()` cannot find them, so they're not being linked

## Evidence: Libraries Not Installed

Checked the system for required libraries:

```bash
$ ls -la /usr/lib/x86_64-linux-gnu/libpsl.*
ls: cannot access '/usr/lib/x86_64-linux-gnu/libpsl.*': No such file or directory

$ ls -la /usr/lib/x86_64-linux-gnu/librtmp.*
ls: cannot access '/usr/lib/x86_64-linux-gnu/librtmp.*': No such file or directory

$ ls -la /usr/lib/x86_64-linux-gnu/libldap.*
ls: cannot access '/usr/lib/x86_64-linux-gnu/libldap.*': No such file or directory

$ ls -la /usr/lib/x86_64-linux-gnu/liblber.*
ls: cannot access '/usr/lib/x86_64-linux-gnu/liblber.*': No such file or directory

$ ls -la /usr/lib/x86_64-linux-gnu/libidn2.*
ls: cannot access '/usr/lib/x86_64-linux-gnu/libidn2.*': No such file or directory
```

**Conclusion**: None of the missing libraries are installed.

## Why This Happens

Ubuntu's pre-compiled `libcurl.a` (static library) was built with support for:
- SSH/SFTP protocol (requires libssh)
- RTMP streaming (requires librtmp)
- LDAP protocol (requires libldap + liblber)
- Internationalized domain names (requires libidn2)
- Public Suffix List cookie validation (requires libpsl)
- Brotli compression (requires libbrotli*)
- HTTP/2 (requires libnghttp2)
- Zstandard compression (requires libzstd)
- Kerberos/GSSAPI auth (requires krb5)

When you **statically link** libcurl.a, all these dependencies must be present, even if your application doesn't use those specific protocols.

## Missing Libraries from b02.log Errors

### 1. libpsl (Public Suffix List)
```
undefined reference to `psl_free'
undefined reference to `psl_latest'
undefined reference to `psl_builtin'
```
**Package needed**: `libpsl-dev`

### 2. libssh (SSH/SFTP)
```
undefined reference to `ssh_connect'
undefined reference to `ssh_free'
undefined reference to `sftp_new'
... (150+ symbols)
```
**Package needed**: `libssh-dev`

### 3. librtmp (RTMP streaming)
```
undefined reference to `RTMP_Alloc'
undefined reference to `RTMP_Connect1'
undefined reference to `RTMP_Write'
```
**Package needed**: `librtmp-dev`

### 4. libidn2 (Internationalized domains)
```
undefined reference to `idn2_lookup_ul'
undefined reference to `idn2_free'
undefined reference to `idn2_check_version'
```
**Package needed**: `libidn2-dev`

### 5. libldap + liblber (LDAP protocol)
```
undefined reference to `ldap_result'
undefined reference to `ldap_msgtype'
undefined reference to `ber_free'
... (50+ symbols)
```
**Packages needed**: `libldap2-dev`

### 6. libbrotli (Brotli compression)
The CMake configuration checks for these, but they may not be installed.
**Package needed**: `libbrotli-dev`

### 7. Additional dependencies
- **libnghttp2**: HTTP/2 support - `libnghttp2-dev`
- **libzstd**: Zstandard compression - `libzstd-dev`
- **zlib**: Deflate/inflate compression - `zlib1g-dev`
- **krb5**: Kerberos/GSSAPI - `libkrb5-dev`

## Solution: Install Required Libraries

### Quick Fix (Ubuntu/Debian):

```bash
sudo apt-get update
sudo apt-get install -y \
    libssh-dev \
    libpsl-dev \
    librtmp-dev \
    libidn2-dev \
    libldap2-dev \
    libbrotli-dev \
    libnghttp2-dev \
    libzstd-dev \
    zlib1g-dev \
    libkrb5-dev
```

### Automated Installation:

We've created a script that detects your OS and installs the correct packages:

```bash
./scripts/install_curl_dependencies.sh
```

This script supports:
- Ubuntu/Debian (apt)
- RHEL/Fedora/Rocky/AlmaLinux (dnf)
- Arch/Manjaro (pacman)
- Alpine (apk)

### After Installation:

```bash
cd build
rm -rf *           # Clean build directory
cmake ..           # Reconfigure - will now find the libraries
make -j$(nproc)    # Build with all CPU cores
```

## How CMake Detection Works

The CMakeLists.txt contains (lines 133-145):

```cmake
find_library(SSH_LIB ssh)
find_library(PSL_LIB psl)
find_library(RTMP_LIB rtmp)
find_library(IDN2_LIB idn2)
find_library(LDAP_LIB ldap)
find_library(LBER_LIB lber)
find_library(BROTLI_COMMON_LIB brotlicommon)
find_library(BROTLI_DEC_LIB_CURL brotlidec)
find_library(BROTLI_ENC_LIB_CURL brotlienc)
```

These commands search standard library paths:
- `/usr/lib/x86_64-linux-gnu/`
- `/usr/lib/`
- `/usr/local/lib/`

If a library is **not found**, the variable is set to `<VAR>-NOTFOUND`, and the library is **not linked**.

### Verification

After installing packages, you can verify CMake found them:

```bash
cd build
cmake .. 2>&1 | grep -E "Found|NOT found"
```

You should see messages like:
```
-- Found libssh for CURL: /usr/lib/x86_64-linux-gnu/libssh.so
-- Found GSSAPI via pkg-config: gssapi_krb5;krb5;k5crypto;com_err
```

## Expected CMake Output After Installation

```
-- Found OpenSSL: /usr/lib/x86_64-linux-gnu/libcrypto.a (found version "3.0.13")
-- Found ZLIB: /usr/lib/x86_64-linux-gnu/libz.so (found version "1.2.11")
-- Found CURL: /usr/lib/x86_64-linux-gnu/libcurl.a (found version "7.81.0")
-- Found libssh for CURL: /usr/lib/x86_64-linux-gnu/libssh.so
-- Found GSSAPI via pkg-config: gssapi_krb5;krb5;k5crypto;com_err;keyutils
```

If libraries are missing, you'll see:
```
-- Could NOT find PSL (missing: PSL_LIB)
```

But the build will continue (libraries are optional, detected with `if()` conditionals).

## Why b02.log Shows Same Errors as b01.log

The CMake changes in b01_comprehensive_fix.md are **correct**, but they only work if the libraries are installed:

```cmake
# This code works correctly:
if(PSL_LIB)
    list(APPEND TEST_FW_UPDATE_LIBS ${PSL_LIB})
endif()

# But if PSL_LIB = "PSL_LIB-NOTFOUND", the if() is false
# and the library is NOT added to the link line
# Result: undefined reference errors at link time
```

## Comparison: b01.log vs b02.log

| Aspect | b01.log | b02.log |
|--------|---------|---------|
| CMake configuration | Original (missing library detection) | Updated (comprehensive detection) |
| Libraries installed | ❌ No | ❌ No |
| Build result | ❌ Failed (undefined refs) | ❌ Failed (same undefined refs) |
| Reason | Not linking libraries | Libraries not found by CMake |

**Key insight**: The CMake changes were applied, but without the libraries installed, CMake can't find them, so they're still not being linked.

## Next Steps

### Step 1: Install Libraries
```bash
./scripts/install_curl_dependencies.sh
```

### Step 2: Clean and Rebuild
```bash
cd build
rm -rf *
cmake ..
make -j$(nproc) 2>&1 | tee ../Issues/b03.log
```

### Step 3: Verify Success

The build should succeed with output like:
```
[100%] Linking CXX executable test_firmware_update_object
[100%] Built target test_firmware_update_object
[100%] Linking CXX executable friendly_example_client
[100%] Built target friendly_example_client
```

No undefined reference errors should appear.

## Alternative: Use Dynamic Linking

If installing all these libraries is problematic, you can switch to **dynamic linking**:

### Option A: Use libcurl.so instead of libcurl.a

Edit CMakeLists.txt line ~131:
```cmake
# Before:
set(CURL_USE_STATIC_LIBS ON)
find_package(CURL REQUIRED)

# After:
set(CURL_USE_STATIC_LIBS OFF)  # Use dynamic libcurl.so
find_package(CURL REQUIRED)
```

**Pros**:
- No need to install -dev packages
- Smaller executable size
- Runtime updates to libcurl without rebuilding

**Cons**:
- Runtime dependency on system libcurl.so
- Less portable (needs matching libcurl version)

### Option B: Build Minimal Static libcurl

Compile libcurl from source with only needed features:

```bash
wget https://curl.se/download/curl-8.7.1.tar.gz
tar xzf curl-8.7.1.tar.gz
cd curl-8.7.1
./configure \
    --disable-shared \
    --enable-static \
    --with-openssl \
    --with-zlib \
    --disable-ldap \
    --disable-ldaps \
    --without-libssh \
    --without-librtmp \
    --without-libidn2 \
    --without-libpsl
make
sudo make install
```

Then update CMakeLists.txt to use the custom libcurl:
```cmake
set(CURL_LIBRARY /usr/local/lib/libcurl.a)
set(CURL_INCLUDE_DIR /usr/local/include)
```

## Summary

**Root Cause**: Required libraries not installed on the system

**Solution**: Install development packages for libcurl's dependencies

**Status**:
- ✅ CMake configuration is correct (from b01_comprehensive_fix.md)
- ❌ Required libraries not installed
- ⏳ Waiting for library installation to proceed

**Action Required**:
```bash
./scripts/install_curl_dependencies.sh
cd build
rm -rf *
cmake ..
make -j$(nproc)
```

## Files Created

- `scripts/install_curl_dependencies.sh` - Automated installation script
- `Issues/b02_analysis.md` - This document

## Related Documentation

- `Issues/b01_comprehensive_fix.md` - CMake configuration changes
- `Issues/build38.log` - Original GSSAPI errors
- `Issues/err12.txt` - GSSAPI detection issues
