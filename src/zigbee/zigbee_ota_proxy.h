/*
 * Zigbee OTA Upgrade Proxy
 *
 * Provides OTA (Over-The-Air) firmware upgrade proxy functionality for Zigbee devices.
 * Supports:
 * - Image announcement to devices
 * - Block-by-block transfer with retry
 * - Upgrade end handling
 * - Multi-device update coordination
 * - Image verification and integrity checking
 */

#ifndef ZIGBEE_OTA_PROXY_H
#define ZIGBEE_OTA_PROXY_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <cstdint>
#include <functional>
#include <mutex>
#include <fstream>

namespace zigbee {

// Forward declarations
class ZigbeeCoordinator;

/**
 * @brief OTA image type enumeration
 */
enum class OtaImageType : uint16_t {
    UPGRADE_IMAGE = 0x0000,
    PICTURE = 0x0001,
    SECURITY_CREDENTIAL = 0xFFC0,
    CONFIGURATION = 0xFFC1,
    LOG = 0xFFC2,
    WILDCARD = 0xFFFF
};

/**
 * @brief OTA upgrade status
 */
enum class OtaUpgradeStatus {
    IDLE,
    ANNOUNCING,
    TRANSFERRING,
    VERIFYING,
    APPLYING,
    COMPLETED,
    FAILED
};

/**
 * @brief OTA image header structure
 */
struct OtaImageHeader {
    uint32_t magicNumber;           // 0x0BEEF11E
    uint16_t headerVersion;         // OTA header version
    uint16_t headerLength;          // Total header length
    uint16_t fieldControl;          // Bit mask of present fields
    uint16_t manufacturerCode;      // Manufacturer code
    uint16_t imageType;             // Image type
    uint32_t fileVersion;           // File version
    uint16_t zigbeeStackVersion;    // Zigbee stack version
    char headerString[32];          // Header string (null-terminated)
    uint32_t totalImageSize;        // Total image size (including header)

    // Optional fields (controlled by fieldControl)
    bool hasSecurityCredentials;
    bool hasDeviceSpecificFile;
    bool hasHardwareVersions;

    uint16_t minHardwareVersion;
    uint16_t maxHardwareVersion;

    OtaImageHeader()
        : magicNumber(0x0BEEF11E),
          headerVersion(0x0100),
          headerLength(56),
          fieldControl(0),
          manufacturerCode(0),
          imageType(0),
          fileVersion(0),
          zigbeeStackVersion(2),
          totalImageSize(0),
          hasSecurityCredentials(false),
          hasDeviceSpecificFile(false),
          hasHardwareVersions(false),
          minHardwareVersion(0),
          maxHardwareVersion(0) {
        std::memset(headerString, 0, sizeof(headerString));
    }
};

/**
 * @brief OTA upgrade session for a device
 */
struct OtaUpgradeSession {
    uint64_t ieeeAddress;
    uint8_t endpoint;
    OtaImageHeader imageHeader;
    std::string imagePath;
    uint32_t currentOffset;
    uint32_t totalSize;
    OtaUpgradeStatus status;
    uint64_t startTime;
    uint64_t lastActivityTime;
    uint16_t retriesRemaining;
    uint8_t blockSize;

    OtaUpgradeSession()
        : ieeeAddress(0),
          endpoint(0),
          currentOffset(0),
          totalSize(0),
          status(OtaUpgradeStatus::IDLE),
          startTime(0),
          lastActivityTime(0),
          retriesRemaining(3),
          blockSize(64) {}
};

/**
 * @brief Zigbee OTA Proxy Class
 */
class ZigbeeOtaProxy {
public:
    /**
     * @brief OTA event callbacks
     */
    using OtaProgressCallback = std::function<void(uint64_t ieeeAddress, uint32_t offset, uint32_t total)>;
    using OtaCompleteCallback = std::function<void(uint64_t ieeeAddress, bool success)>;
    using OtaErrorCallback = std::function<void(uint64_t ieeeAddress, const std::string& error)>;

    /**
     * @brief Constructor
     * @param coordinator Zigbee coordinator instance
     */
    explicit ZigbeeOtaProxy(ZigbeeCoordinator* coordinator);

    /**
     * @brief Destructor
     */
    ~ZigbeeOtaProxy();

    /**
     * @brief Load OTA image file
     * @param imagePath Path to OTA image file
     * @return true if successful
     */
    bool loadImage(const std::string& imagePath);

    /**
     * @brief Announce image availability to all devices
     * @param manufacturerCode Manufacturer code filter (0xFFFF for all)
     * @param imageType Image type filter (0xFFFF for all)
     * @return true if successful
     */
    bool announceImage(uint16_t manufacturerCode = 0xFFFF, uint16_t imageType = 0xFFFF);

    /**
     * @brief Announce image to specific device
     * @param ieeeAddress Target device IEEE address
     * @param endpoint Target endpoint
     * @return true if successful
     */
    bool announceImageToDevice(uint64_t ieeeAddress, uint8_t endpoint);

    /**
     * @brief Start OTA upgrade for a device
     * @param ieeeAddress Target device IEEE address
     * @param endpoint Target endpoint
     * @param imagePath Path to OTA image file
     * @return true if successful
     */
    bool startUpgrade(uint64_t ieeeAddress, uint8_t endpoint, const std::string& imagePath);

    /**
     * @brief Cancel ongoing upgrade for a device
     * @param ieeeAddress Target device IEEE address
     * @return true if successful
     */
    bool cancelUpgrade(uint64_t ieeeAddress);

    /**
     * @brief Handle query next image request from device
     * @param ieeeAddress Source device IEEE address
     * @param endpoint Source endpoint
     * @param manufacturerCode Device manufacturer code
     * @param imageType Device image type
     * @param currentFileVersion Device current file version
     * @param hardwareVersion Device hardware version
     * @return true if handled
     */
    bool handleQueryNextImageRequest(uint64_t ieeeAddress, uint8_t endpoint,
                                    uint16_t manufacturerCode, uint16_t imageType,
                                    uint32_t currentFileVersion, uint16_t hardwareVersion);

    /**
     * @brief Handle image block request from device
     * @param ieeeAddress Source device IEEE address
     * @param endpoint Source endpoint
     * @param fileOffset Requested file offset
     * @param maxDataSize Maximum block size
     * @return true if handled
     */
    bool handleImageBlockRequest(uint64_t ieeeAddress, uint8_t endpoint,
                                 uint32_t fileOffset, uint8_t maxDataSize);

    /**
     * @brief Handle upgrade end request from device
     * @param ieeeAddress Source device IEEE address
     * @param endpoint Source endpoint
     * @param status Upgrade status from device
     * @return true if handled
     */
    bool handleUpgradeEndRequest(uint64_t ieeeAddress, uint8_t endpoint, uint8_t status);

    /**
     * @brief Get active upgrade sessions
     * @return Vector of active sessions
     */
    std::vector<OtaUpgradeSession> getActiveSessions() const;

    /**
     * @brief Get upgrade session for device
     * @param ieeeAddress Device IEEE address
     * @return Pointer to session (nullptr if not found)
     */
    const OtaUpgradeSession* getSession(uint64_t ieeeAddress) const;

    /**
     * @brief Get OTA image header
     * @return Image header
     */
    OtaImageHeader getImageHeader() const;

    /**
     * @brief Check if image is loaded
     * @return true if image is loaded
     */
    bool isImageLoaded() const;

    /**
     * @brief Register progress callback
     * @param callback Callback function
     */
    void onProgress(OtaProgressCallback callback);

    /**
     * @brief Register completion callback
     * @param callback Callback function
     */
    void onComplete(OtaCompleteCallback callback);

    /**
     * @brief Register error callback
     * @param callback Callback function
     */
    void onError(OtaErrorCallback callback);

    /**
     * @brief Set maximum concurrent upgrades
     * @param maxConcurrent Maximum number of concurrent upgrades
     */
    void setMaxConcurrentUpgrades(size_t maxConcurrent);

    /**
     * @brief Set block size for transfers
     * @param blockSize Block size in bytes (default: 64)
     */
    void setBlockSize(uint8_t blockSize);

    /**
     * @brief Set retry count
     * @param retries Number of retries per block (default: 3)
     */
    void setRetryCount(uint16_t retries);

private:
    /**
     * @brief Parse OTA image header
     * @param imagePath Path to image file
     * @return true if successful
     */
    bool parseImageHeader(const std::string& imagePath);

    /**
     * @brief Read image block
     * @param offset File offset
     * @param size Block size
     * @param data Output buffer
     * @return true if successful
     */
    bool readImageBlock(uint32_t offset, size_t size, std::vector<uint8_t>& data);

    /**
     * @brief Send query next image response
     * @param ieeeAddress Target device
     * @param endpoint Target endpoint
     * @param status Response status
     * @return true if successful
     */
    bool sendQueryNextImageResponse(uint64_t ieeeAddress, uint8_t endpoint, uint8_t status);

    /**
     * @brief Send image block response
     * @param ieeeAddress Target device
     * @param endpoint Target endpoint
     * @param status Response status
     * @param fileOffset Block offset
     * @param data Block data
     * @return true if successful
     */
    bool sendImageBlockResponse(uint64_t ieeeAddress, uint8_t endpoint, uint8_t status,
                               uint32_t fileOffset, const std::vector<uint8_t>& data);

    /**
     * @brief Send upgrade end response
     * @param ieeeAddress Target device
     * @param endpoint Target endpoint
     * @param status Response status
     * @param upgradeTime Time until upgrade (seconds)
     * @return true if successful
     */
    bool sendUpgradeEndResponse(uint64_t ieeeAddress, uint8_t endpoint, uint8_t status, uint32_t upgradeTime);

    /**
     * @brief Create or get session for device
     * @param ieeeAddress Device IEEE address
     * @param endpoint Device endpoint
     * @return Pointer to session
     */
    OtaUpgradeSession* createSession(uint64_t ieeeAddress, uint8_t endpoint);

    /**
     * @brief Remove session for device
     * @param ieeeAddress Device IEEE address
     */
    void removeSession(uint64_t ieeeAddress);

    /**
     * @brief Update session activity time
     * @param ieeeAddress Device IEEE address
     */
    void updateSessionActivity(uint64_t ieeeAddress);

    /**
     * @brief Check for stale sessions and clean up
     */
    void cleanupStaleSessions();

    /**
     * @brief Validate image compatibility with device
     * @param manufacturerCode Device manufacturer code
     * @param imageType Device image type
     * @param hardwareVersion Device hardware version
     * @return true if compatible
     */
    bool validateImageCompatibility(uint16_t manufacturerCode, uint16_t imageType, uint16_t hardwareVersion);

    // Member variables
    ZigbeeCoordinator* coordinator_;

    std::string currentImagePath_;
    OtaImageHeader currentImageHeader_;
    bool imageLoaded_;

    std::map<uint64_t, OtaUpgradeSession> activeSessions_;
    mutable std::mutex sessionsMutex_;

    size_t maxConcurrentUpgrades_;
    uint8_t defaultBlockSize_;
    uint16_t defaultRetryCount_;

    // Callbacks
    OtaProgressCallback progressCallback_;
    OtaCompleteCallback completeCallback_;
    OtaErrorCallback errorCallback_;

    // Constants
    static constexpr uint32_t OTA_MAGIC_NUMBER = 0x0BEEF11E;
    static constexpr uint32_t SESSION_TIMEOUT_MS = 300000;  // 5 minutes
    static constexpr uint8_t MAX_BLOCK_SIZE = 128;
    static constexpr uint8_t MIN_BLOCK_SIZE = 32;
};

} // namespace zigbee

#endif // ZIGBEE_OTA_PROXY_H
