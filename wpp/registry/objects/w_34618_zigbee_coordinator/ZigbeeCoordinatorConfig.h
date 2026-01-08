/*
 * ZigbeeCoordinator Configuration (Object 34618)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_COORDINATOR_CONFIG_H
#define WPP_ZIGBEE_COORDINATOR_CONFIG_H

#ifdef OBJ_W_34618_ZIGBEE_COORDINATOR

// Resource enable/disable flags
// All resources are enabled by default

// Zigbee Coordinator Resources
#define RES_34618_0  1  // Network State (R, Integer) - 0=offline, 1=forming, 2=joined, 3=ready, 4=leaving, 5=error
#define RES_34618_1  1  // PAN ID (RW, Integer) - 16-bit PAN identifier
#define RES_34618_2  1  // Extended PAN ID (RW, String) - 64-bit extended PAN ID (hex)
#define RES_34618_3  1  // Channel (RW, Integer) - Radio channel (11-26)
#define RES_34618_4  1  // Network Key (W, Opaque) - 128-bit network encryption key
#define RES_34618_5  1  // Permit Join (R, Boolean) - Permit join status
#define RES_34618_6  1  // Device Count (R, Integer) - Number of joined devices
#define RES_34618_7  1  // Coordinator IEEE Address (R, String) - 64-bit IEEE address (hex)
#define RES_34618_8  1  // Coordinator Type (R, Integer) - 0=unknown, 1=EZSP, 2=Z-Stack
#define RES_34618_9  1  // Firmware Version (R, String) - Coordinator firmware version
#define RES_34618_10 1  // TX Power (RW, Integer) - Transmit power (dBm)
#define RES_34618_11 1  // Security Level (RW, Integer) - Security level (0-7)
#define RES_34618_12 1  // Serial Port (RW, String) - Serial device path
#define RES_34618_13 1  // Baud Rate (RW, Integer) - Serial baud rate
#define RES_34618_14 1  // Route Table Size (R, Integer) - Number of route entries
#define RES_34618_15 1  // Neighbor Table Size (R, Integer) - Number of neighbor entries
#define RES_34618_16 1  // Form Network (E, Execute) - Form new network
#define RES_34618_17 1  // Leave Network (E, Execute) - Leave current network
#define RES_34618_18 1  // Permit Join (E, Execute) - Enable/disable join (arg: duration in seconds)
#define RES_34618_19 1  // Start Touchlink (E, Execute) - Initiate Touchlink commissioning
#define RES_34618_20 1  // Backup Network (E, Execute) - Backup network configuration

#endif // OBJ_W_34618_ZIGBEE_COORDINATOR
#endif // WPP_ZIGBEE_COORDINATOR_CONFIG_H
