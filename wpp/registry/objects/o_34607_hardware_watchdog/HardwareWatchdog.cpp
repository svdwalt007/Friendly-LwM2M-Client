/*
 * HardwareWatchdog
 * Generated on: 2025-11-01 08:40:02
 * Created by: Sinai RnD
 */

#include "o_34607_hardware_watchdog/HardwareWatchdog.h"

#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

/* --------------- Code_cpp block 0 start --------------- */
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>
#include <fstream>
#include <chrono>
#include <ctime>

#define WD_TIMEOUT_MIN 1
#define WD_TIMEOUT_MAX 300
#define WD_INTERVAL_MIN 1
#define WD_INTERVAL_MAX 60
#define STATUS_MIN 0
#define STATUS_MAX 3
/* --------------- Code_cpp block 0 end --------------- */

#define TAG "HardwareWatchdog"

namespace wpp {

HardwareWatchdog::HardwareWatchdog(lwm2m_context_t &context, const OBJ_LINK_T &id): Instance(context, id) {

	/* --------------- Code_cpp block 1 start --------------- */
	m_watchdogFd = -1;
	m_running = false;
	m_devicePath = WD_DEVICE;
	/* --------------- Code_cpp block 1 end --------------- */

	resourcesCreate();
	resourcesInit();

	/* --------------- Code_cpp block 2 start --------------- */
	loadTriggerCount();
	WPP_LOGI(TAG, "HardwareWatchdog instance created");
	/* --------------- Code_cpp block 2 end --------------- */
}

HardwareWatchdog::~HardwareWatchdog() {
	/* --------------- Code_cpp block 3 start --------------- */
	stopWatchdog();
	saveTriggerCount();
	WPP_LOGI(TAG, "HardwareWatchdog instance destroyed");
	/* --------------- Code_cpp block 3 end --------------- */
}

Object & HardwareWatchdog::object(WppClient &ctx) {
	return ctx.registry().hardwareWatchdog();
}

HardwareWatchdog * HardwareWatchdog::instance(WppClient &ctx, ID_T instId) {
	Instance *inst = ctx.registry().hardwareWatchdog().instance(instId);
	if (!inst) return NULL;
	return static_cast<HardwareWatchdog*>(inst);
}

HardwareWatchdog * HardwareWatchdog::createInst(WppClient &ctx, ID_T instId) {
	Instance *inst = ctx.registry().hardwareWatchdog().createInstance(instId);
	if (!inst) return NULL;
	return static_cast<HardwareWatchdog*>(inst);
}

bool HardwareWatchdog::removeInst(WppClient &ctx, ID_T instId) {
	return ctx.registry().hardwareWatchdog().remove(instId);
}

void HardwareWatchdog::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
	/* --------------- Code_cpp block 4 start --------------- */
	/* --------------- Code_cpp block 4 end --------------- */

	operationNotify(*this, resLink, type);

	/* --------------- Code_cpp block 5 start --------------- */
	switch (type) {
	case ItemOp::WRITE:
		WPP_LOGD(TAG, "Server WRITE -> resId: %d, resInstId: %d", resLink.resId, resLink.resInstId);
		// Handle configuration changes
		if (resLink.resId == WATCHDOG_ENABLE_0) {
			BOOL_T enabled = resource(WATCHDOG_ENABLE_0)->get<BOOL_T>();
			if (enabled) {
				startWatchdog();
			} else {
				stopWatchdog();
			}
		} else if (resLink.resId == TIMEOUT_1 || resLink.resId == PET_INTERVAL_2) {
			INT_T timeout = resource(TIMEOUT_1)->get<INT_T>();
			INT_T interval = resource(PET_INTERVAL_2)->get<INT_T>();
			updateConfig(timeout, interval);
		}
		#if RES_34607_5
		else if (resLink.resId == MAGIC_CLOSE_ENABLE_5) {
			// Magic close setting changed
			WPP_LOGI(TAG, "Magic close setting updated");
		}
		#endif
		break;
	case ItemOp::EXECUTE:
		WPP_LOGD(TAG, "Server EXECUTE -> resId: %d, resInstId: %d", resLink.resId, resLink.resInstId);
		#if RES_34607_6
		if (resLink.resId == PET_WATCHDOG_6) {
			if (petWatchdog()) {
				WPP_LOGI(TAG, "Watchdog manually petted via server");
			}
		}
		#endif
		#if RES_34607_7
		if (resLink.resId == RESET_WATCHDOG_7) {
			WPP_LOGI(TAG, "Watchdog reset requested via server");
			stopWatchdog();
			resource(TIMEOUT_1)->set<INT_T>(30);
			resource(PET_INTERVAL_2)->set<INT_T>(5);
			if (resource(WATCHDOG_ENABLE_0)->get<BOOL_T>()) {
				startWatchdog();
			}
		}
		#endif
		break;
	default: break;
	}
	/* --------------- Code_cpp block 5 end --------------- */
}

void HardwareWatchdog::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
	if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);

	/* --------------- Code_cpp block 6 start --------------- */
	switch (type) {
	case ItemOp::WRITE:
		WPP_LOGD(TAG, "User WRITE -> resId: %d, resInstId: %d", resLink.resId, resLink.resInstId);
		break;
	case ItemOp::DELETE:
		WPP_LOGD(TAG, "User DELETE -> resId: %d, resInstId: %d", resLink.resId, resLink.resInstId);
		break;
	default: break;
	}
	/* --------------- Code_cpp block 6 end --------------- */
}

void HardwareWatchdog::resourcesCreate() {
	std::vector<Resource> resources = {
		{WATCHDOG_ENABLE_0,    ItemOp(ItemOp::READ|ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL },    
		{TIMEOUT_1,            ItemOp(ItemOp::READ|ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT },     
		{PET_INTERVAL_2,       ItemOp(ItemOp::READ|ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT },     
		{STATUS_3,             ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT },     
		#if RES_34607_4                                                                                                                                               
		{LAST_PET_TIME_4,      ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::TIME },    
		#endif                                                                                                                                                        
		#if RES_34607_5                                                                                                                                               
		{MAGIC_CLOSE_ENABLE_5, ItemOp(ItemOp::READ|ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL },    
		#endif                                                                                                                                                        
		#if RES_34607_6                                                                                                                                               
		{PET_WATCHDOG_6,       ItemOp(ItemOp::EXECUTE),            IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE }, 
		#endif                                                                                                                                                        
		#if RES_34607_7                                                                                                                                               
		{RESET_WATCHDOG_7,     ItemOp(ItemOp::EXECUTE),            IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE }, 
		#endif                                                                                                                                                        
		#if RES_34607_8                                                                                                                                               
		{PET_COUNT_8,          ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },     
		#endif                                                                                                                                                        
		#if RES_34607_9                                                                                                                                               
		{TRIGGER_COUNT_9,      ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },     
		#endif                                                                                                                                                        
		#if RES_34607_10                                                                                                                                              
		{DEVICE_PATH_10,       ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING },  
		#endif                                                                                                                                                        
		#if RES_34607_11                                                                                                                                              
		{HARDWARE_TIMEOUT_11,  ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },     
		#endif                                                                                                                                                        
		#if RES_34607_12                                                                                                                                              
		{BOOT_COUNT_12,        ItemOp(ItemOp::READ),               IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },     
		#endif                                                                                                                                                        
	};
	setupResources(std::move(resources));
}

void HardwareWatchdog::resourcesInit() {
	/* --------------- Code_cpp block 7 start --------------- */
	// Initialize watchdog enable (default: false/disabled)
	resource(WATCHDOG_ENABLE_0)->set<BOOL_T>(false);

	// Initialize timeout (default: 30 seconds)
	resource(TIMEOUT_1)->set<INT_T>(30);
	resource(TIMEOUT_1)->setDataVerifier((VERIFY_INT_T)[](const INT_T& value) {
		return WD_TIMEOUT_MIN <= value && value <= WD_TIMEOUT_MAX;
	});

	// Initialize pet interval (default: 5 seconds)
	resource(PET_INTERVAL_2)->set<INT_T>(5);
	resource(PET_INTERVAL_2)->setDataVerifier((VERIFY_INT_T)[](const INT_T& value) {
		return WD_INTERVAL_MIN <= value && value <= WD_INTERVAL_MAX;
	});

	// Initialize status (default: inactive)
	resource(STATUS_3)->set<INT_T>(WD_INACTIVE);
	resource(STATUS_3)->setDataVerifier((VERIFY_INT_T)[](const INT_T& value) {
		return STATUS_MIN <= value && value <= STATUS_MAX;
	});

	#if RES_34607_4
	// Initialize last pet time
	resource(LAST_PET_TIME_4)->set<TIME_T>(0);
	#endif

	#if RES_34607_5
	// Initialize magic close (default: true for OpenWRT)
	resource(MAGIC_CLOSE_ENABLE_5)->set<BOOL_T>(true);
	#endif

	#if RES_34607_8
	// Initialize pet count
	resource(PET_COUNT_8)->set<INT_T>(0);
	#endif

	#if RES_34607_9
	// Initialize trigger count (will be loaded from file)
	resource(TRIGGER_COUNT_9)->set<INT_T>(0);
	#endif

	#if RES_34607_10
	// Initialize device path
	resource(DEVICE_PATH_10)->set<STRING_T>(WD_DEVICE);
	#endif

	#if RES_34607_11
	// Initialize hardware timeout (will be read from device)
	resource(HARDWARE_TIMEOUT_11)->set<INT_T>(0);
	#endif

	#if RES_34607_12
	// Initialize boot count
	std::ifstream bootFile(BOOT_COUNT_FILE);
	INT_T bootCount = 1;
	if (bootFile.good()) {
		bootFile >> bootCount;
		bootCount++;
	}
	bootFile.close();

	std::ofstream bootOutFile(BOOT_COUNT_FILE);
	if (bootOutFile.good()) {
		bootOutFile << bootCount;
	}
	bootOutFile.close();

	resource(BOOT_COUNT_12)->set<INT_T>(bootCount);
	#endif
	/* --------------- Code_cpp block 7 end --------------- */
}

/* --------------- Code_cpp block 8 start --------------- */

// Start the watchdog timer
bool HardwareWatchdog::startWatchdog() {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_running) {
		WPP_LOGW(TAG, "Watchdog already running");
		return true;
	}

	// Open watchdog device
	m_watchdogFd = openWatchdogDevice();
	if (m_watchdogFd < 0) {
		WPP_LOGE(TAG, "Failed to open watchdog device");
		resource(STATUS_3)->set<INT_T>(WD_ERROR);
		return false;
	}

	// Set hardware timeout
	INT_T timeout = resource(TIMEOUT_1)->get<INT_T>();
	if (!setHardwareTimeout(timeout)) {
		WPP_LOGW(TAG, "Failed to set hardware timeout, using default");
	}

	// Update hardware timeout resource
	#if RES_34607_11
	int hwTimeout = getHardwareTimeout();
	if (hwTimeout > 0) {
		resource(HARDWARE_TIMEOUT_11)->set<INT_T>(hwTimeout);
	}
	#endif

	// Start petting thread
	m_running = true;
	m_petThread = std::thread(&HardwareWatchdog::watchdogThread, this);

	resource(STATUS_3)->set<INT_T>(WD_ACTIVE);
	WPP_LOGI(TAG, "Watchdog started with timeout=%d, interval=%d",
		timeout, resource(PET_INTERVAL_2)->get<INT_T>());

	return true;
}

// Stop the watchdog timer
void HardwareWatchdog::stopWatchdog() {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_running) {
		return;
	}

	m_running = false;

	// Wait for thread to finish
	if (m_petThread.joinable()) {
		m_petThread.join();
	}

	closeWatchdogDevice();
	resource(STATUS_3)->set<INT_T>(WD_INACTIVE);
	WPP_LOGI(TAG, "Watchdog stopped");
}

// Pet (reset) the watchdog timer
bool HardwareWatchdog::petWatchdog() {
	if (!writeToWatchdog()) {
		WPP_LOGE(TAG, "Failed to pet watchdog");
		return false;
	}

	#if RES_34607_4
	// Update last pet time
	auto now = std::chrono::system_clock::now();
	TIME_T timestamp = std::chrono::system_clock::to_time_t(now);
	resource(LAST_PET_TIME_4)->set<TIME_T>(timestamp);
	#endif

	#if RES_34607_8
	// Increment pet count
	INT_T count = resource(PET_COUNT_8)->get<INT_T>();
	resource(PET_COUNT_8)->set<INT_T>(count + 1);
	#endif

	return true;
}

// Update watchdog configuration
bool HardwareWatchdog::updateConfig(int timeout, int interval) {
	if (timeout < WD_TIMEOUT_MIN || timeout > WD_TIMEOUT_MAX) {
		WPP_LOGE(TAG, "Invalid timeout value: %d", timeout);
		return false;
	}

	if (interval < WD_INTERVAL_MIN || interval > WD_INTERVAL_MAX) {
		WPP_LOGE(TAG, "Invalid interval value: %d", interval);
		return false;
	}

	if (interval >= timeout) {
		WPP_LOGW(TAG, "Interval should be less than timeout");
	}

	bool wasRunning = m_running;
	if (wasRunning) {
		stopWatchdog();
	}

	resource(TIMEOUT_1)->set<INT_T>(timeout);
	resource(PET_INTERVAL_2)->set<INT_T>(interval);

	if (wasRunning && resource(WATCHDOG_ENABLE_0)->get<BOOL_T>()) {
		return startWatchdog();
	}

	return true;
}

// Background thread for automatic watchdog petting
void HardwareWatchdog::watchdogThread() {
	WPP_LOGI(TAG, "Watchdog pet thread started");

	while (m_running) {
		INT_T interval = resource(PET_INTERVAL_2)->get<INT_T>();

		// Sleep for the interval
		for (int i = 0; i < interval && m_running; i++) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		if (m_running) {
			if (!petWatchdog()) {
				WPP_LOGE(TAG, "Automatic pet failed");
				resource(STATUS_3)->set<INT_T>(WD_ERROR);
			}
		}
	}

	WPP_LOGI(TAG, "Watchdog pet thread stopped");
}

// Open the watchdog device
int HardwareWatchdog::openWatchdogDevice() {
	int fd = open(m_devicePath.c_str(), O_WRONLY);
	if (fd < 0) {
		WPP_LOGE(TAG, "Failed to open %s: %s", m_devicePath.c_str(), strerror(errno));
		return -1;
	}

	WPP_LOGI(TAG, "Watchdog device opened: %s (fd=%d)", m_devicePath.c_str(), fd);
	return fd;
}

// Close the watchdog device
void HardwareWatchdog::closeWatchdogDevice() {
	if (m_watchdogFd < 0) {
		return;
	}

	#if RES_34607_5
	// Check if magic close is enabled
	BOOL_T magicClose = resource(MAGIC_CLOSE_ENABLE_5)->get<BOOL_T>();
	if (magicClose) {
		// Write magic character 'V' before closing to disable watchdog
		char v = 'V';
		if (write(m_watchdogFd, &v, 1) < 0) {
			WPP_LOGW(TAG, "Failed to write magic close character");
		} else {
			WPP_LOGI(TAG, "Magic close character written");
		}
	}
	#endif

	close(m_watchdogFd);
	m_watchdogFd = -1;
	WPP_LOGI(TAG, "Watchdog device closed");
}

// Write to watchdog device to pet it
bool HardwareWatchdog::writeToWatchdog() {
	if (m_watchdogFd < 0) {
		WPP_LOGE(TAG, "Watchdog device not open");
		return false;
	}

	// Writing any value to /dev/watchdog pets the watchdog
	char dummy = 0;
	ssize_t result = write(m_watchdogFd, &dummy, 1);
	if (result < 0) {
		WPP_LOGE(TAG, "Failed to write to watchdog: %s", strerror(errno));
		return false;
	}

	return true;
}

// Get hardware timeout from watchdog device
int HardwareWatchdog::getHardwareTimeout() {
	if (m_watchdogFd < 0) {
		return -1;
	}

	int timeout = 0;
	if (ioctl(m_watchdogFd, WDIOC_GETTIMEOUT, &timeout) < 0) {
		WPP_LOGW(TAG, "Failed to get hardware timeout: %s", strerror(errno));
		return -1;
	}

	return timeout;
}

// Set hardware timeout on watchdog device
bool HardwareWatchdog::setHardwareTimeout(int timeout) {
	if (m_watchdogFd < 0) {
		WPP_LOGE(TAG, "Watchdog device not open");
		return false;
	}

	int newTimeout = timeout;
	if (ioctl(m_watchdogFd, WDIOC_SETTIMEOUT, &newTimeout) < 0) {
		WPP_LOGE(TAG, "Failed to set hardware timeout: %s", strerror(errno));
		return false;
	}

	if (newTimeout != timeout) {
		WPP_LOGW(TAG, "Hardware timeout set to %d (requested %d)", newTimeout, timeout);
	}

	return true;
}

// Load watchdog trigger count from persistent storage
void HardwareWatchdog::loadTriggerCount() {
	#if RES_34607_9
	std::ifstream file(TRIGGER_COUNT_FILE);
	if (file.good()) {
		INT_T count = 0;
		file >> count;
		resource(TRIGGER_COUNT_9)->set<INT_T>(count);
		WPP_LOGI(TAG, "Loaded trigger count: %d", count);
	}
	file.close();
	#endif
}

// Save watchdog trigger count to persistent storage
void HardwareWatchdog::saveTriggerCount() {
	#if RES_34607_9
	std::ofstream file(TRIGGER_COUNT_FILE);
	if (file.good()) {
		INT_T count = resource(TRIGGER_COUNT_9)->get<INT_T>();
		file << count;
		WPP_LOGI(TAG, "Saved trigger count: %d", count);
	}
	file.close();
	#endif
}

/* --------------- Code_cpp block 8 end --------------- */

} /* namespace wpp */
