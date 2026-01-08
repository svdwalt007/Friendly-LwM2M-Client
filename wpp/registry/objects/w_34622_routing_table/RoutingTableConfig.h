/*
 * RoutingTable Configuration (Object 34622)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_ROUTING_TABLE_CONFIG_H
#define WPP_ROUTING_TABLE_CONFIG_H

#ifdef OBJ_W_34622_ROUTING_TABLE

// Resource enable/disable flags
// All resources are enabled by default

// Routing Table Resources (0-13)
#define RES_34622_0  1  // Destination Network (RW, String)
#define RES_34622_1  1  // Netmask/Prefix (RW, String)
#define RES_34622_2  1  // Gateway (RW, String)
#define RES_34622_3  1  // Interface (RW, String)
#define RES_34622_4  1  // Metric (RW, Integer)
#define RES_34622_5  1  // Protocol (R, Integer) - 0=unspec, 1=redirect, 2=kernel, 3=boot, 4=static, 5=dhcp
#define RES_34622_6  1  // State (R, Integer) - 0=down, 1=up, 2=unreachable
#define RES_34622_7  1  // Table (RW, Integer) - routing table ID (254=main, 255=local, custom)
#define RES_34622_8  1  // Scope (RW, Integer) - 0=global, 253=link, 254=host
#define RES_34622_9  1  // MTU (RW, Integer) - maximum transmission unit
#define RES_34622_10 1  // Add Route (E, Execute)
#define RES_34622_11 1  // Delete Route (E, Execute)
#define RES_34622_12 1  // Flush Routes (E, Execute)
#define RES_34622_13 1  // Reload Routes (E, Execute)

#endif // OBJ_W_34622_ROUTING_TABLE
#endif // WPP_ROUTING_TABLE_CONFIG_H
