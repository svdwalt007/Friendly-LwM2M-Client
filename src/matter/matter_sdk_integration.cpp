/*
 * Matter SDK Integration Implementation
 * Friendly LwM2M Client - Matter 1.0/1.1 Controller
 */

#include "matter_sdk_integration.h"
#include "matter_cluster_client.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

#ifdef WITH_MATTER_SDK
#include <controller/CHIPDeviceControllerFactory.h>
#include <controller/ExampleOperationalCredentialsIssuer.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/PlatformManager.h>
#include <setup_payload/QRCodeSetupPayloadParser.h>
#include <setup_payload/ManualSetupPayloadParser.h>
#include <lib/support/CHIPMem.h>
#include <lib/support/CodeUtils.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/examples/DeviceAttestationCredsExample.h>
#endif

namespace matter {

// ============================================================================
// Helper Functions
// ============================================================================

static std::string getCurrentTimestamp() {
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buf);
}

static bool createDirectory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return mkdir(path.c_str(), 0755) == 0;
}

static std::string toHexString(const uint8_t* data, size_t length) {
    std::ostringstream oss;
    for (size_t i = 0; i < length; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    }
    return oss.str();
}

static bool fromHexString(const std::string& hex, uint8_t* data, size_t maxLength, size_t& actualLength) {
    if (hex.length() % 2 != 0) return false;

    actualLength = hex.length() / 2;
    if (actualLength > maxLength) return false;

    for (size_t i = 0; i < actualLength; i++) {
        std::string byte = hex.substr(i * 2, 2);
        data[i] = static_cast<uint8_t>(strtol(byte.c_str(), nullptr, 16));
    }
    return true;
}

// ============================================================================
// Matter Controller Implementation
// ============================================================================

MatterController::MatterController()
    : m_initialized(false)
    , m_controllerActive(false)
    , m_fabricIndex(1)
    , m_vendorId(0xFFF1)
    , m_productId(0x8000)
    , m_commissioningStatus(CommissioningStatus::NOT_STARTED)
    , m_commissioningNodeId(0)
    , m_threadBorderRouterEnabled(false)
#ifdef WITH_MATTER_SDK
    , m_commissioner(nullptr)
    , m_fabricTable(nullptr)
#endif
{
}

MatterController::~MatterController() {
    if (m_initialized) {
        shutdown();
    }
}

MatterController& MatterController::getInstance() {
    static MatterController instance;
    return instance;
}

// ============================================================================
// Initialization
// ============================================================================

bool MatterController::initialize(FabricIndex fabricIndex,
                                  const std::string& storageDirectory,
                                  VendorId vendorId,
                                  ProductId productId) {
    if (m_initialized) {
        std::cerr << "[Matter] Controller already initialized" << std::endl;
        return true;
    }

    std::cout << "[Matter] Initializing Matter Controller..." << std::endl;
    std::cout << "[Matter]   Fabric Index: " << static_cast<int>(fabricIndex) << std::endl;
    std::cout << "[Matter]   Storage: " << storageDirectory << std::endl;
    std::cout << "[Matter]   Vendor ID: 0x" << std::hex << vendorId << std::dec << std::endl;
    std::cout << "[Matter]   Product ID: 0x" << std::hex << productId << std::dec << std::endl;

    m_fabricIndex = fabricIndex;
    m_storageDirectory = storageDirectory;
    m_vendorId = vendorId;
    m_productId = productId;

    // Create storage directory if it doesn't exist
    if (!createDirectory(m_storageDirectory)) {
        std::cerr << "[Matter] Failed to create storage directory: " << m_storageDirectory << std::endl;
        return false;
    }

#ifdef WITH_MATTER_SDK
    // Initialize Matter platform
    if (!initializePlatform()) {
        std::cerr << "[Matter] Platform initialization failed" << std::endl;
        return false;
    }

    // Setup fabric
    if (!setupFabric()) {
        std::cerr << "[Matter] Fabric setup failed" << std::endl;
        shutdownPlatform();
        return false;
    }

    // Load persisted devices
    if (!loadPersistedDevices()) {
        std::cerr << "[Matter] Warning: Failed to load persisted devices" << std::endl;
    }
#else
    // Synthetic mode: the Matter SDK headers/libraries are not linked into this
    // build, so we expose a deterministic Fabric ID and let the controller run
    // without a real CHIP stack. This lets LwM2M↔Matter bridging be exercised
    // end-to-end (object models, commissioning flow, cluster reads/writes)
    // without pulling in the full Matter dependency tree.
    std::cout << "[Matter] Running without CHIP SDK; using synthetic fabric" << std::endl;
    m_fabricId = "synthetic-fabric-0000000000000001";
#endif

    m_initialized = true;
    m_controllerActive = true;

    std::cout << "[Matter] Controller initialized successfully" << std::endl;
    std::cout << "[Matter] Fabric ID: " << m_fabricId << std::endl;

    return true;
}

void MatterController::shutdown() {
    if (!m_initialized) {
        return;
    }

    std::cout << "[Matter] Shutting down Matter Controller..." << std::endl;

    // Close all sessions
    for (const auto& pair : m_devices) {
        closeSession(pair.first);
    }

    // Clear cluster clients
    m_clusterClients.clear();

    // Disable Thread Border Router if enabled
    if (m_threadBorderRouterEnabled) {
        disableThreadBorderRouter();
    }

#ifdef WITH_MATTER_SDK
    // Shutdown platform
    shutdownPlatform();
#endif

    m_devices.clear();
    m_controllerActive = false;
    m_initialized = false;

    std::cout << "[Matter] Controller shutdown complete" << std::endl;
}

bool MatterController::initializePlatform() {
#ifdef WITH_MATTER_SDK
    // Initialize CHIP memory
    chip::Platform::MemoryInit();

    // Initialize platform manager
    CHIP_ERROR err = chip::DeviceLayer::PlatformMgr().InitChipStack();
    if (err != CHIP_NO_ERROR) {
        std::cerr << "[Matter] Failed to initialize platform manager" << std::endl;
        return false;
    }

    // Start event loop
    err = chip::DeviceLayer::PlatformMgr().StartEventLoopTask();
    if (err != CHIP_NO_ERROR) {
        std::cerr << "[Matter] Failed to start event loop" << std::endl;
        return false;
    }

    return true;
#else
    return true; // Stub implementation
#endif
}

void MatterController::shutdownPlatform() {
#ifdef WITH_MATTER_SDK
    chip::DeviceLayer::PlatformMgr().StopEventLoopTask();
    chip::DeviceLayer::PlatformMgr().Shutdown();
    chip::Platform::MemoryShutdown();
#endif
}

bool MatterController::setupFabric() {
#ifdef WITH_MATTER_SDK
    // This is a simplified setup - production code needs proper credential management
    // Create fabric ID
    std::ostringstream oss;
    oss << "matter-fabric-" << std::hex << std::setw(16) << std::setfill('0')
        << (static_cast<uint64_t>(m_fabricIndex) << 56 | time(nullptr));
    m_fabricId = oss.str();

    return true;
#else
    return true;
#endif
}

bool MatterController::loadPersistedDevices() {
    std::string devicesFile = m_storageDirectory + "/devices.dat";
    std::ifstream ifs(devicesFile);
    if (!ifs.is_open()) {
        return true; // No devices persisted yet
    }

    std::cout << "[Matter] Loading persisted devices..." << std::endl;
    int count = 0;

    std::string line;
    while (std::getline(ifs, line)) {
        NodeId nodeId;
        if (sscanf(line.c_str(), "%016llx", &nodeId) == 1) {
            auto deviceInfo = std::make_shared<MatterDeviceInfo>();
            deviceInfo->nodeId = nodeId;
            deviceInfo->reachable = false;
            m_devices[nodeId] = deviceInfo;
            count++;
        }
    }

    std::cout << "[Matter] Loaded " << count << " persisted devices" << std::endl;
    return true;
}

void MatterController::persistDevice(NodeId nodeId) {
    std::string devicesFile = m_storageDirectory + "/devices.dat";
    std::ofstream ofs(devicesFile, std::ios::app);
    if (ofs.is_open()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%016llx\n", nodeId);
        ofs << buf;
    }
}

void MatterController::unpersistDevice(NodeId nodeId) {
    std::string devicesFile = m_storageDirectory + "/devices.dat";
    std::string tempFile = devicesFile + ".tmp";

    std::ifstream ifs(devicesFile);
    std::ofstream ofs(tempFile);

    if (ifs.is_open() && ofs.is_open()) {
        std::string line;
        while (std::getline(ifs, line)) {
            NodeId id;
            if (sscanf(line.c_str(), "%016llx", &id) == 1) {
                if (id != nodeId) {
                    ofs << line << "\n";
                }
            }
        }
        ifs.close();
        ofs.close();
        rename(tempFile.c_str(), devicesFile.c_str());
    }
}

// ============================================================================
// Commissioning
// ============================================================================

bool MatterController::parseSetupPayload(const std::string& payload, CommissioningParams& params) {
    if (payload.empty()) {
        return false;
    }

    // Check if QR code or manual code
    if (payload.find("MT:") == 0) {
        // QR code format
        params.method = CommissioningMethod::QR_CODE;
        params.setupPayload = payload;

#ifdef WITH_MATTER_SDK
        chip::SetupPayload setupPayload;
        CHIP_ERROR err = chip::QRCodeSetupPayloadParser(payload).populatePayload(setupPayload);
        if (err != CHIP_NO_ERROR) {
            return false;
        }
        params.setupPIN = setupPayload.setUpPINCode;
        params.discriminator = setupPayload.discriminator;
        params.vendorId = setupPayload.vendorID;
        params.productId = setupPayload.productID;
#else
        // Stub parsing - extract basic info
        params.setupPIN = 12345678;
        params.discriminator = 3840;
#endif
        return true;
    } else if (payload.length() >= 11 && std::all_of(payload.begin(), payload.end(), ::isdigit)) {
        // Manual pairing code (11 or 21 digits)
        params.method = CommissioningMethod::MANUAL_CODE;
        params.setupPayload = payload;

#ifdef WITH_MATTER_SDK
        chip::SetupPayload setupPayload;
        CHIP_ERROR err = chip::ManualSetupPayloadParser(payload).populatePayload(setupPayload);
        if (err != CHIP_NO_ERROR) {
            return false;
        }
        params.setupPIN = setupPayload.setUpPINCode;
        params.discriminator = setupPayload.discriminator;
#else
        // Stub parsing
        params.setupPIN = 12345678;
        params.discriminator = 3840;
#endif
        return true;
    }

    return false;
}

bool MatterController::commissionDevice(const CommissioningParams& params,
                                       CommissioningStatusCallback statusCallback) {
    if (!m_initialized) {
        std::cerr << "[Matter] Controller not initialized" << std::endl;
        return false;
    }

    if (m_commissioningStatus == CommissioningStatus::IN_PROGRESS) {
        std::cerr << "[Matter] Commissioning already in progress" << std::endl;
        return false;
    }

    std::cout << "[Matter] Starting device commissioning..." << std::endl;
    std::cout << "[Matter]   Method: " << static_cast<int>(params.method) << std::endl;
    std::cout << "[Matter]   Setup PIN: " << params.setupPIN << std::endl;
    std::cout << "[Matter]   Discriminator: " << params.discriminator << std::endl;

    m_commissioningCallback = statusCallback;
    m_commissioningStatus = CommissioningStatus::IN_PROGRESS;
    m_commissioningNodeId = 0;

    handleCommissioningStatusUpdate(CommissioningStatus::IN_PROGRESS, "Commissioning started");

#ifdef WITH_MATTER_SDK
    // Real Matter SDK commissioning implementation
    // This is a simplified version - production code needs proper implementation

    // 1. Setup commissioning parameters
    // 2. Start discovery based on method (BLE/WiFi/Thread)
    // 3. Establish PASE session
    // 4. Perform device attestation
    // 5. Configure network credentials
    // 6. Complete commissioning

    // For now, simulate async commissioning
    handleCommissioningStatusUpdate(CommissioningStatus::DEVICE_DISCOVERED, "Device discovered");
    handleCommissioningStatusUpdate(CommissioningStatus::CONNECTING, "Connecting to device");
    handleCommissioningStatusUpdate(CommissioningStatus::AUTHENTICATING, "Authenticating");

    if (!params.wifiSSID.empty() || !params.threadOperationalDataset.empty()) {
        handleCommissioningStatusUpdate(CommissioningStatus::CONFIGURING_NETWORK, "Configuring network");
    }

    handleCommissioningStatusUpdate(CommissioningStatus::OPERATIONAL_DISCOVERY, "Operational discovery");

    // Generate node ID
    m_commissioningNodeId = time(nullptr) & 0xFFFFFFFFFFFFULL;

    // Create device info
    auto deviceInfo = std::make_shared<MatterDeviceInfo>();
    deviceInfo->nodeId = m_commissioningNodeId;
    deviceInfo->vendorId = params.vendorId;
    deviceInfo->productId = params.productId;
    deviceInfo->reachable = true;
    m_devices[m_commissioningNodeId] = deviceInfo;

    persistDevice(m_commissioningNodeId);

    handleCommissioningStatusUpdate(CommissioningStatus::COMPLETED, "Commissioning completed");

    return true;
#else
    // Synthetic mode: drive the commissioning state machine through every
    // canonical stage so subscribers (callbacks, UI, telemetry) observe a
    // realistic sequence even without a real CHIP commissioner.
    handleCommissioningStatusUpdate(CommissioningStatus::DEVICE_DISCOVERED, "Device discovered");
    handleCommissioningStatusUpdate(CommissioningStatus::CONNECTING, "Connecting");
    handleCommissioningStatusUpdate(CommissioningStatus::AUTHENTICATING, "Authenticating");
    handleCommissioningStatusUpdate(CommissioningStatus::COMPLETED, "Commissioning completed");

    // Allocate a fresh synthetic NodeId for this device.
    m_commissioningNodeId = 0x0000000000000001ULL + m_devices.size();
    auto deviceInfo = std::make_shared<MatterDeviceInfo>();
    deviceInfo->nodeId = m_commissioningNodeId;
    deviceInfo->vendorId = params.vendorId;
    deviceInfo->productId = params.productId;
    deviceInfo->deviceType = 256; // Light bulb
    deviceInfo->reachable = true;
    deviceInfo->firmwareVersion = "1.0.0";
    deviceInfo->endpoints.push_back(1);
    m_devices[m_commissioningNodeId] = deviceInfo;

    persistDevice(m_commissioningNodeId);

    return true;
#endif
}

void MatterController::cancelCommissioning() {
    if (m_commissioningStatus != CommissioningStatus::IN_PROGRESS) {
        return;
    }

    std::cout << "[Matter] Cancelling commissioning..." << std::endl;

#ifdef WITH_MATTER_SDK
    // Cancel ongoing commissioning
#endif

    handleCommissioningStatusUpdate(CommissioningStatus::CANCELLED, "Commissioning cancelled");
}

bool MatterController::removeDevice(NodeId nodeId) {
    if (!m_initialized) {
        return false;
    }

    auto it = m_devices.find(nodeId);
    if (it == m_devices.end()) {
        std::cerr << "[Matter] Device not found: 0x" << std::hex << nodeId << std::dec << std::endl;
        return false;
    }

    std::cout << "[Matter] Removing device: 0x" << std::hex << nodeId << std::dec << std::endl;

    // Close session
    closeSession(nodeId);

    // Remove from cluster clients
    m_clusterClients.erase(nodeId);

#ifdef WITH_MATTER_SDK
    // Remove from fabric
#endif

    // Remove from devices map
    m_devices.erase(it);

    // Unpersist
    unpersistDevice(nodeId);

    std::cout << "[Matter] Device removed successfully" << std::endl;

    return true;
}

void MatterController::handleCommissioningStatusUpdate(CommissioningStatus status, const std::string& message) {
    m_commissioningStatus = status;

    std::cout << "[Matter] Commissioning status: " << message << std::endl;

    if (m_commissioningCallback) {
        m_commissioningCallback(status, message);
    }
}

// ============================================================================
// Device Discovery and Management
// ============================================================================

bool MatterController::startDiscovery(CommissioningMethod method,
                                     DeviceDiscoveryCallback callback,
                                     uint32_t timeoutSeconds) {
    if (!m_initialized) {
        return false;
    }

    std::cout << "[Matter] Starting device discovery..." << std::endl;
    std::cout << "[Matter]   Method: " << static_cast<int>(method) << std::endl;
    std::cout << "[Matter]   Timeout: " << timeoutSeconds << " seconds" << std::endl;

#ifdef WITH_MATTER_SDK
    // Real discovery implementation based on method
    switch (method) {
        case CommissioningMethod::BLE:
            // Start BLE discovery
            break;
        case CommissioningMethod::ON_NETWORK:
            // Start mDNS discovery
            break;
        case CommissioningMethod::THREAD:
            // Start Thread discovery
            break;
        default:
            return false;
    }
#else
    // Stub - simulate discovering a device
    if (callback) {
        MatterDeviceInfo device;
        device.nodeId = 0;
        device.vendorId = 0xFFF1;
        device.productId = 0x8000;
        device.deviceType = 256;
        device.serialNumber = "STUB-001";
        device.reachable = true;
        callback(device);
    }
#endif

    return true;
}

void MatterController::stopDiscovery() {
    std::cout << "[Matter] Stopping discovery..." << std::endl;
#ifdef WITH_MATTER_SDK
    // Stop discovery
#endif
}

std::vector<NodeId> MatterController::getCommissionedDevices() const {
    std::vector<NodeId> nodeIds;
    for (const auto& pair : m_devices) {
        nodeIds.push_back(pair.first);
    }
    return nodeIds;
}

std::shared_ptr<MatterDeviceInfo> MatterController::getDeviceInfo(NodeId nodeId) const {
    auto it = m_devices.find(nodeId);
    if (it != m_devices.end()) {
        return it->second;
    }
    return nullptr;
}

bool MatterController::updateDeviceInfo(NodeId nodeId) {
    auto it = m_devices.find(nodeId);
    if (it == m_devices.end()) {
        return false;
    }

#ifdef WITH_MATTER_SDK
    // Query device for current information
    // Read basic information cluster attributes
#endif

    it->second->lastSeen = time(nullptr);
    return true;
}

bool MatterController::isDeviceOnline(NodeId nodeId) const {
    auto deviceInfo = getDeviceInfo(nodeId);
    return deviceInfo && deviceInfo->reachable;
}

// ============================================================================
// Session Management
// ============================================================================

bool MatterController::establishSession(NodeId nodeId) {
    if (!m_initialized) {
        return false;
    }

    auto it = m_devices.find(nodeId);
    if (it == m_devices.end()) {
        return false;
    }

    std::cout << "[Matter] Establishing session with device: 0x" << std::hex << nodeId << std::dec << std::endl;

#ifdef WITH_MATTER_SDK
    // Establish CASE session
#endif

    return true;
}

void MatterController::closeSession(NodeId nodeId) {
    std::cout << "[Matter] Closing session with device: 0x" << std::hex << nodeId << std::dec << std::endl;

#ifdef WITH_MATTER_SDK
    // Close CASE session
#endif
}

bool MatterController::hasActiveSession(NodeId nodeId) const {
#ifdef WITH_MATTER_SDK
    // Check if session is active
    return m_devices.find(nodeId) != m_devices.end();
#else
    return m_devices.find(nodeId) != m_devices.end();
#endif
}

// ============================================================================
// Cluster Interaction
// ============================================================================

std::shared_ptr<MatterClusterClient> MatterController::getClusterClient(NodeId nodeId, EndpointId endpoint) {
    if (!hasActiveSession(nodeId)) {
        if (!establishSession(nodeId)) {
            return nullptr;
        }
    }

    // Create or get existing cluster client
    auto it = m_clusterClients.find(nodeId);
    if (it != m_clusterClients.end()) {
        return it->second;
    }

    // Create new cluster client (implemented in matter_cluster_client.cpp)
    auto client = std::make_shared<MatterClusterClient>(nodeId, endpoint);
    m_clusterClients[nodeId] = client;
    return client;
}

bool MatterController::readAttribute(NodeId nodeId, EndpointId endpoint,
                                    ClusterId cluster, AttributeId attribute,
                                    std::string& value) {
    auto client = getClusterClient(nodeId, endpoint);
    if (!client) {
        return false;
    }

    // Use cluster client to read attribute
    return client->readAttribute(cluster, attribute, value);
}

bool MatterController::writeAttribute(NodeId nodeId, EndpointId endpoint,
                                     ClusterId cluster, AttributeId attribute,
                                     const std::string& value) {
    auto client = getClusterClient(nodeId, endpoint);
    if (!client) {
        return false;
    }

    return client->writeAttribute(cluster, attribute, value);
}

bool MatterController::sendCommand(NodeId nodeId, EndpointId endpoint,
                                  ClusterId cluster, CommandId command,
                                  const std::string& args, std::string& response) {
    auto client = getClusterClient(nodeId, endpoint);
    if (!client) {
        return false;
    }

    return client->sendCommand(cluster, command, args, response);
}

// ============================================================================
// Thread Network Integration
// ============================================================================

bool MatterController::enableThreadBorderRouter(const std::string& networkName,
                                               const std::string& extendedPanId,
                                               const std::string& networkKey,
                                               uint16_t panId,
                                               uint8_t channel) {
    if (!m_initialized) {
        return false;
    }

    std::cout << "[Matter] Enabling Thread Border Router..." << std::endl;
    std::cout << "[Matter]   Network Name: " << networkName << std::endl;
    std::cout << "[Matter]   PAN ID: 0x" << std::hex << panId << std::dec << std::endl;
    std::cout << "[Matter]   Channel: " << static_cast<int>(channel) << std::endl;

    if (!m_threadNetworkManager) {
        m_threadNetworkManager = std::make_shared<ThreadNetworkManager>();
        if (!m_threadNetworkManager->initialize()) {
            std::cerr << "[Matter] Failed to initialize Thread Network Manager" << std::endl;
            m_threadNetworkManager.reset();
            return false;
        }
    }

    if (!m_threadNetworkManager->createNetwork(networkName, extendedPanId, networkKey, panId, channel)) {
        std::cerr << "[Matter] Failed to create Thread network" << std::endl;
        return false;
    }

    if (!m_threadNetworkManager->startBorderRouter()) {
        std::cerr << "[Matter] Failed to start Thread Border Router" << std::endl;
        return false;
    }

    m_threadBorderRouterEnabled = true;
    std::cout << "[Matter] Thread Border Router enabled successfully" << std::endl;

    return true;
}

void MatterController::disableThreadBorderRouter() {
    if (!m_threadBorderRouterEnabled || !m_threadNetworkManager) {
        return;
    }

    std::cout << "[Matter] Disabling Thread Border Router..." << std::endl;

    m_threadNetworkManager->stopBorderRouter();
    m_threadNetworkManager->shutdown();
    m_threadNetworkManager.reset();

    m_threadBorderRouterEnabled = false;

    std::cout << "[Matter] Thread Border Router disabled" << std::endl;
}

std::string MatterController::getThreadOperationalDataset() const {
    if (!m_threadBorderRouterEnabled || !m_threadNetworkManager) {
        return "";
    }

    return m_threadNetworkManager->getOperationalDataset();
}

bool MatterController::getThreadNetworkCredentials(std::string& networkName,
                                                  std::string& extendedPanId,
                                                  std::string& networkKey,
                                                  uint16_t& panId,
                                                  uint8_t& channel) {
    if (!m_threadBorderRouterEnabled || !m_threadNetworkManager) {
        return false;
    }

    auto info = m_threadNetworkManager->getNetworkInfo();
    networkName = info.networkName;
    extendedPanId = info.extendedPanId;
    networkKey = info.networkKey;
    panId = info.panId;
    channel = info.channel;

    return true;
}

// ============================================================================
// Statistics and Diagnostics
// ============================================================================

std::string MatterController::getControllerStatus() const {
    std::ostringstream oss;
    oss << "Matter Controller Status:\n";
    oss << "  Initialized: " << (m_initialized ? "Yes" : "No") << "\n";
    oss << "  Active: " << (m_controllerActive ? "Yes" : "No") << "\n";
    oss << "  Fabric ID: " << m_fabricId << "\n";
    oss << "  Fabric Index: " << static_cast<int>(m_fabricIndex) << "\n";
    oss << "  Commissioned Devices: " << m_devices.size() << "\n";
    oss << "  Thread Border Router: " << (m_threadBorderRouterEnabled ? "Enabled" : "Disabled") << "\n";
    oss << "  Commissioning Status: " << static_cast<int>(m_commissioningStatus);
    return oss.str();
}

void MatterController::handleDeviceStateChange(NodeId nodeId, bool online) {
    auto deviceInfo = getDeviceInfo(nodeId);
    if (deviceInfo) {
        deviceInfo->reachable = online;
        deviceInfo->lastSeen = time(nullptr);
    }

    if (m_deviceStateChangeCallback) {
        m_deviceStateChangeCallback(nodeId, online);
    }
}

void MatterController::handleError(int errorCode, const std::string& errorMessage) {
    std::cerr << "[Matter] Error " << errorCode << ": " << errorMessage << std::endl;

    if (m_errorCallback) {
        m_errorCallback(errorCode, errorMessage);
    }
}

// ============================================================================
// Thread Network Manager Implementation
// ============================================================================

ThreadNetworkManager::ThreadNetworkManager()
    : m_initialized(false)
    , m_running(false) {
}

ThreadNetworkManager::~ThreadNetworkManager() {
    if (m_initialized) {
        shutdown();
    }
}

bool ThreadNetworkManager::initialize() {
    if (m_initialized) {
        return true;
    }

    std::cout << "[Thread] Initializing Thread Network Manager..." << std::endl;

#ifdef WITH_MATTER_SDK
    // Initialize Thread stack
#endif

    m_initialized = true;
    return true;
}

void ThreadNetworkManager::shutdown() {
    if (!m_initialized) {
        return;
    }

    if (m_running) {
        stopBorderRouter();
    }

#ifdef WITH_MATTER_SDK
    // Shutdown Thread stack
#endif

    m_initialized = false;
}

bool ThreadNetworkManager::createNetwork(const std::string& networkName,
                                        const std::string& extendedPanId,
                                        const std::string& networkKey,
                                        uint16_t panId,
                                        uint8_t channel) {
    if (!m_initialized) {
        return false;
    }

    std::cout << "[Thread] Creating Thread network..." << std::endl;

    m_networkInfo.networkName = networkName;
    m_networkInfo.extendedPanId = extendedPanId;
    m_networkInfo.networkKey = networkKey;
    m_networkInfo.panId = panId;
    m_networkInfo.channel = channel;
    m_networkInfo.activeTimestamp = time(nullptr);
    m_networkInfo.borderRouterActive = false;

#ifdef WITH_MATTER_SDK
    // Configure Thread network parameters
#endif

    return true;
}

bool ThreadNetworkManager::startBorderRouter() {
    if (!m_initialized || m_running) {
        return false;
    }

    std::cout << "[Thread] Starting Thread Border Router..." << std::endl;

#ifdef WITH_MATTER_SDK
    // Start Thread Border Router
#endif

    m_running = true;
    m_networkInfo.borderRouterActive = true;

    std::cout << "[Thread] Thread Border Router started" << std::endl;

    return true;
}

void ThreadNetworkManager::stopBorderRouter() {
    if (!m_running) {
        return;
    }

    std::cout << "[Thread] Stopping Thread Border Router..." << std::endl;

#ifdef WITH_MATTER_SDK
    // Stop Thread Border Router
#endif

    m_running = false;
    m_networkInfo.borderRouterActive = false;

    std::cout << "[Thread] Thread Border Router stopped" << std::endl;
}

std::string ThreadNetworkManager::getOperationalDataset() const {
    if (!m_initialized) {
        return "";
    }

    // Create Thread Operational Dataset TLV
    // This is a simplified version - production code needs proper TLV encoding
    std::ostringstream oss;
    oss << "THREAD_DATASET:";
    oss << "name=" << m_networkInfo.networkName << ",";
    oss << "xpanid=" << m_networkInfo.extendedPanId << ",";
    oss << "key=" << m_networkInfo.networkKey << ",";
    oss << "panid=" << std::hex << m_networkInfo.panId << ",";
    oss << "channel=" << std::dec << static_cast<int>(m_networkInfo.channel);

    return oss.str();
}

} // namespace matter
