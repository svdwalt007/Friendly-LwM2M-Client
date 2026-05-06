# ============================================================================
# Build Environment Configuration
# ============================================================================
# Defines build configurations for different deployment environments:
# - TEST: Unit testing, debug symbols, sanitizers, coverage
# - DEV: Development builds with debug info, moderate optimization
# - PREPROD: Pre-production builds, optimized, profiling enabled
# - PROD: Production builds, fully optimized, stripped, minimal size
# ============================================================================

# Set default build environment if not specified
if(NOT DEFINED BUILD_ENV)
    set(BUILD_ENV "DEV" CACHE STRING "Build environment: TEST, DEV, PREPROD, PROD")
endif()

message(STATUS "Build Environment: ${BUILD_ENV}")

# ============================================================================
# TEST Environment
# ============================================================================
if(BUILD_ENV STREQUAL "TEST")
    message(STATUS "Configuring TEST build environment")

    # Enable all testing features
    set(BUILD_TESTS ON CACHE BOOL "" FORCE)
    set(ENABLE_COVERAGE ON CACHE BOOL "" FORCE)
    set(ENABLE_SANITIZERS ON CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES ON CACHE BOOL "" FORCE)

    # Debug optimization
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -D_DEBUG" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG -D_DEBUG" CACHE STRING "" FORCE)

    # Enable verbose logging
    add_definitions(-DLWM2M_LOG_LEVEL=5)  # TRACE level
    add_definitions(-DLWM2M_VERBOSE_LOGS)

    # Enable all optional features for comprehensive testing
    set(WITH_DELTA_FOTA ON CACHE BOOL "" FORCE)
    set(WITH_MQTT ON CACHE BOOL "" FORCE)
    set(WITH_EDGE_AI ON CACHE BOOL "" FORCE)
    set(WITH_STARLINK ON CACHE BOOL "" FORCE)

    # Enable all DTLS providers for testing
    set(DTLS_PROVIDER_MBEDTLS ON CACHE BOOL "" FORCE)
    set(DTLS_PROVIDER_TINYDTLS ON CACHE BOOL "" FORCE)
    set(DTLS_PROVIDER_OPENSSL ON CACHE BOOL "" FORCE)

    # Static linking for reproducible tests
    set(DTLS_STATIC_LINK ON CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# ============================================================================
# DEV Environment
# ============================================================================
elseif(BUILD_ENV STREQUAL "DEV")
    message(STATUS "Configuring DEV build environment")

    # Enable examples and selective testing
    set(BUILD_TESTS ON CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES ON CACHE BOOL "" FORCE)
    set(ENABLE_COVERAGE OFF CACHE BOOL "" FORCE)
    set(ENABLE_SANITIZERS ON CACHE BOOL "" FORCE)

    # RelWithDebInfo: Optimized but with debug symbols
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -DNDEBUG" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O2 -DNDEBUG" CACHE STRING "" FORCE)

    # Standard logging
    add_definitions(-DLWM2M_LOG_LEVEL=4)  # DEBUG level

    # Enable core features (Edge AI and Starlink now enabled by default)
    set(WITH_DELTA_FOTA ON CACHE BOOL "" FORCE)
    set(WITH_MQTT ON CACHE BOOL "" FORCE)
    set(WITH_EDGE_AI ON CACHE BOOL "" FORCE)
    set(WITH_STARLINK ON CACHE BOOL "" FORCE)

    # Default DTLS provider only
    set(DTLS_PROVIDER_MBEDTLS ON CACHE BOOL "" FORCE)
    set(DTLS_PROVIDER_TINYDTLS OFF CACHE BOOL "" FORCE)
    set(DTLS_PROVIDER_OPENSSL OFF CACHE BOOL "" FORCE)

    # Allow shared libs for faster iteration
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# ============================================================================
# PREPROD Environment
# ============================================================================
elseif(BUILD_ENV STREQUAL "PREPROD")
    message(STATUS "Configuring PREPROD build environment")

    # Disable tests, enable examples for validation
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES ON CACHE BOOL "" FORCE)
    set(ENABLE_COVERAGE OFF CACHE BOOL "" FORCE)
    set(ENABLE_SANITIZERS OFF CACHE BOOL "" FORCE)

    # Release with profiling symbols
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -g1" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -g1" CACHE STRING "" FORCE)

    # Production logging (warnings and errors only)
    add_definitions(-DLWM2M_LOG_LEVEL=2)  # WARN level

    # Enable production features based on user configuration
    # (leave WITH_* options as user-configured)

    # Static linking for deployment
    set(DTLS_STATIC_LINK ON CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    # Link-time optimization
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# ============================================================================
# PROD Environment
# ============================================================================
elseif(BUILD_ENV STREQUAL "PROD")
    message(STATUS "Configuring PROD build environment")

    # Disable all testing/development features
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(ENABLE_COVERAGE OFF CACHE BOOL "" FORCE)
    set(ENABLE_SANITIZERS OFF CACHE BOOL "" FORCE)

    # Fully optimized release build
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "" FORCE)

    # Minimal logging (errors only)
    add_definitions(-DLWM2M_LOG_LEVEL=1)  # ERROR level

    # Strip debug symbols
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")

    # Enable production features based on user configuration
    # (leave WITH_* options as user-configured)

    # Static linking, no shared libs
    set(DTLS_STATIC_LINK ON CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    # Aggressive optimization flags
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)  # LTO
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")

else()
    message(FATAL_ERROR "Invalid BUILD_ENV: ${BUILD_ENV}. Must be TEST, DEV, PREPROD, or PROD")
endif()

# ============================================================================
# Common Security Hardening (all environments)
# ============================================================================

# Stack protection
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-strong")

# Position Independent Executable (PIE) for ASLR
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Format string protection
add_definitions(-D_FORTIFY_SOURCE=2)

# ============================================================================
# Build Environment Summary
# ============================================================================

message(STATUS "====================================================")
message(STATUS "Build Environment Configuration Summary")
message(STATUS "====================================================")
message(STATUS "Environment: ${BUILD_ENV}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build Tests: ${BUILD_TESTS}")
message(STATUS "Build Examples: ${BUILD_EXAMPLES}")
message(STATUS "Coverage: ${ENABLE_COVERAGE}")
message(STATUS "Sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "Static DTLS Link: ${DTLS_STATIC_LINK}")
message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Linker Flags: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "====================================================")
