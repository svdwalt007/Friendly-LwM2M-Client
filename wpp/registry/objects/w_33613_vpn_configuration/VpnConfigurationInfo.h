/*
 * VpnConfiguration Info (Object 33613)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_VPN_CONFIGURATION_INFO_H
#define WPP_VPN_CONFIGURATION_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_33613_VPN_CONFIGURATION

#define VPN_CONFIGURATION_OBJECT_ID 33613

static const Lwm2mObjectMetaInfo VPN_CONFIGURATION_META_INFO = {
    .uri = "urn:waltech:lwm2m:33613",
    .id = VPN_CONFIGURATION_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per VPN connection
};

#endif // OBJ_W_33613_VPN_CONFIGURATION
#endif // WPP_VPN_CONFIGURATION_INFO_H
