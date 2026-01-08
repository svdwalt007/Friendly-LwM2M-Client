/*
 * ZigbeeDevice (Object 33461)
 * Walt Technologies - Zigbee Integration
 *
 * Represents individual Zigbee devices in the network
 */

#ifndef WPP_W_33461_ZIGBEE_DEVICE_H
#define WPP_W_33461_ZIGBEE_DEVICE_H

#include "ZigbeeDeviceConfig.h"
#include "ZigbeeDeviceInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class ZigbeeDevice : public Instance {
public:
    enum ID: ID_T {
        IEEE_ADDRESS_0 = 0,          // IEEE Address (R, String)
        NETWORK_ADDRESS_1 = 1,       // Network Address (R, Integer)
        DEVICE_TYPE_2 = 2,           // Device Type (R, Integer)
        MANUFACTURER_3 = 3,          // Manufacturer (R, String)
        MODEL_4 = 4,                 // Model (R, String)
        FIRMWARE_VERSION_5 = 5,      // Firmware Version (R, String)
        POWER_SOURCE_6 = 6,          // Power Source (R, Integer)
        LQI_7 = 7,                   // LQI (R, Integer)
        RSSI_8 = 8,                  // RSSI (R, Integer)
        LAST_SEEN_9 = 9,             // Last Seen (R, Time)
        ENDPOINTS_10 = 10,           // Endpoints (R, Multiple, Integer)
        CLUSTERS_11 = 11,            // Clusters (R, String)
        INTERVIEWED_12 = 12,         // Interviewed (R, Boolean)
        ONLINE_13 = 13,              // Online (R, Boolean)
        PROFILE_ID_14 = 14,          // Profile ID (R, Integer)
        DEVICE_ID_15 = 15,           // Device ID (R, Integer)
        REMOVE_16 = 16,              // Remove (E, Execute)
        INTERVIEW_17 = 17,           // Interview (E, Execute)
        PING_18 = 18,                // Ping (E, Execute)
        READ_ATTRIBUTE_19 = 19,      // Read Attribute (E, Execute)
        WRITE_ATTRIBUTE_20 = 20      // Write Attribute (E, Execute)
    };

    // Device Type Enumeration
    enum DeviceType: uint8_t {
        TYPE_UNKNOWN = 0,
        TYPE_COORDINATOR = 1,
        TYPE_ROUTER = 2,
        TYPE_END_DEVICE = 3,
        TYPE_SLEEPY_END_DEVICE = 4
    };

    // Power Source Enumeration
    enum PowerSource: uint8_t {
        POWER_UNKNOWN = 0,
        POWER_MAINS = 1,
        POWER_BATTERY = 2,
        POWER_DC = 3
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    ZigbeeDevice(Object& object, INST_T instId);
    ~ZigbeeDevice() override;

    /* Set device IEEE address (used for linking to physical device) */
    void setIeeeAddress(uint64_t ieeeAddress);
    uint64_t getIeeeAddress() const;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool removeDevice(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool interviewDevice(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool pingDevice(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool readAttribute(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool writeAttribute(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    void updateFromZigbeeDevice();
    bool isDeviceOnline();

    /* Device IEEE address */
    uint64_t ieeeAddress_;
};

} // namespace wpp

#endif // WPP_W_33461_ZIGBEE_DEVICE_H
