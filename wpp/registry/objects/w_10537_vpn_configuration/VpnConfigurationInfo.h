/*
 * VpnConfiguration Info (Object 10537)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_VPN_CONFIGURATION_INFO_H
#define WPP_VPN_CONFIGURATION_INFO_H

#ifdef OBJ_W_10537_VPN_CONFIGURATION

#define VPN_CONFIGURATION_OBJECT_ID 10537

static const Lwm2mObjectMetaInfo VPN_CONFIGURATION_META_INFO = {
    .uri = "urn:waltech:lwm2m:10537",
    .id = VPN_CONFIGURATION_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per VPN connection
};

#endif // OBJ_W_10537_VPN_CONFIGURATION
#endif // WPP_VPN_CONFIGURATION_INFO_H
