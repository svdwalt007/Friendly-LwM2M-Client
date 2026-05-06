# Fix for err04.txt - DTLS Test Linking Errors

## Problem

The `test_dtls_providers` and `test_dtls_integration` test executables failed to link with mbedTLS library errors:

```
undefined reference to `mbedtls_ssl_read'
undefined reference to `mbedtls_ssl_write'
undefined reference to `mbedtls_ssl_handshake'
... (45+ undefined references)
```

## Root Cause

The test targets had manual, conditional DTLS library linking that checked for `MbedTLS_FOUND`:

```cmake
if(DTLS_PROVIDER_MBEDTLS AND MbedTLS_FOUND)
    if(DTLS_STATIC_LINK)
        target_link_libraries(test_dtls_providers MbedTLS::mbedtls MbedTLS::mbedx509 MbedTLS::mbedcrypto)
    else()
        target_link_libraries(test_dtls_providers mbedtls mbedx509 mbedcrypto)
    endif()
endif()
```

However, the `DTLSOptimization.cmake` module doesn't set the `MbedTLS_FOUND` variable - it uses `find_library()` directly, not `find_package(MbedTLS)`. This caused the condition to fail even when mbedTLS was available and enabled.

## Solution

Replaced the manual, provider-specific linking with calls to the `target_link_dtls_optimized()` helper function from `DTLSOptimization.cmake`:

### Before:
```cmake
# Link required libraries
target_link_libraries(test_dtls_providers
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)

# Add DTLS provider libraries
if(DTLS_PROVIDER_MBEDTLS AND MbedTLS_FOUND)
    if(DTLS_STATIC_LINK)
        target_link_libraries(test_dtls_providers MbedTLS::mbedtls MbedTLS::mbedx509 MbedTLS::mbedcrypto)
    else()
        target_link_libraries(test_dtls_providers mbedtls mbedx509 mbedcrypto)
    endif()
endif()

if(DTLS_PROVIDER_OPENSSL)
    target_link_libraries(test_dtls_providers OpenSSL::SSL OpenSSL::Crypto)
endif()

if(DTLS_PROVIDER_WOLFSSL)
    target_link_libraries(test_dtls_providers wolfssl)
endif()

if(DTLS_PROVIDER_TINYDTLS)
    target_sources_tinydtls(test_dtls_providers)
endif()
```

### After:
```cmake
# Link required libraries
target_link_libraries(test_dtls_providers
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)

# Use optimized DTLS linking (automatically links all enabled providers)
target_link_dtls_optimized(test_dtls_providers)

# Add TinyDTLS sources if enabled
if(DTLS_PROVIDER_TINYDTLS)
    target_sources_tinydtls(test_dtls_providers)
endif()
```

## Changes Made

### File: CMakeLists.txt

**Location 1: test_dtls_providers target (lines ~682-708)**
- Removed manual DTLS library linking with MbedTLS_FOUND checks
- Replaced with single call to `target_link_dtls_optimized(test_dtls_providers)`
- Reduced code from ~27 lines to ~9 lines
- Now automatically links all enabled DTLS providers with optimization flags

**Location 2: test_dtls_integration target (lines ~744-770)**
- Applied identical fix
- Replaced manual DTLS library linking with `target_link_dtls_optimized(test_dtls_integration)`
- Ensures consistency across all test targets

## Benefits

1. **Consistency**: Test targets now use the same DTLS linking logic as the main library
2. **Automatic**: All enabled DTLS providers are automatically linked
3. **Optimized**: Test executables get the same optimization flags (--gc-sections, --as-needed)
4. **Maintainable**: Single source of truth for DTLS linking (DTLSOptimization.cmake)
5. **Correct**: Works regardless of how libraries were found (find_package vs find_library)

## Verification

After this fix, the build should complete successfully:

```bash
cmake --build build
```

Expected result:
- `test_dtls_providers` compiles and links successfully
- `test_dtls_integration` compiles and links successfully
- All mbedTLS symbols are resolved
- OpenSSL, wolfSSL, and TinyDTLS providers (if enabled) also link correctly

## Related Fixes

This is the second in a series of DTLS optimization fixes:

1. **err03_fix.md**: Fixed SHA256 undefined reference by linking OpenSSL::Crypto for non-DTLS crypto operations
2. **err04_fix.md** (this document): Fixed test target DTLS library linking

## Next Steps

1. Complete build and verify all tests compile
2. Run tests to ensure DTLS providers work correctly
3. Verify binary size optimizations are effective
4. Test all four build environments (TEST, DEV, PREPROD, PROD)
