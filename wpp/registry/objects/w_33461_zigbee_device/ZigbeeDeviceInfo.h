/*
 * ZigbeeDevice Info (Object 33461)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_DEVICE_INFO_H
#define WPP_ZIGBEE_DEVICE_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_33461_ZIGBEE_DEVICE

#define ZIGBEE_DEVICE_OBJECT_ID 33461

static const Lwm2mObjectMetaInfo ZIGBEE_DEVICE_META_INFO = {
    .uri = "urn:waltech:lwm2m:33461",
    .id = ZIGBEE_DEVICE_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per Zigbee device
};

#endif // OBJ_W_33461_ZIGBEE_DEVICE
#endif // WPP_ZIGBEE_DEVICE_INFO_H
