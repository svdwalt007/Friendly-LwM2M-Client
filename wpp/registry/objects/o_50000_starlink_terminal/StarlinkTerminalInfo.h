/*
 * Walt Technologies Starlink Terminal Object - Metadata
 *
 * Object ID: 50000
 * Version: 1.0
 * URN: urn:oma:lwm2m:x:50000:1.0
 */

#ifndef STARLINK_TERMINAL_INFO_H
#define STARLINK_TERMINAL_INFO_H

#include "WppTypes.h"

namespace wpp {

/* Object metadata */
constexpr OBJ_ID STARLINK_TERMINAL_OBJ_ID = 50000;
constexpr const char* STARLINK_TERMINAL_URN = "urn:oma:lwm2m:x:50000:1.0";
constexpr const char* STARLINK_TERMINAL_VERSION = "1.0";
constexpr const char* STARLINK_TERMINAL_NAME = "Starlink Terminal";

/* Object supports single instance */
constexpr bool STARLINK_TERMINAL_MULTIPLE_INSTANCES = false;

/* Object is optional (not mandatory for LwM2M compliance) */
constexpr bool STARLINK_TERMINAL_MANDATORY = false;

} // namespace wpp

#endif // STARLINK_TERMINAL_INFO_H
