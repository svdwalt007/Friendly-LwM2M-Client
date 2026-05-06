/*
 * ThreadNetwork Information
 * Object 10531 - Thread Network
 */

#ifndef WPP_W_10531_THREAD_NETWORK_INFO_H
#define WPP_W_10531_THREAD_NETWORK_INFO_H

// Object version information
#define THREAD_NETWORK_OBJECT_VERSION "1.0"

// Default values
#define DEFAULT_THREAD_CHANNEL 15
#define DEFAULT_THREAD_PAN_ID 0x1234
#define DEFAULT_THREAD_NETWORK_NAME "matter-thread"

#include "ObjectInfo.h"

#ifdef OBJ_W_10531_THREAD_NETWORK

namespace wpp {

inline const ObjectInfo THREAD_NETWORK_OBJ_INFO = {
    /* Name */
    "Thread Network",
    /* Object ID */
    static_cast<OBJ_ID>(10531),
    /* URN */
    "urn:waltech:lwm2m:10531",
    /* Object version */
    {1, 0},
    /* LwM2M version */
    {1, 1},
    /* Is single */
    IS_SINGLE::SINGLE,
    /* Is mandatory */
    IS_MANDATORY::OPTIONAL,
    /* Object supported operations */
    ItemOp(ItemOp::READ | ItemOp::WRITE | ItemOp::DISCOVER)
};

} /* namespace wpp */

#endif // OBJ_W_10531_THREAD_NETWORK
#endif // WPP_W_10531_THREAD_NETWORK_INFO_H
