# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.13)

# Project's name
project(WppClient)

# Option to build OpenWRT integration
option(OPENWRT_BUILD "Build OpenWRT integration components" OFF)

# Add subdirectory with docs target
add_subdirectory(doxg)
# Add subdirectory with wpp library target
add_subdirectory(wpp)
# Add subdirectory with examples targets
add_subdirectory(examples)
# Add subdirectory with Catch2 tests
add_subdirectory(tests)

# Add OpenWRT integration if enabled
if(OPENWRT_BUILD)
    message(STATUS "Building with OpenWRT integration support")
    add_subdirectory(openwrt)
endif()