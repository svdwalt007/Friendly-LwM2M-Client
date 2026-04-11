/*
 * LanConfiguration (Object 10535)
 * Walt Technologies - OpenWRT One Router
 *
 * LAN Configuration and DHCP server management for OpenWRT
 */

#ifndef WPP_W_10535_LAN_CONFIGURATION_H
#define WPP_W_10535_LAN_CONFIGURATION_H

#include "LanConfigurationConfig.h"
#include "LanConfigurationInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class LanConfiguration : public Instance {
public:
    enum ID: ID_T {
        LAN_IP_ADDRESS_0 = 0,        // LAN IP Address (RW, String)
        SUBNET_MASK_1 = 1,           // Subnet Mask (RW, String)
        DHCP_ENABLED_2 = 2,          // DHCP Enabled (RW, Boolean)
        DHCP_POOL_START_3 = 3,       // DHCP Pool Start (RW, String)
        DHCP_POOL_END_4 = 4,         // DHCP Pool End (RW, String)
        DHCP_LEASE_TIME_5 = 5,       // DHCP Lease Time (RW, Integer, seconds)
        DNS_SERVER_PRIMARY_6 = 6,    // DNS Server Primary (RW, String)
        DNS_SERVER_SECONDARY_7 = 7,  // DNS Server Secondary (RW, String)
        DOMAIN_NAME_8 = 8,           // Domain Name (RW, String)
        STATIC_LEASES_9 = 9,         // Static Leases (RW, Multiple, String - MAC:IP format)
        ACTIVE_LEASES_10 = 10,       // Active Leases (R, Multiple, String - MAC:IP:hostname:expiry)
        APPLY_CONFIGURATION_11 = 11, // Apply Configuration (E, Execute)
        CONFIGURATION_STATUS_12 = 12 // Configuration Status (R, Integer - 0=pending, 1=applied, 2=error)
    };

    // Configuration Status Enumeration
    enum ConfigStatus: uint8_t {
        STATUS_PENDING = 0,   // Configuration pending application
        STATUS_APPLIED = 1,   // Configuration applied successfully
        STATUS_ERROR = 2,     // Configuration application failed
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    LanConfiguration(Object& object, INST_T instId);
    ~LanConfiguration() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handler */
    static bool applyConfiguration(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool loadFromUCI();
    bool saveToUCI();
    bool applyDhcpConfig();
    bool loadActiveLeases();
    bool isValidIPv4(const std::string& ip);
    bool isValidMAC(const std::string& mac);
    std::string calculateNetworkAddress();
    int cidrFromSubnetMask(const std::string& mask);
};

} // namespace wpp

#endif // WPP_W_10535_LAN_CONFIGURATION_H
