#include "objects.h"
#include "cli_options.h"
#include <iostream>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

/* ------------- Internal helpful states and methods ------------- */

static bool _rebootDevice = false;

#ifdef OBJ_O_4_CONNECTIVITY_MONITORING
static void getIpAddress(string* ip) {
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    // retrieve the current interfaces - returns 0 on success
    if (getifaddrs(&interfaces) == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while (temp_addr != NULL) {
            if (temp_addr->ifa_addr->sa_family == AF_INET && strcmp(temp_addr->ifa_name, "en0")) {
                *ip = inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    freeifaddrs(interfaces);  // free memory
}
#endif // OBJ_O_4_CONNECTIVITY_MONITORING

/* ------------- Methods to init objects ------------- */

void serverInit(WppClient &client, const CliOptions& options) {
    client.registry().registerObj(Lwm2mServer::object(client));
    Instance *server = Lwm2mServer::createInst(client);

	server->set<INT_T>(Lwm2mServer::SHORT_SERVER_ID_0, TEST_SERVER_SHORT_ID);
	server->set<STRING_T>(Lwm2mServer::BINDING_7, WPP_BINDING_UDP);
	// CLI: Use lifetime from command-line options instead of hardcoded value
	server->set<TIME_T>(Lwm2mServer::LIFETIME_1, options.lifetime);
	server->set<BOOL_T>(Lwm2mServer::NOTIFICATION_STORING_WHEN_DISABLED_OR_OFFLINE_6, false);

	#if OBJ_O_2_LWM2M_ACCESS_CONTROL
	Lwm2mAccessControl::create(Lwm2mServer::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
	Lwm2mAccessControl::create(*server, TEST_SERVER_SHORT_ID);
	#endif
}

void securityInit(WppClient &client, const CliOptions& options) {
    client.registry().registerObj(Lwm2mSecurity::object(client));
    wpp::Instance *security = Lwm2mSecurity::createInst(client);

    // CLI: Use server URI from command-line options
    string url = options.server_uri;

    // CLI: Use bootstrap mode from command-line options
    security->set<BOOL_T>(Lwm2mSecurity::BOOTSTRAP_SERVER_1, options.use_bootstrap);

    if (options.use_bootstrap) {
        security->set<INT_T>(Lwm2mSecurity::CLIENT_HOLD_OFF_TIME_11, 10);
    }

    // CLI: Use security mode from command-line options (runtime selection)
    switch (options.security_mode) {
        case SecurityMode::NONE:
            security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_NONE);
            break;

        case SecurityMode::PSK:
            security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_PRE_SHARED_KEY);
            // CLI: Use PSK identity and key from command-line options
            security->set(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3,
                         OPAQUE_T(options.psk_identity.begin(), options.psk_identity.end()));
            security->set(Lwm2mSecurity::SECRET_KEY_5,
                         OPAQUE_T(options.psk_key.begin(), options.psk_key.end()));
            break;

        case SecurityMode::RPK:
            security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY);
            // CLI: Use RPK keys from command-line options
            {
                vector<uint8_t> rpkPublic, rpkPrivate;
                if (hexStringToBytes(options.rpk_public_key, rpkPublic) &&
                    hexStringToBytes(options.rpk_private_key, rpkPrivate)) {
                    security->set(Lwm2mSecurity::PUBLIC_KEY_OR_IDENTITY_3,
                                 OPAQUE_T(rpkPublic.begin(), rpkPublic.end()));
                    security->set(Lwm2mSecurity::SECRET_KEY_5,
                                 OPAQUE_T(rpkPrivate.begin(), rpkPrivate.end()));
                } else {
                    cerr << "Warning: Failed to parse RPK keys, falling back to NO_SEC" << endl;
                    security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_NONE);
                }
            }
            break;

        case SecurityMode::CERT:
            security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_CERTIFICATE);
            // Note: Certificate mode requires file loading which is not fully implemented here
            // The cert_file, key_file, and ca_file paths are available in options
            cerr << "Warning: Certificate mode not fully implemented yet" << endl;
            break;

        default:
            cerr << "Error: Unknown security mode, falling back to NONE" << endl;
            security->set<INT_T>(Lwm2mSecurity::SECURITY_MODE_2, LWM2M_SECURITY_MODE_NONE);
            break;
    }

    security->set<STRING_T>(Lwm2mSecurity::LWM2M_SERVER_URI_0, url);
    security->set<INT_T>(Lwm2mSecurity::SHORT_SERVER_ID_10, TEST_SERVER_SHORT_ID);
}

void deviceInit(WppClient &client, const CliOptions& options) {
    client.registry().registerObj(Device::object(client));
	wpp::Instance *device = Device::createInst(client);

    device->set<EXECUTE_T>(Device::REBOOT_4, [](Instance& inst, ID_T resId, const OPAQUE_T& data) {
    	cout << "Device: execute REBOOT_4" << endl;
    	_rebootDevice = true;
        return true;
    });
    device->set<INT_T>(Device::ERROR_CODE_11, 0, Device::NO_ERROR);
    device->set<STRING_T>(Device::SUPPORTED_BINDING_AND_MODES_16, WPP_BINDING_UDP);
    device->set<STRING_T>(Device::MANUFACTURER_0, "OpenWrt/Walt Technologies");
    device->set<STRING_T>(Device::MODEL_NUMBER_1, "OpenWRT One Router");
    // CLI: Use serial number from command-line options
    device->set<STRING_T>(Device::SERIAL_NUMBER_2, options.serial_number);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
	Lwm2mAccessControl::create(Device::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
	Lwm2mAccessControl::create(*device, TEST_SERVER_SHORT_ID);
	#endif
}

#ifdef OBJ_O_5_FIRMWARE_UPDATE
// For OpenWRT devices with sysupgrade support, you can use the OpenWRT firmware updater:
// To enable: cmake -DUSE_OPENWRT_FW_UPDATER=ON
//
#ifdef USE_OPENWRT_FW_UPDATER
void fwUpdaterInit(WppClient &client) {
    static OpenWrtFwManager fwManager;
    initOpenWrtFirmwareUpdate(client, fwManager);
}
#else
void fwUpdaterInit(WppClient &client) {
    #if RES_5_8
    static FwUriDownloader fwUriDownloader;
	#endif
    static FwAutoDownloader fwAutoDownloader;
	static FirmwareUpdater fwUpdater;

    client.registry().registerObj(FirmwareUpdate::object(client));
    FirmwareUpdate::createInst(client);

    FirmwareUpdate::setFwUpdater(client, fwUpdater);
    FirmwareUpdate::setFwInternalDownloader(client, fwAutoDownloader);
    #if RES_5_8
    FirmwareUpdate::setFwExternalDownloader(client, fwUriDownloader);
    #endif

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
	Lwm2mAccessControl::create(FirmwareUpdate::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
	Lwm2mAccessControl::create(*FirmwareUpdate::instance(client), TEST_SERVER_SHORT_ID);
	#endif
}
#endif
#endif

#ifdef OBJ_O_2_LWM2M_ACCESS_CONTROL
void acInit(WppClient &client) {
    client.registry().registerObj(Lwm2mAccessControl::object(client));
}
#endif

#ifdef OBJ_O_4_CONNECTIVITY_MONITORING
void connMonitoringInit(WppClient &client) {
    client.registry().registerObj(ConnectivityMonitoring::object(client));
    Instance *connMon = ConnectivityMonitoring::createInst(client);

    STRING_T ip;
    getIpAddress(&ip);
	connMon->set<INT_T>(ConnectivityMonitoring::NETWORK_BEARER_0, ConnectivityMonitoring::NtwrkBrr::ETHERNET);
	connMon->set<INT_T>(ConnectivityMonitoring::RADIO_SIGNAL_STRENGTH_2, -20);
    connMon->set<STRING_T>(ConnectivityMonitoring::IP_ADDRESSES_4, 0, ip);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
	Lwm2mAccessControl::create(ConnectivityMonitoring::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
	Lwm2mAccessControl::create(*connMon, TEST_SERVER_SHORT_ID);
	#endif
}
#endif

#ifdef OBJ_O_6_LOCATION
void locationInit(WppClient &client) {
    client.registry().registerObj(Location::object(client));
    Instance *location = Location::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
	Lwm2mAccessControl::create(Location::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
	Lwm2mAccessControl::create(*location, TEST_SERVER_SHORT_ID);
	#endif
}
#endif

#ifdef OBJ_O_12_WLAN_CONNECTIVITY
void wlanConnectivityInit(WppClient &client) {
    client.registry().registerObj(WlanConnectivity::object(client));

    // Create instance for 2.4GHz WiFi (wlan0)
    Instance *wlan0 = WlanConnectivity::createInst(client, 0);

    // Create instance for 5GHz WiFi (wlan1)
    Instance *wlan1 = WlanConnectivity::createInst(client, 1);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
	Lwm2mAccessControl::create(WlanConnectivity::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
	Lwm2mAccessControl::create(*wlan0, TEST_SERVER_SHORT_ID);
	Lwm2mAccessControl::create(*wlan1, TEST_SERVER_SHORT_ID);
	#endif
}
#endif

#ifdef OBJ_O_13_BEARER_SELECTION
void bearerSelectionInit(WppClient &client) {
    client.registry().registerObj(BearerSelection::object(client));
    Instance *bearer = BearerSelection::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
	Lwm2mAccessControl::create(BearerSelection::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
	Lwm2mAccessControl::create(*bearer, TEST_SERVER_SHORT_ID);
	#endif
}
#endif

#ifdef OBJ_O_3339_AUDIO_CLIP
void audioClipInit(WppClient &client) {
    client.registry().registerObj(AudioClip::object(client));
	AudioClip::createInst(client);
	#if OBJ_O_2_LWM2M_ACCESS_CONTROL
	Lwm2mAccessControl::create(AudioClip::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
	Lwm2mAccessControl::create(*AudioClip::instance(client), TEST_SERVER_SHORT_ID);
	#endif
}
#endif

#ifdef OBJ_O_10519_HARDWARE_WATCHDOG
void hardwareWatchdogInit(WppClient &client) {
    client.registry().registerObj(HardwareWatchdog::object(client));
    HardwareWatchdog *watchdog = HardwareWatchdog::createInst(client);

    // Configure watchdog with default settings
    // Note: Watchdog is disabled by default for safety
    // Enable it via LWM2M server or set WATCHDOG_ENABLE_0 to true

    cout << "Hardware Watchdog initialized (Object ID: 34607)" << endl;
    cout << "  Device: /dev/watchdog" << endl;
    cout << "  Default Timeout: 30 seconds" << endl;
    cout << "  Default Interval: 5 seconds" << endl;
    cout << "  Status: Disabled (enable via LWM2M server)" << endl;

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(HardwareWatchdog::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(*watchdog, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

#ifdef OBJ_O_10512_STARLINK_TERMINAL
void starlinkTerminalInit(WppClient &client) {
    client.registry().registerObj(StarlinkTerminal::object(client));
    Instance *starlink = StarlinkTerminal::createInst(client);

    // Initialize with default values - resources are already initialized
    // The gRPC endpoint can be configured if needed:
    // starlink->set<STRING_T>(StarlinkTerminal::GRPC_ENDPOINT_107, "192.168.100.1:9200");

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(StarlinkTerminal::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(*starlink, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

#ifdef OBJ_O_10520_MIKROBUS
void mikrobusInit(WppClient &client) {
    client.registry().registerObj(Mikrobus::object(client));

    // Create instance for MIKROBUS socket 1
    Instance *mikrobus1 = Mikrobus::createInst(client, 0);
    mikrobus1->set<STRING_T>(Mikrobus::SOCKET_NAME_1, "MIKROBUS-1");

    // Create instance for MIKROBUS socket 2 (if hardware supports it)
    // Instance *mikrobus2 = Mikrobus::createInst(client, 1);
    // mikrobus2->set<STRING_T>(Mikrobus::SOCKET_NAME_1, "MIKROBUS-2");

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(Mikrobus::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(*mikrobus1, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

/* ------------- Helpful methods ------------- */

bool isDeviceShouldBeRebooted() {
    return _rebootDevice;
}
/* ---------- Walt Technologies custom objects init begin ---------- */

#ifdef OBJ_W_10513_ROUTER_MANAGEMENT
void routerManagementInit(WppClient &client) {
    client.registry().registerObj(RouterManagement::object(client));
    Instance *routerMgmt = RouterManagement::createInst(client);

    // Default values are set in initResources
    // Can be overridden here if needed
    
    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(RouterManagement::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(*routerMgmt, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

#ifdef OBJ_W_10514_ETHERNET_INTERFACE
void ethernetInterfaceInit(WppClient &client) {
    client.registry().registerObj(EthernetInterface::object(client));
    
    // Create instance for WAN port (eth0)
    Instance *eth0 = EthernetInterface::createInst(client, 0);
    eth0->set<STRING_T>(EthernetInterface::INTERFACE_NAME_0, "eth0");
    eth0->set<INT_T>(EthernetInterface::PORT_TYPE_1, EthernetInterface::PORT_WAN);
    eth0->set<INT_T>(EthernetInterface::SPEED_2, 2500); // 2.5 Gbps
    
    // Create instance for LAN port (eth1)
    Instance *eth1 = EthernetInterface::createInst(client, 1);
    eth1->set<STRING_T>(EthernetInterface::INTERFACE_NAME_0, "eth1");
    eth1->set<INT_T>(EthernetInterface::PORT_TYPE_1, EthernetInterface::PORT_LAN);
    eth1->set<INT_T>(EthernetInterface::SPEED_2, 1000); // 1 Gbps

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(EthernetInterface::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(*eth0, TEST_SERVER_SHORT_ID);
    Lwm2mAccessControl::create(*eth1, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

#ifdef OBJ_W_10515_GPIO_CONTROL
void gpioControlInit(WppClient &client) {
    client.registry().registerObj(GpioControl::object(client));
    
    // Create instance for Status LED
    Instance *ledStatus = GpioControl::createInst(client, 0);
    ledStatus->set<STRING_T>(GpioControl::GPIO_NAME_0, "LED_STATUS");
    ledStatus->set<INT_T>(GpioControl::GPIO_TYPE_2, GpioControl::TYPE_LED);
    
    // Create instance for WLAN LED
    Instance *ledWlan = GpioControl::createInst(client, 1);
    ledWlan->set<STRING_T>(GpioControl::GPIO_NAME_0, "LED_WLAN");
    ledWlan->set<INT_T>(GpioControl::GPIO_TYPE_2, GpioControl::TYPE_LED);
    ledWlan->set<INT_T>(GpioControl::TRIGGER_MODE_6, GpioControl::TRIGGER_NETDEV);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(GpioControl::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(*ledStatus, TEST_SERVER_SHORT_ID);
    Lwm2mAccessControl::create(*ledWlan, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

#ifdef OBJ_W_10516_USB_MANAGEMENT
void usbManagementInit(WppClient &client) {
    client.registry().registerObj(UsbManagement::object(client));
    
    // Create instance for USB 2.0 Type-A port
    Instance *usb1 = UsbManagement::createInst(client, 0);
    usb1->set<STRING_T>(UsbManagement::PORT_NAME_0, "USB1");
    usb1->set<INT_T>(UsbManagement::PORT_TYPE_1, UsbManagement::USB_2_0_TYPE_A);
    
    // Create instance for USB-C port
    Instance *usb2 = UsbManagement::createInst(client, 1);
    usb2->set<STRING_T>(UsbManagement::PORT_NAME_0, "USB2");
    usb2->set<INT_T>(UsbManagement::PORT_TYPE_1, UsbManagement::USB_TYPE_C);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(UsbManagement::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(*usb1, TEST_SERVER_SHORT_ID);
    Lwm2mAccessControl::create(*usb2, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

#ifdef OBJ_W_10517_STORAGE_MANAGEMENT
void storageManagementInit(WppClient &client) {
    client.registry().registerObj(StorageManagement::object(client));
    
    // Create instance for NAND flash
    Instance *nand = StorageManagement::createInst(client, 0);
    nand->set<STRING_T>(StorageManagement::STORAGE_NAME_0, "NAND");
    nand->set<INT_T>(StorageManagement::STORAGE_TYPE_1, StorageManagement::STORAGE_NAND);
    nand->set<STRING_T>(StorageManagement::MOUNT_POINT_2, "/");
    nand->set<BOOL_T>(StorageManagement::IS_BOOTABLE_9, true);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(StorageManagement::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(*nand, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

#ifdef OBJ_W_10518_SYSTEM_MONITOR
void systemMonitorInit(WppClient &client) {
    client.registry().registerObj(SystemMonitor::object(client));
    Instance &sysMon = SystemMonitor::createInst(client);

    // Default values are set in initResources
    // These would be updated periodically from /proc in real implementation

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(SystemMonitor::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(sysMon, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

#ifdef OBJ_W_10521_FIREWALL_CONFIG
void firewallConfigInit(WppClient &client) {
    client.registry().registerObj(FirewallConfig::object(client));

    // Create default firewall rules as examples
    // Rule 0: Allow SSH from WAN to LAN
    Instance *rule0 = FirewallConfig::createInst(client, 0);
    rule0->set<STRING_T>(FirewallConfig::RULE_NAME_0, "allow_ssh");
    rule0->set<BOOL_T>(FirewallConfig::ENABLED_2, true);
    rule0->set<INT_T>(FirewallConfig::ACTION_3, FirewallConfig::ACTION_ACCEPT);
    rule0->set<INT_T>(FirewallConfig::PROTOCOL_4, FirewallConfig::PROTOCOL_TCP);
    rule0->set<INT_T>(FirewallConfig::DEST_PORT_8, 22);
    rule0->set<STRING_T>(FirewallConfig::ZONE_FROM_11, "wan");
    rule0->set<STRING_T>(FirewallConfig::ZONE_TO_12, "lan");
    rule0->set<STRING_T>(FirewallConfig::COMMENT_13, "Allow SSH access");

    // Rule 1: Allow HTTP/HTTPS from WAN to LAN
    Instance *rule1 = FirewallConfig::createInst(client, 1);
    rule1->set<STRING_T>(FirewallConfig::RULE_NAME_0, "allow_web");
    rule1->set<BOOL_T>(FirewallConfig::ENABLED_2, true);
    rule1->set<INT_T>(FirewallConfig::ACTION_3, FirewallConfig::ACTION_ACCEPT);
    rule1->set<INT_T>(FirewallConfig::PROTOCOL_4, FirewallConfig::PROTOCOL_TCP);
    rule1->set<STRING_T>(FirewallConfig::ZONE_FROM_11, "wan");
    rule1->set<STRING_T>(FirewallConfig::ZONE_TO_12, "lan");
    rule1->set<STRING_T>(FirewallConfig::COMMENT_13, "Allow web traffic");

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(FirewallConfig::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(*rule0, TEST_SERVER_SHORT_ID);
    Lwm2mAccessControl::create(*rule1, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

#ifdef OBJ_W_10522_POE_MANAGEMENT
void poeManagementInit(WppClient &client) {
    client.registry().registerObj(PoeManagement::object(client));
    Instance &poe = PoeManagement::createInst(client);

    // Default values are set in initResources
    // PoE support will be auto-detected and status updated periodically

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(PoeManagement::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    Lwm2mAccessControl::create(poe, TEST_SERVER_SHORT_ID);
    #endif
}
#endif

#ifdef OBJ_W_10525_WAN_FAILOVER_POLICY
void wanFailoverPolicyInit(WppClient &client) {
    cout << "DEBUG: Starting WanFailoverPolicy init..." << endl;

    cout << "DEBUG: About to call WanFailoverPolicy::object()..." << endl;
    Object& obj = WanFailoverPolicy::object(client);
    cout << "DEBUG: WanFailoverPolicy::object() returned successfully" << endl;

    cout << "DEBUG: About to register object..." << endl;
    client.registry().registerObj(obj);
    cout << "DEBUG: Object registered successfully" << endl;

    cout << "DEBUG: About to call createInst()..." << endl;
    Instance *inst = WanFailoverPolicy::createInst(client);
    cout << "DEBUG: createInst() returned: " << (inst ? "NON-NULL" : "NULL") << endl;

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        cout << "DEBUG: Creating object-level AC..." << endl;
        Lwm2mAccessControl::create(WanFailoverPolicy::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        cout << "DEBUG: Creating instance-level AC..." << endl;
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
        cout << "DEBUG: AC creation complete" << endl;
    } else {
        cout << "DEBUG: Skipping AC creation (inst is NULL)" << endl;
    }
    #endif

    cout << "DEBUG: WanFailoverPolicy init complete" << endl;
}
#endif

#ifdef OBJ_W_10526_MULTIWAN_HEALTH_CHECK
void multiwanHealthCheckInit(WppClient &client) {
    client.registry().registerObj(MultiWanHealthCheck::object(client));
    Instance *inst = MultiWanHealthCheck::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(MultiWanHealthCheck::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10527_WIFI_CLIENT_MANAGEMENT
void wifiClientManagementInit(WppClient &client) {
    client.registry().registerObj(WifiClientManagement::object(client));
    Instance *inst = WifiClientManagement::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(WifiClientManagement::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10528_WIFI_CHANNEL_OPTIMIZATION
void wifiChannelOptimizationInit(WppClient &client) {
    client.registry().registerObj(WifiChannelOptimization::object(client));
    Instance *inst = WifiChannelOptimization::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(WifiChannelOptimization::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10529_MATTER_BRIDGE
void matterBridgeInit(WppClient &client) {
    client.registry().registerObj(MatterBridge::object(client));
    Instance *inst = MatterBridge::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(MatterBridge::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10530_MATTER_DEVICE
void matterDeviceInit(WppClient &client) {
    client.registry().registerObj(MatterDevice::object(client));
    Instance *inst = MatterDevice::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(MatterDevice::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10531_THREAD_NETWORK
void threadNetworkInit(WppClient &client) {
    client.registry().registerObj(ThreadNetwork::object(client));
    Instance *inst = ThreadNetwork::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(ThreadNetwork::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10532_ZIGBEE_COORDINATOR
void zigbeeCoordinatorInit(WppClient &client) {
    client.registry().registerObj(ZigbeeCoordinator::object(client));
    Instance *inst = ZigbeeCoordinator::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(ZigbeeCoordinator::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10533_ZIGBEE_DEVICE
void zigbeeDeviceInit(WppClient &client) {
    client.registry().registerObj(ZigbeeDevice::object(client));
    Instance *inst = ZigbeeDevice::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(ZigbeeDevice::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10534_ZIGBEE_GROUP
void zigbeeGroupInit(WppClient &client) {
    client.registry().registerObj(ZigbeeGroup::object(client));
    Instance *inst = ZigbeeGroup::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(ZigbeeGroup::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10535_LAN_CONFIGURATION
void lanConfigurationInit(WppClient &client) {
    client.registry().registerObj(LanConfiguration::object(client));
    Instance *inst = LanConfiguration::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(LanConfiguration::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10536_ROUTING_TABLE
void routingTableInit(WppClient &client) {
    client.registry().registerObj(RoutingTable::object(client));
    Instance *inst = RoutingTable::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(RoutingTable::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

#ifdef OBJ_W_10537_VPN_CONFIGURATION
void vpnConfigurationInit(WppClient &client) {
    client.registry().registerObj(VpnConfiguration::object(client));
    Instance *inst = VpnConfiguration::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    if (inst) {
        Lwm2mAccessControl::create(VpnConfiguration::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
        Lwm2mAccessControl::create(*inst, TEST_SERVER_SHORT_ID);
    }
    #endif
}
#endif

/* ---------- Walt Technologies custom objects init end ---------- */
