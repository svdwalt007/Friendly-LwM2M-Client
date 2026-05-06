# Prompt: Friendly LwM2M Client — DTLS/CoAP Specification Coverage Audit & Update Plan

## Context

We have completed an investigation into the latest CoAP/DTLS capabilities implemented by two reference repositories:

- **`boaks/zephyr-coaps-client`** — A Zephyr RTOS CoAP/DTLS 1.2 CID client using Eclipse TinyDTLS, targeting nRF91x1 SoCs over LTE-M/NB-IoT. Implements RFC 9146 (DTLS 1.2 Connection ID) to eliminate frequent DTLS handshakes, reducing exchanges to two IP messages. Not an LwM2M client — raw CoAP/DTLS only.
- **`eclipse-californium/californium`** (4.0.0-M6) — Java CoAP framework with Scandium DTLS module. Implements RFC 9146 (CID), RFC 8449 (Record Size Limit), RFC 7627 (Extended Master Secret), RFC 7252/7641/7959 (CoAP core/Observe/Blockwise), CID-based cluster load balancing, DTLS graceful restart, and virtual thread support (Java 21).

Our investigation confirmed that OMA LwM2M v1.2/v1.2.1/v1.2.2 specifications have updated transport requirements to include:

- DTLS 1.2 Connection ID (RFC 9146) support
- DTLS 1.3 (RFC 9147) as a configurable option (not mandatory for conformance)
- TLS 1.3 (RFC 8446) for TCP bindings
- OSCORE (RFC 8613) independent of DTLS/TLS
- Extended Master Secret (RFC 7627) recommended
- SNI (RFC 6066) required when certificates are used
- EST over CoAP for certificate provisioning (Security Mode 4)

Neither reference repository implements DTLS 1.3 or OSCORE. The primary actionable improvement is DTLS 1.2 CID (RFC 9146), which directly addresses NAT rebinding issues for sleepy LwM2M clients using Queue Mode over LTE-M/NB-IoT.

---

## Task

Perform a systematic review of the **Friendly LwM2M Client** codebase. Assess current DTLS/CoAP specification coverage, identify gaps against the requirements below, and produce a prioritised update plan.

---

## Step 1: Codebase Discovery

Examine the repository structure and identify:

1. **DTLS library in use** — Which DTLS implementation is linked (TinyDTLS, mbedTLS, wolfSSL, Zephyr's built-in, Nordic's nrf_security, or other)? What version? Where is it integrated (submodule, vendored, system dependency)?
2. **CoAP library in use** — Which CoAP stack (Wakaama/liblwm2m, Zephyr's built-in CoAP, libcoap, custom implementation)? What version?
3. **LwM2M protocol version** — What LwM2M version does the client claim conformance to (check registration parameters, `lwm2m=` version tag, supported content formats)?
4. **Transport bindings implemented** — Which transports are supported (CoAP/UDP, CoAP/TCP, CoAP/SMS, Non-IP, CoAP/WebSocket)?
5. **Security modes implemented** — Which credential types (PSK, RPK, x509, NoSec, EST)? Where are credentials stored and provisioned?
6. **Build system and target platform** — CMake/Kconfig/Makefile? Target MCU family? RTOS?

---

## Step 2: Specification Coverage Audit

For each item below, determine: **Supported / Partial / Not Supported / N/A**, with evidence from the codebase (file paths, config flags, API calls).

### 2.1 DTLS 1.2 Core (RFC 6347)

- [ ] Full DTLS 1.2 handshake (ClientHello, HelloVerifyRequest, ServerHello, etc.)
- [ ] Session resumption (abbreviated handshake)
- [ ] Anti-replay protection (sequence number window)
- [ ] DTLS retransmission timer implementation (exponential backoff per RFC 6347 §4.2.4)
- [ ] HelloVerifyRequest / cookie exchange for DoS mitigation
- [ ] Epoch handling for key material transitions

### 2.2 DTLS 1.2 Connection ID — RFC 9146

This is the highest-priority gap to assess.

- [ ] `connection_id` extension advertised in ClientHello
- [ ] CID value generation (fixed-length vs variable-length)
- [ ] Zero-length CID support (advertise support without requesting server use CID)
- [ ] CID-based record format (ContentType 25 / `tls12_cid`)
- [ ] Content type encryption within CID records
- [ ] Record layer padding support within CID records
- [ ] CID renegotiation on session resumption
- [ ] Persistence of CID across sleep/wake cycles (critical for PSM wakeup)
- [ ] Handling of peer IP/port changes while maintaining CID-bound session

### 2.3 DTLS Extensions

- [ ] Extended Master Secret (RFC 7627)
- [ ] Record Size Limit (RFC 8449) — important for constrained MTU environments
- [ ] Maximum Fragment Length Negotiation (RFC 6066 §4)
- [ ] Server Name Indication / SNI (RFC 6066 §3) — required by LwM2M v1.2 for certificate mode
- [ ] Supported Elliptic Curves / Supported Groups (RFC 8422)
- [ ] Signature Algorithms (RFC 5246 §7.4.1.4.1)
- [ ] DTLS-SRTP (not expected, confirm N/A)

### 2.4 Cipher Suites

Verify support for LwM2M-mandated cipher suites:

- [ ] `TLS_PSK_WITH_AES_128_CCM_8` (mandatory for PSK mode)
- [ ] `TLS_PSK_WITH_AES_128_CBC_SHA256` (legacy, check if still offered)
- [ ] `TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8` (mandatory for RPK/x509)
- [ ] `TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256` (legacy)
- [ ] Are CBC suites disabled or deprioritised? (they should be — not recommended for new deployments)

### 2.5 DTLS 1.3 (RFC 9147)

- [ ] Any DTLS 1.3 support present?
- [ ] If not, does the DTLS library have a DTLS 1.3 branch or roadmap?
- [ ] Can the client negotiate downgrade from 1.3 to 1.2 gracefully?

### 2.6 CoAP Core (RFC 7252)

- [ ] CON/NON/ACK/RST message types
- [ ] Token handling (length negotiation, matching)
- [ ] Option processing (critical vs elective, proxy-safe vs proxy-unsafe)
- [ ] Content-Format option support (list all supported formats)
- [ ] Block-wise transfers (RFC 7959) — Block1 (upload) and Block2 (download)
- [ ] Observe (RFC 7641) — registration, notification, deregistration
- [ ] CoAP over TCP/TLS (RFC 8323) — if supported
- [ ] No Server Response option (RFC 7967)
- [ ] ETag / If-Match / If-None-Match conditional options

### 2.7 LwM2M Content Formats

- [ ] TLV (OMA-TLV, mandatory for v1.0)
- [ ] Plain Text (mandatory)
- [ ] Opaque (mandatory)
- [ ] SenML JSON (RFC 8428, mandatory for v1.1+)
- [ ] SenML CBOR (RFC 8428, mandatory for v1.1+)
- [ ] LwM2M CBOR (v1.2+)
- [ ] Core Link Format (RFC 6690, for registration)

### 2.8 OSCORE (RFC 8613)

- [ ] OSCORE security context derivation
- [ ] OSCORE option processing
- [ ] OSCORE + DTLS layered operation
- [ ] OSCORE Object (ID: 21) support
- [ ] Replay protection window

### 2.9 LwM2M Security Object (ID: 0)

Check which resources are implemented:

- [ ] Resource 0: LwM2M Server URI
- [ ] Resource 1: Bootstrap-Server flag
- [ ] Resource 2: Security Mode (0=PSK, 1=RPK, 2=Certificate, 3=NoSec, 4=EST)
- [ ] Resource 3: Public Key or Identity
- [ ] Resource 4: Server Public Key
- [ ] Resource 5: Secret Key
- [ ] Resource 6: SMS Security Mode
- [ ] Resource 10: OSCORE Security Mode
- [ ] Resource 11: SNI
- [ ] Resource 12: Certificate Usage
- [ ] Resource 13: DTLS/TLS Ciphersuite (v1.2+)
- [ ] Resource 14: DTLS/TLS Version (v1.2+ — controls 1.2 vs 1.3 negotiation)

---

## Step 3: Library Version & Upstream Assessment

For each dependency (DTLS library, CoAP library, LwM2M library):

1. What version is currently used?
2. What is the latest stable release?
3. Does the latest release add any of the missing capabilities identified above?
4. Are there breaking API changes between current and latest?
5. What is the library's stance on DTLS 1.3 support (shipped, in-progress, planned, not planned)?
6. Are there known CVEs in the current version that are fixed in later releases?

---

## Step 4: Update Plan

Based on the gaps identified, produce a prioritised plan with the following tiers:

### Tier 1 — Critical (DTLS 1.2 CID + Session Persistence)

- RFC 9146 CID support in ClientHello
- CID persistence across PSM sleep/wake cycles
- Server IP/port change tolerance with active CID
- Testing against Californium/Leshan server with CID enabled

### Tier 2 — High Value (Security Hardening)

- Extended Master Secret (RFC 7627)
- SNI support (RFC 6066) for certificate deployments
- Record Size Limit (RFC 8449) for constrained-MTU links (NB-IoT)
- Deprecate CBC cipher suites, ensure CCM_8 is preferred
- Security Object resources 11–14 (v1.2 additions)

### Tier 3 — Conformance (LwM2M v1.2 Content Formats)

- LwM2M CBOR encoding support
- SenML CBOR if not already present
- Composite operations (Read-Composite, Write-Composite, Observe-Composite)

### Tier 4 — Future (DTLS 1.3 / OSCORE)

- DTLS 1.3 readiness assessment
- OSCORE implementation if required by deployment profile
- EST over CoAP (Security Mode 4) for automated certificate provisioning

For each item, specify:

- Files/modules that need modification
- Estimated complexity (trivial / moderate / significant / requires library upgrade)
- Dependencies or blockers
- Testing approach (unit test, interop test against specific server, ETSI plugtest alignment)

---

## Step 5: Interoperability Verification Plan

Describe how each change will be verified:

1. **CID negotiation** — Wireshark capture showing `connection_id` extension in ClientHello/ServerHello, CID in record headers after handshake
2. **NAT rebind survival** — Simulate IP/port change (e.g., toggle airplane mode on cellular modem), verify CoAP request succeeds on existing DTLS session without re-handshake
3. **Session resumption with CID** — After PSM wakeup, verify abbreviated handshake reuses CID-bound session
4. **Server compatibility matrix** — Test against Leshan (Californium), Wakaama server, Anjay server, and any Friendly LwM2M Server instances
5. **Regression** — Verify existing PSK/RPK/x509 flows still work with CID disabled (backward compatibility)

---

## Output Format

Produce your findings as:

1. A **coverage matrix** (table) mapping each specification item to Supported/Partial/Not Supported with file references
2. A **gap analysis** narrative explaining the impact of each gap
3. A **prioritised update plan** with concrete file-level change descriptions
4. A **dependency upgrade recommendation** with version numbers and breaking change notes
5. A **risk assessment** for each tier of changes
