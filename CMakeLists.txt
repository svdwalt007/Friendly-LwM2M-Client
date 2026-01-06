cmake_minimum_required(VERSION 3.16)

project(FriendlyLwM2MClient
    VERSION 1.0.0
    DESCRIPTION "Enhanced LwM2M Client with Delta Firmware Updates"
    LANGUAGES CXX C
)

# ============================================================================
# Build Options
# ============================================================================

option(WITH_DTLS "Enable DTLS support" ON)
option(WITH_MBEDTLS "Use mbedTLS (vs OpenSSL)" ON)
option(WITH_TINYDTLS "Use TinyDTLS for constrained devices" OFF)
option(WITH_DELTA_FOTA "Enable delta firmware updates" ON)
option(WITH_BSDIFF "Enable BSDIFF algorithm" ON)
option(WITH_VCDIFF "Enable VCDIFF algorithm" ON)
option(WITH_COURGETTE "Enable Courgette algorithm" ON)
option(WITH_COMPRESSION "Enable compression support" ON)
option(WITH_ROLLBACK "Enable A/B partition rollback" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_SANITIZERS "Enable address/undefined sanitizers" OFF)

# ============================================================================
# C++ Standard
# ============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Compiler Flags
# ============================================================================

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif()

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
endif()

if(ENABLE_SANITIZERS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
endif()

# ============================================================================
# Find Dependencies
# ============================================================================

# Required packages
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(CURL REQUIRED)

# Optional compression libraries
find_package(BZip2)
find_package(LibLZMA)

# Check for zstd
find_library(ZSTD_LIB zstd)
find_path(ZSTD_INCLUDE zstd.h)
if(ZSTD_LIB AND ZSTD_INCLUDE)
    set(HAS_ZSTD TRUE)
endif()

# Check for brotli
find_library(BROTLI_ENC_LIB brotlienc)
find_library(BROTLI_DEC_LIB brotlidec)
find_path(BROTLI_INCLUDE brotli/encode.h)
if(BROTLI_ENC_LIB AND BROTLI_DEC_LIB AND BROTLI_INCLUDE)
    set(HAS_BROTLI TRUE)
endif()

# Check for blkid (Linux partition detection)
find_library(BLKID_LIB blkid)
find_path(BLKID_INCLUDE blkid/blkid.h)

# mbedTLS
if(WITH_MBEDTLS)
    find_library(MBEDTLS_LIB mbedtls)
    find_library(MBEDX509_LIB mbedx509)
    find_library(MBEDCRYPTO_LIB mbedcrypto)
    find_path(MBEDTLS_INCLUDE mbedtls/ssl.h)
endif()

# TinyDTLS
if(WITH_TINYDTLS)
    find_library(TINYDTLS_LIB tinydtls)
    find_path(TINYDTLS_INCLUDE tinydtls/dtls.h)
endif()

# libcoap
find_library(COAP_LIB coap-3)
find_path(COAP_INCLUDE coap3/coap.h)

# Testing
if(BUILD_TESTS)
    find_package(GTest)
    if(NOT GTest_FOUND)
        # Fetch GTest if not found
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
        )
        FetchContent_MakeAvailable(googletest)
    endif()
    enable_testing()
endif()

# ============================================================================
# Source Files
# ============================================================================

set(PLATFORM_SOURCES
    src/platform/openwrt_platform.cpp
    src/platform/linux_platform.cpp
)

set(FIRMWARE_SOURCES
    src/firmware/delta_algorithms.cpp
    src/firmware/rollback_manager.cpp
)

set(TRANSPORT_SOURCES
    src/transport/blockwise_transfer.cpp
)

set(OBJECTS_SOURCES
    src/objects/advanced_firmware_update_object.cpp
)

set(ALL_SOURCES
    ${PLATFORM_SOURCES}
    ${FIRMWARE_SOURCES}
    ${TRANSPORT_SOURCES}
    ${OBJECTS_SOURCES}
)

# ============================================================================
# Main Library
# ============================================================================

add_library(friendly_lwm2m ${ALL_SOURCES})

target_include_directories(friendly_lwm2m
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(friendly_lwm2m
    PUBLIC
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        ZLIB::ZLIB
        CURL::libcurl
)

# Optional dependencies
if(BZIP2_FOUND)
    target_link_libraries(friendly_lwm2m PUBLIC BZip2::BZip2)
    target_compile_definitions(friendly_lwm2m PUBLIC HAS_BZIP2=1)
endif()

if(LIBLZMA_FOUND)
    target_link_libraries(friendly_lwm2m PUBLIC LibLZMA::LibLZMA)
    target_compile_definitions(friendly_lwm2m PUBLIC HAS_LZMA=1)
endif()

if(HAS_ZSTD)
    target_link_libraries(friendly_lwm2m PUBLIC ${ZSTD_LIB})
    target_include_directories(friendly_lwm2m PUBLIC ${ZSTD_INCLUDE})
    target_compile_definitions(friendly_lwm2m PUBLIC HAS_ZSTD=1)
endif()

if(HAS_BROTLI)
    target_link_libraries(friendly_lwm2m PUBLIC ${BROTLI_ENC_LIB} ${BROTLI_DEC_LIB})
    target_include_directories(friendly_lwm2m PUBLIC ${BROTLI_INCLUDE})
    target_compile_definitions(friendly_lwm2m PUBLIC HAS_BROTLI=1)
endif()

if(BLKID_LIB AND BLKID_INCLUDE)
    target_link_libraries(friendly_lwm2m PUBLIC ${BLKID_LIB})
    target_include_directories(friendly_lwm2m PUBLIC ${BLKID_INCLUDE})
endif()

if(WITH_MBEDTLS AND MBEDTLS_LIB)
    target_link_libraries(friendly_lwm2m PUBLIC 
        ${MBEDTLS_LIB} ${MBEDX509_LIB} ${MBEDCRYPTO_LIB})
    target_include_directories(friendly_lwm2m PUBLIC ${MBEDTLS_INCLUDE})
    target_compile_definitions(friendly_lwm2m PUBLIC WITH_MBEDTLS=1)
endif()

if(COAP_LIB AND COAP_INCLUDE)
    target_link_libraries(friendly_lwm2m PUBLIC ${COAP_LIB})
    target_include_directories(friendly_lwm2m PUBLIC ${COAP_INCLUDE})
endif()

# Compile definitions
target_compile_definitions(friendly_lwm2m
    PUBLIC
        $<$<BOOL:${WITH_DELTA_FOTA}>:WITH_DELTA_FOTA=1>
        $<$<BOOL:${WITH_BSDIFF}>:WITH_BSDIFF=1>
        $<$<BOOL:${WITH_VCDIFF}>:WITH_VCDIFF=1>
        $<$<BOOL:${WITH_COURGETTE}>:WITH_COURGETTE=1>
        $<$<BOOL:${WITH_ROLLBACK}>:WITH_ROLLBACK=1>
)

# ============================================================================
# Tests
# ============================================================================

if(BUILD_TESTS)
    add_executable(test_delta_algorithms
        test/test_delta_algorithms.cpp
    )
    target_link_libraries(test_delta_algorithms
        friendly_lwm2m
        GTest::gtest
        GTest::gtest_main
    )
    add_test(NAME DeltaAlgorithms COMMAND test_delta_algorithms)

    add_executable(test_blockwise_transfer
        test/test_blockwise_transfer.cpp
    )
    target_link_libraries(test_blockwise_transfer
        friendly_lwm2m
        GTest::gtest
        GTest::gtest_main
    )
    add_test(NAME BlockwiseTransfer COMMAND test_blockwise_transfer)

    add_executable(test_rollback_manager
        test/test_rollback_manager.cpp
    )
    target_link_libraries(test_rollback_manager
        friendly_lwm2m
        GTest::gtest
        GTest::gtest_main
    )
    add_test(NAME RollbackManager COMMAND test_rollback_manager)

    add_executable(test_firmware_update_object
        test/test_firmware_update_object.cpp
    )
    target_link_libraries(test_firmware_update_object
        friendly_lwm2m
        GTest::gtest
        GTest::gtest_main
    )
    add_test(NAME FirmwareUpdateObject COMMAND test_firmware_update_object)

    # All tests target
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS 
            test_delta_algorithms 
            test_blockwise_transfer 
            test_rollback_manager
            test_firmware_update_object
    )

    # Coverage target
    if(ENABLE_COVERAGE)
        add_custom_target(coverage
            COMMAND lcov --capture --directory . --output-file coverage.info
            COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
            COMMAND genhtml coverage.info --output-directory coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endif()
endif()

# ============================================================================
# Examples
# ============================================================================

if(BUILD_EXAMPLES)
    add_executable(example_delta_update
        examples/delta_update_example.cpp
    )
    target_link_libraries(example_delta_update friendly_lwm2m)

    add_executable(example_firmware_client
        examples/firmware_client_example.cpp
    )
    target_link_libraries(example_firmware_client friendly_lwm2m)
endif()

# ============================================================================
# Installation
# ============================================================================

include(GNUInstallDirs)

install(TARGETS friendly_lwm2m
    EXPORT FriendlyLwM2MTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT FriendlyLwM2MTargets
    FILE FriendlyLwM2MTargets.cmake
    NAMESPACE FriendlyLwM2M::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FriendlyLwM2M
)

# Package configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    FriendlyLwM2MConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    cmake/FriendlyLwM2MConfig.cmake.in
    FriendlyLwM2MConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FriendlyLwM2M
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/FriendlyLwM2MConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/FriendlyLwM2MConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FriendlyLwM2M
)

# ============================================================================
# CPack Configuration
# ============================================================================

set(CPACK_PACKAGE_NAME "friendly-lwm2m-client")
set(CPACK_PACKAGE_VENDOR "Friendly Technologies")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

include(CPack)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "Friendly LwM2M Client Configuration:")
message(STATUS "  Version:          ${PROJECT_VERSION}")
message(STATUS "  Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard:     ${CMAKE_CXX_STANDARD}")
message(STATUS "")
message(STATUS "Features:")
message(STATUS "  DTLS:             ${WITH_DTLS}")
message(STATUS "  mbedTLS:          ${WITH_MBEDTLS}")
message(STATUS "  TinyDTLS:         ${WITH_TINYDTLS}")
message(STATUS "  Delta FOTA:       ${WITH_DELTA_FOTA}")
message(STATUS "  BSDIFF:           ${WITH_BSDIFF}")
message(STATUS "  VCDIFF:           ${WITH_VCDIFF}")
message(STATUS "  Courgette:        ${WITH_COURGETTE}")
message(STATUS "  Compression:      ${WITH_COMPRESSION}")
message(STATUS "  Rollback:         ${WITH_ROLLBACK}")
message(STATUS "")
message(STATUS "Build options:")
message(STATUS "  Tests:            ${BUILD_TESTS}")
message(STATUS "  Examples:         ${BUILD_EXAMPLES}")
message(STATUS "  Documentation:    ${BUILD_DOCS}")
message(STATUS "  Coverage:         ${ENABLE_COVERAGE}")
message(STATUS "  Sanitizers:       ${ENABLE_SANITIZERS}")
message(STATUS "")
message(STATUS "Optional libraries found:")
message(STATUS "  BZip2:            ${BZIP2_FOUND}")
message(STATUS "  LZMA:             ${LIBLZMA_FOUND}")
message(STATUS "  Zstd:             ${HAS_ZSTD}")
message(STATUS "  Brotli:           ${HAS_BROTLI}")
message(STATUS "")
