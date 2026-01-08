/*
 * WanFailoverPolicy Configuration (Object 34611)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WAN_FAILOVER_POLICY_CONFIG_H
#define WPP_WAN_FAILOVER_POLICY_CONFIG_H

#ifdef OBJ_W_34611_WAN_FAILOVER_POLICY

// Resource enable/disable flags
// All resources are enabled by default

// WAN Failover Policy Resources (0-20)
#define RES_34611_0  1  // Policy Name (RW, String)
#define RES_34611_1  1  // Primary WAN (RW, String)
#define RES_34611_2  1  // Secondary WAN (RW, String)
#define RES_34611_3  1  // Tertiary WAN (RW, String)
#define RES_34611_4  1  // Mode (RW, Integer) - 0=failover, 1=loadbalance, 2=roundrobin
#define RES_34611_5  1  // Primary Weight (RW, Integer) - 1-100 for load balancing
#define RES_34611_6  1  // Secondary Weight (RW, Integer) - 1-100 for load balancing
#define RES_34611_7  1  // Tertiary Weight (RW, Integer) - 1-100 for load balancing
#define RES_34611_8  1  // Failover Threshold (RW, Integer) - missed pings before failover
#define RES_34611_9  1  // Failback Mode (RW, Integer) - 0=immediate, 1=delayed, 2=manual
#define RES_34611_10 1  // Failback Delay (RW, Integer) - seconds
#define RES_34611_11 1  // Current Active WAN (R, String)
#define RES_34611_12 1  // Policy State (R, Integer) - 0=inactive, 1=active, 2=error
#define RES_34611_13 1  // Last Failover Time (R, Time)
#define RES_34611_14 1  // Failover Count (R, Integer)
#define RES_34611_15 1  // Sticky Connections (RW, Boolean)
#define RES_34611_16 1  // Interface Rules (RW, Multiple, String) - interface:protocol:port
#define RES_34611_17 1  // Apply Policy (E, Execute)
#define RES_34611_18 1  // Force Failover (E, Execute)
#define RES_34611_19 1  // Force Failback (E, Execute)
#define RES_34611_20 1  // Reset Counters (E, Execute)

#endif // OBJ_W_34611_WAN_FAILOVER_POLICY
#endif // WPP_WAN_FAILOVER_POLICY_CONFIG_H
