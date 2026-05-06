/**
 * @file prpl_easymesh.h
 * @brief EasyMesh R2/R4 Integration for prplOS
 *
 * Provides integration with Wi-Fi EasyMesh R2 and R4 standards including:
 * - Controller and Agent detection
 * - Network topology discovery
 * - Client steering and optimization
 * - Channel selection coordination
 * - Backhaul link optimization
 * - Multi-AP coordination
 *
 * EasyMesh integration leverages TR-181 Device.WiFi.DataElements
 * and Multi-AP data model extensions.
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_PRPL_EASYMESH_H
#define FRIENDLY_LWM2M_PRPL_EASYMESH_H

#include "prpl_amxb_client.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include <functional>
#include <chrono>
#include <optional>

namespace lwm2m {
namespace platform {
namespace prpl {

/**
 * @brief EasyMesh protocol version
 */
enum class EasyMeshVersion {
    UNKNOWN,
    R1,
    R2,
    R3,
    R4
};

/**
 * @brief EasyMesh device role
 */
enum class EasyMeshRole {
    UNKNOWN,
    CONTROLLER,
    AGENT,
    CONTROLLER_AND_AGENT
};

/**
 * @brief EasyMesh backhaul type
 */
enum class BackhaulType {
    UNKNOWN,
    ETHERNET,
    WIFI_2G,
    WIFI_5G,
    WIFI_6G
};

/**
 * @brief Steering policy
 */
enum class SteeringPolicy {
    NONE,
    RSSI_BASED,
    LOAD_BASED,
    BAND_STEERING,
    CLIENT_CAPABILITY
};

/**
 * @brief EasyMesh network information
 */
struct EasyMeshNetwork {
    std::string networkId;             // Network SSID or identifier
    std::string controllerMAC;         // Controller MAC address
    uint32_t agentCount = 0;           // Number of agents
    EasyMeshVersion version = EasyMeshVersion::UNKNOWN;
    bool isOperational = false;
};

/**
 * @brief EasyMesh agent device information
 */
struct EasyMeshAgent {
    std::string macAddress;            // Agent MAC address
    std::string ipAddress;             // Agent IP address
    std::string manufacturer;
    std::string model;
    std::string serialNumber;
    BackhaulType backhaulType = BackhaulType::UNKNOWN;
    int8_t backhaulSignalStrength = 0; // dBm (for WiFi backhaul)
    uint32_t backhaulDataRate = 0;     // Mbps
    std::vector<std::string> radioMACs; // Radio MAC addresses
    bool isConnected = false;
    std::chrono::system_clock::time_point lastSeen;
};

/**
 * @brief EasyMesh radio information
 */
struct EasyMeshRadio {
    std::string macAddress;            // Radio MAC address
    std::string agentMAC;              // Parent agent MAC
    std::string band;                  // 2.4GHz, 5GHz, 6GHz
    uint32_t channel = 0;
    uint32_t bandwidth = 20;           // MHz
    int8_t transmitPower = 0;          // dBm
    uint8_t utilization = 0;           // 0-100%
    uint32_t clientCount = 0;
    std::vector<std::string> operatingClasses;
};

/**
 * @brief EasyMesh BSS (Basic Service Set) information
 */
struct EasyMeshBSS {
    std::string bssid;                 // BSSID (MAC address)
    std::string ssid;
    std::string radioMAC;              // Parent radio MAC
    bool enabled = false;
    uint32_t clientCount = 0;
    std::vector<std::string> clientMACs;
};

/**
 * @brief EasyMesh client station information
 */
struct EasyMeshClient {
    std::string macAddress;            // Client MAC address
    std::string ipAddress;
    std::string bssid;                 // Associated BSSID
    std::string agentMAC;              // Agent serving this client
    int8_t signalStrength = 0;         // dBm
    uint32_t dataRate = 0;             // Mbps
    uint64_t bytesReceived = 0;
    uint64_t bytesSent = 0;
    bool isActive = false;
    std::chrono::system_clock::time_point associationTime;
};

/**
 * @brief Steering request
 */
struct SteeringRequest {
    std::string clientMAC;             // Client to steer
    std::string targetBSSID;           // Target BSSID
    std::string targetAgentMAC;        // Target agent MAC
    SteeringPolicy policy = SteeringPolicy::NONE;
    uint32_t disassocTimer = 0;        // Disassociation timer (ms)
    bool mandatoryTarget = false;      // Mandate vs. opportunity mode
};

/**
 * @brief Channel scan request
 */
struct ChannelScanRequest {
    std::string radioMAC;              // Radio to scan
    std::vector<uint32_t> channels;    // Channels to scan (empty = all)
    uint32_t dwellTime = 100;          // Dwell time per channel (ms)
    bool activeScan = true;            // Active vs passive scan
};

/**
 * @brief Channel scan result
 */
struct ChannelScanResult {
    uint32_t channel = 0;
    int8_t noise = 0;                  // dBm
    uint8_t utilization = 0;           // 0-100%
    uint32_t neighborCount = 0;        // Number of detected APs
    std::vector<std::string> neighborBSSIDs;
};

/**
 * @brief EasyMesh event callback
 */
using EasyMeshEventCallback = std::function<void(const std::string& event,
                                                  const std::map<std::string, std::string>& data)>;

/**
 * @brief EasyMesh R2/R4 Integration
 *
 * Provides high-level access to EasyMesh Multi-AP functionality
 * through TR-181 data model and Ambiorix bus.
 */
class PrplEasyMesh {
public:
    /**
     * @brief Constructor
     * @param amxbClient Ambiorix client instance
     */
    explicit PrplEasyMesh(std::shared_ptr<PrplAmxbClient> amxbClient);

    /**
     * @brief Destructor
     */
    ~PrplEasyMesh();

    // Prevent copying
    PrplEasyMesh(const PrplEasyMesh&) = delete;
    PrplEasyMesh& operator=(const PrplEasyMesh&) = delete;

    /**
     * @brief Initialize EasyMesh integration
     * @return true on success
     */
    bool initialize();

    /**
     * @brief Check if EasyMesh is available
     * @return true if EasyMesh daemon is running
     */
    bool isAvailable();

    // ========================================================================
    // Network and Topology
    // ========================================================================

    /**
     * @brief Get EasyMesh network information
     * @return Network info
     */
    EasyMeshNetwork getNetworkInfo();

    /**
     * @brief Get device role
     * @return EasyMesh role (Controller, Agent, or both)
     */
    EasyMeshRole getRole();

    /**
     * @brief Get EasyMesh protocol version
     * @return Protocol version
     */
    EasyMeshVersion getVersion();

    /**
     * @brief Get all agents in network
     * @return Vector of agent info
     */
    std::vector<EasyMeshAgent> getAgents();

    /**
     * @brief Get specific agent by MAC
     * @param macAddress Agent MAC address
     * @return Agent info or empty optional
     */
    std::optional<EasyMeshAgent> getAgent(const std::string& macAddress);

    /**
     * @brief Get all radios in mesh network
     * @return Vector of radio info
     */
    std::vector<EasyMeshRadio> getRadios();

    /**
     * @brief Get radios for specific agent
     * @param agentMAC Agent MAC address
     * @return Vector of radio info
     */
    std::vector<EasyMeshRadio> getRadios(const std::string& agentMAC);

    /**
     * @brief Get all BSSs in network
     * @return Vector of BSS info
     */
    std::vector<EasyMeshBSS> getBSSs();

    /**
     * @brief Get all connected clients
     * @return Vector of client info
     */
    std::vector<EasyMeshClient> getClients();

    /**
     * @brief Get client by MAC address
     * @param macAddress Client MAC address
     * @return Client info or empty optional
     */
    std::optional<EasyMeshClient> getClient(const std::string& macAddress);

    // ========================================================================
    // Client Steering
    // ========================================================================

    /**
     * @brief Steer client to target BSS/Agent
     * @param request Steering request
     * @return true on success
     */
    bool steerClient(const SteeringRequest& request);

    /**
     * @brief Enable/disable band steering
     * @param enable Enable flag
     * @return true on success
     */
    bool setBandSteeringEnabled(bool enable);

    /**
     * @brief Get band steering status
     * @return true if enabled
     */
    bool isBandSteeringEnabled();

    /**
     * @brief Set steering policy
     * @param policy Steering policy
     * @return true on success
     */
    bool setSteeringPolicy(SteeringPolicy policy);

    // ========================================================================
    // Channel Management
    // ========================================================================

    /**
     * @brief Request channel scan
     * @param request Scan request
     * @return Scan results
     */
    std::vector<ChannelScanResult> scanChannels(const ChannelScanRequest& request);

    /**
     * @brief Set channel for radio
     * @param radioMAC Radio MAC address
     * @param channel Channel number
     * @return true on success
     */
    bool setChannel(const std::string& radioMAC, uint32_t channel);

    /**
     * @brief Enable/disable auto channel selection
     * @param radioMAC Radio MAC address
     * @param enable Enable flag
     * @return true on success
     */
    bool setAutoChannelEnabled(const std::string& radioMAC, bool enable);

    /**
     * @brief Trigger coordinated channel selection
     * @return true on success
     */
    bool triggerChannelSelection();

    // ========================================================================
    // Backhaul Optimization
    // ========================================================================

    /**
     * @brief Optimize backhaul connections
     * @return true on success
     */
    bool optimizeBackhaul();

    /**
     * @brief Get backhaul link quality for agent
     * @param agentMAC Agent MAC address
     * @return Signal strength in dBm, or 0 if not WiFi backhaul
     */
    int8_t getBackhaulLinkQuality(const std::string& agentMAC);

    /**
     * @brief Switch agent to different backhaul
     * @param agentMAC Agent MAC address
     * @param targetBSSID Target backhaul BSSID
     * @return true on success
     */
    bool switchBackhaul(const std::string& agentMAC, const std::string& targetBSSID);

    // ========================================================================
    // Events and Monitoring
    // ========================================================================

    /**
     * @brief Subscribe to EasyMesh events
     * @param event Event name (or "*" for all)
     * @param callback Event callback
     * @return Subscription ID
     */
    uint32_t subscribeToEvents(const std::string& event, EasyMeshEventCallback callback);

    /**
     * @brief Unsubscribe from events
     * @param subscriptionId Subscription ID
     * @return true on success
     */
    bool unsubscribe(uint32_t subscriptionId);

    /**
     * @brief Get network statistics
     * @return Map of statistics
     */
    std::map<std::string, uint64_t> getStatistics();

    // ========================================================================
    // Advanced Features (R4)
    // ========================================================================

    /**
     * @brief Enable/disable DFS (Dynamic Frequency Selection)
     * @param radioMAC Radio MAC address
     * @param enable Enable flag
     * @return true on success
     */
    bool setDFSEnabled(const std::string& radioMAC, bool enable);

    /**
     * @brief Enable/disable MBO (Multi-Band Operation)
     * @return true on success
     */
    bool setMBOEnabled(bool enable);

    /**
     * @brief Get supported service priorities
     * @return Vector of service priority values
     */
    std::vector<uint8_t> getSupportedServicePriorities();

private:
    std::shared_ptr<PrplAmxbClient> amxbClient_;
    bool initialized_;
    EasyMeshRole role_;
    EasyMeshVersion version_;

    // Event subscriptions
    struct Subscription {
        uint32_t id;
        std::string event;
        EasyMeshEventCallback callback;
    };
    std::map<uint32_t, Subscription> subscriptions_;
    uint32_t nextSubscriptionId_;

    // Helper methods
    bool detectEasyMesh();
    std::string getRootPath();
    std::string getAgentPath(const std::string& macAddress);
    std::string getRadioPath(const std::string& macAddress);

    // Parsing helpers
    EasyMeshAgent parseAgent(const std::map<std::string, std::string>& params);
    EasyMeshRadio parseRadio(const std::map<std::string, std::string>& params);
    EasyMeshBSS parseBSS(const std::map<std::string, std::string>& params);
    EasyMeshClient parseClient(const std::map<std::string, std::string>& params);
    ChannelScanResult parseScanResult(const std::map<std::string, std::string>& params);

    // Utility
    static std::string macToPath(const std::string& macAddress);
    static std::string pathToMAC(const std::string& path);
};

} // namespace prpl
} // namespace platform
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_PRPL_EASYMESH_H
