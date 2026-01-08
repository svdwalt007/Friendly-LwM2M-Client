/*
 * PoeManagement Configuration (Object 10522)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_POE_MANAGEMENT_CONFIG_H
#define WPP_POE_MANAGEMENT_CONFIG_H

#ifdef OBJ_W_10522_POE_MANAGEMENT

// Resource enable/disable flags
// All resources are enabled by default

// PoE Management Resources (0-11)
#define RES_10522_0  1  // PoE Supported (R, Boolean)
#define RES_10522_1  1  // PoE Enabled (RW, Boolean)
#define RES_10522_2  1  // Input Voltage (R, Float) - volts
#define RES_10522_3  1  // Input Current (R, Float) - amperes
#define RES_10522_4  1  // Input Power (R, Float) - watts
#define RES_10522_5  1  // PoE Standard (R, String) - e.g., "802.3af", "802.3at"
#define RES_10522_6  1  // PoE Class (R, Integer) - 0-8 (802.3bt Class 1-8)
#define RES_10522_7  1  // Max Power Available (R, Float) - watts
#define RES_10522_8  1  // Power Allocated (R, Float) - watts
#define RES_10522_9  1  // Temperature (R, Float) - Celsius
#define RES_10522_10 1  // Status (R, String) - "Enabled", "Disabled", "Error"
#define RES_10522_11 1  // Reset PoE (E, Execute) - Power cycle the PoE

#endif // OBJ_W_10522_POE_MANAGEMENT
#endif // WPP_POE_MANAGEMENT_CONFIG_H
