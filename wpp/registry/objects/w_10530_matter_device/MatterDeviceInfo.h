/*
 * MatterDevice Information
 * Object 10530 - Matter Device
 */

#ifndef WPP_W_10530_MATTER_DEVICE_INFO_H
#define WPP_W_10530_MATTER_DEVICE_INFO_H

// Object version information
#define MATTER_DEVICE_OBJECT_VERSION "1.0"

// Default values
#define DEVICE_UPDATE_INTERVAL_SEC 60

#include "ObjectInfo.h"

#ifdef OBJ_W_10530_MATTER_DEVICE

namespace wpp {

inline const ObjectInfo MATTER_DEVICE_OBJ_INFO = {
    /* Name */
    "Matter Device",
    /* Object ID */
    static_cast<OBJ_ID>(10530),
    /* URN */
    "urn:waltech:lwm2m:10530",
    /* Object version */
    {1, 0},
    /* LwM2M version */
    {1, 1},
    /* Is single */
    IS_SINGLE::MULTIPLE,
    /* Is mandatory */
    IS_MANDATORY::OPTIONAL,
    /* Object supported operations */
    ItemOp(ItemOp::READ | ItemOp::WRITE | ItemOp::DISCOVER)
};

} /* namespace wpp */

#endif // OBJ_W_10530_MATTER_DEVICE
#endif // WPP_W_10530_MATTER_DEVICE_INFO_H
