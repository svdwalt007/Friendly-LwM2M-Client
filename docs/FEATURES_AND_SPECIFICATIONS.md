# Friendly LwM2M Client v1.1.0
## Complete Features & Specifications Reference

**Document Version:** 1.1.0  
**Release Date:** January 7, 2026  
**Friendly Technologies - The IoT & Device Management Company**

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Standards Compliance](#2-standards-compliance)
3. [Transport Layer Specifications](#3-transport-layer-specifications)
4. [MQTT Transport Binding](#4-mqtt-transport-binding)
5. [Security Specifications](#5-security-specifications)
6. [LwM2M Protocol Operations](#6-lwm2m-protocol-operations)
7. [Content Formats & Encoding](#7-content-formats--encoding)
8. [LwM2M Object Model](#8-lwm2m-object-model)
9. [Delta Firmware Update System](#9-delta-firmware-update-system)
10. [A/B Partition & Rollback System](#10-ab-partition--rollback-system)
11. [Edge AI Inference System](#11-edge-ai-inference-system)
12. [Platform Abstraction Layer](#12-platform-abstraction-layer)
13. [Build System & Dependencies](#13-build-system--dependencies)
14. [Performance Specifications](#14-performance-specifications)
15. [API Summary](#15-api-summary)
16. [Server Compatibility Matrix](#16-server-compatibility-matrix)

---

## 1. Executive Summary

### 1.1 Product Overview

The Friendly LwM2M Client is a production-ready, feature-rich implementation of the OMA Lightweight M2M (LwM2M) protocol, designed for IoT device management at scale. It provides comprehensive support for device lifecycle management, firmware updates, and emerging capabilities like MQTT transport and on-device AI inference.

### 1.2 Key Differentiators

| Feature | Description |
|---------|-------------|
| **Full LwM2M v1.2.2 Compliance** | Complete implementation of latest OMA specification |
| **Multi-Transport** | CoAP (UDP/TCP) + MQTT transport bindings |
| **Delta Firmware Updates** | 70-90% bandwidth reduction with BSDIFF/VCDIFF |
| **A/B Rollback** | Safe updates with automatic recovery |
| **Edge AI** | On-device ML inference with TFLite/ONNX |
| **Platform Portable** | Linux, OpenWRT, embedded targets |

### 1.3 Version History

| Version | Date | Major Features |
|---------|------|----------------|
| 1.1.0 | 2026-01-07 | MQTT Transport, Edge AI Inference |
| 1.0.0 | 2025-12-15 | Delta FOTA, A/B Rollback, LwM2M v1.2.2 |
| 0.9.0 | 2025-11-01 | Initial beta release |

---

## 2. Standards Compliance

### 2.1 OMA LwM2M Specifications

| Specification | Version | Document ID | Support |
|---------------|---------|-------------|---------|
| LwM2M Core | 1.0 | OMA-TS-LightweightM2M-V1_0 | ✅ Full |
| LwM2M Core | 1.1 | OMA-TS-LightweightM2M-V1_1 | ✅ Full |
| LwM2M Core | 1.2.1 | OMA-TS-LightweightM2M_Core-V1_2_1 | ✅ Full |
| LwM2M Core | 1.2.2 | OMA-TS-LightweightM2M_Core-V1_2_2 | ✅ Full |
| LwM2M Transport | 1.2.1 | OMA-TS-LightweightM2M_Transport-V1_2_1 | ✅ Full |
| LwM2M Transport | 1.2.2 | OMA-TS-LightweightM2M_Transport-V1_2_2 | ✅ Full |

### 2.2 IETF RFCs

| RFC | Title | Application |
|-----|-------|-------------|
| RFC 7252 | Constrained Application Protocol (CoAP) | Primary protocol |
| RFC 7959 | Block-Wise Transfers in CoAP | Large payload handling |
| RFC 8323 | CoAP over TCP, TLS, and WebSockets | Reliable transport |
| RFC 6347 | Datagram Transport Layer Security (DTLS) 1.2 | UDP security |
| RFC 8446 | Transport Layer Security (TLS) 1.3 | TCP security |
| RFC 8613 | Object Security for CoAP (OSCORE) | Application security |
| RFC 8428 | Sensor Measurement Lists (SenML) | Data encoding |
| RFC 8949 | CBOR (Concise Binary Object Representation) | Binary encoding |
| RFC 3284 | VCDIFF Generic Differencing | Delta updates |

### 2.3 Other Standards

| Standard | Application |
|----------|-------------|
| MQTT 3.1.1 (OASIS) | Pub/sub transport |
| MQTT 5.0 (OASIS) | Enhanced pub/sub transport |
| TensorFlow Lite | ML inference runtime |
| ONNX Runtime | Cross-platform ML |
| Semantic Versioning 2.0 | Version management |

---

## 3. Transport Layer Specifications

### 3.1 Supported Transports

| Transport | Protocol Stack | Security | Status |
|-----------|---------------|----------|--------|
| CoAP/UDP | CoAP → UDP → IP | Optional DTLS | ✅ Production |
| CoAP/DTLS | CoAP → DTLS → UDP → IP | DTLS 1.2/1.3 | ✅ Production |
| CoAP/TCP | CoAP → TCP → IP | Optional TLS | ✅ Production |
| CoAP/TLS | CoAP → TLS → TCP → IP | TLS 1.2/1.3 | ✅ Production |
| MQTT/TCP | LwM2M → MQTT → TCP → IP | Optional TLS | ✅ Production |
| MQTT/TLS | LwM2M → MQTT → TLS → TCP → IP | TLS 1.2/1.3 | ✅ Production |
| CoAP/WebSocket | CoAP → WS → TCP → IP | Optional WSS | 📋 Planned |

### 3.2 CoAP Implementation

#### 3.2.1 Message Types

| Type | Code | Description | Support |
|------|------|-------------|---------|
| CON | 0 | Confirmable | ✅ |
| NON | 1 | Non-confirmable | ✅ |
| ACK | 2 | Acknowledgement | ✅ |
| RST | 3 | Reset | ✅ |

#### 3.2.2 Method Codes

| Method | Code | Description | Support |
|--------|------|-------------|---------|
| GET | 0.01 | Read resource | ✅ |
| POST | 0.02 | Create/Execute | ✅ |
| PUT | 0.03 | Write resource | ✅ |
| DELETE | 0.04 | Delete instance | ✅ |
| FETCH | 0.05 | Composite read | ✅ |
| PATCH | 0.06 | Partial update | ✅ |
| iPATCH | 0.07 | Idempotent patch | ✅ |

#### 3.2.3 Response Codes

| Class | Codes | Description |
|-------|-------|-------------|
| 2.xx | 2.01-2.05 | Success responses |
| 4.xx | 4.00-4.15 | Client errors |
| 5.xx | 5.00-5.05 | Server errors |

### 3.3 Block-wise Transfer (RFC 7959)

#### 3.3.1 Block Size Options

| SZX | Block Size | Use Case |
|-----|------------|----------|
| 0 | 16 bytes | Extremely constrained |
| 1 | 32 bytes | Very constrained |
| 2 | 64 bytes | Constrained |
| 3 | 128 bytes | Default minimum |
| 4 | 256 bytes | Standard |
| 5 | 512 bytes | Recommended |
| 6 | 1024 bytes | Maximum standard |
| 7 | 2048+ bytes | BERT mode |

#### 3.3.2 BERT (Block-wise Extension for Robust Transport)

| Parameter | Value | Description |
|-----------|-------|-------------|
| Minimum BERT size | 2048 bytes | SZX=7 indicator |
| Default BERT size | 8192 bytes | Optimal for TCP |
| Maximum BERT size | 1048576 bytes | 1MB limit |
| Supported transports | TCP, TLS | Reliable only |

#### 3.3.3 Resumable Transfer Features

| Feature | Specification |
|---------|---------------|
| Transfer identification | ETag header (8 bytes) |
| Integrity verification | SHA256 checksum |
| Missing block detection | Bitmap tracking |
| Retry mechanism | Exponential backoff |
| Default retries | 5 attempts |
| Default timeout | 600 seconds |
| Progress callback | Per-block notification |

---

## 4. MQTT Transport Binding

### 4.1 Protocol Support

| Feature | MQTT 3.1.1 | MQTT 5.0 |
|---------|------------|----------|
| Basic pub/sub | ✅ | ✅ |
| QoS 0 (At most once) | ✅ | ✅ |
| QoS 1 (At least once) | ✅ | ✅ |
| QoS 2 (Exactly once) | ✅ | ✅ |
| Retained messages | ✅ | ✅ |
| Last Will Testament | ✅ | ✅ |
| Clean Session | ✅ | ✅ |
| Session Expiry | ❌ | ✅ |
| Message Expiry | ❌ | ✅ |
| Topic Alias | ❌ | ✅ |
| User Properties | ❌ | ✅ |
| Subscription ID | ❌ | ✅ |

### 4.2 Topic Structure (OMA Section 8.2)

```
{prefix}/{direction}/{endpoint_name}[/{operation}]
```

| Component | Description | Example |
|-----------|-------------|---------|
| prefix | Configurable prefix | `lwm2m` |
| direction | Message direction | `rd`, `bs`, `dp` |
| endpoint_name | Client identifier | `device-001` |
| operation | Optional operation | `upd`, `notify`, `req` |

#### 4.2.1 Direction Types

| Direction | Full Name | Purpose |
|-----------|-----------|---------|
| `rd` | Registration Directory | Registration interface |
| `bs` | Bootstrap | Bootstrap interface |
| `dp` | Data/Push | Device management & reporting |

#### 4.2.2 Topic Examples

| Interface | Direction | Topic Pattern |
|-----------|-----------|---------------|
| Registration | Client→Server | `lwm2m/rd/{endpoint}` |
| Reg Update | Client→Server | `lwm2m/rd/{endpoint}/upd` |
| Deregistration | Client→Server | `lwm2m/rd/{endpoint}/dereg` |
| Bootstrap Request | Client→Server | `lwm2m/bs/{endpoint}/req` |
| Bootstrap Commands | Server→Client | `lwm2m/bs/{endpoint}` |
| Read/Write/Execute | Server→Client | `lwm2m/dp/{endpoint}` |
| Response | Client→Server | `lwm2m/dp/{endpoint}/resp` |
| Notification | Client→Server | `lwm2m/dp/{endpoint}/notify` |
| Send | Client→Server | `lwm2m/dp/{endpoint}/data` |

### 4.3 Message Encoding (Section 8.7)

#### 4.3.1 CBOR Payload Structure

| Key | Type | Field | Description |
|-----|------|-------|-------------|
| 0 | Integer | Operation | LwM2M operation code |
| 1 | Integer | Token | Request/response correlation |
| 2 | String | Path | Resource path (e.g., "/3/0/9") |
| 3 | Integer | Content-Format | CoAP content format ID |
| 4 | Bytes | Payload | Encoded data |
| 5 | Integer | Response Code | CoAP response code |
| 6 | String | Error Message | Human-readable error |
| 10 | String | Endpoint Name | Client endpoint |
| 11 | Integer | Lifetime | Registration lifetime |
| 12 | String | LwM2M Version | Protocol version |
| 13 | String | Binding | Binding mode |
| 14 | Boolean | Queue Mode | Queue mode flag |
| 15 | Array | Objects | Object links |
| 16 | String | Alt Path | Alternative path |
| 20 | Integer | Observe Sequence | Notification counter |
| 21 | Integer | Timestamp | Message timestamp |

#### 4.3.2 Operation Codes

| Code | Operation | Direction |
|------|-----------|-----------|
| 1 | Read | Server→Client |
| 2 | Write | Server→Client |
| 3 | Execute | Server→Client |
| 4 | Create | Server→Client |
| 5 | Delete | Server→Client |
| 6 | Discover | Server→Client |
| 7 | Write-Attributes | Server→Client |
| 8 | Observe | Server→Client |
| 9 | Cancel-Observe | Server→Client |
| 10 | Read-Composite | Server→Client |
| 11 | Write-Composite | Server→Client |
| 12 | Observe-Composite | Server→Client |
| 20 | Register | Client→Server |
| 21 | Update | Client→Server |
| 22 | Deregister | Client→Server |
| 23 | Send | Client→Server |
| 24 | Notify | Client→Server |
| 30 | Bootstrap-Request | Client→Server |
| 31 | Bootstrap-Write | Server→Client |
| 32 | Bootstrap-Discover | Server→Client |
| 33 | Bootstrap-Delete | Server→Client |
| 34 | Bootstrap-Finish | Server→Client |

### 4.4 MQTT Configuration Objects

#### 4.4.1 MQTT Server Object (ID: 24)

| Resource | ID | Type | Operations | Mandatory | Description |
|----------|-----|------|------------|-----------|-------------|
| Short Server ID | 0 | Integer | R | Yes | Server identifier |
| Broker URI | 1 | String | RW | Yes | MQTT broker address |
| Broker Port | 2 | Integer | RW | Yes | Broker port (default 8883) |
| Client ID | 3 | String | RW | Yes | MQTT client identifier |
| Clean Session | 4 | Boolean | RW | No | Clean session flag |
| Keep Alive | 5 | Integer | RW | No | Keep-alive interval (seconds) |
| QoS | 6 | Integer | RW | No | Default QoS level |
| Protocol Version | 7 | Integer | RW | No | MQTT version (4=3.1.1, 5=5.0) |
| Username | 8 | String | RW | No | Authentication username |
| Password | 9 | String | W | No | Authentication password |
| Topic Prefix | 10 | String | RW | No | Topic prefix (default "lwm2m") |
| Retain | 11 | Boolean | RW | No | Retain flag for publishes |
| Session Expiry | 12 | Integer | RW | No | MQTT 5.0 session expiry |
| Max Packet Size | 13 | Integer | RW | No | Maximum packet size |
| Receive Maximum | 14 | Integer | RW | No | Receive maximum (5.0) |

#### 4.4.2 LwM2M COSE Object (ID: 23)

| Resource | ID | Type | Operations | Mandatory | Description |
|----------|-----|------|------------|-----------|-------------|
| Short Server ID | 0 | Integer | R | Yes | Server identifier |
| Security Mode | 1 | Integer | RW | Yes | 0=None, 1=PSK, 2=RPK, 3=Cert, 4=Token |
| Public Key/Identity | 2 | Opaque | RW | No | PSK identity or public key |
| Secret Key | 3 | Opaque | W | No | PSK or private key |
| Server Public Key | 4 | Opaque | RW | No | Server's public key |
| Certificate Chain | 5 | String | RW | No | Client certificate path |
| CA Certificate | 6 | String | RW | No | CA certificate path |
| SNI | 7 | String | RW | No | Server Name Indication |
| Verify Peer | 8 | Boolean | RW | No | Verify server certificate |
| ALPN | 9 | String | RW | No | Application protocol |
| Token | 10 | String | W | No | OAuth/JWT token |
| Token Endpoint | 11 | String | RW | No | Token refresh endpoint |

### 4.5 QoS Mapping

| LwM2M Requirement | Recommended MQTT QoS |
|-------------------|---------------------|
| Registration | QoS 1 (At least once) |
| Bootstrap | QoS 1 (At least once) |
| Read/Write/Execute | QoS 1 (At least once) |
| Observe Notifications | QoS 0 or 1 (configurable) |
| Firmware Download | QoS 1 (At least once) |
| Critical Operations | QoS 2 (Exactly once) |

---

## 5. Security Specifications

### 5.1 DTLS Security Modes

| Mode | Value | Key Material | Use Case |
|------|-------|--------------|----------|
| No Security | 0 | None | Development only |
| PSK | 1 | Identity + Key | Simple deployment |
| RPK | 2 | Public/Private keypair | IoT production |
| Certificate | 3 | X.509 + Private key | Enterprise |
| Certificate + EST | 4 | X.509 + EST enrollment | Auto-provisioning |

### 5.2 Cipher Suites

#### 5.2.1 PSK Cipher Suites

| Cipher Suite | Security Level | Performance |
|--------------|----------------|-------------|
| TLS_PSK_WITH_AES_128_CCM_8 | Good | Excellent |
| TLS_PSK_WITH_AES_128_CBC_SHA256 | Good | Good |
| TLS_PSK_WITH_AES_256_CCM | Better | Good |
| TLS_PSK_WITH_AES_256_CBC_SHA384 | Better | Moderate |

#### 5.2.2 Certificate Cipher Suites

| Cipher Suite | Security Level | Performance |
|--------------|----------------|-------------|
| TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 | Better | Good |
| TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 | Better | Good |
| TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 | Best | Moderate |
| TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 | Best | Moderate |

### 5.3 TLS Library Support

| Library | Version | Features | Platform |
|---------|---------|----------|----------|
| mbedTLS | 3.0+ | Full DTLS/TLS, all modes | Primary |
| TinyDTLS | 0.9+ | DTLS only, PSK/RPK | Constrained |
| OpenSSL | 1.1.1+ | Full TLS, certificates | Optional |

### 5.4 OSCORE (RFC 8613)

| Feature | Support |
|---------|---------|
| OSCORE Object (ID: 21) | ✅ |
| AEAD algorithms | AES-CCM-16-64-128 |
| Key derivation | HKDF-SHA-256 |
| Replay protection | Sequence number window |
| Group OSCORE | 📋 Planned |

---

## 6. LwM2M Protocol Operations

### 6.1 Bootstrap Interface

| Operation | Direction | Description | Support |
|-----------|-----------|-------------|---------|
| Bootstrap-Request | C→S | Request bootstrap | ✅ |
| Bootstrap-Write | S→C | Write bootstrap info | ✅ |
| Bootstrap-Read | S→C | Read bootstrap info | ✅ |
| Bootstrap-Discover | S→C | Discover objects | ✅ |
| Bootstrap-Delete | S→C | Delete instances | ✅ |
| Bootstrap-Finish | S→C | Complete bootstrap | ✅ |
| Bootstrap-Pack | S→C | Packed bootstrap | ✅ |

### 6.2 Registration Interface

| Operation | Direction | Description | Support |
|-----------|-----------|-------------|---------|
| Register | C→S | Initial registration | ✅ |
| Update | C→S | Registration update | ✅ |
| De-register | C→S | Graceful removal | ✅ |

#### 6.2.1 Registration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| ep | String | Required | Endpoint name |
| lt | Integer | 86400 | Lifetime (seconds) |
| lwm2m | String | "1.2" | LwM2M version |
| b | String | "U" | Binding mode |
| Q | Flag | - | Queue mode |
| sms | String | - | SMS number |
| objects | Links | Required | Object list |

### 6.3 Device Management Interface

| Operation | Direction | CoAP Method | Description | Support |
|-----------|-----------|-------------|-------------|---------|
| Read | S→C | GET | Read value | ✅ |
| Discover | S→C | GET (Accept: link) | Discover structure | ✅ |
| Write | S→C | PUT/POST | Write value | ✅ |
| Write-Attributes | S→C | PUT (query) | Set attributes | ✅ |
| Execute | S→C | POST | Execute resource | ✅ |
| Create | S→C | POST | Create instance | ✅ |
| Delete | S→C | DELETE | Delete instance | ✅ |
| Read-Composite | S→C | FETCH | Multi-resource read | ✅ |
| Write-Composite | S→C | iPATCH | Multi-resource write | ✅ |

### 6.4 Information Reporting Interface

| Operation | Direction | Description | Support |
|-----------|-----------|-------------|---------|
| Observe | S→C | Subscribe to changes | ✅ |
| Cancel Observation | S→C | Unsubscribe | ✅ |
| Notify | C→S | Send notification | ✅ |
| Observe-Composite | S→C | Multi-resource observe | ✅ |
| Send | C→S | Client-initiated push | ✅ |

#### 6.4.1 Notification Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| pmin | Integer | Minimum period (seconds) |
| pmax | Integer | Maximum period (seconds) |
| gt | Float | Greater than threshold |
| lt | Float | Less than threshold |
| st | Float | Step threshold |
| epmin | Integer | Minimum evaluation period |
| epmax | Integer | Maximum evaluation period |
| edge | Integer | Edge notification mode |
| con | Integer | Confirmable notification |
| hqmax | Integer | Historical queue max |

---

## 7. Content Formats & Encoding

### 7.1 Supported Content Formats

| Format | CoAP ID | MIME Type | LwM2M Version | Support |
|--------|---------|-----------|---------------|---------|
| Plain Text | 0 | text/plain | 1.0+ | ✅ |
| Link Format | 40 | application/link-format | 1.0+ | ✅ |
| Opaque | 42 | application/octet-stream | 1.0+ | ✅ |
| TLV | 11542 | application/vnd.oma.lwm2m+tlv | 1.0+ | ✅ |
| JSON | 11543 | application/vnd.oma.lwm2m+json | 1.0+ | ✅ |
| SenML JSON | 110 | application/senml+json | 1.1+ | ✅ |
| SenML CBOR | 112 | application/senml+cbor | 1.1+ | ✅ |
| LwM2M CBOR | 11544 | application/vnd.oma.lwm2m+cbor | 1.1+ | ✅ |

### 7.2 TLV Encoding

| Type | ID Bits | ID Length | Length Bits | Description |
|------|---------|-----------|-------------|-------------|
| Object Instance | 00 | 8 or 16 | Variable | Instance container |
| Resource Instance | 01 | 8 or 16 | Variable | Multi-instance resource |
| Multiple Resource | 10 | 8 or 16 | Variable | Resource array |
| Resource | 11 | 8 or 16 | Variable | Single resource |

### 7.3 SenML Structure

```json
[
  {"bn": "/3/0/", "bt": 1704633600},
  {"n": "0", "vs": "Friendly Technologies"},
  {"n": "1", "vs": "LwM2M-Client"},
  {"n": "2", "vs": "1.1.0"},
  {"n": "9", "v": 87}
]
```

---

## 8. LwM2M Object Model

### 8.1 Mandatory Objects

| ID | Name | URN | Instances | Description |
|----|------|-----|-----------|-------------|
| 0 | LwM2M Security | urn:oma:lwm2m:oma:0 | Multiple | Server credentials |
| 1 | LwM2M Server | urn:oma:lwm2m:oma:1 | Multiple | Server settings |
| 3 | Device | urn:oma:lwm2m:oma:3 | Single | Device info |

### 8.2 Standard Objects Supported

| ID | Name | URN | Description |
|----|------|-----|-------------|
| 2 | Access Control | urn:oma:lwm2m:oma:2 | ACL management |
| 4 | Connectivity Monitoring | urn:oma:lwm2m:oma:4 | Network status |
| 5 | Firmware Update | urn:oma:lwm2m:oma:5 | Standard FOTA |
| 6 | Location | urn:oma:lwm2m:oma:6 | GPS/Location |
| 7 | Connectivity Statistics | urn:oma:lwm2m:oma:7 | Network stats |
| 10 | Cellular Connectivity | urn:oma:lwm2m:oma:10 | Cellular config |
| 11 | APN Connection Profile | urn:oma:lwm2m:oma:11 | APN settings |
| 21 | OSCORE | urn:oma:lwm2m:oma:21 | Object security |
| 23 | LwM2M COSE | urn:oma:lwm2m:oma:23 | MQTT security |
| 24 | MQTT Server | urn:oma:lwm2m:oma:24 | MQTT config |

### 8.3 Custom Objects

| ID | Name | Description |
|----|------|-------------|
| 33405 | Advanced Firmware Update | Delta updates, rollback |
| 33410 | Edge AI Inference | On-device ML |

### 8.4 Device Object (ID: 3) Resources

| Resource | ID | Type | Operations | Description |
|----------|-----|------|------------|-------------|
| Manufacturer | 0 | String | R | Manufacturer name |
| Model Number | 1 | String | R | Model identifier |
| Serial Number | 2 | String | R | Serial number |
| Firmware Version | 3 | String | R | Firmware version |
| Reboot | 4 | - | E | Reboot device |
| Factory Reset | 5 | - | E | Factory reset |
| Available Power Sources | 6 | Integer[] | R | Power source types |
| Power Source Voltage | 7 | Integer[] | R | Voltages (mV) |
| Power Source Current | 8 | Integer[] | R | Currents (mA) |
| Battery Level | 9 | Integer | R | Battery % |
| Memory Free | 10 | Integer | R | Free memory (KB) |
| Error Code | 11 | Integer[] | R | Error codes |
| Reset Error Code | 12 | - | E | Clear errors |
| Current Time | 13 | Time | RW | Current time |
| UTC Offset | 14 | String | RW | UTC offset |
| Timezone | 15 | String | RW | Timezone |
| Supported Binding | 16 | String | R | Binding modes |
| Device Type | 17 | String | R | Device type |
| Hardware Version | 18 | String | R | Hardware version |
| Software Version | 19 | String | R | Software version |
| Battery Status | 20 | Integer | R | Battery status |
| Memory Total | 21 | Integer | R | Total memory |
| ExtDevInfo | 22 | Objlnk[] | R | External device links |

---

## 9. Delta Firmware Update System

### 9.1 Advanced Firmware Update Object (ID: 33405)

| Resource | ID | Type | Operations | Description |
|----------|-----|------|------------|-------------|
| Package | 0 | Opaque | W | Full firmware package |
| Package URI | 1 | String | W | Download URI |
| Update | 2 | - | E | Execute update |
| State | 3 | Integer | R | Update state |
| Update Result | 4 | Integer | R | Result code |
| Pkg Name | 5 | String | R | Package name |
| Pkg Version | 6 | String | R | Package version |
| Protocol Support | 7 | Integer | R | Protocols bitmap |
| Delivery Method | 8 | Integer | R | Delivery method |
| Cancel | 9 | - | E | Cancel update |
| Delta Package | 10 | Opaque | W | Delta package |
| Delta Package URI | 11 | String | W | Delta download URI |
| Delta Algorithm | 12 | Integer | RW | Algorithm selection |
| Block Size | 13 | Integer | RW | Transfer block size |
| Differential Download | 14 | Boolean | RW | Enable differential |
| Rollback Support | 15 | Boolean | RW | Enable rollback |
| Update Progress | 16 | Integer | R | Progress 0-100 |
| Component List | 17 | String | R | Components JSON |
| Source Version | 18 | String | R | Current version |
| Target Version | 19 | String | R | Target version |
| Checksum | 20 | Opaque | R | Package SHA256 |
| Checksum Algorithm | 21 | Integer | R | Hash algorithm |
| Max Deferred Period | 22 | Integer | RW | Defer timeout |
| Update Schedule | 23 | Time | RW | Scheduled time |

### 9.2 Delta Algorithms

| Algorithm | ID | Library | Best For | Compression |
|-----------|-----|---------|----------|-------------|
| BSDIFF | 1 | bsdiff | Binary firmware | 60-80% |
| VCDIFF | 2 | open-vcdiff | Text/config | 70-90% |
| Courgette | 3 | courgette | x86/ARM executables | 80-95% |

### 9.3 Compression Algorithms

| Algorithm | ID | Library | Ratio | Speed | Memory |
|-----------|-----|---------|-------|-------|--------|
| None | 0 | - | 0% | N/A | N/A |
| ZLIB | 1 | zlib | 60-70% | Fast | Low |
| LZMA | 2 | liblzma | 70-80% | Slow | High |
| Brotli | 3 | brotli | 75-85% | Medium | Medium |
| Zstd | 4 | zstd | 80-90% | Fast | Medium |

### 9.4 Update State Machine

| State | Value | Description |
|-------|-------|-------------|
| IDLE | 0 | No update in progress |
| DOWNLOADING | 1 | Package download active |
| DOWNLOADED | 2 | Download complete |
| VALIDATING | 3 | Checksum verification |
| APPLYING | 4 | Applying delta patch |
| UPDATING | 5 | System update active |
| REBOOTING | 6 | Preparing reboot |

### 9.5 Update Result Codes

| Result | Value | Description |
|--------|-------|-------------|
| SUCCESS | 0 | Update completed |
| NOT_ENOUGH_FLASH | 1 | Insufficient flash |
| NOT_ENOUGH_RAM | 2 | Insufficient RAM |
| CONNECTION_LOST | 3 | Download interrupted |
| INTEGRITY_FAILURE | 4 | Checksum mismatch |
| UNSUPPORTED_TYPE | 5 | Unknown package type |
| INVALID_URI | 6 | Bad download URI |
| FAILED_UPDATE | 7 | Update failed |
| UNSUPPORTED_PROTOCOL | 8 | Protocol not supported |
| DELTA_MISMATCH | 9 | Source version mismatch |
| ROLLBACK_REQUIRED | 10 | Rollback triggered |
| CANCELLED | 11 | User cancelled |
| DEFERRED | 12 | Update deferred |

---

## 10. A/B Partition & Rollback System

### 10.1 Partition Scheme

| Slot | Partition | Description |
|------|-----------|-------------|
| A | Primary | Active firmware |
| B | Secondary | Standby/update target |
| Recovery | Fixed | Recovery/failsafe |
| Data | Persistent | Configuration/data |

### 10.2 RollbackManager API

| Method | Description |
|--------|-------------|
| `createSnapshot(version)` | Create pre-update snapshot |
| `rollback(targetVersion)` | Revert to previous version |
| `getAvailableSnapshots()` | List snapshots |
| `switchSlot()` | Switch active partition |
| `isSlotBootable(slot)` | Verify slot integrity |
| `markSlotSuccessful()` | Confirm boot success |
| `enterRecoveryMode()` | Trigger recovery |
| `getActiveSlot()` | Get current slot |
| `getNextSlot()` | Get update target |

### 10.3 Snapshot Structure

| Field | Type | Description |
|-------|------|-------------|
| version | String | Semantic version |
| timestamp | Time | Creation time |
| slot | Enum | Source slot |
| checksum | SHA256 | State checksum |
| size | Integer | Snapshot size |
| compressed | Boolean | Compression flag |
| metadata | JSON | Additional data |

### 10.4 Boot Verification

| Check | Description | Action on Failure |
|-------|-------------|-------------------|
| Signature | Verify boot image | Try other slot |
| Checksum | Verify integrity | Try other slot |
| Boot count | Track boot attempts | Recovery after 3 |
| Heartbeat | Application health | Rollback |
| Watchdog | System responsiveness | Reset |

---

## 11. Edge AI Inference System

### 11.1 Edge AI Inference Object (ID: 33410)

#### 11.1.1 Model Management Resources (0-19)

| Resource | ID | Type | Ops | Description |
|----------|-----|------|-----|-------------|
| Model Name | 0 | String | R | Loaded model name |
| Model Version | 1 | String | R | Model version |
| Model URI | 2 | String | RW | Download URI |
| Model Format | 3 | Integer | R | Format enum |
| Model State | 4 | Integer | R | State enum |
| Model Size | 5 | Integer | R | Size in bytes |
| Model Checksum | 6 | Opaque | RW | SHA256 hash |
| Load Model | 7 | - | E | Load from storage |
| Unload Model | 8 | - | E | Unload from memory |
| Download Model | 9 | - | E | Download from URI |
| Download Progress | 10 | Integer | R | 0-100% |
| Model Description | 11 | String | R | Human description |
| Model Author | 12 | String | R | Model author |
| Model License | 13 | String | R | License info |
| Creation Date | 14 | Time | R | Model creation date |
| Min Runtime Version | 15 | String | R | Minimum runtime |

#### 11.1.2 Input/Output Resources (20-39)

| Resource | ID | Type | Ops | Description |
|----------|-----|------|-----|-------------|
| Input Tensor Info | 20 | Opaque | R | JSON tensor specs |
| Output Tensor Info | 21 | Opaque | R | JSON tensor specs |
| Input Data | 22 | Opaque | W | Inference input |
| Output Data | 23 | Opaque | R | Inference result |
| Labels | 24 | String | R | Class labels |
| Label Count | 25 | Integer | R | Number of labels |
| Input Count | 26 | Integer | R | Number of inputs |
| Output Count | 27 | Integer | R | Number of outputs |

#### 11.1.3 Inference Control Resources (40-59)

| Resource | ID | Type | Ops | Description |
|----------|-----|------|-----|-------------|
| Run Inference | 40 | - | E | Execute inference |
| Inference Mode | 41 | Integer | RW | Mode enum |
| Accelerator Type | 42 | Integer | RW | Backend enum |
| Num Threads | 43 | Integer | RW | Worker threads |
| Inference Timeout | 44 | Integer | RW | Timeout (ms) |
| Quantization Type | 45 | Integer | R | Quantization enum |
| Batch Size | 46 | Integer | RW | Batch size |
| Priority | 47 | Integer | RW | Execution priority |

#### 11.1.4 Statistics Resources (60-79)

| Resource | ID | Type | Ops | Description |
|----------|-----|------|-----|-------------|
| Inference Count | 60 | Integer | R | Total inferences |
| Latency Avg | 61 | Float | R | Average (ms) |
| Latency Min | 62 | Float | R | Minimum (ms) |
| Latency Max | 63 | Float | R | Maximum (ms) |
| Throughput | 64 | Float | R | Inferences/sec |
| Memory Usage | 65 | Integer | R | Peak bytes |
| Power Consumption | 66 | Float | R | Average mW |
| Error Count | 67 | Integer | R | Failed inferences |
| Last Error | 68 | String | R | Error message |
| Reset Statistics | 69 | - | E | Reset all stats |
| Latency P50 | 70 | Float | R | 50th percentile |
| Latency P95 | 71 | Float | R | 95th percentile |
| Latency P99 | 72 | Float | R | 99th percentile |

#### 11.1.5 Pre/Post Processing Resources (80-99)

| Resource | ID | Type | Ops | Description |
|----------|-----|------|-----|-------------|
| Normalize Input | 80 | Boolean | RW | Enable normalization |
| Input Mean | 81 | Opaque | RW | Per-channel mean |
| Input Stddev | 82 | Opaque | RW | Per-channel stddev |
| Apply Softmax | 83 | Boolean | RW | Softmax output |
| Confidence Threshold | 84 | Float | RW | Min confidence |
| Top K Results | 85 | Integer | RW | Top-K count |
| NMS Enabled | 86 | Boolean | RW | Non-max suppression |
| NMS IoU Threshold | 87 | Float | RW | IoU threshold |
| NMS Score Threshold | 88 | Float | RW | Score threshold |
| Max Detections | 89 | Integer | RW | Max detection count |

#### 11.1.6 Advanced Features Resources (100-119)

| Resource | ID | Type | Ops | Description |
|----------|-----|------|-----|-------------|
| Model Metadata | 100 | String | R | Full JSON metadata |
| Supported Formats | 101 | Opaque | R | Available formats |
| Supported Accelerators | 102 | Opaque | R | Available backends |
| Hardware Capabilities | 103 | String | R | Device capabilities |
| Benchmark Result | 104 | Opaque | R | Benchmark data |
| Run Benchmark | 105 | - | E | Execute benchmark |
| Enable Profiling | 106 | Boolean | RW | Layer profiling |
| Profile Data | 107 | Opaque | R | Layer timings |
| GPU Memory Limit | 108 | Integer | RW | GPU memory cap |
| Allow FP16 | 109 | Boolean | RW | FP16 acceleration |
| Calibration Data | 110 | Opaque | W | Quantization data |

### 11.2 Model Formats

| Format | ID | Extension | Runtime | Support |
|--------|-----|-----------|---------|---------|
| TensorFlow Lite | 0 | .tflite | TFLite | ✅ Full |
| ONNX | 1 | .onnx | ONNX Runtime | ✅ Full |
| PyTorch Mobile | 2 | .pt, .ptl | LibTorch | 📋 Planned |
| Core ML | 3 | .mlmodel | Core ML | 📋 Planned |
| TensorRT | 4 | .engine | TensorRT | 📋 Planned |
| OpenVINO | 5 | .xml/.bin | OpenVINO | 📋 Planned |

### 11.3 Hardware Accelerators

| Accelerator | ID | Description | Platforms |
|-------------|-----|-------------|-----------|
| AUTO | 0 | Automatic selection | All |
| CPU | 1 | CPU inference | All |
| GPU | 2 | GPU delegate | Linux, Android |
| NPU | 3 | Neural accelerator | Specific SoCs |
| DSP | 4 | DSP delegate | Qualcomm |
| TPU | 5 | Edge TPU | Coral devices |
| NNAPI | 6 | Android NNAPI | Android |
| COREML | 7 | Core ML delegate | iOS/macOS |

### 11.4 Quantization Types

| Type | ID | Size | Accuracy | Speed |
|------|-----|------|----------|-------|
| NONE (FP32) | 0 | 4 bytes | Best | Baseline |
| FP16 | 1 | 2 bytes | Good | 2x faster |
| INT8 | 2 | 1 byte | Good | 4x faster |
| INT4 | 3 | 0.5 byte | Moderate | 8x faster |
| DYNAMIC | 4 | Variable | Good | 2-4x faster |
| MIXED | 5 | Variable | Good | 2-4x faster |

### 11.5 Inference Modes

| Mode | ID | Description |
|------|-----|-------------|
| SYNCHRONOUS | 0 | Blocking inference |
| ASYNCHRONOUS | 1 | Non-blocking with callback |
| STREAMING | 2 | Continuous frame processing |
| BATCHED | 3 | Multiple inputs per call |

### 11.6 Model State Machine

| State | Value | Description |
|-------|-------|-------------|
| IDLE | 0 | No model loaded |
| DOWNLOADING | 1 | Download in progress |
| VALIDATING | 2 | Checksum verification |
| LOADING | 3 | Loading into memory |
| READY | 4 | Ready for inference |
| RUNNING | 5 | Inference in progress |
| ERROR | 6 | Error state |
| UPDATING | 7 | Hot-swap in progress |

---

## 12. Platform Abstraction Layer

### 12.1 Supported Platforms

| Platform | Version | Architecture | Status |
|----------|---------|--------------|--------|
| Ubuntu | 20.04+ | x86_64, ARM64 | ✅ Production |
| Debian | 11+ | x86_64, ARM64 | ✅ Production |
| OpenWRT | 24.10+ | ARM64, x86_64 | ✅ Production |
| OpenWRT One | Latest | ARM64 | ✅ Reference |
| Raspberry Pi OS | Bookworm | ARM64 | ✅ Tested |
| Yocto/Buildroot | Various | Various | 🔧 Community |
| FreeRTOS | 10.x | Cortex-M | 📋 Planned |

### 12.2 Platform Interface

| Category | Functions |
|----------|-----------|
| System | `reboot()`, `factoryReset()`, `getUptime()`, `getMemoryInfo()` |
| Storage | `readFile()`, `writeFile()`, `deleteFile()`, `getStorageInfo()` |
| Network | `getNetworkInterfaces()`, `getIPAddress()`, `getSignalStrength()` |
| Time | `getCurrentTime()`, `setCurrentTime()`, `getTimezone()` |
| Firmware | `getCurrentVersion()`, `getHardwareVersion()`, `getPartitionInfo()` |
| Security | `generateRandom()`, `computeHash()`, `encryptData()` |
| Power | `getBatteryLevel()`, `getPowerSource()`, `setPowerMode()` |

### 12.3 OpenWRT Integration

| Component | Description |
|-----------|-------------|
| Package | IPK package generation |
| Init Script | procd service management |
| UCI Config | Configuration integration |
| LUCI | Optional web interface |
| Hotplug | Network event handling |

---

## 13. Build System & Dependencies

### 13.1 Build Requirements

| Tool | Minimum Version | Purpose |
|------|-----------------|---------|
| CMake | 3.16 | Build system |
| GCC | 8.0 | C/C++ compiler |
| Clang | 7.0 | Alternative compiler |
| Make | 4.0 | Build automation |

### 13.2 Required Dependencies

| Library | Version | Purpose | License |
|---------|---------|---------|---------|
| OpenSSL | 1.1.1+ | TLS/crypto | Apache 2.0 |
| libcurl | 7.68+ | HTTP downloads | MIT |
| zlib | 1.2.11+ | Compression | zlib |

### 13.3 Optional Dependencies

| Library | Version | Feature | License |
|---------|---------|---------|---------|
| mbedTLS | 3.0+ | Primary TLS | Apache 2.0 |
| TinyDTLS | 0.9+ | Constrained DTLS | EPL/EDL |
| libcoap | 4.3+ | CoAP protocol | BSD-2 |
| mosquitto | 2.0+ | MQTT client | EPL 2.0 |
| liblzma | 5.2+ | LZMA compression | Public domain |
| zstd | 1.4+ | Zstd compression | BSD-3 |
| brotli | 1.0+ | Brotli compression | MIT |
| bsdiff | 4.3+ | Delta algorithm | BSD-2 |
| TFLite | 2.x | ML inference | Apache 2.0 |
| ONNX Runtime | 1.x | ML inference | MIT |
| GTest | 1.10+ | Unit testing | BSD-3 |

### 13.4 CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | Release | Build type |
| `WITH_DTLS` | ON | DTLS support |
| `WITH_MBEDTLS` | ON | mbedTLS backend |
| `WITH_TINYDTLS` | OFF | TinyDTLS backend |
| `WITH_DELTA_FOTA` | ON | Delta updates |
| `WITH_BSDIFF` | ON | BSDIFF algorithm |
| `WITH_VCDIFF` | ON | VCDIFF algorithm |
| `WITH_COURGETTE` | ON | Courgette algorithm |
| `WITH_COMPRESSION` | ON | Compression support |
| `WITH_ROLLBACK` | ON | A/B rollback |
| `WITH_MQTT` | ON | MQTT transport |
| `WITH_EDGE_AI` | ON | Edge AI inference |
| `WITH_TFLITE` | OFF | TensorFlow Lite |
| `WITH_ONNX` | OFF | ONNX Runtime |
| `BUILD_TESTS` | ON | Unit tests |
| `BUILD_EXAMPLES` | ON | Examples |
| `ENABLE_COVERAGE` | OFF | Code coverage |
| `ENABLE_SANITIZERS` | OFF | Address sanitizer |

---

## 14. Performance Specifications

### 14.1 Memory Requirements

| Component | RAM (min) | RAM (recommended) |
|-----------|-----------|-------------------|
| Core client | 256 KB | 512 KB |
| MQTT transport | 64 KB | 128 KB |
| Delta update | 512 KB | 1 MB |
| Edge AI (CPU) | 2 MB | 8 MB |
| Edge AI (GPU) | 4 MB | 16 MB |

### 14.2 Storage Requirements

| Component | Flash |
|-----------|-------|
| Core library | 200 KB |
| MQTT transport | 50 KB |
| Delta algorithms | 100 KB |
| Edge AI runtime | 2-5 MB |
| Typical model | 5-50 MB |

### 14.3 Network Performance

| Metric | Typical | Maximum |
|--------|---------|---------|
| Registration latency | 50-200 ms | 5 sec |
| Read/Write latency | 10-50 ms | 1 sec |
| Notification frequency | 1/sec | 100/sec |
| MQTT message rate | 100/sec | 1000/sec |
| Firmware download | 1 MB/sec | 10 MB/sec |

### 14.4 AI Inference Performance

| Model Type | CPU (RPi4) | GPU (Coral) |
|------------|------------|-------------|
| MobileNet v2 | 50 ms | 8 ms |
| EfficientNet-B0 | 80 ms | 15 ms |
| YOLO-Tiny | 100 ms | 20 ms |
| ResNet-50 | 200 ms | 40 ms |

---

## 15. API Summary

### 15.1 Core Classes

| Class | Header | Description |
|-------|--------|-------------|
| `lwm2m::Client` | lwm2m_client.h | Main client class |
| `lwm2m::Object` | lwm2m_object.h | Object base class |
| `lwm2m::Resource` | lwm2m_resource.h | Resource base class |

### 15.2 Transport Classes

| Class | Header | Description |
|-------|--------|-------------|
| `transport::CoapTransport` | coap_transport.h | CoAP transport |
| `transport::MqttTransport` | mqtt_transport.h | MQTT transport |
| `transport::BlockwiseTransfer` | blockwise_transfer.h | Block handling |

### 15.3 Firmware Classes

| Class | Header | Description |
|-------|--------|-------------|
| `firmware::DeltaAlgorithms` | delta_algorithms.h | Delta patching |
| `firmware::RollbackManager` | rollback_manager.h | A/B management |

### 15.4 Object Classes

| Class | Header | Object ID |
|-------|--------|-----------|
| `objects::DeviceObject` | device_object.h | 3 |
| `objects::FirmwareUpdateObject` | firmware_update_object.h | 5 |
| `objects::AdvancedFirmwareObject` | advanced_firmware_update_object.h | 33405 |
| `objects::EdgeAIInferenceObject` | edge_ai_inference_object.h | 33410 |

---

## 16. Server Compatibility Matrix

### 16.1 Feature Requirements by Priority

#### Critical (Must Have)

| Feature | Description |
|---------|-------------|
| LwM2M v1.2.x registration | Handle v1.2 version string |
| CoAP Block-wise (RFC 7959) | Block1/Block2 options |
| DTLS 1.2 PSK | Pre-shared key security |
| TLV encoding (11542) | LwM2M TLV format |
| Basic operations | Read, Write, Execute |
| Object 5 Firmware | Standard firmware update |

#### High Priority

| Feature | Description |
|---------|-------------|
| Composite operations | Read/Write/Observe-Composite |
| CBOR encoding (11544) | LwM2M CBOR format |
| SenML CBOR (112) | Sensor data format |
| Send operation | Client-initiated push |
| Certificate security | X.509 certificates |
| BERT transfers | Large block sizes |

#### Medium Priority

| Feature | Description |
|---------|-------------|
| MQTT transport | MQTT binding support |
| Object 33405 | Advanced firmware |
| Delta packages | BSDIFF/VCDIFF support |
| Rollback commands | Rollback coordination |
| Queue mode | Q binding mode |

#### Lower Priority

| Feature | Description |
|---------|-------------|
| Object 33410 | Edge AI inference |
| OSCORE | Application security |
| Observe-Composite | Multi-resource observe |
| Bootstrap-Pack | Packed bootstrap |

### 16.2 Tested Server Compatibility

| Server | Version | Compatibility |
|--------|---------|---------------|
| Friendly One-IoT DMP | 5.x | ✅ Full |
| Leshan | 2.x | ✅ Full |
| Eclipse Wakaama | 1.x | ✅ Full |
| AVSystem Coiote | Latest | ✅ Full |
| Cumulocity IoT | Latest | ⚠️ Partial |

---

## Document Information

| Field | Value |
|-------|-------|
| Document Version | 1.1.0 |
| Created | January 7, 2026 |
| Author | Friendly Technologies |
| Classification | Public |
| Repository | github.com/FriendlyTechnologies/Friendly-LwM2M-Client |

---

*© 2024-2026 Friendly Technologies. All rights reserved.*
