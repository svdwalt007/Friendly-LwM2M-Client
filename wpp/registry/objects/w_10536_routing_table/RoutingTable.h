/*
 * RoutingTable (Object 10536)
 * Walt Technologies - OpenWRT One Router
 *
 * Routing Table management for OpenWRT with support for IPv4/IPv6,
 * policy-based routing, and multi-table routing
 */

#ifndef WPP_W_10536_ROUTING_TABLE_H
#define WPP_W_10536_ROUTING_TABLE_H

#include "RoutingTableConfig.h"
#include "RoutingTableInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class RoutingTable : public Instance {
public:
    enum ID: ID_T {
        DESTINATION_0 = 0,       // Destination Network (RW, String) - IP or CIDR
        NETMASK_1 = 1,           // Netmask/Prefix (RW, String) - netmask or prefix length
        GATEWAY_2 = 2,           // Gateway (RW, String) - next hop IP
        INTERFACE_3 = 3,         // Interface (RW, String) - network interface name
        METRIC_4 = 4,            // Metric (RW, Integer) - route priority
        PROTOCOL_5 = 5,          // Protocol (R, Integer) - route origin
        STATE_6 = 6,             // State (R, Integer) - route state
        TABLE_7 = 7,             // Table (RW, Integer) - routing table ID
        SCOPE_8 = 8,             // Scope (RW, Integer) - route scope
        MTU_9 = 9,               // MTU (RW, Integer) - maximum transmission unit
        ADD_ROUTE_10 = 10,       // Add Route (E, Execute)
        DELETE_ROUTE_11 = 11,    // Delete Route (E, Execute)
        FLUSH_ROUTES_12 = 12,    // Flush Routes (E, Execute)
        RELOAD_ROUTES_13 = 13    // Reload Routes (E, Execute)
    };

    // Protocol Enumeration (route origin)
    enum Protocol: uint8_t {
        PROTO_UNSPEC = 0,    // Unspecified
        PROTO_REDIRECT = 1,  // ICMP redirect
        PROTO_KERNEL = 2,    // Kernel route
        PROTO_BOOT = 3,      // Boot/initial route
        PROTO_STATIC = 4,    // Static route
        PROTO_DHCP = 5       // DHCP-provided route
    };

    // State Enumeration
    enum State: uint8_t {
        STATE_DOWN = 0,         // Route is down
        STATE_UP = 1,           // Route is up and active
        STATE_UNREACHABLE = 2   // Route is unreachable
    };

    // Scope Enumeration
    enum Scope: uint8_t {
        SCOPE_GLOBAL = 0,    // Global route (0)
        SCOPE_LINK = 253,    // Link-local route
        SCOPE_HOST = 254     // Host route
    };

    // Routing Table IDs
    enum Table: uint8_t {
        TABLE_MAIN = 254,    // Main routing table
        TABLE_LOCAL = 255    // Local routing table
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    RoutingTable(Object& object, INST_T instId);
    ~RoutingTable() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool addRoute(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool deleteRoute(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool flushRoutes(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool reloadRoutes(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool loadFromKernel();
    bool loadFromUCI();
    bool saveToUCI();
    bool applyRoute();
    bool removeRoute();
    bool isValidIPv4(const std::string& ip);
    bool isValidIPv6(const std::string& ip);
    bool isValidCIDR(const std::string& cidr);
    bool isValidInterface(const std::string& iface);
    std::string cidrToNetmask(int prefix);
    int netmaskToCIDR(const std::string& mask);
    State checkRouteState();
    std::string executeCommand(const std::string& cmd);
};

} // namespace wpp

#endif // WPP_W_10536_ROUTING_TABLE_H
