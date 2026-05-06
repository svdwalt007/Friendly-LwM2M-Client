# EthernetInterface Object (10514)
# Walt Technologies

wpp_check_definition("OBJ_W_10514_ETHERNET_INTERFACE" is_defined)
if(is_defined)
    # Update SOURCES variable from parent scope.
    set(WPP_SOURCES ${WPP_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/EthernetInterface.cpp)

    # Update WPP_SOURCES variable from parent scope.
    set(WPP_SOURCES ${WPP_SOURCES} PARENT_SCOPE)
endif()
