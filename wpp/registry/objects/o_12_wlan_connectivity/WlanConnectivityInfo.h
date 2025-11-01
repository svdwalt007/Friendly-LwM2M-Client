/*
 * WlanConnectivity Info
 * Generated for OpenWRT One Router
 * Walt Technologies
 */

#ifndef WPP_WLAN_CONNECTIVITY_INFO_H
#define WPP_WLAN_CONNECTIVITY_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_O_12_WLAN_CONNECTIVITY

#define WLAN_CONNECTIVITY_OBJECT_ID 12

static const Lwm2mObjectMetaInfo WLAN_CONNECTIVITY_META_INFO = {
    .uri = "urn:oma:lwm2m:oma:12",
    .id = WLAN_CONNECTIVITY_OBJECT_ID,
    .version = {1, 1},
    .multi = true  // Multiple instances for multiple WiFi interfaces
};

#endif // OBJ_O_12_WLAN_CONNECTIVITY
#endif // WPP_WLAN_CONNECTIVITY_INFO_H
