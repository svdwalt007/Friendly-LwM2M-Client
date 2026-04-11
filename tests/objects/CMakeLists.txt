# Test suite for LwM2M Objects
# Copyright (c) 2024-2026 Friendly Technologies

cmake_minimum_required(VERSION 3.15)

# Enable testing
enable_testing()

# Find Catch2 (use amalgamated version from tests/catch2)
set(CATCH2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/tests/catch2)

# Test executable for Advanced Firmware Update Object
add_executable(test_advanced_firmware_update
    test_advanced_firmware_update.cpp
    ${CMAKE_SOURCE_DIR}/tests/catch2/catch_amalgamated.cpp
)

target_include_directories(test_advanced_firmware_update PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CATCH2_INCLUDE_DIR}
)

target_link_libraries(test_advanced_firmware_update PRIVATE
    friendly_lwm2m
)

target_compile_definitions(test_advanced_firmware_update PRIVATE
    CATCH_CONFIG_FAST_COMPILE
)

add_test(NAME AdvancedFirmwareUpdateTests COMMAND test_advanced_firmware_update)

# Test executable for Edge AI Inference Object
add_executable(test_edge_ai_inference
    test_edge_ai_inference.cpp
    ${CMAKE_SOURCE_DIR}/tests/catch2/catch_amalgamated.cpp
)

target_include_directories(test_edge_ai_inference PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CATCH2_INCLUDE_DIR}
)

target_link_libraries(test_edge_ai_inference PRIVATE
    friendly_lwm2m
)

target_compile_definitions(test_edge_ai_inference PRIVATE
    CATCH_CONFIG_FAST_COMPILE
)

add_test(NAME EdgeAIInferenceTests COMMAND test_edge_ai_inference)

# Test executable for Walt Technologies Objects
add_executable(test_walt_tech_objects
    test_walt_tech_objects.cpp
    ${CMAKE_SOURCE_DIR}/tests/catch2/catch_amalgamated.cpp
)

target_include_directories(test_walt_tech_objects PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/wpp/registry
    ${CMAKE_SOURCE_DIR}/wpp/registry/objects
    ${CATCH2_INCLUDE_DIR}
)

target_link_libraries(test_walt_tech_objects PRIVATE
    friendly_lwm2m
)

# Add all Walt Tech object definitions
target_compile_definitions(test_walt_tech_objects PRIVATE
    CATCH_CONFIG_FAST_COMPILE
    OBJ_O_10512_STARLINK_TERMINAL
    OBJ_W_10513_ROUTER_MANAGEMENT
    OBJ_W_10514_ETHERNET_INTERFACE
    OBJ_W_10515_GPIO_CONTROL
    OBJ_W_10516_USB_MANAGEMENT
    OBJ_W_10517_STORAGE_MANAGEMENT
    OBJ_W_10518_SYSTEM_MONITOR
    OBJ_O_10519_HARDWARE_WATCHDOG
    OBJ_O_10520_MIKROBUS
    OBJ_W_10521_FIREWALL_CONFIG
    OBJ_W_10522_POE_MANAGEMENT
    OBJ_W_10525_WAN_FAILOVER_POLICY
    OBJ_W_10526_MULTIWAN_HEALTH_CHECK
    OBJ_W_10527_WIFI_CLIENT_MANAGEMENT
    OBJ_W_10528_WIFI_CHANNEL_OPTIMIZATION
    OBJ_W_10529_MATTER_BRIDGE
    OBJ_W_10530_MATTER_DEVICE
    OBJ_W_10531_THREAD_NETWORK
    OBJ_W_10532_ZIGBEE_COORDINATOR
    OBJ_W_10533_ZIGBEE_DEVICE
    OBJ_W_10534_ZIGBEE_GROUP
    OBJ_W_10535_LAN_CONFIGURATION
    OBJ_W_10536_ROUTING_TABLE
    OBJ_W_10537_VPN_CONFIGURATION
)

add_test(NAME WaltTechObjectsTests COMMAND test_walt_tech_objects)

# Coverage target (if gcov/lcov available)
if(CMAKE_BUILD_TYPE MATCHES Debug AND CMAKE_COMPILER_IS_GNUCXX)
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)

    if(LCOV AND GENHTML)
        add_custom_target(coverage
            COMMAND ${LCOV} --directory . --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${LCOV} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV} --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info.cleaned
            COMMAND ${GENHTML} coverage.info.cleaned --output-directory coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )
    endif()
endif()

# Valgrind target (if available)
find_program(VALGRIND valgrind)
if(VALGRIND)
    add_custom_target(memcheck
        COMMAND ${VALGRIND}
            --leak-check=full
            --show-leak-kinds=all
            --track-origins=yes
            --verbose
            --log-file=valgrind-out.txt
            $<TARGET_FILE:test_advanced_firmware_update>
        COMMAND ${VALGRIND}
            --leak-check=full
            --show-leak-kinds=all
            --track-origins=yes
            --verbose
            --log-file=valgrind-ai-out.txt
            $<TARGET_FILE:test_edge_ai_inference>
        DEPENDS test_advanced_firmware_update test_edge_ai_inference
        COMMENT "Running tests with Valgrind memory checker"
    )
endif()

# Print test configuration
message(STATUS "Object tests configured:")
message(STATUS "  - Advanced Firmware Update: test_advanced_firmware_update")
message(STATUS "  - Edge AI Inference: test_edge_ai_inference")
message(STATUS "  - Walt Technologies Objects: test_walt_tech_objects")
if(LCOV AND GENHTML)
    message(STATUS "  - Coverage report: make coverage")
endif()
if(VALGRIND)
    message(STATUS "  - Memory check: make memcheck")
endif()
