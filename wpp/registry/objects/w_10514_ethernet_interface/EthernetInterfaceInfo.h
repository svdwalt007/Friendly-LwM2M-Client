/*
 * EthernetInterface Info (Object 10514)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_ETHERNET_INTERFACE_INFO_H
#define WPP_ETHERNET_INTERFACE_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10514_ETHERNET_INTERFACE

#define ETHERNET_INTERFACE_OBJECT_ID 10514

static const Lwm2mObjectMetaInfo ETHERNET_INTERFACE_META_INFO = {
    .uri = "urn:waltech:lwm2m:10514",
    .id = ETHERNET_INTERFACE_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per ethernet port
};

#endif // OBJ_W_10514_ETHERNET_INTERFACE
#endif // WPP_ETHERNET_INTERFACE_INFO_H
