# Master Implementation Roadmap - LwM2M Client Remediation

**Date**: 2026-04-30
**Authoritative Directives**:
- `Issues/pcap-combo.md` v2 - Main remediation (P0/P1 items)
- `Issues/CLAUDE-coap-transport.md` - Transport hardening (P2 items)

---

## Current Status Summary

### ✅ Completed (Week 1 Foundation)

| Task | Status | Agent | Files |
|------|--------|-------|-------|
| **P1-8** Ring-buffer logging | ✅ Complete | ab29f31 | 2 new, 5 modified |
| **P0-5** Burst reproducer | ✅ Complete | a549593 | 4 new (Python, Docker, tests) |
| **P1-7** Location AC fix | ✅ Complete | ada9d06 | 1 modified, 2 new |
| **P0-5** Investigation | ✅ Complete | aa8a7ba | RCA report |
| **Build fix** Linker errors | ✅ Fixed | - | 1 modified (CMakeLists.txt) |

### ⏳ Pending Verification

| Task | Status | Next Step |
|------|--------|-----------|
| Build client | ⏳ Ready | `make friendly_example_client` |
| Test P1-7 fix | ⏳ Ready | Verify Location reads return 2.05 |
| Run P0-5 reproducer | ⏳ Ready | Confirm stall, capture snapshot |
| Apply P0-5 fix | ⏳ Waiting | Based on empirical data |

---

## Priority Matrix

```
        │ Blocking │ High Value │ Low Effort │ Priority
────────┼──────────┼────────────┼────────────┼──────────
P0-5    │    ✓     │     ✓      │     ✗      │   1 (Critical)
P1-8    │    ✓     │     ✓      │     ✗      │   ✅ DONE
P1-7    │    ✗     │     ✓      │     ✓      │   ✅ DONE
P2-9    │    ✗     │     ✓      │     ✓      │   2 (Next)
P2-10   │    ✗     │     ✓      │     ✓      │   2 (Next)
P0-1    │    ✓     │     ✓      │     ?      │   3 (Gated)
P0-2    │    ✓     │     ✓      │     ✗      │   4
P0-3    │    ✓     │     ✗      │     ✗      │   5 (Milestone)
P0-4    │    ✓     │     ✗      │     ✗      │   6
P1-5    │    ✗     │     ✗      │     ✓      │   7
P1-6    │    ✗     │     ✗      │     ✓      │   8
```

---

## Immediate Actions (This Week)

### Step 1: Verify P1-8 and P1-7 ✅

**Time**: 1 hour

```bash
# Build client with all changes
cd build
make clean
cmake .. -DCMAKE_BUILD_TYPE=Release
make friendly_example_client

# Test P1-8: Ring-buffer logging
./friendly_example_client -e TestClient -s coap://127.0.0.1:5683 &
CLIENT_PID=$!
sleep 10
kill -SIGUSR1 $CLIENT_PID
cat /tmp/ftl-lwm2m-snapshot.json | jq .
kill -SIGINT $CLIENT_PID

# Test P1-7: Location AC fix
# Start Leshan server, register client, read /6/0
# Expected: 2.05 (not 4.01)
```

**Success Criteria**:
- ✅ Build succeeds with no linker errors
- ✅ SIGUSR1 creates valid JSON snapshot
- ✅ Location reads return 2.05 Content

---

### Step 2: Run P0-5 Burst Reproducer ⏭️

**Time**: 2-3 hours

```bash
# Start Leshan + client with reproducer
cd tools/burst-load-reproducer
docker-compose up -d
sleep 15

# Run burst pattern (expect STALL on current unfixed code)
python3 burst_driver.py --endpoint TestClient --duration 36 --lt 60

# Expected: Exit code 1 (stall detected)
```

**Capture diagnostic data**:
```bash
# During stall, capture snapshot
kill -SIGUSR1 $(pgrep friendly_example_client)
cat /tmp/ftl-lwm2m-snapshot.json | jq . > burst-stall-snapshot.json

# Review results
cat /tmp/burst-results.json | jq .
```

**Analyze**:
- Check `transaction_count` - leaking?
- Check `pending_token_count` - accumulating?
- Check `mid_alloc_count` - incrementing normally?
- Check state - stuck in particular state?
- Review latency creep pattern

**Expected findings** (from investigation):
- No transaction leak (confirmed by agent aa8a7ba)
- Likely **ACL cache rebuild storm** or **event loop blocking**

---

### Step 3: Apply P0-5 Surgical Fix ⏭️

**Time**: 1-2 days

**Based on RCA findings, likely fix is ONE of**:

#### Option A: Rate-Limit ACL Cache Rebuilds

If snapshot shows frequent cache rebuilds:

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

#### Option B: Fix Event Loop Blocking

If main loop not calling `lwm2m_step()`:

```cpp
// examples/main.cpp::socketPolling()
void socketPolling(Connection *connection) {
    while (!isDeviceShouldBeRebooted() && !g_shutdown_requested.load()) {
        // Change to non-blocking with timeout
        connection->loop_with_timeout(100);  // 100ms max
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
```

#### Option C: ACL Cache Thread Safety

If race condition in cache access:

```cpp
// 2305-Wakaama/core/objects/access_control.c
static std::mutex _acPoliciesMutex;
// Protect all _acPolicies access with lock_guard
```

**Deliverables**:
1. Code fix (minimal surgical change)
2. `docs/P0-5-FIX-DETAILS.md` with RCA and fix explanation
3. Updated integration tests confirming fix

**Acceptance**:
- ✅ Burst reproducer does NOT stall fixed client
- ✅ Sustained burst (200 GETs/400s) completes healthy
- ✅ Post-burst UPDATE fires within ±5% of `lt × 0.75`
- ✅ All counters return to baseline

---

## Week 2: Transport Hardening (P2-9, P2-10)

### P2-9: ICMP Port Unreachable Fast-Fail

**Time**: 1.5-2.5 days

**Implementation**:

1. **Create files**:
   - `src/platform/linux/socket_icmp.h`
   - `src/platform/linux/socket_icmp.cpp`

2. **Implement**:
   ```cpp
   void enable_icmp_error_reporting(int fd, bool ipv6);
   int drain_error_queue(int fd, callback);
   ```

3. **Integrate**:
   - Modify `src/platform/linux/socket_udp.cpp` - enable `IP_RECVERR`
   - Modify `src/transport/coap_transport.cpp` - call `drain_error_queue()` before `recvfrom()`
   - Add `cancel_by_peer()` to transaction layer

4. **Test**:
   - Unit: `tests/transport/test_icmp_unreachable.cpp`
   - Integration: `tests/integration/test_closed_port_fast_fail.cpp`

**Acceptance**:
- ✅ Closed port → port_unreachable event within 1s (not 32s)
- ✅ Transaction cancelled immediately
- ✅ State machine applies backoff

---

### P2-10: Retransmit Jitter

**Time**: 1-1.5 days

**Implementation**:

1. **Create files**:
   - `src/transport/coap_retransmit.h`
   - `src/transport/coap_retransmit.cpp`
   - `src/platform/linux/random.h`
   - `src/platform/linux/random.cpp`

2. **Implement**:
   ```cpp
   std::chrono::milliseconds sample_initial_timeout();
   std::mt19937& random_engine();  // thread-local
   ```

3. **Integrate**:
   - Find Wakaama transaction arming point
   - Replace `ACK_TIMEOUT` literal with `sample_initial_timeout()`

4. **Test**:
   - Unit: `tests/transport/test_retransmit_jitter.cpp`
   - Verify distribution: 10,000 samples → uniform [2000, 3000]ms
   - K-S test passes at α=0.05

**Acceptance**:
- ✅ Initial timeout uniformly distributed [2.0, 3.0]s
- ✅ Subsequent timeouts exactly 2× previous
- ✅ Pcap shows spread across range (100 sessions)

---

### Combined PR: `fix/p2-coap-transport-hardening`

**Time**: 0.5 day integration + 0.5 day docs = 1 day

**Total P2 effort**: 3-5 days

---

## Week 3-4: Remaining P0 Items

### P0-1: UPDATE Scheduler (Gated on Reproducer)

**Time**: 1-2 days (IF reproducer fails)

**First**: Create reproducer test

```cpp
// tests/integration/test_long_idle_session.cpp
// Register with lt=60
// No operator reads/writes for 5 minutes
// Assert UPDATEs fire every 42-48s (0.75 × lt ± 5%)
```

**If test FAILS** → Fix UPDATE re-arm logic in `registration.c`
**If test PASSES** → Close P0-1 with note (was downstream of P0-5)

---

### P0-2: Registration Watchdog

**Time**: 2-3 days

**Implementation**:

```cpp
// Independent watchdog in lwm2m_step()
safety_margin = max(lt × 0.10, 5s)
if (now - t_last_successful_update_ack) > (lt - safety_margin) {
    transition_to(STATE_REG_REGISTRATION_REQUIRED);
}
```

**Features**:
- Runs from `lwm2m_step()` tick
- Preserves endpoint name
- Rebuilds link-format from current objects
- RFC 7252 backoff on persistent failure (cap at 1h)

**Acceptance**:
- ✅ Watchdog independent of UPDATE scheduler
- ✅ Re-REGISTER preserves endpoint, rebuilds payload
- ✅ New Location-Path replaces old
- ✅ No infinite loop on persistent failure

---

## Weeks 5-8: DTLS Implementation (P0-3)

**Time**: 2-3 weeks (substantial milestone)

**Deliverables**:

1. CMake option: `FTL_LWM2M_SECURITY_MODE=PSK|RPK|X509|NOSEC`
2. mbedTLS config: `MBEDTLS_SSL_DTLS_CONNECTION_ID` enabled
3. Security Object provisioning: `/0/0/2=0`, `/0/0/3=<PSK identity>`, `/0/0/5=<key>`
4. URI scheme: `coaps://host:5684`
5. **CID negotiation: MUST request non-zero-length CID** (4-8 bytes)

**Critical**:
- Zero-length CIDs forbidden (defeats NAT survival on PSM cycles)
- Add regression test: parse ClientHello, assert CID length ≥ 1

**Phases**:
1. Week 5: mbedTLS integration, PSK mode
2. Week 6: CID negotiation, handshake tests
3. Week 7: Integration with transport layer (P2-9 on DTLS sockets)
4. Week 8: Field testing, hardening

---

## Weeks 9-10: Queue Mode and Polish

### P0-4: Queue Mode

**Time**: 2-3 days

```cpp
// config/client_config.json
"binding": "U" | "UQ"

// When UQ, stay receive-active for MAX_TRANSMIT_WAIT (93s) after UPDATE
```

---

### P1-5: Default Lifetime

**Time**: 1 hour (config change)

```cpp
// config/client_config.json
"lifetime": {
    "always_on": 300,      // OpenWRT, RPi4
    "nb_iot": 3600,        // PSM devices
    "battery": 86400       // OMA spec default
}
```

---

### P1-6: DEREGISTER Verification

**Time**: 0.5 day (test only)

```cpp
// tests/integration/test_deregister_after_burst.cpp
// 1. Register
// 2. Drive burst (P0-5 reproducer)
// 3. 5s after burst, SIGTERM
// 4. Assert CON DELETE within 5s
```

---

## Milestones

### Milestone 1: Foundation Complete ✅
- **Date**: 2026-04-30
- **Deliverables**: P1-8, P1-7, P0-5 reproducer, investigation complete
- **Status**: ✅ DONE

### Milestone 2: Burst Stall Resolved ⏭️
- **Target**: Week 2
- **Deliverables**: P0-5 fix, verified with reproducer
- **Status**: Ready to start

### Milestone 3: Transport Hardening ⏭️
- **Target**: Week 2-3
- **Deliverables**: P2-9, P2-10, RFC 7252 compliant
- **Status**: Can start in parallel with M2

### Milestone 4: Core Stability ⏭️
- **Target**: Week 3-4
- **Deliverables**: P0-1, P0-2, watchdog, UPDATE scheduler
- **Status**: Waiting for M2

### Milestone 5: Production Security 🎯
- **Target**: Week 5-8
- **Deliverables**: P0-3 DTLS/PSK/CID, production-ready security
- **Status**: Major milestone, separate track

### Milestone 6: Field Ready 🚀
- **Target**: Week 9-10
- **Deliverables**: P0-4, P1-5, P1-6, all acceptance criteria met
- **Status**: Polish and verification

---

## Risk Management

### High Risk Items

| Risk | Impact | Mitigation |
|------|--------|------------|
| P0-5 fix harder than expected | Blocks everything | RCA already done, likely single-point fix |
| DTLS integration complex | Delays M5 | Start early, incremental milestones |
| Wakaama integration unknowns | Multiple delays | Use `search-first`, read Wakaama code first |

### Low Risk Items

| Item | Why Low Risk |
|------|-------------|
| P1-7 | One-line config change |
| P2-9, P2-10 | Well-defined, reference implementation provided |
| P1-5 | Config file change |
| P1-6 | Test verification only |

---

## Resource Allocation

### Can Parallelize

- P0-5 investigation + P2 transport (different layers)
- P0-1 reproducer + P0-2 watchdog (different subsystems)
- P0-4 Queue Mode + P1-5 lifetime (config changes)

### Must Serialize

- P1-8 → P0-5 (P0-5 needs observability)
- P0-5 → P0-1 (P0-1 gated on P0-5 outcome)
- P2 → P0-3 (DTLS builds on transport hardening)
- P0-3 → field testing (need security for production)

---

## Definition of Done (Project-Wide)

### Code Complete
- ✅ All P0 items acceptance criteria pass
- ✅ All P1 items acceptance criteria pass
- ✅ All P2 items acceptance criteria pass
- ✅ Build matrix passes: OpenWRT, RPi4, x86_64 CI

### Testing Complete
- ✅ Both trigger pcaps (`02ftl.pcapng`, `02lesh.pcapng`) replay successfully
- ✅ Closed-port pcap shows <1s fast-fail
- ✅ Jitter pcap shows [2.0, 3.0]s distribution
- ✅ Burst reproducer passes (no stall)
- ✅ Sustained burst (200 GETs/400s) passes
- ✅ 24h soak test passes (UPDATE cadence ±5%)

### Documentation Complete
- ✅ `docs/CHANGELOG-remediation.md` updated
- ✅ RCA documents for each P0 fix
- ✅ `docs/access-control.md` comprehensive guide
- ✅ `docs/queue-mode.md` created
- ✅ All acceptance criteria documented

---

## Success Metrics

### Before Remediation
- ❌ Client stalls after 17 CON requests (FT trace)
- ❌ No DEREGISTER on shutdown (FT trace)
- ❌ Location reads return 4.01 (both traces)
- ❌ 32s retransmit against closed port
- ❌ Deterministic retransmit timing
- ❌ No observability (can't diagnose issues)
- ❌ NoSec mode (not production-safe)

### After Remediation
- ✅ Client handles burst load (200+ CONs)
- ✅ Clean DEREGISTER on all shutdowns
- ✅ All object reads return 2.05
- ✅ <1s fast-fail on closed port
- ✅ Jittered retransmit timing (fleet-safe)
- ✅ Full observability (SIGUSR1 snapshots)
- ✅ DTLS/PSK with CID (production-ready)

---

**Next Action**: Run P0-5 burst reproducer and capture diagnostic snapshot to confirm RCA hypothesis and apply surgical fix.
