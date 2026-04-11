/*
 * Regestry.h
 *
 *  Created on: 18 Jul 2023
 *      Author: valentin
 */

#ifndef WPP_REGISTRY_H_
#define WPP_REGISTRY_H_

#include <vector>
#include "ObjectImpl.h"

/* ---------- Mandatory objects include block begin ---------- */
#ifdef OBJ_M_3_DEVICE
#include "m_3_device/Device.h"
#endif
#ifdef OBJ_M_1_LWM2M_SERVER
#include "m_1_lwm2m_server/Lwm2mServer.h"
#endif
#ifdef OBJ_M_0_LWM2M_SECURITY
#include "m_0_lwm2m_security/Lwm2mSecurity.h"
#endif
/* ---------- Mandatory objects include block end ---------- */

/* ---------- Optional objects include block begin ---------- */
#ifdef OBJ_O_4_CONNECTIVITY_MONITORING
#include "o_4_connectivity_monitoring/ConnectivityMonitoring.h"
#endif
#ifdef OBJ_O_6_LOCATION
#include "o_6_location/Location.h"
#endif
#ifdef OBJ_O_12_WLAN_CONNECTIVITY
#include "o_12_wlan_connectivity/WlanConnectivity.h"
#endif
#ifdef OBJ_O_13_BEARER_SELECTION
#include "o_13_bearer_selection/BearerSelection.h"
#endif
#ifdef OBJ_O_2_LWM2M_ACCESS_CONTROL
#include "o_2_lwm2m_access_control/Lwm2mAccessControl.h"
#endif
#ifdef OBJ_O_3339_AUDIO_CLIP
#include "o_3339_audio_clip/AudioClip.h"
#endif
#ifdef OBJ_O_5_FIRMWARE_UPDATE
#include "o_5_firmware_update/FirmwareUpdate.h"
#endif
#ifdef OBJ_O_10519_HARDWARE_WATCHDOG
#include "o_10519_hardware_watchdog/HardwareWatchdog.h"
#endif
/* ---------- Optional objects include block end ---------- */

/* ---------- Walt Technologies objects include block begin ---------- */
#ifdef OBJ_O_10512_STARLINK_TERMINAL
#include "o_10512_starlink_terminal/StarlinkTerminal.h"
#endif
#ifdef OBJ_O_10520_MIKROBUS
#include "o_10520_mikrobus/Mikrobus.h"
#endif
#ifdef OBJ_W_10513_ROUTER_MANAGEMENT
#include "w_10513_router_management/RouterManagement.h"
#endif
#ifdef OBJ_W_10514_ETHERNET_INTERFACE
#include "w_10514_ethernet_interface/EthernetInterface.h"
#endif
#ifdef OBJ_W_10515_GPIO_CONTROL
#include "w_10515_gpio_control/GpioControl.h"
#endif
#ifdef OBJ_W_10516_USB_MANAGEMENT
#include "w_10516_usb_management/UsbManagement.h"
#endif
#ifdef OBJ_W_10517_STORAGE_MANAGEMENT
#include "w_10517_storage_management/StorageManagement.h"
#endif
#ifdef OBJ_W_10518_SYSTEM_MONITOR
#include "w_10518_system_monitor/SystemMonitor.h"
#endif
#ifdef OBJ_W_10521_FIREWALL_CONFIG
#include "w_10521_firewall_config/FirewallConfig.h"
#endif
#ifdef OBJ_W_10522_POE_MANAGEMENT
#include "w_10522_poe_management/PoeManagement.h"
#endif
/* ---------- Walt Technologies objects include block end ---------- */

namespace wpp {

// TODO: Split mandatory and optional registers
/**
 * @brief The WppRegistry class represents a registry for managing LWM2M objects.
 * 
 * This class provides functionality to register, deregister, and access LWM2M objects.
 * It also allows checking if an object is registered or exists in the registry.
 * The registry can contain both mandatory and optional objects.
 * 
 * @note This class is not copyable or movable.
 */
class WppRegistry {
public:
	explicit WppRegistry(lwm2m_context_t &context);
	~WppRegistry();

	WppRegistry(const WppRegistry&) = delete;
	WppRegistry(WppRegistry&&) = delete;
	WppRegistry& operator=(const WppRegistry&) = delete;
	WppRegistry& operator=(WppRegistry&&) = delete;

	/**
	 * @brief Registers an Object in the registry.
	 *
	 * @param object The Object to register.
	 * @return True if the registration is successful, false otherwise.
	 */
	bool registerObj(Object &object);

	/**
	 * @brief Deregisters an Object from the registry.
	 *
	 * @param object The Object to deregister.
	 * @return True if the deregistration is successful, false otherwise.
	 */
	bool deregisterObj(Object &object);

	/**
	 * @brief Checks if an Object is registered in the registry.
	 *
	 * @param object The Object to check.
	 * @return True if the Object is registered, false otherwise.
	 */
	bool isObjRegistered(Object &object);

	/**
	 * @brief Checks if an Object with the given objId exists in the registry.
	 *
	 * @param objId The ID of the Object to check.
	 * @return True if the Object exists, false otherwise.
	 */
	bool isExist(OBJ_ID objId);

	/**
	 * @brief Retrieves a pointer to the Object with the given objId.
	 *
	 * @param objId The ID of the Object to retrieve.
	 * @return A pointer to the Object if found, nullptr otherwise.
	 */
	Object * object(OBJ_ID objId);

	/**
	 * @brief Gets all registered objects.
	 *
	 * @return A vector of pointers to the registered objects.
	 */
	std::vector<Object *> & objects();

	/* ---------- Mandatory objects prototype block begin ---------- */
	#ifdef OBJ_M_3_DEVICE
	Object & device();
	#endif
	#ifdef OBJ_M_1_LWM2M_SERVER
	Object & lwm2mServer();
	#endif
	#ifdef OBJ_M_0_LWM2M_SECURITY
	Object & lwm2mSecurity();
	#endif
	/* ---------- Mandatory objects prototype block end ---------- */

	/* ---------- Optional objects prototype block begin ---------- */
	#ifdef OBJ_O_4_CONNECTIVITY_MONITORING
	Object & connectivityMonitoring();
	#endif
	#ifdef OBJ_O_6_LOCATION
	Object & location();
	#endif
	#ifdef OBJ_O_12_WLAN_CONNECTIVITY
	Object & wlanConnectivity();
	#endif
	#ifdef OBJ_O_13_BEARER_SELECTION
	Object & bearerSelection();
	#endif
	#ifdef OBJ_O_2_LWM2M_ACCESS_CONTROL
	Object & lwm2mAccessControl();
	#endif
	#ifdef OBJ_O_3339_AUDIO_CLIP
	Object & audioClip();
	#endif
	#ifdef OBJ_O_5_FIRMWARE_UPDATE
	Object & firmwareUpdate();
	#endif
	#ifdef OBJ_O_10519_HARDWARE_WATCHDOG
	Object & hardwareWatchdog();
	#endif
	/* ---------- Optional objects prototype block end ---------- */

	/* ---------- Walt Technologies objects prototype block begin ---------- */
	#ifdef OBJ_O_10512_STARLINK_TERMINAL
	Object & starlinkTerminal();
	#endif
	#ifdef OBJ_O_10520_MIKROBUS
	Object & mikrobus();
	#endif
	#ifdef OBJ_W_10513_ROUTER_MANAGEMENT
	Object & routerManagement();
	#endif
	#ifdef OBJ_W_10514_ETHERNET_INTERFACE
	Object & ethernetInterface();
	#endif
	#ifdef OBJ_W_10515_GPIO_CONTROL
	Object & gpioControl();
	#endif
	#ifdef OBJ_W_10516_USB_MANAGEMENT
	Object & usbManagement();
	#endif
	#ifdef OBJ_W_10517_STORAGE_MANAGEMENT
	Object & storageManagement();
	#endif
	#ifdef OBJ_W_10518_SYSTEM_MONITOR
	Object & systemMonitor();
	#endif
	#ifdef OBJ_W_10521_FIREWALL_CONFIG
	Object & firewallConfig();
	#endif
	#ifdef OBJ_W_10522_POE_MANAGEMENT
	Object & poeManagement();
	#endif
	/* ---------- Walt Technologies objects prototype block end ---------- */

private:
	lwm2m_context_t &_context;
	std::vector<Object *> _objects;
};

} // namespace wpp

#endif // WPP_REGISTRY_H_
