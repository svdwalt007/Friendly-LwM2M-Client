# DTLS CID Implementation - Quick Start Guide

## For Developers Starting CID Implementation

### Prerequisites

✅ Reviewed `CID_IMPLEMENTATION_PLAN.md` (comprehensive technical plan)
✅ Reviewed `CID_IMPLEMENTATION_SUMMARY.md` (executive overview)
✅ Reviewed `PROMPT_LwM2M_Client_CID_Implementation.md` (requirements)
✅ Have development environment set up
✅ Have test LwM2M server with CID support (e.g., Leshan)

---

## Step-by-Step Implementation Checklist

### Phase 1: TinyDTLS Core (Week 1)

#### Day 1-2: Constants and Configuration

- [ ] **File:** `2305-Wakaama/examples/shared/tinydtls/global.h`
  ```c
  // Add after line 102:
  #define TLS_EXT_CID_DRAFT       53
  #define TLS_EXT_CID_RFC9146     54
  #define DTLS_CT_TLS12_CID       25
  #define DTLS_CID_MAX_SIZE       8
  ```

- [ ] **File:** `2305-Wakaama/examples/shared/tinydtls/dtls_config.h`
  ```c
  // Add after line 50:
  #define DTLS_CID                    1
  #define DTLS_CID_PREFER_RFC9146     1
  #define DTLS_CID_FALLBACK_DRAFT     1
  #define DTLS_CID_DEFAULT_LENGTH     8
  ```

- [ ] **Test:** Compile and verify no errors

#### Day 3-4: Peer Structure Extension

- [ ] **File:** `2305-Wakaama/examples/shared/tinydtls/peer.h`
  - Add CID fields to `dtls_peer_t` (see plan section 3.2)
  - Add `dtls_get_peer_by_cid()` declaration

- [ ] **File:** `2305-Wakaama/examples/shared/tinydtls/peer.c`
  - Initialize CID fields in `dtls_new_peer()`
  - Implement `dtls_get_peer_by_cid()` function

- [ ] **Test:** Compile and verify structure size changes

#### Day 5-7: Extension Handling

- [ ] **File:** `2305-Wakaama/examples/shared/tinydtls/dtls.c`

  **Task 1:** Add CID extension to ClientHello
  - Implement `dtls_add_cid_extension()` function
  - Modify `dtls_send_client_hello()` to call it

  **Task 2:** Parse CID extension from ServerHello
  - Modify `check_server_hello()` extension parsing loop
  - Add CID53 and CID54 case handlers
  - Store negotiated CIDs in peer structure

  **Task 3:** Generate our CID after negotiation
  - Use `dtls_prng()` for random CID generation
  - Store in `peer->our_cid`

- [ ] **Test:** Unit test extension encoding/decoding

---

### Phase 2: Record Layer (Week 2)

#### Day 8-10: CID Record Sending

- [ ] **File:** `2305-Wakaama/examples/shared/tinydtls/dtls.c`

  **Task 1:** Implement `dtls_prepare_record_with_cid()`
  - Build record header with content type 25
  - Insert peer CID after sequence number
  - Build inner plaintext (data + inner_content_type)
  - Construct AAD with CID
  - Encrypt using AEAD

  **Task 2:** Modify `dtls_prepare_record()`
  - Check `peer->cid_negotiated`
  - Call CID variant if negotiated
  - Fall back to legacy if not

- [ ] **Test:** Verify record structure with Wireshark

#### Day 11-12: CID Record Receiving

- [ ] **File:** `2305-Wakaama/examples/shared/tinydtls/dtls.c`

  **Task 1:** Detect CID records
  - Check for content type 25 in `dtls_handle_message()`

  **Task 2:** Parse CID record
  - Extract CID from record
  - Lookup peer by CID (not 5-tuple)
  - Verify CID matches `peer->our_cid`

  **Task 3:** Decrypt CID record
  - Build AAD with CID
  - Decrypt ciphertext
  - Extract inner content type (last byte)
  - Process decrypted payload

- [ ] **Test:** Send/receive CID records with test server

#### Day 13-14: AAD Construction

- [ ] **File:** `2305-Wakaama/examples/shared/tinydtls/crypto.c`

  **Task:** Modify AAD building for CID records
  - Standard AAD: 13 bytes
  - CID AAD: 15 + cid_len bytes
  - Include: epoch, seq, type=25, cid_len, cid, inner_type, version, length

- [ ] **Test:** Verify encryption/decryption works

---

### Phase 3: Application Integration (Week 3)

#### Day 15-16: Connection API

- [ ] **File:** `examples/platform/connection_dtls/Connection.h`
  ```cpp
  void enableCID(bool enable, uint8_t cid_length = 8);
  bool isCIDNegotiated(const dtls_connection_t *conn) const;
  std::vector<uint8_t> getOurCID(const dtls_connection_t *conn) const;
  std::vector<uint8_t> getPeerCID(const dtls_connection_t *conn) const;
  ```

- [ ] **File:** `examples/platform/connection_dtls/Connection.cpp`
  - Implement all CID API methods
  - Add CID status logging after handshake

- [ ] **Test:** Verify API returns correct values

#### Day 17: CLI Integration

- [ ] **File:** `examples/cli_options.h`
  - Add `enable_cid`, `prefer_cid54`, `cid_length` fields
  - Add long options: `--enable-cid`, `--disable-cid`, `--cid-length`, `--prefer-cid53`
  - Add help text

- [ ] **File:** `examples/main.cpp`
  - Pass CID config to Connection
  - Display CID status in verbose output

- [ ] **Test:** `./WppExample --help` shows CID options

---

### Phase 4: Testing (Week 3-4)

#### Unit Tests

- [ ] **Extension Encoding**
  ```c
  test_cid_extension_encode()
  test_cid_extension_parse()
  ```

- [ ] **Record Format**
  ```c
  test_cid_record_build()
  test_cid_record_parse()
  ```

- [ ] **AAD Construction**
  ```c
  test_aad_with_cid()
  test_aad_without_cid()
  ```

- [ ] **Peer Lookup**
  ```c
  test_peer_lookup_by_cid()
  test_peer_lookup_fallback()
  ```

#### Integration Tests

- [ ] **CID54 Negotiation**
  - Connect to Leshan with CID enabled
  - Verify CID negotiated
  - Send/receive data

- [ ] **CID53 Fallback**
  - Connect to CID53-only server
  - Verify fallback works

- [ ] **NAT Rebinding**
  - Establish connection
  - Change source port
  - Verify session continues

- [ ] **Backward Compatibility**
  - Connect to non-CID server
  - Verify legacy DTLS works

#### Performance Tests

- [ ] **Handshake Latency**
  - Measure with and without CID
  - Target: <5% increase

- [ ] **Memory Overhead**
  - Measure peer structure size
  - Target: <100 bytes per peer

- [ ] **CPU Usage**
  - Profile encryption/decryption
  - Target: <2% increase

---

## Build and Test Commands

### Build with CID

```bash
cd _build
cmake ../examples -DDTLS_CID=1 -DDTLS_CID_PREFER_RFC9146=1
make -j$(nproc)
```

### Build without CID

```bash
cmake ../examples -DDTLS_CID=0
make -j$(nproc)
```

### Run with CID Enabled

```bash
./WppExample --enable-cid --cid-length=8 -v
```

### Run with CID Disabled

```bash
./WppExample --disable-cid -v
```

### Test with Leshan Server

```bash
# Start Leshan server (with CID support)
java -jar leshan-server-demo.jar -wp 8080 -lp 5683

# Connect client
./WppExample \
    -n testdevice \
    -u coap://localhost:5683 \
    --enable-cid \
    -v
```

---

## Debugging Tips

### Enable DTLS Debug Logs

**File:** `2305-Wakaama/examples/shared/tinydtls/dtls_config.h`
```c
#define DTLS_DEBUG 1
```

### Check CID Negotiation

Look for log messages:
```
Added CID extension (type 54, length 8)
Server sent CID extension (type 54)
CID negotiated: version=RFC9146, our_len=8, peer_len=8
✅ CID negotiated!
   Our CID:  AABBCCDDEEFF1122
   Peer CID: 1122334455667788
```

### Verify with Wireshark

**Filter:** `dtls && dtls.record.content_type == 25`

**Expected:**
- Content Type: `tls12_cid (25)`
- CID visible after sequence number
- Length includes encrypted data + auth tag

### Common Issues

**Issue:** "CID extension not sent"
- Check `DTLS_CID` is defined to 1
- Verify `conn.enableCID(true)` is called

**Issue:** "Server rejected CID"
- Try fallback to CID53: `--prefer-cid53`
- Check server CID support

**Issue:** "CID mismatch on receive"
- Verify `peer->our_cid` was generated
- Check CID extraction offset in record

**Issue:** "Decryption failed"
- Verify AAD construction includes CID
- Check inner content type is appended correctly

---

## Code Review Checklist

Before submitting PR:

- [ ] All compiler warnings resolved
- [ ] Code follows existing TinyDTLS style
- [ ] Comments explain CID-specific logic
- [ ] `#if DTLS_CID` guards all new code
- [ ] No hardcoded magic numbers (use defines)
- [ ] Error handling for all edge cases
- [ ] Memory properly initialized/freed
- [ ] Unit tests written and passing
- [ ] Integration tests passing
- [ ] Documentation updated
- [ ] CHANGELOG entry added

---

## Resources

### Documentation
- **Full Plan:** `CID_IMPLEMENTATION_PLAN.md`
- **Summary:** `CID_IMPLEMENTATION_SUMMARY.md`
- **Requirements:** `PROMPT_LwM2M_Client_CID_Implementation.md`

### RFCs
- **RFC 9146:** https://www.rfc-editor.org/rfc/rfc9146.html (CID final)
- **RFC 6347:** https://www.rfc-editor.org/rfc/rfc6347.html (DTLS 1.2)
- **Draft CID:** https://datatracker.ietf.org/doc/html/draft-ietf-tls-dtls-connection-id

### Tools
- **Leshan Server:** https://github.com/eclipse/leshan
- **Wireshark:** For DTLS packet analysis
- **Valgrind:** For memory leak detection

### TinyDTLS Files
- `dtls.c` - Main state machine
- `peer.h/c` - Session management
- `crypto.h/c` - Encryption/decryption
- `global.h` - Constants and types

---

## Success Indicators

You're on track if you see:

✅ Code compiles with `DTLS_CID=1`
✅ Code compiles with `DTLS_CID=0` (backward compat)
✅ ClientHello contains CID extension (Wireshark)
✅ ServerHello contains CID extension (Wireshark)
✅ Records have content type 25 (Wireshark)
✅ CID visible in record header (Wireshark)
✅ Data transfers successfully
✅ NAT rebinding doesn't break session
✅ Legacy servers still work

---

## Getting Help

**Stuck?** Check:
1. Implementation plan section for that step
2. TinyDTLS existing code for similar functionality
3. RFC 9146 specification
4. Wireshark captures for protocol issues

**Questions?** Contact:
- Implementation team
- LwM2M community forums
- TinyDTLS mailing list

---

**Good luck! 🚀**

Start with Phase 1, Day 1-2 (constants and configuration).
Small, incremental changes are easier to debug!
