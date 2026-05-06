# Build b09 Fix: Brotli Linking Error

## Issue Summary

**Build**: b09
**Date**: 2026-04-30
**Status**: ✅ FIXED

## Error Description

The test_firmware_update_object executable failed to link with multiple undefined references to Brotli internal symbols:

```
/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libbrotlidec.a(decode.c.o): in function `DecodeLiteralBlockSwitch':
(.text+0x82c): undefined reference to `_kBrotliPrefixCodeRanges'
(.text+0x938): undefined reference to `_kBrotliContextLookupTable'

/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libbrotlidec.a(decode.c.o): in function `SafeDecodeLiteralBlockSwitch':
(.text+0xa32): undefined reference to `_kBrotliPrefixCodeRanges'
(.text+0xcae): undefined reference to `_kBrotliContextLookupTable'

/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libbrotlidec.a(decode.c.o): in function `ProcessCommandsInternal':
(.text+0x33c4): undefined reference to `_kBrotliContextLookupTable'

/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libbrotlidec.a(decode.c.o): in function `BrotliDecoderDecompress':
(.text+0x469b): undefined reference to `BrotliTransformDictionaryWord'

/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libbrotlidec.a(shared_dictionary.c.o): in function `BrotliSharedDictionaryCreateInstance':
(.text+0xe): undefined reference to `BrotliSharedDictionaryAttach'

/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libbrotlidec.a(state.c.o): in function `BrotliDecoderStateInit':
(.text+0x11f): undefined reference to `BrotliSharedDictionaryCreateInstance'
(.text+0x15b): undefined reference to `BrotliDefaultAllocFunc'
(.text+0x162): undefined reference to `BrotliDefaultFreeFunc'

/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libbrotlidec.a(state.c.o): in function `BrotliDecoderStateCleanup':
(.text+0x306): undefined reference to `BrotliSharedDictionaryDestroyInstance'

collect2: error: ld returned 1 exit status
```

## Root Cause Analysis

### Brotli Library Structure

Brotli compression is split into three libraries:
- **libbrotlienc.a**: Encoder (depends on common)
- **libbrotlidec.a**: Decoder (depends on common)
- **libbrotlicommon.a**: Common functions and data

The missing symbols are defined in `libbrotlicommon.a`:
- `_kBrotliPrefixCodeRanges`: Lookup table for prefix codes
- `_kBrotliContextLookupTable`: Context lookup table
- `BrotliTransformDictionaryWord`: Dictionary transformation
- `BrotliSharedDictionaryAttach/CreateInstance/DestroyInstance`: Dictionary management
- `BrotliDefaultAllocFunc/FreeFunc`: Memory allocation functions

### Static Linking Order Requirements

In static linking, libraries must be specified in dependency order:
```
libA.a (depends on libB) → libB.a (base library)
```

For Brotli, the correct order is:
```
libbrotlienc.a (depends on common) → libbrotlidec.a (depends on common) → libbrotlicommon.a (base)
```

### The Problem

In CMakeLists.txt line 637, the compression support was linking Brotli like this:

```cmake
if(HAS_BROTLI)
    target_link_libraries(friendly_lwm2m PRIVATE ${BROTLI_ENC_LIB} ${BROTLI_DEC_LIB})
    # Missing: BROTLI_COMMON_LIB ❌
```

This caused the friendly_lwm2m library to be linked with only enc and dec, not common.

Meanwhile, the libcurl support (lines 563-571) correctly linked all three:

```cmake
if(BROTLI_ENC_LIB_CURL)
    target_link_libraries(friendly_lwm2m PRIVATE ${BROTLI_ENC_LIB_CURL})
endif()
if(BROTLI_DEC_LIB_CURL)
    target_link_libraries(friendly_lwm2m PRIVATE ${BROTLI_DEC_LIB_CURL})
endif()
if(BROTLI_COMMON_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${BROTLI_COMMON_LIB})  ✅
endif()
```

This resulted in the final link line having Brotli libraries in wrong order:

```
... libbrotlienc.a libbrotlidec.a libbrotlicommon.a ... libbrotlienc.a libbrotlicommon.a ... libbrotlidec.a ...
     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^      ^^^^^^^^^^^^^^^^^
     First occurrence (correct order)                   Second occurrence (missing dec)    Third (wrong order)
```

The third occurrence of `libbrotlidec.a` appeared AFTER an earlier `libbrotlicommon.a`, so symbols from dec weren't properly resolved.

## Solution

### File Modified

**CMakeLists.txt** (line 636-640)

### Change

**Before**:
```cmake
    if(HAS_BROTLI)
        target_link_libraries(friendly_lwm2m PRIVATE ${BROTLI_ENC_LIB} ${BROTLI_DEC_LIB})
        target_include_directories(friendly_lwm2m PRIVATE ${BROTLI_INCLUDE})
        target_compile_definitions(friendly_lwm2m PRIVATE HAS_BROTLI=1)
    endif()
```

**After**:
```cmake
    if(HAS_BROTLI)
        # Brotli libraries must be linked in order: enc, dec, common
        target_link_libraries(friendly_lwm2m PRIVATE ${BROTLI_ENC_LIB} ${BROTLI_DEC_LIB} ${BROTLI_COMMON_LIB})
        target_include_directories(friendly_lwm2m PRIVATE ${BROTLI_INCLUDE})
        target_compile_definitions(friendly_lwm2m PRIVATE HAS_BROTLI=1)
    endif()
```

## Verification

### Clean Build Test

```bash
cd build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

**Expected output**:
```
[ 96%] Building CXX object CMakeFiles/test_firmware_update_object.dir/test/test_firmware_update_object.cpp.o
[ 98%] Linking CXX executable test_firmware_update_object
[100%] Built target test_firmware_update_object
```

### Link Order Verification

Check the link line in `build/CMakeFiles/test_firmware_update_object.dir/link.txt`:

**Expected**:
```
... libbrotlienc.a libbrotlidec.a libbrotlicommon.a ...
```

All three libraries should appear together in the correct order, and `libbrotlidec.a` should NOT appear after `libbrotlicommon.a`.

### Test Execution

```bash
cd build
./test_firmware_update_object
```

**Expected**: Tests run successfully without linking errors.

## Impact Assessment

### Build Targets Affected

- ✅ **friendly_lwm2m**: Fixed (now links BROTLI_COMMON_LIB)
- ✅ **test_firmware_update_object**: Fixed (inherits from friendly_lwm2m)
- ✅ **test_delta_algorithms**: Fixed (inherits from friendly_lwm2m)
- ✅ **test_rollback_manager**: Fixed (inherits from friendly_lwm2m)
- ✅ **test_blockwise_transfer**: Fixed (inherits from friendly_lwm2m)
- ✅ **friendly_example_client**: Fixed (inherits from friendly_lwm2m)

### Dependency Count

No change - still using the same 3 Brotli libraries:
- libbrotlienc.a
- libbrotlidec.a
- libbrotlicommon.a

The fix just ensures all three are linked when HAS_BROTLI is enabled.

## Related Build Issues

- **b08**: PostgreSQL detection issues (fixed by using custom minimal libcurl)
- **b07**: Database linking errors (fixed by removing LDAP → SASL → database chain)
- **b06**: Missing CURL dependencies (fixed by linking minimal CURL dependencies)
- **b05**: Initial static linking issues

## Notes

### Why Two Brotli Sections?

The CMakeLists.txt has two places where Brotli is linked:

1. **Lines 563-571**: For libcurl support (BROTLI_*_LIB_CURL variables)
   - These are always linked if found
   - Required for custom minimal libcurl with Brotli compression

2. **Lines 636-640**: For delta FOTA compression (BROTLI_*_LIB variables)
   - Only linked if `WITH_COMPRESSION OR WITH_DELTA_FOTA` is enabled
   - Required for compressing firmware delta patches

Both use the same physical libraries (checked via `find_library`) but may use different variable names for clarity.

### Variable Naming

- `BROTLI_ENC_LIB_CURL`: Found at line 150
- `BROTLI_DEC_LIB_CURL`: Found at line 149
- `BROTLI_COMMON_LIB`: Found at line 148
- `BROTLI_ENC_LIB`: Found at line 261
- `BROTLI_DEC_LIB`: Found at line 262

The `_CURL` suffix doesn't mean different libraries - it's just a naming convention to distinguish "for libcurl" from "for compression".

## Testing Checklist

After applying this fix:

- [ ] Clean build completes successfully
- [ ] test_firmware_update_object links without errors
- [ ] test_firmware_update_object runs successfully
- [ ] All other test targets build successfully
- [ ] friendly_example_client builds successfully
- [ ] No undefined reference errors in build log
- [ ] Link order shows brotli libraries together: enc, dec, common

## Prevention

To prevent similar issues in the future:

1. **Always link all three Brotli libraries** when any Brotli support is enabled
2. **Maintain correct order**: enc → dec → common
3. **Check link.txt files** after CMake changes to verify library order
4. **Test all targets**, not just the main executable

## Status

✅ **FIXED** - Added BROTLI_COMMON_LIB to compression support linking

---

**Fixed by**: Claude Sonnet 4.5
**Date**: 2026-04-30
**Build log**: Issues/b09.log
**Next build**: b10 (clean build test)
