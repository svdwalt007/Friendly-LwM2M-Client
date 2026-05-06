# LwM2M Packet Capture Analysis Report

**Date:** 2026-04-15
**Capture File:** `pcap01.csv`
**Client:** 172.16.16.2 (svdwat007-567)
**Server:** 176.9.138.37 (dem-iot.friendly-tech.com)

---

## Executive Summary

✅ **Good News:** The client successfully connects and registers with the LwM2M server.
❌ **Critical Issues:** Two major protocol bugs are causing communication errors.

---

## Detailed Analysis

### 1. Registration Flow ✅ SUCCESS

| Packet | Time | Direction | Message | Result |
|--------|------|-----------|---------|--------|
| #11 | 2.81s | Client → Server | POST /rd?lwm2m=1.1&ep=svdwat007-567&b=U&lt=25 | Initial registration |
| #25 | 4.81s | Client → Server | POST /rd (Retransmission) | Network delay |
| #26-28 | 5.68s | Server → Client | 2.01 Created /rd/92d06b724777556088734bf1b6721362 | **Registration success!** |

**Status:** ✅ Client is successfully registered
**Location:** `/rd/92d06b724777556088734bf1b6721362`
**Lifetime:** 25 seconds
**Binding:** UDP (U)

### 2. Periodic Updates ✅ SUCCESS

| Packet | Time | Direction | Message | Result |
|--------|------|-----------|---------|--------|
| #151 | 26.82s | Client → Server | POST /rd/... (Update) | ✅ 2.04 Changed |
| #278 | 49.82s | Client → Server | POST /rd/... (Update) | ✅ 2.04 Changed |
| #331 | 72.83s | Client → Server | POST /rd/... (Update) | ✅ 2.04 Changed |
| #452 | 95.83s | Client → Server | POST /rd/... (Update) | ✅ 2.04 Changed |

**Status:** ✅ Keep-alive mechanism working correctly
**Update Interval:** ~23 seconds (consistent with lifetime=25s)

---

## 🚨 Critical Issue #1: Duplicate ACK Responses

### Problem Description

For **EVERY** server request, the client sends **TWO ACK packets**:
1. First ACK with correct response code
2. Second ACK with `5.00 Internal Server Error`

### Evidence

```
Example 1: GET /3/0 (Device Object)
  Line 29: Client → Server: ACK 2.05 Content (CORRECT)
  Line 30: Client → Server: ACK 5.00 Internal Server Error (DUPLICATE!)

Example 2: GET /3339/0 (Audio Clip)
  Line 53: Client → Server: ACK 2.05 Content (CORRECT)
  Line 54: Client → Server: ACK 5.00 Internal Server Error (DUPLICATE!)

Example 3: GET /2/10 (ACL Instance)
  Line 61: Client → Server: ACK 2.05 Content (CORRECT)
  Line 62: Client → Server: ACK 5.00 Internal Server Error (DUPLICATE!)
```

### All Occurrences

| Server Request | First ACK | Second ACK (Duplicate) | Lines |
|----------------|-----------|------------------------|-------|
| GET /3/0 | 2.05 Content | 5.00 Internal Server Error | 29-30 |
| GET /3339/0 | 2.05 Content | 5.00 Internal Server Error | 53-54 |
| GET /2/10 | 2.05 Content | 5.00 Internal Server Error | 61-62 |
| GET /4/0 | 2.05 Content | 5.00 Internal Server Error | 74-75 |
| GET /2/2 | 4.01 Unauthorized | 5.00 Internal Server Error | 83-84 |
| GET /5/0 | 2.05 Content | 5.00 Internal Server Error | 129-130 |
| GET /2/3 | 2.05 Content | 5.00 Internal Server Error | 140-141 |
| GET /2/0 | 4.01 Unauthorized | 5.00 Internal Server Error | 166-167 |
| GET /2/1 | 2.05 Content | 5.00 Internal Server Error | 174-175 |
| GET /1/0 | 2.05 Content | 5.00 Internal Server Error | 188-189 |
| GET /2/8 | 4.01 Unauthorized | 5.00 Internal Server Error | 191-192 |
| GET /2/9 | 4.01 Unauthorized | 5.00 Internal Server Error | 202-203 |
| GET /2/6 | 4.01 Unauthorized | 5.00 Internal Server Error | 205-206 |
| GET /2/7 | 2.05 Content | 5.00 Internal Server Error | 224-225 |
| GET /6/0 | 4.01 Unauthorized | 5.00 Internal Server Error | 237-238 |
| GET /2/4 | 4.01 Unauthorized | 5.00 Internal Server Error | 249-250 |
| GET /2/5 | 2.05 Content | 5.00 Internal Server Error | 252-253 |

**Pattern:** 17 duplicate responses out of 17 server requests (100% occurrence rate)

### Root Cause

**File:** `2305-Wakaama/core/packet.c`
**Function:** `lwm2m_handle_packet()`
**Lines:** 650-676

```c
// Line 650: Send first response (correct)
coap_error_code = message_send(contextP, response, fromSessionH);

// Lines 656-676: ERROR - sends second response even after first succeeds
else if (coap_error_code != COAP_IGNORE)
{
    // ... error handling ...
    if (1 == coap_set_status_code(response, coap_error_code))
    {
        coap_error_code = message_send(contextP, response, fromSessionH);  // DUPLICATE!
    }
}
```

**Bug Analysis:**
- After successfully sending the first response at line 650, the code checks if `coap_error_code != COAP_IGNORE`
- If there's any lingering error code (even from previous operations), it sends a second response at line 674
- This creates duplicate ACK packets with different response codes

### Impact

1. **Protocol Violation:** CoAP RFC 7252 states each CON request should receive exactly ONE ACK
2. **Server Confusion:** Server receives conflicting responses (success + error)
3. **Resource Waste:** Double bandwidth usage for every response
4. **Potential Connection Termination:** Some LwM2M servers may disconnect clients exhibiting this behavior
5. **Data Integrity:** Server may not trust the data if it receives contradictory responses

### Severity

🔴 **CRITICAL** - Affects 100% of server-initiated operations

---

## 🚨 Critical Issue #2: Access Control List (ACL) Violations

### Problem Description

Server receives `4.01 Unauthorized` when trying to read Access Control Object (2) instances and Location Object (6).

### Evidence

| Packet | Request | Response | Impact |
|--------|---------|----------|--------|
| #83-84 | GET /2/2 | 4.01 Unauthorized | Cannot read ACL instance 2 |
| #166-167 | GET /2/0 | 4.01 Unauthorized | Cannot read ACL instance 0 |
| #191-192 | GET /2/8 | 4.01 Unauthorized | Cannot read ACL instance 8 |
| #202-203 | GET /2/9 | 4.01 Unauthorized | Cannot read ACL instance 9 |
| #205-206 | GET /2/6 | 4.01 Unauthorized | Cannot read ACL instance 6 |
| #237-238 | GET /6/0 | 4.01 Unauthorized | Cannot read Location object |
| #249-250 | GET /2/4 | 4.01 Unauthorized | Cannot read ACL instance 4 |

**Total:** 7 authorization failures

### Root Cause

The LwM2M Access Control Object (Object ID 2) is not properly configured to allow the server to read:
- Its own instances (recursive ACL check failure)
- Other objects it should have access to (e.g., Location)

**Likely Issues:**
1. ACL instances are not created for all objects
2. ACL permissions don't grant READ access to the server
3. Server Short ID mismatch in ACL configuration

### Current ACL Configuration (Inferred)

Based on successful vs. failed reads:

| Object/Instance | Server Can Read? | ACL Status |
|-----------------|------------------|------------|
| /0/* (Security) | Not requested | N/A |
| /1/0 (Server) | ✅ Yes | Correct ACL |
| /2/0,2,4,6,8,9,10 (ACL) | ❌ No | **Missing/Wrong ACL** |
| /3/0 (Device) | ✅ Yes | Correct ACL |
| /3339/0 (Audio Clip) | ✅ Yes | Correct ACL |
| /4/0 (Connectivity Mon) | ✅ Yes | Correct ACL |
| /5/0 (Firmware Update) | ✅ Yes | Correct ACL |
| /6/0 (Location) | ❌ No | **Missing/Wrong ACL** |

### Impact

1. **Limited Management:** Server cannot fully manage client via LwM2M protocol
2. **Security Issues:** ACL misconfiguration suggests security model problems
3. **OMA Compliance:** Violates LwM2M specification for Object 2 access
4. **Location Services:** Server cannot track device location

### Severity

🟠 **HIGH** - Prevents proper device management and monitoring

---

## Successful Operations ✅

Despite the issues, these operations work correctly:

| Operation | Object | Status |
|-----------|--------|--------|
| Registration | - | ✅ Success |
| Update | - | ✅ Success |
| Read /3/0 | Device | ✅ Success (but with duplicate ACK) |
| Read /4/0 | Connectivity Monitoring | ✅ Success (but with duplicate ACK) |
| Read /5/0 | Firmware Update | ✅ Success (but with duplicate ACK) |
| Read /1/0 | LwM2M Server | ✅ Success (but with duplicate ACK) |
| Read /3339/0 | Audio Clip | ✅ Success (but with duplicate ACK) |
| Read /2/1,3,5,7 | ACL (some instances) | ✅ Success (but with duplicate ACK) |

---

## Recommended Fixes

### Fix #1: Stop Duplicate ACK Responses (URGENT)

**File to modify:** `2305-Wakaama/core/packet.c`
**Function:** `lwm2m_handle_packet()`
**Location:** Lines 650-676

**Current buggy code:**
```c
coap_error_code = message_send(contextP, response, fromSessionH);

lwm2m_free(payload);
response->payload = NULL;
response->payload_len = 0;
}
else if (coap_error_code != COAP_IGNORE)
{
    // This block sends a SECOND response!
    if (1 == coap_set_status_code(response, coap_error_code))
    {
        coap_error_code = message_send(contextP, response, fromSessionH);
    }
}
```

**Proposed fix:**
```c
coap_error_code = message_send(contextP, response, fromSessionH);

lwm2m_free(payload);
response->payload = NULL;
response->payload_len = 0;
}
else if (coap_error_code != COAP_IGNORE && coap_error_code != NO_ERROR)
{
    // Only send error response if NO successful response was sent
    if (message->type == COAP_TYPE_CON)
    {
        coap_init_message(response, COAP_TYPE_ACK, COAP_500_INTERNAL_SERVER_ERROR, message->mid);
    }
    else
    {
        coap_init_message(response, COAP_TYPE_NON, COAP_500_INTERNAL_SERVER_ERROR, message->mid);
    }

    if (1 == coap_set_status_code(response, coap_error_code))
    {
        coap_error_code = message_send(contextP, response, fromSessionH);
    }
}
```

**Alternative fix (safer):**
Add a flag to track if response was already sent:

```c
bool response_sent = false;

if (coap_error_code == NO_ERROR)
{
    // ... existing blockwise code ...

    coap_error_code = message_send(contextP, response, fromSessionH);
    response_sent = true;  // ADDED

    lwm2m_free(payload);
    response->payload = NULL;
    response->payload_len = 0;
}
else if (coap_error_code != COAP_IGNORE && !response_sent)  // MODIFIED
{
    // ... existing error handling ...
    if (1 == coap_set_status_code(response, coap_error_code))
    {
        coap_error_code = message_send(contextP, response, fromSessionH);
    }
}
```

### Fix #2: Configure Access Control List Properly

**Option A: Permissive ACL (for testing)**

Modify `examples/objects.cpp` or use the config file to create ACL instances with full permissions:

```json
{
  "2": {
    "name": "LwM2M Access Control",
    "instances": [
      {
        "id": 0,
        "resources": {
          "0": 1,
          "1": 123,
          "2": 31,
          "3": [123]
        }
      }
    ]
  }
}
```

Where:
- Resource 0: Object ID
- Resource 1: Instance ID
- Resource 2: ACL (31 = full permissions: RWED + Discover)
- Resource 3: Access Control Owner (Server ID 123)

**Option B: Proper ACL (for production)**

Create ACL instances for each object/instance combination:
- Object 2 instances for themselves
- Object 6 (Location) ACL
- Set correct permissions based on security requirements

---

## Testing Recommendations

### 1. Verify Duplicate ACK Fix

After applying Fix #1:
```bash
# Capture traffic
sudo tcpdump -i any -w test_fixed.pcap port 5683

# Run client
./friendly_example_client [options]

# Verify in Wireshark:
# - Each GET request should have exactly ONE ACK response
# - No more 5.00 Internal Server Error duplicates
```

### 2. Verify ACL Fix

After applying Fix #2:
```bash
# Check server can now read all objects
# Expected: All GET requests return 2.05 Content (no 4.01 Unauthorized)
```

### 3. Regression Testing

Run full test suite:
```bash
cd build
make test
```

---

## Conclusion

The client is **functionally working** but has two critical protocol implementation bugs:

1. **Duplicate ACK bug** - Affects 100% of operations, violates CoAP RFC
2. **ACL configuration** - Prevents proper device management

Both issues must be fixed before production deployment.

**Priority:** 🔴 Fix #1 (Duplicate ACK) → 🟠 Fix #2 (ACL Configuration)

---

## References

- **CoAP RFC 7252:** https://tools.ietf.org/html/rfc7252
- **LwM2M Spec 1.1:** https://www.openmobilealliance.org/release/LightweightM2M/V1_1-20180710-A/
- **LwM2M Object 2 (ACL):** OMA-TS-LightweightM2M_Core-V1_1-20180710-A.pdf Section 7.2.2

---

**Report Generated:** 2026-04-15
**Analyzed By:** Claude Code Assistant
**Capture Duration:** 0 - 97.5 seconds
**Total Packets Analyzed:** 68 CoAP packets
