# Immediate Action Plan - PCAP Remediation

> **Authority**: `Issues/pcap-combo.md` v2
> **Created**: 2026-04-30
> **Priority**: Execute in exact order listed

---

## Critical Path Summary

The burst-load stall (P0-5) is the **primary defect** causing the 110-second silence in the FT trace. However, it **cannot be diagnosed** without observability (P1-8).

**Mandatory Sequence**:
```
P1-8 (Observability) → P0-5 Reproducer → P0-5 Fix → Verification
```

---

## Week 1: Foundation

### Day 1-3: P1-8 - Implement Ring-Buffer Logging

**Why First**: Without this, P0-5 investigation is blind guesswork.

#### Deliverables

1. **Ring-Buffer Log Module** (`src/platform/linux/log_ring.cpp`)

```cpp
// src/platform/linux/log_ring.h
#pragma once
#include <cstdint>
#include <array>

struct LogEntry {
    uint64_t t_mono_us;          // Monotonic timestamp
    uint8_t  state_from;         // lwm2m_client_state_t
    uint8_t  state_to;
    uint16_t trigger;            // Event code
    int32_t  error_code;
    uint32_t transaction_count;  // Introspection counters
    uint32_t pending_token_count;
    uint32_t mid_alloc_count;
    uint32_t nvm_inflight_writes;
};

class LogRing {
public:
    static constexpr size_t SIZE = 8192;  // 64 KiB / sizeof(LogEntry)

    void log_transition(uint8_t from, uint8_t to, uint16_t trigger, int32_t err);
    void snapshot_to_file(const char* path);
    const LogEntry& get(size_t index) const;
    size_t count() const;

private:
    std::array<LogEntry, SIZE> entries_;
    size_t head_ = 0;
    uint64_t total_writes_ = 0;
};
```

2. **SIGUSR1 Handler**

```cpp
// examples/main.cpp (add global handler)
#include <csignal>

LogRing g_log_ring;

void sigusr1Handler(int signum) {
    if (signum == SIGUSR1) {
        cout << "[SIGNAL] Received SIGUSR1, writing snapshot..." << endl;

        // Take client ownership to read state
        WppClient* client = WppClient::takeOwnership();

        // Write snapshot
        ofstream snapshot("/tmp/ftl-lwm2m-snapshot.json");
        snapshot << "{\n";
        snapshot << "  \"timestamp\": " << time(NULL) << ",\n";
        snapshot << "  \"client_state\": " << (int)client->getState() << ",\n";
        snapshot << "  \"ring_buffer_entries\": " << g_log_ring.count() << ",\n";
        // ... add transaction_count, token_count, MID_count introspection
        snapshot << "}\n";
        snapshot.close();

        client->giveOwnership();

        cout << "[SIGNAL] Snapshot written to /tmp/ftl-lwm2m-snapshot.json" << endl;
    }
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGUSR1, sigusr1Handler);  // ← Add this
    // ...
}
```

3. **Introspection Counter Integration**

Add counters to WppClient or connection layer:

```cpp
// wpp/client/WppClient.h
class WppClient {
public:
    // Add introspection API
    size_t getTransactionCount() const;
    size_t getPendingTokenCount() const;
    size_t getMidAllocCount() const;
    size_t getNvmInflightWrites() const;

private:
    // Track state
    std::atomic<size_t> transaction_count_{0};
    std::atomic<size_t> pending_token_count_{0};
    // ...
};
```

#### Acceptance Criteria

- ✅ Ring buffer logs every state transition with monotonic timestamp
- ✅ SIGUSR1 writes snapshot to `/tmp/ftl-lwm2m-snapshot.json`
- ✅ Snapshot includes: state, last 10 UPDATE timestamps, counters
- ✅ All timestamps use same clock as `lwm2m_step()`

#### Test Plan

```bash
# Start client
./friendly_example_client -e TestClient -s coap://127.0.0.1:5683 &
CLIENT_PID=$!

# Let it run for 2 minutes
sleep 120

# Trigger snapshot
kill -SIGUSR1 $CLIENT_PID

# Verify snapshot exists and is valid JSON
cat /tmp/ftl-lwm2m-snapshot.json | jq .

# Graceful shutdown
kill -SIGINT $CLIENT_PID
```

---

### Day 4-5: P0-5 - Create Burst-Load Reproducer

**Goal**: Mechanically reproduce the FT-trace stall in controlled environment.

#### Deliverables

1. **Python Burst Driver** (`tools/burst-load-reproducer/burst_driver.py`)

```python
#!/usr/bin/env python3
"""
Burst-load reproducer for P0-5.
Drives instance-level CON GETs against LwM2M client via Leshan HTTP API.
"""
import requests
import time
import sys
import json
from typing import List, Tuple

LESHAN_API = "http://localhost:8080/api/clients"
ENDPOINT = "TestClient"

# FT-trace pattern: 17 instance-level reads over 36s, 6 return 4.01
FT_PATTERN: List[Tuple[str, str]] = [
    ("/3/0",    "expect_2.05"),  # Device
    ("/3339/0", "expect_2.05"),  # AudioClip
    ("/2/10",   "expect_2.05"),  # AC for AudioClip
    ("/4/0",    "expect_2.05"),  # ConnectivityMonitoring
    ("/2/2",    "expect_4.01"),  # AC instance doesn't exist
    ("/5/0",    "expect_2.05"),  # FirmwareUpdate
    ("/2/3",    "expect_2.05"),  # AC for Device
    ("/2/0",    "expect_4.01"),  # AC instance doesn't exist
    ("/2/1",    "expect_2.05"),  # AC for Server
    ("/1/0",    "expect_2.05"),  # Server
    ("/2/8",    "expect_4.01"),  # AC instance doesn't exist
    ("/2/9",    "expect_4.01"),  # AC instance doesn't exist
    ("/2/6",    "expect_4.01"),  # AC instance doesn't exist
    ("/2/7",    "expect_2.05"),  # AC for ConnMon
    ("/6/0",    "expect_4.01"),  # Location (missing AC - P1-7)
    ("/2/4",    "expect_4.01"),  # AC instance doesn't exist
    ("/2/5",    "expect_2.05"),  # AC for FirmwareUpdate
]

def drive_burst(pattern: List[Tuple[str, str]], duration_s: int):
    """Drive the burst pattern over the specified duration."""
    interval = duration_s / len(pattern)
    results = []

    print(f"[BURST] Starting {len(pattern)} requests over {duration_s}s...")
    start_time = time.time()

    for i, (path, expectation) in enumerate(pattern):
        req_start = time.time()

        try:
            url = f"{LESHAN_API}/{ENDPOINT}{path}"
            r = requests.get(url, timeout=10)
            req_latency = time.time() - req_start

            # Check expectation
            expected_code = expectation.split("_")[1]  # "expect_2.05" -> "2.05"
            actual_code = f"{r.status_code // 100}.{r.status_code % 100:02d}"

            match = "MATCH" if expected_code in str(r.status_code) else "MISMATCH"

            results.append({
                "index": i,
                "path": path,
                "expected": expected_code,
                "actual": actual_code,
                "latency_s": req_latency,
                "match": match
            })

            print(f"  [{i+1:2d}/17] {path:12s} → {actual_code} ({req_latency:.3f}s) {match}")

        except requests.exceptions.Timeout:
            print(f"  [{i+1:2d}/17] {path:12s} → TIMEOUT", file=sys.stderr)
            results.append({
                "index": i,
                "path": path,
                "error": "timeout"
            })
        except Exception as e:
            print(f"  [{i+1:2d}/17] {path:12s} → ERROR: {e}", file=sys.stderr)
            results.append({
                "index": i,
                "path": path,
                "error": str(e)
            })

        # Sleep to maintain target interval
        elapsed = time.time() - start_time
        target_time = (i + 1) * interval
        sleep_time = max(0, target_time - elapsed)
        time.sleep(sleep_time)

    total_time = time.time() - start_time
    print(f"[BURST] Completed in {total_time:.1f}s")

    return results

def check_post_burst_health(lt: int, timeout_s: int = 300) -> bool:
    """
    Check if client sends UPDATE after burst.
    Expected: UPDATE within (lt * 0.75) + 5% tolerance.
    """
    expected_update_time = lt * 0.75 * 1.05  # 5% tolerance

    print(f"\n[HEALTH] Waiting for UPDATE (expected within {expected_update_time:.0f}s)...")

    start = time.time()
    last_registration_time = None

    while (time.time() - start) < timeout_s:
        try:
            # Poll Leshan for registration info
            r = requests.get(f"{LESHAN_API}/{ENDPOINT}", timeout=5)
            if r.status_code == 200:
                data = r.json()
                reg_time = data.get("lastUpdate")

                if reg_time and reg_time != last_registration_time:
                    elapsed = time.time() - start
                    print(f"[HEALTH] UPDATE detected at t={elapsed:.1f}s")

                    if elapsed <= expected_update_time:
                        print(f"[HEALTH] ✅ UPDATE on time (within {expected_update_time:.0f}s)")
                        return True
                    else:
                        print(f"[HEALTH] ⚠️  UPDATE late (expected ≤{expected_update_time:.0f}s)")

                    last_registration_time = reg_time

        except Exception as e:
            print(f"[HEALTH] Error polling: {e}", file=sys.stderr)

        time.sleep(1)

    print(f"[HEALTH] ❌ No UPDATE detected within {timeout_s}s - STALL SUSPECTED")
    return False

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="P0-5 Burst-load reproducer")
    parser.add_argument("--endpoint", default="TestClient", help="Client endpoint name")
    parser.add_argument("--duration", type=int, default=36, help="Burst duration in seconds")
    parser.add_argument("--lt", type=int, default=60, help="Client lifetime")
    parser.add_argument("--leshan-url", default="http://localhost:8080", help="Leshan base URL")
    args = parser.parse_args()

    LESHAN_API = f"{args.leshan_url}/api/clients"
    ENDPOINT = args.endpoint

    # Drive burst
    results = drive_burst(FT_PATTERN, args.duration)

    # Save results
    with open("/tmp/burst-results.json", "w") as f:
        json.dump(results, f, indent=2)

    # Check post-burst health
    if not check_post_burst_health(args.lt):
        print("\n❌ STALL DETECTED — P0-5 reproduced", file=sys.stderr)
        sys.exit(1)

    print("\n✅ Client healthy after burst")
    sys.exit(0)
```

2. **Docker Compose Test Harness** (`tools/burst-load-reproducer/docker-compose.yml`)

```yaml
version: '3.8'
services:
  leshan:
    image: eclipse/leshan:latest
    ports:
      - "8080:8080"  # HTTP UI
      - "5683:5683/udp"  # CoAP
    command: -vv

  client:
    build: ../../
    network_mode: "host"
    depends_on:
      - leshan
    command: >
      ./friendly_example_client
      -e TestClient
      -s coap://127.0.0.1:5683
      --lifetime 60
```

3. **Integration Test** (`tests/integration/test_burst_load.cpp`)

```cpp
#include <gtest/gtest.h>
#include "WppClient.h"

TEST(BurstLoad, NoStallUnder17Gets36Seconds) {
    // Start client
    // Drive 17 instance-level GETs over 36s via Python reproducer
    // Assert: UPDATE fires within 48s post-burst (60 * 0.75 + 5%)
    // Assert: transaction_count returns to 0 within 5s
    // Assert: memory/FD counts return to baseline within 10s
}
```

#### Acceptance Criteria

- ✅ Reproducer runs against local Leshan
- ✅ Reproducer reliably stalls **unfixed** client
- ✅ Burst pattern matches FT trace (17 reads, 6x 4.01, 36s)
- ✅ Post-burst health check detects stall

#### Test Plan

```bash
# Start Leshan + unfixed client
cd tools/burst-load-reproducer
docker-compose up -d

# Wait for registration
sleep 10

# Run reproducer (expect FAIL on unfixed client)
python3 burst_driver.py --endpoint TestClient --duration 36 --lt 60

# Check snapshot after stall
kill -SIGUSR1 $(pgrep friendly_example_client)
cat /tmp/ftl-lwm2m-snapshot.json

# Expected: transaction_count > 0, pending_token_count > 0 (leak detected)
```

---

## Week 2-3: Investigation and Fix

### P0-5 Root Cause Analysis

**Using**: Ring-buffer logs + reproducer + SIGUSR1 snapshots

#### Investigation Checklist

1. **Transaction List Audit**
   - [ ] Run reproducer with instrumented build
   - [ ] Check SIGUSR1 snapshot: `transaction_count` before/after burst
   - [ ] If leaking: audit `liblwm2m/transaction.c` for missing `transaction_remove()` calls
   - [ ] Check 4.01 path in `prv_handleOperation()` for early returns

2. **Token/MID Allocator Audit**
   - [ ] Check snapshot: `pending_token_count`, `mid_alloc_count`
   - [ ] If leaking: audit allocation/deallocation symmetry
   - [ ] Verify 4.01 path uses same alloc as 2.05

3. **Object Handler Reentrancy**
   - [ ] Instrument `/2` (Access Control) handler with entry/exit logs
   - [ ] Check for lock acquisition during access check
   - [ ] Verify no deadlock on sequential reentry

4. **Timer Scheduler**
   - [ ] Check if `lwm2m_step()` still being called post-burst
   - [ ] Verify timer queue isn't saturated
   - [ ] Check UPDATE timer is re-armed

5. **NVM Write Contention**
   - [ ] Check snapshot: `nvm_inflight_writes`
   - [ ] If accumulating: make 4.01 logging async or remove

#### Fix Template

Based on RCA, apply **minimal surgical fix**:

```cpp
// Example: If transaction leak in 4.01 path
// liblwm2m/transaction.c or our wrapper

// Before (hypothetical bug):
if (result == COAP_401_UNAUTHORIZED) {
    return;  // ← BUG: early return without cleanup
}

// After (fix):
if (result == COAP_401_UNAUTHORIZED) {
    transaction_remove(context, transaction);  // ← Add cleanup
    return;
}
```

#### Documentation

Create `docs/burst-stall-rca.md`:

```markdown
# P0-5 Burst-Stall Root Cause Analysis

## Summary
[One-paragraph summary of root cause]

## Evidence
- Reproducer: [link to Python script]
- Unfixed behavior: [snapshot showing leak]
- Fixed behavior: [snapshot showing cleanup]

## Root Cause
[Detailed explanation with code references]

## Fix
[Diff and explanation]

## Verification
[Test results showing fix]
```

---

## Quick Wins (Parallel Track)

### P1-7: Location AC Instance (1 Hour)

**File**: `src/objects/access_control_object.cpp` or bootstrap config

```cpp
// Add to AC instance array:
{
    .object_id   = 6,        // Location
    .instance_id = 0,
    .acl         = { },      // Owner has all rights
    .owner_ssid  = 123,
}
```

**Test**:
```bash
# Start client, read Location from Leshan
curl http://localhost:8080/api/clients/TestClient/6/0

# Expected: 2.05 Content (not 4.01)
```

---

## Success Metrics

### Week 1 Complete

- ✅ Ring-buffer logging operational
- ✅ SIGUSR1 snapshot handler working
- ✅ Reproducer reliably stalls unfixed client
- ✅ Snapshot reveals which counter is leaking

### Week 2-3 Complete

- ✅ RCA document written with code references
- ✅ Fix applied and tested
- ✅ Reproducer **does NOT stall** fixed client
- ✅ Sustained burst test (200 GETs/400s) passes
- ✅ Post-burst UPDATE fires on schedule
- ✅ All counters return to baseline

### P1-7 Complete

- ✅ Location reads return 2.05 (not 4.01)
- ✅ AC audit tool in CI

---

## Daily Standup Template

**What I did yesterday**:
- [Task from plan]

**What I'm doing today**:
- [Next task from plan]

**Blockers**:
- [Any issues]

**Metrics**:
- Ring-buffer entries logged: [count]
- Reproducer stall rate: [%]
- Transaction leak confirmed: [yes/no]

---

**Remember**: This is the **critical path**. Every other fix depends on P1-8 and P0-5. Do not deviate from sequence.
