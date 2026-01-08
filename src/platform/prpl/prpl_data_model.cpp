/**
 * @file prpl_data_model.cpp
 * @brief TR-181 Data Model Implementation
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "prpl_data_model.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace lwm2m {
namespace platform {
namespace prpl {

// ============================================================================
// TR-181 Base Paths
// ============================================================================

constexpr const char* TR181_DEVICE_INFO = "Device.DeviceInfo";
constexpr const char* TR181_WIFI = "Device.WiFi";
constexpr const char* TR181_WIFI_RADIO = "Device.WiFi.Radio";
constexpr const char* TR181_WIFI_SSID = "Device.WiFi.SSID";
constexpr const char* TR181_WIFI_AP = "Device.WiFi.AccessPoint";
constexpr const char* TR181_ETHERNET = "Device.Ethernet";
constexpr const char* TR181_ETHERNET_INTERFACE = "Device.Ethernet.Interface";
constexpr const char* TR181_IP = "Device.IP";
constexpr const char* TR181_IP_INTERFACE = "Device.IP.Interface";
constexpr const char* TR181_HOSTS = "Device.Hosts";
constexpr const char* TR181_HOSTS_HOST = "Device.Hosts.Host";

// ============================================================================
// Helper Functions
// ============================================================================

static std::string extractPathIndex(const std::string& path) {
    // Extract index from path like "Device.WiFi.Radio.1" -> "1"
    size_t lastDot = path.find_last_of('.');
    if (lastDot != std::string::npos) {
        return path.substr(lastDot + 1);
    }
    return "";
}

static uint32_t extractPathIndexInt(const std::string& path) {
    std::string idx = extractPathIndex(path);
    if (!idx.empty() && std::all_of(idx.begin(), idx.end(), ::isdigit)) {
        return static_cast<uint32_t>(std::stoul(idx));
    }
    return 0;
}

static bool stringToBool(const std::string& str) {
    return (str == "true" || str == "1" || str == "enabled" || str == "yes");
}

static int64_t stringToInt64(const std::string& str) {
    if (str.empty()) return 0;
    try {
        return std::stoll(str);
    } catch (...) {
        return 0;
    }
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

static uint64_t stringToUint64(const std::string& str) {
    if (str.empty()) return 0;
    try {
        return std::stoull(str);
    } catch (...) {
        return 0;
    }
}

// ============================================================================
// PrplDataModel Implementation
// ============================================================================

PrplDataModel::PrplDataModel(std::shared_ptr<PrplAmxbClient> amxbClient)
    : amxbClient_(amxbClient)
    , initialized_(false)
    , cacheEnabled_(true)
    , cacheTTL_(5) {
}

PrplDataModel::~PrplDataModel() = default;

bool PrplDataModel::initialize() {
    if (!amxbClient_ || !amxbClient_->isConnected()) {
        return false;
    }

    // Verify TR-181 root objects exist
    if (!amxbClient_->exists(TR181_DEVICE_INFO)) {
        return false;
    }

    initialized_ = true;
    return true;
}

// ============================================================================
// Device.DeviceInfo
// ============================================================================

std::map<std::string, std::string> PrplDataModel::getDeviceInfo() {
    if (!initialized_) {
        return {};
    }

    return getCached(TR181_DEVICE_INFO);
}

DeviceInfo PrplDataModel::getDeviceInfoStructured() {
    DeviceInfo info;
    auto params = getDeviceInfo();

    if (params.count("Manufacturer"))
        info.manufacturer = params["Manufacturer"];
    if (params.count("ManufacturerOUI"))
        info.manufacturerOUI = params["ManufacturerOUI"];
    if (params.count("ModelName"))
        info.modelName = params["ModelName"];
    if (params.count("Description"))
        info.description = params["Description"];
    if (params.count("ProductClass"))
        info.productClass = params["ProductClass"];
    if (params.count("SerialNumber"))
        info.serialNumber = params["SerialNumber"];
    if (params.count("HardwareVersion"))
        info.hardwareVersion = params["HardwareVersion"];
    if (params.count("SoftwareVersion"))
        info.softwareVersion = params["SoftwareVersion"];
    if (params.count("ProvisioningCode"))
        info.provisioningCode = params["ProvisioningCode"];
    if (params.count("UpTime"))
        info.upTime = stringToUint32(params["UpTime"]);

    return info;
}

uint32_t PrplDataModel::getUptime() {
    if (!initialized_) {
        return 0;
    }

    std::string uptime = amxbClient_->getValue(TR181_DEVICE_INFO, "UpTime");
    return stringToUint32(uptime);
}

// ============================================================================
// Device.WiFi
// ============================================================================

std::vector<WiFiRadioInfo> PrplDataModel::getWiFiRadios() {
    std::vector<WiFiRadioInfo> radios;

    if (!initialized_) {
        return radios;
    }

    // List all radio instances
    auto radioPaths = amxbClient_->list(std::string(TR181_WIFI_RADIO) + ".*");

    for (const auto& path : radioPaths) {
        auto params = getCached(path);
        if (!params.empty()) {
            auto radio = parseWiFiRadio(params);
            radio.path = path;
            radios.push_back(radio);
        }
    }

    return radios;
}

std::optional<WiFiRadioInfo> PrplDataModel::getWiFiRadio(uint32_t index) {
    if (!initialized_ || index == 0) {
        return std::nullopt;
    }

    std::string path = getWiFiRadioPath(index);
    auto params = getCached(path);

    if (params.empty()) {
        return std::nullopt;
    }

    auto radio = parseWiFiRadio(params);
    radio.path = path;
    return radio;
}

std::vector<WiFiSSIDInfo> PrplDataModel::getWiFiSSIDs() {
    std::vector<WiFiSSIDInfo> ssids;

    if (!initialized_) {
        return ssids;
    }

    auto ssidPaths = amxbClient_->list(std::string(TR181_WIFI_SSID) + ".*");

    for (const auto& path : ssidPaths) {
        auto params = getCached(path);
        if (!params.empty()) {
            auto ssid = parseWiFiSSID(params);
            ssid.path = path;
            ssids.push_back(ssid);
        }
    }

    return ssids;
}

std::vector<WiFiAccessPointInfo> PrplDataModel::getWiFiAccessPoints() {
    std::vector<WiFiAccessPointInfo> aps;

    if (!initialized_) {
        return aps;
    }

    auto apPaths = amxbClient_->list(std::string(TR181_WIFI_AP) + ".*");

    for (const auto& path : apPaths) {
        auto params = getCached(path);
        if (!params.empty()) {
            auto ap = parseWiFiAccessPoint(params);
            ap.path = path;
            aps.push_back(ap);
        }
    }

    return aps;
}

std::vector<WiFiAssociatedDevice> PrplDataModel::getWiFiAssociatedDevices() {
    std::vector<WiFiAssociatedDevice> devices;

    if (!initialized_) {
        return devices;
    }

    // Get all access points
    auto apPaths = amxbClient_->list(std::string(TR181_WIFI_AP) + ".*");

    for (const auto& apPath : apPaths) {
        // Get associated devices for this AP
        auto devicePaths = amxbClient_->list(apPath + ".AssociatedDevice.*");

        for (const auto& devicePath : devicePaths) {
            auto params = getCached(devicePath);
            if (!params.empty()) {
                auto device = parseWiFiAssociatedDevice(params);
                device.path = devicePath;
                devices.push_back(device);
            }
        }
    }

    return devices;
}

std::vector<WiFiAssociatedDevice> PrplDataModel::getWiFiStations(uint32_t apIndex) {
    std::vector<WiFiAssociatedDevice> devices;

    if (!initialized_ || apIndex == 0) {
        return devices;
    }

    std::string apPath = getWiFiAccessPointPath(apIndex);
    auto devicePaths = amxbClient_->list(apPath + ".AssociatedDevice.*");

    for (const auto& devicePath : devicePaths) {
        auto params = getCached(devicePath);
        if (!params.empty()) {
            auto device = parseWiFiAssociatedDevice(params);
            device.path = devicePath;
            devices.push_back(device);
        }
    }

    return devices;
}

std::map<std::string, std::map<std::string, std::string>> PrplDataModel::getWiFiStations() {
    std::map<std::string, std::map<std::string, std::string>> result;

    auto devices = getWiFiAssociatedDevices();
    for (const auto& device : devices) {
        std::map<std::string, std::string> params;
        params["MACAddress"] = device.macAddress;
        params["IPAddress"] = device.ipAddress;
        params["Active"] = device.active ? "true" : "false";
        params["SignalStrength"] = std::to_string(device.signalStrength);
        result[device.path] = params;
    }

    return result;
}

// ============================================================================
// Device.Ethernet
// ============================================================================

std::vector<EthernetInterfaceInfo> PrplDataModel::getEthernetInterfaces() {
    std::vector<EthernetInterfaceInfo> interfaces;

    if (!initialized_) {
        return interfaces;
    }

    auto ifPaths = amxbClient_->list(std::string(TR181_ETHERNET_INTERFACE) + ".*");

    for (const auto& path : ifPaths) {
        auto params = getCached(path);
        if (!params.empty()) {
            auto iface = parseEthernetInterface(params);
            iface.path = path;
            interfaces.push_back(iface);
        }
    }

    return interfaces;
}

std::optional<EthernetInterfaceInfo> PrplDataModel::getEthernetInterface(uint32_t index) {
    if (!initialized_ || index == 0) {
        return std::nullopt;
    }

    std::string path = getEthernetInterfacePath(index);
    auto params = getCached(path);

    if (params.empty()) {
        return std::nullopt;
    }

    auto iface = parseEthernetInterface(params);
    iface.path = path;
    return iface;
}

// ============================================================================
// Device.IP
// ============================================================================

std::vector<IPInterfaceInfo> PrplDataModel::getIPInterfaces() {
    std::vector<IPInterfaceInfo> interfaces;

    if (!initialized_) {
        return interfaces;
    }

    auto ifPaths = amxbClient_->list(std::string(TR181_IP_INTERFACE) + ".*");

    for (const auto& path : ifPaths) {
        auto params = getCached(path);
        if (!params.empty()) {
            auto iface = parseIPInterface(params);
            iface.path = path;
            interfaces.push_back(iface);
        }
    }

    return interfaces;
}

std::map<std::string, std::map<std::string, std::string>> PrplDataModel::getIPInterfacesMap() {
    std::map<std::string, std::map<std::string, std::string>> result;

    auto interfaces = getIPInterfaces();
    for (const auto& iface : interfaces) {
        std::map<std::string, std::string> params;
        params["Name"] = iface.name;
        params["Type"] = iface.type;
        params["Status"] = iface.status;
        params["Enable"] = iface.enabled ? "true" : "false";
        result[iface.path] = params;
    }

    return result;
}

std::optional<IPInterfaceInfo> PrplDataModel::getIPInterface(uint32_t index) {
    if (!initialized_ || index == 0) {
        return std::nullopt;
    }

    std::string path = getIPInterfacePath(index);
    auto params = getCached(path);

    if (params.empty()) {
        return std::nullopt;
    }

    auto iface = parseIPInterface(params);
    iface.path = path;
    return iface;
}

// ============================================================================
// Device.Hosts
// ============================================================================

std::vector<HostInfo> PrplDataModel::getHosts() {
    std::vector<HostInfo> hosts;

    if (!initialized_) {
        return hosts;
    }

    auto hostPaths = amxbClient_->list(std::string(TR181_HOSTS_HOST) + ".*");

    for (const auto& path : hostPaths) {
        auto params = getCached(path);
        if (!params.empty()) {
            auto host = parseHost(params);
            host.path = path;
            hosts.push_back(host);
        }
    }

    return hosts;
}

std::vector<HostInfo> PrplDataModel::getActiveHosts() {
    auto allHosts = getHosts();
    std::vector<HostInfo> activeHosts;

    std::copy_if(allHosts.begin(), allHosts.end(), std::back_inserter(activeHosts),
                 [](const HostInfo& host) { return host.active; });

    return activeHosts;
}

uint32_t PrplDataModel::getHostCount() {
    if (!initialized_) {
        return 0;
    }

    std::string count = amxbClient_->getValue(TR181_HOSTS, "HostNumberOfEntries");
    return stringToUint32(count);
}

// ============================================================================
// Utility Methods
// ============================================================================

WiFiStandard PrplDataModel::parseWiFiStandard(const std::string& standard) {
    std::string lowerStd = standard;
    std::transform(lowerStd.begin(), lowerStd.end(), lowerStd.begin(), ::tolower);

    if (lowerStd.find("a") != std::string::npos && lowerStd.find("ac") == std::string::npos &&
        lowerStd.find("ax") == std::string::npos) {
        return WiFiStandard::IEEE_802_11A;
    } else if (lowerStd.find("b") != std::string::npos) {
        return WiFiStandard::IEEE_802_11B;
    } else if (lowerStd.find("g") != std::string::npos) {
        return WiFiStandard::IEEE_802_11G;
    } else if (lowerStd.find("n") != std::string::npos && lowerStd.find("an") == std::string::npos) {
        return WiFiStandard::IEEE_802_11N;
    } else if (lowerStd.find("ac") != std::string::npos) {
        return WiFiStandard::IEEE_802_11AC;
    } else if (lowerStd.find("ax") != std::string::npos) {
        return WiFiStandard::IEEE_802_11AX;
    } else if (lowerStd.find("be") != std::string::npos) {
        return WiFiStandard::IEEE_802_11BE;
    }

    return WiFiStandard::UNKNOWN;
}

WiFiBand PrplDataModel::parseWiFiBand(const std::string& band) {
    if (band.find("2.4") != std::string::npos || band.find("2400") != std::string::npos) {
        return WiFiBand::BAND_2_4GHZ;
    } else if (band.find("5") != std::string::npos || band.find("5000") != std::string::npos) {
        return WiFiBand::BAND_5GHZ;
    } else if (band.find("6") != std::string::npos || band.find("6000") != std::string::npos) {
        return WiFiBand::BAND_6GHZ;
    }

    return WiFiBand::UNKNOWN;
}

WiFiBand PrplDataModel::channelToBand(uint32_t channel) {
    if (channel >= 1 && channel <= 14) {
        return WiFiBand::BAND_2_4GHZ;
    } else if (channel >= 36 && channel <= 165) {
        return WiFiBand::BAND_5GHZ;
    } else if (channel >= 1 && channel <= 233) {
        // 6 GHz channels use different numbering
        return WiFiBand::BAND_6GHZ;
    }

    return WiFiBand::UNKNOWN;
}

void PrplDataModel::setCacheEnabled(bool enable, uint32_t ttlSeconds) {
    cacheEnabled_ = enable;
    cacheTTL_ = ttlSeconds;

    if (!enable) {
        clearCache();
    }
}

void PrplDataModel::clearCache() {
    cache_.clear();
}

// ============================================================================
// Private Helper Methods
// ============================================================================

std::map<std::string, std::string> PrplDataModel::getCached(const std::string& path) {
    if (cacheEnabled_ && isCacheValid(path)) {
        return cache_[path].data;
    }

    // Fetch from Ambiorix
    auto data = amxbClient_->get(path);

    if (cacheEnabled_) {
        updateCache(path, data);
    }

    return data;
}

void PrplDataModel::updateCache(const std::string& path,
                                const std::map<std::string, std::string>& data) {
    CacheEntry entry;
    entry.timestamp = std::chrono::steady_clock::now();
    entry.data = data;
    cache_[path] = entry;
}

bool PrplDataModel::isCacheValid(const std::string& path) {
    auto it = cache_.find(path);
    if (it == cache_.end()) {
        return false;
    }

    auto now = std::chrono::steady_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.timestamp);

    return age.count() < cacheTTL_;
}

std::string PrplDataModel::getWiFiRadioPath(uint32_t index) {
    return std::string(TR181_WIFI_RADIO) + "." + std::to_string(index);
}

std::string PrplDataModel::getWiFiSSIDPath(uint32_t index) {
    return std::string(TR181_WIFI_SSID) + "." + std::to_string(index);
}

std::string PrplDataModel::getWiFiAccessPointPath(uint32_t index) {
    return std::string(TR181_WIFI_AP) + "." + std::to_string(index);
}

std::string PrplDataModel::getEthernetInterfacePath(uint32_t index) {
    return std::string(TR181_ETHERNET_INTERFACE) + "." + std::to_string(index);
}

std::string PrplDataModel::getIPInterfacePath(uint32_t index) {
    return std::string(TR181_IP_INTERFACE) + "." + std::to_string(index);
}

WiFiRadioInfo PrplDataModel::parseWiFiRadio(const std::map<std::string, std::string>& params) {
    WiFiRadioInfo info;

    if (params.count("Enable"))
        info.enabled = stringToBool(params.at("Enable"));
    if (params.count("Name"))
        info.name = params.at("Name");
    if (params.count("OperatingStandards"))
        info.operatingStandards = params.at("OperatingStandards");
    if (params.count("Channel"))
        info.channel = stringToUint32(params.at("Channel"));
    if (params.count("TransmitPower"))
        info.transmitPower = stringToInt8(params.at("TransmitPower"));
    if (params.count("OperatingChannelBandwidth"))
        info.operatingChannelBandwidth = stringToUint32(params.at("OperatingChannelBandwidth"));

    // Determine standard and band
    if (!info.operatingStandards.empty()) {
        info.standard = parseWiFiStandard(info.operatingStandards);
    }

    if (info.channel > 0) {
        info.band = channelToBand(info.channel);
    }

    // Parse possible channels list
    if (params.count("PossibleChannels")) {
        std::istringstream iss(params.at("PossibleChannels"));
        std::string channel;
        while (std::getline(iss, channel, ',')) {
            info.possibleChannels.push_back(stringToUint32(channel));
        }
    }

    return info;
}

WiFiSSIDInfo PrplDataModel::parseWiFiSSID(const std::map<std::string, std::string>& params) {
    WiFiSSIDInfo info;

    if (params.count("Enable"))
        info.enabled = stringToBool(params.at("Enable"));
    if (params.count("SSID"))
        info.ssid = params.at("SSID");
    if (params.count("BSSID"))
        info.bssid = params.at("BSSID");
    if (params.count("Status"))
        info.status = params.at("Status");

    return info;
}

WiFiAccessPointInfo PrplDataModel::parseWiFiAccessPoint(
    const std::map<std::string, std::string>& params) {
    WiFiAccessPointInfo info;

    if (params.count("Enable"))
        info.enabled = stringToBool(params.at("Enable"));
    if (params.count("SSIDReference"))
        info.ssidReference = params.at("SSIDReference");
    if (params.count("Security.ModeEnabled"))
        info.securityMode = params.at("Security.ModeEnabled");
    if (params.count("AssociatedDeviceNumberOfEntries"))
        info.associatedDeviceCount = stringToUint32(params.at("AssociatedDeviceNumberOfEntries"));

    return info;
}

WiFiAssociatedDevice PrplDataModel::parseWiFiAssociatedDevice(
    const std::map<std::string, std::string>& params) {
    WiFiAssociatedDevice info;

    if (params.count("MACAddress"))
        info.macAddress = params.at("MACAddress");
    if (params.count("IPAddress"))
        info.ipAddress = params.at("IPAddress");
    if (params.count("Active"))
        info.active = stringToBool(params.at("Active"));
    if (params.count("SignalStrength"))
        info.signalStrength = stringToInt8(params.at("SignalStrength"));
    if (params.count("LastDataDownlinkRate"))
        info.lastDataDownlinkRate = stringToUint32(params.at("LastDataDownlinkRate"));
    if (params.count("LastDataUplinkRate"))
        info.lastDataUplinkRate = stringToUint32(params.at("LastDataUplinkRate"));
    if (params.count("BytesReceived"))
        info.bytesReceived = stringToUint64(params.at("BytesReceived"));
    if (params.count("BytesSent"))
        info.bytesSent = stringToUint64(params.at("BytesSent"));

    return info;
}

EthernetInterfaceInfo PrplDataModel::parseEthernetInterface(
    const std::map<std::string, std::string>& params) {
    EthernetInterfaceInfo info;

    if (params.count("Enable"))
        info.enabled = stringToBool(params.at("Enable"));
    if (params.count("Name"))
        info.name = params.at("Name");
    if (params.count("Status"))
        info.status = params.at("Status");
    if (params.count("MACAddress"))
        info.macAddress = params.at("MACAddress");
    if (params.count("MaxBitRate"))
        info.maxBitRate = stringToUint32(params.at("MaxBitRate"));
    if (params.count("DuplexMode"))
        info.duplexMode = params.at("DuplexMode");

    return info;
}

IPInterfaceInfo PrplDataModel::parseIPInterface(const std::map<std::string, std::string>& params) {
    IPInterfaceInfo info;

    if (params.count("Enable"))
        info.enabled = stringToBool(params.at("Enable"));
    if (params.count("Name"))
        info.name = params.at("Name");
    if (params.count("Type"))
        info.type = params.at("Type");
    if (params.count("Status"))
        info.status = params.at("Status");

    // Parse IPv4 addresses
    if (params.count("IPv4Address")) {
        std::istringstream iss(params.at("IPv4Address"));
        std::string addr;
        while (std::getline(iss, addr, ',')) {
            info.ipv4Addresses.push_back(addr);
        }
    }

    // Parse IPv6 addresses
    if (params.count("IPv6Address")) {
        std::istringstream iss(params.at("IPv6Address"));
        std::string addr;
        while (std::getline(iss, addr, ',')) {
            info.ipv6Addresses.push_back(addr);
        }
    }

    return info;
}

HostInfo PrplDataModel::parseHost(const std::map<std::string, std::string>& params) {
    HostInfo info;

    if (params.count("IPAddress"))
        info.ipAddress = params.at("IPAddress");
    if (params.count("PhysAddress"))
        info.macAddress = params.at("PhysAddress");
    if (params.count("HostName"))
        info.hostName = params.at("HostName");
    if (params.count("Active"))
        info.active = stringToBool(params.at("Active"));
    if (params.count("InterfaceType"))
        info.interfaceType = params.at("InterfaceType");
    if (params.count("AddressSource"))
        info.addressSource = params.at("AddressSource");

    return info;
}

} // namespace prpl
} // namespace platform
} // namespace lwm2m
