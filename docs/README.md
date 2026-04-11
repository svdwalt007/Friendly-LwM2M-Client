# Friendly LwM2M Client Documentation

**Version:** 1.2.0
**Last Updated:** April 2026
**LwM2M Specification:** OMA LwM2M v1.2.2

---

## 📚 Quick Navigation

**New to the project?** Start here:
1. **[Quick Start Guide](../QUICK_START.md)** ⚡ - Get running in 5 minutes
2. **[Getting Started](../GETTING_STARTED.md)** 📖 - Comprehensive tutorial
3. **[Build Examples](../BUILD_EXAMPLES.md)** 🔨 - All build configurations

---

## 📋 Documentation Index

### Core Documentation

#### Getting Started & Building
- **[Quick Start Guide](../QUICK_START.md)** - 5-minute setup with automated installation script
- **[Getting Started](../GETTING_STARTED.md)** - Complete tutorial with code examples
- **[Build Examples](../BUILD_EXAMPLES.md)** - Build configurations for all platforms
- **[Installation Script](../scripts/install-ubuntu-prerequisites.sh)** - Automated Ubuntu dependency installation

#### Reference & Configuration
- **[API Reference](API_REFERENCE.md)** - Complete API documentation
- **[Architecture](ARCHITECTURE.md)** - System architecture and design patterns
- **[Configuration](CONFIGURATION.md)** - CMake options and runtime settings
- **[Implementation Status](IMPLEMENTATION_STATUS.md)** - Feature implementation tracking
- **[Troubleshooting](TROUBLESHOOTING.md)** - Common issues and solutions

### Feature-Specific Guides

#### LwM2M Objects
- **[WLAN Connectivity (ID 12)](WLAN_CONNECTIVITY.md)** - WiFi interface management
- **[Bearer Selection (ID 13)](BEARER_SELECTION.md)** - Network bearer selection
- **[Location Object (ID 6)](LOCATION_OBJECT.md)** - GPS and location services with Starlink integration
- **[Starlink Terminal](STARLINK_TERMINAL.md)** - Starlink satellite terminal management
- **[MIKROBUS Object](MIKROBUS_OBJECT.md)** - MIKROBUS socket and Click board management
- **[Hardware Watchdog](WATCHDOG.md)** - Watchdog timer management

### Platform Integration

- **[OpenWrt Integration](OPENWRT_INTEGRATION.md)** - Deploy on OpenWrt routers
- **[Raspberry Pi ARM64 Porting](RPI4_ARM64_PORTING_GUIDE.md)** - RPi4 deployment guide
- **[prplOS Porting Guide](PRPLOS_PORTING_GUIDE.md)** - prplOS platform integration

### Advanced Topics

- **[OMA LwM2M Compatibility Analysis](OMA_LWM2M_COMPATIBILITY_ANALYSIS.md)** - Standards compliance details
- **[Wakaama Submodule Setup](../WAKAAMA_SUBMODULE_SETUP.md)** - Optional WPP objects configuration

---

## 🚀 Key Features (v1.2.0)

### Object ID Mapping

| Object Name | Object ID | Category | Status |
|------------|-----------|----------|--------|
| **Core Objects** |
| LwM2M Security | 0 | Standard | ✅ Mandatory |
| LwM2M Server | 1 | Standard | ✅ Mandatory |
| Device | 3 | Standard | ✅ Mandatory |
| Connectivity Monitoring | 4 | Standard | ✅ Optional |
| Firmware Update | 5 | Standard | ✅ Optional |
| Location | 6 | Standard | ✅ Optional |
| **Advanced Objects** |
| WLAN Connectivity | 12 | Standard | ✅ Optional |
| Bearer Selection | 13 | Standard | ✅ Optional |
| **Custom Objects** |
| **Advanced Firmware Update** | **33405** | Custom | ✅ **New v1.2.0** |
| **MQTT Server** | **10512** | WPP | ✅ **New v1.2.0** |
| **LwM2M COSE** | **10513** | WPP | ✅ **New v1.2.0** |
| **Edge AI Inference** | **10514** | WPP | ✅ **New v1.2.0** |
| **Optional WPP Objects** (require [Wakaama submodule](../WAKAAMA_SUBMODULE_SETUP.md)) |
| WAN Failover Policy | 10525 | WPP | ⚙️ Optional |
| Multi-WAN Health Check | 10526 | WPP | ⚙️ Optional |
| WiFi Client Management | 10527 | WPP | ⚙️ Optional |
| WiFi Channel Optimization | 10528 | WPP | ⚙️ Optional |
| LAN Configuration | 10535 | WPP | ⚙️ Optional |
| Routing Table | 10536 | WPP | ⚙️ Optional |
| VPN Configuration | 10537 | WPP | ⚙️ Optional |

### Advanced Features

- **Delta Firmware Updates** - bsdiff, vcdiff, Courgette algorithms
- **Rollback Management** - A/B partition with automatic rollback
- **MQTT Transport** - OMA LwM2M v1.2.2 Section 8 compliance
- **Edge AI Inference** - TensorFlow Lite and ONNX Runtime backends
- **Blockwise Transfer** - RFC 7959 with BERT support
- **Platform Abstraction** - Linux, OpenWrt, Raspberry Pi support
- **Starlink Integration** - Direct gRPC client for satellite terminals

---

## 📖 Documentation by Use Case

### For New Users
1. Read [Quick Start Guide](../QUICK_START.md)
2. Run [Installation Script](../scripts/install-ubuntu-prerequisites.sh)
3. Build using [Build Examples](../BUILD_EXAMPLES.md#standard-release-build)
4. Connect to LwM2M server

### For Developers
1. Read [Getting Started](../GETTING_STARTED.md)
2. Review [Architecture](ARCHITECTURE.md)
3. Check [API Reference](API_REFERENCE.md)
4. See [Configuration](CONFIGURATION.md) for build options

### For Platform Integrators
1. Choose your platform:
   - [OpenWrt Integration](OPENWRT_INTEGRATION.md)
   - [Raspberry Pi Guide](RPI4_ARM64_PORTING_GUIDE.md)
   - [prplOS Porting](PRPLOS_PORTING_GUIDE.md)
2. Review [Cross-Compilation](../BUILD_EXAMPLES.md#cross-compilation)
3. Check [Platform Abstraction](../GETTING_STARTED.md#custom-platform-implementation)

### For Troubleshooting
1. Check [Troubleshooting Guide](TROUBLESHOOTING.md)
2. Review [Build Examples](../BUILD_EXAMPLES.md#troubleshooting-common-build-issues)
3. Search GitHub Issues
4. Enable debug logging with `--log-level debug`

---

## 📝 Version History

### v1.2.0 (Current - April 2026)

**Major Changes:**
- ✅ **Object ID Renumbering** - Now using 10512-10537 range for WPP objects
- ✅ **Advanced Firmware Update** (ID 33405) - Delta updates with rollback
- ✅ **New Core Objects** - MQTT Server (10512), COSE (10513), Edge AI (10514)
- ✅ **Optional WPP Objects** - 10525-10537 range (require Wakaama submodule)
- ✅ **Comprehensive Documentation** - Quick Start, Getting Started, Build Examples
- ✅ **Installation Automation** - Ubuntu prerequisites installation script
- ✅ **Documentation Rationalization** - Archived obsolete content

**Bug Fixes:**
- ✅ Fixed all test compilation errors
- ✅ Resolved BlockSize enum scope issues
- ✅ Updated test files to match API
- ✅ Fixed UpdateResult enum values

### v1.1.0 (January 2026)
- MQTT Transport Binding
- Edge AI Inference Object (33410)
- Delta Firmware Updates
- A/B Partition Rollback

### v1.0 (November 2025)
- Initial release
- Location object implementation
- Walt Technologies custom objects
- OpenWrt integration

---

## 🗃️ Archived Documentation

Historical and obsolete documentation has been moved to [docs/archive/](archive/README.md):

- **Outdated**: Old quick starts, object renumbering docs, old object IDs
- **Platform-Specific**: Niche platform guides (prpl, OpenWrt One specific)
- **Integration Guides**: Future features not yet implemented (Matter, Zigbee)
- **Analysis Docs**: Historical planning documents
- **Legacy HTML**: Old Doxygen-generated HTML

See [archive/README.md](archive/README.md) for details.

---

## 🆘 Getting Help

### Resources
- **GitHub Issues**: Bug reports and feature requests
- **Discussions**: Questions and community support
- **Email**: support@friendly-tech.com

### Before Asking
1. Check [Troubleshooting Guide](TROUBLESHOOTING.md)
2. Search existing GitHub issues
3. Review relevant documentation
4. Enable debug logging: `--log-level debug`

### Reporting Issues
Include:
- LwM2M Client version (v1.2.0)
- Platform and OS version
- Build configuration
- Steps to reproduce
- Log output (with `--log-level debug`)

---

## 📜 License

Documentation is licensed under MIT License.
Code examples are provided as-is for integration purposes.

---

**Last Updated**: April 11, 2026
**Documentation Version**: 1.2.0
