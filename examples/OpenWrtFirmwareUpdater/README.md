# OpenWRT Firmware Updater

OpenWRT-specific implementation of LWM2M Firmware Update Object (Object ID 5) with full FOTA state machine compliance and sysupgrade integration.

## Features

- **Full LWM2M FOTA Compliance**: Implements all LWM2M firmware update states (IDLE, DOWNLOADING, DOWNLOADED, UPDATING)
- **OpenWRT Integration**: Uses native `sysupgrade` command for safe firmware updates
- **Multiple Delivery Methods**:
  - **PUSH**: Firmware sent directly via LWM2M Package resource
  - **PULL**: Firmware downloaded from URI via Package URI resource
- **Protocol Support**: HTTP, HTTPS, CoAP, CoAPS (depending on available tools)
- **Comprehensive Error Handling**: All LWM2M update result codes supported
- **State Management**: Full state machine with proper transitions
- **Validation**: Firmware validation using `sysupgrade -T`
- **Space Checking**: Verifies available flash space before update

## LWM2M FOTA States

The implementation follows the OMA LWM2M specification for firmware updates:

### States (Resource 3)

| State | Value | Description |
|-------|-------|-------------|
| IDLE | 0 | No update in progress, ready to start download |
| DOWNLOADING | 1 | Firmware is being downloaded |
| DOWNLOADED | 2 | Firmware downloaded successfully, ready to update |
| UPDATING | 3 | Firmware update is in progress |

### Update Results (Resource 5)

| Result | Value | Description |
|--------|-------|-------------|
| Initial | 0 | Default initial state |
| Firmware Updated Successfully | 1 | Update completed successfully |
| Not Enough Flash Memory | 2 | Insufficient storage space |
| Out of RAM | 3 | Insufficient RAM during download/update |
| Connection Lost | 4 | Network connection lost during download |
| Integrity Check Failed | 5 | Downloaded firmware failed validation |
| Unsupported Package Type | 6 | Firmware format not supported |
| Invalid URI | 7 | Package URI is invalid or file not found |
| Firmware Update Failed | 8 | Generic update failure |
| Unsupported Protocol | 9 | Download protocol not supported |

## Components

### OpenWrtFwUpdater
Main firmware updater class that:
- Validates firmware using `sysupgrade -T`
- Checks available flash space
- Executes firmware installation using `sysupgrade`
- Manages update state and results

### OpenWrtFwDownloader
Internal downloader for PUSH method:
- Receives firmware data via LWM2M Package resource
- Writes firmware to temporary storage
- Performs integrity checking
- Notifies updater when complete

### OpenWrtFwUriDownloader
External downloader for PULL method:
- Downloads firmware from HTTP/HTTPS/CoAP/CoAPS URI
- Uses `wget`, `curl`, or `coap-client` depending on availability
- Handles connection errors and retries
- Validates downloaded firmware

### OpenWrtFwManager
Manager class that:
- Initializes all firmware update components
- Manages component lifecycle
- Provides easy integration interface

## Usage

### Basic Integration

```cpp
#include "OpenWrtFwInit.h"

// Global manager instance
static OpenWrtFwManager fwManager;

void initObjects(WppClient &client) {
    // Initialize other LWM2M objects first
    deviceInit(client);
    serverInit(client);
    securityInit(client);

    // Initialize OpenWRT firmware update
    if (!initOpenWrtFirmwareUpdate(client, fwManager)) {
        cerr << "Failed to initialize firmware update" << endl;
        return;
    }

    cout << "Firmware update initialized successfully" << endl;
}

int main() {
    // Create LWM2M client
    WppClient client = ...;

    // Initialize objects
    initObjects(client);

    // Give ownership to registry
    client.giveOwnership();

    // Main loop
    while (running) {
        client.loop();
        sleep(1);
    }

    return 0;
}
```

### Advanced Usage

```cpp
// Access individual components
OpenWrtFwUpdater& updater = fwManager.getUpdater();
OpenWrtFwDownloader& downloader = fwManager.getDownloader();

#if RES_5_8
OpenWrtFwUriDownloader& uriDownloader = fwManager.getUriDownloader();

// Check supported protocols
auto protocols = FirmwareUpdate::supportedProtocols(client);
for (auto proto : protocols) {
    cout << "Supported: " << (int)proto << endl;
}
#endif

// Monitor update state
FirmwareUpdate* fw = FirmwareUpdate::instance(client);
if (fw) {
    INT_T state = fw->get<INT_T>(FirmwareUpdate::STATE_3);
    INT_T result = fw->get<INT_T>(FirmwareUpdate::UPDATE_RESULT_5);

    cout << "Current state: " << state << endl;
    cout << "Last result: " << result << endl;
}
```

## Firmware Update Process

### PUSH Method (Direct Upload)

1. LWM2M server writes firmware data to Package resource (0)
2. `OpenWrtFwDownloader` receives data blocks
3. Firmware saved to `/tmp/lwm2m_firmware.bin`
4. State changes: IDLE → DOWNLOADING → DOWNLOADED
5. LWM2M server executes Update resource (2)
6. `OpenWrtFwUpdater` validates firmware with `sysupgrade -T`
7. If valid, executes `sysupgrade -v /tmp/lwm2m_firmware.bin`
8. State changes: DOWNLOADED → UPDATING
9. System reboots to complete update
10. After reboot, state returns to IDLE with result SUCCESS

### PULL Method (URI Download)

1. LWM2M server writes URI to Package URI resource (1)
2. `OpenWrtFwUriDownloader` downloads from URI
3. Uses `wget`, `curl`, or `coap-client` based on protocol
4. Firmware saved to `/tmp/lwm2m_firmware.bin`
5. State changes: IDLE → DOWNLOADING → DOWNLOADED
6. Continue from step 5 of PUSH method

## Requirements

### System Tools

The firmware updater requires the following OpenWRT tools:

- **Required**:
  - `sysupgrade` - For firmware validation and installation (always available on OpenWRT)

- **Optional** (for PULL method):
  - `wget` - For HTTP/HTTPS downloads (recommended)
  - `curl` - Alternative for HTTP/HTTPS downloads
  - `coap-client` - For CoAP/CoAPS downloads

### Storage Space

- Minimum 1 MB free space in `/tmp` for temporary files
- Firmware size + 512 KB margin in flash for update

## Configuration

### Build Configuration

Enable firmware update in CMake:

```cmake
set(OBJ_O_5_FIRMWARE_UPDATE ON)  # Enable Object 5
set(RES_5_8 ON)                  # Enable URI download (optional)
set(RES_5_6 ON)                  # Enable package name (optional)
set(RES_5_7 ON)                  # Enable package version (optional)
```

### Runtime Configuration

Firmware files are stored in `/tmp`:
- Temporary file: `/tmp/lwm2m_firmware.tmp`
- Final file: `/tmp/lwm2m_firmware.bin`
- Metadata: `/tmp/lwm2m_fw_metadata.txt`

## Error Handling

The implementation handles various error scenarios:

- **Connection Lost**: Detected during URI download, reported via R_CONN_LOST
- **Invalid Firmware**: Detected by sysupgrade validation, reported via R_UNSUPPORTED_PKG_TYPE
- **Insufficient Space**: Checked before download/update, reported via R_NOT_ENOUGH_FLASH
- **Integrity Failure**: File size mismatch or validation failure, reported via R_INTEGRITY_CHECK_FAIL
- **Invalid URI**: 404 errors or invalid URLs, reported via R_INVALID_URI

## Security Considerations

- Firmware validation using OpenWRT's built-in sysupgrade checks
- HTTPS certificate verification using system CA certificates
- Temporary files stored in `/tmp` (cleared on reboot)
- No firmware kept after successful update

## Testing

### Manual Testing

1. **Test PUSH method**:
```bash
# From LWM2M server, write firmware to Package resource
lwm2m-client write /5/0/0 @firmware.bin
lwm2m-client execute /5/0/2
```

2. **Test PULL method**:
```bash
# From LWM2M server, write URI to Package URI resource
lwm2m-client write /5/0/1 "http://example.com/firmware.bin"
```

3. **Monitor state**:
```bash
# Read current state
lwm2m-client read /5/0/3

# Read update result
lwm2m-client read /5/0/5
```

## Known Limitations

- System will reboot during firmware update (expected behavior)
- Download progress not reported (LWM2M limitation)
- CoAP support requires coap-client package
- Large firmware images (>100MB) may fail on devices with limited RAM

## Troubleshooting

### Download Fails

- Check network connectivity
- Verify URI is accessible
- Check available space in `/tmp`
- Ensure wget/curl is installed for HTTP downloads

### Validation Fails

- Verify firmware is correct for device model
- Check sysupgrade compatibility
- Review sysupgrade logs: `logread | grep sysupgrade`

### Update Fails

- Check available flash space
- Verify firmware integrity
- Review system logs after reboot: `logread`

## License

Part of Friendly-LwM2M-Client project.
