# Friendly LwM2M Client Documentation

**Version:** 1.0
**Last Updated:** November 2025
**Project:** OpenWRT LwM2M Client with Location & Starlink Integration

---

## 📚 Documentation Index

This documentation provides comprehensive guides for implementing, configuring, and deploying the Friendly LwM2M Client on OpenWRT devices, with special focus on the Location object (ID 6) and Starlink Terminal integration.

### Getting Started

- **[Quick Start Guide](QUICKSTART.md)** - Get up and running in minutes
  - Prerequisites and dependencies
  - Build and installation steps
  - Basic configuration
  - First connection to LwM2M server

### Implementation & Development

- **[Implementation Guide](IMPLEMENTATION_GUIDE.md)** - Step-by-step implementation
  - Building from source
  - Cross-compilation for OpenWRT
  - Custom object development
  - Testing and validation

- **[Architecture Overview](ARCHITECTURE.md)** - System design and structure
  - Component architecture
  - Object registry system
  - Task queue management
  - Data flow diagrams

### Feature Documentation

- **[Location Object (ID 6)](LOCATION_OBJECT.md)** - Complete Location object reference
  - OMA LwM2M Location specification
  - GPS data sources (Starlink, gpsd, UCI)
  - Resource definitions
  - Code examples

- **[Starlink Terminal Object (ID 34600)](STARLINK_TERMINAL.md)** - Starlink satellite terminal management
  - 70+ resources for comprehensive monitoring and control
  - Network performance telemetry and diagnostics
  - Obstruction detection and sky view analysis
  - Dish alignment and GPS tracking
  - Alert system and power management
  - gRPC integration guide

- **[OpenWRT Integration](OPENWRT_INTEGRATION.md)** - OpenWRT-specific features
  - Hardware information gathering
  - Network connectivity monitoring
  - GPS/Location services
  - System integration

### Reference Material

- **[API Reference](API_REFERENCE.md)** - Complete API documentation
  - Core classes and methods
  - Object implementations
  - Helper utilities
  - Code examples

- **[Configuration Guide](CONFIGURATION.md)** - Configuration options
  - CMake build options
  - Resource configuration
  - Object enablement
  - Runtime settings

### Support

- **[Troubleshooting Guide](TROUBLESHOOTING.md)** - Common issues and solutions
  - Build errors
  - Runtime issues
  - Connectivity problems
  - Debug techniques

---

## 🚀 Key Features

### OMA LwM2M Objects Implemented

#### Mandatory Objects
- **Device (ID 3)** - Device information with OpenWRT integration
- **LwM2M Server (ID 1)** - Server configuration
- **LwM2M Security (ID 0)** - Security credentials

#### Optional Objects
- **Location (ID 6)** ⭐ NEW - GPS location with Starlink integration
- **Connectivity Monitoring (ID 4)** - Network status with OpenWRT data
- **Firmware Update (ID 5)** - FOTA support for OpenWRT
- **Hardware Watchdog (ID 34607)** - Watchdog timer management

#### Walt Technologies Custom Objects (34600-34607)
- **Starlink Terminal (ID 34600)** ⭐ FEATURED - Comprehensive Starlink Gen3 satellite terminal management
  - 70+ resources across 10 functional groups
  - Real-time telemetry: throughput, latency, SNR, packet loss
  - Obstruction monitoring with 12-wedge sky view analysis
  - Dish alignment tracking (azimuth/elevation)
  - 16 alert conditions (thermal, motors, obstructions, water detection)
  - Integrated GPS (lat/lon/alt with satellite count)
  - Power management (current, mean, min/max, total energy)
  - Control actions: reboot, speed test, factory reset, obstruction map
  - Historical statistics and performance analytics
  - gRPC interface to 192.168.100.1:9200
- **Router Management (ID 34601)** - OpenWRT router configuration
- **Ethernet Interface (ID 34602)** - Port monitoring and control
- **GPIO Control (ID 34603)** - LED and GPIO management
- **USB Management (ID 34604)** - USB port control
- **Storage Management (ID 34605)** - Storage device management
- **System Monitor (ID 34606)** - CPU, RAM, uptime monitoring

### OpenWRT One Platform Support

Optimized for **OpenWRT One** (Banana Pi BPI-R4):
- MediaTek MT7981B SoC
- 2.5GbE WAN + Gigabit LAN ports
- Multiple power sources (12V DC, PoE, USB-C PD)
- WiFi 6 support
- USB and NVMe expansion

### Location Data Sources

The Location object supports multiple GPS sources with automatic fallback:

1. **Starlink Terminal GPS** (Highest Priority)
   - Connects to Starlink dish at 192.168.100.1:9200
   - Accuracy: ~10 meters
   - Provides latitude, longitude, altitude
   - Real-time satellite tracking

2. **gpsd Daemon**
   - Standard GPS daemon integration
   - Accuracy: ~15 meters
   - Supports multiple GPS receivers

3. **Manual Configuration**
   - UCI-based configuration
   - User-defined coordinates
   - Fallback for fixed installations

---

## 📋 Quick Reference

### Build Commands

```bash
# Standard build
mkdir build && cd build
cmake ..
make

# OpenWRT build with location support
mkdir build && cd build
cmake -DOPENWRT_BUILD=ON ..
make
```

### Enable/Disable Objects

Edit `wpp/configs/wpp_config.cmake`:

```cmake
# Enable Location object
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_6_LOCATION)

# Enable Starlink Terminal
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_34600_STARLINK_TERMINAL)
```

### Key File Locations

```
wpp/registry/objects/
├── o_6_location/              # Location object
│   ├── Location.h
│   ├── Location.cpp
│   ├── OpenWrtLocationInfo.h
│   └── LocationConfig.h
├── o_34600_starlink_terminal/ # Starlink object
├── m_3_device/                # Device object
└── o_4_connectivity_monitoring/ # Connectivity object

wpp/configs/
└── wpp_config.cmake           # Main configuration

examples/
├── main.cpp                   # Main application
└── objects.cpp                # Object initialization
```

---

## 🔗 Related Resources

### OMA LwM2M Specifications
- [OMA LwM2M Registry](https://github.com/OpenMobileAlliance/lwm2m-registry)
- [Location Object Specification](https://github.com/OpenMobileAlliance/lwm2m-registry/blob/prod/6.xml)
- [Device Object Specification](https://github.com/OpenMobileAlliance/lwm2m-registry/blob/prod/3.xml)

### OpenWRT Resources
- [OpenWRT One Hardware](https://openwrt.org/toh/openwrt/one)
- [OpenWRT Documentation](https://openwrt.org/docs/start)
- [UCI Configuration System](https://openwrt.org/docs/guide-user/base-system/uci)

### Project Components
- [Wakaama LwM2M Stack](https://github.com/eclipse/wakaama)
- [Starlink gRPC API](https://github.com/sparky8512/starlink-grpc-tools)

### Starlink Resources
- [Starlink Gen3 Dishy Specifications](https://www.starlink.com)
- [Starlink Protocol Buffers](https://github.com/starlink-community/starlink-grpc-api)
- [Starlink Community Tools](https://github.com/sparky8512/starlink-grpc-tools)

---

## 💡 Common Use Cases

### 1. Asset Tracking
Use the Location object with Starlink Terminal for real-time tracking:
- Mobile installations (vehicles, vessels)
- Remote site monitoring
- Field equipment tracking

### 2. Network Infrastructure Monitoring
Monitor OpenWRT routers with full telemetry:
- Device health (memory, CPU, uptime)
- Network connectivity status
- Power source monitoring
- Environmental sensors

### 3. IoT Gateway Deployment
Use as an IoT gateway with LwM2M device management:
- Centralized device management
- Firmware updates (FOTA)
- Configuration management
- Status monitoring

### 4. Satellite Internet Management
Manage Starlink terminals via LwM2M:
- Dish alignment monitoring
- Performance metrics
- Obstruction detection
- GPS location tracking

---

## 📞 Support & Contribution

### Getting Help

1. Check the [Troubleshooting Guide](TROUBLESHOOTING.md)
2. Review the [API Reference](API_REFERENCE.md)
3. Search existing issues
4. Create a new issue with detailed information

### Contributing

Contributions are welcome! Please follow these guidelines:

1. **Code Style**: Follow existing patterns
2. **Documentation**: Update docs for new features
3. **Testing**: Test on OpenWRT hardware
4. **Commit Messages**: Use conventional commits

### License

This project is open source. See LICENSE file for details.

---

## 📝 Version History

### Version 1.0 (November 2025)
- ✅ Location object (ID 6) implementation
- ✅ OpenWRT integration for Device and Connectivity Monitoring
- ✅ Starlink Terminal object (ID 34600)
- ✅ Walt Technologies custom objects (34601-34607)
- ✅ OpenWRT One platform support
- ✅ Comprehensive documentation

### Planned Features
- [ ] Enhanced FOTA with incremental updates
- [ ] WebSocket transport support
- [ ] MQTT bridge integration
- [ ] Additional IPSO objects

---

## 🏗️ Project Structure

```
Friendly-LwM2M-Client/
├── docs/                      # Documentation (you are here)
│   ├── README.md
│   ├── QUICKSTART.md
│   ├── IMPLEMENTATION_GUIDE.md
│   ├── LOCATION_OBJECT.md
│   ├── OPENWRT_INTEGRATION.md
│   ├── API_REFERENCE.md
│   ├── CONFIGURATION.md
│   ├── TROUBLESHOOTING.md
│   └── ARCHITECTURE.md
├── wpp/                       # WPP LwM2M library
│   ├── registry/              # Object registry
│   │   └── objects/           # LwM2M objects
│   └── configs/               # Configuration files
├── examples/                  # Example applications
├── utils/                     # Utilities and tools
│   └── object_maker/          # Object XML definitions
└── CMakeLists.txt            # Build configuration
```

---

**Next Steps:** Start with the [Quick Start Guide](QUICKSTART.md) to build and run your first LwM2M client!
