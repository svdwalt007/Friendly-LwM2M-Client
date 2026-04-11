/*
 * ZigbeeDevice Info (Object 10533)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_DEVICE_INFO_H
#define WPP_ZIGBEE_DEVICE_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10533_ZIGBEE_DEVICE

#define ZIGBEE_DEVICE_OBJECT_ID 34619

static const Lwm2mObjectMetaInfo ZIGBEE_DEVICE_META_INFO = {
    .uri = "urn:waltech:lwm2m:34619",
    .id = ZIGBEE_DEVICE_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per Zigbee device
};

#endif // OBJ_W_10533_ZIGBEE_DEVICE
#endif // WPP_ZIGBEE_DEVICE_INFO_H
