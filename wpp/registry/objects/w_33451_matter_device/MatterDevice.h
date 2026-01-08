/*
 * MatterDevice (Object 33451)
 * Matter Device Instance for LwM2M
 *
 * Represents a commissioned Matter device
 */

#ifndef WPP_W_33451_MATTER_DEVICE_H
#define WPP_W_33451_MATTER_DEVICE_H

#include "MatterDeviceConfig.h"
#include "MatterDeviceInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class MatterDevice : public Instance {
public:
    enum ID: ID_T {
        NODE_ID_0 = 0,              // Node ID (R, String - hex format)
        VENDOR_ID_1 = 1,            // Vendor ID (R, Integer)
        PRODUCT_ID_2 = 2,           // Product ID (R, Integer)
        DEVICE_TYPE_3 = 3,          // Device Type (R, Integer)
        DEVICE_TYPE_NAME_4 = 4,     // Device Type Name (R, String)
        ENDPOINTS_5 = 5,            // Endpoints (R, Multiple, Integer)
        REACHABLE_6 = 6,            // Reachable (R, Boolean)
        FIRMWARE_VERSION_7 = 7,     // Firmware Version (R, String)
        SERIAL_NUMBER_8 = 8,        // Serial Number (R, String)
        PRODUCT_NAME_9 = 9,         // Product Name (R, String)
        LAST_SEEN_10 = 10,          // Last Seen (R, Time)
        IP_ADDRESS_11 = 11,         // IP Address (R, String)
        RSSI_12 = 12,               // RSSI (R, Integer)
        INTERACT_13 = 13,           // Interact with Device (E, Execute)
        READ_ATTRIBUTE_14 = 14,     // Read Attribute (E, Execute)
        WRITE_ATTRIBUTE_15 = 15,    // Write Attribute (E, Execute)
        SEND_COMMAND_16 = 16        // Send Command (E, Execute)
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    MatterDevice(Object& object, INST_T instId);
    ~MatterDevice() override;

    /* Device management */
    void setNodeId(uint64_t nodeId);
    uint64_t getNodeId() const;
    void updateDeviceInfo();

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool interact(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool readAttribute(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool writeAttribute(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool sendCommand(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool queryDeviceInformation();
    std::string getDeviceTypeName(uint16_t deviceType);
    bool parseInteractionArgs(const std::string& args, uint16_t& endpoint,
                             uint32_t& cluster, uint32_t& attributeOrCommand);

    uint64_t m_nodeId;
};

} // namespace wpp

#endif // WPP_W_33451_MATTER_DEVICE_H
