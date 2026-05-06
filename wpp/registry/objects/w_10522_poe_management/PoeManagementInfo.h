/*
 * PoeManagement Info (Object 10522)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_POE_MANAGEMENT_INFO_H
#define WPP_POE_MANAGEMENT_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_W_10522_POE_MANAGEMENT

#define POE_MANAGEMENT_OBJECT_ID 10522

static const Lwm2mObjectMetaInfo POE_MANAGEMENT_META_INFO = {
    .uri = "urn:waltech:lwm2m:10522",
    .id = POE_MANAGEMENT_OBJECT_ID,
    .version = {1, 0},
    .multi = false  // Single instance - one PoE controller
};

#endif // OBJ_W_10522_POE_MANAGEMENT
#endif // WPP_POE_MANAGEMENT_INFO_H
