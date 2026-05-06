# DTLS Connection ID (CID) Implementation Plan
## For Friendly LwM2M Client (TinyDTLS 0.8.6)

**Status:** Ready for Implementation
**Target:** TinyDTLS-based DTLS implementation
**Standards:** RFC 9146 (CID54) + draft-ietf-tls-dtls-connection-id (CID53)

---

## Executive Summary

This document provides a complete implementation plan for adding DTLS Connection ID (CID) support to the Friendly LwM2M Client, which uses TinyDTLS 0.8.6 as its DTLS library. The implementation will enable session persistence across NAT rebinding events and reduce handshake overhead for battery-powered IoT devices.

**Key Finding:** TinyDTLS does NOT have built-in CID support, requiring modifications to the library source code.

---

## Table of Contents

1. [Current Architecture Analysis](#1-current-architecture-analysis)
2. [CID Implementation Strategy](#2-cid-implementation-strategy)
3. [Detailed Implementation Steps](#3-detailed-implementation-steps)
4. [File Modifications Required](#4-file-modifications-required)
5. [Configuration and API](#5-configuration-and-api)
6. [Testing Strategy](#6-testing-strategy)
7. [Migration and Deployment](#7-migration-and-deployment)
8. [Risk Analysis](#8-risk-analysis)

---

## 1. Current Architecture Analysis

### 1.1 DTLS Library: TinyDTLS 0.8.6

**Location:** `2305-Wakaama/examples/shared/tinydtls/`

**Key Characteristics:**
- Minimal DTLS 1.2 implementation (~10K lines)
- Designed for constrained IoT devices
- No built-in CID support
- Supports PSK and RPK authentication
- AES-CCM-8 encryption (AEAD cipher)

### 1.2 Current DTLS Flow

```
┌──────────────────────────────────────────────────────────────┐
│                     Current Architecture                      │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  Application (WppExample)                                    │
│         ↓                                                    │
│  Connection.cpp (DTLS wrapper)                               │
│         ↓                                                    │
│  TinyDTLS Library                                            │
│    • dtls.c (handshake + record layer)                       │
│    • crypto.c (encryption/decryption)                        │
│    • peer.c (session state)                                  │
│         ↓                                                    │
│  UDP Socket (CoAP transport)                                 │
│                                                              │
│  Session Keying: (IP, Port) 5-tuple                          │
│  ❌ Problem: NAT rebinding breaks session                    │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

### 1.3 Current Session Management

**Session Structure** (`peer.h`):
```c
typedef struct dtls_peer_t {
  struct dtls_peer_t *next;
  session_t session;                    // IP:Port 5-tuple
  dtls_state state;                     // Handshake state
  dtls_security_parameters_t *security_params[2];  // epoch 0 & 1
  dtls_handshake_parameters_t *handshake_params;
  // ❌ NO CID FIELDS - NEED TO ADD
} dtls_peer_t;
```

**Problem:** Sessions are keyed by `session_t` which contains:
- Source IP address
- Source port
- Destination IP address
- Destination port

When NAT rebinding occurs (e.g., after device sleep), the source port changes → session lookup fails → full re-handshake required.

### 1.4 Current Record Format

**Standard DTLS 1.2 Record** (`dtls.h`, line 342):
```c
typedef struct {
  uint8 content_type;       // 20, 21, 22, 23
  uint16 version;           // 0xFEFD (DTLS 1.2)
  uint16 epoch;
  uint48 sequence_number;
  uint16 length;
  uint8 fragment[length];
} dtls_record_header_t;
```

**Target:** Add CID variant with content type 25 (tls12_cid).

---

## 2. CID Implementation Strategy

### 2.1 Two-Phase Approach

**Phase 1: TinyDTLS Core Modifications** (HIGH RISK)
- Modify TinyDTLS source code directly
- Add CID extension support
- Implement CID record format
- Update AEAD AAD construction

**Phase 2: Application Integration** (LOW RISK)
- Add CID configuration options
- Implement CID persistence
- Update connection management
- Add monitoring/diagnostics

### 2.2 Design Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| **Modify TinyDTLS?** | ✅ Yes, in-place | No external CID-capable DTLS library for embedded |
| **Support CID53?** | ✅ Yes | Some servers still use draft version |
| **Support CID54?** | ✅ Yes (preferred) | RFC 9146 is the final standard |
| **Fallback Strategy** | Graceful degradation | Try CID54 → CID53 → No CID |
| **CID Length** | 8 bytes (configurable 1-8) | Good balance of security vs overhead |
| **CID Generation** | Hardware RNG | Maximum uniqueness and security |
| **Persistence** | Optional (NVS) | For devices with persistent storage |

### 2.3 Backward Compatibility

```
┌─────────────────────────────────────────────────────────────┐
│               Compatibility Matrix                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Server Support    Client Behavior        Result           │
│  ─────────────────────────────────────────────────────      │
│  CID54             Send CID54 ext      → CID54 session ✅   │
│  CID53             Fallback CID53      → CID53 session ✅   │
│  No CID            Omit extension      → Legacy DTLS   ✅   │
│  Rejects CID       Retry without CID   → Legacy DTLS   ✅   │
│                                                             │
│  ✅ ZERO BREAKING CHANGES                                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. Detailed Implementation Steps

### Step 1: Add CID Extension Constants (LOW RISK)

**File:** `2305-Wakaama/examples/shared/tinydtls/global.h`

**Add after line 102:**
```c
/* DTLS Connection ID (CID) extension types */
#define TLS_EXT_CID_DRAFT       53  /* draft-ietf-tls-dtls-connection-id */
#define TLS_EXT_CID_RFC9146     54  /* RFC 9146 (final standard) */

/* CID record content type */
#define DTLS_CT_TLS12_CID       25  /* RFC 9146 Section 5 */

/* CID configuration */
#define DTLS_CID_MAX_SIZE       8   /* Maximum CID length in bytes */
#define DTLS_CID_MIN_SIZE       1   /* Minimum CID length */
```

---

### Step 2: Extend Peer Structure (MEDIUM RISK)

**File:** `2305-Wakaama/examples/shared/tinydtls/peer.h`

**Add CID fields to `dtls_peer_t` structure (after line 75):**
```c
typedef struct dtls_peer_t {
  struct dtls_peer_t *next;
  session_t session;
  dtls_state state;
  dtls_security_parameters_t *security_params[2];
  dtls_handshake_parameters_t *handshake_params;

  /* ===== CID SUPPORT (RFC 9146) ===== */
  unsigned int cid_negotiated:1;      /* CID successfully negotiated */
  unsigned int cid_version:2;         /* 0=none, 1=CID53, 2=CID54 */
  uint8_t our_cid_len;                /* Length of CID we use for RX */
  uint8_t peer_cid_len;               /* Length of CID peer uses for RX */
  uint8_t our_cid[DTLS_CID_MAX_SIZE]; /* Our CID for incoming demux */
  uint8_t peer_cid[DTLS_CID_MAX_SIZE];/* Peer's CID for outgoing records */
  /* ================================== */
} dtls_peer_t;
```

**Initialize in `dtls_new_peer()` function (`peer.c`, line ~120):**
```c
peer->cid_negotiated = 0;
peer->cid_version = 0;
peer->our_cid_len = 0;
peer->peer_cid_len = 0;
memset(peer->our_cid, 0, DTLS_CID_MAX_SIZE);
memset(peer->peer_cid, 0, DTLS_CID_MAX_SIZE);
```

---

### Step 3: Add CID Configuration (LOW RISK)

**File:** `2305-Wakaama/examples/shared/tinydtls/dtls_config.h`

**Add after existing #defines (line ~50):**
```c
/** Enable DTLS Connection ID (RFC 9146) support */
#ifndef DTLS_CID
#define DTLS_CID 1
#endif

/** Prefer RFC 9146 (CID54) over draft (CID53) */
#ifndef DTLS_CID_PREFER_RFC9146
#define DTLS_CID_PREFER_RFC9146 1
#endif

/** Enable CID53 fallback if server doesn't support CID54 */
#ifndef DTLS_CID_FALLBACK_DRAFT
#define DTLS_CID_FALLBACK_DRAFT 1
#endif

/** Default CID length to request (1-8 bytes) */
#ifndef DTLS_CID_DEFAULT_LENGTH
#define DTLS_CID_DEFAULT_LENGTH 8
#endif
```

---

### Step 4: Implement CID Extension Handling (HIGH RISK)

**File:** `2305-Wakaama/examples/shared/tinydtls/dtls.c`

#### 4.1 Add CID Extension Builder for ClientHello

**Add new function (around line 1500, near other extension builders):**
```c
#if DTLS_CID
/**
 * Build CID extension for ClientHello
 * @param peer DTLS peer context
 * @param p Buffer to write extension data
 * @param ext_type Extension type (CID53 or CID54)
 * @return Number of bytes written
 */
static int
dtls_add_cid_extension(dtls_peer_t *peer, uint8 *p, uint16_t ext_type) {
  uint8 *p_orig = p;

  /* Extension Type */
  dtls_int_to_uint16(p, ext_type);
  p += sizeof(uint16);

  /* Extension Length: 1 byte (CID length we want to receive) */
  dtls_int_to_uint16(p, 1);
  p += sizeof(uint16);

  /* CID Length we want server to use when sending to us */
  *p++ = DTLS_CID_DEFAULT_LENGTH;

  dtls_debug("Added CID extension (type %d, length %d)\n",
             ext_type, DTLS_CID_DEFAULT_LENGTH);

  return p - p_orig;
}
#endif /* DTLS_CID */
```

#### 4.2 Modify ClientHello Builder to Include CID Extension

**Find:** `dtls_send_client_hello()` function (around line 2400)

**Add CID extension after existing extensions:**
```c
/* Add existing extensions (elliptic curves, point formats, etc.) */
// ... existing code ...

#if DTLS_CID
  /* Add CID extension - try RFC 9146 first */
  if (DTLS_CID_PREFER_RFC9146) {
    p += dtls_add_cid_extension(peer, p, TLS_EXT_CID_RFC9146);
  } else {
    /* Try draft version first if configured */
    p += dtls_add_cid_extension(peer, p, TLS_EXT_CID_DRAFT);
  }
#endif
```

#### 4.3 Parse CID Extension from ServerHello

**Find:** `check_server_hello()` function (around line 1800)

**Add CID extension parsing in extension loop:**
```c
#if DTLS_CID
      case TLS_EXT_CID_RFC9146:
      case TLS_EXT_CID_DRAFT:
        {
          dtls_debug("Server sent CID extension (type %d)\n", ext_type);

          /* Extension data: cid_length (1 byte) + cid_value (variable) */
          if (ext_length < 1) {
            dtls_warn("Invalid CID extension length\n");
            return dtls_alert_fatal_create(DTLS_ALERT_DECODE_ERROR);
          }

          /* Get CID length server wants to receive from us */
          peer->peer_cid_len = dtls_uint8_to_int(data);
          data += sizeof(uint8);

          if (peer->peer_cid_len > 0) {
            if (peer->peer_cid_len > DTLS_CID_MAX_SIZE) {
              dtls_warn("Server requested CID too long (%d)\n", peer->peer_cid_len);
              return dtls_alert_fatal_create(DTLS_ALERT_ILLEGAL_PARAMETER);
            }

            /* Copy the CID we should use when sending to server */
            if (ext_length - 1 != peer->peer_cid_len) {
              dtls_warn("CID extension length mismatch\n");
              return dtls_alert_fatal_create(DTLS_ALERT_DECODE_ERROR);
            }

            memcpy(peer->peer_cid, data, peer->peer_cid_len);

            /* Generate our own CID for incoming demux */
            peer->our_cid_len = DTLS_CID_DEFAULT_LENGTH;
            dtls_prng(peer->our_cid, peer->our_cid_len);

            /* Mark CID as negotiated */
            peer->cid_negotiated = 1;
            peer->cid_version = (ext_type == TLS_EXT_CID_RFC9146) ? 2 : 1;

            dtls_info("CID negotiated: version=%s, our_len=%d, peer_len=%d\n",
                     (peer->cid_version == 2) ? "RFC9146" : "draft",
                     peer->our_cid_len, peer->peer_cid_len);
          }
        }
        break;
#endif /* DTLS_CID */
```

---

### Step 5: Implement CID Record Format (HIGH RISK)

**File:** `2305-Wakaama/examples/shared/tinydtls/dtls.c`

#### 5.1 Modify Record Sending Function

**Find:** `dtls_send_handshake_msg_hash()` or `dtls_send()` functions (around line 1200)

**Add CID record builder:**
```c
#if DTLS_CID
/**
 * Build DTLS record with CID (RFC 9146 Section 5)
 * @param security Security parameters (keys, epoch, seq)
 * @param type Inner content type (real content type)
 * @param peer Peer context (contains CID info)
 * @param buf Plaintext data to encrypt
 * @param buflen Length of plaintext
 * @param sendbuf Output buffer for record
 * @return Length of output record, or <0 on error
 */
static int
dtls_prepare_record_with_cid(dtls_security_parameters_t *security,
                              unsigned char type,
                              dtls_peer_t *peer,
                              uint8 *buf, size_t buflen,
                              uint8 *sendbuf) {
  uint8 *p = sendbuf;

  /* Outer content type is always TLS12_CID (25) */
  dtls_int_to_uint8(p, DTLS_CT_TLS12_CID);
  p += sizeof(uint8);

  /* Protocol version */
  dtls_int_to_uint16(p, DTLS_VERSION);
  p += sizeof(uint16);

  /* Epoch */
  dtls_int_to_uint16(p, security->epoch);
  p += sizeof(uint16);

  /* Sequence Number (48 bits) */
  dtls_int_to_uint48(p, security->rseq);
  p += sizeof(uint48);

  /* Connection ID */
  memcpy(p, peer->peer_cid, peer->peer_cid_len);
  p += peer->peer_cid_len;

  /* Build inner plaintext: data + inner_content_type */
  uint8 inner_plaintext[buflen + 1];
  memcpy(inner_plaintext, buf, buflen);
  inner_plaintext[buflen] = type;  /* Real content type at END */

  /* Build AAD for AEAD (see RFC 9146 Section 5) */
  dtls_cipher_context_t *cipher_context = security->read_cipher;
  uint8 nonce[DTLS_CCM_NONCE_SIZE];
  uint8 aad[13 + peer->peer_cid_len + 1];  /* Larger AAD with CID */
  int aad_len = 0;

  /* AAD: epoch (2) + seq (6) + outer_type (1) + cid_len (1) + cid +
          inner_type (1) + version (2) + length (2) */
  dtls_int_to_uint16(aad + aad_len, security->epoch);
  aad_len += 2;
  dtls_int_to_uint48(aad + aad_len, security->rseq);
  aad_len += 6;
  aad[aad_len++] = DTLS_CT_TLS12_CID;               /* Outer type */
  aad[aad_len++] = peer->peer_cid_len;              /* CID length */
  memcpy(aad + aad_len, peer->peer_cid, peer->peer_cid_len);
  aad_len += peer->peer_cid_len;
  aad[aad_len++] = type;                            /* Inner type */
  dtls_int_to_uint16(aad + aad_len, DTLS_VERSION);  /* Version */
  aad_len += 2;
  dtls_int_to_uint16(aad + aad_len, buflen + 1);    /* Inner plaintext length */
  aad_len += 2;

  /* Encrypt inner plaintext */
  long int res = dtls_encrypt(inner_plaintext, buflen + 1,
                              p + sizeof(uint16),  /* Output after length field */
                              nonce, DTLS_CCM_NONCE_SIZE,
                              aad, aad_len,
                              cipher_context);
  if (res < 0) {
    dtls_warn("Encryption failed with CID\n");
    return res;
  }

  /* Length field */
  dtls_int_to_uint16(p, res);
  p += sizeof(uint16) + res;

  return p - sendbuf;
}
#endif /* DTLS_CID */
```

**Modify `dtls_prepare_record()` to check for CID:**
```c
static int
dtls_prepare_record(dtls_peer_t *peer,
                    dtls_security_parameters_t *security,
                    unsigned char *type,
                    uint8 *buf, size_t buflen,
                    uint8 *sendbuf, size_t *rlen) {
#if DTLS_CID
  /* Use CID record format if negotiated */
  if (peer->cid_negotiated && peer->peer_cid_len > 0) {
    int res = dtls_prepare_record_with_cid(security, *type, peer,
                                           buf, buflen, sendbuf);
    if (res > 0) {
      *rlen = res;
      return 0;
    }
    /* Fallthrough to legacy on error */
  }
#endif

  /* Original legacy record building */
  // ... existing code ...
}
```

#### 5.2 Modify Record Receiving Function

**Find:** `dtls_handle_message()` function (around line 3412)

**Add CID record parser before existing record handling:**
```c
#if DTLS_CID
  /* Check for CID record (content type 25) */
  if (dtls_uint8_to_int(msg_hdr->content_type) == DTLS_CT_TLS12_CID) {
    dtls_debug("Received CID record\n");

    /* Find peer by CID instead of 5-tuple */
    dtls_peer_t *peer = dtls_get_peer_by_cid(ctx, msg + 13, msg_len - 13);
    if (!peer) {
      dtls_warn("No peer found for CID\n");
      return -1;
    }

    /* Verify CID matches */
    size_t header_len = 13;  /* type(1) + ver(2) + epoch(2) + seq(6) + cid_len + cid */
    if (msg_len < header_len + peer->our_cid_len + 2) {
      dtls_warn("CID record too short\n");
      return -1;
    }

    uint8 *cid_field = msg + 13;  /* After standard header */
    if (memcmp(cid_field, peer->our_cid, peer->our_cid_len) != 0) {
      dtls_warn("CID mismatch\n");
      return -1;
    }

    /* Decrypt and extract inner content type */
    uint8 *cid_payload = cid_field + peer->our_cid_len;
    uint16 cid_length = dtls_uint16_to_int(cid_payload);
    cid_payload += 2;

    /* Decrypt with AAD including CID */
    uint8 decrypted[cid_length];
    // ... decryption code using modified AAD ...

    /* Inner content type is LAST byte */
    uint8 inner_type = decrypted[cid_length - 1];
    size_t payload_len = cid_length - 1;

    /* Process decrypted content with real type */
    return dtls_handle_decrypted_record(ctx, peer, inner_type,
                                        decrypted, payload_len);
  }
#endif /* DTLS_CID */

  /* Fall through to legacy record handling */
  // ... existing code ...
```

#### 5.3 Add CID-based Peer Lookup

**Add new function in `peer.c`:**
```c
#if DTLS_CID
/**
 * Find peer by Connection ID
 * @param ctx DTLS context
 * @param cid Connection ID to search for
 * @param cid_len Length of CID
 * @return Peer if found, NULL otherwise
 */
dtls_peer_t *
dtls_get_peer_by_cid(const dtls_context_t *ctx,
                     const uint8 *cid, size_t cid_len) {
  dtls_peer_t *peer;

  for (peer = list_head(ctx->peers); peer; peer = list_item_next(peer)) {
    if (peer->cid_negotiated &&
        peer->our_cid_len == cid_len &&
        memcmp(peer->our_cid, cid, cid_len) == 0) {
      return peer;
    }
  }

  return NULL;
}
#endif /* DTLS_CID */
```

**Add declaration in `peer.h`:**
```c
#if DTLS_CID
dtls_peer_t *dtls_get_peer_by_cid(const dtls_context_t *ctx,
                                  const uint8 *cid, size_t cid_len);
#endif
```

---

### Step 6: Application Integration (LOW RISK)

**File:** `examples/platform/connection_dtls/Connection.cpp`

#### 6.1 Add CID Configuration Options

**Add to Connection class (Connection.h):**
```cpp
class Connection : public WppConnection {
public:
    // ... existing methods ...

    // CID configuration
    void enableCID(bool enable, uint8_t cid_length = 8);
    bool isCIDNegotiated(const dtls_connection_t *conn) const;
    std::vector<uint8_t> getOurCID(const dtls_connection_t *conn) const;
    std::vector<uint8_t> getPeerCID(const dtls_connection_t *conn) const;

private:
    bool _cid_enabled = true;  // CID enabled by default
    uint8_t _cid_length = 8;   // Default CID length
};
```

#### 6.2 Implement Configuration Methods

**Add to Connection.cpp:**
```cpp
void Connection::enableCID(bool enable, uint8_t cid_length) {
    _cid_enabled = enable;
    _cid_length = (cid_length >= 1 && cid_length <= 8) ? cid_length : 8;

    std::cout << "CID " << (enable ? "enabled" : "disabled")
              << " (length: " << (int)_cid_length << ")" << std::endl;
}

bool Connection::isCIDNegotiated(const dtls_connection_t *conn) const {
    if (!conn || !conn->dtlsSession) return false;

    dtls_peer_t *peer = dtls_get_peer(conn->dtlsContext, conn->dtlsSession);
    return peer && peer->cid_negotiated;
}

std::vector<uint8_t> Connection::getOurCID(const dtls_connection_t *conn) const {
    if (!conn || !conn->dtlsSession) return {};

    dtls_peer_t *peer = dtls_get_peer(conn->dtlsContext, conn->dtlsSession);
    if (!peer || !peer->cid_negotiated) return {};

    return std::vector<uint8_t>(peer->our_cid, peer->our_cid + peer->our_cid_len);
}

std::vector<uint8_t> Connection::getPeerCID(const dtls_connection_t *conn) const {
    if (!conn || !conn->dtlsSession) return {};

    dtls_peer_t *peer = dtls_get_peer(conn->dtlsContext, conn->dtlsSession);
    if (!peer || !peer->cid_negotiated) return {};

    return std::vector<uint8_t>(peer->peer_cid, peer->peer_cid + peer->peer_cid_len);
}
```

#### 6.3 Add CID Status Logging

**Modify `connect()` function to log CID status after handshake:**
```cpp
// After successful connection (around line 195)
if (isCIDNegotiated(connP)) {
    auto our_cid = getOurCID(connP);
    auto peer_cid = getPeerCID(connP);

    std::cout << "✅ CID negotiated!" << std::endl;
    std::cout << "   Our CID:  ";
    for (auto b : our_cid) printf("%02X", b);
    std::cout << std::endl << "   Peer CID: ";
    for (auto b : peer_cid) printf("%02X", b);
    std::cout << std::endl;
} else {
    std::cout << "⚠️  CID not negotiated (legacy DTLS)" << std::endl;
}
```

---

### Step 7: CLI Options Integration (LOW RISK)

**File:** `examples/cli_options.h`

**Add CID options:**
```cpp
struct CliOptions {
    // ... existing fields ...

    // CID configuration
    bool enable_cid;              // Enable CID negotiation
    bool prefer_cid54;            // Prefer RFC 9146 over draft
    uint8_t cid_length;           // CID length (1-8 bytes)

    CliOptions()
        // ... existing initializers ...
        , enable_cid(true)
        , prefer_cid54(true)
        , cid_length(8)
    {}
};
```

**Add command-line options:**
```cpp
{"enable-cid",   no_argument,       0, 134},
{"disable-cid",  no_argument,       0, 135},
{"cid-length",   required_argument, 0, 136},
{"prefer-cid53", no_argument,       0, 137},
```

**Add help text:**
```cpp
<< "DTLS CID Options:\n"
<< "      --enable-cid             Enable CID negotiation (default)\n"
<< "      --disable-cid            Disable CID negotiation\n"
<< "      --cid-length=LENGTH      CID length in bytes 1-8 (default: 8)\n"
<< "      --prefer-cid53           Prefer draft CID53 over RFC9146\n\n"
```

---

## 4. File Modifications Required

### Summary Table

| File | Lines | Risk | Changes |
|------|-------|------|---------|
| `global.h` | +10 | LOW | Add CID constants |
| `dtls_config.h` | +20 | LOW | Add CID config macros |
| `peer.h` | +8 | MED | Extend peer structure |
| `peer.c` | +30 | MED | Add CID peer lookup, init |
| `dtls.c` | +300 | HIGH | Extension handling, record format |
| `crypto.c` | +50 | MED | Modify AAD construction |
| `Connection.h` | +15 | LOW | Add CID API methods |
| `Connection.cpp` | +80 | LOW | Implement CID API |
| `cli_options.h` | +30 | LOW | Add CID CLI options |
| `main.cpp` | +10 | LOW | Configure CID from CLI |

**Total LOC:** ~550 lines added/modified
**High Risk Files:** 1 (dtls.c)
**Medium Risk Files:** 3 (peer.h/c, crypto.c)
**Low Risk Files:** 5 (config, app integration)

---

## 5. Configuration and API

### 5.1 Compile-Time Configuration

**Build with CID support:**
```bash
cd _build
cmake ../examples -DDTLS_CID=1 -DDTLS_CID_PREFER_RFC9146=1
make -j$(nproc)
```

**Build without CID support:**
```bash
cmake ../examples -DDTLS_CID=0
make -j$(nproc)
```

### 5.2 Runtime Configuration

**Via CLI:**
```bash
# Enable CID with 8-byte length (default)
./WppExample --enable-cid

# Disable CID
./WppExample --disable-cid

# Custom CID length
./WppExample --enable-cid --cid-length=4

# Prefer draft CID53
./WppExample --enable-cid --prefer-cid53
```

**Via Code:**
```cpp
Connection connection(options.local_port, options.address_family);
connection.enableCID(true, 8);  // Enable with 8-byte CID
```

### 5.3 Monitoring API

**Check CID status:**
```cpp
if (connection.isCIDNegotiated(conn)) {
    auto our_cid = connection.getOurCID(conn);
    auto peer_cid = connection.getPeerCID(conn);

    std::cout << "CID active" << std::endl;
    // Use CID values for logging, persistence, etc.
}
```

---

## 6. Testing Strategy

### 6.1 Unit Tests (TinyDTLS Level)

**Test 1: CID Extension Encoding**
```c
void test_cid_extension_encode() {
    uint8 buffer[100];
    int len = dtls_add_cid_extension(NULL, buffer, TLS_EXT_CID_RFC9146);

    assert(len == 5);  // type(2) + length(2) + cid_length(1)
    assert(buffer[0] == 0x00 && buffer[1] == 0x36);  // Type 54
    assert(buffer[4] == 8);  // CID length
}
```

**Test 2: CID Extension Parsing**
```c
void test_cid_extension_parse() {
    uint8 server_hello_cid[] = {
        0x00, 0x36,  // Extension type 54
        0x00, 0x09,  // Extension length 9
        0x08,        // CID length 8
        0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x90  // CID value
    };

    dtls_peer_t peer = {0};
    int res = parse_cid_extension(&peer, server_hello_cid, sizeof(server_hello_cid));

    assert(res == 0);
    assert(peer.cid_negotiated == 1);
    assert(peer.peer_cid_len == 8);
    assert(memcmp(peer.peer_cid, "\xAB\xCD\xEF\x12\x34\x56\x78\x90", 8) == 0);
}
```

**Test 3: CID Record Format**
```c
void test_cid_record_build() {
    dtls_peer_t peer = {
        .cid_negotiated = 1,
        .peer_cid_len = 8,
        .peer_cid = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22}
    };

    uint8 sendbuf[200];
    uint8 data[] = "Hello";
    int len = dtls_prepare_record_with_cid(&security, DTLS_CT_APPLICATION_DATA,
                                           &peer, data, 5, sendbuf);

    assert(len > 0);
    assert(sendbuf[0] == DTLS_CT_TLS12_CID);  // Content type 25
    // Verify CID is at correct offset
    assert(memcmp(sendbuf + 13, peer.peer_cid, 8) == 0);
}
```

### 6.2 Integration Tests (Application Level)

**Test 1: CID Negotiation with RFC 9146 Server**
```cpp
TEST(CIDIntegration, RFC9146_Negotiation) {
    // Setup
    Connection conn("56830", AF_INET);
    conn.enableCID(true, 8);

    // Connect to server with CID54 support
    auto session = conn.connect(server_uri, security_obj);
    ASSERT_TRUE(session != nullptr);

    // Verify CID negotiated
    EXPECT_TRUE(conn.isCIDNegotiated(session));

    // Verify CID values
    auto our_cid = conn.getOurCID(session);
    auto peer_cid = conn.getPeerCID(session);
    EXPECT_EQ(our_cid.size(), 8);
    EXPECT_EQ(peer_cid.size(), 8);
}
```

**Test 2: Fallback to CID53**
```cpp
TEST(CIDIntegration, CID53_Fallback) {
    // Connect to server that only supports CID53
    Connection conn("56830", AF_INET);
    conn.enableCID(true, 8);

    auto session = conn.connect(cid53_server_uri, security_obj);
    ASSERT_TRUE(session != nullptr);

    // Should still negotiate CID (draft version)
    EXPECT_TRUE(conn.isCIDNegotiated(session));
}
```

**Test 3: NAT Rebinding Survival**
```cpp
TEST(CIDIntegration, NAT_Rebinding) {
    // 1. Establish connection with CID
    Connection conn("56830", AF_INET);
    conn.enableCID(true, 8);
    auto session = conn.connect(server_uri, security_obj);
    ASSERT_TRUE(conn.isCIDNegotiated(session));

    // 2. Send initial message
    conn.send(session, "Hello", 5);
    auto response1 = conn.receive(session);
    EXPECT_TRUE(response1.size() > 0);

    // 3. Simulate NAT rebinding (change source port)
    //    In real test: close socket, rebind to new ephemeral port
    conn.rebindSocket();

    // 4. Send message after rebind
    conn.send(session, "World", 5);
    auto response2 = conn.receive(session);

    // Should succeed without re-handshake
    EXPECT_TRUE(response2.size() > 0);

    // Verify no re-handshake occurred
    EXPECT_EQ(conn.getHandshakeCount(session), 1);
}
```

**Test 4: Backward Compatibility (No CID Server)**
```cpp
TEST(CIDIntegration, NoCI D_Server_Compatibility) {
    Connection conn("56830", AF_INET);
    conn.enableCID(true, 8);

    // Connect to server without CID support
    auto session = conn.connect(legacy_server_uri, security_obj);
    ASSERT_TRUE(session != nullptr);

    // Should work in legacy mode
    EXPECT_FALSE(conn.isCIDNegotiated(session));

    // Verify data transfer works
    conn.send(session, "Test", 4);
    auto response = conn.receive(session);
    EXPECT_TRUE(response.size() > 0);
}
```

### 6.3 Server Testing Matrix

| Server Type | CID Support | Expected Result |
|-------------|-------------|-----------------|
| Leshan (latest) | CID54 | ✅ CID negotiated (RFC 9146) |
| Leshan (old) | CID53 | ✅ CID negotiated (draft) |
| AVSystem Coiote | CID54 | ✅ CID negotiated |
| Eclipse Californium | No CID | ✅ Legacy DTLS works |
| Custom test server | CID54 + CID53 | ✅ Prefers CID54 |

---

## 7. Migration and Deployment

### 7.1 Deployment Phases

**Phase 1: Development (Week 1-2)**
- Implement TinyDTLS modifications
- Add application integration
- Unit tests

**Phase 2: Testing (Week 3)**
- Integration tests with test servers
- NAT rebinding simulation
- Backward compatibility verification

**Phase 3: Controlled Rollout (Week 4)**
- Deploy to 10% of devices (CID enabled)
- Monitor handshake success rate
- Collect CID negotiation metrics

**Phase 4: Full Deployment (Week 5+)**
- Gradual rollout to 100%
- Monitor NAT rebinding events
- Measure handshake reduction

### 7.2 Rollback Plan

**If issues occur:**
1. Disable CID via config: `cmake -DDTLS_CID=0`
2. Or disable at runtime: `--disable-cid`
3. Revert TinyDTLS changes (keep commits atomic)

**Risk Mitigation:**
- Feature flag: `DTLS_CID` can be disabled
- Runtime toggle: `--disable-cid`
- Graceful fallback to legacy DTLS

### 7.3 Monitoring Metrics

**Key Metrics to Track:**
```
- CID negotiation success rate (target: >95%)
- CID version distribution (CID54 vs CID53 vs None)
- NAT rebinding events survived (before: 0%, after: >90%)
- Handshake count per 24h (should decrease significantly)
- DTLS session lifetime (should increase)
- Data transfer success rate (should remain 100%)
```

---

## 8. Risk Analysis

### 8.1 Risk Matrix

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| TinyDTLS modifications break existing DTLS | Medium | High | Comprehensive testing, feature flag |
| Server incompatibility | Low | Medium | Fallback to legacy, test matrix |
| Memory overhead | Low | Low | CID adds only 8 bytes per peer |
| Performance degradation | Low | Low | Minimal crypto overhead (same AEAD) |
| NAT rebinding doesn't work | Medium | High | Extensive testing, real-world trials |

### 8.2 Testing Requirements

**Critical Tests:**
- ✅ Existing DTLS PSK still works
- ✅ Existing DTLS RPK still works
- ✅ CID negotiation with CID54 server
- ✅ CID negotiation with CID53 server
- ✅ Fallback to no CID with legacy server
- ✅ NAT rebinding survival
- ✅ Session persistence across sleep/wake
- ✅ No memory leaks
- ✅ No performance regression

### 8.3 Known Limitations

1. **TinyDTLS Modification Required**
   - Risk: Upstream updates may conflict
   - Mitigation: Maintain patches, contribute upstream

2. **No DTLS 1.3 Support**
   - TinyDTLS only supports DTLS 1.2
   - DTLS 1.3 has native CID support
   - Future migration path: Switch to mbedTLS 3.x

3. **Server Support Variability**
   - Not all servers support CID yet
   - Graceful fallback ensures compatibility

---

## 9. Success Criteria

### 9.1 Functional Requirements

- ✅ CID negotiation with RFC 9146 servers
- ✅ CID negotiation with draft servers
- ✅ Backward compatibility with non-CID servers
- ✅ Session survives NAT rebinding
- ✅ Configurable at compile-time and runtime
- ✅ No regression in existing DTLS functionality

### 9.2 Performance Requirements

- ✅ Handshake latency increase <5%
- ✅ Memory overhead <100 bytes per connection
- ✅ CPU overhead <2% for encryption/decryption
- ✅ NAT rebinding recovery <1 second

### 9.3 Quality Requirements

- ✅ Code coverage >80% for new code
- ✅ All unit tests pass
- ✅ All integration tests pass
- ✅ No compiler warnings
- ✅ Documentation complete

---

## 10. Next Steps

### Immediate Actions

1. **Review this plan** with team
2. **Set up test environment** with CID-capable server (Leshan)
3. **Create feature branch:** `feature/dtls-cid-rfc9146`
4. **Begin implementation:** Start with Step 1 (constants)

### Development Workflow

```
Week 1: TinyDTLS modifications (Steps 1-5)
Week 2: Application integration (Steps 6-7)
Week 3: Testing and validation
Week 4: Documentation and deployment prep
Week 5: Controlled rollout
```

### Questions to Resolve

1. **Persistence Strategy:** Should CID be persisted across device reboots?
2. **Server Testing:** Which LwM2M servers support CID54/CID53?
3. **Metrics Collection:** What telemetry should be added for monitoring?
4. **Upstream Contribution:** Should we contribute CID support to TinyDTLS project?

---

## Appendix A: TinyDTLS CID Extension Example

```c
/* ClientHello with CID extension */
Handshake Type: Client Hello (1)
  Length: 89
  Message Sequence: 0
  Fragment Offset: 0
  Fragment Length: 89
  Version: DTLS 1.2 (0xfefd)
  Random: ...
  Session ID Length: 0
  Cookie Length: 0
  Cipher Suites Length: 4
    Cipher Suite: TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 (0xc0ae)
    Cipher Suite: TLS_PSK_WITH_AES_128_CCM_8 (0xc0a8)
  Compression Methods Length: 1
    Compression Method: null (0)
  Extensions Length: 47
    Extension: elliptic_curves (len=8)
      Elliptic Curves: secp256r1 (0x0017)
    Extension: ec_point_formats (len=2)
      EC Point Format: uncompressed (0)
    Extension: connection_id (len=1)  ← NEW!
      Type: 0x0036 (54, RFC 9146)     ← NEW!
      Length: 1
      CID Length: 8                   ← NEW!

/* ServerHello with CID extension */
Handshake Type: Server Hello (2)
  ...
  Extensions:
    Extension: connection_id (len=9)  ← NEW!
      Type: 0x0036 (54)               ← NEW!
      Length: 9
      CID Length: 8                   ← NEW!
      CID: AB CD EF 12 34 56 78 90    ← NEW!
```

---

## Appendix B: CID Record Format Example

```
Standard DTLS Record (No CID):
+----------------+------------------+
| Content Type   | 23 (app data)    |
| Version        | 0xFEFD           |
| Epoch          | 0x0001           |
| Sequence       | 0x000000000042   |
| Length         | 0x0018           |
| Fragment       | [encrypted data] |
+----------------+------------------+
Total: 13 + length bytes

CID Record (RFC 9146):
+----------------+----------------------+
| Content Type   | 25 (tls12_cid)       | ← Changed!
| Version        | 0xFEFD               |
| Epoch          | 0x0001               |
| Sequence       | 0x000000000042       |
| CID            | AB CD EF 12 34 56... | ← Added!
| Length         | 0x0019               |
| Fragment       | [encrypted data]     |
|                | [inner content type] | ← Added!
+----------------+----------------------+
Total: 13 + CID_len + length bytes
```

---

## Appendix C: AAD Construction Differences

```c
/* AAD without CID (standard DTLS 1.2 with AEAD) */
AAD = epoch (2) || seq_num (6) || type (1) || version (2) || length (2)
    = 13 bytes

/* AAD with CID (RFC 9146 Section 5) */
AAD = epoch (2) || seq_num (6) || tls12_cid (1) || cid_len (1) ||
      cid (var) || inner_type (1) || version (2) || length (2)
    = 15 + CID_len bytes
```

**Key Difference:** Inner content type is part of AAD, not plaintext.

---

**Document Version:** 1.0
**Last Updated:** 2026-04-12
**Author:** Implementation Plan based on RFC 9146 and TinyDTLS 0.8.6 analysis
**Status:** Ready for Development

---

## Quick Start Checklist

- [ ] Review this plan with development team
- [ ] Set up test server with CID support (Leshan)
- [ ] Create feature branch `feature/dtls-cid-rfc9146`
- [ ] Implement Step 1: Add constants to `global.h`
- [ ] Implement Step 2: Extend `dtls_peer_t` structure
- [ ] Implement Step 3: Add config to `dtls_config.h`
- [ ] Implement Step 4: Extension handling in `dtls.c`
- [ ] Implement Step 5: Record format in `dtls.c`
- [ ] Implement Step 6: Application integration
- [ ] Implement Step 7: CLI options
- [ ] Write unit tests
- [ ] Write integration tests
- [ ] Test with RFC 9146 server
- [ ] Test with draft server
- [ ] Test backward compatibility
- [ ] Document changes
- [ ] Create pull request
- [ ] Code review
- [ ] Merge to main
- [ ] Deploy to test devices

**Ready to implement!** 🚀
