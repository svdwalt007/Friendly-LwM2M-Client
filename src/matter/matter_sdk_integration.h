/*
 * Matter SDK Integration Header
 * Friendly LwM2M Client - Matter 1.0/1.1 Controller Implementation
 *
 * Provides Matter controller functionality with:
 * - Device commissioning (BLE, WiFi, Thread)
 * - Session management
 * - Thread Border Router integration
 * - QR code and manual pairing support
 */

#ifndef MATTER_SDK_INTEGRATION_H
#define MATTER_SDK_INTEGRATION_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

// Conditional compilation for Matter SDK availability
#ifdef WITH_MATTER_SDK
#include <controller/CHIPDeviceController.h>
#include <controller/CommissioningDelegate.h>
#include <platform/CHIPDeviceLayer.h>
#include <credentials/FabricTable.h>
#include <crypto/CHIPCryptoPAL.h>
#endif

namespace matter {

// Forward declarations
class MatterClusterClient;
class ThreadNetworkManager;

// ============================================================================
// Type Definitions
// ============================================================================

using NodeId = uint64_t;
using EndpointId = uint16_t;
using ClusterId = uint32_t;
using AttributeId = uint32_t;
using CommandId = uint32_t;
using VendorId = uint16_t;
using ProductId = uint16_t;
using FabricIndex = uint8_t;

// ============================================================================
// Device Information
// ============================================================================

struct MatterDeviceInfo {
    NodeId nodeId;
    VendorId vendorId;
    ProductId productId;
    uint16_t deviceType;
    std::string serialNumber;
    std::string firmwareVersion;
    std::vector<EndpointId> endpoints;
    bool reachable;
    int8_t rssi;
    std::string ipAddress;
    uint64_t lastSeen;
};

// ============================================================================
// Commissioning Parameters
// ============================================================================

enum class CommissioningMethod {
    UNKNOWN = 0,
    QR_CODE,
    MANUAL_CODE,
    BLE,
    SOFT_AP,
    ON_NETWORK,
    THREAD
};

enum class CommissioningStatus {
    NOT_STARTED = 0,
    IN_PROGRESS,
    DEVICE_DISCOVERED,
    CONNECTING,
    AUTHENTICATING,
    CONFIGURING_NETWORK,
    OPERATIONAL_DISCOVERY,
    COMPLETED,
    FAILED,
    TIMEOUT,
    CANCELLED
};

struct CommissioningParams {
    CommissioningMethod method;
    std::string setupPayload;       // QR code or manual pairing code
    uint32_t setupPIN;
    uint16_t discriminator;

    // BLE parameters
    std::string bleDeviceName;
    uint64_t bleDeviceId;

    // Network parameters
    std::string wifiSSID;
    std::string wifiPassword;
    std::string threadOperationalDataset;

    // Thread Border Router parameters
    bool useThreadBorderRouter;
    std::string threadNetworkName;

    // Advanced options
    uint32_t timeoutSeconds;
    bool skipCommissioningComplete;
    bool attemptWiFiNetworkScan;
    bool attemptThreadNetworkScan;

    CommissioningParams()
        : method(CommissioningMethod::UNKNOWN)
        , setupPIN(0)
        , discriminator(0)
        , bleDeviceId(0)
        , useThreadBorderRouter(false)
        , timeoutSeconds(300)
        , skipCommissioningComplete(false)
        , attemptWiFiNetworkScan(false)
        , attemptThreadNetworkScan(false) {}
};

// ============================================================================
// Callbacks
// ============================================================================

using CommissioningStatusCallback = std::function<void(CommissioningStatus status, const std::string& message)>;
using DeviceDiscoveryCallback = std::function<void(const MatterDeviceInfo& device)>;
using DeviceStateChangeCallback = std::function<void(NodeId nodeId, bool online)>;
using ErrorCallback = std::function<void(int errorCode, const std::string& errorMessage)>;

// ============================================================================
// Matter Controller
// ============================================================================

class MatterController {
public:
    // Singleton access
    static MatterController& getInstance();

    // Prevent copying
    MatterController(const MatterController&) = delete;
    MatterController& operator=(const MatterController&) = delete;

    // ========================================================================
    // Initialization and Lifecycle
    // ========================================================================

    /**
     * Initialize Matter controller stack
     * @param fabricIndex Fabric index to use (default: 1)
     * @param storageDirectory Directory for persistent storage
     * @param vendorId Controller vendor ID
     * @param productId Controller product ID
     * @return true if initialization successful
     */
    bool initialize(FabricIndex fabricIndex = 1,
                   const std::string& storageDirectory = "/var/lib/matter",
                   VendorId vendorId = 0xFFF1,
                   ProductId productId = 0x8000);

    /**
     * Shutdown Matter controller and cleanup resources
     */
    void shutdown();

    /**
     * Check if controller is initialized
     */
    bool isInitialized() const { return m_initialized; }

    // ========================================================================
    // Commissioning
    // ========================================================================

    /**
     * Parse QR code or manual pairing code
     * @param payload QR code string or manual pairing code
     * @param params Output commissioning parameters
     * @return true if parsing successful
     */
    bool parseSetupPayload(const std::string& payload, CommissioningParams& params);

    /**
     * Commission a new device
     * @param params Commissioning parameters
     * @param statusCallback Callback for status updates
     * @return true if commissioning started successfully
     */
    bool commissionDevice(const CommissioningParams& params,
                         CommissioningStatusCallback statusCallback = nullptr);

    /**
     * Cancel ongoing commissioning process
     */
    void cancelCommissioning();

    /**
     * Remove commissioned device
     * @param nodeId Node ID of device to remove
     * @return true if device removed successfully
     */
    bool removeDevice(NodeId nodeId);

    /**
     * Get commissioning status
     */
    CommissioningStatus getCommissioningStatus() const { return m_commissioningStatus; }

    // ========================================================================
    // Device Discovery and Management
    // ========================================================================

    /**
     * Start device discovery
     * @param method Discovery method (BLE, WiFi, Thread)
     * @param callback Callback for discovered devices
     * @param timeoutSeconds Discovery timeout
     * @return true if discovery started successfully
     */
    bool startDiscovery(CommissioningMethod method,
                       DeviceDiscoveryCallback callback,
                       uint32_t timeoutSeconds = 30);

    /**
     * Stop device discovery
     */
    void stopDiscovery();

    /**
     * Get all commissioned devices
     */
    std::vector<NodeId> getCommissionedDevices() const;

    /**
     * Get device information
     * @param nodeId Node ID of device
     * @return Device information or nullptr if not found
     */
    std::shared_ptr<MatterDeviceInfo> getDeviceInfo(NodeId nodeId) const;

    /**
     * Update device information (queries device for current state)
     * @param nodeId Node ID of device to update
     * @return true if update successful
     */
    bool updateDeviceInfo(NodeId nodeId);

    /**
     * Check if device is online/reachable
     */
    bool isDeviceOnline(NodeId nodeId) const;

    // ========================================================================
    // Session Management
    // ========================================================================

    /**
     * Establish CASE session with device
     * @param nodeId Node ID of device
     * @return true if session established
     */
    bool establishSession(NodeId nodeId);

    /**
     * Close session with device
     * @param nodeId Node ID of device
     */
    void closeSession(NodeId nodeId);

    /**
     * Check if session is active
     */
    bool hasActiveSession(NodeId nodeId) const;

    // ========================================================================
    // Cluster Interaction
    // ========================================================================

    /**
     * Get cluster client for device interaction
     * @param nodeId Node ID of device
     * @param endpoint Endpoint ID
     * @return Cluster client or nullptr if session not available
     */
    std::shared_ptr<MatterClusterClient> getClusterClient(NodeId nodeId, EndpointId endpoint);

    /**
     * Read attribute from device
     * @param nodeId Node ID of device
     * @param endpoint Endpoint ID
     * @param cluster Cluster ID
     * @param attribute Attribute ID
     * @param value Output value (type depends on attribute)
     * @return true if read successful
     */
    bool readAttribute(NodeId nodeId, EndpointId endpoint,
                      ClusterId cluster, AttributeId attribute,
                      std::string& value);

    /**
     * Write attribute to device
     * @param nodeId Node ID of device
     * @param endpoint Endpoint ID
     * @param cluster Cluster ID
     * @param attribute Attribute ID
     * @param value Value to write (JSON encoded)
     * @return true if write successful
     */
    bool writeAttribute(NodeId nodeId, EndpointId endpoint,
                       ClusterId cluster, AttributeId attribute,
                       const std::string& value);

    /**
     * Send command to device
     * @param nodeId Node ID of device
     * @param endpoint Endpoint ID
     * @param cluster Cluster ID
     * @param command Command ID
     * @param args Command arguments (JSON encoded)
     * @param response Output response (JSON encoded)
     * @return true if command successful
     */
    bool sendCommand(NodeId nodeId, EndpointId endpoint,
                    ClusterId cluster, CommandId command,
                    const std::string& args, std::string& response);

    // ========================================================================
    // Thread Network Integration
    // ========================================================================

    /**
     * Enable Thread Border Router functionality
     * @param networkName Thread network name
     * @param extendedPanId Extended PAN ID (16 bytes hex)
     * @param networkKey Network key (16 bytes hex)
     * @param panId PAN ID
     * @param channel Thread channel (11-26)
     * @return true if enabled successfully
     */
    bool enableThreadBorderRouter(const std::string& networkName,
                                  const std::string& extendedPanId,
                                  const std::string& networkKey,
                                  uint16_t panId,
                                  uint8_t channel);

    /**
     * Disable Thread Border Router
     */
    void disableThreadBorderRouter();

    /**
     * Check if Thread Border Router is enabled
     */
    bool isThreadBorderRouterEnabled() const { return m_threadBorderRouterEnabled; }

    /**
     * Get Thread network operational dataset
     */
    std::string getThreadOperationalDataset() const;

    /**
     * Get Thread network credentials for commissioning
     */
    bool getThreadNetworkCredentials(std::string& networkName,
                                    std::string& extendedPanId,
                                    std::string& networkKey,
                                    uint16_t& panId,
                                    uint8_t& channel);

    // ========================================================================
    // Callbacks
    // ========================================================================

    void setDeviceStateChangeCallback(DeviceStateChangeCallback callback) {
        m_deviceStateChangeCallback = callback;
    }

    void setErrorCallback(ErrorCallback callback) {
        m_errorCallback = callback;
    }

    // ========================================================================
    // Statistics and Diagnostics
    // ========================================================================

    uint32_t getCommissionedDeviceCount() const {
        return static_cast<uint32_t>(m_devices.size());
    }

    std::string getControllerStatus() const;

    std::string getFabricId() const { return m_fabricId; }

    bool isControllerActive() const { return m_controllerActive; }

private:
    // Private constructor for singleton
    MatterController();
    ~MatterController();

    // Implementation methods
    bool initializePlatform();
    void shutdownPlatform();
    bool setupFabric();
    bool loadPersistedDevices();
    void persistDevice(NodeId nodeId);
    void unpersistDevice(NodeId nodeId);
    void handleCommissioningStatusUpdate(CommissioningStatus status, const std::string& message);
    void handleDeviceStateChange(NodeId nodeId, bool online);
    void handleError(int errorCode, const std::string& errorMessage);

    // Thread helper methods
    bool setupThreadNetworkData(const std::string& operationalDataset);

    // Member variables
    bool m_initialized;
    bool m_controllerActive;
    FabricIndex m_fabricIndex;
    std::string m_storageDirectory;
    VendorId m_vendorId;
    ProductId m_productId;
    std::string m_fabricId;

    // Commissioning state
    CommissioningStatus m_commissioningStatus;
    CommissioningStatusCallback m_commissioningCallback;
    NodeId m_commissioningNodeId;

    // Device management
    std::map<NodeId, std::shared_ptr<MatterDeviceInfo>> m_devices;
    std::map<NodeId, std::shared_ptr<MatterClusterClient>> m_clusterClients;

    // Thread Border Router
    bool m_threadBorderRouterEnabled;
    std::shared_ptr<ThreadNetworkManager> m_threadNetworkManager;

    // Callbacks
    DeviceStateChangeCallback m_deviceStateChangeCallback;
    ErrorCallback m_errorCallback;

#ifdef WITH_MATTER_SDK
    // Matter SDK specific members
    chip::Controller::DeviceCommissioner* m_commissioner;
    chip::FabricTable* m_fabricTable;
#endif
};

// ============================================================================
// Thread Network Manager
// ============================================================================

class ThreadNetworkManager {
public:
    ThreadNetworkManager();
    ~ThreadNetworkManager();

    bool initialize();
    void shutdown();

    bool createNetwork(const std::string& networkName,
                      const std::string& extendedPanId,
                      const std::string& networkKey,
                      uint16_t panId,
                      uint8_t channel);

    bool startBorderRouter();
    void stopBorderRouter();

    bool isRunning() const { return m_running; }

    std::string getOperationalDataset() const;

    struct NetworkInfo {
        std::string networkName;
        std::string extendedPanId;
        std::string networkKey;
        uint16_t panId;
        uint8_t channel;
        uint64_t activeTimestamp;
        bool borderRouterActive;
    };

    NetworkInfo getNetworkInfo() const { return m_networkInfo; }

private:
    bool m_initialized;
    bool m_running;
    NetworkInfo m_networkInfo;

#ifdef WITH_MATTER_SDK
    // Thread-specific members
#endif
};

} // namespace matter

#endif // MATTER_SDK_INTEGRATION_H
