/*
 * UciConfig.cpp
 *
 * OpenWRT UCI integration implementation
 */

#include "UciConfig.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace wpp {

UciConfig::UciConfig(const std::string& configName)
    : m_ctx(nullptr)
    , m_package(nullptr)
    , m_configName(configName)
    , m_lastError("")
{
    initialize();
}

UciConfig::~UciConfig() {
    cleanup();
}

bool UciConfig::initialize() {
    m_ctx = uci_alloc_context();
    if (!m_ctx) {
        m_lastError = "Failed to allocate UCI context";
        return false;
    }
    return true;
}

void UciConfig::cleanup() {
    if (m_package) {
        uci_unload(m_ctx, m_package);
        m_package = nullptr;
    }
    if (m_ctx) {
        uci_free_context(m_ctx);
        m_ctx = nullptr;
    }
}

bool UciConfig::load() {
    if (!m_ctx) {
        m_lastError = "UCI context not initialized";
        return false;
    }

    return loadPackage();
}

bool UciConfig::loadPackage() {
    if (m_package) {
        uci_unload(m_ctx, m_package);
        m_package = nullptr;
    }

    int ret = uci_load(m_ctx, m_configName.c_str(), &m_package);
    if (ret != UCI_OK) {
        m_lastError = "Failed to load UCI package: " + m_configName;
        if (m_ctx->err) {
            m_lastError += " - ";
            char* errMsg = nullptr;
            uci_get_errorstr(m_ctx, &errMsg, nullptr);
            if (errMsg) {
                m_lastError += errMsg;
                free(errMsg);
            }
        }
        return false;
    }

    return true;
}

bool UciConfig::save() {
    if (!m_ctx || !m_package) {
        m_lastError = "UCI context or package not initialized";
        return false;
    }

    int ret = uci_commit(m_ctx, &m_package, false);
    if (ret != UCI_OK) {
        m_lastError = "Failed to commit UCI changes";
        return false;
    }

    return true;
}

bool UciConfig::reload() {
    return load();
}

struct uci_section* UciConfig::findSection(const std::string& sectionName) const {
    if (!m_package) {
        return nullptr;
    }

    struct uci_element* e;
    uci_foreach_element(&m_package->sections, e) {
        struct uci_section* s = uci_to_section(e);
        if (s && s->e.name && sectionName == s->e.name) {
            return s;
        }
    }

    return nullptr;
}

std::string UciConfig::getOptionValue(struct uci_section* section,
                                      const std::string& optionName) const {
    if (!section) {
        return "";
    }

    struct uci_element* e;
    uci_foreach_element(&section->options, e) {
        struct uci_option* o = uci_to_option(e);
        if (o && o->e.name && optionName == o->e.name) {
            if (o->type == UCI_TYPE_STRING && o->v.string) {
                return o->v.string;
            }
        }
    }

    return "";
}

std::string UciConfig::getString(const std::string& section,
                                const std::string& option,
                                const std::string& defaultValue) const {
    if (!m_package) {
        return defaultValue;
    }

    struct uci_section* s = findSection(section);
    if (!s) {
        return defaultValue;
    }

    std::string value = getOptionValue(s, option);
    return value.empty() ? defaultValue : value;
}

int UciConfig::getInt(const std::string& section,
                     const std::string& option,
                     int defaultValue) const {
    std::string value = getString(section, option, "");
    if (value.empty()) {
        return defaultValue;
    }

    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

bool UciConfig::getBool(const std::string& section,
                       const std::string& option,
                       bool defaultValue) const {
    std::string value = getString(section, option, "");
    if (value.empty()) {
        return defaultValue;
    }

    // UCI booleans can be: 1/0, true/false, yes/no, on/off, enabled/disabled
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return (value == "1" || value == "true" || value == "yes" ||
            value == "on" || value == "enabled");
}

bool UciConfig::setString(const std::string& section,
                         const std::string& option,
                         const std::string& value) {
    if (!m_ctx || !m_package) {
        m_lastError = "UCI context or package not initialized";
        return false;
    }

    struct uci_ptr ptr;
    std::memset(&ptr, 0, sizeof(ptr));

    std::string optionPath = m_configName + "." + section + "." + option;
    char* path = strdup(optionPath.c_str());

    int ret = uci_lookup_ptr(m_ctx, &ptr, path, true);
    free(path);

    if (ret != UCI_OK) {
        m_lastError = "Failed to lookup UCI option: " + optionPath;
        return false;
    }

    ptr.value = value.c_str();
    ret = uci_set(m_ctx, &ptr);

    if (ret != UCI_OK) {
        m_lastError = "Failed to set UCI option: " + optionPath;
        return false;
    }

    return true;
}

bool UciConfig::setInt(const std::string& section,
                      const std::string& option,
                      int value) {
    return setString(section, option, std::to_string(value));
}

bool UciConfig::setBool(const std::string& section,
                       const std::string& option,
                       bool value) {
    return setString(section, option, value ? "1" : "0");
}

bool UciConfig::exists() const {
    if (!m_ctx) {
        return false;
    }

    // Try to load the package to check existence
    struct uci_package* pkg = nullptr;
    int ret = uci_load(m_ctx, m_configName.c_str(), &pkg);
    if (pkg) {
        uci_unload(m_ctx, pkg);
    }

    return (ret == UCI_OK);
}

std::string UciConfig::getLastError() const {
    return m_lastError;
}

std::vector<UciConfig::ServerConfig> UciConfig::getServers() const {
    std::vector<ServerConfig> servers;

    if (!m_package) {
        return servers;
    }

    // Iterate through all sections looking for server type
    struct uci_element* e;
    uci_foreach_element(&m_package->sections, e) {
        struct uci_section* s = uci_to_section(e);
        if (s && s->type && std::string(s->type) == "server") {
            ServerConfig config;
            config.uri = getOptionValue(s, "uri");
            config.serverId = std::stoi(getOptionValue(s, "server_id"));
            config.lifetime = std::stoi(getOptionValue(s, "lifetime"));
            config.binding = getOptionValue(s, "binding");

            std::string bootstrap = getOptionValue(s, "bootstrap");
            config.bootstrapServer = (bootstrap == "1" || bootstrap == "true");

            std::string secInst = getOptionValue(s, "security_instance");
            config.securityInstance = secInst.empty() ? 0 : std::stoi(secInst);

            servers.push_back(config);
        }
    }

    return servers;
}

std::vector<UciConfig::SecurityConfig> UciConfig::getSecurityConfigs() const {
    std::vector<SecurityConfig> configs;

    if (!m_package) {
        return configs;
    }

    // Iterate through all sections looking for security type
    struct uci_element* e;
    uci_foreach_element(&m_package->sections, e) {
        struct uci_section* s = uci_to_section(e);
        if (s && s->type && std::string(s->type) == "security") {
            SecurityConfig config;

            std::string mode = getOptionValue(s, "mode");
            config.securityMode = mode.empty() ? 0 : std::stoi(mode);

            config.pskIdentity = getOptionValue(s, "psk_identity");
            config.pskKey = getOptionValue(s, "psk_key");
            config.publicKey = getOptionValue(s, "public_key");
            config.privateKey = getOptionValue(s, "private_key");
            config.serverPublicKey = getOptionValue(s, "server_public_key");

            configs.push_back(config);
        }
    }

    return configs;
}

UciConfig::DeviceConfig UciConfig::getDeviceConfig() const {
    DeviceConfig config;

    config.manufacturer = getString("device", "manufacturer", "Friendly Technologies");
    config.modelNumber = getString("device", "model", "LwM2M-Client-1.0");
    config.serialNumber = getString("device", "serial", "");
    config.firmwareVersion = getString("device", "firmware_version", "1.0.0");
    config.hardwareVersion = getString("device", "hardware_version", "1.0");
    config.deviceType = getString("device", "device_type", "Generic");

    return config;
}

UciConfig::NetworkConfig UciConfig::getNetworkConfig() const {
    NetworkConfig config;

    config.interface = getString("network", "interface", "wan");
    config.port = getInt("network", "port", 5683);
    config.enableIPv4 = getBool("network", "ipv4", true);
    config.enableIPv6 = getBool("network", "ipv6", false);

    return config;
}

UciConfig::GeneralConfig UciConfig::getGeneralConfig() const {
    GeneralConfig config;

    config.endpointName = getString("general", "endpoint_name", "lwm2m-client");
    config.logLevel = getInt("general", "log_level", 2); // Warning by default
    config.enableBootstrap = getBool("general", "enable_bootstrap", false);
    config.objectsConfig = getString("general", "objects_config", "");

    return config;
}

std::vector<uint8_t> UciConfig::hexToBytes(const std::string& hexStr) {
    std::vector<uint8_t> bytes;

    if (hexStr.empty() || hexStr.length() % 2 != 0) {
        return bytes;
    }

    for (size_t i = 0; i < hexStr.length(); i += 2) {
        std::string byteStr = hexStr.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}

std::string UciConfig::bytesToHex(const std::vector<uint8_t>& bytes) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    for (uint8_t byte : bytes) {
        oss << std::setw(2) << static_cast<int>(byte);
    }

    return oss.str();
}

} // namespace wpp
