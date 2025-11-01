# CMakeLists.txt for OpenWRT LwM2M Client Example
cmake_minimum_required(VERSION 3.13)

project(OpenWRT_LwM2M_Example)

# Find required libraries
find_library(UCI_LIBRARY uci)
find_library(UBOX_LIBRARY ubox)
find_library(UBUS_LIBRARY ubus)

# Source files
set(EXAMPLE_SOURCES
    main.cpp
)

# Create executable
add_executable(lwm2m-client ${EXAMPLE_SOURCES})

# Include directories
target_include_directories(lwm2m-client PRIVATE
    ${CMAKE_SOURCE_DIR}/wpp
    ${CMAKE_SOURCE_DIR}/wpp/client
    ${CMAKE_SOURCE_DIR}/wpp/platform
    ${CMAKE_SOURCE_DIR}/wpp/platform/connection
    ${CMAKE_SOURCE_DIR}/wpp/registry
    ${CMAKE_SOURCE_DIR}/wpp/registry/data_model/object
    ${CMAKE_SOURCE_DIR}/wpp/registry/data_model/instance
    ${CMAKE_SOURCE_DIR}/wpp/registry/objects/m_0_lwm2m_security
    ${CMAKE_SOURCE_DIR}/wpp/registry/objects/m_1_lwm2m_server
    ${CMAKE_SOURCE_DIR}/wpp/registry/objects/m_3_device
    ${CMAKE_SOURCE_DIR}/wpp/registry/objects/o_4_connectivity_monitoring
    ${CMAKE_CURRENT_SOURCE_DIR}/../platform
    ${CMAKE_CURRENT_SOURCE_DIR}/../uci
    ${CMAKE_CURRENT_SOURCE_DIR}/../ubus
    ${CMAKE_CURRENT_SOURCE_DIR}/../sysfs
)

# Link libraries
target_link_libraries(lwm2m-client
    WppOpenWRT
    Wpp
    ${UCI_LIBRARY}
    ${UBUS_LIBRARY}
    ${UBOX_LIBRARY}
    pthread
)

# Install executable
install(TARGETS lwm2m-client
    RUNTIME DESTINATION bin
)
