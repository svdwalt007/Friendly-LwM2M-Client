# ==============================================================================
# Matter SDK Integration
# ==============================================================================

cmake_minimum_required(VERSION 3.16)

# Source files
set(MATTER_SOURCES
    matter_sdk_integration.cpp
    matter_cluster_client.cpp
)

# Header files
set(MATTER_HEADERS
    matter_sdk_integration.h
    matter_cluster_client.h
    matter_device_types.h
)

# Create matter library
add_library(matter_integration STATIC ${MATTER_SOURCES})

# Include directories
target_include_directories(matter_integration
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link with system libraries
target_link_libraries(matter_integration
    PUBLIC
        Threads::Threads
)

# ==============================================================================
# Matter SDK Integration (Optional)
# ==============================================================================

# Option to enable real Matter SDK integration
option(WITH_MATTER_SDK "Enable Matter SDK integration" OFF)

if(WITH_MATTER_SDK)
    message(STATUS "Matter SDK integration enabled")

    # Try to find Matter SDK (CHIP)
    # The Matter SDK should be built separately and installed
    find_path(MATTER_SDK_INCLUDE_DIR
        NAMES controller/CHIPDeviceController.h
        PATHS
            /usr/local/include/matter
            /usr/include/matter
            ${MATTER_SDK_ROOT}/include
        DOC "Matter SDK include directory"
    )

    find_library(MATTER_SDK_CONTROLLER_LIB
        NAMES CHIPController chip_controller
        PATHS
            /usr/local/lib
            /usr/lib
            ${MATTER_SDK_ROOT}/lib
        DOC "Matter SDK Controller library"
    )

    if(MATTER_SDK_INCLUDE_DIR AND MATTER_SDK_CONTROLLER_LIB)
        message(STATUS "Matter SDK found")
        message(STATUS "  Include: ${MATTER_SDK_INCLUDE_DIR}")
        message(STATUS "  Library: ${MATTER_SDK_CONTROLLER_LIB}")

        # Add Matter SDK include directories
        target_include_directories(matter_integration
            PRIVATE
                ${MATTER_SDK_INCLUDE_DIR}
        )

        # Link with Matter SDK libraries
        target_link_libraries(matter_integration
            PRIVATE
                ${MATTER_SDK_CONTROLLER_LIB}
        )

        # Enable Matter SDK in code
        target_compile_definitions(matter_integration PUBLIC WITH_MATTER_SDK=1)

        # Matter SDK typically requires these additional libraries
        find_package(PkgConfig)
        if(PKG_CONFIG_FOUND)
            pkg_check_modules(GLIB QUIET glib-2.0)
            if(GLIB_FOUND)
                target_link_libraries(matter_integration PRIVATE ${GLIB_LIBRARIES})
                target_include_directories(matter_integration PRIVATE ${GLIB_INCLUDE_DIRS})
            endif()
        endif()

        # Additional Matter SDK dependencies
        target_link_libraries(matter_integration
            PRIVATE
                dl
                pthread
                m
        )
    else()
        message(WARNING "Matter SDK not found - Matter integration will use stub implementation")
        message(WARNING "To enable full Matter support:")
        message(WARNING "  1. Build and install Matter SDK (https://github.com/project-chip/connectedhomeip)")
        message(WARNING "  2. Set MATTER_SDK_ROOT to Matter SDK installation path")
        message(WARNING "  3. Rebuild with -DWITH_MATTER_SDK=ON -DMATTER_SDK_ROOT=/path/to/matter")
    endif()
else()
    message(STATUS "Matter SDK integration disabled (using stub implementation)")
    message(STATUS "To enable: cmake -DWITH_MATTER_SDK=ON")
endif()

# ==============================================================================
# Compiler Options
# ==============================================================================

# C++ standard
target_compile_features(matter_integration PUBLIC cxx_std_17)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(matter_integration PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
    )
endif()

# ==============================================================================
# Installation
# ==============================================================================

# Install library
install(TARGETS matter_integration
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install headers
install(FILES ${MATTER_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/matter
)

# ==============================================================================
# Documentation
# ==============================================================================

# Create README for Matter integration
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
"# Matter SDK Integration

This directory contains the Matter SDK integration for the Friendly LwM2M Client.

## Overview

The Matter integration provides:
- Matter controller functionality via LwM2M objects
- Device commissioning (QR code, manual pairing, BLE, WiFi, Thread)
- Cluster-based device interaction
- Thread Border Router support
- Comprehensive Matter device type definitions

## Components

### Core Integration
- `matter_sdk_integration.h/.cpp` - Main Matter controller implementation
- `matter_cluster_client.h/.cpp` - Cluster interaction layer
- `matter_device_types.h` - Matter device type definitions

### LwM2M Objects
- Object 33450 - Matter Bridge Controller
- Object 33451 - Matter Device Instance
- Object 33452 - Thread Network Configuration

## Building

### Stub Mode (Default)
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### With Matter SDK
1. Build and install Matter SDK:
   ```bash
   git clone https://github.com/project-chip/connectedhomeip.git
   cd connectedhomeip
   ./scripts/build/build_examples.py --target linux-x64-all-clusters
   ```

2. Build Friendly LwM2M Client with Matter:
   ```bash
   cmake -DWITH_MATTER_SDK=ON -DMATTER_SDK_ROOT=/path/to/connectedhomeip/out ..
   make
   ```

## Usage

### Initialize Matter Controller
```cpp
#include \"matter/matter_sdk_integration.h\"

auto& controller = matter::MatterController::getInstance();
controller.initialize(1, \"/var/lib/matter\", 0xFFF1, 0x8000);
```

### Commission a Device
```cpp
matter::CommissioningParams params;
controller.parseSetupPayload(\"MT:Y.K9042C00KA0648G00\", params);
controller.commissionDevice(params, [](auto status, auto message) {
    std::cout << \"Status: \" << message << std::endl;
});
```

### Interact with Device
```cpp
auto client = controller.getClusterClient(nodeId, 1);
if (client) {
    client->onOffTurnOn();

    uint8_t level;
    client->levelGetCurrentLevel(level);
}
```

### Enable Thread Border Router
```cpp
controller.enableThreadBorderRouter(
    \"matter-thread\",     // Network name
    \"1111111122222222\",  // Extended PAN ID
    \"00112233445566778899aabbccddeeff\", // Network key
    0x1234,               // PAN ID
    15                    // Channel
);
```

## Matter Device Types

The integration supports all standard Matter device types:

### Lighting
- On/Off Light (0x0100)
- Dimmable Light (0x0101)
- Color Temperature Light (0x010C)
- Extended Color Light (0x010D)

### Sensors
- Contact Sensor (0x0015)
- Occupancy Sensor (0x0107)
- Temperature Sensor (0x0302)
- Humidity Sensor (0x0307)

### Actuators
- Door Lock (0x000A)
- Window Covering (0x0202)
- Thermostat (0x0301)

And many more - see `matter_device_types.h` for complete list.

## Supported Clusters

- OnOff (0x0006)
- Level Control (0x0008)
- Color Control (0x0300)
- Temperature Measurement (0x0402)
- Occupancy Sensing (0x0406)
- Door Lock (0x0101)
- Window Covering (0x0102)
- Switch (0x003B)
- Basic Information (0x0028)
- Descriptor (0x001D)

## Thread Border Router

The integration includes Thread Border Router support for commissioning and
communicating with Thread-based Matter devices.

Requirements:
- Compatible hardware with Thread radio (e.g., nRF52840, EFR32)
- Matter SDK with Thread support
- OpenThread Border Router (OTBR) libraries

## Testing

### Test Matter Controller
```bash
# Initialize controller
lwm2m-client --enable-matter

# Commission device via LwM2M
# Write QR code to Object 33450, Resource 7 (Commission Device)

# Check commissioned devices
# Read Object 33450, Resource 1 (Commissioned Devices Count)
```

## References

- Matter Specification: https://csa-iot.org/developer-resource/specifications-download-request/
- Matter SDK: https://github.com/project-chip/connectedhomeip
- Thread Specification: https://www.threadgroup.org/support#specifications
- LwM2M Specification: https://www.openmobilealliance.org/release/LightweightM2M/

## License

See main project LICENSE file.
"
)

# ==============================================================================
# Summary
# ==============================================================================

message(STATUS "")
message(STATUS "Matter Integration:")
message(STATUS "  Library:          matter_integration")
message(STATUS "  Sources:          ${MATTER_SOURCES}")
message(STATUS "  Headers:          ${MATTER_HEADERS}")
message(STATUS "  Matter SDK:       ${WITH_MATTER_SDK}")
if(WITH_MATTER_SDK AND MATTER_SDK_INCLUDE_DIR)
    message(STATUS "  SDK Include:      ${MATTER_SDK_INCLUDE_DIR}")
    message(STATUS "  SDK Library:      ${MATTER_SDK_CONTROLLER_LIB}")
endif()
message(STATUS "")
