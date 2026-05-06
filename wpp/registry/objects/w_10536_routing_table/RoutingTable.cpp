/*
 * RoutingTable Implementation (Object 10536)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_10536_routing_table/RoutingTable.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"

#ifdef OBJ_W_10536_ROUTING_TABLE

#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#include <regex>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TAG "RoutingTable"

using namespace wpp;

/* Static object methods */
Object& RoutingTable::object(WppClient& ctx) {
    return ctx.registry().routingTable();
}

RoutingTable* RoutingTable::createInst(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().routingTable().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<RoutingTable*>(inst);
}

RoutingTable* RoutingTable::instance(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().routingTable().instance(instId);
    if (!inst) return NULL;
    return static_cast<RoutingTable*>(inst);
}

bool RoutingTable::removeInst(WppClient& ctx, ID_T instId) {
    return ctx.registry().routingTable().remove(instId);
}

/* Instance lifecycle */
RoutingTable::RoutingTable(lwm2m_context_t& context, const OBJ_LINK_T& id)
    : Instance(context, id) {

    resourcesCreate();
    resourcesInit();

    WPP_LOGD(TAG, "RoutingTable instance created");
}

RoutingTable::~RoutingTable() {
    WPP_LOGD(TAG, "RoutingTable instance destroyed");
}

void RoutingTable::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
    operationNotify(*this, resLink, type);
}

void RoutingTable::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
    if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);
}

/* Create resource metadata */
void RoutingTable::resourcesCreate() {
    // Set execute handlers
    resource(ADD_ROUTE_10)->set<EXECUTE_T>(addRoute);
    resource(DELETE_ROUTE_11)->set<EXECUTE_T>(deleteRoute);
    resource(FLUSH_ROUTES_12)->set<EXECUTE_T>(flushRoutes);
    resource(RELOAD_ROUTES_13)->set<EXECUTE_T>(reloadRoutes);

    // Set data validators
    resource(DESTINATION_0)->setDataVerifier((VERIFY_STRING_T)[](const STRING_T& dest) {
        // Validate IP or CIDR notation
        if (dest.find('/') != std::string::npos) {
            // Validate CIDR
            size_t pos = dest.find('/');
            std::string ip = dest.substr(0, pos);
            std::string prefixStr = dest.substr(pos + 1);

            struct sockaddr_in sa4;
            struct sockaddr_in6 sa6;
            bool isValidIP = (inet_pton(AF_INET, ip.c_str(), &(sa4.sin_addr)) == 1) ||
                            (inet_pton(AF_INET6, ip.c_str(), &(sa6.sin6_addr)) == 1);

            if (!isValidIP) {
                WPP_LOGE(TAG, "Invalid IP in CIDR notation: %s", dest.c_str());
                return false;
            }

            try {
                int prefix = std::stoi(prefixStr);
                if (ip.find(':') != std::string::npos) {
                    if (prefix < 0 || prefix > 128) {
                        WPP_LOGE(TAG, "Invalid IPv6 prefix: %d", prefix);
                        return false;
                    }
                } else {
                    if (prefix < 0 || prefix > 32) {
                        WPP_LOGE(TAG, "Invalid IPv4 prefix: %d", prefix);
                        return false;
                    }
                }
            } catch (...) {
                WPP_LOGE(TAG, "Invalid CIDR notation: %s", dest.c_str());
                return false;
            }
        } else {
            struct sockaddr_in sa4;
            struct sockaddr_in6 sa6;
            if (inet_pton(AF_INET, dest.c_str(), &(sa4.sin_addr)) != 1 &&
                inet_pton(AF_INET6, dest.c_str(), &(sa6.sin6_addr)) != 1) {
                WPP_LOGE(TAG, "Invalid destination IP: %s", dest.c_str());
                return false;
            }
        }
        return true;
    });

    resource(NETMASK_1)->setDataVerifier((VERIFY_STRING_T)[](const STRING_T& mask) {
        if (!mask.empty()) {
            struct sockaddr_in sa;
            if (inet_pton(AF_INET, mask.c_str(), &(sa.sin_addr)) != 1) {
                WPP_LOGE(TAG, "Invalid netmask: %s", mask.c_str());
                return false;
            }
        }
        return true;
    });

    resource(GATEWAY_2)->setDataVerifier((VERIFY_STRING_T)[](const STRING_T& gw) {
        if (!gw.empty()) {
            struct sockaddr_in sa4;
            struct sockaddr_in6 sa6;
            if (inet_pton(AF_INET, gw.c_str(), &(sa4.sin_addr)) != 1 &&
                inet_pton(AF_INET6, gw.c_str(), &(sa6.sin6_addr)) != 1) {
                WPP_LOGE(TAG, "Invalid gateway IP: %s", gw.c_str());
                return false;
            }
        }
        return true;
    });

    resource(INTERFACE_3)->setDataVerifier((VERIFY_STRING_T)[](const STRING_T& iface) {
        if (iface.empty() || iface.length() > 16) {
            WPP_LOGE(TAG, "Invalid interface: %s", iface.c_str());
            return false;
        }
        std::regex ifaceRegex("^[a-zA-Z0-9._-]+$");
        if (!std::regex_match(iface, ifaceRegex)) {
            WPP_LOGE(TAG, "Invalid interface name: %s", iface.c_str());
            return false;
        }
        return true;
    });

    resource(METRIC_4)->setDataVerifier((VERIFY_INT_T)[](const INT_T& metric) {
        if (metric < 0 || metric > 65535) {
            WPP_LOGE(TAG, "Invalid metric: %lld (must be 0-65535)", metric);
            return false;
        }
        return true;
    });

    resource(MTU_9)->setDataVerifier((VERIFY_INT_T)[](const INT_T& mtu) {
        if (mtu < 68 || mtu > 65535) {
            WPP_LOGE(TAG, "Invalid MTU: %lld (must be 68-65535)", mtu);
            return false;
        }
        return true;
    });
}

/* Initialize resource values */
void RoutingTable::resourcesInit() {
    WPP_LOGD(TAG, "Initializing RoutingTable resource values");

    // Default routing values
    resource(DESTINATION_0)->set<STRING_T>("0.0.0.0/0");  // Default route
    resource(NETMASK_1)->set<STRING_T>("0.0.0.0");
    resource(GATEWAY_2)->set<STRING_T>("192.168.1.1");
    resource(INTERFACE_3)->set<STRING_T>("eth0");
    resource(METRIC_4)->set<INT_T>(100);
    resource(PROTOCOL_5)->set<INT_T>(PROTO_STATIC);
    resource(STATE_6)->set<INT_T>(STATE_DOWN);
    resource(TABLE_7)->set<INT_T>(TABLE_MAIN);
    resource(SCOPE_8)->set<INT_T>(SCOPE_GLOBAL);
    resource(MTU_9)->set<INT_T>(1500);

#ifdef OPENWRT_BUILD
    // Load existing routes from kernel
    loadFromKernel();
    // Load persistent routes from UCI
    loadFromUCI();
#endif
}

/* Execute handler: Add Route */
bool RoutingTable::addRoute(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    RoutingTable& rt = static_cast<RoutingTable&>(inst);

    WPP_LOGI(TAG, "Adding route for instance %d", rt.getInstanceID());

#ifdef OPENWRT_BUILD
    if (!rt.applyRoute()) {
        WPP_LOGE(TAG, "Failed to add route");
        return false;
    }

    // Save to UCI for persistence
    if (!rt.saveToUCI()) {
        WPP_LOGW(TAG, "Route added but failed to save to UCI");
    }

    rt.resource(STATE_6)->set<INT_T>(STATE_UP);
    WPP_LOGI(TAG, "Route added successfully");
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot add route");
    return false;
#endif
}

/* Execute handler: Delete Route */
bool RoutingTable::deleteRoute(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    RoutingTable& rt = static_cast<RoutingTable&>(inst);

    WPP_LOGI(TAG, "Deleting route for instance %d", rt.getInstanceID());

#ifdef OPENWRT_BUILD
    if (!rt.removeRoute()) {
        WPP_LOGE(TAG, "Failed to delete route");
        return false;
    }

    rt.resource(STATE_6)->set<INT_T>(STATE_DOWN);
    WPP_LOGI(TAG, "Route deleted successfully");
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot delete route");
    return false;
#endif
}

/* Execute handler: Flush Routes */
bool RoutingTable::flushRoutes(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    RoutingTable& rt = static_cast<RoutingTable&>(inst);

    WPP_LOGI(TAG, "Flushing routes for instance %d", rt.getInstanceID());

#ifdef OPENWRT_BUILD
    INT_T tableId = rt.resource(TABLE_7)->get<INT_T>();

    std::stringstream cmd;
    cmd << "ip route flush table " << tableId << " 2>&1";

    std::string output = rt.executeCommand(cmd.str());

    if (output.find("error") != std::string::npos ||
        output.find("failed") != std::string::npos) {
        WPP_LOGE(TAG, "Failed to flush routes: %s", output.c_str());
        return false;
    }

    WPP_LOGI(TAG, "Routes flushed successfully from table %lld", tableId);
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot flush routes");
    return false;
#endif
}

/* Execute handler: Reload Routes */
bool RoutingTable::reloadRoutes(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    RoutingTable& rt = static_cast<RoutingTable&>(inst);

    WPP_LOGI(TAG, "Reloading routes for instance %d", rt.getInstanceID());

#ifdef OPENWRT_BUILD
    // Reload from kernel routing table
    if (!rt.loadFromKernel()) {
        WPP_LOGE(TAG, "Failed to reload routes from kernel");
        return false;
    }

    WPP_LOGI(TAG, "Routes reloaded successfully");
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot reload routes");
    return false;
#endif
}

/* Load routes from kernel routing table */
bool RoutingTable::loadFromKernel() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Loading routes from kernel");

    // Use 'ip route show' to get current routes
    std::string output = executeCommand("ip route show 2>/dev/null");

    if (output.empty()) {
        WPP_LOGW(TAG, "No routes found in kernel table");
        return true;
    }

    // Parse the output (example: "default via 192.168.1.1 dev eth0")
    std::istringstream iss(output);
    std::string line;

    if (std::getline(iss, line)) {
        // Parse first route only for this instance
        std::regex routeRegex(R"(^(\S+)\s+(?:via\s+(\S+))?\s*(?:dev\s+(\S+))?\s*(?:metric\s+(\d+))?)");
        std::smatch match;

        if (std::regex_search(line, match, routeRegex)) {
            std::string dest = match[1].str();
            std::string gateway = match[2].str();
            std::string iface = match[3].str();
            std::string metricStr = match[4].str();

            if (!dest.empty()) {
                resource(DESTINATION_0)->set<STRING_T>(dest);

                // Parse CIDR if present
                if (dest.find('/') != std::string::npos) {
                    size_t pos = dest.find('/');
                    int prefix = std::stoi(dest.substr(pos + 1));
                    resource(NETMASK_1)->set<STRING_T>(cidrToNetmask(prefix));
                }
            }

            if (!gateway.empty()) {
                resource(GATEWAY_2)->set<STRING_T>(gateway);
            }

            if (!iface.empty()) {
                resource(INTERFACE_3)->set<STRING_T>(iface);
            }

            if (!metricStr.empty()) {
                resource(METRIC_4)->set<INT_T>(std::stoll(metricStr));
            }

            resource(STATE_6)->set<INT_T>(STATE_UP);
        }
    }

    return true;
#else
    return false;
#endif
}

/* Load routes from UCI configuration */
bool RoutingTable::loadFromUCI() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Loading routes from UCI");

    // UCI routes are stored in /etc/config/network
    std::stringstream cmd;
    cmd << "uci show network | grep route 2>/dev/null";

    std::string output = executeCommand(cmd.str());

    if (!output.empty()) {
        WPP_LOGD(TAG, "Found UCI route configuration");
        // Parse UCI output and update resources
        // Format: network.@route[0].target='0.0.0.0/0'
    }

    return true;
#else
    return false;
#endif
}

/* Save route to UCI configuration */
bool RoutingTable::saveToUCI() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Saving route to UCI");

    std::string dest = resource(DESTINATION_0)->get<STRING_T>();
    std::string gateway = resource(GATEWAY_2)->get<STRING_T>();
    std::string iface = resource(INTERFACE_3)->get<STRING_T>();
    INT_T metric = resource(METRIC_4)->get<INT_T>();

    // Create a new UCI route section
    std::stringstream cmd;
    cmd << "uci add network route && ";
    cmd << "uci set network.@route[-1].target='" << dest << "' && ";
    cmd << "uci set network.@route[-1].gateway='" << gateway << "' && ";
    cmd << "uci set network.@route[-1].interface='" << iface << "' && ";
    cmd << "uci set network.@route[-1].metric='" << metric << "' && ";
    cmd << "uci commit network 2>&1";

    std::string output = executeCommand(cmd.str());

    if (output.find("error") != std::string::npos) {
        WPP_LOGE(TAG, "Failed to save route to UCI: %s", output.c_str());
        return false;
    }

    WPP_LOGI(TAG, "Route saved to UCI successfully");
    return true;
#else
    return false;
#endif
}

/* Apply route to kernel routing table */
bool RoutingTable::applyRoute() {
#ifdef OPENWRT_BUILD
    std::string dest = resource(DESTINATION_0)->get<STRING_T>();
    std::string gateway = resource(GATEWAY_2)->get<STRING_T>();
    std::string iface = resource(INTERFACE_3)->get<STRING_T>();
    INT_T metric = resource(METRIC_4)->get<INT_T>();
    INT_T table = resource(TABLE_7)->get<INT_T>();
    INT_T mtu = resource(MTU_9)->get<INT_T>();

    // Build ip route add command
    std::stringstream cmd;
    cmd << "ip route add " << dest;

    if (!gateway.empty() && gateway != "0.0.0.0") {
        cmd << " via " << gateway;
    }

    if (!iface.empty()) {
        cmd << " dev " << iface;
    }

    if (metric > 0) {
        cmd << " metric " << metric;
    }

    if (table != TABLE_MAIN) {
        cmd << " table " << table;
    }

    if (mtu > 0 && mtu != 1500) {
        cmd << " mtu " << mtu;
    }

    cmd << " 2>&1";

    WPP_LOGD(TAG, "Executing: %s", cmd.str().c_str());
    std::string output = executeCommand(cmd.str());

    if (output.find("File exists") != std::string::npos) {
        WPP_LOGW(TAG, "Route already exists");
        return true;
    }

    if (output.find("error") != std::string::npos ||
        output.find("failed") != std::string::npos) {
        WPP_LOGE(TAG, "Failed to add route: %s", output.c_str());
        return false;
    }

    return true;
#else
    return false;
#endif
}

/* Remove route from kernel routing table */
bool RoutingTable::removeRoute() {
#ifdef OPENWRT_BUILD
    std::string dest = resource(DESTINATION_0)->get<STRING_T>();
    INT_T table = resource(TABLE_7)->get<INT_T>();

    // Build ip route del command
    std::stringstream cmd;
    cmd << "ip route del " << dest;

    if (table != TABLE_MAIN) {
        cmd << " table " << table;
    }

    cmd << " 2>&1";

    WPP_LOGD(TAG, "Executing: %s", cmd.str().c_str());
    std::string output = executeCommand(cmd.str());

    if (output.find("No such process") != std::string::npos) {
        WPP_LOGW(TAG, "Route does not exist");
        return true;
    }

    if (output.find("error") != std::string::npos) {
        WPP_LOGE(TAG, "Failed to delete route: %s", output.c_str());
        return false;
    }

    return true;
#else
    return false;
#endif
}

/* Check route state */
RoutingTable::State RoutingTable::checkRouteState() {
#ifdef OPENWRT_BUILD
    std::string dest = resource(DESTINATION_0)->get<STRING_T>();
    INT_T table = resource(TABLE_7)->get<INT_T>();

    std::stringstream cmd;
    cmd << "ip route show " << dest << " table " << table << " 2>/dev/null";

    std::string output = executeCommand(cmd.str());

    if (output.empty()) {
        return STATE_DOWN;
    }

    if (output.find("unreachable") != std::string::npos) {
        return STATE_UNREACHABLE;
    }

    return STATE_UP;
#else
    return STATE_DOWN;
#endif
}

/* Validate IPv4 address */
bool RoutingTable::isValidIPv4(const std::string& ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) == 1;
}

/* Validate IPv6 address */
bool RoutingTable::isValidIPv6(const std::string& ip) {
    struct sockaddr_in6 sa;
    return inet_pton(AF_INET6, ip.c_str(), &(sa.sin6_addr)) == 1;
}

/* Validate CIDR notation */
bool RoutingTable::isValidCIDR(const std::string& cidr) {
    size_t pos = cidr.find('/');
    if (pos == std::string::npos) {
        return false;
    }

    std::string ip = cidr.substr(0, pos);
    std::string prefixStr = cidr.substr(pos + 1);

    // Validate IP part
    bool isIPv4 = isValidIPv4(ip);
    bool isIPv6 = isValidIPv6(ip);

    if (!isIPv4 && !isIPv6) {
        return false;
    }

    // Validate prefix length
    try {
        int prefix = std::stoi(prefixStr);
        if (isIPv4 && (prefix < 0 || prefix > 32)) {
            return false;
        }
        if (isIPv6 && (prefix < 0 || prefix > 128)) {
            return false;
        }
    } catch (...) {
        return false;
    }

    return true;
}

/* Validate network interface name */
bool RoutingTable::isValidInterface(const std::string& iface) {
    // Check if interface exists
    if (iface.empty() || iface.length() > 16) {
        return false;
    }

    // Interface name should contain only alphanumeric, dash, underscore, or dot
    std::regex ifaceRegex("^[a-zA-Z0-9._-]+$");
    return std::regex_match(iface, ifaceRegex);
}

/* Convert CIDR prefix to netmask */
std::string RoutingTable::cidrToNetmask(int prefix) {
    if (prefix < 0 || prefix > 32) {
        return "0.0.0.0";
    }

    uint32_t mask = (prefix == 0) ? 0 : (~0U << (32 - prefix));

    struct in_addr addr;
    addr.s_addr = htonl(mask);

    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);

    return std::string(buffer);
}

/* Convert netmask to CIDR prefix */
int RoutingTable::netmaskToCIDR(const std::string& mask) {
    struct in_addr addr;
    if (inet_pton(AF_INET, mask.c_str(), &addr) != 1) {
        return 0;
    }

    uint32_t maskVal = ntohl(addr.s_addr);
    int prefix = 0;

    while (maskVal) {
        prefix++;
        maskVal <<= 1;
    }

    return prefix;
}

/* Execute shell command and return output */
std::string RoutingTable::executeCommand(const std::string& cmd) {
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        WPP_LOGE(TAG, "Failed to execute command: %s", cmd.c_str());
        return result;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);

    // Remove trailing newline
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

#endif // OBJ_W_10536_ROUTING_TABLE
