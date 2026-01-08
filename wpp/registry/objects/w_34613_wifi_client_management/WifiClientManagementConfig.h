/*
 * WifiClientManagement Configuration (Object 34613)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WIFI_CLIENT_MANAGEMENT_CONFIG_H
#define WPP_WIFI_CLIENT_MANAGEMENT_CONFIG_H

#ifdef OBJ_W_34613_WIFI_CLIENT_MANAGEMENT

// Resource enable/disable flags
// All resources are enabled by default

// WiFi Client Management Resources (0-22)
#define RES_34613_0  1  // MAC Address (R, String)
#define RES_34613_1  1  // IP Address (R, String)
#define RES_34613_2  1  // Hostname (R, String)
#define RES_34613_3  1  // Interface (R, String)
#define RES_34613_4  1  // Signal Strength/RSSI (R, Integer) - dBm
#define RES_34613_5  1  // Noise Level (R, Integer) - dBm
#define RES_34613_6  1  // SNR (R, Integer) - dB
#define RES_34613_7  1  // TX Rate (R, Integer) - Mbps
#define RES_34613_8  1  // RX Rate (R, Integer) - Mbps
#define RES_34613_9  1  // TX Bytes (R, Integer)
#define RES_34613_10 1  // RX Bytes (R, Integer)
#define RES_34613_11 1  // Connected Time (R, Integer) - seconds
#define RES_34613_12 1  // Capability (R, String) - 802.11 standard
#define RES_34613_13 1  // Band (R, String) - 2.4GHz/5GHz/6GHz
#define RES_34613_14 1  // Bandwidth (R, Integer) - MHz
#define RES_34613_15 1  // ACL State (RW, String) - allowed/denied/none
#define RES_34613_16 1  // QoS Priority (RW, Integer) - 0-7
#define RES_34613_17 1  // Isolated (RW, Boolean)
#define RES_34613_18 1  // Last Activity (R, Integer) - seconds since last activity
#define RES_34613_19 1  // Disconnect Client (E, Execute)
#define RES_34613_20 1  // Add to Blacklist (E, Execute)
#define RES_34613_21 1  // Add to Whitelist (E, Execute)
#define RES_34613_22 1  // Steer to Band (E, Execute) - parameter: "2.4" or "5" or "6"

#endif // OBJ_W_34613_WIFI_CLIENT_MANAGEMENT
#endif // WPP_WIFI_CLIENT_MANAGEMENT_CONFIG_H
