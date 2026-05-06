# Fix for err05.txt - CMake Signature Mismatch Error

## Problem

CMake configuration failed with the following error:

```
CMake Error at cmake/DTLSOptimization.cmake:233 (target_link_libraries):
  The plain signature for target_link_libraries has already been used with
  the target "test_dtls_providers".  All uses of target_link_libraries with a
  target must be either all-keyword or all-plain.

  The uses of the plain signature are here:

   * CMakeLists.txt:682 (target_link_libraries)

Call Stack (most recent call first):
  CMakeLists.txt:689 (target_link_dtls_optimized)
```

Same error for `test_dtls_integration` at line 744.

## Root Cause

CMake requires consistency in how `target_link_libraries()` is called for each target. There are two signatures:

1. **Plain signature** (old style):
   ```cmake
   target_link_libraries(my_target
       lib1
       lib2
   )
   ```

2. **Keyword signature** (modern style):
   ```cmake
   target_link_libraries(my_target
       PRIVATE lib1
       PUBLIC lib2
   )
   ```

The test targets were using the **plain signature** (line 682, 744):
```cmake
target_link_libraries(test_dtls_providers
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)
```

But `target_link_dtls_optimized()` function uses the **keyword signature** (DTLSOptimization.cmake:233):
```cmake
target_link_libraries(${target_name} PRIVATE ${DTLS_LIBRARIES})
```

CMake doesn't allow mixing both signatures for the same target.

## Solution

Changed the test targets to use the keyword signature to match `target_link_dtls_optimized()`.

### File: CMakeLists.txt

**Location 1: test_dtls_providers (line ~682)**

Before:
```cmake
target_link_libraries(test_dtls_providers
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)
```

After:
```cmake
target_link_libraries(test_dtls_providers
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)
```

**Location 2: test_dtls_integration (line ~744)**

Before:
```cmake
target_link_libraries(test_dtls_integration
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)
```

After:
```cmake
target_link_libraries(test_dtls_integration
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)
```

## Why PRIVATE?

The `PRIVATE` keyword means these dependencies are:
- Only needed by this target internally
- Not propagated to targets that link against this one
- Appropriate for test executables (they don't expose interfaces)

This is the correct choice for test executables since:
1. Tests are final executables, not libraries
2. No other targets link against test executables
3. Dependencies don't need to be transitive

## Benefits

1. **Consistent**: All `target_link_libraries()` calls for these targets now use keyword signature
2. **Modern**: Keyword signature is the recommended CMake style (3.0+)
3. **Clear**: Explicitly states dependency visibility (PRIVATE vs PUBLIC vs INTERFACE)
4. **Compatible**: Works with `target_link_dtls_optimized()` helper function

## CMake Best Practices

For modern CMake (3.0+), always use the keyword signature:
- `PRIVATE`: Dependency only used internally
- `PUBLIC`: Dependency used internally AND exposed in public interface
- `INTERFACE`: Dependency only in public interface (header-only libs)

## Verification

After this fix, CMake configuration should complete successfully:

```bash
cmake -B build -DBUILD_ENV=DEV
cmake --build build
```

Expected result:
- No signature mismatch errors
- All targets configure correctly
- Build proceeds to compilation

## Related Fixes

This is the third in a series of DTLS optimization fixes:

1. **err03_fix.md**: Fixed SHA256 undefined reference (OpenSSL::Crypto for checksums)
2. **err04_fix.md**: Fixed test target DTLS library linking (use target_link_dtls_optimized)
3. **err05_fix.md** (this document): Fixed CMake signature mismatch (PRIVATE keyword)

## Next Steps

1. Complete CMake configuration
2. Build all targets
3. Run tests to verify DTLS providers work
4. Test all four build environments (TEST, DEV, PREPROD, PROD)
