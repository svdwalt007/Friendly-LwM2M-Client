# RouterManagement Object (10513)
# Walt Technologies

wpp_check_definition("OBJ_W_10513_ROUTER_MANAGEMENT" is_defined)
if(is_defined)
    # Update SOURCES variable from parent scope.
    set(WPP_SOURCES ${WPP_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/RouterManagement.cpp)

    # Update WPP_SOURCES variable from parent scope.
    set(WPP_SOURCES ${WPP_SOURCES} PARENT_SCOPE)
endif()
