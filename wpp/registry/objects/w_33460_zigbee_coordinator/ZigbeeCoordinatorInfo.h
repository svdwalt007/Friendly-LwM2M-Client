/*
 * ZigbeeCoordinator Info (Object 33460)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_COORDINATOR_INFO_H
#define WPP_ZIGBEE_COORDINATOR_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_33460_ZIGBEE_COORDINATOR

#define ZIGBEE_COORDINATOR_OBJECT_ID 33460

static const Lwm2mObjectMetaInfo ZIGBEE_COORDINATOR_META_INFO = {
    .uri = "urn:waltech:lwm2m:33460",
    .id = ZIGBEE_COORDINATOR_OBJECT_ID,
    .version = {1, 0},
    .multi = false  // Single instance - coordinator singleton
};

#endif // OBJ_W_33460_ZIGBEE_COORDINATOR
#endif // WPP_ZIGBEE_COORDINATOR_INFO_H
