cmake_minimum_required(VERSION 3.16)

project(FriendlyLwM2MClient
    VERSION 1.1.0
    DESCRIPTION "Enhanced LwM2M Client with MQTT Transport, Edge AI, and Delta Firmware Updates"
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
option(WITH_MQTT "Enable MQTT transport binding" ON)
option(WITH_EDGE_AI "Enable Edge AI inference" ON)
option(WITH_TFLITE "Enable TensorFlow Lite backend" OFF)
option(WITH_ONNX "Enable ONNX Runtime backend" OFF)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_SANITIZERS "Enable address/undefined sanitizers" OFF)

# ============================================================================
# v1.2.0 New Feature Options
# ============================================================================

# OpenWRT Extended Integration
option(WITH_OPENWRT_EXTENDED "Enable extended OpenWRT integration" ON)
option(WITH_LUCI_MODULE "Build LuCI web interface module" ON)
option(WITH_HOTPLUG_HANDLERS "Install hotplug event handlers" ON)

# Router Management Objects
option(WITH_LAN_CONFIG "Enable LAN Configuration object (33611)" ON)
option(WITH_ROUTING_TABLE "Enable Routing Table object (33612)" ON)
option(WITH_VPN_CONFIG "Enable VPN Configuration object (33613)" ON)

# Multi-WAN and Failover
option(WITH_WAN_FAILOVER "Enable WAN Failover Policy object (33420)" ON)
option(WITH_MULTIWAN_HEALTH "Enable Multi-WAN Health Check object (33421)" ON)

# WiFi Management
option(WITH_WIFI_CLIENT_MGMT "Enable WiFi Client Management object (33422)" ON)
option(WITH_WIFI_CHANNEL_OPT "Enable WiFi Channel Optimization object (33423)" ON)

# Starlink Integration
option(WITH_STARLINK "Enable Starlink integration" ON)
option(WITH_STARLINK_GRPC "Enable Starlink gRPC support (requires gRPC)" OFF)

# prpl Integration
option(WITH_PRPL "Enable prplOS/prplMesh integration" OFF)
option(WITH_PRPL_EASYMESH "Enable EasyMesh R2/R4 support" OFF)
option(WITH_PRPL_AMBIORIX "Enable Ambiorix bus support" OFF)

# Matter Integration
option(WITH_MATTER "Enable Matter/Thread integration" OFF)
option(WITH_MATTER_BRIDGE "Enable Matter Bridge object (33450)" OFF)
option(WITH_THREAD_BR "Enable Thread Border Router support" OFF)

# Zigbee Integration
option(WITH_ZIGBEE "Enable Zigbee integration" OFF)
option(WITH_ZIGBEE_EZSP "Enable Silicon Labs EZSP support" OFF)
option(WITH_ZIGBEE_ZSTACK "Enable TI Z-Stack support" OFF)

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

# MQTT - Eclipse Paho
if(WITH_MQTT)
    find_library(PAHO_MQTT_C_LIB paho-mqtt3as)
    find_library(PAHO_MQTT_CPP_LIB paho-mqttpp3)
    find_path(PAHO_MQTT_INCLUDE mqtt/async_client.h)
endif()

# TensorFlow Lite
if(WITH_TFLITE)
    find_library(TFLITE_LIB tensorflowlite)
    find_path(TFLITE_INCLUDE tensorflow/lite/interpreter.h)
endif()

# ONNX Runtime
if(WITH_ONNX)
    find_library(ONNX_LIB onnxruntime)
    find_path(ONNX_INCLUDE onnxruntime_cxx_api.h)
endif()

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

# MQTT Transport sources (conditional)
if(WITH_MQTT)
    list(APPEND TRANSPORT_SOURCES
        src/transport/mqtt_transport.cpp
    )
endif()

set(OBJECTS_SOURCES
    src/objects/advanced_firmware_update_object.cpp
)

# MQTT Objects (conditional)
if(WITH_MQTT)
    list(APPEND OBJECTS_SOURCES
        src/objects/mqtt_server_object.cpp
        src/objects/lwm2m_cose_object.cpp
    )
endif()

# Edge AI Objects (conditional)
if(WITH_EDGE_AI)
    list(APPEND OBJECTS_SOURCES
        src/objects/edge_ai_inference_object.cpp
    )
endif()

set(ALL_SOURCES
    ${PLATFORM_SOURCES}
    ${FIRMWARE_SOURCES}
    ${TRANSPORT_SOURCES}
    ${OBJECTS_SOURCES}
)

# ============================================================================
# v1.2.0 Extended Source Files
# ============================================================================

# Starlink Integration
if(WITH_STARLINK)
    list(APPEND ALL_SOURCES
        src/starlink/starlink_grpc_client.cpp
        src/starlink/starlink_data_collector.cpp
    )
endif()

# prpl Integration
if(WITH_PRPL)
    list(APPEND ALL_SOURCES
        src/platform/prpl/prpl_platform.cpp
        src/platform/prpl/prpl_amxb_client.cpp
        src/platform/prpl/prpl_data_model.cpp
        src/platform/prpl/prpl_easymesh.cpp
        src/platform/prpl/prpl_mesh_object.cpp
    )
endif()

# Matter Integration
if(WITH_MATTER)
    list(APPEND ALL_SOURCES
        src/matter/matter_sdk_integration.cpp
        src/matter/matter_cluster_client.cpp
    )
endif()

# Zigbee Integration
if(WITH_ZIGBEE)
    list(APPEND ALL_SOURCES
        src/zigbee/zigbee_coordinator.cpp
        src/zigbee/zigbee_zcl_handlers.cpp
        src/zigbee/zigbee_ota_proxy.cpp
    )
endif()

# ============================================================================
# WPP Registry Objects (v1.2.0)
# ============================================================================

set(WPP_OBJECTS_SOURCES "")

# Multi-WAN Objects
if(WITH_WAN_FAILOVER)
    list(APPEND WPP_OBJECTS_SOURCES
        wpp/registry/objects/w_34611_wan_failover_policy/WanFailoverPolicy.cpp
    )
    add_definitions(-DOBJ_W_34611_WAN_FAILOVER_POLICY)
endif()

if(WITH_MULTIWAN_HEALTH)
    list(APPEND WPP_OBJECTS_SOURCES
        wpp/registry/objects/w_34612_multiwan_health_check/MultiWanHealthCheck.cpp
    )
    add_definitions(-DOBJ_W_34612_MULTIWAN_HEALTH_CHECK)
endif()

# WiFi Management Objects
if(WITH_WIFI_CLIENT_MGMT)
    list(APPEND WPP_OBJECTS_SOURCES
        wpp/registry/objects/w_34613_wifi_client_management/WifiClientManagement.cpp
    )
    add_definitions(-DOBJ_W_34613_WIFI_CLIENT_MANAGEMENT)
endif()

if(WITH_WIFI_CHANNEL_OPT)
    list(APPEND WPP_OBJECTS_SOURCES
        wpp/registry/objects/w_34614_wifi_channel_optimization/WifiChannelOptimization.cpp
    )
    add_definitions(-DOBJ_W_34614_WIFI_CHANNEL_OPTIMIZATION)
endif()

# Matter LwM2M Objects
if(WITH_MATTER_BRIDGE)
    list(APPEND WPP_OBJECTS_SOURCES
        wpp/registry/objects/w_34615_matter_bridge/MatterBridge.cpp
        wpp/registry/objects/w_34616_matter_device/MatterDevice.cpp
        wpp/registry/objects/w_34617_thread_network/ThreadNetwork.cpp
    )
    add_definitions(-DOBJ_W_34615_MATTER_BRIDGE)
    add_definitions(-DOBJ_W_34616_MATTER_DEVICE)
    add_definitions(-DOBJ_W_34617_THREAD_NETWORK)
endif()

# Zigbee LwM2M Objects
if(WITH_ZIGBEE)
    list(APPEND WPP_OBJECTS_SOURCES
        wpp/registry/objects/w_34618_zigbee_coordinator/ZigbeeCoordinator.cpp
        wpp/registry/objects/w_34619_zigbee_device/ZigbeeDevice.cpp
        wpp/registry/objects/w_34620_zigbee_group/ZigbeeGroup.cpp
    )
    add_definitions(-DOBJ_W_34618_ZIGBEE_COORDINATOR)
    add_definitions(-DOBJ_W_34619_ZIGBEE_DEVICE)
    add_definitions(-DOBJ_W_34620_ZIGBEE_GROUP)
endif()

# Router Management Objects
if(WITH_LAN_CONFIG)
    list(APPEND WPP_OBJECTS_SOURCES
        wpp/registry/objects/w_34621_lan_configuration/LanConfiguration.cpp
    )
    add_definitions(-DOBJ_W_34621_LAN_CONFIGURATION)
endif()

if(WITH_ROUTING_TABLE)
    list(APPEND WPP_OBJECTS_SOURCES
        wpp/registry/objects/w_34622_routing_table/RoutingTable.cpp
    )
    add_definitions(-DOBJ_W_34622_ROUTING_TABLE)
endif()

if(WITH_VPN_CONFIG)
    list(APPEND WPP_OBJECTS_SOURCES
        wpp/registry/objects/w_34623_vpn_configuration/VpnConfiguration.cpp
    )
    add_definitions(-DOBJ_W_34623_VPN_CONFIGURATION)
endif()

list(APPEND ALL_SOURCES ${WPP_OBJECTS_SOURCES})

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

# MQTT Paho libraries
if(WITH_MQTT AND PAHO_MQTT_CPP_LIB AND PAHO_MQTT_C_LIB)
    target_link_libraries(friendly_lwm2m PUBLIC
        ${PAHO_MQTT_CPP_LIB} ${PAHO_MQTT_C_LIB})
    target_include_directories(friendly_lwm2m PUBLIC ${PAHO_MQTT_INCLUDE})
    target_compile_definitions(friendly_lwm2m PUBLIC WITH_MQTT=1)
elseif(WITH_MQTT)
    # MQTT enabled but libraries not found - stub implementation
    target_compile_definitions(friendly_lwm2m PUBLIC WITH_MQTT=1 MQTT_STUB=1)
endif()

# TensorFlow Lite
if(WITH_TFLITE AND TFLITE_LIB AND TFLITE_INCLUDE)
    target_link_libraries(friendly_lwm2m PUBLIC ${TFLITE_LIB})
    target_include_directories(friendly_lwm2m PUBLIC ${TFLITE_INCLUDE})
    target_compile_definitions(friendly_lwm2m PUBLIC WITH_TFLITE=1)
endif()

# ONNX Runtime
if(WITH_ONNX AND ONNX_LIB AND ONNX_INCLUDE)
    target_link_libraries(friendly_lwm2m PUBLIC ${ONNX_LIB})
    target_include_directories(friendly_lwm2m PUBLIC ${ONNX_INCLUDE})
    target_compile_definitions(friendly_lwm2m PUBLIC WITH_ONNX=1)
endif()

# Edge AI compile definitions
if(WITH_EDGE_AI)
    target_compile_definitions(friendly_lwm2m PUBLIC WITH_EDGE_AI=1)
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
message(STATUS "Core Features:")
message(STATUS "  DTLS:             ${WITH_DTLS}")
message(STATUS "  mbedTLS:          ${WITH_MBEDTLS}")
message(STATUS "  TinyDTLS:         ${WITH_TINYDTLS}")
message(STATUS "  Delta FOTA:       ${WITH_DELTA_FOTA}")
message(STATUS "  BSDIFF:           ${WITH_BSDIFF}")
message(STATUS "  VCDIFF:           ${WITH_VCDIFF}")
message(STATUS "  Courgette:        ${WITH_COURGETTE}")
message(STATUS "  Compression:      ${WITH_COMPRESSION}")
message(STATUS "  Rollback:         ${WITH_ROLLBACK}")
message(STATUS "  MQTT Transport:   ${WITH_MQTT}")
message(STATUS "  Edge AI:          ${WITH_EDGE_AI}")
message(STATUS "  TensorFlow Lite:  ${WITH_TFLITE}")
message(STATUS "  ONNX Runtime:     ${WITH_ONNX}")
message(STATUS "")
message(STATUS "v1.2.0 Extended Features:")
message(STATUS "  OpenWRT Extended: ${WITH_OPENWRT_EXTENDED}")
message(STATUS "  LUCI Module:      ${WITH_LUCI_MODULE}")
message(STATUS "  Hotplug Handlers: ${WITH_HOTPLUG_HANDLERS}")
message(STATUS "  LAN Config:       ${WITH_LAN_CONFIG}")
message(STATUS "  Routing Table:    ${WITH_ROUTING_TABLE}")
message(STATUS "  VPN Config:       ${WITH_VPN_CONFIG}")
message(STATUS "  WAN Failover:     ${WITH_WAN_FAILOVER}")
message(STATUS "  Multi-WAN Health: ${WITH_MULTIWAN_HEALTH}")
message(STATUS "  WiFi Client Mgmt: ${WITH_WIFI_CLIENT_MGMT}")
message(STATUS "  WiFi Channel Opt: ${WITH_WIFI_CHANNEL_OPT}")
message(STATUS "  Starlink:         ${WITH_STARLINK}")
message(STATUS "  Starlink gRPC:    ${WITH_STARLINK_GRPC}")
message(STATUS "")
message(STATUS "Platform Integrations:")
message(STATUS "  prpl Integration: ${WITH_PRPL}")
message(STATUS "  prpl EasyMesh:    ${WITH_PRPL_EASYMESH}")
message(STATUS "  prpl Ambiorix:    ${WITH_PRPL_AMBIORIX}")
message(STATUS "  Matter/Thread:    ${WITH_MATTER}")
message(STATUS "  Matter Bridge:    ${WITH_MATTER_BRIDGE}")
message(STATUS "  Thread BR:        ${WITH_THREAD_BR}")
message(STATUS "  Zigbee:           ${WITH_ZIGBEE}")
message(STATUS "  Zigbee EZSP:      ${WITH_ZIGBEE_EZSP}")
message(STATUS "  Zigbee Z-Stack:   ${WITH_ZIGBEE_ZSTACK}")
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
