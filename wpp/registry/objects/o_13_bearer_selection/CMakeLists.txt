# Bearer Selection Object
# OMA LwM2M Object ID: 13

wpp_check_definition("OBJ_O_13_BEARER_SELECTION" is_defined)
if(is_defined)
	# Update SOURCES variable from parent scope.
	set(WPP_SOURCES ${WPP_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/BearerSelection.cpp)

	# Update WPP_SOURCES variable from parent scope.
	set(WPP_SOURCES ${WPP_SOURCES} PARENT_SCOPE)

endif()
