#include <iostream>
#include <thread>
#include <chrono>

#include "Connection.h"
#include "objects.h"

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

int main() {
	cout << endl << "---- Creating required components ----" << endl;
	Connection connection("56830", AF_INET);

	// Client initialization
	cout << endl << "---- Creating WppClient ----" << endl;
	string clientName = "walttech888"; // OpenWRT One endpoint name
	cout << "WppClient name: " << clientName << endl;
	WppClient::create({clientName, "", ""}, connection, wppErrorHandler);
	WppClient *client = WppClient::takeOwnershipBlocking();

	// Initialize wpp objects
	#ifdef OBJ_O_2_LWM2M_ACCESS_CONTROL
	acInit(*client);
	#endif
	cout << endl << "---- Initialization wpp Server ----" << endl;
	serverInit(*client);
	cout << endl << "---- Initialization wpp Security ----" << endl;
	securityInit(*client);
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
	#ifdef OBJ_O_3339_AUDIO_CLIP
	cout << endl << "---- Initialization wpp AudioClip ----" << endl;
	audioClipInit(*client);
	#endif

	/* ---------- Walt Technologies custom objects initialization ---------- */
	#ifdef OBJ_W_34601_ROUTER_MANAGEMENT
	cout << endl << "---- Initialization wpp RouterManagement ----" << endl;
	routerManagementInit(*client);
	#endif
	#ifdef OBJ_W_34602_ETHERNET_INTERFACE
	cout << endl << "---- Initialization wpp EthernetInterface ----" << endl;
	ethernetInterfaceInit(*client);
	#endif
	#ifdef OBJ_W_34603_GPIO_CONTROL
	cout << endl << "---- Initialization wpp GpioControl ----" << endl;
	gpioControlInit(*client);
	#endif
	#ifdef OBJ_W_34604_USB_MANAGEMENT
	cout << endl << "---- Initialization wpp UsbManagement ----" << endl;
	usbManagementInit(*client);
	#endif
	#ifdef OBJ_W_34605_STORAGE_MANAGEMENT
	cout << endl << "---- Initialization wpp StorageManagement ----" << endl;
	storageManagementInit(*client);
	#endif
	#ifdef OBJ_W_34606_SYSTEM_MONITOR
	cout << endl << "---- Initialization wpp SystemMonitor ----" << endl;
	systemMonitorInit(*client);
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

