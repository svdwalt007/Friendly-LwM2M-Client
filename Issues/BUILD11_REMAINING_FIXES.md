# Build11 Remaining Fixes

## Summary
The following .cpp files have been partially fixed but need manual completion of the implementation pattern changes.

## Files Requiring Manual Fixes

### 1. WifiClientManagement.cpp
**Location:** `wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.cpp`

**Changes Needed:**

Line ~28-30: Replace
```cpp
Object& WifiClientManagement::object(WppClient& ctx) {
    return ctx.registry().getObject(WIFI_CLIENT_MGMT_OBJECT_ID);
}
```
With:
```cpp
Object& WifiClientManagement::object(WppClient& ctx) {
    return ctx.registry().wifiClientManagement();
}
```

Line ~32-34: Replace
```cpp
Instance* WifiClientManagement::createInst(WppClient& ctx, ID_T instId) {
    return object(ctx).createInstance(instId);
}
```
With:
```cpp
WifiClientManagement* WifiClientManagement::createInst(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().wifiClientManagement().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<WifiClientManagement*>(inst);
}
```

Line ~36-38: Replace
```cpp
Instance* WifiClientManagement::instance(WppClient& ctx, ID_T instId) {
    return object(ctx).instance(instId);
}
```
With:
```cpp
WifiClientManagement* WifiClientManagement::instance(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().wifiClientManagement().instance(instId);
    if (!inst) return NULL;
    return static_cast<WifiClientManagement*>(inst);
}
```

Line ~40-42: Replace
```cpp
bool WifiClientManagement::removeInst(WppClient& ctx, ID_T instId) {
    return object(ctx).remove(instId);
}
```
With:
```cpp
bool WifiClientManagement::removeInst(WppClient& ctx, ID_T instId) {
    return ctx.registry().wifiClientManagement().remove(instId);
}
```

Line ~45-47: Replace constructor
```cpp
WifiClientManagement::WifiClientManagement(Object& object, ID_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "WifiClientManagement instance %d created", instId);
}
```
With:
```cpp
WifiClientManagement::WifiClientManagement(lwm2m_context_t& context, const OBJ_LINK_T& id)
    : Instance(context, id) {

    resourcesCreate();
    resourcesInit();

    WPP_LOGD(TAG, "WifiClientManagement instance created");
}
```

Add after destructor:
```cpp
void WifiClientManagement::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
    operationNotify(*this, resLink, type);
}

void WifiClientManagement::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
    if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);
}
```

Line ~56: Rename method
```cpp
bool WifiClientManagement::initResources(ItemOp *itemOp) {
```
To:
```cpp
void WifiClientManagement::resourcesCreate() {
    std::vector<Resource> resources = {
        // ... define all resources with metadata
    };
    setupResources(std::move(resources));
}

void WifiClientManagement::resourcesInit() {
```

Remove `bool validate()` method entirely (convert to setDataVerifier later)

Update all `set<TYPE>(RES, val)` to `resource(RES)->set<TYPE>(val)`
Update all `get<TYPE>(RES)` to `resource(RES)->get<TYPE>()`

---

### 2. WifiChannelOptimization.cpp
**Location:** `wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.cpp`

Apply same pattern as WifiClientManagement, but use:
- Registry method: `wifiChannelOptimization()`
- Class name: `WifiChannelOptimization`

---

### 3. MatterBridge.cpp
**Location:** `wpp/registry/objects/w_10529_matter_bridge/MatterBridge.cpp`

Apply same pattern, but use:
- Registry method: `matterBridge()`
- Class name: `MatterBridge`

---

### 4. MatterDevice.cpp
**Location:** `wpp/registry/objects/w_10530_matter_device/MatterDevice.cpp`

Apply same pattern, but use:
- Registry method: `matterDevice()`
- Class name: `MatterDevice`

---

### 5. ThreadNetwork.cpp
**Location:** `wpp/registry/objects/w_10531_thread_network/ThreadNetwork.cpp`

Apply same pattern, but use:
- Registry method: `threadNetwork()`
- Class name: `ThreadNetwork`

---

### 6. LanConfiguration.cpp
**Location:** `wpp/registry/objects/w_10535_lan_configuration/LanConfiguration.cpp`

Apply same pattern, but use:
- Registry method: `lanConfiguration()`
- Class name: `LanConfiguration`

---

### 7. VpnConfiguration.cpp
**Location:** `wpp/registry/objects/w_10537_vpn_configuration/VpnConfiguration.cpp`

Apply same pattern, but use:
- Registry method: `vpnConfiguration()`
- Class name: `VpnConfiguration`

---

## Automated Fixes Already Applied

✅ INST_T → ID_T (all occurrences)
✅ WppClient& client → WppClient& ctx
✅ ::remove( → ::removeInst(

## Fixes Still Needed (Manual)

For each of the 7 files above:

1. ❌ Fix object() method - use registry accessor
2. ❌ Fix createInst() - change return type, use registry accessor, add NULL check
3. ❌ Fix instance() - change return type, use registry accessor, add NULL check
4. ❌ Fix removeInst() - use registry accessor directly
5. ❌ Fix constructor - change signature, add resourcesCreate/Init calls
6. ❌ Add serverOperationNotifier() and userOperationNotifier() methods
7. ❌ Split initResources into resourcesCreate() and resourcesInit()
8. ❌ Remove validate() method
9. ❌ Update resource access from set<T>(ID, val) to resource(ID)->set<T>(val)
10. ❌ Update resource access from get<T>(ID) to resource(ID)->get<T>()

## Estimation

- Time per file: ~15-20 minutes (careful manual editing)
- Total time: ~2-2.5 hours for all 7 files
- Complexity: Medium (repetitive but needs attention to detail)

## Recommendation

Given the repetitive nature and error-prone sed scripts, I recommend:

1. **Option A:** Fix one file completely as template, then copy pattern to others
   - Start with simplest: VpnConfiguration or LanConfiguration
   - Use as reference for the other 6

2. **Option B:** Use WanFailoverPolicy.cpp as complete reference
   - It's already fully fixed and working
   - Copy implementation patterns from there

3. **Option C:** Create a Python script to do proper AST-based transformation
   - More reliable than sed
   - Can handle complex replacements correctly
   - Takes longer to write but safer to run

## Current Status

**Compilation will fail** until these 7 files are properly fixed.

The header files (.h) have been fixed, but the implementations (.cpp) need manual completion.

## Priority Order

Fix in this order (simplest to most complex):

1. VpnConfiguration.cpp (likely simplest)
2. LanConfiguration.cpp
3. WifiChannelOptimization.cpp
4. WifiClientManagement.cpp
5. ThreadNetwork.cpp
6. MatterDevice.cpp
7. MatterBridge.cpp (likely most complex - Matter protocol integration)

---

**Next Action:** Pick Option A and fix VpnConfiguration.cpp as template, then apply to others.
