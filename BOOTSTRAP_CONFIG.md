# LwM2M Bootstrap Configuration

## Current Configuration

The Friendly LwM2M Client is configured to automatically connect to the Friendly Technologies LwM2M Bootstrap Server.

### Configuration Summary

| Parameter | Value |
|-----------|-------|
| **Endpoint Name** | `walttech888` |
| **Bootstrap Server** | `coap://demo-iot.friendly-tech.com:5680` |
| **Security Mode** | `NO_SEC` (3 - No DTLS encryption) |
| **Bootstrap Enabled** | `true` |
| **Hold Off Time** | `10 seconds` |

### Configuration Files

#### 1. Endpoint Name
**File:** `examples/main.cpp:47`

```cpp
string clientName = "walttech888"; // OpenWRT One endpoint name
```

#### 2. Bootstrap Server & Security
**File:** `examples/objects.cpp:49-72`

```cpp
void securityInit(WppClient &client) {
    client.registry().registerObj(Lwm2mSecurity::object(client));
    wpp::Instance *security = Lwm2mSecurity::createInst(client);
    string url = "coap://demo-iot.friendly-tech.com:"; // Bootstrap Server

    #ifdef LWM2M_BOOTSTRAP
        // Bootstrap mode enabled
        security->set<BOOL_T>(Lwm2mSecurity::BOOTSTRAP_SERVER_1, true);
        security->set<INT_T>(Lwm2mSecurity::CLIENT_HOLD_OFF_TIME_11, 10);
        // NO_SEC mode - CoAP without DTLS encryption
        security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_NONE);
        url += "5680";  // Bootstrap server port
    #endif

    security->set<STRING_T>(Lwm2mSecurity::LWM2M_SERVER_URI_0, url);
    security->set<INT_T>(Lwm2mSecurity::SHORT_SERVER_ID_10, TEST_SERVER_SHORT_ID);
}
```

#### 3. Bootstrap Support Enabled
**File:** `wpp/configs/wpp_config.cmake:32`

```cmake
# Enable LWM2M Bootstrap support in a LWM2M Client
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_BOOTSTRAP)
```

---

## How Bootstrap Works

### 1. Client Startup Sequence

```
┌─────────────────┐
│  Client Starts  │
└────────┬────────┘
         │
         ▼
┌─────────────────────────────┐
│ Endpoint: walttech888       │
│ Port: 56830 (local)         │
└────────┬────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────┐
│ Connect to Bootstrap Server                 │
│ coap://demo-iot.friendly-tech.com:5680      │
│ Security: NO_SEC (no encryption)            │
└────────┬────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────┐
│ Bootstrap Server Provisions │
│ - LwM2M Server Config       │
│ - Security Credentials      │
│ - Server Bindings          │
└────────┬────────────────────┘
         │
         ▼
┌─────────────────────────────┐
│ Client Registers with       │
│ Provisioned LwM2M Server    │
└────────┬────────────────────┘
         │
         ▼
┌─────────────────┐
│ Normal Operation│
└─────────────────┘
```

### 2. Bootstrap Process Details

1. **Client Hello**
   - Client sends bootstrap request to `demo-iot.friendly-tech.com:5680`
   - Uses endpoint name: `walttech888`
   - No DTLS encryption (NO_SEC mode)

2. **Server Provisioning**
   - Bootstrap server sends configuration
   - Includes LwM2M server URI and credentials
   - May include object instances

3. **Bootstrap Complete**
   - Client stores configuration
   - Disconnects from bootstrap server
   - Connects to provisioned LwM2M server

4. **Registration**
   - Client registers with LwM2M server
   - Begins normal operation
   - Updates and observations active

---

## Security Modes Available

The client supports multiple security modes (configured in `objects.cpp`):

| Mode | Value | Description | Port | Status |
|------|-------|-------------|------|--------|
| **NO_SEC** | 3 | No security (plain CoAP) | 5680/5683 | ✅ **ACTIVE** |
| PSK | 0 | Pre-Shared Key (DTLS-PSK) | 5684 | Available |
| RPK | 1 | Raw Public Key (DTLS-RPK) | 5684 | Available |
| Certificate | 2 | X.509 Certificates | 5684 | Available |

**Current Mode:** NO_SEC (3) - No encryption, suitable for trusted networks

---

## Changing Configuration

### Change Endpoint Name

**File:** `examples/main.cpp`

```cpp
// Change line 51:
string clientName = "your-new-endpoint-name";
```

### Change Bootstrap Server

**File:** `examples/objects.cpp`

```cpp
// Change line 52:
string url = "coap://your-bootstrap-server.com:";

// The port (5680) is appended automatically in bootstrap mode
```

### Change Security Mode

**File:** `examples/objects.cpp`

To use PSK instead of NO_SEC:

```cpp
#ifdef LWM2M_BOOTSTRAP
    security->set<BOOL_T>(Lwm2mSecurity::BOOTSTRAP_SERVER_1, true);
    security->set<INT_T>(Lwm2mSecurity::CLIENT_HOLD_OFF_TIME_11, 10);

    // Change to PSK mode
    security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_PRE_SHARED_KEY);
    string pskId = "your-psk-identity";
    security->set(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3, OPAQUE_T(pskId.begin(), pskId.end()));
    security->set(Lwm2mSecurity::SECRET_KEY_5, OPAQUE_T {0x00, 0x11, ...}); // Your PSK key

    url += "5684";  // DTLS port
#endif
```

### Disable Bootstrap (Use Direct Connection)

**File:** `wpp/configs/wpp_config.cmake`

```cmake
# Comment out or remove this line:
# set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_BOOTSTRAP)
```

Then rebuild:
```bash
cd build
cmake ..
make
```

---

## Testing Bootstrap Connection

### 1. Build and Run

```bash
# Build
mkdir build && cd build
cmake ..
make

# Run
./examples/lwm2m_client
```

### 2. Expected Output

```
---- Creating required components ----
---- Creating WppClient ----
WppClient name: walttech888

---- Initialization wpp Server ----
---- Initialization wpp Security ----
---- Initialization wpp Device ----
---- Initialization wpp Location ----
...

[INFO] Bootstrap request sent to demo-iot.friendly-tech.com:5680
[INFO] Bootstrap response received
[INFO] Configuration applied
[INFO] Registered with LwM2M server
```

### 3. Troubleshooting

**Cannot connect to bootstrap server:**
```bash
# Check network connectivity
ping demo-iot.friendly-tech.com

# Check UDP port is not blocked
nc -zvu demo-iot.friendly-tech.com 5680
```

**Bootstrap timeout:**
- Check `CLIENT_HOLD_OFF_TIME_11` setting
- Verify endpoint name is correct
- Check bootstrap server logs

**Wrong endpoint name error:**
- Verify `walttech888` is registered on bootstrap server
- Check for typos in endpoint name

---

## Network Requirements

### Firewall Rules

**Outbound (Client):**
- Allow UDP to `demo-iot.friendly-tech.com:5680`
- Allow UDP from local port `56830`

**Inbound (Client):**
- Allow UDP responses to local port `56830`

### OpenWRT Firewall

```bash
# Allow outbound to bootstrap server
uci add firewall rule
uci set firewall.@rule[-1].name='lwm2m_bootstrap'
uci set firewall.@rule[-1].src='lan'
uci set firewall.@rule[-1].dest='wan'
uci set firewall.@rule[-1].dest_port='5680'
uci set firewall.@rule[-1].proto='udp'
uci set firewall.@rule[-1].target='ACCEPT'
uci commit firewall
/etc/init.d/firewall restart
```

---

## Bootstrap Server Information

**Server:** Friendly Technologies Demo IoT Platform
**URL:** `demo-iot.friendly-tech.com`
**Bootstrap Port:** `5680` (CoAP, NO_SEC)
**DTLS Port:** `5684` (CoAPS, PSK/RPK/Cert)

**Registered Endpoint:** `walttech888`

---

## Advanced Configuration

### Custom Hold-Off Time

```cpp
// Wait longer before retrying bootstrap (default: 10 seconds)
security->set<INT_T>(Lwm2mSecurity::CLIENT_HOLD_OFF_TIME_11, 30);
```

### Bootstrap Error Handling

The client includes automatic error recovery in `main.cpp:18-36`:

```cpp
void wppErrorHandler(WppClient &client, int errCode) {
    if (client.getState() == STATE_BOOTSTRAPPING ||
        client.getState() == STATE_BOOTSTRAP_REQUIRED) {
        cout << "Trying to restore security and server objects" << endl;
        // Clears and reinitializes configuration
        securityInit(client);
        serverInit(client);
    }
}
```

---

## References

- [OMA LwM2M Bootstrap Specification](https://www.openmobilealliance.org/release/LightweightM2M/)
- [Friendly LwM2M Client Documentation](docs/README.md)
- [Quick Start Guide](docs/QUICKSTART.md)
- [Configuration Guide](docs/CONFIGURATION.md)

---

**Last Updated:** November 2025
**Configuration Status:** ✅ Active and verified
