/*
 * Location
 * OMA LWM2M Object ID: 6
 *
 * Implementation of Location object with OpenWRT integration
 */

#include "o_6_location/Location.h"

#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

/* --------------- Code_cpp block 0 start --------------- */
#ifdef OPENWRT_BUILD
#include "OpenWrtLocationInfo.h"
#endif
/* --------------- Code_cpp block 0 end --------------- */

#define TAG "Location"

namespace wpp {

Location::Location(lwm2m_context_t &context, const OBJ_LINK_T &id): Instance(context, id) {

	/* --------------- Code_cpp block 1 start --------------- */
	_locationUpdateTaskId = WPP_ERR_TASK_ID;
	/* --------------- Code_cpp block 1 end --------------- */

	resourcesCreate();
	resourcesInit();

	/* --------------- Code_cpp block 2 start --------------- */
	/* --------------- Code_cpp block 2 end --------------- */
}

Location::~Location() {
	/* --------------- Code_cpp block 3 start --------------- */
	WppTaskQueue::requestToRemoveTask(_locationUpdateTaskId);
	/* --------------- Code_cpp block 3 end --------------- */
}

Object & Location::object(WppClient &ctx) {
	return ctx.registry().location();
}

Location * Location::instance(WppClient &ctx, ID_T instId) {
	Instance *inst = ctx.registry().location().instance(instId);
	if (!inst) return NULL;
	return static_cast<Location*>(inst);
}

Location * Location::createInst(WppClient &ctx, ID_T instId) {
	Instance *inst = ctx.registry().location().createInstance(instId);
	if (!inst) return NULL;
	return static_cast<Location*>(inst);
}

bool Location::removeInst(WppClient &ctx, ID_T instId) {
	return ctx.registry().location().remove(instId);
}

void Location::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
	/* --------------- Code_cpp block 4 start --------------- */
	/* --------------- Code_cpp block 4 end --------------- */

	operationNotify(*this, resLink, type);

	/* --------------- Code_cpp block 5 start --------------- */
	/* --------------- Code_cpp block 5 end --------------- */
}

void Location::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
	if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);

	/* --------------- Code_cpp block 6 start --------------- */
	/* --------------- Code_cpp block 6 end --------------- */
}

void Location::resourcesCreate() {
	std::vector<Resource> resources = {
		{LATITUDE_0,             ItemOp(ItemOp::READ),              IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT },
		{LONGITUDE_1,            ItemOp(ItemOp::READ),              IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT },
		#if RES_6_2
		{ALTITUDE_2,             ItemOp(ItemOp::READ),              IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT },
		#endif
		#if RES_6_3
		{RADIUS_3,               ItemOp(ItemOp::READ),              IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT },
		#endif
		#if RES_6_4
		{VELOCITY_4,             ItemOp(ItemOp::READ),              IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::OPAQUE },
		#endif
		{TIMESTAMP_5,            ItemOp(ItemOp::READ),              IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::TIME },
		#if RES_6_6
		{SPEED_6,                ItemOp(ItemOp::READ),              IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT },
		#endif
	};
	setupResources(std::move(resources));
}

void Location::resourcesInit() {
	/* --------------- Code_cpp block 7 start --------------- */
	#ifdef OPENWRT_BUILD
	// Initialize with actual location data from OpenWRT
	double lat, lon, alt;
	if (OpenWrtLocationInfo::getAllLocation(lat, lon, alt)) {
		resource(LATITUDE_0)->set<FLOAT_T>(lat);
		resource(LONGITUDE_1)->set<FLOAT_T>(lon);
		#if RES_6_2
		resource(ALTITUDE_2)->set<FLOAT_T>(alt);
		#endif
		WPP_LOGI(TAG, "Location initialized from OpenWRT: lat=%f, lon=%f, alt=%f", lat, lon, alt);
	} else {
		// No location available, set defaults
		resource(LATITUDE_0)->set<FLOAT_T>(0.0);
		resource(LONGITUDE_1)->set<FLOAT_T>(0.0);
		#if RES_6_2
		resource(ALTITUDE_2)->set<FLOAT_T>(0.0);
		#endif
		WPP_LOGW(TAG, "No location source available, using defaults");
	}

	#if RES_6_3
	resource(RADIUS_3)->set<FLOAT_T>(OpenWrtLocationInfo::getRadius());
	#endif

	#if RES_6_4
	// Velocity is optional and complex (3GPP format), leave unimplemented for now
	OPAQUE_T emptyVelocity;
	resource(VELOCITY_4)->set<OPAQUE_T>(emptyVelocity);
	#endif

	resource(TIMESTAMP_5)->set<TIME_T>(OpenWrtLocationInfo::getTimestamp());

	#if RES_6_6
	resource(SPEED_6)->set<FLOAT_T>(0.0); // Will be calculated on updates
	#endif

	// Set up periodic location updates (every 60 seconds)
	_locationUpdateTaskId = WppTaskQueue::addTask(60, [this](WppClient &client, void *ctx) -> bool {
		updateLocation();
		return false; // Return false to keep the task running
	});

	#else
	// Non-OpenWRT build - use default values
	resource(LATITUDE_0)->set<FLOAT_T>(0.0);
	resource(LONGITUDE_1)->set<FLOAT_T>(0.0);
	#if RES_6_2
	resource(ALTITUDE_2)->set<FLOAT_T>(0.0);
	#endif
	#if RES_6_3
	resource(RADIUS_3)->set<FLOAT_T>(100.0);
	#endif
	#if RES_6_4
	OPAQUE_T emptyVelocity;
	resource(VELOCITY_4)->set<OPAQUE_T>(emptyVelocity);
	#endif
	resource(TIMESTAMP_5)->set<TIME_T>(std::time(nullptr));
	#if RES_6_6
	resource(SPEED_6)->set<FLOAT_T>(0.0);
	#endif
	#endif

	/* --------------- Code_cpp block 7 end --------------- */
}

/* --------------- Code_cpp block 8 start --------------- */
bool Location::updateLocation() {
	#ifdef OPENWRT_BUILD
	double lat, lon, alt;

	// Get current location
	if (OpenWrtLocationInfo::getAllLocation(lat, lon, alt)) {
		// Update location resources
		resource(LATITUDE_0)->set<FLOAT_T>(lat);
		notifyResChanged(LATITUDE_0);

		resource(LONGITUDE_1)->set<FLOAT_T>(lon);
		notifyResChanged(LONGITUDE_1);

		#if RES_6_2
		resource(ALTITUDE_2)->set<FLOAT_T>(alt);
		notifyResChanged(ALTITUDE_2);
		#endif

		#if RES_6_3
		resource(RADIUS_3)->set<FLOAT_T>(OpenWrtLocationInfo::getRadius());
		notifyResChanged(RADIUS_3);
		#endif

		// Update timestamp
		time_t now = OpenWrtLocationInfo::getTimestamp();
		resource(TIMESTAMP_5)->set<TIME_T>(now);
		notifyResChanged(TIMESTAMP_5);

		#if RES_6_6
		// Update speed (calculated from position changes)
		double speed = OpenWrtLocationInfo::getSpeed();
		resource(SPEED_6)->set<FLOAT_T>(speed);
		notifyResChanged(SPEED_6);
		#endif

		WPP_LOGD(TAG, "Location updated: lat=%f, lon=%f, alt=%f", lat, lon, alt);
		return true;
	} else {
		WPP_LOGW(TAG, "Failed to update location - no source available");
		return false;
	}
	#else
	// Non-OpenWRT build
	return false;
	#endif
}
/* --------------- Code_cpp block 8 end --------------- */

} /* namespace wpp */
