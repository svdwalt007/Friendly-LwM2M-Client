# DTLS Provider Selection Guide

**Version:** 1.0
**Date:** April 12, 2026
**For:** Friendly LwM2M Client v1.2.2+

---

## Table of Contents

1. [Overview](#overview)
2. [Provider Comparison](#provider-comparison)
3. [Selection Criteria](#selection-criteria)
4. [Provider Details](#provider-details)
5. [Build Configuration](#build-configuration)
6. [Runtime Selection](#runtime-selection)
7. [Best Practices](#best-practices)
8. [Troubleshooting](#troubleshooting)

---

## Overview

The Friendly LwM2M Client supports **four DTLS providers** through a unified abstraction layer:

| Provider | Version | Recommended For |
|----------|---------|-----------------|
| **mbedTLS** | 3.6+ | Production deployments, embedded systems (DEFAULT) |
| **TinyDTLS** | 0.8.6 | Constrained devices, minimal footprint |
| **OpenSSL** | 3.x | Desktop/server environments, maximum compatibility |
| **wolfSSL** | 5.x | Performance-critical applications, embedded |

**Default Provider:** mbedTLS 3.6+
- Production-grade security
- Smaller footprint than OpenSSL
- Active development and maintenance
- Apache 2.0 license

---

## Provider Comparison

### Quick Reference Matrix

| Feature | TinyDTLS | mbedTLS | OpenSSL | wolfSSL |
|---------|----------|---------|---------|---------|
| **DTLS 1.2** | ✅ | ✅ | ✅ | ✅ |
| **DTLS 1.3** | ❌ | 🚧 Dev | ✅ | ✅ |
| **PSK Auth** | ✅ | ✅ | ✅ | ✅ |
| **RPK Auth** | ✅ | ❌* | ❌* | ❌* |
| **Certificate Auth** | ❌ | ✅ | ✅ | ✅ |
| **RFC 9146 CID** | ✅ | ✅ | ⚠️ 3.2+ | ✅ |
| **Binary Size** | ~50KB | ~200KB | ~500KB | ~250KB |
| **RAM Usage** | Low | Medium | High | Medium |
| **License** | EPL/EDL | Apache 2.0 | Apache 2.0 | GPL/Commercial |
| **Maturity** | Stable | Very Stable | Very Stable | Stable |
| **Maintenance** | Limited | Active | Very Active | Active |

*RPK implementation pending

### Detailed Comparison

#### 1. TinyDTLS 0.8.6

**Strengths:**
- ✅ Smallest binary size (~50KB)
- ✅ Minimal RAM footprint
- ✅ RFC 9146 CID support
- ✅ Well-tested in LwM2M deployments
- ✅ PSK and RPK authentication

**Limitations:**
- ❌ No certificate support
- ❌ No DTLS 1.3
- ❌ Limited cipher suite support
- ⚠️ Slower development cycle

**Best For:**
- IoT devices with <256KB flash
- Battery-powered devices
- When PSK/RPK is sufficient
- Legacy deployments

#### 2. mbedTLS 3.6+ (RECOMMENDED)

**Strengths:**
- ✅ Production-grade security
- ✅ Optimized for embedded systems
- ✅ Native RFC 9146 CID support
- ✅ PSK and certificate support
- ✅ Active development
- ✅ Apache 2.0 license
- ✅ Extensive documentation

**Limitations:**
- ⚠️ DTLS 1.3 still in development
- ❌ RPK not yet implemented

**Best For:**
- **Production deployments** (DEFAULT)
- Embedded Linux (OpenWRT, Yocto)
- Commercial products
- Long-term maintenance
- When license compatibility matters

#### 3. OpenSSL 3.x

**Strengths:**
- ✅ Industry standard
- ✅ DTLS 1.3 support
- ✅ Comprehensive cipher suites
- ✅ Extensive testing
- ✅ Wide platform support
- ✅ Strong community

**Limitations:**
- ❌ Larger binary size (~500KB)
- ❌ Higher memory usage
- ⚠️ CID support experimental (3.2+)
- ⚠️ Complex API

**Best For:**
- Desktop/server applications
- Testing and development
- Maximum protocol compatibility
- When binary size is not a concern
- Integration with existing OpenSSL infrastructure

#### 4. wolfSSL 5.x

**Strengths:**
- ✅ Performance-optimized
- ✅ DTLS 1.3 support
- ✅ RFC 9146 CID support
- ✅ Embedded-friendly
- ✅ Commercial support available

**Limitations:**
- ⚠️ GPL license (commercial license available)
- ⚠️ CID support varies by build configuration

**Best For:**
- Performance-critical applications
- Commercial products (with license)
- When speed is priority
- Embedded systems with moderate resources

---

## Selection Criteria

### Decision Tree

```
START: Which DTLS provider should I use?

┌─────────────────────────────────────────────┐
│ Do you have < 256KB flash?                  │
│   YES → TinyDTLS                            │
│   NO  → Continue                            │
└─────────────────────────────────────────────┘
           ↓
┌─────────────────────────────────────────────┐
│ Do you need certificate authentication?     │
│   NO  → TinyDTLS (if PSK/RPK sufficient)    │
│   YES → Continue                            │
└─────────────────────────────────────────────┘
           ↓
┌─────────────────────────────────────────────┐
│ Is this for production deployment?          │
│   YES → mbedTLS (RECOMMENDED)               │
│   NO  → Continue                            │
└─────────────────────────────────────────────┘
           ↓
┌─────────────────────────────────────────────┐
│ Do you need DTLS 1.3?                       │
│   YES → OpenSSL or wolfSSL                  │
│   NO  → Continue                            │
└─────────────────────────────────────────────┘
           ↓
┌─────────────────────────────────────────────┐
│ Is performance critical?                    │
│   YES → wolfSSL                             │
│   NO  → mbedTLS (RECOMMENDED)               │
└─────────────────────────────────────────────┘
```

### Use Case Recommendations

| Use Case | Recommended Provider | Alternative |
|----------|---------------------|-------------|
| OpenWRT Router | mbedTLS | TinyDTLS |
| Battery IoT Device | TinyDTLS | mbedTLS |
| Industrial Gateway | mbedTLS | wolfSSL |
| Desktop Development | OpenSSL | mbedTLS |
| Commercial Product | mbedTLS or wolfSSL | OpenSSL |
| Research/Testing | OpenSSL | Any |
| Legacy Deployment | TinyDTLS | mbedTLS |

---

## Provider Details

### mbedTLS Configuration

**Package Requirements (Ubuntu/Debian):**
```bash
sudo apt-get install libmbedtls-dev
```

**CMake Configuration:**
```cmake
option(DTLS_PROVIDER_MBEDTLS "Include mbedTLS provider" ON)
set(DTLS_DEFAULT_PROVIDER "mbedtls")
```

**Build Command:**
```bash
cmake -DDTLS_PROVIDER_MBEDTLS=ON -DDTLS_DEFAULT_PROVIDER=mbedtls ..
make
```

**Runtime Selection:**
```bash
./friendly_example_client --dtls-provider=mbedtls -h coaps://server:5684
```

**Features:**
- DTLS 1.2: ✅ Full support
- PSK: ✅ All cipher suites
- Certificates: ✅ X.509 with RSA/ECDSA
- CID: ✅ RFC 9146 (compile-time option: `MBEDTLS_SSL_DTLS_CONNECTION_ID`)

### TinyDTLS Configuration

**No External Package Required** - Built from source

**CMake Configuration:**
```cmake
option(DTLS_PROVIDER_TINYDTLS "Include TinyDTLS provider" ON)
```

**Build Command:**
```bash
cmake -DDTLS_PROVIDER_TINYDTLS=ON ..
make
```

**Runtime Selection:**
```bash
./friendly_example_client --dtls-provider=tinydtls -h coaps://server:5684
```

**Features:**
- DTLS 1.2: ✅ Full support
- PSK: ✅ (compile with `-DDTLS_WITH_PSK`)
- RPK: ✅ (compile with `-DDTLS_WITH_RPK`)
- CID: ✅ RFC 9146 (compile-time option: `DTLS_CID`)

### OpenSSL Configuration

**Package Requirements (Ubuntu/Debian):**
```bash
sudo apt-get install libssl-dev
```

**CMake Configuration:**
```cmake
option(DTLS_PROVIDER_OPENSSL "Include OpenSSL provider" ON)
set(DTLS_DEFAULT_PROVIDER "openssl")
```

**Build Command:**
```bash
cmake -DDTLS_PROVIDER_OPENSSL=ON -DDTLS_DEFAULT_PROVIDER=openssl ..
make
```

**Runtime Selection:**
```bash
./friendly_example_client --dtls-provider=openssl -h coaps://server:5684
```

**Features:**
- DTLS 1.2: ✅ Full support
- DTLS 1.3: ✅ OpenSSL 3.0+
- PSK: ✅ All cipher suites
- Certificates: ✅ X.509 full support
- CID: ⚠️ Experimental (OpenSSL 3.2+)

### wolfSSL Configuration

**Package Requirements:**
```bash
# From source or package manager
sudo apt-get install libwolfssl-dev
```

**CMake Configuration:**
```cmake
option(DTLS_PROVIDER_WOLFSSL "Include wolfSSL provider" ON)
set(DTLS_DEFAULT_PROVIDER "wolfssl")
```

**Build Command:**
```bash
cmake -DDTLS_PROVIDER_WOLFSSL=ON -DDTLS_DEFAULT_PROVIDER=wolfssl ..
make
```

**Runtime Selection:**
```bash
./friendly_example_client --dtls-provider=wolfssl -h coaps://server:5684
```

**Features:**
- DTLS 1.2: ✅ Full support
- DTLS 1.3: ✅ (with `WOLFSSL_DTLS13`)
- PSK: ✅ All cipher suites
- Certificates: ✅ X.509 full support
- CID: ✅ (with `WOLFSSL_DTLS_CID`)

---

## Build Configuration

### Multi-Provider Build

You can enable multiple providers in a single build:

```bash
cmake \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_OPENSSL=ON \
  -DDTLS_PROVIDER_WOLFSSL=OFF \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  ..
make
```

**Benefits:**
- Runtime provider selection
- Testing multiple providers
- Fallback options

**Drawbacks:**
- Larger binary size
- All provider libraries must be installed

### Single-Provider Build

For production, enable only the needed provider:

```bash
cmake \
  -DDTLS_PROVIDER_TINYDTLS=OFF \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_OPENSSL=OFF \
  -DDTLS_PROVIDER_WOLFSSL=OFF \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  ..
make
```

**Benefits:**
- Minimal binary size
- Simpler dependency management
- Faster builds

### Static vs Dynamic Linking

**Static Linking (Default):**
```cmake
option(DTLS_STATIC_LINK "Link DTLS libraries statically" ON)
```

**Dynamic Linking:**
```cmake
option(DTLS_STATIC_LINK "Link DTLS libraries statically" OFF)
```

**Recommendations:**
- Static: Embedded systems, single deployments
- Dynamic: Desktop, shared library environments

---

## Runtime Selection

### Command-Line Options

**List Available Providers:**
```bash
./friendly_example_client --list-dtls-providers
```

**Output:**
```
Available DTLS Providers:

  * tinydtls  - TinyDTLS 0.8.6 (Embedded)
                Lightweight DTLS 1.2, RFC 9146 CID support
                Best for: Constrained devices, minimal footprint

  * mbedtls   - mbedTLS 3.6.0 (Recommended Default)
                Production-grade DTLS 1.2, CID support
                Best for: General use, embedded systems

Usage: --dtls-provider=<name>  (default: auto)
```

**Select Provider:**
```bash
# Auto-detect (uses priority: mbedTLS > TinyDTLS > OpenSSL > wolfSSL)
./friendly_example_client -h coaps://server:5684

# Explicit selection
./friendly_example_client --dtls-provider=mbedtls -h coaps://server:5684
./friendly_example_client --dtls-provider=tinydtls -h coaps://server:5684
./friendly_example_client --dtls-provider=openssl -h coaps://server:5684
./friendly_example_client --dtls-provider=wolfssl -h coaps://server:5684
```

### Programmatic Selection

```cpp
#include "Connection.h"

// Use default provider (auto-detected)
Connection conn1("5683", AF_INET);

// Explicit provider selection
Connection conn2("5683", AF_INET, "mbedtls");
Connection conn3("5683", AF_INET, "tinydtls");

// Query provider information
const char* name = conn2.getProviderName();     // "mbedTLS"
const char* version = conn2.getProviderVersion(); // "3.6.0"
bool cid = conn2.providerSupportsCID();          // true
```

---

## Best Practices

### 1. Production Deployments

✅ **DO:**
- Use mbedTLS as default
- Enable only required providers
- Use static linking
- Test with target LwM2M servers
- Enable CID support if available

❌ **DON'T:**
- Mix providers in same deployment
- Use OpenSSL on embedded systems
- Disable security features for "performance"

### 2. Development & Testing

✅ **DO:**
- Enable all providers for testing
- Use OpenSSL for desktop development
- Test provider fallbacks
- Benchmark each provider

### 3. Constrained Devices

✅ **DO:**
- Use TinyDTLS for <256KB flash
- Profile memory usage
- Use PSK instead of certificates
- Enable CID for NAT traversal

### 4. Security Considerations

✅ **DO:**
- Keep libraries updated
- Use strong cipher suites
- Enable certificate validation
- Monitor security advisories

❌ **DON'T:**
- Disable verification in production
- Use weak PSK keys
- Ignore security warnings

---

## Troubleshooting

### Provider Not Available

**Error:**
```
Error: Invalid DTLS provider. Must be: auto|tinydtls|mbedtls|openssl|wolfssl
```

**Solution:**
Check enabled providers:
```bash
./friendly_example_client --list-dtls-providers
```

Rebuild with required provider:
```bash
cmake -DDTLS_PROVIDER_MBEDTLS=ON ..
make
```

### Library Not Found

**Error:**
```
CMake Error: Could not find MbedTLS
```

**Solution (Ubuntu/Debian):**
```bash
sudo apt-get install libmbedtls-dev
```

**Solution (from source):**
```bash
git clone https://github.com/Mbed-TLS/mbedtls.git
cd mbedtls
cmake .
make
sudo make install
```

### Handshake Failures

**Symptom:** DTLS handshake fails

**Debug Steps:**
1. Verify cipher suite compatibility
2. Check PSK/certificate configuration
3. Enable DTLS debug logging
4. Test with different provider
5. Verify server supports selected provider features

**Provider-Specific Debugging:**
```bash
# TinyDTLS
export DTLS_DEBUG=1

# mbedTLS
export MBEDTLS_DEBUG_LEVEL=3

# OpenSSL
export OPENSSL_DEBUG=1
```

### Performance Issues

**Symptom:** Slow DTLS handshake or throughput

**Optimization:**
1. Try wolfSSL (fastest)
2. Enable hardware crypto acceleration
3. Tune cipher suite selection
4. Profile with different providers

**Benchmark Command:**
```bash
time ./friendly_example_client --dtls-provider=<provider> ...
```

---

## Appendix

### A. Cipher Suite Support

| Cipher Suite | TinyDTLS | mbedTLS | OpenSSL | wolfSSL |
|--------------|----------|---------|---------|---------|
| PSK-AES128-CCM | ✅ | ✅ | ✅ | ✅ |
| PSK-AES128-GCM | ❌ | ✅ | ✅ | ✅ |
| ECDHE-PSK-AES128 | ❌ | ✅ | ✅ | ✅ |
| ECDHE-ECDSA-AES128 | ✅ | ✅ | ✅ | ✅ |
| RSA-AES128-CBC | ❌ | ✅ | ✅ | ✅ |

### B. Binary Size Comparison (Release Build)

| Provider | Client Binary | Provider Library | Total |
|----------|---------------|------------------|-------|
| TinyDTLS only | 450 KB | ~50 KB (static) | 500 KB |
| mbedTLS only | 450 KB | ~200 KB | 650 KB |
| OpenSSL only | 450 KB | ~500 KB | 950 KB |
| wolfSSL only | 450 KB | ~250 KB | 700 KB |
| All providers | 520 KB | Varies | 1.5+ MB |

### C. Memory Usage (Runtime)

| Provider | Heap (per session) | Stack |
|----------|-------------------|-------|
| TinyDTLS | ~8 KB | ~4 KB |
| mbedTLS | ~16 KB | ~8 KB |
| OpenSSL | ~32 KB | ~16 KB |
| wolfSSL | ~20 KB | ~12 KB |

### D. Supported Platforms

| Platform | TinyDTLS | mbedTLS | OpenSSL | wolfSSL |
|----------|----------|---------|---------|---------|
| Linux (x86_64) | ✅ | ✅ | ✅ | ✅ |
| Linux (ARM) | ✅ | ✅ | ✅ | ✅ |
| OpenWRT | ✅ | ✅ | ✅ | ✅ |
| FreeRTOS | ✅ | ✅ | ❌ | ✅ |
| Zephyr | ✅ | ✅ | ❌ | ✅ |
| macOS | ✅ | ✅ | ✅ | ✅ |
| Windows | ⚠️ | ✅ | ✅ | ✅ |

---

## Support & Resources

### Documentation
- mbedTLS: https://mbed-tls.readthedocs.io/
- TinyDTLS: https://github.com/eclipse/tinydtls
- OpenSSL: https://www.openssl.org/docs/
- wolfSSL: https://www.wolfssl.com/documentation/

### Community
- GitHub Issues: https://github.com/walt-id/friendly-lwm2m-client/issues
- LwM2M Forum: https://github.com/OpenMobileAlliance/OMA_LwM2M_for_Developers

---

**Document Version:** 1.0
**Last Updated:** April 12, 2026
**Maintained By:** Friendly LwM2M Client Team
