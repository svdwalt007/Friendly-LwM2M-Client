/*
 * WanFailoverPolicy Configuration (Object 33420)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WAN_FAILOVER_POLICY_CONFIG_H
#define WPP_WAN_FAILOVER_POLICY_CONFIG_H

#ifdef OBJ_W_33420_WAN_FAILOVER_POLICY

// Resource enable/disable flags
// All resources are enabled by default

// WAN Failover Policy Resources (0-20)
#define RES_33420_0  1  // Policy Name (RW, String)
#define RES_33420_1  1  // Primary WAN (RW, String)
#define RES_33420_2  1  // Secondary WAN (RW, String)
#define RES_33420_3  1  // Tertiary WAN (RW, String)
#define RES_33420_4  1  // Mode (RW, Integer) - 0=failover, 1=loadbalance, 2=roundrobin
#define RES_33420_5  1  // Primary Weight (RW, Integer) - 1-100 for load balancing
#define RES_33420_6  1  // Secondary Weight (RW, Integer) - 1-100 for load balancing
#define RES_33420_7  1  // Tertiary Weight (RW, Integer) - 1-100 for load balancing
#define RES_33420_8  1  // Failover Threshold (RW, Integer) - missed pings before failover
#define RES_33420_9  1  // Failback Mode (RW, Integer) - 0=immediate, 1=delayed, 2=manual
#define RES_33420_10 1  // Failback Delay (RW, Integer) - seconds
#define RES_33420_11 1  // Current Active WAN (R, String)
#define RES_33420_12 1  // Policy State (R, Integer) - 0=inactive, 1=active, 2=error
#define RES_33420_13 1  // Last Failover Time (R, Time)
#define RES_33420_14 1  // Failover Count (R, Integer)
#define RES_33420_15 1  // Sticky Connections (RW, Boolean)
#define RES_33420_16 1  // Interface Rules (RW, Multiple, String) - interface:protocol:port
#define RES_33420_17 1  // Apply Policy (E, Execute)
#define RES_33420_18 1  // Force Failover (E, Execute)
#define RES_33420_19 1  // Force Failback (E, Execute)
#define RES_33420_20 1  // Reset Counters (E, Execute)

#endif // OBJ_W_33420_WAN_FAILOVER_POLICY
#endif // WPP_WAN_FAILOVER_POLICY_CONFIG_H
