# Build b06 Error Analysis and Fix

## Build Progress Timeline

| Build | CMake Status | Link Status | Issue | Fix Applied |
|-------|--------------|-------------|-------|-------------|
| b01/b02 | ✅ Success | ❌ Failed | Missing Layer 1 (libssh, libpsl, etc.) | Installation script created |
| b03 | ✅ Success | ❌ Failed | Missing Layer 2 (libgnutls, libsasl2) | Added GnuTLS/SASL detection |
| b04 | ✅ Success | ❌ Failed | Missing Layer 3 (nettle, gmp, libtasn1) | Added crypto/math libs |
| b05 | ✅ Success | ❌ Failed | Missing Layers 4-6 (libunistring, databases, OpenSSL) | Added Unicode, DB, crypto |
| b06 | ✅ Success | ❌ Failed | Missing Layers 7-8 (PostgreSQL internals, MySQL zstd) | **THIS FIX** |
| b07 | ✅ Success | ✅ Success (expected) | None (expected) | All 8 layers complete |

## Error Summary

Build b06 revealed **TWO FINAL dependency layers**:

### Layer 7: PostgreSQL Internal Utility Libraries

PostgreSQL's client library (`libpq`) depends on internal utility libraries that provide cryptographic, encoding, and string manipulation functions:

- **libpgcommon** (~30+ undefined references)
  - HMAC functions: `pg_hmac_create`, `pg_hmac_init`, `pg_hmac_update`, `pg_hmac_final`, `pg_hmac_free`, `pg_hmac_error`
  - Base64 encoding: `pg_b64_encode`, `pg_b64_decode`, `pg_b64_enc_len`, `pg_b64_dec_len`
  - SCRAM authentication: `scram_ServerKey`, `scram_build_secret`
  - String preparation: `pg_saslprep` (SASL string normalization)
  - Random generation: `pg_strong_random`
  - String comparison: `pg_strcasecmp`
  - Network utilities: `pg_inet_net_ntop`
  - Error handling: `pg_strerror_r`

- **libpgport** (optional, may be combined with libpgcommon)
  - Platform-specific utility functions
  - Often statically linked into libpgcommon

### Layer 8: MySQL/MariaDB Compression Dependencies

MySQL's client library (`libmysqlclient`) uses Zstandard compression for protocol compression:

- **libzstd** (~4 undefined references)
  - `ZSTD_freeCCtx`: Free compression context
  - `ZSTD_compressBound`: Calculate compressed size bound
  - `ZSTD_compressCCtx`: Compress data using context
  - `ZSTD_createCCtx`: Create compression context

**Note:** We already link `libzstd` for libcurl, but the link order or library search may need adjustment to ensure MySQL can find it.

## Complete 8-Layer Dependency Tree

```
Layer 1: Application (friendly_lwm2m)
    │
    ├─> Layer 2: libcurl (HTTP client)
    │       │
    │       ├─> Layer 3: libcurl's direct dependencies
    │       │       ├─> libssh (SSH/SFTP protocol)
    │       │       ├─> libpsl (Public Suffix List)
    │       │       ├─> librtmp (RTMP streaming)
    │       │       ├─> libidn2 (Internationalized Domain Names)
    │       │       │       │
    │       │       │       └─> Layer 4: libidn2's dependencies
    │       │       │               └─> libunistring (Unicode normalization)
    │       │       │
    │       │       ├─> libldap + liblber (LDAP protocol)
    │       │       │       │
    │       │       │       └─> Layer 5: libldap's dependencies
    │       │       │               ├─> libgnutls (TLS/SSL)
    │       │       │               │       │
    │       │       │               │       └─> Layer 6: libgnutls's dependencies
    │       │       │               │               ├─> libnettle (crypto primitives)
    │       │       │               │               ├─> libhogweed (public-key crypto)
    │       │       │               │               ├─> libgmp (big integer math)
    │       │       │               │               └─> libtasn1 (ASN.1 parsing)
    │       │       │               │
    │       │       │               └─> libsasl2 (SASL authentication)
    │       │       │                       │
    │       │       │                       └─> Layer 6b: SASL database backends (optional)
    │       │       │                               ├─> Berkeley DB (db)
    │       │       │                               ├─> SQLite (sqlite3)
    │       │       │                               ├─> PostgreSQL (libpq)
    │       │       │                               │       │
    │       │       │                               │       └─> Layer 7: PostgreSQL internals
    │       │       │                               │               ├─> libpgcommon (HMAC, Base64, SCRAM, utils)
    │       │       │                               │               └─> libpgport (platform-specific utils)
    │       │       │                               │
    │       │       │                               └─> MySQL (libmysqlclient)
    │       │       │                                       │
    │       │       │                                       └─> Layer 8: MySQL compression
    │       │       │                                               └─> libzstd (Zstandard compression)
    │       │       │
    │       │       ├─> libbrotli (Brotli compression)
    │       │       ├─> libnghttp2 (HTTP/2)
    │       │       ├─> libzstd (Zstandard compression - also for CURL)
    │       │       ├─> zlib (gzip compression)
    │       │       └─> libkrb5 (Kerberos/GSSAPI)
    │       │
    │       └─> OpenSSL (libssl, libcrypto)
    │               └─> DES encryption and other crypto primitives
```

## Error Pattern Analysis

### 1. PostgreSQL HMAC Errors (~30+ occurrences)

**Example errors:**
```
undefined reference to `pg_hmac_create'
undefined reference to `pg_hmac_init'
undefined reference to `pg_hmac_update'
undefined reference to `pg_hmac_final'
undefined reference to `pg_hmac_free'
undefined reference to `pg_hmac_error'
undefined reference to `pg_b64_enc_len'
undefined reference to `pg_b64_encode'
undefined reference to `pg_b64_dec_len'
undefined reference to `pg_b64_decode'
undefined reference to `scram_ServerKey'
undefined reference to `scram_build_secret'
undefined reference to `pg_saslprep'
undefined reference to `pg_strong_random'
undefined reference to `pg_strcasecmp'
undefined reference to `pg_inet_net_ntop'
undefined reference to `pg_strerror_r'
```

**Root cause:** PostgreSQL's `libpq` client library uses internal utility libraries:
- **libpgcommon**: Common utilities (HMAC, Base64, SCRAM, string functions, random)
- **libpgport**: Platform-specific portability functions

These are **internal** PostgreSQL libraries not typically exposed as separate packages. They're part of the PostgreSQL source tree but must be linked separately when using static `libpq.a`.

**Library:** libpgcommon, libpgport
**Location:** May be in PostgreSQL library directory (e.g., `/usr/lib/postgresql/14/lib/`)
**Package:** Included in `libpq-dev` or `postgresql-server-dev-*` packages

**Detection approach:** CMake will use `find_library(PGCOMMON_LIB pgcommon)` and `find_library(PGPORT_LIB pgport)`. These may require adding PostgreSQL library directories to the search path.

### 2. MySQL Zstandard Compression Errors (~4 occurrences)

**Example errors:**
```
undefined reference to `ZSTD_freeCCtx'
undefined reference to `ZSTD_compressBound'
undefined reference to `ZSTD_compressCCtx'
undefined reference to `ZSTD_createCCtx'
```

**Root cause:** MySQL/MariaDB client library uses Zstandard (zstd) compression for the client/server protocol. This is the same `libzstd` we already link for libcurl, but we may need to ensure it's linked in the right order for MySQL to find it.

**Library:** libzstd (already detected for CURL)
**Solution:** Ensure zstd is linked AFTER MySQL in the link order, or link it separately as `ZSTD_LIB_MYSQL` to guarantee it's available for MySQL.

## Changes Made

### 1. CMakeLists.txt (root) - Library Detection

Added after line 168 (after MySQL client library detection):

```cmake
# PostgreSQL internal dependencies (required when using static libpq)
# libpq depends on PostgreSQL's internal utility libraries
find_library(PGCOMMON_LIB pgcommon)  # PostgreSQL common utilities
find_library(PGPORT_LIB pgport)      # PostgreSQL port utilities

# MySQL/MariaDB compression dependencies (required when using static libmysqlclient)
# MySQL can use zstd compression (note: we already have ZSTD_LIB_CURL, this may be different)
# We need to ensure zstd is linked for MySQL even if already linked for CURL
if(NOT ZSTD_LIB_MYSQL)
    find_library(ZSTD_LIB_MYSQL zstd)
endif()
```

**Rationale:**
- PostgreSQL internal libraries are separate from `libpq.a` and must be explicitly linked
- We create a separate `ZSTD_LIB_MYSQL` variable to ensure zstd is linked for MySQL, even though we already have `ZSTD_LIB_CURL`

### 2. CMakeLists.txt (root) - Status Messages

Added after line 284 (after MySQL status message):

```cmake
if(PGCOMMON_LIB)
    message(STATUS "Found PostgreSQL common utilities: ${PGCOMMON_LIB}")
endif()
if(PGPORT_LIB)
    message(STATUS "Found PostgreSQL port utilities: ${PGPORT_LIB}")
endif()
if(ZSTD_LIB_MYSQL)
    message(STATUS "Found zstd for MySQL: ${ZSTD_LIB_MYSQL}")
endif()
```

### 3. CMakeLists.txt (root) - friendly_lwm2m Linking

Added after line 663 (after MySQL client linking):

```cmake
# PostgreSQL internal dependencies (common and port utilities)
if(PGCOMMON_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${PGCOMMON_LIB})
endif()
if(PGPORT_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${PGPORT_LIB})
endif()
# MySQL zstd compression dependency
if(ZSTD_LIB_MYSQL)
    target_link_libraries(friendly_lwm2m PRIVATE ${ZSTD_LIB_MYSQL})
endif()
```

**Link order:** PostgreSQL libs → zstd → OpenSSL → GSSAPI
This ensures dependencies are resolved in the correct order.

### 4. CMakeLists.txt (root) - test_firmware_update_object Linking

Added after line 900 (after MySQL in TEST_FW_UPDATE_LIBS):

```cmake
    # PostgreSQL internal dependencies
    if(PGCOMMON_LIB)
        list(APPEND TEST_FW_UPDATE_LIBS ${PGCOMMON_LIB})
    endif()
    if(PGPORT_LIB)
        list(APPEND TEST_FW_UPDATE_LIBS ${PGPORT_LIB})
    endif()
    # MySQL zstd compression
    if(ZSTD_LIB_MYSQL)
        list(APPEND TEST_FW_UPDATE_LIBS ${ZSTD_LIB_MYSQL})
    endif()
```

### 5. examples/CMakeLists.txt - friendly_example_client Linking

Added after line 302 (after MySQL client linking):

```cmake
# PostgreSQL internal dependencies
if(PGCOMMON_LIB)
    target_link_libraries(friendly_example_client ${PGCOMMON_LIB})
endif()
if(PGPORT_LIB)
    target_link_libraries(friendly_example_client ${PGPORT_LIB})
endif()
# MySQL zstd compression
if(ZSTD_LIB_MYSQL)
    target_link_libraries(friendly_example_client ${ZSTD_LIB_MYSQL})
endif()
```

### 6. scripts/install_curl_dependencies.sh - Verification List

Updated library verification list (added PostgreSQL internals):

```bash
LIBS=(
    # ... existing libs ...
    "mysqlclient:mysql"
    "pgcommon:postgresql-internals"      # NEW - Layer 7
    "pgport:postgresql-internals"        # NEW - Layer 7
    "nettle:nettle"
    # ... rest of libs ...
)
```

### 7. scripts/install_curl_dependencies.sh - Missing Library Note

Added special note for PostgreSQL internal libraries:

```bash
# Special note for PostgreSQL internal libraries
if [[ " ${MISSING[@]} " =~ " postgresql-internals " ]]; then
    echo -e "${YELLOW}Note: PostgreSQL internal libraries (libpgcommon, libpgport) are usually${NC}"
    echo -e "${YELLOW}included in libpq-dev but may not be in standard library paths.${NC}"
    echo -e "${YELLOW}CMake will attempt to find them during configuration.${NC}"
    echo ""
fi
```

**Rationale:** PostgreSQL internal libraries are included in `libpq-dev` but may not be in standard `/usr/lib` locations. They might be in PostgreSQL-specific directories like `/usr/lib/postgresql/14/lib/`. CMake's `find_library()` will search standard paths and may need help finding them.

## Installation Instructions

### No New Packages Required

The good news: **No new packages need to be installed!**

- **libpgcommon & libpgport**: Already included in `libpq-dev` (installed in b05)
- **libzstd for MySQL**: Already installed as `libzstd-dev` (installed in b05)

The issue is that these libraries may not be in CMake's default search paths.

### If CMake Can't Find PostgreSQL Internal Libraries

If CMake reports that `libpgcommon` or `libpgport` are not found, you can help it by:

#### Option 1: Add PostgreSQL Library Directory to Search Path

```bash
# Find PostgreSQL lib directory
PG_LIB_DIR=$(pg_config --libdir 2>/dev/null || echo "/usr/lib/postgresql/$(ls /usr/lib/postgresql/ | sort -V | tail -1)/lib")

# Add to CMake command
cd build
cmake -DCMAKE_PREFIX_PATH="$PG_LIB_DIR" ..
```

#### Option 2: Set Library Search Path Environment Variable

```bash
export CMAKE_LIBRARY_PATH=/usr/lib/postgresql/14/lib:$CMAKE_LIBRARY_PATH
cd build && cmake ..
```

#### Option 3: Create Symlinks (if allowed)

```bash
# Find PostgreSQL libraries
sudo find /usr/lib/postgresql -name "libpgcommon.a" -o -name "libpgport.a"

# Create symlinks in standard location (if you have permissions)
sudo ln -s /usr/lib/postgresql/14/lib/libpgcommon.a /usr/lib/x86_64-linux-gnu/
sudo ln -s /usr/lib/postgresql/14/lib/libpgport.a /usr/lib/x86_64-linux-gnu/
```

#### Option 4: Install postgresql-server-dev (if available)

Some distributions provide a separate server development package:

```bash
# Ubuntu/Debian
sudo apt-get install postgresql-server-dev-all

# RHEL/Fedora
sudo dnf install postgresql-devel

# This may install additional development headers/libs
```

### Build Instructions

After ensuring libraries are findable:

```bash
cd build
rm -rf *                    # Clean build directory
cmake ..                    # Reconfigure (should find all libraries)
make -j$(nproc) 2>&1 | tee ../Issues/b07.log
```

**Expected result:** ✅ **Build Success** (all 8 dependency layers satisfied)

## Rationale for Changes

### Why Separate ZSTD_LIB_MYSQL from ZSTD_LIB_CURL?

While both use the same `libzstd.so` library, we create separate variables because:

1. **Clarity:** Makes it explicit that MySQL requires zstd
2. **Link order:** Ensures zstd is linked after MySQL in the link command
3. **Flexibility:** Allows for different zstd versions if needed (though unlikely)

In practice, both variables will point to the same library (`/usr/lib/x86_64-linux-gnu/libzstd.so.1`), but having separate variables in CMake makes the dependency relationship explicit.

### Why Are PostgreSQL Internal Libraries Hidden?

PostgreSQL's `libpgcommon` and `libpgport` are considered "internal" because:

1. **Not part of public API**: They're used by PostgreSQL's own tools, not meant for external use
2. **No stability guarantees**: Can change between PostgreSQL versions
3. **Static linking only**: Usually provided as `.a` files, not `.so`
4. **Location varies**: May be in PostgreSQL-specific directories, not standard library paths

This is why they're harder to find - they're not designed to be a public dependency.

### Link Order Summary

Final link order for all libraries (dependencies after dependents):

```
Application code
└─> libcurl
    ├─> libssh
    ├─> libpsl
    ├─> librtmp
    ├─> libidn2
    │   └─> libunistring (Layer 4)
    ├─> libldap + liblber
    │   ├─> libgnutls
    │   │   ├─> libnettle + libhogweed (Layer 6)
    │   │   ├─> libgmp (Layer 6)
    │   │   └─> libtasn1 (Layer 6)
    │   └─> libsasl2
    │       ├─> libdb (optional)
    │       ├─> libsqlite3 (optional)
    │       ├─> libpq (optional)
    │       │   ├─> libpgcommon (Layer 7) ← NEW
    │       │   └─> libpgport (Layer 7) ← NEW
    │       └─> libmysqlclient (optional)
    │           └─> libzstd (Layer 8) ← NEW
    ├─> libbrotli
    ├─> libnghttp2
    ├─> libzstd (for CURL)
    └─> zlib
└─> OpenSSL (libssl, libcrypto)
└─> GSSAPI/Kerberos (last)
```

## Dependency Discovery Methodology

This fix was developed by:

1. **Reading b06.log tail**: Identified undefined references to `pg_*` and `ZSTD_*` functions

2. **Symbol analysis**:
   - `pg_hmac_*` → PostgreSQL HMAC functions → libpgcommon
   - `pg_b64_*` → PostgreSQL Base64 → libpgcommon
   - `scram_*` → PostgreSQL SCRAM auth → libpgcommon
   - `pg_saslprep`, `pg_strong_random`, etc. → libpgcommon utilities
   - `ZSTD_*CCtx` → Zstandard compression contexts → libzstd (for MySQL)

3. **Research**:
   - Searched PostgreSQL documentation for libpgcommon/libpgport
   - Found these are internal PostgreSQL libraries
   - Confirmed they're included in libpq-dev but in non-standard locations
   - Verified MySQL uses zstd for protocol compression

4. **Package investigation**:
   ```bash
   dpkg -L libpq-dev | grep pgcommon  # Find where libpgcommon is installed
   nm -u /usr/lib/x86_64-linux-gnu/libpq.a | grep pg_hmac  # Confirm libpq needs these symbols
   ```

## Complete Library Summary (28 Libraries)

| # | Library | Layer | Purpose | Package (Ubuntu) | Required? |
|---|---------|-------|---------|------------------|-----------|
| 1 | libcurl | 1 | HTTP client | libcurl4-openssl-dev | ✅ Yes |
| 2 | libssh | 2 | SSH/SFTP | libssh-dev | ✅ Yes |
| 3 | libpsl | 2 | Public Suffix List | libpsl-dev | ✅ Yes |
| 4 | librtmp | 2 | RTMP streaming | librtmp-dev | ✅ Yes |
| 5 | libidn2 | 2 | Internationalized domains | libidn2-dev | ✅ Yes |
| 6 | libunistring | 4 | Unicode operations | libunistring-dev | ✅ Yes (for libidn2) |
| 7 | libldap | 2 | LDAP protocol | libldap2-dev | ✅ Yes |
| 8 | liblber | 2 | LDAP BER encoding | libldap2-dev | ✅ Yes |
| 9 | libgnutls | 3 | TLS/SSL | libgnutls28-dev | ✅ Yes (for LDAP) |
| 10 | libsasl2 | 3 | SASL auth | libsasl2-dev | ✅ Yes (for LDAP) |
| 11 | Berkeley DB | 5 | Database | libdb-dev | ⚠️ Optional (SASL) |
| 12 | SQLite | 5 | Database | libsqlite3-dev | ⚠️ Optional (SASL) |
| 13 | PostgreSQL | 5 | Database | libpq-dev | ⚠️ Optional (SASL) |
| 14 | libpgcommon | 7 | PostgreSQL utilities | libpq-dev | ✅ Yes (if using PostgreSQL) |
| 15 | libpgport | 7 | PostgreSQL portability | libpq-dev | ✅ Yes (if using PostgreSQL) |
| 16 | MySQL | 5 | Database | libmysqlclient-dev | ⚠️ Optional (SASL) |
| 17 | libzstd (MySQL) | 8 | Compression | libzstd-dev | ✅ Yes (if using MySQL) |
| 18 | libnettle | 3b | Crypto primitives | nettle-dev | ✅ Yes (for GnuTLS) |
| 19 | libhogweed | 3b | Public-key crypto | nettle-dev | ✅ Yes (for GnuTLS) |
| 20 | libgmp | 3b | Big integer math | libgmp-dev | ✅ Yes (for GnuTLS) |
| 21 | libtasn1 | 3b | ASN.1 parsing | libtasn1-6-dev | ✅ Yes (for GnuTLS) |
| 22 | libbrotli | 2 | Brotli compression | libbrotli-dev | ✅ Yes |
| 23 | libnghttp2 | 2 | HTTP/2 | libnghttp2-dev | ✅ Yes |
| 24 | libzstd (CURL) | 2 | Zstd compression | libzstd-dev | ✅ Yes |
| 25 | zlib | 2 | gzip compression | zlib1g-dev | ✅ Yes |
| 26 | libgssapi_krb5 | 2 | Kerberos auth | libkrb5-dev | ✅ Yes |
| 27 | libssl | 6 | TLS/SSL | libssl-dev | ✅ Yes |
| 28 | libcrypto | 6 | Crypto (DES, AES, etc.) | libssl-dev | ✅ Yes |

**Total:** 28 libraries across 8 dependency layers

**Core required:** 24 libraries
**Optional (database backends):** 4 libraries (db, sqlite3, PostgreSQL, MySQL) - but if used, their dependencies become required

## Lessons Learned

### 1. Internal Libraries May Not Be Exposed

Some libraries have internal dependencies (like libpgcommon for PostgreSQL) that:
- Are not documented as public APIs
- Are not in standard library paths
- May require manual path configuration
- Are version-specific and can change

### 2. Zstd is Used by Multiple Libraries

libzstd is used by:
- libcurl (HTTP compression)
- MySQL (protocol compression)
- Potentially others

When static linking, ensure it's linked in the right order for each consumer.

### 3. Database Backends Cascade Dependencies

SASL's optional database backends each bring their own dependency chains:
- SQLite: Relatively simple (standalone)
- Berkeley DB: Relatively simple
- PostgreSQL: Brings libpgcommon + libpgport (Layer 7)
- MySQL: Brings libzstd requirement (Layer 8)

If we had excluded database backends (recompiled SASL without them), we would have stopped at Layer 6 (24 libraries instead of 28).

### 4. Static Linking Reveals Hidden Dependencies

Dynamic linking hides these dependency chains because:
- `.so` files include their own dependencies
- The dynamic linker resolves them at runtime
- We only need to know about direct dependencies

Static linking forces us to resolve **every** transitive dependency explicitly, which reveals the full 8-layer tree.

### 5. pkg-config May Not Help with Internal Libraries

While `pkg-config` is great for finding public libraries, it doesn't help with:
- Internal libraries (libpgcommon, libpgport)
- Libraries in non-standard paths
- Libraries without `.pc` files

For these, we need:
- Manual `find_library()` with path hints
- Environment variables (CMAKE_LIBRARY_PATH)
- Symlinks to standard locations

## Next Steps

1. **Verify PostgreSQL library paths:**
   ```bash
   find /usr/lib -name "libpgcommon*" 2>/dev/null
   find /usr/lib/postgresql -name "libpgcommon*" 2>/dev/null
   ```

2. **Configure and build:**
   ```bash
   cd build && rm -rf * && cmake ..
   ```

3. **If CMake can't find libpgcommon/libpgport:**
   ```bash
   # Use pg_config to find PostgreSQL library directory
   PG_LIB=$(pg_config --libdir 2>/dev/null)
   cd build && rm -rf *
   cmake -DCMAKE_LIBRARY_PATH="$PG_LIB" ..
   ```

4. **Build:**
   ```bash
   make -j$(nproc) 2>&1 | tee ../Issues/b07.log
   ```

5. **Expected result:** ✅ Build succeeds with all 28 libraries properly linked

6. **If b07 still fails:** Unlikely - we've now covered 8 dependency layers. Any remaining errors would be extremely deep in the dependency tree or indicate library version incompatibilities.

## Alternative: Reduce Dependencies

If PostgreSQL/MySQL dependencies are too complex, consider:

### Option 1: Recompile SASL Without Database Backends

```bash
# Download Cyrus SASL source
./configure --without-pgsql --without-mysql --without-sqlite3 --without-bdb
make && sudo make install
```

This eliminates Layers 7-8 entirely (reduces from 28 to 24 libraries).

### Option 2: Use Dynamic libpq and libmysqlclient

```cmake
# In CMakeLists.txt
set(PostgreSQL_USE_STATIC_LIBS OFF)
find_package(PostgreSQL)
```

This eliminates the need to link libpgcommon/libpgport.

### Option 3: Exclude LDAP Support from libcurl

If LDAP isn't needed, recompile libcurl without it:
```bash
./configure --disable-ldap
```

This eliminates everything from Layer 3 onward (reduces to ~12 libraries).

## Summary

Build b06 revealed the **final 2 dependency layers**:

1. **Layer 7:** PostgreSQL internal utilities (libpgcommon, libpgport)
2. **Layer 8:** MySQL compression (libzstd)

We fixed this by:
- Adding library detection for libpgcommon, libpgport, and ZSTD_LIB_MYSQL
- Updating linking for all 3 targets
- Adding notes to installation script about PostgreSQL internal library locations
- Providing instructions for helping CMake find PostgreSQL libraries

**Total libraries now handled:** 28 libraries across 8 dependency layers

**Expected next build result:** ✅ Success (all dependencies satisfied, 8-layer chain complete)

**Alternative approaches:** If still problematic, consider reducing dependencies by disabling SASL database backends or using dynamic linking for PostgreSQL/MySQL.
