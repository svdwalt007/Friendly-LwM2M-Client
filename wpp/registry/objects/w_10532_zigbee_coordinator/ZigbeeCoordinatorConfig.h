/*
 * ZigbeeCoordinator Configuration (Object 10532)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_COORDINATOR_CONFIG_H
#define WPP_ZIGBEE_COORDINATOR_CONFIG_H

#ifdef OBJ_W_10532_ZIGBEE_COORDINATOR

// Object ID
#define ZIGBEE_COORDINATOR_OBJECT_ID (wpp::OBJ_ID)10532

// Resource enable/disable flags
// All resources are enabled by default

// Zigbee Coordinator Resources
#define RES_10532_0  1  // Network State (R, Integer) - 0=offline, 1=forming, 2=joined, 3=ready, 4=leaving, 5=error
#define RES_10532_1  1  // PAN ID (RW, Integer) - 16-bit PAN identifier
#define RES_10532_2  1  // Extended PAN ID (RW, String) - 64-bit extended PAN ID (hex)
#define RES_10532_3  1  // Channel (RW, Integer) - Radio channel (11-26)
#define RES_10532_4  1  // Network Key (W, Opaque) - 128-bit network encryption key
#define RES_10532_5  1  // Permit Join (R, Boolean) - Permit join status
#define RES_10532_6  1  // Device Count (R, Integer) - Number of joined devices
#define RES_10532_7  1  // Coordinator IEEE Address (R, String) - 64-bit IEEE address (hex)
#define RES_10532_8  1  // Coordinator Type (R, Integer) - 0=unknown, 1=EZSP, 2=Z-Stack
#define RES_10532_9  1  // Firmware Version (R, String) - Coordinator firmware version
#define RES_10532_10 1  // TX Power (RW, Integer) - Transmit power (dBm)
#define RES_10532_11 1  // Security Level (RW, Integer) - Security level (0-7)
#define RES_10532_12 1  // Serial Port (RW, String) - Serial device path
#define RES_10532_13 1  // Baud Rate (RW, Integer) - Serial baud rate
#define RES_10532_14 1  // Route Table Size (R, Integer) - Number of route entries
#define RES_10532_15 1  // Neighbor Table Size (R, Integer) - Number of neighbor entries
#define RES_10532_16 1  // Form Network (E, Execute) - Form new network
#define RES_10532_17 1  // Leave Network (E, Execute) - Leave current network
#define RES_10532_18 1  // Permit Join (E, Execute) - Enable/disable join (arg: duration in seconds)
#define RES_10532_19 1  // Start Touchlink (E, Execute) - Initiate Touchlink commissioning
#define RES_10532_20 1  // Backup Network (E, Execute) - Backup network configuration

#endif // OBJ_W_10532_ZIGBEE_COORDINATOR
#endif // WPP_ZIGBEE_COORDINATOR_CONFIG_H
