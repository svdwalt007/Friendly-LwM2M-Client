# FindMbedTLS.cmake
#
# Find mbedTLS library (version 3.0+)
#
# This module defines:
#  MbedTLS_FOUND - System has mbedTLS
#  MbedTLS_INCLUDE_DIRS - The mbedTLS include directories
#  MbedTLS_LIBRARIES - The libraries needed to use mbedTLS
#  MbedTLS_VERSION - The version of mbedTLS found
#  MbedTLS::mbedtls - Imported target for mbedtls
#  MbedTLS::mbedx509 - Imported target for mbedx509
#  MbedTLS::mbedcrypto - Imported target for mbedcrypto

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_MBEDTLS QUIET mbedtls)
endif()

# Find the include directory
find_path(MbedTLS_INCLUDE_DIR
    NAMES mbedtls/ssl.h
    HINTS ${PC_MBEDTLS_INCLUDE_DIRS}
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
)

# Find the mbedtls library
find_library(MbedTLS_LIBRARY
    NAMES mbedtls
    HINTS ${PC_MBEDTLS_LIBRARY_DIRS}
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
)

# Find the mbedx509 library
find_library(MbedX509_LIBRARY
    NAMES mbedx509
    HINTS ${PC_MBEDTLS_LIBRARY_DIRS}
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
)

# Find the mbedcrypto library
find_library(MbedCrypto_LIBRARY
    NAMES mbedcrypto
    HINTS ${PC_MBEDTLS_LIBRARY_DIRS}
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
)

# Extract version if available
if(MbedTLS_INCLUDE_DIR AND EXISTS "${MbedTLS_INCLUDE_DIR}/mbedtls/version.h")
    file(STRINGS "${MbedTLS_INCLUDE_DIR}/mbedtls/version.h" MbedTLS_VERSION_LINE
         REGEX "^#define[ \t]+MBEDTLS_VERSION_STRING[ \t]+\"[^\"]+\"")
    if(MbedTLS_VERSION_LINE)
        string(REGEX REPLACE "^#define[ \t]+MBEDTLS_VERSION_STRING[ \t]+\"([^\"]+)\".*" "\\1"
               MbedTLS_VERSION "${MbedTLS_VERSION_LINE}")
    endif()
endif()

# Handle standard arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MbedTLS
    REQUIRED_VARS MbedTLS_LIBRARY MbedX509_LIBRARY MbedCrypto_LIBRARY MbedTLS_INCLUDE_DIR
    VERSION_VAR MbedTLS_VERSION
)

if(MbedTLS_FOUND)
    set(MbedTLS_INCLUDE_DIRS ${MbedTLS_INCLUDE_DIR})
    set(MbedTLS_LIBRARIES ${MbedTLS_LIBRARY} ${MbedX509_LIBRARY} ${MbedCrypto_LIBRARY})

    # Create imported targets
    if(NOT TARGET MbedTLS::mbedtls)
        add_library(MbedTLS::mbedtls UNKNOWN IMPORTED)
        set_target_properties(MbedTLS::mbedtls PROPERTIES
            IMPORTED_LOCATION "${MbedTLS_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${MbedTLS_INCLUDE_DIR}"
        )
    endif()

    if(NOT TARGET MbedTLS::mbedx509)
        add_library(MbedTLS::mbedx509 UNKNOWN IMPORTED)
        set_target_properties(MbedTLS::mbedx509 PROPERTIES
            IMPORTED_LOCATION "${MbedX509_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${MbedTLS_INCLUDE_DIR}"
        )
    endif()

    if(NOT TARGET MbedTLS::mbedcrypto)
        add_library(MbedTLS::mbedcrypto UNKNOWN IMPORTED)
        set_target_properties(MbedTLS::mbedcrypto PROPERTIES
            IMPORTED_LOCATION "${MbedCrypto_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${MbedTLS_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(
    MbedTLS_INCLUDE_DIR
    MbedTLS_LIBRARY
    MbedX509_LIBRARY
    MbedCrypto_LIBRARY
)
