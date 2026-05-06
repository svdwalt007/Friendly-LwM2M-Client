# DTLS Multi-Provider Implementation Plan

**Date:** April 12, 2026
**Version:** 1.0
**Status:** In Progress

---

## EXECUTIVE SUMMARY

This document outlines the implementation plan for adding multi-DTLS provider support to the Friendly LwM2M Client, allowing runtime selection between OpenSSL, mbedTLS 3.6+, wolfSSL, and TinyDTLS.

### Key Changes

1. **Rename** WppExample → friendly_example_client
2. **Add** version information to startup banner and `-h` output
3. **Create** DTLS Provider Abstraction Layer with factory pattern
4. **Support** multiple DTLS libraries:
   - TinyDTLS 0.8.6 (existing, for constrained devices)
   - OpenSSL 3.x
   - **mbedTLS 3.6+** (new default for production)
   - wolfSSL 5.x
5. **Default** linkage: Static (embedded in executable)
6. **Option** for dynamic library linkage

---

## CURRENT STATE ANALYSIS

### Existing DTLS Libraries

**TinyDTLS 0.8.6:**
- Location: `2305-Wakaama/examples/shared/tinydtls/`
- Usage: Embedded DTLS for constrained devices
- Features: PSK, RPK, basic DTLS 1.2
- CID Support: ✅ RFC 9146 implemented (Phase 1-3)
- Pros: Small footprint, no dependencies
- Cons: Limited cipher suites, no DTLS 1.3

**OpenSSL (current for libcoap):**
- Version: Detected at build time
- Usage: libcoap DTLS backend
- Location: System library
- Features: Full DTLS 1.2/1.3, all cipher suites
- Pros: Industry standard, well-tested
- Cons: Large footprint, licensing (Apache 2.0)

### Current Build System

```cmake
option(WITH_DTLS "Enable DTLS support" ON)
option(WITH_MBEDTLS "Use mbedTLS (vs OpenSSL)" ON)
option(WITH_TINYDTLS "Use TinyDTLS for constrained devices" OFF)
```

**Issue:** These are mutually exclusive compile-time options, not runtime selectable.

---

## IMPLEMENTATION PHASES

### Phase 1: Preparation ✅ IN PROGRESS

#### 1.1 Rename WppExample → friendly_example_client ✅
- [x] `examples/CMakeLists.txt`
- [ ] Documentation references
- [ ] README files
- [ ] Build scripts

#### 1.2 Version Information ✅
- [x] Create `examples/version_info.h`
- [ ] Update `main.cpp` to display version banner
- [ ] Add `--version` flag support
- [ ] Update `-h` help text

### Phase 2: DTLS Provider Abstraction Layer

#### 2.1 Abstract Interface Design

**File:** `examples/platform/connection_dtls/DtlsProvider.h`

```cpp
class DtlsProvider {
public:
    virtual ~DtlsProvider() = default;

    // Lifecycle
    virtual int init() = 0;
    virtual int cleanup() = 0;

    // Context management
    virtual void* createContext() = 0;
    virtual void destroyContext(void* ctx) = 0;

    // Session management
    virtual void* createSession(void* ctx, const session_t* session) = 0;
    virtual void destroySession(void* session) = 0;

    // I/O
    virtual int send(void* session, const uint8_t* data, size_t len) = 0;
    virtual int recv(void* session, uint8_t* data, size_t len) = 0;

    // Handshake
    virtual int connect(void* session) = 0;
    virtual int accept(void* session) = 0;

    // Security configuration
    virtual int setPSK(void* ctx, const char* identity, const uint8_t* key, size_t key_len) = 0;
    virtual int setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
                       const uint8_t* pub_key, size_t pub_len) = 0;
    virtual int setCertificate(void* ctx, const char* cert_file, const char* key_file) = 0;

    // CID support (RFC 9146)
    virtual bool supportsCID() const = 0;
    virtual int enableCID(void* session, uint8_t cid_len) = 0;
    virtual bool isCIDNegotiated(void* session) const = 0;

    // Provider information
    virtual const char* getName() const = 0;
    virtual const char* getVersion() const = 0;
};
```

#### 2.2 Factory Pattern

**File:** `examples/platform/connection_dtls/DtlsProviderFactory.h`

```cpp
enum class DtlsProviderType {
    TINYDTLS,
    OPENSSL,
    MBEDTLS,
    WOLFSSL
};

class DtlsProviderFactory {
public:
    static DtlsProvider* create(DtlsProviderType type);
    static DtlsProvider* createDefault();  // Returns mbedTLS
    static DtlsProvider* createFromConfig(const std::string& provider_name);

    // Query available providers
    static std::vector<std::string> getAvailableProviders();
    static bool isProviderAvailable(const std::string& name);
};
```

### Phase 3: Provider Implementations

#### 3.1 TinyDTLS Provider (Existing Code Wrapper)

**File:** `examples/platform/connection_dtls/TinyDtlsProvider.cpp`

- Wrap existing `Connection.cpp` TinyDTLS code
- Implement `DtlsProvider` interface
- Preserve RFC 9146 CID support
- **Status:** Modify existing code

#### 3.2 OpenSSL Provider

**File:** `examples/platform/connection_dtls/OpenSslProvider.cpp`

**Features:**
- DTLS 1.2 + DTLS 1.3 support
- Full cipher suite support
- PSK, RPK, Certificate auth
- CID support: Check OpenSSL 3.2+ (RFC 9146)

**Dependencies:**
```cmake
find_package(OpenSSL 3.0 REQUIRED)
```

#### 3.3 mbedTLS 3.6+ Provider (NEW DEFAULT)

**File:** `examples/platform/connection_dtls/MbedTlsProvider.cpp`

**Features:**
- DTLS 1.2 (DTLS 1.3 in development)
- Optimized for embedded devices
- PSK, RPK, Certificate auth
- CID support: ✅ mbedTLS 2.x+ (RFC 9146)

**Dependencies:**
```cmake
find_package(MbedTLS 3.6 REQUIRED)
```

**Why mbedTLS as default:**
- Smaller footprint than OpenSSL
- Better for embedded targets
- Active development
- Apache 2.0 license
- Native CID support

#### 3.4 wolfSSL Provider

**File:** `examples/platform/connection_dtls/WolfSslProvider.cpp`

**Features:**
- DTLS 1.2 + DTLS 1.3
- Optimized for performance
- PSK, RPK, Certificate auth
- CID support: Check wolfSSL 5.x

**Dependencies:**
```cmake
find_package(wolfSSL 5.0 REQUIRED)
```

### Phase 4: CMake Configuration Update

#### 4.1 New Build Options

```cmake
# DTLS Provider Selection
option(WITH_DTLS "Enable DTLS support" ON)

# Select DTLS provider (mutually exclusive at build time, can add multiple)
option(DTLS_PROVIDER_TINYDTLS "Include TinyDTLS provider" ON)
option(DTLS_PROVIDER_OPENSSL "Include OpenSSL DTLS provider" OFF)
option(DTLS_PROVIDER_MBEDTLS "Include mbedTLS DTLS provider" ON)
option(DTLS_PROVIDER_WOLFSSL "Include wolfSSL DTLS provider" OFF)

# Default provider selection
set(DTLS_DEFAULT_PROVIDER "mbedTLS" CACHE STRING "Default DTLS provider")
set_property(CACHE DTLS_DEFAULT_PROVIDER PROPERTY STRINGS "TinyDTLS" "OpenSSL" "mbedTLS" "wolfSSL")

# Linkage type
option(DTLS_STATIC_LINK "Link DTLS libraries statically" ON)
```

#### 4.2 Conditional Compilation

```cmake
if(DTLS_PROVIDER_TINYDTLS)
    add_definitions(-DWITH_TINYDTLS_PROVIDER)
    # Add TinyDTLS sources
    include(../2305-Wakaama/examples/shared/tinydtls.cmake)
endif()

if(DTLS_PROVIDER_OPENSSL)
    find_package(OpenSSL 3.0 REQUIRED)
    add_definitions(-DWITH_OPENSSL_PROVIDER)
    target_link_libraries(friendly_example_client OpenSSL::SSL OpenSSL::Crypto)
endif()

if(DTLS_PROVIDER_MBEDTLS)
    find_package(MbedTLS 3.6 REQUIRED)
    add_definitions(-DWITH_MBEDTLS_PROVIDER)
    if(DTLS_STATIC_LINK)
        target_link_libraries(friendly_example_client MbedTLS::mbedtls MbedTLS::mbedx509 MbedTLS::mbedcrypto)
    else()
        target_link_libraries(friendly_example_client mbedtls mbedx509 mbedcrypto)
    endif()
endif()

if(DTLS_PROVIDER_WOLFSSL)
    find_package(wolfSSL 5.0 REQUIRED)
    add_definitions(-DWITH_WOLFSSL_PROVIDER)
    target_link_libraries(friendly_example_client wolfssl)
endif()
```

### Phase 5: CLI Integration

#### 5.1 New Command-Line Options

**File:** `examples/cli_options.h`

```cpp
struct CliOptions {
    // ... existing fields ...

    // DTLS Provider selection
    std::string dtls_provider;  // "tinydtls", "openssl", "mbedtls", "wolfssl", "auto"
    bool list_dtls_providers;
    bool show_version;
};
```

#### 5.2 CLI Parsing Updates

```bash
--dtls-provider <name>    Select DTLS provider (tinydtls, openssl, mbedtls, wolfssl, auto)
--list-dtls-providers     List available DTLS providers
--version                 Show version information
-V                        Short version info
```

### Phase 6: Connection Layer Updates

#### 6.1 Update Connection.h

**File:** `examples/platform/connection_dtls/Connection.h`

```cpp
class Connection {
private:
    DtlsProvider* m_dtls_provider;
    void* m_dtls_context;

public:
    Connection(int port, int addressFamily, DtlsProviderType provider_type);
    Connection(int port, int addressFamily, const std::string& provider_name);
    ~Connection();

    // Add provider query methods
    DtlsProvider* getDtlsProvider() const { return m_dtls_provider; }
    const char* getDtlsProviderName() const;
    const char* getDtlsProviderVersion() const;
};
```

#### 6.2 Update Connection.cpp

- Replace direct TinyDTLS calls with provider interface
- Add provider factory initialization
- Handle provider selection logic

---

## IMPLEMENTATION TIMELINE

### Week 1: Foundation
- [x] Day 1: Rename WppExample → friendly_example_client
- [x] Day 1: Add version information
- [ ] Day 2-3: Create DTLS Provider abstraction interface
- [ ] Day 4-5: Implement factory pattern

### Week 2: Provider Implementations
- [ ] Day 1-2: TinyDTLS provider (wrap existing)
- [ ] Day 3-4: mbedTLS 3.6+ provider (new default)
- [ ] Day 5: OpenSSL provider

### Week 3: Integration & Testing
- [ ] Day 1-2: wolfSSL provider
- [ ] Day 3: CMake configuration updates
- [ ] Day 4: CLI integration
- [ ] Day 5: Testing & validation

### Week 4: Documentation & Polish
- [ ] Day 1-2: Update all documentation
- [ ] Day 3: Create migration guide
- [ ] Day 4: Performance benchmarking
- [ ] Day 5: Final testing & release

---

## MIGRATION STRATEGY

### Backward Compatibility

**Option 1: Automatic Migration**
- Default to mbedTLS 3.6+ for new builds
- Keep TinyDTLS as option for constrained devices
- Detect existing builds and preserve provider

**Option 2: Explicit Selection**
```bash
# Old behavior (TinyDTLS)
./friendly_example_client --dtls-provider tinydtls

# New default (mbedTLS)
./friendly_example_client  # Uses mbedTLS by default

# Explicit selection
./friendly_example_client --dtls-provider openssl
./friendly_example_client --dtls-provider wolfssl
```

### Configuration File Support

**File:** `dtls_config.conf`
```ini
[dtls]
provider = mbedtls
static_link = true
cid_enabled = true
cid_length = 8
```

---

## TESTING STRATEGY

### Unit Tests

1. **Provider Factory Tests**
   - Test provider creation
   - Test provider availability detection
   - Test default provider selection

2. **Provider Interface Tests**
   - Test each provider's init/cleanup
   - Test PSK/RPK/Certificate configuration
   - Test send/recv operations

3. **CID Support Tests**
   - Verify CID support detection
   - Test CID negotiation (mbedTLS, TinyDTLS)
   - Verify non-CID providers handle gracefully

### Integration Tests

1. **Multi-Provider Test**
   - Build with all providers
   - Switch providers at runtime
   - Verify session establishment with each

2. **Server Compatibility**
   - Test each provider against Leshan
   - Test each provider against Coiote
   - Verify CID negotiation per provider

### Performance Benchmarks

| Provider | Handshake Time | Throughput | Memory Usage | Binary Size |
|----------|---------------|-----------|--------------|-------------|
| TinyDTLS | Baseline | Baseline | Baseline | Baseline |
| OpenSSL | TBD | TBD | TBD | TBD |
| mbedTLS | TBD | TBD | TBD | TBD |
| wolfSSL | TBD | TBD | TBD | TBD |

---

## SUCCESS CRITERIA

### Phase Completion

- [ ] All providers implement `DtlsProvider` interface
- [ ] Factory pattern allows runtime selection
- [ ] CMake supports multiple providers in single build
- [ ] CLI allows provider selection via `--dtls-provider`
- [ ] Version banner shows selected provider
- [ ] Documentation updated for all providers
- [ ] All existing tests pass with each provider
- [ ] CID support verified for TinyDTLS + mbedTLS

### Production Readiness

- [ ] mbedTLS 3.6+ as default passes all tests
- [ ] Backward compatibility with TinyDTLS preserved
- [ ] Performance acceptable for all providers
- [ ] Documentation complete
- [ ] Migration guide available

---

## RISK ASSESSMENT

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Provider API mismatch | Medium | High | Thorough abstraction design, comprehensive testing |
| CID support varies | High | Medium | Document CID support per provider, graceful fallback |
| Build complexity | Medium | Medium | Clear CMake documentation, default configurations |
| Performance regression | Low | High | Benchmark each provider, optimize hot paths |
| Breaking changes | Medium | High | Preserve backward compatibility, migration guide |

---

## DELIVERABLES

### Code

1. `examples/version_info.h` ✅
2. `examples/platform/connection_dtls/DtlsProvider.h`
3. `examples/platform/connection_dtls/DtlsProviderFactory.h`
4. `examples/platform/connection_dtls/TinyDtlsProvider.cpp`
5. `examples/platform/connection_dtls/MbedTlsProvider.cpp`
6. `examples/platform/connection_dtls/OpenSslProvider.cpp`
7. `examples/platform/connection_dtls/WolfSslProvider.cpp`
8. Updated `examples/CMakeLists.txt` ✅
9. Updated `examples/main.cpp`
10. Updated `examples/cli_options.h`

### Documentation

1. `DTLS_MULTI_PROVIDER_IMPLEMENTATION_PLAN.md` ✅ (this document)
2. `DTLS_PROVIDER_SELECTION_GUIDE.md`
3. `MIGRATION_GUIDE_MULTI_DTLS.md`
4. Updated `README.md`
5. Updated `BUILD_EXAMPLES.md`

### Testing

1. Provider unit tests
2. Integration test suite
3. Performance benchmarks
4. Server compatibility matrix

---

## CURRENT STATUS

**Date:** April 12, 2026
**Phase:** Phase 1 - Preparation (40% complete)

**Completed:**
- ✅ Version information header created
- ✅ WppExample renamed to friendly_example_client in CMakeLists.txt

**In Progress:**
- 🔄 Update main.cpp with version banner
- 🔄 Update CLI help text

**Next Steps:**
1. Complete version information integration
2. Begin DTLS Provider abstraction interface design
3. Implement factory pattern
4. Start mbedTLS provider implementation

---

**Document Version:** 1.0
**Last Updated:** April 12, 2026
**Status:** Living Document - Updated as implementation progresses
