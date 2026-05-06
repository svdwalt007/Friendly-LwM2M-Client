#ifndef USER_OBJECTS_H
#define USER_OBJECTS_H

#include "WppClient.h"
#if OBJ_O_5_FIRMWARE_UPDATE
#ifdef USE_OPENWRT_FW_UPDATER
#include "OpenWrtFwInit.h"
#else
#include "FwUriDownloader.h"
#include "FwAutoDownloader.h"
#include "FirmwareUpdater.h"
#endif
#endif

using namespace wpp;

// Forward declaration for CLI options
struct CliOptions;

#define TEST_SERVER_SHORT_ID 123

/* ------------- Methods to init objects ------------- */
void serverInit(WppClient &client, const CliOptions& options);

void securityInit(WppClient &client, const CliOptions& options);

void deviceInit(WppClient &client, const CliOptions& options);

#ifdef OBJ_O_5_FIRMWARE_UPDATE
void fwUpdaterInit(WppClient &client);
#endif

#ifdef OBJ_O_2_LWM2M_ACCESS_CONTROL
void acInit(WppClient &client);
#endif

#ifdef OBJ_O_4_CONNECTIVITY_MONITORING
void connMonitoringInit(WppClient &client);
#endif

#ifdef OBJ_O_6_LOCATION
void locationInit(WppClient &client);
#endif

#ifdef OBJ_O_12_WLAN_CONNECTIVITY
void wlanConnectivityInit(WppClient &client);
#endif

#ifdef OBJ_O_13_BEARER_SELECTION
void bearerSelectionInit(WppClient &client);
#endif

#ifdef OBJ_O_3339_AUDIO_CLIP
void audioClipInit(WppClient &client);
#endif

#ifdef OBJ_O_10519_HARDWARE_WATCHDOG
void hardwareWatchdogInit(WppClient &client);
#endif

/* ---------- Walt Technologies objects init declarations ---------- */
#ifdef OBJ_O_10512_STARLINK_TERMINAL
void starlinkTerminalInit(WppClient &client);
#endif

#ifdef OBJ_O_10520_MIKROBUS
void mikrobusInit(WppClient &client);
#endif

#ifdef OBJ_W_10513_ROUTER_MANAGEMENT
void routerManagementInit(WppClient &client);
#endif

#ifdef OBJ_W_10514_ETHERNET_INTERFACE
void ethernetInterfaceInit(WppClient &client);
#endif

#ifdef OBJ_W_10515_GPIO_CONTROL
void gpioControlInit(WppClient &client);
#endif

#ifdef OBJ_W_10516_USB_MANAGEMENT
void usbManagementInit(WppClient &client);
#endif

#ifdef OBJ_W_10517_STORAGE_MANAGEMENT
void storageManagementInit(WppClient &client);
#endif

#ifdef OBJ_W_10518_SYSTEM_MONITOR
void systemMonitorInit(WppClient &client);
#endif

#ifdef OBJ_W_10521_FIREWALL_CONFIG
void firewallConfigInit(WppClient &client);
#endif

#ifdef OBJ_W_10522_POE_MANAGEMENT
void poeManagementInit(WppClient &client);
#endif

#ifdef OBJ_W_10525_WAN_FAILOVER_POLICY
void wanFailoverPolicyInit(WppClient &client);
#endif

#ifdef OBJ_W_10526_MULTIWAN_HEALTH_CHECK
void multiwanHealthCheckInit(WppClient &client);
#endif

#ifdef OBJ_W_10527_WIFI_CLIENT_MANAGEMENT
void wifiClientManagementInit(WppClient &client);
#endif

#ifdef OBJ_W_10528_WIFI_CHANNEL_OPTIMIZATION
void wifiChannelOptimizationInit(WppClient &client);
#endif

#ifdef OBJ_W_10529_MATTER_BRIDGE
void matterBridgeInit(WppClient &client);
#endif

#ifdef OBJ_W_10530_MATTER_DEVICE
void matterDeviceInit(WppClient &client);
#endif

#ifdef OBJ_W_10531_THREAD_NETWORK
void threadNetworkInit(WppClient &client);
#endif

#ifdef OBJ_W_10532_ZIGBEE_COORDINATOR
void zigbeeCoordinatorInit(WppClient &client);
#endif

#ifdef OBJ_W_10533_ZIGBEE_DEVICE
void zigbeeDeviceInit(WppClient &client);
#endif

#ifdef OBJ_W_10534_ZIGBEE_GROUP
void zigbeeGroupInit(WppClient &client);
#endif

#ifdef OBJ_W_10535_LAN_CONFIGURATION
void lanConfigurationInit(WppClient &client);
#endif

#ifdef OBJ_W_10536_ROUTING_TABLE
void routingTableInit(WppClient &client);
#endif

#ifdef OBJ_W_10537_VPN_CONFIGURATION
void vpnConfigurationInit(WppClient &client);
#endif

/* ------------- Helpful methods ------------- */
bool isDeviceShouldBeRebooted();

#endif //USER_OBJECTS_H