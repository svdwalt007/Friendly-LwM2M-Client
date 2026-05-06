/**
 * OpenWrtFwUpdater.h
 *
 * OpenWRT-specific implementation of LWM2M Firmware Update Object (Object 5)
 * Integrates with OpenWRT's sysupgrade system for firmware updates
 * Implements full LWM2M FOTA state machine compliance
 */

#ifndef OPENWRT_FW_UPDATER_H_
#define OPENWRT_FW_UPDATER_H_

#include "WppClient.h"
#include "FwUpdater.h"
#include <string>
#include <mutex>
#include <thread>
#include <atomic>

using namespace wpp;
using namespace std;

/**
 * @brief OpenWRT Firmware Updater class
 *
 * This class implements firmware update functionality for OpenWRT devices
 * using the sysupgrade command. It follows the LWM2M FOTA state machine:
 *
 * States:
 * - IDLE: No update in progress
 * - DOWNLOADING: Firmware is being downloaded
 * - DOWNLOADED: Firmware downloaded successfully, ready to update
 * - UPDATING: Firmware update is in progress
 *
 * The updater performs:
 * 1. Firmware validation using 'sysupgrade -T'
 * 2. Flash space verification
 * 3. Firmware installation using 'sysupgrade'
 * 4. Proper state management and error handling
 */
class OpenWrtFwUpdater: public FwUpdater {
public:
    OpenWrtFwUpdater();
    ~OpenWrtFwUpdater();

    /**
     * @brief Set the firmware file path
     * @param path Path to the downloaded firmware file
     */
    void setFirmwarePath(const std::string& path);

private:
    // FwUpdater interface implementation
    void startUpdating() override;
    bool isUpdated() override;
    FwUpdRes lastUpdateResult() override;

    #if RES_5_6
    STRING_T pkgName() override;
    #endif

    #if RES_5_7
    STRING_T pkgVersion() override;
    #endif

    void reset() override;

private:
    // Internal update operations
    void performUpdate();
    bool validateFirmware();
    bool checkFlashSpace();
    bool extractMetadata();
    void executeSysupgrade();
    void cleanup();

    // Utility methods
    int executeCommand(const std::string& cmd, std::string& output);
    bool fileExists(const std::string& path);
    long getFileSize(const std::string& path);
    long getAvailableSpace(const std::string& path);

private:
    std::string _firmwarePath;          // Path to firmware file
    std::string _pkgName;               // Package name
    std::string _pkgVersion;            // Package version
    FwUpdRes _lastUpdateResult;         // Last update result

    std::atomic<bool> _updateInProgress;// Update in progress flag
    std::atomic<bool> _updateCompleted; // Update completed flag
    std::thread _updateThread;          // Background update thread
    std::mutex _mutex;                  // Thread synchronization

    // Configuration
    static constexpr const char* FIRMWARE_TEMP_DIR = "/tmp";
    static constexpr const char* FIRMWARE_FILE_NAME = "lwm2m_firmware.bin";
    static constexpr const char* METADATA_FILE = "/tmp/lwm2m_fw_metadata.txt";
    static constexpr long MIN_FLASH_MARGIN_KB = 512;  // Minimum free space margin
};

#endif // OPENWRT_FW_UPDATER_H_
