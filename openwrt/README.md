# OpenWRT Package Integration for Friendly LwM2M Client

## Overview

This directory contains the OpenWRT package integration for the Friendly LwM2M Client, specifically configured for the **OpenWRT One router** with comprehensive device management capabilities.

## Package Information

- **Package Name**: friendly-lwm2m-client
- **Version**: 1.0.0
- **Target Platform**: OpenWRT One (MediaTek MT7981B)
- **OpenWRT Version**: v24.10.4
- **Bootstrap Server**: coap://demo-iot.friendly-tech.com:5680
- **Endpoint Name**: walttech888

## Features

### Implemented LwM2M Objects

#### OMA Standard Objects
- **Object 0**: LwM2M Security (bootstrap credentials)
- **Object 1**: LwM2M Server (registration, lifetime)
- **Object 3**: Device (manufacturer, model, reboot)
- **Object 4**: Connectivity Monitoring (network bearer, IP addresses)
- **Object 5**: Firmware Update (OTA updates)
- **Object 12**: WLAN Connectivity (WiFi management)
- **Object 13**: Bearer Selection (network selection)
- **Object 3331**: Memory (RAM monitoring)

#### Walt Technologies Custom Objects (34601-34649 range)
- **Object 34601**: Router Management (LAN/WAN, DHCP, DNS, firewall)
- **Object 34602**: Ethernet Interface Management (port statistics, link status)
- **Object 34603**: GPIO Control (LEDs, buttons, GPIO pins)
- **Object 34604**: USB Management (USB port monitoring and control)
- **Object 34605**: Storage Management (NAND, NVMe, USB storage)
- **Object 34606**: System Monitor (CPU, memory, thermal, load average)

## Build Instructions

### Prerequisites

1. OpenWRT SDK v24.10.4 for MediaTek MT7981B
2. Git with submodules support
3. Required build tools: cmake, gcc, g++, make

### Building the Package

#### Method 1: Using OpenWRT SDK

```bash
# Download and extract OpenWRT SDK v24.10.4
cd ~/openwrt-sdk

# Clone the package into the SDK
cd package
git clone --recursive https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git friendly-lwm2m-client

# Create symlink to OpenWRT Makefile
cd friendly-lwm2m-client
ln -s openwrt/Makefile ./Makefile

# Return to SDK root and update feeds
cd ~/openwrt-sdk
./scripts/feeds update -a
./scripts/feeds install -a

# Configure the build
make menuconfig
# Navigate to: Network > IoT > friendly-lwm2m-client
# Select it with 'M' (module) or '*' (built-in)

# Build the package
make package/friendly-lwm2m-client/compile V=s
```

#### Method 2: Using Image Builder

```bash
# Download OpenWRT Image Builder
cd ~/openwrt-imagebuilder

# Build custom image with LwM2M client
make image PROFILE=openwrt-one \
    PACKAGES="friendly-lwm2m-client luci"
```

### Package Installation

#### Install on Running System

```bash
# Copy the package to the router
scp bin/packages/*/packages/friendly-lwm2m-client_*.ipk root@192.168.1.1:/tmp/

# SSH to the router
ssh root@192.168.1.1

# Install the package
opkg update
opkg install /tmp/friendly-lwm2m-client_*.ipk
```

#### Verify Installation

```bash
# Check if service is running
/etc/init.d/lwm2m-client status

# View logs
logread | grep lwm2m

# Check configuration
cat /etc/config/lwm2m-client
```

## Configuration

### UCI Configuration

Edit `/etc/config/lwm2m-client`:

```bash
uci set lwm2m-client.config.enabled='1'
uci set lwm2m-client.config.bootstrap_uri='coap://demo-iot.friendly-tech.com:5680'
uci set lwm2m-client.config.endpoint_name='walttech888'
uci set lwm2m-client.config.local_port='56830'
uci commit lwm2m-client
/etc/init.d/lwm2m-client restart
```

### Service Management

```bash
# Start the service
/etc/init.d/lwm2m-client start

# Stop the service
/etc/init.d/lwm2m-client stop

# Restart the service
/etc/init.d/lwm2m-client restart

# Enable on boot
/etc/init.d/lwm2m-client enable

# Disable on boot
/etc/init.d/lwm2m-client disable
```

## File Locations

```
/usr/sbin/lwm2m-client          # Main executable
/etc/init.d/lwm2m-client        # Init script
/etc/config/lwm2m-client        # UCI configuration
/etc/lwm2m/endpoint.conf        # Runtime configuration
/var/run/lwm2m-client.pid       # PID file
```

## Dependencies

The package automatically installs these dependencies:

- libc
- libstdcpp (C++17 support)
- libopenssl (DTLS/encryption)
- libuci (OpenWRT configuration)
- libubox (OpenWRT core library)
- libubus (Inter-process communication)
- libiwinfo (WiFi information)

## Troubleshooting

### Client Won't Start

```bash
# Check logs
logread -f | grep lwm2m

# Check configuration
cat /etc/config/lwm2m-client

# Verify bootstrap server is reachable
ping demo-iot.friendly-tech.com

# Test CoAP connectivity
# Install coap-client: opkg install libcoap
coap-client -m get coap://demo-iot.friendly-tech.com:5680
```

### Bootstrap Issues

```bash
# Verify security configuration in logs
logread | grep -i security

# Check network connectivity
ifconfig
route -n

# Verify firewall rules allow CoAP (UDP 5680)
iptables -L -n | grep 5680
```

### Memory Issues

```bash
# Check available memory
free -m

# Reduce log level to save memory
uci set lwm2m-client.config.log_level='3'  # Errors only
uci commit lwm2m-client
/etc/init.d/lwm2m-client restart
```

## Development

### Adding Custom Objects

1. Create object directory: `wpp/registry/objects/w_{ID}_{name}/`
2. Implement object class files
3. Update `wpp/configs/wpp_config.cmake` to add compile flag
4. Update `examples/objects.cpp` to initialize object
5. Rebuild package

### Testing

```bash
# Build with verbose logging
CMAKE_OPTIONS="-DWPP_LOGS_LEVEL=0" make package/friendly-lwm2m-client/compile V=s

# Monitor real-time logs
ssh root@openwrt-one "logread -f | grep lwm2m"
```

## License

Apache License 2.0

## Support

- GitHub: https://github.com/Friendly-Technologies/Friendly-LwM2M-Client
- Issues: https://github.com/Friendly-Technologies/Friendly-LwM2M-Client/issues
- Walt Technologies: contact@walttech.com

## References

- [OMA LwM2M Specification](https://www.openmobilealliance.org/release/LightweightM2M/)
- [OpenWRT Package Build System](https://openwrt.org/docs/guide-developer/packages)
- [OpenWRT One Hardware](https://openwrt.org/toh/openwrt/one)
- [Friendly LwM2M Documentation](https://github.com/Friendly-Technologies/Friendly-LwM2M-Client)
