/*
 * ZigbeeGroup Info (Object 33462)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_GROUP_INFO_H
#define WPP_ZIGBEE_GROUP_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_33462_ZIGBEE_GROUP

#define ZIGBEE_GROUP_OBJECT_ID 33462

static const Lwm2mObjectMetaInfo ZIGBEE_GROUP_META_INFO = {
    .uri = "urn:waltech:lwm2m:33462",
    .id = ZIGBEE_GROUP_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per Zigbee group
};

#endif // OBJ_W_33462_ZIGBEE_GROUP
#endif // WPP_ZIGBEE_GROUP_INFO_H
