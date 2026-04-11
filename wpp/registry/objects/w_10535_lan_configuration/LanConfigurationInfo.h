/*
 * LanConfiguration Info (Object 10535)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_LAN_CONFIGURATION_INFO_H
#define WPP_LAN_CONFIGURATION_INFO_H

#ifdef OBJ_W_10535_LAN_CONFIGURATION

#define LAN_CONFIGURATION_OBJECT_ID 10535

static const Lwm2mObjectMetaInfo LAN_CONFIGURATION_META_INFO = {
    .uri = "urn:waltech:lwm2m:10535",
    .id = LAN_CONFIGURATION_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per LAN interface
};

#endif // OBJ_W_10535_LAN_CONFIGURATION
#endif // WPP_LAN_CONFIGURATION_INFO_H
