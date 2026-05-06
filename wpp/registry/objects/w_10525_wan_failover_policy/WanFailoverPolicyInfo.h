/*
 * WanFailoverPolicy Info (Object 10525)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WAN_FAILOVER_POLICY_INFO_H
#define WPP_WAN_FAILOVER_POLICY_INFO_H
#include "ObjectInfo.h"


#ifdef OBJ_W_10525_WAN_FAILOVER_POLICY

namespace wpp {

inline const ObjectInfo WAN_FAILOVER_POLICY_OBJ_INFO = {
    /* Name */
    "WAN Failover Policy",
    /* Object ID */
    static_cast<OBJ_ID>(10525),
    /* URN */
    "urn:waltech:lwm2m:10525",
    /* Object version */
    {1, 0},
    /* LwM2M version */
    {1, 1},
    /* Is single */
    IS_SINGLE::MULTIPLE,
    /* Is mandatory */
    IS_MANDATORY::OPTIONAL,
    /* Object supported operations */
    ItemOp(ItemOp::READ | ItemOp::WRITE | ItemOp::DISCOVER | ItemOp::EXECUTE)
};

} /* namespace wpp */

#endif // OBJ_W_10525_WAN_FAILOVER_POLICY
#endif // WPP_WAN_FAILOVER_POLICY_INFO_H
