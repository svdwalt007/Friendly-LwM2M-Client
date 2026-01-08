/*
 * MultiWanHealthCheck Info (Object 34612)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_MULTIWAN_HEALTH_CHECK_INFO_H
#define WPP_MULTIWAN_HEALTH_CHECK_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34612_MULTIWAN_HEALTH_CHECK

#define MULTIWAN_HEALTH_CHECK_OBJECT_ID 34612

static const Lwm2mObjectMetaInfo MULTIWAN_HEALTH_CHECK_META_INFO = {
    .uri = "urn:waltech:lwm2m:34612",
    .id = MULTIWAN_HEALTH_CHECK_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per WAN interface
};

#endif // OBJ_W_34612_MULTIWAN_HEALTH_CHECK
#endif // WPP_MULTIWAN_HEALTH_CHECK_INFO_H
