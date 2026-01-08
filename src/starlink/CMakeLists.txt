# ==============================================================================
# Starlink gRPC Integration
# ==============================================================================

cmake_minimum_required(VERSION 3.16)

# Source files
set(STARLINK_SOURCES
    starlink_grpc_client.cpp
    starlink_data_collector.cpp
)

# Header files
set(STARLINK_HEADERS
    starlink_proto_types.h
    starlink_grpc_client.h
    starlink_data_collector.h
)

# Create starlink library
add_library(starlink_grpc STATIC ${STARLINK_SOURCES})

# Include directories
target_include_directories(starlink_grpc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link with system libraries
target_link_libraries(starlink_grpc
    PUBLIC
        Threads::Threads
)

# ==============================================================================
# gRPC Integration (Optional)
# ==============================================================================

# Option to enable real gRPC integration
option(WITH_STARLINK_GRPC "Enable Starlink gRPC integration" OFF)

if(WITH_STARLINK_GRPC)
    # Find gRPC and Protobuf
    find_package(Protobuf REQUIRED)
    find_package(gRPC CONFIG)

    if(NOT gRPC_FOUND)
        # Try to find gRPC with pkg-config
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GRPC REQUIRED grpc++)
        pkg_check_modules(GRPCPP REQUIRED grpc++_unsecure)
    endif()

    if(gRPC_FOUND OR GRPC_FOUND)
        message(STATUS "gRPC found - Starlink gRPC integration enabled")

        # Starlink protobuf definitions
        # Note: You need to obtain the actual .proto files from Starlink/SpaceX
        # or extract them from the dish protoset
        set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")

        if(EXISTS "${PROTO_DIR}/device.proto")
            # Generate protobuf and gRPC sources
            set(PROTO_FILES
                ${PROTO_DIR}/device.proto
            )

            protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

            # Add generated sources to library
            target_sources(starlink_grpc PRIVATE ${PROTO_SRCS})
            target_include_directories(starlink_grpc PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

            # Link with gRPC
            if(TARGET gRPC::grpc++)
                target_link_libraries(starlink_grpc PUBLIC gRPC::grpc++ gRPC::grpc)
            else()
                target_link_libraries(starlink_grpc PUBLIC ${GRPCPP_LIBRARIES})
                target_include_directories(starlink_grpc PUBLIC ${GRPCPP_INCLUDE_DIRS})
            endif()

            # Link with Protobuf
            target_link_libraries(starlink_grpc PUBLIC protobuf::libprotobuf)

            # Enable gRPC in code
            target_compile_definitions(starlink_grpc PUBLIC WITH_STARLINK_GRPC=1)
        else()
            message(WARNING "Starlink proto files not found at ${PROTO_DIR}")
            message(WARNING "To enable full gRPC support:")
            message(WARNING "  1. Extract .proto files from Starlink dish protoset")
            message(WARNING "  2. Place them in ${PROTO_DIR}/")
            message(WARNING "  3. Rebuild with -DWITH_STARLINK_GRPC=ON")
        endif()
    else()
        message(WARNING "gRPC not found - Starlink will use stub implementation")
    endif()
else()
    message(STATUS "Starlink gRPC integration disabled (using stub implementation)")
    message(STATUS "To enable: cmake -DWITH_STARLINK_GRPC=ON")
endif()

# ==============================================================================
# Installation
# ==============================================================================

# Install library
install(TARGETS starlink_grpc
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install headers
install(FILES ${STARLINK_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/starlink
)

# ==============================================================================
# Documentation
# ==============================================================================

# Create README for proto extraction
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/proto/README.md"
"# Starlink Protobuf Definitions

To enable full Starlink gRPC integration, you need to extract the protobuf
definitions from your Starlink dish.

## Extraction Methods

### Method 1: Using grpcurl (Recommended)

1. Install grpcurl:
   ```bash
   go install github.com/fullstorydev/grpcurl/cmd/grpcurl@latest
   ```

2. Extract protoset from dish:
   ```bash
   grpcurl -plaintext -protoset-out device.protoset 192.168.100.1:9200 describe
   ```

3. Convert protoset to .proto files:
   ```bash
   protoc --descriptor_set_in=device.protoset --decode_raw > device.proto
   ```

### Method 2: Using Starlink CLI

If you have the official Starlink CLI tool, you can extract the definitions
from its installation directory.

### Method 3: Community Resources

The Starlink community has reverse-engineered the protobuf definitions.
Search for \"starlink grpc proto\" for community resources.

## Required Proto Files

Place the following files in this directory:
- device.proto         (Main device service definition)
- common.proto         (Common message types)
- dish.proto          (Dish-specific messages)
- wifi.proto          (WiFi configuration messages)

## Building with gRPC

Once the proto files are in place:

```bash
cd /home/user/Friendly-LwM2M-Client
mkdir build && cd build
cmake -DWITH_STARLINK_GRPC=ON ..
make
```

## Testing Connection

Test your Starlink gRPC connection:

```bash
grpcurl -plaintext 192.168.100.1:9200 list
grpcurl -plaintext 192.168.100.1:9200 SpaceX.API.Device.Device/Handle
```

## References

- Starlink gRPC API Documentation: https://github.com/starlink-community
- gRPC Protocol: https://grpc.io
- Protocol Buffers: https://developers.google.com/protocol-buffers
"
)

# ==============================================================================
# Summary
# ==============================================================================

message(STATUS "")
message(STATUS "Starlink gRPC Integration:")
message(STATUS "  Library:          starlink_grpc")
message(STATUS "  Sources:          ${STARLINK_SOURCES}")
message(STATUS "  Headers:          ${STARLINK_HEADERS}")
message(STATUS "  gRPC Support:     ${WITH_STARLINK_GRPC}")
message(STATUS "")
