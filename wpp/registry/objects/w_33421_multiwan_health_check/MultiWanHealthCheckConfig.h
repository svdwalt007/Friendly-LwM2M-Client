/*
 * MultiWanHealthCheck Configuration (Object 33421)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_MULTIWAN_HEALTH_CHECK_CONFIG_H
#define WPP_MULTIWAN_HEALTH_CHECK_CONFIG_H

#ifdef OBJ_W_33421_MULTIWAN_HEALTH_CHECK

// Resource enable/disable flags
// All resources are enabled by default

// Multi-WAN Health Check Resources (0-23)
#define RES_33421_0  1  // Interface Name (RW, String)
#define RES_33421_1  1  // Probe Type (RW, Integer) - icmp=0, http=1, dns=2, tcp=3
#define RES_33421_2  1  // Target Host (RW, String) - IP or hostname
#define RES_33421_3  1  // Target Port (RW, Integer) - for HTTP/TCP probes
#define RES_33421_4  1  // Probe Interval (RW, Integer) - seconds
#define RES_33421_5  1  // Probe Timeout (RW, Integer) - milliseconds
#define RES_33421_6  1  // Failure Threshold (RW, Integer) - consecutive failures before marking unhealthy
#define RES_33421_7  1  // Recovery Threshold (RW, Integer) - consecutive successes before marking healthy
#define RES_33421_8  1  // Current State (R, Integer) - healthy=0, degraded=1, unhealthy=2
#define RES_33421_9  1  // Last Probe Time (R, Time) - timestamp of last probe
#define RES_33421_10 1  // Last Probe Result (R, Boolean) - true=success, false=failure
#define RES_33421_11 1  // Average Latency (R, Float) - milliseconds
#define RES_33421_12 1  // Min Latency (R, Float) - milliseconds
#define RES_33421_13 1  // Max Latency (R, Float) - milliseconds
#define RES_33421_14 1  // Jitter (R, Float) - milliseconds
#define RES_33421_15 1  // Packet Loss (R, Float) - percentage 0.0-100.0
#define RES_33421_16 1  // Consecutive Failures (R, Integer)
#define RES_33421_17 1  // Consecutive Successes (R, Integer)
#define RES_33421_18 1  // Total Probes (R, Integer)
#define RES_33421_19 1  // Failed Probes (R, Integer)
#define RES_33421_20 1  // Start Probing (E, Execute)
#define RES_33421_21 1  // Stop Probing (E, Execute)
#define RES_33421_22 1  // Force Check (E, Execute)
#define RES_33421_23 1  // Reset Stats (E, Execute)

#endif // OBJ_W_33421_MULTIWAN_HEALTH_CHECK
#endif // WPP_MULTIWAN_HEALTH_CHECK_CONFIG_H
