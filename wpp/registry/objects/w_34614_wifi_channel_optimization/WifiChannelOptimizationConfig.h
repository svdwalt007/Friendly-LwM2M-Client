/*
 * WifiChannelOptimization Configuration (Object 34614)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WIFI_CHANNEL_OPTIMIZATION_CONFIG_H
#define WPP_WIFI_CHANNEL_OPTIMIZATION_CONFIG_H

#ifdef OBJ_W_34614_WIFI_CHANNEL_OPTIMIZATION

// Resource enable/disable flags
// All resources are enabled by default

// WiFi Channel Optimization Resources (0-21)
#define RES_34614_0  1  // Interface (RW, String) - e.g., "radio0", "radio1"
#define RES_34614_1  1  // Current Channel (R, Integer)
#define RES_34614_2  1  // Current Width (R, Integer) - 20/40/80/160 MHz
#define RES_34614_3  1  // Auto Channel (RW, Boolean)
#define RES_34614_4  1  // Scan Interval (RW, Integer) - seconds
#define RES_34614_5  1  // Neighboring APs (R, Integer) - count
#define RES_34614_6  1  // Channel Utilization (R, Integer) - percentage
#define RES_34614_7  1  // Noise Floor (R, Integer) - dBm
#define RES_34614_8  1  // Interference Level (R, Integer) - 0-100
#define RES_34614_9  1  // DFS Enabled (RW, Boolean)
#define RES_34614_10 1  // DFS State (R, String) - "CAC", "Available", "NOP"
#define RES_34614_11 1  // Radar Detected (R, Boolean)
#define RES_34614_12 1  // Best 2.4G Channel (R, Integer)
#define RES_34614_13 1  // Best 5G Channel (R, Integer)
#define RES_34614_14 1  // Best 6G Channel (R, Integer)
#define RES_34614_15 1  // Channel Survey (R, Multiple, String) - channel:busy:noise
#define RES_34614_16 1  // Last Scan Time (R, Time) - Unix timestamp
#define RES_34614_17 1  // Last Optimization (R, Time) - Unix timestamp
#define RES_34614_18 1  // Scan Now (E, Execute)
#define RES_34614_19 1  // Optimize Channel (E, Execute)
#define RES_34614_20 1  // Set Channel (E, Execute) - with argument
#define RES_34614_21 1  // Enable DFS (E, Execute)

#endif // OBJ_W_34614_WIFI_CHANNEL_OPTIMIZATION
#endif // WPP_WIFI_CHANNEL_OPTIMIZATION_CONFIG_H
