# CLAUDE.md — FTL LwM2M Client Remediation Directive (v2)

> **Scope:** `github.com/svdwalt007/Friendly-LwM2M-Client`
> **Trigger pcaps:**
>   - `02ftl.pcapng` — vs Friendly One-IoT DM (DemoDM IoT), endpoint `svdwalt007-555`, 17 instance-level CON reads over 36 s, **client silent for 110 s after first UPDATE**
>   - `02lesh.pcapng` — vs `leshan.eclipseprojects.io`, endpoint `svdwalt007-444`, 7 object-level CON reads over 28 s, **clean register/update/serve/deregister cycle**
> **Status:** Active remediation, **v2** (cross-trace updates). Stable issue IDs preserved from v1.

## 0 — Changelog from v1

The Leshan capture materially changes the diagnosis. Five items moved:

| Item | v1 status | v2 status | Why |
|---|---|---|---|
| **P0-1** — UPDATE scheduler fires once | confirmed bug | **gated on reproducer first** | Leshan trace ended before the 2nd UPDATE timer would have fired (t=117 s vs ~t=133 s). FT-trace evidence is suggestive but not conclusive. |
| **P0-5** *(NEW)* — burst-load stall | n/a | **active P0** | FT trace stalled, Leshan trace did not, with the same client binary. The differentiator is server-side load pattern (17 CONs in 36 s vs 7 CONs in 28 s). This is now the prime suspect for the FT silence. |
| **P1-6** — no DEREGISTER on shutdown | confirmed bug | **REFUTED, scope replaced** | Leshan frame 2524 is a clean `CON DELETE /rd/{loc}` initiated by the client. The FT-trace absence of DEREGISTER is a downstream symptom of P0-5, not a missing handler. |
| **P1-7** — Location ACL gap | confirmed | **confirmed across both traces, concrete fix specified** | Both servers receive 4.01 on `/6`. Fix is one AC instance in the bootstrap config. |
| **P1-8** — logging/observability | important | **critical (gates P0-5)** | Without ring-buffer state capture we cannot diagnose the burst stall from production traces. |

Stable defect IDs preserved. Issue/PR titles can continue to reference `P0-1`, `P1-7`, etc.

## 1 — Project context

This client is built on Eclipse Wakaama with a HAL/BSP/Objects/Core layout, currently targeting OpenWRT One Router (24.10.4), with active porting to Raspberry Pi 4 and a future nRF9161/Zephyr target. The DTLS backend is mbedTLS for Linux-class targets. Deployed servers tested so far: Friendly One-IoT DM (production target) and Eclipse Leshan (interop reference).

**This file is authoritative for all remediation work.** When the directive here conflicts with prior comments, TODOs, or ambient code patterns, this file wins.

## 2 — Mission

Two pcaps of the same client binary against two different servers produced sharply divergent client behavior. Root-cause both traces, fix all defects in priority order. Each defect below has a **symptom** (with explicit pcap evidence), a **root cause hypothesis**, **acceptance criteria** (mechanically verifiable), and a **test plan**.

You **must** apply the `tdd-workflow`, `karpathy-guidelines`, and `verification-loop` skills throughout. Read those before touching code. Use `search-first` to confirm Wakaama already exposes a public API before adding wrappers — Wakaama has surprisingly complete primitives and we have over-wrapped it in places already.

---

## 3 — Defects

### P0-1 · UPDATE scheduler — gated on reproducer first

**Symptom (FT trace).** Frame 270 sends `CON POST /rd/{loc}` at t=57.11 s; ACK at frame 271 (t=58.74 s). Then **52.65 s of total client silence** through the end of the LwM2M conversation. No second UPDATE, no DEREGISTER, no application traffic.

**Symptom (Leshan trace, counter-evidence).** Same client binary, same object set, same `lt=60`. UPDATE fires at t=88.23 s; ACK at t=88.45 s. The session is then voluntarily terminated via DEREGISTER at t=117.22 s — **16 seconds before the second UPDATE timer would have been due** (~t=133.45 s, i.e. `UPDATE_ACK + lt × 0.75`). The Leshan trace does **not** test the re-arm path.

**Conclusion.** The FT-trace silence is real but possibly downstream of P0-5 (burst stall) rather than a scheduler defect in its own right. Resolve by reproducer first.

**Required action — reproduce in isolation before any code changes.**

1. Add `tests/integration/test_long_idle_session.cpp`:
   - register against a local Leshan or Wakaama test server with `lt=60`,
   - issue **no operator-side reads or writes for 5 minutes**,
   - assert UPDATEs fire at intervals of `(0.75 × lt) ± 5%` (i.e. every 42–48 s) for the full duration.
2. **If the test fails** — there is a real scheduler bug. Apply the fix described in §3.P0-1.fix below.
3. **If the test passes** — the FT-trace silence has another cause (almost certainly P0-5). Close P0-1 with a note, redirect effort to P0-5.

#### P0-1.fix — Required behavior (only if reproducer fails)

Per OMA-TS-LightweightM2M_Core v1.1 §6.2.3, the client MUST send a registration UPDATE before the registered lifetime expires. Standard practice: `t_next_update = t_last_successful_update_ack + (lt × 0.75)`, with jitter ≤ ±5 % to avoid synchronized fleet pulses.

Inspect, in this order:

1. `src/core/registration.cpp` — the success path of `2.04 Changed` on `/rd/{loc}`. Confirm a timer is re-armed for the next UPDATE.
2. Wakaama's `liblwm2m/registration.c::registration_step()` — the state transition `STATE_REG_UPDATE_SENT → STATE_REG_REGISTERED`. Confirm `lwm2m_step()` is called from the event loop on every tick with monotonic time.
3. Our event-loop driver in `src/platform/.../event_loop.cpp` — confirm nothing upstream blocks `lwm2m_step()` invocation.

**Acceptance criteria** (only if fix is applied).

- [ ] Reproducer test from §P0-1 step 1 passes for ≥ 5 minutes with `lt=60`.
- [ ] 24 h soak with `lt=300` shows UPDATE cadence of `(0.75 × lt) ± 5%`.
- [ ] Three consecutive UPDATE failures within one lifetime window trigger fall-through to full re-REGISTER (see P0-2).
- [ ] Re-arm logic is unit-testable without a network — fake clock + fake transport.

---

### P0-2 · No registration watchdog / re-REGISTER on lifetime breach

[Unchanged from v1 — independent of P0-1's outcome, still required.]

**Symptom.** Even if UPDATE works most of the time, a single missed UPDATE silently drops the device from the server registry. The client has no fallback path that detects "I have not had a successful UPDATE in too long" and forces a full re-REGISTER.

**Required behavior.** Independent of the UPDATE scheduler, run a watchdog: if `(now − t_last_successful_update_ack) > (lt − safety_margin)`, transition the state machine to `STATE_REG_REGISTRATION_REQUIRED` and re-REGISTER. Use `safety_margin = max(lt × 0.10, 5 s)`.

**Acceptance criteria.**

- [ ] Watchdog runs from the same `lwm2m_step()` tick as the UPDATE scheduler, with no shared timer state that can be corrupted by either path.
- [ ] Forced re-REGISTER preserves the endpoint name and rebuilds the link-format payload from the current object set (do not cache a stale payload).
- [ ] After re-REGISTER, the new `Location-Path` from the `2.01 Created` ACK replaces the old one.
- [ ] No infinite re-REGISTER loop on persistent failure — apply RFC 7252-style backoff with a hard cap (suggest 1 h).

**Test plan.**

- Unit: simulate a silently-dropped UPDATE (no ACK, transport returns success). Assert re-REGISTER fires by `t = lt × 0.95` at the latest.
- Integration: with Leshan, drop a single UPDATE via firewall rule mid-session; assert client recovers within one lifetime window.

---

### P0-3 · NoSec mode in production-shaped sessions

[Unchanged from v1 — confirmed in BOTH traces. Both sessions are plaintext on UDP/5683.]

**Required behavior.** Per LwM2M v1.1 §7.1, NoSec is permissible only on physically-isolated networks. For any deployment touching the public internet (which both `leshan.eclipseprojects.io` and `176.9.138.37` are), the client MUST use one of: PSK (mode 0), RPK (mode 1), x.509 (mode 2), or EST (mode 4). Default for this codebase is **PSK with mbedTLS**.

**Required deliverables.**

1. CMake option `FTL_LWM2M_SECURITY_MODE=PSK|RPK|X509|NOSEC` with default `PSK`. `NOSEC` builds **must** emit a compile-time `#warning` and a runtime startup banner.
2. mbedTLS build configuration: enable `MBEDTLS_SSL_DTLS_CONNECTION_ID` (RFC 9146), `MBEDTLS_SSL_PROTO_DTLS`, `MBEDTLS_SSL_DTLS_HELLO_VERIFY`. Disable TLS-only protos if TCP isn't used (binary-size discipline on MCU targets).
3. Security Object resource provisioning path (`src/objects/security_object.cpp`): on factory init, populate `/0/0/2 = 0`, `/0/0/3 = <PSK identity>`, `/0/0/5 = <PSK key>`, with the credential blob loaded from secure NVM.
4. Server URI scheme switches from `coap://host:5683` to `coaps://host:5684` when security mode ≠ NoSec.
5. **CID negotiation: client MUST request a non-zero-length CID** in the `connection_id` extension of every ClientHello. Zero-length CIDs are explicitly forbidden in this codebase — they defeat NAT survival on PSM cycles. Add a regression test that parses the ClientHello and asserts CID length ≥ 1 byte (recommend 4–8 bytes for cluster-routing prefix space).

**Acceptance criteria.**

- [ ] Building with `-DFTL_LWM2M_SECURITY_MODE=PSK` produces a binary that handshakes successfully against Leshan with a PSK identity/key pair from a config file.
- [ ] tshark on the resulting pcap shows: ClientHello carries `connection_id` extension with non-zero length; ServerHello echoes its own CID; post-handshake records use ContentType 25 (`tls12_cid`).
- [ ] `NOSEC` build emits the warning banner on stdout and in the system log within 1 s of startup.

---

### P0-4 · Queue Mode declaration mismatched to device capability

[Unchanged from v1.]

**Symptom.** Both traces register with `b=U` (UDP only, always-on). For any device class that ever sleeps (NB-IoT PSM, LTE-M eDRX, OpenWRT power-save), this is wrong.

**Required behavior.** Per LwM2M v1.1 §6.2.1.1, declare `b=UQ` for any sleeping or duty-cycled device. The server then buffers operations until the next wake/UPDATE. Combined with DTLS CID, this is what makes the stack PSM-safe.

**Required deliverables.**

- Build-time / config-time selection of binding mode in `config/client_config.json` (`"binding": "U" | "UQ"`).
- When `binding=UQ`, the client MUST also implement the wake/listen window logic: on UPDATE success, stay receive-active for at least `MAX_TRANSMIT_WAIT` seconds (RFC 7252 §4.8.2 default 93 s) before going back to sleep.
- Document the relationship between `lt`, the wake interval, and the listen window in `docs/queue-mode.md`.

**Acceptance criteria.**

- [ ] `binding=UQ` builds register with `b=UQ` and surface "Queue Mode" in the server's device row.
- [ ] After UPDATE, the client does not enter low-power sleep until either a queued op is received or `MAX_TRANSMIT_WAIT` elapses.
- [ ] No regression on always-on (`binding=U`) targets.

---

### P0-5 (NEW) · Burst-load stall — primary investigation

**Symptom (FT trace).** Server fires 17 instance-level CON GETs over 36 s (~1 per 2 s sustained):

```
GET /3/0  /3339/0  /2/10  /4/0  /2/2  /5/0  /2/3  /2/0  /2/1
GET /1/0  /2/8  /2/9  /2/6  /2/7  /6/0  /2/4  /2/5
```

Six return 4.01 (correct ACL behaviour: client doesn't own those instances). Reply latency on the 4.01 path *creeps upward* on early instances — frames 103, 121, 145 had 0.73–0.86 s reply latencies vs the 0.18 s baseline of the 2.05 path — then settles. **After the burst completes (t=42 s), the client emits one UPDATE at t=57 s, then goes silent for the remaining 110 s of capture.** No further UPDATE, no DEREGISTER, no ICMP Port Unreachable (so the process is still alive at the OS level — the application loop is the thing that wedged).

**Symptom (Leshan trace, counter-evidence).** 7 object-level CONs over 28 s (~1 per 4 s, with two ~16 s pauses). Reply latency stable. Client behaves correctly throughout: UPDATE, served a post-UPDATE read, DEREGISTERed cleanly. **The Leshan load pattern does not trigger the stall.**

**Hypothesis.** Some code path exercised by burst-density instance-level reads — most likely the 4.01 generation path or its ACL lookup — leaks or deadlocks state, leaving the main event loop unable to progress past the burst.

**Investigation plan, in priority order.**

1. **Wakaama transaction list audit.** In `liblwm2m/transaction.c`, every CON request creates a transaction. Verify that the 4.01 reply path calls `transaction_remove()` exactly the same way the 2.05 path does. Look for early-return paths in `prv_handleOperation()`, our object handlers, and the ACL-check call chain (`acc_ctrl_check_rights()` or whatever we named our wrapper) that bypass cleanup.
2. **Token / MID allocator audit.** Check whether 4.01 generation uses a different alloc path than 2.05, and whether either path can leak under repeated calls. The latency creep on early 4.01s is a hint that the first few generations are doing extra work the later ones aren't.
3. **Object-handler reentrancy.** The `/2` (Access Control) handler is reentered for each fan-out instance. If it holds any non-recursive lock (mutex, atomic flag, file handle, NVM transaction) across the access-check, sequential reentry within one event loop tick could deadlock or accumulate.
4. **Platform timer scheduler.** If the timer queue uses bounded storage and isn't drained while servicing CoAP I/O, a busy I/O period could starve the registration UPDATE timer permanently. Look at the `lwm2m_step()` dispatcher's interaction with timer-armed callbacks.
5. **NVM write contention.** If we log denials to NVM synchronously, six 4.01s in 30 s could saturate the write path and indirectly stall the event loop.

**Required deliverables.**

1. **Reproducer harness:** `tools/burst-load-reproducer/` — a small Python or C++ program that connects to a local Leshan, drives N instance-level CON GETs over M seconds against our client, with mixed-ACL targets so we generate both 2.05 and 4.01 responses. Configurable: `--n-reads`, `--duration`, `--mix-ratio`, `--target-paths`. Skeleton in §9.
2. **Instrumented build:** with P1-8's ring-buffer log enabled, capture the state machine through a reproduced burst.
3. **RCA write-up:** `docs/burst-stall-rca.md` identifying which of the candidates above is the actual cause, with code references and the fix diff.

**Acceptance criteria.**

- [ ] Reproducer reliably stalls the unfixed client with N=17 GETs over 36 s, mix=6/11 4.01s.
- [ ] Reproducer reliably does **not** stall the fixed client with the same parameters.
- [ ] Sustained-burst test (N=200 GETs over 400 s, simulating a long management session) completes with the client healthy and registered.
- [ ] Post-burst, the client emits the next scheduled UPDATE within `± 5%` of `lt × 0.75`.
- [ ] All Wakaama transactions are reaped — `transaction_count()` (or our equivalent introspection) returns to baseline within 5 s of burst completion.
- [ ] Memory/FD/lock counts return to baseline within 10 s of burst completion (verified via `/proc/<pid>/status` and `lsof` on Linux targets).

**Test plan.**

- Unit: stub the failing-ACL handler, call it 100 times in a tight loop in the test harness, assert no resource accumulation across the call sequence.
- Integration: run the reproducer against a real Leshan with both unfixed and fixed builds. Capture pcaps. Assert post-burst UPDATE on the fixed build only.
- Soak: 4 h management-pattern simulator (200 reads/h, mixed access bits) ends with the client healthy, registered, and within 10 % of baseline resource usage.

---

### P1-5 · Default lifetime far too short

[Unchanged from v1.]

**Symptom.** Both traces register with `lt=60`. This is bring-up debugging, not production.

**Required behavior.** Default `lt` in `config/client_config.json`:

| Target class | Default `lt` | Rationale |
|---|---|---|
| Always-on broadband (OpenWRT, RPi4) | `300` s | Frequent enough to detect dead links, infrequent enough to not flood |
| NB-IoT / LTE-M with PSM (nRF9161 future) | `3600` s | Matches typical PSM TAU of 1 h |
| Battery-pessimal sleepy device | `86400` s | OMA spec default |

`lt=60` is reserved for `make test` and CI runs only and must require an explicit `--debug-short-lifetime` flag at runtime.

**Acceptance criteria.**

- [ ] Default-config builds register with `lt ≥ 300`.
- [ ] Attempting `lt < 60` triggers a config-validation error at startup, not a runtime surprise.

---

### P1-6 · DEREGISTER reliability under shutdown — RETRACTED original scope

**Original v1 finding (REFUTED).** v1 of this directive claimed "the client does not send DEREGISTER on shutdown." This was based on the FT trace alone. **The Leshan trace contradicts it:** frame 2524 is a clean `CON DELETE /rd/fgCzoj1blP` from the client at t=117.22 s, ACKed at frame 2525 (`2.02 Deleted`).

**Revised scope.** The DEREGISTER path exists and works on a healthy client. What needs verification is whether it remains reliable under *all* shutdown conditions, particularly after a burst-load event (P0-5). Working hypothesis: the FT-trace absence of DEREGISTER is a downstream symptom of the burst stall — the application loop was already wedged when the operator tried to shut it down.

**Required deliverables.**

- `tests/integration/test_deregister_after_burst.cpp`:
  1. Register the client.
  2. Drive the burst-CON pattern from the P0-5 reproducer.
  3. 5 s after burst completion, send SIGTERM.
  4. Assert `CON DELETE /rd/{loc}` is observed on the wire within 5 s of SIGTERM.
- A 5-second SIGTERM timeout on the DEREGISTER wait, after which the client falls through to ungraceful exit and logs a structured "DEREGISTER timed out, possible upstream stall" event.

**Acceptance criteria.**

- [ ] DEREGISTER fires within 5 s of SIGTERM in 100 % of clean-state shutdowns.
- [ ] Once P0-5 is fixed, DEREGISTER fires within 5 s of SIGTERM in ≥ 99 % of post-burst shutdowns.
- [ ] If DEREGISTER cannot be sent, the client logs a structured event before exit and never hangs the shell session indefinitely.

---

### P1-7 · Location AC instance — confirmed missing across both servers

**Symptom.** Confirmed in both traces:
- FT (instance-level): `GET /6/0` → `4.01 Unauthorized` (frame 169).
- Leshan (object-level): `GET /6` → `4.01 Unauthorized` (frame 1681).

The Leshan trace's TLV inspection of the successful object-level `GET /2` confirms which AC instances exist for SSID 123:

```
/2/1  → covers /1/0    (Server),   Owner=123
/2/3  → covers /3/0    (Device),   Owner=123
/2/5  → covers /5/0    (Firmware), Owner=123
/2/7  → covers /4/0    (Conn Mon), Owner=123
/2/10 → covers /3339/0 (Buzzer),   Owner=123
```

**No AC instance covers `/6/0` (Location).** That is why both servers receive 4.01 — the client correctly enforces the spec, but the bootstrap config never granted the LwM2M Server access to Location.

**Required fix.** Add one AC instance to the bootstrap config / factory init in `src/objects/access_control_object.cpp` (or wherever the seed AC instances live):

```cpp
// New 12th AC instance — grants LwM2M Server (SSID 123) access to /6/0 Location.
// Owner=123 with empty ACL means the Owner has full rights and no other server
// is granted anything, which matches the existing convention in this client.
{
    .object_id   = 6,        // Location
    .instance_id = 0,
    .acl         = { },      // empty multi-resource; Owner has all rights
    .owner_ssid  = 123,      // matches the LwM2M Server's Short Server ID
}
```

Same audit should verify every declared object instance has at least one AC instance whose Owner = active Server SSID, or whose ACL multi-resource grants Read.

**Required deliverables.**

1. Patch the bootstrap config / AC factory to include the missing instance.
2. `tools/audit-acl-coverage.py` — reads the AC seed config, asserts every declared object instance has matching AC coverage. Fail CI if not.
3. Update `docs/access-control.md` documenting the AC-coverage rule and how to add new AC instances when adding new objects.

**Acceptance criteria.**

- [ ] Replaying either trigger pcap's read pattern produces no 4.01 on `/6` or `/6/0`.
- [ ] `tools/audit-acl-coverage.py` runs in CI and gates merges.
- [ ] Adding a new object instance to the registration set without a corresponding AC instance fails the audit.

---

### P1-8 · Logging / observability — promoted to CRITICAL

[Promoted in importance — P0-5's investigation cannot proceed without this.]

**Symptom.** When the client went silent at t=58.74 s in the FT trace, there is no in-band signal of why. We can't tell from the pcap whether the event loop stalled, the UPDATE timer didn't re-arm, the transaction list saturated, or the process hung on a lock. **Doing P0-5's RCA without this in place means guessing.** Build this first.

**Required deliverables.**

1. **Persistent ring-buffer log in NVM** (`src/platform/.../log_ring.cpp`) of every state-machine transition with monotonic timestamp. Size: 4 KiB on MCU, 64 KiB on Linux-class targets.
2. **Structured log lines** (one per transition) with fields: `t_mono_us`, `state_from`, `state_to`, `trigger`, `error_code`. Emit as JSON lines on stdout for Linux targets, as a binary record on MCU (with a separate decoder tool).
3. **Resource introspection counters** updated on every state transition: `transaction_count`, `pending_token_count`, `mid_alloc_count`, `nvm_inflight_writes`. These are the variables P0-5 needs to monitor.
4. **`SIGUSR1` snapshot handler** on Linux targets that writes a state dump: current registration state, last 10 UPDATE timestamps, current `lt`, last 16 CoAP MIDs, last 4 DTLS handshake outcomes, current values of all introspection counters. Output goes to a configurable path (default: `/tmp/ftl-lwm2m-snapshot.json`).

**Acceptance criteria.**

- [ ] After the burst-load reproducer (P0-5) runs, a `SIGUSR1` snapshot reveals which counter (transactions, tokens, MIDs, NVM) is leaking.
- [ ] The ring-buffer log alone is sufficient to answer "when did the last UPDATE succeed?" and "what was the state machine doing at second 90?" without consulting a pcap.
- [ ] All log entries are timestamped with the same monotonic clock used by `lwm2m_step()` — no drift between the log and the protocol state.

---

## 4 — Cross-cutting requirements

[Unchanged from v1.]

- **No new public Wakaama-equivalent APIs.** If you find yourself adding `ftl_*` wrappers around `lwm2m_*` calls, stop and confirm with `search-first` that the underlying primitive doesn't already do what you need.
- **C++17 minimum, RAII everywhere.** No `new`/`delete` in new code. Use `std::unique_ptr` for owning, `std::span`/`gsl::span` for non-owning views over buffers. mbedTLS handles wrapped in RAII (`MbedtlsCtx`, `MbedtlsSslCtx`) — no bare `mbedtls_*_init`/`mbedtls_*_free` pairs in application code.
- **No silent failure paths.** Every error path either logs structured to the ring buffer or propagates a `std::expected`-style result. `void` functions that can fail are forbidden — return `Result<T>` or `std::expected<T, ErrorCode>`.
- **Every fix lands with tests first.** `tdd-workflow` skill is mandatory, not aspirational. Target ≥ 80 % line coverage on touched files measured via `gcov`/`lcov`.
- **Build matrix passes:** `aarch64-openwrt-linux-musl`, `aarch64-linux-gnu` (RPi4 Debian), `x86_64-linux-gnu` (host CI). nRF9161/Zephyr is out of scope for these defects but must not be broken at the CMake level.
- **No `printf` in production paths.** Logging goes through the ring buffer module only.

---

## 5 — Workflow

For each defect:

1. **Branch:** `fix/p<N>-<short-slug>` off `main` (e.g., `fix/p0-5-burst-stall-rca`).
2. **Reproduce first.** Write the failing test before the fix. Show the failure in the PR description. **For P0-5 specifically, the reproducer harness IS the deliverable — get it landed before touching client code.**
3. **Surgical change.** Smallest possible diff to make the test pass. No drive-by refactors in the same PR — open a separate `chore/` branch for those.
4. **Verify with the analyzer in §8.** Capture a pcap of the test run, feed it to the analyzer, attach the analyzer report to the PR.
5. **Self-review with `karpathy-guidelines`** before requesting human review. State assumptions explicitly in the PR body.
6. **Use `verification-loop`** for any change touching the registration state machine — manual reasoning has burned us before.

---

## 6 — Recommended fix order

Given the cross-trace findings, address defects in this sequence:

1. **P1-8 first** — without observability, P0-5 cannot be debugged.
2. **P0-5 next** — this is the prime suspect for the worst observed defect (FT silence). May refute or confirm P0-1 as a side-effect.
3. **P0-1 reproducer** — runs after P0-5 lands. If P0-5's fix also restored UPDATE cadence, P0-1 closes with a note and no further code change.
4. **P1-7** — one-config-line fix, do it in the same week as P0-5 since it's the second-most-visible defect.
5. **P1-6 verification** — write the post-burst-DEREGISTER test, confirm it passes after P0-5 is fixed.
6. **P0-3 (DTLS)** — substantial work, plan as its own milestone after the stability defects above are closed.
7. **P0-2 (watchdog), P0-4 (Queue Mode), P1-5 (lifetime default)** — straightforward, can land in parallel with the DTLS work.

---

## 7 — Definition of Done

A defect is closed when **all** of the following are true:

- [ ] Acceptance criteria for the defect are mechanically verified (tests in CI, not just local).
- [ ] **Both trigger pcaps**, replayed against the fixed client, produce no equivalent failure.
- [ ] No regression in the existing test suite.
- [ ] The structured log ring buffer (P1-8) shows the corrected behavior.
- [ ] A short note in `docs/CHANGELOG-remediation.md` explaining what changed and why, citing the RFC / TS section that motivated it.

---

## 8 — Repro & verification harness

Before claiming any P0 defect is fixed, run the full harness. Both pcap patterns are reproduced — Leshan-style (good case) and FT-style (stress case).

```bash
# tools/verify-pcap-regressions.sh
#
# Replays the FTL client against a local Leshan instance under controlled
# conditions and runs tshark-based assertions matching every defect in this
# directive across BOTH the FT and Leshan load patterns.

./tools/verify-pcap-regressions.sh \
  --client-binary ./build/ftl-lwm2m-client \
  --leshan-uri    coap://127.0.0.1:5683 \
  --duration      1800 \
  --lt            60 \
  --pattern       leshan-paced     `# 7 object-level CONs over 28 s` \
  --pattern       ft-burst         `# 17 instance-level CONs over 36 s` \
  --pattern       sustained-burst  `# 200 mixed CONs over 400 s` \
  --expect        update_cadence_within_75pct \
  --expect        no_silent_window_gt_lt \
  --expect        no_stall_post_burst \
  --expect        deregister_on_sigterm \
  --expect        deregister_on_post_burst_sigterm \
  --expect        ac_reads_no_unexpected_4_01 \
  --expect        location_read_succeeds \
  --expect        transactions_drain_within_5s \
  --report-out    ./artifacts/regression-report.txt
```

Each `--expect` corresponds to a tshark-driven assertion. The script must exit non-zero if any expectation fails. CI gates on this.

For DTLS-on builds (post-P0-3), an equivalent `verify-pcap-regressions-dtls.sh` runs against `coaps://127.0.0.1:5684` and additionally asserts:

- ClientHello CID extension length ≥ 1 byte
- ServerHello echoes its own CID
- All post-handshake records use ContentType 25 (`tls12_cid`)

---

## 9 — Reproducer harness for P0-5

`tools/burst-load-reproducer/` — driven directly by Leshan's HTTP management API.

Skeleton:

```python
# tools/burst-load-reproducer/burst_driver.py
#
# Drives N instance-level CON GETs over M seconds against the device under
# test, with a configurable mix of accessible/inaccessible targets to
# generate both 2.05 and 4.01 responses.

import requests, time, sys

LESHAN_API = "http://localhost:8080/api/clients"
ENDPOINT   = "svdwalt007-test"

# FT-trace pattern: 17 instance-level reads over 36 s, 6 of which return 4.01.
# Order matches the actual frame sequence from 02ftl.pcapng frames 31..187.
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
    """After burst, watch for next UPDATE within (lt * 0.75 + 5%)."""
    # Use Leshan's observation-events stream or poll registration state
    ...

if __name__ == "__main__":
    drive_burst(FT_PATTERN, duration_s=36)
    if not check_post_burst_health():
        print("STALL DETECTED — P0-5 reproduced", file=sys.stderr)
        sys.exit(1)
```

Wire this into CI as a post-merge stress test against a Leshan container.

---

## 10 — References

OMA LwM2M:
- OMA-TS-LightweightM2M_Core-V1_1, §6.2 (Registration interface), §7.1 (Security), §7.3 (Access Control)
- OMA-TS-LightweightM2M_Core-V1_2_2, §7.3.2.1 (Owner / MAX_ID semantics), §7.1.7 (DTLS CID)
- OMA-TS-LightweightM2M_Transport-V1_1, §5 (UDP/CoAP binding), §5.4 (Queue Mode)

IETF:
- RFC 7252 — CoAP core, §4.4 (MID/Token), §4.8 (Reliability)
- RFC 7641 — CoAP Observe
- RFC 7959 — Blockwise transfers
- RFC 6347 — DTLS 1.2
- RFC 9146 — DTLS Connection ID **(highest priority for the next milestone)**
- RFC 7627 — Extended Master Secret
- RFC 6066 — SNI

Wakaama (likely audit targets for P0-5):
- `liblwm2m/registration.c` — registration state machine reference
- `liblwm2m/transaction.c` — primary suspect for P0-5 audit
- `liblwm2m/objects.c` — Object/Instance dispatch, second suspect for P0-5
- `liblwm2m/acl_check.c` (or our wrapper around it) — ACL evaluation, third suspect for P0-5

Internal:
- `docs/architecture.md` — HAL/BSP/Objects/Core boundaries
- `docs/queue-mode.md` *(to be written under P0-4)*
- `docs/access-control.md` *(to be written/extended under P1-7)*
- `docs/burst-stall-rca.md` *(to be written under P0-5)*
- `docs/CHANGELOG-remediation.md` — running log of fixes against this directive

Trigger pcaps (in `artifacts/`):
- `02ftl.pcapng` — FT One-IoT DM, exhibits the burst stall and silence
- `02lesh.pcapng` — Leshan, clean baseline, used as control for cross-trace diagnosis

---

## 11 — Out of scope (do not touch in this remediation)

- Server-side fixes. Both Friendly One-IoT and Leshan have their own remediation tracks; do not work around server bugs in the client. (Note: the FT server's instance-fan-out read strategy is partially responsible for triggering P0-5, but the *correct* fix is to harden our client, not to change the server.)
- nRF9161/Zephyr port. Maintain CMake compatibility but do not implement defect fixes against that target until the Linux-class targets are green.
- DTLS 1.3 (RFC 9147). Optional in LwM2M v1.2; revisit after RFC 9146 + persistence work lands.
- Rust hybrid/FFI exploration. Architectural decision pending; no code changes in this branch.

---

*This file is the contract between the remediation effort and any agent (human or AI) working in this repo. If a requirement here is wrong, fix the file in the same PR as the code change and call it out in review.*
