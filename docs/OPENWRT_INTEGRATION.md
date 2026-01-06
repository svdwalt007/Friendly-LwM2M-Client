# OpenWRT Integration Guide

Complete guide for integrating the Friendly LwM2M Client with OpenWRT.

## Table of Contents

1. [Overview](#overview)
2. [Quick Start](#quick-start)
3. [Building the Package](#building-the-package)
4. [Configuration](#configuration)
5. [UCI Integration](#uci-integration)
6. [Service Management](#service-management)
7. [Platform Features](#platform-features)
8. [Troubleshooting](#troubleshooting)

---

## Overview

### Supported OpenWRT Versions

| Version | Status | Notes |
|---------|--------|-------|
| 24.10.x | ✅ Recommended | Primary development target |
| 23.05.x | ✅ Supported | Full compatibility |
| 22.03.x | ⚠️ Compatible | May need minor adjustments |
| 21.02.x | ⚠️ Limited | Older toolchain |

### Supported Architectures

| Architecture | Target Example | Status |
|--------------|----------------|--------|
| ARM64 (aarch64) | OpenWRT One, RPi 4 | ✅ Primary |
| ARM (armhf) | RPi 3, many routers | ✅ Supported |
| x86_64 | VM, PC Engines | ✅ Supported |
| MIPS | Many legacy routers | ✅ Supported |

---

## Quick Start

### Using Pre-built Package

```bash
# Add Friendly Technologies feed
echo "src/gz friendly https://packages.friendly-tech.com/openwrt/24.10" \
    >> /etc/opkg/customfeeds.conf

# Update package lists
opkg update

# Install
opkg install friendly-lwm2m-client

# Configure
uci set lwm2m.@server[0].uri='coaps://lwm2m.example.com:5684'
uci set lwm2m.@server[0].endpoint='my-router'
uci commit lwm2m

# Start service
/etc/init.d/lwm2m enable
/etc/init.d/lwm2m start
```

### Building from Source

```bash
# Get OpenWRT SDK
wget https://downloads.openwrt.org/releases/24.10.4/targets/x86/64/openwrt-sdk-24.10.4-x86-64_gcc-13.3.0_musl.Linux-x86_64.tar.xz
tar xf openwrt-sdk-*.tar.xz
cd openwrt-sdk-*/

# Add custom feed
cat >> feeds.conf << 'EOF'
src-git friendly https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git;main
EOF

# Update feeds
./scripts/feeds update -a
./scripts/feeds install friendly-lwm2m-client

# Build package
make package/friendly-lwm2m-client/compile V=s

# Find package
ls bin/packages/*/friendly/friendly-lwm2m-client*.ipk
```

---

## Building the Package

### Package Makefile

The package is defined in `openwrt/Makefile`:

```makefile
include $(TOPDIR)/rules.mk

PKG_NAME:=friendly-lwm2m-client
PKG_VERSION:=1.1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git
PKG_SOURCE_VERSION:=v$(PKG_VERSION)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/friendly-lwm2m-client
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Friendly LwM2M Client
  URL:=https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client
  DEPENDS:=+libstdcpp +libopenssl +libcurl +zlib +liblzma +libzstd
  MAINTAINER:=Friendly Technologies <support@friendly-tech.com>
endef

define Package/friendly-lwm2m-client/description
  Enhanced LwM2M client supporting OMA LwM2M v1.2.2 with:
  - Delta firmware updates (BSDIFF, VCDIFF)
  - A/B partition rollback
  - MQTT transport binding
  - Edge AI inference (optional)
endef

CMAKE_OPTIONS += \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_MQTT=ON \
    -DWITH_EDGE_AI=OFF \
    -DWITH_ROLLBACK=ON \
    -DBUILD_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF

define Package/friendly-lwm2m-client/conffiles
/etc/config/lwm2m
endef

define Package/friendly-lwm2m-client/install
    $(INSTALL_DIR) $(1)/usr/bin
    $(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/lwm2m_client $(1)/usr/bin/

    $(INSTALL_DIR) $(1)/usr/lib
    $(CP) $(PKG_INSTALL_DIR)/usr/lib/libfriendly_lwm2m.so* $(1)/usr/lib/

    $(INSTALL_DIR) $(1)/etc/init.d
    $(INSTALL_BIN) ./files/lwm2m.init $(1)/etc/init.d/lwm2m

    $(INSTALL_DIR) $(1)/etc/config
    $(INSTALL_CONF) ./files/lwm2m.config $(1)/etc/config/lwm2m

    $(INSTALL_DIR) $(1)/var/lib/lwm2m
endef

$(eval $(call BuildPackage,friendly-lwm2m-client))
```

### Build Options

```bash
# Minimal build (smaller size)
make menuconfig
# Set: Utilities -> friendly-lwm2m-client -> Configuration
#   [*] Enable MQTT transport
#   [ ] Enable Edge AI (disabled for size)
#   [*] Enable delta firmware updates
#   [*] Enable A/B rollback

# Build
make package/friendly-lwm2m-client/compile V=s
```

---

## Configuration

### Default Configuration File

`/etc/config/lwm2m`:

```
config lwm2m 'global'
    option enabled '1'
    option log_level 'info'
    option endpoint_name ''
    option config_dir '/etc/lwm2m'
    option data_dir '/var/lib/lwm2m'

config server 'server0'
    option enabled '1'
    option uri 'coaps://lwm2m.example.com:5684'
    option security_mode 'psk'
    option psk_identity ''
    option psk_key ''
    option lifetime '86400'
    option binding 'U'
    option queue_mode '0'

config mqtt 'mqtt0'
    option enabled '0'
    option broker_uri ''
    option broker_port '8883'
    option client_id ''
    option qos '1'
    option use_tls '1'

config firmware 'firmware'
    option delta_enabled '1'
    option rollback_enabled '1'
    option download_path '/tmp/firmware'
    option max_size '134217728'

config edge_ai 'edge_ai'
    option enabled '0'
    option model_path '/var/lib/lwm2m/models'
    option max_model_size '104857600'
```

### Configuration Examples

#### Basic CoAP/DTLS Setup

```bash
uci set lwm2m.global.endpoint_name='openwrt-router-001'
uci set lwm2m.server0.uri='coaps://lwm2m.example.com:5684'
uci set lwm2m.server0.security_mode='psk'
uci set lwm2m.server0.psk_identity='router001'
uci set lwm2m.server0.psk_key='0102030405060708090A0B0C0D0E0F10'
uci commit lwm2m
/etc/init.d/lwm2m restart
```

#### MQTT Transport Setup

```bash
uci set lwm2m.mqtt0.enabled='1'
uci set lwm2m.mqtt0.broker_uri='mqtt.example.com'
uci set lwm2m.mqtt0.broker_port='8883'
uci set lwm2m.mqtt0.client_id='openwrt-router-001'
uci set lwm2m.mqtt0.use_tls='1'
uci commit lwm2m
/etc/init.d/lwm2m restart
```

#### Certificate-Based Security

```bash
# Copy certificates
mkdir -p /etc/lwm2m/certs
scp client.pem client.key ca.pem router:/etc/lwm2m/certs/

# Configure
uci set lwm2m.server0.security_mode='cert'
uci set lwm2m.server0.client_cert='/etc/lwm2m/certs/client.pem'
uci set lwm2m.server0.client_key='/etc/lwm2m/certs/client.key'
uci set lwm2m.server0.ca_cert='/etc/lwm2m/certs/ca.pem'
uci commit lwm2m
/etc/init.d/lwm2m restart
```

---

## UCI Integration

### Reading Configuration

```cpp
#include "platform/openwrt_platform.h"

using namespace lwm2m::platform;

OpenWRTPlatform platform;
platform.initialize();

// Read UCI values
std::string endpoint = platform.uciGet("lwm2m.global.endpoint_name");
std::string serverUri = platform.uciGet("lwm2m.server0.uri");
std::string securityMode = platform.uciGet("lwm2m.server0.security_mode");

// Parse lifetime
int lifetime = std::stoi(platform.uciGet("lwm2m.server0.lifetime"));
```

### Writing Configuration

```cpp
// Update configuration
platform.uciSet("lwm2m.server0.lifetime", "43200");
platform.uciCommit("lwm2m");
```

### UCI Callback for Runtime Changes

```cpp
// Watch for configuration changes
platform.uciWatch("lwm2m", [](const std::string& section, const std::string& option) {
    std::cout << "Config changed: " << section << "." << option << std::endl;
    // Reload configuration or restart service
});
```

---

## Service Management

### Init Script

`/etc/init.d/lwm2m`:

```bash
#!/bin/sh /etc/rc.common

START=95
STOP=10

USE_PROCD=1

PROG=/usr/bin/lwm2m_client
CONFIG_FILE=/etc/config/lwm2m

validate_section_lwm2m() {
    uci_load_validate lwm2m lwm2m "$1" "$2" \
        'enabled:bool:1' \
        'log_level:string:info' \
        'endpoint_name:string' \
        'config_dir:string:/etc/lwm2m' \
        'data_dir:string:/var/lib/lwm2m'
}

validate_section_server() {
    uci_load_validate lwm2m server "$1" "$2" \
        'enabled:bool:1' \
        'uri:string' \
        'security_mode:string:psk' \
        'psk_identity:string' \
        'psk_key:string' \
        'lifetime:uinteger:86400' \
        'binding:string:U' \
        'queue_mode:bool:0'
}

start_service() {
    validate_section_lwm2m global start_instance
}

start_instance() {
    [ "$2" = 0 ] || {
        echo "validation failed" >&2
        return 1
    }

    [ "$enabled" = "0" ] && return 0

    procd_open_instance
    procd_set_param command $PROG
    procd_append_param command --config "$config_dir/lwm2m.json"
    procd_append_param command --log-level "$log_level"

    [ -n "$endpoint_name" ] && procd_append_param command --endpoint "$endpoint_name"

    procd_set_param respawn
    procd_set_param stderr 1
    procd_set_param stdout 1
    procd_close_instance
}

stop_service() {
    killall -TERM lwm2m_client 2>/dev/null
}

reload_service() {
    stop
    start
}

service_triggers() {
    procd_add_reload_trigger "lwm2m"
}
```

### Service Commands

```bash
# Enable/disable service
/etc/init.d/lwm2m enable
/etc/init.d/lwm2m disable

# Start/stop/restart
/etc/init.d/lwm2m start
/etc/init.d/lwm2m stop
/etc/init.d/lwm2m restart

# Check status
/etc/init.d/lwm2m status

# View logs
logread | grep lwm2m
logread -f | grep lwm2m  # Follow logs
```

---

## Platform Features

### MTD Flash Operations

```cpp
OpenWRTPlatform platform;

// Read from MTD partition
std::vector<uint8_t> data;
if (platform.mtdRead("firmware", data)) {
    std::cout << "Read " << data.size() << " bytes from firmware partition" << std::endl;
}

// Write to MTD partition
std::vector<uint8_t> newFirmware = loadFirmware();
if (platform.mtdWrite("firmware_backup", newFirmware)) {
    std::cout << "Firmware backup written" << std::endl;
}

// Erase MTD partition
if (platform.mtdErase("firmware_backup")) {
    std::cout << "Partition erased" << std::endl;
}
```

### Device Information

```cpp
DeviceInfo info = platform.getDeviceInfo();

std::cout << "Manufacturer: " << info.manufacturer << std::endl;
std::cout << "Model: " << info.modelNumber << std::endl;
std::cout << "Serial: " << info.serialNumber << std::endl;
std::cout << "Firmware: " << info.firmwareVersion << std::endl;
std::cout << "Hardware: " << info.hardwareVersion << std::endl;

// OpenWRT-specific
std::cout << "Target: " << platform.uciGet("system.@system[0].target") << std::endl;
std::cout << "Board: " << platform.uciGet("system.@system[0].board") << std::endl;
```

### Network Information

```cpp
NetworkInfo net = platform.getNetworkInfo();

std::cout << "IP Address: " << net.ipAddress << std::endl;
std::cout << "Gateway: " << net.gateway << std::endl;
std::cout << "MAC: " << net.macAddress << std::endl;
std::cout << "Signal: " << net.signalStrength << " dBm" << std::endl;
std::cout << "Link: " << (net.linkUp ? "Up" : "Down") << std::endl;
```

### A/B Partition Support

```cpp
// Check current boot partition
PartitionSlot active = platform.getActiveSlot();
std::cout << "Active slot: " << (active == PartitionSlot::SLOT_A ? "A" : "B") << std::endl;

// Write to inactive partition
platform.writeToInactiveSlot(firmwareData);

// Verify and switch
if (platform.isSlotBootable(PartitionSlot::SLOT_B)) {
    platform.switchSlot();
    platform.reboot();
}
```

---

## Troubleshooting

### Common Issues

#### Package Installation Fails

```bash
# Check dependencies
opkg info friendly-lwm2m-client

# Install missing dependencies
opkg update
opkg install libstdcpp libopenssl libcurl zlib

# Force reinstall
opkg remove friendly-lwm2m-client
opkg install friendly-lwm2m-client --force-reinstall
```

#### Service Won't Start

```bash
# Check configuration
uci show lwm2m

# Test manually
/usr/bin/lwm2m_client --config /etc/lwm2m/lwm2m.json --log-level debug

# Check logs
logread | grep lwm2m | tail -50
```

#### Connection Failures

```bash
# Test network connectivity
ping lwm2m.example.com

# Test DNS
nslookup lwm2m.example.com

# Check firewall
iptables -L -n | grep 5684

# Test CoAP connection
coap-client -m get coap://lwm2m.example.com:5683/.well-known/core
```

#### Certificate Issues

```bash
# Verify certificate
openssl x509 -in /etc/lwm2m/certs/client.pem -text -noout

# Test TLS connection
openssl s_client -connect lwm2m.example.com:5684 \
    -cert /etc/lwm2m/certs/client.pem \
    -key /etc/lwm2m/certs/client.key \
    -CAfile /etc/lwm2m/certs/ca.pem
```

### Debug Mode

```bash
# Enable debug logging
uci set lwm2m.global.log_level='debug'
uci commit lwm2m
/etc/init.d/lwm2m restart

# Watch logs
logread -f | grep lwm2m
```

### Package Size Optimization

```bash
# Check package size
opkg info friendly-lwm2m-client | grep Size

# Minimal build options for smaller size
# In menuconfig:
# - Disable Edge AI
# - Disable examples
# - Enable -Os optimization
```

---

*For more information, see the [Build Guide](BUILD_GUIDE.md) or open an issue on GitHub.*
