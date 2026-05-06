/*
 * WifiChannelOptimization Info (Object 10528)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_WIFI_CHANNEL_OPTIMIZATION_INFO_H
#define WPP_WIFI_CHANNEL_OPTIMIZATION_INFO_H
#include "ObjectInfo.h"


#ifdef OBJ_W_10528_WIFI_CHANNEL_OPTIMIZATION

namespace wpp {

inline const ObjectInfo WIFI_CHANNEL_OPTIMIZATION_OBJ_INFO = {
    /* Name */
    "WiFi Channel Optimization",
    /* Object ID */
    static_cast<OBJ_ID>(10528),
    /* URN */
    "urn:waltech:lwm2m:10528",
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

#endif // OBJ_W_10528_WIFI_CHANNEL_OPTIMIZATION
#endif // WPP_WIFI_CHANNEL_OPTIMIZATION_INFO_H
