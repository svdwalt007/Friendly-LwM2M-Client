/*
 * WifiChannelOptimization Info (Object 34614)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WIFI_CHANNEL_OPTIMIZATION_INFO_H
#define WPP_WIFI_CHANNEL_OPTIMIZATION_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34614_WIFI_CHANNEL_OPTIMIZATION

#define WIFI_CHANNEL_OPTIMIZATION_OBJECT_ID 34614

static const Lwm2mObjectMetaInfo WIFI_CHANNEL_OPTIMIZATION_META_INFO = {
    .uri = "urn:waltech:lwm2m:34614",
    .id = WIFI_CHANNEL_OPTIMIZATION_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per wireless interface
};

#endif // OBJ_W_34614_WIFI_CHANNEL_OPTIMIZATION
#endif // WPP_WIFI_CHANNEL_OPTIMIZATION_INFO_H
