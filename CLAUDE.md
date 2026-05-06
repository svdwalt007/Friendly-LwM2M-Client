# Friendly LwM2M Client v1.2.2 — Project Memory

> Repo: [svdwalt007/Friendly-LwM2M-Client](https://github.com/svdwalt007/Friendly-LwM2M-Client) · Working branch: `claude/consolidate-branches-xCb7h`

## What this is

A reference **OMA LwM2M v1.2.2 client** for IoT devices, built on (and extending) Wakaama. Production-targeted, with multi-DTLS provider support, MQTT transport binding, edge AI inference, delta firmware updates, and A/B partition rollback.

| | |
|---|---|
| **LwM2M version** | 1.2.2 |
| **Wakaama base** | `2305-Wakaama/` (vendored) |
| **Build** | CMake (large `CMakeLists.txt`) |
| **DTLS providers** | TinyDTLS, **mbedTLS 3.6+ (default)**, OpenSSL 3.x, wolfSSL 5.x |
| **Transports** | CoAP/UDP, CoAP/DTLS, MQTT 3.1.1/5.0 |
| **Encodings** | TLV, JSON, SenML-JSON, SenML-CBOR, LwM2M CBOR |

## Layout

```
.
├── 2305-Wakaama/                          # Vendored Wakaama base (do not modify casually)
├── CMakeLists.txt                         # Top-level build (~46k)
├── _build/, build/, build_dev/, build_preprod/   # Generated — never commit
├── Issues/                                # Internal issue notes
├── Prompts/                               # Authoring prompts used during development
├── Friendly_LwM2M_Client_v1.2.2_Technical_Pitch_Deck.html   # Customer collateral
├── README.md                              # Long, authoritative
├── QUICK_BUILD_ALL_OBJECTS.txt
├── QUICK_REFERENCE_BUILD_FIXES.md
├── backup_object_id_update_*/             # Local backups, ignore
├── build.log                              # Generated, ignore
└── LICENSE
```

## Branch context — `claude/consolidate-branches-xCb7h`

A Claude-driven branch consolidation. Treat as a working branch, not main: small focused commits, no broad refactors. Read recent commits before adding new changes — the goal is to land cleanly, not expand scope.

## What's new in v1.2.2 (so you don't accidentally undo it)

- **Multi-DTLS provider** with factory pattern + RFC 9146 CID across providers; DTLS 1.3 on OpenSSL/wolfSSL.
- **Renamed example client**: `WppExample` → `friendly_example_client`.
- **MQTT transport binding** per OMA LwM2M v1.2.2 §8 with CBOR encoding.
- **Edge AI Inference** via TFLite + ONNX Runtime; new Object **33410**.
- **Delta FOTA** (BSDIFF / VCDIFF / Courgette) + **A/B partition rollback**.
- **New objects**: COSE (23), MQTT Server (24), Edge AI Inference (33410).

## Build

```bash
# Default (mbedTLS, all providers, full features)
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j

# Provider selection at configure time
cmake -DDTLS_PROVIDER_TINYDTLS=ON \
      -DDTLS_PROVIDER_MBEDTLS=ON \
      -DDTLS_PROVIDER_OPENSSL=OFF \
      -DDTLS_PROVIDER_WOLFSSL=OFF \
      -DDTLS_DEFAULT_PROVIDER=mbedtls \
      ..

# Run
./friendly_example_client --list-dtls-providers
./friendly_example_client --dtls-provider=mbedtls -h coaps://server:5684
```

The two reference files for build trouble are `QUICK_BUILD_ALL_OBJECTS.txt` and `QUICK_REFERENCE_BUILD_FIXES.md` — consult them before re-deriving fixes.

## Architectural rules

1. **Wakaama vendoring is intentional.** `2305-Wakaama/` is the upstream base. Don't fork code into the rest of the tree if it can live as a wrapper around Wakaama.
2. **DTLS provider abstraction** — anything new in the security path goes through the factory. Don't add provider-specific code paths in the core client.
3. **OMA LwM2M conformance is the spec, not preference.** Object IDs, resource IDs, content formats, and version semantics follow OMA-TS — cite the section in PRs.
4. **MQTT transport must follow OMA LwM2M v1.2.2 §8** end-to-end. CBOR is mandatory; do not introduce JSON paths to MQTT.
5. **A/B rollback** — never ship a FOTA path that can brick a device. Test the rollback every time the FOTA flow changes.
6. **Object 33410 (Edge AI Inference)** is a custom extension; keep its definition in lockstep with `Specs/`-style docs and the corresponding object XML if/when added to the LwM2M registry.

## Standards

- OMA LwM2M v1.2.2 (`OMA-TS-LightweightM2M_*`)
- RFC 7252 (CoAP), RFC 7641 (Observe), RFC 7959 (Block-wise)
- RFC 6347 (DTLS 1.2), RFC 9147 (DTLS 1.3), RFC 9146 (DTLS CID)
- RFC 8613 (OSCORE), RFC 9052/9053 (COSE)
- RFC 8949 (CBOR), RFC 8428 (SenML)

## Sensitive files / never edit casually

- `2305-Wakaama/` — vendored upstream; modify only with a deliberate downstream patch and document it.
- `Friendly_LwM2M_Client_v1.2.2_Technical_Pitch_Deck.html` — customer collateral.
- `build*/`, `_build/`, `backup_object_id_update_*/`, `build.log` — generated/transient; never commit.
- `Issues/`, `Prompts/` — internal notes.

## Commit style

Conventional commits. Reference OMA-TS section or RFC when fixing protocol-conformance bugs:
- `fix(mqtt): correct topic binding per OMA-TS-LightweightM2M_Core §8.4`
- `feat(dtls): add CID rotation per RFC 9146 §6`
- `chore(wakaama): refresh vendored base`


## QA DISCIPLINE (OpenClaw)

# CLAUDE.md — Project Behavioural Contract
# Version: 2.0 | Generic / Platform-Adaptive
# Source: OpenClaw / Tokoloshe workspace
# ─────────────────────────────────────────────────────────────────────────────
# THIS FILE IS THE LAW. All instructions here override conversational requests.
# Session start: READ THIS FILE COMPLETELY before touching any code.
# ─────────────────────────────────────────────────────────────────────────────

## PROJECT IDENTITY
```
PROJECT_NAME   : Friendly-LwM2M-Client
REPO_URL       : <fill>
PRIMARY_LANG   : C++20
PLATFORM_TYPE  : client
TARGET_PLATFORM: linux-x86_64
BUILD_SYSTEM   : cmake
TEST_FRAMEWORK : googletest
CI_SYSTEM      : github-actions
COVERAGE_TOOL  : gcovr
LINT_TOOL      : clang-tidy
```

---

## ██ LAWS — NEVER VIOLATE ██

These are hard constraints. No exception, no override, no "just this once".

### L1 — CODE INTEGRITY
- NEVER remove existing code without an explicit instruction naming the specific code
- NEVER comment out code silently — delete it or keep it, document why
- NEVER rename symbols without updating ALL call sites in the same commit
- NEVER change function signatures without updating all callers
- If net LOC in a modified file decreases unexpectedly → STOP, explain, wait for approval

### L2 — COMPLETION STANDARDS
- NEVER leave `TODO`, `FIXME`, `HACK`, `XXX`, `stub`, `placeholder`, `not implemented` in any file under `src/` or `lib/`
- NEVER generate partial implementations — complete the full logic or do not touch the file
- NEVER use empty catch blocks, swallowed exceptions, or silent error returns without a documented reason
- NEVER use `throw new RuntimeException("not implemented")` or equivalent as a placeholder
- Every `if` branch must have a corresponding test. Every `catch` must have a corresponding test.

### L3 — ANTI-HALLUCINATION
- NEVER invent API signatures — read the actual file or header before using
- NEVER assume a library function exists — verify in the dependency manifest first
- NEVER invent spec behaviour (RFC, OMA, 3GPP, POSIX) — cite the section or flag uncertainty
- If you are unsure about existing behaviour → READ THE FILE, do not guess
- Before any edit: state what you read and from where

### L4 — TEST COVERAGE
- 100% LINE COVERAGE is the minimum — not a target, a gate
- 100% BRANCH COVERAGE for all non-trivial logic
- Every public API function must have at minimum: happy path, error path, boundary test
- Test files are production quality — no stubs, no pass-through assertions, no `assertTrue(true)`
- Mock objects must verify behaviour, not just existence

### L5 — SAFETY & SECURITY (platform-aware)
- No hardcoded credentials, tokens, keys, or secrets — ever
- No `unsafe` blocks (Rust), `reinterpret_cast` (C++), or `eval()` (JS) without documented justification
- Input validation at all trust boundaries
- For embedded: no dynamic allocation in ISR context, no blocking calls in real-time paths

---

## SESSION START PROTOCOL

On every new session, execute in order:

```
1. Read CLAUDE.md (this file) — fully
2. Read MEMORY.md — load project state
3. Read BUILD_PROMPTS.md — load current sprint items
4. Run: cmake --build build --config Debug — verify baseline compiles
5. Run: ctest --test-dir build --output-on-failure  — verify baseline tests pass
6. Report: "Session init complete. Build: [PASS/FAIL]. Tests: [N/N]. 
            Open items: [count]. Ready."
```

If baseline build or tests fail → report immediately, do NOT proceed with new work.

---

## PLATFORM DETECTION & ROUTING

Detect platform from `PROJECT_IDENTITY` above and apply the matching section.

### ── EMBEDDED (Zephyr / FreeRTOS / ESP-IDF / bare-metal) ──
```
BUILD_CMD  : west build -b <board> / cmake --build build / idf.py build
TEST_CMD   : ctest --test-dir build --output-on-failure / pytest tests/host/
COVERAGE   : gcovr --branches --fail-under-line 100 --fail-under-branch 100
LINT       : clang-tidy src/**/*.cpp -- $(cat compile_flags.txt)
STATIC     : cppcheck --enable=all --error-exitcode=1 src/
EXTRA LAWS :
  - No heap allocation in ISR or hard-real-time paths (use static pools)
  - No blocking I/O without timeout — every wait has a deadline
  - Stack usage must be analysed (west build --target ram_report or similar)
  - RAII for all peripheral handles — no naked register manipulation in app layer
  - Thread safety: document every shared resource and its guard mechanism
```

### ── SERVER (Java Spring Boot / Python FastAPI / Node.js / Go / Rust) ──
```
BUILD_CMD  : ./gradlew build / mvn package / pip install -e . / npm run build / go build / cargo build
TEST_CMD   : ./gradlew test / pytest -x / npm test / go test ./... / cargo test
COVERAGE   : jacoco ≥100% / coverage.py --fail-under=100 / c8 --100 / go test -cover / tarpaulin --fail-under 100
LINT       : checkstyle + spotbugs / ruff + mypy --strict / eslint + tsc --noEmit / golangci-lint / clippy -D warnings
EXTRA LAWS :
  - All external I/O (DB, HTTP, queue) must be mockable — no direct calls in business logic
  - All config via environment / config file — zero hardcoded values
  - Health check + metrics endpoints mandatory for any long-running service
  - DB migrations versioned and reversible
  - API contracts (OpenAPI / protobuf) committed alongside implementation
```

### ── CLIENT (React/TS / React Native / Flutter / Qt/C++) ──
```
BUILD_CMD  : npm run build / flutter build / cmake --build build
TEST_CMD   : npm test -- --coverage / flutter test --coverage / ctest
COVERAGE   : istanbul/c8 --100 / lcov --fail-under-line 100 / gcovr
LINT       : eslint + tsc --noEmit / dart analyze / clang-tidy
EXTRA LAWS :
  - All network calls abstracted behind a repository/service layer
  - UI components tested with mock data — no live network in unit tests
  - Accessibility: WCAG AA for web, platform a11y APIs for mobile
  - No business logic in UI components — thin view layer only
```

### ── LIBRARY / SDK ──
```
EXTRA LAWS :
  - Zero runtime dependencies unless explicitly approved
  - Stable ABI guarantee documented in CHANGELOG
  - All public API headers/interfaces have complete docstrings
  - Semantic versioning enforced — breaking changes bump major
  - Example code in /examples/ must compile and pass as part of CI
```

---

## COMPLETION GATE

**A task is NOT done until ALL of the following pass:**

```bash
# Step 1 — Build (zero warnings)
cmake --build build --config Debug 2>&1 | tee .build.log
grep -E "error:|warning:" .build.log && exit 1 || echo "BUILD: PASS"

# Step 2 — Tests (100% pass)
ctest --test-dir build --output-on-failure 2>&1 | tee .test.log
grep -E "FAILED|ERROR" .test.log && exit 1 || echo "TESTS: PASS"

# Step 3 — Coverage (100% line + branch)
./scripts/coverage_gate.sh 2>&1 | tee .coverage.log
# Must not exit non-zero

# Step 4 — Lint (zero findings)
clang-tidy 2>&1 | tee .lint.log

# Step 5 — No forbidden patterns
grep -rn "TODO\|FIXME\|HACK\|stub\|placeholder\|not implemented" src/ lib/ && \
  echo "FAIL: Forbidden patterns found" && exit 1

# Step 6 — No silent deletions
git diff --diff-filter=D HEAD --name-only | \
  grep -v "^test\|^spec\|^__test__" && \
  echo "WARNING: Source files deleted — justify each one"

# Step 7 — No net LOC regression (modified files)
git diff HEAD --stat | grep -E "deletion" 
# Review each deletion manually
```

Mark task done only after reporting: `"GATE: ALL PASS"`.

---

## VERIFICATION LOOP (mandatory after every change)

See: `SKILL-verification-loop.md`

Execute after EVERY edit, no matter how small:
1. Build → fix all errors and warnings
2. Test → fix all failures
3. Coverage → add tests for any uncovered branch
4. Lint → fix all findings
5. Diff check → justify any deletions
6. Report results inline before proceeding

---

## TDD CONTRACT

1. Write the failing test FIRST — show test failure output
2. Write minimal implementation to make it pass — show pass output
3. Refactor — keep tests green
4. Coverage gate — if any branch uncovered, add test before closing
5. NEVER write implementation before a failing test exists

See: `SKILL-tdd-workflow.md`

---

## ANTI-HALLUCINATION PROTOCOL

Before implementing anything non-trivial:

```
SCRATCHPAD (required):
- What does this function/module need to do? (one sentence max)
- What are ALL the error/edge paths?
- Which existing files/functions does this touch?
- Which spec/RFC/standard governs this behaviour? (cite section)
- What tests will prove correctness?
- What could break in existing tests?
```

Output the scratchpad, wait for implicit approval (continue) before writing code.

---

## CODE REVIEW PROTOCOL

See: `CODE_REVIEW.md` for full checklist.

Multi-pass mandatory for PRs:
```
Pass 1 (Safety)     : memory, bounds, error paths, resource leaks
Pass 2 (Correctness): logic, state machines, protocol compliance  
Pass 3 (Tests)      : coverage gaps, assertion quality, mock fidelity
Pass 4 (Maintainability): clarity, docs, naming, fragility
```

---

## SESSION WRAP PROTOCOL (/wrapup)

Before closing any session:
1. Run full COMPLETION GATE — report results
2. Update `MEMORY.md` with: decisions made, coverage gaps found, open threads
3. Update `BUILD_PROMPTS.md` — move completed items, add new open items
4. Commit checkpoint: `git add -A && git commit -m "wip: [description] — session wrap"`
5. Report: open items count, coverage gaps by file, next session starting point

---

## MEMORY.md PERSISTENCE RULES

Store in MEMORY.md (survives sessions):
- Architectural decisions and rationale
- Known coverage gaps by file + line
- Fragile areas requiring extra test care
- External API quirks discovered during dev
- Performance baselines

Do NOT store:
- Session narratives
- Completed task descriptions
- Information derivable from the code itself

---

## ESCALATION — WHEN TO STOP AND ASK

Stop immediately and ask the user if:
- Requirement is ambiguous and two valid interpretations exist
- A spec section is unclear or contradictory
- A fix requires touching more than 3 files unexpectedly
- Test coverage cannot reach 100% due to dead code or platform constraints
- A dependency version conflict is found
- Build fails for reasons not immediately obvious

Do NOT guess. Do NOT proceed. Ask.
