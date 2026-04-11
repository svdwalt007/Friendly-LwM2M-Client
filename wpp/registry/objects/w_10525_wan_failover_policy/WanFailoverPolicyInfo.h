/*
 * WanFailoverPolicy Info (Object 10525)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WAN_FAILOVER_POLICY_INFO_H
#define WPP_WAN_FAILOVER_POLICY_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10525_WAN_FAILOVER_POLICY

#define WAN_FAILOVER_POLICY_OBJECT_ID 34611

static const Lwm2mObjectMetaInfo WAN_FAILOVER_POLICY_META_INFO = {
    .uri = "urn:waltech:lwm2m:34611",
    .id = WAN_FAILOVER_POLICY_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per failover policy
};

#endif // OBJ_W_10525_WAN_FAILOVER_POLICY
#endif // WPP_WAN_FAILOVER_POLICY_INFO_H
