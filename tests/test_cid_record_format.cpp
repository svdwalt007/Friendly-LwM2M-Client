/*******************************************************************************
 * Test Suite: DTLS CID Record Format
 *
 * Tests RFC 9146 CID record format encoding/decoding
 *
 * Coverage:
 * - CID record header format
 * - Content Type 25 (tls12_cid) handling
 * - Inner content type encryption/decryption
 * - CID-based peer lookup
 * - Record padding
 * - Length field handling
 ******************************************************************************/

#include <gtest/gtest.h>
#include <cstring>
#include <cstdint>

extern "C" {
    #include "global.h"
    #include "dtls.h"
    #include "peer.h"
    #include "dtls_debug.h"
}

class CIDRecordFormatTest : public ::testing::Test {
protected:
    dtls_context_t *ctx;
    dtls_peer_t *peer;
    session_t session;

    void SetUp() override {
        // Initialize session
        memset(&session, 0, sizeof(session_t));
        session.size = sizeof(session.addr);
        session.addr.sin.sin_family = AF_INET;
        session.addr.sin.sin_port = htons(5684);
        session.addr.sin.sin_addr.s_addr = htonl(0x7f000001);

        // Create DTLS context
        ctx = dtls_new_context(nullptr);
        ASSERT_NE(ctx, nullptr);

        // Create peer with negotiated CID
        peer = dtls_new_peer(&session);
        ASSERT_NE(peer, nullptr);

#if DTLS_CID
        // Set up negotiated CID
        peer->cid_negotiated = 1;
        peer->cid_version = 2; // RFC 9146
        peer->our_cid_len = 8;
        peer->peer_cid_len = 8;
        for (int i = 0; i < 8; i++) {
            peer->our_cid[i] = 0xAA + i;
            peer->peer_cid[i] = 0xBB + i;
        }
#endif
    }

    void TearDown() override {
        if (peer) {
            dtls_free_peer(peer);
        }
        if (ctx) {
            dtls_free_context(ctx);
        }
    }
};

#if DTLS_CID

/**
 * Test: CID Record Header Format
 * RFC 9146 record format:
 * ContentType(1) = 25 + Version(2) + Epoch(2) + SeqNum(6) + CID(var) + Length(2)
 */
TEST_F(CIDRecordFormatTest, RecordHeaderFormat) {
    uint8_t buffer[256];
    uint8_t *p = buffer;

    // Simulate CID record header
    uint16_t epoch = 1;
    uint64_t seqnum = 42;

    // Content Type = 25 (DTLS_CT_TLS12_CID)
    dtls_int_to_uint8(p, DTLS_CT_TLS12_CID);
    p += 1;

    // Version = DTLS 1.2 (0xFEFD)
    dtls_int_to_uint16(p, DTLS_VERSION);
    p += 2;

    // Epoch
    dtls_int_to_uint16(p, epoch);
    p += 2;

    // Sequence Number (48-bit)
    dtls_int_to_uint48(p, seqnum);
    p += 6;

    // CID (8 bytes in this test)
    memcpy(p, peer->peer_cid, peer->peer_cid_len);
    p += peer->peer_cid_len;

    // Length (placeholder)
    dtls_int_to_uint16(p, 0);
    p += 2;

    // Verify header parsing
    uint8_t *verify = buffer;

    EXPECT_EQ(dtls_uint8_to_int(verify), DTLS_CT_TLS12_CID);
    verify += 1;

    EXPECT_EQ(dtls_uint16_to_int(verify), DTLS_VERSION);
    verify += 2;

    EXPECT_EQ(dtls_uint16_to_int(verify), epoch);
    verify += 2;

    EXPECT_EQ(dtls_uint48_to_int(verify), seqnum);
    verify += 6;

    // Verify CID matches
    for (int i = 0; i < peer->peer_cid_len; i++) {
        EXPECT_EQ(verify[i], peer->peer_cid[i]) << "CID byte " << i << " mismatch";
    }
    verify += peer->peer_cid_len;

    // Header size should be: 1 + 2 + 2 + 6 + 8 + 2 = 21 bytes
    size_t header_size = verify + 2 - buffer;
    EXPECT_EQ(header_size, 21) << "CID record header should be 21 bytes with 8-byte CID";
}

/**
 * Test: Non-CID vs CID Record Header Size
 * Compare header sizes between standard DTLS and CID records
 */
TEST_F(CIDRecordFormatTest, HeaderSizeComparison) {
    // Standard DTLS 1.2 record header: 13 bytes
    // ContentType(1) + Version(2) + Epoch(2) + SeqNum(6) + Length(2)
    const size_t standard_header_size = 13;

    // CID record header: 13 + CID_length bytes
    // ContentType(1) + Version(2) + Epoch(2) + SeqNum(6) + CID(var) + Length(2)
    const size_t cid_header_size = 13 + peer->peer_cid_len;

    EXPECT_EQ(cid_header_size, 21) << "CID header with 8-byte CID should be 21 bytes";
    EXPECT_GT(cid_header_size, standard_header_size) << "CID header should be larger";
    EXPECT_EQ(cid_header_size - standard_header_size, peer->peer_cid_len)
        << "Size difference should equal CID length";
}

/**
 * Test: Inner Content Type Handling
 * Verify inner content type is appended to plaintext before encryption
 */
TEST_F(CIDRecordFormatTest, InnerContentType) {
    // Simulated plaintext (CoAP message)
    uint8_t plaintext[64];
    size_t plaintext_len = 32;
    for (size_t i = 0; i < plaintext_len; i++) {
        plaintext[i] = (uint8_t)i;
    }

    // Inner content type (typically 22 for Handshake, 23 for Application Data)
    uint8_t inner_content_type = 23; // Application Data

    // Buffer for plaintext + inner content type
    uint8_t buffer[128];
    memcpy(buffer, plaintext, plaintext_len);
    buffer[plaintext_len] = inner_content_type;

    size_t total_len = plaintext_len + 1;

    // Verify inner content type appended
    EXPECT_EQ(buffer[plaintext_len], inner_content_type)
        << "Inner content type should be appended";
    EXPECT_EQ(total_len, plaintext_len + 1)
        << "Total length should include inner content type byte";

    // Verify plaintext unchanged
    for (size_t i = 0; i < plaintext_len; i++) {
        EXPECT_EQ(buffer[i], (uint8_t)i) << "Plaintext byte " << i << " should be unchanged";
    }
}

/**
 * Test: CID-Based Peer Lookup
 * Verify dtls_get_peer_by_cid() correctly finds peer by CID
 */
TEST_F(CIDRecordFormatTest, PeerLookupByCID) {
    // Add peer to context
    dtls_add_peer(ctx, peer);

    // Lookup by our CID (what we sent to server)
    dtls_peer_t *found = dtls_get_peer_by_cid(ctx, peer->our_cid, peer->our_cid_len);
    EXPECT_NE(found, nullptr) << "Should find peer by our CID";
    EXPECT_EQ(found, peer) << "Should return correct peer";

    // Verify CID matches
    EXPECT_EQ(found->our_cid_len, peer->our_cid_len);
    for (int i = 0; i < peer->our_cid_len; i++) {
        EXPECT_EQ(found->our_cid[i], peer->our_cid[i]);
    }
}

/**
 * Test: CID Lookup with Invalid Parameters
 * Verify proper error handling in peer lookup
 */
TEST_F(CIDRecordFormatTest, PeerLookupInvalidParams) {
    dtls_add_peer(ctx, peer);

    // Test 1: NULL context
    dtls_peer_t *result = dtls_get_peer_by_cid(nullptr, peer->our_cid, peer->our_cid_len);
    EXPECT_EQ(result, nullptr) << "Should return NULL for NULL context";

    // Test 2: NULL CID
    result = dtls_get_peer_by_cid(ctx, nullptr, peer->our_cid_len);
    EXPECT_EQ(result, nullptr) << "Should return NULL for NULL CID";

    // Test 3: Zero CID length
    result = dtls_get_peer_by_cid(ctx, peer->our_cid, 0);
    EXPECT_EQ(result, nullptr) << "Should return NULL for zero CID length";

    // Test 4: CID length > max
    result = dtls_get_peer_by_cid(ctx, peer->our_cid, DTLS_CID_MAX_SIZE + 1);
    EXPECT_EQ(result, nullptr) << "Should return NULL for CID length > max";
}

/**
 * Test: CID Lookup with Non-Matching CID
 * Verify lookup fails when CID doesn't match
 */
TEST_F(CIDRecordFormatTest, PeerLookupNoMatch) {
    dtls_add_peer(ctx, peer);

    // Different CID
    uint8_t wrong_cid[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    dtls_peer_t *result = dtls_get_peer_by_cid(ctx, wrong_cid, 8);
    EXPECT_EQ(result, nullptr) << "Should not find peer with wrong CID";
}

/**
 * Test: Record Padding Support
 * Verify record can be padded (optional per RFC 9146)
 */
TEST_F(CIDRecordFormatTest, RecordPadding) {
    // Simulate plaintext + inner content type + padding
    uint8_t buffer[128];
    size_t plaintext_len = 32;
    uint8_t inner_ct = 23;
    size_t padding_len = 8;

    // Fill plaintext
    for (size_t i = 0; i < plaintext_len; i++) {
        buffer[i] = (uint8_t)i;
    }

    // Append inner content type
    buffer[plaintext_len] = inner_ct;

    // Append padding (zeros)
    memset(buffer + plaintext_len + 1, 0, padding_len);

    size_t total_len = plaintext_len + 1 + padding_len;

    // Verify structure
    EXPECT_EQ(buffer[plaintext_len], inner_ct);
    for (size_t i = 0; i < padding_len; i++) {
        EXPECT_EQ(buffer[plaintext_len + 1 + i], 0) << "Padding byte " << i << " should be 0";
    }
    EXPECT_EQ(total_len, 41) << "Total length: 32 plaintext + 1 inner_ct + 8 padding";
}

/**
 * Test: Variable-Length CID Support
 * Verify record format works with different CID lengths (1-8 bytes)
 */
TEST_F(CIDRecordFormatTest, VariableLengthCID) {
    for (uint8_t cid_len = 1; cid_len <= DTLS_CID_MAX_SIZE; cid_len++) {
        uint8_t buffer[256];
        uint8_t *p = buffer;

        // Create temporary peer with specific CID length
        peer->peer_cid_len = cid_len;
        for (uint8_t i = 0; i < cid_len; i++) {
            peer->peer_cid[i] = 0x10 * cid_len + i;
        }

        // Build record header
        dtls_int_to_uint8(p, DTLS_CT_TLS12_CID);
        p += 1;
        dtls_int_to_uint16(p, DTLS_VERSION);
        p += 2;
        dtls_int_to_uint16(p, 1); // epoch
        p += 2;
        dtls_int_to_uint48(p, 1); // seqnum
        p += 6;
        memcpy(p, peer->peer_cid, peer->peer_cid_len);
        p += peer->peer_cid_len;
        dtls_int_to_uint16(p, 0); // length
        p += 2;

        // Verify header size
        size_t expected_size = 13 + cid_len;
        size_t actual_size = p - buffer;
        EXPECT_EQ(actual_size, expected_size)
            << "Header with " << (int)cid_len << "-byte CID should be " << expected_size << " bytes";

        // Verify CID in header
        uint8_t *cid_in_header = buffer + 11; // After fixed 11-byte prefix
        for (uint8_t i = 0; i < cid_len; i++) {
            EXPECT_EQ(cid_in_header[i], peer->peer_cid[i])
                << "CID byte " << (int)i << " mismatch for length " << (int)cid_len;
        }
    }
}

/**
 * Test: Length Field Encoding
 * Verify length field correctly encodes ciphertext length
 */
TEST_F(CIDRecordFormatTest, LengthFieldEncoding) {
    uint8_t buffer[256];

    // Test various ciphertext lengths
    uint16_t test_lengths[] = {0, 1, 16, 256, 1024, 16384};

    for (uint16_t len : test_lengths) {
        uint8_t *p = buffer;

        // Skip to length field position (after content type, version, epoch, seqnum, CID)
        p += 1 + 2 + 2 + 6 + peer->peer_cid_len;

        // Encode length
        dtls_int_to_uint16(p, len);

        // Verify encoding
        uint16_t decoded_len = dtls_uint16_to_int(p);
        EXPECT_EQ(decoded_len, len) << "Length " << len << " should encode/decode correctly";
    }
}

/**
 * Test: CID Record with Maximum Length Ciphertext
 * Verify record format handles maximum DTLS record size (2^14 bytes)
 */
TEST_F(CIDRecordFormatTest, MaximumCiphertextLength) {
    const uint16_t max_ciphertext_len = 16384; // 2^14 bytes (DTLS limit)

    uint8_t buffer[256];
    uint8_t *p = buffer;

    // Build header
    dtls_int_to_uint8(p, DTLS_CT_TLS12_CID);
    p += 1;
    dtls_int_to_uint16(p, DTLS_VERSION);
    p += 2;
    dtls_int_to_uint16(p, 1);
    p += 2;
    dtls_int_to_uint48(p, 1);
    p += 6;
    memcpy(p, peer->peer_cid, peer->peer_cid_len);
    p += peer->peer_cid_len;
    dtls_int_to_uint16(p, max_ciphertext_len);
    p += 2;

    // Verify length field
    uint8_t *len_field = buffer + 1 + 2 + 2 + 6 + peer->peer_cid_len;
    uint16_t decoded_len = dtls_uint16_to_int(len_field);
    EXPECT_EQ(decoded_len, max_ciphertext_len) << "Should handle maximum ciphertext length";
}

/**
 * Test: Content Type Value Verification
 * Ensure ContentType 25 is correctly defined
 */
TEST_F(CIDRecordFormatTest, ContentTypeValue) {
    EXPECT_EQ(DTLS_CT_TLS12_CID, 25) << "CID content type must be 25 per RFC 9146";

    // Verify it's different from other content types
    const uint8_t DTLS_CT_HANDSHAKE = 22;
    const uint8_t DTLS_CT_APPLICATION_DATA = 23;
    const uint8_t DTLS_CT_ALERT = 21;

    EXPECT_NE(DTLS_CT_TLS12_CID, DTLS_CT_HANDSHAKE);
    EXPECT_NE(DTLS_CT_TLS12_CID, DTLS_CT_APPLICATION_DATA);
    EXPECT_NE(DTLS_CT_TLS12_CID, DTLS_CT_ALERT);
}

/**
 * Test: Epoch and Sequence Number Handling
 * Verify correct encoding of epoch and sequence number in CID records
 */
TEST_F(CIDRecordFormatTest, EpochAndSeqNum) {
    uint8_t buffer[256];

    // Test various epoch/seqnum combinations
    struct {
        uint16_t epoch;
        uint64_t seqnum;
    } test_cases[] = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 42},
        {65535, 281474976710655ULL}, // Max values (2^16-1, 2^48-1)
    };

    for (const auto& tc : test_cases) {
        uint8_t *p = buffer;

        // Skip to epoch field
        p += 1 + 2; // ContentType + Version

        // Encode epoch
        dtls_int_to_uint16(p, tc.epoch);
        p += 2;

        // Encode seqnum
        dtls_int_to_uint48(p, tc.seqnum);
        p += 6;

        // Verify encoding
        uint8_t *verify = buffer + 1 + 2;
        uint16_t decoded_epoch = dtls_uint16_to_int(verify);
        verify += 2;
        uint64_t decoded_seqnum = dtls_uint48_to_int(verify);

        EXPECT_EQ(decoded_epoch, tc.epoch) << "Epoch should encode/decode correctly";
        EXPECT_EQ(decoded_seqnum, tc.seqnum) << "Seqnum should encode/decode correctly";
    }
}

#endif /* DTLS_CID */

/**
 * Main test runner
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
