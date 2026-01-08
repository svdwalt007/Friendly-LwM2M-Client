/**
 * @file prpl_easymesh.cpp
 * @brief EasyMesh R2/R4 Integration Implementation
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "prpl_easymesh.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace lwm2m {
namespace platform {
namespace prpl {

// ============================================================================
// TR-181 EasyMesh Paths
// ============================================================================

constexpr const char* TR181_MULTIAP = "Device.WiFi.DataElements";
constexpr const char* TR181_MULTIAP_NETWORK = "Device.WiFi.DataElements.Network";
constexpr const char* TR181_MULTIAP_DEVICE = "Device.WiFi.DataElements.Network.Device";
constexpr const char* TR181_MULTIAP_RADIO = "Device.WiFi.DataElements.Network.Device.Radio";
constexpr const char* TR181_MULTIAP_BSS = "Device.WiFi.DataElements.Network.Device.Radio.BSS";
constexpr const char* TR181_MULTIAP_STA = "Device.WiFi.DataElements.Network.Device.Radio.BSS.STA";

// ============================================================================
// Helper Functions
// ============================================================================

static bool stringToBool(const std::string& str) {
    return (str == "true" || str == "1" || str == "enabled" || str == "yes");
}

static uint32_t stringToUint32(const std::string& str) {
    if (str.empty()) return 0;
    try {
        return static_cast<uint32_t>(std::stoul(str));
    } catch (...) {
        return 0;
    }
}

static int8_t stringToInt8(const std::string& str) {
    if (str.empty()) return 0;
    try {
        return static_cast<int8_t>(std::stoi(str));
    } catch (...) {
        return 0;
    }
}

static uint8_t stringToUint8(const std::string& str) {
    if (str.empty()) return 0;
    try {
        return static_cast<uint8_t>(std::stoul(str));
    } catch (...) {
        return 0;
    }
}

static uint64_t stringToUint64(const std::string& str) {
    if (str.empty()) return 0;
    try {
        return std::stoull(str);
    } catch (...) {
        return 0;
    }
}

// ============================================================================
// PrplEasyMesh Implementation
// ============================================================================

PrplEasyMesh::PrplEasyMesh(std::shared_ptr<PrplAmxbClient> amxbClient)
    : amxbClient_(amxbClient)
    , initialized_(false)
    , role_(EasyMeshRole::UNKNOWN)
    , version_(EasyMeshVersion::UNKNOWN)
    , nextSubscriptionId_(1) {
}

PrplEasyMesh::~PrplEasyMesh() {
    // Unsubscribe from all events
    subscriptions_.clear();
}

bool PrplEasyMesh::initialize() {
    if (!amxbClient_ || !amxbClient_->isConnected()) {
        return false;
    }

    if (!detectEasyMesh()) {
        return false;
    }

    initialized_ = true;
    return true;
}

bool PrplEasyMesh::isAvailable() {
    return initialized_ && amxbClient_ && amxbClient_->isConnected();
}

bool PrplEasyMesh::detectEasyMesh() {
    // Check if Multi-AP data model exists
    if (!amxbClient_->exists(TR181_MULTIAP)) {
        return false;
    }

    // Determine role
    auto networkParams = amxbClient_->get(TR181_MULTIAP_NETWORK);
    if (networkParams.count("ControllerRole")) {
        bool isController = stringToBool(networkParams["ControllerRole"]);
        bool isAgent = stringToBool(networkParams["AgentRole"]);

        if (isController && isAgent) {
            role_ = EasyMeshRole::CONTROLLER_AND_AGENT;
        } else if (isController) {
            role_ = EasyMeshRole::CONTROLLER;
        } else if (isAgent) {
            role_ = EasyMeshRole::AGENT;
        }
    }

    // Determine version
    if (networkParams.count("Version")) {
        std::string ver = networkParams["Version"];
        if (ver.find("R4") != std::string::npos) {
            version_ = EasyMeshVersion::R4;
        } else if (ver.find("R3") != std::string::npos) {
            version_ = EasyMeshVersion::R3;
        } else if (ver.find("R2") != std::string::npos) {
            version_ = EasyMeshVersion::R2;
        } else if (ver.find("R1") != std::string::npos) {
            version_ = EasyMeshVersion::R1;
        }
    }

    return true;
}

std::string PrplEasyMesh::getRootPath() {
    return TR181_MULTIAP_NETWORK;
}

std::string PrplEasyMesh::getAgentPath(const std::string& macAddress) {
    return std::string(TR181_MULTIAP_DEVICE) + ".[ID==\"" + macToPath(macAddress) + "\"]";
}

std::string PrplEasyMesh::getRadioPath(const std::string& macAddress) {
    return std::string(TR181_MULTIAP_RADIO) + ".[ID==\"" + macToPath(macAddress) + "\"]";
}

std::string PrplEasyMesh::macToPath(const std::string& macAddress) {
    // Convert MAC to path-safe format (replace : with -)
    std::string path = macAddress;
    std::replace(path.begin(), path.end(), ':', '-');
    return path;
}

std::string PrplEasyMesh::pathToMAC(const std::string& path) {
    // Extract MAC from path and convert back
    size_t lastDot = path.find_last_of('.');
    if (lastDot != std::string::npos) {
        std::string mac = path.substr(lastDot + 1);
        std::replace(mac.begin(), mac.end(), '-', ':');
        return mac;
    }
    return "";
}

// ============================================================================
// Network and Topology
// ============================================================================

EasyMeshNetwork PrplEasyMesh::getNetworkInfo() {
    EasyMeshNetwork network;

    if (!initialized_) {
        return network;
    }

    auto params = amxbClient_->get(TR181_MULTIAP_NETWORK);

    if (params.count("ID"))
        network.networkId = params["ID"];
    if (params.count("ControllerMAC"))
        network.controllerMAC = params["ControllerMAC"];
    if (params.count("AgentCount"))
        network.agentCount = stringToUint32(params["AgentCount"]);

    network.version = version_;
    network.isOperational = (role_ != EasyMeshRole::UNKNOWN);

    return network;
}

EasyMeshRole PrplEasyMesh::getRole() {
    return role_;
}

EasyMeshVersion PrplEasyMesh::getVersion() {
    return version_;
}

std::vector<EasyMeshAgent> PrplEasyMesh::getAgents() {
    std::vector<EasyMeshAgent> agents;

    if (!initialized_) {
        return agents;
    }

    auto devicePaths = amxbClient_->list(std::string(TR181_MULTIAP_DEVICE) + ".*");

    for (const auto& path : devicePaths) {
        auto params = amxbClient_->get(path);
        if (!params.empty()) {
            auto agent = parseAgent(params);
            agents.push_back(agent);
        }
    }

    return agents;
}

std::optional<EasyMeshAgent> PrplEasyMesh::getAgent(const std::string& macAddress) {
    if (!initialized_) {
        return std::nullopt;
    }

    std::string path = getAgentPath(macAddress);
    auto params = amxbClient_->get(path);

    if (params.empty()) {
        return std::nullopt;
    }

    return parseAgent(params);
}

std::vector<EasyMeshRadio> PrplEasyMesh::getRadios() {
    std::vector<EasyMeshRadio> radios;

    if (!initialized_) {
        return radios;
    }

    auto radioPaths = amxbClient_->list(std::string(TR181_MULTIAP_RADIO) + ".*");

    for (const auto& path : radioPaths) {
        auto params = amxbClient_->get(path);
        if (!params.empty()) {
            auto radio = parseRadio(params);
            radios.push_back(radio);
        }
    }

    return radios;
}

std::vector<EasyMeshRadio> PrplEasyMesh::getRadios(const std::string& agentMAC) {
    std::vector<EasyMeshRadio> radios;

    if (!initialized_) {
        return radios;
    }

    // Get all radios and filter by agent MAC
    auto allRadios = getRadios();
    std::copy_if(allRadios.begin(), allRadios.end(), std::back_inserter(radios),
                 [&agentMAC](const EasyMeshRadio& radio) {
                     return radio.agentMAC == agentMAC;
                 });

    return radios;
}

std::vector<EasyMeshBSS> PrplEasyMesh::getBSSs() {
    std::vector<EasyMeshBSS> bsss;

    if (!initialized_) {
        return bsss;
    }

    auto bssPaths = amxbClient_->list(std::string(TR181_MULTIAP_BSS) + ".*");

    for (const auto& path : bssPaths) {
        auto params = amxbClient_->get(path);
        if (!params.empty()) {
            auto bss = parseBSS(params);
            bsss.push_back(bss);
        }
    }

    return bsss;
}

std::vector<EasyMeshClient> PrplEasyMesh::getClients() {
    std::vector<EasyMeshClient> clients;

    if (!initialized_) {
        return clients;
    }

    auto clientPaths = amxbClient_->list(std::string(TR181_MULTIAP_STA) + ".*");

    for (const auto& path : clientPaths) {
        auto params = amxbClient_->get(path);
        if (!params.empty()) {
            auto client = parseClient(params);
            clients.push_back(client);
        }
    }

    return clients;
}

std::optional<EasyMeshClient> PrplEasyMesh::getClient(const std::string& macAddress) {
    if (!initialized_) {
        return std::nullopt;
    }

    // Search through all clients
    auto clients = getClients();
    auto it = std::find_if(clients.begin(), clients.end(),
                          [&macAddress](const EasyMeshClient& c) {
                              return c.macAddress == macAddress;
                          });

    if (it != clients.end()) {
        return *it;
    }

    return std::nullopt;
}

// ============================================================================
// Client Steering
// ============================================================================

bool PrplEasyMesh::steerClient(const SteeringRequest& request) {
    if (!initialized_) {
        return false;
    }

    // Invoke steering method on controller
    std::map<std::string, std::string> args;
    args["ClientMAC"] = request.clientMAC;
    args["TargetBSSID"] = request.targetBSSID;
    args["DisassociationTimer"] = std::to_string(request.disassocTimer);
    args["MandatoryTarget"] = request.mandatoryTarget ? "true" : "false";

    std::map<std::string, std::string> result;
    return amxbClient_->invoke(TR181_MULTIAP_NETWORK, "SteerClient", args, result);
}

bool PrplEasyMesh::setBandSteeringEnabled(bool enable) {
    if (!initialized_) {
        return false;
    }

    std::map<std::string, std::string> params;
    params["BandSteeringEnabled"] = enable ? "true" : "false";

    return amxbClient_->set(TR181_MULTIAP_NETWORK, params);
}

bool PrplEasyMesh::isBandSteeringEnabled() {
    if (!initialized_) {
        return false;
    }

    std::string value = amxbClient_->getValue(TR181_MULTIAP_NETWORK, "BandSteeringEnabled");
    return stringToBool(value);
}

bool PrplEasyMesh::setSteeringPolicy(SteeringPolicy policy) {
    if (!initialized_) {
        return false;
    }

    std::string policyStr;
    switch (policy) {
        case SteeringPolicy::RSSI_BASED: policyStr = "RSSI"; break;
        case SteeringPolicy::LOAD_BASED: policyStr = "Load"; break;
        case SteeringPolicy::BAND_STEERING: policyStr = "Band"; break;
        case SteeringPolicy::CLIENT_CAPABILITY: policyStr = "Capability"; break;
        default: policyStr = "None"; break;
    }

    std::map<std::string, std::string> params;
    params["SteeringPolicy"] = policyStr;

    return amxbClient_->set(TR181_MULTIAP_NETWORK, params);
}

// ============================================================================
// Channel Management
// ============================================================================

std::vector<ChannelScanResult> PrplEasyMesh::scanChannels(const ChannelScanRequest& request) {
    std::vector<ChannelScanResult> results;

    if (!initialized_) {
        return results;
    }

    // Build channel list
    std::ostringstream channelList;
    for (size_t i = 0; i < request.channels.size(); ++i) {
        if (i > 0) channelList << ",";
        channelList << request.channels[i];
    }

    std::map<std::string, std::string> args;
    args["RadioMAC"] = request.radioMAC;
    args["Channels"] = channelList.str();
    args["DwellTime"] = std::to_string(request.dwellTime);
    args["ActiveScan"] = request.activeScan ? "true" : "false";

    std::map<std::string, std::string> result;
    if (amxbClient_->invoke(getRadioPath(request.radioMAC), "ScanChannels", args, result)) {
        // Parse results
        // Implementation depends on result format
    }

    return results;
}

bool PrplEasyMesh::setChannel(const std::string& radioMAC, uint32_t channel) {
    if (!initialized_) {
        return false;
    }

    std::map<std::string, std::string> params;
    params["Channel"] = std::to_string(channel);

    return amxbClient_->set(getRadioPath(radioMAC), params);
}

bool PrplEasyMesh::setAutoChannelEnabled(const std::string& radioMAC, bool enable) {
    if (!initialized_) {
        return false;
    }

    std::map<std::string, std::string> params;
    params["AutoChannelEnabled"] = enable ? "true" : "false";

    return amxbClient_->set(getRadioPath(radioMAC), params);
}

bool PrplEasyMesh::triggerChannelSelection() {
    if (!initialized_) {
        return false;
    }

    return amxbClient_->invoke(TR181_MULTIAP_NETWORK, "TriggerChannelSelection");
}

// ============================================================================
// Backhaul Optimization
// ============================================================================

bool PrplEasyMesh::optimizeBackhaul() {
    if (!initialized_) {
        return false;
    }

    return amxbClient_->invoke(TR181_MULTIAP_NETWORK, "OptimizeBackhaul");
}

int8_t PrplEasyMesh::getBackhaulLinkQuality(const std::string& agentMAC) {
    if (!initialized_) {
        return 0;
    }

    auto agent = getAgent(agentMAC);
    if (agent.has_value()) {
        return agent->backhaulSignalStrength;
    }

    return 0;
}

bool PrplEasyMesh::switchBackhaul(const std::string& agentMAC, const std::string& targetBSSID) {
    if (!initialized_) {
        return false;
    }

    std::map<std::string, std::string> args;
    args["AgentMAC"] = agentMAC;
    args["TargetBSSID"] = targetBSSID;

    return amxbClient_->invoke(TR181_MULTIAP_NETWORK, "SwitchBackhaul", args);
}

// ============================================================================
// Events and Monitoring
// ============================================================================

uint32_t PrplEasyMesh::subscribeToEvents(const std::string& event,
                                         EasyMeshEventCallback callback) {
    if (!initialized_) {
        return 0;
    }

    // Subscribe via Ambiorix
    uint32_t amxbSubId = amxbClient_->subscribe(
        TR181_MULTIAP_NETWORK,
        event,
        [callback](const std::string& evt, const std::string& obj,
                   const std::map<std::string, std::string>& data) {
            callback(evt, data);
        });

    if (amxbSubId == 0) {
        return 0;
    }

    // Store subscription
    Subscription sub;
    sub.id = nextSubscriptionId_++;
    sub.event = event;
    sub.callback = callback;

    subscriptions_[sub.id] = sub;

    return sub.id;
}

bool PrplEasyMesh::unsubscribe(uint32_t subscriptionId) {
    auto it = subscriptions_.find(subscriptionId);
    if (it == subscriptions_.end()) {
        return false;
    }

    subscriptions_.erase(it);
    return true;
}

std::map<std::string, uint64_t> PrplEasyMesh::getStatistics() {
    std::map<std::string, uint64_t> stats;

    if (!initialized_) {
        return stats;
    }

    auto params = amxbClient_->get(TR181_MULTIAP_NETWORK);

    if (params.count("BytesSent"))
        stats["BytesSent"] = stringToUint64(params["BytesSent"]);
    if (params.count("BytesReceived"))
        stats["BytesReceived"] = stringToUint64(params["BytesReceived"]);
    if (params.count("PacketsSent"))
        stats["PacketsSent"] = stringToUint64(params["PacketsSent"]);
    if (params.count("PacketsReceived"))
        stats["PacketsReceived"] = stringToUint64(params["PacketsReceived"]);

    return stats;
}

// ============================================================================
// Advanced Features (R4)
// ============================================================================

bool PrplEasyMesh::setDFSEnabled(const std::string& radioMAC, bool enable) {
    if (!initialized_) {
        return false;
    }

    std::map<std::string, std::string> params;
    params["DFSEnabled"] = enable ? "true" : "false";

    return amxbClient_->set(getRadioPath(radioMAC), params);
}

bool PrplEasyMesh::setMBOEnabled(bool enable) {
    if (!initialized_) {
        return false;
    }

    std::map<std::string, std::string> params;
    params["MBOEnabled"] = enable ? "true" : "false";

    return amxbClient_->set(TR181_MULTIAP_NETWORK, params);
}

std::vector<uint8_t> PrplEasyMesh::getSupportedServicePriorities() {
    std::vector<uint8_t> priorities;

    if (!initialized_) {
        return priorities;
    }

    // Query supported service priorities
    std::string prioStr = amxbClient_->getValue(TR181_MULTIAP_NETWORK,
                                                 "SupportedServicePriorities");

    // Parse comma-separated list
    std::istringstream iss(prioStr);
    std::string prio;
    while (std::getline(iss, prio, ',')) {
        priorities.push_back(stringToUint8(prio));
    }

    return priorities;
}

// ============================================================================
// Parsing Helpers
// ============================================================================

EasyMeshAgent PrplEasyMesh::parseAgent(const std::map<std::string, std::string>& params) {
    EasyMeshAgent agent;

    if (params.count("ID"))
        agent.macAddress = params.at("ID");
    if (params.count("IPAddress"))
        agent.ipAddress = params.at("IPAddress");
    if (params.count("Manufacturer"))
        agent.manufacturer = params.at("Manufacturer");
    if (params.count("ModelName"))
        agent.model = params.at("ModelName");
    if (params.count("SerialNumber"))
        agent.serialNumber = params.at("SerialNumber");
    if (params.count("Connected"))
        agent.isConnected = stringToBool(params.at("Connected"));

    // Parse backhaul info
    if (params.count("BackhaulType")) {
        std::string type = params.at("BackhaulType");
        if (type == "Ethernet") {
            agent.backhaulType = BackhaulType::ETHERNET;
        } else if (type == "WiFi2G") {
            agent.backhaulType = BackhaulType::WIFI_2G;
        } else if (type == "WiFi5G") {
            agent.backhaulType = BackhaulType::WIFI_5G;
        } else if (type == "WiFi6G") {
            agent.backhaulType = BackhaulType::WIFI_6G;
        }
    }

    if (params.count("BackhaulSignalStrength"))
        agent.backhaulSignalStrength = stringToInt8(params.at("BackhaulSignalStrength"));
    if (params.count("BackhaulDataRate"))
        agent.backhaulDataRate = stringToUint32(params.at("BackhaulDataRate"));

    return agent;
}

EasyMeshRadio PrplEasyMesh::parseRadio(const std::map<std::string, std::string>& params) {
    EasyMeshRadio radio;

    if (params.count("ID"))
        radio.macAddress = params.at("ID");
    if (params.count("Band"))
        radio.band = params.at("Band");
    if (params.count("Channel"))
        radio.channel = stringToUint32(params.at("Channel"));
    if (params.count("Bandwidth"))
        radio.bandwidth = stringToUint32(params.at("Bandwidth"));
    if (params.count("TransmitPower"))
        radio.transmitPower = stringToInt8(params.at("TransmitPower"));
    if (params.count("Utilization"))
        radio.utilization = stringToUint8(params.at("Utilization"));
    if (params.count("ClientCount"))
        radio.clientCount = stringToUint32(params.at("ClientCount"));

    return radio;
}

EasyMeshBSS PrplEasyMesh::parseBSS(const std::map<std::string, std::string>& params) {
    EasyMeshBSS bss;

    if (params.count("BSSID"))
        bss.bssid = params.at("BSSID");
    if (params.count("SSID"))
        bss.ssid = params.at("SSID");
    if (params.count("Enabled"))
        bss.enabled = stringToBool(params.at("Enabled"));
    if (params.count("ClientCount"))
        bss.clientCount = stringToUint32(params.at("ClientCount"));

    return bss;
}

EasyMeshClient PrplEasyMesh::parseClient(const std::map<std::string, std::string>& params) {
    EasyMeshClient client;

    if (params.count("MACAddress"))
        client.macAddress = params.at("MACAddress");
    if (params.count("IPAddress"))
        client.ipAddress = params.at("IPAddress");
    if (params.count("BSSID"))
        client.bssid = params.at("BSSID");
    if (params.count("Active"))
        client.isActive = stringToBool(params.at("Active"));
    if (params.count("SignalStrength"))
        client.signalStrength = stringToInt8(params.at("SignalStrength"));
    if (params.count("DataRate"))
        client.dataRate = stringToUint32(params.at("DataRate"));
    if (params.count("BytesReceived"))
        client.bytesReceived = stringToUint64(params.at("BytesReceived"));
    if (params.count("BytesSent"))
        client.bytesSent = stringToUint64(params.at("BytesSent"));

    return client;
}

ChannelScanResult PrplEasyMesh::parseScanResult(const std::map<std::string, std::string>& params) {
    ChannelScanResult result;

    if (params.count("Channel"))
        result.channel = stringToUint32(params.at("Channel"));
    if (params.count("Noise"))
        result.noise = stringToInt8(params.at("Noise"));
    if (params.count("Utilization"))
        result.utilization = stringToUint8(params.at("Utilization"));
    if (params.count("NeighborCount"))
        result.neighborCount = stringToUint32(params.at("NeighborCount"));

    return result;
}

} // namespace prpl
} // namespace platform
} // namespace lwm2m
