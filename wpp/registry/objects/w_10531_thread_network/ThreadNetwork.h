/*
 * ThreadNetwork (Object 10531)
 * Thread Network Configuration and Status for Matter
 *
 * Provides Thread Border Router network information
 */

#ifndef WPP_W_10531_THREAD_NETWORK_H
#define WPP_W_10531_THREAD_NETWORK_H

#include "ThreadNetworkConfig.h"
#include "ThreadNetworkInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class ThreadNetwork : public Instance {
public:
    enum ID: ID_T {
        NETWORK_NAME_0 = 0,           // Network Name (RW, String)
        PAN_ID_1 = 1,                 // PAN ID (RW, Integer)
        CHANNEL_2 = 2,                // Channel (RW, Integer, 11-26)
        EXTENDED_PAN_ID_3 = 3,        // Extended PAN ID (RW, String, hex)
        NETWORK_KEY_4 = 4,            // Network Key (RW, String, hex)
        ACTIVE_TIMESTAMP_5 = 5,       // Active Timestamp (R, Time)
        BORDER_ROUTER_STATE_6 = 6,    // Border Router State (R, Integer)
        OPERATIONAL_DATASET_7 = 7,    // Operational Dataset (R, Opaque)
        DEVICES_COUNT_8 = 8,          // Connected Devices Count (R, Integer)
        CREATE_NETWORK_9 = 9,         // Create Network (E, Execute)
        START_BORDER_ROUTER_10 = 10,  // Start Border Router (E, Execute)
        STOP_BORDER_ROUTER_11 = 11    // Stop Border Router (E, Execute)
    };

    // Border Router State Enumeration
    enum BorderRouterState: uint8_t {
        STATE_DISABLED = 0,      // Border Router disabled
        STATE_STOPPED = 1,       // Border Router stopped
        STATE_STARTING = 2,      // Border Router starting
        STATE_RUNNING = 3,       // Border Router running
        STATE_ERROR = 4          // Border Router error state
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    ThreadNetwork(Object& object, INST_T instId);
    ~ThreadNetwork() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool createNetwork(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool startBorderRouter(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool stopBorderRouter(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool validateNetworkParameters();
    bool applyNetworkConfiguration();
    void updateOperationalDataset();
    void updateBorderRouterState(BorderRouterState state);
    std::string generateRandomKey();
    std::string generateRandomExtendedPanId();
};

} // namespace wpp

#endif // WPP_W_10531_THREAD_NETWORK_H
