# Comprehensive libcurl Static Linking Fix for b01.log

**Date**: 2026-04-30
**Issue**: Hundreds of undefined references when statically linking libcurl.a
**Root Cause**: Ubuntu's libcurl compiled with extensive optional features requiring all transitive dependencies

## Problem Analysis

### Missing Libraries Identified from b01.log:

1. **libpsl** (Public Suffix List) - Cookie domain validation
   - Missing symbols: `psl_free`, `psl_latest`, `psl_builtin`, `psl_is_cookie_domain_acceptable`

2. **libssh** (SSH/SFTP support) - ~150+ undefined references
   - Missing symbols: `ssh_connect`, `ssh_free`, `ssh_scp_*`, `sftp_*`, etc.

3. **librtmp** (RTMP streaming protocol)
   - Missing symbols: `RTMP_Alloc`, `RTMP_Connect1`, `RTMP_Write`, `RTMP_Read`, `RTMP_Close`

4. **libidn2** (Internationalized domain names)
   - Missing symbols: `idn2_lookup_ul`, `idn2_free`, `idn2_check_version`, `idn2_to_unicode_8z8z`

5. **libldap/liblber** (LDAP protocol) - ~50+ undefined references
   - Missing symbols: `ldap_*` functions, `ber_*` functions for BER encoding

6. **libbrotlienc/libbrotlidec/libbrotlicommon** (Brotli compression)
   - Missing symbols: `BrotliTransformDictionaryWord`, `_kBrotliPrefixCodeRanges`, `BrotliSharedDictionaryAttach`

7. **libz** (zlib compression) - CRITICAL
   - Missing symbols: `inflateInit_`, `deflate`, `inflate`, `deflateEnd`, `inflateEnd`
   - Used by both libssh and libcurl for compression

## Changes Made

### 1. CMakeLists.txt (Root)

#### Fixed brotli library order (lines 518-530):
Changed from incorrect order to proper dependency order:
```cmake
# Brotli libraries must be in this order: enc, dec, common (for proper symbol resolution)
if(BROTLI_ENC_LIB_CURL)
    target_link_libraries(friendly_lwm2m PRIVATE ${BROTLI_ENC_LIB_CURL})
endif()
if(BROTLI_DEC_LIB_CURL)
    target_link_libraries(friendly_lwm2m PRIVATE ${BROTLI_DEC_LIB_CURL})
endif()
if(BROTLI_COMMON_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${BROTLI_COMMON_LIB})
endif()
```

**Why this order**: libbrotlienc depends on libbrotlidec, which depends on libbrotlicommon.

#### Added ZLIB to test_firmware_update_object (lines 682-730):
```cmake
# Brotli libraries must be in this order: enc, dec, common
if(BROTLI_ENC_LIB_CURL)
    list(APPEND TEST_FW_UPDATE_LIBS ${BROTLI_ENC_LIB_CURL})
endif()
if(BROTLI_DEC_LIB_CURL)
    list(APPEND TEST_FW_UPDATE_LIBS ${BROTLI_DEC_LIB_CURL})
endif()
if(BROTLI_COMMON_LIB)
    list(APPEND TEST_FW_UPDATE_LIBS ${BROTLI_COMMON_LIB})
endif()
if(PSL_LIB)
    list(APPEND TEST_FW_UPDATE_LIBS ${PSL_LIB})
endif()
if(RTMP_LIB)
    list(APPEND TEST_FW_UPDATE_LIBS ${RTMP_LIB})
endif()
if(IDN2_LIB)
    list(APPEND TEST_FW_UPDATE_LIBS ${IDN2_LIB})
endif()
if(LDAP_LIB)
    list(APPEND TEST_FW_UPDATE_LIBS ${LDAP_LIB})
endif()
if(LBER_LIB)
    list(APPEND TEST_FW_UPDATE_LIBS ${LBER_LIB})
endif()
# Add GSSAPI/Kerberos dependencies
if(GSSAPI_FOUND)
    list(APPEND TEST_FW_UPDATE_LIBS ${GSSAPI_LIBRARIES})
endif()
# Add zlib (required by libssh and libcurl for compression)
list(APPEND TEST_FW_UPDATE_LIBS ZLIB::ZLIB)
# OpenSSL must come last for proper symbol resolution
list(APPEND TEST_FW_UPDATE_LIBS OpenSSL::SSL OpenSSL::Crypto)
```

**Key addition**: `ZLIB::ZLIB` was missing entirely from the test executable's link line.

### 2. examples/CMakeLists.txt

#### Comprehensive library linking for friendly_example_client (lines 230-256):
```cmake
# Link CURL dependencies if found (required for static libcurl)
# Note: Order matters for static linking - dependencies must come before their providers
if(SSH_LIB)
    target_link_libraries(friendly_example_client ${SSH_LIB})
endif()
if(NGHTTP2_LIB)
    target_link_libraries(friendly_example_client ${NGHTTP2_LIB})
endif()
if(ZSTD_LIB_CURL)
    target_link_libraries(friendly_example_client ${ZSTD_LIB_CURL})
endif()
# Brotli libraries must be in this order: enc, dec, common
if(BROTLI_ENC_LIB_CURL)
    target_link_libraries(friendly_example_client ${BROTLI_ENC_LIB_CURL})
endif()
if(BROTLI_DEC_LIB_CURL)
    target_link_libraries(friendly_example_client ${BROTLI_DEC_LIB_CURL})
endif()
if(BROTLI_COMMON_LIB)
    target_link_libraries(friendly_example_client ${BROTLI_COMMON_LIB})
endif()
if(PSL_LIB)
    target_link_libraries(friendly_example_client ${PSL_LIB})
endif()
if(RTMP_LIB)
    target_link_libraries(friendly_example_client ${RTMP_LIB})
endif()
if(IDN2_LIB)
    target_link_libraries(friendly_example_client ${IDN2_LIB})
endif()
if(LDAP_LIB)
    target_link_libraries(friendly_example_client ${LDAP_LIB})
endif()
if(LBER_LIB)
    target_link_libraries(friendly_example_client ${LBER_LIB})
endif()
# GSSAPI/Kerberos dependencies (required by libssh and libcurl with GSSAPI support)
if(GSSAPI_FOUND)
    target_link_libraries(friendly_example_client ${GSSAPI_LIBRARIES})
endif()
# Add zlib (required by libssh and libcurl for compression)
target_link_libraries(friendly_example_client ZLIB::ZLIB)
```

**Previously missing**:
- All three brotli libraries in correct order
- PSL_LIB (libpsl)
- RTMP_LIB (librtmp)
- IDN2_LIB (libidn2)
- LDAP_LIB (libldap)
- LBER_LIB (liblber)
- ZLIB::ZLIB

## Library Link Order Explanation

For static linking, library order matters because the linker resolves symbols **left to right**:

1. **Application/Test executable** (needs symbols)
2. **friendly_lwm2m library** (needs symbols)
3. **libcurl dependencies** (provide symbols to libcurl)
   - libssh (needs GSSAPI, zlib)
   - libnghttp2 (HTTP/2)
   - libzstd (compression)
   - **libbrotlienc** → depends on libbrotlidec
   - **libbrotlidec** → depends on libbrotlicommon
   - **libbrotlicommon** (base library)
   - libpsl (cookie domains)
   - librtmp (RTMP protocol)
   - libidn2 (IDN support)
   - libldap (LDAP protocol)
   - liblber (BER encoding for LDAP)
   - **GSSAPI/Kerberos** (authentication)
   - **ZLIB** (compression - used by libssh and libcurl)
4. **OpenSSL** (SSL/TLS - used by everything)

**Critical ordering rules**:
- Brotli: enc before dec before common
- ZLIB must come after libssh (libssh uses it)
- OpenSSL must come last (everything uses it)

## Library Detection

The libraries are detected in CMakeLists.txt lines 133-145:

```cmake
# CURL dependencies (required when using static libcurl)
find_library(SSH_LIB ssh)
find_library(NGHTTP2_LIB nghttp2)
find_library(ZSTD_LIB_CURL zstd)
find_library(BROTLI_COMMON_LIB brotlicommon)
find_library(BROTLI_DEC_LIB_CURL brotlidec)
find_library(BROTLI_ENC_LIB_CURL brotlienc)
find_library(PSL_LIB psl)
find_library(RTMP_LIB rtmp)
find_library(IDN2_LIB idn2)
find_library(LDAP_LIB ldap)
find_library(LBER_LIB lber)
```

These use `find_library()` which searches standard system library paths.

## Installation (if libraries are missing)

If CMake cannot find these libraries, install them:

### Ubuntu/Debian:
```bash
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

### RHEL/Fedora:
```bash
sudo dnf install -y \
    libssh-devel \
    libpsl-devel \
    librtmp-devel \
    libidn2-devel \
    openldap-devel \
    brotli-devel \
    libnghttp2-devel \
    libzstd-devel \
    zlib-devel \
    krb5-devel
```

## Expected Build Outcome

After these changes:

1. **CMake configuration** should find all libraries (or skip gracefully if not found)
2. **Build** should succeed without undefined reference errors
3. **Linking** will include all necessary symbols from libcurl's dependencies

## Testing

To verify the fix:

```bash
cd build
cmake ..
make clean
make -j$(nproc) 2>&1 | tee build_output.log
```

Check for:
- ✅ No "undefined reference to" errors
- ✅ Successful linking of test_firmware_update_object
- ✅ Successful linking of friendly_example_client

## Alternative Solutions (Not Implemented)

### Option 2: Use Dynamic Linking
Instead of static libcurl.a, use dynamic libcurl.so:

```cmake
# In CMakeLists.txt
set(CURL_USE_STATIC_LIBS OFF)
find_package(CURL REQUIRED)
```

**Pros**: Simpler, no need to link transitive dependencies
**Cons**: Runtime dependency on system libcurl.so

### Option 3: Build Custom Minimal libcurl
Compile libcurl from source with only needed features:

```bash
./configure \
    --disable-rtsp \
    --disable-ldap \
    --disable-ldaps \
    --without-librtmp \
    --without-libidn2 \
    --without-libpsl \
    --with-ssl \
    --with-zlib
make
```

**Pros**: Minimal dependencies, smaller binary
**Cons**: Complex build process, maintenance burden

## Summary

This fix implements **Option 1** (comprehensive static linking) by:

1. ✅ Correcting brotli library order (enc → dec → common)
2. ✅ Adding ZLIB::ZLIB to all executables (was missing)
3. ✅ Adding PSL, RTMP, IDN2, LDAP, LBER to friendly_example_client (were missing)
4. ✅ Maintaining proper library order for static linking
5. ✅ Using conditional linking (libraries only linked if found)

**Impact**: Comprehensive fix for all libcurl static linking issues in Ubuntu environments where libcurl is compiled with maximum feature support.

## Related Files

- `CMakeLists.txt` (root) - Main build configuration
- `examples/CMakeLists.txt` - Example client build configuration
- `Issues/build38.log` - Original GSSAPI errors (fixed earlier)
- `Issues/err12.txt` - GSSAPI library detection issues (fixed earlier)
- `Issues/b01.log` - Comprehensive libcurl dependency errors (fixed by this change)

## References

- [libcurl documentation](https://curl.se/libcurl/)
- [CMake find_library](https://cmake.org/cmake/help/latest/command/find_library.html)
- [Static linking order](https://eli.thegreenplace.net/2013/07/09/library-order-in-static-linking)
- [Brotli library structure](https://github.com/google/brotli)
