/*
 * VpnConfiguration Info (Object 10537)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_VPN_CONFIGURATION_INFO_H
#define WPP_VPN_CONFIGURATION_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_W_10537_VPN_CONFIGURATION

namespace wpp {

inline const ObjectInfo VPN_CONFIGURATION_OBJ_INFO = {
    /* Name */
    "VPN Configuration",
    /* Object ID */
    static_cast<OBJ_ID>(10537),
    /* URN */
    "urn:waltech:lwm2m:10537",
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

#endif // OBJ_W_10537_VPN_CONFIGURATION
#endif // WPP_VPN_CONFIGURATION_INFO_H
