/*
 * ZigbeeCoordinator Info (Object 10532)
 * Walt Technologies - Zigbee Integration
 */

#ifndef WPP_ZIGBEE_COORDINATOR_INFO_H
#define WPP_ZIGBEE_COORDINATOR_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_W_10532_ZIGBEE_COORDINATOR

namespace wpp {

inline const ObjectInfo ZIGBEE_COORDINATOR_OBJ_INFO = {
    /* Name */
    "Zigbee Coordinator",
    /* Object ID */
    static_cast<OBJ_ID>(10532),
    /* URN */
    "urn:waltech:lwm2m:10532",
    /* Object version */
    {1, 0},
    /* LwM2M version */
    {1, 1},
    /* Is single */
    IS_SINGLE::SINGLE,
    /* Is mandatory */
    IS_MANDATORY::OPTIONAL,
    /* Object supported operations */
    ItemOp(ItemOp::READ | ItemOp::WRITE | ItemOp::DISCOVER | ItemOp::EXECUTE)
};

} /* namespace wpp */

#endif // OBJ_W_10532_ZIGBEE_COORDINATOR
#endif // WPP_ZIGBEE_COORDINATOR_INFO_H
