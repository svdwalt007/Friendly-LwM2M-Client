/*
 * ZigbeeDevice Configuration (Object 10533)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_DEVICE_CONFIG_H
#define WPP_ZIGBEE_DEVICE_CONFIG_H

#ifdef OBJ_W_10533_ZIGBEE_DEVICE

// Object ID
#define ZIGBEE_DEVICE_OBJECT_ID (wpp::OBJ_ID)10533

// Resource enable/disable flags
// All resources are enabled by default

// Zigbee Device Resources
#define RES_10533_0  1  // IEEE Address (R, String) - 64-bit IEEE address (hex)
#define RES_10533_1  1  // Network Address (R, Integer) - 16-bit network address
#define RES_10533_2  1  // Device Type (R, Integer) - 0=unknown, 1=coordinator, 2=router, 3=end_device
#define RES_10533_3  1  // Manufacturer (R, String) - Manufacturer name
#define RES_10533_4  1  // Model (R, String) - Model identifier
#define RES_10533_5  1  // Firmware Version (R, String) - Device firmware version
#define RES_10533_6  1  // Power Source (R, Integer) - 0=unknown, 1=mains, 2=battery, 3=dc
#define RES_10533_7  1  // LQI (R, Integer) - Link Quality Indicator (0-255)
#define RES_10533_8  1  // RSSI (R, Integer) - Received Signal Strength (dBm)
#define RES_10533_9  1  // Last Seen (R, Time) - Last communication timestamp
#define RES_10533_10 1  // Endpoints (R, Multiple, Integer) - Active endpoint list
#define RES_10533_11 1  // Clusters (R, String) - JSON array of supported clusters
#define RES_10533_12 1  // Interviewed (R, Boolean) - Interview complete flag
#define RES_10533_13 1  // Online (R, Boolean) - Device online status
#define RES_10533_14 1  // Profile ID (R, Integer) - Zigbee profile ID
#define RES_10533_15 1  // Device ID (R, Integer) - Zigbee device ID
#define RES_10533_16 1  // Remove (E, Execute) - Remove device from network
#define RES_10533_17 1  // Interview (E, Execute) - Start device interview
#define RES_10533_18 1  // Ping (E, Execute) - Ping device
#define RES_10533_19 1  // Read Attribute (E, Execute) - Read ZCL attribute
#define RES_10533_20 1  // Write Attribute (E, Execute) - Write ZCL attribute

#endif // OBJ_W_10533_ZIGBEE_DEVICE
#endif // WPP_ZIGBEE_DEVICE_CONFIG_H
