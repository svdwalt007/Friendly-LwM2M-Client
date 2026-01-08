/*
 * FirewallConfig Configuration (Object 10521)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_FIREWALL_CONFIG_CONFIG_H
#define WPP_FIREWALL_CONFIG_CONFIG_H

#ifdef OBJ_W_10521_FIREWALL_CONFIG

// Resource enable/disable flags
// All resources are enabled by default

// Firewall Configuration Resources (0-15)
#define RES_10521_0  1  // Rule Name (RW, String)
#define RES_10521_1  1  // Rule Index (R, Integer)
#define RES_10521_2  1  // Enabled (RW, Boolean)
#define RES_10521_3  1  // Action (RW, Integer) - 0=Accept, 1=Reject, 2=Drop
#define RES_10521_4  1  // Protocol (RW, Integer) - 0=All, 1=TCP, 2=UDP, 3=ICMP
#define RES_10521_5  1  // Source IP (RW, String)
#define RES_10521_6  1  // Source Port (RW, Integer)
#define RES_10521_7  1  // Destination IP (RW, String)
#define RES_10521_8  1  // Destination Port (RW, Integer)
#define RES_10521_9  1  // Interface (RW, String) - e.g., "wan", "lan"
#define RES_10521_10 1  // Direction (RW, Integer) - 0=Input, 1=Output, 2=Forward
#define RES_10521_11 1  // Zone From (RW, String) - e.g., "wan", "lan"
#define RES_10521_12 1  // Zone To (RW, String) - e.g., "lan", "wan"
#define RES_10521_13 1  // Comment (RW, String)
#define RES_10521_14 1  // Apply Rule (E, Execute)
#define RES_10521_15 1  // Delete Rule (E, Execute)

#endif // OBJ_W_10521_FIREWALL_CONFIG
#endif // WPP_FIREWALL_CONFIG_CONFIG_H
