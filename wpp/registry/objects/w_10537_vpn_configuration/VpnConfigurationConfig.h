/*
 * VpnConfiguration Configuration (Object 10537)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_VPN_CONFIGURATION_CONFIG_H
#define WPP_VPN_CONFIGURATION_CONFIG_H

#ifdef OBJ_W_10537_VPN_CONFIGURATION

#include "WppTypes.h"

#define VPN_CONFIG_OBJECT_ID (wpp::OBJ_ID)10537

// Resource enable/disable flags
// All resources are enabled by default

// VPN Configuration Resources (0-25)
#define RES_10537_0  1  // VPN Name (RW, String)
#define RES_10537_1  1  // Protocol Type (RW, Integer) - 0=OpenVPN, 1=WireGuard, 2=IPsec
#define RES_10537_2  1  // Server Address (RW, String)
#define RES_10537_3  1  // Server Port (RW, Integer)
#define RES_10537_4  1  // Local Address (R, String)
#define RES_10537_5  1  // Remote Network (RW, String)
#define RES_10537_6  1  // Authentication Type (RW, Integer)
#define RES_10537_7  1  // Username (RW, String)
#define RES_10537_8  1  // Password (RW, String)
#define RES_10537_9  1  // Certificate (RW, String)
#define RES_10537_10 1  // Private Key (RW, String)
#define RES_10537_11 1  // Public Key (R, String)
#define RES_10537_12 1  // Pre-Shared Key (RW, String)
#define RES_10537_13 1  // Connection State (R, Integer)
#define RES_10537_14 1  // Bytes Sent (R, Integer)
#define RES_10537_15 1  // Bytes Received (R, Integer)
#define RES_10537_16 1  // Uptime (R, Integer) - seconds
#define RES_10537_17 1  // Last Error (R, String)
#define RES_10537_18 1  // Keepalive Interval (RW, Integer) - seconds
#define RES_10537_19 1  // Reconnect Delay (RW, Integer) - seconds
#define RES_10537_20 1  // Split Tunnel (RW, Boolean)
#define RES_10537_21 1  // Kill Switch (RW, Boolean)
#define RES_10537_22 1  // Connect (E, Execute)
#define RES_10537_23 1  // Disconnect (E, Execute)
#define RES_10537_24 1  // Restart (E, Execute)
#define RES_10537_25 1  // Regenerate Keys (E, Execute)

#endif // OBJ_W_10537_VPN_CONFIGURATION
#endif // WPP_VPN_CONFIGURATION_CONFIG_H
