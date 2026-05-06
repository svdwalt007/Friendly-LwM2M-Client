# CMake minimum version requirement
cmake_minimum_required(VERSION 3.14)

# Project's name
project(friendly_example_client C CXX)

# Add cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include file with compiler config
include(../wpp/configs/compiler_config.cmake)
# Include file with wpp config
include(../wpp/configs/wpp_config.cmake)
# Add file with wpp lib configs
include(../wpp/wpp.cmake)

# Compiler options
add_compile_options(
    # Disabled because Connection for dtls cast from 'sockaddr *' to 'sockaddr_in *' increases required alignment from 2 to 4
    -Wno-cast-align
    # Reduce noise: Too many false positives
    -Wno-uninitialized
)

# Compiler-specific options for variadic macro compatibility
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        # Allows to compile tinydtls without errors
        -Wno-c99-extensions
        # Allow usage ##__VA_ARGS__ in macros (needed for WPP logging)
        -Wno-gnu-zero-variadic-macro-arguments
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(
        # Allow GNU extension in variadic macros (needed for WPP logging)
        -Wno-variadic-macros
        # Disable pedantic warnings for C++ (needed for ##__VA_ARGS__)
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-pedantic>
    )
endif()

# Generating .map file
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=output.map")

# Find the libcurl package
find_package(CURL REQUIRED)

# CURL dependencies (required when using static libcurl)
find_library(SSH_LIB ssh)
find_library(NGHTTP2_LIB nghttp2)
find_library(ZSTD_LIB_CURL zstd)
find_library(BROTLI_COMMON_LIB brotlicommon)
find_library(BROTLI_DEC_LIB_CURL brotlidec)

# GSSAPI/Kerberos libraries (required by libcurl and libssh with GSSAPI support)
# Try using pkg-config first, then fall back to find_library
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(KRB5_GSSAPI QUIET krb5-gssapi)
    if(KRB5_GSSAPI_FOUND)
        set(GSSAPI_LIBRARIES ${KRB5_GSSAPI_LIBRARIES})
        set(GSSAPI_FOUND TRUE)
        message(STATUS "Found GSSAPI via pkg-config: ${KRB5_GSSAPI_LIBRARIES}")
    endif()
endif()

# Fall back to manual library search if pkg-config didn't find it
if(NOT GSSAPI_FOUND)
    find_library(GSSAPI_KRB5_LIB gssapi_krb5)
    find_library(KRB5_LIB krb5)
    find_library(K5CRYPTO_LIB k5crypto)
    find_library(COM_ERR_LIB com_err)
    find_library(KEYUTILS_LIB keyutils)

    if(GSSAPI_KRB5_LIB AND KRB5_LIB)
        set(GSSAPI_LIBRARIES ${GSSAPI_KRB5_LIB} ${KRB5_LIB})
        if(K5CRYPTO_LIB)
            list(APPEND GSSAPI_LIBRARIES ${K5CRYPTO_LIB})
        endif()
        if(COM_ERR_LIB)
            list(APPEND GSSAPI_LIBRARIES ${COM_ERR_LIB})
        endif()
        if(KEYUTILS_LIB)
            list(APPEND GSSAPI_LIBRARIES ${KEYUTILS_LIB})
        endif()
        set(GSSAPI_FOUND TRUE)
        message(STATUS "Found GSSAPI for CURL/SSH: ${GSSAPI_LIBRARIES}")
    else()
        message(STATUS "GSSAPI not found - will link without GSSAPI support")
        message(STATUS "  If build fails with GSSAPI errors, install: sudo apt-get install libkrb5-dev")
    endif()
endif()

if(SSH_LIB)
    message(STATUS "Found libssh for CURL: ${SSH_LIB}")
endif()
if(NGHTTP2_LIB)
    message(STATUS "Found nghttp2 for CURL: ${NGHTTP2_LIB}")
endif()
if(ZSTD_LIB_CURL)
    message(STATUS "Found zstd for CURL: ${ZSTD_LIB_CURL}")
endif()
if(BROTLI_COMMON_LIB AND BROTLI_DEC_LIB_CURL)
    message(STATUS "Found brotli for CURL: ${BROTLI_COMMON_LIB}, ${BROTLI_DEC_LIB_CURL}")
endif()

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Variables that will be filled in subdirectories
set(SOURCES main.cpp objects.cpp ConfigLoader.cpp)
set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/)

# General options
option(WITH_UDP_DTLS "Enable UDP DTLS" ON)

# DTLS Multi-Provider Configuration
option(DTLS_PROVIDER_TINYDTLS "Include TinyDTLS provider" ON)
option(DTLS_PROVIDER_MBEDTLS "Include mbedTLS provider" ON)
option(DTLS_PROVIDER_OPENSSL "Include OpenSSL provider" OFF)
option(DTLS_PROVIDER_WOLFSSL "Include wolfSSL provider" OFF)
set(DTLS_DEFAULT_PROVIDER "mbedtls" CACHE STRING "Default DTLS provider (auto|tinydtls|mbedtls|openssl|wolfssl)")
option(DTLS_STATIC_LINK "Link DTLS libraries statically" ON)

if (WITH_UDP_DTLS)
    set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WITH_UDP_DTLS)
    set(WPP_DEFINITIONS ${WPP_DEFINITIONS} DTLS_WITH_PSK)
    # set(WPP_DEFINITIONS ${WPP_DEFINITIONS} DTLS_WITH_RPK)

    # Configure DTLS providers
    set(DTLS_PROVIDER_SOURCES "")
    set(DTLS_PROVIDER_LIBRARIES "")

    # TinyDTLS Provider
    if(DTLS_PROVIDER_TINYDTLS)
        message(STATUS "Enabling TinyDTLS provider")
        set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WITH_TINYDTLS_PROVIDER)
        list(APPEND DTLS_PROVIDER_SOURCES
            platform/connection_dtls/TinyDtlsProvider.cpp
        )
    endif()

    # mbedTLS Provider
    if(DTLS_PROVIDER_MBEDTLS)
        message(STATUS "Enabling mbedTLS provider")
        find_package(MbedTLS)
        if(MbedTLS_FOUND)
            set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WITH_MBEDTLS_PROVIDER)
            list(APPEND DTLS_PROVIDER_SOURCES
                platform/connection_dtls/MbedTlsProvider.cpp
            )
            if(DTLS_STATIC_LINK)
                list(APPEND DTLS_PROVIDER_LIBRARIES MbedTLS::mbedtls MbedTLS::mbedx509 MbedTLS::mbedcrypto)
            else()
                list(APPEND DTLS_PROVIDER_LIBRARIES mbedtls mbedx509 mbedcrypto)
            endif()
        else()
            message(WARNING "mbedTLS not found, mbedTLS provider will not be available")
            set(DTLS_PROVIDER_MBEDTLS OFF)
        endif()
    endif()

    # OpenSSL Provider (already found earlier)
    if(DTLS_PROVIDER_OPENSSL)
        message(STATUS "Enabling OpenSSL provider")
        if(OpenSSL_FOUND)
            set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WITH_OPENSSL_PROVIDER)
            list(APPEND DTLS_PROVIDER_SOURCES
                platform/connection_dtls/OpenSslProvider.cpp
            )
            # OpenSSL already linked via OpenSSL::SSL and OpenSSL::Crypto
        else()
            message(WARNING "OpenSSL not found, OpenSSL provider will not be available")
            set(DTLS_PROVIDER_OPENSSL OFF)
        endif()
    endif()

    # wolfSSL Provider
    if(DTLS_PROVIDER_WOLFSSL)
        message(STATUS "Enabling wolfSSL provider")
        find_package(PkgConfig)
        if(PkgConfig_FOUND)
            pkg_check_modules(WOLFSSL wolfssl)
            if(WOLFSSL_FOUND)
                set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WITH_WOLFSSL_PROVIDER)
                list(APPEND DTLS_PROVIDER_SOURCES
                    platform/connection_dtls/WolfSslProvider.cpp
                )
                if(DTLS_STATIC_LINK)
                    list(APPEND DTLS_PROVIDER_LIBRARIES ${WOLFSSL_STATIC_LIBRARIES})
                else()
                    list(APPEND DTLS_PROVIDER_LIBRARIES ${WOLFSSL_LIBRARIES})
                endif()
                include_directories(${WOLFSSL_INCLUDE_DIRS})
            else()
                message(WARNING "wolfSSL not found, wolfSSL provider will not be available")
                set(DTLS_PROVIDER_WOLFSSL OFF)
            endif()
        else()
            message(WARNING "PkgConfig not found, cannot search for wolfSSL")
            set(DTLS_PROVIDER_WOLFSSL OFF)
        endif()
    endif()

    # Add DtlsProviderFactory source
    list(APPEND DTLS_PROVIDER_SOURCES
        platform/connection_dtls/DtlsProviderFactory.cpp
    )

    # Validate at least one provider is available
    if(NOT DTLS_PROVIDER_TINYDTLS AND NOT DTLS_PROVIDER_MBEDTLS AND NOT DTLS_PROVIDER_OPENSSL AND NOT DTLS_PROVIDER_WOLFSSL)
        message(FATAL_ERROR "At least one DTLS provider must be enabled")
    endif()

    message(STATUS "DTLS Provider Sources: ${DTLS_PROVIDER_SOURCES}")
    message(STATUS "DTLS Provider Libraries: ${DTLS_PROVIDER_LIBRARIES}")
endif()

add_executable(friendly_example_client)
# List of preprocessor defines that will be set for compiler with key -D
target_compile_definitions(friendly_example_client PUBLIC ${WPP_DEFINITIONS})
# Link wpp sources to tests
target_link_wpp(friendly_example_client wpp)
# Link with Wpp library
target_link_libraries(friendly_example_client pthread)

# P1-8: Link with friendly_lwm2m library for log_ring on Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(friendly_example_client friendly_lwm2m)
endif()

# ============================================================================
# Link Minimal CURL Dependencies (for custom libcurl from /usr/local/lib)
# ============================================================================
# Note: Order matters for static linking - dependencies must come before their providers
# Our minimal libcurl only needs: HTTP/2, compression (zstd, brotli), and OpenSSL

# HTTP/2 support (optional but recommended)
if(NGHTTP2_LIB)
    target_link_libraries(friendly_example_client ${NGHTTP2_LIB})
endif()

# Zstd compression (optional but recommended)
if(ZSTD_LIB_CURL)
    target_link_libraries(friendly_example_client ${ZSTD_LIB_CURL})
endif()

# Brotli compression libraries (must be in this order: enc, dec, common)
if(BROTLI_ENC_LIB_CURL)
    target_link_libraries(friendly_example_client ${BROTLI_ENC_LIB_CURL})
endif()
if(BROTLI_DEC_LIB_CURL)
    target_link_libraries(friendly_example_client ${BROTLI_DEC_LIB_CURL})
endif()
if(BROTLI_COMMON_LIB)
    target_link_libraries(friendly_example_client ${BROTLI_COMMON_LIB})
endif()

# GSSAPI/Kerberos (if libcurl was built with GSSAPI support)
if(GSSAPI_FOUND)
    target_link_libraries(friendly_example_client ${GSSAPI_LIBRARIES})
endif()

# Zlib compression (required by libcurl)
target_link_libraries(friendly_example_client ZLIB::ZLIB)

# ============================================================================
# REMOVED: Unnecessary CURL dependencies
# ============================================================================
# The following libraries are NO LONGER linked because our custom minimal
# libcurl was built without LDAP, SSH, RTMP, IDN, and database support:
#
# - SSH_LIB (libssh for SFTP/SCP)
# - PSL_LIB (Public Suffix List)
# - RTMP_LIB (RTMP streaming)
# - IDN2_LIB (Internationalized Domain Names)
# - LDAP_LIB, LBER_LIB (LDAP protocol)
# - GNUTLS_LIB (TLS for LDAP)
# - SASL2_LIB (SASL authentication for LDAP)
# - NETTLE_LIB, HOGWEED_LIB, GMP_LIB, TASN1_LIB (GnuTLS crypto)
# - UNISTRING_LIB (Unicode string handling for IDN2)
# - Direct OpenSSL::Crypto linking here (now handled below with libcoap)
#
# This reduces dependency count from 28 libraries to ~8 libraries (71% reduction)

# Note: OpenSSL::SSL and OpenSSL::Crypto are linked below with CURL and libcoap
# OpenSSL provides SSL/TLS for HTTPS connections

# Add subdiretories with sources
add_subdirectory(platform)
check_target_definition(friendly_example_client "OBJ_O_5_FIRMWARE_UPDATE" is_defined)
if(is_defined)
    # Build libcoap
    execute_process(
        COMMAND bash libcoap_build.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/coap
        RESULT_VARIABLE result
        OUTPUT_VARIABLE libcoap_path
        ERROR_VARIABLE error
    )
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "Failed to build libcoap: ${error}")
    else()
        message(STATUS "libcoap built successfully. Library path: ${libcoap_path}")
    endif()
    # Add libcoap to the project
    target_link_libraries(friendly_example_client ${libcoap_path}/libcoap-3.so CURL::libcurl OpenSSL::SSL OpenSSL::Crypto)

    # Option to use OpenWRT firmware updater instead of example updater
    option(USE_OPENWRT_FW_UPDATER "Use OpenWRT firmware updater with sysupgrade integration" OFF)

    if(USE_OPENWRT_FW_UPDATER)
        # Add OpenWRT FirmwareUpdater to the project
        message(STATUS "Using OpenWRT Firmware Updater")
        add_subdirectory(OpenWrtFirmwareUpdater)
        target_link_libraries(friendly_example_client OpenWrtFirmwareUpdater)
        target_compile_definitions(friendly_example_client PRIVATE USE_OPENWRT_FW_UPDATER)
    else()
        # Add example FirmwareUpdater to the project
        message(STATUS "Using example Firmware Updater")
        add_subdirectory(FirmwareUpdater)
    endif()
endif()

# Include sources
target_sources(friendly_example_client PUBLIC ${SOURCES})
# List of directories that will be set for compiler with key -I
target_include_directories(friendly_example_client PUBLIC ${INCLUDES} ${libcoap_path}/include/)

# P1-8: Include log_ring header directory for Linux builds
target_include_directories(friendly_example_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/platform/linux)

# Add DTLS provider sources
if (WITH_UDP_DTLS)
    # Add provider abstraction sources
    target_sources(friendly_example_client PRIVATE ${DTLS_PROVIDER_SOURCES})

    # Link provider libraries
    if(DTLS_PROVIDER_LIBRARIES)
        target_link_libraries(friendly_example_client ${DTLS_PROVIDER_LIBRARIES})
    endif()

    # Add TinyDTLS sources if TinyDTLS provider is enabled
    if(DTLS_PROVIDER_TINYDTLS)
        # tinydtls.cmake already included in parent CMakeLists.txt
        target_sources_tinydtls(friendly_example_client)
    endif()
endif()

# message(STATUS "friendly_example_client:INCLUDES: ${INCLUDES}")
# message(STATUS "friendly_example_client:SOURCES: ${SOURCES}")
# message(STATUS "friendly_example_client:COMPILE_DEFINITIONS: ${COMPILE_DEFINITIONS}")

# Generate a .bin file 
add_custom_command(TARGET friendly_example_client POST_BUILD
                   COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:friendly_example_client> $<TARGET_FILE_DIR:friendly_example_client>/friendly_example_client.bin
                   COMMENT "Creating binary file")
# Generate a .hex file 
add_custom_command(TARGET friendly_example_client POST_BUILD
                   COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:friendly_example_client> $<TARGET_FILE_DIR:friendly_example_client>/friendly_example_client.hex
                   COMMENT "Creating hex file")
# Get information about size compiled program
add_custom_command(TARGET friendly_example_client POST_BUILD
                   COMMAND size $<TARGET_FILE:friendly_example_client>
                   COMMENT "Compiled program size")