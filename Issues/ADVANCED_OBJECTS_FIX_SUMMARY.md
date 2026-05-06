# Advanced Objects Code Fix Summary

## Overview

This document summarizes the systematic fixes applied to resolve compilation errors in advanced WPP objects (10525-10537).

**Status:** ✅ Code fixes applied, ready for testing
**Date:** 2024-04-28
**Approach:** Fixed code errors instead of disabling objects

---

## Problem Description

Advanced WPP objects (10525-10537) had systematic code errors due to being generated from a faulty template:

1. Using non-existent type `INST_T`
2. Wrong `Lwm2mObjectMetaInfo` type
3. Incorrect method overrides (`validate`, `initResources`)
4. Wrong constructor signatures
5. Wrong WppRegistry API usage
6. Missing registry accessor methods
7. Non-existent method calls (`setExecute`, `getMultiple`)

---

## Objects Fixed

### Fixed Objects (13 total):
- ✅ 10525: WAN Failover Policy
- ✅ 10526: Multi-WAN Health Check
- ✅ 10527: WiFi Client Management
- ✅ 10528: WiFi Channel Optimization
- ✅ 10529: Matter Bridge
- ✅ 10530: Matter Device
- ✅ 10531: Thread Network
- ✅ 10532: Zigbee Coordinator
- ✅ 10533: Zigbee Device
- ✅ 10534: Zigbee Group
- ✅ 10535: LAN Configuration
- ✅ 10536: Routing Table
- ✅ 10537: VPN Configuration

---

## Systematic Fixes Applied

### Fix 1: Corrected Static Method Signatures (.h files)

**Before (WRONG):**
```cpp
static Object& object(WppClient& client);
static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
static Instance* instance(WppClient& client, INST_T instId);
static bool remove(WppClient& client, INST_T instId);
```

**After (CORRECT):**
```cpp
static Object& object(WppClient& ctx);
static ClassName* createInst(WppClient& ctx, ID_T instId = ID_T_MAX_VAL);
static ClassName* instance(WppClient& ctx, ID_T instId = ID_T_MAX_VAL);
static bool removeInst(WppClient& ctx, ID_T instId);
```

**Changes:**
- Replaced `INST_T` with `ID_T` (correct type from WppTypes.h)
- Changed return type from `Instance*` to `ClassName*` for type safety
- Renamed `remove()` to `removeInst()` for consistency
- Changed parameter name from `client` to `ctx`

---

### Fix 2: Corrected Constructor Signature (.h files)

**Before (WRONG):**
```cpp
ClassName(Object& object, INST_T instId);
~ClassName() override;  // override keyword not needed
```

**After (CORRECT):**
```cpp
ClassName(lwm2m_context_t& context, const OBJ_LINK_T& id);
~ClassName();
```

**Changes:**
- Changed from `(Object&, INST_T)` to `(lwm2m_context_t&, const OBJ_LINK_T&)`
- Matches base class `Instance` constructor signature
- Removed unnecessary `override` from destructor

---

### Fix 3: Replaced Invalid Override Methods (.h files)

**Before (WRONG):**
```cpp
protected:
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    bool initResources(ItemOp *) override;
```

**After (CORRECT):**
```cpp
protected:
    void serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) override;
    void userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) override;

private:
    void resourcesCreate();
    void resourcesInit();
```

**Changes:**
- Removed `validate()` - doesn't exist in base class
- Removed `initResources()` - doesn't exist in base class
- Added correct `serverOperationNotifier()` and `userOperationNotifier()` overrides
- Replaced `initResources()` with `resourcesCreate()` and `resourcesInit()`

---

### Fix 4: Fixed Static Method Implementations (.cpp files)

**Before (WRONG):**
```cpp
Object& ClassName::object(WppClient& client) {
    return client.registry().getObject(OBJECT_ID);  // getObject doesn't exist
}

Instance* ClassName::createInst(WppClient& client, INST_T instId) {
    return object(client).createInstance(instId);
}
```

**After (CORRECT):**
```cpp
Object& ClassName::object(WppClient& ctx) {
    return ctx.registry().className();  // Use specific registry method
}

ClassName* ClassName::createInst(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().className().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<ClassName*>(inst);
}
```

**Changes:**
- Used specific registry accessor methods (e.g., `wanFailoverPolicy()`)
- Removed call to non-existent `getObject()` method
- Added NULL check and static_cast for type safety

---

### Fix 5: Fixed Constructor Implementation (.cpp files)

**Before (WRONG):**
```cpp
ClassName::ClassName(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "Instance %d created", instId);
}
```

**After (CORRECT):**
```cpp
ClassName::ClassName(lwm2m_context_t& context, const OBJ_LINK_T& id)
    : Instance(context, id) {

    resourcesCreate();
    resourcesInit();

    WPP_LOGD(TAG, "Instance created");
}
```

**Changes:**
- Updated signature to match Instance base class
- Added `resourcesCreate()` and `resourcesInit()` calls
- Removed `instId` access (use id from base class if needed)

---

### Fix 6: Added Notification Methods (.cpp files)

**Before:** No notification methods

**After (ADDED):**
```cpp
void ClassName::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
    operationNotify(*this, resLink, type);
}

void ClassName::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
    if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);
}
```

---

### Fix 7: Created Resource Setup Methods (.cpp files)

**Before (WRONG):**
```cpp
bool ClassName::initResources(ItemOp *itemOp) {
    set<STRING_T>(RESOURCE_0, "value");
    set<INT_T>(RESOURCE_1, 42);
    setExecute(EXECUTE_RES, handler);
    return true;
}
```

**After (CORRECT):**
```cpp
void ClassName::resourcesCreate() {
    std::vector<Resource> resources = {
        {RESOURCE_0, ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {RESOURCE_1, ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {EXECUTE_RES, ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL, TYPE_ID::NONE},
    };
    setupResources(std::move(resources));
}

void ClassName::resourcesInit() {
    resource(RESOURCE_0)->set<STRING_T>("value");
    resource(RESOURCE_1)->set<INT_T>(42);
    resource(EXECUTE_RES)->setExecuteHandler(handler);
}
```

**Changes:**
- Split into two methods: `resourcesCreate()` and `resourcesInit()`
- `resourcesCreate()` defines resource metadata
- `resourcesInit()` sets initial values
- Changed `set<TYPE>(ID, value)` to `resource(ID)->set<TYPE>(value)`
- Changed `setExecute(ID, handler)` to `resource(ID)->setExecuteHandler(handler)`

---

### Fix 8: Fixed Resource Access Patterns (.cpp files)

**Before (WRONG):**
```cpp
// Direct access (doesn't exist)
set<STRING_T>(PRIMARY_WAN_1, "wan");
std::string wan = get<STRING_T>(PRIMARY_WAN_1);
setExecute(APPLY_POLICY_17, applyPolicy);
```

**After (CORRECT):**
```cpp
// Access through resource() method
resource(PRIMARY_WAN_1)->set<STRING_T>("wan");
std::string wan = resource(PRIMARY_WAN_1)->get<STRING_T>();
resource(APPLY_POLICY_17)->setExecuteHandler(applyPolicy);
```

**Pattern:**
- **set:** `resource(ID)->set<TYPE>(value)`
- **get:** `resource(ID)->get<TYPE>()`
- **setExecuteHandler:** `resource(ID)->setExecuteHandler(handler)`

---

### Fix 9: Added Registry Accessor Methods

Added to `WppRegistry.h`:
```cpp
#ifdef OBJ_W_10525_WAN_FAILOVER_POLICY
Object & wanFailoverPolicy();
#endif
#ifdef OBJ_W_10526_MULTIWAN_HEALTH_CHECK
Object & multiWanHealthCheck();
#endif
// ... (and 11 more)
```

Added to `WppRegistry.cpp`:
```cpp
#ifdef OBJ_W_10525_WAN_FAILOVER_POLICY
Object & WppRegistry::wanFailoverPolicy() {
	return *object(WAN_FAILOVER_POLICY_OBJECT_ID);
}
#endif
// ... (and 11 more)
```

---

## Files Modified

### Header Files Modified (13 files):
- `wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.h`
- `wpp/registry/objects/w_10526_multiwan_health_check/MultiWanHealthCheck.h`
- `wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.h`
- `wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.h`
- `wpp/registry/objects/w_10529_matter_bridge/MatterBridge.h`
- `wpp/registry/objects/w_10530_matter_device/MatterDevice.h`
- `wpp/registry/objects/w_10531_thread_network/ThreadNetwork.h`
- `wpp/registry/objects/w_10532_zigbee_coordinator/ZigbeeCoordinator.h`
- `wpp/registry/objects/w_10533_zigbee_device/ZigbeeDevice.h`
- `wpp/registry/objects/w_10534_zigbee_group/ZigbeeGroup.h`
- `wpp/registry/objects/w_10535_lan_configuration/LanConfiguration.h`
- `wpp/registry/objects/w_10536_routing_table/RoutingTable.h`
- `wpp/registry/objects/w_10537_vpn_configuration/VpnConfiguration.h`

### Implementation Files Modified (13 files):
- `wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.cpp`
- (... same pattern for 10526-10537)

### Registry Files Modified (2 files):
- `wpp/registry/WppRegistry.h` - Added includes and method prototypes
- `wpp/registry/WppRegistry.cpp` - Added method implementations

---

## Automation Scripts Created

### `scripts/build/fix_advanced_objects.sh`
Automated fix script that applies all systematic fixes to the 13 advanced objects.

**Usage:**
```bash
./scripts/build/fix_advanced_objects.sh
```

**Features:**
- Fixes header files (.h)
- Fixes implementation files (.cpp)
- Updates resource access patterns
- Adds notification methods

---

## Testing Plan

### 1. Build Configuration
Enable all advanced objects in CMake:
```bash
cd build
rm -f CMakeCache.txt
cmake -DBUILD_ENV=DEV \
      -DBUILD_TESTS=ON \
      -DBUILD_EXAMPLES=ON \
      -DDTLS_PROVIDER_MBEDTLS=ON \
      -DWITH_DTLS=ON \
      -DWITH_WAN_FAILOVER=ON \
      -DWITH_MULTIWAN_HEALTH=ON \
      -DWITH_WIFI_CLIENT_MGMT=ON \
      -DWITH_WIFI_CHANNEL_OPT=ON \
      -DWITH_MATTER_BRIDGE=ON \
      -DWITH_MATTER_DEVICE=ON \
      -DWITH_THREAD_NETWORK=ON \
      -DWITH_ZIGBEE_COORDINATOR=ON \
      -DWITH_ZIGBEE_DEVICE=ON \
      -DWITH_ZIGBEE_GROUP=ON \
      -DWITH_LAN_CONFIG=ON \
      -DWITH_ROUTING_TABLE=ON \
      -DWITH_VPN_CONFIG=ON \
      ..
make -j$(nproc)
```

### 2. Expected Result
- ✅ All 13 advanced objects compile without errors
- ✅ All existing objects (0-6, 10512-10522) still compile
- ✅ Total of 36 objects compiled successfully

### 3. Known Remaining Issues
- TODO: Multiple resource instance iteration (getMultiple alternative)
- Validation logic removed (was in validate method, needs reimplementation if required)

---

## Comparison: Before vs After

### Before (Broken):
```cpp
class WanFailoverPolicy : public Instance {
    static Instance* createInst(Object &obj, INST_T instId);  // ❌ INST_T doesn't exist
    WanFailoverPolicy(Object& obj, INST_T instId);            // ❌ Wrong signature
    bool validate(...) override;                               // ❌ Doesn't override anything
    bool initResources(ItemOp*) override;                      // ❌ Doesn't override anything
};
```

### After (Fixed):
```cpp
class WanFailoverPolicy : public Instance {
    static WanFailoverPolicy* createInst(WppClient& ctx, ID_T instId = ID_T_MAX_VAL);  // ✅ Correct types
    WanFailoverPolicy(lwm2m_context_t& context, const OBJ_LINK_T& id);                  // ✅ Matches base class
    void serverOperationNotifier(...) override;                                          // ✅ Real override
    void userOperationNotifier(...) override;                                            // ✅ Real override
    void resourcesCreate();                                                              // ✅ Proper method
    void resourcesInit();                                                                // ✅ Proper method
};
```

---

## Summary

**Total Objects Fixed:** 13
**Total Files Modified:** 28 (13 .h + 13 .cpp + 2 registry files)
**Fix Categories:** 9
**Lines of Code Changed:** ~2000+

**Status:** ✅ All code fixes applied
**Next Step:** Build and test with all objects enabled

---

## References

- Working object reference: `wpp/registry/objects/o_4_connectivity_monitoring/`
- Base class: `wpp/registry/data_model/instance/Instance.h`
- Type definitions: `wpp/registry/data_model/types/WppTypes.h`
- Registry: `wpp/registry/WppRegistry.h`

---

## Commands for User

### Build with all objects:
```bash
cd build
rm CMakeCache.txt
cmake .. -DWITH_WAN_FAILOVER=ON -DWITH_MULTIWAN_HEALTH=ON -DWITH_WIFI_CLIENT_MGMT=ON -DWITH_WIFI_CHANNEL_OPT=ON -DWITH_MATTER_BRIDGE=ON -DWITH_MATTER_DEVICE=ON -DWITH_THREAD_NETWORK=ON -DWITH_ZIGBEE_COORDINATOR=ON -DWITH_ZIGBEE_DEVICE=ON -DWITH_ZIGBEE_GROUP=ON -DWITH_LAN_CONFIG=ON -DWITH_ROUTING_TABLE=ON -DWITH_VPN_CONFIG=ON
make -j$(nproc) 2>&1 | tee ../Issues/build_with_advanced_objects.log
```

### If errors occur:
```bash
# Check the build log
less Issues/build_with_advanced_objects.log

# Search for specific errors
grep "error:" Issues/build_with_advanced_objects.log
```

---

**End of Summary**
