/**
 * OpenWrtFwInit.h
 *
 * Helper functions for initializing OpenWRT firmware update functionality
 * Provides easy integration into existing LWM2M client applications
 */

#ifndef OPENWRT_FW_INIT_H_
#define OPENWRT_FW_INIT_H_

#include "WppClient.h"
#include "OpenWrtFwUpdater.h"
#include "OpenWrtFwDownloader.h"
#include "OpenWrtFwUriDownloader.h"
#include <memory>

using namespace wpp;

/**
 * @brief OpenWRT Firmware Update Manager
 *
 * This class manages the lifecycle of OpenWRT firmware update components
 * and provides a simple interface for initialization.
 *
 * Usage example:
 * @code
 * // In your main application:
 * static OpenWrtFwManager fwManager;
 *
 * void setup() {
 *     WppClient client = ...;
 *     fwManager.initialize(client);
 * }
 * @endcode
 */
class OpenWrtFwManager {
public:
    OpenWrtFwManager();
    ~OpenWrtFwManager();

    /**
     * @brief Initialize OpenWRT firmware update functionality
     * @param client Reference to the WppClient instance
     * @return true if initialization was successful, false otherwise
     */
    bool initialize(WppClient &client);

    /**
     * @brief Get the firmware updater instance
     * @return Reference to the OpenWrtFwUpdater
     */
    OpenWrtFwUpdater& getUpdater() { return *_updater; }

    /**
     * @brief Get the internal downloader instance
     * @return Reference to the OpenWrtFwDownloader
     */
    OpenWrtFwDownloader& getDownloader() { return *_downloader; }

#if RES_5_8
    /**
     * @brief Get the URI downloader instance
     * @return Reference to the OpenWrtFwUriDownloader
     */
    OpenWrtFwUriDownloader& getUriDownloader() { return *_uriDownloader; }
#endif

private:
    std::unique_ptr<OpenWrtFwUpdater> _updater;
    std::unique_ptr<OpenWrtFwDownloader> _downloader;
#if RES_5_8
    std::unique_ptr<OpenWrtFwUriDownloader> _uriDownloader;
#endif
};

/**
 * @brief Initialize OpenWRT firmware update for a LWM2M client
 *
 * This is a convenience function that sets up all firmware update components:
 * - Registers the Firmware Update object (Object 5)
 * - Creates a firmware update instance
 * - Sets up the firmware updater with sysupgrade integration
 * - Sets up internal downloader for PUSH method
 * - Sets up external URI downloader for PULL method (if RES_5_8 enabled)
 *
 * @param client Reference to the WppClient instance
 * @param manager Reference to the OpenWrtFwManager to store components
 * @return true if initialization was successful, false otherwise
 *
 * @note This function should be called during application initialization,
 *       after the client has been created but before giveOwnership() is called.
 *
 * Example usage:
 * @code
 * static OpenWrtFwManager fwManager;
 *
 * void initObjects(WppClient &client) {
 *     // Initialize other objects first...
 *     deviceInit(client);
 *     serverInit(client);
 *
 *     // Initialize OpenWRT firmware update
 *     if (!initOpenWrtFirmwareUpdate(client, fwManager)) {
 *         cerr << "Failed to initialize firmware update" << endl;
 *     }
 * }
 * @endcode
 */
bool initOpenWrtFirmwareUpdate(WppClient &client, OpenWrtFwManager &manager);

#endif // OPENWRT_FW_INIT_H_
