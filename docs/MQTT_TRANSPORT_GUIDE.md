# MQTT Transport Guide

Complete guide for using the LwM2M MQTT Transport Binding in the Friendly LwM2M Client.

## Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Configuration](#configuration)
4. [Basic Usage](#basic-usage)
5. [Advanced Features](#advanced-features)
6. [Security](#security)
7. [Integration Examples](#integration-examples)
8. [Troubleshooting](#troubleshooting)
9. [Reference](#reference)

---

## Overview

### What is MQTT Transport?

The MQTT Transport Binding is defined in OMA LwM2M v1.2.1 Section 8 as an alternative to CoAP for LwM2M communication. It provides:

- **Firewall-friendly**: Works through NAT and corporate firewalls
- **Persistent sessions**: Reconnect without re-registration
- **Existing infrastructure**: Leverage your MQTT broker investment
- **High latency tolerance**: Better for satellite and cellular links

### When to Use MQTT

| Scenario | Recommended Transport |
|----------|----------------------|
| Constrained devices (< 128KB RAM) | CoAP/UDP |
| Direct device-to-server | CoAP/DTLS |
| Behind NAT/firewall | **MQTT** |
| Existing MQTT infrastructure | **MQTT** |
| High-latency networks (satellite) | **MQTT** |
| Frequent small updates | CoAP/UDP |
| Large payloads | **MQTT** or CoAP/TCP |

### Specification Compliance

| Feature | OMA Spec Reference | Support |
|---------|-------------------|---------|
| MQTT 3.1.1 | Section 8.1 | ✅ Full |
| MQTT 5.0 | Section 8.1 | ✅ Full |
| Topic structure | Section 8.2 | ✅ Full |
| CBOR encoding | Section 8.7 | ✅ Full |
| Bootstrap interface | Section 8.3.1 | ✅ Full |
| Registration interface | Section 8.3.2 | ✅ Full |
| Device Management | Section 8.3.3 | ✅ Full |
| Information Reporting | Section 8.3.4 | ✅ Full |

---

## Architecture

### Topic Structure

```
{prefix}/{direction}/{endpoint_name}[/{operation}]
```

| Direction | Purpose | Example |
|-----------|---------|---------|
| `rd` | Registration | `lwm2m/rd/device001` |
| `bs` | Bootstrap | `lwm2m/bs/device001/req` |
| `dp` | Data/Commands | `lwm2m/dp/device001/notify` |

### Message Flow

```
┌──────────┐                              ┌──────────────┐
│  Client  │                              │ LwM2M Server │
└────┬─────┘                              └──────┬───────┘
     │                                           │
     │  CONNECT (TLS/PSK)                        │
     │─────────────────────────────────────────► │
     │                                           │
     │  SUBSCRIBE lwm2m/dp/{endpoint}/#          │
     │─────────────────────────────────────────► │
     │                                           │
     │  PUBLISH lwm2m/rd/{endpoint}              │
     │  (Registration: endpoint, lifetime, ...)   │
     │─────────────────────────────────────────► │
     │                                           │
     │  PUBLISH lwm2m/dp/{endpoint}/read         │
     │  (Read request: /3/0/0)                   │
     │ ◄─────────────────────────────────────────│
     │                                           │
     │  PUBLISH lwm2m/dp/{endpoint}/resp         │
     │  (Response: "Friendly Device")            │
     │─────────────────────────────────────────► │
     │                                           │
```

### LwM2M Objects for MQTT

| Object ID | Name | Purpose |
|-----------|------|---------|
| 23 | LwM2M COSE | MQTT security configuration |
| 24 | MQTT Server | Broker connection settings |

---

## Configuration

### Basic Configuration

```cpp
#include "transport/mqtt_transport.h"

using namespace lwm2m::transport;

// Create configuration
MqttTransport::Config config;

// Server settings (Object 24)
config.server.brokerUri = "mqtt.example.com";
config.server.brokerPort = 8883;
config.server.clientId = "lwm2m-device-001";
config.server.topicPrefix = "lwm2m";
config.server.cleanSession = false;
config.server.keepAlive = 60;
config.server.qos = 1;  // AT_LEAST_ONCE
config.server.protocolVersion = 5;  // MQTT 5.0

// Security settings (Object 23)
config.security.securityMode = MqttSecurityMode::PSK;
config.security.pskIdentity = "device001";
config.security.pskKey = "my-secret-key-16bytes";
config.security.verifyPeer = true;
```

### Configuration Options

#### MqttServerConfig (Object 24)

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `brokerUri` | string | "" | Broker hostname/IP |
| `brokerPort` | uint16_t | 8883 | Broker port |
| `clientId` | string | "" | MQTT client identifier |
| `cleanSession` | bool | false | Clean session flag |
| `keepAlive` | uint16_t | 60 | Keep-alive interval (seconds) |
| `qos` | uint8_t | 1 | Default QoS level (0, 1, 2) |
| `topicPrefix` | string | "lwm2m" | Topic prefix |
| `protocolVersion` | uint8_t | 4 | MQTT version (4=3.1.1, 5=5.0) |
| `username` | string | "" | Optional username |
| `password` | string | "" | Optional password |
| `sessionExpiry` | uint32_t | 0 | Session expiry (MQTT 5.0) |

#### MqttCoseConfig (Object 23)

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `securityMode` | enum | PSK | Security mode |
| `pskIdentity` | string | "" | PSK identity |
| `pskKey` | string | "" | PSK key |
| `certificateChain` | string | "" | Path to client cert |
| `privateKey` | string | "" | Path to private key |
| `caCertificate` | string | "" | Path to CA cert |
| `sni` | string | "" | Server Name Indication |
| `verifyPeer` | bool | true | Verify server cert |

---

## Basic Usage

### Connect and Register

```cpp
#include "transport/mqtt_transport.h"

using namespace lwm2m::transport;

int main() {
    // Configure
    MqttTransport::Config config;
    config.server.brokerUri = "mqtt.example.com";
    config.server.brokerPort = 8883;
    config.server.clientId = "my-device";
    config.security.securityMode = MqttSecurityMode::PSK;
    config.security.pskIdentity = "device001";
    config.security.pskKey = "secretkey1234567";

    // Create transport
    MqttTransport mqtt(config);

    // Connect
    auto result = mqtt.connect();
    if (result != MqttResult::SUCCESS) {
        std::cerr << "Connection failed: " << static_cast<int>(result) << std::endl;
        return 1;
    }

    // Register with LwM2M server
    MqttRegistrationParams params;
    params.endpointName = "my-device";
    params.lifetime = 86400;  // 24 hours
    params.lwm2mVersion = "1.2";
    params.bindingMode = "U";  // UDP binding reported
    params.queueMode = true;
    params.objectLinks = {
        "</1/0>",
        "</3/0>",
        "</5/0>",
        "</33410/0>"
    };

    mqtt.registerClient(params);

    // Process messages
    while (running) {
        mqtt.processMessages(1000);  // 1 second timeout
    }

    // Clean disconnect
    mqtt.deregister();
    mqtt.disconnect(true);

    return 0;
}
```

### Handle Server Requests

```cpp
// Handle Read requests
mqtt.onRead([](const LwM2MPayload& request, auto respond) {
    std::cout << "Read: " << request.path << std::endl;

    LwM2MPayload response;
    response.token = request.token;

    if (request.path == "/3/0/0") {
        // Device manufacturer
        response.responseCode = LwM2MResponseCode::CONTENT;
        response.contentFormat = ContentFormat::TEXT_PLAIN;
        response.data = stringToBytes("Friendly Technologies");
    } else {
        response.responseCode = LwM2MResponseCode::NOT_FOUND;
    }

    respond(response);
});

// Handle Write requests
mqtt.onWrite([](const LwM2MPayload& request, auto respond) {
    std::cout << "Write: " << request.path << std::endl;
    std::cout << "Data: " << bytesToString(request.data) << std::endl;

    LwM2MPayload response;
    response.token = request.token;
    response.responseCode = LwM2MResponseCode::CHANGED;

    respond(response);
});

// Handle Execute requests
mqtt.onExecute([](const LwM2MPayload& request, auto respond) {
    std::cout << "Execute: " << request.path << std::endl;

    if (request.path == "/3/0/4") {
        // Reboot
        std::cout << "Rebooting device..." << std::endl;
        // system("reboot");
    }

    LwM2MPayload response;
    response.token = request.token;
    response.responseCode = LwM2MResponseCode::CHANGED;

    respond(response);
});
```

### Send Notifications

```cpp
// Send observation notification
std::vector<uint8_t> temperatureData = {0x19, 0x00, 0x17};  // CBOR: 23°C

mqtt.sendNotification(
    "/3303/0/5700",           // Temperature sensor value
    temperatureData,
    ContentFormat::SENML_CBOR
);

// Send client-initiated data (Send operation)
std::vector<std::string> paths = {"/3303/0/5700", "/3303/0/5701"};
std::vector<uint8_t> sensorData = encodeSenMLData(paths, values);

mqtt.sendData(paths, sensorData, ContentFormat::SENML_CBOR);
```

---

## Advanced Features

### QoS Levels

```cpp
// QoS 0 - At most once (fire and forget)
mqtt.setDefaultQoS(MqttQoS::AT_MOST_ONCE);

// QoS 1 - At least once (guaranteed delivery)
mqtt.setDefaultQoS(MqttQoS::AT_LEAST_ONCE);

// QoS 2 - Exactly once (no duplicates)
mqtt.setDefaultQoS(MqttQoS::EXACTLY_ONCE);

// Per-message QoS
mqtt.sendNotification(path, data, format, MqttQoS::EXACTLY_ONCE);
```

### Session Persistence

```cpp
// Enable persistent session
config.server.cleanSession = false;
config.server.sessionExpiry = 86400;  // MQTT 5.0: 24 hours

// On reconnect, messages queued during disconnect are delivered
mqtt.setOnReconnect([]() {
    std::cout << "Reconnected, processing queued messages..." << std::endl;
});
```

### Last Will and Testament

```cpp
// Configure LWT for server to detect client death
config.server.willTopic = "lwm2m/status/my-device";
config.server.willMessage = "offline";
config.server.willQos = 1;
config.server.willRetain = true;
```

### Composite Operations

```cpp
// Handle composite read
mqtt.onReadComposite([](const LwM2MPayload& request, auto respond) {
    // request.paths contains multiple paths
    std::vector<std::string> paths = request.compositePaths;

    LwM2MPayload response;
    response.token = request.token;
    response.responseCode = LwM2MResponseCode::CONTENT;
    response.contentFormat = ContentFormat::SENML_CBOR;

    // Build composite response
    response.data = buildCompositeResponse(paths);

    respond(response);
});

// Handle composite write
mqtt.onWriteComposite([](const LwM2MPayload& request, auto respond) {
    // Process multiple write operations
    auto operations = parseCompositeWrite(request.data);

    for (const auto& op : operations) {
        writeResource(op.path, op.value);
    }

    LwM2MPayload response;
    response.token = request.token;
    response.responseCode = LwM2MResponseCode::CHANGED;

    respond(response);
});
```

### Statistics and Monitoring

```cpp
// Get transport statistics
auto stats = mqtt.getStatistics();

std::cout << "Messages sent: " << stats.messagesSent << std::endl;
std::cout << "Messages received: " << stats.messagesReceived << std::endl;
std::cout << "Bytes sent: " << stats.bytesSent << std::endl;
std::cout << "Bytes received: " << stats.bytesReceived << std::endl;
std::cout << "Errors: " << stats.errorCount << std::endl;
std::cout << "Reconnections: " << stats.reconnections << std::endl;

// Reset statistics
mqtt.resetStatistics();
```

---

## Security

### PSK (Pre-Shared Key)

```cpp
config.security.securityMode = MqttSecurityMode::PSK;
config.security.pskIdentity = "device-001";
config.security.pskKey = "0102030405060708090A0B0C0D0E0F10";  // Hex or string
```

### Certificate-Based

```cpp
config.security.securityMode = MqttSecurityMode::CERTIFICATE;
config.security.certificateChain = "/etc/lwm2m/client.pem";
config.security.privateKey = "/etc/lwm2m/client.key";
config.security.caCertificate = "/etc/lwm2m/ca.pem";
config.security.sni = "mqtt.example.com";
config.security.verifyPeer = true;
```

### Token-Based (OAuth2/JWT)

```cpp
config.security.securityMode = MqttSecurityMode::TOKEN;
config.security.authToken = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9...";

// Token refresh callback
mqtt.setTokenRefreshCallback([&config]() {
    config.security.authToken = refreshOAuth2Token();
    return config.security.authToken;
});
```

---

## Integration Examples

### AWS IoT Core

```cpp
MqttTransport::Config config;
config.server.brokerUri = "a1b2c3d4e5f6g7.iot.us-west-2.amazonaws.com";
config.server.brokerPort = 8883;
config.server.clientId = "lwm2m-device-001";
config.server.protocolVersion = 4;  // MQTT 3.1.1

config.security.securityMode = MqttSecurityMode::CERTIFICATE;
config.security.certificateChain = "/etc/lwm2m/device.pem.crt";
config.security.privateKey = "/etc/lwm2m/device.pem.key";
config.security.caCertificate = "/etc/lwm2m/AmazonRootCA1.pem";
```

### Azure IoT Hub

```cpp
MqttTransport::Config config;
config.server.brokerUri = "my-hub.azure-devices.net";
config.server.brokerPort = 8883;
config.server.clientId = "device-001";
config.server.username = "my-hub.azure-devices.net/device-001/?api-version=2021-04-12";

config.security.securityMode = MqttSecurityMode::TOKEN;
config.security.authToken = generateSasToken();
```

### Eclipse Mosquitto

```cpp
MqttTransport::Config config;
config.server.brokerUri = "localhost";
config.server.brokerPort = 8883;
config.server.clientId = "lwm2m-test-device";

config.security.securityMode = MqttSecurityMode::PSK;
config.security.pskIdentity = "device001";
config.security.pskKey = "secretpassword";
```

---

## Troubleshooting

### Connection Issues

| Error | Cause | Solution |
|-------|-------|----------|
| `ERROR_TIMEOUT` | Network unreachable | Check firewall, DNS |
| `ERROR_TLS` | Certificate error | Verify certs, check CA |
| `ERROR_AUTH` | Bad credentials | Check PSK/username/password |
| `ERROR_PROTOCOL` | Version mismatch | Try MQTT 3.1.1 |

### Debug Logging

```cpp
// Enable debug output
mqtt.setLogLevel(MqttLogLevel::DEBUG);

mqtt.setLogCallback([](MqttLogLevel level, const std::string& message) {
    std::cout << "[MQTT] " << message << std::endl;
});
```

### Network Diagnostics

```bash
# Test broker connectivity
mosquitto_pub -h mqtt.example.com -p 8883 \
    --cafile ca.pem --cert client.pem --key client.key \
    -t test -m "hello"

# Monitor MQTT traffic
mosquitto_sub -h localhost -t 'lwm2m/#' -v
```

---

## Reference

### MqttResult Codes

| Code | Description |
|------|-------------|
| `SUCCESS` | Operation completed |
| `ERROR_TIMEOUT` | Operation timed out |
| `ERROR_NETWORK` | Network error |
| `ERROR_TLS` | TLS/SSL error |
| `ERROR_AUTH` | Authentication failed |
| `ERROR_PROTOCOL` | Protocol error |
| `ERROR_NOT_CONNECTED` | Not connected |

### LwM2M Response Codes

| Code | Value | Description |
|------|-------|-------------|
| `CREATED` | 2.01 | Resource created |
| `DELETED` | 2.02 | Resource deleted |
| `CHANGED` | 2.04 | Resource changed |
| `CONTENT` | 2.05 | Content returned |
| `BAD_REQUEST` | 4.00 | Bad request |
| `UNAUTHORIZED` | 4.01 | Unauthorized |
| `NOT_FOUND` | 4.04 | Resource not found |
| `METHOD_NOT_ALLOWED` | 4.05 | Method not allowed |

### Content Formats

| Format | ID | Use Case |
|--------|-----|----------|
| `TEXT_PLAIN` | 0 | Simple values |
| `OPAQUE` | 42 | Binary data |
| `LWM2M_TLV` | 11542 | LwM2M encoding |
| `LWM2M_JSON` | 11543 | JSON encoding |
| `SENML_JSON` | 110 | Sensor data (JSON) |
| `SENML_CBOR` | 112 | Sensor data (CBOR) |
| `LWM2M_CBOR` | 11544 | LwM2M CBOR |

---

*For more information, see the [API Reference](API_REFERENCE.md) or the [OMA LwM2M Specification](https://www.openmobilealliance.org/release/LightweightM2M/).*
