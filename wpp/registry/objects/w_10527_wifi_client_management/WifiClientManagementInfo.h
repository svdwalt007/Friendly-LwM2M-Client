/*
 * WifiClientManagement Info (Object 10527)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WIFI_CLIENT_MANAGEMENT_INFO_H
#define WPP_WIFI_CLIENT_MANAGEMENT_INFO_H
#include "ObjectInfo.h"


#ifdef OBJ_W_10527_WIFI_CLIENT_MANAGEMENT

namespace wpp {

inline const ObjectInfo WIFI_CLIENT_MANAGEMENT_OBJ_INFO = {
    /* Name */
    "WiFi Client Management",
    /* Object ID */
    static_cast<OBJ_ID>(10527),
    /* URN */
    "urn:waltech:lwm2m:10527",
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

#endif // OBJ_W_10527_WIFI_CLIENT_MANAGEMENT
#endif // WPP_WIFI_CLIENT_MANAGEMENT_INFO_H
