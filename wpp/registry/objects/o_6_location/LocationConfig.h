/*
 * Location Object Configuration
 * OMA LWM2M Object ID: 6
 */

#ifndef WPP_LOCATION_CONFIG_H
#define WPP_LOCATION_CONFIG_H

#ifdef OBJ_O_6_LOCATION

/* --------------- Config block 0 start --------------- */
// Mandatory resources - always enabled
// RES_6_0 - Latitude (Float) - Always enabled (mandatory)
// RES_6_1 - Longitude (Float) - Always enabled (mandatory)
// RES_6_5 - Timestamp (Time) - Always enabled (mandatory)

// Optional resources - can be enabled/disabled
#define RES_6_2 1  // Altitude (Float, meters)
#define RES_6_3 1  // Radius (Float, meters - uncertainty)
#define RES_6_4 0  // Velocity (Opaque, 3GPP format) - disabled by default
#define RES_6_6 1  // Speed (Float, m/s)
/* --------------- Config block 0 end --------------- */

#endif // OBJ_O_6_LOCATION
#endif // WPP_LOCATION_CONFIG_H
