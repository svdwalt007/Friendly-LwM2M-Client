/*
 * MultiWanHealthCheck Info (Object 10526)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_MULTIWAN_HEALTH_CHECK_INFO_H
#define WPP_MULTIWAN_HEALTH_CHECK_INFO_H
#include "ObjectInfo.h"


#ifdef OBJ_W_10526_MULTIWAN_HEALTH_CHECK

namespace wpp {

inline const ObjectInfo MULTIWAN_HEALTH_CHECK_OBJ_INFO = {
    /* Name */
    "Multi-WAN Health Check",
    /* Object ID */
    static_cast<OBJ_ID>(10526),
    /* URN */
    "urn:waltech:lwm2m:10526",
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

#endif // OBJ_W_10526_MULTIWAN_HEALTH_CHECK
#endif // WPP_MULTIWAN_HEALTH_CHECK_INFO_H
