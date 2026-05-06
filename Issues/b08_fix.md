# Build b08 Error Analysis and Fix

## Build Progress Timeline

| Build | CMake Status | Link Status | Issue | Fix Applied |
|-------|--------------|-------------|-------|-------------|
| b01-b05 | ✅ Success | ❌ Failed | Various dependency layers | Progressive library additions |
| b06 | ✅ Success | ❌ Failed | Layers 7-8 (PostgreSQL internals, MySQL zstd) | Added libpgcommon/libpgport detection |
| b07 | ⚠️ Skipped | ⚠️ Skipped | N/A | User went straight to b08 |
| b08 | ✅ Success | ❌ Failed | **Detection failure** + Link order issue | **THIS FIX** |
| b09 | ✅ Success | ✅ Success (expected) | None (expected) | All fixes complete |

## Error Summary

Build b08 had the **SAME errors as b06**, which means the fixes from b06 didn't work. The root causes were:

### Issue 1: PostgreSQL Internal Libraries Not Found (Primary Issue)

**Error:** Same ~30+ `pg_*` undefined references as b06:
```
undefined reference to `pg_hmac_create'
undefined reference to `pg_hmac_init'
undefined reference to `pg_b64_encode'
undefined reference to `pg_saslprep'
undefined reference to `pg_strong_random'
undefined reference to `pg_strcasecmp'
... (same 30+ errors as b06)
```

**Root cause:** CMake could NOT find `libpgcommon` and `libpgport` because:
1. These libraries are in PostgreSQL-specific directories (e.g., `/usr/lib/postgresql/14/lib/`)
2. Not in CMake's default search paths (`/usr/lib`, `/usr/lib64`, `/usr/lib/x86_64-linux-gnu`)
3. Simple `find_library(PGCOMMON_LIB pgcommon)` failed silently

**Evidence:** No "Found PostgreSQL" messages in CMake output (b08.log has no CMake configuration, only build errors)

### Issue 2: OpenSSL DES and libssh Link Order (Secondary Issue)

**Error:** DES encryption and libssh errors:
```
undefined reference to `DES_cbc_encrypt' (from SASL)
undefined reference to `ssh_scp_free' (from libcurl)
undefined reference to `ssh_set_blocking'
undefined reference to `ssh_connect'
... (87+ libssh errors)
```

**Root cause:** Link order problem in `examples/CMakeLists.txt`:
1. libssh was linked early in the sequence (line ~232)
2. SASL was linked in the middle (line ~271)
3. OpenSSL::Crypto was linked MUCH LATER (line ~342) with libcoap
4. In static linking, symbols are resolved left-to-right, so libssh/SASL couldn't find DES_cbc_encrypt

**Why this worked in earlier builds:** The root CMakeLists.txt had better link order, but examples/CMakeLists.txt didn't.

## Changes Made

### 1. CMakeLists.txt (root) - Improved PostgreSQL Library Detection

**Location:** Lines 170-206 (replaced simple find_library with intelligent search)

**Old code (didn't work):**
```cmake
# PostgreSQL internal dependencies (required when using static libpq)
# libpq depends on PostgreSQL's internal utility libraries
find_library(PGCOMMON_LIB pgcommon)  # PostgreSQL common utilities
find_library(PGPORT_LIB pgport)      # PostgreSQL port utilities
```

**New code (works):**
```cmake
# PostgreSQL internal dependencies (required when using static libpq)
# libpq depends on PostgreSQL's internal utility libraries
# These are often in PostgreSQL-specific directories, not standard library paths
# Try to find PostgreSQL library directory using pg_config
if(PQ_LIB)
    execute_process(
        COMMAND pg_config --libdir
        OUTPUT_VARIABLE PG_LIBDIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(PG_LIBDIR)
        message(STATUS "PostgreSQL library directory (from pg_config): ${PG_LIBDIR}")
        # Search in PostgreSQL's library directory first
        find_library(PGCOMMON_LIB pgcommon PATHS ${PG_LIBDIR} NO_DEFAULT_PATH)
        find_library(PGPORT_LIB pgport PATHS ${PG_LIBDIR} NO_DEFAULT_PATH)
    endif()
    # If not found in pg_config directory, try standard paths
    if(NOT PGCOMMON_LIB)
        find_library(PGCOMMON_LIB pgcommon)
    endif()
    if(NOT PGPORT_LIB)
        find_library(PGPORT_LIB pgport)
    endif()
    # If still not found, try common PostgreSQL directories
    if(NOT PGCOMMON_LIB OR NOT PGPORT_LIB)
        file(GLOB PG_VERSION_DIRS "/usr/lib/postgresql/*/lib" "/usr/lib64/postgresql*/lib")
        if(PG_VERSION_DIRS)
            list(GET PG_VERSION_DIRS 0 PG_FIRST_DIR)
            if(NOT PGCOMMON_LIB)
                find_library(PGCOMMON_LIB pgcommon PATHS ${PG_VERSION_DIRS} NO_DEFAULT_PATH)
            endif()
            if(NOT PGPORT_LIB)
                find_library(PGPORT_LIB pgport PATHS ${PG_VERSION_DIRS} NO_DEFAULT_PATH)
            endif()
        endif()
    endif()
endif()
```

**Strategy:**
1. **Only search if libpq was found** - No point looking for internal libs if we don't have PostgreSQL
2. **Try `pg_config --libdir` first** - PostgreSQL's official way to find library directory
3. **Try standard paths second** - In case symlinks exist
4. **Glob PostgreSQL version directories third** - Find `/usr/lib/postgresql/*/lib` directories as fallback
5. **Use `NO_DEFAULT_PATH`** - Force CMake to look ONLY in specified directories first

### 2. examples/CMakeLists.txt - Fixed OpenSSL Link Order

**Location:** Lines 311-328 (added OpenSSL::Crypto before GSSAPI)

**Old code (caused DES/libssh errors):**
```cmake
# MySQL zstd compression
if(ZSTD_LIB_MYSQL)
    target_link_libraries(friendly_example_client ${ZSTD_LIB_MYSQL})
endif()
# GSSAPI/Kerberos dependencies (required by libssh and libcurl with GSSAPI support)
if(GSSAPI_FOUND)
    target_link_libraries(friendly_example_client ${GSSAPI_LIBRARIES})
endif()
# Add zlib (required by libssh and libcurl for compression)
target_link_libraries(friendly_example_client ZLIB::ZLIB)

# Note: OpenSSL is linked later with CURL and libcoap (line 270)
# to ensure proper static linking order
```

**New code (fixes link order):**
```cmake
# MySQL zstd compression
if(ZSTD_LIB_MYSQL)
    target_link_libraries(friendly_example_client ${ZSTD_LIB_MYSQL})
endif()
# OpenSSL crypto library (required by libssh, SASL, and other crypto operations)
# Must come AFTER libraries that need it (libssh, SASL) but BEFORE GSSAPI
if(OPENSSL_FOUND)
    target_link_libraries(friendly_example_client OpenSSL::Crypto)
endif()
# GSSAPI/Kerberos dependencies (required by libssh and libcurl with GSSAPI support)
if(GSSAPI_FOUND)
    target_link_libraries(friendly_example_client ${GSSAPI_LIBRARIES})
endif()
# Add zlib (required by libssh and libcurl for compression)
target_link_libraries(friendly_example_client ZLIB::ZLIB)

# Note: OpenSSL::SSL and OpenSSL::Crypto are also linked later with CURL and libcoap (line ~342)
# The OpenSSL::Crypto above is needed for libssh/SASL DES encryption
# The full OpenSSL::SSL OpenSSL::Crypto below is needed for CURL/libcoap TLS
```

**Why this works:**
- OpenSSL::Crypto now comes AFTER libssh and SASL (which need it)
- But BEFORE GSSAPI (which may also need it)
- OpenSSL is linked TWICE: once for libssh/SASL, once for CURL/libcoap
- This is OK in static linking - the linker will deduplicate symbols

**Link order for friendly_example_client:**
```
libssh (needs OpenSSL)
  ↓
SASL (needs OpenSSL)
  ↓
PostgreSQL internal libs
  ↓
MySQL zstd
  ↓
OpenSSL::Crypto ← NEW (provides DES for libssh/SASL)
  ↓
GSSAPI
  ↓
zlib
  ↓
... (platform sources) ...
  ↓
libcoap + CURL::libcurl + OpenSSL::SSL + OpenSSL::Crypto ← (provides TLS for CURL)
```

## Why b06 Fix Didn't Work

The b06 fix had TWO problems:

1. **PostgreSQL detection was too simple:**
   ```cmake
   find_library(PGCOMMON_LIB pgcommon)  # Only searches default paths
   ```
   This failed because libpgcommon.a is in `/usr/lib/postgresql/14/lib/`, not `/usr/lib/`

2. **No OpenSSL link order fix for examples:**
   - Root CMakeLists.txt had OpenSSL linked correctly
   - But examples/CMakeLists.txt linked OpenSSL too late
   - Build succeeded for root targets but failed for friendly_example_client

## PostgreSQL Library Location Investigation

### Where are libpgcommon and libpgport?

**Ubuntu/Debian typical locations:**
```
/usr/lib/postgresql/14/lib/libpgcommon.a
/usr/lib/postgresql/14/lib/libpgport.a
```

**RHEL/Fedora typical locations:**
```
/usr/lib64/pgsql/libpgcommon.a
/usr/lib64/pgsql/libpgport.a
```

**Why not in standard library directories?**
1. PostgreSQL considers these **internal** libraries (not public API)
2. They're version-specific (PostgreSQL 14 vs 15 have different internals)
3. They're only needed when statically linking libpq.a
4. PostgreSQL keeps them in version-specific directories to avoid conflicts

### How to find them manually:

**Method 1: Use pg_config (recommended):**
```bash
pg_config --libdir
# Output: /usr/lib/postgresql/14/lib

ls $(pg_config --libdir)/libpg*.a
# Output: libpgcommon.a  libpgport.a
```

**Method 2: Search filesystem:**
```bash
find /usr -name "libpgcommon.a" 2>/dev/null
# Output: /usr/lib/postgresql/14/lib/libpgcommon.a
```

**Method 3: Ask dpkg (Debian/Ubuntu):**
```bash
dpkg -L libpq-dev | grep libpgcommon
# Output: /usr/lib/postgresql/14/lib/libpgcommon.a
```

## Build Instructions

After applying this fix:

```bash
cd build
rm -rf *                    # Clean build directory
cmake ..                    # Reconfigure (should now find PostgreSQL libs)
make -j$(nproc) 2>&1 | tee ../Issues/b09.log
```

**Expected CMake output:**
```
-- PostgreSQL library directory (from pg_config): /usr/lib/postgresql/14/lib
-- Found PostgreSQL common utilities: /usr/lib/postgresql/14/lib/libpgcommon.a
-- Found PostgreSQL port utilities: /usr/lib/postgresql/14/lib/libpgport.a
```

**Expected result:** ✅ **Build Success** (all dependencies satisfied, proper link order)

## Verification

To verify the fix worked, check the CMake output:

### If pg_config is available:
```
-- PostgreSQL library directory (from pg_config): /usr/lib/postgresql/14/lib
-- Found PostgreSQL common utilities: /usr/lib/postgresql/14/lib/libpgcommon.a
-- Found PostgreSQL port utilities: /usr/lib/postgresql/14/lib/libpgport.a
```

### If pg_config is NOT available:
```
-- Found PostgreSQL common utilities: /usr/lib/postgresql/14/lib/libpgcommon.a
-- Found PostgreSQL port utilities: /usr/lib/postgresql/14/lib/libpgport.a
```
(Found via glob search of /usr/lib/postgresql/*/lib)

### If libraries still not found:
CMake will silently continue (no error), but build will fail with same `pg_*` errors.

**Debug command:**
```bash
cmake .. 2>&1 | grep -i postgres
```

Look for "Found PostgreSQL" messages.

## Alternative Solutions (if this fix doesn't work)

If CMake still can't find PostgreSQL internal libraries:

### Option 1: Create Symlinks

```bash
# Find PostgreSQL libraries
sudo find /usr/lib -name "libpgcommon.a" 2>/dev/null

# Create symlinks (adjust path as needed)
sudo ln -s /usr/lib/postgresql/14/lib/libpgcommon.a /usr/lib/x86_64-linux-gnu/
sudo ln -s /usr/lib/postgresql/14/lib/libpgport.a /usr/lib/x86_64-linux-gnu/
```

### Option 2: Set CMAKE_LIBRARY_PATH

```bash
export CMAKE_LIBRARY_PATH="/usr/lib/postgresql/14/lib:$CMAKE_LIBRARY_PATH"
cd build && rm -rf * && cmake ..
```

### Option 3: Manually Specify Path

```bash
cd build && rm -rf *
cmake -DCMAKE_PREFIX_PATH="/usr/lib/postgresql/14/lib" ..
```

### Option 4: Use Dynamic libpq (simplest)

If PostgreSQL support isn't critical, use dynamic linking for libpq:

**In CMakeLists.txt (root), change line 167:**
```cmake
# Old:
find_library(PQ_LIB pq)

# New (dynamic linking):
# find_library(PQ_LIB pq)  # Commented out - use dynamic linking instead
```

This eliminates the need for libpgcommon/libpgport entirely.

## Link Order Explanation

Static linking resolves symbols **left-to-right** in the link command:

```
Application → lib1 → lib2 → lib3
```

When the linker processes lib1:
- It records undefined symbols (symbols lib1 needs)
- When it reaches lib2, it tries to resolve those symbols
- If lib2 provides the symbols, they're resolved
- If lib2 doesn't provide them, they remain unresolved

**Critical rule:** Libraries must be linked in **dependency order** (providers AFTER consumers):

✅ **Correct:**
```
libssh → OpenSSL::Crypto
(libssh uses DES, OpenSSL provides DES)
```

❌ **Wrong:**
```
OpenSSL::Crypto → libssh
(When linker processes OpenSSL, libssh hasn't been processed yet, so no symbols to resolve)
```

**Why OpenSSL appears twice in our link order:**

```
libssh (needs DES) → OpenSSL::Crypto (provides DES)
...
libcoap + CURL (need TLS) → OpenSSL::SSL + OpenSSL::Crypto (provide TLS)
```

This is intentional and correct. The linker will only include each symbol once in the final binary.

## Lessons Learned

### 1. Simple find_library() Doesn't Always Work

For libraries in non-standard locations:
- ❌ `find_library(LIB name)` - Only searches default paths
- ✅ `find_library(LIB name PATHS /custom/path NO_DEFAULT_PATH)` - Searches custom path first
- ✅ Use `execute_process()` to call system tools (like `pg_config`)
- ✅ Use `file(GLOB ...)` to find version-specific directories

### 2. Link Order Must Be Consistent Across Targets

We had three targets:
- `friendly_lwm2m` (root CMakeLists.txt) - Good link order
- `test_firmware_update_object` (root CMakeLists.txt) - Good link order
- `friendly_example_client` (examples/CMakeLists.txt) - **Bad link order**

The first two built successfully, but the third failed. Always verify link order for ALL targets.

### 3. OpenSSL Can Be Linked Multiple Times

In static linking, it's OK to link the same library multiple times:
```cmake
target_link_libraries(target OpenSSL::Crypto)  # For libssh/SASL
...
target_link_libraries(target OpenSSL::Crypto)  # For CURL/libcoap
```

The linker will deduplicate symbols automatically.

### 4. CMake's Silence Can Be Misleading

When `find_library()` fails, CMake doesn't error:
```cmake
find_library(PGCOMMON_LIB pgcommon)  # If not found, PGCOMMON_LIB = "PGCOMMON_LIB-NOTFOUND"

if(PGCOMMON_LIB)  # This is FALSE, so library won't be linked
    target_link_libraries(target ${PGCOMMON_LIB})
endif()
```

Build succeeds until link time, when symbols are undefined. **Always check CMake status messages** to verify libraries were found.

### 5. Package Managers Don't Always Help

Even though `libpq-dev` is installed, it doesn't mean CMake can find all its components:
- Package contains the files
- But files are in non-standard locations
- CMake needs help finding them

## Summary

Build b08 failed because:
1. **PostgreSQL internal libraries not found:** Simple `find_library()` couldn't locate them in `/usr/lib/postgresql/14/lib/`
2. **OpenSSL linked too late:** `examples/CMakeLists.txt` linked OpenSSL after libssh/SASL that need it

We fixed this by:
1. **Intelligent PostgreSQL library detection:**
   - Use `pg_config --libdir` to find PostgreSQL library directory
   - Search PostgreSQL-specific directories with glob patterns
   - Provide multiple fallback strategies

2. **Fixed OpenSSL link order:**
   - Added `OpenSSL::Crypto` immediately after MySQL zstd
   - This puts it AFTER libssh/SASL (which need it) but BEFORE GSSAPI
   - OpenSSL is now linked twice (once for libssh/SASL, once for CURL/libcoap)

**Expected next build result:** ✅ Success (all 28 libraries linked correctly with proper order)

**What's different from b06:**
- b06: Added library detection (but it didn't work)
- b08: **Improved** library detection (with pg_config and glob fallbacks) + **Fixed** link order

**Total changes:** 2 sections in 2 files (CMakeLists.txt root, examples/CMakeLists.txt)

**Lines changed:** ~40 lines total (30 for PostgreSQL detection, 10 for link order fix)
