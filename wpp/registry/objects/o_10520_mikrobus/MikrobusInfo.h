/*
 * Walt Technologies MIKROBUS Object - Metadata
 *
 * Object ID: 10520
 * Version: 1.0
 * URN: urn:oma:lwm2m:x:10520:1.0
 */

#ifndef MIKROBUS_INFO_H
#define MIKROBUS_INFO_H

#include "WppTypes.h"
#include "ObjectInfo.h"
#include "ObjectID.h"

namespace wpp {

/* Object metadata */
constexpr OBJ_ID MIKROBUS_OBJ_ID = 10520;
constexpr const char* MIKROBUS_URN = "urn:oma:lwm2m:x:10520:1.0";
constexpr const char* MIKROBUS_VERSION = "1.0";
constexpr const char* MIKROBUS_NAME = "MIKROBUS";

/* Object supports multiple instances (one per socket) */
constexpr bool MIKROBUS_MULTIPLE_INSTANCES = true;

/* Object is optional (not mandatory for LwM2M compliance) */
constexpr bool MIKROBUS_MANDATORY = false;

/* Object info structure for registry */
inline const ObjectInfo MIKROBUS_OBJ_INFO = {
    "MIKROBUS",                         // Name
    OBJ_ID::MIKROBUS,                   // Object ID
    "urn:oma:lwm2m:x:10520:1.0",       // URN
    {1, 0},                             // Object version
    {1, 0},                             // LwM2M version
    IS_SINGLE::MULTIPLE,                // Multiple instances
    IS_MANDATORY::OPTIONAL,             // Optional
    ItemOp(ItemOp::READ|ItemOp::WRITE|ItemOp::DISCOVER|ItemOp::EXECUTE),
};

} // namespace wpp

#endif // MIKROBUS_INFO_H
