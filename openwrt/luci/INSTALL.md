# LuCI App LwM2M - Installation Guide

Complete installation guide for the LwM2M Client LuCI web interface.

## Prerequisites

Before installing the LuCI app, ensure you have:

1. **OpenWRT Device** running version 22.03 or later
2. **Friendly LwM2M Client** package installed (friendly-lwm2m-client)
3. **LuCI Base System** installed (luci, luci-base)
4. **Network Connectivity** to download packages or transfer files

## Installation Methods

### Method 1: From OpenWRT Packages Feed (Recommended)

If the package is available in your OpenWRT packages feed:

```bash
# Update package lists
opkg update

# Install the LuCI app
opkg install luci-app-lwm2m

# Restart services to activate
/etc/init.d/rpcd restart
rm -rf /tmp/luci-indexcache /tmp/luci-modulecache
/etc/init.d/uhttpd restart
```

### Method 2: Build from OpenWRT SDK

#### Step 1: Setup OpenWRT SDK

```bash
# Download OpenWRT SDK for your architecture
cd ~/
wget https://downloads.openwrt.org/releases/23.05.0/targets/<arch>/generic/openwrt-sdk-*.tar.xz
tar xf openwrt-sdk-*.tar.xz
cd openwrt-sdk-*

# Update feeds
./scripts/feeds update -a
./scripts/feeds install -a
```

#### Step 2: Add LuCI App Source

```bash
# Clone the Friendly LwM2M Client repository
cd package
git clone https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git

# Create symlink for LuCI app
ln -s Friendly-LwM2M-Client/openwrt/luci luci-app-lwm2m
```

#### Step 3: Configure and Build

```bash
# Return to SDK root
cd ..

# Configure build
make menuconfig

# Navigate to: LuCI > 3. Applications > luci-app-lwm2m
# Press 'M' to select as module
# Save and exit

# Build the package
make package/luci-app-lwm2m/compile V=s

# Find the built package
find bin/packages -name "luci-app-lwm2m*.ipk"
```

#### Step 4: Install on Device

```bash
# Copy package to OpenWRT device
scp bin/packages/*/luci/luci-app-lwm2m*.ipk root@192.168.1.1:/tmp/

# SSH to device
ssh root@192.168.1.1

# Install
cd /tmp
opkg install luci-app-lwm2m*.ipk

# Restart services
/etc/init.d/rpcd restart
rm -rf /tmp/luci-indexcache /tmp/luci-modulecache
/etc/init.d/uhttpd restart
```

### Method 3: Manual Installation (Development)

For development or testing without building a package:

#### Step 1: Prepare Files on Device

```bash
# SSH to OpenWRT device
ssh root@192.168.1.1

# Create directories
mkdir -p /usr/lib/lua/luci/controller
mkdir -p /usr/lib/lua/luci/model/cbi/lwm2m
mkdir -p /usr/lib/lua/luci/view/lwm2m
mkdir -p /usr/share/rpcd/acl.d
mkdir -p /usr/share/luci/menu.d
```

#### Step 2: Copy Files

On your development machine:

```bash
cd /path/to/Friendly-LwM2M-Client/openwrt/luci

# Copy controller
scp luasrc/controller/lwm2m.lua root@192.168.1.1:/usr/lib/lua/luci/controller/

# Copy CBI models
scp luasrc/model/cbi/lwm2m/*.lua root@192.168.1.1:/usr/lib/lua/luci/model/cbi/lwm2m/

# Copy views
scp luasrc/view/lwm2m/*.htm root@192.168.1.1:/usr/lib/lua/luci/view/lwm2m/

# Copy ACL
scp root/usr/share/rpcd/acl.d/luci-app-lwm2m.json root@192.168.1.1:/usr/share/rpcd/acl.d/
```

#### Step 3: Create Menu Entry

```bash
# On OpenWRT device
cat > /usr/share/luci/menu.d/luci-app-lwm2m.json << 'EOF'
{
	"admin/services/lwm2m": {
		"title": "LwM2M Client",
		"order": 60,
		"action": {
			"type": "alias",
			"path": "admin/services/lwm2m/status"
		},
		"depends": {
			"acl": [ "luci-app-lwm2m" ]
		}
	}
}
EOF
```

#### Step 4: Restart Services

```bash
# Clear caches and restart
rm -rf /tmp/luci-indexcache /tmp/luci-modulecache
/etc/init.d/rpcd restart
/etc/init.d/uhttpd restart
```

### Method 4: Using Image Builder

Build a complete OpenWRT image with LuCI app included:

```bash
# Download Image Builder
cd ~/
wget https://downloads.openwrt.org/releases/23.05.0/targets/<arch>/generic/openwrt-imagebuilder-*.tar.xz
tar xf openwrt-imagebuilder-*.tar.xz
cd openwrt-imagebuilder-*

# Add custom packages feed
echo "src/gz custom file:///path/to/packages" >> repositories.conf

# Build image
make image PROFILE=<your-device> \
    PACKAGES="friendly-lwm2m-client luci-app-lwm2m luci luci-ssl-openssl"

# Flash the resulting image
```

## Post-Installation Verification

### 1. Check Package Installation

```bash
opkg list-installed | grep lwm2m
```

Expected output:
```
friendly-lwm2m-client - 1.0.0
luci-app-lwm2m - 1.0.0
```

### 2. Verify Files

```bash
# Check controller
ls -l /usr/lib/lua/luci/controller/lwm2m.lua

# Check CBI models
ls -l /usr/lib/lua/luci/model/cbi/lwm2m/

# Check views
ls -l /usr/lib/lua/luci/view/lwm2m/

# Check ACL
ls -l /usr/share/rpcd/acl.d/luci-app-lwm2m.json
```

### 3. Test ubus Access

```bash
# Check if lwm2m ubus object is available
ubus list | grep lwm2m

# Call status method
ubus call lwm2m status
```

### 4. Access Web Interface

1. Open browser: `http://192.168.1.1` (or your router IP)
2. Login with credentials
3. Navigate to **Services** menu
4. Look for **LwM2M Client** entry
5. Click to access the interface

## Troubleshooting

### Issue: Menu Entry Not Appearing

**Solution:**
```bash
# Clear LuCI cache
rm -rf /tmp/luci-indexcache /tmp/luci-modulecache

# Rebuild index
lua -e "require('luci.dispatcher').createindex()"

# Restart uhttpd
/etc/init.d/uhttpd restart

# Hard refresh browser (Ctrl+Shift+R)
```

### Issue: Permission Denied

**Solution:**
```bash
# Check ACL file exists
cat /usr/share/rpcd/acl.d/luci-app-lwm2m.json

# Restart rpcd
/etc/init.d/rpcd restart

# Check session permissions
ubus call session list
```

### Issue: "Failed to connect to ubus" Error

**Solution:**
```bash
# Ensure LwM2M client is installed
opkg list-installed | grep friendly-lwm2m-client

# Start the service
/etc/init.d/lwm2m start

# Check service status
/etc/init.d/lwm2m status

# Test ubus manually
ubus call lwm2m status
```

### Issue: Status Page Not Updating

**Solution:**
1. Check browser console for JavaScript errors (F12)
2. Verify service is running: `/etc/init.d/lwm2m status`
3. Test AJAX endpoint directly: `curl http://localhost/cgi-bin/luci/admin/services/lwm2m/status_json`
4. Check browser network tab for failed requests
5. Try different browser or clear browser cache

### Issue: Configuration Not Saving

**Solution:**
```bash
# Check UCI file permissions
ls -l /etc/config/lwm2m
chmod 644 /etc/config/lwm2m

# Test UCI manually
uci show lwm2m
uci set lwm2m.general.log_level=7
uci commit lwm2m

# Check for UCI lock files
ls -la /var/lock/*.lock
rm /var/lock/uci.lock  # if exists
```

## Uninstallation

### Remove Package

```bash
# Remove LuCI app
opkg remove luci-app-lwm2m

# Clear cache
rm -rf /tmp/luci-indexcache /tmp/luci-modulecache

# Restart services
/etc/init.d/rpcd restart
/etc/init.d/uhttpd restart
```

### Manual Cleanup (if needed)

```bash
# Remove files
rm -f /usr/lib/lua/luci/controller/lwm2m.lua
rm -rf /usr/lib/lua/luci/model/cbi/lwm2m
rm -rf /usr/lib/lua/luci/view/lwm2m
rm -f /usr/share/rpcd/acl.d/luci-app-lwm2m.json
rm -f /usr/share/luci/menu.d/luci-app-lwm2m.json

# Clear cache
rm -rf /tmp/luci-indexcache /tmp/luci-modulecache
```

## Upgrading

### From Package

```bash
opkg update
opkg upgrade luci-app-lwm2m
rm -rf /tmp/luci-indexcache /tmp/luci-modulecache
/etc/init.d/rpcd restart
/etc/init.d/uhttpd restart
```

### Manual Upgrade

1. Backup current configuration: `uci export lwm2m > /tmp/lwm2m-backup.conf`
2. Remove old files (see Uninstallation)
3. Install new files (see Manual Installation)
4. Restore configuration: `uci import lwm2m < /tmp/lwm2m-backup.conf`
5. Restart services

## Security Notes

1. **HTTPS**: Enable HTTPS for LuCI in production:
   ```bash
   opkg install luci-ssl-openssl
   /etc/init.d/uhttpd restart
   ```

2. **Firewall**: Restrict LuCI access:
   ```bash
   # Allow only from LAN
   uci set uhttpd.main.listen_http='192.168.1.1:80'
   uci set uhttpd.main.listen_https='192.168.1.1:443'
   uci commit uhttpd
   /etc/init.d/uhttpd restart
   ```

3. **Strong Password**: Change default root password:
   ```bash
   passwd
   ```

## Getting Help

- **Documentation**: See README.md in this directory
- **GitHub Issues**: https://github.com/Friendly-Technologies/Friendly-LwM2M-Client/issues
- **OpenWRT Forum**: https://forum.openwrt.org/
- **LwM2M Server Logs**: Check server-side logs for connection issues

## Next Steps

After installation:

1. Configure general settings (endpoint name, network)
2. Add LwM2M server configuration
3. Configure security credentials
4. Start the service
5. Monitor status page for registration

See README.md for detailed configuration guide.
