/*
 * Location Object Info
 * OMA LWM2M Object ID: 6
 */

#ifndef WPP_LOCATION_INFO_H
#define WPP_LOCATION_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_O_6_LOCATION

namespace wpp {

inline const ObjectInfo LOCATION_OBJ_INFO = {
	/* Name */
	"Location",
	/* Object ID */
	OBJ_ID::LOCATION,
	/* URN */
	"urn:oma:lwm2m:oma:6:1.0",
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
	ItemOp(ItemOp::READ|
		   ItemOp::DISCOVER),
	/* --------------- Info block 0 end --------------- */
};

} /* namespace wpp */

#endif /* OBJ_O_6_LOCATION */
#endif // WPP_LOCATION_INFO_H
