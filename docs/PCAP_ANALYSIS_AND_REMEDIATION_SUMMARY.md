# PCAP Analysis and Remediation Summary

## Executive Summary

Analysis of two packet captures (`02ftl.pcapng` and `02lesh.pcapng`) revealed critical defects in the Friendly LwM2M Client. The same client binary behaved drastically differently against two servers:

- **Friendly One-IoT DM**: Client went **silent for 110 seconds** after handling 17 instance-level CON reads
- **Eclipse Leshan**: Client operated normally with clean register/update/serve/deregister cycle

This document summarizes root causes, recommended fixes, and implementation priorities based on the comprehensive directive in `Issues/pcap-combo.md`.

---

## Critical Findings

### Root Cause: Burst-Load Stall (P0-5) - PRIMARY ISSUE

**Symptom**: After handling 17 instance-level GET requests over 36 seconds (FT server), the client:
1. Sent one UPDATE at t=57s
2. Went **completely silent** for remaining 110 seconds
3. Never sent second UPDATE, DEREGISTER, or any application traffic
4. Process still alive (no ICMP Port Unreachable) but event loop wedged

**Counter-Evidence**: Same binary against Leshan (7 object-level GETs over 28s) behaved perfectly.

**Hypothesis**: The 4.01 (Unauthorized) generation path, ACL lookup, or transaction cleanup has a **resource leak or deadlock** triggered by burst density.

**Key Observations**:
- 6 of 17 requests returned 4.01 (correct ACL behavior)
- Reply latency on 4.01 path **increased** during burst (0.73-0.86s vs 0.18s baseline)
- Latency creep suggests progressive resource accumulation
- Event loop stalled **after** burst completed, not during

---

## Priority Defects and Fixes

### P0 (Critical - Prevents Production Use)

#### **P0-5: Burst-Load Stall** ⚠️ **MUST FIX FIRST**

**Investigation Areas** (in order):

1. **Wakaama Transaction List Audit** (`liblwm2m/transaction.c`)
   - Verify 4.01 reply path calls `transaction_remove()` identically to 2.05 path
   - Check for early-return paths in ACL-check chain that bypass cleanup
   - Look for leaks in `prv_handleOperation()` and object handlers

2. **Token/MID Allocator Audit**
   - Verify 4.01 uses same allocation path as 2.05
   - Check for leaks under repeated calls
   - Latency creep on early 4.01s suggests extra work in first few allocations

3. **Object Handler Reentrancy**
   - `/2` (Access Control) handler re-entered for each instance
   - Check for non-recursive locks held across access-check
   - Sequential reentry could deadlock or accumulate resources

4. **Platform Timer Scheduler**
   - If timer queue uses bounded storage, busy I/O could starve UPDATE timer
   - Check `lwm2m_step()` dispatcher interaction with timer-armed callbacks

5. **NVM Write Contention**
   - If logging 4.01 denials synchronously to NVM, 6 in 30s could saturate
   - Could indirectly stall event loop

**Required Deliverables**:
- Burst-load reproducer harness (Python script using Leshan HTTP API)
- Instrumented build with ring-buffer logging (P1-8)
- RCA write-up identifying exact cause with code references

**Acceptance Criteria**:
- ✅ Reproducer reliably stalls unfixed client (N=17 GETs, 36s, 6/11 4.01s)
- ✅ Reproducer does NOT stall fixed client
- ✅ Sustained burst test (N=200 over 400s) completes with healthy client
- ✅ Post-burst UPDATE fires within ±5% of `lt × 0.75`
- ✅ All transactions reaped (count returns to baseline within 5s)
- ✅ Memory/FD/lock counts return to baseline within 10s

---

#### **P0-1: UPDATE Scheduler** (GATED - Reproduce First)

**Status**: Inconclusive. Needs reproducer before any code changes.

**Evidence**:
- FT trace: UPDATE at t=57s, then silence (but likely downstream of P0-5)
- Leshan trace: UPDATE at t=88s, session ended at t=117s (before 2nd UPDATE due)

**Action**: Create reproducer test first:
```cpp
// tests/integration/test_long_idle_session.cpp
// Register with lt=60, no operator reads/writes for 5 minutes
// Assert UPDATEs fire every 42-48s (0.75 × lt ± 5%)
```

**If test FAILS** → Apply fix in registration.c UPDATE re-arm logic
**If test PASSES** → Close with note, was downstream of P0-5

---

#### **P0-2: No Registration Watchdog**

**Issue**: No fallback if UPDATE silently fails. Single missed UPDATE drops device from registry.

**Fix**: Independent watchdog that detects `(now - t_last_successful_update_ack) > (lt - safety_margin)` and forces re-REGISTER.

**Implementation**:
```cpp
safety_margin = max(lt × 0.10, 5s)
if (now - t_last_successful_update_ack) > (lt - safety_margin) {
    transition_to(STATE_REG_REGISTRATION_REQUIRED);
}
```

**Requirements**:
- Runs from `lwm2m_step()` tick, independent of UPDATE scheduler
- Preserves endpoint name, rebuilds link-format from current objects
- New Location-Path from 2.01 ACK replaces old one
- RFC 7252 backoff on persistent failure (cap at 1h)

---

#### **P0-3: NoSec Mode in Production**

**Issue**: Both traces use plaintext UDP/5683. **Not production-safe.**

**Fix**: Implement PSK with mbedTLS + DTLS CID (RFC 9146).

**Deliverables**:
1. CMake option `FTL_LWM2M_SECURITY_MODE=PSK|RPK|X509|NOSEC` (default PSK)
2. mbedTLS config: enable `MBEDTLS_SSL_DTLS_CONNECTION_ID`
3. Security Object provisioning: `/0/0/2=0`, `/0/0/3=<PSK identity>`, `/0/0/5=<key>`
4. URI scheme: `coaps://host:5684` when security ≠ NoSec
5. **CID negotiation: MUST request non-zero-length CID** (4-8 bytes recommended)

**Critical**: Zero-length CIDs forbidden (defeats NAT survival on PSM cycles).

---

#### **P0-4: Queue Mode Mismatch**

**Issue**: Both traces register with `b=U` (always-on) but device may sleep.

**Fix**:
- Config option for `"binding": "U" | "UQ"`
- When `UQ`, stay receive-active for `MAX_TRANSMIT_WAIT` (93s) after UPDATE

---

### P1 (High - Production Quality Issues)

#### **P1-7: Location AC Instance Missing** ✅ **EASY WIN**

**Symptom**:
- FT: `GET /6/0` → `4.01 Unauthorized` (frame 169)
- Leshan: `GET /6` → `4.01 Unauthorized` (frame 1681)

**Root Cause**: Bootstrap config has AC instances for objects 1,3,4,5,3339 but **NOT for object 6 (Location)**.

**Fix** (ONE CONFIG LINE):
```cpp
// src/objects/access_control_object.cpp
{
    .object_id   = 6,        // Location
    .instance_id = 0,
    .acl         = { },      // empty = Owner has all rights
    .owner_ssid  = 123,      // matches LwM2M Server SSID
}
```

**Deliverables**:
1. Add missing AC instance to bootstrap config
2. Create `tools/audit-acl-coverage.py` to verify every object has AC coverage
3. Update `docs/access-control.md` documenting AC-coverage rule

**Acceptance**:
- ✅ Replaying either pcap's read pattern produces no 4.01 on `/6`
- ✅ Audit tool runs in CI, gates merges

---

#### **P1-8: Logging/Observability** 🔴 **CRITICAL - DO FIRST**

**Status**: **Promoted to CRITICAL** - P0-5 investigation impossible without this.

**Why Critical**: When client went silent at t=58.74s, no in-band signal explains why. Can't diagnose without observability.

**Required Deliverables**:

1. **Persistent Ring-Buffer Log**
   - Location: NVM (`src/platform/.../log_ring.cpp`)
   - Size: 4 KiB (MCU), 64 KiB (Linux)
   - Content: State-machine transitions with monotonic timestamps

2. **Structured Log Lines**
   - Format: JSON lines (Linux), binary records (MCU)
   - Fields: `t_mono_us`, `state_from`, `state_to`, `trigger`, `error_code`

3. **Resource Introspection Counters**
   - Updated on every state transition
   - Counters: `transaction_count`, `pending_token_count`, `mid_alloc_count`, `nvm_inflight_writes`

4. **SIGUSR1 Snapshot Handler** (Linux)
   - Writes state dump to `/tmp/ftl-lwm2m-snapshot.json`
   - Contains: registration state, last 10 UPDATE timestamps, current `lt`, last 16 MIDs, last 4 DTLS outcomes, all introspection counters

**Acceptance**:
- ✅ After burst reproducer, SIGUSR1 reveals which counter is leaking
- ✅ Ring-buffer alone answers "when did last UPDATE succeed?" without pcap
- ✅ All timestamps use same monotonic clock as `lwm2m_step()`

---

#### **P1-6: DEREGISTER Reliability** (SCOPE REVISED)

**Original v1 Finding**: "Client doesn't send DEREGISTER" → **REFUTED**

**Evidence**: Leshan frame 2524 shows clean `CON DELETE /rd/fgCzoj1blP` at t=117.22s.

**Revised Scope**: DEREGISTER path exists and works on healthy client. Need to verify it remains reliable **after burst-load event** (P0-5).

**Hypothesis**: FT trace's missing DEREGISTER is downstream symptom of burst stall.

**Required**:
- Test: `test_deregister_after_burst.cpp`
  1. Register
  2. Drive burst-CON pattern (P0-5)
  3. 5s after burst, send SIGTERM
  4. Assert `CON DELETE` within 5s
- 5-second SIGTERM timeout, then fall through with structured log event

---

#### **P1-5: Default Lifetime Too Short**

**Issue**: Both traces use `lt=60` (debug value, not production).

**Fix**: Production defaults:

| Target | Default `lt` | Rationale |
|--------|-------------|-----------|
| Always-on (OpenWRT, RPi4) | 300s | Balance detection vs flooding |
| NB-IoT/LTE-M with PSM | 3600s | Match PSM TAU of 1h |
| Battery sleepy device | 86400s | OMA spec default |

**Requirement**: `lt < 60` triggers config validation error at startup.

---

## Implementation Priority

**CRITICAL PATH** (Do in this exact order):

```
1. P1-8 (Observability) ← Without this, can't debug P0-5
         ↓
2. P0-5 (Burst Stall) ← Prime suspect, may resolve P0-1 as side-effect
         ↓
3. P0-1 Reproducer   ← If P0-5 fixed UPDATE, P0-1 closes with note
         ↓
4. P1-7 (Location AC) ← One-line fix, do same week as P0-5
         ↓
5. P1-6 Verification  ← Write post-burst-DEREGISTER test
         ↓
6. P0-3 (DTLS/PSK)    ← Substantial work, separate milestone
         ↓
7. P0-2, P0-4, P1-5   ← Straightforward, parallel with DTLS
```

---

## Code Quality Requirements

### Cross-Cutting Rules

1. **No New Wakaama Wrappers**: Use `search-first`, confirm primitives don't already exist
2. **C++17 RAII**: No `new`/`delete`, use `std::unique_ptr`, `std::span`
3. **No Silent Failures**: Return `std::expected<T, ErrorCode>`, no `void` for failable functions
4. **TDD Mandatory**: Tests first, ≥80% line coverage via gcov/lcov
5. **Build Matrix**: `aarch64-openwrt-linux-musl`, `aarch64-linux-gnu`, `x86_64-linux-gnu`
6. **No printf in Production**: Only ring-buffer logging

### Skills Required

- `ecc-tdd-workflow` - Test-driven development
- `karpathy-guidelines` - Avoid LLM coding mistakes
- `ecc-verification-loop` - Manual reasoning validation
- `ecc-search-first` - Research before coding

---

## Verification Harness

All P0 fixes must pass:

```bash
./tools/verify-pcap-regressions.sh \
  --client-binary ./build/ftl-lwm2m-client \
  --leshan-uri coap://127.0.0.1:5683 \
  --duration 1800 \
  --lt 60 \
  --pattern leshan-paced       # 7 object-level CONs/28s
  --pattern ft-burst            # 17 instance-level CONs/36s
  --pattern sustained-burst     # 200 mixed CONs/400s
  --expect update_cadence_within_75pct \
  --expect no_silent_window_gt_lt \
  --expect no_stall_post_burst \
  --expect deregister_on_sigterm \
  --expect deregister_on_post_burst_sigterm \
  --expect ac_reads_no_unexpected_4_01 \
  --expect location_read_succeeds \
  --expect transactions_drain_within_5s \
  --report-out ./artifacts/regression-report.txt
```

---

## Burst-Load Reproducer (P0-5)

**Python harness** (`tools/burst-load-reproducer/burst_driver.py`):

```python
import requests, time, sys

LESHAN_API = "http://localhost:8080/api/clients"
ENDPOINT   = "svdwalt007-test"

# FT-trace pattern: 17 instance-level reads, 36s, 6x 4.01
FT_PATTERN = [
    ("/3/0",    "expect_2.05"),  ("/3339/0", "expect_2.05"),
    ("/2/10",   "expect_2.05"),  ("/4/0",    "expect_2.05"),
    ("/2/2",    "expect_4.01"),  ("/5/0",    "expect_2.05"),
    ("/2/3",    "expect_2.05"),  ("/2/0",    "expect_4.01"),
    ("/2/1",    "expect_2.05"),  ("/1/0",    "expect_2.05"),
    ("/2/8",    "expect_4.01"),  ("/2/9",    "expect_4.01"),
    ("/2/6",    "expect_4.01"),  ("/2/7",    "expect_2.05"),
    ("/6/0",    "expect_4.01"),  ("/2/4",    "expect_4.01"),
    ("/2/5",    "expect_2.05"),
]

def drive_burst(pattern, duration_s):
    interval = duration_s / len(pattern)
    for path, expectation in pattern:
        r = requests.get(f"{LESHAN_API}/{ENDPOINT}{path}", timeout=10)
        check_expectation(r, expectation)
        time.sleep(interval)

def check_post_burst_health(timeout_s=300):
    """Watch for next UPDATE within (lt * 0.75 + 5%)."""
    # Poll Leshan registration state or observation events
    ...

if __name__ == "__main__":
    drive_burst(FT_PATTERN, duration_s=36)
    if not check_post_burst_health():
        print("STALL DETECTED — P0-5 reproduced", file=sys.stderr)
        sys.exit(1)
```

---

## Definition of Done

Defect closed when **ALL** true:

- ✅ Acceptance criteria mechanically verified in CI
- ✅ Both trigger pcaps replayed against fixed client produce no failure
- ✅ No regression in existing test suite
- ✅ Ring-buffer log (P1-8) shows corrected behavior
- ✅ `docs/CHANGELOG-remediation.md` updated with change rationale

---

## Quick Reference: Defect Summary

| ID | Title | Severity | Status | Quick Fix? |
|----|-------|----------|--------|-----------|
| **P0-5** | Burst-load stall | Critical | Active | No - Requires RCA |
| **P0-1** | UPDATE scheduler | Critical | Gated | Unknown - Reproduce first |
| **P0-2** | No registration watchdog | Critical | Active | Medium complexity |
| **P0-3** | NoSec in production | Critical | Active | Large - DTLS/mbedTLS |
| **P0-4** | Queue Mode mismatch | Critical | Active | Medium complexity |
| **P1-7** | Location AC missing | High | Active | **YES - One line** |
| **P1-8** | Logging/observability | **Critical** | Active | Medium complexity |
| **P1-6** | DEREGISTER reliability | High | Revised | Test verification only |
| **P1-5** | Default lifetime short | High | Active | **YES - Config change** |

---

## Key Insights from Cross-Trace Analysis

1. **Burst Density Matters**: 17 CONs/36s stalled, 7 CONs/28s didn't → Load pattern is differentiator
2. **4.01 Path Suspect**: Latency creep on 4.01 responses suggests progressive resource accumulation
3. **DEREGISTER Works**: Leshan trace proves clean shutdown exists, FT absence is downstream symptom
4. **Observability Gap**: Can't diagnose production issues without ring-buffer logging
5. **AC Coverage Gap**: Location object inaccessible due to missing AC instance

---

## Next Steps

### Immediate (Week 1)

1. **Implement P1-8** (Ring-buffer logging + SIGUSR1 snapshot)
   - Required for all other investigation
   - 3-5 days of work

2. **Create P0-5 Reproducer**
   - Python burst-load harness
   - Confirm reproduces stall
   - 1-2 days of work

### Week 2-3

3. **Fix P0-5** (Burst-load stall)
   - Use reproducer + ring-buffer logs
   - Audit transaction/token/ACL paths
   - RCA write-up

4. **Fix P1-7** (Location AC)
   - One-line config fix
   - Add audit tool
   - 1 day of work

### Week 4+

5. **Run P0-1 Reproducer** (UPDATE scheduler)
   - If fails → fix registration.c
   - If passes → close with note

6. **Verify P1-6** (DEREGISTER after burst)

7. **Implement P0-3** (DTLS/PSK/CID)
   - Separate milestone
   - 2-3 weeks of work

8. **Implement P0-2, P0-4, P1-5**
   - Can parallelize with DTLS

---

**Authority**: `Issues/pcap-combo.md` v2 is authoritative. This summary for quick reference only.
