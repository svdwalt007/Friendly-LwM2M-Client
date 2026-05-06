# Building Minimal libcurl for LwM2M Client

## Why Build a Custom libcurl?

The system's `libcurl.a` was compiled with many features you don't need for an IoT LwM2M client:

| Feature | Needed for LwM2M? | Dependencies It Brings |
|---------|-------------------|------------------------|
| **HTTP/HTTPS** | ✅ YES | OpenSSL (2 libs) |
| **HTTP/2** | ⚠️ OPTIONAL | nghttp2 (1 lib) |
| SSH/SCP/SFTP | ❌ NO | libssh + GSSAPI/Kerberos (6 libs) |
| **LDAP** | ❌ NO | libldap + GnuTLS + SASL + databases (18+ libs) |
| RTMP | ❌ NO | librtmp (1 lib) |
| IDN | ⚠️ OPTIONAL | libidn2 + libunistring (2 libs) |

**Current:** 28 libraries
**Minimal:** 5-8 libraries (80% reduction!)

## What You Actually Need

For LwM2M firmware updates over HTTPS:

1. **HTTP/1.1** - Core protocol (built-in to curl)
2. **TLS/SSL** - HTTPS encryption (OpenSSL)
3. **Compression** - Optional: gzip, brotli, zstd
4. **HTTP/2** - Optional but recommended for efficiency

**Result:** 5-8 libraries instead of 28

## Step-by-Step Build Guide

### Step 1: Install Build Dependencies

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    autoconf \
    automake \
    libtool \
    pkg-config \
    libssl-dev \
    zlib1g-dev \
    libbrotli-dev \
    libnghttp2-dev \
    libzstd-dev
```

### Step 2: Download libcurl Source

```bash
cd /tmp
wget https://curl.se/download/curl-8.5.0.tar.gz
tar xzf curl-8.5.0.tar.gz
cd curl-8.5.0
```

**Latest version check:** https://curl.se/download/

### Step 3: Configure Minimal Build

```bash
./configure \
    --prefix=/usr/local \
    --enable-static \
    --disable-shared \
    --with-openssl \
    --with-zlib \
    --with-brotli \
    --with-nghttp2 \
    --with-zstd \
    --disable-ldap \
    --disable-ldaps \
    --disable-rtsp \
    --disable-dict \
    --disable-telnet \
    --disable-tftp \
    --disable-pop3 \
    --disable-imap \
    --disable-smtp \
    --disable-gopher \
    --disable-mqtt \
    --disable-manual \
    --disable-libcurl-option \
    --without-libidn2 \
    --without-librtmp \
    --without-libssh \
    --without-libssh2 \
    --without-libpsl \
    --without-nghttp3 \
    --without-ngtcp2
```

**What this does:**
- `--enable-static`: Build static library (.a)
- `--disable-shared`: Don't build dynamic library (.so)
- `--with-openssl`: Enable HTTPS (required)
- `--with-zlib/brotli/nghttp2/zstd`: Enable compression and HTTP/2
- `--disable-ldap*`: ✅ **Removes entire LDAP dependency chain**
- `--disable-rtsp/dict/telnet/etc.`: Remove unused protocols
- `--without-libssh*`: ✅ **Removes SSH/SFTP support**
- `--without-librtmp`: ✅ **Removes RTMP support**
- `--without-libpsl`: ✅ **Removes cookie domain checking**
- `--without-libidn2`: ✅ **Removes internationalized domain names**

### Step 4: Build and Install

```bash
make -j$(nproc)
sudo make install
```

This installs to `/usr/local/lib/libcurl.a` (doesn't overwrite system libcurl).

### Step 5: Verify the Build

```bash
# Check what protocols are supported
/usr/local/bin/curl-config --protocols
# Should show: DICT FILE FTP FTPS GOPHER GOPHERS HTTP HTTPS
# Should NOT show: LDAP LDAPS RTMP SCP SFTP

# Check what features are enabled
/usr/local/bin/curl-config --features
# Should show: AsynchDNS brotli HSTS HTTP2 HTTPS-proxy IDN IPv6 Largefile libz NTLM SSL TLS-SRP UnixSockets zstd
# Should NOT show: GSS-API LDAP(S) RTMP SPNEGO SSH

# Check static library size
ls -lh /usr/local/lib/libcurl.a
# Should be ~2-3 MB (vs system's 4-5 MB)

# Check dependencies
nm -u /usr/local/lib/libcurl.a | grep -v "^$" | head -20
# Should see: OpenSSL symbols (SSL_*, BIO_*, etc.)
# Should see: zlib symbols (inflate*, deflate*)
# Should NOT see: ldap_*, ssh_*, sasl_*, pg_*
```

### Step 6: Update CMakeLists.txt to Use Custom libcurl

Edit `d:\Dev\Friendly-LwM2M-Client\CMakeLists.txt`:

```cmake
# Around line 100, BEFORE find_package(CURL):

# Use custom minimal libcurl instead of system libcurl
set(CURL_LIBRARY /usr/local/lib/libcurl.a)
set(CURL_INCLUDE_DIR /usr/local/include)
set(CURL_FOUND TRUE)

# Now find_package will use our custom paths
find_package(CURL REQUIRED)
```

**Or use pkg-config:**

```cmake
# Use custom curl-config
set(ENV{PKG_CONFIG_PATH} "/usr/local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)
```

### Step 7: Remove Unnecessary Dependencies

Edit `d:\Dev\Friendly-LwM2M-Client\CMakeLists.txt` and **comment out** the libraries you no longer need:

```cmake
# Line ~135 - Comment out SSH (not needed with minimal curl)
# find_library(SSH_LIB ssh)

# Line ~137 - Comment out RTMP (not needed)
# find_library(RTMP_LIB rtmp)

# Line ~141 - Comment out IDN2 (not needed)
# find_library(IDN2_LIB idn2)

# Line ~143 - Comment out LDAP (not needed)
# find_library(LDAP_LIB ldap)
# find_library(LBER_LIB lber)

# Line ~161 - Comment out libunistring (was for IDN2)
# find_library(UNISTRING_LIB unistring)

# Lines ~165-168 - Comment out database backends (were for SASL - all removed)

# Lines ~147-150 - Comment out LDAP dependencies
# find_library(GNUTLS_LIB gnutls)
# find_library(SASL2_LIB sasl2)

# Lines ~152-157 - Comment out GnuTLS dependencies
# find_library(NETTLE_LIB nettle)
# find_library(HOGWEED_LIB hogweed)
# find_library(GMP_LIB gmp)
# find_library(TASN1_LIB tasn1)

# Line ~139 - Comment out PSL (not needed)
# find_library(PSL_LIB psl)
```

**Keep these (required for minimal curl):**

```cmake
# OpenSSL (for HTTPS)
find_package(OpenSSL REQUIRED)

# Compression libraries
find_package(ZLIB REQUIRED)
find_library(BROTLI_COMMON_LIB brotlicommon)
find_library(BROTLI_DEC_LIB_CURL brotlidec)
find_library(BROTLI_ENC_LIB_CURL brotlienc)
find_library(NGHTTP2_LIB nghttp2)
find_library(ZSTD_LIB_CURL zstd)

# GSSAPI/Kerberos (may be needed by OpenSSL on some systems)
# Keep the GSSAPI detection block (lines ~186-210)
```

### Step 8: Update Linking

The linking sections in CMakeLists.txt (root) and examples/CMakeLists.txt will automatically skip the commented-out libraries since the `if(LIB_NAME)` checks will be false.

### Step 9: Clean Build

```bash
cd /path/to/Friendly-LwM2M-Client/build
rm -rf *
cmake ..
make -j$(nproc) 2>&1 | tee ../Issues/minimal_build.log
```

## Expected Results

### Before (System libcurl):

```
Dependencies: 28 libraries
- libcurl
- libssh, libpsl, librtmp, libidn2, libunistring
- libldap, liblber
- libgnutls, libsasl2
- libnettle, libhogweed, libgmp, libtasn1
- database backends (all removed)
- libbrotli (3), libnghttp2, libzstd, zlib
- OpenSSL (2), GSSAPI/Kerberos (4)

Build time: 2-3 minutes
Binary size: ~15-20 MB
```

### After (Minimal libcurl):

```
Dependencies: 8 libraries
- libcurl (custom minimal)
- libbrotli (3: common, dec, enc)
- libnghttp2
- libzstd
- zlib
- OpenSSL (2: SSL, Crypto)

Optional: GSSAPI/Kerberos (4) - only if needed

Build time: 30-60 seconds
Binary size: ~5-8 MB
```

**Reduction:**
- 71% fewer dependencies (28 → 8)
- 66% smaller binary
- 75% faster builds

## Minimal libcurl Feature Set

Your custom libcurl will support:

✅ **Supported Protocols:**
- HTTP/1.1
- HTTP/2 (via nghttp2)
- HTTPS (TLS 1.2, TLS 1.3)
- FTP/FTPS (if needed for firmware servers)
- FILE (local files)

✅ **Supported Features:**
- TLS/SSL encryption
- Certificate verification
- gzip, brotli, zstd compression
- HTTP/2 multiplexing
- Large file support
- IPv4/IPv6
- DNS resolution
- Cookies
- HTTP authentication (Basic, Digest, NTLM)

❌ **Removed (not needed for LwM2M):**
- LDAP/LDAPS directory access
- SSH/SCP/SFTP file transfer
- RTMP streaming
- Internationalized domain names (IDN)
- Public Suffix List (PSL) cookie domain checking
- RTSP streaming control
- DICT dictionary protocol
- Telnet, TFTP, Gopher
- Email protocols (POP3, IMAP, SMTP)
- MQTT

## Alternative: Even More Minimal (HTTP-only)

If you want the absolute minimum (just HTTP/HTTPS, no HTTP/2 or compression):

```bash
./configure \
    --prefix=/usr/local \
    --enable-static \
    --disable-shared \
    --with-openssl \
    --without-zlib \
    --without-brotli \
    --without-nghttp2 \
    --without-zstd \
    --disable-ldap \
    --disable-ldaps \
    --disable-rtsp \
    --disable-dict \
    --disable-telnet \
    --disable-tftp \
    --disable-pop3 \
    --disable-imap \
    --disable-smtp \
    --disable-gopher \
    --disable-mqtt \
    --disable-ftp \
    --disable-file \
    --disable-manual \
    --without-libidn2 \
    --without-librtmp \
    --without-libssh \
    --without-libssh2 \
    --without-libpsl
```

**Result:** Only 3 dependencies: libcurl, OpenSSL::SSL, OpenSSL::Crypto

**Tradeoff:** No compression (larger firmware downloads), no HTTP/2 (slower)

## Testing Your Minimal Build

### Test 1: Basic HTTPS Request

```bash
/usr/local/bin/curl -v https://www.google.com
```

Should work perfectly.

### Test 2: HTTP/2 Support

```bash
/usr/local/bin/curl -v --http2 https://www.google.com
```

Should show `HTTP/2 200` in response.

### Test 3: Compression Support

```bash
/usr/local/bin/curl -v -H "Accept-Encoding: gzip, br, zstd" https://www.google.com
```

Should show `Content-Encoding: br` or `gzip` in response.

### Test 4: Verify LDAP is Disabled

```bash
/usr/local/bin/curl ldap://ldap.example.com
```

Should show: `Protocol "ldap" not supported or disabled in libcurl`

### Test 5: Check Dependencies

```bash
ldd /usr/local/bin/curl
```

Should show:
- libssl.so
- libcrypto.so
- libbrotlidec.so (if enabled)
- libnghttp2.so (if enabled)
- libzstd.so (if enabled)
- libz.so (if enabled)
- libc.so, libpthread.so (system)

Should NOT show:
- libssh.so
- libldap.so
- libsasl2.so

## Integration with LwM2M Client

After building minimal libcurl, your LwM2M client will:

1. **Build faster** - Fewer dependencies to link
2. **Deploy smaller** - Smaller static binary
3. **Start faster** - Less code to initialize
4. **Be more secure** - Smaller attack surface (no LDAP, SSH, etc.)
5. **Be easier to maintain** - Fewer dependencies to update

## Troubleshooting

### Issue: CMake still finds system libcurl

**Solution:** Be explicit about library path:

```cmake
set(CURL_LIBRARY /usr/local/lib/libcurl.a CACHE FILEPATH "Custom libcurl" FORCE)
```

### Issue: Missing HTTP/2 support at runtime

**Solution:** Verify nghttp2 was linked:

```bash
nm /usr/local/lib/libcurl.a | grep nghttp2
```

If empty, rebuild curl with `--with-nghttp2`.

### Issue: Certificate verification fails

**Solution:** Install ca-certificates:

```bash
sudo apt-get install ca-certificates
```

Or point curl to certificate bundle:

```bash
export CURL_CA_BUNDLE=/etc/ssl/certs/ca-certificates.crt
```

### Issue: Still getting LDAP symbols

**Solution:** Make sure you're using the custom libcurl:

```bash
# Check what cmake found
grep CURL_LIBRARY build/CMakeCache.txt

# Should show: /usr/local/lib/libcurl.a
# If shows: /usr/lib/x86_64-linux-gnu/libcurl.a (wrong!)
```

Clear cache and reconfigure:

```bash
rm -rf build/*
cmake -DCURL_LIBRARY=/usr/local/lib/libcurl.a ..
```

## Automation Script

Create `scripts/build_minimal_curl.sh`:

```bash
#!/bin/bash
set -e

CURL_VERSION="8.5.0"
INSTALL_PREFIX="/usr/local"

echo "Building minimal libcurl ${CURL_VERSION}..."

# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential autoconf automake libtool pkg-config \
    libssl-dev zlib1g-dev libbrotli-dev libnghttp2-dev libzstd-dev

# Download and extract
cd /tmp
wget "https://curl.se/download/curl-${CURL_VERSION}.tar.gz"
tar xzf "curl-${CURL_VERSION}.tar.gz"
cd "curl-${CURL_VERSION}"

# Configure minimal build
./configure \
    --prefix="${INSTALL_PREFIX}" \
    --enable-static \
    --disable-shared \
    --with-openssl \
    --with-zlib \
    --with-brotli \
    --with-nghttp2 \
    --with-zstd \
    --disable-ldap \
    --disable-ldaps \
    --disable-rtsp \
    --disable-dict \
    --disable-telnet \
    --disable-tftp \
    --disable-pop3 \
    --disable-imap \
    --disable-smtp \
    --disable-gopher \
    --disable-mqtt \
    --disable-manual \
    --without-libidn2 \
    --without-librtmp \
    --without-libssh \
    --without-libssh2 \
    --without-libpsl

# Build and install
make -j$(nproc)
sudo make install

# Verify
echo ""
echo "Minimal libcurl installed to ${INSTALL_PREFIX}"
echo "Protocols: $("${INSTALL_PREFIX}/bin/curl-config" --protocols)"
echo "Features: $("${INSTALL_PREFIX}/bin/curl-config" --features)"
echo ""
echo "Static library: ${INSTALL_PREFIX}/lib/libcurl.a"
ls -lh "${INSTALL_PREFIX}/lib/libcurl.a"

# Cleanup
cd /
rm -rf "/tmp/curl-${CURL_VERSION}" "/tmp/curl-${CURL_VERSION}.tar.gz"

echo ""
echo "Done! Add to CMakeLists.txt:"
echo "  set(CURL_LIBRARY ${INSTALL_PREFIX}/lib/libcurl.a)"
```

Make it executable:

```bash
chmod +x scripts/build_minimal_curl.sh
./scripts/build_minimal_curl.sh
```

## Summary

By building a custom minimal libcurl:

| Metric | System libcurl | Minimal libcurl | Savings |
|--------|---------------|-----------------|---------|
| **Dependencies** | 28 libraries | 8 libraries | 71% fewer |
| **Build time** | 2-3 minutes | 30-60 seconds | 75% faster |
| **Binary size** | 15-20 MB | 5-8 MB | 66% smaller |
| **Protocols** | 15+ protocols | 4 protocols | Only what you need |
| **Security** | Large attack surface | Minimal attack surface | More secure |

**Recommendation:** Use minimal libcurl for production IoT deployments. The reduced complexity, smaller binary, and faster builds are worth the one-time setup effort.
