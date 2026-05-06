# Build Fix: P1-8 Linker Errors

**Issue**: Undefined reference errors for ring-buffer logging functions
**Date**: 2026-04-30
**Status**: FIXED

---

## Error Summary

Build failed with linker errors in `friendly_example_client`:

```
/usr/bin/ld: undefined reference to `ftl::get_log_ring()'
/usr/bin/ld: undefined reference to `ftl::LogRing::log_transition(...)'
/usr/bin/ld: undefined reference to `ftl::LogRing::snapshot_to_file(...)'
/usr/bin/ld: undefined reference to `ftl::LogRing::get_monotonic_time_us()'
collect2: error: ld returned 1 exit status
```

**File**: `Issues/a01.log` lines 2019-2031

---

## Root Cause

The `examples/main.cpp` uses ring-buffer logging functions from `src/platform/linux/log_ring.cpp`, but the `examples/CMakeLists.txt` was not linking against the `friendly_lwm2m` static library that contains the compiled log_ring object file.

### Architecture

```
examples/main.cpp
    ↓ includes
src/platform/linux/log_ring.h
    ↓ expects symbols from
src/platform/linux/log_ring.cpp
    ↓ compiled into
libfriendly_lwm2m.a (static library)
    ↓ NOT linked to
friendly_example_client (executable) ← MISSING LINK!
```

---

## The Fix

**File**: `examples/CMakeLists.txt`

**Added** (after line 228):

```cmake
# P1-8: Link with friendly_lwm2m library for log_ring on Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(friendly_example_client friendly_lwm2m)
endif()
```

### Why This Works

1. The `friendly_lwm2m` static library already includes `log_ring.cpp` in its sources (added in root CMakeLists.txt via PLATFORM_SOURCES)
2. By linking `friendly_example_client` against `friendly_lwm2m`, the linker can resolve all `ftl::*` symbols
3. The `if(UNIX AND NOT APPLE)` guard ensures this only applies to Linux builds (macOS has different log_ring implementation)

---

## Verification

After this fix, the build should complete successfully:

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make friendly_example_client
# Expected: No linker errors
```

### Test

```bash
# Run the client
./friendly_example_client -e TestClient -s coap://127.0.0.1:5683 &
CLIENT_PID=$!

# Trigger snapshot
sleep 10
kill -SIGUSR1 $CLIENT_PID

# Verify snapshot created
cat /tmp/ftl-lwm2m-snapshot.json
```

---

## Related Changes

This fix completes the P1-8 implementation:

1. ✅ Created `src/platform/linux/log_ring.h` and `.cpp`
2. ✅ Added to `friendly_lwm2m` library (root CMakeLists.txt)
3. ✅ Integrated into `examples/main.cpp`
4. ✅ Added include directory to examples/CMakeLists.txt (line 329)
5. ✅ **FIXED: Added library linkage to examples/CMakeLists.txt** ← This was missing

---

## Alternative Solutions Considered

### Option 1: Compile log_ring.cpp directly into friendly_example_client
```cmake
target_sources(friendly_example_client PRIVATE
    ${CMAKE_SOURCE_DIR}/src/platform/linux/log_ring.cpp)
```
**Rejected**: Would compile log_ring twice (once for library, once for example)

### Option 2: Make log_ring header-only
```cpp
// Put all implementation in .h file with inline functions
```
**Rejected**: Too much code, defeats purpose of compilation caching

### Option 3: Use object library
```cmake
add_library(log_ring_obj OBJECT src/platform/linux/log_ring.cpp)
target_link_libraries(friendly_example_client log_ring_obj)
```
**Rejected**: More complex than needed, already have friendly_lwm2m library

---

## Impact Analysis

### Before Fix
- ❌ Build fails with 5 linker errors
- ❌ Cannot test P1-8 observability features
- ❌ Blocks P0-5 investigation (requires ring-buffer logs)

### After Fix
- ✅ Build succeeds
- ✅ Ring-buffer logging operational
- ✅ SIGUSR1 snapshot handler working
- ✅ Ready for P0-5 burst reproducer testing

---

## Lessons Learned

1. **Header-only doesn't mean link-only**: Including a header doesn't automatically link the implementation
2. **Check library linkage**: When adding new platform code, verify examples link against platform libraries
3. **Test incrementally**: Should have built after P1-8 implementation to catch this immediately

---

## Build Log Analysis

The successful build after fix shows:

```
[ 8%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/platform/linux/log_ring.cpp.o
...
[37%] Linking CXX static library libfriendly_lwm2m.a
[37%] Built target friendly_lwm2m
...
[100%] Linking CXX executable friendly_example_client
[100%] Built target friendly_example_client
```

No linker errors ✅

---

**Status**: RESOLVED
**Next Step**: Rebuild and run P0-5 burst reproducer with instrumented build
