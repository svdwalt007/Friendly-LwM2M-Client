# ==============================================================================
# Zigbee Integration
# ==============================================================================

cmake_minimum_required(VERSION 3.16)

# Source files
set(ZIGBEE_SOURCES
    zigbee_coordinator.cpp
    zigbee_zcl_handlers.cpp
    zigbee_ota_proxy.cpp
)

# Header files
set(ZIGBEE_HEADERS
    zigbee_coordinator.h
    zigbee_zcl_handlers.h
    zigbee_ota_proxy.h
    zigbee_device_types.h
)

# Create zigbee library
add_library(zigbee STATIC ${ZIGBEE_SOURCES})

# Include directories
target_include_directories(zigbee
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link with system libraries
target_link_libraries(zigbee
    PUBLIC
        Threads::Threads
)

# ==============================================================================
# Optional: Silicon Labs EZSP Support
# ==============================================================================

option(WITH_EZSP_EXTENDED "Enable extended EZSP features" OFF)

if(WITH_EZSP_EXTENDED)
    message(STATUS "EZSP extended features enabled")
    target_compile_definitions(zigbee PUBLIC WITH_EZSP_EXTENDED=1)
endif()

# ==============================================================================
# Optional: Z-Stack Support
# ==============================================================================

option(WITH_ZSTACK_EXTENDED "Enable extended Z-Stack features" OFF)

if(WITH_ZSTACK_EXTENDED)
    message(STATUS "Z-Stack extended features enabled")
    target_compile_definitions(zigbee PUBLIC WITH_ZSTACK_EXTENDED=1)
endif()

# ==============================================================================
# Optional: Green Power Support
# ==============================================================================

option(WITH_GREEN_POWER "Enable Green Power support" OFF)

if(WITH_GREEN_POWER)
    message(STATUS "Green Power support enabled")
    target_compile_definitions(zigbee PUBLIC WITH_GREEN_POWER=1)
endif()

# ==============================================================================
# Optional: Touchlink Support
# ==============================================================================

option(WITH_TOUCHLINK "Enable Touchlink commissioning" ON)

if(WITH_TOUCHLINK)
    message(STATUS "Touchlink commissioning enabled")
    target_compile_definitions(zigbee PUBLIC WITH_TOUCHLINK=1)
endif()

# ==============================================================================
# Compiler Options
# ==============================================================================

# Enable C++17 for std::filesystem and other features
target_compile_features(zigbee PUBLIC cxx_std_17)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(zigbee PRIVATE
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
install(TARGETS zigbee
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install headers
install(FILES ${ZIGBEE_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zigbee
)

# ==============================================================================
# Configuration Notes
# ==============================================================================

message(STATUS "")
message(STATUS "Zigbee Integration Configuration:")
message(STATUS "  Library:          zigbee")
message(STATUS "  Sources:          ${ZIGBEE_SOURCES}")
message(STATUS "  Headers:          ${ZIGBEE_HEADERS}")
message(STATUS "  EZSP Extended:    ${WITH_EZSP_EXTENDED}")
message(STATUS "  Z-Stack Extended: ${WITH_ZSTACK_EXTENDED}")
message(STATUS "  Green Power:      ${WITH_GREEN_POWER}")
message(STATUS "  Touchlink:        ${WITH_TOUCHLINK}")
message(STATUS "")
message(STATUS "Supported Coordinator Types:")
message(STATUS "  - Silicon Labs EZSP (EFR32 series)")
message(STATUS "  - Texas Instruments Z-Stack (CC2531, CC2652)")
message(STATUS "  - Dresden Elektronik ConBee/RaspBee")
message(STATUS "  - ZiGate USB/WiFi")
message(STATUS "")
message(STATUS "Zigbee Features:")
message(STATUS "  - Network formation and management")
message(STATUS "  - Device discovery and interviewing")
message(STATUS "  - ZCL 8 cluster support")
message(STATUS "  - OTA firmware upgrade proxy")
message(STATUS "  - Group and scene management")
message(STATUS "  - Route and neighbor table management")
message(STATUS "")

# ==============================================================================
# Documentation
# ==============================================================================

# Generate coordinator setup guide
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
"# Zigbee Integration

Comprehensive Zigbee coordinator support for the Friendly LwM2M Client.

## Supported Hardware

### Silicon Labs EZSP (EmberZNet Serial Protocol)
- EFR32MG21, EFR32MG12, EFR32MG13 series
- Connect via serial (UART) at 115200 baud
- Firmware: EmberZNet 6.x or 7.x

### Texas Instruments Z-Stack
- CC2531 USB dongle
- CC2652R/P coordinator
- CC1352P multi-band coordinator
- Connect via serial at 115200 baud
- Firmware: Z-Stack 3.0 or later

### Dresden Elektronik
- ConBee II USB
- RaspBee II HAT
- Native serial protocol

### ZiGate
- ZiGate USB
- ZiGate WiFi
- ZiGate HAT

## Quick Start

### 1. Connect Coordinator

```bash
# Find coordinator device
ls /dev/ttyUSB* /dev/ttyACM*

# Verify permissions
sudo usermod -a -G dialout \$USER

# Test connection
screen /dev/ttyUSB0 115200
```

### 2. Configure LwM2M Object

Set Zigbee Coordinator object (33460) resources:
- Serial Port: /dev/ttyUSB0
- Baud Rate: 115200
- Coordinator Type: 1 (EZSP) or 2 (Z-Stack)
- Channel: 15 (recommended for 2.4GHz)
- PAN ID: 0x1A62

### 3. Form Network

Execute Form Network resource (ID 16) to create Zigbee network.

### 4. Join Devices

Execute Permit Join resource (ID 18) with duration (e.g., 60 seconds).
Put Zigbee devices into pairing mode.

## Channel Selection

Recommended channels to minimize Wi-Fi interference:
- Channel 15: Between Wi-Fi 1 and 6
- Channel 20: Between Wi-Fi 6 and 11
- Channel 25: Above Wi-Fi 11

## Troubleshooting

### Coordinator Not Responding
1. Check serial port permissions
2. Verify baud rate matches coordinator
3. Try different USB port
4. Check coordinator firmware version

### Devices Not Joining
1. Verify permit join is enabled
2. Check if device is in pairing mode
3. Ensure device is within range
4. Try factory reset on device

### Poor Network Performance
1. Check LQI values (should be >100)
2. Verify channel selection
3. Add router devices to extend mesh
4. Check for interference sources

## Advanced Features

### OTA Firmware Updates
Load OTA image files and announce to devices for wireless firmware updates.

### Green Power
Support for battery-free Green Power devices (switches, sensors).

### Touchlink
Commission Zigbee Light Link devices without traditional pairing.

### Group Control
Create groups for simultaneous control of multiple devices.

## API Reference

See ZIGBEE_INTEGRATION.md for complete API documentation.

## License

BSD 3-Clause License - See project LICENSE file.
"
)

message(STATUS "Created Zigbee integration README")
