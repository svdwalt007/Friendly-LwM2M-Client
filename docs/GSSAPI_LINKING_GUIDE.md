# GSSAPI Linking Guide for Static Builds

This guide explains the GSSAPI/Kerberos linking requirements when building the Friendly LwM2M Client with static libraries.

## Quick Fix

If you're getting GSSAPI undefined reference errors:

```bash
# Install GSSAPI libraries
./scripts/install_gssapi_deps.sh

# Rebuild
cd build && rm -rf * && cmake .. && make -j$(nproc)
```

## Problem Background

### What is GSSAPI?

GSSAPI (Generic Security Services Application Program Interface) is a standardized interface for Kerberos authentication. It's used by `libcurl` and `libssh` for:

- **Kerberos authentication** (single sign-on)
- **SPNEGO** (HTTP negotiate authentication)
- **SSH GSSAPI** key exchange

### Why Does Static Linking Need It?

When using **dynamic libraries** (`.so` files):
- Dependencies are resolved automatically at runtime
- GSSAPI libraries are loaded when needed

When using **static libraries** (`.a` files):
- ALL dependencies must be linked explicitly at compile time
- Missing libraries cause "undefined reference" errors

### Ubuntu/Debian Default Behavior

On Ubuntu/Debian, `libcurl` and `libssh` are compiled **with GSSAPI support** by default:

```bash
# Check if your libcurl uses GSSAPI
curl-config --features | grep GSS-API
# Output: GSS-API

# Check symbols in static libcurl
nm /usr/lib/x86_64-linux-gnu/libcurl.a | grep gss_
# Shows many GSSAPI symbols
```

This means when you statically link `libcurl.a`, you **must** also link the GSSAPI libraries.

## The Build Errors

### Example Error Output

```
/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libcurl.a(libcurl_la-krb5.o):
  undefined reference to `gss_init_sec_context'
/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libcurl.a(libcurl_la-curl_gssapi.o):
  undefined reference to `gss_display_status'
/usr/bin/ld: /usr/lib/x86_64-linux-gnu/libssh.a(gssapi.c.o):
  undefined reference to `gss_delete_sec_context'
```

### Why This Happens

```
                    ┌─────────────────┐
                    │ Your Application│
                    └────────┬────────┘
                             │
                     ┌───────▼───────┐
                     │ libcurl.a     │
                     │ (static)      │
                     └───┬───────┬───┘
                         │       │
          ┌──────────────┘       └──────────────┐
          │                                     │
   ┌──────▼────────┐                   ┌───────▼──────┐
   │ OpenSSL       │                   │ GSSAPI       │
   │ (linked ✓)    │                   │ (missing ✗)  │
   └───────────────┘                   └──────────────┘
```

The build fails because libcurl needs GSSAPI, but it's not linked.

## Solution Architecture

### Two-Stage Detection

The CMake configuration now uses a two-stage approach:

```
┌─────────────────────────────────────────────────┐
│ Stage 1: Try pkg-config                        │
│   ✓ Most reliable                              │
│   ✓ Cross-platform                             │
│   ✓ Handles dependencies automatically         │
└────────────┬────────────────────────────────────┘
             │
             ├──[SUCCESS]─────► Use GSSAPI_LIBRARIES
             │
             └──[FAILED]──┐
                          │
┌─────────────────────────▼────────────────────────┐
│ Stage 2: Manual library search                  │
│   - find_library(gssapi_krb5)                   │
│   - find_library(krb5)                          │
│   - find_library(k5crypto)                      │
│   - find_library(com_err)                       │
│   - find_library(keyutils)                      │
└────────────┬─────────────────────────────────────┘
             │
             ├──[FOUND]──────► Set GSSAPI_LIBRARIES
             │
             └──[NOT FOUND]──► Warning message + Continue
```

### CMake Code Flow

```cmake
# 1. Try pkg-config first
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(KRB5_GSSAPI QUIET krb5-gssapi)
    if(KRB5_GSSAPI_FOUND)
        set(GSSAPI_LIBRARIES ${KRB5_GSSAPI_LIBRARIES})
        set(GSSAPI_FOUND TRUE)
    endif()
endif()

# 2. Fall back to manual search
if(NOT GSSAPI_FOUND)
    find_library(GSSAPI_KRB5_LIB gssapi_krb5)
    find_library(KRB5_LIB krb5)
    # ... other libraries ...

    if(GSSAPI_KRB5_LIB AND KRB5_LIB)
        set(GSSAPI_LIBRARIES ${GSSAPI_KRB5_LIB} ${KRB5_LIB} ...)
        set(GSSAPI_FOUND TRUE)
    else()
        message(WARNING "GSSAPI not found - build may fail")
    endif()
endif()

# 3. Link if found
if(GSSAPI_FOUND)
    target_link_libraries(my_target PRIVATE ${GSSAPI_LIBRARIES})
endif()
```

## Installation Methods

### Method 1: Automated Script (Recommended)

```bash
./scripts/install_gssapi_deps.sh
```

This script:
- Detects your Linux distribution
- Installs the correct packages
- Verifies installation
- Tests CMake detection

### Method 2: Manual Installation

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y libkrb5-dev libkeyutils-dev pkg-config
```

**RHEL/CentOS/Fedora:**
```bash
# Fedora 22+ / RHEL 8+
sudo dnf install -y krb5-devel keyutils-libs-devel pkgconfig

# RHEL/CentOS 7
sudo yum install -y krb5-devel keyutils-libs-devel pkgconfig
```

**Arch Linux:**
```bash
sudo pacman -S krb5 keyutils pkgconf
```

**Alpine Linux:**
```bash
apk add krb5-dev keyutils-dev e2fsprogs-dev pkgconfig
```

## Package Breakdown

| Package | Provides | Required By | Purpose |
|---------|----------|-------------|---------|
| `libkrb5-dev` | `libgssapi_krb5.so`, `libkrb5.so` | libcurl, libssh | Kerberos 5 + GSSAPI |
| `libkeyutils-dev` | `libkeyutils.so` | Kerberos | Linux keyring access |
| `pkg-config` | `pkg-config` binary | CMake | Library detection |
| Headers | `krb5.h`, `gssapi.h` | Compilation | Development headers |

## Verification Steps

### Step 1: Check Package Installation

```bash
# Debian/Ubuntu
dpkg -l | grep -E "libkrb5-dev|libkeyutils-dev"

# RHEL/Fedora
rpm -qa | grep -E "krb5-devel|keyutils"

# Any Linux
ldconfig -p | grep -E "gssapi|krb5"
```

Expected output:
```
libgssapi_krb5.so.2 -> /usr/lib/x86_64-linux-gnu/libgssapi_krb5.so.2
libkrb5.so.3 -> /usr/lib/x86_64-linux-gnu/libkrb5.so.3
```

### Step 2: Check pkg-config

```bash
pkg-config --exists krb5-gssapi && echo "✓ Found" || echo "✗ Not found"
pkg-config --libs krb5-gssapi
```

Expected output:
```
✓ Found
-lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err
```

### Step 3: Check CMake Detection

```bash
cd build
rm -rf *
cmake .. 2>&1 | grep -i gssapi
```

Expected output:
```
-- Found GSSAPI via pkg-config: gssapi_krb5;krb5;k5crypto;com_err
```

Or (fallback):
```
-- Found GSSAPI for CURL/SSH: /usr/lib/x86_64-linux-gnu/libgssapi_krb5.so;...
```

## Linking Order

For static linking, the order matters:

```cmake
target_link_libraries(my_app
    # 1. Application library
    friendly_lwm2m

    # 2. Direct dependencies
    CURL::libcurl

    # 3. CURL's dependencies
    ${SSH_LIB}           # libssh
    ${NGHTTP2_LIB}       # HTTP/2

    # 4. SSH/CURL authentication
    ${GSSAPI_LIBRARIES}  # GSSAPI/Kerberos

    # 5. Cryptography (must be last)
    OpenSSL::SSL
    OpenSSL::Crypto
)
```

**Why this order?**
- Libraries that **use** symbols must come **before** libraries that **provide** symbols
- OpenSSL is last because both CURL and GSSAPI use it

## Troubleshooting

### Problem: "GSSAPI not found" but packages installed

**Solution 1:** pkg-config can't find the `.pc` file

```bash
# Find the .pc file location
find /usr -name "krb5-gssapi.pc" 2>/dev/null

# Add to PKG_CONFIG_PATH
export PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig:$PKG_CONFIG_PATH
cmake ..
```

**Solution 2:** Wrong package name

```bash
# Try different pkg-config names
pkg-config --exists krb5 && echo "Use krb5"
pkg-config --exists mit-krb5-gssapi && echo "Use mit-krb5-gssapi"
pkg-config --exists heimdal-gssapi && echo "Use heimdal-gssapi"
```

### Problem: Build still fails with GSSAPI errors

**Check:** Did CMake actually find GSSAPI?

```bash
cd build
grep "GSSAPI" CMakeCache.txt
```

Should show:
```
GSSAPI_FOUND:BOOL=TRUE
GSSAPI_LIBRARIES:STRING=gssapi_krb5;krb5;...
```

If not found, libraries aren't installed correctly.

### Problem: Multiple Kerberos implementations

You might have both MIT Kerberos and Heimdal:

```bash
# Check which one libcurl was compiled against
ldd /usr/lib/x86_64-linux-gnu/libcurl.so | grep krb5
```

Match your dev packages to the same implementation.

## Alternative: Build Without GSSAPI

If you don't need Kerberos authentication:

### Option 1: Use Dynamic Linking

```bash
cmake -DDTLS_STATIC_LINK=OFF ..
```

Dynamic libraries handle dependencies automatically.

### Option 2: Rebuild libcurl Without GSSAPI

```bash
# Download libcurl source
wget https://curl.se/download/curl-8.5.0.tar.gz
tar xzf curl-8.5.0.tar.gz
cd curl-8.5.0

# Configure without GSSAPI
./configure --without-gssapi --enable-static --disable-shared \
    --with-ssl --prefix=/usr/local

# Build and install
make -j$(nproc)
sudo make install

# Update library path in CMakeLists.txt
cmake -DCURL_LIBRARY=/usr/local/lib/libcurl.a ..
```

## Build Environment Matrix

| Static Linking | GSSAPI Installed | Result |
|----------------|------------------|--------|
| OFF | No | ✓ Builds (dynamic libs) |
| OFF | Yes | ✓ Builds (dynamic libs) |
| ON | No | ✗ Link errors |
| ON | Yes | ✓ Builds |

## Performance Impact

Including GSSAPI libraries adds to binary size:

```
Without GSSAPI:  ~8 MB binary
With GSSAPI:     ~12 MB binary (+4 MB)
```

But provides these features:
- Kerberos authentication
- SPNEGO/Negotiate HTTP auth
- SSH GSSAPI key exchange
- Enterprise SSO integration

## Security Considerations

### When to Include GSSAPI

✅ **Include if:**
- Your environment uses Kerberos
- You need enterprise SSO
- Connecting to Kerberos-protected resources
- Building for corporate networks

❌ **Exclude if:**
- Embedded/IoT devices (size-constrained)
- No Kerberos infrastructure
- Public internet only
- Minimal attack surface required

### Kerberos Configuration

If you include GSSAPI, configure `/etc/krb5.conf`:

```ini
[libdefaults]
    default_realm = YOUR.DOMAIN.COM
    dns_lookup_kdc = true
    dns_lookup_realm = true
```

Without proper configuration, GSSAPI is harmless but non-functional.

## References

- [MIT Kerberos Documentation](https://web.mit.edu/kerberos/)
- [RFC 4121 - Kerberos GSSAPI](https://tools.ietf.org/html/rfc4121)
- [libcurl GSSAPI](https://curl.se/docs/manpage.html#--negotiate)
- [CMake pkg-config](https://cmake.org/cmake/help/latest/module/FindPkgConfig.html)

## Summary

**Problem:** Static linking requires explicit GSSAPI libraries
**Solution:** Install dev packages + enhanced CMake detection
**Result:** Build succeeds with full authentication support

For most users:
```bash
./scripts/install_gssapi_deps.sh && cd build && cmake .. && make
```

That's it!
