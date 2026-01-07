/*
 * EthernetInterface Info (Object 34602)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_ETHERNET_INTERFACE_INFO_H
#define WPP_ETHERNET_INTERFACE_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34602_ETHERNET_INTERFACE

#define ETHERNET_INTERFACE_OBJECT_ID 34602

static const Lwm2mObjectMetaInfo ETHERNET_INTERFACE_META_INFO = {
    .uri = "urn:waltech:lwm2m:34602",
    .id = ETHERNET_INTERFACE_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per ethernet port
};

#endif // OBJ_W_34602_ETHERNET_INTERFACE
#endif // WPP_ETHERNET_INTERFACE_INFO_H
