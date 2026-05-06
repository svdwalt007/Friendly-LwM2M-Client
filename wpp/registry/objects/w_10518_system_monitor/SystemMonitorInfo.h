/*
 * SystemMonitor Info (Object 10518)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_SYSTEM_MONITOR_INFO_H
#define WPP_SYSTEM_MONITOR_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_W_10518_SYSTEM_MONITOR

#define SYSTEM_MONITOR_OBJECT_ID 10518

static const Lwm2mObjectMetaInfo SYSTEM_MONITOR_META_INFO = {
    .uri = "urn:waltech:lwm2m:10518",
    .id = SYSTEM_MONITOR_OBJECT_ID,
    .version = {1, 0},
    .multi = false  // Single instance - system-wide monitoring
};

#endif // OBJ_W_10518_SYSTEM_MONITOR
#endif // WPP_SYSTEM_MONITOR_INFO_H
