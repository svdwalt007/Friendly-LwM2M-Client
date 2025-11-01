# WLAN Connectivity Object
# OMA LwM2M Object ID: 12

wpp_check_definition("OBJ_O_12_WLAN_CONNECTIVITY" is_defined)
if(is_defined)
	# Update SOURCES variable from parent scope.
	set(WPP_SOURCES ${WPP_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/WlanConnectivity.cpp)

	# Update WPP_SOURCES variable from parent scope.
	set(WPP_SOURCES ${WPP_SOURCES} PARENT_SCOPE)

endif()
