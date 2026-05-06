# Graceful Shutdown - Quick Reference

## How to Trigger Graceful Shutdown

Press **Ctrl+C** while the client is running.

## What Happens

1. **Signal Caught**: SIGINT signal triggers shutdown flag
2. **Main Loop Exits**: Application stops normal operation
3. **State Check**: Verifies client is registered (STATE_READY)
4. **De-register**: Sends DE-REGISTER to all LwM2M servers
5. **Process**: Loops for up to 2 seconds to complete de-register
6. **Cleanup**: Stops threads and destroys client
7. **Exit**: Application terminates cleanly

## Expected Output

```
^C
[SIGNAL] Received SIGINT, shutting down gracefully...
[1777545435] [INFO] Shutting down...
[1777545435] [INFO] Stopping LwM2M client...
[1777545435] [INFO] Current client state: 5
[1777545435] [INFO] Sending DE-REGISTER request to server
[1777545435] [INFO] State transition: 5 → deregistering
[1777545435] [INFO] State transition: 5 → 0
[1777545435] [INFO] DE-REGISTER complete
[1777545435] [INFO] LwM2M client stopped
[1777545435] [INFO] Closing example...
[1777545435] [INFO] Destroying LwM2M client...
[1777545435] [INFO] LwM2M client destroyed
[1777545435] [INFO] Goodbye!
```

## Code Flow

```cpp
// 1. Signal handler sets shutdown flag
g_shutdown_requested.store(true);

// 2. Take client ownership
client = WppClient::takeOwnership();

// 3. Send de-register if ready
if (client->getState() == STATE_READY) {
    client->deregister();
}

// 4. Process de-register with timeout
time_t timeout = time(NULL) + 2;
while (time(NULL) < timeout) {
    client->loop();
    // Check for state change...
}

// 5. Cleanup
WppClient::remove();
```

## State Transitions

- **Before**: STATE_READY (5) - Registered and operational
- **During**: Processing de-register message
- **After**: STATE_INITIAL (0) - Unregistered

## Timeout Behavior

- **Duration**: 2 seconds maximum
- **Polling**: Every 100 milliseconds
- **Purpose**: Allows message transmission and response
- **Early Exit**: Breaks immediately on state change

## Server Impact

### With De-register
- ✅ Immediate cleanup
- ✅ Client removed from registry
- ✅ "De-registered" status

### Without De-register
- ❌ Waits for lifetime expiration
- ❌ Client marked "offline" or "expired"
- ❌ Delayed cleanup (could be hours)

## Testing

### Test 1: Normal Shutdown
```bash
./friendly_example_client -e TestClient -s coap://127.0.0.1:5683
# Press Ctrl+C after registration
# Expected: Quick shutdown with state transition logged
```

### Test 2: Server Unreachable
```bash
./friendly_example_client -e TestClient -s coap://192.168.1.200:5683
# Press Ctrl+C
# Expected: 2-second timeout, then clean exit
```

### Test 3: Early Shutdown
```bash
./friendly_example_client -e TestClient -s coap://127.0.0.1:5683
# Press Ctrl+C immediately (before registration)
# Expected: No de-register attempt, clean exit
```

## Wireshark Verification

**Filter**: `coap`

**Look for**:
- DELETE request to `/rd/<endpoint>`
- Server ACK with 2.02 Deleted response

## Implementation Details

**File**: `examples/main.cpp`

**Key Functions**:
- `signalHandler()` - Catches SIGINT
- `client->deregister()` - Sends de-register
- `client->loop()` - Processes network messages

**Includes**:
- `<csignal>` - Signal handling
- `<atomic>` - Thread-safe shutdown flag

## Troubleshooting

### Shutdown hangs for 2 seconds
- **Cause**: Server unreachable or not responding
- **Solution**: Normal behavior, timeout will trigger

### No "State transition" logged
- **Cause**: Client wasn't registered (STATE_READY)
- **Solution**: Normal - de-register only sent if registered

### Client still shows on server
- **Cause**: Server didn't receive de-register
- **Check**: Network connectivity, firewall rules
- **Verify**: Use Wireshark to confirm message sent

### Build errors
- **Error**: `lwm2m_state_t` not found
- **Fix**: Use `lwm2m_client_state_t` instead

## Related Documentation

- **GRACEFUL_SHUTDOWN_IMPLEMENTATION.md** - Complete implementation guide
- **DE_REGISTER_UPDATE_SUMMARY.md** - Technical details and changes
- **CLI_USAGE.md** - Command-line options

## API Reference

```cpp
// WppClient methods used
void deregister();                    // Send de-register to servers
lwm2m_client_state_t getState();      // Get current client state
time_t loop();                        // Process network messages
```

## LwM2M States

| Name | Value | Description |
|------|-------|-------------|
| STATE_INITIAL | 0 | Unregistered |
| STATE_BOOTSTRAP_REQUIRED | 1 | Bootstrap needed |
| STATE_BOOTSTRAPPING | 2 | Bootstrapping |
| STATE_REGISTER_REQUIRED | 3 | Registration needed |
| STATE_REGISTERING | 4 | Registering |
| STATE_READY | 5 | Registered and ready |

---

**Quick Tip**: The graceful shutdown ensures your LwM2M server knows the client disconnected intentionally, enabling immediate cleanup instead of waiting for registration expiration.
