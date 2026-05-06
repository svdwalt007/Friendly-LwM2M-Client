# GpioControl Object (10515)
# Walt Technologies

wpp_check_definition("OBJ_W_10515_GPIO_CONTROL" is_defined)
if(is_defined)
    # Update SOURCES variable from parent scope.
    set(WPP_SOURCES ${WPP_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/GpioControl.cpp)

    # Update WPP_SOURCES variable from parent scope.
    set(WPP_SOURCES ${WPP_SOURCES} PARENT_SCOPE)
endif()
