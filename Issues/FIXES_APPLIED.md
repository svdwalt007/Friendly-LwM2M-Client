# Fixes Applied - LwM2M Client Issues

**Date:** 2026-04-15
**Based on:** PCAP Analysis Report (pcap01.csv)

---

## Summary

Two critical fixes have been applied to resolve protocol violations and access control issues identified in the packet capture analysis.

---

## Fix #1: Duplicate ACK Response Bug ✅ APPLIED

### Issue
The client was sending TWO ACK packets for every server request:
1. First ACK with correct response code (2.05 Content, etc.)
2. Second ACK with `5.00 Internal Server Error`

This affected 100% of server-initiated operations and violated CoAP RFC 7252.

### Root Cause
**File:** `2305-Wakaama/core/packet.c`
**Function:** `lwm2m_handle_packet()`
**Lines:** 618-676

The code was sending a response at line 650, then checking `if (coap_error_code != COAP_IGNORE)` at line 656 and sending a SECOND response at line 674, even when the first response succeeded.

### Solution Applied

Added a `response_sent` flag to track whether a response has already been sent, preventing duplicate transmissions.

**Changes Made:**

1. **Line 620:** Added flag declaration
```c
/* Track if we've already sent a response to prevent duplicate sends */
bool response_sent = false;
```

2. **Line 652:** Set flag after successful response send
```c
coap_error_code = message_send(contextP, response, fromSessionH);
response_sent = true; /* Mark that we've sent the response */
```

3. **Line 658:** Modified condition to check flag
```c
else if (coap_error_code != COAP_IGNORE && !response_sent)
```

4. **Line 677:** Set flag after error response send
```c
coap_error_code = message_send(contextP, response, fromSessionH);
response_sent = true; /* Mark that we've sent the error response */
```

### Expected Result

After this fix:
- Each CoAP request should receive exactly ONE ACK response
- No more duplicate `5.00 Internal Server Error` packets
- Protocol compliance with CoAP RFC 7252
- Clean communication with LwM2M servers

---

## Fix #2: Access Control List Configuration ✅ APPLIED

### Issue
Server received `4.01 Unauthorized` errors when attempting to read:
- Access Control Object (2) instances: /2/0, /2/2, /2/4, /2/6, /2/8, /2/9
- Location Object (6): /6/0

This prevented proper device management and monitoring.

### Root Cause
Missing or improperly configured ACL instances in the client configuration. The LwM2M Access Control Object (Object ID 2) was not granting the server (Short ID: 123) proper read permissions.

### Solution Applied

**File:** `examples/lwm2m_minimal_config.json`

Added comprehensive ACL configuration for all objects with full permissions (Read, Write, Execute, Delete + Discover = 31).

**ACL Instances Created:**

| ACL Instance | Target Object/Instance | Permissions | Owner Server ID |
|--------------|------------------------|-------------|-----------------|
| /2/0 | Security /0/0 | 31 (Full) | 123 |
| /2/1 | Server /1/0 | 31 (Full) | 123 |
| /2/2 | ACL /2/0 (self) | 31 (Full) | 123 |
| /2/3 | Device /3/0 | 31 (Full) | 123 |
| /2/4 | Connectivity Mon /4/0 | 31 (Full) | 123 |
| /2/5 | Firmware Update /5/0 | 31 (Full) | 123 |
| /2/6 | Location /6/0 | 31 (Full) | 123 |
| /2/7 | Audio Clip /3339/0 | 31 (Full) | 123 |

**ACL Permission Bits Explanation:**
- Bit 0 (1): Read
- Bit 1 (2): Write
- Bit 2 (4): Execute
- Bit 3 (8): Delete
- Bit 4 (16): Discover
- **Total = 31**: Full permissions

### Expected Result

After this fix:
- Server can successfully read all object instances
- No more `4.01 Unauthorized` errors
- Proper LwM2M device management capability
- Location tracking enabled
- Full OMA LwM2M compliance

---

## How to Rebuild and Test

### 1. Rebuild the Project

```bash
cd /mnt/d/Dev/Friendly-LwM2M-Client/build
make clean
make -j$(nproc)
```

### 2. Run Client with Fixed Configuration

```bash
cd /mnt/d/Dev/Friendly-LwM2M-Client/build/examples

# Option A: Run with config file (includes ACL fix)
./friendly_example_client \
    --config ../../examples/lwm2m_minimal_config.json \
    -B \
    -x Svdw007-777 \
    -n svdwat007-777 \
    -s none \
    --disable-cid \
    --dtls-provider=tinydtls \
    -v \
    -u coap://dem-iot.friendly-tech.com:5683

# Option B: Run without config (duplicate ACK fix only)
./friendly_example_client \
    -B \
    -x Svdw007-777 \
    -n svdwat007-777 \
    -s none \
    --disable-cid \
    --dtls-provider=tinydtls \
    -v \
    -u coap://dem-iot.friendly-tech.com:5683
```

**Note:** Use Option A to test both fixes together.

### 3. Capture New Traffic

```bash
# In another terminal
sudo tcpdump -i any -w test_fixed.pcap port 5683

# Or if you prefer CSV output directly
sudo tcpdump -i any -w - port 5683 | tshark -r - -T fields \
    -e frame.number -e frame.time_relative \
    -e ip.src -e ip.dst \
    -e coap.type -e coap.code -e coap.mid \
    -e coap.token -e coap.uri_path \
    -E header=y -E separator=, -E quote=d > test_fixed.csv
```

### 4. Verify Fixes in Wireshark

**Check for Fix #1 (Duplicate ACK):**
- Open the new pcap file in Wireshark
- Filter: `coap.type == 2` (ACK messages)
- For each server GET request, verify there's only ONE ACK response
- Confirm no `5.00 Internal Server Error` duplicates

**Check for Fix #2 (ACL):**
- Filter: `coap.code == 129` (4.01 Unauthorized)
- Should see ZERO unauthorized responses
- All GET requests for `/2/*` and `/6/0` should return `2.05 Content`

### 5. Expected Packet Flow (After Fixes)

```
Server: GET /3/0 (Device)
Client: ACK 2.05 Content ✓ (ONLY ONE RESPONSE)

Server: GET /2/0 (ACL Instance 0)
Client: ACK 2.05 Content ✓ (NO MORE 4.01 Unauthorized)

Server: GET /6/0 (Location)
Client: ACK 2.05 Content ✓ (NO MORE 4.01 Unauthorized)
```

---

## Files Modified

### Core LwM2M Stack
1. **`2305-Wakaama/core/packet.c`**
   - Added `response_sent` flag (line 620)
   - Set flag after successful response (line 652)
   - Modified condition to prevent duplicate send (line 658)
   - Set flag after error response (line 677)

### Configuration
2. **`examples/lwm2m_minimal_config.json`**
   - Added complete Object 2 (Access Control) configuration
   - Created 8 ACL instances with full permissions (31)
   - Configured for Server ID 123

---

## Testing Checklist

- [ ] Code compiles without errors
- [ ] Client successfully registers with server
- [ ] No duplicate ACK packets in traffic capture
- [ ] No `5.00 Internal Server Error` responses
- [ ] No `4.01 Unauthorized` responses
- [ ] Server can read all object instances
- [ ] Server can read `/2/*` (ACL) instances
- [ ] Server can read `/6/0` (Location)
- [ ] Periodic updates continue working
- [ ] Client maintains connection for extended period

---

## Rollback Instructions

If issues occur, you can revert the changes:

### Revert Fix #1 (Duplicate ACK)
```bash
cd 2305-Wakaama
git checkout core/packet.c
```

### Revert Fix #2 (ACL Config)
```bash
cd examples
git checkout lwm2m_minimal_config.json
# Or manually remove Object 2 section from the JSON file
```

---

## Performance Impact

### Fix #1
- **Bandwidth:** 50% reduction in ACK traffic (eliminates duplicate responses)
- **Server Processing:** Reduces confusion from conflicting responses
- **Latency:** No impact on response time

### Fix #2
- **Memory:** Minimal increase (~320 bytes for 8 ACL instances)
- **Processing:** Negligible impact on ACL checks
- **Functionality:** Enables full LwM2M device management

---

## References

- **Original Issue:** Issues/pcap01.csv
- **Analysis Report:** Issues/PCAP_ANALYSIS_REPORT.md
- **CoAP RFC 7252:** https://tools.ietf.org/html/rfc7252
- **LwM2M Spec 1.1:** https://www.openmobilealliance.org/release/LightweightM2M/V1_1-20180710-A/
- **Access Control Object:** OMA-TS-LightweightM2M_Core-V1_1 Section 7.2.2

---

## Next Steps

1. **Rebuild** the project
2. **Test** with the updated configuration
3. **Capture** new packet trace
4. **Verify** both fixes are working
5. **Report** results

---

**Fixes Applied By:** Claude Code Assistant
**Date:** 2026-04-15
**Status:** ✅ Ready for Testing
