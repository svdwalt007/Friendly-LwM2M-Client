#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "FriendlyLwM2M::friendly_lwm2m" for configuration "Release"
set_property(TARGET FriendlyLwM2M::friendly_lwm2m APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(FriendlyLwM2M::friendly_lwm2m PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfriendly_lwm2m.a"
  )

list(APPEND _cmake_import_check_targets FriendlyLwM2M::friendly_lwm2m )
list(APPEND _cmake_import_check_files_for_FriendlyLwM2M::friendly_lwm2m "${_IMPORT_PREFIX}/lib/libfriendly_lwm2m.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
