/*
 * WifiClientManagement Info (Object 10527)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WIFI_CLIENT_MANAGEMENT_INFO_H
#define WPP_WIFI_CLIENT_MANAGEMENT_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10527_WIFI_CLIENT_MANAGEMENT

#define WIFI_CLIENT_MANAGEMENT_OBJECT_ID 34613

static const Lwm2mObjectMetaInfo WIFI_CLIENT_MANAGEMENT_META_INFO = {
    .uri = "urn:waltech:lwm2m:34613",
    .id = WIFI_CLIENT_MANAGEMENT_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per connected WiFi client
};

#endif // OBJ_W_10527_WIFI_CLIENT_MANAGEMENT
#endif // WPP_WIFI_CLIENT_MANAGEMENT_INFO_H
