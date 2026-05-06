# Build b05 Error Analysis and Fix

## Build Progress Timeline

| Build | CMake Status | Link Status | Issue | Fix Applied |
|-------|--------------|-------------|-------|-------------|
| b01/b02 | ✅ Success | ❌ Failed | Missing Layer 1 libs (libssh, libpsl, etc.) | Installation script created |
| b03 | ✅ Success | ❌ Failed | Missing Layer 2 libs (libgnutls, libsasl2) | Added GnuTLS/SASL detection |
| b04 | ✅ Success | ❌ Failed | Missing Layer 3 libs (nettle, gmp, libtasn1) | Added crypto/math libs |
| b05 | ✅ Success | ❌ Failed | Missing Layer 4-6 libs (see below) | **THIS FIX** |
| b06 | ✅ Success | ✅ Success (expected) | None (expected) | All layers complete |

## Error Summary

Build b05 revealed **THREE NEW dependency layers** beyond what we had detected:

### Layer 4: libidn2 Dependencies
- **libunistring** (~56 undefined references)
  - Unicode normalization: `uninorm_nfc`, `uninorm_nfkc`, `u32_normalize`
  - Unicode string operations: `u8_strlen`, `u8_tolower`, `u32_to_u8`
  - Unicode character properties: `uc_is_property_*`, `uc_is_general_category`, `UC_CATEGORY_Zs`

### Layer 5: SASL Database Backend Dependencies (Optional)
These are only needed if SASL was compiled with database backend support:

- **Berkeley DB** (~8 references): `db_create`, `db_strerror`
- **SQLite** (~20 references): `sqlite3_exec`, `sqlite3_open`, `sqlite3_close`, etc.
- **PostgreSQL** (~15 references): `PQexec`, `PQconnectdb`, `PQstatus`, `PQgetvalue`, etc.
- **MySQL** (~15 references): `mysql_real_query`, `mysql_init`, `mysql_real_connect`, etc.
- **More LDAP functions** (~12 references): `ldap_whoami_s`, `ldap_modify_ext_s`, `ldap_first_message`, `ldap_next_message`

### Layer 6: Additional Crypto Dependencies
- **OpenSSL crypto** (~2 references): `DES_cbc_encrypt`
  - DES encryption used by libssh or SASL
- **Nettle constant-time operations** (~6 references): `nettle_cnd_memcpy`, `nettle_sha3_256_shake`
  - Additional Nettle functions beyond basic crypto

### Repeated Errors from Previous Layers
- **libssh** (~87 references): Still appearing, suggesting link order issue

## Complete Dependency Tree (6 Layers)

```
Layer 1: libcurl (application's direct dependency)
    │
    ├─> Layer 2: libcurl's direct dependencies
    │       ├─> libssh (SSH/SFTP protocol)
    │       ├─> libpsl (Public Suffix List)
    │       ├─> librtmp (RTMP streaming)
    │       ├─> libidn2 (Internationalized Domain Names)
    │       │       │
    │       │       └─> Layer 4: libidn2's dependencies
    │       │               └─> libunistring (Unicode normalization)
    │       │
    │       ├─> libldap + liblber (LDAP protocol)
    │       │       │
    │       │       └─> Layer 3: libldap's dependencies
    │       │               ├─> libgnutls (TLS/SSL)
    │       │               │       │
    │       │               │       └─> Layer 3b: libgnutls's dependencies
    │       │               │               ├─> libnettle (crypto primitives)
    │       │               │               │       │
    │       │               │               │       └─> Layer 6: Additional nettle functions
    │       │               │               │               └─> nettle (constant-time ops)
    │       │               │               │
    │       │               │               ├─> libhogweed (public-key crypto)
    │       │               │               ├─> libgmp (big integer math)
    │       │               │               └─> libtasn1 (ASN.1 parsing)
    │       │               │
    │       │               └─> libsasl2 (SASL authentication)
    │       │                       │
    │       │                       └─> Layer 5: SASL database backends (optional)
    │       │                               ├─> Berkeley DB (db)
    │       │                               ├─> SQLite (sqlite3)
    │       │                               ├─> PostgreSQL (libpq)
    │       │                               ├─> MySQL (libmysqlclient)
    │       │                               └─> More LDAP functions
    │       │
    │       ├─> libbrotli (Brotli compression)
    │       ├─> libnghttp2 (HTTP/2)
    │       ├─> libzstd (Zstandard compression)
    │       ├─> zlib (gzip compression)
    │       └─> libkrb5 (Kerberos/GSSAPI)
    │
    └─> Layer 6: Crypto library
            └─> OpenSSL (libssl, libcrypto)
                    └─> DES encryption and other primitives
```

## Error Pattern Analysis

### 1. libunistring Errors (~56 occurrences)

**Example errors:**
```
undefined reference to `u8_strlen'
undefined reference to `uninorm_nfkc'
undefined reference to `u8_tolower'
undefined reference to `uninorm_nfc'
undefined reference to `u32_normalize'
undefined reference to `u32_to_u8'
undefined reference to `uc_is_property_not_a_character'
undefined reference to `uc_is_property_join_control'
undefined reference to `UC_CATEGORY_Zs'
undefined reference to `uc_is_general_category'
```

**Root cause:** libidn2 (Internationalized Domain Names) uses libunistring for:
- Unicode string normalization (NFC, NFKC)
- Unicode character classification
- UTF-8/UTF-32 string conversion
- Unicode property checks

**Library:** libunistring
**Package:** libunistring-dev (Ubuntu), libunistring-devel (RHEL)

### 2. SASL Database Backend Errors (~58 occurrences total)

**Berkeley DB errors (~8):**
```
undefined reference to `db_create'
undefined reference to `db_strerror'
```

**SQLite errors (~20):**
```
undefined reference to `sqlite3_exec'
undefined reference to `sqlite3_open'
undefined reference to `sqlite3_close'
undefined reference to `sqlite3_free'
undefined reference to `sqlite3_errmsg'
```

**PostgreSQL errors (~15):**
```
undefined reference to `PQexec'
undefined reference to `PQconnectdb'
undefined reference to `PQstatus'
undefined reference to `PQgetvalue'
undefined reference to `PQresultStatus'
undefined reference to `PQntuples'
undefined reference to `PQclear'
undefined reference to `PQerrorMessage'
undefined reference to `PQfinish'
```

**MySQL errors (~15):**
```
undefined reference to `mysql_real_query'
undefined reference to `mysql_init'
undefined reference to `mysql_real_connect'
undefined reference to `mysql_errno'
undefined reference to `mysql_store_result'
undefined reference to `mysql_fetch_row'
undefined reference to `mysql_free_result'
undefined reference to `mysql_close'
```

**Root cause:** Cyrus SASL can use multiple database backends for storing user credentials. The Ubuntu libsasl2.a was compiled with support for:
- Berkeley DB (traditional Unix database)
- SQLite (embedded SQL database)
- PostgreSQL (client library for PostgreSQL server)
- MySQL (client library for MySQL/MariaDB server)

These are **optional** - SASL will work without them if not using database authentication.

**Libraries:** db, sqlite3, pq (PostgreSQL), mysqlclient
**Packages:**
- Ubuntu: libdb-dev, libsqlite3-dev, libpq-dev, libmysqlclient-dev
- RHEL: libdb-devel, sqlite-devel, postgresql-devel, mysql-devel

### 3. Additional LDAP Functions (~12 occurrences)

**Example errors:**
```
undefined reference to `ldap_whoami_s'
undefined reference to `ldap_modify_ext_s'
undefined reference to `ldap_first_message'
undefined reference to `ldap_next_message'
```

**Root cause:** SASL's LDAP auxiliary property plugin uses additional LDAP functions not covered by basic libldap linking.

**Library:** Already have libldap, just need proper link order

### 4. OpenSSL DES Errors (~2 occurrences)

**Example errors:**
```
undefined reference to `DES_cbc_encrypt'
```

**Root cause:** libssh or SASL uses DES encryption from OpenSSL's crypto library.

**Library:** OpenSSL crypto (libcrypto)
**Package:** Already installed (OpenSSL::Crypto in CMake)

Note: CMake's `find_package(OpenSSL)` already finds both libssl and libcrypto, we just need to ensure proper linking.

### 5. Additional Nettle Functions (~6 occurrences)

**Example errors:**
```
undefined reference to `nettle_cnd_memcpy'
undefined reference to `nettle_sha3_256_shake'
```

**Root cause:** Hogweed (Nettle's public-key library) uses additional Nettle functions:
- Constant-time memory operations (side-channel attack resistance)
- SHA3/SHAKE hash functions

**Library:** Already have nettle, just additional symbols

### 6. Repeated libssh Errors (~87 occurrences)

**Example errors:**
```
undefined reference to `ssh_scp_free'
undefined reference to `ssh_set_blocking'
undefined reference to `ssh_connect'
```

**Root cause:** Library link order issue - libssh symbols still unresolved suggests it needs to come earlier in link order, OR these errors are cascading from missing dependencies (OpenSSL crypto).

**Fix:** Ensure OpenSSL crypto is linked (provides DES for libssh)

## Changes Made

### 1. CMakeLists.txt (root) - Library Detection

Added after line 157 (after existing GnuTLS dependencies):

```cmake
# libidn2 dependencies (required when using static libidn2)
# libidn2 uses libunistring for Unicode normalization and character properties
find_library(UNISTRING_LIB unistring)

# SASL database backend dependencies (optional - required if SASL uses these backends)
# Cyrus SASL can optionally use Berkeley DB, SQLite, PostgreSQL, or MySQL for user databases
find_library(DB_LIB db)           # Berkeley DB
find_library(SQLITE3_LIB sqlite3) # SQLite
find_library(PQ_LIB pq)           # PostgreSQL client library
find_library(MYSQLCLIENT_LIB mysqlclient)  # MySQL client library

# OpenSSL crypto library (required by libssh, SASL, and other crypto operations)
# Provides DES and other cryptographic primitives
find_package(OpenSSL)
```

**Why this order?**
- libunistring is a dependency of libidn2 (Layer 4)
- Database libraries are optional dependencies of SASL (Layer 5)
- OpenSSL is used by multiple libraries (Layer 6 - lowest level)

### 2. CMakeLists.txt (root) - Status Messages

Added after line 254 (after existing status messages):

```cmake
if(UNISTRING_LIB)
    message(STATUS "Found libunistring for libidn2: ${UNISTRING_LIB}")
endif()
if(OPENSSL_FOUND)
    message(STATUS "Found OpenSSL for crypto: ${OPENSSL_LIBRARIES}")
endif()
if(DB_LIB)
    message(STATUS "Found Berkeley DB for SASL (optional): ${DB_LIB}")
endif()
if(SQLITE3_LIB)
    message(STATUS "Found SQLite for SASL (optional): ${SQLITE3_LIB}")
endif()
if(PQ_LIB)
    message(STATUS "Found PostgreSQL for SASL (optional): ${PQ_LIB}")
endif()
if(MYSQLCLIENT_LIB)
    message(STATUS "Found MySQL for SASL (optional): ${MYSQLCLIENT_LIB}")
endif()
```

### 3. CMakeLists.txt (root) - friendly_lwm2m Linking

Added after line 625 (after existing TASN1_LIB linking):

```cmake
# libidn2 dependencies (Unicode string handling)
if(UNISTRING_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${UNISTRING_LIB})
endif()
# SASL database backend dependencies (optional - only if SASL uses them)
if(DB_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${DB_LIB})
endif()
if(SQLITE3_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${SQLITE3_LIB})
endif()
if(PQ_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${PQ_LIB})
endif()
if(MYSQLCLIENT_LIB)
    target_link_libraries(friendly_lwm2m PRIVATE ${MYSQLCLIENT_LIB})
endif()
# OpenSSL crypto library (DES and other cryptographic primitives)
if(OPENSSL_FOUND)
    target_link_libraries(friendly_lwm2m PRIVATE ${OPENSSL_LIBRARIES})
endif()
```

**Important:** These come BEFORE GSSAPI (which was already at the end).

### 4. CMakeLists.txt (root) - test_firmware_update_object Linking

Added after line 851 (after existing TASN1_LIB in TEST_FW_UPDATE_LIBS):

```cmake
    # libidn2 dependencies (Unicode string handling)
    if(UNISTRING_LIB)
        list(APPEND TEST_FW_UPDATE_LIBS ${UNISTRING_LIB})
    endif()
    # SASL database backend dependencies (optional)
    if(DB_LIB)
        list(APPEND TEST_FW_UPDATE_LIBS ${DB_LIB})
    endif()
    if(SQLITE3_LIB)
        list(APPEND TEST_FW_UPDATE_LIBS ${SQLITE3_LIB})
    endif()
    if(PQ_LIB)
        list(APPEND TEST_FW_UPDATE_LIBS ${PQ_LIB})
    endif()
    if(MYSQLCLIENT_LIB)
        list(APPEND TEST_FW_UPDATE_LIBS ${MYSQLCLIENT_LIB})
    endif()
```

**Note:** OpenSSL is already added later (line ~859: `OpenSSL::SSL OpenSSL::Crypto`), so we don't duplicate it.

### 5. examples/CMakeLists.txt - friendly_example_client Linking

Added after line 285 (after existing TASN1_LIB linking):

```cmake
# libidn2 dependencies (Unicode string handling)
if(UNISTRING_LIB)
    target_link_libraries(friendly_example_client ${UNISTRING_LIB})
endif()
# SASL database backend dependencies (optional)
if(DB_LIB)
    target_link_libraries(friendly_example_client ${DB_LIB})
endif()
if(SQLITE3_LIB)
    target_link_libraries(friendly_example_client ${SQLITE3_LIB})
endif()
if(PQ_LIB)
    target_link_libraries(friendly_example_client ${PQ_LIB})
endif()
if(MYSQLCLIENT_LIB)
    target_link_libraries(friendly_example_client ${MYSQLCLIENT_LIB})
endif()
```

**Note:** OpenSSL is added later when libcoap is linked (line ~314).

### 6. scripts/install_curl_dependencies.sh - Ubuntu/Debian

Updated package list (lines 38-59):

```bash
sudo apt-get install -y \
    libssh-dev \
    libpsl-dev \
    librtmp-dev \
    libidn2-dev \
    libunistring-dev \        # NEW - Layer 4
    libldap2-dev \
    libgnutls28-dev \
    libsasl2-dev \
    libdb-dev \               # NEW - Layer 5
    libsqlite3-dev \          # NEW - Layer 5
    libpq-dev \               # NEW - Layer 5
    libmysqlclient-dev \      # NEW - Layer 5
    nettle-dev \
    libgmp-dev \
    libtasn1-6-dev \
    libbrotli-dev \
    libnghttp2-dev \
    libzstd-dev \
    zlib1g-dev \
    libkrb5-dev \
    libssl-dev                # NEW - Layer 6 (explicit)
```

### 7. scripts/install_curl_dependencies.sh - RHEL/Fedora

Updated package list (lines 64-85):

```bash
sudo dnf install -y \
    libssh-devel \
    libpsl-devel \
    librtmp-devel \
    libidn2-devel \
    libunistring-devel \      # NEW
    openldap-devel \
    gnutls-devel \
    cyrus-sasl-devel \
    libdb-devel \             # NEW
    sqlite-devel \            # NEW
    postgresql-devel \        # NEW
    mysql-devel \             # NEW
    nettle-devel \
    gmp-devel \
    libtasn1-devel \
    brotli-devel \
    libnghttp2-devel \
    libzstd-devel \
    zlib-devel \
    krb5-devel \
    openssl-devel             # NEW
```

### 8. scripts/install_curl_dependencies.sh - Arch Linux

Updated package list (lines 90-110):

```bash
sudo pacman -S --needed \
    libssh \
    libpsl \
    rtmpdump \
    libidn2 \
    libunistring \            # NEW
    openldap \
    gnutls \
    libsasl \
    db \                      # NEW
    sqlite \                  # NEW
    postgresql-libs \         # NEW
    mariadb-libs \            # NEW (MySQL equivalent)
    nettle \
    gmp \
    libtasn1 \
    brotli \
    libnghttp2 \
    zstd \
    zlib \
    krb5 \
    openssl                   # NEW
```

### 9. scripts/install_curl_dependencies.sh - Alpine Linux

Updated package list (lines 110-137):

```bash
sudo apk add \
    libssh-dev \
    libpsl-dev \
    rtmpdump-dev \
    libidn2-dev \
    libunistring-dev \        # NEW
    openldap-dev \
    gnutls-dev \
    cyrus-sasl-dev \
    db-dev \                  # NEW
    sqlite-dev \              # NEW
    postgresql-dev \          # NEW
    mariadb-connector-c-dev \ # NEW
    nettle-dev \
    gmp-dev \
    libtasn1-dev \
    brotli-dev \
    nghttp2-dev \
    zstd-dev \
    zlib-dev \
    krb5-dev \
    openssl-dev               # NEW
```

### 10. scripts/install_curl_dependencies.sh - Verification List

Updated library verification list (lines 165-185):

```bash
LIBS=(
    "ssh:libssh"
    "psl:libpsl"
    "rtmp:librtmp"
    "idn2:libidn2"
    "unistring:libunistring"            # NEW - Layer 4
    "ldap:libldap"
    "lber:liblber"
    "gnutls:gnutls"
    "sasl2:cyrus-sasl"
    "db:berkeley-db"                    # NEW - Layer 5
    "sqlite3:sqlite"                    # NEW - Layer 5
    "pq:postgresql"                     # NEW - Layer 5
    "mysqlclient:mysql"                 # NEW - Layer 5
    "nettle:nettle"
    "hogweed:nettle"
    "gmp:gmp"
    "tasn1:libtasn1"
    "brotlicommon:libbrotli"
    "brotlidec:libbrotli"
    "brotlienc:libbrotli"
    "nghttp2:libnghttp2"
    "zstd:libzstd"
    "z:zlib"
    "gssapi_krb5:krb5"
    "ssl:openssl"                       # NEW - Layer 6
    "crypto:openssl"                    # NEW - Layer 6
)
```

## Rationale for Changes

### Why Add Database Libraries (Optional)?

The SASL database libraries (Berkeley DB, SQLite, PostgreSQL, MySQL) are **optional** dependencies. They're only used if:

1. SASL authentication is enabled
2. User credentials are stored in a database (vs PAM, Kerberos, etc.)

However, because Ubuntu's libsasl2.a was compiled with database backend support, we need to link these libraries **even if we don't use them**. This is a quirk of static linking - all symbols must be resolved even for unused code paths.

**Alternative approaches:**
1. **Link database libraries** (this fix) - Simple, works with pre-built SASL
2. **Recompile SASL without database support** - Complex, loses functionality
3. **Use dynamic linking for SASL** - Simpler, but defeats static linking goal

We chose option 1 for simplicity and maximum compatibility.

### Why Add OpenSSL Explicitly?

We already had `find_package(OpenSSL)` in CMakeLists.txt, but we weren't using it in the link commands. The b05 errors showed we need OpenSSL's crypto library for DES encryption.

CMake's `find_package(OpenSSL)` provides:
- `OpenSSL::SSL` - TLS/SSL library
- `OpenSSL::Crypto` - Cryptographic primitives (DES, AES, SHA, etc.)

For test_firmware_update_object, we were already linking `OpenSSL::Crypto` at the end. For friendly_lwm2m and friendly_example_client, we now explicitly link `${OPENSSL_LIBRARIES}` which includes both SSL and Crypto.

### Why This Link Order?

Libraries must be linked in **reverse dependency order** (dependencies after dependents):

1. **Application code** (friendly_lwm2m)
2. **Direct dependencies** (libcurl, libssh, etc.)
3. **Layer 2 dependencies** (libldap, libidn2, etc.)
4. **Layer 3 dependencies** (libgnutls, libsasl2, libunistring)
5. **Layer 4 dependencies** (nettle, gmp, libtasn1, database libs)
6. **Layer 5 dependencies** (OpenSSL, GSSAPI/Kerberos)
7. **System libraries** (zlib, pthread, etc.) - last

This ensures that when the linker resolves symbols in library A, all libraries that A depends on are already available.

## Installation Instructions

### Ubuntu/Debian

```bash
./scripts/install_curl_dependencies.sh
```

This will install all 26+ libraries (including optional database backends).

### Manual Installation (if script fails)

```bash
sudo apt-get update
sudo apt-get install -y \
    libssh-dev libpsl-dev librtmp-dev libidn2-dev libunistring-dev \
    libldap2-dev libgnutls28-dev libsasl2-dev \
    libdb-dev libsqlite3-dev libpq-dev libmysqlclient-dev \
    nettle-dev libgmp-dev libtasn1-6-dev \
    libbrotli-dev libnghttp2-dev libzstd-dev zlib1g-dev \
    libkrb5-dev libssl-dev
```

### RHEL/Fedora

```bash
sudo dnf install -y \
    libssh-devel libpsl-devel librtmp-devel libidn2-devel libunistring-devel \
    openldap-devel gnutls-devel cyrus-sasl-devel \
    libdb-devel sqlite-devel postgresql-devel mysql-devel \
    nettle-devel gmp-devel libtasn1-devel \
    brotli-devel libnghttp2-devel libzstd-devel zlib-devel \
    krb5-devel openssl-devel
```

## Build Instructions

After installing libraries and applying this fix:

```bash
cd build
rm -rf *                    # Clean build directory
cmake ..                    # Reconfigure with new libraries
make -j$(nproc) 2>&1 | tee ../Issues/b06.log
```

**Expected result:** ✅ **Build Success** (no more undefined references)

## Dependency Discovery Methodology

This fix was developed by:

1. **Reading build log**: Identified unique undefined reference patterns
2. **Symbol analysis**: Grouped errors by prefix:
   - `u8_*`, `uninorm_*`, `uc_is_*` → libunistring
   - `db_*` → Berkeley DB
   - `sqlite3_*` → SQLite
   - `PQ*` → PostgreSQL
   - `mysql_*` → MySQL
   - `DES_*` → OpenSSL crypto
   - `nettle_cnd_*`, `nettle_sha3_*` → Additional Nettle
   - `ldap_*` → More LDAP functions

3. **Counting occurrences**: Determined importance/scope of each missing library
4. **Dependency research**: Identified which library depends on which:
   - libidn2 → libunistring (documented in libidn2 source)
   - libsasl2 → databases (optional plugins, found in build errors)
   - libssh → OpenSSL crypto (documented in libssh)
   - libhogweed → additional Nettle functions (Nettle source)

5. **Package identification**: Found correct -dev packages for each library on each platform

## Tools for Finding Missing Libraries

### 1. Find which library provides a symbol

```bash
# Search installed libraries
nm -D /usr/lib/x86_64-linux-gnu/*.so 2>/dev/null | grep "symbol_name"

# Search packages (Ubuntu)
apt-file search symbol_name

# Find which package provides library
dpkg -S libunistring.so
```

### 2. Find undefined symbols in a library

```bash
nm -u /usr/lib/x86_64-linux-gnu/libidn2.a | grep " U " | head -20
```

This shows symbols that the library needs from other libraries.

### 3. Find dependencies of a shared library

```bash
ldd /usr/lib/x86_64-linux-gnu/libidn2.so
```

This shows runtime dependencies (but static libs are different).

### 4. Search for package by file

```bash
apt-cache search libunistring
dpkg -l | grep unistring
```

## Complete Library Summary (26+ Libraries)

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
| 14 | MySQL | 5 | Database | libmysqlclient-dev | ⚠️ Optional (SASL) |
| 15 | libnettle | 3b | Crypto primitives | nettle-dev | ✅ Yes (for GnuTLS) |
| 16 | libhogweed | 3b | Public-key crypto | nettle-dev | ✅ Yes (for GnuTLS) |
| 17 | libgmp | 3b | Big integer math | libgmp-dev | ✅ Yes (for GnuTLS) |
| 18 | libtasn1 | 3b | ASN.1 parsing | libtasn1-6-dev | ✅ Yes (for GnuTLS) |
| 19 | libbrotli | 2 | Brotli compression | libbrotli-dev | ✅ Yes |
| 20 | libnghttp2 | 2 | HTTP/2 | libnghttp2-dev | ✅ Yes |
| 21 | libzstd | 2 | Zstd compression | libzstd-dev | ✅ Yes |
| 22 | zlib | 2 | gzip compression | zlib1g-dev | ✅ Yes |
| 23 | libgssapi_krb5 | 2 | Kerberos auth | libkrb5-dev | ✅ Yes |
| 24 | libkrb5 | 2 | Kerberos | libkrb5-dev | ✅ Yes |
| 25 | libssl | 6 | TLS/SSL | libssl-dev | ✅ Yes |
| 26 | libcrypto | 6 | Crypto (DES, AES, etc.) | libssl-dev | ✅ Yes |

**Total:** 26 libraries across 6 dependency layers

**Truly required:** 22 libraries (all except 4 database backends)
**Optional but included:** 4 database libraries (db, sqlite3, pq, mysqlclient)

## Lessons Learned

### 1. Static Linking Requires ALL Transitive Dependencies

When static linking, you must provide **every** library that any of your dependencies use, even for code paths you never execute. This is because the linker must resolve all symbols at compile time.

### 2. Dependency Discovery is Iterative

Each build reveals the next layer of missing dependencies:
- b01/b02: Layer 1 (libcurl's direct deps)
- b03: Layer 2 (libldap's deps)
- b04: Layer 3 (libgnutls's deps)
- b05: Layers 4-6 (libidn2, SASL, crypto deps)

This is **normal** for static linking with complex libraries.

### 3. Optional Dependencies Still Need Linking

SASL's database backends are optional at runtime, but when SASL is compiled with database support, the symbols must be resolved at link time. We can't skip them just because we don't use them.

### 4. Link Order Matters

Libraries must be linked in reverse dependency order. If A depends on B, then B must come **after** A in the link command. Getting this wrong causes "undefined reference" errors even when all libraries are present.

### 5. Use Automated Tools

Instead of manually adding libraries one at a time:
- Create installation scripts for each platform
- Use consistent patterns (find_library, status messages, conditional linking)
- Document dependencies in a clear tree structure
- Create comprehensive verification

### 6. Platform Differences

Different Linux distributions have different package names:
- Ubuntu: libmysqlclient-dev
- Alpine: mariadb-connector-c-dev
- Arch: mariadb-libs

An installation script must handle these differences.

## Next Steps

1. **Install libraries:**
   ```bash
   ./scripts/install_curl_dependencies.sh
   ```

2. **Reconfigure and rebuild:**
   ```bash
   cd build && rm -rf * && cmake ..
   make -j$(nproc) 2>&1 | tee ../Issues/b06.log
   ```

3. **Expected result:** ✅ Build succeeds with all 26 libraries properly linked

4. **If b06 still fails:** Analyze b06.log for any remaining missing dependencies (unlikely - we've covered 6 layers)

## Alternative: Dynamic Linking

If static linking continues to be problematic, consider:

```cmake
# In CMakeLists.txt
set(BUILD_SHARED_LIBS ON)  # Use dynamic linking instead
```

**Pros:**
- Much simpler dependency management
- Smaller binary size
- System libraries automatically updated

**Cons:**
- Runtime dependencies required
- Slightly slower startup (dynamic loader)
- Deployment complexity (must bundle .so files)

For embedded systems (OpenWRT), static linking is preferred despite the complexity.

## Summary

Build b05 revealed **3 additional dependency layers** beyond what we had:

1. **Layer 4:** libunistring (Unicode operations for libidn2)
2. **Layer 5:** Database backends for SASL (Berkeley DB, SQLite, PostgreSQL, MySQL)
3. **Layer 6:** OpenSSL crypto (DES encryption for libssh/SASL)

We fixed this by:
- Adding library detection for all 6 new libraries
- Updating linking for all 3 targets (friendly_lwm2m, test_firmware_update_object, friendly_example_client)
- Updating installation script for all 4 platforms
- Adding proper status messages and documentation

**Total libraries now handled:** 26+ libraries across 6 dependency layers

**Expected next build result:** ✅ Success (all dependencies satisfied)
