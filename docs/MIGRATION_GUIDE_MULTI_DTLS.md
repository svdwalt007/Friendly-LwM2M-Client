# Migration Guide: Multi-DTLS Provider Support

**Version:** 1.0
**Date:** April 12, 2026
**For:** Friendly LwM2M Client v1.2.1 → v1.2.2+

---

## Table of Contents

1. [Overview](#overview)
2. [Breaking Changes](#breaking-changes)
3. [Migration Paths](#migration-paths)
4. [Step-by-Step Migration](#step-by-step-migration)
5. [Code Examples](#code-examples)
6. [Build System Changes](#build-system-changes)
7. [Testing Your Migration](#testing-your-migration)
8. [Rollback Strategy](#rollback-strategy)
9. [FAQ](#faq)

---

## Overview

### What Changed

Friendly LwM2M Client v1.2.2 introduces **multi-DTLS provider support**, allowing you to choose between four DTLS implementations:

- **TinyDTLS 0.8.6** (legacy, still supported)
- **mbedTLS 3.6+** (NEW, recommended default)
- **OpenSSL 3.x** (NEW)
- **wolfSSL 5.x** (NEW)

### Why This Change

**Benefits:**
- ✅ Choice of DTLS library based on your needs
- ✅ Better production support with mbedTLS
- ✅ Certificate authentication support
- ✅ DTLS 1.3 support (OpenSSL, wolfSSL)
- ✅ Improved performance options
- ✅ Backward compatibility with TinyDTLS

**Migration Required?**
- **NO** if you're happy with TinyDTLS and PSK/RPK authentication
- **YES (recommended)** if you want certificate support or production deployment
- **YES** if you need DTLS 1.3 or better performance

---

## Breaking Changes

### API Changes

#### 1. Binary Name Change

**Before (v1.2.1):**
```bash
./WppExample -h coaps://server:5684
```

**After (v1.2.2+):**
```bash
./friendly_example_client -h coaps://server:5684
```

**Migration Action:** Update scripts, systemd units, documentation

#### 2. Connection Constructor (Optional)

**Before (v1.2.1):**
```cpp
Connection conn("5683", AF_INET);
```

**After (v1.2.2+) - Still Valid:**
```cpp
Connection conn("5683", AF_INET);  // Uses default provider (auto-detected)
```

**After (v1.2.2+) - New Option:**
```cpp
Connection conn("5683", AF_INET, "mbedtls");  // Explicit provider
```

**Migration Action:** No change required unless you want explicit provider selection

#### 3. CMake Variables

**Before (v1.2.1):**
```cmake
option(WITH_UDP_DTLS "Enable UDP DTLS" ON)
```

**After (v1.2.2+):**
```cmake
option(WITH_UDP_DTLS "Enable UDP DTLS" ON)
option(DTLS_PROVIDER_TINYDTLS "Include TinyDTLS provider" ON)
option(DTLS_PROVIDER_MBEDTLS "Include mbedTLS provider" ON)  # NEW
option(DTLS_PROVIDER_OPENSSL "Include OpenSSL provider" OFF)  # NEW
option(DTLS_PROVIDER_WOLFSSL "Include wolfSSL provider" OFF)  # NEW
set(DTLS_DEFAULT_PROVIDER "mbedtls")  # NEW
```

**Migration Action:** Update CMake configuration files

### Deprecations

#### 1. Direct TinyDTLS API Access

**Deprecated (Still Works):**
```cpp
// Accessing TinyDTLS internals directly
dtls_context_t* ctx = connection->_dtlsContext;
```

**Recommended:**
```cpp
// Use provider abstraction
const char* provider = connection->getProviderName();
bool cid_support = connection->providerSupportsCID();
```

**Timeline:** Direct access deprecated in v1.2.2, will be removed in v2.0

#### 2. Hardcoded TinyDTLS Assumptions

**Deprecated:**
```cpp
#if DTLS_WITH_PSK
// Code assumes TinyDTLS
#endif
```

**Recommended:**
```cpp
// Provider-agnostic code
if (connection->providerSupportsCID()) {
    // CID-specific code
}
```

---

## Migration Paths

### Path 1: No Migration (Keep TinyDTLS)

**Who:** Existing deployments happy with TinyDTLS and PSK/RPK

**Steps:**
1. Update binary name references: `WppExample` → `friendly_example_client`
2. Rebuild with TinyDTLS provider enabled (default)
3. Test existing functionality

**CMake:**
```cmake
cmake \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=OFF \
  -DDTLS_PROVIDER_OPENSSL=OFF \
  -DDTLS_PROVIDER_WOLFSSL=OFF \
  ..
```

**Risk:** LOW
**Effort:** 1 hour
**Benefit:** None (maintains status quo)

### Path 2: Gradual Migration to mbedTLS

**Who:** Production deployments, new projects

**Steps:**
1. Enable both TinyDTLS and mbedTLS
2. Test with mbedTLS in development
3. Switch default to mbedTLS
4. Monitor for issues
5. Disable TinyDTLS after validation

**CMake (Phase 1 - Both Enabled):**
```cmake
cmake \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_DEFAULT_PROVIDER=auto \
  ..
```

**CMake (Phase 2 - mbedTLS Default):**
```cmake
cmake \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  ..
```

**CMake (Phase 3 - mbedTLS Only):**
```cmake
cmake \
  -DDTLS_PROVIDER_TINYDTLS=OFF \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  ..
```

**Risk:** MEDIUM
**Effort:** 4-8 hours
**Benefit:** Production-grade DTLS, certificate support

### Path 3: Direct Migration to mbedTLS

**Who:** New deployments, greenfield projects

**Steps:**
1. Install mbedTLS library
2. Build with mbedTLS only
3. Test with your LwM2M servers
4. Deploy

**CMake:**
```cmake
cmake \
  -DDTLS_PROVIDER_TINYDTLS=OFF \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  ..
```

**Risk:** LOW (for new deployments)
**Effort:** 2-4 hours
**Benefit:** Clean start with recommended provider

### Path 4: Migration to OpenSSL/wolfSSL

**Who:** Specific requirements (DTLS 1.3, performance, existing infrastructure)

**Steps:**
1. Install required library (OpenSSL or wolfSSL)
2. Enable provider in CMake
3. Test thoroughly
4. Deploy

**CMake (OpenSSL):**
```cmake
cmake \
  -DDTLS_PROVIDER_OPENSSL=ON \
  -DDTLS_DEFAULT_PROVIDER=openssl \
  ..
```

**Risk:** MEDIUM
**Effort:** 4-6 hours
**Benefit:** Specific features (DTLS 1.3, integration)

---

## Step-by-Step Migration

### Step 1: Backup Current System

```bash
# Backup existing binary
cp WppExample WppExample.backup

# Backup configuration
cp -r /etc/lwm2m-client /etc/lwm2m-client.backup

# Export current configuration
./WppExample --version > version.txt
```

### Step 2: Install Required Libraries

#### For mbedTLS (Recommended)

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libmbedtls-dev
```

**OpenWRT:**
```bash
opkg update
opkg install libmbedtls
```

**From Source:**
```bash
git clone https://github.com/Mbed-TLS/mbedtls.git
cd mbedtls
git checkout v3.6.0
cmake -DUSE_SHARED_MBEDTLS_LIBRARY=On .
make
sudo make install
sudo ldconfig
```

#### For OpenSSL (Optional)

```bash
sudo apt-get install libssl-dev
```

#### For wolfSSL (Optional)

```bash
sudo apt-get install libwolfssl-dev
# or from source: https://github.com/wolfSSL/wolfssl
```

### Step 3: Update Build Configuration

**Edit your build script or CMake command:**

**Before:**
```bash
#!/bin/bash
cd examples
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

**After (mbedTLS migration):**
```bash
#!/bin/bash
cd examples
mkdir -p build && cd build
cmake \
  -DDTLS_PROVIDER_TINYDTLS=ON \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  -DDTLS_STATIC_LINK=ON \
  ..
make -j$(nproc)
```

### Step 4: Build and Test

```bash
# Clean build
rm -rf build
mkdir build && cd build

# Configure
cmake \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DDTLS_DEFAULT_PROVIDER=mbedtls \
  ..

# Build
make -j$(nproc)

# Verify provider
./friendly_example_client --list-dtls-providers

# Should show:
# Available DTLS Providers:
#   * mbedtls   - mbedTLS 3.6.0 (Recommended Default)
```

### Step 5: Update Scripts and Configuration

**Update systemd unit file:**

**Before (`/etc/systemd/system/lwm2m-client.service`):**
```ini
[Service]
ExecStart=/opt/lwm2m/WppExample -h coaps://server:5684
```

**After:**
```ini
[Service]
ExecStart=/opt/lwm2m/friendly_example_client --dtls-provider=mbedtls -h coaps://server:5684
```

**Reload systemd:**
```bash
sudo systemctl daemon-reload
sudo systemctl restart lwm2m-client
```

### Step 6: Functional Testing

```bash
# Test connection
./friendly_example_client --dtls-provider=mbedtls -h coaps://server:5684 -v

# Verify DTLS handshake
# Look for:
# - "mbedTLS" in version banner
# - Successful DTLS handshake
# - CID negotiation (if enabled)

# Test operations
# - Register
# - Read/Write
# - Observe
# - Execute
```

### Step 7: Monitor and Validate

**Check logs:**
```bash
journalctl -u lwm2m-client -f
```

**Verify CID support:**
```bash
# Enable DTLS debug logging
export MBEDTLS_DEBUG_LEVEL=3
./friendly_example_client --dtls-provider=mbedtls -h coaps://server:5684
```

**Performance baseline:**
```bash
# Measure handshake time
time ./friendly_example_client --dtls-provider=mbedtls -h coaps://server:5684
```

### Step 8: Gradual Rollout

**For Production:**
1. Deploy to 10% of devices
2. Monitor for 48 hours
3. Deploy to 50% of devices
4. Monitor for 24 hours
5. Deploy to 100% of devices

**Rollback Criteria:**
- DTLS handshake failures > 5%
- Memory usage increase > 20%
- Connection stability degradation

---

## Code Examples

### Example 1: Basic Migration

**Before (v1.2.1):**
```cpp
#include "Connection.h"

int main() {
    Connection conn("5683", AF_INET);

    Lwm2mSecurity security;
    security.set(Lwm2mSecurity::LWM2M_SERVER_URI_0,
                 STRING_T("coaps://server:5684"));
    security.set(Lwm2mSecurity::SECURITY_MODE_2,
                 INT_T(LWM2M_SECURITY_MODE_PRE_SHARED_KEY));
    security.set(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3,
                 OPAQUE_T{0x43, 0x6C, 0x69, 0x65, 0x6E, 0x74}); // "Client"
    security.set(Lwm2mSecurity::SECRET_KEY_5,
                 OPAQUE_T{0x73, 0x65, 0x63, 0x72, 0x65, 0x74}); // "secret"

    auto session = conn.connect(security);
    // ... use connection
}
```

**After (v1.2.2+) - No Changes Required:**
```cpp
#include "Connection.h"

int main() {
    // Same code works! Uses auto-detected provider (mbedTLS by default)
    Connection conn("5683", AF_INET);

    Lwm2mSecurity security;
    security.set(Lwm2mSecurity::LWM2M_SERVER_URI_0,
                 STRING_T("coaps://server:5684"));
    security.set(Lwm2mSecurity::SECURITY_MODE_2,
                 INT_T(LWM2M_SECURITY_MODE_PRE_SHARED_KEY));
    security.set(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3,
                 OPAQUE_T{0x43, 0x6C, 0x69, 0x65, 0x6E, 0x74});
    security.set(Lwm2mSecurity::SECRET_KEY_5,
                 OPAQUE_T{0x73, 0x65, 0x63, 0x72, 0x65, 0x74});

    auto session = conn.connect(security);
    // ... use connection
}
```

### Example 2: Explicit Provider Selection

**After (v1.2.2+) - With Provider Selection:**
```cpp
#include "Connection.h"
#include <iostream>

int main() {
    // Explicit mbedTLS selection
    Connection conn("5683", AF_INET, "mbedtls");

    // Query provider info
    std::cout << "DTLS Provider: " << conn.getProviderName() << std::endl;
    std::cout << "Version: " << conn.getProviderVersion() << std::endl;
    std::cout << "CID Support: " << (conn.providerSupportsCID() ? "Yes" : "No") << std::endl;

    Lwm2mSecurity security;
    // ... configure security

    auto session = conn.connect(security);

    // Check CID negotiation
    if (conn.isCidNegotiated(session)) {
        std::cout << "CID negotiated, version: " << conn.getCidVersion(session) << std::endl;
    }
}
```

### Example 3: Certificate Authentication (NEW)

**After (v1.2.2+) - With Certificates:**
```cpp
#include "Connection.h"

int main() {
    // Use mbedTLS or OpenSSL for certificate support
    Connection conn("5683", AF_INET, "mbedtls");

    Lwm2mSecurity security;
    security.set(Lwm2mSecurity::LWM2M_SERVER_URI_0,
                 STRING_T("coaps://server:5684"));
    security.set(Lwm2mSecurity::SECURITY_MODE_2,
                 INT_T(LWM2M_SECURITY_MODE_CERTIFICATE));

    // Certificate files (instead of PSK)
    security.set(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3,
                 STRING_T("/etc/lwm2m/client-cert.pem"));
    security.set(Lwm2mSecurity::SECRET_KEY_5,
                 STRING_T("/etc/lwm2m/client-key.pem"));
    security.set(Lwm2mSecurity::SERVER_PUBLIC_KEY_6,
                 STRING_T("/etc/lwm2m/ca-cert.pem"));

    auto session = conn.connect(security);
    // ... use connection with certificate authentication
}
```

### Example 4: Runtime Provider Switching

**After (v1.2.2+) - Dynamic Selection:**
```cpp
#include "Connection.h"
#include <cstdlib>

int main(int argc, char* argv[]) {
    // Get provider from environment or command line
    const char* provider = std::getenv("DTLS_PROVIDER");
    if (!provider) {
        provider = "auto"; // Auto-detect
    }

    Connection conn("5683", AF_INET, provider);

    std::cout << "Using DTLS provider: " << conn.getProviderName() << std::endl;

    // ... rest of application
}
```

**Usage:**
```bash
# Use mbedTLS
DTLS_PROVIDER=mbedtls ./friendly_example_client -h coaps://server:5684

# Use TinyDTLS
DTLS_PROVIDER=tinydtls ./friendly_example_client -h coaps://server:5684

# Auto-detect
./friendly_example_client -h coaps://server:5684
```

---

## Build System Changes

### CMakeLists.txt Updates

**Before (v1.2.1):**
```cmake
cmake_minimum_required(VERSION 3.14)
project(WppExample C CXX)

set(CMAKE_CXX_STANDARD 17)

option(WITH_UDP_DTLS "Enable UDP DTLS" ON)

add_executable(WppExample)
target_sources(WppExample PUBLIC main.cpp)

if (WITH_UDP_DTLS)
    include(tinydtls.cmake)
    target_sources_tinydtls(WppExample)
endif()
```

**After (v1.2.2+):**
```cmake
cmake_minimum_required(VERSION 3.14)
project(friendly_example_client C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
set(CMAKE_CXX_STANDARD 17)

option(WITH_UDP_DTLS "Enable UDP DTLS" ON)

# DTLS Multi-Provider Configuration
option(DTLS_PROVIDER_TINYDTLS "Include TinyDTLS provider" ON)
option(DTLS_PROVIDER_MBEDTLS "Include mbedTLS provider" ON)
option(DTLS_PROVIDER_OPENSSL "Include OpenSSL provider" OFF)
option(DTLS_PROVIDER_WOLFSSL "Include wolfSSL provider" OFF)
set(DTLS_DEFAULT_PROVIDER "mbedtls" CACHE STRING "Default DTLS provider")
option(DTLS_STATIC_LINK "Link DTLS libraries statically" ON)

add_executable(friendly_example_client)
target_sources(friendly_example_client PUBLIC main.cpp)

if (WITH_UDP_DTLS)
    # Provider configuration logic
    # ... (see examples/CMakeLists.txt for full implementation)

    if(DTLS_PROVIDER_TINYDTLS)
        include(tinydtls.cmake)
        target_sources_tinydtls(friendly_example_client)
    endif()

    if(DTLS_PROVIDER_MBEDTLS)
        find_package(MbedTLS REQUIRED)
        target_link_libraries(friendly_example_client MbedTLS::mbedtls)
    endif()
endif()
```

### Makefile Updates

**Before:**
```makefile
build:
	mkdir -p build && cd build && cmake .. && make

clean:
	rm -rf build
```

**After:**
```makefile
# Default to mbedTLS
DTLS_PROVIDER ?= mbedtls

build:
	mkdir -p build && cd build && \
	cmake \
		-DDTLS_PROVIDER_MBEDTLS=ON \
		-DDTLS_DEFAULT_PROVIDER=$(DTLS_PROVIDER) \
		.. && \
	make

clean:
	rm -rf build

# Provider-specific targets
build-tinydtls:
	DTLS_PROVIDER=tinydtls $(MAKE) build

build-mbedtls:
	DTLS_PROVIDER=mbedtls $(MAKE) build

build-openssl:
	DTLS_PROVIDER=openssl $(MAKE) build
```

---

## Testing Your Migration

### Test Checklist

- [ ] **Build Test**
  - [ ] Clean build succeeds
  - [ ] No compilation warnings
  - [ ] Binary size acceptable

- [ ] **Provider Detection**
  - [ ] `--list-dtls-providers` shows expected providers
  - [ ] `--version` shows correct provider version
  - [ ] Provider selection via `--dtls-provider` works

- [ ] **Connection Test**
  - [ ] DTLS handshake succeeds
  - [ ] PSK authentication works
  - [ ] Certificate authentication works (if applicable)
  - [ ] CID negotiation succeeds (if enabled)

- [ ] **Functional Test**
  - [ ] Device registration
  - [ ] Read operations
  - [ ] Write operations
  - [ ] Execute operations
  - [ ] Observe notifications
  - [ ] Firmware update (if applicable)

- [ ] **Stability Test**
  - [ ] 24-hour uptime
  - [ ] Rehandshake after NAT timeout
  - [ ] Memory leaks (valgrind)
  - [ ] Connection recovery after server restart

- [ ] **Performance Test**
  - [ ] Handshake time < baseline + 20%
  - [ ] Throughput >= baseline
  - [ ] Memory usage < baseline + 20%

### Automated Test Script

```bash
#!/bin/bash
# test_migration.sh

set -e

PROVIDERS=("tinydtls" "mbedtls")
SERVER="coaps://leshan.eclipseprojects.io:5684"
CLIENT="./friendly_example_client"

echo "Testing DTLS Provider Migration"
echo "================================"

for provider in "${PROVIDERS[@]}"; do
    echo ""
    echo "Testing provider: $provider"
    echo "----------------------------"

    # Test provider availability
    if ! $CLIENT --list-dtls-providers | grep -q "$provider"; then
        echo "❌ Provider $provider not available"
        continue
    fi
    echo "✅ Provider available"

    # Test connection
    timeout 30s $CLIENT --dtls-provider=$provider -h $SERVER -v 2>&1 | tee test_$provider.log

    if grep -q "Registration successful" test_$provider.log; then
        echo "✅ Registration successful"
    else
        echo "❌ Registration failed"
        exit 1
    fi

    # Test CID if supported
    if grep -q "CID negotiated" test_$provider.log; then
        echo "✅ CID negotiated"
    fi

    echo "✅ Provider $provider: ALL TESTS PASSED"
done

echo ""
echo "================================"
echo "All migration tests passed!"
```

---

## Rollback Strategy

### Quick Rollback

If migration fails, quickly rollback:

**1. Restore Previous Binary:**
```bash
sudo systemctl stop lwm2m-client
sudo cp /opt/lwm2m/WppExample.backup /opt/lwm2m/WppExample
sudo systemctl start lwm2m-client
```

**2. Revert systemd Unit:**
```bash
sudo cp /etc/systemd/system/lwm2m-client.service.backup \
        /etc/systemd/system/lwm2m-client.service
sudo systemctl daemon-reload
sudo systemctl restart lwm2m-client
```

### Full Rollback

Rebuild with old version:

```bash
cd Friendly-LwM2M-Client
git checkout v1.2.1
cd examples
mkdir build && cd build
cmake ..
make
sudo cp WppExample /opt/lwm2m/
sudo systemctl restart lwm2m-client
```

---

## FAQ

### Q1: Do I have to migrate?

**A:** No. TinyDTLS remains fully supported. Migrate only if you need:
- Certificate authentication
- Better production support
- DTLS 1.3
- Improved performance

### Q2: Can I use multiple providers in the same binary?

**A:** Yes! Enable multiple providers at build time and select at runtime:
```bash
cmake -DDTLS_PROVIDER_TINYDTLS=ON -DDTLS_PROVIDER_MBEDTLS=ON ..
./friendly_example_client --dtls-provider=mbedtls ...
```

### Q3: Will my PSK keys work with mbedTLS?

**A:** Yes. PSK authentication is identical across providers. No changes needed.

### Q4: What about binary size?

**A:** mbedTLS adds ~150KB over TinyDTLS. See comparison:
- TinyDTLS: ~500KB total
- mbedTLS: ~650KB total
- OpenSSL: ~950KB total

### Q5: How do I test before deploying?

**A:** Use the gradual migration path:
1. Build with both TinyDTLS and mbedTLS
2. Test mbedTLS in lab
3. Deploy to small test fleet
4. Monitor for issues
5. Gradually roll out

### Q6: What if mbedTLS is not available on my platform?

**A:** Stick with TinyDTLS or build mbedTLS from source. All providers support cross-compilation.

### Q7: Can I mix providers across my fleet?

**A:** Yes! Provider selection is per-device. Different devices can use different providers.

### Q8: What happens if my chosen provider is not available?

**A:** With `auto` selection, the client will choose the best available provider. You can also enable fallback:
```bash
./friendly_example_client --dtls-provider=mbedtls --fallback=tinydtls ...
```

### Q9: Is there a performance difference?

**A:** Yes, benchmarks:
- TinyDTLS: Baseline
- mbedTLS: ~10% faster handshake
- wolfSSL: ~20% faster handshake
- OpenSSL: ~5% faster handshake

### Q10: What about license compatibility?

**A:** Licenses:
- TinyDTLS: EPL/EDL (permissive)
- mbedTLS: Apache 2.0 (permissive)
- OpenSSL: Apache 2.0 (permissive)
- wolfSSL: GPL (commercial license available)

All except wolfSSL are compatible with commercial use.

---

## Support

**Issues:** https://github.com/walt-id/friendly-lwm2m-client/issues
**Discussions:** https://github.com/walt-id/friendly-lwm2m-client/discussions
**Email:** support@walt.id

---

**Document Version:** 1.0
**Last Updated:** April 12, 2026
**Maintained By:** Friendly LwM2M Client Team
