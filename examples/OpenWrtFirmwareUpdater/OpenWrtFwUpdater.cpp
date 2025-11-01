/**
 * OpenWrtFwUpdater.cpp
 *
 * Implementation of OpenWRT-specific firmware updater with full
 * LWM2M FOTA state compliance and sysupgrade integration
 */

#include "OpenWrtFwUpdater.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

OpenWrtFwUpdater::OpenWrtFwUpdater()
    : _pkgName("")
    , _pkgVersion("")
    , _lastUpdateResult(FwUpdRes::R_INITIAL)
    , _updateInProgress(false)
    , _updateCompleted(false)
{
    // Initialize firmware path
    _firmwarePath = std::string(FIRMWARE_TEMP_DIR) + "/" + FIRMWARE_FILE_NAME;

    // Try to read existing metadata
    extractMetadata();

    cout << "OpenWrtFwUpdater: Initialized" << endl;
}

OpenWrtFwUpdater::~OpenWrtFwUpdater() {
    // Wait for update thread to complete
    if (_updateThread.joinable()) {
        _updateThread.join();
    }
    cleanup();
}

void OpenWrtFwUpdater::setFirmwarePath(const std::string& path) {
    std::lock_guard<std::mutex> lock(_mutex);
    _firmwarePath = path;
    cout << "OpenWrtFwUpdater: Firmware path set to: " << _firmwarePath << endl;
}

void OpenWrtFwUpdater::startUpdating() {
    std::lock_guard<std::mutex> lock(_mutex);

    cout << "OpenWrtFwUpdater: Starting firmware update" << endl;

    // Reset state
    _updateCompleted = false;
    _lastUpdateResult = FwUpdRes::R_INITIAL;

    // Verify firmware file exists
    if (!fileExists(_firmwarePath)) {
        cout << "OpenWrtFwUpdater: ERROR - Firmware file not found: " << _firmwarePath << endl;
        _lastUpdateResult = FwUpdRes::R_FW_UPD_FAIL;
        _updateCompleted = true;
        return;
    }

    // Validate firmware
    if (!validateFirmware()) {
        // validateFirmware sets the appropriate error result
        _updateCompleted = true;
        return;
    }

    // Check flash space
    if (!checkFlashSpace()) {
        cout << "OpenWrtFwUpdater: ERROR - Not enough flash space" << endl;
        _lastUpdateResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
        _updateCompleted = true;
        return;
    }

    // Extract metadata from firmware
    if (!extractMetadata()) {
        cout << "OpenWrtFwUpdater: WARNING - Could not extract metadata" << endl;
        // Continue anyway with default values
    }

    // Start update in background thread
    _updateInProgress = true;
    _updateThread = std::thread(&OpenWrtFwUpdater::performUpdate, this);
    _updateThread.detach();
}

bool OpenWrtFwUpdater::isUpdated() {
    return _updateCompleted.load();
}

FwUpdRes OpenWrtFwUpdater::lastUpdateResult() {
    return _lastUpdateResult;
}

#if RES_5_6
STRING_T OpenWrtFwUpdater::pkgName() {
    std::lock_guard<std::mutex> lock(_mutex);
    return _pkgName.empty() ? "OpenWRT" : _pkgName;
}
#endif

#if RES_5_7
STRING_T OpenWrtFwUpdater::pkgVersion() {
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_pkgVersion.empty()) {
        return _pkgVersion;
    }

    // Try to get version from /etc/openwrt_release
    std::string output;
    if (executeCommand("grep DISTRIB_RELEASE /etc/openwrt_release | cut -d'=' -f2 | tr -d \"'\"", output) == 0) {
        // Remove trailing newline
        if (!output.empty() && output[output.length()-1] == '\n') {
            output.erase(output.length()-1);
        }
        return output.empty() ? "unknown" : output;
    }

    return "unknown";
}
#endif

void OpenWrtFwUpdater::reset() {
    std::lock_guard<std::mutex> lock(_mutex);
    cout << "OpenWrtFwUpdater: Reset" << endl;

    // Wait for update thread if running
    if (_updateThread.joinable()) {
        _updateThread.join();
    }

    _updateInProgress = false;
    _updateCompleted = false;
    _lastUpdateResult = FwUpdRes::R_INITIAL;

    cleanup();
}

void OpenWrtFwUpdater::performUpdate() {
    cout << "OpenWrtFwUpdater: Performing firmware update..." << endl;

    try {
        // Execute sysupgrade
        executeSysupgrade();

        // If we reach here, sysupgrade scheduling was successful
        // The actual update will happen after reboot
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _lastUpdateResult = FwUpdRes::R_FW_UPD_SUCCESS;
            _updateInProgress = false;
            _updateCompleted = true;
        }

        cout << "OpenWrtFwUpdater: Firmware update initiated successfully" << endl;
        cout << "OpenWrtFwUpdater: System will reboot to complete update" << endl;

    } catch (const std::exception& e) {
        cout << "OpenWrtFwUpdater: ERROR - Exception during update: " << e.what() << endl;
        std::lock_guard<std::mutex> lock(_mutex);
        _lastUpdateResult = FwUpdRes::R_FW_UPD_FAIL;
        _updateInProgress = false;
        _updateCompleted = true;
    }
}

bool OpenWrtFwUpdater::validateFirmware() {
    cout << "OpenWrtFwUpdater: Validating firmware..." << endl;

    // Use sysupgrade -T to test the firmware image
    std::string cmd = "sysupgrade -T " + _firmwarePath + " 2>&1";
    std::string output;
    int result = executeCommand(cmd, output);

    if (result != 0) {
        cout << "OpenWrtFwUpdater: ERROR - Firmware validation failed" << endl;
        cout << "OpenWrtFwUpdater: sysupgrade output: " << output << endl;

        // Determine specific error
        if (output.find("not enough space") != std::string::npos) {
            _lastUpdateResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
        } else if (output.find("invalid") != std::string::npos ||
                   output.find("unsupported") != std::string::npos) {
            _lastUpdateResult = FwUpdRes::R_UNSUPPORTED_PKG_TYPE;
        } else if (output.find("checksum") != std::string::npos ||
                   output.find("signature") != std::string::npos) {
            _lastUpdateResult = FwUpdRes::R_INTEGRITY_CHECK_FAIL;
        } else {
            _lastUpdateResult = FwUpdRes::R_FW_UPD_FAIL;
        }
        return false;
    }

    cout << "OpenWrtFwUpdater: Firmware validation successful" << endl;
    return true;
}

bool OpenWrtFwUpdater::checkFlashSpace() {
    cout << "OpenWrtFwUpdater: Checking flash space..." << endl;

    long firmwareSize = getFileSize(_firmwarePath);
    if (firmwareSize < 0) {
        cout << "OpenWrtFwUpdater: ERROR - Could not get firmware size" << endl;
        return false;
    }

    long availableSpace = getAvailableSpace(FIRMWARE_TEMP_DIR);
    if (availableSpace < 0) {
        cout << "OpenWrtFwUpdater: ERROR - Could not get available space" << endl;
        return false;
    }

    // Convert to KB for comparison
    long firmwareSizeKB = firmwareSize / 1024;
    long availableSpaceKB = availableSpace / 1024;
    long requiredSpaceKB = firmwareSizeKB + MIN_FLASH_MARGIN_KB;

    cout << "OpenWrtFwUpdater: Firmware size: " << firmwareSizeKB << " KB" << endl;
    cout << "OpenWrtFwUpdater: Available space: " << availableSpaceKB << " KB" << endl;
    cout << "OpenWrtFwUpdater: Required space: " << requiredSpaceKB << " KB" << endl;

    if (availableSpaceKB < requiredSpaceKB) {
        cout << "OpenWrtFwUpdater: ERROR - Insufficient space" << endl;
        return false;
    }

    cout << "OpenWrtFwUpdater: Flash space check passed" << endl;
    return true;
}

bool OpenWrtFwUpdater::extractMetadata() {
    // Try to extract metadata from the firmware image
    // OpenWRT firmware images may contain metadata in various formats
    // This is a simplified implementation

    // Try to get package name from image
    std::string cmd = "strings " + _firmwarePath + " | grep -i 'openwrt\\|version' | head -1";
    std::string output;

    if (executeCommand(cmd, output) == 0 && !output.empty()) {
        // Remove newline
        if (output[output.length()-1] == '\n') {
            output.erase(output.length()-1);
        }

        std::lock_guard<std::mutex> lock(_mutex);

        // Extract version if present
        size_t versionPos = output.find("version");
        if (versionPos != std::string::npos) {
            _pkgVersion = output.substr(versionPos + 7);
            // Trim whitespace
            _pkgVersion.erase(0, _pkgVersion.find_first_not_of(" \t\n\r"));
        }

        // Set package name
        if (_pkgName.empty()) {
            _pkgName = "OpenWRT";
        }

        cout << "OpenWrtFwUpdater: Extracted metadata - Name: " << _pkgName
             << ", Version: " << _pkgVersion << endl;
    }

    // Also try to read from metadata file if it exists
    std::ifstream metaFile(METADATA_FILE);
    if (metaFile.is_open()) {
        std::string line;
        while (std::getline(metaFile, line)) {
            if (line.find("name=") == 0) {
                std::lock_guard<std::mutex> lock(_mutex);
                _pkgName = line.substr(5);
            } else if (line.find("version=") == 0) {
                std::lock_guard<std::mutex> lock(_mutex);
                _pkgVersion = line.substr(8);
            }
        }
        metaFile.close();
    }

    return true;
}

void OpenWrtFwUpdater::executeSysupgrade() {
    cout << "OpenWrtFwUpdater: Executing sysupgrade..." << endl;

    // Build sysupgrade command
    // -v: verbose
    // -n: do not save configuration over reflash (optional, can be removed)
    std::string cmd = "sysupgrade -v " + _firmwarePath + " &";

    cout << "OpenWrtFwUpdater: Command: " << cmd << endl;

    // Execute sysupgrade
    // Note: This will trigger a system reboot
    int result = system(cmd.c_str());

    if (result != 0) {
        cout << "OpenWrtFwUpdater: ERROR - sysupgrade command failed with code: " << result << endl;
        throw std::runtime_error("sysupgrade execution failed");
    }

    // Give sysupgrade time to start
    sleep(2);

    cout << "OpenWrtFwUpdater: sysupgrade initiated" << endl;
}

void OpenWrtFwUpdater::cleanup() {
    // Clean up temporary files
    if (fileExists(_firmwarePath)) {
        remove(_firmwarePath.c_str());
    }
    if (fileExists(METADATA_FILE)) {
        remove(METADATA_FILE);
    }
}

int OpenWrtFwUpdater::executeCommand(const std::string& cmd, std::string& output) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        return -1;
    }

    char buffer[256];
    output.clear();

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

    int result = pclose(pipe);
    return WEXITSTATUS(result);
}

bool OpenWrtFwUpdater::fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

long OpenWrtFwUpdater::getFileSize(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return st.st_size;
    }
    return -1;
}

long OpenWrtFwUpdater::getAvailableSpace(const std::string& path) {
    struct statvfs st;
    if (statvfs(path.c_str(), &st) == 0) {
        return st.f_bavail * st.f_frsize;
    }
    return -1;
}
