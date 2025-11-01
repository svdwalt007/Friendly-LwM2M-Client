/*
 * Walt Technologies Starlink Terminal Object - Configuration
 *
 * Optional Resource Configuration
 * Set to 0 to disable optional resources, 1 to enable
 */

#ifndef STARLINK_TERMINAL_CONFIG_H
#define STARLINK_TERMINAL_CONFIG_H

/* Network Performance & Status */
#ifndef RES_34600_14
#define RES_34600_14 1  // Seconds to First Slot
#endif
#ifndef RES_34600_15
#define RES_34600_15 1  // SNR
#endif
#ifndef RES_34600_16
#define RES_34600_16 1  // Is SNR Above Noise Floor
#endif
#ifndef RES_34600_17
#define RES_34600_17 1  // Download Usage
#endif
#ifndef RES_34600_18
#define RES_34600_18 1  // Upload Usage
#endif

/* Obstruction Monitoring - Advanced */
#ifndef RES_34600_32
#define RES_34600_32 1  // Obstruction Duration
#endif
#ifndef RES_34600_33
#define RES_34600_33 1  // Obstruction Interval
#endif
#ifndef RES_34600_34
#define RES_34600_34 1  // Obstruction Valid S
#endif
#ifndef RES_34600_35
#define RES_34600_35 1  // Wedges Fraction Obstructed
#endif

/* Dish Alignment */
#ifndef RES_34600_42
#define RES_34600_42 1  // Dish Stowed
#endif
#ifndef RES_34600_43
#define RES_34600_43 1  // Dish Alignment Status
#endif

/* Alert System - Extended Alerts */
#ifndef RES_34600_62
#define RES_34600_62 1  // Low Motor Current
#endif
#ifndef RES_34600_63
#define RES_34600_63 1  // Lower Signal Than Predicted
#endif
#ifndef RES_34600_64
#define RES_34600_64 1  // Obstruction Map Reset
#endif
#ifndef RES_34600_65
#define RES_34600_65 1  // Dish Water Detected
#endif
#ifndef RES_34600_66
#define RES_34600_66 1  // Router Water Detected
#endif

/* GPS Location - Optional if GPS disabled */
#ifndef RES_34600_83
#define RES_34600_83 1  // Latitude
#endif
#ifndef RES_34600_84
#define RES_34600_84 1  // Longitude
#endif
#ifndef RES_34600_85
#define RES_34600_85 1  // Altitude
#endif

/* Power Management - Advanced */
#ifndef RES_34600_92
#define RES_34600_92 1  // Min Power
#endif
#ifndef RES_34600_93
#define RES_34600_93 1  // Max Power
#endif
#ifndef RES_34600_94
#define RES_34600_94 1  // Total Energy
#endif

/* Control Actions - Advanced */
#ifndef RES_34600_103
#define RES_34600_103 0  // Factory Reset (disabled by default for safety)
#endif
#ifndef RES_34600_104
#define RES_34600_104 1  // Get Obstruction Map
#endif
#ifndef RES_34600_105
#define RES_34600_105 1  // Obstruction Map Data
#endif

/* Historical Data - Advanced Statistics */
#ifndef RES_34600_114
#define RES_34600_114 1  // Stdev Full Ping Latency
#endif
#ifndef RES_34600_118
#define RES_34600_118 1  // Latency Deciles
#endif
#ifndef RES_34600_119
#define RES_34600_119 1  // Load Bucket Stats
#endif

#endif // STARLINK_TERMINAL_CONFIG_H
