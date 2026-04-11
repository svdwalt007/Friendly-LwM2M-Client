
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was FriendlyLwM2MConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

# FriendlyLwM2M CMake Config File
# This file is used by CMake's find_package() to locate and configure FriendlyLwM2M

include(CMakeFindDependencyMacro)

# Find required dependencies
if(ON)
    if(ON)
        find_dependency(MbedTLS)
    else()
        find_dependency(OpenSSL)
    endif()
endif()

if(ON)
    find_dependency(Paho)
endif()

# Include the exported targets
include("${CMAKE_CURRENT_LIST_DIR}/FriendlyLwM2MTargets.cmake")

# Set variables for consumers
set(FriendlyLwM2M_FOUND TRUE)
set(FriendlyLwM2M_VERSION 1.2.2)
set(FriendlyLwM2M_VERSION_MAJOR 1)
set(FriendlyLwM2M_VERSION_MINOR 2)
set(FriendlyLwM2M_VERSION_PATCH 2)

# Feature flags
set(FriendlyLwM2M_WITH_DTLS ON)
set(FriendlyLwM2M_WITH_MBEDTLS ON)
set(FriendlyLwM2M_WITH_TINYDTLS OFF)
set(FriendlyLwM2M_WITH_DELTA_FOTA ON)
set(FriendlyLwM2M_WITH_MQTT ON)
set(FriendlyLwM2M_WITH_EDGE_AI ON)

# Component libraries
set(FriendlyLwM2M_LIBRARIES wpp wakaama)

if(ON)
    list(APPEND FriendlyLwM2M_LIBRARIES delta_fota)
endif()

if(ON)
    list(APPEND FriendlyLwM2M_LIBRARIES mqtt_transport)
endif()

if(ON)
    list(APPEND FriendlyLwM2M_LIBRARIES edge_ai)
endif()

# Include directories
set(FriendlyLwM2M_INCLUDE_DIRS
    ""
    "/wpp"
    "/wakaama"
)

check_required_components(FriendlyLwM2M)
