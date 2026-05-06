# Claude Code Prompt: DTLS Connection ID (CID) Implementation

## Project Context

**Codebase:** Friendly LwM2M Client
**Language:** C++17/20
**Platform:** Embedded (resource-constrained devices, NB-IoT/LTE-M modems)
**RTOS:** Zephyr RTOS (primary), also bare-metal and FreeRTOS variants
**DTLS Library:** Likely mbedTLS or tinydtls (verify in codebase)
**Standards:** OMA LwM2M 1.0/1.1/1.2, RFC 6347 (DTLS 1.2), RFC 9146 (CID)

## Objective

Implement DTLS Connection ID (CID) support in the Friendly LwM2M Client to enable:
1. Session persistence across NAT rebinding events
2. Reduced handshake overhead for sleepy/battery-powered devices
3. Compatibility with servers supporting CID Option 53 (draft) and CID Option 54 (RFC 9146 final)

## Background: Why CID Matters

```
WITHOUT CID:
┌─────────────────────────────────────────────────────────────────┐
│ Device sends packet from 192.168.1.100:45678                    │
│ Server stores session keyed to (192.168.1.100, 45678)           │
│                                                                 │
│ Device sleeps for 4 hours...                                    │
│ NAT binding expires                                             │
│                                                                 │
│ Device wakes, new source port 192.168.1.100:52341               │
│ Server cannot correlate → DTLS Alert → Full re-handshake       │
└─────────────────────────────────────────────────────────────────┘

WITH CID:
┌─────────────────────────────────────────────────────────────────┐
│ Device sends packet with CID=0xABCD1234                         │
│ Server stores session keyed to CID, not 5-tuple                 │
│                                                                 │
│ Device sleeps for 4 hours...                                    │
│ NAT binding expires                                             │
│                                                                 │
│ Device wakes, different source port but same CID=0xABCD1234     │
│ Server demuxes on CID → Session found → Continue without HS    │
└─────────────────────────────────────────────────────────────────┘
```

## Implementation Requirements

### 1. CID Extension Negotiation (Handshake)

Both CID53 and CID54 use the same extension structure but different extension type values:

```cpp
// Extension type identifiers
constexpr uint16_t DTLS_EXT_CID_DRAFT = 53;   // draft-ietf-tls-dtls-connection-id
constexpr uint16_t DTLS_EXT_CID_RFC9146 = 54; // RFC 9146 (final)

// CID extension structure in ClientHello/ServerHello
struct CidExtension {
    uint8_t cid_length;      // Length of CID we want to RECEIVE (0-255)
    uint8_t cid[];           // Our CID value (if cid_length > 0 in ServerHello)
};
```

**ClientHello behavior:**
```cpp
// Client indicates the CID length it wants to RECEIVE from server
// Client does NOT include its own CID yet - that comes in ServerHello response
void buildClientHelloCidExtension(Buffer& ext, uint8_t desired_cid_length) {
    ext.writeUint16(config.use_cid_draft ? DTLS_EXT_CID_DRAFT : DTLS_EXT_CID_RFC9146);
    ext.writeUint16(1);  // Extension data length
    ext.writeUint8(desired_cid_length);  // CID length we want to receive
}
```

**ServerHello processing:**
```cpp
// Server response contains:
// - Length of CID server wants to receive from us
// - The actual CID value we should use in our records
void processServerHelloCidExtension(const Buffer& ext) {
    uint8_t server_wants_cid_length = ext.readUint8();
    
    if (server_wants_cid_length > 0) {
        // Read the CID we must include in our records TO the server
        our_cid_for_server.resize(server_wants_cid_length);
        ext.read(our_cid_for_server.data(), server_wants_cid_length);
        cid_negotiated = true;
    }
    
    // Note: Server also sends extension with CID length it will use for us
    // We generate/store that CID for demuxing incoming records
}
```

### 2. Record Layer Modifications

**CID Record Header (RFC 9146 content type = 25):**

```cpp
// Standard DTLS 1.2 record header (no CID)
struct DTLSRecordHeader {
    uint8_t  content_type;     // 20-25
    uint16_t version;          // 0xFEFD for DTLS 1.2
    uint16_t epoch;
    uint8_t  sequence_number[6];
    uint16_t length;
};

// DTLS 1.2 record header WITH CID (content_type = 25 = tls12_cid)
struct DTLSRecordHeaderWithCID {
    uint8_t  content_type;     // 25 (tls12_cid) - FIXED
    uint16_t version;          // 0xFEFD
    uint16_t epoch;
    uint8_t  sequence_number[6];
    uint8_t  cid[cid_length];  // Variable length, negotiated
    uint16_t length;
    // Followed by: encrypted { real_content_type (1 byte) + payload + padding + MAC }
};
```

**Sending records with CID:**
```cpp
void sendRecordWithCID(ContentType inner_type, const uint8_t* data, size_t len) {
    Buffer record;
    
    // Outer content type is always tls12_cid (25) when CID is active
    record.writeUint8(ContentType::TLS12_CID);  // 25
    record.writeUint16(DTLS_VERSION_1_2);       // 0xFEFD
    record.writeUint16(current_epoch);
    record.write(sequence_number, 6);
    record.write(peer_cid.data(), peer_cid.size());  // CID for server
    
    // Build inner plaintext: real_content_type + data
    Buffer plaintext;
    plaintext.write(data, len);
    plaintext.writeUint8(static_cast<uint8_t>(inner_type));  // Real type at END
    
    // Encrypt (inner content type is part of AAD for AEAD ciphers)
    Buffer ciphertext = encrypt(plaintext, buildAAD(inner_type));
    
    record.writeUint16(ciphertext.size());
    record.write(ciphertext);
    
    transport->send(record);
    incrementSequenceNumber();
}
```

**Receiving records with CID:**
```cpp
ContentType receiveRecordWithCID(const Buffer& record, Buffer& plaintext) {
    uint8_t outer_type = record.readUint8();
    
    if (outer_type == ContentType::TLS12_CID) {
        // CID record - extract CID and verify it's ours
        record.skip(2 + 2 + 6);  // version, epoch, sequence
        
        uint8_t received_cid[our_cid.size()];
        record.read(received_cid, our_cid.size());
        
        if (memcmp(received_cid, our_cid.data(), our_cid.size()) != 0) {
            return ContentType::INVALID;  // CID mismatch - drop
        }
        
        uint16_t length = record.readUint16();
        Buffer ciphertext(length);
        record.read(ciphertext.data(), length);
        
        // Decrypt
        Buffer decrypted = decrypt(ciphertext, buildAAD(/* from outer header */));
        
        // Real content type is LAST byte of decrypted data
        ContentType inner_type = static_cast<ContentType>(
            decrypted[decrypted.size() - 1]
        );
        
        // Remove inner content type byte
        plaintext.write(decrypted.data(), decrypted.size() - 1);
        
        return inner_type;
    } else {
        // Legacy record without CID
        return processLegacyRecord(record, plaintext);
    }
}
```

### 3. Configuration Interface

```cpp
struct DtlsCidConfig {
    bool enabled = true;                    // Enable CID negotiation
    bool prefer_cid54 = true;               // Prefer RFC 9146 over draft
    bool fallback_to_cid53 = true;          // Try CID53 if server rejects CID54
    uint8_t cid_length = 8;                 // CID length we want to receive (1-8 typical)
    bool generate_random_cid = true;        // Random vs sequential CID generation
    
    // For devices that persist CID across reboots
    std::optional<std::vector<uint8_t>> persisted_cid;
};

class DtlsSession {
public:
    void configureCID(const DtlsCidConfig& config);
    
    bool isCIDNegotiated() const;
    std::vector<uint8_t> getOurCID() const;     // CID we use in incoming demux
    std::vector<uint8_t> getPeerCID() const;    // CID we send to server
    
    // For persistence across sleep cycles
    void persistCIDState(NonVolatileStorage& nvs);
    bool restoreCIDState(const NonVolatileStorage& nvs);
};
```

### 4. Server Compatibility Detection

Some servers support CID53 (draft), some support CID54 (RFC 9146), some support both, some support neither. Implement graceful negotiation:

```cpp
enum class CIDNegotiationResult {
    CID54_NEGOTIATED,      // RFC 9146 - preferred
    CID53_NEGOTIATED,      // Draft - acceptable
    CID_NOT_SUPPORTED,     // Server doesn't support CID
    CID_REJECTED,          // Server explicitly rejected
    HANDSHAKE_FAILED       // Unrelated failure
};

CIDNegotiationResult negotiateCID() {
    // Strategy 1: Try CID54 first (if preferred)
    if (config.prefer_cid54) {
        auto result = attemptHandshakeWithExtension(DTLS_EXT_CID_RFC9146);
        if (result == SUCCESS) return CID54_NEGOTIATED;
        
        // If server sent Alert or ignored extension, try CID53
        if (config.fallback_to_cid53) {
            result = attemptHandshakeWithExtension(DTLS_EXT_CID_DRAFT);
            if (result == SUCCESS) return CID53_NEGOTIATED;
        }
    }
    
    // Strategy 2: Try CID53 first (for legacy server compatibility)
    else {
        auto result = attemptHandshakeWithExtension(DTLS_EXT_CID_DRAFT);
        if (result == SUCCESS) return CID53_NEGOTIATED;
    }
    
    // Fallback: Proceed without CID
    return performHandshakeWithoutCID();
}
```

### 5. Integration Points

Identify and modify these components in the existing codebase:

```
┌─────────────────────────────────────────────────────────────────┐
│                    INTEGRATION CHECKLIST                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  [ ] DTLS Handshake Module                                      │
│      • Add CID extension building in ClientHello                │
│      • Parse CID extension from ServerHello                     │
│      • Store negotiated CIDs in session state                   │
│                                                                 │
│  [ ] DTLS Record Layer                                          │
│      • Modify record header construction for CID                │
│      • Add content_type 25 (tls12_cid) handling                 │
│      • Update AEAD AAD construction to include CID              │
│      • Handle inner content type extraction                     │
│                                                                 │
│  [ ] Session State Management                                   │
│      • Add CID fields to session structure                      │
│      • Implement CID persistence for sleep/wake                 │
│      • Update session serialization/deserialization             │
│                                                                 │
│  [ ] Configuration/API                                          │
│      • Add CID config options to client config                  │
│      • Expose CID status via API                                │
│      • Add CID-related error codes                              │
│                                                                 │
│  [ ] Demultiplexing (if client handles multiple sessions)       │
│      • Add CID-based session lookup                             │
│      • Fallback to 5-tuple for non-CID records                  │
│                                                                 │
│  [ ] Testing                                                    │
│      • Unit tests for CID extension encoding/decoding           │
│      • Integration tests with CID53/CID54 servers               │
│      • NAT rebinding simulation tests                           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 6. AAD (Additional Authenticated Data) Construction

For AEAD ciphers (AES-CCM, AES-GCM), the AAD must include CID when present:

```cpp
// RFC 9146 Section 5: AAD for records with CID
Buffer buildAADWithCID(uint16_t epoch, 
                       const uint8_t* seq_num,  // 6 bytes
                       ContentType inner_type,
                       uint16_t length_of_inner_plaintext) {
    Buffer aad;
    
    // For CID records, AAD includes:
    // - 8-byte sequence number (epoch + seq)
    // - tls12_cid content type (25)
    // - CID length + CID
    // - Inner content type
    // - Protocol version
    // - Length of inner plaintext (before padding)
    
    aad.writeUint16(epoch);
    aad.write(seq_num, 6);
    aad.writeUint8(ContentType::TLS12_CID);  // 25
    aad.writeUint8(peer_cid.size());
    aad.write(peer_cid.data(), peer_cid.size());
    aad.writeUint8(static_cast<uint8_t>(inner_type));
    aad.writeUint16(DTLS_VERSION_1_2);
    aad.writeUint16(length_of_inner_plaintext);
    
    return aad;
}
```

### 7. CID Generation

```cpp
class CIDGenerator {
public:
    // Random CID (recommended for security)
    static std::vector<uint8_t> generateRandom(size_t length) {
        std::vector<uint8_t> cid(length);
        hardware_random(cid.data(), length);  // Use HW RNG
        return cid;
    }
    
    // Deterministic CID from device identity (useful for debugging)
    static std::vector<uint8_t> generateFromIMEI(const std::string& imei, 
                                                   size_t length) {
        // Hash IMEI and take first N bytes
        auto hash = sha256(imei);
        return std::vector<uint8_t>(hash.begin(), hash.begin() + length);
    }
    
    // Sequential CID with device prefix (for multi-session devices)
    static std::vector<uint8_t> generateSequential(uint16_t device_id,
                                                    uint16_t session_counter,
                                                    size_t length) {
        std::vector<uint8_t> cid(length);
        // First 2 bytes: device ID, next 2: session counter, rest: random
        cid[0] = device_id >> 8;
        cid[1] = device_id & 0xFF;
        cid[2] = session_counter >> 8;
        cid[3] = session_counter & 0xFF;
        if (length > 4) {
            hardware_random(cid.data() + 4, length - 4);
        }
        return cid;
    }
};
```

### 8. Error Handling

```cpp
enum class CIDError {
    NONE,
    EXTENSION_ENCODING_FAILED,
    EXTENSION_DECODING_FAILED,
    SERVER_REJECTED_CID,
    CID_LENGTH_MISMATCH,
    CID_VERIFICATION_FAILED,
    RECORD_DECRYPTION_FAILED_BAD_CID,
    PERSISTENCE_FAILED
};

// Report CID-specific errors via existing error callback mechanism
void onCIDError(CIDError error, const char* details);
```

## Testing Strategy

### Unit Tests

```cpp
TEST(CIDExtension, EncodeClientHello) {
    Buffer ext;
    buildClientHelloCidExtension(ext, 8, DTLS_EXT_CID_RFC9146);
    EXPECT_EQ(ext.size(), 5);  // type(2) + length(2) + cid_length(1)
    EXPECT_EQ(ext[0], 0x00); EXPECT_EQ(ext[1], 0x36);  // 54 = CID RFC9146
    EXPECT_EQ(ext[4], 0x08);  // requesting 8-byte CID
}

TEST(CIDRecord, InnerContentTypeExtraction) {
    // Verify inner content type is correctly extracted from decrypted payload
    Buffer decrypted = {0x01, 0x02, 0x03, 0x17};  // Data + type 23 (app_data)
    auto type = extractInnerContentType(decrypted);
    EXPECT_EQ(type, ContentType::APPLICATION_DATA);
    EXPECT_EQ(decrypted.size(), 3);  // Type byte removed
}
```

### Integration Tests

```cpp
TEST(CIDIntegration, NATRebindingSimulation) {
    // 1. Establish DTLS session with CID
    auto session = establishSession(CID_ENABLED);
    ASSERT_TRUE(session.isCIDNegotiated());
    
    // 2. Send initial message
    session.send("Hello");
    EXPECT_TRUE(receiveAck());
    
    // 3. Simulate NAT rebinding (change source port)
    transport.rebindToNewPort();
    
    // 4. Send message on new port with same CID
    session.send("World");
    EXPECT_TRUE(receiveAck());  // Should succeed - server uses CID
    
    // 5. Verify no re-handshake occurred
    EXPECT_EQ(session.getHandshakeCount(), 1);
}
```

## Files to Examine/Modify

Based on typical LwM2M client architecture, examine these areas:

```
src/
├── dtls/
│   ├── dtls_session.cpp          # Session state - ADD CID fields
│   ├── dtls_handshake.cpp        # Handshake - ADD CID extension handling
│   ├── dtls_record.cpp           # Record layer - ADD CID record format
│   ├── dtls_crypto.cpp           # Crypto - MODIFY AAD construction
│   └── dtls_extensions.h         # Extensions - ADD CID extension types
├── transport/
│   ├── coap_transport.cpp        # May need CID-aware demuxing
│   └── udp_socket.cpp            # No changes expected
├── config/
│   └── client_config.h           # ADD CID configuration options
└── persist/
    └── session_store.cpp         # ADD CID persistence
```

## Standards References

- **RFC 9146**: Connection Identifier for DTLS 1.2 (final standard)
  - https://www.rfc-editor.org/rfc/rfc9146.html
  
- **draft-ietf-tls-dtls-connection-id**: Draft specification (CID 53)
  - https://datatracker.ietf.org/doc/html/draft-ietf-tls-dtls-connection-id
  
- **RFC 6347**: DTLS 1.2
  - https://www.rfc-editor.org/rfc/rfc6347.html

## Success Criteria

1. **CID54 Negotiation**: Client successfully negotiates CID with RFC 9146 compliant servers
2. **CID53 Fallback**: Client falls back to draft CID when server only supports CID53
3. **NAT Traversal**: Session survives NAT rebinding without re-handshake
4. **Backward Compatibility**: Client works with servers that don't support CID
5. **Persistence**: CID state persists across device sleep/wake cycles
6. **No Regression**: All existing DTLS functionality continues to work

## Notes for Implementation

1. **mbedTLS**: As of v3.x, mbedTLS has CID support but may need configuration. Check `MBEDTLS_SSL_DTLS_CONNECTION_ID` config option.

2. **tinydtls**: May require more extensive modifications as CID support is limited.

3. **Memory Constraints**: CID adds 1-8 bytes per record. Ensure buffer sizes accommodate this.

4. **Sequence Number**: CID doesn't change sequence number handling - these are independent.

5. **Session Resumption**: CID is orthogonal to session resumption. Both can be used together.

---

## Start Here

1. First, identify the DTLS library used in the codebase
2. Check if the library has native CID support that can be enabled
3. If not, implement CID extension handling in the handshake module
4. Then modify the record layer for CID record format
5. Add configuration options and persistence
6. Test with Leshan or Friendly server configured for CID53/CID54
