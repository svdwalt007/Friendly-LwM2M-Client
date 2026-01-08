# Walt Technologies Starlink Terminal Object (ID: 10512)
# CMake configuration

if(DEFINED OBJ_O_10512_STARLINK_TERMINAL)
    message(STATUS "Building Starlink Terminal Object (ID: 10512)")

    # Add object source files
    set(WPP_SOURCES ${WPP_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/StarlinkTerminal.cpp
        PARENT_SCOPE
    )

    # Add include directory
    set(WPP_INCLUDE_DIRS ${WPP_INCLUDE_DIRS}
        ${CMAKE_CURRENT_LIST_DIR}
        PARENT_SCOPE
    )

    # Note: gRPC integration requires additional dependencies
    # Uncomment and configure when adding gRPC support:
    #
    # find_package(gRPC REQUIRED)
    # find_package(Protobuf REQUIRED)
    #
    # set(WPP_LINK_LIBRARIES ${WPP_LINK_LIBRARIES}
    #     gRPC::grpc++
    #     protobuf::libprotobuf
    #     PARENT_SCOPE
    # )

endif()
