/*
 * PoeManagement Info (Object 34610)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_POE_MANAGEMENT_INFO_H
#define WPP_POE_MANAGEMENT_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34610_POE_MANAGEMENT

#define POE_MANAGEMENT_OBJECT_ID 34610

static const Lwm2mObjectMetaInfo POE_MANAGEMENT_META_INFO = {
    .uri = "urn:waltech:lwm2m:34610",
    .id = POE_MANAGEMENT_OBJECT_ID,
    .version = {1, 0},
    .multi = false  // Single instance - one PoE controller
};

#endif // OBJ_W_34610_POE_MANAGEMENT
#endif // WPP_POE_MANAGEMENT_INFO_H
