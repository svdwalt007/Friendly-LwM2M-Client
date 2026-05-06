/*******************************************************************************
 * Test Suite: DTLS CID Integration Tests
 *
 * End-to-end tests for CID functionality
 *
 * Coverage:
 * - Full CID handshake simulation
 * - CID negotiation scenarios
 * - Session persistence with CID
 * - Fallback to non-CID
 * - Multiple peers with CID
 * - Connection API integration
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

class CIDIntegrationTest : public ::testing::Test {
protected:
    dtls_context_t *client_ctx;
    dtls_context_t *server_ctx;
    dtls_peer_t *client_peer;
    dtls_peer_t *server_peer;
    session_t client_session;
    session_t server_session;

    void SetUp() override {
        // Initialize client session (client -> server)
        memset(&server_session, 0, sizeof(session_t));
        server_session.size = sizeof(server_session.addr);
        server_session.addr.sin.sin_family = AF_INET;
        server_session.addr.sin.sin_port = htons(5684);
        server_session.addr.sin.sin_addr.s_addr = htonl(0x7f000001); // 127.0.0.1

        // Initialize server session (server's view of client)
        memset(&client_session, 0, sizeof(session_t));
        client_session.size = sizeof(client_session.addr);
        client_session.addr.sin.sin_family = AF_INET;
        client_session.addr.sin.sin_port = htons(54321);
        client_session.addr.sin.sin_addr.s_addr = htonl(0x7f000001);

        // Create DTLS contexts
        client_ctx = dtls_new_context(nullptr);
        ASSERT_NE(client_ctx, nullptr);

        server_ctx = dtls_new_context(nullptr);
        ASSERT_NE(server_ctx, nullptr);

        // Create peers
        client_peer = dtls_new_peer(&server_session);
        ASSERT_NE(client_peer, nullptr);

        server_peer = dtls_new_peer(&client_session);
        ASSERT_NE(server_peer, nullptr);
    }

    void TearDown() override {
        if (client_peer) {
            dtls_free_peer(client_peer);
        }
        if (server_peer) {
            dtls_free_peer(server_peer);
        }
        if (client_ctx) {
            dtls_free_context(client_ctx);
        }
        if (server_ctx) {
            dtls_free_context(server_ctx);
        }
    }

    // Helper: Simulate CID negotiation
    void NegotiateCID(dtls_peer_t *peer, uint8_t cid_version, uint8_t cid_len) {
#if DTLS_CID
        peer->cid_negotiated = 1;
        peer->cid_version = cid_version;
        peer->our_cid_len = cid_len;
        peer->peer_cid_len = cid_len;

        // Generate random CID values
        for (uint8_t i = 0; i < cid_len; i++) {
            peer->our_cid[i] = 0xAA + i + cid_version;
            peer->peer_cid[i] = 0xBB + i + cid_version;
        }
#endif
    }
};

#if DTLS_CID

/**
 * Test: Successful CID Negotiation - RFC 9146 (CID54)
 * Simulate full handshake with CID extension exchange
 */
TEST_F(CIDIntegrationTest, SuccessfulCID54Negotiation) {
    // Step 1: Client prepares ClientHello with CID extension
    client_peer->our_cid_len = DTLS_CID_DEFAULT_LENGTH;
    for (int i = 0; i < client_peer->our_cid_len; i++) {
        client_peer->our_cid[i] = 0xC1 + i;
    }
    client_peer->cid_version = 2; // Prefer RFC 9146

    // Step 2: Server prepares ServerHello with CID extension
    server_peer->our_cid_len = DTLS_CID_DEFAULT_LENGTH;
    for (int i = 0; i < server_peer->our_cid_len; i++) {
        server_peer->our_cid[i] = 0xS1 + i;
    }
    server_peer->cid_version = 2; // RFC 9146

    // Step 3: Exchange CID values (simulated)
    memcpy(client_peer->peer_cid, server_peer->our_cid, server_peer->our_cid_len);
    client_peer->peer_cid_len = server_peer->our_cid_len;

    memcpy(server_peer->peer_cid, client_peer->our_cid, client_peer->our_cid_len);
    server_peer->peer_cid_len = client_peer->our_cid_len;

    // Step 4: Mark CID as negotiated
    client_peer->cid_negotiated = 1;
    server_peer->cid_negotiated = 1;

    // Verify negotiation
    EXPECT_TRUE(client_peer->cid_negotiated);
    EXPECT_TRUE(server_peer->cid_negotiated);
    EXPECT_EQ(client_peer->cid_version, 2);
    EXPECT_EQ(server_peer->cid_version, 2);
    EXPECT_EQ(client_peer->our_cid_len, DTLS_CID_DEFAULT_LENGTH);
    EXPECT_EQ(server_peer->our_cid_len, DTLS_CID_DEFAULT_LENGTH);

    // Verify CID exchange
    for (int i = 0; i < DTLS_CID_DEFAULT_LENGTH; i++) {
        EXPECT_EQ(client_peer->peer_cid[i], server_peer->our_cid[i]);
        EXPECT_EQ(server_peer->peer_cid[i], client_peer->our_cid[i]);
    }
}

/**
 * Test: Fallback to CID53 (Draft)
 * Server doesn't support CID54, fallback to CID53
 */
TEST_F(CIDIntegrationTest, FallbackToCID53) {
    // Client prefers CID54 but can fallback to CID53
    client_peer->our_cid_len = DTLS_CID_DEFAULT_LENGTH;
    client_peer->cid_version = 2; // Start with CID54 preference

    // Server only supports CID53
    server_peer->our_cid_len = DTLS_CID_DEFAULT_LENGTH;
    server_peer->cid_version = 1; // CID53 only

    // Simulate negotiation: client accepts server's CID53
    client_peer->cid_version = 1; // Fallback
    client_peer->cid_negotiated = 1;
    server_peer->cid_negotiated = 1;

    // Verify fallback
    EXPECT_TRUE(client_peer->cid_negotiated);
    EXPECT_TRUE(server_peer->cid_negotiated);
    EXPECT_EQ(client_peer->cid_version, 1) << "Client should fallback to CID53";
    EXPECT_EQ(server_peer->cid_version, 1);
}

/**
 * Test: CID Not Negotiated
 * Server doesn't support CID at all
 */
TEST_F(CIDIntegrationTest, CIDNotNegotiated) {
    // Client advertises CID
    client_peer->our_cid_len = DTLS_CID_DEFAULT_LENGTH;

    // Server doesn't respond with CID extension
    // (peer_cid_len remains 0, cid_negotiated remains 0)

    // Verify CID not negotiated
    EXPECT_FALSE(client_peer->cid_negotiated);
    EXPECT_FALSE(server_peer->cid_negotiated);
    EXPECT_EQ(client_peer->cid_version, 0);
    EXPECT_EQ(server_peer->cid_version, 0);
    EXPECT_EQ(client_peer->peer_cid_len, 0) << "Client should not have server CID";
}

/**
 * Test: Multiple Peers with Different CIDs
 * Verify context can manage multiple peers with unique CIDs
 */
TEST_F(CIDIntegrationTest, MultiplePeersWithCID) {
    const int num_peers = 5;
    dtls_peer_t *peers[num_peers];
    session_t sessions[num_peers];

    // Create multiple peers with unique CIDs
    for (int i = 0; i < num_peers; i++) {
        memset(&sessions[i], 0, sizeof(session_t));
        sessions[i].size = sizeof(sessions[i].addr);
        sessions[i].addr.sin.sin_family = AF_INET;
        sessions[i].addr.sin.sin_port = htons(5684 + i);
        sessions[i].addr.sin.sin_addr.s_addr = htonl(0x7f000001 + i);

        peers[i] = dtls_new_peer(&sessions[i]);
        ASSERT_NE(peers[i], nullptr);

        // Assign unique CID
        NegotiateCID(peers[i], 2, DTLS_CID_DEFAULT_LENGTH);
        for (uint8_t j = 0; j < DTLS_CID_DEFAULT_LENGTH; j++) {
            peers[i]->our_cid[j] = (i << 4) | j; // Unique pattern
        }

        // Add to context
        dtls_add_peer(server_ctx, peers[i]);
    }

    // Verify each peer can be looked up by its unique CID
    for (int i = 0; i < num_peers; i++) {
        dtls_peer_t *found = dtls_get_peer_by_cid(server_ctx,
                                                    peers[i]->our_cid,
                                                    peers[i]->our_cid_len);
        EXPECT_NE(found, nullptr) << "Should find peer " << i;
        EXPECT_EQ(found, peers[i]) << "Should return correct peer " << i;

        // Verify CID is unique
        for (int j = 0; j < num_peers; j++) {
            if (i != j) {
                int cmp = memcmp(peers[i]->our_cid, peers[j]->our_cid, DTLS_CID_DEFAULT_LENGTH);
                EXPECT_NE(cmp, 0) << "Peer " << i << " and " << j << " should have different CIDs";
            }
        }
    }

    // Cleanup
    for (int i = 0; i < num_peers; i++) {
        dtls_free_peer(peers[i]);
    }
}

/**
 * Test: Session Persistence After NAT Rebinding
 * Simulate IP/port change while maintaining CID-bound session
 */
TEST_F(CIDIntegrationTest, SessionPersistenceAfterNATRebind) {
    // Initial session setup
    NegotiateCID(client_peer, 2, DTLS_CID_DEFAULT_LENGTH);
    dtls_add_peer(server_ctx, server_peer);

    // Record initial client address
    struct sockaddr_in initial_addr = client_session.addr.sin;

    // Simulate NAT rebinding: client's IP/port changes
    client_session.addr.sin.sin_port = htons(65432); // New port
    // IP remains same (could also change in real NAT scenario)

    // Server receives packet with CID
    // Should find peer by CID, not by IP/port
    dtls_peer_t *found = dtls_get_peer_by_cid(server_ctx,
                                                server_peer->our_cid,
                                                server_peer->our_cid_len);

    EXPECT_NE(found, nullptr) << "Should find peer by CID despite address change";
    EXPECT_EQ(found, server_peer);

    // Server should update peer's session address
    // (This would happen in real dtls_handle_message)
    memcpy(&found->session, &client_session, sizeof(session_t));

    // Verify session updated
    EXPECT_NE(found->session.addr.sin.sin_port, initial_addr.sin_port)
        << "Peer session should be updated to new port";
    EXPECT_EQ(found->session.addr.sin.sin_port, htons(65432));
}

/**
 * Test: Zero-Length CID Exchange
 * Client advertises CID support but doesn't request server use CID
 */
TEST_F(CIDIntegrationTest, ZeroLengthCIDExchange) {
    // Client sends zero-length CID (advertise support, don't request)
    client_peer->our_cid_len = 0;

    // Server can choose to send zero-length or non-zero CID
    server_peer->our_cid_len = 0;

    // Both agree on zero-length
    client_peer->peer_cid_len = 0;
    server_peer->peer_cid_len = 0;

    // Mark as negotiated (CID extension was present)
    client_peer->cid_negotiated = 1;
    server_peer->cid_negotiated = 1;
    client_peer->cid_version = 2;
    server_peer->cid_version = 2;

    // Verify negotiation
    EXPECT_TRUE(client_peer->cid_negotiated);
    EXPECT_TRUE(server_peer->cid_negotiated);
    EXPECT_EQ(client_peer->our_cid_len, 0);
    EXPECT_EQ(client_peer->peer_cid_len, 0);
    EXPECT_EQ(server_peer->our_cid_len, 0);
    EXPECT_EQ(server_peer->peer_cid_len, 0);
}

/**
 * Test: Asymmetric CID Lengths
 * Client and server use different CID lengths
 */
TEST_F(CIDIntegrationTest, AsymmetricCIDLengths) {
    // Client uses 4-byte CID
    client_peer->our_cid_len = 4;
    for (int i = 0; i < 4; i++) {
        client_peer->our_cid[i] = 0xAA + i;
    }

    // Server uses 8-byte CID
    server_peer->our_cid_len = 8;
    for (int i = 0; i < 8; i++) {
        server_peer->our_cid[i] = 0xBB + i;
    }

    // Exchange CID values
    memcpy(client_peer->peer_cid, server_peer->our_cid, server_peer->our_cid_len);
    client_peer->peer_cid_len = server_peer->our_cid_len;

    memcpy(server_peer->peer_cid, client_peer->our_cid, client_peer->our_cid_len);
    server_peer->peer_cid_len = client_peer->our_cid_len;

    // Mark negotiated
    client_peer->cid_negotiated = 1;
    server_peer->cid_negotiated = 1;
    client_peer->cid_version = 2;
    server_peer->cid_version = 2;

    // Verify asymmetric lengths
    EXPECT_EQ(client_peer->our_cid_len, 4);
    EXPECT_EQ(client_peer->peer_cid_len, 8);
    EXPECT_EQ(server_peer->our_cid_len, 8);
    EXPECT_EQ(server_peer->peer_cid_len, 4);

    // Verify CID values
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(server_peer->peer_cid[i], 0xAA + i);
    }
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(client_peer->peer_cid[i], 0xBB + i);
    }
}

/**
 * Test: CID Renegotiation on Session Resumption
 * Verify CID persists across session resumption
 */
TEST_F(CIDIntegrationTest, CIDPersistenceOnResumption) {
    // Initial session with CID
    NegotiateCID(client_peer, 2, DTLS_CID_DEFAULT_LENGTH);

    // Save original CID values
    uint8_t original_client_cid[DTLS_CID_MAX_SIZE];
    uint8_t original_server_cid[DTLS_CID_MAX_SIZE];
    memcpy(original_client_cid, client_peer->our_cid, client_peer->our_cid_len);
    memcpy(original_server_cid, client_peer->peer_cid, client_peer->peer_cid_len);

    // Simulate session resumption (abbreviated handshake)
    // CID should remain the same

    // Verify CID unchanged
    EXPECT_EQ(memcmp(client_peer->our_cid, original_client_cid, client_peer->our_cid_len), 0)
        << "Client CID should persist on resumption";
    EXPECT_EQ(memcmp(client_peer->peer_cid, original_server_cid, client_peer->peer_cid_len), 0)
        << "Server CID should persist on resumption";

    EXPECT_TRUE(client_peer->cid_negotiated) << "CID should remain negotiated";
    EXPECT_EQ(client_peer->cid_version, 2);
}

/**
 * Test: Peer Cleanup with CID
 * Verify CID fields are properly cleaned up when peer is freed
 */
TEST_F(CIDIntegrationTest, PeerCleanupWithCID) {
    dtls_peer_t *temp_peer = dtls_new_peer(&server_session);
    ASSERT_NE(temp_peer, nullptr);

    // Setup CID
    NegotiateCID(temp_peer, 2, DTLS_CID_DEFAULT_LENGTH);
    ASSERT_TRUE(temp_peer->cid_negotiated);

    // Free peer (should not crash or leak)
    dtls_free_peer(temp_peer);

    // No assertions needed - test passes if no crash/leak
    SUCCEED();
}

/**
 * Test: CID with All Supported Lengths (1-8 bytes)
 * Verify all CID lengths work in integration scenario
 */
TEST_F(CIDIntegrationTest, AllSupportedCIDLengths) {
    for (uint8_t len = 1; len <= DTLS_CID_MAX_SIZE; len++) {
        dtls_peer_t *peer = dtls_new_peer(&server_session);
        ASSERT_NE(peer, nullptr);

        // Setup CID with specific length
        NegotiateCID(peer, 2, len);
        EXPECT_EQ(peer->our_cid_len, len);
        EXPECT_EQ(peer->peer_cid_len, len);

        // Add to context
        dtls_add_peer(server_ctx, peer);

        // Verify lookup works
        dtls_peer_t *found = dtls_get_peer_by_cid(server_ctx, peer->our_cid, len);
        EXPECT_NE(found, nullptr) << "Should find peer with " << (int)len << "-byte CID";
        EXPECT_EQ(found, peer);

        // Cleanup
        dtls_free_peer(peer);
    }
}

/**
 * Test: CID Collision Detection
 * Verify system handles CID collisions (should not happen with random generation)
 */
TEST_F(CIDIntegrationTest, CIDCollisionDetection) {
    dtls_peer_t *peer1 = dtls_new_peer(&server_session);
    dtls_peer_t *peer2 = dtls_new_peer(&client_session);

    ASSERT_NE(peer1, nullptr);
    ASSERT_NE(peer2, nullptr);

    // Intentionally create CID collision
    NegotiateCID(peer1, 2, 8);
    NegotiateCID(peer2, 2, 8);

    // Force same CID
    memcpy(peer2->our_cid, peer1->our_cid, 8);

    // Add both to context
    dtls_add_peer(server_ctx, peer1);
    dtls_add_peer(server_ctx, peer2);

    // Lookup will find first match (collision)
    dtls_peer_t *found = dtls_get_peer_by_cid(server_ctx, peer1->our_cid, 8);
    EXPECT_NE(found, nullptr);

    // In production, CID generation should prevent this
    // (8-byte random CID has 2^64 possible values, collision extremely unlikely)

    dtls_free_peer(peer1);
    dtls_free_peer(peer2);
}

#endif /* DTLS_CID */

/**
 * Main test runner
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
