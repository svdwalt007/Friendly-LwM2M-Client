/*
 * ZigbeeGroup Configuration (Object 34620)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_GROUP_CONFIG_H
#define WPP_ZIGBEE_GROUP_CONFIG_H

#ifdef OBJ_W_34620_ZIGBEE_GROUP

// Resource enable/disable flags
// All resources are enabled by default

// Zigbee Group Resources
#define RES_34620_0  1  // Group ID (RW, Integer) - 16-bit group identifier (0x0001-0xFFF7)
#define RES_34620_1  1  // Group Name (RW, String) - Human-readable group name
#define RES_34620_2  1  // Member Devices (R, Multiple, String) - IEEE addresses of member devices
#define RES_34620_3  1  // Member Count (R, Integer) - Number of devices in group
#define RES_34620_4  1  // Scene Count (R, Integer) - Number of scenes defined for group
#define RES_34620_5  1  // Current Scene (RW, Integer) - Currently active scene ID
#define RES_34620_6  1  // Add Member (E, Execute) - Add device to group (arg: IEEE address)
#define RES_34620_7  1  // Remove Member (E, Execute) - Remove device from group (arg: IEEE address)
#define RES_34620_8  1  // Scene Recall (E, Execute) - Recall scene (arg: scene ID)
#define RES_34620_9  1  // Scene Store (E, Execute) - Store current state as scene (arg: scene ID)
#define RES_34620_10 1  // On (E, Execute) - Turn on all devices in group
#define RES_34620_11 1  // Off (E, Execute) - Turn off all devices in group
#define RES_34620_12 1  // Toggle (E, Execute) - Toggle all devices in group
#define RES_34620_13 1  // Set Level (E, Execute) - Set brightness level (arg: level 0-254)
#define RES_34620_14 1  // Set Color (E, Execute) - Set color (arg: hue, saturation)

#endif // OBJ_W_34620_ZIGBEE_GROUP
#endif // WPP_ZIGBEE_GROUP_CONFIG_H
