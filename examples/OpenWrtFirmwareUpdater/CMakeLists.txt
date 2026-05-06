# CMakeLists.txt for OpenWRT Firmware Updater
#
# This library provides OpenWRT-specific implementation of LWM2M
# Firmware Update Object (Object 5) with sysupgrade integration

cmake_minimum_required(VERSION 3.10)

project(OpenWrtFirmwareUpdater)

# Source files
set(SOURCES
    OpenWrtFwUpdater.cpp
    OpenWrtFwDownloader.cpp
    OpenWrtFwUriDownloader.cpp
    OpenWrtFwInit.cpp
)

# Create static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Include directories
# Note: CMAKE_SOURCE_DIR points to examples/, so we need to go up one level with ..
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/../wpp
    ${CMAKE_SOURCE_DIR}/../wpp/client
    ${CMAKE_SOURCE_DIR}/../wpp/registry
    ${CMAKE_SOURCE_DIR}/../wpp/registry/data_model
    ${CMAKE_SOURCE_DIR}/../wpp/registry/data_model/object
    ${CMAKE_SOURCE_DIR}/../wpp/registry/data_model/object/observer
    ${CMAKE_SOURCE_DIR}/../wpp/registry/data_model/instance
    ${CMAKE_SOURCE_DIR}/../wpp/registry/data_model/instance/observer
    ${CMAKE_SOURCE_DIR}/../wpp/registry/data_model/resource
    ${CMAKE_SOURCE_DIR}/../wpp/registry/data_model/types
    ${CMAKE_SOURCE_DIR}/../wpp/registry/objects
    ${CMAKE_SOURCE_DIR}/../wpp/registry/objects/o_5_firmware_update
    ${CMAKE_SOURCE_DIR}/../wpp/registry/objects/o_5_firmware_update/interfaces
    ${CMAKE_SOURCE_DIR}/../wpp/registry/objects/m_0_lwm2m_security
    ${CMAKE_SOURCE_DIR}/../wpp/registry/objects/m_3_device
    ${CMAKE_SOURCE_DIR}/../wpp/utils
    ${CMAKE_SOURCE_DIR}/../wpp/utils/bindings
    ${CMAKE_SOURCE_DIR}/../wpp/utils/task_queue
    ${CMAKE_SOURCE_DIR}/../wpp/platform
    ${CMAKE_SOURCE_DIR}/../wpp/platform/connection
    ${CMAKE_SOURCE_DIR}/../2305-Wakaama/include
)

# Link with pthread for thread support
target_link_libraries(${PROJECT_NAME} PUBLIC pthread)

# Add WPP definitions (includes LWM2M_CLIENT_MODE and other necessary defines)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${WPP_DEFINITIONS})

# Compiler options
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Enable exceptions for this library
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-fexceptions>
)

# Set position independent code for static library
set_target_properties(${PROJECT_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Installation
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES
    OpenWrtFwUpdater.h
    OpenWrtFwDownloader.h
    OpenWrtFwUriDownloader.h
    OpenWrtFwInit.h
    DESTINATION include/OpenWrtFirmwareUpdater
)

message(STATUS "OpenWrtFirmwareUpdater library configured")
