# OpenWRT Hotplug Architecture for LwM2M Client

## System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         OpenWRT System Events                           │
└────────────────────────────────┬────────────────────────────────────────┘
                                 │
                 ┌───────────────┼───────────────┐
                 │               │               │
        ┌────────▼────────┐ ┌───▼────┐ ┌───────▼────────┐
        │  Network Events │ │  USB   │ │ Button Events  │
        │  (iface + net)  │ │ Events │ │                │
        └────────┬────────┘ └───┬────┘ └───────┬────────┘
                 │              │              │
        ┌────────▼────────┐ ┌──▼───┐  ┌───────▼────────┐
        │ 99-lwm2m (iface)│ │99-lwm2m│ │ 99-lwm2m (button)│
        │ 99-lwm2m (net)  │ │ (usb) │ │                │
        └────────┬────────┘ └───┬────┘ └───────┬────────┘
                 │              │              │
                 └──────────────┼──────────────┘
                                │
                     ┌──────────▼──────────┐
                     │  ubus Message Bus   │
                     │                     │
                     │  Events:            │
                     │  - interface.*      │
                     │  - usb.*            │
                     │  - button.*         │
                     │  - storage.*        │
                     │                     │
                     │  Methods:           │
                     │  - status           │
                     │  - update           │
                     │  - resource_read    │
                     │  - resource_write   │
                     └──────────┬──────────┘
                                │
                     ┌──────────▼──────────┐
                     │  LwM2M Client       │
                     │  Daemon             │
                     │                     │
                     │  - UbusInterface    │
                     │  - Event Handlers   │
                     │  - Object Registry  │
                     └──────────┬──────────┘
                                │
                ┌───────────────┼───────────────┐
                │               │               │
        ┌───────▼────────┐ ┌───▼────┐ ┌───────▼────────┐
        │   Object 4     │ │Obj 12  │ │  Object 34602  │
        │ Connectivity   │ │ WLAN   │ │   Ethernet     │
        └────────────────┘ └────────┘ └────────────────┘
                │               │               │
        ┌───────▼────────┐ ┌───▼────┐ ┌───────▼────────┐
        │  Object 34603  │ │Obj     │ │  Object 34605  │
        │   GPIO/Button  │ │ 34604  │ │    Storage     │
        │                │ │  USB   │ │                │
        └────────────────┘ └────────┘ └────────────────┘
                                │
                     ┌──────────▼──────────┐
                     │  LwM2M Server       │
                     │  (CoAP/CoAPS)       │
                     │                     │
                     │  Receives:          │
                     │  - Resource updates │
                     │  - Observations     │
                     │  - Notifications    │
                     └─────────────────────┘
```

## Event Flow Examples

### Example 1: Network Interface Comes Up

```
1. User executes: ifup wan
                     │
2. OpenWRT procd triggers hotplug
                     │
3. /etc/hotplug.d/iface/99-lwm2m executes
   - Reads: ACTION=ifup, INTERFACE=wan, DEVICE=eth1
   - Validates environment
   - Checks if LwM2M client running
                     │
4. Sends ubus event: lwm2m.interface.ifup
   - Data: {"interface":"wan","action":"ifup","device":"eth1"}
                     │
5. Calls ubus method: lwm2m.update
   - Triggers registration update
                     │
6. LwM2M Client receives event
   - Updates Object 4 (Connectivity Monitoring)
     * Network bearer: 41 (Ethernet)
     * IP addresses: from ifconfig
   - Updates Object 34602 (Ethernet Interface)
     * Link status: up
     * Speed: 1000 Mbps
     * Duplex: full
                     │
7. LwM2M Client sends notification to server
   - CoAP NOTIFY for observed resources
   - Server receives updated network status
```

### Example 2: USB Storage Device Connected

```
1. User plugs in USB flash drive
                     │
2. Linux kernel detects device
   - Creates /dev/sda, /dev/sda1
                     │
3. OpenWRT hotplug triggered
                     │
4. /etc/hotplug.d/usb/99-lwm2m executes
   - Reads: ACTION=add, DEVTYPE=usb_device
   - PRODUCT=0951/1666/0110 (Kingston DataTraveler)
   - TYPE=0/0/0 (USB Mass Storage)
                     │
5. Script parses device information
   - Vendor ID: 0x0951 (Kingston)
   - Product ID: 0x1666
   - Device type: mass_storage
                     │
6. Sends ubus event: lwm2m.usb.add
   - Data: {"action":"add","vendor_id":"0951",...}
                     │
7. Waits 1 second for enumeration
                     │
8. Detects block device /dev/sda1
                     │
9. Sends ubus event: lwm2m.storage.attached
   - Data: {"devices":"sda,sda1","usb_device":"1:2"}
                     │
10. LwM2M Client receives events
    - Updates Object 34604 (USB Management)
      * USB port: 1:2
      * Vendor: Kingston
      * Status: connected
    - Updates Object 34605 (Storage Management)
      * Device: /dev/sda1
      * Type: USB
      * Size: from df command
                     │
11. LwM2M Client notifies server
    - New USB device available
    - Storage capacity information
```

### Example 3: Reset Button Long Press (Factory Reset)

```
1. User presses reset button
                     │
2. GPIO driver detects button press
                     │
3. /etc/hotplug.d/button/99-lwm2m executes
   - ACTION=pressed, BUTTON=reset, SEEN=0
                     │
4. Sends ubus event: lwm2m.button.pressed
   - Data: {"button":"reset","action":"pressed",...}
                     │
5. Turns on status LED (red)
                     │
6. User holds button for 12 seconds
                     │
7. GPIO driver triggers release event
                     │
8. /etc/hotplug.d/button/99-lwm2m executes
   - ACTION=released, BUTTON=reset, SEEN=12
                     │
9. Classifies press type: very_long (≥10s)
                     │
10. Sends ubus event: lwm2m.button.released
    - Data: {"button":"reset","duration":12,"press_type":"very_long"}
                     │
11. Sends special event: lwm2m.system.factory_reset_trigger
    - Data: {"trigger":"factory_reset","duration":12}
                     │
12. Turns off status LED
                     │
13. Writes state file: /var/run/lwm2m/button_reset_state
                     │
14. Updates statistics: /var/run/lwm2m/button_stats
                     │
15. LwM2M Client receives events
    - Updates Object 34603 (GPIO Control)
      * Button state: released
      * Last press: 12 seconds
      * Press count: incremented
                     │
16. LwM2M Client triggers factory reset
    - Sends notification to server
    - Initiates reset procedure
```

## State Management

### Persistent State
```
/etc/config/lwm2m
├── general.enabled          # Enable/disable hotplug handlers
├── network.*                # Network configuration
└── device.*                 # Device information
```

### Runtime State
```
/var/run/lwm2m/
├── button_reset_state       # Last reset button event
├── button_wps_state         # Last WPS button event
├── button_power_state       # Last power button event
└── button_stats             # Press count for all buttons
```

### sysfs State (Read-Only)
```
/sys/class/net/eth0/
├── address                  # MAC address
├── speed                    # Link speed (Mbps)
├── duplex                   # full/half
├── operstate                # up/down
└── statistics/
    ├── rx_bytes
    ├── tx_bytes
    ├── rx_packets
    └── tx_packets

/sys/class/leds/
├── led:red:status/
│   └── brightness           # 0=off, 1=on
└── led:green:power/
    └── brightness

/sys/class/gpio/
└── gpio12/
    ├── value                # Button state
    └── direction            # in/out
```

## Performance Characteristics

### Execution Times
```
Network Interface Handler:  < 0.5s
USB Device Handler:         < 1.0s (with enumeration delay)
Network Device Handler:     < 0.8s (with wireless scan)
Button Handler:             < 0.2s
```

### Resource Usage
```
Memory per handler:         ~1-2 MB
CPU usage:                  < 1% (during event)
ubus message size:          < 1 KB
Log message rate:           1-5 messages per event
```

### Event Processing Rate
```
Maximum event rate:         100 events/second
Typical event rate:         1-10 events/minute
Event queue depth:          Unlimited (handled by procd)
```

## Security Model

### Process Isolation
```
┌─────────────────────────────────┐
│  Hotplug Handler (root)         │
│  - Runs as root (required)      │
│  - Limited execution time       │
│  - Input validation             │
│  - Safe shell practices         │
└───────────┬─────────────────────┘
            │ ubus
┌───────────▼─────────────────────┐
│  LwM2M Client (nobody)          │
│  - Runs as unprivileged user    │
│  - Restricted file access       │
│  - Capability-based security    │
└───────────┬─────────────────────┘
            │ CoAP
┌───────────▼─────────────────────┐
│  LwM2M Server (remote)          │
│  - TLS/DTLS encryption          │
│  - Authentication (PSK/Cert)    │
│  - Authorization (ACL)          │
└─────────────────────────────────┘
```

### Input Validation
All environment variables are validated:
- Non-empty checks
- Pattern matching (e.g., eth*, wlan*)
- Numeric range validation
- Path traversal prevention
- Command injection prevention

### Privilege Management
```
Hotplug handlers:     root (required by procd)
LwM2M client:         nobody:nogroup
State files:          root:root (644)
State directory:      root:root (755)
```

## Integration Checklist

### For Application Developers

- [ ] Subscribe to ubus events in client code
- [ ] Implement event handlers for each event type
- [ ] Update corresponding LwM2M objects
- [ ] Trigger registration updates when needed
- [ ] Handle edge cases (rapid events, missing data)
- [ ] Test with actual hardware events
- [ ] Verify resource values are correct
- [ ] Check notification delivery to server

### For System Integrators

- [ ] Install hotplug scripts to /etc/hotplug.d/
- [ ] Set executable permissions (755)
- [ ] Configure UCI settings (/etc/config/lwm2m)
- [ ] Enable LwM2M client at boot
- [ ] Configure logging (logread/syslog)
- [ ] Test with manual hotplug triggers
- [ ] Verify ubus communication works
- [ ] Check system resource usage
- [ ] Configure firewall rules (if needed)
- [ ] Set up monitoring/alerting

### For Quality Assurance

- [ ] Test all event types (interface, USB, net, button)
- [ ] Verify event data is complete and accurate
- [ ] Test error handling (missing variables, client down)
- [ ] Check log messages are appropriate
- [ ] Verify ubus events are sent correctly
- [ ] Test rapid event sequences
- [ ] Check resource usage under load
- [ ] Verify state files are created correctly
- [ ] Test special button actions (factory reset, etc.)
- [ ] Validate LwM2M object updates

## Troubleshooting Workflow

```
┌─────────────────────────┐
│  Event not working?     │
└───────────┬─────────────┘
            │
    ┌───────▼────────┐
    │ Check if script│
    │  is executable │
    └───────┬────────┘
            │ Yes
    ┌───────▼────────┐
    │  Check logs    │
    │ (logread -f)   │
    └───────┬────────┘
            │
    ┌───────▼────────┐
    │ Check if LwM2M │
    │ client running │
    └───────┬────────┘
            │ Yes
    ┌───────▼────────┐
    │ Check if ubus  │
    │ object exists  │
    └───────┬────────┘
            │ Yes
    ┌───────▼────────┐
    │ Test manually  │
    │  with hotplug- │
    │    call        │
    └───────┬────────┘
            │
    ┌───────▼────────┐
    │ Check ubus     │
    │ events arrive  │
    └───────┬────────┘
            │ Yes
    ┌───────▼────────┐
    │ Verify object  │
    │ resources are  │
    │    updated     │
    └────────────────┘
```

## References

- [OpenWRT Hotplug](https://openwrt.org/docs/guide-user/base-system/hotplug)
- [OpenWRT ubus](https://openwrt.org/docs/techref/ubus)
- [procd Init System](https://openwrt.org/docs/guide-developer/procd-init-scripts)
- [LwM2M Specification](https://www.openmobilealliance.org/release/LightweightM2M/)
