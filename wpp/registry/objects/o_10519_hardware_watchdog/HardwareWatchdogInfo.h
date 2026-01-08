/*
 * Generated on: 2025-11-01 08:40:02
 * Created by: Sinai RnD
 */

#ifndef WPP_HARDWARE_WATCHDOG_INFO_H
#define WPP_HARDWARE_WATCHDOG_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_O_10519_HARDWARE_WATCHDOG

namespace wpp {

inline const ObjectInfo HARDWARE_WATCHDOG_OBJ_INFO = {
	/* Name */
	"Hardware Watchdog",
	/* Object ID */
	OBJ_ID::HARDWARE_WATCHDOG,
	/* URN */
	"urn:oma:lwm2m:x:10519",
	/* Object version */
	{1,0},
	/* Lwm2m version */
	{1,0},
	/* Is single */
	IS_SINGLE::SINGLE,
	/* Is Mandatory */
	IS_MANDATORY::OPTIONAL,
	/* Object supported operations */
	/* --------------- Info block 0 start --------------- */
	ItemOp(
		   ItemOp::READ|
		   ItemOp::WRITE|
		   ItemOp::DISCOVER|
		   ItemOp::EXECUTE),
	/* --------------- Info block 0 end --------------- */
};

} /* namespace wpp */

#endif /* OBJ_O_10519_HARDWARE_WATCHDOG */
#endif // WPP_HARDWARE_WATCHDOG_INFO_H
