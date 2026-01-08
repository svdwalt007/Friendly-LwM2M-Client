# CMakeLists.txt for prpl Platform Abstraction Layer
#
# Copyright (c) 2024-2026 Friendly Technologies
# Licensed under the MIT License

cmake_minimum_required(VERSION 3.10)

# ============================================================================
# prpl Platform Library
# ============================================================================

# Source files
set(PRPL_PLATFORM_SOURCES
    prpl_platform.cpp
    prpl_amxb_client.cpp
    prpl_data_model.cpp
    prpl_easymesh.cpp
    prpl_mesh_object.cpp
)

# Header files (for IDEs)
set(PRPL_PLATFORM_HEADERS
    prpl_platform.h
    prpl_amxb_client.h
    prpl_data_model.h
    prpl_easymesh.h
    prpl_mesh_object.h
)

# ============================================================================
# Conditional Compilation Options
# ============================================================================

option(ENABLE_PRPL_PLATFORM "Enable prpl platform support" ON)
option(HAVE_AMBIORIX "Ambiorix library available" OFF)

# ============================================================================
# Dependencies
# ============================================================================

# Find Ambiorix libraries (optional)
if(HAVE_AMBIORIX)
    find_library(AMXB_LIB amxb)
    find_library(AMXC_LIB amxc)
    find_library(AMXD_LIB amxd)
    find_library(AMXP_LIB amxp)

    if(AMXB_LIB AND AMXC_LIB AND AMXD_LIB AND AMXP_LIB)
        message(STATUS "Ambiorix libraries found")
        set(AMBIORIX_LIBRARIES ${AMXB_LIB} ${AMXC_LIB} ${AMXD_LIB} ${AMXP_LIB})
        add_definitions(-DHAVE_AMBIORIX)
    else()
        message(WARNING "Ambiorix libraries not found. prpl platform will compile but functionality will be limited.")
        set(HAVE_AMBIORIX OFF)
    endif()
endif()

# Find OpenSSL (required for SHA256 hash)
find_package(OpenSSL REQUIRED)

# ============================================================================
# Create Library
# ============================================================================

if(ENABLE_PRPL_PLATFORM)
    add_library(prpl_platform STATIC
        ${PRPL_PLATFORM_SOURCES}
        ${PRPL_PLATFORM_HEADERS}
    )

    # Include directories
    target_include_directories(prpl_platform
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include/platform
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # Link libraries
    target_link_libraries(prpl_platform
        PUBLIC
            OpenSSL::SSL
            OpenSSL::Crypto
    )

    # Link Ambiorix if available
    if(HAVE_AMBIORIX)
        target_link_libraries(prpl_platform
            PRIVATE
                ${AMBIORIX_LIBRARIES}
        )
    endif()

    # Compiler options
    target_compile_features(prpl_platform PUBLIC cxx_std_17)

    target_compile_options(prpl_platform PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O2>
    )

    # Conditional compilation definitions
    if(HAVE_AMBIORIX)
        target_compile_definitions(prpl_platform PRIVATE HAVE_AMBIORIX)
    endif()

    # Install targets
    install(TARGETS prpl_platform
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )

    install(FILES ${PRPL_PLATFORM_HEADERS}
        DESTINATION include/platform/prpl
    )

    message(STATUS "prpl platform library configured successfully")
else()
    message(STATUS "prpl platform support disabled")
endif()

# ============================================================================
# Examples and Tests (optional)
# ============================================================================

option(BUILD_PRPL_EXAMPLES "Build prpl platform examples" OFF)
option(BUILD_PRPL_TESTS "Build prpl platform tests" OFF)

if(BUILD_PRPL_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_PRPL_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================================
# Documentation
# ============================================================================

# Add documentation files
install(FILES
    ${CMAKE_SOURCE_DIR}/docs/PRPL_INTEGRATION.md
    DESTINATION share/doc/friendly-lwm2m-client
    OPTIONAL
)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "prpl Platform Configuration Summary:")
message(STATUS "  Enabled:                ${ENABLE_PRPL_PLATFORM}")
message(STATUS "  Ambiorix Support:       ${HAVE_AMBIORIX}")
message(STATUS "  Build Examples:         ${BUILD_PRPL_EXAMPLES}")
message(STATUS "  Build Tests:            ${BUILD_PRPL_TESTS}")
message(STATUS "")
