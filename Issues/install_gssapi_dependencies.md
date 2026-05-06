# Installing GSSAPI/Kerberos Dependencies

The build errors in `build38.log` were caused by missing GSSAPI/Kerberos libraries. These libraries are required when using static versions of libcurl and libssh that were compiled with GSSAPI support.

## Quick Install (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y \
    libkrb5-dev \
    libgssapi-krb5-2 \
    libkeyutils-dev \
    libcom-err2
```

## Quick Install (RHEL/CentOS/Fedora)

```bash
sudo yum install -y \
    krb5-devel \
    keyutils-libs-devel \
    libcom_err-devel
```

Or for Fedora 22+:
```bash
sudo dnf install -y \
    krb5-devel \
    keyutils-libs-devel \
    libcom_err-devel
```

## Quick Install (Alpine Linux)

```bash
apk add \
    krb5-dev \
    keyutils-dev \
    e2fsprogs-dev
```

## Quick Install (Arch Linux)

```bash
sudo pacman -S krb5 keyutils
```

## Verify Installation

Check that the libraries are available:

```bash
# Check for development files
ls /usr/lib/x86_64-linux-gnu/libgssapi_krb5.* 2>/dev/null
ls /usr/lib/x86_64-linux-gnu/libkrb5.* 2>/dev/null
ls /usr/lib/x86_64-linux-gnu/libk5crypto.* 2>/dev/null
ls /usr/lib/x86_64-linux-gnu/libcom_err.* 2>/dev/null
ls /usr/lib/x86_64-linux-gnu/libkeyutils.* 2>/dev/null

# Or use pkg-config
pkg-config --exists krb5-gssapi && echo "GSSAPI available" || echo "GSSAPI not found"
```

## What These Libraries Provide

| Library | Purpose |
|---------|---------|
| `libgssapi_krb5` | Generic Security Services API for Kerberos 5 |
| `libkrb5` | Kerberos 5 authentication library |
| `libk5crypto` | Kerberos 5 cryptographic functions |
| `libcom_err` | Common error description library (used by Kerberos) |
| `libkeyutils` | Linux kernel keyring access library |

## Why Are These Needed?

When libcurl and libssh are compiled with GSSAPI support (default on most Linux distributions):
- They can authenticate using Kerberos tickets
- They support SSH with GSSAPI key exchange
- They enable Single Sign-On (SSO) in enterprise environments

When using **static linking** (`.a` files), all transitive dependencies must be explicitly provided. The CMake configuration now automatically detects and links these libraries when available.

## Build Without GSSAPI (Alternative)

If you don't need GSSAPI authentication and want to avoid these dependencies, you can:

### Option 1: Use Dynamic Linking
```bash
cmake -DDTLS_STATIC_LINK=OFF ..
```

### Option 2: Build libcurl/libssh Without GSSAPI
Rebuild libcurl and libssh from source with `--without-gssapi`:

```bash
# Example for libcurl
./configure --without-gssapi --enable-static --disable-shared
make
sudo make install
```

### Option 3: Use System Dynamic Libraries
The CMake configuration now gracefully handles missing GSSAPI libraries - they will only be linked if found. The build will succeed as long as your static libcurl/libssh don't require them.

## Checking Your libcurl/libssh Configuration

To see what your installed libcurl was compiled with:

```bash
curl-config --features
# Look for: GSS-API

curl-config --libs
# Shows all linked libraries
```

For libssh:
```bash
# Check symbols in static library
nm /usr/lib/x86_64-linux-gnu/libssh.a | grep -i gssapi
# If output is empty, libssh doesn't use GSSAPI
```

## Troubleshooting

### Error: "Package krb5-gssapi was not found"
Your system uses a different package name. Try:
- Ubuntu/Debian: `libkrb5-dev`
- RHEL/CentOS: `krb5-devel`
- Arch: `krb5`

### Error: Libraries found but linking still fails
Check library paths:
```bash
# Add custom library path
cmake -DCMAKE_PREFIX_PATH=/usr/local ..
```

### Error: Multiple Kerberos installations
You might have Heimdal and MIT Kerberos both installed:
```bash
# Prefer MIT Kerberos (most common)
export PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig:$PKG_CONFIG_PATH
cmake ..
```

## After Installation

Once the libraries are installed, rebuild the project:

```bash
cd /path/to/Friendly-LwM2M-Client
rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

Or use the provided test script:
```bash
./test_build38_fix.sh
```

## See Also

- [build38_fix_summary.md](build38_fix_summary.md) - Details of the CMake changes
- [MIT Kerberos Documentation](https://web.mit.edu/kerberos/)
- [RFC 4121 - Kerberos GSS-API](https://tools.ietf.org/html/rfc4121)
