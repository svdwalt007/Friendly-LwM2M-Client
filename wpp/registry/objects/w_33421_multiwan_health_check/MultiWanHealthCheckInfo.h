/*
 * MultiWanHealthCheck Info (Object 33421)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_MULTIWAN_HEALTH_CHECK_INFO_H
#define WPP_MULTIWAN_HEALTH_CHECK_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_33421_MULTIWAN_HEALTH_CHECK

#define MULTIWAN_HEALTH_CHECK_OBJECT_ID 33421

static const Lwm2mObjectMetaInfo MULTIWAN_HEALTH_CHECK_META_INFO = {
    .uri = "urn:waltech:lwm2m:33421",
    .id = MULTIWAN_HEALTH_CHECK_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per WAN interface
};

#endif // OBJ_W_33421_MULTIWAN_HEALTH_CHECK
#endif // WPP_MULTIWAN_HEALTH_CHECK_INFO_H
