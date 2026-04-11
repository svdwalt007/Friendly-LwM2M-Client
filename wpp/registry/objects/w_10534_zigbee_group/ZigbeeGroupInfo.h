/*
 * ZigbeeGroup Info (Object 10534)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_GROUP_INFO_H
#define WPP_ZIGBEE_GROUP_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10534_ZIGBEE_GROUP

#define ZIGBEE_GROUP_OBJECT_ID 34620

static const Lwm2mObjectMetaInfo ZIGBEE_GROUP_META_INFO = {
    .uri = "urn:waltech:lwm2m:34620",
    .id = ZIGBEE_GROUP_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per Zigbee group
};

#endif // OBJ_W_10534_ZIGBEE_GROUP
#endif // WPP_ZIGBEE_GROUP_INFO_H
