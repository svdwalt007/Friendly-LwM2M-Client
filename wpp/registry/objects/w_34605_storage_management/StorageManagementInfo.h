/*
 * StorageManagement Info (Object 34605)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_STORAGE_MANAGEMENT_INFO_H
#define WPP_STORAGE_MANAGEMENT_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34605_STORAGE_MANAGEMENT

#define STORAGE_MANAGEMENT_OBJECT_ID 34605

static const Lwm2mObjectMetaInfo STORAGE_MANAGEMENT_META_INFO = {
    .uri = "urn:waltech:lwm2m:34605",
    .id = STORAGE_MANAGEMENT_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per storage device
};

#endif // OBJ_W_34605_STORAGE_MANAGEMENT
#endif // WPP_STORAGE_MANAGEMENT_INFO_H
