#ifndef USER_OBJECTS_H
#define USER_OBJECTS_H

#include "WppClient.h"
#if OBJ_O_5_FIRMWARE_UPDATE
#include "FwUriDownloader.h"
#include "FwAutoDownloader.h"
#include "FirmwareUpdater.h"
#endif

using namespace wpp;

#define TEST_SERVER_SHORT_ID 123

/* ------------- Methods to init objects ------------- */
void serverInit(WppClient &client);

void securityInit(WppClient &client);

void deviceInit(WppClient &client);

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

#ifdef OBJ_O_3339_AUDIO_CLIP
void audioClipInit(WppClient &client);
#endif

#ifdef OBJ_O_34607_HARDWARE_WATCHDOG
void hardwareWatchdogInit(WppClient &client);
#endif

/* ---------- Walt Technologies objects init declarations ---------- */
#ifdef OBJ_O_34600_STARLINK_TERMINAL
void starlinkTerminalInit(WppClient &client);
#endif

#ifdef OBJ_O_34608_MIKROBUS
void mikrobusInit(WppClient &client);
#endif

#ifdef OBJ_W_34601_ROUTER_MANAGEMENT
void routerManagementInit(WppClient &client);
#endif

#ifdef OBJ_W_34602_ETHERNET_INTERFACE
void ethernetInterfaceInit(WppClient &client);
#endif

#ifdef OBJ_W_34603_GPIO_CONTROL
void gpioControlInit(WppClient &client);
#endif

#ifdef OBJ_W_34604_USB_MANAGEMENT
void usbManagementInit(WppClient &client);
#endif

#ifdef OBJ_W_34605_STORAGE_MANAGEMENT
void storageManagementInit(WppClient &client);
#endif

#ifdef OBJ_W_34606_SYSTEM_MONITOR
void systemMonitorInit(WppClient &client);
#endif

/* ------------- Helpful methods ------------- */
bool isDeviceShouldBeRebooted();

#endif //USER_OBJECTS_H