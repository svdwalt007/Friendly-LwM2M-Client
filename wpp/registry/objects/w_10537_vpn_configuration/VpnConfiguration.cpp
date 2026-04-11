/*
 * VpnConfiguration Implementation (Object 10537)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_10537_vpn_configuration/VpnConfiguration.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"
#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10537_VPN_CONFIGURATION

#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#include <regex>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define TAG "VpnConfiguration"

using namespace wpp;

/* Static object methods */
Object& VpnConfiguration::object(WppClient& client) {
    return client.registry().getObject(VPN_CONFIGURATION_OBJECT_ID);
}

Instance* VpnConfiguration::createInst(WppClient& client, INST_T instId) {
    return object(client).createInstance(instId);
}

Instance* VpnConfiguration::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool VpnConfiguration::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

/* Instance lifecycle */
VpnConfiguration::VpnConfiguration(Object& object, INST_T instId)
    : Instance(object, instId), connectionStartTime(0), isMonitoring(false) {
    WPP_LOGD(TAG, "VpnConfiguration instance %d created", instId);
}

VpnConfiguration::~VpnConfiguration() {
    WPP_LOGD(TAG, "VpnConfiguration instance %d destroyed", instId());
    stopMonitoring();
}

/* Initialize resources */
bool VpnConfiguration::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing VpnConfiguration resources for instance %d", instId());

    // Default VPN configuration values
    set<STRING_T>(VPN_NAME_0, "vpn_" + std::to_string(instId()));
    set<INT_T>(PROTOCOL_TYPE_1, PROTOCOL_OPENVPN);
    set<STRING_T>(SERVER_ADDRESS_2, "");
    set<INT_T>(SERVER_PORT_3, 1194);  // Default OpenVPN port
    set<STRING_T>(LOCAL_ADDRESS_4, "");
    set<STRING_T>(REMOTE_NETWORK_5, "");
    set<INT_T>(AUTHENTICATION_TYPE_6, AUTH_PASSWORD);
    set<STRING_T>(USERNAME_7, "");
    set<STRING_T>(PASSWORD_8, "");
    set<STRING_T>(CERTIFICATE_9, "");
    set<STRING_T>(PRIVATE_KEY_10, "");
    set<STRING_T>(PUBLIC_KEY_11, "");
    set<STRING_T>(PRE_SHARED_KEY_12, "");
    set<INT_T>(CONNECTION_STATE_13, STATE_DISCONNECTED);
    set<INT_T>(BYTES_SENT_14, 0);
    set<INT_T>(BYTES_RECEIVED_15, 0);
    set<INT_T>(UPTIME_16, 0);
    set<STRING_T>(LAST_ERROR_17, "");
    set<INT_T>(KEEPALIVE_INTERVAL_18, 10);  // 10 seconds
    set<INT_T>(RECONNECT_DELAY_19, 30);     // 30 seconds
    set<BOOL_T>(SPLIT_TUNNEL_20, false);
    set<BOOL_T>(KILL_SWITCH_21, false);

    // Set execute handlers
    setExecute(CONNECT_22, executeConnect);
    setExecute(DISCONNECT_23, executeDisconnect);
    setExecute(RESTART_24, executeRestart);
    setExecute(REGENERATE_KEYS_25, executeRegenerateKeys);

#ifdef OPENWRT_BUILD
    // Load existing configuration from UCI if available
    loadFromUCI();
    // Update initial connection status
    updateConnectionStatus();
#endif

    return true;
}

/* Validation */
bool VpnConfiguration::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case SERVER_ADDRESS_2: {
            const STRING_T& addr = *(const STRING_T*)data;
            if (addr.empty()) {
                WPP_LOGE(TAG, "Server address cannot be empty");
                return false;
            }
            // Validate IP or hostname
            if (!isValidIPAddress(addr)) {
                // Try to resolve as hostname
                struct addrinfo hints, *result;
                memset(&hints, 0, sizeof(hints));
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;

                if (getaddrinfo(addr.c_str(), NULL, &hints, &result) != 0) {
                    WPP_LOGE(TAG, "Invalid server address or hostname: %s", addr.c_str());
                    return false;
                }
                freeaddrinfo(result);
            }
            break;
        }
        case SERVER_PORT_3: {
            INT_T port = *(const INT_T*)data;
            if (!isValidPort(port)) {
                WPP_LOGE(TAG, "Invalid port number: %lld (must be 1-65535)", port);
                return false;
            }
            break;
        }
        case REMOTE_NETWORK_5: {
            const STRING_T& network = *(const STRING_T*)data;
            if (!network.empty() && !isValidCIDR(network)) {
                WPP_LOGE(TAG, "Invalid CIDR notation: %s", network.c_str());
                return false;
            }
            break;
        }
        case PROTOCOL_TYPE_1: {
            INT_T protocol = *(const INT_T*)data;
            if (protocol < PROTOCOL_OPENVPN || protocol > PROTOCOL_IPSEC) {
                WPP_LOGE(TAG, "Invalid protocol type: %lld", protocol);
                return false;
            }
            break;
        }
        case AUTHENTICATION_TYPE_6: {
            INT_T authType = *(const INT_T*)data;
            if (authType < AUTH_PASSWORD || authType > AUTH_PUBKEY) {
                WPP_LOGE(TAG, "Invalid authentication type: %lld", authType);
                return false;
            }
            break;
        }
        case CERTIFICATE_9: {
            const STRING_T& cert = *(const STRING_T*)data;
            if (!cert.empty() && !validateCertificate(cert)) {
                WPP_LOGE(TAG, "Invalid certificate format");
                return false;
            }
            break;
        }
        case PRIVATE_KEY_10: {
            const STRING_T& key = *(const STRING_T*)data;
            if (!key.empty() && !validatePrivateKey(key)) {
                WPP_LOGE(TAG, "Invalid private key format");
                return false;
            }
            break;
        }
        case KEEPALIVE_INTERVAL_18: {
            INT_T interval = *(const INT_T*)data;
            if (interval < 1 || interval > 300) {
                WPP_LOGE(TAG, "Invalid keepalive interval: %lld (must be 1-300 seconds)", interval);
                return false;
            }
            break;
        }
        case RECONNECT_DELAY_19: {
            INT_T delay = *(const INT_T*)data;
            if (delay < 5 || delay > 600) {
                WPP_LOGE(TAG, "Invalid reconnect delay: %lld (must be 5-600 seconds)", delay);
                return false;
            }
            break;
        }
    }
    return true;
}

/* Execute handler: Connect */
bool VpnConfiguration::executeConnect(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    VpnConfiguration& vpn = static_cast<VpnConfiguration&>(inst);
    WPP_LOGI(TAG, "Connecting VPN instance %d", vpn.instId());

    // Check if already connected
    INT_T currentState = vpn.get<INT_T>(CONNECTION_STATE_13);
    if (currentState == STATE_CONNECTED || currentState == STATE_CONNECTING) {
        WPP_LOGW(TAG, "VPN already connected or connecting");
        return true;
    }

    // Set state to connecting
    vpn.set<INT_T>(CONNECTION_STATE_13, STATE_CONNECTING);
    vpn.set<STRING_T>(LAST_ERROR_17, "");

#ifdef OPENWRT_BUILD
    // Save configuration to UCI
    if (!vpn.saveToUCI()) {
        vpn.set<STRING_T>(LAST_ERROR_17, "Failed to save configuration");
        vpn.set<INT_T>(CONNECTION_STATE_13, STATE_ERROR);
        return false;
    }

    // Connect based on protocol type
    INT_T protocol = vpn.get<INT_T>(PROTOCOL_TYPE_1);
    bool success = false;

    switch (protocol) {
        case PROTOCOL_OPENVPN:
            success = vpn.connectOpenVPN();
            break;
        case PROTOCOL_WIREGUARD:
            success = vpn.connectWireGuard();
            break;
        case PROTOCOL_IPSEC:
            success = vpn.connectIPsec();
            break;
        default:
            vpn.set<STRING_T>(LAST_ERROR_17, "Unknown protocol type");
            vpn.set<INT_T>(CONNECTION_STATE_13, STATE_ERROR);
            return false;
    }

    if (success) {
        vpn.connectionStartTime = time(nullptr);
        vpn.set<INT_T>(CONNECTION_STATE_13, STATE_CONNECTED);
        vpn.startMonitoring();

        // Setup additional features
        if (vpn.get<BOOL_T>(KILL_SWITCH_21)) {
            vpn.setupKillSwitch();
        }
        if (vpn.get<BOOL_T>(SPLIT_TUNNEL_20)) {
            vpn.setupSplitTunnel();
        }

        WPP_LOGI(TAG, "VPN connected successfully");
    } else {
        vpn.set<INT_T>(CONNECTION_STATE_13, STATE_ERROR);
        WPP_LOGE(TAG, "VPN connection failed");
    }

    return success;
#else
    vpn.set<STRING_T>(LAST_ERROR_17, "OpenWRT build not enabled");
    vpn.set<INT_T>(CONNECTION_STATE_13, STATE_ERROR);
    return false;
#endif
}

/* Execute handler: Disconnect */
bool VpnConfiguration::executeDisconnect(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    VpnConfiguration& vpn = static_cast<VpnConfiguration&>(inst);
    WPP_LOGI(TAG, "Disconnecting VPN instance %d", vpn.instId());

    // Check if already disconnected
    INT_T currentState = vpn.get<INT_T>(CONNECTION_STATE_13);
    if (currentState == STATE_DISCONNECTED) {
        WPP_LOGW(TAG, "VPN already disconnected");
        return true;
    }

    vpn.set<INT_T>(CONNECTION_STATE_13, STATE_DISCONNECTING);
    vpn.stopMonitoring();

#ifdef OPENWRT_BUILD
    // Remove kill switch first
    if (vpn.get<BOOL_T>(KILL_SWITCH_21)) {
        vpn.removeKillSwitch();
    }

    // Disconnect based on protocol type
    INT_T protocol = vpn.get<INT_T>(PROTOCOL_TYPE_1);
    bool success = false;

    switch (protocol) {
        case PROTOCOL_OPENVPN:
            success = vpn.disconnectOpenVPN();
            break;
        case PROTOCOL_WIREGUARD:
            success = vpn.disconnectWireGuard();
            break;
        case PROTOCOL_IPSEC:
            success = vpn.disconnectIPsec();
            break;
        default:
            success = false;
    }

    vpn.set<INT_T>(CONNECTION_STATE_13, STATE_DISCONNECTED);
    vpn.set<INT_T>(UPTIME_16, 0);
    vpn.connectionStartTime = 0;

    WPP_LOGI(TAG, "VPN disconnected");
    return success;
#else
    vpn.set<INT_T>(CONNECTION_STATE_13, STATE_DISCONNECTED);
    return false;
#endif
}

/* Execute handler: Restart */
bool VpnConfiguration::executeRestart(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    VpnConfiguration& vpn = static_cast<VpnConfiguration&>(inst);
    WPP_LOGI(TAG, "Restarting VPN instance %d", vpn.instId());

    // Disconnect first
    executeDisconnect(inst, resId, data);

    // Wait a moment
    sleep(2);

    // Reconnect
    return executeConnect(inst, resId, data);
}

/* Execute handler: Regenerate Keys */
bool VpnConfiguration::executeRegenerateKeys(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    VpnConfiguration& vpn = static_cast<VpnConfiguration&>(inst);
    WPP_LOGI(TAG, "Regenerating keys for VPN instance %d", vpn.instId());

    INT_T protocol = vpn.get<INT_T>(PROTOCOL_TYPE_1);

    if (protocol == PROTOCOL_WIREGUARD) {
#ifdef OPENWRT_BUILD
        if (vpn.generateWireGuardKeys()) {
            WPP_LOGI(TAG, "WireGuard keys regenerated successfully");
            vpn.saveToUCI();
            return true;
        }
#endif
        WPP_LOGE(TAG, "Failed to regenerate WireGuard keys");
        return false;
    } else {
        WPP_LOGW(TAG, "Key regeneration only supported for WireGuard");
        return false;
    }
}

/* Load configuration from UCI */
bool VpnConfiguration::loadFromUCI() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Loading VPN configuration from UCI");

    std::string vpnName = get<STRING_T>(VPN_NAME_0);
    std::stringstream cmd;
    FILE* pipe;
    char buffer[1024];

    // Read protocol type
    cmd.str("");
    cmd << "uci get vpn." << vpnName << ".protocol 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            std::string protocol(buffer);
            if (protocol == "openvpn") set<INT_T>(PROTOCOL_TYPE_1, PROTOCOL_OPENVPN);
            else if (protocol == "wireguard") set<INT_T>(PROTOCOL_TYPE_1, PROTOCOL_WIREGUARD);
            else if (protocol == "ipsec") set<INT_T>(PROTOCOL_TYPE_1, PROTOCOL_IPSEC);
        }
        pclose(pipe);
    }

    // Read server address
    cmd.str("");
    cmd << "uci get vpn." << vpnName << ".server 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            set<STRING_T>(SERVER_ADDRESS_2, buffer);
        }
        pclose(pipe);
    }

    // Read server port
    cmd.str("");
    cmd << "uci get vpn." << vpnName << ".port 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            set<INT_T>(SERVER_PORT_3, atoi(buffer));
        }
        pclose(pipe);
    }

    // Read username
    cmd.str("");
    cmd << "uci get vpn." << vpnName << ".username 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            set<STRING_T>(USERNAME_7, buffer);
        }
        pclose(pipe);
    }

    // Read remote network
    cmd.str("");
    cmd << "uci get vpn." << vpnName << ".remote_network 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            set<STRING_T>(REMOTE_NETWORK_5, buffer);
        }
        pclose(pipe);
    }

    // Read keepalive interval
    cmd.str("");
    cmd << "uci get vpn." << vpnName << ".keepalive 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            set<INT_T>(KEEPALIVE_INTERVAL_18, atoi(buffer));
        }
        pclose(pipe);
    }

    // Read split tunnel setting
    cmd.str("");
    cmd << "uci get vpn." << vpnName << ".split_tunnel 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            set<BOOL_T>(SPLIT_TUNNEL_20, (buffer[0] == '1'));
        }
        pclose(pipe);
    }

    // Read kill switch setting
    cmd.str("");
    cmd << "uci get vpn." << vpnName << ".kill_switch 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            set<BOOL_T>(KILL_SWITCH_21, (buffer[0] == '1'));
        }
        pclose(pipe);
    }

    WPP_LOGD(TAG, "Loaded VPN configuration from UCI");
    return true;
#else
    return false;
#endif
}

/* Save configuration to UCI */
bool VpnConfiguration::saveToUCI() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Saving VPN configuration to UCI");

    std::string vpnName = get<STRING_T>(VPN_NAME_0);
    std::stringstream cmd;

    // Create VPN section if it doesn't exist
    cmd.str("");
    cmd << "uci get vpn." << vpnName << " 2>/dev/null || uci set vpn." << vpnName << "=vpn";
    system(cmd.str().c_str());

    // Set protocol
    cmd.str("");
    INT_T protocol = get<INT_T>(PROTOCOL_TYPE_1);
    const char* protocolStr = "openvpn";
    if (protocol == PROTOCOL_WIREGUARD) protocolStr = "wireguard";
    else if (protocol == PROTOCOL_IPSEC) protocolStr = "ipsec";
    cmd << "uci set vpn." << vpnName << ".protocol='" << protocolStr << "'";
    system(cmd.str().c_str());

    // Set server address
    cmd.str("");
    cmd << "uci set vpn." << vpnName << ".server='" << get<STRING_T>(SERVER_ADDRESS_2) << "'";
    system(cmd.str().c_str());

    // Set server port
    cmd.str("");
    cmd << "uci set vpn." << vpnName << ".port='" << get<INT_T>(SERVER_PORT_3) << "'";
    system(cmd.str().c_str());

    // Set username
    cmd.str("");
    cmd << "uci set vpn." << vpnName << ".username='" << get<STRING_T>(USERNAME_7) << "'";
    system(cmd.str().c_str());

    // Set password (if not empty)
    STRING_T password = get<STRING_T>(PASSWORD_8);
    if (!password.empty()) {
        cmd.str("");
        cmd << "uci set vpn." << vpnName << ".password='" << password << "'";
        system(cmd.str().c_str());
    }

    // Set remote network
    cmd.str("");
    cmd << "uci set vpn." << vpnName << ".remote_network='" << get<STRING_T>(REMOTE_NETWORK_5) << "'";
    system(cmd.str().c_str());

    // Set keepalive interval
    cmd.str("");
    cmd << "uci set vpn." << vpnName << ".keepalive='" << get<INT_T>(KEEPALIVE_INTERVAL_18) << "'";
    system(cmd.str().c_str());

    // Set reconnect delay
    cmd.str("");
    cmd << "uci set vpn." << vpnName << ".reconnect_delay='" << get<INT_T>(RECONNECT_DELAY_19) << "'";
    system(cmd.str().c_str());

    // Set split tunnel
    cmd.str("");
    cmd << "uci set vpn." << vpnName << ".split_tunnel='" << (get<BOOL_T>(SPLIT_TUNNEL_20) ? "1" : "0") << "'";
    system(cmd.str().c_str());

    // Set kill switch
    cmd.str("");
    cmd << "uci set vpn." << vpnName << ".kill_switch='" << (get<BOOL_T>(KILL_SWITCH_21) ? "1" : "0") << "'";
    system(cmd.str().c_str());

    // Commit changes
    system("uci commit vpn");

    // Generate protocol-specific configuration
    switch (protocol) {
        case PROTOCOL_OPENVPN:
            generateOpenVPNConfig();
            break;
        case PROTOCOL_WIREGUARD:
            generateWireGuardConfig();
            break;
        case PROTOCOL_IPSEC:
            generateIPsecConfig();
            break;
    }

    WPP_LOGD(TAG, "Saved VPN configuration to UCI");
    return true;
#else
    return false;
#endif
}

/* Connect OpenVPN */
bool VpnConfiguration::connectOpenVPN() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Connecting OpenVPN");

    std::string ifname = getInterfaceName();
    std::stringstream cmd;

    // Start OpenVPN client
    cmd.str("");
    cmd << "/etc/init.d/openvpn start " << ifname;
    int result = system(cmd.str().c_str());

    if (result == 0) {
        // Wait for connection to establish
        sleep(3);
        updateConnectionStatus();
        return (get<INT_T>(CONNECTION_STATE_13) == STATE_CONNECTED);
    }

    set<STRING_T>(LAST_ERROR_17, "Failed to start OpenVPN");
    return false;
#else
    return false;
#endif
}

/* Connect WireGuard */
bool VpnConfiguration::connectWireGuard() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Connecting WireGuard");

    std::string ifname = getInterfaceName();
    std::stringstream cmd;

    // Bring up WireGuard interface
    cmd.str("");
    cmd << "ifup " << ifname;
    int result = system(cmd.str().c_str());

    if (result == 0) {
        // Setup routing
        setupRouting();
        // Setup DNS
        setupDNS();

        sleep(2);
        updateConnectionStatus();
        return (get<INT_T>(CONNECTION_STATE_13) == STATE_CONNECTED);
    }

    set<STRING_T>(LAST_ERROR_17, "Failed to start WireGuard");
    return false;
#else
    return false;
#endif
}

/* Connect IPsec */
bool VpnConfiguration::connectIPsec() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Connecting IPsec");

    std::string vpnName = get<STRING_T>(VPN_NAME_0);
    std::stringstream cmd;

    // Start strongSwan
    system("/etc/init.d/ipsec start");
    sleep(1);

    // Initiate connection
    cmd.str("");
    cmd << "ipsec up " << vpnName;
    int result = system(cmd.str().c_str());

    if (result == 0) {
        sleep(3);
        updateConnectionStatus();
        return (get<INT_T>(CONNECTION_STATE_13) == STATE_CONNECTED);
    }

    set<STRING_T>(LAST_ERROR_17, "Failed to start IPsec");
    return false;
#else
    return false;
#endif
}

/* Disconnect OpenVPN */
bool VpnConfiguration::disconnectOpenVPN() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Disconnecting OpenVPN");

    std::string ifname = getInterfaceName();
    std::stringstream cmd;

    cmd.str("");
    cmd << "/etc/init.d/openvpn stop " << ifname;
    system(cmd.str().c_str());

    return true;
#else
    return false;
#endif
}

/* Disconnect WireGuard */
bool VpnConfiguration::disconnectWireGuard() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Disconnecting WireGuard");

    std::string ifname = getInterfaceName();
    std::stringstream cmd;

    cmd.str("");
    cmd << "ifdown " << ifname;
    system(cmd.str().c_str());

    return true;
#else
    return false;
#endif
}

/* Disconnect IPsec */
bool VpnConfiguration::disconnectIPsec() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Disconnecting IPsec");

    std::string vpnName = get<STRING_T>(VPN_NAME_0);
    std::stringstream cmd;

    cmd.str("");
    cmd << "ipsec down " << vpnName;
    system(cmd.str().c_str());

    return true;
#else
    return false;
#endif
}

/* Generate OpenVPN configuration */
bool VpnConfiguration::generateOpenVPNConfig() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Generating OpenVPN configuration");

    std::string vpnName = get<STRING_T>(VPN_NAME_0);
    std::string configPath = "/etc/openvpn/" + vpnName + ".conf";

    std::ofstream configFile(configPath);
    if (!configFile.is_open()) {
        WPP_LOGE(TAG, "Failed to create OpenVPN config file");
        return false;
    }

    // Write OpenVPN configuration
    configFile << "client\n";
    configFile << "dev tun\n";
    configFile << "proto udp\n";
    configFile << "remote " << get<STRING_T>(SERVER_ADDRESS_2) << " " << get<INT_T>(SERVER_PORT_3) << "\n";
    configFile << "resolv-retry infinite\n";
    configFile << "nobind\n";
    configFile << "persist-key\n";
    configFile << "persist-tun\n";

    // Authentication
    INT_T authType = get<INT_T>(AUTHENTICATION_TYPE_6);
    if (authType == AUTH_PASSWORD) {
        configFile << "auth-user-pass /etc/openvpn/" << vpnName << ".auth\n";

        // Write credentials file
        std::string authPath = "/etc/openvpn/" + vpnName + ".auth";
        std::ofstream authFile(authPath);
        if (authFile.is_open()) {
            authFile << get<STRING_T>(USERNAME_7) << "\n";
            authFile << get<STRING_T>(PASSWORD_8) << "\n";
            authFile.close();
            chmod(authPath.c_str(), 0600);
        }
    } else if (authType == AUTH_CERTIFICATE) {
        // Write certificate files
        std::string certPath = "/etc/openvpn/" + vpnName + ".crt";
        std::string keyPath = "/etc/openvpn/" + vpnName + ".key";

        std::ofstream certFile(certPath);
        if (certFile.is_open()) {
            certFile << get<STRING_T>(CERTIFICATE_9);
            certFile.close();
        }

        std::ofstream keyFile(keyPath);
        if (keyFile.is_open()) {
            keyFile << get<STRING_T>(PRIVATE_KEY_10);
            keyFile.close();
            chmod(keyPath.c_str(), 0600);
        }

        configFile << "cert " << certPath << "\n";
        configFile << "key " << keyPath << "\n";
    }

    // Compression
    configFile << "comp-lzo\n";

    // Keepalive
    INT_T keepalive = get<INT_T>(KEEPALIVE_INTERVAL_18);
    configFile << "keepalive " << keepalive << " " << (keepalive * 3) << "\n";

    // Cipher
    configFile << "cipher AES-256-CBC\n";
    configFile << "auth SHA256\n";

    // Logging
    configFile << "verb 3\n";
    configFile << "status /var/log/openvpn-" << vpnName << "-status.log\n";

    // Route options
    if (!get<BOOL_T>(SPLIT_TUNNEL_20)) {
        configFile << "redirect-gateway def1\n";
    }

    configFile.close();
    chmod(configPath.c_str(), 0600);

    WPP_LOGD(TAG, "OpenVPN configuration generated");
    return true;
#else
    return false;
#endif
}

/* Generate WireGuard configuration */
bool VpnConfiguration::generateWireGuardConfig() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Generating WireGuard configuration");

    std::string vpnName = get<STRING_T>(VPN_NAME_0);
    std::string ifname = getInterfaceName();

    // Configure network interface via UCI
    std::stringstream cmd;

    // Create interface
    cmd.str("");
    cmd << "uci set network." << ifname << "=interface";
    system(cmd.str().c_str());

    cmd.str("");
    cmd << "uci set network." << ifname << ".proto='wireguard'";
    system(cmd.str().c_str());

    // Set private key
    STRING_T privateKey = get<STRING_T>(PRIVATE_KEY_10);
    if (privateKey.empty()) {
        generateWireGuardKeys();
        privateKey = get<STRING_T>(PRIVATE_KEY_10);
    }

    cmd.str("");
    cmd << "uci set network." << ifname << ".private_key='" << privateKey << "'";
    system(cmd.str().c_str());

    // Set listen port
    cmd.str("");
    cmd << "uci set network." << ifname << ".listen_port='51820'";
    system(cmd.str().c_str());

    // Add peer
    cmd.str("");
    cmd << "uci add network wireguard_" << ifname;
    system(cmd.str().c_str());

    cmd.str("");
    cmd << "uci set network.@wireguard_" << ifname << "[-1].public_key='" << get<STRING_T>(PUBLIC_KEY_11) << "'";
    system(cmd.str().c_str());

    cmd.str("");
    cmd << "uci set network.@wireguard_" << ifname << "[-1].endpoint_host='" << get<STRING_T>(SERVER_ADDRESS_2) << "'";
    system(cmd.str().c_str());

    cmd.str("");
    cmd << "uci set network.@wireguard_" << ifname << "[-1].endpoint_port='" << get<INT_T>(SERVER_PORT_3) << "'";
    system(cmd.str().c_str());

    // Allowed IPs
    STRING_T remoteNetwork = get<STRING_T>(REMOTE_NETWORK_5);
    if (remoteNetwork.empty()) {
        remoteNetwork = "0.0.0.0/0, ::/0";  // Route all traffic
    }

    cmd.str("");
    cmd << "uci set network.@wireguard_" << ifname << "[-1].allowed_ips='" << remoteNetwork << "'";
    system(cmd.str().c_str());

    // Persistent keepalive
    cmd.str("");
    cmd << "uci set network.@wireguard_" << ifname << "[-1].persistent_keepalive='" << get<INT_T>(KEEPALIVE_INTERVAL_18) << "'";
    system(cmd.str().c_str());

    // Pre-shared key (if available)
    STRING_T psk = get<STRING_T>(PRE_SHARED_KEY_12);
    if (!psk.empty()) {
        cmd.str("");
        cmd << "uci set network.@wireguard_" << ifname << "[-1].preshared_key='" << psk << "'";
        system(cmd.str().c_str());
    }

    system("uci commit network");

    WPP_LOGD(TAG, "WireGuard configuration generated");
    return true;
#else
    return false;
#endif
}

/* Generate IPsec configuration */
bool VpnConfiguration::generateIPsecConfig() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Generating IPsec configuration");

    std::string vpnName = get<STRING_T>(VPN_NAME_0);

    // Write ipsec.conf
    std::ofstream ipsecConf("/etc/ipsec.conf", std::ios::app);
    if (ipsecConf.is_open()) {
        ipsecConf << "\nconn " << vpnName << "\n";
        ipsecConf << "    auto=start\n";
        ipsecConf << "    keyexchange=ikev2\n";
        ipsecConf << "    type=tunnel\n";
        ipsecConf << "    left=%defaultroute\n";
        ipsecConf << "    leftauth=eap-mschapv2\n";
        ipsecConf << "    leftsourceip=%config\n";
        ipsecConf << "    right=" << get<STRING_T>(SERVER_ADDRESS_2) << "\n";
        ipsecConf << "    rightauth=pubkey\n";
        ipsecConf << "    rightid=%any\n";

        STRING_T remoteNetwork = get<STRING_T>(REMOTE_NETWORK_5);
        if (!remoteNetwork.empty()) {
            ipsecConf << "    rightsubnet=" << remoteNetwork << "\n";
        } else {
            ipsecConf << "    rightsubnet=0.0.0.0/0\n";
        }

        ipsecConf << "    eap_identity=" << get<STRING_T>(USERNAME_7) << "\n";
        ipsecConf << "    dpdaction=restart\n";
        ipsecConf << "    dpddelay=" << get<INT_T>(KEEPALIVE_INTERVAL_18) << "s\n";
        ipsecConf.close();
    }

    // Write ipsec.secrets
    std::ofstream ipsecSecrets("/etc/ipsec.secrets", std::ios::app);
    if (ipsecSecrets.is_open()) {
        ipsecSecrets << "\n" << get<STRING_T>(USERNAME_7)
                    << " : EAP \"" << get<STRING_T>(PASSWORD_8) << "\"\n";
        ipsecSecrets.close();
        chmod("/etc/ipsec.secrets", 0600);
    }

    WPP_LOGD(TAG, "IPsec configuration generated");
    return true;
#else
    return false;
#endif
}

/* Generate WireGuard keys */
bool VpnConfiguration::generateWireGuardKeys() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Generating WireGuard keys");

    FILE* pipe;
    char buffer[256];

    // Generate private key
    pipe = popen("wg genkey", "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            set<STRING_T>(PRIVATE_KEY_10, buffer);

            // Generate public key from private key
            std::string privKey(buffer);
            pclose(pipe);

            std::stringstream cmd;
            cmd << "echo '" << privKey << "' | wg pubkey";
            pipe = popen(cmd.str().c_str(), "r");
            if (pipe && fgets(buffer, sizeof(buffer), pipe)) {
                buffer[strcspn(buffer, "\n")] = 0;
                set<STRING_T>(PUBLIC_KEY_11, buffer);
                pclose(pipe);
                return true;
            }
        }
        if (pipe) pclose(pipe);
    }

    WPP_LOGE(TAG, "Failed to generate WireGuard keys");
    return false;
#else
    return false;
#endif
}

/* Setup kill switch */
bool VpnConfiguration::setupKillSwitch() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Setting up VPN kill switch");

    std::string ifname = getInterfaceName();
    std::stringstream cmd;

    // Block all traffic except through VPN interface
    cmd.str("");
    cmd << "iptables -I FORWARD -o " << ifname << " -j ACCEPT && "
        << "iptables -I FORWARD -i " << ifname << " -j ACCEPT && "
        << "iptables -I FORWARD -j REJECT";
    system(cmd.str().c_str());

    // IPv6
    cmd.str("");
    cmd << "ip6tables -I FORWARD -o " << ifname << " -j ACCEPT && "
        << "ip6tables -I FORWARD -i " << ifname << " -j ACCEPT && "
        << "ip6tables -I FORWARD -j REJECT";
    system(cmd.str().c_str());

    WPP_LOGD(TAG, "VPN kill switch enabled");
    return true;
#else
    return false;
#endif
}

/* Remove kill switch */
bool VpnConfiguration::removeKillSwitch() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Removing VPN kill switch");

    std::string ifname = getInterfaceName();
    std::stringstream cmd;

    // Remove kill switch rules
    cmd.str("");
    cmd << "iptables -D FORWARD -o " << ifname << " -j ACCEPT 2>/dev/null; "
        << "iptables -D FORWARD -i " << ifname << " -j ACCEPT 2>/dev/null; "
        << "iptables -D FORWARD -j REJECT 2>/dev/null";
    system(cmd.str().c_str());

    cmd.str("");
    cmd << "ip6tables -D FORWARD -o " << ifname << " -j ACCEPT 2>/dev/null; "
        << "ip6tables -D FORWARD -i " << ifname << " -j ACCEPT 2>/dev/null; "
        << "ip6tables -D FORWARD -j REJECT 2>/dev/null";
    system(cmd.str().c_str());

    WPP_LOGD(TAG, "VPN kill switch removed");
    return true;
#else
    return false;
#endif
}

/* Setup split tunnel */
bool VpnConfiguration::setupSplitTunnel() {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Setting up split tunnel");

    std::string ifname = getInterfaceName();
    STRING_T remoteNetwork = get<STRING_T>(REMOTE_NETWORK_5);

    if (!remoteNetwork.empty()) {
        std::stringstream cmd;
        cmd << "ip route add " << remoteNetwork << " dev " << ifname;
        system(cmd.str().c_str());
    }

    return true;
#else
    return false;
#endif
}

/* Setup routing */
bool VpnConfiguration::setupRouting() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Setting up VPN routing");

    std::string ifname = getInterfaceName();
    std::stringstream cmd;

    if (!get<BOOL_T>(SPLIT_TUNNEL_20)) {
        // Route all traffic through VPN
        cmd.str("");
        cmd << "ip route add default dev " << ifname;
        system(cmd.str().c_str());
    } else {
        // Setup split tunnel routing
        setupSplitTunnel();
    }

    return true;
#else
    return false;
#endif
}

/* Setup DNS */
bool VpnConfiguration::setupDNS() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Setting up VPN DNS");

    std::string ifname = getInterfaceName();

    // Configure DNS via DHCP or static
    // This prevents DNS leaks
    std::stringstream cmd;
    cmd << "uci set network." << ifname << ".peerdns='0'";
    system(cmd.str().c_str());

    system("uci commit network");

    return true;
#else
    return false;
#endif
}

/* Update connection status */
bool VpnConfiguration::updateConnectionStatus() {
#ifdef OPENWRT_BUILD
    INT_T protocol = get<INT_T>(PROTOCOL_TYPE_1);
    std::string ifname = getInterfaceName();
    bool isConnected = false;

    // Check interface status
    std::stringstream cmd;
    cmd << "ip link show " << ifname << " 2>/dev/null | grep -q 'state UP'";

    if (system(cmd.str().c_str()) == 0) {
        isConnected = true;

        // Get local IP address
        cmd.str("");
        cmd << "ip -4 addr show " << ifname << " 2>/dev/null | grep inet | awk '{print $2}' | cut -d/ -f1";
        FILE* pipe = popen(cmd.str().c_str(), "r");
        if (pipe) {
            char buffer[256];
            if (fgets(buffer, sizeof(buffer), pipe)) {
                buffer[strcspn(buffer, "\n")] = 0;
                set<STRING_T>(LOCAL_ADDRESS_4, buffer);
            }
            pclose(pipe);
        }
    }

    // Update state
    if (isConnected) {
        set<INT_T>(CONNECTION_STATE_13, STATE_CONNECTED);
        if (connectionStartTime > 0) {
            set<INT_T>(UPTIME_16, time(nullptr) - connectionStartTime);
        }
    } else {
        INT_T currentState = get<INT_T>(CONNECTION_STATE_13);
        if (currentState == STATE_CONNECTED || currentState == STATE_CONNECTING) {
            set<INT_T>(CONNECTION_STATE_13, STATE_DISCONNECTED);
        }
    }

    // Update traffic stats
    updateTrafficStats();

    return true;
#else
    return false;
#endif
}

/* Update traffic statistics */
bool VpnConfiguration::updateTrafficStats() {
#ifdef OPENWRT_BUILD
    std::string ifname = getInterfaceName();
    INT_T protocol = get<INT_T>(PROTOCOL_TYPE_1);

    if (protocol == PROTOCOL_WIREGUARD) {
        // Get WireGuard statistics
        std::stringstream cmd;
        cmd << "wg show " << ifname << " transfer 2>/dev/null";
        FILE* pipe = popen(cmd.str().c_str(), "r");
        if (pipe) {
            char buffer[256];
            if (fgets(buffer, sizeof(buffer), pipe)) {
                // Parse: <peer_pubkey>\t<rx_bytes>\t<tx_bytes>
                char* token = strtok(buffer, "\t");
                if (token) token = strtok(NULL, "\t");  // rx_bytes
                if (token) {
                    set<INT_T>(BYTES_RECEIVED_15, atoll(token));
                    token = strtok(NULL, "\t");  // tx_bytes
                    if (token) {
                        set<INT_T>(BYTES_SENT_14, atoll(token));
                    }
                }
            }
            pclose(pipe);
        }
    } else {
        // Get interface statistics from /sys
        std::stringstream rxPath, txPath;
        rxPath << "/sys/class/net/" << ifname << "/statistics/rx_bytes";
        txPath << "/sys/class/net/" << ifname << "/statistics/tx_bytes";

        std::ifstream rxFile(rxPath.str());
        if (rxFile.is_open()) {
            INT_T rxBytes;
            rxFile >> rxBytes;
            set<INT_T>(BYTES_RECEIVED_15, rxBytes);
            rxFile.close();
        }

        std::ifstream txFile(txPath.str());
        if (txFile.is_open()) {
            INT_T txBytes;
            txFile >> txBytes;
            set<INT_T>(BYTES_SENT_14, txBytes);
            txFile.close();
        }
    }

    return true;
#else
    return false;
#endif
}

/* Get interface name */
std::string VpnConfiguration::getInterfaceName() {
    std::string vpnName = get<STRING_T>(VPN_NAME_0);
    INT_T protocol = get<INT_T>(PROTOCOL_TYPE_1);

    if (protocol == PROTOCOL_OPENVPN) {
        return "tun" + std::to_string(instId());
    } else if (protocol == PROTOCOL_WIREGUARD) {
        return "wg" + std::to_string(instId());
    } else if (protocol == PROTOCOL_IPSEC) {
        return "ipsec" + std::to_string(instId());
    }

    return vpnName;
}

/* Start monitoring */
bool VpnConfiguration::startMonitoring() {
    WPP_LOGD(TAG, "Starting VPN monitoring");
    isMonitoring = true;
    // In a real implementation, this would start a background thread
    // to periodically update connection status and statistics
    return true;
}

/* Stop monitoring */
bool VpnConfiguration::stopMonitoring() {
    WPP_LOGD(TAG, "Stopping VPN monitoring");
    isMonitoring = false;
    return true;
}

/* Validate certificate */
bool VpnConfiguration::validateCertificate(const std::string& cert) {
    // Basic validation: check for PEM format markers
    if (cert.find("-----BEGIN CERTIFICATE-----") == std::string::npos ||
        cert.find("-----END CERTIFICATE-----") == std::string::npos) {
        return false;
    }
    return true;
}

/* Validate private key */
bool VpnConfiguration::validatePrivateKey(const std::string& key) {
    // Basic validation: check for PEM format markers
    if ((key.find("-----BEGIN PRIVATE KEY-----") == std::string::npos &&
         key.find("-----BEGIN RSA PRIVATE KEY-----") == std::string::npos) ||
        (key.find("-----END PRIVATE KEY-----") == std::string::npos &&
         key.find("-----END RSA PRIVATE KEY-----") == std::string::npos)) {
        return false;
    }
    return true;
}

/* Validate IPv4 or IPv6 address */
bool VpnConfiguration::isValidIPAddress(const std::string& ip) {
    struct sockaddr_in sa4;
    struct sockaddr_in6 sa6;
    return (inet_pton(AF_INET, ip.c_str(), &(sa4.sin_addr)) == 1) ||
           (inet_pton(AF_INET6, ip.c_str(), &(sa6.sin6_addr)) == 1);
}

/* Validate port number */
bool VpnConfiguration::isValidPort(int port) {
    return (port >= 1 && port <= 65535);
}

/* Validate CIDR notation */
bool VpnConfiguration::isValidCIDR(const std::string& cidr) {
    size_t slashPos = cidr.find('/');
    if (slashPos == std::string::npos) {
        return false;
    }

    std::string ip = cidr.substr(0, slashPos);
    std::string prefixStr = cidr.substr(slashPos + 1);

    if (!isValidIPAddress(ip)) {
        return false;
    }

    try {
        int prefix = std::stoi(prefixStr);
        // Check if it's IPv4 or IPv6
        if (ip.find(':') != std::string::npos) {
            // IPv6
            return (prefix >= 0 && prefix <= 128);
        } else {
            // IPv4
            return (prefix >= 0 && prefix <= 32);
        }
    } catch (...) {
        return false;
    }
}

#endif // OBJ_W_10537_VPN_CONFIGURATION
