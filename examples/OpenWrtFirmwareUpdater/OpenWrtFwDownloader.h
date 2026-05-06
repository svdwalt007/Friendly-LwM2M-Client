/**
 * OpenWrtFwDownloader.h
 *
 * OpenWRT-specific implementation of LWM2M Firmware Internal Downloader
 * Handles PUSH method where firmware is sent directly via LWM2M PACKAGE resource
 * Implements full state management and error handling
 */

#ifndef OPENWRT_FW_DOWNLOADER_H_
#define OPENWRT_FW_DOWNLOADER_H_

#include "WppClient.h"
#include "FwInternalDl.h"
#include "OpenWrtFwUpdater.h"
#include <string>
#include <fstream>
#include <mutex>
#include <atomic>

using namespace wpp;
using namespace std;

/**
 * @brief OpenWRT Firmware Internal Downloader (PUSH method)
 *
 * This class implements firmware download functionality for the PUSH method,
 * where the LWM2M server sends firmware data directly to the device via
 * the PACKAGE resource (Resource 0).
 *
 * Download process:
 * 1. downloadIsStarted() - Initialize download, create temporary file
 * 2. saveDownloadedBlock() - Receive and save firmware blocks
 * 3. downloadIsCompleted() - Finalize download, validate integrity
 * 4. downloadResult() - Return success or error code
 *
 * The downloader performs:
 * - Block-by-block firmware reception
 * - Flash space verification
 * - Basic integrity checking
 * - Error handling for various failure modes
 */
class OpenWrtFwDownloader: public FwInternalDl {
public:
    /**
     * @brief Constructor
     * @param updater Reference to the firmware updater to notify about the firmware path
     */
    explicit OpenWrtFwDownloader(OpenWrtFwUpdater& updater);
    ~OpenWrtFwDownloader();

private:
    // FwInternalDl interface implementation
    void downloadIsStarted() override;
    bool saveDownloadedBlock(const OPAQUE_T &dataBlock) override;
    void downloadIsCompleted() override;
    FwUpdRes downloadResult() override;
    void reset() override;

private:
    // Internal operations
    bool initializeDownload();
    void finalizeDownload();
    bool verifyDownload();
    void cleanup();
    bool checkAvailableSpace(size_t requiredSize);

private:
    OpenWrtFwUpdater& _updater;         // Reference to updater
    std::string _firmwarePath;          // Path to firmware file being downloaded
    std::ofstream _fileStream;          // File stream for writing firmware
    FwUpdRes _downloadResult;           // Download result
    size_t _bytesDownloaded;            // Total bytes downloaded
    bool _downloadActive;               // Download in progress flag
    std::mutex _mutex;                  // Thread synchronization

    // Configuration
    static constexpr const char* FIRMWARE_TEMP_DIR = "/tmp";
    static constexpr const char* FIRMWARE_FILE_NAME = "lwm2m_firmware.bin";
    static constexpr const char* FIRMWARE_TEMP_FILE = "/tmp/lwm2m_firmware.tmp";
    static constexpr size_t MAX_FIRMWARE_SIZE = 100 * 1024 * 1024; // 100 MB
    static constexpr size_t MIN_FREE_SPACE = 1024 * 1024; // 1 MB minimum
};

#endif // OPENWRT_FW_DOWNLOADER_H_
