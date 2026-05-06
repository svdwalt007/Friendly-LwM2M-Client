/*
 * MatterBridge Information
 * Object 10529 - Matter Bridge
 */

#ifndef WPP_W_10529_MATTER_BRIDGE_INFO_H
#define WPP_W_10529_MATTER_BRIDGE_INFO_H

// Object version information
#define MATTER_BRIDGE_OBJECT_VERSION "1.0"

// Default values
#define DEFAULT_VENDOR_ID 0xFFF1
#define DEFAULT_PRODUCT_ID 0x8000

#include "ObjectInfo.h"

#ifdef OBJ_W_10529_MATTER_BRIDGE

namespace wpp {

inline const ObjectInfo MATTER_BRIDGE_OBJ_INFO = {
    /* Name */
    "Matter Bridge",
    /* Object ID */
    static_cast<OBJ_ID>(10529),
    /* URN */
    "urn:waltech:lwm2m:10529",
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

#endif // OBJ_W_10529_MATTER_BRIDGE
#endif // WPP_W_10529_MATTER_BRIDGE_INFO_H
