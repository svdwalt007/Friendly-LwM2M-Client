# Zigbee Integration for Friendly LwM2M Client

## Table of Contents
- [Overview](#overview)
- [Architecture](#architecture)
- [Supported Hardware](#supported-hardware)
- [LwM2M Objects](#lwm2m-objects)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage Examples](#usage-examples)
- [ZCL Cluster Support](#zcl-cluster-support)
- [OTA Firmware Updates](#ota-firmware-updates)
- [Troubleshooting](#troubleshooting)
- [API Reference](#api-reference)

## Overview

The Zigbee Integration provides comprehensive Zigbee coordinator functionality for the Friendly LwM2M Client, enabling centralized management and control of Zigbee mesh networks through the LwM2M protocol. This integration supports multiple coordinator types, ZCL 8 compliance, OTA upgrades, and advanced features like Green Power and Touchlink commissioning.

### Key Features

- **Multiple Coordinator Support**: EZSP (Silicon Labs), Z-Stack (Texas Instruments), ConBee, ZiGate
- **ZCL 8 Compliance**: Full support for Zigbee Cluster Library 8
- **Network Management**: Formation, joining control, topology management
- **Device Discovery**: Automatic interviewing and capability detection
- **OTA Proxy**: Over-the-air firmware upgrades for Zigbee devices
- **Group Control**: Simultaneous control of multiple devices
- **Scene Management**: Store and recall device states
- **Green Power**: Support for battery-free devices
- **Touchlink**: ZLL commissioning for compatible devices

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    LwM2M Server                             │
└─────────────────────────┬───────────────────────────────────┘
                          │ LwM2M/CoAP
┌─────────────────────────┴───────────────────────────────────┐
│              Friendly LwM2M Client                          │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  LwM2M Objects (10532, 10533, 10534)                  │  │
│  └─────────────────────┬─────────────────────────────────┘  │
│  ┌─────────────────────┴─────────────────────────────────┐  │
│  │  Zigbee Coordinator (C++ Library)                     │  │
│  │  - Network Management                                 │  │
│  │  - Device Discovery                                   │  │
│  │  - ZCL Handlers                                       │  │
│  │  - OTA Proxy                                          │  │
│  └─────────────────────┬─────────────────────────────────┘  │
└────────────────────────┴─────────────────────────────────────┘
                         │ Serial (UART/USB)
┌────────────────────────┴─────────────────────────────────────┐
│  Zigbee Coordinator Hardware (EZSP/Z-Stack/ConBee/ZiGate)   │
└─────────────────────────┬───────────────────────────────────┘
                          │ Zigbee 802.15.4
           ┌──────────────┼──────────────┐
           │              │              │
      ┌────┴────┐    ┌────┴────┐   ┌────┴────┐
      │  Light  │    │  Sensor │   │ Switch  │
      └─────────┘    └─────────┘   └─────────┘
      Zigbee Devices (End Devices/Routers)
```

## Supported Hardware

### Silicon Labs EZSP (EmberZNet Serial Protocol)

**Compatible Modules:**
- EFR32MG21 series (Zigbee 3.0)
- EFR32MG12 series
- EFR32MG13 series
- EM3581, EM3582, EM3585, EM3586, EM3587, EM3588

**Configuration:**
- Serial Protocol: EZSP
- Baud Rate: 115200 (typical)
- Serial Parameters: 8N1 (8 data bits, no parity, 1 stop bit)
- Flow Control: None or hardware (RTS/CTS)

**Firmware Requirements:**
- EmberZNet 6.x or later
- EZSP Protocol Version 8 or later recommended

### Texas Instruments Z-Stack

**Compatible Modules:**
- CC2531 USB dongle
- CC2652R/P coordinator
- CC2652RB coordinator
- CC1352P multi-band coordinator

**Configuration:**
- Serial Protocol: Z-Stack UART API
- Baud Rate: 115200
- Serial Parameters: 8N1
- Flow Control: None

**Firmware Requirements:**
- Z-Stack 3.0 or later
- Z-Stack 3.30+ recommended for Zigbee 3.0

### Dresden Elektronik

**Compatible Modules:**
- ConBee II USB
- RaspBee II HAT for Raspberry Pi

**Configuration:**
- Serial Protocol: Native ConBee protocol
- Baud Rate: 38400 (ConBee II)
- Auto-detection supported

### ZiGate

**Compatible Modules:**
- ZiGate USB
- ZiGate WiFi
- ZiGate HAT for Raspberry Pi

**Configuration:**
- Serial Protocol: ZiGate protocol
- Baud Rate: 115200
- Both USB and WiFi variants supported

## LwM2M Objects

### Object 10532: Zigbee Coordinator

Manages the Zigbee coordinator and network.

**Resources:**
- **0**: Network State (R, Integer) - 0=Offline, 1=Forming, 2=Joined, 3=Ready, 4=Leaving, 5=Error
- **1**: PAN ID (RW, Integer) - 16-bit network identifier
- **2**: Extended PAN ID (RW, String) - 64-bit extended network ID
- **3**: Channel (RW, Integer) - Radio channel 11-26
- **4**: Network Key (W, Opaque) - 128-bit encryption key
- **5**: Permit Join Status (R, Boolean) - Join permission state
- **6**: Device Count (R, Integer) - Number of joined devices
- **7**: Coordinator IEEE Address (R, String) - Coordinator MAC address
- **8**: Coordinator Type (R, Integer) - 1=EZSP, 2=Z-Stack, 3=ConBee, 4=ZiGate
- **9**: Firmware Version (R, String) - Coordinator firmware version
- **10**: TX Power (RW, Integer) - Transmit power in dBm
- **11**: Security Level (RW, Integer) - Security level 0-7
- **12**: Serial Port (RW, String) - Serial device path
- **13**: Baud Rate (RW, Integer) - Serial baud rate
- **14**: Route Table Size (R, Integer) - Route entries count
- **15**: Neighbor Table Size (R, Integer) - Neighbor entries count
- **16**: Form Network (E, Execute) - Form new network
- **17**: Leave Network (E, Execute) - Leave current network
- **18**: Permit Join Command (E, Execute) - Enable/disable joining
- **19**: Start Touchlink (E, Execute) - Initiate Touchlink
- **20**: Backup Network (E, Execute) - Backup network configuration

### Object 10533: Zigbee Device

Represents individual Zigbee devices in the network. Multiple instances.

**Resources:**
- **0**: IEEE Address (R, String) - 64-bit device address
- **1**: Network Address (R, Integer) - 16-bit network address
- **2**: Device Type (R, Integer) - 1=Coordinator, 2=Router, 3=End Device, 4=Sleepy End Device
- **3**: Manufacturer (R, String) - Manufacturer name
- **4**: Model (R, String) - Model identifier
- **5**: Firmware Version (R, String) - Device firmware version
- **6**: Power Source (R, Integer) - 1=Mains, 2=Battery, 3=DC
- **7**: LQI (R, Integer) - Link Quality Indicator 0-255
- **8**: RSSI (R, Integer) - Signal strength in dBm
- **9**: Last Seen (R, Time) - Last communication timestamp
- **10**: Endpoints (R, Multiple, Integer) - Active endpoint list
- **11**: Clusters (R, String) - Supported clusters JSON
- **12**: Interviewed (R, Boolean) - Interview complete flag
- **13**: Online (R, Boolean) - Device online status
- **14**: Profile ID (R, Integer) - Zigbee profile ID
- **15**: Device ID (R, Integer) - Zigbee device ID
- **16**: Remove (E, Execute) - Remove device from network
- **17**: Interview (E, Execute) - Start device interview
- **18**: Ping (E, Execute) - Ping device
- **19**: Read Attribute (E, Execute) - Read ZCL attribute
- **20**: Write Attribute (E, Execute) - Write ZCL attribute

### Object 10534: Zigbee Group

Manages Zigbee groups for collective device control. Multiple instances.

**Resources:**
- **0**: Group ID (RW, Integer) - 16-bit group identifier
- **1**: Group Name (RW, String) - Human-readable name
- **2**: Member Devices (R, Multiple, String) - Member IEEE addresses
- **3**: Member Count (R, Integer) - Number of members
- **4**: Scene Count (R, Integer) - Number of scenes
- **5**: Current Scene (RW, Integer) - Active scene ID
- **6**: Add Member (E, Execute) - Add device to group
- **7**: Remove Member (E, Execute) - Remove device from group
- **8**: Scene Recall (E, Execute) - Recall stored scene
- **9**: Scene Store (E, Execute) - Store current state as scene
- **10**: On (E, Execute) - Turn on all devices
- **11**: Off (E, Execute) - Turn off all devices
- **12**: Toggle (E, Execute) - Toggle all devices
- **13**: Set Level (E, Execute) - Set brightness level
- **14**: Set Color (E, Execute) - Set color (hue/saturation)

## Installation

### Prerequisites

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake git
sudo apt-get install -y libpthread-stubs0-dev

# Add user to dialout group for serial access
sudo usermod -a -G dialout $USER
# Logout and login for group change to take effect
```

### Building

```bash
cd /home/user/Friendly-LwM2M-Client
mkdir build && cd build

# Basic build
cmake ..

# Build with optional features
cmake -DWITH_EZSP_EXTENDED=ON \
      -DWITH_ZSTACK_EXTENDED=ON \
      -DWITH_GREEN_POWER=ON \
      -DWITH_TOUCHLINK=ON \
      ..

make zigbee
```

### Build Options

- `WITH_EZSP_EXTENDED`: Enable extended EZSP features (default: OFF)
- `WITH_ZSTACK_EXTENDED`: Enable extended Z-Stack features (default: OFF)
- `WITH_GREEN_POWER`: Enable Green Power support (default: OFF)
- `WITH_TOUCHLINK`: Enable Touchlink commissioning (default: ON)

## Configuration

### 1. Connect Coordinator

```bash
# Find coordinator device
ls /dev/ttyUSB* /dev/ttyACM*

# Test connection (press Ctrl+A, K to exit)
screen /dev/ttyUSB0 115200

# Set persistent device name (optional)
sudo nano /etc/udev/rules.d/99-zigbee.rules
# Add: SUBSYSTEM=="tty", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", SYMLINK+="zigbee"
sudo udevadm control --reload-rules
```

### 2. Configure Coordinator Object

Via LwM2M Server, set object 10532 resources:

```json
{
  "serial_port": "/dev/ttyUSB0",
  "baud_rate": 115200,
  "coordinator_type": 1,
  "channel": 15,
  "pan_id": 6818,
  "extended_pan_id": "DD2211AA44556677",
  "tx_power": 20,
  "security_level": 5
}
```

### 3. Form Network

Execute resource 16 (Form Network) on object 10532.

Monitor resource 0 (Network State):
- 1 = Forming (in progress)
- 3 = Ready (success)
- 5 = Error (failed)

### 4. Permit Joining

Execute resource 18 (Permit Join Command) with argument:
- Duration in seconds (0-254)
- Example: 60 seconds for controlled joining

## Usage Examples

### Example 1: Form Network and Add Devices

```python
# Python pseudocode using LwM2M client library

# Configure coordinator
lwm2m.write("/10532/0/12", "/dev/ttyUSB0")  # Serial port
lwm2m.write("/10532/0/13", 115200)          # Baud rate
lwm2m.write("/10532/0/3", 15)               # Channel
lwm2m.write("/10532/0/1", 0x1A62)           # PAN ID

# Form network
lwm2m.execute("/10532/0/16")

# Wait for network ready
while lwm2m.read("/10532/0/0") != 3:
    time.sleep(1)

# Permit joining for 60 seconds
lwm2m.execute("/10532/0/18", bytes([60]))

print("Network ready - add devices now")
```

### Example 2: Monitor Devices

```python
# Get device count
device_count = lwm2m.read("/10532/0/6")
print(f"Devices in network: {device_count}")

# Enumerate devices
for inst_id in range(device_count):
    ieee = lwm2m.read(f"/10533/{inst_id}/0")
    model = lwm2m.read(f"/10533/{inst_id}/4")
    lqi = lwm2m.read(f"/10533/{inst_id}/7")
    online = lwm2m.read(f"/10533/{inst_id}/13")

    print(f"Device {ieee}: {model}, LQI={lqi}, Online={online}")
```

### Example 3: Create Group and Control Lights

```python
# Create group
group_inst = lwm2m.create("/10534")
lwm2m.write(f"/10534/{group_inst}/0", 0x0001)  # Group ID
lwm2m.write(f"/10534/{group_inst}/1", "Living Room Lights")

# Add devices to group
device1_ieee = 0x00124B0012345678
device2_ieee = 0x00124B00ABCDEF12
lwm2m.execute(f"/10534/{group_inst}/6", device1_ieee.to_bytes(8, 'little'))
lwm2m.execute(f"/10534/{group_inst}/6", device2_ieee.to_bytes(8, 'little'))

# Turn on group
lwm2m.execute(f"/10534/{group_inst}/10")

# Set brightness to 50%
lwm2m.execute(f"/10534/{group_inst}/13", bytes([127]))

# Store as scene 1
lwm2m.execute(f"/10534/{group_inst}/9", bytes([1]))

# Later: recall scene 1
lwm2m.execute(f"/10534/{group_inst}/8", bytes([1]))
```

## ZCL Cluster Support

### Supported Clusters

**General Clusters:**
- 0x0000: Basic
- 0x0001: Power Configuration
- 0x0003: Identify
- 0x0004: Groups
- 0x0005: Scenes

**Lighting Clusters:**
- 0x0006: On/Off
- 0x0008: Level Control
- 0x0300: Color Control

**Measurement Clusters:**
- 0x0400: Illuminance Measurement
- 0x0402: Temperature Measurement
- 0x0403: Pressure Measurement
- 0x0405: Relative Humidity Measurement
- 0x0406: Occupancy Sensing

**Security Clusters:**
- 0x0500: IAS Zone
- 0x0501: IAS ACE

**Smart Energy Clusters:**
- 0x0702: Metering (Simple Metering)
- 0x0B04: Electrical Measurement

**Other Clusters:**
- 0x0019: OTA Upgrade
- 0x0101: Door Lock
- 0x0102: Window Covering
- 0x0201: Thermostat

### Cluster Handler Examples

```cpp
// Example: Control On/Off cluster
OnOffClusterHandler* onOffHandler = handlerManager.getHandler(0x0006);
onOffHandler->turnOn(deviceIeee, endpoint);
onOffHandler->turnOff(deviceIeee, endpoint);

// Example: Control Level cluster
LevelControlClusterHandler* levelHandler = handlerManager.getHandler(0x0008);
levelHandler->setLevel(deviceIeee, endpoint, 128, 10);  // 50% brightness, 1 second transition

// Example: Control Color cluster
ColorControlClusterHandler* colorHandler = handlerManager.getHandler(0x0300);
colorHandler->setColorHueSaturation(deviceIeee, endpoint, 128, 254);  // Green, full saturation
```

## OTA Firmware Updates

### OTA Image Format

Zigbee OTA images follow the OTA Upgrade Cluster specification:

**Header Structure:**
- Magic Number: 0x0BEEF11E
- Header Version: 0x0100
- Manufacturer Code: 16-bit
- Image Type: 16-bit
- File Version: 32-bit
- Stack Version: 16-bit
- Header String: 32 bytes
- Total Image Size: 32-bit

### Using OTA Proxy

```cpp
// Initialize OTA proxy
ZigbeeOtaProxy otaProxy(coordinator);

// Load OTA image
otaProxy.loadImage("/path/to/firmware.zigbee");

// Register callbacks
otaProxy.onProgress([](uint64_t ieee, uint32_t offset, uint32_t total) {
    int percent = (offset * 100) / total;
    std::cout << "Device " << std::hex << ieee
              << " progress: " << percent << "%" << std::endl;
});

otaProxy.onComplete([](uint64_t ieee, bool success) {
    std::cout << "Device " << std::hex << ieee
              << " upgrade " << (success ? "successful" : "failed") << std::endl;
});

// Announce image to specific device
otaProxy.announceImageToDevice(deviceIeee, endpoint);

// Or announce to all devices
otaProxy.announceImage();
```

### OTA Best Practices

1. **Test Updates**: Always test firmware on a single device first
2. **Backup**: Backup network configuration before updates
3. **Power**: Ensure devices have adequate power (mains or full battery)
4. **Timing**: Schedule updates during maintenance windows
5. **Monitoring**: Monitor progress and handle failures
6. **Verification**: Verify firmware version after update

## Troubleshooting

### Coordinator Issues

**Problem: Coordinator not responding**
```bash
# Check device exists
ls -l /dev/ttyUSB0

# Check permissions
sudo chmod 666 /dev/ttyUSB0

# Test with minicom/screen
screen /dev/ttyUSB0 115200

# Check dmesg for errors
dmesg | tail -20
```

**Problem: Network formation fails**
- Verify channel is clear (use Zigbee sniffer)
- Check for conflicting networks (change PAN ID)
- Ensure coordinator firmware is up to date
- Try different channel (11, 15, 20, 25)

### Device Issues

**Problem: Devices not joining**
- Verify permit join is enabled
- Check device is in pairing mode
- Ensure device is within range (< 10m for initial pairing)
- Try factory reset on device
- Check coordinator has capacity (max devices limit)

**Problem: Poor communication quality**
- Check LQI values (resource 7, should be > 100)
- Verify RSSI (resource 8, should be > -80 dBm)
- Add router devices to extend mesh
- Check for interference (Wi-Fi, microwave, etc.)
- Relocate coordinator to central location

**Problem: Device offline**
- Check Last Seen timestamp (resource 9)
- Execute Ping (resource 18) to test connectivity
- Verify device power source
- Check route table for path to device
- Re-interview device if necessary

### Network Issues

**Problem: High latency**
- Check network size (>50 devices may need optimization)
- Verify route table health
- Add more routers to improve mesh
- Check for broadcast storms
- Optimize polling intervals

**Problem: Devices dropping**
- Monitor neighbor table stability
- Check for power issues
- Verify channel quality
- Review error logs
- Consider network rejoin strategy

## API Reference

### C++ API

#### ZigbeeCoordinator Class

```cpp
class ZigbeeCoordinator {
public:
    // Initialization
    bool initialize();
    void shutdown();

    // Network management
    bool formNetwork(const ZigbeeNetworkParams& params);
    bool joinNetwork(const ZigbeeNetworkParams& params);
    bool leaveNetwork();
    bool permitJoin(uint16_t duration);

    // Device management
    bool removeDevice(uint64_t ieeeAddress);
    bool interviewDevice(uint64_t ieeeAddress);

    // ZCL commands
    bool sendZclCommand(uint64_t ieeeAddress, uint8_t endpoint,
                       uint16_t clusterId, uint8_t commandId,
                       const std::vector<uint8_t>& payload);

    // Binding
    bool bindCluster(uint64_t sourceIeee, uint8_t sourceEndpoint,
                    uint16_t clusterId, uint64_t destIeee, uint8_t destEndpoint);

    // Getters
    NetworkState getNetworkState() const;
    std::vector<ZigbeeDeviceInfo> getDevices() const;
    const ZigbeeDeviceInfo* getDevice(uint64_t ieeeAddress) const;

    // Callbacks
    void onDeviceJoined(DeviceJoinedCallback callback);
    void onDeviceLeft(DeviceLeftCallback callback);
    void onDeviceMessage(DeviceMessageCallback callback);
};
```

#### ZclClusterHandler Base Class

```cpp
class ZclClusterHandler {
public:
    virtual uint16_t getClusterId() const = 0;
    virtual std::string getClusterName() const = 0;

    virtual bool handleCommand(uint64_t ieeeAddress, uint8_t endpoint,
                              uint8_t commandId,
                              const std::vector<uint8_t>& payload) = 0;

    virtual bool handleAttributeReport(uint64_t ieeeAddress, uint8_t endpoint,
                                       const std::vector<ZclAttributeValue>& attributes) = 0;
};
```

#### ZigbeeOtaProxy Class

```cpp
class ZigbeeOtaProxy {
public:
    // Image management
    bool loadImage(const std::string& imagePath);
    bool announceImage(uint16_t manufacturerCode = 0xFFFF,
                      uint16_t imageType = 0xFFFF);

    // Upgrade control
    bool startUpgrade(uint64_t ieeeAddress, uint8_t endpoint,
                     const std::string& imagePath);
    bool cancelUpgrade(uint64_t ieeeAddress);

    // Callbacks
    void onProgress(OtaProgressCallback callback);
    void onComplete(OtaCompleteCallback callback);
    void onError(OtaErrorCallback callback);

    // Configuration
    void setMaxConcurrentUpgrades(size_t maxConcurrent);
    void setBlockSize(uint8_t blockSize);
    void setRetryCount(uint16_t retries);
};
```

## Performance Considerations

### Network Capacity

- **EZSP**: Up to 200 devices (hardware dependent)
- **Z-Stack**: Up to 100-200 devices (CC2652: 200, CC2531: 50)
- **ConBee**: Up to 200 devices
- **Recommended**: Keep under 100 devices for optimal performance

### Memory Usage

- Coordinator: ~2-4 MB RAM
- Per device: ~1-2 KB RAM
- Route table: ~8 bytes per entry
- Neighbor table: ~12 bytes per entry

### Network Traffic

- Heartbeat messages: Every 7.5 seconds (routers)
- Poll interval: Configurable (sleepy devices)
- Broadcast limit: 9 per minute (network-wide)
- Group commands: Single broadcast vs. multiple unicasts

## Security Considerations

1. **Network Key**: Use randomly generated 128-bit keys
2. **Key Storage**: Store network keys securely (encrypted)
3. **Permit Join**: Always time-limited, never leave open
4. **Physical Security**: Secure coordinator physical access
5. **Firmware**: Keep coordinator firmware updated
6. **Logging**: Monitor join attempts and unauthorized access

## License

BSD 3-Clause License

Copyright (c) 2025, Walt Technologies
All rights reserved.

## Support

For issues, questions, or contributions:
- GitHub: https://github.com/waltech/friendly-lwm2m-client
- Documentation: https://friendly-lwm2m.readthedocs.io
- Email: support@waltech.com
