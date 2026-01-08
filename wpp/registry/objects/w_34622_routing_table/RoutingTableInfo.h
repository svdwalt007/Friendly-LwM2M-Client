/*
 * RoutingTable Info (Object 34622)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_ROUTING_TABLE_INFO_H
#define WPP_ROUTING_TABLE_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34622_ROUTING_TABLE

#define ROUTING_TABLE_OBJECT_ID 34622

static const Lwm2mObjectMetaInfo ROUTING_TABLE_META_INFO = {
    .uri = "urn:waltech:lwm2m:34622",
    .id = ROUTING_TABLE_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per route
};

#endif // OBJ_W_34622_ROUTING_TABLE
#endif // WPP_ROUTING_TABLE_INFO_H
