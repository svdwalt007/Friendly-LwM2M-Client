# Generated on: 2025-11-01 08:40:02
# Created by: Sinai RnD

wpp_check_definition("OBJ_O_34607_HARDWARE_WATCHDOG" is_defined)
if(is_defined)
	# Update SOURCES variable from parent scope.
	set(WPP_SOURCES ${WPP_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/HardwareWatchdog.cpp)

	# --------------- CMake block 0 start ---------------
	# --------------- CMake block 0 end ---------------

	# Update WPP_SOURCES variable from parent scope.
	set(WPP_SOURCES ${WPP_SOURCES} PARENT_SCOPE)

endif()