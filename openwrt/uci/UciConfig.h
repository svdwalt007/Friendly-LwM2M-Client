/*
 * UciConfig.h
 *
 * OpenWRT UCI (Unified Configuration Interface) integration for LwM2M client
 * Provides C++ wrapper around UCI library for reading/writing configuration
 */

#ifndef WPP_UCI_CONFIG_H
#define WPP_UCI_CONFIG_H

#include <string>
#include <vector>
#include <memory>
#include <map>

extern "C" {
#include <uci.h>
}

namespace wpp {

/**
 * @brief UCI Configuration Reader/Writer for OpenWRT
 *
 * Provides high-level C++ interface to OpenWRT's UCI configuration system.
 * Manages configuration for LwM2M client including server endpoints, security,
 * and device settings.
 */
class UciConfig {
public:
    /**
     * @brief Configuration section types
     */
    enum class SectionType {
        SERVER,      // LwM2M server configuration
        SECURITY,    // Security credentials
        DEVICE,      // Device information
        NETWORK,     // Network settings
        GENERAL      // General client settings
    };

    /**
     * @brief Server configuration structure
     */
    struct ServerConfig {
        std::string uri;           // Server URI (coap:// or coaps://)
        int serverId;              // Short Server ID
        int lifetime;              // Registration lifetime (seconds)
        std::string binding;       // Binding mode (U, UQ, S, SQ, US, UQS)
        bool bootstrapServer;      // Is this a bootstrap server?
        int securityInstance;      // Link to security object instance
    };

    /**
     * @brief Security configuration structure
     */
    struct SecurityConfig {
        int securityMode;          // 0=PSK, 1=RPK, 2=Certificate, 3=NoSec
        std::string pskIdentity;   // PSK Identity (hex string)
        std::string pskKey;        // PSK Secret Key (hex string)
        std::string publicKey;     // Client public key (hex string)
        std::string privateKey;    // Client private key (hex string)
        std::string serverPublicKey; // Server public key (hex string)
    };

    /**
     * @brief Device configuration structure
     */
    struct DeviceConfig {
        std::string manufacturer;
        std::string modelNumber;
        std::string serialNumber;
        std::string firmwareVersion;
        std::string hardwareVersion;
        std::string deviceType;
    };

    /**
     * @brief Network configuration structure
     */
    struct NetworkConfig {
        std::string interface;     // Network interface name (e.g., "wan", "eth0")
        int port;                  // Local CoAP port
        bool enableIPv4;           // Enable IPv4
        bool enableIPv6;           // Enable IPv6
    };

    /**
     * @brief General client configuration
     */
    struct GeneralConfig {
        std::string endpointName;  // Client endpoint name
        int logLevel;              // 0=None, 1=Error, 2=Warning, 3=Info, 4=Debug
        bool enableBootstrap;      // Enable bootstrap mode
        std::string objectsConfig; // Path to additional object configuration
    };

    /**
     * @brief Constructor
     * @param configName UCI configuration name (default: "lwm2m")
     */
    explicit UciConfig(const std::string& configName = "lwm2m");

    /**
     * @brief Destructor - cleanup UCI context
     */
    ~UciConfig();

    // Prevent copying
    UciConfig(const UciConfig&) = delete;
    UciConfig& operator=(const UciConfig&) = delete;

    /**
     * @brief Load configuration from UCI
     * @return true if successful, false otherwise
     */
    bool load();

    /**
     * @brief Save configuration to UCI
     * @return true if successful, false otherwise
     */
    bool save();

    /**
     * @brief Reload configuration from UCI
     * @return true if successful, false otherwise
     */
    bool reload();

    /**
     * @brief Get server configurations
     * @return Vector of server configurations
     */
    std::vector<ServerConfig> getServers() const;

    /**
     * @brief Get security configurations
     * @return Vector of security configurations
     */
    std::vector<SecurityConfig> getSecurityConfigs() const;

    /**
     * @brief Get device configuration
     * @return Device configuration structure
     */
    DeviceConfig getDeviceConfig() const;

    /**
     * @brief Get network configuration
     * @return Network configuration structure
     */
    NetworkConfig getNetworkConfig() const;

    /**
     * @brief Get general configuration
     * @return General configuration structure
     */
    GeneralConfig getGeneralConfig() const;

    /**
     * @brief Read string option from UCI
     * @param section Section name
     * @param option Option name
     * @param defaultValue Default value if not found
     * @return Option value or default
     */
    std::string getString(const std::string& section, const std::string& option,
                         const std::string& defaultValue = "") const;

    /**
     * @brief Read integer option from UCI
     * @param section Section name
     * @param option Option name
     * @param defaultValue Default value if not found
     * @return Option value or default
     */
    int getInt(const std::string& section, const std::string& option,
              int defaultValue = 0) const;

    /**
     * @brief Read boolean option from UCI
     * @param section Section name
     * @param option Option name
     * @param defaultValue Default value if not found
     * @return Option value or default
     */
    bool getBool(const std::string& section, const std::string& option,
                bool defaultValue = false) const;

    /**
     * @brief Write string option to UCI
     * @param section Section name
     * @param option Option name
     * @param value Value to write
     * @return true if successful
     */
    bool setString(const std::string& section, const std::string& option,
                  const std::string& value);

    /**
     * @brief Write integer option to UCI
     * @param section Section name
     * @param option Option name
     * @param value Value to write
     * @return true if successful
     */
    bool setInt(const std::string& section, const std::string& option, int value);

    /**
     * @brief Write boolean option to UCI
     * @param section Section name
     * @param option Option name
     * @param value Value to write
     * @return true if successful
     */
    bool setBool(const std::string& section, const std::string& option, bool value);

    /**
     * @brief Check if UCI configuration exists
     * @return true if configuration file exists
     */
    bool exists() const;

    /**
     * @brief Get last error message
     * @return Error message string
     */
    std::string getLastError() const;

private:
    struct uci_context* m_ctx;
    struct uci_package* m_package;
    std::string m_configName;
    mutable std::string m_lastError;

    /**
     * @brief Initialize UCI context
     * @return true if successful
     */
    bool initialize();

    /**
     * @brief Cleanup UCI resources
     */
    void cleanup();

    /**
     * @brief Load UCI package
     * @return true if successful
     */
    bool loadPackage();

    /**
     * @brief Find UCI section by name
     * @param sectionName Section name to find
     * @return Pointer to section or nullptr
     */
    struct uci_section* findSection(const std::string& sectionName) const;

    /**
     * @brief Get option value from section
     * @param section Section pointer
     * @param optionName Option name
     * @return Option value or empty string
     */
    std::string getOptionValue(struct uci_section* section,
                               const std::string& optionName) const;

    /**
     * @brief Parse hex string to byte vector
     * @param hexStr Hex string (e.g., "0A1B2C")
     * @return Byte vector
     */
    static std::vector<uint8_t> hexToBytes(const std::string& hexStr);

    /**
     * @brief Convert byte vector to hex string
     * @param bytes Byte vector
     * @return Hex string
     */
    static std::string bytesToHex(const std::vector<uint8_t>& bytes);
};

} // namespace wpp

#endif // WPP_UCI_CONFIG_H
