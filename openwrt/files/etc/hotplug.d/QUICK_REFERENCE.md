# LwM2M Hotplug Handlers - Quick Reference

## File Locations

```bash
/etc/hotplug.d/iface/99-lwm2m      # Network interfaces (ifup/ifdown)
/etc/hotplug.d/usb/99-lwm2m        # USB devices (add/remove)
/etc/hotplug.d/net/99-lwm2m        # Network devices (add/remove)
/etc/hotplug.d/button/99-lwm2m     # Button events (pressed/released)
```

## Quick Test Commands

```bash
# Test interface handler
ACTION=ifup INTERFACE=wan DEVICE=eth1 /etc/hotplug.d/iface/99-lwm2m

# Test USB handler
ACTION=add DEVTYPE=usb_device PRODUCT=0951/1666/0110 TYPE=8/6/80 \
  BUSNUM=1 DEVNUM=2 DEVPATH=/devices/platform/usb/1-1 \
  /etc/hotplug.d/usb/99-lwm2m

# Test network device handler
ACTION=add DEVICENAME=eth0 /etc/hotplug.d/net/99-lwm2m

# Test button handler
ACTION=pressed BUTTON=reset SEEN=0 /etc/hotplug.d/button/99-lwm2m
```

## Logging Commands

```bash
logread | grep lwm2m-hotplug           # View all hotplug logs
logread -f | grep lwm2m-hotplug        # Follow logs in real-time
logread | grep lwm2m-hotplug | grep err # Errors only
```

## ubus Commands

```bash
# Subscribe to events
ubus subscribe lwm2m.*
ubus subscribe lwm2m.interface.*
ubus subscribe lwm2m.usb.*
ubus subscribe lwm2m.button.*

# Call methods
ubus call lwm2m status
ubus call lwm2m update
ubus call lwm2m resource_read '{"object_id":4,"instance_id":0,"resource_id":0}'
```

## LwM2M Objects Updated

| Handler | Objects Updated |
|---------|----------------|
| `iface/99-lwm2m` | Object 4 (Connectivity), Object 34602 (Ethernet) |
| `usb/99-lwm2m` | Object 34604 (USB), Object 34605 (Storage) |
| `net/99-lwm2m` | Object 12 (WLAN), Object 34602 (Ethernet), Object 4 (Connectivity) |
| `button/99-lwm2m` | Object 34603 (GPIO) |

## State Files

```bash
/var/run/lwm2m/button_*_state      # Button state
/var/run/lwm2m/button_stats        # Button press counts
```

## Enable/Disable

```bash
# Enable
uci set lwm2m.general.enabled='1'
uci commit lwm2m

# Disable
uci set lwm2m.general.enabled='0'
uci commit lwm2m
```

## Troubleshooting

```bash
# Check if scripts are executable
ls -la /etc/hotplug.d/*/99-lwm2m

# Check if LwM2M client is running
ps | grep lwm2m-client

# Check if ubus object exists
ubus list | grep lwm2m

# Restart LwM2M client
/etc/init.d/lwm2m restart
```

## Event Data Examples

### Interface Event
```json
{
  "interface": "wan",
  "action": "ifup",
  "device": "eth1"
}
```

### USB Event
```json
{
  "action": "add",
  "bus": "1",
  "device": "2",
  "vendor_id": "0951",
  "product_id": "1666",
  "device_type": "mass_storage",
  "manufacturer": "Kingston",
  "product": "DataTraveler 3.0"
}
```

### Network Device Event
```json
{
  "action": "add",
  "device": "eth0",
  "mac_address": "00:11:22:33:44:55",
  "speed": "1000",
  "duplex": "full",
  "operstate": "up"
}
```

### Button Event
```json
{
  "button": "reset",
  "action": "released",
  "duration": 2,
  "press_type": "medium",
  "timestamp": 1704707200
}
```

## Installation

```bash
# During package build (automatic)
make package/lwm2m-client/compile

# Manual installation
scp -r openwrt/files/etc/hotplug.d/* root@openwrt:/etc/hotplug.d/
ssh root@openwrt "chmod +x /etc/hotplug.d/*/99-lwm2m"
```

## Documentation

- [README.md](README.md) - Complete documentation
- [HOTPLUG_INTEGRATION.md](../../HOTPLUG_INTEGRATION.md) - Integration guide
- [OPENWRT_INTEGRATION.md](../../../docs/OPENWRT_INTEGRATION.md) - OpenWRT guide
