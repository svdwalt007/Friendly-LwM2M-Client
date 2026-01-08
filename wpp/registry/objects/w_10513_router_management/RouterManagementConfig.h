/*
 * RouterManagement Config (Object 10513)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_ROUTER_MANAGEMENT_CONFIG_H
#define WPP_ROUTER_MANAGEMENT_CONFIG_H

#ifdef OBJ_W_10513_ROUTER_MANAGEMENT

/* --------------- Config block 0 start --------------- */
// All resources enabled for comprehensive router management
#define RES_10513_0  1   // Router Name
#define RES_10513_1  1   // LAN IP Address
#define RES_10513_2  1   // LAN Subnet Mask
#define RES_10513_3  1   // DHCP Enabled
#define RES_10513_4  1   // DHCP Start IP
#define RES_10513_5  1   // DHCP End IP
#define RES_10513_6  1   // DHCP Lease Time
#define RES_10513_7  1   // DNS Server 1
#define RES_10513_8  1   // DNS Server 2
#define RES_10513_9  1   // WAN Connection Type
#define RES_10513_10 1   // WAN IP Address
#define RES_10513_11 1   // WAN Gateway
#define RES_10513_12 1   // Firewall Enabled
#define RES_10513_13 1   // NAT Enabled
#define RES_10513_14 1   // UPnP Enabled
#define RES_10513_15 1   // Apply Configuration
#define RES_10513_16 1   // Reset to Defaults
/* --------------- Config block 0 end --------------- */

#endif // OBJ_W_10513_ROUTER_MANAGEMENT
#endif // WPP_ROUTER_MANAGEMENT_CONFIG_H
