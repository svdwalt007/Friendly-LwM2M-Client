/*
 * LanConfiguration Info (Object 10535)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_LAN_CONFIGURATION_INFO_H
#define WPP_LAN_CONFIGURATION_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_W_10535_LAN_CONFIGURATION

namespace wpp {

inline const ObjectInfo LAN_CONFIGURATION_OBJ_INFO = {
    /* Name */
    "LAN Configuration",
    /* Object ID */
    static_cast<OBJ_ID>(10535),
    /* URN */
    "urn:waltech:lwm2m:10535",
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

#endif // OBJ_W_10535_LAN_CONFIGURATION
#endif // WPP_LAN_CONFIGURATION_INFO_H
