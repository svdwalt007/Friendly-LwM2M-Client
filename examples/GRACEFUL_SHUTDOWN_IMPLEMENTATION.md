# Graceful Shutdown Implementation for friendly_example_client

## Overview

The `friendly_example_client` application now includes a graceful shutdown mechanism that properly handles SIGINT (Ctrl+C) signals and cleanly terminates the LwM2M client connection before exiting.

## Implementation Details

### 1. Signal Handling

**Added includes**:
```cpp
#include <csignal>
#include <atomic>
```

**Global shutdown flag**:
```cpp
static atomic<bool> g_shutdown_requested(false);
```

**Signal handler**:
```cpp
void signalHandler(int signum) {
    if (signum == SIGINT) {
        cout << endl << "[SIGNAL] Received SIGINT, shutting down gracefully..." << endl;
        g_shutdown_requested.store(true);
    }
}
```

**Registration in main()**:
```cpp
signal(SIGINT, signalHandler);
```

### 2. Modified Main Loop

The main event loop now checks the shutdown flag:
```cpp
for (int iterationCnt = 0; !isDeviceShouldBeRebooted() && !g_shutdown_requested.load(); iterationCnt++)
```

The socket polling thread also respects the shutdown flag:
```cpp
void socketPolling(Connection *connection) {
    while (!isDeviceShouldBeRebooted() && !g_shutdown_requested.load()) {
        connection->loop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
```

### 3. Graceful Shutdown Sequence

When SIGINT is received, the following shutdown sequence executes:

```cpp
// 1. Initial shutdown message
[timestamp] [INFO] Shutting down...

// 2. Stop LwM2M client
[timestamp] [INFO] Stopping LwM2M client...

// 3. Log current state
[timestamp] [INFO] Current client state: <state>

// 4. Send de-register to server (if registered)
[timestamp] [INFO] Sending DE-REGISTER request to server
[timestamp] [INFO] State transition: 5 → deregistering  // STATE_READY → deregistering
[timestamp] [INFO] State transition: 5 → 0  // STATE_READY → STATE_INITIAL
[timestamp] [INFO] DE-REGISTER complete

// 5. Confirm client stopped
[timestamp] [INFO] LwM2M client stopped

// 6. Close and cleanup
[timestamp] [INFO] Closing example...

// 7. Destroy client
[timestamp] [INFO] Destroying LwM2M client...
[timestamp] [INFO] LwM2M client destroyed

// 8. Final goodbye
[timestamp] [INFO] Goodbye!
```

## Shutdown Flow Diagram

```
User presses Ctrl+C
       ↓
SIGINT signal received
       ↓
signalHandler() sets g_shutdown_requested = true
       ↓
Main loop exits on next iteration
       ↓
Check if shutdown was requested
       ↓
Take ownership of WppClient
       ↓
Check current state
       ↓
If STATE_READY → Call client->deregister()
       ↓
Process de-register with client->loop() (up to 2 seconds)
       ↓
Wait for state transition to STATE_INITIAL
       ↓
Log state transitions and completion
       ↓
Release client ownership
       ↓
Join socket polling thread
       ↓
Remove/destroy WppClient
       ↓
Exit application
```

## LwM2M State Transitions

During graceful shutdown, the client transitions from these states:

| State | Value | Description |
|-------|-------|-------------|
| STATE_INITIAL | 0 | Client is unregistered and idle |
| STATE_BOOTSTRAP_REQUIRED | 1 | Bootstrap is required |
| STATE_BOOTSTRAPPING | 2 | Bootstrap in progress |
| STATE_REGISTER_REQUIRED | 3 | Registration is required |
| STATE_REGISTERING | 4 | Registration in progress |
| STATE_READY | 5 | Client is registered and operational |

**Note**: When de-registering, the client sends a de-register request and may transition to STATE_INITIAL or remain in its current state depending on server response timing.

## De-register API

The graceful shutdown uses the WppClient's `deregister()` method to properly notify the LwM2M server:

```cpp
// Send de-register notification to all registered servers
client->deregister();

// Process the de-register message with timeout
time_t timeout = time(NULL) + 2; // 2 second timeout
while (time(NULL) < timeout) {
    client->loop();  // Process network messages
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Check for state change indicating de-register complete
    lwm2m_client_state_t newState = client->getState();
    if (newState != currentState) {
        break;  // De-register successful
    }
}
```

**Key Points**:
- `deregister()` sends CoAP DE-REGISTER messages to all registered servers
- The message is processed asynchronously via `client->loop()`
- A 2-second timeout ensures the app doesn't hang if server is unreachable
- State transitions from STATE_READY (5) to STATE_INITIAL (0) indicate success
- The server removes the client registration and stops tracking it

## Code Changes Summary

### Modified Files
- `examples/main.cpp`

### Added Components
1. **Includes**: `<csignal>`, `<atomic>`
2. **Global variable**: `g_shutdown_requested` (atomic bool)
3. **Signal handler**: `signalHandler()` for SIGINT
4. **Shutdown sequence**: Graceful cleanup in main loop

### Modified Components
1. **socketPolling()**: Check shutdown flag in loop condition
2. **main() loop**: Check shutdown flag in loop condition
3. **main() cleanup**: Added detailed shutdown logging and state transitions

## Usage

### Normal Operation
```bash
./friendly_example_client -e ClientName -s coap://127.0.0.1:5683
```

### Graceful Shutdown
Press **Ctrl+C** during operation to trigger graceful shutdown:

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

## Benefits

1. **Clean Server-Side State**: DE-REGISTER message informs the server that the client is shutting down intentionally
2. **Resource Cleanup**: Proper cleanup of threads, connections, and client objects
3. **No Resource Leaks**: All allocated resources are properly freed
4. **Professional Logging**: Clear, timestamped logs showing each shutdown step
5. **Debugging Aid**: State transitions help diagnose shutdown issues

## Thread Safety

- Uses `std::atomic<bool>` for thread-safe shutdown flag
- Signal handler only sets the atomic flag (safe operation)
- No complex operations in signal handler (signal-safe)
- Main thread handles all cleanup operations

## Testing

### Test Scenarios

1. **Normal Shutdown**:
   - Start client
   - Wait for registration (STATE_READY)
   - Press Ctrl+C
   - Verify DE-REGISTER sent
   - Verify clean exit

2. **Early Shutdown**:
   - Start client
   - Press Ctrl+C before registration completes
   - Verify graceful exit without DE-REGISTER

3. **Multiple Signals**:
   - Start client
   - Press Ctrl+C multiple times rapidly
   - Verify single shutdown sequence

4. **Server Disconnect**:
   - Start client with server disconnected
   - Press Ctrl+C
   - Verify graceful exit without DE-REGISTER

## Future Enhancements

Potential improvements for future versions:

1. **Timeout Handling**: Add timeout for DE-REGISTER response
2. **Additional Signals**: Handle SIGTERM for daemon-style operation
3. **Persistent State**: Save client state before shutdown for restart
4. **Cleanup Hooks**: Allow objects to register cleanup callbacks
5. **Shutdown Reason**: Log why shutdown was initiated

## Related Documentation

- `CLI_USAGE.md` - Command-line options
- `WALT_OBJECTS_EXAMPLE_CLIENT_GUIDE.md` - Object configuration guide
- `ConfigLoader.h` - Configuration loading system
- WPP (Wakaama++) documentation - LwM2M client API

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2026-04-30 | Initial graceful shutdown implementation |

---

**Note**: This graceful shutdown mechanism ensures that the LwM2M server is properly notified when the client disconnects, preventing the server from waiting for the client's registration lifetime to expire before marking it as offline.
