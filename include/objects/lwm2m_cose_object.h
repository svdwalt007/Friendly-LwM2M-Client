/**
 * @file lwm2m_cose_object.h
 * @brief OMA LwM2M COSE Object (ID: 23)
 *
 * Defines the LwM2M COSE (CBOR Object Signing and Encryption) security
 * configuration object for MQTT transport binding.
 *
 * Object ID: 23
 * URN: urn:oma:lwm2m:oma:23
 * Multiple Instances: Yes
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_COSE_OBJECT_H
#define FRIENDLY_LWM2M_COSE_OBJECT_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace lwm2m {
namespace objects {

/**
 * @brief LwM2M COSE Object ID
 */
constexpr uint16_t LWM2M_COSE_OBJECT_ID = 23;

/**
 * @brief Resource IDs for LwM2M COSE Object (ID: 23)
 */
enum class CoseResourceId : uint16_t {
    SHORT_SERVER_ID = 0,            ///< Short Server ID (Integer, R, Mandatory)
    SECURITY_MODE = 1,              ///< Security Mode (Integer, RW, Mandatory)
    PUBLIC_KEY_OR_IDENTITY = 2,     ///< Public Key or PSK Identity (Opaque, RW, Optional)
    SECRET_KEY = 3,                 ///< Secret Key or PSK (Opaque, W, Optional)
    SERVER_PUBLIC_KEY = 4,          ///< Server's Public Key (Opaque, RW, Optional)
    CERTIFICATE_CHAIN = 5,          ///< Client Certificate Chain Path (String, RW, Optional)
    CA_CERTIFICATE = 6,             ///< CA Certificate Path (String, RW, Optional)
    SNI = 7,                        ///< Server Name Indication (String, RW, Optional)
    VERIFY_PEER = 8,                ///< Verify Server Certificate (Boolean, RW, Optional)
    ALPN = 9,                       ///< Application Layer Protocol Negotiation (String, RW, Optional)
    TOKEN = 10,                     ///< OAuth/JWT Token (String, W, Optional)
    TOKEN_ENDPOINT = 11             ///< Token Refresh Endpoint (String, RW, Optional)
};

/**
 * @brief COSE Security modes
 */
enum class CoseSecurityMode : uint8_t {
    NONE = 0,           ///< No security
    PSK = 1,            ///< Pre-Shared Key
    RPK = 2,            ///< Raw Public Key
    CERTIFICATE = 3,    ///< X.509 Certificate
    TOKEN = 4           ///< OAuth/JWT Token
};

/**
 * @brief COSE Security configuration
 */
struct CoseConfig {
    uint16_t shortServerId = 0;
    CoseSecurityMode securityMode = CoseSecurityMode::NONE;
    std::vector<uint8_t> publicKeyOrIdentity;
    std::vector<uint8_t> secretKey;
    std::vector<uint8_t> serverPublicKey;
    std::string certificateChain;
    std::string caCertificate;
    std::string sni;
    bool verifyPeer = true;
    std::string alpn;
    std::string token;
    std::string tokenEndpoint;
};

/**
 * @brief Callback for security configuration changes
 */
using CoseConfigChangeCallback = std::function<void(uint16_t instanceId,
                                                     CoseResourceId resourceId)>;

/**
 * @brief LwM2M COSE Object Implementation
 *
 * Manages COSE security configurations for LwM2M MQTT transport.
 */
class Lwm2mCoseObject {
public:
    /**
     * @brief Constructor
     * @param instanceId Object instance ID
     */
    explicit Lwm2mCoseObject(uint16_t instanceId = 0);

    /**
     * @brief Destructor
     */
    ~Lwm2mCoseObject();

    // Prevent copying
    Lwm2mCoseObject(const Lwm2mCoseObject&) = delete;
    Lwm2mCoseObject& operator=(const Lwm2mCoseObject&) = delete;

    /**
     * @brief Initialize the object
     * @return true on success
     */
    bool initialize();

    // ========================================================================
    // Resource Read Operations
    // ========================================================================

    /**
     * @brief Read string resource
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(CoseResourceId resourceId, std::string& value) const;

    /**
     * @brief Read integer resource
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(CoseResourceId resourceId, int64_t& value) const;

    /**
     * @brief Read boolean resource
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(CoseResourceId resourceId, bool& value) const;

    /**
     * @brief Read opaque resource
     * @param resourceId Resource ID
     * @param value Output value
     * @return true on success
     */
    bool read(CoseResourceId resourceId, std::vector<uint8_t>& value) const;

    // ========================================================================
    // Resource Write Operations
    // ========================================================================

    /**
     * @brief Write string resource
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(CoseResourceId resourceId, const std::string& value);

    /**
     * @brief Write integer resource
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(CoseResourceId resourceId, int64_t value);

    /**
     * @brief Write boolean resource
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(CoseResourceId resourceId, bool value);

    /**
     * @brief Write opaque resource
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true on success
     */
    bool write(CoseResourceId resourceId, const std::vector<uint8_t>& value);

    // ========================================================================
    // Configuration Access
    // ========================================================================

    /**
     * @brief Get full configuration
     * @return COSE configuration
     */
    CoseConfig getConfig() const;

    /**
     * @brief Set full configuration
     * @param config Configuration to set
     */
    void setConfig(const CoseConfig& config);

    /**
     * @brief Get Short Server ID
     * @return Server ID
     */
    uint16_t getShortServerId() const { return config_.shortServerId; }

    /**
     * @brief Get Security Mode
     * @return Security mode
     */
    CoseSecurityMode getSecurityMode() const { return config_.securityMode; }

    /**
     * @brief Get PSK Identity
     * @return Identity bytes
     */
    std::vector<uint8_t> getPskIdentity() const { return config_.publicKeyOrIdentity; }

    /**
     * @brief Get Server Public Key
     * @return Public key bytes
     */
    std::vector<uint8_t> getServerPublicKey() const { return config_.serverPublicKey; }

    /**
     * @brief Check if peer verification is enabled
     * @return true if enabled
     */
    bool isVerifyPeerEnabled() const { return config_.verifyPeer; }

    // ========================================================================
    // Security Operations
    // ========================================================================

    /**
     * @brief Set PSK credentials
     * @param identity PSK identity
     * @param key PSK key
     */
    void setPskCredentials(const std::vector<uint8_t>& identity,
                           const std::vector<uint8_t>& key);

    /**
     * @brief Set certificate credentials
     * @param certChainPath Path to certificate chain
     * @param privateKeyPath Path to private key (stored separately)
     * @param caCertPath Path to CA certificate
     */
    void setCertificateCredentials(const std::string& certChainPath,
                                    const std::string& caCertPath);

    /**
     * @brief Set OAuth token
     * @param token OAuth/JWT token
     * @param tokenEndpoint Token refresh endpoint
     */
    void setTokenCredentials(const std::string& token,
                              const std::string& tokenEndpoint = "");

    /**
     * @brief Clear all credentials
     */
    void clearCredentials();

    // ========================================================================
    // Callbacks
    // ========================================================================

    /**
     * @brief Set configuration change callback
     * @param callback Callback function
     */
    void setConfigChangeCallback(CoseConfigChangeCallback callback);

    // ========================================================================
    // Object Information
    // ========================================================================

    /**
     * @brief Get object ID
     * @return Object ID (23)
     */
    static uint16_t getObjectId() { return LWM2M_COSE_OBJECT_ID; }

    /**
     * @brief Get instance ID
     * @return Instance ID
     */
    uint16_t getInstanceId() const { return instanceId_; }

    /**
     * @brief Check if resource is readable
     * @param resourceId Resource ID
     * @return true if readable
     */
    static bool isReadable(CoseResourceId resourceId);

    /**
     * @brief Check if resource is writable
     * @param resourceId Resource ID
     * @return true if writable
     */
    static bool isWritable(CoseResourceId resourceId);

    /**
     * @brief Check if resource is mandatory
     * @param resourceId Resource ID
     * @return true if mandatory
     */
    static bool isMandatory(CoseResourceId resourceId);

private:
    uint16_t instanceId_;
    CoseConfig config_;
    CoseConfigChangeCallback configChangeCallback_;

    void notifyChange(CoseResourceId resourceId);
};

/**
 * @brief Factory for creating LwM2M COSE Object instances
 */
class Lwm2mCoseObjectFactory {
public:
    /**
     * @brief Create new instance
     * @param instanceId Instance ID
     * @return Object instance
     */
    static std::unique_ptr<Lwm2mCoseObject> create(uint16_t instanceId = 0);

    /**
     * @brief Create from configuration
     * @param config Configuration
     * @param instanceId Instance ID
     * @return Object instance
     */
    static std::unique_ptr<Lwm2mCoseObject> create(const CoseConfig& config,
                                                    uint16_t instanceId = 0);

    /**
     * @brief Create with PSK credentials
     * @param shortServerId Server ID
     * @param identity PSK identity
     * @param key PSK key
     * @param instanceId Instance ID
     * @return Object instance
     */
    static std::unique_ptr<Lwm2mCoseObject> createWithPsk(uint16_t shortServerId,
                                                           const std::vector<uint8_t>& identity,
                                                           const std::vector<uint8_t>& key,
                                                           uint16_t instanceId = 0);
};

} // namespace objects
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_COSE_OBJECT_H
