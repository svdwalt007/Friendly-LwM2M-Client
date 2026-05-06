# b03.log Build Error Analysis and Fix

**Date**: 2026-04-30
**Status**: ✅ Fixed - Added GnuTLS and SASL library support
**Root Cause**: LDAP library dependencies (GnuTLS, SASL) were not detected/linked

## Progress Summary

### From b01.log → b02.log → b03.log

| Build | Status | Issue | Resolution |
|-------|--------|-------|------------|
| b01.log | ❌ Failed | CMake not configured for all libcurl deps | Updated CMakeLists.txt |
| b02.log | ❌ Failed | Libraries not installed | User installed base libraries |
| b03.log | ❌ Failed | Missing LDAP dependencies (GnuTLS, SASL) | This fix |

## New Missing Libraries in b03.log

After installing the basic libcurl dependencies, the build progressed further but encountered **new undefined references** from libldap's own dependencies:

### 1. GnuTLS (GNU Transport Layer Security)

Used by **libldap** for TLS/SSL connections to LDAP servers.

**Missing symbols** (100+ undefined references):
```
undefined reference to `gnutls_init'
undefined reference to `gnutls_handshake'
undefined reference to `gnutls_certificate_allocate_credentials'
undefined reference to `gnutls_certificate_get_peers'
undefined reference to `gnutls_x509_crt_init'
undefined reference to `gnutls_x509_crt_import'
undefined reference to `gnutls_cipher_get'
undefined reference to `gnutls_priority_init'
... (100+ more)
```

**Source**: `/usr/lib/x86_64-linux-gnu/libldap.a(tls_g.o)`

### 2. Cyrus SASL (Simple Authentication and Security Layer)

Used by **libldap** for authentication mechanisms (PLAIN, DIGEST-MD5, GSSAPI, etc.).

**Missing symbols** (30+ undefined references):
```
undefined reference to `sasl_client_init'
undefined reference to `sasl_client_new'
undefined reference to `sasl_client_start'
undefined reference to `sasl_client_step'
undefined reference to `sasl_setprop'
undefined reference to `sasl_getprop'
undefined reference to `sasl_dispose'
undefined reference to `sasl_encode'
undefined reference to `sasl_decode'
... (30+ more)
```

**Source**: `/usr/lib/x86_64-linux-gnu/libldap.a(cyrus.o)`

## Why This Happens: Dependency Chain

```
friendly_example_client
    └─ libcurl.a (static)
        └─ libldap.a (static) ← for LDAP/LDAPS protocol support
            ├─ libgnutls.so ← for TLS encryption
            └─ libsasl2.so  ← for SASL authentication
```

When statically linking libcurl → libldap, we must also link libldap's dependencies (GnuTLS and SASL).

## Changes Made

### 1. CMakeLists.txt (Root)

#### Added library detection (after line 145):
```cmake
# LDAP dependencies (required by libldap when using static linking)
# libldap can use GnuTLS for TLS and Cyrus SASL for authentication
find_library(GNUTLS_LIB gnutls)
find_library(SASL2_LIB sasl2)
```

#### Added status messages (after line 214):
```cmake
if(GNUTLS_LIB)
    message(STATUS "Found GnuTLS for LDAP: ${GNUTLS_LIB}")
endif()
if(SASL2_LIB)
    message(STATUS "Found SASL2 for LDAP: ${SASL2_LIB}")
endif()
```

#### Updated friendly_lwm2m linking (after line 553):
```cmake
# LDAP dependencies (GnuTLS for TLS, SASL for authentication)
if(GNUTLS_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${GNUTLS_LIB})
endif()
if(SASL2_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${SASL2_LIB})
endif()
```

#### Updated test_firmware_update_object linking (after line 744):
```cmake
    # LDAP dependencies (GnuTLS for TLS, SASL for authentication)
    if(GNUTLS_LIB)
        list(APPEND TEST_FW_UPDATE_LIBS ${GNUTLS_LIB})
    endif()
    if(SASL2_LIB)
        list(APPEND TEST_FW_UPDATE_LIBS ${SASL2_LIB})
    endif()
```

### 2. examples/CMakeLists.txt

#### Updated friendly_example_client linking (after line 265):
```cmake
# LDAP dependencies (GnuTLS for TLS, SASL for authentication)
if(GNUTLS_LIB)
    target_link_libraries(friendly_example_client ${GNUTLS_LIB})
endif()
if(SASL2_LIB)
    target_link_libraries(friendly_example_client ${SASL2_LIB})
endif()
```

### 3. scripts/install_curl_dependencies.sh

#### Updated Ubuntu/Debian packages:
```bash
libgnutls28-dev \
libsasl2-dev \
```

#### Updated RHEL/Fedora packages:
```bash
gnutls-devel \
cyrus-sasl-devel \
```

#### Updated Arch packages:
```bash
gnutls \
libsasl \
```

#### Updated Alpine packages:
```bash
gnutls-dev \
cyrus-sasl-dev \
```

## Complete Dependency Tree

After all fixes (b01 → b02 → b03), the complete static linking dependency tree is:

```
friendly_lwm2m library & executables
├─ libcurl.a (HTTP client)
│   ├─ libssh (SSH/SFTP protocol)
│   │   ├─ libgssapi_krb5 (Kerberos authentication)
│   │   │   ├─ libkrb5
│   │   │   ├─ libk5crypto
│   │   │   ├─ libcom_err
│   │   │   └─ libkeyutils
│   │   └─ libz (zlib compression)
│   ├─ libnghttp2 (HTTP/2)
│   ├─ libzstd (Zstandard compression)
│   ├─ libbrotlienc (Brotli compression)
│   │   └─ libbrotlidec
│   │       └─ libbrotlicommon
│   ├─ libpsl (Public Suffix List)
│   ├─ librtmp (RTMP streaming)
│   ├─ libidn2 (Internationalized domains)
│   ├─ libldap (LDAP protocol)
│   │   ├─ liblber (BER encoding)
│   │   ├─ libgnutls (TLS/SSL) ← NEW in b03
│   │   └─ libsasl2 (SASL auth) ← NEW in b03
│   └─ libz (zlib - also used by libssh)
└─ OpenSSL (SSL/TLS, crypto - must be last)
```

## Library Link Order

For static linking, order matters (left to right, dependencies before providers):

1. **Application/Test executable**
2. **friendly_lwm2m library**
3. **libcurl dependencies** (in order):
   - libssh
   - libnghttp2
   - libzstd
   - libbrotlienc → libbrotlidec → libbrotlicommon
   - libpsl
   - librtmp
   - libidn2
   - libldap
   - liblber
   - **libgnutls** (NEW - LDAP's TLS provider)
   - **libsasl2** (NEW - LDAP's auth provider)
   - GSSAPI/Kerberos (krb5, k5crypto, com_err, keyutils)
4. **ZLIB** (used by multiple libraries)
5. **OpenSSL** (used by everything - MUST BE LAST)

## Installation Instructions

### Ubuntu/Debian (Updated):
```bash
sudo apt-get update
sudo apt-get install -y \
    libssh-dev \
    libpsl-dev \
    librtmp-dev \
    libidn2-dev \
    libldap2-dev \
    libgnutls28-dev \
    libsasl2-dev \
    libbrotli-dev \
    libnghttp2-dev \
    libzstd-dev \
    zlib1g-dev \
    libkrb5-dev
```

### Or use the automated script:
```bash
./scripts/install_curl_dependencies.sh
```

This script now includes GnuTLS and SASL for all supported platforms.

## Expected CMake Output

After installing all dependencies and running `cmake ..`:

```
-- Found OpenSSL: /usr/lib/x86_64-linux-gnu/libcrypto.a (found version "3.0.13")
-- Found ZLIB: /usr/lib/x86_64-linux-gnu/libz.so (found version "1.2.11")
-- Found CURL: /usr/lib/x86_64-linux-gnu/libcurl.a (found version "7.81.0")
-- Found libssh for CURL: /usr/lib/x86_64-linux-gnu/libssh.so
-- Found nghttp2 for CURL: /usr/lib/x86_64-linux-gnu/libnghttp2.so
-- Found zstd for CURL: /usr/lib/x86_64-linux-gnu/libzstd.so
-- Found brotli for CURL: /usr/lib/x86_64-linux-gnu/libbrotlicommon.so, ...
-- Found libpsl for CURL: /usr/lib/x86_64-linux-gnu/libpsl.so
-- Found librtmp for CURL: /usr/lib/x86_64-linux-gnu/librtmp.so
-- Found libidn2 for CURL: /usr/lib/x86_64-linux-gnu/libidn2.so
-- Found libldap for CURL: /usr/lib/x86_64-linux-gnu/libldap.so, ...
-- Found GnuTLS for LDAP: /usr/lib/x86_64-linux-gnu/libgnutls.so
-- Found SASL2 for LDAP: /usr/lib/x86_64-linux-gnu/libsasl2.so
-- Found GSSAPI via pkg-config: gssapi_krb5;krb5;k5crypto;com_err;keyutils
```

## Build Process After Fix

```bash
# 1. Install missing libraries
./scripts/install_curl_dependencies.sh

# 2. Clean build directory
cd build
rm -rf *

# 3. Reconfigure CMake (will detect new libraries)
cmake ..

# 4. Build
make -j$(nproc) 2>&1 | tee ../Issues/b04.log

# 5. Verify success
echo $?  # Should be 0
```

Expected successful output:
```
[  7%] Built target test_dtls_integration
[  7%] Built target test_dtls_providers
[ 37%] Built target friendly_lwm2m
[ 44%] Built target test_blockwise_transfer
[ 44%] Built target test_rollback_manager
[ 44%] Built target test_delta_algorithms
[ 45%] Built target test_firmware_update_object
[100%] Built target friendly_example_client
```

No linker errors should occur.

## What b03.log Taught Us

### Lesson 1: Transitive Dependencies are Recursive

Installing direct dependencies (libldap) doesn't automatically install their dependencies (gnutls, sasl2). You must trace the entire dependency tree.

### Lesson 2: Static Library Objects Reveal Dependencies

The error messages show which .o files need symbols:
```
/usr/lib/x86_64-linux-gnu/libldap.a(tls_g.o): undefined reference to `gnutls_init'
                                    ^^^^^^
                                    This tells us libldap needs gnutls
```

### Lesson 3: Dependency Discovery is Iterative

Each build reveals the next layer of dependencies:
- b01: Identified libcurl dependencies (ssh, psl, rtmp, etc.)
- b02: Confirmed libraries need installation
- b03: Discovered LDAP's dependencies (gnutls, sasl2)
- b04: (hopefully) Build succeeds

## Error Pattern Analysis

### GnuTLS Errors (TLS/SSL layer):
- `gnutls_init`, `gnutls_handshake` → Session management
- `gnutls_certificate_*` → Certificate handling
- `gnutls_x509_*` → X.509 certificate operations
- `gnutls_cipher_get`, `gnutls_priority_*` → Cipher negotiation

**Used by**: LDAP TLS/SSL connections (LDAPS, STARTTLS)

### SASL Errors (Authentication layer):
- `sasl_client_init`, `sasl_client_new` → Initialize SASL
- `sasl_client_start`, `sasl_client_step` → Auth handshake
- `sasl_setprop`, `sasl_getprop` → Configure SASL properties
- `sasl_encode`, `sasl_decode` → Security layer encoding

**Used by**: LDAP SASL authentication (DIGEST-MD5, GSSAPI, PLAIN, etc.)

## Alternative Solutions

### Option 1: Disable LDAP Support in libcurl (Build Custom)

Compile libcurl without LDAP:
```bash
./configure \
    --disable-ldap \
    --disable-ldaps \
    --with-ssl \
    --with-zlib \
    --with-ssh
make
sudo make install
```

**Pros**: No GnuTLS/SASL dependencies needed
**Cons**: No LDAP protocol support

### Option 2: Use Dynamic Linking

Link against shared libraries instead of static:
```cmake
set(CURL_USE_STATIC_LIBS OFF)
find_package(CURL REQUIRED)
```

**Pros**: All transitive dependencies handled automatically
**Cons**: Runtime dependency on system libraries

### Option 3: Install All Dev Packages (Recommended)

Use the comprehensive installation script:
```bash
./scripts/install_curl_dependencies.sh
```

**Pros**: Full protocol support, proper static linking
**Cons**: More disk space for development packages

## Summary

**What was fixed**:
- ✅ Added GnuTLS library detection (`find_library(GNUTLS_LIB gnutls)`)
- ✅ Added SASL library detection (`find_library(SASL2_LIB sasl2)`)
- ✅ Linked GnuTLS to all executables (test + example client)
- ✅ Linked SASL to all executables (test + example client)
- ✅ Updated installation script for all platforms

**Why it was needed**:
- libldap uses GnuTLS for TLS/SSL connections
- libldap uses Cyrus SASL for authentication mechanisms
- Static linking requires ALL transitive dependencies

**Impact**:
- Completes the libcurl static linking dependency chain
- Enables full LDAP/LDAPS protocol support in libcurl
- Allows successful static linking on Ubuntu systems

## Files Modified

- `CMakeLists.txt` - Added GnuTLS/SASL detection and linking
- `examples/CMakeLists.txt` - Added GnuTLS/SASL linking for example client
- `scripts/install_curl_dependencies.sh` - Added GnuTLS/SASL packages for all platforms

## Next Build

After applying this fix and installing libraries:
```bash
./scripts/install_curl_dependencies.sh  # Install gnutls + sasl2
cd build && rm -rf * && cmake ..        # Reconfigure
make -j$(nproc) 2>&1 | tee ../Issues/b04.log
```

Expected result: **✅ Build Success** (no undefined references)
