/*
 * Location
 * OMA LWM2M Object ID: 6
 *
 * Description: This LwM2M Object provides a range of location telemetry
 *              related information which can be queried by the LwM2M Server.
 */

#ifndef WPP_O_6_LOCATION_H
#define WPP_O_6_LOCATION_H

#include "LocationConfig.h"
#include "LocationInfo.h"
#include "Instance.h"
#include "InstSubject.h"

/* --------------- Code_h block 0 start --------------- */
#include "WppTaskQueue.h"
/* --------------- Code_h block 0 end --------------- */

namespace wpp {

class WppClient;
class Object;

/* --------------- Code_h block 1 start --------------- */
/* --------------- Code_h block 1 end --------------- */

class Location : public Instance {
public:
	enum ID: ID_T {
		LATITUDE_0 = 0,          // Mandatory - Float - Degrees
		LONGITUDE_1 = 1,         // Mandatory - Float - Degrees
		#if RES_6_2
		ALTITUDE_2 = 2,          // Optional - Float - Meters
		#endif
		#if RES_6_3
		RADIUS_3 = 3,            // Optional - Float - Meters (uncertainty)
		#endif
		#if RES_6_4
		VELOCITY_4 = 4,          // Optional - Opaque - 3GPP velocity
		#endif
		TIMESTAMP_5 = 5,         // Mandatory - Time - Unix timestamp
		#if RES_6_6
		SPEED_6 = 6,             // Optional - Float - m/s
		#endif
	};

	/* --------------- Code_h block 2 start --------------- */
	/* --------------- Code_h block 2 end --------------- */

public:
	Location(lwm2m_context_t &context, const OBJ_LINK_T &id);
	~Location();

	/* --------------- Helpful methods --------------- */
	/**
	 * @brief Gets the Object reference.
	 * @param ctx - WppClient context.
	 * @return A reference to the Object.
	 */
	static Object & object(WppClient &ctx);

	/**
	 * @brief Gets an instance of the object.
	 * @param ctx - WppClient context.
	 * @param instId The instance ID. If not provided, the first available instance is returned.
	 * @return A pointer to the Instance object or NULL.
	 */
	static Location * instance(WppClient &ctx, ID_T instId = ID_T_MAX_VAL);

	/**
	 * @brief Creates an instance of the object.
	 * @param ctx - WppClient context
	 * @param instanceID The instance ID.
	 * @return A pointer to the created Instance object or NULL.
	 */
	static Location * createInst(WppClient &ctx, ID_T instId = ID_T_MAX_VAL);

	/**
	 * @brief Removes an instance of the object.
	 * @param ctx - WppClient context.
	 * @param instId The instance ID.
	 * @return True if the instance was successfully removed, false otherwise.
	 */
	static bool removeInst(WppClient &ctx, ID_T instId);

	/* --------------- Code_h block 3 start --------------- */
	/* --------------- Code_h block 3 end --------------- */

protected:
	/* --------------- Instance implementation part --------------- */
	/*
	 * Handles information about resource operation that made server
	 */
	void serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) override;
	/*
	 * Handles information about resource operation that made user
	 */
	void userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) override;

	/* --------------- Code_h block 4 start --------------- */
	/* --------------- Code_h block 4 end --------------- */

private:
	/* --------------- Class private methods --------------- */
	/*
	 * Creates resources without initializing.
	 */
	void resourcesCreate();
	/*
	 * Initialize resources with default values
	 * SINGLE resource always must have at least one instance.
	 */
	void resourcesInit();

	/* --------------- Code_h block 5 start --------------- */
	/**
	 * @brief Update location data from available sources
	 * @return true if successful
	 */
	bool updateLocation();
	/* --------------- Code_h block 5 end --------------- */

private:
	/* --------------- Class private properties --------------- */
	/* --------------- Code_h block 6 start --------------- */
	WppTaskQueue::task_id_t _locationUpdateTaskId;
	/* --------------- Code_h block 6 end --------------- */
};

} /* namespace wpp */

#endif /* WPP_O_6_LOCATION_H */
