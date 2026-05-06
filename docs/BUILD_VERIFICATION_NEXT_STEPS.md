# Build Verification - Next Steps

**Date**: 2026-04-30
**Status**: Ready to build and verify Week 1 work
**Context**: All code changes complete, linker fix applied, ready for compilation

---

## Current Status

### ✅ Completed Code Changes

All Week 1 foundation work has been implemented:

| Component | Status | Agent/Work |
|-----------|--------|------------|
| **P1-8** Ring-buffer logging | ✅ Complete | Agent ab29f31 |
| **P0-5** Burst reproducer | ✅ Complete | Agent a549593 |
| **P1-7** Location AC fix | ✅ Complete | Agent ada9d06 |
| **P0-5** Investigation | ✅ Complete | Agent aa8a7ba |
| **Build fix** Linker errors | ✅ Fixed | examples/CMakeLists.txt |
| **Graceful shutdown** | ✅ Complete | examples/main.cpp |

### ⏭️ Pending Verification

**NOT YET COMPILED** - All changes need to be built and tested.

---

## Build Environment Note

The current session is running in **Git Bash on Windows** which doesn't have the full Unix build toolchain (cmake, make, gcc) in the PATH.

### Two Options:

#### Option 1: WSL/Linux Environment (Recommended)
If you have WSL or a Linux environment with build tools:
```bash
# In WSL or Linux terminal
cd /mnt/d/Dev/Friendly-LwM2M-Client
./scripts/dev_setup_complete.sh
```

#### Option 2: Windows Native Build
If using Windows with CMake and MSVC/MinGW installed:
```cmd
# In CMD or PowerShell with CMake in PATH
cd D:\Dev\Friendly-LwM2M-Client\build
cmake .. -DCMAKE_BUILD_TYPE=Release -DDTLS_PROVIDER_MBEDTLS=ON -DWITH_WAN_FAILOVER=ON -DWITH_MULTIWAN_HEALTH=ON -DWITH_WIFI_CLIENT_MGMT=ON -DWITH_WIFI_CHANNEL_OPT=ON -DBUILD_TESTS=ON
cmake --build . --target friendly_example_client
```

---

## Step-by-Step Build Instructions

### Step 1: Clean and Reconfigure

```bash
cd build
rm -rf CMakeFiles CMakeCache.txt

# Reconfigure with all options
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  -DBUILD_TESTS=ON
```

**Expected output**:
```
-- Configuring done
-- Generating done
-- Build files have been written to: .../build
```

### Step 2: Build the Client

```bash
make friendly_example_client
# Or: cmake --build . --target friendly_example_client
```

**Expected output**:
```
[  8%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/platform/linux/log_ring.cpp.o
[ 37%] Linking CXX static library libfriendly_lwm2m.a
[ 37%] Built target friendly_lwm2m
...
[100%] Linking CXX executable examples/friendly_example_client
[100%] Built target friendly_example_client
```

**Critical Success Criteria**:
- ✅ NO linker errors for `ftl::get_log_ring()` and related functions
- ✅ NO undefined references to ring-buffer logging
- ✅ Build completes successfully
- ✅ Executable created: `examples/friendly_example_client`

### Step 3: Verify Binary Exists

```bash
ls -lh examples/friendly_example_client
file examples/friendly_example_client

# Check ring-buffer symbols are linked (not undefined)
nm examples/friendly_example_client | grep -i log_ring | head -5
```

**Expected**:
- Executable file ~2-5 MB
- Symbols show `T` (defined in text section), NOT `U` (undefined)

---

## Verification Tests

### Test 1: P1-8 Ring-Buffer Logging

**Purpose**: Verify SIGUSR1 snapshot generation works

```bash
# Start a Leshan server (if needed)
docker run -d --name leshan -p 8080:8080 -p 5683:5683/udp eclipse/leshan:latest

# Start the client
./examples/friendly_example_client -e TestClient -s coap://127.0.0.1:5683 &
CLIENT_PID=$!

# Let it initialize and register
sleep 10

# Trigger diagnostic snapshot
kill -SIGUSR1 $CLIENT_PID

# Verify snapshot created
cat /tmp/ftl-lwm2m-snapshot.json

# Check JSON is valid
cat /tmp/ftl-lwm2m-snapshot.json | jq .

# Graceful shutdown (test SIGINT handler)
kill -SIGINT $CLIENT_PID
wait $CLIENT_PID
```

**Expected Output**:

1. **Snapshot file exists**: `/tmp/ftl-lwm2m-snapshot.json`
2. **Valid JSON** with structure:
```json
{
  "timestamp_utc": "2026-04-30T...",
  "client_state": 5,
  "ring_buffer": {
    "total_entries": 1024,
    "entries_written": 47,
    "entries": [
      {
        "seq": 0,
        "t_mono_us": 1234567890,
        "state_from": 0,
        "state_to": 1,
        "trigger": 100,
        "error_code": 0,
        "transaction_count": 0,
        "pending_token_count": 0,
        "mid_alloc_count": 1,
        "nvm_inflight_writes": 0
      }
    ]
  },
  "introspection": {
    "transaction_count": 0,
    "pending_token_count": 0,
    "mid_alloc_count": 45,
    "observed_count": 0,
    "server_count": 1
  }
}
```

3. **Graceful shutdown** with output:
```
^C
[SIGNAL] Received SIGINT, shutting down gracefully...
[1777545435] [INFO] Shutting down...
[1777545435] [INFO] Stopping LwM2M client...
[1777545435] [INFO] Current client state: 5
[1777545435] [INFO] Sending DE-REGISTER request to server
[1777545435] [INFO] State transition: 5 → deregistering
[1777545435] [INFO] State transition: 5 → 0
[1777545435] [INFO] DE-REGISTER complete
[1777545435] [INFO] LwM2M client stopped
[1777545435] [INFO] Closing example...
[1777545435] [INFO] Destroying LwM2M client...
[1777545435] [INFO] LwM2M client destroyed
[1777545435] [INFO] Goodbye!
```

**Success Criteria**:
- ✅ Snapshot file created
- ✅ Valid JSON structure
- ✅ Ring buffer entries present
- ✅ Introspection counters populated
- ✅ SIGINT triggers graceful shutdown
- ✅ DE-REGISTER sent to server

---

### Test 2: P1-7 Location Access Control Fix

**Purpose**: Verify Location object reads return 2.05 (not 4.01)

```bash
# Ensure Leshan server is running
docker ps | grep leshan || docker run -d --name leshan -p 8080:8080 -p 5683:5683/udp eclipse/leshan:latest

# Start client
./examples/friendly_example_client -e TestClient -s coap://127.0.0.1:5683 --lifetime 60 &
CLIENT_PID=$!

# Wait for registration
sleep 15

# Test Location object read via Leshan REST API
curl -s http://localhost:8080/api/clients/TestClient/6/0 | jq .

# Or via CoAP directly
coap-client -m get coap://127.0.0.1:5683/rd/TestClient/6/0

# Clean up
kill -SIGINT $CLIENT_PID
docker stop leshan && docker rm leshan
```

**Expected**:
- ✅ HTTP 200 OK from Leshan API
- ✅ JSON response with Location data (latitude, longitude, etc.)
- ✅ CoAP response code: **2.05 Content** (not 4.01 Unauthorized)

**Before Fix** (what was wrong):
```
CoAP Response: 4.01 Unauthorized
Body: empty
```

**After Fix** (what should happen now):
```
CoAP Response: 2.05 Content
Body: TLV data with Location resources
```

**Root Cause Fixed**:
- `locationInit()` now creates instance-level AC: `Lwm2mAccessControl::create(*location, TEST_SERVER_SHORT_ID)`
- Server ID 123 now has permission to read `/6/0`

---

## Expected Build Issues (If Any)

### Issue 1: Linker Errors for Ring-Buffer (SHOULD BE FIXED)

**Symptom**:
```
undefined reference to `ftl::get_log_ring()'
undefined reference to `ftl::LogRing::log_transition(...)'
```

**Status**: ✅ **SHOULD BE FIXED** by `examples/CMakeLists.txt` change adding:
```cmake
target_link_libraries(friendly_example_client friendly_lwm2m)
```

**If still occurs**: The fix wasn't applied or CMake cache needs clearing.

### Issue 2: Warnings About Unused Parameters

**Not blocking** - Compiler warnings like:
```
warning: unused parameter 'priv_len' [-Wunused-parameter]
```

**Location**: `MbedTlsProvider.cpp`, `test_dtls_integration.cpp`

**Action**: Ignore for now - these are cosmetic and don't affect functionality.

### Issue 3: Submodule Issues

**Symptom**: Missing Wakaama or libcoap files

**Fix**:
```bash
git submodule update --init --recursive
```

---

## After Build Success - Next Steps

Once the build succeeds and tests pass:

### Immediate Next Steps (Week 2)

#### 1. Run P0-5 Burst Reproducer

**Purpose**: Reproduce the burst stall with instrumented client

```bash
cd tools/burst-load-reproducer

# Start Leshan + client with Docker Compose
docker-compose up -d

# Wait for services to start
sleep 15

# Run burst pattern (expect STALL on current unfixed code)
python3 burst_driver.py --endpoint TestClient --duration 36 --lt 60

# Expected exit code: 1 (stall detected)
echo "Exit code: $?"
```

#### 2. Capture Diagnostic Snapshot During Stall

```bash
# While reproducer is running and client is stalling
kill -SIGUSR1 $(pgrep friendly_example_client)

# Save snapshot for analysis
cat /tmp/ftl-lwm2m-snapshot.json | jq . > burst-stall-snapshot.json

# Review results
cat /tmp/burst-results.json | jq .
```

**What to look for**:
- `transaction_count` - should return to 0 between requests (if leaking, stays high)
- `pending_token_count` - should return to 0 (if accumulating, resource leak)
- `mid_alloc_count` - should increment normally (if stuck, MID exhaustion)
- `state` - check if stuck in particular state
- `ring_buffer.entries` - review latency patterns and state transitions

#### 3. Analyze Root Cause

Based on Agent aa8a7ba investigation, the issue is likely:

**Hypothesis A: ACL Cache Rebuild Storm**
- Check frequency of ACL cache rebuilds in logs
- If `_updateAcPolicies` flag stuck true, cache rebuilds on EVERY request
- Causes synchronous I/O blocking event loop

**Hypothesis B: Event Loop Blocking**
- Main loop not calling `lwm2m_step()` frequently enough
- UPDATE timers miss their deadlines
- Application-level issue, not Wakaama core

**Evidence from FT trace**:
- Latency creep on 4.01 responses (0.73-0.86s vs 0.18s baseline)
- Burst density is differentiator (17/36s stalls, 7/28s doesn't)
- Progressive work accumulation pattern

#### 4. Apply Surgical Fix

Based on empirical data, apply ONE of:

**Option A: Rate-Limit ACL Cache Rebuilds**
```cpp
// 2305-Wakaama/core/objects/access_control.c::prv_update_policy_cache()
static time_t last_update = 0;
time_t now = lwm2m_gettime();
if (now - last_update < MIN_CACHE_REBUILD_INTERVAL) {
    return true;  // Use stale cache
}
last_update = now;
// ... existing rebuild logic
```

**Option B: Fix Event Loop Blocking**
```cpp
// examples/main.cpp::socketPolling()
void socketPolling(Connection *connection) {
    while (!isDeviceShouldBeRebooted() && !g_shutdown_requested.load()) {
        connection->loop_with_timeout(100);  // 100ms max
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
```

**Option C: ACL Cache Thread Safety**
```cpp
// 2305-Wakaama/core/objects/access_control.c
static std::mutex _acPoliciesMutex;
// Protect all _acPolicies access with lock_guard
```

---

## Week 2-3: Transport Hardening (Can Start in Parallel)

After P0-5 investigation starts, can begin P2-9 and P2-10:

### P2-9: ICMP Port Unreachable Fast-Fail

**Implementation**: ~1.5-2.5 days

1. Create `src/platform/linux/socket_icmp.{h,cpp}`
2. Implement `enable_icmp_error_reporting()` and `drain_error_queue()`
3. Integrate with `src/transport/coap_transport.cpp`
4. Add `cancel_by_peer()` to transaction layer
5. Create tests

### P2-10: Retransmit Jitter

**Implementation**: ~1-1.5 days

1. Create `src/transport/coap_retransmit.{h,cpp}`
2. Create `src/platform/linux/random.{h,cpp}`
3. Implement `sample_initial_timeout()` with uniform [2.0, 3.0]s distribution
4. Hook into Wakaama transaction timer
5. Create distribution tests

---

## Documentation References

| Document | Purpose |
|----------|---------|
| `MASTER_IMPLEMENTATION_ROADMAP.md` | Complete project plan with all milestones |
| `BUILD_STATUS_AFTER_FIX.md` | Build instructions and expected output |
| `BUILD_FIX_P1-8_LINKER_ERRORS.md` | Analysis of linker error fix |
| `IMPLEMENTATION_STATUS_REPORT.md` | Comprehensive status of Week 1 work |
| `P2_COAP_TRANSPORT_REVIEW.md` | P2-9 and P2-10 implementation plans |
| `Issues/pcap-combo.md` | Original defect analysis (P0/P1 items) |
| `Issues/CLAUDE-coap-transport.md` | Transport hardening directive (P2 items) |

---

## Summary

**Current State**: All Week 1 code complete, linker fix applied, ready to build

**Next Action**: Build the client in appropriate environment (WSL/Linux or Windows with CMake)

**Success Criteria**:
1. ✅ Build succeeds with no linker errors
2. ✅ SIGUSR1 creates valid JSON snapshot
3. ✅ Location reads return 2.05 Content (not 4.01)
4. ✅ Graceful shutdown works with DE-REGISTER

**After Success**: Run P0-5 burst reproducer, capture snapshot, analyze, apply surgical fix

---

**Status**: Waiting for build to be executed in appropriate environment
**Confidence**: High - All fixes are surgical and well-tested patterns
**Risk**: Low - Changes are minimal and isolated

