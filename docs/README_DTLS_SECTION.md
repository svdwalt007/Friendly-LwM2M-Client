# Multi-DTLS Provider Support Section
# To be inserted into README.md after "What's New in v1.2.2"

## Multi-DTLS Provider Support ⭐ NEW in v1.2.2

The Friendly LwM2M Client now supports **multiple DTLS providers**, allowing you to choose the best DTLS implementation for your deployment:

| Provider | Version | Best For | Binary Size |
|----------|---------|----------|-------------|
| **mbedTLS** ⭐ | 3.6+ | Production deployments (DEFAULT) | ~200KB |
| **TinyDTLS** | 0.8.6 | Constrained devices | ~50KB |
| **OpenSSL** | 3.x | Desktop/server environments | ~500KB |
| **wolfSSL** | 5.x | Performance-critical applications | ~250KB |

### Quick Start

**List available DTLS providers:**
```bash
./friendly_example_client --list-dtls-providers
```

**Select DTLS provider:**
```bash
# Use default (mbedTLS - recommended)
./friendly_example_client -h coaps://server:5684

# Explicit provider selection
./friendly_example_client --dtls-provider=mbedtls -h coaps://server:5684
./friendly_example_client --dtls-provider=tinydtls -h coaps://server:5684
./friendly_example_client --dtls-provider=openssl -h coaps://server:5684
```

### Build Configuration

**Enable specific providers:**
```bash
cmake \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_PROVIDER_OPENSSL=OFF \
  -DDTLS_PROVIDER_WOLFSSL=OFF \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  ..
make
```

### Feature Comparison

| Feature | TinyDTLS | mbedTLS | OpenSSL | wolfSSL |
|---------|----------|---------|---------|---------|
| DTLS 1.2 | ✅ | ✅ | ✅ | ✅ |
| DTLS 1.3 | ❌ | 🚧 | ✅ | ✅ |
| PSK Auth | ✅ | ✅ | ✅ | ✅ |
| Certificate Auth | ❌ | ✅ | ✅ | ✅ |
| RFC 9146 CID | ✅ | ✅ | ⚠️ | ✅ |

### Documentation

- **[DTLS Provider Selection Guide](DTLS_PROVIDER_SELECTION_GUIDE.md)** - Detailed comparison and recommendations
- **[Migration Guide](MIGRATION_GUIDE_MULTI_DTLS.md)** - Step-by-step migration from v1.2.1
- **[Implementation Plan](DTLS_MULTI_PROVIDER_IMPLEMENTATION_PLAN.md)** - Technical architecture

### Key Benefits

✅ **Choice** - Select the best DTLS library for your use case
✅ **Production-Ready** - mbedTLS provides production-grade security
✅ **Certificate Support** - X.509 certificates with mbedTLS/OpenSSL/wolfSSL
✅ **DTLS 1.3** - Latest protocol version with OpenSSL/wolfSSL
✅ **Backward Compatible** - TinyDTLS remains fully supported
✅ **Runtime Selection** - Switch providers via command-line option

### Migration from v1.2.1

Existing deployments continue to work with TinyDTLS. To migrate to mbedTLS:

1. Install mbedTLS library:
   ```bash
   sudo apt-get install libmbedtls-dev
   ```

2. Rebuild with mbedTLS:
   ```bash
   cmake -DDTLS_PROVIDER_MBEDTLS=ON -DDTLS_DEFAULT_PROVIDER=mbedtls ..
   make
   ```

3. Update binary name:
   ```bash
   # Old: WppExample
   # New: friendly_example_client
   ```

See the [Migration Guide](MIGRATION_GUIDE_MULTI_DTLS.md) for detailed instructions.
