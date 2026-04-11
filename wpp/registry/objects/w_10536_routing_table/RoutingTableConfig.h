/*
 * RoutingTable Configuration (Object 10536)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_ROUTING_TABLE_CONFIG_H
#define WPP_ROUTING_TABLE_CONFIG_H

#ifdef OBJ_W_10536_ROUTING_TABLE

// Resource enable/disable flags
// All resources are enabled by default

// Routing Table Resources (0-13)
#define RES_10536_0  1  // Destination Network (RW, String)
#define RES_10536_1  1  // Netmask/Prefix (RW, String)
#define RES_10536_2  1  // Gateway (RW, String)
#define RES_10536_3  1  // Interface (RW, String)
#define RES_10536_4  1  // Metric (RW, Integer)
#define RES_10536_5  1  // Protocol (R, Integer) - 0=unspec, 1=redirect, 2=kernel, 3=boot, 4=static, 5=dhcp
#define RES_10536_6  1  // State (R, Integer) - 0=down, 1=up, 2=unreachable
#define RES_10536_7  1  // Table (RW, Integer) - routing table ID (254=main, 255=local, custom)
#define RES_10536_8  1  // Scope (RW, Integer) - 0=global, 253=link, 254=host
#define RES_10536_9  1  // MTU (RW, Integer) - maximum transmission unit
#define RES_10536_10 1  // Add Route (E, Execute)
#define RES_10536_11 1  // Delete Route (E, Execute)
#define RES_10536_12 1  // Flush Routes (E, Execute)
#define RES_10536_13 1  // Reload Routes (E, Execute)

#endif // OBJ_W_10536_ROUTING_TABLE
#endif // WPP_ROUTING_TABLE_CONFIG_H
