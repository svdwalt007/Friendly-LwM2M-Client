#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

// Include appropriate connection header based on DTLS provider
#ifdef WITH_TINYDTLS_PROVIDER
#include "Connection.h"
#else
#include "Connection_new.h"
#endif

#include "objects.h"
#include "cli_options.h"
#include "version_info.h"
#include "ConfigLoader.h"
#include "WppRegistry.h"

// P1-8: Ring-buffer logging system for observability
#ifdef __linux__
#include "log_ring.h"
#endif

using namespace std;
using namespace wpp;

// Global shutdown flag
static atomic<bool> g_shutdown_requested(false);

void socketPolling(Connection *connection) {
	while (!isDeviceShouldBeRebooted() && !g_shutdown_requested.load()) {
		connection->loop();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

// Signal handler for graceful shutdown
void signalHandler(int signum) {
	if (signum == SIGINT) {
		cout << endl << "[SIGNAL] Received SIGINT, shutting down gracefully..." << endl;
		g_shutdown_requested.store(true);

#ifdef __linux__
		// Log shutdown event to ring buffer
		ftl::LogRing& log = ftl::get_log_ring();
		uint8_t currentState = log.get_current_state();
		log.log_transition(currentState, currentState, ftl::TriggerEvent::SHUTDOWN_INITIATED, 0);
#endif
	}
}

#ifdef __linux__
// P1-8: SIGUSR1 handler for diagnostic snapshot
// This handler writes a JSON snapshot to /tmp/ftl-lwm2m-snapshot.json
// containing current state, introspection counters, and recent log entries.
//
// Usage: kill -SIGUSR1 <pid>
//
// The snapshot includes:
// - Current client state
// - Last 10 UPDATE timestamps
// - Current lifetime (lt)
// - Last 16 CoAP MIDs
// - Last 4 DTLS handshake outcomes
// - All introspection counters (transaction_count, pending_token_count, etc.)
// - Last 10 ring buffer entries
//
// This is async-signal-safe (uses only write() and pre-generated snapshot)
volatile sig_atomic_t g_sigusr1_pending = 0;

void sigusr1Handler(int signum) {
	if (signum == SIGUSR1) {
		// Set flag for main loop to handle (async-signal-safe)
		g_sigusr1_pending = 1;
	}
}

// Handle SIGUSR1 in main loop context (not signal context)
// This allows us to safely access WppClient and generate a full snapshot
void handlePendingSigusr1() {
	if (!g_sigusr1_pending) {
		return;
	}
	g_sigusr1_pending = 0;

	cout << endl << "[SIGNAL] Received SIGUSR1, writing diagnostic snapshot..." << endl;

	ftl::LogRing& log = ftl::get_log_ring();

	// Log the SIGUSR1 event
	uint8_t currentState = log.get_current_state();
	log.log_transition(currentState, currentState, ftl::TriggerEvent::SIGUSR1_RECEIVED, 0);

	// Update counters from WppClient if available
	WppClient* client = WppClient::takeOwnership();
	if (client) {
		// Update introspection counters from live client state
		log.counters().transaction_count.store(
			static_cast<uint32_t>(client->getTransactionCount()),
			std::memory_order_relaxed);
		log.counters().pending_token_count.store(
			static_cast<uint32_t>(client->getPendingTokenCount()),
			std::memory_order_relaxed);
		log.counters().mid_alloc_count.store(
			client->getNextMid(),
			std::memory_order_relaxed);

		// Update current state
		log.set_current_state(static_cast<uint8_t>(client->getState()));

		client->giveOwnership();
	}

	// Write snapshot to file
	const char* snapshotPath = "/tmp/ftl-lwm2m-snapshot.json";
	if (log.snapshot_to_file(snapshotPath)) {
		cout << "[SIGNAL] Snapshot written to " << snapshotPath << endl;
	} else {
		cerr << "[SIGNAL] ERROR: Failed to write snapshot to " << snapshotPath << endl;
	}
}
#endif

// Global options for use in error handler
static CliOptions *g_options = nullptr;

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

		if (g_options) {
			securityInit(client, *g_options);
			serverInit(client, *g_options);
		}
	}
}

// Found Wakaama bugs:
// TODO: Device work with NON confirmation messages

int main(int argc, char* argv[]) {
	// Install signal handler for graceful shutdown
	signal(SIGINT, signalHandler);

#ifdef __linux__
	// P1-8: Install SIGUSR1 handler for diagnostic snapshots
	signal(SIGUSR1, sigusr1Handler);
#endif

	// Parse command-line options
	CliOptions options;
	if (!parseCliOptions(argc, argv, options)) {
		return 1;
	}
	g_options = &options;  // Make options available to error handler

	// Display version banner
	if (!options.quiet) {
		cout << version::getVersionBanner() << endl;
	}

	// Print configuration summary
	if (!options.quiet && options.verbosity > 0) {
		cout << "==================================================" << endl;
		cout << "  Configuration" << endl;
		cout << "==================================================" << endl;
		cout << "Endpoint Name:    " << options.endpoint_name << endl;
		cout << "Serial Number:    " << options.serial_number << endl;
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
			default: cout << "Unknown"; break;
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
	// CLI: Pass options to deviceInit
	deviceInit(*client, options);
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
	#ifdef OBJ_W_10525_WAN_FAILOVER_POLICY
	cout << endl << "---- Initialization wpp WanFailoverPolicy ----" << endl;
	wanFailoverPolicyInit(*client);
	#endif
	#ifdef OBJ_W_10526_MULTIWAN_HEALTH_CHECK
	cout << endl << "---- Initialization wpp MultiWanHealthCheck ----" << endl;
	multiwanHealthCheckInit(*client);
	#endif
	#ifdef OBJ_W_10527_WIFI_CLIENT_MANAGEMENT
	cout << endl << "---- Initialization wpp WifiClientManagement ----" << endl;
	wifiClientManagementInit(*client);
	#endif
	#ifdef OBJ_W_10528_WIFI_CHANNEL_OPTIMIZATION
	cout << endl << "---- Initialization wpp WifiChannelOptimization ----" << endl;
	wifiChannelOptimizationInit(*client);
	#endif
	#ifdef OBJ_W_10529_MATTER_BRIDGE
	cout << endl << "---- Initialization wpp MatterBridge ----" << endl;
	matterBridgeInit(*client);
	#endif
	#ifdef OBJ_W_10530_MATTER_DEVICE
	cout << endl << "---- Initialization wpp MatterDevice ----" << endl;
	matterDeviceInit(*client);
	#endif
	#ifdef OBJ_W_10531_THREAD_NETWORK
	cout << endl << "---- Initialization wpp ThreadNetwork ----" << endl;
	threadNetworkInit(*client);
	#endif
	#ifdef OBJ_W_10532_ZIGBEE_COORDINATOR
	cout << endl << "---- Initialization wpp ZigbeeCoordinator ----" << endl;
	zigbeeCoordinatorInit(*client);
	#endif
	#ifdef OBJ_W_10533_ZIGBEE_DEVICE
	cout << endl << "---- Initialization wpp ZigbeeDevice ----" << endl;
	zigbeeDeviceInit(*client);
	#endif
	#ifdef OBJ_W_10534_ZIGBEE_GROUP
	cout << endl << "---- Initialization wpp ZigbeeGroup ----" << endl;
	zigbeeGroupInit(*client);
	#endif
	#ifdef OBJ_W_10535_LAN_CONFIGURATION
	cout << endl << "---- Initialization wpp LanConfiguration ----" << endl;
	lanConfigurationInit(*client);
	#endif
	#ifdef OBJ_W_10536_ROUTING_TABLE
	cout << endl << "---- Initialization wpp RoutingTable ----" << endl;
	routingTableInit(*client);
	#endif
	#ifdef OBJ_W_10537_VPN_CONFIGURATION
	cout << endl << "---- Initialization wpp VpnConfiguration ----" << endl;
	vpnConfigurationInit(*client);
	#endif

	// Load configuration from JSON file
	cout << endl << "---- Loading configuration from JSON ----" << endl;
	ConfigLoader configLoader;

	// Register all initialized objects
	configLoader.registerObject(0, &Lwm2mSecurity::object(*client));
	configLoader.registerObject(1, &Lwm2mServer::object(*client));
	configLoader.registerObject(3, &Device::object(*client));
	#ifdef OBJ_O_4_CONNECTIVITY_MONITORING
	configLoader.registerObject(4, &ConnectivityMonitoring::object(*client));
	#endif
	#ifdef OBJ_O_5_FIRMWARE_UPDATE
	configLoader.registerObject(5, &FirmwareUpdate::object(*client));
	#endif
	#ifdef OBJ_O_6_LOCATION
	configLoader.registerObject(6, &Location::object(*client));
	#endif
	#ifdef OBJ_O_12_WLAN_CONNECTIVITY
	configLoader.registerObject(12, &WlanConnectivity::object(*client));
	#endif
	#ifdef OBJ_O_13_BEARER_SELECTION
	configLoader.registerObject(13, &BearerSelection::object(*client));
	#endif
	#ifdef OBJ_O_3339_AUDIO_CLIP
	configLoader.registerObject(3339, &AudioClip::object(*client));
	#endif
	#ifdef OBJ_O_10512_STARLINK_TERMINAL
	configLoader.registerObject(10512, &StarlinkTerminal::object(*client));
	#endif
	#ifdef OBJ_O_10519_HARDWARE_WATCHDOG
	configLoader.registerObject(10519, &HardwareWatchdog::object(*client));
	#endif
	#ifdef OBJ_O_10520_MIKROBUS
	configLoader.registerObject(10520, &Mikrobus::object(*client));
	#endif
	#ifdef OBJ_W_10513_ROUTER_MANAGEMENT
	configLoader.registerObject(10513, &RouterManagement::object(*client));
	#endif
	#ifdef OBJ_W_10514_ETHERNET_INTERFACE
	configLoader.registerObject(10514, &EthernetInterface::object(*client));
	#endif
	#ifdef OBJ_W_10515_GPIO_CONTROL
	configLoader.registerObject(10515, &GpioControl::object(*client));
	#endif
	#ifdef OBJ_W_10516_USB_MANAGEMENT
	configLoader.registerObject(10516, &UsbManagement::object(*client));
	#endif
	#ifdef OBJ_W_10517_STORAGE_MANAGEMENT
	configLoader.registerObject(10517, &StorageManagement::object(*client));
	#endif
	#ifdef OBJ_W_10518_SYSTEM_MONITOR
	configLoader.registerObject(10518, &SystemMonitor::object(*client));
	#endif
	#ifdef OBJ_W_10521_FIREWALL_CONFIG
	configLoader.registerObject(10521, &FirewallConfig::object(*client));
	#endif
	#ifdef OBJ_W_10522_POE_MANAGEMENT
	configLoader.registerObject(10522, &PoeManagement::object(*client));
	#endif
	#ifdef OBJ_W_10525_WAN_FAILOVER_POLICY
	configLoader.registerObject(10525, &WanFailoverPolicy::object(*client));
	#endif
	#ifdef OBJ_W_10526_MULTIWAN_HEALTH_CHECK
	configLoader.registerObject(10526, &MultiWanHealthCheck::object(*client));
	#endif
	#ifdef OBJ_W_10527_WIFI_CLIENT_MANAGEMENT
	configLoader.registerObject(10527, &WifiClientManagement::object(*client));
	#endif
	#ifdef OBJ_W_10528_WIFI_CHANNEL_OPTIMIZATION
	configLoader.registerObject(10528, &WifiChannelOptimization::object(*client));
	#endif
	#ifdef OBJ_W_10529_MATTER_BRIDGE
	configLoader.registerObject(10529, &MatterBridge::object(*client));
	#endif
	#ifdef OBJ_W_10530_MATTER_DEVICE
	configLoader.registerObject(10530, &MatterDevice::object(*client));
	#endif
	#ifdef OBJ_W_10531_THREAD_NETWORK
	configLoader.registerObject(10531, &ThreadNetwork::object(*client));
	#endif
	#ifdef OBJ_W_10532_ZIGBEE_COORDINATOR
	configLoader.registerObject(10532, &ZigbeeCoordinator::object(*client));
	#endif
	#ifdef OBJ_W_10533_ZIGBEE_DEVICE
	configLoader.registerObject(10533, &ZigbeeDevice::object(*client));
	#endif
	#ifdef OBJ_W_10534_ZIGBEE_GROUP
	configLoader.registerObject(10534, &ZigbeeGroup::object(*client));
	#endif
	#ifdef OBJ_W_10535_LAN_CONFIGURATION
	configLoader.registerObject(10535, &LanConfiguration::object(*client));
	#endif
	#ifdef OBJ_W_10536_ROUTING_TABLE
	configLoader.registerObject(10536, &RoutingTable::object(*client));
	#endif
	#ifdef OBJ_W_10537_VPN_CONFIGURATION
	configLoader.registerObject(10537, &VpnConfiguration::object(*client));
	#endif

	// Load and apply configuration
	if (configLoader.loadFromFile("lwm2m_objects_config.json")) {
		cout << "Configuration file loaded successfully" << endl;
		if (configLoader.applyConfiguration()) {
			cout << "Configuration applied successfully" << endl;
		} else {
			cout << "Warning: Some configuration values could not be applied" << endl;
		}
	} else {
		cout << "Warning: Could not load configuration file, using defaults" << endl;
	}

	// Giving ownership to registry
	client->giveOwnership();

	// Add tasks with send operation
	#if defined(LWM2M_SUPPORT_SENML_JSON) && RES_1_23 && RES_3_13
	WppTaskQueue::addTask(5, [](WppClient &wppClient, void *ctx) {
		WPP_LOGD(TAG_WPP_TASK, "Task: Send operation, sending current time to the server");
		DataLink dataLink = {{OBJ_ID::DEVICE, 0}, {Device::CURRENT_TIME_13,}};
		wppClient.send(dataLink);
		return false;
	});
	#endif

	cout << endl << "---- Starting Connection thread ----" << endl;
	thread my_thread(socketPolling, &connection);

	time_t callTime = 0;
#ifdef __linux__
	// P1-8: Initialize log ring with lifetime from options
	ftl::LogRing& logRing = ftl::get_log_ring();
	logRing.set_lifetime(options.lifetime);
#endif

	for (int iterationCnt = 0; !isDeviceShouldBeRebooted() && !g_shutdown_requested.load(); iterationCnt++) {
		time_t currTime = time(NULL);

#ifdef __linux__
		// P1-8: Handle pending SIGUSR1 snapshot requests
		handlePendingSigusr1();
#endif

		cout << endl << "---- iteration:" << iterationCnt << ", time: " << time(NULL) << " ----" << endl;
		if (currTime >= callTime || connection.getPacketQueueSize()) {
			// Handle client state and process packets from the server
			client = WppClient::takeOwnership();
			if (client) {
#ifdef __linux__
				// P1-8: Track state transitions
				uint8_t prevState = logRing.get_current_state();
				lwm2m_client_state_t newClientState = client->getState();
				uint8_t newState = static_cast<uint8_t>(newClientState);

				// Log state transition if changed
				if (prevState != newState) {
					logRing.log_transition(prevState, newState,
						ftl::TriggerEvent::LWM2M_STEP_CALLED, 0);
				}
				logRing.set_current_state(newState);

				// Update introspection counters
				logRing.counters().transaction_count.store(
					static_cast<uint32_t>(client->getTransactionCount()),
					std::memory_order_relaxed);
				logRing.counters().pending_token_count.store(
					static_cast<uint32_t>(client->getPendingTokenCount()),
					std::memory_order_relaxed);
				logRing.counters().mid_alloc_count.store(
					client->getNextMid(),
					std::memory_order_relaxed);

				// Track UPDATE timestamps (STATE_READY indicates successful registration/update)
				if (newClientState == STATE_READY && prevState != newState) {
					logRing.update_timestamps().record(ftl::LogRing::get_monotonic_time_us());
				}
#endif
				callTime = currTime + client->loop();
				client->giveOwnership();
				cout << "Sleep time: " << callTime - time(NULL) << endl;
			}
		}
		this_thread::sleep_for(chrono::seconds(1));
	}

	// Graceful shutdown sequence
	if (g_shutdown_requested.load()) {
		cout << "[" << time(NULL) << "] [INFO] Shutting down..." << endl;

		// Take ownership of client for shutdown
		cout << "[" << time(NULL) << "] [INFO] Stopping LwM2M client..." << endl;
		client = WppClient::takeOwnership();

		if (client) {
			// Get current state
			lwm2m_client_state_t currentState = client->getState();
			cout << "[" << time(NULL) << "] [INFO] Current client state: " << currentState << endl;

			// If registered, send de-register
			if (currentState == STATE_READY) {
				cout << "[" << time(NULL) << "] [INFO] Sending DE-REGISTER request to server" << endl;

				// Call deregister to notify server
				client->deregister();

				cout << "[" << time(NULL) << "] [INFO] State transition: " << currentState << " → deregistering" << endl;

				// Process the de-register message
				time_t timeout = time(NULL) + 2; // 2 second timeout
				while (time(NULL) < timeout) {
					client->loop();
					this_thread::sleep_for(chrono::milliseconds(100));

					// Check if state changed
					lwm2m_client_state_t newState = client->getState();
					if (newState != currentState) {
						cout << "[" << time(NULL) << "] [INFO] State transition: " << currentState << " → " << newState << endl;
						currentState = newState;
						break;
					}
				}

				cout << "[" << time(NULL) << "] [INFO] DE-REGISTER complete" << endl;
			}

			cout << "[" << time(NULL) << "] [INFO] LwM2M client stopped" << endl;
			client->giveOwnership();
		}
	}

	cout << endl << "[" << time(NULL) << "] [INFO] Closing example..." << endl;

	// Wait for socket polling thread to finish
	my_thread.join();

	// Destroy client
	cout << "[" << time(NULL) << "] [INFO] Destroying LwM2M client..." << endl;
	WppClient::remove();
	cout << "[" << time(NULL) << "] [INFO] LwM2M client destroyed" << endl;

	cout << "[" << time(NULL) << "] [INFO] Goodbye!" << endl;

	return 0;
}

