/*
 * BearerSelection Info
 * OMA LwM2M Object ID 13
 * Preferred network bearer selection
 */

#ifndef WPP_BEARER_SELECTION_INFO_H
#define WPP_BEARER_SELECTION_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_O_13_BEARER_SELECTION

#define BEARER_SELECTION_OBJECT_ID 13

static const Lwm2mObjectMetaInfo BEARER_SELECTION_META_INFO = {
    .uri = "urn:oma:lwm2m:oma:13",
    .id = BEARER_SELECTION_OBJECT_ID,
    .version = {1, 0},
    .multi = false  // Single instance
};

#endif // OBJ_O_13_BEARER_SELECTION
#endif // WPP_BEARER_SELECTION_INFO_H
