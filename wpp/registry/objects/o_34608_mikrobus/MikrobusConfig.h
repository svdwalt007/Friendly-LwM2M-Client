/*
 * Walt Technologies MIKROBUS Object - Configuration
 *
 * Optional Resource Configuration
 * Set to 0 to disable optional resources, 1 to enable
 */

#ifndef MIKROBUS_CONFIG_H
#define MIKROBUS_CONFIG_H

/* Click Board Information - Optional */
#ifndef RES_34608_10
#define RES_34608_10 1  // Board Manufacturer
#endif
#ifndef RES_34608_11
#define RES_34608_11 1  // Board Model
#endif
#ifndef RES_34608_12
#define RES_34608_12 1  // Board Firmware Version
#endif
#ifndef RES_34608_13
#define RES_34608_13 1  // Board Serial Number
#endif

/* Interface Configuration - Optional */
#ifndef RES_34608_21
#define RES_34608_21 1  // I2C Address
#endif
#ifndef RES_34608_22
#define RES_34608_22 1  // SPI Mode
#endif
#ifndef RES_34608_23
#define RES_34608_23 1  // SPI Speed
#endif
#ifndef RES_34608_24
#define RES_34608_24 1  // UART Baud Rate
#endif

/* GPIO Control - Optional */
#ifndef RES_34608_31
#define RES_34608_31 1  // AN (Analog) Value
#endif
#ifndef RES_34608_32
#define RES_34608_32 1  // PWM Duty Cycle
#endif
#ifndef RES_34608_33
#define RES_34608_33 1  // PWM Frequency
#endif
#ifndef RES_34608_34
#define RES_34608_34 1  // INT Pin State
#endif

/* Control Actions - Optional */
#ifndef RES_34608_101
#define RES_34608_101 1  // Initialize Click Board
#endif
#ifndef RES_34608_102
#define RES_34608_102 1  // Reset Click Board
#endif
#ifndef RES_34608_103
#define RES_34608_103 1  // Shutdown Click Board
#endif

/* Advanced Features - Optional */
#ifndef RES_34608_110
#define RES_34608_110 1  // Read Manifest
#endif
#ifndef RES_34608_111
#define RES_34608_111 1  // Click Board Manifest Data
#endif

#endif // MIKROBUS_CONFIG_H
