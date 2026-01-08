/*
 * EthernetInterface (Object 10514)
 * Walt Technologies - OpenWRT One Router
 *
 * Ethernet port management and statistics
 */

#ifndef WPP_W_10514_ETHERNET_INTERFACE_H
#define WPP_W_10514_ETHERNET_INTERFACE_H

#include "EthernetInterfaceConfig.h"
#include "EthernetInterfaceInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class EthernetInterface : public Instance {
public:
    enum ID: ID_T {
        INTERFACE_NAME_0 = 0,
        PORT_TYPE_1 = 1,
        SPEED_2 = 2,
        DUPLEX_3 = 3,
        LINK_STATUS_4 = 4,
        MAC_ADDRESS_5 = 5,
        MTU_6 = 6,
        TOTAL_BYTES_SENT_7 = 7,
        TOTAL_BYTES_RECEIVED_8 = 8,
        TOTAL_PACKETS_SENT_9 = 9,
        TOTAL_PACKETS_RECEIVED_10 = 10,
        TRANSMIT_ERRORS_11 = 11,
        RECEIVE_ERRORS_12 = 12,
        INTERFACE_ENABLED_13 = 13,
    };

    // Port Type Enumeration
    enum PortType: uint8_t {
        PORT_LAN = 0,
        PORT_WAN = 1
    };

    // Duplex Mode Enumeration
    enum DuplexMode: uint8_t {
        HALF_DUPLEX = 0,
        FULL_DUPLEX = 1
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId = 0);
    static bool remove(WppClient& client, INST_T instId = 0);

    /* Instance lifecycle */
    EthernetInterface(Object& object, INST_T instId);
    ~EthernetInterface() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Helper methods */
    void updateInterfaceStatus();
    std::string getInterfaceName() const;

    /* Member variables */
    void* _updateTaskId;
};

} // namespace wpp

#endif // WPP_W_10514_ETHERNET_INTERFACE_H
