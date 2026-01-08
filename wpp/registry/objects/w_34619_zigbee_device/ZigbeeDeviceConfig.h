/*
 * ZigbeeDevice Configuration (Object 34619)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_DEVICE_CONFIG_H
#define WPP_ZIGBEE_DEVICE_CONFIG_H

#ifdef OBJ_W_34619_ZIGBEE_DEVICE

// Resource enable/disable flags
// All resources are enabled by default

// Zigbee Device Resources
#define RES_34619_0  1  // IEEE Address (R, String) - 64-bit IEEE address (hex)
#define RES_34619_1  1  // Network Address (R, Integer) - 16-bit network address
#define RES_34619_2  1  // Device Type (R, Integer) - 0=unknown, 1=coordinator, 2=router, 3=end_device
#define RES_34619_3  1  // Manufacturer (R, String) - Manufacturer name
#define RES_34619_4  1  // Model (R, String) - Model identifier
#define RES_34619_5  1  // Firmware Version (R, String) - Device firmware version
#define RES_34619_6  1  // Power Source (R, Integer) - 0=unknown, 1=mains, 2=battery, 3=dc
#define RES_34619_7  1  // LQI (R, Integer) - Link Quality Indicator (0-255)
#define RES_34619_8  1  // RSSI (R, Integer) - Received Signal Strength (dBm)
#define RES_34619_9  1  // Last Seen (R, Time) - Last communication timestamp
#define RES_34619_10 1  // Endpoints (R, Multiple, Integer) - Active endpoint list
#define RES_34619_11 1  // Clusters (R, String) - JSON array of supported clusters
#define RES_34619_12 1  // Interviewed (R, Boolean) - Interview complete flag
#define RES_34619_13 1  // Online (R, Boolean) - Device online status
#define RES_34619_14 1  // Profile ID (R, Integer) - Zigbee profile ID
#define RES_34619_15 1  // Device ID (R, Integer) - Zigbee device ID
#define RES_34619_16 1  // Remove (E, Execute) - Remove device from network
#define RES_34619_17 1  // Interview (E, Execute) - Start device interview
#define RES_34619_18 1  // Ping (E, Execute) - Ping device
#define RES_34619_19 1  // Read Attribute (E, Execute) - Read ZCL attribute
#define RES_34619_20 1  // Write Attribute (E, Execute) - Write ZCL attribute

#endif // OBJ_W_34619_ZIGBEE_DEVICE
#endif // WPP_ZIGBEE_DEVICE_CONFIG_H
