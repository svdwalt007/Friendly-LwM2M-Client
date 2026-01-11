# LwM2M Server Gap Analysis for Friendly LwM2M Client

## Executive Summary

This document provides a comprehensive gap analysis for implementing an LwM2M server that fully supports the Friendly LwM2M Client. The analysis focuses on DTLS/TLS/PKI setup requirements, protocol compatibility, and object support.

**Version**: 1.0
**Date**: January 2026
**Client Version**: Friendly LwM2M Client v1.1.0
**Target Server**: Advanced-LWM2M-Server-CPP

---

## 1. Client Capabilities Summary

### 1.1 Protocol Support

| Feature | Client Support | Server Requirement |
|---------|---------------|-------------------|
| LwM2M 1.0 | Full (100%) | Must Support |
| LwM2M 1.1 | Substantial (85%) | Should Support |
| LwM2M 1.2 | Partial (50%) | Optional |
| CoAP/UDP | Full | Must Support |
| CoAP/DTLS | Full | Must Support |
| Bootstrap | Full | Should Support |

### 1.2 Security Modes Supported by Client

| Mode | Client Support | OMA Spec | Priority |
|------|---------------|----------|----------|
| NoSec (Mode 3) | ✅ Full | 1.0 | LOW |
| PSK (Mode 0) | ✅ Full | 1.0 | HIGH |
| RPK (Mode 1) | ✅ Full | 1.0 | MEDIUM |
| X.509 Certificate (Mode 2) | 🟡 Partial | 1.0 | HIGH |
| Certificate + EST | ❌ Commercial | 1.2 | FUTURE |

### 1.3 Data Formats Supported by Client

| Format | Content-Type | Client Support | Priority |
|--------|--------------|----------------|----------|
| Plain Text | text/plain | ✅ Full | LOW |
| Opaque | application/octet-stream | ✅ Full | LOW |
| TLV | application/vnd.oma.lwm2m+tlv | ✅ Full | HIGH |
| JSON | application/vnd.oma.lwm2m+json | ✅ Full | HIGH |
| SenML JSON | application/senml+json (110) | ✅ Full | HIGH |
| SenML CBOR | application/senml+cbor (112) | ✅ Full | MEDIUM |
| CBOR | application/cbor | ✅ Full | MEDIUM |
| LwM2M CBOR | - | ✅ Full | LOW |

---

## 2. DTLS/TLS/PKI Gap Analysis

### 2.1 DTLS Requirements

#### 2.1.1 DTLS Version Support

| Version | Client | Server Gap | Priority |
|---------|--------|-----------|----------|
| DTLS 1.2 (RFC 6347) | ✅ Default | REQUIRED | CRITICAL |
| DTLS 1.3 (RFC 9147) | 🟡 Platform-dependent | RECOMMENDED | HIGH |
| DTLS Connection ID | 🟡 Platform-dependent | RECOMMENDED | MEDIUM |

**Gap Assessment**:
- Server MUST support DTLS 1.2 for baseline compatibility
- Server SHOULD support DTLS 1.3 for enhanced security
- Connection ID support helps with NAT traversal and mobile clients

#### 2.1.2 DTLS Cipher Suites

**Required Cipher Suites** (per OMA LwM2M TS):

```
# PSK Mode (Priority: CRITICAL)
TLS_PSK_WITH_AES_128_CCM_8
TLS_PSK_WITH_AES_128_CCM
TLS_PSK_WITH_AES_128_CBC_SHA256

# RPK Mode (Priority: HIGH)
TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
TLS_ECDHE_ECDSA_WITH_AES_128_CCM
TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256

# Certificate Mode (Priority: HIGH)
TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256
```

**Client Security Object Resources** (`wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.h`):

| Resource ID | Resource Name | Gap |
|-------------|--------------|-----|
| 0 | LWM2M Server URI | Server must parse coap:// and coaps:// |
| 2 | Security Mode | Server must support modes 0-3 |
| 3 | Public Key or Identity | Server must handle PSK ID, RPK, X.509 |
| 4 | Server Public Key | Server must provide RPK/Cert for validation |
| 5 | Secret Key | Server must have corresponding PSK or trust anchor |
| 10 | Short Server ID | Server must match SSID |
| 12 | SNI (Server Name Indication) | Server should support SNI |
| 15 | Certificate Usage | Server must support cert chain validation |
| 16 | DTLS/TLS Ciphersuite | Server must support negotiated suite |

### 2.2 PSK Mode Implementation

**Client Configuration** (`examples/objects.cpp`):
```cpp
security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_PRE_SHARED_KEY);
security->set<OPAQUE_T>(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3, psk_identity);
security->set<OPAQUE_T>(Lwm2mSecurity::SECRET_KEY_5, psk_key);
```

**Server Requirements**:

| Requirement | Description | Gap |
|-------------|-------------|-----|
| PSK Database | Store PSK identity-key pairs | MUST implement |
| PSK Identity Lookup | Retrieve key by identity during handshake | MUST implement |
| PSK Key Derivation | Support for derived keys (optional) | SHOULD implement |
| PSK Hint | Server PSK hint for client selection | SHOULD implement |

**Recommended Implementation**:
```cpp
// Server PSK callback
int psk_server_callback(
    SSL *ssl,
    const char *identity,
    unsigned char *psk,
    unsigned int max_psk_len
) {
    // 1. Look up identity in database
    // 2. Retrieve corresponding PSK
    // 3. Copy PSK to output buffer
    // 4. Return PSK length
}
```

### 2.3 RPK Mode Implementation

**Client RPK Resources**:
- Resource 3: Client's raw public key (ECDSA P-256)
- Resource 4: Server's raw public key (for server authentication)
- Resource 5: Client's private key

**Server Requirements**:

| Requirement | Description | Gap |
|-------------|-------------|-----|
| RPK Generation | Generate server ECDSA P-256 key pair | MUST implement |
| RPK Validation | Validate client's public key | MUST implement |
| Key Format | SubjectPublicKeyInfo (RFC 7250) | MUST support |
| Curve Support | NIST P-256 (secp256r1) | MUST support |
| Key Storage | Secure storage for server private key | MUST implement |

**Key Format**:
```
SubjectPublicKeyInfo ::= SEQUENCE {
    algorithm AlgorithmIdentifier,
    subjectPublicKey BIT STRING
}
```

### 2.4 X.509 Certificate Mode Implementation

**Client Certificate Resources**:
- Resource 3: Client certificate (X.509 DER)
- Resource 4: Server certificate/CA (for chain validation)
- Resource 5: Client private key
- Resource 13: Certificate hash algorithm (SHA-256/384/512)
- Resource 15: Certificate usage (CA, Service Cert, Domain Cert, Pin)

**Server Requirements**:

| Requirement | Description | Gap | Priority |
|-------------|-------------|-----|----------|
| CA Certificate | Root CA for client cert validation | MUST implement | CRITICAL |
| Server Certificate | Valid X.509 for server authentication | MUST implement | CRITICAL |
| Certificate Chain | Support intermediate CA certs | SHOULD implement | HIGH |
| CRL/OCSP | Certificate revocation checking | SHOULD implement | MEDIUM |
| Certificate Pinning | Support for pinned certificates | MAY implement | LOW |
| SNI Support | Server Name Indication handling | SHOULD implement | HIGH |

**Certificate Chain Validation**:
```
Client Cert → Intermediate CA (optional) → Root CA (server trust anchor)
```

**Server Certificate Requirements**:
- Valid X.509v3 certificate
- Extended Key Usage: serverAuth
- Subject Alternative Name with server hostname/IP
- Key usage: digitalSignature, keyEncipherment

### 2.5 PKI Infrastructure Requirements

#### 2.5.1 Certificate Authority Setup

**Gap**: Server needs CA infrastructure for production deployments

| Component | Description | Implementation |
|-----------|-------------|----------------|
| Root CA | Self-signed root certificate | OpenSSL or dedicated HSM |
| Intermediate CA | Optional signing CA | Recommended for production |
| Client Cert Issuance | Provisioning client certificates | Manual or automated (EST) |
| Server Cert | Server's authentication certificate | Must be signed by trusted CA |

**Recommended CA Structure**:
```
Root CA (offline, HSM protected)
    ├── Device Issuing CA (for client certs)
    │       ├── Client Cert 1
    │       ├── Client Cert 2
    │       └── ...
    └── Server CA (for server certs)
            └── LwM2M Server Cert
```

#### 2.5.2 Certificate Provisioning Methods

| Method | Description | Client Support | Server Gap |
|--------|-------------|---------------|-----------|
| Bootstrap | Server provisions cert via bootstrap | ✅ Supported | MUST implement |
| Factory | Pre-provisioned at manufacturing | ✅ Supported | N/A |
| EST | Enrollment over Secure Transport | ❌ Commercial | FUTURE |

### 2.6 Security Implementation Checklist

**DTLS Server Checklist**:

- [ ] DTLS 1.2 support with required cipher suites
- [ ] PSK mode with identity-key database
- [ ] RPK mode with ECDSA P-256 support
- [ ] Certificate mode with X.509v3 validation
- [ ] Certificate chain validation
- [ ] Server certificate properly configured
- [ ] SNI support for virtual hosting
- [ ] Session resumption support
- [ ] DTLS retransmission handling
- [ ] DTLS fragment reassembly

**PKI Checklist**:

- [ ] Root CA certificate generated/obtained
- [ ] Server certificate with proper extensions
- [ ] Client certificate trust anchor configured
- [ ] Certificate revocation mechanism (CRL/OCSP)
- [ ] Secure private key storage
- [ ] Certificate expiration monitoring

---

## 3. Protocol Operations Gap Analysis

### 3.1 Registration Interface

**Client Behavior**:
- Sends REGISTER to /rd endpoint
- Includes: endpoint name, lifetime, LwM2M version, binding mode
- Expects: 2.01 Created response with location

**Server Requirements**:

| Operation | Description | Gap |
|-----------|-------------|-----|
| Register | Accept POST to /rd | MUST implement |
| Update | Accept POST to /rd/{location} | MUST implement |
| De-register | Accept DELETE to /rd/{location} | MUST implement |
| Lifetime Tracking | Monitor client lifetime | MUST implement |
| Stale Client Cleanup | Remove expired registrations | MUST implement |

### 3.2 Device Management Interface

**Operations the Client Supports**:

| Operation | CoAP Method | Server Gap |
|-----------|-------------|-----------|
| Read | GET | MUST implement |
| Write | PUT/POST | MUST implement |
| Execute | POST (no payload) | MUST implement |
| Create | POST | MUST implement |
| Delete | DELETE | MUST implement |
| Discover | GET (Accept: link-format) | MUST implement |
| Write-Attributes | PUT (query params) | MUST implement |

### 3.3 Information Reporting Interface

**Client Observation Support**:

| Feature | Client Support | Server Gap |
|---------|---------------|-----------|
| Observe | ✅ Full | MUST implement |
| Cancel Observe | ✅ Full | MUST implement |
| Notify | ✅ Full | MUST handle |
| Confirmable Notify | ✅ Supported | SHOULD implement |
| pmin/pmax | ✅ Supported | MUST implement |
| gt/lt/st | ✅ Supported | MUST implement |

### 3.4 Bootstrap Interface

**Client Bootstrap Support**:
- Bootstrap Request to /bs endpoint
- Accepts Bootstrap Write, Delete, Discover, Finish

**Server Bootstrap Requirements**:

| Operation | Description | Gap |
|-----------|-------------|-----|
| Bootstrap Request | Accept POST to /bs | MUST implement |
| Bootstrap Write | Provision Security/Server objects | MUST implement |
| Bootstrap Delete | Clear object instances | MUST implement |
| Bootstrap Discover | List available objects | SHOULD implement |
| Bootstrap Finish | Complete bootstrap sequence | MUST implement |

---

## 4. Object Support Gap Analysis

### 4.1 Mandatory Objects

| Object ID | Name | Client | Server Gap |
|-----------|------|--------|-----------|
| 0 | LwM2M Security | ✅ Full | MUST understand |
| 1 | LwM2M Server | ✅ Full | MUST understand |
| 3 | Device | ✅ Full | MUST understand |

### 4.2 Standard Optional Objects

| Object ID | Name | Client | Server Gap |
|-----------|------|--------|-----------|
| 2 | Access Control | ✅ Supported | SHOULD understand |
| 4 | Connectivity Monitoring | ✅ Supported | SHOULD understand |
| 5 | Firmware Update | ✅ Supported | SHOULD understand |
| 6 | Location | ✅ Supported | SHOULD understand |
| 12 | WLAN Connectivity | ✅ Supported | SHOULD understand |
| 13 | Bearer Selection | ✅ Supported | SHOULD understand |

### 4.3 Walt Technologies Custom Objects (10512-10522)

| Object ID | Name | Description | Server Gap |
|-----------|------|-------------|-----------|
| 10512 | Starlink Terminal | Starlink dish management | MAY understand |
| 10513 | Router Management | OpenWRT router config | MAY understand |
| 10514 | Ethernet Interface | Ethernet port monitoring | MAY understand |
| 10515 | GPIO Control | LED/GPIO management | MAY understand |
| 10516 | USB Management | USB device control | MAY understand |
| 10517 | Storage Management | Storage monitoring | MAY understand |
| 10518 | System Monitor | System resource monitoring | MAY understand |
| 10519 | Hardware Watchdog | Watchdog timer control | MAY understand |
| 10520 | MIKROBUS | Click board expansion | MAY understand |
| 10521 | Firewall Config | iptables/nftables config | MAY understand |
| 10522 | PoE Management | PoE port control | MAY understand |

### 4.4 Walt Technologies Extended Objects (34611-34623)

| Object ID | Name | Description | Server Gap |
|-----------|------|-------------|-----------|
| 34611 | WAN Failover Policy | Multi-WAN failover | MAY understand |
| 34612 | Multi-WAN Health Check | WAN health monitoring | MAY understand |
| 34613 | WiFi Client Management | WiFi client tracking | MAY understand |
| 34614 | WiFi Channel Optimization | Channel selection | MAY understand |
| 34615 | Matter Bridge | Matter protocol bridge | MAY understand |
| 34616 | Matter Device | Matter device representation | MAY understand |
| 34617 | Thread Network | Thread mesh management | MAY understand |
| 34618 | Zigbee Coordinator | Zigbee coordinator control | MAY understand |
| 34619 | Zigbee Device | Zigbee device representation | MAY understand |
| 34620 | Zigbee Group | Zigbee group management | MAY understand |
| 34621 | LAN Configuration | LAN network config | MAY understand |
| 34622 | Routing Table | IP routing management | MAY understand |
| 34623 | VPN Configuration | VPN tunnel config | MAY understand |

---

## 5. Implementation Priorities

### 5.1 Critical (Must Have)

1. **DTLS 1.2 with PSK** - Most common deployment mode
2. **Registration Interface** - Core LwM2M functionality
3. **Read/Write/Execute** - Basic device management
4. **Observe/Notify** - Information reporting
5. **TLV and JSON encoding** - Standard data formats

### 5.2 High Priority (Should Have)

1. **DTLS with X.509** - Enterprise deployments
2. **Bootstrap Server** - Device provisioning
3. **Firmware Update** - FOTA support
4. **SenML JSON/CBOR** - LwM2M 1.1 formats
5. **DTLS 1.3** - Modern security

### 5.3 Medium Priority (Nice to Have)

1. **RPK Mode** - Lightweight certificate alternative
2. **Custom Object Support** - Walt Technologies objects
3. **Block-wise Transfer** - Large payload support
4. **Send Operation** - LwM2M 1.1 telemetry

### 5.4 Low Priority (Future)

1. **MQTT Transport** - Alternative transport
2. **Composite Operations** - Efficiency improvement
3. **EST Support** - Automated certificate enrollment
4. **Gateway Mode** - Proxy non-LwM2M devices

---

## 6. Server Implementation Recommendations

### 6.1 Technology Stack

**Recommended Libraries**:

| Component | Recommended | Alternative |
|-----------|-------------|-------------|
| CoAP | libcoap | Californium (Java) |
| DTLS | mbedTLS or OpenSSL | wolfSSL |
| TLV/JSON | Custom or tinycbor | nlohmann/json |
| Database | SQLite | PostgreSQL |

### 6.2 Architecture Pattern

```
┌─────────────────────────────────────────────────────────────┐
│                    LwM2M Server                              │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ Registration │  │   Device     │  │   Bootstrap      │  │
│  │   Manager    │  │   Manager    │  │   Server         │  │
│  └──────┬───────┘  └──────┬───────┘  └────────┬─────────┘  │
│         │                  │                    │            │
│  ┌──────▼──────────────────▼────────────────────▼────────┐  │
│  │                    CoAP Layer                          │  │
│  │  Observe │ Block │ Request/Response │ Resource Handler │  │
│  └──────────────────────────┬────────────────────────────┘  │
│                              │                               │
│  ┌──────────────────────────▼────────────────────────────┐  │
│  │                   DTLS Layer                           │  │
│  │  PSK │ RPK │ X.509 │ Session │ Cipher Suite Negotiation│  │
│  └──────────────────────────┬────────────────────────────┘  │
│                              │                               │
│  ┌──────────────────────────▼────────────────────────────┐  │
│  │                   UDP Socket                           │  │
│  │          Port 5683 (CoAP) │ Port 5684 (CoAPS)         │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                               │
└───────────────────────────────────────────────────────────────┘
```

### 6.3 Security Configuration Template

**server_security.conf**:
```ini
[dtls]
enabled = true
port = 5684

# DTLS version
min_version = dtls1.2
max_version = dtls1.3

# Cipher suites (in order of preference)
cipher_suites = TLS_PSK_WITH_AES_128_CCM_8,TLS_PSK_WITH_AES_128_CCM,TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8,TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256

# Session
session_timeout = 86400
session_cache_size = 1000

[psk]
enabled = true
database = /etc/lwm2m/psk.db

[certificate]
enabled = true
server_cert = /etc/lwm2m/certs/server.crt
server_key = /etc/lwm2m/certs/server.key
ca_cert = /etc/lwm2m/certs/ca.crt
verify_client = true
crl_file = /etc/lwm2m/certs/crl.pem

[rpk]
enabled = true
server_key = /etc/lwm2m/keys/server_rpk.der
trusted_keys = /etc/lwm2m/keys/trusted/
```

---

## 7. Testing Recommendations

### 7.1 Security Mode Testing

| Test Case | Description | Expected Result |
|-----------|-------------|-----------------|
| PSK-001 | Valid PSK authentication | DTLS handshake success |
| PSK-002 | Invalid PSK identity | DTLS handshake failure |
| PSK-003 | Invalid PSK key | DTLS handshake failure |
| CERT-001 | Valid client certificate | DTLS handshake success |
| CERT-002 | Expired certificate | DTLS handshake failure |
| CERT-003 | Revoked certificate | DTLS handshake failure |
| RPK-001 | Valid RPK authentication | DTLS handshake success |
| RPK-002 | Unknown public key | DTLS handshake failure |

### 7.2 Interoperability Testing

Test server against:
1. Friendly LwM2M Client (this client)
2. Eclipse Wakaama client
3. Anjay LwM2M client
4. Other commercial LwM2M clients

### 7.3 Performance Testing

| Metric | Target | Test Method |
|--------|--------|-------------|
| DTLS Handshake Time | < 500ms | Measure handshake duration |
| Registration Time | < 200ms | Measure end-to-end registration |
| Concurrent Clients | 1000+ | Load test with simulated clients |
| Observation Updates | 10,000/sec | Stress test notify handling |

---

## 8. Summary of Gaps

### 8.1 Critical Gaps (Must Address)

| Gap | Impact | Effort |
|-----|--------|--------|
| DTLS 1.2 with PSK | No secure communication | HIGH |
| Registration Interface | No device management | HIGH |
| Read/Write/Execute | No resource access | HIGH |
| TLV/JSON encoding | No data exchange | MEDIUM |

### 8.2 High Priority Gaps

| Gap | Impact | Effort |
|-----|--------|--------|
| X.509 Certificate Mode | No enterprise security | HIGH |
| Bootstrap Server | No device provisioning | MEDIUM |
| Firmware Update support | No FOTA | MEDIUM |
| Observe/Notify | No telemetry | MEDIUM |

### 8.3 Medium Priority Gaps

| Gap | Impact | Effort |
|-----|--------|--------|
| RPK Mode | Limited security options | MEDIUM |
| DTLS 1.3 | Older security protocol | MEDIUM |
| Custom Object Support | Limited device features | LOW |
| SenML formats | Limited 1.1 compatibility | LOW |

---

## 9. Conclusion

The Friendly LwM2M Client provides comprehensive LwM2M support with full LwM2M 1.0 and substantial LwM2M 1.1 compatibility. To fully support this client, the server implementation should prioritize:

1. **DTLS with PSK** - Most deployments use PSK for its simplicity
2. **X.509 Certificate Mode** - Required for enterprise/production deployments
3. **Complete Registration Interface** - Foundation for all device management
4. **Bootstrap Server** - Enables scalable device provisioning
5. **Multiple Data Formats** - TLV, JSON, and SenML for flexibility

The custom Walt Technologies objects (10512-10522, 34611-34623) provide rich device management capabilities for OpenWRT-based routers and IoT gateways, including Zigbee, Matter, and Thread protocol support.

---

## Appendix A: Client Security Configuration Reference

**Location**: `examples/objects.cpp`

```cpp
void securityInit(WppClient &client) {
    Instance *security = Lwm2mSecurity::createInst(client);

    // Server URI
    security->set<STRING_T>(Lwm2mSecurity::LWM2M_SERVER_URI_0, "coaps://server:5684");

    // Security Mode (0=PSK, 1=RPK, 2=Cert, 3=NoSec)
    security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_PRE_SHARED_KEY);

    // PSK Identity and Key
    security->set<OPAQUE_T>(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3, identity);
    security->set<OPAQUE_T>(Lwm2mSecurity::SECRET_KEY_5, psk);

    // Server ID
    security->set<INT_T>(Lwm2mSecurity::SHORT_SERVER_ID_10, 123);
}
```

---

## Appendix B: Object XML Files Location

All Walt Technologies LwM2M Object XML definitions are available at:
- `utils/object_maker/` - Individual XML files (10512-10522.xml, 34611-34623.xml)
- `utils/object_maker/walt_technologies_lwm2m_objects.zip` - Complete package
- `utils/object_maker/DDF_Walt_Technologies.xml` - OMA Registry entries

---

**Document Version**: 1.0
**Last Updated**: January 2026
**Author**: Claude AI Assistant
