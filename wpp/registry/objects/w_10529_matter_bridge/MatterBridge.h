/*
 * MatterBridge (Object 10529)
 * Matter Bridge Controller for LwM2M
 *
 * Provides Matter controller functionality via LwM2M interface
 */

#ifndef WPP_W_10529_MATTER_BRIDGE_H
#define WPP_W_10529_MATTER_BRIDGE_H

#include "MatterBridgeConfig.h"
#include "MatterBridgeInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class MatterBridge : public Instance {
public:
    enum ID: ID_T {
        CONTROLLER_STATUS_0 = 0,           // Controller Status (R, String)
        COMMISSIONED_DEVICES_COUNT_1 = 1,  // Commissioned Devices Count (R, Integer)
        THREAD_NETWORK_SSID_2 = 2,        // Thread Network Name (RW, String)
        THREAD_ENABLED_3 = 3,              // Thread Border Router Enabled (RW, Boolean)
        FABRIC_ID_4 = 4,                   // Fabric ID (R, String)
        VENDOR_ID_5 = 5,                   // Controller Vendor ID (RW, Integer)
        PRODUCT_ID_6 = 6,                  // Controller Product ID (RW, Integer)
        COMMISSION_DEVICE_7 = 7,           // Commission Device (E, Execute)
        REMOVE_DEVICE_8 = 8,               // Remove Device (E, Execute)
        DISCOVER_DEVICES_9 = 9,            // Discover Devices (E, Execute)
        ENABLE_THREAD_BR_10 = 10           // Enable Thread Border Router (E, Execute)
    };

    /* Static object methods */
    static Object& object(WppClient& ctx);
    static MatterBridge* createInst(WppClient& ctx, ID_T instId = ID_T_MAX_VAL);
    static MatterBridge* instance(WppClient& ctx, ID_T instId = ID_T_MAX_VAL);
    static bool removeInst(WppClient& ctx, ID_T instId);

    /* Instance lifecycle */
    MatterBridge(lwm2m_context_t& context, const OBJ_LINK_T& id);
    ~MatterBridge();

protected:
    void serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) override;
    void userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) override;

private:
    /* Private methods */
    void resourcesCreate();
    void resourcesInit();

    /* Execute handlers */
    static bool commissionDevice(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool removeDevice(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool discoverDevices(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool enableThreadBR(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool initializeMatterController();
    void shutdownMatterController();
    void updateControllerStatus();
    void updateDeviceCount();
    std::string parseCommissioningPayload(const std::string& payload);
    bool setupThreadBorderRouter();
};

} // namespace wpp

#endif // WPP_W_10529_MATTER_BRIDGE_H
