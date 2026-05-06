# ============================================================================
# DTLS Library Optimization
# ============================================================================
# Optimizes DTLS library linking by:
# 1. Linking only the selected DTLS provider(s)
# 2. Using --gc-sections to remove unused symbols
# 3. Minimizing binary size with --as-needed
# 4. Static linking with symbol stripping
# ============================================================================

# ============================================================================
# DTLS Provider Configuration
# ============================================================================

set(DTLS_LIBRARIES "")
set(DTLS_INCLUDE_DIRS "")
set(DTLS_DEFINITIONS "")
set(DTLS_PROVIDER_COUNT 0)

# ============================================================================
# mbedTLS Provider
# ============================================================================
if(DTLS_PROVIDER_MBEDTLS)
    find_library(MBEDTLS_LIB mbedtls)
    find_library(MBEDX509_LIB mbedx509)
    find_library(MBEDCRYPTO_LIB mbedcrypto)
    find_path(MBEDTLS_INCLUDE mbedtls/ssl.h)

    if(MBEDTLS_LIB AND MBEDX509_LIB AND MBEDCRYPTO_LIB AND MBEDTLS_INCLUDE)
        message(STATUS "Found mbedTLS: ${MBEDTLS_LIB}")

        list(APPEND DTLS_LIBRARIES ${MBEDTLS_LIB} ${MBEDX509_LIB} ${MBEDCRYPTO_LIB})
        list(APPEND DTLS_INCLUDE_DIRS ${MBEDTLS_INCLUDE})
        list(APPEND DTLS_DEFINITIONS -DDTLS_PROVIDER_MBEDTLS)

        math(EXPR DTLS_PROVIDER_COUNT "${DTLS_PROVIDER_COUNT}+1")

        # mbedTLS optimization: Disable unused ciphers and features
        if(BUILD_ENV STREQUAL "PROD")
            # Minimal cipher suite for production (PSK + AES-CCM)
            list(APPEND DTLS_DEFINITIONS
                -DMBEDTLS_KEY_EXCHANGE_PSK_ENABLED
                -DMBEDTLS_AES_C
                -DMBEDTLS_CCM_C
                -DMBEDTLS_GCM_C
                -DMBEDTLS_SSL_DTLS_CONNECTION_ID
            )
        endif()
    else()
        message(WARNING "mbedTLS requested but not found. Disabling DTLS_PROVIDER_MBEDTLS.")
        set(DTLS_PROVIDER_MBEDTLS OFF)
    endif()
endif()

# ============================================================================
# TinyDTLS Provider
# ============================================================================
if(DTLS_PROVIDER_TINYDTLS)
    find_library(TINYDTLS_LIB tinydtls)
    find_path(TINYDTLS_INCLUDE tinydtls/dtls.h)

    if(TINYDTLS_LIB AND TINYDTLS_INCLUDE)
        message(STATUS "Found TinyDTLS: ${TINYDTLS_LIB}")

        list(APPEND DTLS_LIBRARIES ${TINYDTLS_LIB})
        list(APPEND DTLS_INCLUDE_DIRS ${TINYDTLS_INCLUDE})
        list(APPEND DTLS_DEFINITIONS -DDTLS_PROVIDER_TINYDTLS)

        math(EXPR DTLS_PROVIDER_COUNT "${DTLS_PROVIDER_COUNT}+1")
    else()
        message(WARNING "TinyDTLS requested but not found. Disabling DTLS_PROVIDER_TINYDTLS.")
        set(DTLS_PROVIDER_TINYDTLS OFF)
    endif()
endif()

# ============================================================================
# OpenSSL Provider
# ============================================================================
if(DTLS_PROVIDER_OPENSSL)
    find_package(OpenSSL REQUIRED)

    if(OPENSSL_FOUND)
        message(STATUS "Found OpenSSL: ${OPENSSL_VERSION}")

        list(APPEND DTLS_LIBRARIES OpenSSL::SSL OpenSSL::Crypto)
        list(APPEND DTLS_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
        list(APPEND DTLS_DEFINITIONS -DDTLS_PROVIDER_OPENSSL)

        math(EXPR DTLS_PROVIDER_COUNT "${DTLS_PROVIDER_COUNT}+1")
    else()
        message(WARNING "OpenSSL requested but not found. Disabling DTLS_PROVIDER_OPENSSL.")
        set(DTLS_PROVIDER_OPENSSL OFF)
    endif()
endif()

# ============================================================================
# wolfSSL Provider
# ============================================================================
if(DTLS_PROVIDER_WOLFSSL)
    find_library(WOLFSSL_LIB wolfssl)
    find_path(WOLFSSL_INCLUDE wolfssl/ssl.h)

    if(WOLFSSL_LIB AND WOLFSSL_INCLUDE)
        message(STATUS "Found wolfSSL: ${WOLFSSL_LIB}")

        list(APPEND DTLS_LIBRARIES ${WOLFSSL_LIB})
        list(APPEND DTLS_INCLUDE_DIRS ${WOLFSSL_INCLUDE})
        list(APPEND DTLS_DEFINITIONS -DDTLS_PROVIDER_WOLFSSL)

        math(EXPR DTLS_PROVIDER_COUNT "${DTLS_PROVIDER_COUNT}+1")
    else()
        message(WARNING "wolfSSL requested but not found. Disabling DTLS_PROVIDER_WOLFSSL.")
        set(DTLS_PROVIDER_WOLFSSL OFF)
    endif()
endif()

# ============================================================================
# Validate DTLS Configuration
# ============================================================================

if(DTLS_PROVIDER_COUNT EQUAL 0)
    message(FATAL_ERROR "No DTLS provider found! Enable at least one: MBEDTLS, TINYDTLS, OPENSSL, or WOLFSSL")
endif()

message(STATUS "DTLS Providers enabled: ${DTLS_PROVIDER_COUNT}")

# Set default provider if not specified
if(NOT DTLS_DEFAULT_PROVIDER)
    if(DTLS_PROVIDER_MBEDTLS)
        set(DTLS_DEFAULT_PROVIDER "mbedtls")
    elseif(DTLS_PROVIDER_TINYDTLS)
        set(DTLS_DEFAULT_PROVIDER "tinydtls")
    elseif(DTLS_PROVIDER_OPENSSL)
        set(DTLS_DEFAULT_PROVIDER "openssl")
    elseif(DTLS_PROVIDER_WOLFSSL)
        set(DTLS_DEFAULT_PROVIDER "wolfssl")
    endif()
endif()

message(STATUS "Default DTLS Provider: ${DTLS_DEFAULT_PROVIDER}")
list(APPEND DTLS_DEFINITIONS -DDTLS_DEFAULT_PROVIDER="${DTLS_DEFAULT_PROVIDER}")

# ============================================================================
# Optimized Linking Flags
# ============================================================================

# Function sections and data sections (required for --gc-sections)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")

# Linker optimization flags
set(DTLS_LINKER_FLAGS "")

# Remove unused sections (symbol garbage collection)
list(APPEND DTLS_LINKER_FLAGS "-Wl,--gc-sections")

# Only link needed libraries (avoid over-linking)
list(APPEND DTLS_LINKER_FLAGS "-Wl,--as-needed")

# Combine relocations (reduce binary size)
list(APPEND DTLS_LINKER_FLAGS "-Wl,-O1")

# Production-specific optimizations
if(BUILD_ENV STREQUAL "PROD")
    # Strip all symbols
    list(APPEND DTLS_LINKER_FLAGS "-Wl,--strip-all")

    # Optimize for size
    list(APPEND DTLS_LINKER_FLAGS "-Wl,--gc-sections")
    list(APPEND DTLS_LINKER_FLAGS "-Wl,--icf=all")  # Identical Code Folding

    # Compress debug info (if any remains)
    list(APPEND DTLS_LINKER_FLAGS "-Wl,--compress-debug-sections=zlib")
endif()

# Static linking if requested
if(DTLS_STATIC_LINK)
    message(STATUS "Static DTLS linking enabled")

    # Link DTLS libraries statically
    list(APPEND DTLS_LINKER_FLAGS "-static-libgcc")
    list(APPEND DTLS_LINKER_FLAGS "-static-libstdc++")

    # For static linking, prefer .a libraries
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

    # Rebuild DTLS library list with static variants
    set(DTLS_LIBRARIES_STATIC "")
    foreach(lib ${DTLS_LIBRARIES})
        if(TARGET ${lib})
            # It's a CMake target, keep as-is
            list(APPEND DTLS_LIBRARIES_STATIC ${lib})
        else()
            # Try to find static version
            get_filename_component(lib_name ${lib} NAME_WE)
            string(REGEX REPLACE "^lib" "" lib_name_clean ${lib_name})

            find_library(${lib_name_clean}_STATIC_LIB
                NAMES lib${lib_name_clean}.a ${lib_name_clean}
                PATHS /usr/lib /usr/local/lib
                NO_DEFAULT_PATH
            )

            if(${lib_name_clean}_STATIC_LIB)
                message(STATUS "  Using static library: ${${lib_name_clean}_STATIC_LIB}")
                list(APPEND DTLS_LIBRARIES_STATIC ${${lib_name_clean}_STATIC_LIB})
            else()
                message(STATUS "  Static library not found for ${lib_name_clean}, using dynamic")
                list(APPEND DTLS_LIBRARIES_STATIC ${lib})
            endif()
        endif()
    endforeach()

    set(DTLS_LIBRARIES ${DTLS_LIBRARIES_STATIC})
endif()

# Convert linker flags list to string
string(REPLACE ";" " " DTLS_LINKER_FLAGS_STR "${DTLS_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${DTLS_LINKER_FLAGS_STR}")

# ============================================================================
# Helper Function: Link DTLS to Target
# ============================================================================

function(target_link_dtls_optimized target_name)
    # Add DTLS definitions
    target_compile_definitions(${target_name} PRIVATE ${DTLS_DEFINITIONS})

    # Add DTLS include directories
    target_include_directories(${target_name} PRIVATE ${DTLS_INCLUDE_DIRS})

    # Link DTLS libraries with optimization
    target_link_libraries(${target_name} PRIVATE ${DTLS_LIBRARIES})

    # Apply linker flags
    set_target_properties(${target_name} PROPERTIES
        LINK_FLAGS "${DTLS_LINKER_FLAGS_STR}"
    )

    message(STATUS "Target '${target_name}' linked with optimized DTLS libraries")
endfunction()

# ============================================================================
# DTLS Configuration Summary
# ============================================================================

message(STATUS "====================================================")
message(STATUS "DTLS Configuration Summary")
message(STATUS "====================================================")
message(STATUS "DTLS Providers: ${DTLS_PROVIDER_COUNT}")
message(STATUS "  - mbedTLS: ${DTLS_PROVIDER_MBEDTLS}")
message(STATUS "  - TinyDTLS: ${DTLS_PROVIDER_TINYDTLS}")
message(STATUS "  - OpenSSL: ${DTLS_PROVIDER_OPENSSL}")
message(STATUS "  - wolfSSL: ${DTLS_PROVIDER_WOLFSSL}")
message(STATUS "Default Provider: ${DTLS_DEFAULT_PROVIDER}")
message(STATUS "Static Linking: ${DTLS_STATIC_LINK}")
message(STATUS "Libraries: ${DTLS_LIBRARIES}")
message(STATUS "Linker Flags: ${DTLS_LINKER_FLAGS_STR}")
message(STATUS "====================================================")
