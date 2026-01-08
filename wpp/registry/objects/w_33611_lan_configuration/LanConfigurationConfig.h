/*
 * LanConfiguration Configuration (Object 33611)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_LAN_CONFIGURATION_CONFIG_H
#define WPP_LAN_CONFIGURATION_CONFIG_H

#ifdef OBJ_W_33611_LAN_CONFIGURATION

// Resource enable/disable flags
// All resources are enabled by default

// LAN Configuration Resources (0-12)
#define RES_33611_0  1  // LAN IP Address (RW, String)
#define RES_33611_1  1  // Subnet Mask (RW, String)
#define RES_33611_2  1  // DHCP Enabled (RW, Boolean)
#define RES_33611_3  1  // DHCP Pool Start (RW, String)
#define RES_33611_4  1  // DHCP Pool End (RW, String)
#define RES_33611_5  1  // DHCP Lease Time (RW, Integer) - seconds
#define RES_33611_6  1  // DNS Server Primary (RW, String)
#define RES_33611_7  1  // DNS Server Secondary (RW, String)
#define RES_33611_8  1  // Domain Name (RW, String)
#define RES_33611_9  1  // Static Leases (RW, Multiple, String) - MAC:IP format
#define RES_33611_10 1  // Active Leases (R, Multiple, String) - MAC:IP:hostname:expiry
#define RES_33611_11 1  // Apply Configuration (E, Execute)
#define RES_33611_12 1  // Configuration Status (R, Integer) - 0=pending, 1=applied, 2=error

#endif // OBJ_W_33611_LAN_CONFIGURATION
#endif // WPP_LAN_CONFIGURATION_CONFIG_H
