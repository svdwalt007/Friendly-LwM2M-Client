#include "WppRegistry.h"
#include "WppLogs.h"
#include "liblwm2m.h"

namespace wpp {

WppRegistry::WppRegistry(lwm2m_context_t &context): _context(context) {
    WPP_LOGD(TAG_WPP_REG, "Creating registry instance");
	
	/* ---------- Mandatory objects init block begin ---------- */
	#ifdef OBJ_M_3_DEVICE
	_objects.push_back(new ObjectImpl<Device>(_context, DEVICE_OBJ_INFO));
	#endif
	#ifdef OBJ_M_1_LWM2M_SERVER
	_objects.push_back(new ObjectImpl<Lwm2mServer>(_context, LWM2M_SERVER_OBJ_INFO));
	#endif
	#ifdef OBJ_M_0_LWM2M_SECURITY
	_objects.push_back(new ObjectImpl<Lwm2mSecurity>(_context, LWM2M_SECURITY_OBJ_INFO));
	#endif
	/* ---------- Mandatory objects init block end ---------- */

	/* ---------- Optional objects init block begin ---------- */
	#ifdef OBJ_O_4_CONNECTIVITY_MONITORING
	_objects.push_back(new ObjectImpl<ConnectivityMonitoring>(_context, CONNECTIVITY_MONITORING_OBJ_INFO));
	#endif
	#ifdef OBJ_O_6_LOCATION
	_objects.push_back(new ObjectImpl<Location>(_context, LOCATION_OBJ_INFO));
	#endif
	#ifdef OBJ_O_12_WLAN_CONNECTIVITY
	_objects.push_back(new ObjectImpl<WlanConnectivity>(_context, WLAN_CONNECTIVITY_META_INFO));
	#endif
	#ifdef OBJ_O_13_BEARER_SELECTION
	_objects.push_back(new ObjectImpl<BearerSelection>(_context, BEARER_SELECTION_META_INFO));
	#endif
	#ifdef OBJ_O_2_LWM2M_ACCESS_CONTROL
	_objects.push_back(new ObjectImpl<Lwm2mAccessControl>(_context, LWM2M_ACCESS_CONTROL_OBJ_INFO));
	#endif
	#if OBJ_O_3339_AUDIO_CLIP
	_objects.push_back(new ObjectImpl<AudioClip>(_context, AUDIO_CLIP_OBJ_INFO));
	#endif
	#if OBJ_O_5_FIRMWARE_UPDATE
	_objects.push_back(new ObjectImpl<FirmwareUpdate>(_context, FIRMWARE_UPDATE_OBJ_INFO));
	#endif
	#if OBJ_O_10519_HARDWARE_WATCHDOG
	_objects.push_back(new ObjectImpl<HardwareWatchdog>(_context, HARDWARE_WATCHDOG_OBJ_INFO));
	#endif
	/* ---------- Optional objects init block end ---------- */

	/* ---------- Walt Technologies objects init block begin ---------- */
	#ifdef OBJ_O_10512_STARLINK_TERMINAL
	_objects.push_back(new ObjectImpl<StarlinkTerminal>(_context, STARLINK_TERMINAL_OBJ_INFO));
	#endif
	#ifdef OBJ_O_10520_MIKROBUS
	_objects.push_back(new ObjectImpl<Mikrobus>(_context, MIKROBUS_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10513_ROUTER_MANAGEMENT
	_objects.push_back(new ObjectImpl<RouterManagement>(_context, ROUTER_MANAGEMENT_META_INFO));
	#endif
	#ifdef OBJ_W_10514_ETHERNET_INTERFACE
	_objects.push_back(new ObjectImpl<EthernetInterface>(_context, ETHERNET_INTERFACE_META_INFO));
	#endif
	#ifdef OBJ_W_10515_GPIO_CONTROL
	_objects.push_back(new ObjectImpl<GpioControl>(_context, GPIO_CONTROL_META_INFO));
	#endif
	#ifdef OBJ_W_10516_USB_MANAGEMENT
	_objects.push_back(new ObjectImpl<UsbManagement>(_context, USB_MANAGEMENT_META_INFO));
	#endif
	#ifdef OBJ_W_10517_STORAGE_MANAGEMENT
	_objects.push_back(new ObjectImpl<StorageManagement>(_context, STORAGE_MANAGEMENT_META_INFO));
	#endif
	#ifdef OBJ_W_10518_SYSTEM_MONITOR
	_objects.push_back(new ObjectImpl<SystemMonitor>(_context, SYSTEM_MONITOR_META_INFO));
	#endif
	#ifdef OBJ_W_10521_FIREWALL_CONFIG
	_objects.push_back(new ObjectImpl<FirewallConfig>(_context, FIREWALL_CONFIG_META_INFO));
	#endif
	#ifdef OBJ_W_10522_POE_MANAGEMENT
	_objects.push_back(new ObjectImpl<PoeManagement>(_context, POE_MANAGEMENT_META_INFO));
	#endif
	#ifdef OBJ_W_10525_WAN_FAILOVER_POLICY
	_objects.push_back(new ObjectImpl<WanFailoverPolicy>(_context, WAN_FAILOVER_POLICY_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10526_MULTIWAN_HEALTH_CHECK
	_objects.push_back(new ObjectImpl<MultiWanHealthCheck>(_context, MULTIWAN_HEALTH_CHECK_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10527_WIFI_CLIENT_MANAGEMENT
	_objects.push_back(new ObjectImpl<WifiClientManagement>(_context, WIFI_CLIENT_MANAGEMENT_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10528_WIFI_CHANNEL_OPTIMIZATION
	_objects.push_back(new ObjectImpl<WifiChannelOptimization>(_context, WIFI_CHANNEL_OPTIMIZATION_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10529_MATTER_BRIDGE
	_objects.push_back(new ObjectImpl<MatterBridge>(_context, MATTER_BRIDGE_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10530_MATTER_DEVICE
	_objects.push_back(new ObjectImpl<MatterDevice>(_context, MATTER_DEVICE_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10531_THREAD_NETWORK
	_objects.push_back(new ObjectImpl<ThreadNetwork>(_context, THREAD_NETWORK_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10532_ZIGBEE_COORDINATOR
	_objects.push_back(new ObjectImpl<ZigbeeCoordinator>(_context, ZIGBEE_COORDINATOR_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10533_ZIGBEE_DEVICE
	_objects.push_back(new ObjectImpl<ZigbeeDevice>(_context, ZIGBEE_DEVICE_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10534_ZIGBEE_GROUP
	_objects.push_back(new ObjectImpl<ZigbeeGroup>(_context, ZIGBEE_GROUP_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10535_LAN_CONFIGURATION
	_objects.push_back(new ObjectImpl<LanConfiguration>(_context, LAN_CONFIGURATION_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10536_ROUTING_TABLE
	_objects.push_back(new ObjectImpl<RoutingTable>(_context, ROUTING_TABLE_OBJ_INFO));
	#endif
	#ifdef OBJ_W_10537_VPN_CONFIGURATION
	_objects.push_back(new ObjectImpl<VpnConfiguration>(_context, VPN_CONFIGURATION_OBJ_INFO));
	#endif
	/* ---------- Walt Technologies objects init block end ---------- */
}

WppRegistry::~WppRegistry() {
	for (auto obj : _objects) {
		delete obj;
	}
	_objects.clear();
}

bool WppRegistry::registerObj(Object &object) {
	WPP_LOGD(TAG_WPP_CLIENT, "Register object with id: %d", object.getObjectID());
	return !lwm2m_add_object(&_context, &object.getLwm2mObject());
}

bool WppRegistry::deregisterObj(Object &object) {
	WPP_LOGD(TAG_WPP_CLIENT, "Deregister object with id: %d", object.getObjectID());
	return !lwm2m_remove_object(&_context, object.getLwm2mObject().objID);
}

bool WppRegistry::isObjRegistered(Object &object) {
	lwm2m_object_t * lwm2m_object = (lwm2m_object_t *)LWM2M_LIST_FIND(_context.objectList, object.getLwm2mObject().objID);
	return lwm2m_object != NULL;
}

bool WppRegistry::isExist(OBJ_ID objId) {
	return object(objId) != NULL;
}

Object * WppRegistry::object(OBJ_ID objId) {
	auto finder = [objId](const Object *obj) -> bool { return obj->getObjectID() == objId; };
	auto objIter = std::find_if(_objects.begin(), _objects.end(), finder);
	return objIter != _objects.end()? *objIter : NULL;
}

std::vector<Object *> & WppRegistry::objects() {
	return _objects;
}

/* ---------- Mandatory objects method block begin ---------- */
#ifdef OBJ_M_3_DEVICE
Object & WppRegistry::device() {
	return *object(OBJ_ID::DEVICE);
}
#endif
#ifdef OBJ_M_1_LWM2M_SERVER
Object & WppRegistry::lwm2mServer() {
	return *object(OBJ_ID::LWM2M_SERVER);
}
#endif
#ifdef OBJ_M_0_LWM2M_SECURITY
Object & WppRegistry::lwm2mSecurity() {
	return *object(OBJ_ID::LWM2M_SECURITY);
}
#endif
/* ---------- Mandatory objects method block end ---------- */

/* ---------- Optional objects method block begin ---------- */
#ifdef OBJ_O_4_CONNECTIVITY_MONITORING
Object & WppRegistry::connectivityMonitoring() {
	return *object(OBJ_ID::CONNECTIVITY_MONITORING);
}
#endif
#ifdef OBJ_O_6_LOCATION
Object & WppRegistry::location() {
	return *object(OBJ_ID::LOCATION);
}
#endif
#ifdef OBJ_O_12_WLAN_CONNECTIVITY
Object & WppRegistry::wlanConnectivity() {
	return *object(OBJ_ID::WLAN_CONNECTIVITY);
}
#endif
#ifdef OBJ_O_13_BEARER_SELECTION
Object & WppRegistry::bearerSelection() {
	return *object(OBJ_ID::BEARER_SELECTION);
}
#endif
#ifdef OBJ_O_2_LWM2M_ACCESS_CONTROL
Object & WppRegistry::lwm2mAccessControl() {
	return *object(OBJ_ID::LWM2M_ACCESS_CONTROL);
}
#endif
#if OBJ_O_3339_AUDIO_CLIP
Object & WppRegistry::audioClip() {
	return *object(OBJ_ID::AUDIO_CLIP);
}
#endif
#if OBJ_O_5_FIRMWARE_UPDATE
Object & WppRegistry::firmwareUpdate() {
	return *object(OBJ_ID::FIRMWARE_UPDATE);
}
#endif
#if OBJ_O_10519_HARDWARE_WATCHDOG
Object & WppRegistry::hardwareWatchdog() {
	return *object(OBJ_ID::HARDWARE_WATCHDOG);
}
#endif
/* ---------- Optional objects method block end ---------- */

/* ---------- Walt Technologies objects method block begin ---------- */
#ifdef OBJ_O_10512_STARLINK_TERMINAL
Object & WppRegistry::starlinkTerminal() {
	return *object(OBJ_ID::STARLINK_TERMINAL);
}
#endif
#ifdef OBJ_O_10520_MIKROBUS
Object & WppRegistry::mikrobus() {
	return *object(OBJ_ID::MIKROBUS);
}
#endif
#ifdef OBJ_W_10513_ROUTER_MANAGEMENT
Object & WppRegistry::routerManagement() {
	return *object(ROUTER_MANAGEMENT_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10514_ETHERNET_INTERFACE
Object & WppRegistry::ethernetInterface() {
	return *object(ETHERNET_INTERFACE_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10515_GPIO_CONTROL
Object & WppRegistry::gpioControl() {
	return *object(GPIO_CONTROL_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10516_USB_MANAGEMENT
Object & WppRegistry::usbManagement() {
	return *object(USB_MANAGEMENT_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10517_STORAGE_MANAGEMENT
Object & WppRegistry::storageManagement() {
	return *object(STORAGE_MANAGEMENT_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10518_SYSTEM_MONITOR
Object & WppRegistry::systemMonitor() {
	return *object(SYSTEM_MONITOR_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10521_FIREWALL_CONFIG
Object & WppRegistry::firewallConfig() {
	return *object(FIREWALL_CONFIG_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10522_POE_MANAGEMENT
Object & WppRegistry::poeManagement() {
	return *object(POE_MANAGEMENT_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10525_WAN_FAILOVER_POLICY
Object & WppRegistry::wanFailoverPolicy() {
	return *object(WAN_FAILOVER_POLICY_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10526_MULTIWAN_HEALTH_CHECK
Object & WppRegistry::multiWanHealthCheck() {
	return *object(MULTIWAN_HEALTH_CHECK_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10527_WIFI_CLIENT_MANAGEMENT
Object & WppRegistry::wifiClientManagement() {
	return *object(WIFI_CLIENT_MANAGEMENT_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10528_WIFI_CHANNEL_OPTIMIZATION
Object & WppRegistry::wifiChannelOptimization() {
	return *object(WIFI_CHANNEL_OPTIMIZATION_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10529_MATTER_BRIDGE
Object & WppRegistry::matterBridge() {
	return *object(MATTER_BRIDGE_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10530_MATTER_DEVICE
Object & WppRegistry::matterDevice() {
	return *object(MATTER_DEVICE_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10531_THREAD_NETWORK
Object & WppRegistry::threadNetwork() {
	return *object(THREAD_NETWORK_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10532_ZIGBEE_COORDINATOR
Object & WppRegistry::zigbeeCoordinator() {
	return *object(ZIGBEE_COORDINATOR_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10533_ZIGBEE_DEVICE
Object & WppRegistry::zigbeeDevice() {
	return *object(ZIGBEE_DEVICE_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10534_ZIGBEE_GROUP
Object & WppRegistry::zigbeeGroup() {
	return *object(ZIGBEE_GROUP_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10535_LAN_CONFIGURATION
Object & WppRegistry::lanConfiguration() {
	return *object(LAN_CONFIG_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10536_ROUTING_TABLE
Object & WppRegistry::routingTable() {
	return *object(ROUTING_TABLE_OBJECT_ID);
}
#endif
#ifdef OBJ_W_10537_VPN_CONFIGURATION
Object & WppRegistry::vpnConfiguration() {
	return *object(VPN_CONFIG_OBJECT_ID);
}
#endif
/* ---------- Walt Technologies objects method block end ---------- */

} //wpp
