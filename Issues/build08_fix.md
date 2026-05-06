# Build Error Fix: build08.log

## Issues Found

### Multiple Code Errors in WPP Advanced Objects (10525-10530)

**Error Categories:**
1. `'INST_T' has not been declared` - Undefined type used in method signatures
2. `'Lwm2mObjectMetaInfo' does not name a type` - Missing type definition
3. Methods marked 'override' but do not override - Interface mismatch
4. `'class wpp::WppRegistry' has no member named 'getObject'` - Wrong API usage
5. `no matching function for call to 'wpp::Instance::Instance(wpp::Object&, int&)'` - Wrong constructor signature
6. `'setExecute' was not declared in this scope` - Missing method
7. `'getMultiple' was not declared in this scope` - Missing method

**Affected Files:**
- wpp/registry/objects/w_10525_wan_failover_policy/ (WanFailoverPolicy.h/.cpp)
- wpp/registry/objects/w_10526_multiwan_health_check/ (MultiWanHealthCheck.h/.cpp)
- wpp/registry/objects/w_10527_wifi_client_management/ (WifiClientManagement.h/.cpp)
- wpp/registry/objects/w_10528_wifi_channel_optimization/ (WifiChannelOptimization.h/.cpp)
- wpp/registry/objects/w_10529_matter_bridge/ (MatterBridge.h/.cpp)
- wpp/registry/objects/w_10530_matter_device/ (MatterDevice.h/.cpp)

**Root Cause:**
The WPP advanced objects (10525-10537) were likely generated from a **template or code generator that contained errors**. The code does not match the actual WPP framework API.

---

## Error 1: INST_T Type Does Not Exist

**Problem:**
```cpp
// WRONG (in advanced objects):
static Instance* createInst(Object &obj, INST_T instId);
static Instance* createInst(Object &obj, INST_T instId, ItemOp *...);
// etc.
```

**Correct Pattern (from working objects):**
```cpp
// CORRECT (from ConnectivityMonitoring, Device, etc.):
ConnectivityMonitoring(lwm2m_context_t &context, const OBJ_LINK_T &id);
```

The type `INST_T` **does not exist** in WppTypes.h. The correct types are:
- `ID_T` - for resource IDs
- `OBJ_LINK_T` - for object/instance ID pairs
- Instance ID is part of `OBJ_LINK_T`

---

## Error 2: Lwm2mObjectMetaInfo Does Not Exist

**Problem:**
```cpp
// In *Info.h files:
extern const Lwm2mObjectMetaInfo WAN_FAILOVER_POLICY_INFO;
```

This type does not exist in the WPP framework. The correct type should be `ObjectInfo` from `ObjectInfo.h`.

---

## Error 3: Wrong Method Signatures

**Problem:**
```cpp
// WRONG:
bool validate(ID_T resId, const void* data, size_t len) override;
bool initResources(ItemOp* itemOp) override;
```

These methods are marked `override` but don't actually override anything in the base class `Instance`.

**Correct Pattern:**
Working objects (Device, ConnectivityMonitoring, etc.) don't override these methods.

---

## Error 4: Wrong WppRegistry API Usage

**Problem:**
```cpp
// WRONG:
Object &obj = ctx.registry().getObject(10525);
```

**Correct Pattern:**
```cpp
// CORRECT:
static Object & object(WppClient &ctx) {
    // Access object directly, don't use getObject()
    // This is typically defined in the WppRegistry
}
```

---

## Error 5: Wrong Instance Constructor Call

**Problem:**
```cpp
// WRONG:
Instance(obj, instId)  // Using Object& and int
```

**Correct Pattern:**
```cpp
// CORRECT:
Instance(context, id)  // Using lwm2m_context_t& and const OBJ_LINK_T&
```

---

## Error 6 & 7: Missing Methods

**Problem:**
```cpp
setExecute(...)    // Method doesn't exist
getMultiple(...)   // Method doesn't exist
```

These methods don't exist in the `Instance` base class or `ResourceContainer`.

---

## Comparison: Working vs Broken Objects

### Working Objects (0-6, 10512-10522) ✅

**Constructor:**
```cpp
ConnectivityMonitoring(lwm2m_context_t &context, const OBJ_LINK_T &id);
```

**No INST_T usage**
**No override methods**
**Correct API usage**

### Broken Objects (10525-10537) ❌

**Constructor attempts:**
```cpp
WanFailoverPolicy(..., INST_T instId)  // WRONG TYPE
Instance(obj, instId)                   // WRONG SIGNATURE
```

**Uses INST_T everywhere**
**Overrides non-existent methods**
**Wrong API calls**

---

## Recommended Fix Strategy

Given the extent of errors across 6+ object types with multiple files each, there are three approaches:

### Option 1: Disable Advanced Objects (RECOMMENDED for now)

Disable the problematic objects in CMake configuration:

```bash
cmake -DBUILD_ENV=DEV \
      -DBUILD_TESTS=ON \
      -DBUILD_EXAMPLES=ON \
      -DDTLS_PROVIDER_MBEDTLS=ON \
      -DWITH_DTLS=ON \
      -DWITH_DELTA_FOTA=ON \
      -DWITH_COMPRESSION=ON \
      -DWITH_MQTT=ON \
      -DWITH_OPENWRT_EXTENDED=ON \
      -DWITH_WAN_FAILOVER=OFF \           # Disable
      -DWITH_MULTIWAN_HEALTH=OFF \        # Disable
      -DWITH_WIFI_CLIENT_MGMT=OFF \       # Disable
      -DWITH_WIFI_CHANNEL_OPT=OFF \       # Disable
      -DWITH_MATTER_BRIDGE=OFF \          # Disable
      -DWITH_LAN_CONFIG=OFF \             # May also have issues
      -DWITH_ROUTING_TABLE=OFF \          # May also have issues
      -DWITH_VPN_CONFIG=OFF \             # May also have issues
      ..
```

**Pros:**
- Quickest path to a working build
- Can test the rest of the system
- Can fix objects incrementally

**Cons:**
- Advanced features unavailable

### Option 2: Fix Code Systematically

Manually fix each object to match the working object pattern:

1. Replace all `INST_T` with proper types
2. Fix `Lwm2mObjectMetaInfo` → `ObjectInfo`
3. Remove incorrect `override` methods
4. Fix constructor signatures
5. Fix WppRegistry API usage
6. Remove calls to non-existent methods

**Pros:**
- Objects will work correctly
- Permanent solution

**Cons:**
- Time-consuming (6 objects × 2 files × multiple errors each)
- Risk of introducing new errors

### Option 3: Regenerate Objects from Correct Template

If these objects were generated, fix the template and regenerate.

**Pros:**
- Fixes all objects at once
- Ensures consistency

**Cons:**
- Requires access to generator
- May lose manual customizations

---

## Immediate Fix: Disable Problematic Objects

To proceed with building and testing, disable the advanced objects:

```bash
cd build
cmake -DBUILD_ENV=DEV \
      -DBUILD_TESTS=ON \
      -DBUILD_EXAMPLES=ON \
      -DDTLS_PROVIDER_MBEDTLS=ON \
      -DDTLS_DEFAULT_PROVIDER=mbedtls \
      -DWITH_DTLS=ON \
      -DWITH_DELTA_FOTA=ON \
      -DWITH_BSDIFF=ON \
      -DWITH_VCDIFF=ON \
      -DWITH_COURGETTE=ON \
      -DWITH_COMPRESSION=ON \
      -DWITH_ROLLBACK=ON \
      -DWITH_MQTT=ON \
      -DWITH_OPENWRT_EXTENDED=ON \
      -DWITH_LUCI_MODULE=ON \
      -DWITH_HOTPLUG_HANDLERS=ON \
      -DWITH_WAN_FAILOVER=OFF \
      -DWITH_MULTIWAN_HEALTH=OFF \
      -DWITH_WIFI_CLIENT_MGMT=OFF \
      -DWITH_WIFI_CHANNEL_OPT=OFF \
      -DWITH_MATTER_BRIDGE=OFF \
      -DWITH_LAN_CONFIG=OFF \
      -DWITH_ROUTING_TABLE=OFF \
      -DWITH_VPN_CONFIG=OFF \
      ..
make -j$(nproc)
```

This will build:
- ✅ All mandatory objects (0, 1, 3)
- ✅ All optional OMA objects (2, 4, 5, 6)
- ✅ IPSO objects (3331, 3339)
- ✅ Platform objects (10512-10522) - These work correctly
- ❌ Advanced objects (10525-10537) - Disabled due to code errors

---

## Testing

### Verify Build Without Advanced Objects
```bash
cd build
cmake .. -DWITH_WAN_FAILOVER=OFF \
         -DWITH_MULTIWAN_HEALTH=OFF \
         -DWITH_WIFI_CLIENT_MGMT=OFF \
         -DWITH_WIFI_CHANNEL_OPT=OFF \
         -DWITH_MATTER_BRIDGE=OFF \
         -DWITH_LAN_CONFIG=OFF \
         -DWITH_ROUTING_TABLE=OFF \
         -DWITH_VPN_CONFIG=OFF
make -j$(nproc)
```

Expected: Clean build with all working objects.

---

## CURL Linking Issue (Still Present)

The libcurl linking errors are still present:
```
undefined reference to `ssh_get_error'
undefined reference to `nghttp2_strerror'
...
```

These will be resolved once compilation succeeds and the build01 CURL dependency fixes take effect.

---

## Summary

**Issue - Systematic Code Errors in Advanced Objects:**
- **Symptom**: Multiple compilation errors in objects 10525-10537
- **Cause**: Objects were generated with incorrect API usage, wrong types (INST_T), wrong method signatures
- **Root Cause**: Template or code generator had errors; objects don't match WPP framework API
- **Recommended Fix**: Disable advanced objects for now; fix them incrementally later
- **Status**: ⚠️ Workaround available (disable objects)

**Object Status:**
- ✅ Mandatory objects (0, 1, 3) - Working
- ✅ Optional OMA objects (2, 4, 5, 6) - Working
- ✅ IPSO objects (3331, 3339) - Working
- ✅ Platform objects (10512-10522) - Working
- ❌ Advanced objects (10525-10537) - Code errors (disable for now)

**Next Steps:**
1. Build with advanced objects disabled
2. Test core functionality
3. Fix advanced objects incrementally (or regenerate from corrected template)
4. Re-enable objects as they're fixed
