/*
 * ZigbeeDevice Info (Object 10533)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_DEVICE_INFO_H
#define WPP_ZIGBEE_DEVICE_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_W_10533_ZIGBEE_DEVICE

namespace wpp {

inline const ObjectInfo ZIGBEE_DEVICE_OBJ_INFO = {
    /* Name */
    "Zigbee Device",
    /* Object ID */
    static_cast<OBJ_ID>(10533),
    /* URN */
    "urn:waltech:lwm2m:10533",
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

#endif // OBJ_W_10533_ZIGBEE_DEVICE
#endif // WPP_ZIGBEE_DEVICE_INFO_H
