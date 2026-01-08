/*
 * FirewallConfig Info (Object 10521)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_FIREWALL_CONFIG_INFO_H
#define WPP_FIREWALL_CONFIG_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10521_FIREWALL_CONFIG

#define FIREWALL_CONFIG_OBJECT_ID 10521

static const Lwm2mObjectMetaInfo FIREWALL_CONFIG_META_INFO = {
    .uri = "urn:waltech:lwm2m:10521",
    .id = FIREWALL_CONFIG_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per firewall rule
};

#endif // OBJ_W_10521_FIREWALL_CONFIG
#endif // WPP_FIREWALL_CONFIG_INFO_H
