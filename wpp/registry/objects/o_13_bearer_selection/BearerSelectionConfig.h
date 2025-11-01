/*
 * BearerSelection Config
 * OMA LwM2M Object ID 13
 */

#ifndef WPP_BEARER_SELECTION_CONFIG_H
#define WPP_BEARER_SELECTION_CONFIG_H

#ifdef OBJ_O_13_BEARER_SELECTION

/* --------------- Config block 0 start --------------- */
// All resources enabled for comprehensive bearer selection
#define RES_13_0 1   // Preferred Communications Bearer
#define RES_13_1 1   // Acceptable RSSI (GSM)
#define RES_13_2 1   // Acceptable RSCP (UMTS)
#define RES_13_3 1   // Acceptable RSRP (LTE)
#define RES_13_4 1   // Acceptable RSSI (WiFi/WLAN)
#define RES_13_5 1   // Cell Lock List
#define RES_13_6 1   // Operator List
#define RES_13_7 1   // Operator List Mode
#define RES_13_8 1   // Available Network Bearers
#define RES_13_9 1   // Acceptable Signal Strength Variation
#define RES_13_10 1  // Higher Priority Plmn Search Timer
#define RES_13_11 1  // Attach Without PDN Connection
/* --------------- Config block 0 end --------------- */

#endif // OBJ_O_13_BEARER_SELECTION
#endif // WPP_BEARER_SELECTION_CONFIG_H
