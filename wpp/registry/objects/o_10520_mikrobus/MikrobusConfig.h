/*
 * Walt Technologies MIKROBUS Object - Configuration
 *
 * Optional Resource Configuration
 * Set to 0 to disable optional resources, 1 to enable
 */

#ifndef MIKROBUS_CONFIG_H
#define MIKROBUS_CONFIG_H

/* Click Board Information - Optional */
#ifndef RES_10520_10
#define RES_10520_10 1  // Board Manufacturer
#endif
#ifndef RES_10520_11
#define RES_10520_11 1  // Board Model
#endif
#ifndef RES_10520_12
#define RES_10520_12 1  // Board Firmware Version
#endif
#ifndef RES_10520_13
#define RES_10520_13 1  // Board Serial Number
#endif

/* Interface Configuration - Optional */
#ifndef RES_10520_21
#define RES_10520_21 1  // I2C Address
#endif
#ifndef RES_10520_22
#define RES_10520_22 1  // SPI Mode
#endif
#ifndef RES_10520_23
#define RES_10520_23 1  // SPI Speed
#endif
#ifndef RES_10520_24
#define RES_10520_24 1  // UART Baud Rate
#endif

/* GPIO Control - Optional */
#ifndef RES_10520_31
#define RES_10520_31 1  // AN (Analog) Value
#endif
#ifndef RES_10520_32
#define RES_10520_32 1  // PWM Duty Cycle
#endif
#ifndef RES_10520_33
#define RES_10520_33 1  // PWM Frequency
#endif
#ifndef RES_10520_34
#define RES_10520_34 1  // INT Pin State
#endif

/* Control Actions - Optional */
#ifndef RES_10520_101
#define RES_10520_101 1  // Initialize Click Board
#endif
#ifndef RES_10520_102
#define RES_10520_102 1  // Reset Click Board
#endif
#ifndef RES_10520_103
#define RES_10520_103 1  // Shutdown Click Board
#endif

/* Advanced Features - Optional */
#ifndef RES_10520_110
#define RES_10520_110 1  // Read Manifest
#endif
#ifndef RES_10520_111
#define RES_10520_111 1  // Click Board Manifest Data
#endif

#endif // MIKROBUS_CONFIG_H
