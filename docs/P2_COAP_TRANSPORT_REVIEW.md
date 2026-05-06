# P2 CoAP/UDP Transport Hardening - Review and Implementation Plan

**Directive**: `Issues/CLAUDE-coap-transport.md`
**Priority**: P2 (Not blocking, but high-leverage for field robustness)
**Status**: Ready for implementation
**Estimated Effort**: 3-5 days (both items in single PR)

---

## Executive Summary

Analysis of pcap `03ADV.pcapng` revealed two RFC 7252 compliance issues in the CoAP/UDP transport layer:

1. **P2-9**: ICMP Port Unreachable ignored → Client retransmits for 32s against dead server
2. **P2-10**: No random jitter on retransmit timeouts → Deterministic timing (fleet thundering herd risk)

Both are **small, mechanical fixes** with **high field-robustness value**.

---

## Issue 1: P2-9 - ICMP Port Unreachable Ignored

### Problem

**Observed behavior** (`03ADV.pcapng`):
```
t=  2.286   CON POST /rd?...    ↳ ICMP Port Unreachable (+0.305 ms)
t=  4.286   [Retx 1, +2.000 s]  ↳ ICMP Port Unreachable
t=  8.287   [Retx 2, +4.001 s]  ↳ ICMP Port Unreachable
t= 16.288   [Retx 3, +8.002 s]  ↳ ICMP Port Unreachable
t= 32.280   [Retx 4, +15.991 s] ↳ ICMP Port Unreachable
```

**Problem**: Client ignores ICMP errors and retries for **32 seconds** despite kernel reporting destination unreachable after every attempt (0.3ms latency).

### Root Cause

**Linux unconnected UDP sockets** deliver ICMP errors via the **socket error queue** (`MSG_ERRQUEUE`), not as return values from `recvfrom()`.

**Without**:
- `setsockopt(IP_RECVERR, 1)` enabled
- Explicit `recvmsg(..., MSG_ERRQUEUE)` calls

**Result**: ICMP errors silently discarded by kernel.

### RFC 7252 Requirement

**§4.4, §10.1**: CoAP implementation SHOULD treat repeated ICMP Destination Unreachable as transmission failure.

**On Port Unreachable**:
1. Cancel in-flight transaction immediately
2. Skip remaining retransmissions
3. Surface error to registration state machine
4. Apply same backoff as MAX_RETRANSMIT exhaustion

### Proposed Fix

#### Linux Implementation

**File**: `src/platform/linux/socket_udp.cpp`

```cpp
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <linux/errqueue.h>

void enable_icmp_error_reporting(int fd, bool ipv6) {
    int on = 1;
    if (ipv6) {
        setsockopt(fd, IPPROTO_IPV6, IPV6_RECVERR, &on, sizeof(on));
    } else {
        setsockopt(fd, IPPROTO_IP, IP_RECVERR, &on, sizeof(on));
    }
}

int drain_error_queue(int fd,
                      std::function<void(const sockaddr_storage&,
                                         int icmp_type,
                                         int icmp_code)> on_icmp_unreachable)
{
    int processed = 0;
    for (;;) {
        char ctrl[256];
        sockaddr_storage peer{};
        iovec iov{};
        char dummy[1];
        iov.iov_base = dummy;
        iov.iov_len = sizeof(dummy);

        msghdr msg{};
        msg.msg_name = &peer;
        msg.msg_namelen = sizeof(peer);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = ctrl;
        msg.msg_controllen = sizeof(ctrl);

        ssize_t n = ::recvmsg(fd, &msg, MSG_ERRQUEUE | MSG_DONTWAIT);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) return processed;
            return -1;
        }

        // Walk control messages for extended error
        for (cmsghdr* c = CMSG_FIRSTHDR(&msg); c; c = CMSG_NXTHDR(&msg, c)) {
            bool v4 = (c->cmsg_level == IPPROTO_IP && c->cmsg_type == IP_RECVERR);
            bool v6 = (c->cmsg_level == IPPROTO_IPV6 && c->cmsg_type == IPV6_RECVERR);
            if (!v4 && !v6) continue;

            auto* ee = reinterpret_cast<sock_extended_err*>(CMSG_DATA(c));
            if (ee->ee_origin == SO_EE_ORIGIN_ICMP ||
                ee->ee_origin == SO_EE_ORIGIN_ICMP6) {
                if (ee->ee_type == ICMP_DEST_UNREACH) {
                    on_icmp_unreachable(peer, ee->ee_type, ee->ee_code);
                    ++processed;
                }
            }
        }
    }
}
```

**File**: `src/transport/coap_transport.cpp`

```cpp
void CoapTransport::poll_once() {
    // 1) Drain ICMP errors FIRST
    drain_error_queue(socket_fd_, [this](const sockaddr_storage& peer, int t, int c) {
        // P1-8: Structured logging to ring buffer
        ftl::get_log_ring().log_event("icmp_unreachable", peer, t, c);

        if (c == ICMP_PORT_UNREACH) {
            // Cancel in-flight transaction for this peer
            transactions_.cancel_by_peer(peer, TransportError::PortUnreachable);
        }
    });

    // 2) Normal receive path
    receive_datagrams();
}
```

#### Zephyr Implementation (Future nRF9161)

**Strategy**: Use **connected UDP** + check `send()` return value

```cpp
// src/platform/zephyr/socket_udp.cpp
ssize_t send_datagram(int fd, std::span<const uint8_t> buf) {
    ssize_t n = ::send(fd, buf.data(), buf.size(), 0);
    if (n < 0 && errno == ECONNREFUSED) {
        // ICMP error surfaced via connected UDP socket
        on_transport_error_(TransportError::PortUnreachable);
        return n;
    }
    return n;
}
```

**Trade-off**: Connected UDP socket can only receive from one peer. OK for LwM2M v1.1 single-server, but breaks Bootstrap-Server or mid-session migration (LwM2M v1.2 §6.1.3).

### Acceptance Criteria

- ✅ Pointing client at closed port causes `port_unreachable` event within **1 second** (not 32s)
- ✅ Registration state machine applies same backoff as MAX_RETRANSMIT exhaustion
- ✅ `IP_RECVERR`/`IPV6_RECVERR` enabled on all LwM2M UDP sockets
- ✅ Zephyr targets use connected-UDP fast-fail
- ✅ No regression on healthy server connection

### Test Plan

**Unit Test**: `tests/transport/test_icmp_unreachable.cpp`
- Fake socket layer injects synthetic `IP_RECVERR` cmsgs
- Verify `drain_error_queue` invokes callback with correct peer/ICMP code
- Verify transaction cancelled by 5-tuple

**Integration Test**: `tests/integration/test_closed_port_fast_fail.cpp`
- Launch client against closed port
- Assert `port_unreachable` in log within 1s
- Assert registration state machine reports failure within 1s

**Regression**: Run Leshan-paced and FT-burst patterns, confirm they pass

---

## Issue 2: P2-10 - No Random Jitter on Retransmit Timeout

### Problem

**Observed retransmit deltas**: 2.000, 4.001, 8.002, 15.991 seconds

**Problem**: Pure deterministic doublings with **no random factor**.

### Root Cause

RFC 7252 §4.8.2 requires:

> *The initial timeout is set to a random duration between `ACK_TIMEOUT` and `(ACK_TIMEOUT × ACK_RANDOM_FACTOR)`.*

**Spec defaults**:
- `ACK_TIMEOUT = 2s`
- `ACK_RANDOM_FACTOR = 1.5`
- → Initial timeout uniformly in **[2.0, 3.0]** seconds

**Observed**: Exact 2.000s = using `ACK_TIMEOUT` directly without jitter.

### Why This Matters

**Fleet behavior**: Devices with deterministic retransmit create **coordinated traffic spikes** after network outage.

**Random factor exists to spread retransmissions** → Avoid thundering herd during recovery.

### Required Behavior

For each CON transaction:

```
INITIAL_TIMEOUT = uniform_random(ACK_TIMEOUT, ACK_TIMEOUT × ACK_RANDOM_FACTOR)
NEXT_TIMEOUT    = 2 × PREVIOUS_TIMEOUT
```

**Example**: If initial = 2.7s → sequence: 2.7s, 5.4s, 10.8s, 21.6s (NOT 2.0, 4.0, 8.0, 16.0)

### Random Source Requirements

- Seeded once at startup from non-deterministic source (`std::random_device`, `getrandom(2)`)
- Thread-safe (use `thread_local` or mutex)
- **NOT** `rand()` (not thread-safe, poor distribution)

### Proposed Fix

**File**: `src/transport/coap_retransmit.h`

```cpp
namespace ftl::lwm2m::coap {

constexpr std::chrono::milliseconds kAckTimeout{2000};
constexpr double                    kAckRandomFactor = 1.5;
constexpr int                       kMaxRetransmit   = 4;

// Returns initial CON retransmit timeout, sampled per RFC 7252 §4.8.2
std::chrono::milliseconds sample_initial_timeout();

// Returns timeout for Nth retransmission (N >= 1), given initial
std::chrono::milliseconds nth_retransmit_timeout(
    std::chrono::milliseconds initial, int retx_n);

}  // namespace ftl::lwm2m::coap
```

**File**: `src/transport/coap_retransmit.cpp`

```cpp
#include "coap_retransmit.h"
#include "platform/random.h"

namespace ftl::lwm2m::coap {

std::chrono::milliseconds sample_initial_timeout() {
    // Uniform in [ACK_TIMEOUT, ACK_TIMEOUT × ACK_RANDOM_FACTOR]
    const auto lo = static_cast<double>(kAckTimeout.count());
    const auto hi = lo * kAckRandomFactor;
    std::uniform_real_distribution<double> dist(lo, hi);
    const auto sample = dist(platform::random_engine());
    return std::chrono::milliseconds{static_cast<int64_t>(sample)};
}

std::chrono::milliseconds nth_retransmit_timeout(
    std::chrono::milliseconds initial, int retx_n) {
    // Each retransmit doubles previous timeout
    return initial * (1 << retx_n);
}

}  // namespace ftl::lwm2m::coap
```

**File**: `src/platform/linux/random.cpp`

```cpp
#include "random.h"
#include <random>

namespace ftl::lwm2m::platform {

std::mt19937& random_engine() {
    // Thread-local, seeded once per thread
    thread_local std::mt19937 engine{std::random_device{}()};
    return engine;
}

}  // namespace ftl::lwm2m::platform
```

**Zephyr**: Replace `std::random_device` with `sys_rand32_get()` from `<zephyr/random/random.h>`.

### Wakaama Integration

Wakaama **already accepts** random factor via `COAP_RESPONSE_RANDOM_FACTOR`.

**Find** the call site that arms transactions (likely `transaction_send_wrapper()` or setting `transacP->retrans_time`).

**Replace** direct `LWM2M_COAP_DEFAULT_RESPONSE_TIMEOUT` assignment with `sample_initial_timeout()`.

### Acceptance Criteria

- ✅ Initial CON timeout in `[2000, 3000]` ms
- ✅ Across 100 transactions, values uniformly distributed (K-S test passes at α=0.05)
- ✅ Each subsequent timeout exactly `2×` previous (no additional jitter on doublings)
- ✅ Pcap shows initial retransmit deltas spread across [2.0, 3.0]s range across 100 sessions
- ✅ No regression in existing retransmit tests (update to accept range)

### Test Plan

**Unit Test**: `tests/transport/test_retransmit_jitter.cpp`
- Sample `sample_initial_timeout()` 10,000 times
- Assert: `min ≥ 2000`, `max ≤ 3000`, `|mean - 2500| < 25` ms
- Assert: `nth_retransmit_timeout(2500, 1) == 5000`, `(2500, 2) == 10000`

**Integration Test**:
- Drive 50 CON exchanges against peer that drops all datagrams
- Capture deltas in pcap
- Assert initial-retx deltas are NOT all equal

---

## Implementation Strategy

### Single PR Approach

**Branch**: `fix/p2-coap-transport-hardening`

**Rationale**:
- Both items touch same transport layer
- Share test infrastructure
- Faster review than sequential PRs
- Changes don't conflict

### Files to Create

**New files**:
1. `src/transport/coap_retransmit.h`
2. `src/transport/coap_retransmit.cpp`
3. `src/platform/linux/random.h`
4. `src/platform/linux/random.cpp`
5. `src/platform/linux/socket_icmp.h` (ICMP handling)
6. `src/platform/linux/socket_icmp.cpp`
7. `tests/transport/test_icmp_unreachable.cpp`
8. `tests/transport/test_retransmit_jitter.cpp`
9. `tests/integration/test_closed_port_fast_fail.cpp`

**Files to modify**:
1. `src/platform/linux/socket_udp.cpp` - Enable `IP_RECVERR`
2. `src/transport/coap_transport.cpp` - Wire up `drain_error_queue()`
3. `src/core/transaction.cpp` - Add `cancel_by_peer()` API
4. Wakaama integration point - Use `sample_initial_timeout()`
5. `CMakeLists.txt` - Add new sources

### Cross-Cutting Requirements

From directive:

- ✅ **C++17 minimum, RAII everywhere** - Socket FDs in `UniqueFd`, no bare `close()`
- ✅ **No `printf` in production** - Use P1-8 structured logging to ring buffer
- ✅ **TDD workflow** - Tests land before fix
- ✅ **Build matrix**: `aarch64-openwrt-linux-musl`, `aarch64-linux-gnu`, `x86_64-linux-gnu`
- ✅ **No new Wakaama APIs** - Reuse existing random-factor hook

---

## Definition of Done

- ✅ Both acceptance criteria sets pass in CI
- ✅ Closed-port pcap: Client emits one datagram, receives ICMP, exits within 1s
- ✅ Slow-peer pcap: Initial retx deltas spread across [2.0, 3.0]s over 50 trials
- ✅ No regression in `02ftl.pcapng` / `02lesh.pcapng` replay tests
- ✅ Note in `docs/CHANGELOG-remediation.md` citing RFC 7252 §4.4, §4.8.2, §10.1

---

## PR Description Template

```markdown
# P2 CoAP/UDP Transport Hardening

Fixes two RFC 7252 compliance issues identified in pcap `03ADV.pcapng`.

## Issues

### P2-9: ICMP Port Unreachable Ignored
**Problem**: Client retransmitted for 32s against closed port despite ICMP errors.
**Fix**: Enable `IP_RECVERR`, drain error queue before receive, cancel transactions on Port Unreachable.
**Evidence**: Frames 67-717 in `03ADV.pcapng`

### P2-10: No Random Jitter on Retransmit
**Problem**: Deterministic 2.000s initial timeout (fleet thundering herd risk).
**Fix**: Sample initial timeout uniformly in [2.0, 3.0]s per RFC 7252 §4.8.2.
**Evidence**: Exact 2.000, 4.001, 8.002, 15.991s deltas in pcap

## Before/After Measurements

### P2-9: Closed Port Fast-Fail
- **Before**: 32s retransmit cycle (5 attempts)
- **After**: <1s port_unreachable event, immediate cancellation

### P2-10: Jitter Distribution
- **Before**: 100% of sessions start with 2.000s
- **After**: Uniform distribution [2.0, 3.0]s (histogram attached)

## Testing

- ✅ Unit tests: `test_icmp_unreachable`, `test_retransmit_jitter`
- ✅ Integration: `test_closed_port_fast_fail`
- ✅ Regression: FT-burst and Leshan-paced patterns pass

## Directive

Implements `Issues/CLAUDE-coap-transport.md` (P2-9 and P2-10)
```

---

## Integration with Existing Work

### Relationship to P0-5 (Burst Stall)

**Independent**: P2 fixes are transport-layer robustness, don't affect P0-5 investigation.

**Can be implemented in parallel** with P0-5 fix.

### Relationship to P1-8 (Observability)

**Depends on P1-8**: ICMP error logging uses structured ring-buffer events.

**Sequence**: P1-8 → P2 (can start P2 now that P1-8 is complete)

### Relationship to P0-3 (DTLS)

**Prerequisite for P0-3**: ICMP error handling must work with DTLS sockets.

**Sequence**: P2 → P0-3 (test ICMP on CoAP/UDP first, then apply to DTLS)

---

## Effort Estimate

### P2-9 (ICMP Fast-Fail)
- Implementation: 1-2 days
- Testing: 0.5 day
- **Total**: 1.5-2.5 days

### P2-10 (Retransmit Jitter)
- Implementation: 0.5-1 day
- Testing: 0.5 day
- **Total**: 1-1.5 days

### Combined PR
- Integration: 0.5 day
- Documentation: 0.5 day
- **Total**: 3-5 days

---

## Risks and Mitigations

### Risk 1: Wakaama Integration Point Unknown

**Risk**: May not know exactly where to hook `sample_initial_timeout()` into Wakaama.

**Mitigation**:
- Use `grep` to find `COAP_RESPONSE_TIMEOUT` usage in Wakaama
- Check `liblwm2m/transaction.c` for timer arming
- Review existing transaction send wrappers

### Risk 2: RAII Socket Wrapper Doesn't Exist

**Risk**: Directive requires `UniqueFd` wrapper, may not be in codebase.

**Mitigation**:
- Check if `UniqueFd` or similar exists
- If not, create minimal RAII wrapper:
  ```cpp
  class UniqueFd {
      int fd_;
  public:
      explicit UniqueFd(int fd) : fd_(fd) {}
      ~UniqueFd() { if (fd_ >= 0) close(fd_); }
      // Move-only, no copy
  };
  ```

### Risk 3: Thread-Local RNG on Embedded

**Risk**: Zephyr may not support `thread_local`.

**Mitigation**:
- Use Zephyr platform RNG (`sys_rand32_get()`) directly in sample function
- No need for thread-local engine on single-threaded embedded

---

## Recommendation

**Priority**: Implement P2 transport fixes **after P1-8 is verified working** (build succeeds, SIGUSR1 snapshots work).

**Sequence**:
1. ✅ P1-8 complete (ring-buffer logging)
2. ✅ P1-7 complete (Location AC fix)
3. ⏭️ **P2-9 + P2-10** (CoAP transport hardening) ← Do this next
4. ⏭️ P0-5 investigation (burst stall) ← Can run in parallel
5. ⏭️ P0-3 (DTLS/PSK) ← After P2 transport layer is hardened

**Rationale**:
- Small, well-defined fixes with clear acceptance criteria
- High field-robustness value
- Builds foundation for DTLS work (P0-3)
- Can be done in parallel with P0-5 investigation

---

**Status**: Ready for implementation. Directive is comprehensive, requirements clear, test plan well-defined.
