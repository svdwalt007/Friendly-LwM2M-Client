# LuCI Web Interface for Friendly LwM2M Client

Professional web interface for the Friendly LwM2M Client on OpenWRT, providing intuitive configuration and real-time monitoring capabilities.

## Overview

This LuCI application provides a complete web-based interface for managing the LwM2M client on OpenWRT devices. It integrates seamlessly with UCI configuration and ubus for real-time status monitoring and control.

## Features

### 1. Real-Time Status Dashboard
- Live connection status with auto-refresh (5-second intervals)
- Service control (start, stop, restart, reload)
- Registration status monitoring
- Server connection information
- Client state visualization
- System uptime tracking

### 2. General Settings Configuration
- **General Settings**
  - Endpoint name configuration
  - Bootstrap mode enable/disable
  - Log level selection (Emergency to Debug)
  - Custom objects configuration path

- **Network Settings**
  - Network interface selection
  - Local UDP port configuration
  - IPv4/IPv6 support toggle

- **Device Information**
  - Manufacturer and model
  - Serial number
  - Firmware and hardware versions
  - Device type classification

### 3. LwM2M Server Management
- Multiple server configuration support
- Server URI configuration (CoAP/CoAPS)
- Server ID and lifetime settings
- Binding mode selection (U, UQ, S, SQ, US, UQS)
- Bootstrap server designation
- Security instance mapping

### 4. Security Configuration
- **NoSec Mode** - No security (testing only)
- **PSK Mode** - Pre-Shared Key authentication
  - PSK identity configuration
  - Hex key input with validation
  - Key length validation (8-32 bytes)
- **RPK Mode** - Raw Public Key
  - Public/private key pair
  - Server public key
- **Certificate Mode** - X.509 certificates
  - Certificate chain management
  - Private key configuration
  - CA certificate setup

### 5. Objects Management
- View all registered LwM2M objects
- Display object instances and status
- Standard OMA objects (0-32767)
- Custom objects (34601-34649)
- Firmware update status monitoring
- Object resource information

### 6. Interactive Operations
- Manual registration trigger
- Registration update
- Deregister from server
- Configuration reload
- Resource read/write operations (via AJAX)

## Installation

### Prerequisites
- OpenWRT 22.03 or later
- friendly-lwm2m-client package installed
- LuCI base system

### Method 1: Build with OpenWRT SDK

```bash
# Copy the luci directory to OpenWRT packages feed
cd ~/openwrt-sdk
cp -r /path/to/Friendly-LwM2M-Client/openwrt/luci package/luci-app-lwm2m

# Update feeds
./scripts/feeds update -a
./scripts/feeds install -a

# Select the package
make menuconfig
# Navigate to: LuCI > 3. Applications > luci-app-lwm2m
# Press 'M' to build as module

# Build
make package/luci-app-lwm2m/compile V=s
```

### Method 2: Manual Installation

```bash
# On the OpenWRT device
opkg update
opkg install luci-app-lwm2m

# Restart services
/etc/init.d/rpcd restart
/etc/init.d/uhttpd restart
```

### Method 3: From Source

```bash
# On your development machine
cd /path/to/Friendly-LwM2M-Client/openwrt/luci

# Create package
tar czf luci-app-lwm2m.tar.gz *

# Copy to OpenWRT device
scp luci-app-lwm2m.tar.gz root@192.168.1.1:/tmp/

# On OpenWRT device
cd /
tar xzf /tmp/luci-app-lwm2m.tar.gz
rm -rf /tmp/luci-indexcache /tmp/luci-modulecache
/etc/init.d/rpcd restart
```

## Usage

### Accessing the Interface

1. Open web browser and navigate to your OpenWRT device (e.g., http://192.168.1.1)
2. Login with your credentials
3. Navigate to **Services > LwM2M Client**

### Quick Start Configuration

1. **Configure General Settings**
   - Go to **General Settings** tab
   - Set a unique endpoint name
   - Select appropriate log level
   - Configure network interface (usually 'wan')

2. **Add LwM2M Server**
   - In **General Settings**, scroll to **LwM2M Server Configuration**
   - Click **Add** to create a new server
   - Enter server URI (e.g., `coap://leshan.eclipseprojects.io:5683`)
   - Set server ID (e.g., 1)
   - Configure lifetime (300-86400 seconds)
   - Save changes

3. **Configure Security**
   - Go to **Security** tab
   - Add a new security configuration matching your server name
   - Select security mode:
     - **NoSec** for testing
     - **PSK** for production (most common)
   - For PSK mode:
     - Enter PSK identity
     - Enter PSK key in hex format
   - Save changes

4. **Start the Service**
   - Go to **Status** tab
   - Click **Start Service**
   - Monitor registration status
   - Check **Registered Objects** section

### Example PSK Configuration

```
Server Configuration:
  Name: my_server
  URI: coaps://myserver.example.com:5684
  Server ID: 1
  Lifetime: 300
  Security Instance: 0

Security Configuration:
  Name: my_server_security
  Mode: PSK
  PSK Identity: mydevice123
  PSK Key: 73656372657470736b6b6579313233
           (hex for "secretpskkey123")
```

### Generating PSK Keys

```bash
# Generate random 16-byte (128-bit) PSK key
openssl rand -hex 16

# Generate random 32-byte (256-bit) PSK key
openssl rand -hex 32
```

## Directory Structure

```
openwrt/luci/
├── Makefile                                  # OpenWRT package Makefile
├── README.md                                 # This file
├── luasrc/
│   ├── controller/
│   │   └── lwm2m.lua                        # Main controller (routing)
│   ├── model/
│   │   └── cbi/
│   │       └── lwm2m/
│   │           ├── main.lua                 # General settings CBI
│   │           ├── objects.lua              # Objects view CBI
│   │           └── security.lua             # Security settings CBI
│   └── view/
│       └── lwm2m/
│           └── status.htm                   # Status page template
└── root/
    └── usr/
        └── share/
            └── rpcd/
                └── acl.d/
                    └── luci-app-lwm2m.json  # ACL permissions
```

## API Endpoints

The controller provides the following AJAX endpoints:

### Status Information
- **GET** `/admin/services/lwm2m/status_json`
  - Returns: Service status, client status, uptime

### Object Management
- **GET** `/admin/services/lwm2m/objects_json`
  - Returns: List of registered objects

### Resource Operations
- **GET** `/admin/services/lwm2m/resource_read`
  - Parameters: `object_id`, `instance_id`, `resource_id`
  - Returns: Resource value

- **POST** `/admin/services/lwm2m/resource_write`
  - Parameters: `object_id`, `instance_id`, `resource_id`, `value`
  - Returns: Success status

### Client Actions
- **POST** `/admin/services/lwm2m/action`
  - Parameter: `action` (register|update|deregister|reload|start|stop|restart)
  - Returns: Action result

## Troubleshooting

### Interface Not Appearing

```bash
# Clear LuCI cache
rm -rf /tmp/luci-indexcache /tmp/luci-modulecache

# Restart services
/etc/init.d/rpcd restart
/etc/init.d/uhttpd restart
```

### Permission Denied Errors

```bash
# Check ACL configuration
cat /usr/share/rpcd/acl.d/luci-app-lwm2m.json

# Verify user permissions
ubus call session list
```

### Status Not Updating

1. Check if LwM2M service is running:
   ```bash
   /etc/init.d/lwm2m status
   ```

2. Verify ubus connectivity:
   ```bash
   ubus call lwm2m status
   ```

3. Check browser console for JavaScript errors

4. Verify network connectivity in browser

### Configuration Not Saving

1. Check UCI configuration:
   ```bash
   uci show lwm2m
   ```

2. Verify write permissions:
   ```bash
   ls -la /etc/config/lwm2m
   ```

3. Check system logs:
   ```bash
   logread | grep lwm2m
   ```

## Browser Compatibility

- Chrome/Chromium 90+
- Firefox 88+
- Safari 14+
- Edge 90+
- Opera 76+

## Security Considerations

1. **HTTPS Access**: Always access LuCI over HTTPS in production
2. **Strong Passwords**: Use strong passwords for LuCI authentication
3. **Firewall Rules**: Restrict LuCI access to trusted networks
4. **PSK Keys**: Use strong random PSK keys (minimum 128 bits)
5. **Certificate Validation**: Always validate certificates in production
6. **NoSec Mode**: Never use NoSec mode in production environments

## Development

### Adding New Features

1. Controller modifications: Edit `luasrc/controller/lwm2m.lua`
2. UI pages: Add CBI models in `luasrc/model/cbi/lwm2m/`
3. Templates: Add HTM files in `luasrc/view/lwm2m/`
4. Rebuild package and install

### Testing

```bash
# On OpenWRT device
/etc/init.d/lwm2m stop
lwm2m-client -v  # Run in foreground with verbose logging

# In browser, monitor network tab for AJAX calls
# Check browser console for JavaScript errors
```

### Debugging

```bash
# Enable debug logging
uci set lwm2m.general.log_level='7'
uci commit lwm2m
/etc/init.d/lwm2m restart

# Monitor logs
logread -f | grep lwm2m

# Check ubus interface
ubus -v list lwm2m
ubus call lwm2m status
```

## Contributing

Contributions are welcome! Please ensure:
- Code follows OpenWRT/LuCI conventions
- All user-facing strings are translatable
- AJAX endpoints return proper JSON
- Security best practices are followed
- Documentation is updated

## License

MIT License - See LICENSE file for details

## Support

For issues and questions:
- GitHub Issues: https://github.com/Friendly-Technologies/Friendly-LwM2M-Client
- Documentation: See main project README
- OpenWRT Forum: https://forum.openwrt.org/

## Credits

Developed by Friendly Technologies
Based on Wakaama LwM2M implementation
