#include <iostream>
#include <thread>
#include <chrono>

#include "Connection.h"
#include "objects.h"
#include "cli_options.h"

using namespace std;
using namespace wpp;

void socketPolling(Connection *connection) {
	while (!isDeviceShouldBeRebooted()) {
		connection->loop();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void wppErrorHandler(WppClient &client, int errCode) {
	cout << "wppErrorHandler(): Error: " << errCode << endl;
	if (client.getState() == STATE_BOOTSTRAPPING || client.getState() == STATE_BOOTSTRAP_REQUIRED) {
		cout << "Trying to restore security and server objects" << endl;
		Object &securityObj = Lwm2mSecurity::object(client);
		Object &serverObj = Lwm2mServer::object(client);

		#if OBJ_O_2_LWM2M_ACCESS_CONTROL
		for (auto *inst : securityObj.instances()) Lwm2mAccessControl::remove(*inst);
		for (auto *inst : serverObj.instances()) Lwm2mAccessControl::remove(*inst);
		#endif

		securityObj.clear();
		serverObj.clear();

		securityInit(client);
		serverInit(client);
	}
}

// Found Wakaama bugs:
// TODO: Device work with NON confirmation messages

int main(int argc, char* argv[]) {
	// Parse command-line options
	CliOptions options;
	if (!parseCliOptions(argc, argv, options)) {
		return 1;
	}

	// Print configuration summary
	if (!options.quiet && options.verbosity > 0) {
		cout << "==================================================" << endl;
		cout << "  Friendly LwM2M Client Configuration" << endl;
		cout << "==================================================" << endl;
		cout << "Endpoint Name:    " << options.endpoint_name << endl;
		cout << "Server URI:       " << options.server_uri << endl;
		cout << "Local Port:       " << options.local_port << endl;
		cout << "Lifetime:         " << options.lifetime << " seconds" << endl;
		cout << "Address Family:   " << (options.address_family == AF_INET ? "IPv4" : "IPv6") << endl;
		cout << "Bootstrap Mode:   " << (options.use_bootstrap ? "Enabled" : "Disabled") << endl;
		cout << "Security Mode:    ";
		switch (options.security_mode) {
			case SecurityMode::NONE: cout << "None"; break;
			case SecurityMode::PSK:  cout << "PSK"; break;
			case SecurityMode::RPK:  cout << "RPK"; break;
			case SecurityMode::CERT: cout << "Certificate"; break;
		}
		cout << endl;
		cout << "==================================================" << endl;
	}

	cout << endl << "---- Creating required components ----" << endl;
	// CLI: Use local port and address family from command-line options
	Connection connection(options.local_port, options.address_family);

	// Client initialization
	cout << endl << "---- Creating WppClient ----" << endl;
	// CLI: Use endpoint name from command-line options
	string clientName = options.endpoint_name;
	cout << "WppClient name: " << clientName << endl;
	WppClient::create({clientName, "", ""}, connection, wppErrorHandler);
	WppClient *client = WppClient::takeOwnershipBlocking();

	// Initialize wpp objects
	#ifdef OBJ_O_2_LWM2M_ACCESS_CONTROL
	acInit(*client);
	#endif
	cout << endl << "---- Initialization wpp Server ----" << endl;
	// CLI: Pass options to serverInit
	serverInit(*client, options);
	cout << endl << "---- Initialization wpp Security ----" << endl;
	// CLI: Pass options to securityInit
	securityInit(*client, options);
	cout << endl << "---- Initialization wpp Device ----" << endl;
	deviceInit(*client);
	#ifdef OBJ_O_5_FIRMWARE_UPDATE
	cout << endl << "---- Initialization wpp FirmwareUpdate ----" << endl;
	fwUpdaterInit(*client);
	#endif
	#ifdef OBJ_O_4_CONNECTIVITY_MONITORING
	cout << endl << "---- Initialization wpp ConnectivityMonitoring ----" << endl;
	connMonitoringInit(*client);
	#endif
	#ifdef OBJ_O_6_LOCATION
	cout << endl << "---- Initialization wpp Location ----" << endl;
	locationInit(*client);
	#endif
	#ifdef OBJ_O_12_WLAN_CONNECTIVITY
	cout << endl << "---- Initialization wpp WLAN Connectivity ----" << endl;
	wlanConnectivityInit(*client);
	#endif
	#ifdef OBJ_O_13_BEARER_SELECTION
	cout << endl << "---- Initialization wpp Bearer Selection ----" << endl;
	bearerSelectionInit(*client);
	#endif
	#ifdef OBJ_O_3339_AUDIO_CLIP
	cout << endl << "---- Initialization wpp AudioClip ----" << endl;
	audioClipInit(*client);
	#endif
	#ifdef OBJ_O_10519_HARDWARE_WATCHDOG
	cout << endl << "---- Initialization wpp HardwareWatchdog ----" << endl;
	hardwareWatchdogInit(*client);
	#endif

	/* ---------- Walt Technologies objects initialization ---------- */
	#ifdef OBJ_O_10512_STARLINK_TERMINAL
	cout << endl << "---- Initialization Walt Technologies StarlinkTerminal ----" << endl;
	starlinkTerminalInit(*client);
	#endif
	#ifdef OBJ_O_10520_MIKROBUS
	cout << endl << "---- Initialization Walt Technologies MIKROBUS ----" << endl;
	mikrobusInit(*client);
	#endif
	#ifdef OBJ_W_10513_ROUTER_MANAGEMENT
	cout << endl << "---- Initialization wpp RouterManagement ----" << endl;
	routerManagementInit(*client);
	#endif
	#ifdef OBJ_W_10514_ETHERNET_INTERFACE
	cout << endl << "---- Initialization wpp EthernetInterface ----" << endl;
	ethernetInterfaceInit(*client);
	#endif
	#ifdef OBJ_W_10515_GPIO_CONTROL
	cout << endl << "---- Initialization wpp GpioControl ----" << endl;
	gpioControlInit(*client);
	#endif
	#ifdef OBJ_W_10516_USB_MANAGEMENT
	cout << endl << "---- Initialization wpp UsbManagement ----" << endl;
	usbManagementInit(*client);
	#endif
	#ifdef OBJ_W_10517_STORAGE_MANAGEMENT
	cout << endl << "---- Initialization wpp StorageManagement ----" << endl;
	storageManagementInit(*client);
	#endif
	#ifdef OBJ_W_10518_SYSTEM_MONITOR
	cout << endl << "---- Initialization wpp SystemMonitor ----" << endl;
	systemMonitorInit(*client);
	#endif
	#ifdef OBJ_W_10521_FIREWALL_CONFIG
	cout << endl << "---- Initialization wpp FirewallConfig ----" << endl;
	firewallConfigInit(*client);
	#endif
	#ifdef OBJ_W_10522_POE_MANAGEMENT
	cout << endl << "---- Initialization wpp PoeManagement ----" << endl;
	poeManagementInit(*client);
	#endif

	// Giving ownership to registry
	client->giveOwnership();

	// Add tasks with send operation
	#if defined(LWM2M_SUPPORT_SENML_JSON) && RES_1_23 && RES_3_13
	WppTaskQueue::addTask(5, [](WppClient &client, void *ctx) {
		WPP_LOGD(TAG_WPP_TASK, "Task: Send operation, sending current time to the server");
		DataLink dataLink = {{OBJ_ID::DEVICE, 0}, {Device::CURRENT_TIME_13,}};
		client.send(dataLink);
		return false;
	});
	#endif

	cout << endl << "---- Starting Connection thread ----" << endl;
	thread my_thread(socketPolling, &connection);

	time_t callTime = 0;
	for (int iterationCnt = 0; !isDeviceShouldBeRebooted(); iterationCnt++) {
		time_t currTime = time(NULL);

		cout << endl << "---- iteration:" << iterationCnt << ", time: " << time(NULL) << " ----" << endl;
		if (currTime >= callTime || connection.getPacketQueueSize()) {
			// Handle client state and process packets from the server
			client = WppClient::takeOwnership();
			if (client) { 
				callTime = currTime + client->loop();
				client->giveOwnership();
				cout << "Sleep time: " << callTime - time(NULL) << endl;
			}
		}
		this_thread::sleep_for(chrono::seconds(1));
	}

	cout << endl << "---- Closing example ----" << endl;
	my_thread.join();
	WppClient::remove();
	
	return 0;
}

