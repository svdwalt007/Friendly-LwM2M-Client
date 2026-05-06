/*
 * RouterManagement Info (Object 10513)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_ROUTER_MANAGEMENT_INFO_H
#define WPP_ROUTER_MANAGEMENT_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_W_10513_ROUTER_MANAGEMENT

#define ROUTER_MANAGEMENT_OBJECT_ID 10513

static const Lwm2mObjectMetaInfo ROUTER_MANAGEMENT_META_INFO = {
    .uri = "urn:waltech:lwm2m:10513",
    .id = ROUTER_MANAGEMENT_OBJECT_ID,
    .version = {1, 0},
    .multi = false  // Single instance - one router configuration
};

#endif // OBJ_W_10513_ROUTER_MANAGEMENT
#endif // WPP_ROUTER_MANAGEMENT_INFO_H
