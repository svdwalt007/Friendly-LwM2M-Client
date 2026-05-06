# DTLS Multi-Provider Quick Reference

**Friendly LwM2M Client v1.2.2** | [Full Documentation →](DTLS_PROVIDER_SELECTION_GUIDE.md)

---

## 🚀 Quick Start (30 seconds)

```bash
# List available providers
./friendly_example_client --list-dtls-providers

# Use mbedTLS (recommended default)
./friendly_example_client -h coaps://server:5684

# Use specific provider
./friendly_example_client --dtls-provider=<provider> -h coaps://server:5684
```

---

## 📦 Available Providers

| Provider | Use When | Binary Size | Command |
|----------|----------|-------------|---------|
| **mbedTLS** ⭐ | Production (DEFAULT) | ~200KB | `--dtls-provider=mbedtls` |
| **TinyDTLS** | <256KB flash | ~50KB | `--dtls-provider=tinydtls` |
| **OpenSSL** | Desktop/server | ~500KB | `--dtls-provider=openssl` |
| **wolfSSL** | Performance-critical | ~250KB | `--dtls-provider=wolfssl` |

---

## 🛠️ Build Commands

### Production (mbedTLS only)
```bash
cmake -DDTLS_PROVIDER_MBEDTLS=ON -DDTLS_DEFAULT_PROVIDER=mbedtls ..
make
```

### Embedded (TinyDTLS only)
```bash
cmake -DDTLS_PROVIDER_TINYDTLS=ON ..
make
```

### Multi-Provider (testing)
```bash
cmake \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_DEFAULT_PROVIDER=auto \
  ..
make
```

---

## 📋 Feature Matrix

| Feature | TinyDTLS | mbedTLS | OpenSSL | wolfSSL |
|---------|:--------:|:-------:|:-------:|:-------:|
| DTLS 1.2 | ✅ | ✅ | ✅ | ✅ |
| DTLS 1.3 | ❌ | 🚧 | ✅ | ✅ |
| PSK | ✅ | ✅ | ✅ | ✅ |
| Certificates | ❌ | ✅ | ✅ | ✅ |
| RFC 9146 CID | ✅ | ✅ | ⚠️ | ✅ |

---

## 💻 Code Snippets

### Default Usage (Auto-Detect)
```cpp
#include "Connection.h"
Connection conn("5683", AF_INET);  // Uses mbedTLS
```

### Explicit Provider
```cpp
Connection conn("5683", AF_INET, "mbedtls");
```

### Query Provider
```cpp
std::cout << conn.getProviderName();    // "mbedTLS"
std::cout << conn.getProviderVersion(); // "3.6.0"
bool cid = conn.providerSupportsCID();  // true
```

---

## 🔧 Installation

### Ubuntu/Debian
```bash
# mbedTLS (recommended)
sudo apt-get install libmbedtls-dev

# OpenSSL
sudo apt-get install libssl-dev

# wolfSSL
sudo apt-get install libwolfssl-dev
```

### OpenWRT
```bash
opkg update
opkg install libmbedtls
```

---

## 📚 Documentation Links

- **[Selection Guide](DTLS_PROVIDER_SELECTION_GUIDE.md)** - Which provider?
- **[Migration Guide](MIGRATION_GUIDE_MULTI_DTLS.md)** - Upgrade from v1.2.1
- **[Implementation Plan](DTLS_MULTI_PROVIDER_IMPLEMENTATION_PLAN.md)** - Architecture
- **[Final Summary](DTLS_MULTI_PROVIDER_FINAL_SUMMARY.md)** - Complete overview

---

## 🎯 Decision Tree

```
Need certificates? → YES → mbedTLS/OpenSSL/wolfSSL
                  ↓ NO
Flash < 256KB?    → YES → TinyDTLS
                  ↓ NO
Production?       → YES → mbedTLS ⭐
                  ↓ NO
Need DTLS 1.3?    → YES → OpenSSL/wolfSSL
                  ↓ NO
Default           → mbedTLS ⭐
```

---

## ⚡ Common Tasks

### Show Version
```bash
./friendly_example_client --version
```

### List Providers
```bash
./friendly_example_client --list-dtls-providers
```

### Test Connection
```bash
./friendly_example_client \
  --dtls-provider=mbedtls \
  -h coaps://leshan.eclipseprojects.io:5684 \
  -v
```

### Check CID Support
```cpp
if (conn.isCidNegotiated(session)) {
    int version = conn.getCidVersion(session); // 54 = RFC 9146
}
```

---

## 🐛 Troubleshooting

### Provider not found
```bash
# Check available
./friendly_example_client --list-dtls-providers

# Install library
sudo apt-get install libmbedtls-dev

# Rebuild
cmake -DDTLS_PROVIDER_MBEDTLS=ON ..
make
```

### Handshake fails
```bash
# Try different provider
./friendly_example_client --dtls-provider=tinydtls ...

# Enable debug
export MBEDTLS_DEBUG_LEVEL=3
./friendly_example_client --dtls-provider=mbedtls ...
```

---

## 📞 Support

- **Issues:** https://github.com/walt-id/friendly-lwm2m-client/issues
- **Discussions:** https://github.com/walt-id/friendly-lwm2m-client/discussions

---

**Quick Ref v1.0** | [View Full Guide →](DTLS_PROVIDER_SELECTION_GUIDE.md)
