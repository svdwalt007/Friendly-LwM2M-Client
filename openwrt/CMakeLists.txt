# CMakeLists.txt for OpenWRT integration components
cmake_minimum_required(VERSION 3.13)

project(OpenWRT_LwM2M_Integration)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenWRT libraries
find_library(UCI_LIBRARY uci)
find_library(UBOX_LIBRARY ubox)
find_library(UBUS_LIBRARY ubus)

if(NOT UCI_LIBRARY)
    message(WARNING "libuci not found - UCI support will be disabled")
endif()

if(NOT UBUS_LIBRARY)
    message(WARNING "libubus not found - ubus support will be disabled")
endif()

# Source files for OpenWRT integration
set(OPENWRT_SOURCES
    platform/OpenWrtConnection.cpp
    platform/OpenWrtPlatform.cpp
)

# Conditionally add UCI support
if(UCI_LIBRARY)
    list(APPEND OPENWRT_SOURCES uci/UciConfig.cpp)
    add_definitions(-DHAVE_UCI)
endif()

# Conditionally add ubus support
if(UBUS_LIBRARY)
    list(APPEND OPENWRT_SOURCES ubus/UbusInterface.cpp)
    add_definitions(-DHAVE_UBUS)
endif()

# Add sysfs monitor
list(APPEND OPENWRT_SOURCES sysfs/SysfsMonitor.cpp)

# Create OpenWRT integration library
add_library(WppOpenWRT STATIC ${OPENWRT_SOURCES})

# Include directories
target_include_directories(WppOpenWRT PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/uci
    ${CMAKE_CURRENT_SOURCE_DIR}/ubus
    ${CMAKE_CURRENT_SOURCE_DIR}/sysfs
    ${CMAKE_SOURCE_DIR}/wpp/platform
    ${CMAKE_SOURCE_DIR}/wpp/platform/connection
    ${CMAKE_SOURCE_DIR}/wpp/client
    ${CMAKE_SOURCE_DIR}/wpp/registry
)

# Link libraries
if(UCI_LIBRARY)
    target_link_libraries(WppOpenWRT PUBLIC ${UCI_LIBRARY})
endif()

if(UBUS_LIBRARY AND UBOX_LIBRARY)
    target_link_libraries(WppOpenWRT PUBLIC ${UBUS_LIBRARY} ${UBOX_LIBRARY})
endif()

# Install library and headers
install(TARGETS WppOpenWRT
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES
    platform/OpenWrtConnection.h
    platform/OpenWrtPlatform.h
    DESTINATION include/lwm2m/platform
)

if(UCI_LIBRARY)
    install(FILES uci/UciConfig.h DESTINATION include/lwm2m/uci)
endif()

if(UBUS_LIBRARY)
    install(FILES ubus/UbusInterface.h DESTINATION include/lwm2m/ubus)
endif()

install(FILES sysfs/SysfsMonitor.h DESTINATION include/lwm2m/sysfs)

# Build example if requested
option(BUILD_OPENWRT_EXAMPLE "Build OpenWRT integration example" ON)

if(BUILD_OPENWRT_EXAMPLE)
    add_subdirectory(example)
endif()
