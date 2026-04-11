/*
 * ZigbeeCoordinator Info (Object 10532)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_COORDINATOR_INFO_H
#define WPP_ZIGBEE_COORDINATOR_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10532_ZIGBEE_COORDINATOR

#define ZIGBEE_COORDINATOR_OBJECT_ID 34618

static const Lwm2mObjectMetaInfo ZIGBEE_COORDINATOR_META_INFO = {
    .uri = "urn:waltech:lwm2m:34618",
    .id = ZIGBEE_COORDINATOR_OBJECT_ID,
    .version = {1, 0},
    .multi = false  // Single instance - coordinator singleton
};

#endif // OBJ_W_10532_ZIGBEE_COORDINATOR
#endif // WPP_ZIGBEE_COORDINATOR_INFO_H
