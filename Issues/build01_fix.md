# Build Error Fix: build01.log

## Issues Found

### Issue 1: Missing WppLogs.h Header
**Error:**
```
fatal error: WppLogs.h: No such file or directory
```

**Affected Files:**
- wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.cpp
- wpp/registry/objects/w_10526_multiwan_health_check/MultiWanHealthCheck.cpp
- wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.cpp
- wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.cpp
- wpp/registry/objects/w_10529_matter_bridge/MatterBridge.cpp

**Root Cause:**
The `wpp/registry/data_model/resource/Resource.h` includes `WppLogs.h` at line 11, but the `wpp/utils` directory (where `WppLogs.h` is located) was not added to the include directories in CMakeLists.txt.

**Fix:**
Added `${CMAKE_CURRENT_SOURCE_DIR}/wpp/utils` to `target_include_directories` for the `friendly_lwm2m` target.

**File Modified:** `CMakeLists.txt` (line ~388-400)

```cmake
target_include_directories(friendly_lwm2m
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/objects
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/instance
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/object/observer
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/resource
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/types
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/utils  # <-- ADDED
)
```

---

### Issue 2: Missing libcurl Dependencies
**Error:**
```
undefined reference to `ssh_get_error'
undefined reference to `sftp_get_error'
undefined reference to `nghttp2_strerror'
undefined reference to `nghttp2_http2_strerror'
undefined reference to `nghttp2_session_callbacks_set_error_callback'
undefined reference to `ZSTD_isError'
undefined reference to `BrotliDecoderGetErrorCode'
```

**Root Cause:**
The project uses a **static build of libcurl** (`/usr/lib/x86_64-linux-gnu/libcurl.a`) which was compiled with support for:
- SSH/SFTP (libssh)
- HTTP/2 (nghttp2)
- Compression (zstd, brotli)

When linking statically, all of libcurl's dependencies must be explicitly linked into the final binary.

**Fix:**
Added detection and linking of libcurl dependencies in CMakeLists.txt.

**File Modified:** `CMakeLists.txt`

**Change 1: Detect CURL dependencies (after line 131):**
```cmake
# CURL dependencies (required when using static libcurl)
# libcurl can be built with support for SSH, HTTP/2, compression, etc.
find_library(SSH_LIB ssh)
find_library(NGHTTP2_LIB nghttp2)
find_library(ZSTD_LIB_CURL zstd)
find_library(BROTLI_COMMON_LIB brotlicommon)
find_library(BROTLI_DEC_LIB_CURL brotlidec)
if(SSH_LIB)
    message(STATUS "Found libssh for CURL: ${SSH_LIB}")
endif()
if(NGHTTP2_LIB)
    message(STATUS "Found nghttp2 for CURL: ${NGHTTP2_LIB}")
endif()
if(ZSTD_LIB_CURL)
    message(STATUS "Found zstd for CURL: ${ZSTD_LIB_CURL}")
endif()
if(BROTLI_COMMON_LIB AND BROTLI_DEC_LIB_CURL)
    message(STATUS "Found brotli for CURL: ${BROTLI_COMMON_LIB}, ${BROTLI_DEC_LIB_CURL}")
endif()
```

**Change 2: Link CURL dependencies (after line ~422):**
```cmake
# Link CURL dependencies if found (required for static libcurl)
if(SSH_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${SSH_LIB})
endif()
if(NGHTTP2_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${NGHTTP2_LIB})
endif()
if(ZSTD_LIB_CURL)
    target_link_libraries(friendly_lwm2m PRIVATE ${ZSTD_LIB_CURL})
endif()
if(BROTLI_COMMON_LIB AND BROTLI_DEC_LIB_CURL)
    target_link_libraries(friendly_lwm2m PRIVATE ${BROTLI_COMMON_LIB} ${BROTLI_DEC_LIB_CURL})
endif()
```

---

## Testing

### Verify Include Path Fix
```bash
# Rebuild to verify WppLogs.h is now found
cd build
cmake ..
make -j$(nproc)
```

Expected: WPP objects (10525-10529) should compile without "WppLogs.h: No such file or directory" errors.

### Verify CURL Dependencies Fix
```bash
# Check if libssh and nghttp2 are found
cmake .. | grep -E "(libssh|nghttp2|zstd|brotli)"
```

Expected output:
```
-- Found libssh for CURL: /usr/lib/x86_64-linux-gnu/libssh.so
-- Found nghttp2 for CURL: /usr/lib/x86_64-linux-gnu/libnghttp2.so
-- Found zstd for CURL: /usr/lib/x86_64-linux-gnu/libzstd.so
-- Found brotli for CURL: /usr/lib/x86_64-linux-gnu/libbrotlicommon.so, /usr/lib/x86_64-linux-gnu/libbrotlidec.so
```

### Full Build Test
```bash
# Clean rebuild
rm -rf build
mkdir build
cd build
cmake -DBUILD_ENV=DEV -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON \
      -DDTLS_PROVIDER_MBEDTLS=ON -DWITH_DTLS=ON \
      -DWITH_DELTA_FOTA=ON -DWITH_COMPRESSION=ON \
      -DWITH_MQTT=ON -DWITH_OPENWRT_EXTENDED=ON \
      -DWITH_WAN_FAILOVER=ON -DWITH_MULTIWAN_HEALTH=ON \
      -DWITH_WIFI_CLIENT_MGMT=ON -DWITH_WIFI_CHANNEL_OPT=ON \
      -DWITH_MATTER_BRIDGE=ON ..
make -j$(nproc)
```

Expected: Clean build with no compilation or linking errors.

---

## Technical Details

### Why Static libcurl Requires Dependencies

When using a **dynamic** libcurl (`.so`):
- Dependencies are resolved at runtime
- The .so file has references to libssh.so, nghttp2.so, etc.
- The linker doesn't need to know about these dependencies

When using a **static** libcurl (`.a`):
- All code is embedded into the final binary
- The linker needs explicit references to all dependency symbols
- Missing dependencies cause "undefined reference" errors

### CURL Build Features

Check what features your system's libcurl was built with:
```bash
curl-config --features
```

Common features that require additional libraries:
- **AsynchDNS**: May require c-ares
- **HTTP2**: Requires nghttp2
- **HTTPS-proxy**: Requires SSL library
- **SFTP**: Requires libssh
- **brotli**: Requires brotli libraries
- **zstd**: Requires zstd library

### Alternative Solution: Use Dynamic libcurl

If you want to avoid linking all dependencies explicitly, use dynamic libcurl:
```bash
# Install development package
sudo apt-get install libcurl4-openssl-dev

# CMake will prefer the dynamic library
find_package(CURL REQUIRED)
```

However, static linking provides:
- Single self-contained binary
- No runtime dependency issues
- Slightly better performance (no dynamic symbol resolution)

---

## Files Modified

1. **CMakeLists.txt**
   - Added `wpp/utils` to include directories (line ~400)
   - Added CURL dependency detection (after line 131)
   - Added CURL dependency linking (after line ~422)

---

## Dependencies Required

For successful build, ensure these packages are installed:

### Ubuntu/Debian:
```bash
sudo apt-get install \
    libssh-dev \
    libnghttp2-dev \
    libzstd-dev \
    libbrotli-dev \
    libcurl4-openssl-dev
```

### Fedora/RHEL:
```bash
sudo dnf install \
    libssh-devel \
    libnghttp2-devel \
    libzstd-devel \
    brotli-devel \
    libcurl-devel
```

---

## Summary

**Issue 1 - Missing Include Directory:**
- **Symptom**: `fatal error: WppLogs.h: No such file or directory`
- **Cause**: `wpp/utils` not in include path
- **Fix**: Added to `target_include_directories`

**Issue 2 - Static libcurl Dependencies:**
- **Symptom**: `undefined reference` to ssh, nghttp2, zstd, brotli functions
- **Cause**: Static libcurl requires explicit linking of all dependencies
- **Fix**: Added find_library and conditional linking for all CURL dependencies

Both issues are now resolved and the build should complete successfully.
