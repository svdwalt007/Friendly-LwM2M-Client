/*
 * MultiWanHealthCheck Info (Object 10526)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_MULTIWAN_HEALTH_CHECK_INFO_H
#define WPP_MULTIWAN_HEALTH_CHECK_INFO_H

#ifdef OBJ_W_10526_MULTIWAN_HEALTH_CHECK

#define MULTIWAN_HEALTH_CHECK_OBJECT_ID 10526

static const Lwm2mObjectMetaInfo MULTIWAN_HEALTH_CHECK_META_INFO = {
    .uri = "urn:waltech:lwm2m:10526",
    .id = MULTIWAN_HEALTH_CHECK_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per WAN interface
};

#endif // OBJ_W_10526_MULTIWAN_HEALTH_CHECK
#endif // WPP_MULTIWAN_HEALTH_CHECK_INFO_H
