# Build Fix Summary for build38.log Issues

## Problem Analysis

The build errors in `build38.log` were caused by missing GSSAPI/Kerberos library dependencies when using static versions of `libcurl.a` and `libssh.a`.

### Error Details

1. **GSSAPI undefined references** - Both libssh and libcurl were compiled with GSSAPI support, requiring:
   - `gssapi_krb5`
   - `krb5`
   - `k5crypto`
   - `com_err`
   - `keyutils`

2. **OpenSSL linking order** - `SSL_get_error` undefined references indicate OpenSSL needs to be linked after other dependencies in the static linking chain.

## Changes Made

### 1. Root CMakeLists.txt (d:\Dev\Friendly-LwM2M-Client\CMakeLists.txt)

#### Added GSSAPI Library Detection (lines 133-152)
```cmake
# CURL dependencies (required when using static libcurl)
find_library(SSH_LIB ssh)
find_library(NGHTTP2_LIB nghttp2)
find_library(ZSTD_LIB_CURL zstd)
find_library(BROTLI_COMMON_LIB brotlicommon)
find_library(BROTLI_DEC_LIB_CURL brotlidec)
find_library(GSSAPI_KRB5_LIB gssapi_krb5)      # NEW
find_library(KRB5_LIB krb5)                     # NEW
find_library(K5CRYPTO_LIB k5crypto)             # NEW
find_library(COM_ERR_LIB com_err)               # NEW
find_library(KEYUTILS_LIB keyutils)             # NEW
```

#### Added GSSAPI Linking to friendly_lwm2m Library (lines 452-477)
```cmake
# Link CURL dependencies if found (required for static libcurl)
if(SSH_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${SSH_LIB})
endif()
# ... other dependencies ...
# GSSAPI/Kerberos dependencies (required by libssh and libcurl with GSSAPI support)
if(GSSAPI_KRB5_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${GSSAPI_KRB5_LIB})
endif()
if(KRB5_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${KRB5_LIB})
endif()
if(K5CRYPTO_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${K5CRYPTO_LIB})
endif()
if(COM_ERR_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${COM_ERR_LIB})
endif()
if(KEYUTILS_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${KEYUTILS_LIB})
endif()
```

#### Fixed test_firmware_update_object Linking (lines 601-632)
Added explicit GSSAPI/Kerberos dependencies to the test executable:
```cmake
target_link_libraries(test_firmware_update_object
    friendly_lwm2m
    GTest::gtest
    GTest::gtest_main
)
# Link CURL/SSH dependencies explicitly for static linking
if(SSH_LIB)
    target_link_libraries(test_firmware_update_object PRIVATE ${SSH_LIB})
endif()
if(GSSAPI_KRB5_LIB)
    target_link_libraries(test_firmware_update_object PRIVATE ${GSSAPI_KRB5_LIB})
endif()
# ... other GSSAPI dependencies ...
# OpenSSL must come last for proper symbol resolution
target_link_libraries(test_firmware_update_object PRIVATE OpenSSL::SSL OpenSSL::Crypto)
```

### 2. Examples CMakeLists.txt (d:\Dev\Friendly-LwM2M-Client\examples\CMakeLists.txt)

#### Added GSSAPI Library Detection (lines 51-69)
Same pattern as root CMakeLists.txt - added detection for GSSAPI/Kerberos libraries.

#### Added GSSAPI Linking to friendly_example_client (lines 190-218)
```cmake
# Link CURL dependencies if found (required for static libcurl)
if(SSH_LIB)
    target_link_libraries(friendly_example_client ${SSH_LIB})
endif()
# ... other dependencies ...
# GSSAPI/Kerberos dependencies (required by libssh and libcurl with GSSAPI support)
if(GSSAPI_KRB5_LIB)
    target_link_libraries(friendly_example_client ${GSSAPI_KRB5_LIB})
endif()
if(KRB5_LIB)
    target_link_libraries(friendly_example_client ${KRB5_LIB})
endif()
# ... other GSSAPI dependencies ...
```

## Why This Fix Works

1. **Static Linking Dependency Resolution**: When using static libraries (`.a` files), all transitive dependencies must be explicitly linked. Dynamic libraries (`.so`) handle this automatically, but static libraries require manual specification.

2. **GSSAPI is Optional**: libcurl and libssh can be compiled with or without GSSAPI support. The Ubuntu/Debian packages use GSSAPI for Kerberos authentication, so when using static versions, these dependencies must be provided.

3. **Proper Linking Order**: In static linking, the order matters:
   - Libraries that use symbols must come before libraries that provide them
   - OpenSSL must come last because both libcurl and libssh use it
   - GSSAPI libraries must come after libssh/libcurl but before OpenSSL

4. **Conditional Linking**: The fix uses `if()` checks so the build still works on systems without GSSAPI (e.g., minimal builds, embedded systems).

## Testing the Fix

To test the fix on your Linux build system (WSL/Docker/remote):

```bash
cd /path/to/Friendly-LwM2M-Client
rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

Expected result:
- No more "undefined reference to" errors for GSSAPI functions
- No more "undefined reference to `SSL_get_error`" errors
- Successful linking of `friendly_example_client` and `test_firmware_update_object`

## Additional Notes

- This fix only affects static linking builds
- If you switch to dynamic linking (`DTLS_STATIC_LINK=OFF`), these dependencies are handled automatically
- The fix maintains backward compatibility - builds without GSSAPI still work
- All libraries are linked as PRIVATE to avoid polluting the public API

## Files Modified

1. `CMakeLists.txt` - Root build configuration
2. `examples/CMakeLists.txt` - Example client build configuration

## Verification Checklist

- [ ] Build completes without linker errors
- [ ] `friendly_example_client` links successfully
- [ ] `test_firmware_update_object` links successfully
- [ ] All tests pass: `make test` or `ctest`
- [ ] No warnings about missing GSSAPI libraries (unless intentionally absent)
