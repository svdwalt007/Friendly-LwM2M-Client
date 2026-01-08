/*
 * WanFailoverPolicy Info (Object 33420)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WAN_FAILOVER_POLICY_INFO_H
#define WPP_WAN_FAILOVER_POLICY_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_33420_WAN_FAILOVER_POLICY

#define WAN_FAILOVER_POLICY_OBJECT_ID 33420

static const Lwm2mObjectMetaInfo WAN_FAILOVER_POLICY_META_INFO = {
    .uri = "urn:waltech:lwm2m:33420",
    .id = WAN_FAILOVER_POLICY_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per failover policy
};

#endif // OBJ_W_33420_WAN_FAILOVER_POLICY
#endif // WPP_WAN_FAILOVER_POLICY_INFO_H
