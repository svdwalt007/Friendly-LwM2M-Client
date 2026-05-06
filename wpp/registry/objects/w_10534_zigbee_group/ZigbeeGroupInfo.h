/*
 * ZigbeeGroup Info (Object 10534)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_GROUP_INFO_H
#define WPP_ZIGBEE_GROUP_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_W_10534_ZIGBEE_GROUP

namespace wpp {

inline const ObjectInfo ZIGBEE_GROUP_OBJ_INFO = {
    /* Name */
    "Zigbee Group",
    /* Object ID */
    static_cast<OBJ_ID>(10534),
    /* URN */
    "urn:waltech:lwm2m:10534",
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

#endif // OBJ_W_10534_ZIGBEE_GROUP
#endif // WPP_ZIGBEE_GROUP_INFO_H
