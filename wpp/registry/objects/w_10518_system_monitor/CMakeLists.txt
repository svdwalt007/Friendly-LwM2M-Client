# SystemMonitor Object (10518)
# Walt Technologies

wpp_check_definition("OBJ_W_10518_SYSTEM_MONITOR" is_defined)
if(is_defined)
    # Update SOURCES variable from parent scope.
    set(WPP_SOURCES ${WPP_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/SystemMonitor.cpp)

    # Update WPP_SOURCES variable from parent scope.
    set(WPP_SOURCES ${WPP_SOURCES} PARENT_SCOPE)
endif()
