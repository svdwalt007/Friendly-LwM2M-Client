/*
 * HardwareWatchdog
 * Generated on: 2025-11-01 08:40:02
 * Created by: Sinai RnD
 */

#ifndef WPP_O_34607_HARDWARE_WATCHDOG_H
#define WPP_O_34607_HARDWARE_WATCHDOG_H

#include "HardwareWatchdogConfig.h"
#include "HardwareWatchdogInfo.h"
#include "Instance.h"
#include "InstSubject.h"

/* --------------- Сode_h block 0 start --------------- */
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
/* --------------- Сode_h block 0 end --------------- */

namespace wpp {

class WppClient;
class Object;

/* --------------- Сode_h block 1 start --------------- */
/* --------------- Сode_h block 1 end --------------- */

class HardwareWatchdog : public Instance {
public:
	enum ID: ID_T {
		WATCHDOG_ENABLE_0 = 0,
		TIMEOUT_1 = 1,
		PET_INTERVAL_2 = 2,
		STATUS_3 = 3,
		#if RES_34607_4
		LAST_PET_TIME_4 = 4,
		#endif
		#if RES_34607_5
		MAGIC_CLOSE_ENABLE_5 = 5,
		#endif
		#if RES_34607_6
		PET_WATCHDOG_6 = 6,
		#endif
		#if RES_34607_7
		RESET_WATCHDOG_7 = 7,
		#endif
		#if RES_34607_8
		PET_COUNT_8 = 8,
		#endif
		#if RES_34607_9
		TRIGGER_COUNT_9 = 9,
		#endif
		#if RES_34607_10
		DEVICE_PATH_10 = 10,
		#endif
		#if RES_34607_11
		HARDWARE_TIMEOUT_11 = 11,
		#endif
		#if RES_34607_12
		BOOT_COUNT_12 = 12,
		#endif
	};

	/* --------------- Code_h block 2 start --------------- */
	// Watchdog status enumeration
	enum WatchdogStatus: uint8_t {
		WD_INACTIVE = 0,  // Disabled
		WD_ACTIVE = 1,    // Running normally
		WD_TRIGGERED = 2, // Reboot triggered
		WD_ERROR = 3      // Device error
	};
	/* --------------- Code_h block 2 end --------------- */

public:
	HardwareWatchdog(lwm2m_context_t &context, const OBJ_LINK_T &id);
	~HardwareWatchdog();

	/* --------------- Helpful methods --------------- */
	/**
	 * @brief Gets the Object reference.
	 * @param ctx - WppClient context.
	 * @return A reference to the Object.
	 */
	static Object & object(WppClient &ctx);

	/**
	 * @brief Gets an instance of the object.
	 * @param ctx - WppClient context.
	 * @param instId The instance ID. If not provided, the first available instance is returned.
	 * @return A pointer to the Instance object or NULL.
	 */
	static HardwareWatchdog * instance(WppClient &ctx, ID_T instId = ID_T_MAX_VAL);

	/**
	 * @brief Creates an instance of the object.
	 * @param ctx - WppClient context
	 * @param instanceID The instance ID.
	 * @return A pointer to the created Instance object or NULL.
	 */
	static HardwareWatchdog * createInst(WppClient &ctx, ID_T instId = ID_T_MAX_VAL);

	/**
	 * @brief Removes an instance of the object.
	 * @param ctx - WppClient context.
	 * @param instId The instance ID.
	 * @return True if the instance was successfully removed, false otherwise.
	 */
	static bool removeInst(WppClient &ctx, ID_T instId);

	/* --------------- Code_h block 3 start --------------- */
	/**
	 * @brief Start the watchdog timer
	 * @return true if started successfully
	 */
	bool startWatchdog();

	/**
	 * @brief Stop the watchdog timer
	 */
	void stopWatchdog();

	/**
	 * @brief Pet (reset) the watchdog timer
	 * @return true if pet was successful
	 */
	bool petWatchdog();

	/**
	 * @brief Update watchdog configuration
	 * @param timeout New timeout value in seconds
	 * @param interval New pet interval in seconds
	 * @return true if configuration was updated successfully
	 */
	bool updateConfig(int timeout, int interval);

	/**
	 * @brief Read watchdog trigger count from persistent storage
	 */
	void loadTriggerCount();

	/**
	 * @brief Save watchdog trigger count to persistent storage
	 */
	void saveTriggerCount();
	/* --------------- Code_h block 3 end --------------- */

protected:
	/* --------------- Instance implementation part --------------- */
	/*
	 * Handles information about resource operation that made server
	 */
	void serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) override;
	/*
	 * Handles information about resource operation that made user
	 */
	void userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) override;

	/* --------------- Code_h block 4 start --------------- */
	/* --------------- Code_h block 4 end --------------- */

private:
	/* --------------- Class private methods --------------- */
	/*
	 * Creates resources without initializing.
	 */
	void resourcesCreate();
	/*
	 * Initialize resources with default values
	 * SINGLE resource always must have at least one instance.
	 */	
	void resourcesInit();
	
	/* --------------- Code_h block 5 start --------------- */
	/**
	 * @brief Background thread function for automatic watchdog petting
	 */
	void watchdogThread();

	/**
	 * @brief Open the watchdog device
	 * @return file descriptor or -1 on error
	 */
	int openWatchdogDevice();

	/**
	 * @brief Close the watchdog device
	 */
	void closeWatchdogDevice();

	/**
	 * @brief Write to watchdog device to pet it
	 * @return true if successful
	 */
	bool writeToWatchdog();

	/**
	 * @brief Get hardware timeout from watchdog device
	 * @return timeout in seconds or -1 on error
	 */
	int getHardwareTimeout();

	/**
	 * @brief Set hardware timeout on watchdog device
	 * @param timeout Timeout in seconds
	 * @return true if successful
	 */
	bool setHardwareTimeout(int timeout);
	/* --------------- Code_h block 5 end --------------- */

private:
	/* --------------- Class private properties --------------- */
	/* --------------- Code_h block 6 start --------------- */
	int m_watchdogFd;                      // File descriptor for /dev/watchdog
	std::thread m_petThread;               // Thread for automatic petting
	std::atomic<bool> m_running;           // Flag to control thread execution
	std::mutex m_mutex;                    // Mutex for thread-safe operations
	std::string m_devicePath;              // Path to watchdog device
	static constexpr const char* WD_DEVICE = "/dev/watchdog";
	static constexpr const char* TRIGGER_COUNT_FILE = "/tmp/lwm2m_wd_triggers";
	static constexpr const char* BOOT_COUNT_FILE = "/tmp/lwm2m_wd_boots";
	/* --------------- Code_h block 6 end --------------- */
};

} /* namespace wpp */

#endif /* WPP_O_34607_HARDWARE_WATCHDOG_H */
