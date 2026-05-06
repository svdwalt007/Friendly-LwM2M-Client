/**
 * OpenWrtFwUriDownloader.h
 *
 * OpenWRT-specific implementation of LWM2M Firmware External Downloader
 * Handles PULL method where firmware is downloaded from a URI
 * Supports HTTP, HTTPS, CoAP, CoAPS protocols using wget/curl
 */

#ifndef OPENWRT_FW_URI_DOWNLOADER_H_
#define OPENWRT_FW_URI_DOWNLOADER_H_

#include "WppClient.h"
#include "FwExternalDl.h"
#include "OpenWrtFwUpdater.h"
#include "m_0_lwm2m_security/Lwm2mSecurity.h"
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

using namespace wpp;
using namespace std;

#if RES_5_8

/**
 * @brief OpenWRT Firmware External URI Downloader (PULL method)
 *
 * This class implements firmware download functionality for the PULL method,
 * where the device downloads firmware from a URI specified by the LWM2M server
 * via the PACKAGE_URI resource (Resource 1).
 *
 * Download process:
 * 1. startDownloading() - Initiate download from URI
 * 2. Background thread downloads firmware using wget/curl
 * 3. isDownloaded() - Check if download is complete
 * 4. downloadResult() - Return success or error code
 *
 * Supported protocols:
 * - HTTP
 * - HTTPS
 * - CoAP (if available)
 * - CoAPS (if available)
 *
 * The downloader performs:
 * - Protocol detection and tool selection (wget/curl)
 * - Secure downloads with certificate validation
 * - Connection error handling
 * - Integrity checking
 * - Progress monitoring
 */
class OpenWrtFwUriDownloader: public FwExternalDl {
public:
    /**
     * @brief Constructor
     * @param updater Reference to the firmware updater to notify about the firmware path
     */
    explicit OpenWrtFwUriDownloader(OpenWrtFwUpdater& updater);
    ~OpenWrtFwUriDownloader();

private:
    // FwExternalDl interface implementation
    std::vector<FwUpdProtocol> supportedProtocols() override;
    void startDownloading(const STRING_T &uri, Lwm2mSecurity &security) override;
    bool isDownloaded() override;
    FwUpdRes downloadResult() override;
    void reset() override;

private:
    // Internal operations
    void performDownload(const std::string& uri, Lwm2mSecurity* security);
    bool downloadWithWget(const std::string& uri, Lwm2mSecurity* security);
    bool downloadWithCurl(const std::string& uri, Lwm2mSecurity* security);
    bool downloadWithCoap(const std::string& uri, Lwm2mSecurity* security);
    bool verifyDownload();
    void cleanup();
    int executeCommand(const std::string& cmd, std::string& output);
    bool isToolAvailable(const std::string& tool);
    std::string getProtocolFromUri(const std::string& uri);

private:
    OpenWrtFwUpdater& _updater;         // Reference to updater
    std::string _firmwarePath;          // Path to downloaded firmware file
    FwUpdRes _downloadResult;           // Download result
    std::atomic<bool> _downloadCompleted; // Download completed flag
    std::thread _downloadThread;        // Background download thread
    std::mutex _mutex;                  // Thread synchronization

    // Available download tools
    bool _hasWget;
    bool _hasCurl;
    bool _hasCoap;

    // Configuration
    static constexpr const char* FIRMWARE_TEMP_DIR = "/tmp";
    static constexpr const char* FIRMWARE_FILE_NAME = "lwm2m_firmware.bin";
    static constexpr const char* FIRMWARE_TEMP_FILE = "/tmp/lwm2m_firmware_uri.tmp";
    static constexpr int DOWNLOAD_TIMEOUT_SEC = 600; // 10 minutes
};

#endif // RES_5_8

#endif // OPENWRT_FW_URI_DOWNLOADER_H_
