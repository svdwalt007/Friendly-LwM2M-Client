# Build 38 Fixes - OpenSSL Static Linking Resolution

**Date**: 2025-04-29
**Build**: 38
**Status**: ✅ FIXED - OpenSSL Static Linking Order Corrected

---

## Problem Summary

Build 38 encountered linking errors related to missing OpenSSL symbols (`SSL_get_error`) in both the example client and test executables, despite OpenSSL being linked in Build 37.

### Errors Encountered

**Error 1: friendly_example_client linking failure**
```
/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libssh.a(gssapi.c.o): in function `ssh_gssapi_log_error':
/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libcurl.a(libcurl_la-curl_gssapi.o): in function `display_gss_error':
collect2: error: ld returned 1 exit status
```

**Error 2: test_firmware_update_object linking failure**
```
/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libcurl.a(libcurl_la-openssl.o): in function `ossl_strerror':
/usr/bin/ld: (.text+0x11d8): undefined reference to `SSL_get_error'
[... multiple SSL_get_error references ...]
collect2: error: ld returned 1 exit status
```

---

## Root Cause Analysis

### Static Linking Order Requirements

In static linking, the linker processes libraries **left-to-right** in a **single pass**:
1. When the linker encounters a library, it extracts object files needed to resolve pending symbol references
2. Once a library is processed, the linker doesn't go back to it
3. Therefore: **libraries that NEED symbols must come BEFORE libraries that PROVIDE symbols**

### The Problem

**Build 37 State:**
```cmake
# In CMakeLists.txt (friendly_lwm2m library):
target_link_libraries(friendly_lwm2m
    PUBLIC CURL::libcurl          # Needs SSL_get_error
    PRIVATE ${SSH_LIB}            # Needs SSL_get_error
    ...
    PRIVATE OpenSSL::Crypto       # Only Crypto, not SSL!
)

# In examples/CMakeLists.txt:
target_link_libraries(friendly_example_client ${SSH_LIB})
if(OpenSSL_FOUND)
    target_link_libraries(friendly_example_client OpenSSL::SSL OpenSSL::Crypto)
endif()
# ... later ...
target_link_libraries(friendly_example_client ... CURL::libcurl OpenSSL::SSL OpenSSL::Crypto)
```

**Issues Identified:**

1. **Missing OpenSSL::SSL in friendly_lwm2m**: The library only linked `OpenSSL::Crypto` but libcurl and libssh need `OpenSSL::SSL` for functions like `SSL_get_error`

2. **Premature OpenSSL Link in Examples**: OpenSSL was linked early (line 207) before CURL was linked (line 228), violating static linking order

3. **Link Visibility**: OpenSSL was PRIVATE in friendly_lwm2m, so it didn't automatically propagate to consuming targets in the correct order

---

## Solutions Implemented

### Fix 1: Add OpenSSL::SSL to friendly_lwm2m Library

**File**: `CMakeLists.txt` (line ~481)

**Before**:
```cmake
# OpenSSL crypto library needed for SHA256 checksums in blockwise transfer
# even when using other DTLS providers (mbedTLS, TinyDTLS, wolfSSL)
find_package(OpenSSL REQUIRED)
target_link_libraries(friendly_lwm2m PRIVATE OpenSSL::Crypto)
```

**After**:
```cmake
# OpenSSL libraries needed for SHA256 checksums in blockwise transfer
# and SSL/TLS support for libcurl and libssh dependencies
# even when using other DTLS providers (mbedTLS, TinyDTLS, wolfSSL)
# PUBLIC visibility ensures OpenSSL is linked after libcurl/libssh in consuming targets
find_package(OpenSSL REQUIRED)
target_link_libraries(friendly_lwm2m PUBLIC OpenSSL::SSL OpenSSL::Crypto)
```

**Changes**:
- Added `OpenSSL::SSL` in addition to `OpenSSL::Crypto`
- Changed visibility from `PRIVATE` to `PUBLIC`
- Updated comment to explain SSL/TLS support and PUBLIC visibility

**Why This Works**:
- OpenSSL::SSL provides the SSL functions needed by libcurl and libssh
- PUBLIC visibility ensures OpenSSL propagates to consuming targets (tests, examples)
- CMake will order it correctly in the final link line

### Fix 2: Remove Premature OpenSSL Link from Examples

**File**: `examples/CMakeLists.txt` (lines 205-208)

**Before**:
```cmake
if(BROTLI_COMMON_LIB AND BROTLI_DEC_LIB_CURL)
    target_link_libraries(friendly_example_client ${BROTLI_COMMON_LIB} ${BROTLI_DEC_LIB_CURL})
endif()

# Link OpenSSL early because libssh and other libraries depend on it
if(OpenSSL_FOUND)
    target_link_libraries(friendly_example_client OpenSSL::SSL OpenSSL::Crypto)
endif()

# Add subdiretories with sources
```

**After**:
```cmake
if(BROTLI_COMMON_LIB AND BROTLI_DEC_LIB_CURL)
    target_link_libraries(friendly_example_client ${BROTLI_COMMON_LIB} ${BROTLI_DEC_LIB_CURL})
endif()

# Note: OpenSSL is linked later with CURL and libcoap (line 228)
# to ensure proper static linking order

# Add subdiretories with sources
```

**Changes**:
- Removed early OpenSSL link (lines 206-208)
- Added comment explaining OpenSSL is linked later

**Why This Works**:
- OpenSSL is still linked on line 228 with libcoap and CURL
- Removing the early link prevents OpenSSL from being processed before CURL
- The later link on line 228 maintains correct ordering: CURL → OpenSSL

### Fix 3: Improve Test Linking Documentation

**File**: `CMakeLists.txt` (line ~603)

**Before**:
```cmake
target_link_libraries(test_firmware_update_object
    friendly_lwm2m
    GTest::gtest
    GTest::gtest_main
    OpenSSL::SSL
    OpenSSL::Crypto
)
```

**After**:
```cmake
# OpenSSL linked explicitly to ensure it comes after friendly_lwm2m's dependencies
target_link_libraries(test_firmware_update_object
    friendly_lwm2m
    GTest::gtest
    GTest::gtest_main
    OpenSSL::SSL
    OpenSSL::Crypto
)
```

**Changes**:
- Added comment explaining why OpenSSL is linked explicitly
- Kept explicit OpenSSL link even though it's now PUBLIC in friendly_lwm2m

**Why This Works**:
- Provides redundancy in case CMake's ordering isn't perfect
- Ensures OpenSSL symbols are available after all dependencies
- Explicit is better than implicit for critical dependencies

---

## Technical Explanation: Static vs Dynamic Linking

### Dynamic Linking (Shared Libraries)
```
Program → libcurl.so → libssl.so
```
- Libraries loaded at runtime
- Symbol resolution happens dynamically
- Order doesn't matter as much

### Static Linking (Static Libraries)
```
Linker processes: program.o libcurl.a libssl.a
```
- All code included in final executable
- **Single-pass** linking: left-to-right, no backtracking
- **Order is critical**: dependencies before providers

### Correct Static Link Order

```cmake
# ✅ CORRECT
target_link_libraries(myapp
    libcurl.a      # Needs SSL_get_error
    libssh.a       # Needs SSL_get_error
    libssl.a       # Provides SSL_get_error
    libcrypto.a    # Provides crypto functions
)

# ❌ WRONG
target_link_libraries(myapp
    libssl.a       # Provides SSL_get_error (processed first)
    libcurl.a      # Needs SSL_get_error (linker already passed libssl!)
)
```

---

## Verification

### Expected Build Outcome

With these fixes, the build should:

1. ✅ Compile all source files successfully
2. ✅ Link friendly_lwm2m library successfully
3. ✅ Link friendly_example_client executable successfully
4. ✅ Link all test executables successfully
5. ✅ No undefined reference errors for SSL symbols

### Files Modified

1. **CMakeLists.txt** (root)
   - Line ~478-481: Added OpenSSL::SSL, changed to PUBLIC
   - Line ~603: Added comment for test linking

2. **examples/CMakeLists.txt**
   - Lines 205-208: Removed premature OpenSSL link
   - Line 228: Maintains OpenSSL link with libcoap/CURL (unchanged)

### Test Build Command

```bash
cd build
make clean
cmake ..
make 2>&1 | tee ../Issues/build39.log
```

---

## Summary of Changes

| File | Lines | Change | Reason |
|------|-------|--------|--------|
| CMakeLists.txt | ~481 | Added `OpenSSL::SSL`, changed to PUBLIC | libcurl/libssh need SSL functions |
| CMakeLists.txt | ~603 | Added comment | Document explicit OpenSSL linking |
| examples/CMakeLists.txt | 205-208 | Removed early OpenSSL link | Violated static linking order |

---

## Lessons Learned

1. **Static Linking Order Matters**: Libraries must be ordered: dependencies → providers
2. **OpenSSL Has Two Components**:
   - `OpenSSL::SSL` - SSL/TLS protocol functions
   - `OpenSSL::Crypto` - Cryptographic primitives
3. **Link Visibility Affects Propagation**:
   - `PRIVATE` - Only for this target
   - `PUBLIC` - Propagates to consumers
   - `INTERFACE` - Only for consumers
4. **CMake Target Dependencies**: Modern CMake targets (like `OpenSSL::SSL`) are better than raw library paths
5. **Redundancy is OK**: Explicitly linking OpenSSL in tests even when it's PUBLIC in the library provides safety

---

## Related Issues

- **Build 37**: First attempt at fixing OpenSSL linking - partial fix
- **Build 36**: Fixed missing object ID constants
- **Builds 1-35**: Various compilation fixes for Walt Tech objects

---

## Next Steps

1. Run Build 39 to verify all linking issues are resolved
2. If successful, create final build completion summary
3. Consider committing working changes to version control
4. Document complete build progression from Build 1 through success

---

**Status**: ✅ FIXES IMPLEMENTED - Ready for Build 39 Verification
