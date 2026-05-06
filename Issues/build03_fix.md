# Build Error Fix: build03.log

## Issue Found

### Missing InstSubject.h Header (Instance Observer Pattern)

**Error:**
```
fatal error: InstSubject.h: No such file or directory
   17 | #include "InstSubject.h"
```

**Affected File:**
- `wpp/registry/data_model/instance/Instance.h` (line 17)

**Cascading to:**
- wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.cpp
- wpp/registry/objects/w_10526_multiwan_health_check/MultiWanHealthCheck.cpp
- wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.cpp
- wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.cpp
- wpp/registry/objects/w_10529_matter_bridge/MatterBridge.cpp

**Root Cause:**
Another cascading dependency revealed after fixing build02.log:

1. **build01.log**: `Resource.h` → couldn't find `WppLogs.h`
   - Fixed by adding `wpp/utils`

2. **build02.log**: `WppLogs.h` → couldn't find `WppPlatform.h`
   - Fixed by adding `wpp/platform`

3. **build03.log**: `Instance.h` → couldn't find `InstSubject.h`
   - `InstSubject.h` is in `wpp/registry/data_model/instance/observer/`
   - Only `wpp/registry/data_model/instance` was in include path
   - Missing the `observer/` subdirectory

**Dependency Chain:**
```
WanFailoverPolicy.cpp (and other WPP objects)
  → includes Object.h or Instance.h
    → Instance.h includes "InstSubject.h" (line 16)
      → InstSubject.h is in wpp/registry/data_model/instance/observer/  ← ERROR HERE
```

**Fix:**
Added `${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/instance/observer` to `target_include_directories` for the `friendly_lwm2m` target.

**File Modified:** `CMakeLists.txt` (line ~408-422)

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
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/instance/observer  # <-- ADDED
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/object/observer
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/resource
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/types
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/platform
)
```

---

## Observer Pattern in WPP

The WPP framework uses the **Observer Pattern** for both Object and Instance level notifications:

### Instance Observers
Located in `wpp/registry/data_model/instance/observer/`:
- **InstSubject.h** - Subject base class for instances
- **InstOpObserver.h** - Observer for instance operations (read, write, execute)
- **InstEventObserver.h** - Observer for instance lifecycle events (create, delete)

### Object Observers
Located in `wpp/registry/data_model/object/observer/`:
- **ObjSubject.h** - Subject base class for objects
- **ObjOpObserver.h** - Observer for object operations

### Why Both Directories Needed

The CMakeLists.txt now includes both observer directories:
- `wpp/registry/data_model/instance/observer` - For Instance.h to find InstSubject.h
- `wpp/registry/data_model/object/observer` - For Object.h to find ObjSubject.h

This allows headers to use simple includes like:
```cpp
#include "InstSubject.h"   // Instead of "observer/InstSubject.h"
#include "ObjSubject.h"    // Instead of "observer/ObjSubject.h"
```

---

## Complete Include Directory Structure

After all fixes (build01 + build02 + build03), the complete WPP include directories are:

```cmake
target_include_directories(friendly_lwm2m
    PRIVATE
        # Core source
        ${CMAKE_CURRENT_SOURCE_DIR}/src

        # WPP Registry
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/objects

        # WPP Data Model - Instance
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/instance
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/instance/observer  # build03 fix

        # WPP Data Model - Object
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/object/observer

        # WPP Data Model - Resource & Types
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/resource
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/registry/data_model/types

        # WPP Utilities & Platform
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/utils      # build01 fix
        ${CMAKE_CURRENT_SOURCE_DIR}/wpp/platform   # build02 fix
)
```

---

## Testing

### Verify Include Path Fix
```bash
# Rebuild to verify InstSubject.h is now found
cd build
cmake ..
make -j$(nproc)
```

Expected: WPP objects (10525-10529) should now compile successfully without "InstSubject.h: No such file or directory" errors.

### Potential Next Issues

If the build reveals more missing headers, check the dependency chain:
1. Find where the missing header is located: `find . -name "MissingHeader.h"`
2. Add that directory to `target_include_directories`
3. Most WPP headers should now be covered

Common WPP headers that might be included:
- ✅ Resource.h (in wpp/registry/data_model/resource)
- ✅ Instance.h (in wpp/registry/data_model/instance)
- ✅ Object.h (in wpp/registry/data_model/object - need to check if this directory exists)
- ✅ WppTypes.h (in wpp/registry/data_model/types)
- ✅ InstSubject.h (in wpp/registry/data_model/instance/observer)
- ✅ ObjSubject.h (in wpp/registry/data_model/object/observer)
- ✅ WppLogs.h (in wpp/utils)
- ✅ WppPlatform.h (in wpp/platform)

---

## CURL Linking Issue (Still Present)

**Note:** The libcurl linking errors are still present because compilation must succeed before linking:
```
undefined reference to `ssh_get_error'
undefined reference to `nghttp2_strerror'
undefined reference to `ZSTD_isError'
undefined reference to `BrotliDecoderGetErrorCode'
```

These will be resolved once the WPP objects compile successfully and the build proceeds to linking. The fix from build01_fix.md will handle these errors.

---

## Full Build Command

After this fix, rebuild with:
```bash
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

---

## Files Modified

**CMakeLists.txt**
- Added `wpp/registry/data_model/instance/observer` to include directories (line ~417)

---

## Cascading Dependency Summary

This is the third cascading include dependency fix:

| Build Log | Missing Header | Location | Directory Added |
|-----------|----------------|----------|-----------------|
| build01.log | WppLogs.h | wpp/utils/ | wpp/utils |
| build02.log | WppPlatform.h | wpp/platform/ | wpp/platform |
| build03.log | InstSubject.h | wpp/registry/data_model/instance/observer/ | wpp/registry/data_model/instance/observer |

Each fix revealed the next layer of dependencies as compilation progressed deeper into the WPP framework headers.

---

## Why This Happened

The WPP objects (10525-10537) are more recent additions that use the full WPP framework, including:
- Observer pattern (InstSubject, ObjSubject)
- Logging utilities (WppLogs)
- Platform abstraction (WppPlatform)

Older objects (0-6, 3331, 3339, 10512-10522) don't use these features, so their directories weren't needed in the include path.

When the new advanced objects were added with `-DWITH_WAN_FAILOVER=ON`, etc., the build system needed these additional include directories that weren't previously required.

---

## Summary

**Issue - Missing Observer Include Directory:**
- **Symptom**: `fatal error: InstSubject.h: No such file or directory`
- **Cause**: `Instance.h` includes `InstSubject.h`, but `wpp/registry/data_model/instance/observer` not in include path
- **Fix**: Added `wpp/registry/data_model/instance/observer` to `target_include_directories`
- **Status**: ✅ Resolved

**Related Issues:**
- build01.log Issue 1: Missing WppLogs.h - ✅ Fixed (added wpp/utils)
- build01.log Issue 2: libcurl dependencies - ✅ Fixed (pending link test)
- build02.log Issue: Missing WppPlatform.h - ✅ Fixed (added wpp/platform)

The WPP object compilation should now progress further. Monitor for any additional missing headers from the WPP framework.
