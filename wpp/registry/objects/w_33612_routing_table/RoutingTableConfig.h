/*
 * RoutingTable Configuration (Object 33612)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_ROUTING_TABLE_CONFIG_H
#define WPP_ROUTING_TABLE_CONFIG_H

#ifdef OBJ_W_33612_ROUTING_TABLE

// Resource enable/disable flags
// All resources are enabled by default

// Routing Table Resources (0-13)
#define RES_33612_0  1  // Destination Network (RW, String)
#define RES_33612_1  1  // Netmask/Prefix (RW, String)
#define RES_33612_2  1  // Gateway (RW, String)
#define RES_33612_3  1  // Interface (RW, String)
#define RES_33612_4  1  // Metric (RW, Integer)
#define RES_33612_5  1  // Protocol (R, Integer) - 0=unspec, 1=redirect, 2=kernel, 3=boot, 4=static, 5=dhcp
#define RES_33612_6  1  // State (R, Integer) - 0=down, 1=up, 2=unreachable
#define RES_33612_7  1  // Table (RW, Integer) - routing table ID (254=main, 255=local, custom)
#define RES_33612_8  1  // Scope (RW, Integer) - 0=global, 253=link, 254=host
#define RES_33612_9  1  // MTU (RW, Integer) - maximum transmission unit
#define RES_33612_10 1  // Add Route (E, Execute)
#define RES_33612_11 1  // Delete Route (E, Execute)
#define RES_33612_12 1  // Flush Routes (E, Execute)
#define RES_33612_13 1  // Reload Routes (E, Execute)

#endif // OBJ_W_33612_ROUTING_TABLE
#endif // WPP_ROUTING_TABLE_CONFIG_H
