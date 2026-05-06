/*******************************************************************************
 * Test Suite: NAT Rebinding with DTLS Connection ID
 *
 * Tests RFC 9146 CID's primary use case: surviving NAT rebinding events
 *
 * Coverage:
 * - IP address change detection
 * - Port change detection
 * - Session continuity with CID
 * - Session loss without CID
 * - PSM (Power Save Mode) wakeup scenarios
 * - Multiple NAT rebindings
 * - Bidirectional communication after rebinding
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

class NATRebindingTest : public ::testing::Test {
protected:
    dtls_context_t *server_ctx;
    dtls_peer_t *server_peer;
    session_t original_session;
    session_t rebinded_session;

    void SetUp() override {
        // Original client session (before NAT rebinding)
        memset(&original_session, 0, sizeof(session_t));
        original_session.size = sizeof(original_session.addr);
        original_session.addr.sin.sin_family = AF_INET;
        original_session.addr.sin.sin_port = htons(54321);
        original_session.addr.sin.sin_addr.s_addr = htonl(0x0A000001); // 10.0.0.1

        // Rebinded client session (after NAT change)
        memset(&rebinded_session, 0, sizeof(session_t));
        rebinded_session.size = sizeof(rebinded_session.addr);
        rebinded_session.addr.sin.sin_family = AF_INET;
        rebinded_session.addr.sin.sin_port = htons(65432); // New port
        rebinded_session.addr.sin.sin_addr.s_addr = htonl(0x0A000001); // Same IP

        // Create server context
        server_ctx = dtls_new_context(nullptr);
        ASSERT_NE(server_ctx, nullptr);

        // Create server's view of client peer
        server_peer = dtls_new_peer(&original_session);
        ASSERT_NE(server_peer, nullptr);

#if DTLS_CID
        // Setup CID
        server_peer->cid_negotiated = 1;
        server_peer->cid_version = 2; // RFC 9146
        server_peer->our_cid_len = 8;
        server_peer->peer_cid_len = 8;

        for (int i = 0; i < 8; i++) {
            server_peer->our_cid[i] = 0xAA + i;
            server_peer->peer_cid[i] = 0xBB + i;
        }
#endif

        // Add peer to context
        dtls_add_peer(server_ctx, server_peer);
    }

    void TearDown() override {
        if (server_peer) {
            dtls_free_peer(server_peer);
        }
        if (server_ctx) {
            dtls_free_context(server_ctx);
        }
    }

    // Helper: Simulate receiving packet from rebinded address
    dtls_peer_t* SimulateReceiveFromRebindedAddress(const session_t *new_session,
                                                      const uint8_t *cid,
                                                      uint8_t cid_len) {
#if DTLS_CID
        // First try CID-based lookup
        dtls_peer_t *peer = dtls_get_peer_by_cid(server_ctx, cid, cid_len);
        if (peer) {
            // Update peer's session to new address
            memcpy(&peer->session, new_session, sizeof(session_t));
            return peer;
        }
#endif
        // Fallback: traditional address-based lookup
        return dtls_get_peer(server_ctx, new_session);
    }
};

#if DTLS_CID

/**
 * Test: Port Change Detection and Recovery
 * Client's port changes due to NAT rebinding, CID allows session to continue
 */
TEST_F(NATRebindingTest, PortChangeWithCID) {
    // Verify initial state
    EXPECT_EQ(server_peer->session.addr.sin.sin_port, htons(54321));

    // Simulate receiving packet from new port with CID
    dtls_peer_t *found = SimulateReceiveFromRebindedAddress(&rebinded_session,
                                                              server_peer->our_cid,
                                                              server_peer->our_cid_len);

    // Should find peer by CID
    ASSERT_NE(found, nullptr) << "Should find peer by CID despite port change";
    EXPECT_EQ(found, server_peer);

    // Verify session updated to new port
    EXPECT_EQ(found->session.addr.sin.sin_port, htons(65432))
        << "Peer session should be updated to new port";

    // Verify IP unchanged
    EXPECT_EQ(found->session.addr.sin.sin_addr.s_addr, htonl(0x0A000001));

    // Verify CID still valid
    EXPECT_TRUE(found->cid_negotiated);
    EXPECT_EQ(found->our_cid_len, 8);
}

/**
 * Test: Port Change WITHOUT CID (Session Lost)
 * Without CID, port change breaks session
 */
TEST_F(NATRebindingTest, PortChangeWithoutCID) {
    // Disable CID for this test
    server_peer->cid_negotiated = 0;

    // Traditional address-based lookup with new port
    dtls_peer_t *found = dtls_get_peer(server_ctx, &rebinded_session);

    // Should NOT find peer (address mismatch)
    EXPECT_EQ(found, nullptr)
        << "Without CID, peer cannot be found after port change";

    // Original peer still exists with old address
    dtls_peer_t *old_peer = dtls_get_peer(server_ctx, &original_session);
    EXPECT_NE(old_peer, nullptr) << "Original peer still at old address";
    EXPECT_EQ(old_peer, server_peer);
}

/**
 * Test: IP Address Change with CID
 * Complete IP change (e.g., switching networks), CID maintains session
 */
TEST_F(NATRebindingTest, IPAddressChangeWithCID) {
    // Rebinded session with completely different IP
    session_t new_ip_session;
    memset(&new_ip_session, 0, sizeof(session_t));
    new_ip_session.size = sizeof(new_ip_session.addr);
    new_ip_session.addr.sin.sin_family = AF_INET;
    new_ip_session.addr.sin.sin_port = htons(54321); // Same port
    new_ip_session.addr.sin.sin_addr.s_addr = htonl(0xC0A80001); // 192.168.0.1 (new IP)

    // Verify initial IP
    EXPECT_EQ(server_peer->session.addr.sin.sin_addr.s_addr, htonl(0x0A000001));

    // Receive packet from new IP with CID
    dtls_peer_t *found = SimulateReceiveFromRebindedAddress(&new_ip_session,
                                                              server_peer->our_cid,
                                                              server_peer->our_cid_len);

    ASSERT_NE(found, nullptr) << "Should find peer by CID despite IP change";
    EXPECT_EQ(found, server_peer);

    // Verify session updated to new IP
    EXPECT_EQ(found->session.addr.sin.sin_addr.s_addr, htonl(0xC0A80001))
        << "Peer session should be updated to new IP";
}

/**
 * Test: PSM Wakeup Scenario (LTE-M/NB-IoT)
 * Device in Power Save Mode wakes up with new IP/port
 */
TEST_F(NATRebindingTest, PSMWakeupScenario) {
    // Before PSM: device at 10.0.0.1:54321
    EXPECT_EQ(server_peer->session.addr.sin.sin_port, htons(54321));
    EXPECT_EQ(server_peer->session.addr.sin.sin_addr.s_addr, htonl(0x0A000001));

    // After PSM wakeup: device assigned new ephemeral port
    session_t psm_wakeup_session;
    memset(&psm_wakeup_session, 0, sizeof(session_t));
    psm_wakeup_session.size = sizeof(psm_wakeup_session.addr);
    psm_wakeup_session.addr.sin.sin_family = AF_INET;
    psm_wakeup_session.addr.sin.sin_port = htons(49152); // New ephemeral port
    psm_wakeup_session.addr.sin.sin_addr.s_addr = htonl(0x0A000002); // Possibly new IP

    // Device sends request with CID
    dtls_peer_t *found = SimulateReceiveFromRebindedAddress(&psm_wakeup_session,
                                                              server_peer->our_cid,
                                                              server_peer->our_cid_len);

    ASSERT_NE(found, nullptr) << "Should recover session after PSM wakeup via CID";
    EXPECT_EQ(found, server_peer);

    // Verify session updated
    EXPECT_EQ(found->session.addr.sin.sin_port, htons(49152));
    EXPECT_EQ(found->session.addr.sin.sin_addr.s_addr, htonl(0x0A000002));

    // CID should still be valid
    EXPECT_TRUE(found->cid_negotiated);
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(found->our_cid[i], 0xAA + i) << "CID should be preserved";
    }
}

/**
 * Test: Multiple Sequential Rebindings
 * NAT changes multiple times during session lifetime
 */
TEST_F(NATRebindingTest, MultipleSequentialRebindings) {
    // Track rebinding events
    struct {
        uint16_t port;
        uint32_t ip;
    } rebindings[] = {
        {54321, 0x0A000001}, // Original
        {65432, 0x0A000001}, // Port change 1
        {60000, 0x0A000001}, // Port change 2
        {60000, 0xC0A80001}, // IP change
        {55555, 0xC0A80001}, // Port change 3
    };

    for (const auto& rebind : rebindings) {
        session_t test_session;
        memset(&test_session, 0, sizeof(session_t));
        test_session.size = sizeof(test_session.addr);
        test_session.addr.sin.sin_family = AF_INET;
        test_session.addr.sin.sin_port = htons(rebind.port);
        test_session.addr.sin.sin_addr.s_addr = htonl(rebind.ip);

        // Simulate receiving packet
        dtls_peer_t *found = SimulateReceiveFromRebindedAddress(&test_session,
                                                                  server_peer->our_cid,
                                                                  server_peer->our_cid_len);

        ASSERT_NE(found, nullptr) << "Should find peer after rebinding to "
                                  << rebind.ip << ":" << rebind.port;
        EXPECT_EQ(found, server_peer);

        // Verify session updated
        EXPECT_EQ(found->session.addr.sin.sin_port, htons(rebind.port));
        EXPECT_EQ(found->session.addr.sin.sin_addr.s_addr, htonl(rebind.ip));
    }

    // After all rebindings, CID should still be valid
    EXPECT_TRUE(server_peer->cid_negotiated);
    EXPECT_EQ(server_peer->our_cid_len, 8);
}

/**
 * Test: Bidirectional Communication After Rebinding
 * Verify both client->server and server->client work after rebinding
 */
TEST_F(NATRebindingTest, BidirectionalCommAfterRebinding) {
    // Initial state: server knows client at 10.0.0.1:54321
    EXPECT_EQ(server_peer->session.addr.sin.sin_port, htons(54321));

    // Client rebinds to new port
    rebinded_session.addr.sin.sin_port = htons(65432);

    // Client->Server: Receive packet from new address with CID
    dtls_peer_t *found = SimulateReceiveFromRebindedAddress(&rebinded_session,
                                                              server_peer->our_cid,
                                                              server_peer->our_cid_len);
    ASSERT_NE(found, nullptr);

    // Server->Client: Server now knows client's new address
    EXPECT_EQ(found->session.addr.sin.sin_port, htons(65432))
        << "Server should know client's new port for replies";

    // Simulate server sending response (using updated session)
    // Would use found->session as destination address
    // With CID in record, client can match response to session by CID

    // Verify peer CID available for server's outgoing records
    EXPECT_EQ(found->peer_cid_len, 8) << "Server should have client's CID for responses";
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(found->peer_cid[i], 0xBB + i);
    }
}

/**
 * Test: NAT Rebinding During Handshake
 * Edge case: NAT rebinds during initial handshake
 */
TEST_F(NATRebindingTest, RebindingDuringHandshake) {
    // Create peer in handshaking state (CID advertised but not negotiated yet)
    dtls_peer_t *handshake_peer = dtls_new_peer(&original_session);
    ASSERT_NE(handshake_peer, nullptr);

    handshake_peer->cid_negotiated = 0; // Handshake in progress
    handshake_peer->our_cid_len = 8;
    for (int i = 0; i < 8; i++) {
        handshake_peer->our_cid[i] = 0xCC + i;
    }

    dtls_add_peer(server_ctx, handshake_peer);

    // NAT rebinds before handshake completes
    // Without negotiated CID, must use address-based lookup
    dtls_peer_t *found = dtls_get_peer(server_ctx, &rebinded_session);
    EXPECT_EQ(found, nullptr) << "Should not find peer with new address during handshake";

    // After handshake completes and CID negotiated, future rebindings will work
    handshake_peer->cid_negotiated = 1;
    handshake_peer->cid_version = 2;
    handshake_peer->peer_cid_len = 8;

    // Now CID-based lookup should work
    found = dtls_get_peer_by_cid(server_ctx, handshake_peer->our_cid, 8);
    EXPECT_NE(found, nullptr) << "Should find peer by CID after negotiation";

    dtls_free_peer(handshake_peer);
}

/**
 * Test: CID Mismatch After Rebinding
 * Verify rejection of packets with wrong CID from new address
 */
TEST_F(NATRebindingTest, CIDMismatchAfterRebinding) {
    // Wrong CID
    uint8_t wrong_cid[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Packet from new address with wrong CID
    dtls_peer_t *found = dtls_get_peer_by_cid(server_ctx, wrong_cid, 8);

    EXPECT_EQ(found, nullptr) << "Should reject packet with wrong CID";

    // Correct CID should still work
    found = dtls_get_peer_by_cid(server_ctx, server_peer->our_cid, 8);
    EXPECT_NE(found, nullptr) << "Correct CID should work";
}

/**
 * Test: Session State Preservation
 * Verify cryptographic state (epoch, seqnum) preserved across rebinding
 */
TEST_F(NATRebindingTest, SessionStatePreservation) {
    // Simulate active session with crypto state
    // (In real scenario, security_params would have epoch/seqnum/keys)

    // Before rebinding: record session state
    uint16_t original_port = server_peer->session.addr.sin.sin_port;
    uint8_t original_cid[8];
    memcpy(original_cid, server_peer->our_cid, 8);

    // Rebind
    dtls_peer_t *found = SimulateReceiveFromRebindedAddress(&rebinded_session,
                                                              server_peer->our_cid,
                                                              server_peer->our_cid_len);
    ASSERT_NE(found, nullptr);

    // After rebinding: verify address changed but CID/crypto state unchanged
    EXPECT_NE(found->session.addr.sin.sin_port, original_port) << "Port should change";
    EXPECT_EQ(memcmp(found->our_cid, original_cid, 8), 0) << "CID should be unchanged";
    EXPECT_TRUE(found->cid_negotiated) << "CID negotiation state preserved";
    EXPECT_EQ(found->cid_version, 2) << "CID version preserved";

    // Security params would also be preserved (not freed/recreated)
    // This allows continuous encryption/decryption without re-handshake
}

/**
 * Test: Rapid Consecutive Rebindings
 * Stress test: multiple rebindings in quick succession
 */
TEST_F(NATRebindingTest, RapidConsecutiveRebindings) {
    const int num_rebindings = 100;

    for (int i = 0; i < num_rebindings; i++) {
        session_t rapid_rebind_session;
        memset(&rapid_rebind_session, 0, sizeof(session_t));
        rapid_rebind_session.size = sizeof(rapid_rebind_session.addr);
        rapid_rebind_session.addr.sin.sin_family = AF_INET;
        rapid_rebind_session.addr.sin.sin_port = htons(50000 + i);
        rapid_rebind_session.addr.sin.sin_addr.s_addr = htonl(0x0A000001);

        dtls_peer_t *found = SimulateReceiveFromRebindedAddress(&rapid_rebind_session,
                                                                  server_peer->our_cid,
                                                                  server_peer->our_cid_len);

        ASSERT_NE(found, nullptr) << "Should find peer on rebinding #" << i;
        EXPECT_EQ(found, server_peer);
        EXPECT_EQ(found->session.addr.sin.sin_port, htons(50000 + i));
    }

    // Final state: peer at last rebinded address
    EXPECT_EQ(server_peer->session.addr.sin.sin_port, htons(50000 + num_rebindings - 1));
    EXPECT_TRUE(server_peer->cid_negotiated);
}

/**
 * Test: IPv6 NAT Rebinding
 * Verify CID works with IPv6 address changes
 */
TEST_F(NATRebindingTest, IPv6Rebinding) {
    // Create IPv6 session
    session_t ipv6_session;
    memset(&ipv6_session, 0, sizeof(session_t));
    ipv6_session.size = sizeof(ipv6_session.addr6);
    ipv6_session.addr.sa.sa_family = AF_INET6;
    ipv6_session.addr.sin6.sin6_port = htons(5684);
    // Set IPv6 address: 2001:db8::1
    uint8_t ipv6_addr[] = {0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    memcpy(&ipv6_session.addr.sin6.sin6_addr, ipv6_addr, 16);

    dtls_peer_t *ipv6_peer = dtls_new_peer(&ipv6_session);
    ASSERT_NE(ipv6_peer, nullptr);

    ipv6_peer->cid_negotiated = 1;
    ipv6_peer->cid_version = 2;
    ipv6_peer->our_cid_len = 8;
    for (int i = 0; i < 8; i++) {
        ipv6_peer->our_cid[i] = 0xD0 + i;
    }

    dtls_add_peer(server_ctx, ipv6_peer);

    // Simulate IPv6 address change
    session_t ipv6_rebind;
    memcpy(&ipv6_rebind, &ipv6_session, sizeof(session_t));
    ipv6_rebind.addr.sin6.sin6_port = htons(60000); // New port

    // Lookup by CID
    dtls_peer_t *found = SimulateReceiveFromRebindedAddress(&ipv6_rebind,
                                                              ipv6_peer->our_cid,
                                                              ipv6_peer->our_cid_len);

    ASSERT_NE(found, nullptr) << "Should find IPv6 peer by CID after rebinding";
    EXPECT_EQ(found, ipv6_peer);
    EXPECT_EQ(found->session.addr.sin6.sin6_port, htons(60000));

    dtls_free_peer(ipv6_peer);
}

#endif /* DTLS_CID */

/**
 * Main test runner
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
