/**
 * OpenWrtFwInit.cpp
 *
 * Implementation of OpenWRT firmware update initialization helpers
 */

#include "OpenWrtFwInit.h"
#include "o_5_firmware_update/FirmwareUpdate.h"
#include <iostream>

using namespace std;

OpenWrtFwManager::OpenWrtFwManager()
    : _updater(nullptr)
    , _downloader(nullptr)
#if RES_5_8
    , _uriDownloader(nullptr)
#endif
{
}

OpenWrtFwManager::~OpenWrtFwManager() {
    // Smart pointers will handle cleanup
}

bool OpenWrtFwManager::initialize(WppClient &client) {
    cout << "OpenWrtFwManager: Initializing firmware update..." << endl;

    try {
        // Create updater first (downloader needs reference to it)
        _updater = std::make_unique<OpenWrtFwUpdater>();

        // Create downloaders
        _downloader = std::make_unique<OpenWrtFwDownloader>(*_updater);

#if RES_5_8
        _uriDownloader = std::make_unique<OpenWrtFwUriDownloader>(*_updater);
#endif

        // Register Firmware Update object
        if (!client.registry().registerObj(FirmwareUpdate::object(client))) {
            cerr << "OpenWrtFwManager: ERROR - Failed to register Firmware Update object" << endl;
            return false;
        }

        // Create Firmware Update instance
        if (!FirmwareUpdate::createInst(client)) {
            cerr << "OpenWrtFwManager: ERROR - Failed to create Firmware Update instance" << endl;
            return false;
        }

        // Set firmware updater
        if (!FirmwareUpdate::setFwUpdater(client, *_updater)) {
            cerr << "OpenWrtFwManager: ERROR - Failed to set firmware updater" << endl;
            return false;
        }

        // Set internal downloader (PUSH method)
        if (!FirmwareUpdate::setFwInternalDownloader(client, *_downloader)) {
            cerr << "OpenWrtFwManager: ERROR - Failed to set internal downloader" << endl;
            return false;
        }

#if RES_5_8
        // Set external URI downloader (PULL method)
        if (!FirmwareUpdate::setFwExternalDownloader(client, *_uriDownloader)) {
            cerr << "OpenWrtFwManager: ERROR - Failed to set external downloader" << endl;
            return false;
        }

        // Log supported protocols
        auto protocols = FirmwareUpdate::supportedProtocols(client);
        cout << "OpenWrtFwManager: Supported protocols: ";
        for (auto proto : protocols) {
            switch (proto) {
                case FwUpdProtocol::COAP: cout << "CoAP "; break;
                case FwUpdProtocol::COAPS: cout << "CoAPs "; break;
                case FwUpdProtocol::HTTP: cout << "HTTP "; break;
                case FwUpdProtocol::HTTPS: cout << "HTTPS "; break;
                case FwUpdProtocol::COAP_TCP: cout << "CoAP+TCP "; break;
                case FwUpdProtocol::COAP_TLS: cout << "CoAPs+TCP "; break;
                default: break;
            }
        }
        cout << endl;
#endif

        cout << "OpenWrtFwManager: Firmware update initialized successfully" << endl;
        return true;

    } catch (const std::exception& e) {
        cerr << "OpenWrtFwManager: ERROR - Exception during initialization: " << e.what() << endl;
        return false;
    }
}

bool initOpenWrtFirmwareUpdate(WppClient &client, OpenWrtFwManager &manager) {
    return manager.initialize(client);
}
