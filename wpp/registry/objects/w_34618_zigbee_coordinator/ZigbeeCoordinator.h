/*
 * ZigbeeCoordinator (Object 34618)
 * Walt Technologies - Zigbee Integration
 *
 * Comprehensive Zigbee coordinator management with support for:
 * - EZSP (Silicon Labs EmberZNet)
 * - Z-Stack (Texas Instruments)
 * - Network formation and management
 * - Device discovery and control
 */

#ifndef WPP_W_34618_ZIGBEE_COORDINATOR_H
#define WPP_W_34618_ZIGBEE_COORDINATOR_H

#include "ZigbeeCoordinatorConfig.h"
#include "ZigbeeCoordinatorInfo.h"
#include "Instance.h"
#include "InstSubject.h"

// Forward declarations for Zigbee coordinator
namespace zigbee {
    class ZigbeeCoordinator;
}

namespace wpp {

class WppClient;
class Object;

class ZigbeeCoordinator : public Instance {
public:
    enum ID: ID_T {
        NETWORK_STATE_0 = 0,           // Network State (R, Integer)
        PAN_ID_1 = 1,                  // PAN ID (RW, Integer)
        EXTENDED_PAN_ID_2 = 2,         // Extended PAN ID (RW, String)
        CHANNEL_3 = 3,                 // Channel (RW, Integer)
        NETWORK_KEY_4 = 4,             // Network Key (W, Opaque)
        PERMIT_JOIN_5 = 5,             // Permit Join Status (R, Boolean)
        DEVICE_COUNT_6 = 6,            // Device Count (R, Integer)
        COORDINATOR_IEEE_7 = 7,        // Coordinator IEEE Address (R, String)
        COORDINATOR_TYPE_8 = 8,        // Coordinator Type (R, Integer)
        FIRMWARE_VERSION_9 = 9,        // Firmware Version (R, String)
        TX_POWER_10 = 10,              // TX Power (RW, Integer)
        SECURITY_LEVEL_11 = 11,        // Security Level (RW, Integer)
        SERIAL_PORT_12 = 12,           // Serial Port (RW, String)
        BAUD_RATE_13 = 13,             // Baud Rate (RW, Integer)
        ROUTE_TABLE_SIZE_14 = 14,      // Route Table Size (R, Integer)
        NEIGHBOR_TABLE_SIZE_15 = 15,   // Neighbor Table Size (R, Integer)
        FORM_NETWORK_16 = 16,          // Form Network (E, Execute)
        LEAVE_NETWORK_17 = 17,         // Leave Network (E, Execute)
        PERMIT_JOIN_CMD_18 = 18,       // Permit Join Command (E, Execute)
        START_TOUCHLINK_19 = 19,       // Start Touchlink (E, Execute)
        BACKUP_NETWORK_20 = 20         // Backup Network (E, Execute)
    };

    // Network State Enumeration
    enum NetworkState: uint8_t {
        STATE_OFFLINE = 0,
        STATE_FORMING = 1,
        STATE_JOINED = 2,
        STATE_READY = 3,
        STATE_LEAVING = 4,
        STATE_ERROR = 5
    };

    // Coordinator Type Enumeration
    enum CoordinatorType: uint8_t {
        TYPE_UNKNOWN = 0,
        TYPE_EZSP = 1,       // Silicon Labs EmberZNet
        TYPE_Z_STACK = 2,    // Texas Instruments Z-Stack
        TYPE_CONBEE = 3,     // Dresden Elektronik ConBee
        TYPE_ZIGATE = 4      // ZiGate
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = 0);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    ZigbeeCoordinator(Object& object, INST_T instId);
    ~ZigbeeCoordinator() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool formNetwork(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool leaveNetwork(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool permitJoin(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool startTouchlink(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool backupNetwork(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool initializeCoordinator();
    void shutdownCoordinator();
    bool applyNetworkConfig();
    void updateNetworkState();
    void updateDeviceCount();
    void updateRoutingTables();
    bool saveNetworkConfig();
    bool loadNetworkConfig();

    /* State refresh */
    void refreshState();
    void startPeriodicRefresh();
    void stopPeriodicRefresh();

    /* Zigbee coordinator instance */
    std::shared_ptr<zigbee::ZigbeeCoordinator> coordinator_;

    /* Refresh thread */
    std::thread refreshThread_;
    std::atomic<bool> refreshRunning_;
};

} // namespace wpp

#endif // WPP_W_34618_ZIGBEE_COORDINATOR_H
