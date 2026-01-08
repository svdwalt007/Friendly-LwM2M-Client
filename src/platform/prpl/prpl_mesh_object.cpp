/**
 * @file prpl_mesh_object.cpp
 * @brief prplMesh LwM2M Object Implementation
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "prpl_mesh_object.h"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

namespace lwm2m {
namespace platform {
namespace prpl {

// ============================================================================
// Helper Functions
// ============================================================================

static std::string calculateHash(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return oss.str();
}

// ============================================================================
// PrplMeshObject Implementation
// ============================================================================

PrplMeshObject::PrplMeshObject(uint16_t instanceId,
                               std::shared_ptr<PrplEasyMesh> easyMesh,
                               std::shared_ptr<PrplDataModel> dataModel)
    : instanceId_(instanceId)
    , easyMesh_(easyMesh)
    , dataModel_(dataModel)
    , initialized_(false)
    , bandSteeringEnabled_(false)
    , autoChannelEnabled_(false)
    , backhaulOptimizationEnabled_(false)
    , steeringPolicy_(MeshSteeringPolicy::NONE) {
}

PrplMeshObject::~PrplMeshObject() = default;

bool PrplMeshObject::initialize() {
    if (!easyMesh_ || !easyMesh_->isAvailable()) {
        return false;
    }

    // Initialize mesh data
    if (!updateMeshData()) {
        return false;
    }

    // Read current configuration
    bandSteeringEnabled_ = easyMesh_->isBandSteeringEnabled();

    initialized_ = true;
    return true;
}

// ============================================================================
// Resource Read Operations
// ============================================================================

bool PrplMeshObject::read(PrplMeshResourceId resourceId, std::string& value) {
    if (!initialized_) {
        return false;
    }

    switch (resourceId) {
        case PrplMeshResourceId::NETWORK_ID:
            value = networkInfo_.networkId;
            return true;

        case PrplMeshResourceId::NETWORK_NAME:
            value = networkInfo_.networkId; // Same as network ID for now
            return true;

        case PrplMeshResourceId::CONTROLLER_MAC:
            value = networkInfo_.controllerMAC;
            return true;

        case PrplMeshResourceId::TOPOLOGY_HASH:
            value = topologyHash();
            return true;

        default:
            return false;
    }
}

bool PrplMeshObject::read(PrplMeshResourceId resourceId, int64_t& value) {
    if (!initialized_) {
        return false;
    }

    switch (resourceId) {
        case PrplMeshResourceId::MESH_VERSION:
            value = convertVersion(networkInfo_.version);
            return true;

        case PrplMeshResourceId::NODE_ROLE:
            value = static_cast<int64_t>(convertRole(easyMesh_->getRole()));
            return true;

        case PrplMeshResourceId::AGENT_COUNT:
            value = metrics_.agentCount;
            return true;

        case PrplMeshResourceId::RADIO_COUNT:
            value = metrics_.radioCount;
            return true;

        case PrplMeshResourceId::BSS_COUNT:
            value = metrics_.bssCount;
            return true;

        case PrplMeshResourceId::CLIENT_COUNT:
            value = metrics_.clientCount;
            return true;

        case PrplMeshResourceId::AVG_BACKHAUL_RSSI:
            value = metrics_.avgBackhaulRSSI;
            return true;

        case PrplMeshResourceId::MIN_BACKHAUL_RSSI:
            value = metrics_.minBackhaulRSSI;
            return true;

        case PrplMeshResourceId::AVG_CLIENT_RSSI:
            value = metrics_.avgClientRSSI;
            return true;

        case PrplMeshResourceId::MIN_CLIENT_RSSI:
            value = metrics_.minClientRSSI;
            return true;

        case PrplMeshResourceId::TOTAL_THROUGHPUT:
            value = metrics_.totalThroughput;
            return true;

        case PrplMeshResourceId::BYTES_SENT:
            value = statistics_.bytesSent;
            return true;

        case PrplMeshResourceId::BYTES_RECEIVED:
            value = statistics_.bytesReceived;
            return true;

        case PrplMeshResourceId::PACKETS_SENT:
            value = statistics_.packetsSent;
            return true;

        case PrplMeshResourceId::PACKETS_RECEIVED:
            value = statistics_.packetsReceived;
            return true;

        case PrplMeshResourceId::STEERING_COUNT:
            value = statistics_.steeringCount;
            return true;

        case PrplMeshResourceId::STEERING_POLICY:
            value = static_cast<int64_t>(steeringPolicy_);
            return true;

        default:
            return false;
    }
}

bool PrplMeshObject::read(PrplMeshResourceId resourceId, bool& value) {
    if (!initialized_) {
        return false;
    }

    switch (resourceId) {
        case PrplMeshResourceId::IS_OPERATIONAL:
            value = networkInfo_.isOperational;
            return true;

        case PrplMeshResourceId::BAND_STEERING_ENABLED:
            value = bandSteeringEnabled_;
            return true;

        case PrplMeshResourceId::AUTO_CHANNEL_ENABLED:
            value = autoChannelEnabled_;
            return true;

        case PrplMeshResourceId::BACKHAUL_OPTIMIZATION:
            value = backhaulOptimizationEnabled_;
            return true;

        default:
            return false;
    }
}

bool PrplMeshObject::readMulti(PrplMeshResourceId resourceId, uint16_t instanceId,
                               std::string& value) {
    if (!initialized_) {
        return false;
    }

    switch (resourceId) {
        case PrplMeshResourceId::AGENT_MAC_LIST:
            if (instanceId < agents_.size()) {
                value = agents_[instanceId].macAddress;
                return true;
            }
            break;

        case PrplMeshResourceId::AGENT_IP_LIST:
            if (instanceId < agents_.size()) {
                value = agents_[instanceId].ipAddress;
                return true;
            }
            break;

        case PrplMeshResourceId::RADIO_MAC_LIST:
            if (instanceId < radios_.size()) {
                value = radios_[instanceId].macAddress;
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

bool PrplMeshObject::readMulti(PrplMeshResourceId resourceId, uint16_t instanceId,
                               int64_t& value) {
    if (!initialized_) {
        return false;
    }

    switch (resourceId) {
        case PrplMeshResourceId::AGENT_STATUS_LIST:
            if (instanceId < agents_.size()) {
                value = agents_[instanceId].isConnected ? 1 : 0;
                return true;
            }
            break;

        case PrplMeshResourceId::RADIO_CHANNEL_LIST:
            if (instanceId < radios_.size()) {
                value = radios_[instanceId].channel;
                return true;
            }
            break;

        case PrplMeshResourceId::RADIO_UTILIZATION_LIST:
            if (instanceId < radios_.size()) {
                value = radios_[instanceId].utilization;
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

// ============================================================================
// Resource Write Operations
// ============================================================================

bool PrplMeshObject::write(PrplMeshResourceId resourceId, const std::string& value) {
    if (!initialized_) {
        return false;
    }

    // Trigger callback if set
    if (writeCallback_) {
        return writeCallback_(resourceId, value);
    }

    return false;
}

bool PrplMeshObject::write(PrplMeshResourceId resourceId, int64_t value) {
    if (!initialized_) {
        return false;
    }

    switch (resourceId) {
        case PrplMeshResourceId::STEERING_POLICY: {
            auto policy = static_cast<MeshSteeringPolicy>(value);
            steeringPolicy_ = policy;

            // Map to EasyMesh steering policy
            SteeringPolicy easyMeshPolicy;
            switch (policy) {
                case MeshSteeringPolicy::RSSI_BASED:
                    easyMeshPolicy = SteeringPolicy::RSSI_BASED;
                    break;
                case MeshSteeringPolicy::LOAD_BASED:
                    easyMeshPolicy = SteeringPolicy::LOAD_BASED;
                    break;
                case MeshSteeringPolicy::BAND_STEERING:
                    easyMeshPolicy = SteeringPolicy::BAND_STEERING;
                    break;
                case MeshSteeringPolicy::CLIENT_CAPABILITY:
                    easyMeshPolicy = SteeringPolicy::CLIENT_CAPABILITY;
                    break;
                default:
                    easyMeshPolicy = SteeringPolicy::NONE;
                    break;
            }

            return easyMesh_->setSteeringPolicy(easyMeshPolicy);
        }

        default:
            break;
    }

    // Trigger callback if set
    if (writeCallback_) {
        return writeCallback_(resourceId, std::to_string(value));
    }

    return false;
}

bool PrplMeshObject::write(PrplMeshResourceId resourceId, bool value) {
    if (!initialized_) {
        return false;
    }

    switch (resourceId) {
        case PrplMeshResourceId::BAND_STEERING_ENABLED:
            bandSteeringEnabled_ = value;
            return easyMesh_->setBandSteeringEnabled(value);

        case PrplMeshResourceId::AUTO_CHANNEL_ENABLED:
            autoChannelEnabled_ = value;
            // Apply to all radios
            for (const auto& radio : radios_) {
                easyMesh_->setAutoChannelEnabled(radio.macAddress, value);
            }
            return true;

        case PrplMeshResourceId::BACKHAUL_OPTIMIZATION:
            backhaulOptimizationEnabled_ = value;
            if (value) {
                return easyMesh_->optimizeBackhaul();
            }
            return true;

        default:
            break;
    }

    // Trigger callback if set
    if (writeCallback_) {
        return writeCallback_(resourceId, value ? "true" : "false");
    }

    return false;
}

// ============================================================================
// Execute Operations
// ============================================================================

bool PrplMeshObject::execute(PrplMeshResourceId resourceId, const std::string& arguments) {
    if (!initialized_) {
        return false;
    }

    switch (resourceId) {
        case PrplMeshResourceId::TRIGGER_TOPOLOGY_QUERY:
            return updateMeshData();

        case PrplMeshResourceId::TRIGGER_CHANNEL_SCAN:
            // Trigger scan on all radios
            for (const auto& radio : radios_) {
                ChannelScanRequest request;
                request.radioMAC = radio.macAddress;
                request.activeScan = true;
                easyMesh_->scanChannels(request);
            }
            return true;

        case PrplMeshResourceId::TRIGGER_CHANNEL_SELECT:
            return easyMesh_->triggerChannelSelection();

        case PrplMeshResourceId::OPTIMIZE_BACKHAUL:
            return easyMesh_->optimizeBackhaul();

        case PrplMeshResourceId::RESET_STATISTICS:
            resetStatistics();
            return true;

        default:
            break;
    }

    // Trigger callback if set
    if (executeCallback_) {
        return executeCallback_(resourceId, arguments);
    }

    return false;
}

// ============================================================================
// Data Update
// ============================================================================

bool PrplMeshObject::updateMeshData() {
    if (!easyMesh_ || !easyMesh_->isAvailable()) {
        return false;
    }

    // Update network info
    networkInfo_ = easyMesh_->getNetworkInfo();

    // Update agents
    agents_ = easyMesh_->getAgents();

    // Update radios
    radios_ = easyMesh_->getRadios();

    // Update clients
    clients_ = easyMesh_->getClients();

    // Calculate metrics
    calculateMetrics();

    // Update statistics
    updateStatistics();

    return true;
}

MeshStatistics PrplMeshObject::getStatistics() {
    return statistics_;
}

MeshTopologyMetrics PrplMeshObject::getTopologyMetrics() {
    return metrics_;
}

void PrplMeshObject::resetStatistics() {
    statistics_ = MeshStatistics();
}

// ============================================================================
// Private Helper Methods
// ============================================================================

void PrplMeshObject::calculateMetrics() {
    metrics_.agentCount = static_cast<uint32_t>(agents_.size());
    metrics_.radioCount = static_cast<uint32_t>(radios_.size());
    metrics_.clientCount = static_cast<uint32_t>(clients_.size());

    // Calculate backhaul metrics
    std::vector<int8_t> backhaulRSSI;
    for (const auto& agent : agents_) {
        if (agent.backhaulSignalStrength != 0) {
            backhaulRSSI.push_back(agent.backhaulSignalStrength);
        }
    }

    if (!backhaulRSSI.empty()) {
        metrics_.avgBackhaulRSSI = std::accumulate(backhaulRSSI.begin(), backhaulRSSI.end(), 0) /
                                   static_cast<int8_t>(backhaulRSSI.size());
        metrics_.minBackhaulRSSI = *std::min_element(backhaulRSSI.begin(), backhaulRSSI.end());
    } else {
        metrics_.avgBackhaulRSSI = 0;
        metrics_.minBackhaulRSSI = 0;
    }

    // Calculate client metrics
    std::vector<int8_t> clientRSSI;
    for (const auto& client : clients_) {
        if (client.signalStrength != 0) {
            clientRSSI.push_back(client.signalStrength);
        }
    }

    if (!clientRSSI.empty()) {
        metrics_.avgClientRSSI = std::accumulate(clientRSSI.begin(), clientRSSI.end(), 0) /
                                 static_cast<int8_t>(clientRSSI.size());
        metrics_.minClientRSSI = *std::min_element(clientRSSI.begin(), clientRSSI.end());
    } else {
        metrics_.avgClientRSSI = 0;
        metrics_.minClientRSSI = 0;
    }

    // Calculate total throughput (sum of all client data rates)
    uint64_t totalDataRate = 0;
    for (const auto& client : clients_) {
        totalDataRate += client.dataRate;
    }
    metrics_.totalThroughput = totalDataRate;

    // Query BSS count
    auto bsss = easyMesh_->getBSSs();
    metrics_.bssCount = static_cast<uint32_t>(bsss.size());
}

void PrplMeshObject::updateStatistics() {
    // Get statistics from EasyMesh
    auto stats = easyMesh_->getStatistics();

    if (stats.count("BytesSent")) {
        statistics_.bytesSent = stats["BytesSent"];
    }
    if (stats.count("BytesReceived")) {
        statistics_.bytesReceived = stats["BytesReceived"];
    }
    if (stats.count("PacketsSent")) {
        statistics_.packetsSent = stats["PacketsSent"];
    }
    if (stats.count("PacketsReceived")) {
        statistics_.packetsReceived = stats["PacketsReceived"];
    }
}

std::string PrplMeshObject::topologyHash() {
    // Create a deterministic string representing the topology
    std::ostringstream oss;

    oss << "network:" << networkInfo_.networkId << ";";
    oss << "controller:" << networkInfo_.controllerMAC << ";";
    oss << "agents:" << agents_.size() << ";";

    // Add agent MACs sorted for determinism
    std::vector<std::string> agentMACs;
    for (const auto& agent : agents_) {
        agentMACs.push_back(agent.macAddress);
    }
    std::sort(agentMACs.begin(), agentMACs.end());

    for (const auto& mac : agentMACs) {
        oss << "a:" << mac << ";";
    }

    // Add radio MACs
    std::vector<std::string> radioMACs;
    for (const auto& radio : radios_) {
        radioMACs.push_back(radio.macAddress);
    }
    std::sort(radioMACs.begin(), radioMACs.end());

    for (const auto& mac : radioMACs) {
        oss << "r:" << mac << ";";
    }

    // Calculate SHA256 hash of the topology string
    return calculateHash(oss.str());
}

MeshNodeRole PrplMeshObject::convertRole(EasyMeshRole role) {
    switch (role) {
        case EasyMeshRole::CONTROLLER:
            return MeshNodeRole::CONTROLLER;
        case EasyMeshRole::AGENT:
            return MeshNodeRole::AGENT;
        case EasyMeshRole::CONTROLLER_AND_AGENT:
            return MeshNodeRole::CONTROLLER_AND_AGENT;
        default:
            return MeshNodeRole::UNKNOWN;
    }
}

uint8_t PrplMeshObject::convertVersion(EasyMeshVersion version) {
    switch (version) {
        case EasyMeshVersion::R1: return 1;
        case EasyMeshVersion::R2: return 2;
        case EasyMeshVersion::R3: return 3;
        case EasyMeshVersion::R4: return 4;
        default: return 0;
    }
}

} // namespace prpl
} // namespace platform
} // namespace lwm2m
