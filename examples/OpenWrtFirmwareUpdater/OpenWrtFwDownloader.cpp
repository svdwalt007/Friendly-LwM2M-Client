/**
 * OpenWrtFwDownloader.cpp
 *
 * Implementation of OpenWRT-specific firmware internal downloader
 * for PUSH method (firmware sent via LWM2M PACKAGE resource)
 */

#include "OpenWrtFwDownloader.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

OpenWrtFwDownloader::OpenWrtFwDownloader(OpenWrtFwUpdater& updater)
    : _updater(updater)
    , _downloadResult(FwUpdRes::R_INITIAL)
    , _bytesDownloaded(0)
    , _downloadActive(false)
{
    _firmwarePath = std::string(FIRMWARE_TEMP_DIR) + "/" + FIRMWARE_FILE_NAME;
    cout << "OpenWrtFwDownloader: Initialized" << endl;
}

OpenWrtFwDownloader::~OpenWrtFwDownloader() {
    cleanup();
}

void OpenWrtFwDownloader::downloadIsStarted() {
    std::lock_guard<std::mutex> lock(_mutex);

    cout << "OpenWrtFwDownloader: Download started" << endl;

    // Clean up any previous download
    cleanup();

    // Initialize download
    if (!initializeDownload()) {
        cout << "OpenWrtFwDownloader: ERROR - Failed to initialize download" << endl;
        _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        return;
    }

    _downloadActive = true;
    _bytesDownloaded = 0;
    _downloadResult = FwUpdRes::R_INITIAL;

    cout << "OpenWrtFwDownloader: Ready to receive firmware data" << endl;
}

bool OpenWrtFwDownloader::saveDownloadedBlock(const OPAQUE_T &dataBlock) {
    std::lock_guard<std::mutex> lock(_mutex);

    if (!_downloadActive) {
        cout << "OpenWrtFwDownloader: ERROR - Download not active" << endl;
        _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        return false;
    }

    // Check if this is the complete firmware data (all in one block)
    if (dataBlock.empty()) {
        cout << "OpenWrtFwDownloader: Received empty data block" << endl;
        return true;
    }

    // Check size constraints
    if (_bytesDownloaded + dataBlock.size() > MAX_FIRMWARE_SIZE) {
        cout << "OpenWrtFwDownloader: ERROR - Firmware too large" << endl;
        _downloadResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
        _downloadActive = false;
        cleanup();
        return false;
    }

    // Check available space
    if (!checkAvailableSpace(dataBlock.size())) {
        cout << "OpenWrtFwDownloader: ERROR - Not enough space" << endl;
        _downloadResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
        _downloadActive = false;
        cleanup();
        return false;
    }

    // Write data to file
    try {
        _fileStream.write(reinterpret_cast<const char*>(dataBlock.data()), dataBlock.size());
        if (!_fileStream.good()) {
            cout << "OpenWrtFwDownloader: ERROR - Failed to write data" << endl;
            _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
            _downloadActive = false;
            cleanup();
            return false;
        }

        _bytesDownloaded += dataBlock.size();
        cout << "OpenWrtFwDownloader: Saved block (" << dataBlock.size()
             << " bytes), total: " << _bytesDownloaded << " bytes" << endl;

    } catch (const std::exception& e) {
        cout << "OpenWrtFwDownloader: ERROR - Exception while writing: " << e.what() << endl;
        _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        _downloadActive = false;
        cleanup();
        return false;
    }

    return true;
}

void OpenWrtFwDownloader::downloadIsCompleted() {
    std::lock_guard<std::mutex> lock(_mutex);

    cout << "OpenWrtFwDownloader: Download completed" << endl;

    if (!_downloadActive) {
        cout << "OpenWrtFwDownloader: WARNING - Download was not active" << endl;
        return;
    }

    _downloadActive = false;
    finalizeDownload();
}

FwUpdRes OpenWrtFwDownloader::downloadResult() {
    std::lock_guard<std::mutex> lock(_mutex);
    return _downloadResult;
}

void OpenWrtFwDownloader::reset() {
    std::lock_guard<std::mutex> lock(_mutex);

    cout << "OpenWrtFwDownloader: Reset" << endl;

    _downloadActive = false;
    _bytesDownloaded = 0;
    _downloadResult = FwUpdRes::R_INITIAL;

    cleanup();
}

bool OpenWrtFwDownloader::initializeDownload() {
    // Open temporary file for writing
    _fileStream.open(FIRMWARE_TEMP_FILE, std::ios::binary | std::ios::trunc);

    if (!_fileStream.is_open()) {
        cout << "OpenWrtFwDownloader: ERROR - Failed to open file: " << FIRMWARE_TEMP_FILE << endl;
        return false;
    }

    cout << "OpenWrtFwDownloader: Opened temporary file: " << FIRMWARE_TEMP_FILE << endl;
    return true;
}

void OpenWrtFwDownloader::finalizeDownload() {
    // Close file stream
    if (_fileStream.is_open()) {
        _fileStream.flush();
        _fileStream.close();
    }

    cout << "OpenWrtFwDownloader: Total bytes downloaded: " << _bytesDownloaded << endl;

    // Check if we received any data
    if (_bytesDownloaded == 0) {
        cout << "OpenWrtFwDownloader: ERROR - No data received" << endl;
        _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        cleanup();
        return;
    }

    // Verify the download
    if (!verifyDownload()) {
        // verifyDownload sets the appropriate error code
        cleanup();
        return;
    }

    // Move temporary file to final location
    if (rename(FIRMWARE_TEMP_FILE, _firmwarePath.c_str()) != 0) {
        cout << "OpenWrtFwDownloader: ERROR - Failed to move firmware file" << endl;
        _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        cleanup();
        return;
    }

    cout << "OpenWrtFwDownloader: Firmware saved to: " << _firmwarePath << endl;

    // Notify updater about the firmware path
    _updater.setFirmwarePath(_firmwarePath);

    // Download successful
    _downloadResult = FwUpdRes::R_INITIAL;

    cout << "OpenWrtFwDownloader: Download finalized successfully" << endl;
}

bool OpenWrtFwDownloader::verifyDownload() {
    // Basic verification
    struct stat st;
    if (stat(FIRMWARE_TEMP_FILE, &st) != 0) {
        cout << "OpenWrtFwDownloader: ERROR - Downloaded file not found" << endl;
        _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        return false;
    }

    // Check file size
    if (st.st_size == 0) {
        cout << "OpenWrtFwDownloader: ERROR - Downloaded file is empty" << endl;
        _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        return false;
    }

    if (static_cast<size_t>(st.st_size) != _bytesDownloaded) {
        cout << "OpenWrtFwDownloader: ERROR - File size mismatch" << endl;
        _downloadResult = FwUpdRes::R_INTEGRITY_CHECK_FAIL;
        return false;
    }

    // Check minimum firmware size (should be at least a few KB)
    if (st.st_size < 1024) {
        cout << "OpenWrtFwDownloader: ERROR - Firmware too small, likely invalid" << endl;
        _downloadResult = FwUpdRes::R_UNSUPPORTED_PKG_TYPE;
        return false;
    }

    cout << "OpenWrtFwDownloader: Verification passed" << endl;
    return true;
}

void OpenWrtFwDownloader::cleanup() {
    // Close file stream if open
    if (_fileStream.is_open()) {
        _fileStream.close();
    }

    // Remove temporary file
    if (access(FIRMWARE_TEMP_FILE, F_OK) == 0) {
        remove(FIRMWARE_TEMP_FILE);
    }
}

bool OpenWrtFwDownloader::checkAvailableSpace(size_t requiredSize) {
    struct statvfs st;
    if (statvfs(FIRMWARE_TEMP_DIR, &st) != 0) {
        cout << "OpenWrtFwDownloader: ERROR - Failed to get filesystem stats" << endl;
        return false;
    }

    size_t availableSpace = st.f_bavail * st.f_frsize;
    size_t totalRequired = requiredSize + MIN_FREE_SPACE;

    if (availableSpace < totalRequired) {
        cout << "OpenWrtFwDownloader: ERROR - Insufficient space. Available: "
             << availableSpace << " bytes, Required: " << totalRequired << " bytes" << endl;
        return false;
    }

    return true;
}
