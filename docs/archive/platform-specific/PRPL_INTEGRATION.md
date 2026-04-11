# prpl Platform Integration Guide

## Overview

The prpl Platform Abstraction Layer provides comprehensive integration between the Friendly LwM2M Client and prplOS-based systems. This integration enables advanced features including:

- **Ambiorix (amxb/amxd)** bus integration for data model access
- **TR-181 Device:2** data model support
- **EasyMesh R2/R4** Multi-AP coordination
- **prplMesh LwM2M Object (34640)** for mesh network management
- **USP Agent** interaction capabilities

## Architecture

### Component Overview

```
┌─────────────────────────────────────────────────────────────┐
│             Friendly LwM2M Client                           │
├─────────────────────────────────────────────────────────────┤
│  prplMesh LwM2M Object (34640)                             │
│  ├─ Network Topology                                        │
│  ├─ Link Quality Metrics                                    │
│  └─ Steering Policies                                       │
├─────────────────────────────────────────────────────────────┤
│  prpl Platform Abstraction Layer                           │
│  ├─ PrplPlatform         (Main Platform Interface)         │
│  ├─ PrplAmxbClient       (Ambiorix Bus Client)             │
│  ├─ PrplDataModel        (TR-181 Access Layer)             │
│  ├─ PrplEasyMesh         (EasyMesh Integration)            │
│  └─ PrplMeshObject       (Mesh LwM2M Object)               │
├─────────────────────────────────────────────────────────────┤
│  prplOS Services                                            │
│  ├─ Ambiorix Bus (ubus/pcb)                                │
│  ├─ TR-181 Data Model                                       │
│  ├─ ieee1905d (EasyMesh)                                    │
│  └─ USP Agent                                               │
└─────────────────────────────────────────────────────────────┘
```

### Key Components

#### 1. PrplAmxbClient
Provides abstraction over Ambiorix bus operations:
- Connection management (ubus, pcb backends)
- Object operations (get, set, add, delete)
- Method invocation
- Event subscription
- Transaction support

#### 2. PrplDataModel
High-level access to TR-181 i2 data model:
- Device.DeviceInfo
- Device.WiFi (Radios, SSIDs, AccessPoints, Stations)
- Device.Ethernet (Interfaces)
- Device.IP (Interfaces)
- Device.Hosts (Host table)
- Caching and type conversion utilities

#### 3. PrplEasyMesh
EasyMesh R2/R4 Multi-AP integration:
- Controller/Agent role detection
- Network topology discovery
- Client steering (RSSI, load-based, band)
- Channel selection coordination
- Backhaul optimization
- Event subscription

#### 4. PrplMeshObject
Custom LwM2M Object (34640) for mesh management:
- Network status and configuration
- Topology metrics
- Link quality monitoring
- Steering policy control
- Management operations (scan, optimize, etc.)

## Building with prpl Support

### Prerequisites

1. **prplOS System** with:
   - Ambiorix libraries (libamxb, libamxc, libamxd, libamxp)
   - TR-181 data model support
   - Optional: ieee1905d for EasyMesh
   - Optional: USP agent

2. **Build Dependencies**:
   ```bash
   # Install Ambiorix development packages
   opkg install libamxb-dev libamxc-dev libamxd-dev libamxp-dev

   # Install OpenSSL (for hash calculations)
   opkg install libopenssl-dev
   ```

### CMake Configuration

```bash
mkdir build && cd build

cmake .. \
  -DENABLE_PRPL_PLATFORM=ON \
  -DHAVE_AMBIORIX=ON \
  -DCMAKE_BUILD_TYPE=Release

make -j$(nproc)
make install
```

### Conditional Compilation

The prpl platform uses conditional compilation to support builds with or without Ambiorix:

```cpp
#ifdef HAVE_AMBIORIX
  // Full Ambiorix integration
#else
  // Stub implementation
#endif
```

## Configuration

### Runtime Configuration

#### 1. Ambiorix Bus URI

The default bus URI is `ubus:/var/run/ubus/ubus.sock`. To use a different backend:

```cpp
// Use PCB (prpl control bus)
PrplPlatform::initialize("pcb:/var/run/pcb.sock");

// Use ubus (OpenWrt micro bus)
PrplPlatform::initialize("ubus:/var/run/ubus/ubus.sock");
```

#### 2. Environment Variables

```bash
# Set Ambiorix bus backend
export AMXB_BACKENDS="ubus,pcb"

# Enable debug logging
export AMXB_DEBUG=1
export PRPL_LWM2M_DEBUG=1
```

#### 3. Configuration File

Create `/etc/lwm2m/prpl.conf`:

```ini
[prpl]
bus_uri = ubus:/var/run/ubus/ubus.sock
enable_easymesh = true
enable_mesh_object = true
cache_ttl = 5

[easymesh]
auto_channel = true
band_steering = true
backhaul_optimization = true
steering_policy = rssi_based

[mesh_object]
instance_id = 0
update_interval = 30
```

## API Reference

### Platform Initialization

```cpp
#include "platform/prpl/prpl_platform.h"

using namespace lwm2m::platform::prpl;

// Detect prplOS
auto osInfo = PrplPlatform::detectPrplOS();
if (!osInfo.isPresent) {
    std::cerr << "Not running on prplOS" << std::endl;
    return false;
}

// Initialize platform
if (!PrplPlatform::initialize()) {
    std::cerr << "Failed to initialize prpl platform" << std::endl;
    return false;
}

// Get components
auto amxbClient = PrplPlatform::getAmxbClient();
auto dataModel = PrplPlatform::getDataModel();
auto easyMesh = PrplPlatform::getEasyMesh();
```

### TR-181 Data Access

```cpp
#include "platform/prpl/prpl_data_model.h"

// Get device information
auto deviceInfo = dataModel->getDeviceInfoStructured();
std::cout << "Model: " << deviceInfo.modelName << std::endl;
std::cout << "Serial: " << deviceInfo.serialNumber << std::endl;
std::cout << "Version: " << deviceInfo.softwareVersion << std::endl;

// Get WiFi radios
auto radios = dataModel->getWiFiRadios();
for (const auto& radio : radios) {
    std::cout << "Radio: " << radio.name
              << " Channel: " << radio.channel
              << " Band: " << static_cast<int>(radio.band) << std::endl;
}

// Get active WiFi clients
auto clients = dataModel->getWiFiAssociatedDevices();
for (const auto& client : clients) {
    if (client.active) {
        std::cout << "Client: " << client.macAddress
                  << " RSSI: " << static_cast<int>(client.signalStrength)
                  << " dBm" << std::endl;
    }
}

// Get network interfaces
auto interfaces = dataModel->getIPInterfaces();
for (const auto& iface : interfaces) {
    std::cout << "Interface: " << iface.name
              << " Status: " << iface.status << std::endl;
    for (const auto& ip : iface.ipv4Addresses) {
        std::cout << "  IPv4: " << ip << std::endl;
    }
}
```

### EasyMesh Integration

```cpp
#include "platform/prpl/prpl_easymesh.h"

// Check EasyMesh availability
if (!easyMesh->isAvailable()) {
    std::cerr << "EasyMesh not available" << std::endl;
    return;
}

// Get network information
auto network = easyMesh->getNetworkInfo();
std::cout << "Network ID: " << network.networkId << std::endl;
std::cout << "Controller: " << network.controllerMAC << std::endl;
std::cout << "Agents: " << network.agentCount << std::endl;

// Get all agents
auto agents = easyMesh->getAgents();
for (const auto& agent : agents) {
    std::cout << "Agent: " << agent.macAddress
              << " Model: " << agent.model
              << " Connected: " << agent.isConnected << std::endl;

    if (agent.backhaulType == BackhaulType::WIFI_5G) {
        std::cout << "  Backhaul RSSI: "
                  << static_cast<int>(agent.backhaulSignalStrength)
                  << " dBm" << std::endl;
    }
}

// Enable band steering
easyMesh->setBandSteeringEnabled(true);
easyMesh->setSteeringPolicy(SteeringPolicy::RSSI_BASED);

// Steer a client
SteeringRequest request;
request.clientMAC = "AA:BB:CC:DD:EE:FF";
request.targetBSSID = "11:22:33:44:55:66";
request.policy = SteeringPolicy::RSSI_BASED;
request.disassocTimer = 100; // ms

if (easyMesh->steerClient(request)) {
    std::cout << "Client steering initiated" << std::endl;
}

// Trigger channel selection
easyMesh->triggerChannelSelection();

// Optimize backhaul
easyMesh->optimizeBackhaul();
```

### prplMesh LwM2M Object

```cpp
#include "platform/prpl/prpl_mesh_object.h"

// Create mesh object instance
auto meshObject = std::make_shared<PrplMeshObject>(
    0,  // Instance ID
    easyMesh,
    dataModel
);

if (!meshObject->initialize()) {
    std::cerr << "Failed to initialize mesh object" << std::endl;
    return;
}

// Read resources
std::string networkId;
meshObject->read(PrplMeshResourceId::NETWORK_ID, networkId);
std::cout << "Network ID: " << networkId << std::endl;

int64_t agentCount;
meshObject->read(PrplMeshResourceId::AGENT_COUNT, agentCount);
std::cout << "Agent Count: " << agentCount << std::endl;

bool isOperational;
meshObject->read(PrplMeshResourceId::IS_OPERATIONAL, isOperational);
std::cout << "Operational: " << isOperational << std::endl;

// Write resources
meshObject->write(PrplMeshResourceId::BAND_STEERING_ENABLED, true);
meshObject->write(PrplMeshResourceId::STEERING_POLICY,
                  static_cast<int64_t>(MeshSteeringPolicy::RSSI_BASED));

// Execute operations
meshObject->execute(PrplMeshResourceId::TRIGGER_TOPOLOGY_QUERY);
meshObject->execute(PrplMeshResourceId::TRIGGER_CHANNEL_SELECT);
meshObject->execute(PrplMeshResourceId::OPTIMIZE_BACKHAUL);

// Get metrics
auto metrics = meshObject->getTopologyMetrics();
std::cout << "Topology Metrics:" << std::endl;
std::cout << "  Agents: " << metrics.agentCount << std::endl;
std::cout << "  Radios: " << metrics.radioCount << std::endl;
std::cout << "  Clients: " << metrics.clientCount << std::endl;
std::cout << "  Avg Backhaul RSSI: " << static_cast<int>(metrics.avgBackhaulRSSI) << " dBm" << std::endl;
std::cout << "  Avg Client RSSI: " << static_cast<int>(metrics.avgClientRSSI) << " dBm" << std::endl;
```

### Ambiorix Bus Operations

```cpp
#include "platform/prpl/prpl_amxb_client.h"

auto amxbClient = std::make_shared<PrplAmxbClient>();

// Connect to bus
if (!amxbClient->connect("ubus:/var/run/ubus/ubus.sock")) {
    std::cerr << "Failed to connect to Ambiorix bus" << std::endl;
    return;
}

// Get object parameters
auto params = amxbClient->get("Device.DeviceInfo");
for (const auto& [key, value] : params) {
    std::cout << key << " = " << value << std::endl;
}

// Set parameters
std::map<std::string, std::string> updates;
updates["FriendlyName"] = "My prplOS Device";
amxbClient->set("Device.DeviceInfo", updates);

// Invoke method
std::map<std::string, std::string> args;
args["Mode"] = "Full";
std::map<std::string, std::string> result;
amxbClient->invoke("Device.WiFi", "Reset", args, result);

// Subscribe to events
uint32_t subId = amxbClient->subscribe(
    "Device.WiFi.*",
    "ClientConnect",
    [](const std::string& event, const std::string& object,
       const std::map<std::string, std::string>& data) {
        std::cout << "Event: " << event << " on " << object << std::endl;
        for (const auto& [key, value] : data) {
            std::cout << "  " << key << " = " << value << std::endl;
        }
    }
);

// Process events
amxbClient->processEvents(100); // 100ms timeout

// Unsubscribe
amxbClient->unsubscribe(subId);
```

## EasyMesh Integration Guide

### Network Topology Discovery

```cpp
// Get complete topology
auto agents = easyMesh->getAgents();
auto radios = easyMesh->getRadios();
auto bsss = easyMesh->getBSSs();
auto clients = easyMesh->getClients();

// Build topology map
std::map<std::string, std::vector<EasyMeshRadio>> agentRadios;
for (const auto& radio : radios) {
    agentRadios[radio.agentMAC].push_back(radio);
}

// Display topology
for (const auto& agent : agents) {
    std::cout << "Agent: " << agent.macAddress << std::endl;

    for (const auto& radio : agentRadios[agent.macAddress]) {
        std::cout << "  Radio: " << radio.macAddress
                  << " Ch:" << radio.channel
                  << " Util:" << static_cast<int>(radio.utilization) << "%"
                  << std::endl;
    }
}
```

### Channel Selection

```cpp
// Scan channels on a radio
ChannelScanRequest scanReq;
scanReq.radioMAC = "11:22:33:44:55:66";
scanReq.channels = {36, 40, 44, 48}; // 5GHz channels
scanReq.dwellTime = 100; // ms per channel
scanReq.activeScan = true;

auto scanResults = easyMesh->scanChannels(scanReq);

// Find best channel (lowest utilization)
uint32_t bestChannel = 0;
uint8_t minUtilization = 100;

for (const auto& result : scanResults) {
    if (result.utilization < minUtilization) {
        minUtilization = result.utilization;
        bestChannel = result.channel;
    }
}

// Set the best channel
if (bestChannel > 0) {
    easyMesh->setChannel(scanReq.radioMAC, bestChannel);
}

// Or trigger automatic coordinated channel selection
easyMesh->triggerChannelSelection();
```

### Client Steering

```cpp
// RSSI-based steering
auto clients = easyMesh->getClients();
for (const auto& client : clients) {
    if (client.signalStrength < -70) { // Weak signal
        // Find better AP
        auto radios = easyMesh->getRadios();
        // ... logic to find best target BSSID ...

        SteeringRequest req;
        req.clientMAC = client.macAddress;
        req.targetBSSID = targetBSSID;
        req.policy = SteeringPolicy::RSSI_BASED;
        req.disassocTimer = 100;
        req.mandatoryTarget = false; // Opportunity mode

        easyMesh->steerClient(req);
    }
}
```

### Backhaul Optimization

```cpp
// Monitor backhaul quality
auto agents = easyMesh->getAgents();
for (const auto& agent : agents) {
    if (agent.backhaulType != BackhaulType::ETHERNET) {
        int8_t rssi = easyMesh->getBackhaulLinkQuality(agent.macAddress);

        if (rssi < -65) { // Weak backhaul
            std::cout << "Weak backhaul on agent " << agent.macAddress
                      << ": " << static_cast<int>(rssi) << " dBm" << std::endl;
        }
    }
}

// Trigger optimization
easyMesh->optimizeBackhaul();
```

## TR-181 Data Model Mapping

### Device.DeviceInfo

| TR-181 Parameter | prplDataModel Method | Type |
|------------------|----------------------|------|
| Manufacturer | getDeviceInfo()["Manufacturer"] | String |
| ModelName | getDeviceInfo()["ModelName"] | String |
| SerialNumber | getDeviceInfo()["SerialNumber"] | String |
| SoftwareVersion | getDeviceInfo()["SoftwareVersion"] | String |
| UpTime | getUptime() | uint32_t |

### Device.WiFi.Radio.{i}

| TR-181 Parameter | WiFiRadioInfo Field | Type |
|------------------|---------------------|------|
| Enable | enabled | bool |
| Name | name | string |
| Channel | channel | uint32_t |
| OperatingStandards | standard | WiFiStandard |
| TransmitPower | transmitPower | int8_t |
| OperatingChannelBandwidth | operatingChannelBandwidth | uint32_t |

### Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}

| TR-181 Parameter | WiFiAssociatedDevice Field | Type |
|------------------|----------------------------|------|
| MACAddress | macAddress | string |
| IPAddress | ipAddress | string |
| Active | active | bool |
| SignalStrength | signalStrength | int8_t |
| LastDataDownlinkRate | lastDataDownlinkRate | uint32_t |
| BytesReceived | bytesReceived | uint64_t |

## Troubleshooting

### Common Issues

#### 1. "Failed to connect to Ambiorix bus"

**Cause**: ubus daemon not running or socket not accessible

**Solution**:
```bash
# Check ubus daemon
ps | grep ubusd

# Restart ubus
/etc/init.d/ubus restart

# Check socket permissions
ls -l /var/run/ubus/ubus.sock
```

#### 2. "TR-181 object not found"

**Cause**: Required data model plugin not loaded

**Solution**:
```bash
# List loaded data model modules
ubus list | grep Device

# Load required module
modprobe dm_device_wifi
```

#### 3. "EasyMesh not available"

**Cause**: ieee1905d daemon not running

**Solution**:
```bash
# Check EasyMesh daemon
ps | grep ieee1905d

# Start daemon
/etc/init.d/ieee1905 start

# Enable on boot
/etc/init.d/ieee1905 enable
```

### Debug Logging

Enable verbose logging:

```cpp
// Enable Ambiorix client debug mode
amxbClient->setDebugMode(true);

// Enable data model caching with short TTL for testing
dataModel->setCacheEnabled(true, 1); // 1 second TTL
```

Environment variables:
```bash
export AMXB_DEBUG=1
export AMXD_DEBUG=1
export PRPL_LWM2M_DEBUG=1
```

### Performance Tuning

#### Cache Configuration

```cpp
// Disable caching for real-time data
dataModel->setCacheEnabled(false);

// Enable caching with custom TTL
dataModel->setCacheEnabled(true, 10); // 10 seconds

// Clear cache manually
dataModel->clearCache();
```

#### Batch Operations

```cpp
// Use transactions for multiple operations
amxbClient->beginTransaction();

amxbClient->set("Device.WiFi.Radio.1", {{"Channel", "36"}});
amxbClient->set("Device.WiFi.Radio.2", {{"Channel", "149"}});
amxbClient->set("Device.WiFi.Radio.1", {{"Enable", "true"}});

if (amxbClient->commitTransaction()) {
    std::cout << "All changes applied" << std::endl;
} else {
    amxbClient->abortTransaction();
    std::cerr << "Transaction failed" << std::endl;
}
```

## Platform Detection

The prpl platform can be detected at runtime:

```cpp
auto osInfo = PrplPlatform::detectPrplOS();

std::cout << "prplOS Present: " << osInfo.isPresent << std::endl;
std::cout << "Version: " << osInfo.version << std::endl;
std::cout << "Variant: " << osInfo.variant << std::endl;
std::cout << "Ambiorix: " << osInfo.hasAmbiorix << std::endl;
std::cout << "USP Agent: " << osInfo.hasUSPAgent << std::endl;
std::cout << "EasyMesh: " << osInfo.hasEasyMesh << std::endl;
std::cout << "Board: " << osInfo.boardName << std::endl;
std::cout << "Chipset: " << osInfo.chipsetModel << std::endl;
```

## License

Copyright (c) 2024-2026 Friendly Technologies

Licensed under the MIT License. See LICENSE file for details.

## Support

For issues and questions:
- GitHub: https://github.com/friendly-technologies/Friendly-LwM2M-Client
- Documentation: https://docs.friendlytech.com/lwm2m/prpl
- Email: support@friendlytech.com
