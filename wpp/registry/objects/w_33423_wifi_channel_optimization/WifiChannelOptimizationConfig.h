/*
 * WifiChannelOptimization Configuration (Object 33423)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WIFI_CHANNEL_OPTIMIZATION_CONFIG_H
#define WPP_WIFI_CHANNEL_OPTIMIZATION_CONFIG_H

#ifdef OBJ_W_33423_WIFI_CHANNEL_OPTIMIZATION

// Resource enable/disable flags
// All resources are enabled by default

// WiFi Channel Optimization Resources (0-21)
#define RES_33423_0  1  // Interface (RW, String) - e.g., "radio0", "radio1"
#define RES_33423_1  1  // Current Channel (R, Integer)
#define RES_33423_2  1  // Current Width (R, Integer) - 20/40/80/160 MHz
#define RES_33423_3  1  // Auto Channel (RW, Boolean)
#define RES_33423_4  1  // Scan Interval (RW, Integer) - seconds
#define RES_33423_5  1  // Neighboring APs (R, Integer) - count
#define RES_33423_6  1  // Channel Utilization (R, Integer) - percentage
#define RES_33423_7  1  // Noise Floor (R, Integer) - dBm
#define RES_33423_8  1  // Interference Level (R, Integer) - 0-100
#define RES_33423_9  1  // DFS Enabled (RW, Boolean)
#define RES_33423_10 1  // DFS State (R, String) - "CAC", "Available", "NOP"
#define RES_33423_11 1  // Radar Detected (R, Boolean)
#define RES_33423_12 1  // Best 2.4G Channel (R, Integer)
#define RES_33423_13 1  // Best 5G Channel (R, Integer)
#define RES_33423_14 1  // Best 6G Channel (R, Integer)
#define RES_33423_15 1  // Channel Survey (R, Multiple, String) - channel:busy:noise
#define RES_33423_16 1  // Last Scan Time (R, Time) - Unix timestamp
#define RES_33423_17 1  // Last Optimization (R, Time) - Unix timestamp
#define RES_33423_18 1  // Scan Now (E, Execute)
#define RES_33423_19 1  // Optimize Channel (E, Execute)
#define RES_33423_20 1  // Set Channel (E, Execute) - with argument
#define RES_33423_21 1  // Enable DFS (E, Execute)

#endif // OBJ_W_33423_WIFI_CHANNEL_OPTIMIZATION
#endif // WPP_WIFI_CHANNEL_OPTIMIZATION_CONFIG_H
