/**
 * OpenWrtFwUriDownloader.cpp
 *
 * Implementation of OpenWRT-specific firmware external URI downloader
 * for PULL method (firmware downloaded from URI via Package URI resource)
 */

#include "OpenWrtFwUriDownloader.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>

#if RES_5_8

OpenWrtFwUriDownloader::OpenWrtFwUriDownloader(OpenWrtFwUpdater& updater)
    : _updater(updater)
    , _downloadResult(FwUpdRes::R_INITIAL)
    , _downloadCompleted(false)
    , _hasWget(false)
    , _hasCurl(false)
    , _hasCoap(false)
{
    _firmwarePath = std::string(FIRMWARE_TEMP_DIR) + "/" + FIRMWARE_FILE_NAME;

    // Check available download tools
    _hasWget = isToolAvailable("wget");
    _hasCurl = isToolAvailable("curl");
    _hasCoap = isToolAvailable("coap-client");

    cout << "OpenWrtFwUriDownloader: Initialized" << endl;
    cout << "OpenWrtFwUriDownloader: wget available: " << (_hasWget ? "yes" : "no") << endl;
    cout << "OpenWrtFwUriDownloader: curl available: " << (_hasCurl ? "yes" : "no") << endl;
    cout << "OpenWrtFwUriDownloader: coap-client available: " << (_hasCoap ? "yes" : "no") << endl;
}

OpenWrtFwUriDownloader::~OpenWrtFwUriDownloader() {
    if (_downloadThread.joinable()) {
        _downloadThread.join();
    }
    cleanup();
}

std::vector<FwUpdProtocol> OpenWrtFwUriDownloader::supportedProtocols() {
    std::vector<FwUpdProtocol> protocols;

    // HTTP/HTTPS supported if wget or curl available
    if (_hasWget || _hasCurl) {
        protocols.push_back(FwUpdProtocol::HTTP);
        protocols.push_back(FwUpdProtocol::HTTPS);
    }

    // CoAP/CoAPS supported if coap-client available
    if (_hasCoap) {
        protocols.push_back(FwUpdProtocol::COAP);
        protocols.push_back(FwUpdProtocol::COAPS);
    }

    return protocols;
}

void OpenWrtFwUriDownloader::startDownloading(const STRING_T &uri, Lwm2mSecurity &security) {
    std::lock_guard<std::mutex> lock(_mutex);

    cout << "OpenWrtFwUriDownloader: Starting download from URI: " << uri << endl;

    // Clean up any previous download
    cleanup();

    // Reset state
    _downloadCompleted = false;
    _downloadResult = FwUpdRes::R_INITIAL;

    // Start download in background thread
    // We need to copy the security object data we need before starting the thread
    _downloadThread = std::thread(&OpenWrtFwUriDownloader::performDownload, this, uri, &security);
}

bool OpenWrtFwUriDownloader::isDownloaded() {
    return _downloadCompleted.load();
}

FwUpdRes OpenWrtFwUriDownloader::downloadResult() {
    std::lock_guard<std::mutex> lock(_mutex);
    return _downloadResult;
}

void OpenWrtFwUriDownloader::reset() {
    std::lock_guard<std::mutex> lock(_mutex);

    cout << "OpenWrtFwUriDownloader: Reset" << endl;

    // Wait for download thread to complete
    if (_downloadThread.joinable()) {
        _downloadThread.join();
    }

    _downloadCompleted = false;
    _downloadResult = FwUpdRes::R_INITIAL;

    cleanup();
}

void OpenWrtFwUriDownloader::performDownload(const std::string& uri, Lwm2mSecurity* security) {
    cout << "OpenWrtFwUriDownloader: Performing download..." << endl;

    try {
        bool success = false;
        std::string protocol = getProtocolFromUri(uri);

        if (protocol == "http" || protocol == "https") {
            // Try wget first, then curl
            if (_hasWget) {
                success = downloadWithWget(uri, security);
            } else if (_hasCurl) {
                success = downloadWithCurl(uri, security);
            } else {
                cout << "OpenWrtFwUriDownloader: ERROR - No HTTP download tool available" << endl;
                std::lock_guard<std::mutex> lock(_mutex);
                _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
                _downloadCompleted = true;
                return;
            }
        } else if (protocol == "coap" || protocol == "coaps") {
            if (_hasCoap) {
                success = downloadWithCoap(uri, security);
            } else {
                cout << "OpenWrtFwUriDownloader: ERROR - CoAP client not available" << endl;
                std::lock_guard<std::mutex> lock(_mutex);
                _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
                _downloadCompleted = true;
                return;
            }
        } else {
            cout << "OpenWrtFwUriDownloader: ERROR - Unsupported protocol: " << protocol << endl;
            std::lock_guard<std::mutex> lock(_mutex);
            _downloadResult = FwUpdRes::R_UNSUPPORTED_PROTOCOL;
            _downloadCompleted = true;
            return;
        }

        if (success) {
            // Verify the download
            if (verifyDownload()) {
                // Move temp file to final location
                if (rename(FIRMWARE_TEMP_FILE, _firmwarePath.c_str()) == 0) {
                    cout << "OpenWrtFwUriDownloader: Download successful" << endl;
                    _updater.setFirmwarePath(_firmwarePath);

                    std::lock_guard<std::mutex> lock(_mutex);
                    _downloadResult = FwUpdRes::R_INITIAL; // Success
                } else {
                    cout << "OpenWrtFwUriDownloader: ERROR - Failed to move downloaded file" << endl;
                    std::lock_guard<std::mutex> lock(_mutex);
                    _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
                }
            } else {
                // verifyDownload sets appropriate error
                cleanup();
            }
        }

        _downloadCompleted = true;

    } catch (const std::exception& e) {
        cout << "OpenWrtFwUriDownloader: ERROR - Exception: " << e.what() << endl;
        std::lock_guard<std::mutex> lock(_mutex);
        _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        _downloadCompleted = true;
        cleanup();
    }
}

bool OpenWrtFwUriDownloader::downloadWithWget(const std::string& uri, Lwm2mSecurity* security) {
    cout << "OpenWrtFwUriDownloader: Downloading with wget..." << endl;

    // Build wget command
    std::stringstream cmd;
    cmd << "wget";
    cmd << " --timeout=" << DOWNLOAD_TIMEOUT_SEC;
    cmd << " --tries=3";
    cmd << " -O " << FIRMWARE_TEMP_FILE;

    // Add security options for HTTPS
    if (uri.find("https://") == 0) {
        // For production, you might want to use certificates from security object
        // For now, we'll use system certificates
        cmd << " --ca-certificate=/etc/ssl/certs/ca-certificates.crt";
    }

    cmd << " \"" << uri << "\"";
    cmd << " 2>&1";

    cout << "OpenWrtFwUriDownloader: Command: " << cmd.str() << endl;

    std::string output;
    int result = executeCommand(cmd.str(), output);

    if (result != 0) {
        cout << "OpenWrtFwUriDownloader: ERROR - wget failed: " << output << endl;

        std::lock_guard<std::mutex> lock(_mutex);
        if (output.find("Connection") != std::string::npos ||
            output.find("network") != std::string::npos ||
            output.find("timeout") != std::string::npos) {
            _downloadResult = FwUpdRes::R_CONN_LOST;
        } else if (output.find("404") != std::string::npos ||
                   output.find("not found") != std::string::npos) {
            _downloadResult = FwUpdRes::R_INVALID_URI;
        } else {
            _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        }
        return false;
    }

    return true;
}

bool OpenWrtFwUriDownloader::downloadWithCurl(const std::string& uri, Lwm2mSecurity* security) {
    cout << "OpenWrtFwUriDownloader: Downloading with curl..." << endl;

    // Build curl command
    std::stringstream cmd;
    cmd << "curl";
    cmd << " --max-time " << DOWNLOAD_TIMEOUT_SEC;
    cmd << " --retry 3";
    cmd << " -L";  // Follow redirects
    cmd << " -o " << FIRMWARE_TEMP_FILE;

    // Add security options for HTTPS
    if (uri.find("https://") == 0) {
        cmd << " --cacert /etc/ssl/certs/ca-certificates.crt";
    }

    cmd << " \"" << uri << "\"";
    cmd << " 2>&1";

    cout << "OpenWrtFwUriDownloader: Command: " << cmd.str() << endl;

    std::string output;
    int result = executeCommand(cmd.str(), output);

    if (result != 0) {
        cout << "OpenWrtFwUriDownloader: ERROR - curl failed: " << output << endl;

        std::lock_guard<std::mutex> lock(_mutex);
        if (output.find("Connection") != std::string::npos ||
            output.find("network") != std::string::npos ||
            output.find("timeout") != std::string::npos) {
            _downloadResult = FwUpdRes::R_CONN_LOST;
        } else if (output.find("404") != std::string::npos ||
                   output.find("not found") != std::string::npos) {
            _downloadResult = FwUpdRes::R_INVALID_URI;
        } else {
            _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        }
        return false;
    }

    return true;
}

bool OpenWrtFwUriDownloader::downloadWithCoap(const std::string& uri, Lwm2mSecurity* security) {
    cout << "OpenWrtFwUriDownloader: Downloading with CoAP..." << endl;

    // Build coap-client command
    std::stringstream cmd;
    cmd << "coap-client";
    cmd << " -m get";
    cmd << " -o " << FIRMWARE_TEMP_FILE;
    cmd << " \"" << uri << "\"";
    cmd << " 2>&1";

    cout << "OpenWrtFwUriDownloader: Command: " << cmd.str() << endl;

    std::string output;
    int result = executeCommand(cmd.str(), output);

    if (result != 0) {
        cout << "OpenWrtFwUriDownloader: ERROR - coap-client failed: " << output << endl;

        std::lock_guard<std::mutex> lock(_mutex);
        if (output.find("timeout") != std::string::npos ||
            output.find("network") != std::string::npos) {
            _downloadResult = FwUpdRes::R_CONN_LOST;
        } else if (output.find("not found") != std::string::npos) {
            _downloadResult = FwUpdRes::R_INVALID_URI;
        } else {
            _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        }
        return false;
    }

    return true;
}

bool OpenWrtFwUriDownloader::verifyDownload() {
    cout << "OpenWrtFwUriDownloader: Verifying download..." << endl;

    struct stat st;
    if (stat(FIRMWARE_TEMP_FILE, &st) != 0) {
        cout << "OpenWrtFwUriDownloader: ERROR - Downloaded file not found" << endl;
        std::lock_guard<std::mutex> lock(_mutex);
        _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        return false;
    }

    // Check file size
    if (st.st_size == 0) {
        cout << "OpenWrtFwUriDownloader: ERROR - Downloaded file is empty" << endl;
        std::lock_guard<std::mutex> lock(_mutex);
        _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
        return false;
    }

    // Check minimum firmware size
    if (st.st_size < 1024) {
        cout << "OpenWrtFwUriDownloader: ERROR - Firmware too small" << endl;
        std::lock_guard<std::mutex> lock(_mutex);
        _downloadResult = FwUpdRes::R_UNSUPPORTED_PKG_TYPE;
        return false;
    }

    cout << "OpenWrtFwUriDownloader: Downloaded file size: " << st.st_size << " bytes" << endl;
    cout << "OpenWrtFwUriDownloader: Verification passed" << endl;
    return true;
}

void OpenWrtFwUriDownloader::cleanup() {
    // Remove temporary file
    if (access(FIRMWARE_TEMP_FILE, F_OK) == 0) {
        remove(FIRMWARE_TEMP_FILE);
    }
}

int OpenWrtFwUriDownloader::executeCommand(const std::string& cmd, std::string& output) {
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

bool OpenWrtFwUriDownloader::isToolAvailable(const std::string& tool) {
    std::string cmd = "which " + tool + " > /dev/null 2>&1";
    return system(cmd.c_str()) == 0;
}

std::string OpenWrtFwUriDownloader::getProtocolFromUri(const std::string& uri) {
    size_t pos = uri.find("://");
    if (pos != std::string::npos) {
        return uri.substr(0, pos);
    }
    return "";
}

#endif // RES_5_8
