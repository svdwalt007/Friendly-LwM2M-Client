/**
 * @file prpl_mesh_object.h
 * @brief prplMesh LwM2M Object (ID: 33440)
 *
 * Defines a custom LwM2M object for prplMesh network management.
 * This object provides mesh network topology, node management,
 * link quality metrics, and steering policies.
 *
 * Object ID: 33440
 * URN: urn:prpl:lwm2m:mesh:1
 * Multiple Instances: Yes
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_PRPL_MESH_OBJECT_H
#define FRIENDLY_LWM2M_PRPL_MESH_OBJECT_H

#include "prpl_easymesh.h"
#include "prpl_data_model.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace lwm2m {
namespace platform {
namespace prpl {

/**
 * @brief prplMesh Object ID
 */
constexpr uint16_t PRPL_MESH_OBJECT_ID = 33440;

/**
 * @brief Resource IDs for prplMesh Object (ID: 33440)
 */
enum class PrplMeshResourceId : uint16_t {
    // Network Configuration (0-99)
    NETWORK_ID = 0,                 ///< Network Identifier (String, R, Mandatory)
    NETWORK_NAME = 1,               ///< Network Name (String, R, Mandatory)
    CONTROLLER_MAC = 2,             ///< Controller MAC Address (String, R, Mandatory)
    MESH_VERSION = 3,               ///< EasyMesh Version (Integer, R, Mandatory)
    NODE_ROLE = 4,                  ///< Node Role (Integer, R, Mandatory)
    IS_OPERATIONAL = 5,             ///< Operational Status (Boolean, R, Mandatory)

    // Topology Information (100-199)
    AGENT_COUNT = 100,              ///< Number of Agents (Integer, R, Mandatory)
    RADIO_COUNT = 101,              ///< Number of Radios (Integer, R, Mandatory)
    BSS_COUNT = 102,                ///< Number of BSSs (Integer, R, Mandatory)
    CLIENT_COUNT = 103,             ///< Number of Clients (Integer, R, Mandatory)
    TOPOLOGY_HASH = 104,            ///< Topology Change Hash (String, R, Optional)

    // Link Quality Metrics (200-299)
    AVG_BACKHAUL_RSSI = 200,        ///< Average Backhaul RSSI (Integer, R, Optional)
    MIN_BACKHAUL_RSSI = 201,        ///< Minimum Backhaul RSSI (Integer, R, Optional)
    AVG_CLIENT_RSSI = 202,          ///< Average Client RSSI (Integer, R, Optional)
    MIN_CLIENT_RSSI = 203,          ///< Minimum Client RSSI (Integer, R, Optional)
    TOTAL_THROUGHPUT = 204,         ///< Total Network Throughput (Integer, R, Optional)

    // Statistics (300-399)
    BYTES_SENT = 300,               ///< Total Bytes Sent (Integer, R, Optional)
    BYTES_RECEIVED = 301,           ///< Total Bytes Received (Integer, R, Optional)
    PACKETS_SENT = 302,             ///< Total Packets Sent (Integer, R, Optional)
    PACKETS_RECEIVED = 303,         ///< Total Packets Received (Integer, R, Optional)
    STEERING_COUNT = 304,           ///< Number of Steerings (Integer, R, Optional)

    // Configuration (400-499)
    BAND_STEERING_ENABLED = 400,    ///< Band Steering Enabled (Boolean, RW, Optional)
    AUTO_CHANNEL_ENABLED = 401,     ///< Auto Channel Selection (Boolean, RW, Optional)
    BACKHAUL_OPTIMIZATION = 402,    ///< Backhaul Optimization (Boolean, RW, Optional)
    STEERING_POLICY = 403,          ///< Steering Policy (Integer, RW, Optional)

    // Management Operations (500-599)
    TRIGGER_TOPOLOGY_QUERY = 500,   ///< Trigger Topology Query (Execute, E, Optional)
    TRIGGER_CHANNEL_SCAN = 501,     ///< Trigger Channel Scan (Execute, E, Optional)
    TRIGGER_CHANNEL_SELECT = 502,   ///< Trigger Channel Selection (Execute, E, Optional)
    OPTIMIZE_BACKHAUL = 503,        ///< Optimize Backhaul Links (Execute, E, Optional)
    RESET_STATISTICS = 504,         ///< Reset Statistics (Execute, E, Optional)

    // Agent List (600-699) - Multi-instance resource
    AGENT_MAC_LIST = 600,           ///< Agent MAC Addresses (String, R, Multiple, Optional)
    AGENT_IP_LIST = 601,            ///< Agent IP Addresses (String, R, Multiple, Optional)
    AGENT_STATUS_LIST = 602,        ///< Agent Status (Integer, R, Multiple, Optional)

    // Radio List (700-799) - Multi-instance resource
    RADIO_MAC_LIST = 700,           ///< Radio MAC Addresses (String, R, Multiple, Optional)
    RADIO_CHANNEL_LIST = 701,       ///< Radio Channels (Integer, R, Multiple, Optional)
    RADIO_UTILIZATION_LIST = 702,   ///< Radio Utilization (Integer, R, Multiple, Optional)
};

/**
 * @brief Node role enumeration
 */
enum class MeshNodeRole : uint8_t {
    UNKNOWN = 0,
    CONTROLLER = 1,
    AGENT = 2,
    CONTROLLER_AND_AGENT = 3
};

/**
 * @brief Steering policy enumeration
 */
enum class MeshSteeringPolicy : uint8_t {
    NONE = 0,
    RSSI_BASED = 1,
    LOAD_BASED = 2,
    BAND_STEERING = 3,
    CLIENT_CAPABILITY = 4
};

/**
 * @brief Mesh statistics
 */
struct MeshStatistics {
    uint64_t bytesSent = 0;
    uint64_t bytesReceived = 0;
    uint64_t packetsSent = 0;
    uint64_t packetsReceived = 0;
    uint32_t steeringCount = 0;
};

/**
 * @brief Mesh topology metrics
 */
struct MeshTopologyMetrics {
    uint32_t agentCount = 0;
    uint32_t radioCount = 0;
    uint32_t bssCount = 0;
    uint32_t clientCount = 0;
    int8_t avgBackhaulRSSI = 0;
    int8_t minBackhaulRSSI = 0;
    int8_t avgClientRSSI = 0;
    int8_t minClientRSSI = 0;
    uint64_t totalThroughput = 0;
};

/**
 * @brief Callback for resource write operations
 */
using ResourceWriteCallback = std::function<bool(PrplMeshResourceId resourceId,
                                                  const std::string& value)>;

/**
 * @brief Callback for execute operations
 */
using ExecuteCallback = std::function<bool(PrplMeshResourceId resourceId,
                                            const std::string& arguments)>;

/**
 * @brief prplMesh Object Implementation
 *
 * Manages prplMesh network information and operations through LwM2M.
 * Integrates with EasyMesh subsystem to provide real-time mesh data.
 */
class PrplMeshObject {
public:
    /**
     * @brief Constructor
     * @param instanceId Object instance ID
     * @param easyMesh EasyMesh integration instance
     * @param dataModel TR-181 data model instance
     */
    explicit PrplMeshObject(uint16_t instanceId,
                           std::shared_ptr<PrplEasyMesh> easyMesh,
                           std::shared_ptr<PrplDataModel> dataModel);

    /**
     * @brief Destructor
     */
    ~PrplMeshObject();

    // Prevent copying
    PrplMeshObject(const PrplMeshObject&) = delete;
    PrplMeshObject& operator=(const PrplMeshObject&) = delete;

    /**
     * @brief Initialize the object
     * @return true on success
     */
    bool initialize();

    /**
     * @brief Get object instance ID
     * @return Instance ID
     */
    uint16_t getInstanceId() const { return instanceId_; }

    // ========================================================================
    // Resource Read Operations
    // ========================================================================

    /**
     * @brief Read string resource
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(PrplMeshResourceId resourceId, std::string& value);

    /**
     * @brief Read integer resource
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(PrplMeshResourceId resourceId, int64_t& value);

    /**
     * @brief Read boolean resource
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(PrplMeshResourceId resourceId, bool& value);

    /**
     * @brief Read multi-instance string resource
     * @param resourceId Resource ID
     * @param instanceId Resource instance ID
     * @param value Output value
     * @return true on success
     */
    bool readMulti(PrplMeshResourceId resourceId, uint16_t instanceId, std::string& value);

    /**
     * @brief Read multi-instance integer resource
     * @param resourceId Resource ID
     * @param instanceId Resource instance ID
     * @param value Output value
     * @return true on success
     */
    bool readMulti(PrplMeshResourceId resourceId, uint16_t instanceId, int64_t& value);

    // ========================================================================
    // Resource Write Operations
    // ========================================================================

    /**
     * @brief Write string resource
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(PrplMeshResourceId resourceId, const std::string& value);

    /**
     * @brief Write integer resource
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(PrplMeshResourceId resourceId, int64_t value);

    /**
     * @brief Write boolean resource
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(PrplMeshResourceId resourceId, bool value);

    // ========================================================================
    // Execute Operations
    // ========================================================================

    /**
     * @brief Execute resource operation
     * @param resourceId Resource ID
     * @param arguments Optional arguments
     * @return true on success
     */
    bool execute(PrplMeshResourceId resourceId, const std::string& arguments = "");

    // ========================================================================
    // Callbacks
    // ========================================================================

    /**
     * @brief Set write callback
     * @param callback Callback function
     */
    void setWriteCallback(ResourceWriteCallback callback) {
        writeCallback_ = callback;
    }

    /**
     * @brief Set execute callback
     * @param callback Callback function
     */
    void setExecuteCallback(ExecuteCallback callback) {
        executeCallback_ = callback;
    }

    // ========================================================================
    // Data Update
    // ========================================================================

    /**
     * @brief Update mesh data from EasyMesh
     * @return true on success
     */
    bool updateMeshData();

    /**
     * @brief Get current statistics
     * @return Mesh statistics
     */
    MeshStatistics getStatistics();

    /**
     * @brief Get topology metrics
     * @return Topology metrics
     */
    MeshTopologyMetrics getTopologyMetrics();

    /**
     * @brief Reset statistics counters
     */
    void resetStatistics();

private:
    uint16_t instanceId_;
    std::shared_ptr<PrplEasyMesh> easyMesh_;
    std::shared_ptr<PrplDataModel> dataModel_;
    bool initialized_;

    // Cached mesh data
    EasyMeshNetwork networkInfo_;
    std::vector<EasyMeshAgent> agents_;
    std::vector<EasyMeshRadio> radios_;
    std::vector<EasyMeshClient> clients_;

    // Statistics
    MeshStatistics statistics_;
    MeshTopologyMetrics metrics_;

    // Configuration
    bool bandSteeringEnabled_;
    bool autoChannelEnabled_;
    bool backhaulOptimizationEnabled_;
    MeshSteeringPolicy steeringPolicy_;

    // Callbacks
    ResourceWriteCallback writeCallback_;
    ExecuteCallback executeCallback_;

    // Helper methods
    void calculateMetrics();
    void updateStatistics();
    std::string topologyHash();
    MeshNodeRole convertRole(EasyMeshRole role);
    uint8_t convertVersion(EasyMeshVersion version);
};

} // namespace prpl
} // namespace platform
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_PRPL_MESH_OBJECT_H
