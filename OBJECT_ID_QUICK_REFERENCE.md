# LwM2M Object ID Quick Reference

**Walt Technologies Object ID Ranges: 10512-10537**

## Quick Lookup Table

| New ID | Object Name | Old ID | CMake Define |
|--------|-------------|--------|--------------|
| **10512** | Starlink Terminal | 34600 | `OBJ_O_10512_STARLINK_TERMINAL` |
| **10513** | Router Management | 34601 | `OBJ_W_10513_ROUTER_MANAGEMENT` |
| **10514** | Ethernet Interface | 34602 | `OBJ_W_10514_ETHERNET_INTERFACE` |
| **10515** | GPIO Control | 34603 | `OBJ_W_10515_GPIO_CONTROL` |
| **10516** | USB Management | 34604 | `OBJ_W_10516_USB_MANAGEMENT` |
| **10517** | Storage Management | 34605 | `OBJ_W_10517_STORAGE_MANAGEMENT` |
| **10518** | System Monitor | 34606 | `OBJ_W_10518_SYSTEM_MONITOR` |
| **10519** | Hardware Watchdog | 34607 | `OBJ_O_10519_HARDWARE_WATCHDOG` |
| **10520** | MIKROBUS | 34608 | `OBJ_O_10520_MIKROBUS` |
| **10521** | Firewall Config | 34609 | `OBJ_W_10521_FIREWALL_CONFIG` |
| **10522** | PoE Management | 34610 | `OBJ_W_10522_POE_MANAGEMENT` |
| **10525** | WAN Failover Policy | 34611 | `OBJ_W_10525_WAN_FAILOVER_POLICY` |
| **10526** | Multi-WAN Health Check | 34612 | `OBJ_W_10526_MULTIWAN_HEALTH_CHECK` |
| **10527** | WiFi Client Management | 34613 | `OBJ_W_10527_WIFI_CLIENT_MANAGEMENT` |
| **10528** | WiFi Channel Optimization | 34614 | `OBJ_W_10528_WIFI_CHANNEL_OPTIMIZATION` |
| **10529** | Matter Bridge | 34615 | `OBJ_W_10529_MATTER_BRIDGE` |
| **10530** | Matter Device | 34616 | `OBJ_W_10530_MATTER_DEVICE` |
| **10531** | Thread Network | 34617 | `OBJ_W_10531_THREAD_NETWORK` |
| **10532** | Zigbee Coordinator | 34618 | `OBJ_W_10532_ZIGBEE_COORDINATOR` |
| **10533** | Zigbee Device | 34619 | `OBJ_W_10533_ZIGBEE_DEVICE` |
| **10534** | Zigbee Group | 34620 | `OBJ_W_10534_ZIGBEE_GROUP` |
| **10535** | LAN Configuration | 34621 | `OBJ_W_10535_LAN_CONFIGURATION` |
| **10536** | Routing Table | 34622 | `OBJ_W_10536_ROUTING_TABLE` |
| **10537** | VPN Configuration | 34623 | `OBJ_W_10537_VPN_CONFIGURATION` |

## Reserved IDs

- **10523-10524**: Reserved for future expansion

## Object URN Format

- **New Format**: `urn:oma:lwm2m:x:10512` (for Starlink Terminal)
- **Old Format**: `urn:oma:lwm2m:x:34600` (deprecated)

## Build System Usage

To enable an object in your build, use:

```cmake
add_definitions(-DOBJ_W_10513_ROUTER_MANAGEMENT)
add_definitions(-DOBJ_O_10512_STARLINK_TERMINAL)
```

## Code References

```cpp
#ifdef OBJ_W_10513_ROUTER_MANAGEMENT
#include "w_10513_router_management/RouterManagement.h"
#endif
```

## Directory Structure

```
wpp/registry/objects/
├── o_10512_starlink_terminal/
├── o_10519_hardware_watchdog/
├── o_10520_mikrobus/
├── w_10513_router_management/
├── w_10514_ethernet_interface/
├── w_10515_gpio_control/
├── w_10516_usb_management/
├── w_10517_storage_management/
├── w_10518_system_monitor/
├── w_10521_firewall_config/
├── w_10522_poe_management/
├── w_10525_wan_failover_policy/
├── w_10526_multiwan_health_check/
├── w_10527_wifi_client_management/
├── w_10528_wifi_channel_optimization/
├── w_10529_matter_bridge/
├── w_10530_matter_device/
├── w_10531_thread_network/
├── w_10532_zigbee_coordinator/
├── w_10533_zigbee_device/
├── w_10534_zigbee_group/
├── w_10535_lan_configuration/
├── w_10536_routing_table/
└── w_10537_vpn_configuration/
```

## Grep Commands for Verification

```bash
# Find any remaining old IDs
grep -r "346[0-2][0-9]" wpp/registry/objects/ --include="*.cpp" --include="*.h"

# List all 105xx objects
ls -1 wpp/registry/objects/ | grep "^[ow]_105"

# Count objects
ls -1d wpp/registry/objects/{o,w}_105* | wc -l
# Should return: 24
```

---

**Last Updated:** 2026-04-11
**Migration Status:** ✅ Complete
