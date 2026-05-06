/*
 * RoutingTable Info (Object 10536)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_ROUTING_TABLE_INFO_H
#define WPP_ROUTING_TABLE_INFO_H
#include "ObjectInfo.h"


#ifdef OBJ_W_10536_ROUTING_TABLE

namespace wpp {

inline const ObjectInfo ROUTING_TABLE_OBJ_INFO = {
    /* Name */
    "Routing Table",
    /* Object ID */
    static_cast<OBJ_ID>(10536),
    /* URN */
    "urn:waltech:lwm2m:10536",
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

#endif // OBJ_W_10536_ROUTING_TABLE
#endif // WPP_ROUTING_TABLE_INFO_H
