# OpenWRT Firmware Updater - Integration Guide

This guide explains how to integrate the OpenWRT Firmware Updater with sysupgrade support into your LWM2M client application.

## Overview

The OpenWRT Firmware Updater provides a production-ready implementation of LWM2M Object 5 (Firmware Update) specifically designed for OpenWRT devices. It integrates with OpenWRT's native `sysupgrade` utility for safe and reliable firmware updates.

## Features

✅ **Full LWM2M Compliance**: Implements all FOTA states (IDLE, DOWNLOADING, DOWNLOADED, UPDATING)
✅ **OpenWRT Native**: Uses `sysupgrade` for firmware validation and installation
✅ **PUSH & PULL**: Supports both direct upload and URI-based downloads
✅ **Multi-Protocol**: HTTP, HTTPS, CoAP, CoAPS (based on available tools)
✅ **Error Handling**: All LWM2M error codes properly implemented
✅ **Thread-Safe**: Safe for concurrent operations
✅ **Production-Ready**: Comprehensive error checking and validation

## Quick Start

### Step 1: Enable Firmware Update in CMake

```bash
cd Friendly-LwM2M-Client
mkdir -p _build && cd _build

cmake .. \
  -DOBJ_O_5_FIRMWARE_UPDATE=ON \
  -DUSE_OPENWRT_FW_UPDATER=ON \
  -DRES_5_6=ON \
  -DRES_5_7=ON \
  -DRES_5_8=ON

make
```

### Step 2: Include Headers

```cpp
#include "WppClient.h"
#include "OpenWrtFwInit.h"
```

### Step 3: Initialize in Your Application

```cpp
// Global firmware update manager
static OpenWrtFwManager g_fwManager;

void initObjects(WppClient &client) {
    // ... initialize other objects ...

    // Initialize OpenWRT firmware update
    if (!initOpenWrtFirmwareUpdate(client, g_fwManager)) {
        std::cerr << "Failed to initialize firmware update" << std::endl;
        return;
    }

    std::cout << "Firmware update initialized successfully" << std::endl;
}
```

That's it! Your application now supports LWM2M firmware updates with OpenWRT sysupgrade integration.

## Detailed Integration

### Complete Application Example

```cpp
#include "WppClient.h"
#include "OpenWrtFwInit.h"
#include "o_5_firmware_update/FirmwareUpdate.h"
#include "m_3_device/Device.h"
#include <iostream>
#include <signal.h>

using namespace wpp;
using namespace std;

// Global variables
static bool g_running = true;
static OpenWrtFwManager g_fwManager;

// Signal handler
void signalHandler(int signal) {
    g_running = false;
}

// Initialize LWM2M objects
bool initObjects(WppClient &client) {
    // 1. Device object
    client.registry().registerObj(Device::object(client));
    Device *device = Device::createInst(client);
    device->set<STRING_T>(Device::MANUFACTURER_0, "YourCompany");
    device->set<STRING_T>(Device::MODEL_NUMBER_1, "OpenWRT-One");
    device->set<STRING_T>(Device::FIRMWARE_VERSION_3, "1.0.0");

    // 2. Firmware Update object (OpenWRT specific)
    if (!initOpenWrtFirmwareUpdate(client, g_fwManager)) {
        cerr << "Failed to initialize firmware update" << endl;
        return false;
    }

    return true;
}

// Monitor firmware update progress
void monitorFirmwareUpdate(WppClient &client) {
    static INT_T lastState = -1;

    FirmwareUpdate *fw = FirmwareUpdate::instance(client);
    if (!fw) return;

    INT_T state = fw->get<INT_T>(FirmwareUpdate::STATE_3);
    if (state != lastState) {
        cout << "FW State: " << state << endl;
        lastState = state;
    }
}

int main() {
    signal(SIGINT, signalHandler);

    // Create connection and client
    Connection connection("56830", AF_INET);
    WppClient *client = WppClient::create(
        {"OpenWrtDevice", "", ""},
        connection,
        nullptr
    );

    // Initialize objects
    if (!initObjects(*client)) {
        return 1;
    }

    // Give ownership to registry
    client->giveOwnership();

    // Main loop
    while (g_running) {
        client->loop();
        monitorFirmwareUpdate(*client);
        sleep(1);
    }

    return 0;
}
```

## Build Configuration

### CMake Options

| Option | Description | Default |
|--------|-------------|---------|
| `OBJ_O_5_FIRMWARE_UPDATE` | Enable Firmware Update Object | OFF |
| `USE_OPENWRT_FW_UPDATER` | Use OpenWRT implementation | OFF |
| `RES_5_6` | Enable Package Name resource | OFF |
| `RES_5_7` | Enable Package Version resource | OFF |
| `RES_5_8` | Enable URI download (PULL method) | OFF |

### Build for OpenWRT

```bash
# Configure for OpenWRT
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/openwrt/toolchain.cmake \
  -DOBJ_O_5_FIRMWARE_UPDATE=ON \
  -DUSE_OPENWRT_FW_UPDATER=ON \
  -DRES_5_6=ON \
  -DRES_5_7=ON \
  -DRES_5_8=ON \
  -DCMAKE_BUILD_TYPE=MinSizeRel

# Build
make -j$(nproc)

# Result binary will be optimized for OpenWRT
```

## LWM2M Server Integration

### Test with Leshan Demo Server

1. Start your LWM2M client with firmware update enabled
2. Connect to Leshan Demo Server (https://leshan.eclipseprojects.io/)
3. Find your device in the client list
4. Navigate to Object 5 (Firmware Update)

### PUSH Method (Direct Upload)

```bash
# From Leshan UI:
# 1. Go to Object 5, Instance 0, Resource 0 (Package)
# 2. Click "Write"
# 3. Select "Opaque" type
# 4. Upload your firmware.bin file
# 5. The device will:
#    - Receive firmware data
#    - Save to /tmp/lwm2m_firmware.bin
#    - Change state to DOWNLOADED
# 6. Execute Resource 2 (Update)
# 7. The device will:
#    - Validate firmware with sysupgrade -T
#    - Install firmware with sysupgrade
#    - Reboot to complete update
```

### PULL Method (URI Download)

```bash
# From Leshan UI:
# 1. Go to Object 5, Instance 0, Resource 1 (Package URI)
# 2. Click "Write"
# 3. Enter firmware URL (e.g., "http://example.com/firmware.bin")
# 4. The device will:
#    - Download firmware from URI
#    - Validate and save to /tmp/lwm2m_firmware.bin
#    - Change state to DOWNLOADED
# 5. Execute Resource 2 (Update) to install
```

## State Machine

The firmware updater implements the full LWM2M FOTA state machine:

```
┌──────┐
│ IDLE │ ◄───────────────────────────┐
└──┬───┘                             │
   │ Write Package/Package URI       │
   │                                 │
   ▼                                 │
┌─────────────┐                      │
│ DOWNLOADING │                      │
└──────┬──────┘                      │
       │ Download Complete           │
       │                             │
       ▼                             │
┌────────────┐                       │
│ DOWNLOADED │                       │
└──────┬─────┘                       │
       │ Execute Update              │
       │                             │
       ▼                             │
┌──────────┐                         │
│ UPDATING │ ────────────────────────┘
└──────────┘  Update Complete/Error
              (after reboot)
```

### State Descriptions

- **IDLE (0)**: Ready to receive firmware
- **DOWNLOADING (1)**: Firmware download in progress
- **DOWNLOADED (2)**: Firmware ready to install
- **UPDATING (3)**: Firmware installation in progress

### Error Handling

All errors return to IDLE state with appropriate result code:

| Error | Code | Description |
|-------|------|-------------|
| Not Enough Flash | 2 | Insufficient storage space |
| Connection Lost | 4 | Network error during download |
| Integrity Failed | 5 | Firmware validation failed |
| Unsupported Type | 6 | Invalid firmware format |
| Invalid URI | 7 | URI not accessible |
| Update Failed | 8 | Installation failed |

## Monitoring Update Progress

### Read Current State

```cpp
FirmwareUpdate *fw = FirmwareUpdate::instance(client);
if (fw) {
    INT_T state = fw->get<INT_T>(FirmwareUpdate::STATE_3);
    INT_T result = fw->get<INT_T>(FirmwareUpdate::UPDATE_RESULT_5);

    cout << "State: " << state << ", Result: " << result << endl;

    #if RES_5_7
    STRING_T version = fw->get<STRING_T>(FirmwareUpdate::PKGVERSION_7);
    cout << "Current version: " << version << endl;
    #endif
}
```

### Observe State Changes

```cpp
void monitorFirmwareUpdate(WppClient &client) {
    static INT_T lastState = -1;

    FirmwareUpdate *fw = FirmwareUpdate::instance(client);
    if (!fw) return;

    INT_T currentState = fw->get<INT_T>(FirmwareUpdate::STATE_3);

    if (currentState != lastState) {
        switch (currentState) {
            case S_IDLE:
                cout << "State: IDLE" << endl;
                break;
            case S_DOWNLOADING:
                cout << "State: DOWNLOADING" << endl;
                break;
            case S_DOWNLOADED:
                cout << "State: DOWNLOADED - Ready to update" << endl;
                break;
            case S_UPDATING:
                cout << "State: UPDATING - System will reboot" << endl;
                break;
        }
        lastState = currentState;
    }
}

// Call periodically in main loop
while (running) {
    client->loop();
    monitorFirmwareUpdate(*client);
    sleep(1);
}
```

## Advanced Usage

### Access Individual Components

```cpp
// Get updater for custom operations
OpenWrtFwUpdater& updater = g_fwManager.getUpdater();

// Get downloader for custom operations
OpenWrtFwDownloader& downloader = g_fwManager.getDownloader();

#if RES_5_8
// Get URI downloader
OpenWrtFwUriDownloader& uriDownloader = g_fwManager.getUriDownloader();

// Check supported protocols
auto protocols = FirmwareUpdate::supportedProtocols(client);
for (auto proto : protocols) {
    switch (proto) {
        case FwUpdProtocol::HTTP:
            cout << "HTTP supported" << endl;
            break;
        case FwUpdProtocol::HTTPS:
            cout << "HTTPS supported" << endl;
            break;
        // ... etc
    }
}
#endif
```

### Custom Firmware Path

```cpp
// Set custom firmware path (if needed)
OpenWrtFwUpdater& updater = g_fwManager.getUpdater();
updater.setFirmwarePath("/tmp/custom_firmware.bin");
```

## Deployment

### OpenWRT Package Integration

To integrate into an OpenWRT package:

1. **Makefile** (example):

```makefile
include $(TOPDIR)/rules.mk

PKG_NAME:=lwm2m-client
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/lwm2m-client
  SECTION:=net
  CATEGORY:=Network
  TITLE:=LWM2M Client with FOTA support
  DEPENDS:=+libpthread +libstdcpp +wget
endef

CMAKE_OPTIONS += \
  -DOBJ_O_5_FIRMWARE_UPDATE=ON \
  -DUSE_OPENWRT_FW_UPDATER=ON \
  -DRES_5_6=ON \
  -DRES_5_7=ON \
  -DRES_5_8=ON

define Package/lwm2m-client/install
  $(INSTALL_DIR) $(1)/usr/sbin
  $(INSTALL_BIN) $(PKG_BUILD_DIR)/WppExample $(1)/usr/sbin/lwm2m-client
  $(INSTALL_DIR) $(1)/etc/init.d
  $(INSTALL_BIN) ./files/lwm2m-client.init $(1)/etc/init.d/lwm2m-client
endef

$(eval $(call BuildPackage,lwm2m-client))
```

2. **Init Script** (files/lwm2m-client.init):

```bash
#!/bin/sh /etc/rc.common
START=99
STOP=10

USE_PROCD=1
PROG=/usr/sbin/lwm2m-client

start_service() {
    procd_open_instance
    procd_set_param command $PROG
    procd_set_param respawn
    procd_set_param stdout 1
    procd_set_param stderr 1
    procd_close_instance
}
```

### Runtime Dependencies

Ensure these packages are installed on the target device:

```bash
opkg update
opkg install wget libpthread libstdcpp
```

Optional (for CoAP support):
```bash
opkg install libcoap
```

## Troubleshooting

### Issue: Download fails with "Connection Lost"

**Solution**: Check network connectivity and firewall settings

```bash
# Test connectivity
ping 8.8.8.8

# Check if wget works
wget http://example.com/test.bin

# Check firewall
iptables -L -v
```

### Issue: Validation fails with "Unsupported Package Type"

**Solution**: Verify firmware is correct for device model

```bash
# Test firmware validation manually
sysupgrade -T /tmp/lwm2m_firmware.bin

# Check device model
cat /proc/cpuinfo
cat /tmp/sysinfo/board_name
```

### Issue: Update fails with "Not Enough Flash"

**Solution**: Free up space or use smaller firmware

```bash
# Check available space
df -h

# Clean up old packages
opkg remove --autoremove unused-package

# Clean temporary files
rm -rf /tmp/*
```

### Issue: State stuck in UPDATING

**Cause**: This is normal - system is rebooting

**Solution**: Wait for device to complete reboot (may take 1-5 minutes)

## Security Considerations

1. **Firmware Validation**: Always enabled via sysupgrade
2. **HTTPS Support**: Uses system CA certificates
3. **Temporary Storage**: Files cleared on reboot
4. **Signature Verification**: Handled by sysupgrade (OpenWRT)

## Performance

- **Memory Usage**: ~500KB additional RAM during download
- **Flash Usage**: Temporary space = firmware size + 1MB
- **Download Speed**: Network-limited (typical: 100KB/s - 10MB/s)
- **Update Time**: 2-5 minutes (including reboot)

## Testing

See `example_usage.cpp` for a complete working example.

To test manually:

```bash
# Build and run
./WppExample

# From another terminal, simulate LWM2M operations
# (or use Leshan Demo Server)
```

## Support

For issues or questions:
- GitHub Issues: https://github.com/Friendly-Technologies/Friendly-LwM2M-Client/issues
- Documentation: See README.md in OpenWrtFirmwareUpdater directory

## License

Part of Friendly-LwM2M-Client project.
