/*
 * WifiClientManagement Configuration (Object 33422)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WIFI_CLIENT_MANAGEMENT_CONFIG_H
#define WPP_WIFI_CLIENT_MANAGEMENT_CONFIG_H

#ifdef OBJ_W_33422_WIFI_CLIENT_MANAGEMENT

// Resource enable/disable flags
// All resources are enabled by default

// WiFi Client Management Resources (0-22)
#define RES_33422_0  1  // MAC Address (R, String)
#define RES_33422_1  1  // IP Address (R, String)
#define RES_33422_2  1  // Hostname (R, String)
#define RES_33422_3  1  // Interface (R, String)
#define RES_33422_4  1  // Signal Strength/RSSI (R, Integer) - dBm
#define RES_33422_5  1  // Noise Level (R, Integer) - dBm
#define RES_33422_6  1  // SNR (R, Integer) - dB
#define RES_33422_7  1  // TX Rate (R, Integer) - Mbps
#define RES_33422_8  1  // RX Rate (R, Integer) - Mbps
#define RES_33422_9  1  // TX Bytes (R, Integer)
#define RES_33422_10 1  // RX Bytes (R, Integer)
#define RES_33422_11 1  // Connected Time (R, Integer) - seconds
#define RES_33422_12 1  // Capability (R, String) - 802.11 standard
#define RES_33422_13 1  // Band (R, String) - 2.4GHz/5GHz/6GHz
#define RES_33422_14 1  // Bandwidth (R, Integer) - MHz
#define RES_33422_15 1  // ACL State (RW, String) - allowed/denied/none
#define RES_33422_16 1  // QoS Priority (RW, Integer) - 0-7
#define RES_33422_17 1  // Isolated (RW, Boolean)
#define RES_33422_18 1  // Last Activity (R, Integer) - seconds since last activity
#define RES_33422_19 1  // Disconnect Client (E, Execute)
#define RES_33422_20 1  // Add to Blacklist (E, Execute)
#define RES_33422_21 1  // Add to Whitelist (E, Execute)
#define RES_33422_22 1  // Steer to Band (E, Execute) - parameter: "2.4" or "5" or "6"

#endif // OBJ_W_33422_WIFI_CLIENT_MANAGEMENT
#endif // WPP_WIFI_CLIENT_MANAGEMENT_CONFIG_H
