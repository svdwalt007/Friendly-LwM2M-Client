/*
 * main.cpp
 *
 * OpenWRT LwM2M Client Example
 * Demonstrates full integration with UCI, ubus, and sysfs
 */

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <csignal>

// Wpp library
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppGuard.h"

// LwM2M Objects
#include "Lwm2mSecurity.h"
#include "Lwm2mServer.h"
#include "Lwm2mDevice.h"
#include "Lwm2mConnectivityMonitoring.h"

// OpenWRT integration
#include "OpenWrtConnection.h"
#include "OpenWrtPlatform.h"
#include "UciConfig.h"
#include "UbusInterface.h"
#include "SysfsMonitor.h"

using namespace wpp;

// Global flag for graceful shutdown
static volatile bool g_running = true;

// Signal handler
void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << ", shutting down..." << std::endl;
    g_running = false;
}

// Helper function to setup Device object from UCI and sysfs
void setupDeviceObject(UciConfig& uci, SysfsMonitor& sysfs) {
    auto& device = Lwm2mDevice::instance();
    auto config = uci.getDeviceConfig();
    auto sysInfo = sysfs.getSystemInfo();
    auto memInfo = sysfs.getMemoryInfo();
    auto cpuInfo = sysfs.getCpuInfo();

    // Set device information from UCI
    device.setManufacturer(config.manufacturer);
    device.setModelNumber(config.modelNumber);
    device.setSerialNumber(config.serialNumber.empty() ? sysInfo.hostname : config.serialNumber);
    device.setFirmwareVersion(config.firmwareVersion);
    device.setHardwareVersion(config.hardwareVersion);
    device.setDeviceType(config.deviceType);

    // Set system information from sysfs
    device.setAvailablePowerSources({1}); // 1 = DC power (typical for routers)
    device.setPowerSourceVoltage({12000}); // 12V in mV
    device.setPowerSourceCurrent({500}); // 500mA

    // Set memory information
    uint64_t totalMemKB = memInfo.totalKB;
    uint64_t freeMemKB = memInfo.freeKB;
    device.setMemoryTotal(totalMemKB);
    device.setMemoryFree(freeMemKB);

    // Set current time
    device.setCurrentTime(WppPlatform::getTime());

    std::cout << "Device object configured:" << std::endl;
    std::cout << "  Manufacturer: " << config.manufacturer << std::endl;
    std::cout << "  Model: " << config.modelNumber << std::endl;
    std::cout << "  Serial: " << device.getSerialNumber() << std::endl;
    std::cout << "  Firmware: " << config.firmwareVersion << std::endl;
    std::cout << "  Memory: " << freeMemKB << "/" << totalMemKB << " KB" << std::endl;
}

// Helper function to setup Connectivity Monitoring from sysfs
void setupConnectivityMonitoring(SysfsMonitor& sysfs, const std::string& interface) {
    auto& connMon = Lwm2mConnectivityMonitoring::instance();
    auto netInfo = sysfs.getNetworkInfo(interface);

    if (!netInfo.empty()) {
        auto& net = netInfo[0];

        // Set network bearer (0=GSM, 21=WLAN, 41=Ethernet, 42=DSL, 43=PLC)
        int bearer = 41; // Default to Ethernet
        if (interface.find("wlan") != std::string::npos ||
            interface.find("wifi") != std::string::npos) {
            bearer = 21; // WLAN
        }
        connMon.setNetworkBearer(bearer);

        // Set available network bearers
        connMon.setAvailableNetworkBearer({bearer});

        // Set IP addresses
        if (!net.ipv4.empty()) {
            connMon.setIpAddresses({net.ipv4});
        }

        // Set link quality (signal strength for wireless)
        if (net.signalStrength != -100) {
            connMon.setLinkQuality(std::abs(net.signalStrength));
        }

        std::cout << "Connectivity Monitoring configured:" << std::endl;
        std::cout << "  Interface: " << interface << std::endl;
        std::cout << "  Bearer: " << bearer << std::endl;
        std::cout << "  Status: " << (net.isUp ? "UP" : "DOWN") << std::endl;
    }
}

// Helper function to setup Security and Server from UCI
bool setupSecurityAndServer(UciConfig& uci, WppRegistry& registry) {
    auto servers = uci.getServers();
    auto securities = uci.getSecurityConfigs();

    if (servers.empty()) {
        std::cerr << "No LwM2M servers configured in UCI!" << std::endl;
        return false;
    }

    for (size_t i = 0; i < servers.size() && i < securities.size(); i++) {
        const auto& srv = servers[i];
        const auto& sec = securities[i];

        // Create Security object instance
        OBJ_LINK_T securityLink = registry.createInstance(ID_T(0), INSTANCE_ID_T(i));
        if (!securityLink) {
            std::cerr << "Failed to create Security instance " << i << std::endl;
            continue;
        }

        auto security = Lwm2mSecurity::cast(securityLink);
        if (!security) {
            std::cerr << "Failed to cast Security instance " << i << std::endl;
            continue;
        }

        // Configure security
        security->setUri(srv.uri);
        security->setBootstrapServer(srv.bootstrapServer);
        security->setSecurityMode(sec.securityMode);
        security->setShortServerId(srv.serverId);

        // Set security credentials based on mode
        if (sec.securityMode == 0) { // PSK
            // TODO: Convert hex strings to byte arrays
            // security->setPskIdentity(hexToBytes(sec.pskIdentity));
            // security->setPskKey(hexToBytes(sec.pskKey));
        }

        // Create Server object instance
        OBJ_LINK_T serverLink = registry.createInstance(ID_T(1), INSTANCE_ID_T(i));
        if (!serverLink) {
            std::cerr << "Failed to create Server instance " << i << std::endl;
            continue;
        }

        auto server = Lwm2mServer::cast(serverLink);
        if (!server) {
            std::cerr << "Failed to cast Server instance " << i << std::endl;
            continue;
        }

        // Configure server
        server->setShortServerId(srv.serverId);
        server->setLifetime(srv.lifetime);
        server->setNotificationStoring(false);
        server->setBinding(srv.binding);

        std::cout << "Configured server: " << srv.uri << std::endl;
        std::cout << "  Server ID: " << srv.serverId << std::endl;
        std::cout << "  Lifetime: " << srv.lifetime << "s" << std::endl;
        std::cout << "  Binding: " << srv.binding << std::endl;
        std::cout << "  Security: " << (sec.securityMode == 3 ? "NoSec" : "Secured") << std::endl;
    }

    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "OpenWRT LwM2M Client starting..." << std::endl;

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Enable syslog for daemon operation
    setUseSyslog(true);
    setLogLevel(LOG_INFO);

    // Initialize UCI configuration
    UciConfig uci("lwm2m");
    if (!uci.load()) {
        std::cerr << "Failed to load UCI configuration: " << uci.getLastError() << std::endl;
        return 1;
    }

    auto generalConfig = uci.getGeneralConfig();
    auto networkConfig = uci.getNetworkConfig();

    std::cout << "Configuration loaded:" << std::endl;
    std::cout << "  Endpoint: " << generalConfig.endpointName << std::endl;
    std::cout << "  Interface: " << networkConfig.interface << std::endl;
    std::cout << "  Port: " << networkConfig.port << std::endl;

    // Initialize sysfs monitor
    SysfsMonitor sysfs;

    // Create OpenWRT connection
    int addressFamily = AF_INET6;
    if (!networkConfig.enableIPv6) {
        addressFamily = AF_INET;
    }

    auto connection = std::make_unique<OpenWrtConnection>(
        networkConfig.interface,
        std::to_string(networkConfig.port),
        addressFamily
    );

    if (!connection->isReady()) {
        std::cerr << "Failed to initialize network connection!" << std::endl;
        return 1;
    }

    std::cout << "Network connection ready:" << std::endl;
    std::cout << "  Local address: " << connection->getLocalAddress() << std::endl;
    std::cout << "  Local port: " << connection->getLocalPort() << std::endl;

    // Get WppClient instance
    WppClient& client = WppClient::instance();

    // Take ownership before configuration
    WppClient::OBJ_REGISTRY_T registry = client.takeOwnership();

    // Setup LwM2M objects
    setupDeviceObject(uci, sysfs);
    setupConnectivityMonitoring(sysfs, networkConfig.interface);

    if (!setupSecurityAndServer(uci, registry)) {
        std::cerr << "Failed to setup Security and Server objects!" << std::endl;
        client.giveOwnership(std::move(registry));
        return 1;
    }

    // Give ownership back
    client.giveOwnership(std::move(registry));

    // Set endpoint name
    client.setEndpointName(generalConfig.endpointName);

    // Set connection
    client.setConnection(connection.get());

    // Initialize ubus interface
    auto ubus = std::make_unique<UbusInterface>(&client, "lwm2m");
    if (ubus->init()) {
        if (ubus->registerObject()) {
            std::cout << "ubus interface registered: lwm2m" << std::endl;
        } else {
            std::cerr << "Failed to register ubus object: " << ubus->getLastError() << std::endl;
        }
    } else {
        std::cerr << "Failed to initialize ubus: " << ubus->getLastError() << std::endl;
    }

    // Start client
    if (!client.start()) {
        std::cerr << "Failed to start LwM2M client!" << std::endl;
        return 1;
    }

    std::cout << "LwM2M client started successfully!" << std::endl;
    std::cout << "Press Ctrl+C to stop..." << std::endl;

    // Main loop
    while (g_running) {
        // Process network packets
        connection->loop();

        // Process ubus events
        if (ubus->isConnected()) {
            ubus->process(10); // 10ms timeout
        }

        // Process LwM2M client
        client.loop();

        // Update device resources periodically
        static time_t lastUpdate = 0;
        time_t now = WppPlatform::getTime();
        if (now - lastUpdate >= 60) { // Update every minute
            auto& device = Lwm2mDevice::instance();
            auto memInfo = sysfs.getMemoryInfo();

            device.setMemoryFree(memInfo.freeKB);
            device.setCurrentTime(now);

            lastUpdate = now;
        }

        // Small sleep to prevent CPU spinning
        usleep(1000); // 1ms
    }

    // Cleanup
    std::cout << "Stopping LwM2M client..." << std::endl;
    client.stop();

    if (ubus->isConnected()) {
        ubus->unregisterObject();
    }

    std::cout << "LwM2M client stopped." << std::endl;
    return 0;
}
