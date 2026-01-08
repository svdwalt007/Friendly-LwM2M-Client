# LwM2M Client OpenWRT Hotplug Handlers

Comprehensive hotplug event handlers for integrating the LwM2M client with OpenWRT's hardware event system.

## Overview

These hotplug scripts enable the LwM2M client to automatically respond to hardware and network events on OpenWRT devices. They provide seamless integration between the operating system's event system and LwM2M object model.

## Architecture

```
OpenWRT System Events
        │
        ├─── Network Interface (ifup/ifdown)
        │    └─── /etc/hotplug.d/iface/99-lwm2m
        │         ├─── Connectivity Monitoring (Object 4)
        │         └─── Ethernet Interface (Object 34602)
        │
        ├─── USB Devices (add/remove)
        │    └─── /etc/hotplug.d/usb/99-lwm2m
        │         ├─── USB Management (Object 34604)
        │         ├─── Storage Management (Object 34605)
        │         └─── Connectivity Monitoring (Object 4)
        │
        ├─── Network Devices (add/remove)
        │    └─── /etc/hotplug.d/net/99-lwm2m
        │         ├─── Ethernet Interface (Object 34602)
        │         ├─── WLAN Connectivity (Object 12)
        │         └─── Connectivity Monitoring (Object 4)
        │
        └─── Button Events (pressed/released)
             └─── /etc/hotplug.d/button/99-lwm2m
                  ├─── GPIO Control (Object 34603)
                  └─── Special Actions (factory reset, WPS, etc.)
```

## Installed Scripts

### 1. Network Interface Handler
**File:** `/etc/hotplug.d/iface/99-lwm2m`

**Triggers:**
- Interface brought up (`ifup`)
- Interface brought down (`ifdown`)

**Environment Variables:**
- `ACTION`: "ifup" or "ifdown"
- `INTERFACE`: Interface name (e.g., "wan", "lan")
- `DEVICE`: Physical device name (e.g., "eth0", "wlan0")

**LwM2M Objects Updated:**
- **Object 4** (Connectivity Monitoring): Network bearer, IP addresses
- **Object 34602** (Ethernet Interface): Link status, interface state

**Example Events:**
```bash
# WAN interface comes up
ACTION=ifup INTERFACE=wan DEVICE=eth1

# WiFi interface goes down
ACTION=ifdown INTERFACE=wlan DEVICE=wlan0
```

**ubus Events Sent:**
- `lwm2m.interface.ifup`
- `lwm2m.interface.ifdown`
- `lwm2m.ethernet.ifup`
- `lwm2m.ethernet.ifdown`

---

### 2. USB Device Handler
**File:** `/etc/hotplug.d/usb/99-lwm2m`

**Triggers:**
- USB device connected (`add`)
- USB device disconnected (`remove`)

**Environment Variables:**
- `ACTION`: "add" or "remove"
- `DEVTYPE`: "usb_device" or "usb_interface"
- `PRODUCT`: USB product ID (VID/PID/version)
- `TYPE`: Device class/subclass/protocol
- `BUSNUM`: USB bus number
- `DEVNUM`: USB device number
- `DEVPATH`: sysfs device path

**LwM2M Objects Updated:**
- **Object 34604** (USB Management): USB device list, status
- **Object 34605** (Storage Management): USB storage devices
- **Object 4** (Connectivity Monitoring): USB modems/cellular

**Example Events:**
```bash
# USB flash drive connected
ACTION=add DEVTYPE=usb_device PRODUCT=0951/1666/0110 TYPE=0/0/0

# USB modem disconnected
ACTION=remove DEVTYPE=usb_device PRODUCT=12d1/1506/0102 TYPE=2/0/0
```

**ubus Events Sent:**
- `lwm2m.usb.add`
- `lwm2m.usb.remove`
- `lwm2m.storage.attached`
- `lwm2m.modem.add`
- `lwm2m.modem.remove`

**USB Device Type Detection:**
| Class Code | Device Type |
|------------|-------------|
| 00 | Composite |
| 01 | Audio |
| 02 | Communications/Modem |
| 03 | HID (Human Interface Device) |
| 06 | Image/Camera |
| 07 | Printer |
| 08 | Mass Storage |
| 09 | USB Hub |
| 0e | Video |
| e0 | Wireless Controller |
| ff | Vendor Specific |

---

### 3. Network Device Handler
**File:** `/etc/hotplug.d/net/99-lwm2m`

**Triggers:**
- Network device added to system (`add`)
- Network device removed from system (`remove`)

**Environment Variables:**
- `ACTION`: "add" or "remove"
- `DEVICENAME`: Network device name (e.g., "eth0", "wlan0")
- `INTERFACE`: Logical interface name
- `DEVPATH`: sysfs device path

**LwM2M Objects Updated:**
- **Object 34602** (Ethernet Interface): Ethernet port status
- **Object 12** (WLAN Connectivity): WiFi interface status
- **Object 4** (Connectivity Monitoring): Primary bearer detection

**Example Events:**
```bash
# Ethernet port added
ACTION=add DEVICENAME=eth0

# WiFi device removed
ACTION=remove DEVICENAME=wlan0
```

**ubus Events Sent:**
- `lwm2m.netdev.ethernet.add`
- `lwm2m.netdev.ethernet.remove`
- `lwm2m.netdev.wireless.add`
- `lwm2m.netdev.wireless.remove`
- `lwm2m.connectivity.bearer.add`
- `lwm2m.connectivity.bearer.remove`

**Network Device Type Detection:**
| Device Pattern | Type |
|----------------|------|
| eth* | Ethernet |
| wlan*, wl*, ath*, ra* | Wireless |
| usb* | USB Ethernet |
| wwan*, 3g-*, lte-* | Cellular |
| ppp* | PPP |
| br-* | Bridge |
| tun*, tap* | Tunnel |

---

### 4. Button Event Handler
**File:** `/etc/hotplug.d/button/99-lwm2m`

**Triggers:**
- Button pressed (`pressed`)
- Button released (`released`)
- Long press timeout (`timeout`)

**Environment Variables:**
- `ACTION`: "pressed", "released", or "timeout"
- `BUTTON`: Button name (e.g., "reset", "wps", "power")
- `SEEN`: Duration button was pressed (in seconds)
- `SEQNUM`: Event sequence number

**LwM2M Objects Updated:**
- **Object 34603** (GPIO Control): Button state, press count

**Example Events:**
```bash
# Reset button pressed
ACTION=pressed BUTTON=reset SEEN=0

# Reset button released after 2 seconds
ACTION=released BUTTON=reset SEEN=2

# WPS button pressed
ACTION=pressed BUTTON=wps SEEN=0
```

**ubus Events Sent:**
- `lwm2m.button.pressed`
- `lwm2m.button.released`
- `lwm2m.button.wps`
- `lwm2m.system.factory_reset_trigger`
- `lwm2m.system.shutdown_trigger`

**Button Press Types:**
| Duration | Type | Description |
|----------|------|-------------|
| < 1s | Short | Quick press |
| 1-3s | Medium | Brief hold |
| 3-10s | Long | Long press |
| > 10s | Very Long | Extended hold (factory reset) |

**Special Button Actions:**
- **Reset Button** (very long press ≥10s): Factory reset trigger
- **WPS Button** (any press): WiFi Protected Setup trigger
- **Power Button** (long press ≥3s): Shutdown trigger

**State Files:**
Button events create state files in `/var/run/lwm2m/`:
- `/var/run/lwm2m/button_<name>_state`: Current button state
- `/var/run/lwm2m/button_stats`: Press count statistics

---

## Installation

### Automatic Installation
When building the OpenWRT package, these scripts are automatically installed:

```bash
cd openwrt
make package/lwm2m-client/compile
opkg install lwm2m-client_*.ipk
```

### Manual Installation
To install manually on an existing OpenWRT system:

```bash
# Copy hotplug scripts
scp -r openwrt/files/etc/hotplug.d/* root@openwrt:/etc/hotplug.d/

# Make scripts executable
ssh root@openwrt "chmod +x /etc/hotplug.d/iface/99-lwm2m"
ssh root@openwrt "chmod +x /etc/hotplug.d/usb/99-lwm2m"
ssh root@openwrt "chmod +x /etc/hotplug.d/net/99-lwm2m"
ssh root@openwrt "chmod +x /etc/hotplug.d/button/99-lwm2m"

# Restart procd to load hotplug handlers
ssh root@openwrt "/etc/init.d/procd restart"
```

---

## Configuration

### Enable/Disable Hotplug Handlers

The handlers check if the LwM2M client is enabled before processing events:

```bash
# Enable LwM2M client (enables hotplug handlers)
uci set lwm2m.general.enabled='1'
uci commit lwm2m

# Disable LwM2M client (disables hotplug handlers)
uci set lwm2m.general.enabled='0'
uci commit lwm2m
```

### Configure Logging

Hotplug events are logged to syslog with the tag `lwm2m-hotplug`:

```bash
# View hotplug event logs
logread | grep lwm2m-hotplug

# View only error logs
logread | grep lwm2m-hotplug | grep err

# Follow logs in real-time
logread -f | grep lwm2m-hotplug
```

**Log Levels:**
- `daemon.info`: Informational messages (normal events)
- `daemon.err`: Error messages (failures)

---

## ubus Integration

All hotplug handlers use ubus for communication with the LwM2M client daemon.

### ubus Object
**Name:** `lwm2m`

### ubus Events
Hotplug handlers send events that the LwM2M client can subscribe to:

```bash
# Subscribe to all LwM2M events
ubus subscribe lwm2m.*

# Subscribe to specific event types
ubus subscribe lwm2m.interface.*
ubus subscribe lwm2m.usb.*
ubus subscribe lwm2m.button.*
```

### ubus Methods
Hotplug handlers may call these ubus methods:

```bash
# Trigger LwM2M registration update
ubus call lwm2m update

# Get LwM2M client status
ubus call lwm2m status

# Read resource value
ubus call lwm2m resource_read '{"object_id":4, "instance_id":0, "resource_id":0}'

# Write resource value
ubus call lwm2m resource_write '{"object_id":34603, "instance_id":0, "resource_id":5, "value":"1"}'
```

---

## Testing

### Test Network Interface Events

```bash
# Trigger interface up event
ifup wan

# Trigger interface down event
ifdown wan

# Monitor logs
logread -f | grep lwm2m-hotplug
```

### Test USB Events

```bash
# Connect USB device (hardware)
# Or simulate with hotplug-call:

hotplug-call usb <<EOF
ACTION=add
DEVTYPE=usb_device
PRODUCT=0951/1666/0110
TYPE=0/0/0
BUSNUM=1
DEVNUM=2
DEVPATH=/devices/platform/soc/1e1c0000.xhci/usb1/1-1
EOF

# Check logs
logread | grep lwm2m-hotplug-usb
```

### Test Network Device Events

```bash
# Simulate network device add
hotplug-call net <<EOF
ACTION=add
DEVICENAME=eth0
EOF

# Check logs
logread | grep lwm2m-hotplug-net
```

### Test Button Events

```bash
# Simulate button press
hotplug-call button <<EOF
ACTION=pressed
BUTTON=reset
SEEN=0
EOF

# Simulate button release after 2 seconds
hotplug-call button <<EOF
ACTION=released
BUTTON=reset
SEEN=2
EOF

# Check logs
logread | grep lwm2m-hotplug-button

# Check button state file
cat /var/run/lwm2m/button_reset_state

# Check button statistics
cat /var/run/lwm2m/button_stats
```

---

## Debugging

### Enable Debug Logging

Modify the scripts to add debug output:

```bash
# Add at the beginning of any handler
set -x  # Enable shell debug mode

# Or add debug logging function
debug_log() {
    logger -t lwm2m-hotplug-debug -p daemon.debug "$@"
}
```

### Check Hotplug Execution

```bash
# Monitor all hotplug events
logread -f | grep hotplug

# Test if scripts are executable
ls -la /etc/hotplug.d/*/99-lwm2m

# Expected output:
# -rwxr-xr-x 1 root root ... /etc/hotplug.d/iface/99-lwm2m
# -rwxr-xr-x 1 root root ... /etc/hotplug.d/usb/99-lwm2m
# -rwxr-xr-x 1 root root ... /etc/hotplug.d/net/99-lwm2m
# -rwxr-xr-x 1 root root ... /etc/hotplug.d/button/99-lwm2m
```

### Check ubus Availability

```bash
# List ubus objects
ubus list | grep lwm2m

# Expected output:
# lwm2m

# Check ubus methods
ubus -v list lwm2m

# Test ubus call
ubus call lwm2m status
```

### Manual Hotplug Testing

Test handlers manually without actual hardware events:

```bash
# Test interface handler
ACTION=ifup INTERFACE=wan DEVICE=eth1 /etc/hotplug.d/iface/99-lwm2m

# Test USB handler
ACTION=add DEVTYPE=usb_device PRODUCT=0951/1666/0110 \
  TYPE=0/0/0 BUSNUM=1 DEVNUM=2 \
  DEVPATH=/devices/platform/soc/usb/1-1 \
  /etc/hotplug.d/usb/99-lwm2m

# Test network device handler
ACTION=add DEVICENAME=eth0 /etc/hotplug.d/net/99-lwm2m

# Test button handler
ACTION=pressed BUTTON=reset SEEN=0 /etc/hotplug.d/button/99-lwm2m
```

---

## Performance Considerations

### Event Processing Time
Hotplug handlers are designed to execute quickly (< 1 second) to avoid blocking the hotplug queue.

**Optimization techniques:**
- Asynchronous ubus calls
- Background processing for long-running tasks
- Caching frequently accessed data

### Resource Usage
Each handler runs as a separate process and consumes minimal resources:
- **Memory:** ~1-2 MB per handler instance
- **CPU:** Negligible (< 1% during event processing)

### Event Rate Limiting
For high-frequency events (e.g., network flapping), the handlers include built-in delays:
- Network stabilization delay: 2 seconds
- USB enumeration delay: 1 second

---

## Security Considerations

### Input Validation
All handlers validate environment variables before processing to prevent injection attacks.

### Permission Requirements
Handlers run with root privileges (required for hotplug scripts) but:
- Sanitize all inputs
- Use safe shell practices (quoted variables)
- Avoid executing user-provided data

### State File Security
State files are created with restrictive permissions:
```bash
# State directory
mkdir -p /var/run/lwm2m
chmod 755 /var/run/lwm2m

# State files
touch /var/run/lwm2m/button_reset_state
chmod 644 /var/run/lwm2m/button_reset_state
```

---

## Integration with LwM2M Client

### Event Notification Flow

```
1. Hardware Event Occurs
   │
   ├─→ OpenWRT Hotplug System
       │
       ├─→ Hotplug Handler Script (99-lwm2m)
           │
           ├─→ Parse Event Data
           ├─→ Validate Environment
           ├─→ Check if LwM2M Client Running
           │
           ├─→ Send ubus Event
           │   └─→ LwM2M Client Receives Event
           │       └─→ Update Object Resources
           │           └─→ Notify LwM2M Server
           │
           ├─→ Call ubus Method (optional)
           │   └─→ Trigger Registration Update
           │
           └─→ Write State File
               └─→ LwM2M Client Reads State
```

### Client-Side Implementation

The LwM2M client should implement ubus event subscribers:

```cpp
// Subscribe to hotplug events
UbusInterface ubus;
ubus.init();
ubus.subscribeEvent("lwm2m.interface.*", [](const std::string& data) {
    // Parse JSON data
    // Update Connectivity Monitoring object
});

ubus.subscribeEvent("lwm2m.usb.*", [](const std::string& data) {
    // Parse JSON data
    // Update USB Management object
});

ubus.subscribeEvent("lwm2m.button.*", [](const std::string& data) {
    // Parse JSON data
    // Update GPIO Control object
});
```

---

## Troubleshooting

### Handler Not Executing

**Problem:** Hotplug handler doesn't run when event occurs

**Solutions:**
1. Check if script is executable:
   ```bash
   ls -la /etc/hotplug.d/*/99-lwm2m
   chmod +x /etc/hotplug.d/*/99-lwm2m
   ```

2. Check if LwM2M client is enabled:
   ```bash
   uci get lwm2m.general.enabled
   uci set lwm2m.general.enabled='1'
   uci commit lwm2m
   ```

3. Check for syntax errors:
   ```bash
   sh -n /etc/hotplug.d/iface/99-lwm2m
   ```

### ubus Communication Failure

**Problem:** Handler can't communicate with LwM2M client

**Solutions:**
1. Check if LwM2M client is running:
   ```bash
   ps | grep lwm2m-client
   /etc/init.d/lwm2m start
   ```

2. Check if ubus object exists:
   ```bash
   ubus list | grep lwm2m
   ```

3. Check ubus permissions:
   ```bash
   ls -la /var/run/ubus.sock
   ```

### Missing Environment Variables

**Problem:** Handler reports missing ACTION or other variables

**Solutions:**
1. Check hotplug environment:
   ```bash
   hotplug-call --help
   ```

2. Test with manual variables:
   ```bash
   ACTION=add DEVICENAME=eth0 /etc/hotplug.d/net/99-lwm2m
   ```

---

## File Locations

```
/etc/hotplug.d/
├── iface/
│   └── 99-lwm2m              # Network interface handler
├── usb/
│   └── 99-lwm2m              # USB device handler
├── net/
│   └── 99-lwm2m              # Network device handler
├── button/
│   └── 99-lwm2m              # Button event handler
└── README.md                 # This file

/var/run/lwm2m/               # Runtime state directory
├── button_*_state            # Button state files
└── button_stats              # Button press statistics

/var/log/                     # Log files (via syslog/logread)
```

---

## References

- [OpenWRT Hotplug Documentation](https://openwrt.org/docs/guide-user/base-system/hotplug)
- [OpenWRT ubus Documentation](https://openwrt.org/docs/techref/ubus)
- [LwM2M Object Specification](https://technical.openmobilealliance.org/OMNA/LwM2M/LwM2MRegistry.html)
- [OpenWRT Network Configuration](https://openwrt.org/docs/guide-user/base-system/basic-networking)

---

## License

Copyright (C) 2024 Friendly Technologies

This software is provided under the terms specified in the project LICENSE file.
