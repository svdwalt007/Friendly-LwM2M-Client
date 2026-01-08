/*
 * MultiWanHealthCheck Configuration (Object 34612)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_MULTIWAN_HEALTH_CHECK_CONFIG_H
#define WPP_MULTIWAN_HEALTH_CHECK_CONFIG_H

#ifdef OBJ_W_34612_MULTIWAN_HEALTH_CHECK

// Resource enable/disable flags
// All resources are enabled by default

// Multi-WAN Health Check Resources (0-23)
#define RES_34612_0  1  // Interface Name (RW, String)
#define RES_34612_1  1  // Probe Type (RW, Integer) - icmp=0, http=1, dns=2, tcp=3
#define RES_34612_2  1  // Target Host (RW, String) - IP or hostname
#define RES_34612_3  1  // Target Port (RW, Integer) - for HTTP/TCP probes
#define RES_34612_4  1  // Probe Interval (RW, Integer) - seconds
#define RES_34612_5  1  // Probe Timeout (RW, Integer) - milliseconds
#define RES_34612_6  1  // Failure Threshold (RW, Integer) - consecutive failures before marking unhealthy
#define RES_34612_7  1  // Recovery Threshold (RW, Integer) - consecutive successes before marking healthy
#define RES_34612_8  1  // Current State (R, Integer) - healthy=0, degraded=1, unhealthy=2
#define RES_34612_9  1  // Last Probe Time (R, Time) - timestamp of last probe
#define RES_34612_10 1  // Last Probe Result (R, Boolean) - true=success, false=failure
#define RES_34612_11 1  // Average Latency (R, Float) - milliseconds
#define RES_34612_12 1  // Min Latency (R, Float) - milliseconds
#define RES_34612_13 1  // Max Latency (R, Float) - milliseconds
#define RES_34612_14 1  // Jitter (R, Float) - milliseconds
#define RES_34612_15 1  // Packet Loss (R, Float) - percentage 0.0-100.0
#define RES_34612_16 1  // Consecutive Failures (R, Integer)
#define RES_34612_17 1  // Consecutive Successes (R, Integer)
#define RES_34612_18 1  // Total Probes (R, Integer)
#define RES_34612_19 1  // Failed Probes (R, Integer)
#define RES_34612_20 1  // Start Probing (E, Execute)
#define RES_34612_21 1  // Stop Probing (E, Execute)
#define RES_34612_22 1  // Force Check (E, Execute)
#define RES_34612_23 1  // Reset Stats (E, Execute)

#endif // OBJ_W_34612_MULTIWAN_HEALTH_CHECK
#endif // WPP_MULTIWAN_HEALTH_CHECK_CONFIG_H
