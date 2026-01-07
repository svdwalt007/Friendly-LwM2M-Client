/**
 * example_usage.cpp
 *
 * Example demonstrating how to integrate OpenWRT firmware update
 * functionality into a LWM2M client application
 */

#include "WppClient.h"
#include "OpenWrtFwInit.h"
#include "o_5_firmware_update/FirmwareUpdate.h"
#include "m_3_device/Device.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace wpp;
using namespace std;

// Global variables
static bool g_running = true;
static OpenWrtFwManager g_fwManager;

// Signal handler
void signalHandler(int signal) {
    cout << "Signal " << signal << " received, shutting down..." << endl;
    g_running = false;
}

// Initialize LWM2M objects
bool initObjects(WppClient &client) {
    cout << "Initializing LWM2M objects..." << endl;

    // Register and initialize Device object
    if (!client.registry().registerObj(Device::object(client))) {
        cerr << "Failed to register Device object" << endl;
        return false;
    }

    Device *device = Device::createInst(client);
    if (!device) {
        cerr << "Failed to create Device instance" << endl;
        return false;
    }

    // Set device information
    device->set<STRING_T>(Device::MANUFACTURER_0, "OpenWRT");
    device->set<STRING_T>(Device::MODEL_NUMBER_1, "One Router");
    device->set<STRING_T>(Device::SERIAL_NUMBER_2, "123456789");
    device->set<STRING_T>(Device::FIRMWARE_VERSION_3, "1.0.0");
    device->set<STRING_T>(Device::DEVICE_TYPE_17, "Router");

    // Set reboot handler
    device->set<EXECUTE_T>(Device::REBOOT_4, [](Instance& inst, ID_T resId, const OPAQUE_T& data) {
        cout << "Device: Reboot requested" << endl;
        system("reboot");
        return true;
    });

    cout << "Device object initialized" << endl;

    // Initialize OpenWRT Firmware Update
    if (!initOpenWrtFirmwareUpdate(client, g_fwManager)) {
        cerr << "Failed to initialize firmware update" << endl;
        return false;
    }

    cout << "Firmware update initialized" << endl;

    return true;
}

// Monitor firmware update state
void monitorFirmwareUpdate(WppClient &client) {
    static INT_T lastState = -1;
    static INT_T lastResult = -1;

    FirmwareUpdate *fw = FirmwareUpdate::instance(client);
    if (!fw) return;

    INT_T currentState = fw->get<INT_T>(FirmwareUpdate::STATE_3);
    INT_T currentResult = fw->get<INT_T>(FirmwareUpdate::UPDATE_RESULT_5);

    // Report state changes
    if (currentState != lastState) {
        cout << "Firmware Update State changed: ";
        switch (currentState) {
            case S_IDLE:
                cout << "IDLE";
                break;
            case S_DOWNLOADING:
                cout << "DOWNLOADING";
                break;
            case S_DOWNLOADED:
                cout << "DOWNLOADED";
                break;
            case S_UPDATING:
                cout << "UPDATING";
                break;
            default:
                cout << "UNKNOWN(" << currentState << ")";
        }
        cout << endl;
        lastState = currentState;
    }

    // Report result changes
    if (currentResult != lastResult) {
        cout << "Firmware Update Result changed: ";
        switch (currentResult) {
            case R_INITIAL:
                cout << "Initial";
                break;
            case R_FW_UPD_SUCCESS:
                cout << "Success";
                break;
            case R_NOT_ENOUGH_FLASH:
                cout << "Not enough flash";
                break;
            case R_OUT_OF_RAM:
                cout << "Out of RAM";
                break;
            case R_CONN_LOST:
                cout << "Connection lost";
                break;
            case R_INTEGRITY_CHECK_FAIL:
                cout << "Integrity check failed";
                break;
            case R_UNSUPPORTED_PKG_TYPE:
                cout << "Unsupported package type";
                break;
            case R_INVALID_URI:
                cout << "Invalid URI";
                break;
            case R_FW_UPD_FAIL:
                cout << "Update failed";
                break;
            case R_UNSUPPORTED_PROTOCOL:
                cout << "Unsupported protocol";
                break;
            default:
                cout << "UNKNOWN(" << currentResult << ")";
        }
        cout << endl;
        lastResult = currentResult;

        // Display additional information
        #if RES_5_6
        STRING_T pkgName = fw->get<STRING_T>(FirmwareUpdate::PKGNAME_6);
        if (!pkgName.empty()) {
            cout << "  Package Name: " << pkgName << endl;
        }
        #endif

        #if RES_5_7
        STRING_T pkgVersion = fw->get<STRING_T>(FirmwareUpdate::PKGVERSION_7);
        if (!pkgVersion.empty()) {
            cout << "  Package Version: " << pkgVersion << endl;
        }
        #endif
    }
}

int main(int argc, char *argv[]) {
    cout << "========================================" << endl;
    cout << "OpenWRT LWM2M Client with FOTA Support" << endl;
    cout << "========================================" << endl;

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        // Create connection (example using default port)
        // In production, this should be configured from command line or config file
        Connection connection("56830", AF_INET);

        // Create LWM2M client
        WppClient *client = WppClient::create(
            {"OpenWrtRouter", "", ""},  // Endpoint name, manufacturer, model
            connection,
            nullptr  // Error handler (optional)
        );

        if (!client) {
            cerr << "Failed to create LWM2M client" << endl;
            return 1;
        }

        cout << "LWM2M client created" << endl;

        // Initialize objects
        if (!initObjects(*client)) {
            cerr << "Failed to initialize objects" << endl;
            return 1;
        }

        // Give ownership to registry
        // After this call, the registry manages the lifecycle of all objects
        client->giveOwnership();

        cout << "Client initialized successfully" << endl;
        cout << "Waiting for LWM2M server connection..." << endl;

        // Main loop
        int loopCounter = 0;
        while (g_running) {
            // Process LWM2M client events
            client->loop();

            // Monitor firmware update state periodically
            if (loopCounter % 10 == 0) {  // Every 10 iterations
                monitorFirmwareUpdate(*client);
            }

            loopCounter++;
            sleep(1);
        }

        cout << "Shutting down..." << endl;

    } catch (const std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    cout << "Goodbye!" << endl;
    return 0;
}
