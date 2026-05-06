# CLAUDE-coap-transport.md — FTL Client CoAP/UDP Transport Hardening

> **Scope:** `github.com/svdwalt007/Friendly-LwM2M-Client` — `src/transport/`, `src/platform/<target>/socket_*.{cpp,h}`, and the Wakaama integration in `src/core/`.
> **Trigger pcap:** `03ADV.pcapng` — same client binary against `127.0.0.1:5683`, server not listening, capture exposed two distinct transport-layer defects.
> **Status:** Active. Both items below are **P2** — not blocking, but small, mechanical, and high-leverage for field robustness. Land them in a single PR.

## 1 — Context

The trigger pcap captures a pre-condition failure: the Advanced LwM2M Server CPP was not bound on the destination port. Five `CON POST /rd?…` attempts from the client over ~32 s, every one of them met with `ICMP Type 3 Code 3` (Port Unreachable). The dialog itself is uninteresting (no server response was ever produced), but the client's behaviour exposed two minor RFC 7252 deviations that this directive fixes.

```
t=  2.286   CON POST /rd?...    ↳ ICMP Port Unreachable (+0.305 ms)
t=  4.286   [Retx 1, +2.000 s]  ↳ ICMP Port Unreachable
t=  8.287   [Retx 2, +4.001 s]  ↳ ICMP Port Unreachable
t= 16.288   [Retx 3, +8.002 s]  ↳ ICMP Port Unreachable
t= 32.280   [Retx 4, +15.991 s] ↳ ICMP Port Unreachable
            client gives up (RFC 7252 MAX_RETRANSMIT=4 reached)
```

Two issues:

1. **ICMP Port Unreachable was ignored** — the client kept retransmitting for 32 s against a destination the kernel had repeatedly told it was dead.
2. **Retransmit timing has no random jitter** — exact 2.000 / 4.001 / 8.002 / 15.991 s deltas, deterministic doublings of `ACK_TIMEOUT = 2 s`. RFC 7252 §4.8.2 mandates a random factor.

Both are fixed in the transport layer. No registration state-machine changes required.

---

## 2 — P2-9 · Honor ICMP Port Unreachable to fail fast

### Symptom

5 × `CON POST /rd` retransmissions over 32 s against a closed UDP port, despite the kernel surfacing `ICMP Type 3 Code 3` after every single send. The ICMP arrived 0.3 ms after each transmission — well within any reasonable polling window — but the client's recv path never read the kernel's error queue, so the registration state machine had no idea the destination was unreachable.

### Root cause

The CoAP transport layer uses an unconnected UDP socket and `sendto()` / `recvfrom()`. On Linux, ICMP errors on unconnected sockets are delivered via the **socket error queue** (`MSG_ERRQUEUE`), not as return values from the next `recvfrom()`. Without `setsockopt(SO, IP_RECVERR)` enabled and an explicit `recvmsg(…, MSG_ERRQUEUE)` drain in the receive path, ICMP errors are silently discarded.

On embedded targets (Zephyr / LwIP) the equivalent mechanism is `errno == ECONNREFUSED` returned from a *connected* UDP socket's `send()` — but only if the socket has been `connect()`-ed to the peer. With unconnected UDP, errors are dropped.

### Required behavior

Per RFC 7252 §4.4 and §10.1, a CoAP implementation SHOULD treat repeated `ICMP Destination Unreachable` as a transmission failure for the affected transaction. After receiving Port Unreachable (specifically), the client MUST:

1. Cancel the in-flight CoAP transaction for that destination immediately.
2. Skip remaining retransmissions for that MID.
3. Surface the error to the registration state machine as a transport failure.
4. Apply the same fall-through path that an exhausted retransmit triggers (re-REGISTER backoff, log structured event, etc.).

### Code locations to inspect

| File | What to change |
|---|---|
| `src/platform/<target>/socket_udp.cpp` | Enable `IP_RECVERR` (and `IPV6_RECVERR` for v6). Add `drain_error_queue()` helper. |
| `src/transport/coap_transport.cpp` | Call `drain_error_queue()` before `recvfrom()` on every receive tick. On ICMP_PORT_UNREACH, look up the in-flight transaction by destination 5-tuple and cancel it. |
| `src/core/transaction.cpp` (or equivalent Wakaama wrapper) | Provide a `cancel_transaction_by_endpoint(const Endpoint&)` API that the transport layer calls. |

### Reference implementation — Linux (OpenWRT, RPi4, host CI)

Socket setup:

```cpp
// src/platform/linux/socket_udp.cpp
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <linux/errqueue.h>

void enable_icmp_error_reporting(int fd, bool ipv6) {
    int on = 1;
    if (ipv6) {
        if (::setsockopt(fd, IPPROTO_IPV6, IPV6_RECVERR, &on, sizeof(on)) < 0) {
            log_warn("IPV6_RECVERR setsockopt failed: {}", strerror(errno));
        }
    } else {
        if (::setsockopt(fd, IPPROTO_IP, IP_RECVERR, &on, sizeof(on)) < 0) {
            log_warn("IP_RECVERR setsockopt failed: {}", strerror(errno));
        }
    }
}
```

Drain function:

```cpp
// Returns the number of ICMP errors processed, or -1 on internal failure.
// Each surfaced error invokes `on_icmp_unreachable` with the offending peer.
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
            log_warn("recvmsg(MSG_ERRQUEUE) failed: {}", strerror(errno));
            return -1;
        }

        // Walk control messages for the extended error
        for (cmsghdr* c = CMSG_FIRSTHDR(&msg); c != nullptr; c = CMSG_NXTHDR(&msg, c)) {
            const bool v4 = (c->cmsg_level == IPPROTO_IP   && c->cmsg_type == IP_RECVERR);
            const bool v6 = (c->cmsg_level == IPPROTO_IPV6 && c->cmsg_type == IPV6_RECVERR);
            if (!v4 && !v6) continue;

            auto* ee = reinterpret_cast<sock_extended_err*>(CMSG_DATA(c));
            if (ee->ee_origin == SO_EE_ORIGIN_ICMP || ee->ee_origin == SO_EE_ORIGIN_ICMP6) {
                if (ee->ee_type == ICMP_DEST_UNREACH /* same value as ICMPV6_DEST_UNREACH for our purposes */) {
                    on_icmp_unreachable(peer, ee->ee_type, ee->ee_code);
                    ++processed;
                }
            }
        }
    }
}
```

Wire-up in the transport receive tick:

```cpp
// src/transport/coap_transport.cpp
void CoapTransport::poll_once() {
    // 1) Drain any kernel-reported ICMP errors first
    drain_error_queue(socket_fd_, [this](const sockaddr_storage& peer, int t, int c) {
        log_event("icmp_unreachable", peer, t, c);
        if (c == ICMP_PORT_UNREACH) {
            // Cancel any in-flight transaction targeting this peer.
            // The transaction layer, in turn, surfaces transport failure to
            // the registration state machine.
            transactions_.cancel_by_peer(peer, TransportError::PortUnreachable);
        }
    });

    // 2) Now do the normal recv path
    receive_datagrams();
}
```

### Reference implementation — Zephyr / LwIP (nRF9161 future target)

`IP_RECVERR` is not available on Zephyr's BSD socket layer. Use connected UDP for the LwM2M peer and check the return of `send()`:

```cpp
// src/platform/zephyr/socket_udp.cpp
//
// On Zephyr, connect() the UDP socket to the LwM2M peer once at registration
// time. ICMP errors then surface as ECONNREFUSED on subsequent send() calls.
// See Zephyr docs: net_context with NET_CONTEXT_NET_PKT_POOL plus connect().
ssize_t send_datagram(int fd, std::span<const uint8_t> buf) {
    ssize_t n = ::send(fd, buf.data(), buf.size(), 0);
    if (n < 0 && errno == ECONNREFUSED) {
        // Peer ICMP-rejected a recent datagram; treat as transport error.
        on_transport_error_(TransportError::PortUnreachable);
        return n;
    }
    return n;
}
```

Note: `connect()`-ing the UDP socket means the client cannot accept inbound packets from any other peer. This is fine for LwM2M v1.1 single-server topology, but breaks if we ever add a Bootstrap-Server or migrate mid-session (LwM2M v1.2 §6.1.3). Leave a comment in the code flagging this trade-off.

### Acceptance criteria

- [ ] Pointing the client at a closed port (`coap://127.0.0.1:9999/`) causes it to log a `port_unreachable` event and surface a transport error to the registration state machine within **1 s** of the first send — instead of retrying for 32 s.
- [ ] After ICMP-driven cancellation, the registration state machine applies the same backoff path as `MAX_RETRANSMIT` exhaustion (currently: enter `STATE_REG_FAILED`, schedule retry with capped exponential backoff).
- [ ] `IP_RECVERR` / `IPV6_RECVERR` are enabled on every UDP socket used for LwM2M traffic, including DTLS sockets once P0-3 lands.
- [ ] On Zephyr targets, the connected-UDP path produces the equivalent fast-fail behaviour.
- [ ] No regression: a healthy server connection over loopback works exactly as before.

### Test plan

1. **Unit** (`tests/transport/test_icmp_unreachable.cpp`): with a fake socket layer that injects synthetic `IP_RECVERR` cmsgs, confirm `drain_error_queue` invokes the callback with correct peer and ICMP code, and that the transaction layer cancels by 5-tuple.
2. **Integration** (`tests/integration/test_closed_port_fast_fail.cpp`): launch the client against a deliberately-closed port. Assert the log contains `port_unreachable` and that the registration state machine reports failure within 1 s.
3. **Regression**: run the full Leshan-paced and FT-burst patterns from `CLAUDE.md`; confirm they still pass.

---

## 3 — P2-10 · Apply random jitter to CoAP retransmit timeout

### Symptom

Observed retransmit deltas: **2.000, 4.001, 8.002, 15.991 s**. Pure deterministic doublings of `ACK_TIMEOUT = 2 s`, with no random factor applied.

### Root cause

Per RFC 7252 §4.8.2:

> *The initial timeout is set to a random duration (often not an integral number of seconds) between `ACK_TIMEOUT` and `(ACK_TIMEOUT × ACK_RANDOM_FACTOR)`.*

Defaults:
- `ACK_TIMEOUT = 2 s`
- `ACK_RANDOM_FACTOR = 1.5`
- → initial timeout uniformly in `[2.0, 3.0]` s

The exact 2.000 s observation means our timer arming code uses `ACK_TIMEOUT` directly without sampling within the spec range. Wakaama's `liblwm2m/transaction.c` does support a random factor via `COAP_RESPONSE_RANDOM_FACTOR`, but our integration probably doesn't pass a meaningful jitter source, defaulting to the floor.

The matter at scale: a fleet of devices that all retransmit at deterministic intervals will produce coordinated traffic spikes after a network outage. The random factor exists exactly to spread retransmissions out so the recovery isn't a thundering herd.

### Required behavior

For each in-flight CON transaction, when arming the next retransmit timer:

```
INITIAL_TIMEOUT  = uniform_random(ACK_TIMEOUT, ACK_TIMEOUT × ACK_RANDOM_FACTOR)
NEXT_TIMEOUT     = 2 × PREVIOUS_TIMEOUT
```

So if the initial sample is 2.7 s, the sequence is 2.7, 5.4, 10.8, 21.6 — not 2.0, 4.0, 8.0, 16.0.

The random source must be:
- seeded once at startup from a non-deterministic source (`std::random_device`, `getrandom(2)`, or platform RNG),
- thread-safe if the transport runs in its own thread (use a `thread_local` engine or a mutex),
- **not** `rand()` — it's not thread-safe and has poor distribution properties.

### Code locations to inspect

| File | What to change |
|---|---|
| `src/transport/coap_retransmit.cpp` (or wherever the transaction timer is armed) | Replace `ACK_TIMEOUT` literal with `sample_initial_timeout()`. |
| `src/platform/<target>/random.cpp` | Provide `RandomEngine` singleton with platform-appropriate seeding. |
| `tests/transport/test_retransmit_jitter.cpp` | Verify distribution. |

### Reference implementation

```cpp
// src/transport/coap_retransmit.h
namespace ftl::lwm2m::coap {

constexpr std::chrono::milliseconds kAckTimeout{2000};   // RFC 7252 §4.8.1
constexpr double                    kAckRandomFactor = 1.5;
constexpr int                       kMaxRetransmit   = 4;

// Returns the initial CON retransmit timeout, sampled per RFC 7252 §4.8.2.
std::chrono::milliseconds sample_initial_timeout();

// Returns the timeout for the Nth retransmission (N >= 1), given the initial.
std::chrono::milliseconds nth_retransmit_timeout(std::chrono::milliseconds initial,
                                                  int retx_n);

}  // namespace ftl::lwm2m::coap
```

```cpp
// src/transport/coap_retransmit.cpp
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

std::chrono::milliseconds nth_retransmit_timeout(std::chrono::milliseconds initial,
                                                  int retx_n) {
    // RFC 7252 §4.8.2: each retransmit doubles the previous timeout.
    return initial * (1 << retx_n);
}

}  // namespace ftl::lwm2m::coap
```

```cpp
// src/platform/linux/random.cpp
#include "random.h"
#include <random>

namespace ftl::lwm2m::platform {

std::mt19937& random_engine() {
    // Thread-local, seeded once per thread from std::random_device.
    thread_local std::mt19937 engine{std::random_device{}()};
    return engine;
}

}  // namespace ftl::lwm2m::platform
```

For Zephyr / nRF9161, replace `std::random_device` with the platform RNG (e.g. `sys_rand32_get()` from `<zephyr/random/random.h>`).

### Wakaama integration

Wakaama already accepts a random factor — find the call site that arms transactions and ensure we provide a real sample, not the floor. Likely in our `transaction_send_wrapper()` or wherever we set `transacP->retrans_time`. Replace any `LWM2M_COAP_DEFAULT_RESPONSE_TIMEOUT`-style direct assignment with `sample_initial_timeout()`.

### Acceptance criteria

- [ ] Initial CON timeout falls within `[2000, 3000]` ms, and across 100 transactions in a unit test, observed values cover the full range with approximately uniform distribution (Kolmogorov–Smirnov test passes at α = 0.05).
- [ ] Each subsequent retransmit timeout is exactly `2 ×` the previous (no additional jitter on doublings — RFC 7252 only randomizes the initial).
- [ ] A pcap of a closed-port test (after P2-9 lands, this won't generate retransmits — so use a slow / non-responding peer instead) shows initial retransmit deltas spread across the [2.0, 3.0] s range across 100 sessions, not always 2.000 s.
- [ ] No regression: existing retransmit tests using `ACK_TIMEOUT` directly are updated to accept a range.

### Test plan

1. **Unit** (`tests/transport/test_retransmit_jitter.cpp`):
   - Sample `sample_initial_timeout()` 10,000 times, assert `min ≥ 2000`, `max ≤ 3000`, `|mean − 2500| < 25` ms.
   - Assert `nth_retransmit_timeout(2500, 1) == 5000`, `(2500, 2) == 10000`, etc.
2. **Integration**: drive 50 short CON exchanges against a peer that drops every datagram. Capture deltas in pcap. Assert initial-retx deltas are not all equal.

---

## 4 — Cross-cutting requirements

These apply to both items above. Same conventions as the main `CLAUDE.md`:

- **C++17 minimum, RAII everywhere.** Socket FDs wrapped in `UniqueFd` (or equivalent), no bare `close()` in error paths.
- **No `printf` in production paths.** All log lines go through the structured logger / ring buffer (per the main `CLAUDE.md` P1-8). The `log_event("port_unreachable", …)` calls in §2 above MUST be structured events, not printf.
- **`tdd-workflow`, `karpathy-guidelines`, `verification-loop` skills** apply. Tests land before the fix.
- **Build matrix passes:** `aarch64-openwrt-linux-musl`, `aarch64-linux-gnu`, `x86_64-linux-gnu`. Zephyr target gets a CMake-only stub for now (see §2 Zephyr note); full implementation can land alongside the broader nRF9161 port.
- **No new public Wakaama-equivalent APIs.** P2-10 reuses Wakaama's existing random-factor hook; don't add a parallel timer system.

---

## 5 — Workflow

Both items in **one PR**: `fix/p2-coap-transport-hardening`. They touch the same layer, share test infrastructure, and reviewing them together is faster than serially. Branch name pattern matches the main `CLAUDE.md` convention.

PR description must include:

- Link to this directive.
- The pcap evidence for both deviations (frame numbers from `03ADV.pcapng`).
- Before/after timing measurements (closed-port test for P2-9; jitter histogram for P2-10).
- Confirmation that the existing FT and Leshan regression tests still pass.

---

## 6 — Definition of Done

- [ ] Both acceptance criteria sets pass in CI.
- [ ] A new pcap of a closed-port test shows the client emit one CoAP datagram, receive ICMP, and exit the transaction within 1 s.
- [ ] A new pcap of a slow-responding peer shows initial retx deltas spread across [2.0, 3.0] s across 50 trials.
- [ ] No regression in `02ftl.pcapng` / `02lesh.pcapng` replay tests.
- [ ] Note added to `docs/CHANGELOG-remediation.md` citing RFC 7252 §4.4, §4.8.2, §10.1.

---

## 7 — References

- **RFC 7252** — Constrained Application Protocol
  - §4.4 (Messages and Endpoints — ICMP handling)
  - §4.8.1 (Transmission Parameters)
  - §4.8.2 (Time Values and Derived Transmission Parameters)
  - §10.1 (Implementation Considerations — operational behaviour)
- **Linux man pages**
  - `ip(7)` — `IP_RECVERR` socket option
  - `ipv6(7)` — `IPV6_RECVERR`
  - `cmsg(3)` — control message walking
- **Zephyr** — `<zephyr/net/socket.h>`, `<zephyr/random/random.h>`
- **Wakaama** — `liblwm2m/transaction.c`, `liblwm2m/internals.h` (`COAP_RESPONSE_TIMEOUT`, `COAP_RESPONSE_RANDOM_FACTOR`)

Trigger pcap: `artifacts/03ADV.pcapng` — frames 67, 68, 81, 82, 160, 161, 293, 294, 716, 717.

---

*This directive supplements the main `CLAUDE.md`. It does not replace any existing P0/P1 item — both fixes here are P2 and can land independently of the burst-stall investigation (P0-5) and the DTLS work (P0-3).*
