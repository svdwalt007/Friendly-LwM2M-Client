/*
 * Zigbee Coordinator Interface
 *
 * Provides comprehensive Zigbee network coordinator functionality supporting:
 * - Silicon Labs EZSP (EmberZNet Serial Protocol) for EFR32 modules
 * - Texas Instruments Z-Stack for CC2531/CC2652 modules
 * - Network formation, joining, and topology management
 * - Device discovery and interviewing
 * - ZCL cluster interface
 * - Green Power and Touchlink commissioning
 */

#ifndef ZIGBEE_COORDINATOR_H
#define ZIGBEE_COORDINATOR_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <cstdint>
#include <mutex>
#include <thread>
#include <atomic>

namespace zigbee {

// Forward declarations
class ZigbeeDevice;
class ZclClusterHandler;
struct ZigbeeNetworkParams;

/**
 * @brief Zigbee coordinator type enumeration
 */
enum class CoordinatorType {
    UNKNOWN = 0,
    EZSP,           // Silicon Labs EmberZNet Serial Protocol (EFR32)
    Z_STACK,        // Texas Instruments Z-Stack (CC2531/CC2652)
    CONBEE,         // Dresden Elektronik ConBee/RaspBee
    ZIGATE          // ZiGate USB/WiFi
};

/**
 * @brief Zigbee network state enumeration
 */
enum class NetworkState {
    OFFLINE = 0,
    FORMING,
    JOINED,
    READY,
    LEAVING,
    ERROR
};

/**
 * @brief Zigbee device type enumeration
 */
enum class DeviceType {
    UNKNOWN = 0,
    COORDINATOR,
    ROUTER,
    END_DEVICE,
    SLEEPY_END_DEVICE
};

/**
 * @brief Zigbee network parameters
 */
struct ZigbeeNetworkParams {
    uint16_t panId;                 // PAN ID (0x0000 - 0xFFFF)
    uint64_t extendedPanId;         // Extended PAN ID (64-bit)
    uint8_t channel;                // Radio channel (11-26)
    uint8_t networkKey[16];         // Network encryption key
    uint8_t txPower;                // Transmit power (dBm)
    uint8_t securityLevel;          // Security level (0-7)
    bool permitJoin;                // Permit joining flag
    uint16_t permitJoinDuration;    // Permit join duration (seconds)

    ZigbeeNetworkParams() :
        panId(0x1A62),
        extendedPanId(0xDD2211AA44556677),
        channel(15),
        txPower(20),
        securityLevel(5),
        permitJoin(false),
        permitJoinDuration(60) {
        // Default network key (should be randomized in production)
        for (int i = 0; i < 16; i++) {
            networkKey[i] = static_cast<uint8_t>(i);
        }
    }
};

/**
 * @brief Zigbee device information
 */
struct ZigbeeDeviceInfo {
    uint64_t ieeeAddress;           // IEEE 64-bit address
    uint16_t networkAddress;        // Network 16-bit address
    DeviceType deviceType;          // Device type
    std::string manufacturer;       // Manufacturer name
    std::string model;              // Model identifier
    std::string firmwareVersion;    // Firmware version
    uint8_t lqi;                    // Link Quality Indicator
    int8_t rssi;                    // Received Signal Strength Indicator
    uint8_t powerSource;            // Power source (battery, mains)
    uint64_t lastSeen;              // Last communication timestamp
    std::vector<uint8_t> endpoints; // Active endpoints
    bool interviewed;               // Interview complete flag

    ZigbeeDeviceInfo() :
        ieeeAddress(0),
        networkAddress(0xFFFF),
        deviceType(DeviceType::UNKNOWN),
        lqi(0),
        rssi(-100),
        powerSource(0),
        lastSeen(0),
        interviewed(false) {}
};

/**
 * @brief Zigbee endpoint descriptor
 */
struct EndpointDescriptor {
    uint8_t endpoint;
    uint16_t profileId;
    uint16_t deviceId;
    uint8_t deviceVersion;
    std::vector<uint16_t> inClusters;
    std::vector<uint16_t> outClusters;
};

/**
 * @brief Zigbee route table entry
 */
struct RouteTableEntry {
    uint16_t destination;
    uint16_t nextHop;
    uint8_t status;
    uint8_t age;
    uint8_t routeRecordState;
};

/**
 * @brief Zigbee neighbor table entry
 */
struct NeighborTableEntry {
    uint64_t ieeeAddress;
    uint16_t networkAddress;
    DeviceType deviceType;
    uint8_t rxOnWhenIdle;
    uint8_t relationship;
    uint8_t depth;
    uint8_t lqi;
};

/**
 * @brief Zigbee Coordinator Class
 *
 * Main coordinator class providing comprehensive Zigbee network management
 */
class ZigbeeCoordinator {
public:
    /**
     * @brief Device event callback types
     */
    using DeviceJoinedCallback = std::function<void(const ZigbeeDeviceInfo&)>;
    using DeviceLeftCallback = std::function<void(uint64_t ieeeAddress)>;
    using DeviceMessageCallback = std::function<void(uint64_t ieeeAddress, uint8_t endpoint,
                                                     uint16_t clusterId, const std::vector<uint8_t>& data)>;
    using NetworkStateCallback = std::function<void(NetworkState state)>;

    /**
     * @brief Constructor
     * @param serialPort Serial port path (e.g., "/dev/ttyUSB0")
     * @param coordinatorType Type of coordinator adapter
     * @param baudRate Serial baud rate (default: 115200)
     */
    ZigbeeCoordinator(const std::string& serialPort,
                      CoordinatorType coordinatorType = CoordinatorType::EZSP,
                      uint32_t baudRate = 115200);

    /**
     * @brief Destructor
     */
    ~ZigbeeCoordinator();

    /**
     * @brief Initialize the coordinator
     * @return true if successful
     */
    bool initialize();

    /**
     * @brief Shutdown the coordinator
     */
    void shutdown();

    /**
     * @brief Form a new Zigbee network
     * @param params Network parameters
     * @return true if successful
     */
    bool formNetwork(const ZigbeeNetworkParams& params);

    /**
     * @brief Join an existing Zigbee network
     * @param params Network parameters
     * @return true if successful
     */
    bool joinNetwork(const ZigbeeNetworkParams& params);

    /**
     * @brief Leave the current network
     * @return true if successful
     */
    bool leaveNetwork();

    /**
     * @brief Permit devices to join the network
     * @param duration Duration in seconds (0 = disable, 255 = forever)
     * @return true if successful
     */
    bool permitJoin(uint16_t duration);

    /**
     * @brief Remove a device from the network
     * @param ieeeAddress IEEE address of device to remove
     * @return true if successful
     */
    bool removeDevice(uint64_t ieeeAddress);

    /**
     * @brief Interview a device (discover capabilities)
     * @param ieeeAddress IEEE address of device
     * @return true if successful
     */
    bool interviewDevice(uint64_t ieeeAddress);

    /**
     * @brief Send ZCL command to a device
     * @param ieeeAddress Target device IEEE address
     * @param endpoint Target endpoint
     * @param clusterId Cluster ID
     * @param commandId Command ID
     * @param payload Command payload
     * @return true if successful
     */
    bool sendZclCommand(uint64_t ieeeAddress, uint8_t endpoint,
                        uint16_t clusterId, uint8_t commandId,
                        const std::vector<uint8_t>& payload);

    /**
     * @brief Bind a cluster between two devices
     * @param sourceIeee Source device IEEE address
     * @param sourceEndpoint Source endpoint
     * @param clusterId Cluster ID
     * @param destIeee Destination device IEEE address
     * @param destEndpoint Destination endpoint
     * @return true if successful
     */
    bool bindCluster(uint64_t sourceIeee, uint8_t sourceEndpoint,
                     uint16_t clusterId, uint64_t destIeee, uint8_t destEndpoint);

    /**
     * @brief Unbind a cluster
     * @param sourceIeee Source device IEEE address
     * @param sourceEndpoint Source endpoint
     * @param clusterId Cluster ID
     * @param destIeee Destination device IEEE address
     * @param destEndpoint Destination endpoint
     * @return true if successful
     */
    bool unbindCluster(uint64_t sourceIeee, uint8_t sourceEndpoint,
                       uint16_t clusterId, uint64_t destIeee, uint8_t destEndpoint);

    /**
     * @brief Get current network state
     * @return Network state
     */
    NetworkState getNetworkState() const;

    /**
     * @brief Get network parameters
     * @return Network parameters
     */
    ZigbeeNetworkParams getNetworkParams() const;

    /**
     * @brief Get all discovered devices
     * @return Vector of device info
     */
    std::vector<ZigbeeDeviceInfo> getDevices() const;

    /**
     * @brief Get device information by IEEE address
     * @param ieeeAddress IEEE address
     * @return Device info (nullptr if not found)
     */
    const ZigbeeDeviceInfo* getDevice(uint64_t ieeeAddress) const;

    /**
     * @brief Get device count
     * @return Number of devices in network
     */
    size_t getDeviceCount() const;

    /**
     * @brief Get route table
     * @return Vector of route entries
     */
    std::vector<RouteTableEntry> getRouteTable() const;

    /**
     * @brief Get neighbor table
     * @return Vector of neighbor entries
     */
    std::vector<NeighborTableEntry> getNeighborTable() const;

    /**
     * @brief Register device joined callback
     * @param callback Callback function
     */
    void onDeviceJoined(DeviceJoinedCallback callback);

    /**
     * @brief Register device left callback
     * @param callback Callback function
     */
    void onDeviceLeft(DeviceLeftCallback callback);

    /**
     * @brief Register device message callback
     * @param callback Callback function
     */
    void onDeviceMessage(DeviceMessageCallback callback);

    /**
     * @brief Register network state callback
     * @param callback Callback function
     */
    void onNetworkStateChanged(NetworkStateCallback callback);

    /**
     * @brief Initiate Touchlink commissioning
     * @return true if successful
     */
    bool touchlinkCommission();

    /**
     * @brief Get coordinator IEEE address
     * @return IEEE address
     */
    uint64_t getCoordinatorIeeeAddress() const;

    /**
     * @brief Get coordinator network address
     * @return Network address (should be 0x0000)
     */
    uint16_t getCoordinatorNetworkAddress() const;

    /**
     * @brief Get firmware version
     * @return Firmware version string
     */
    std::string getFirmwareVersion() const;

    /**
     * @brief Get coordinator type
     * @return Coordinator type
     */
    CoordinatorType getCoordinatorType() const;

    /**
     * @brief Check if coordinator is ready
     * @return true if ready
     */
    bool isReady() const;

private:
    // EZSP-specific methods
    bool initializeEzsp();
    bool formNetworkEzsp(const ZigbeeNetworkParams& params);
    bool permitJoinEzsp(uint16_t duration);
    bool sendZclCommandEzsp(uint16_t networkAddr, uint8_t endpoint,
                            uint16_t clusterId, uint8_t commandId,
                            const std::vector<uint8_t>& payload);
    void processEzspFrame();

    // Z-Stack-specific methods
    bool initializeZStack();
    bool formNetworkZStack(const ZigbeeNetworkParams& params);
    bool permitJoinZStack(uint16_t duration);
    bool sendZclCommandZStack(uint16_t networkAddr, uint8_t endpoint,
                              uint16_t clusterId, uint8_t commandId,
                              const std::vector<uint8_t>& payload);
    void processZStackFrame();

    // Common serial communication
    bool openSerialPort();
    void closeSerialPort();
    ssize_t serialWrite(const uint8_t* data, size_t length);
    ssize_t serialRead(uint8_t* data, size_t length);

    // Device management
    void addDevice(const ZigbeeDeviceInfo& deviceInfo);
    void updateDevice(uint64_t ieeeAddress, const ZigbeeDeviceInfo& deviceInfo);
    void removeDeviceInternal(uint64_t ieeeAddress);

    // Interview process
    bool readBasicCluster(uint64_t ieeeAddress, uint8_t endpoint);
    bool discoverAttributes(uint64_t ieeeAddress, uint8_t endpoint, uint16_t clusterId);
    bool requestNodeDescriptor(uint16_t networkAddr);
    bool requestActiveEndpoints(uint16_t networkAddr);
    bool requestSimpleDescriptor(uint16_t networkAddr, uint8_t endpoint);

    // Background processing thread
    void processingThread();
    void handleIncomingMessage(const std::vector<uint8_t>& data);

    // State management
    void setState(NetworkState state);

    // Member variables
    std::string serialPort_;
    CoordinatorType coordinatorType_;
    uint32_t baudRate_;
    int serialFd_;

    NetworkState networkState_;
    ZigbeeNetworkParams networkParams_;

    uint64_t coordinatorIeeeAddress_;
    uint16_t coordinatorNetworkAddress_;
    std::string firmwareVersion_;

    std::map<uint64_t, ZigbeeDeviceInfo> devices_;
    std::map<uint64_t, std::vector<EndpointDescriptor>> deviceEndpoints_;

    std::vector<RouteTableEntry> routeTable_;
    std::vector<NeighborTableEntry> neighborTable_;

    // Callbacks
    DeviceJoinedCallback deviceJoinedCallback_;
    DeviceLeftCallback deviceLeftCallback_;
    DeviceMessageCallback deviceMessageCallback_;
    NetworkStateCallback networkStateCallback_;

    // Thread synchronization
    mutable std::mutex devicesMutex_;
    mutable std::mutex networkMutex_;
    std::mutex serialMutex_;

    // Processing thread
    std::unique_ptr<std::thread> processingThread_;
    std::atomic<bool> running_;

    // EZSP-specific state
    uint8_t ezspSequence_;
    std::map<uint8_t, std::vector<uint8_t>> ezspPendingResponses_;

    // Z-Stack-specific state
    uint8_t zStackSequence_;
    std::map<uint8_t, std::vector<uint8_t>> zStackPendingResponses_;
};

} // namespace zigbee

#endif // ZIGBEE_COORDINATOR_H
