/**
 * @file lwm2m_cose_object.cpp
 * @brief OMA LwM2M COSE Object (ID: 23) Implementation
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "objects/lwm2m_cose_object.h"

namespace lwm2m {
namespace objects {

// ============================================================================
// Constructor / Destructor
// ============================================================================

Lwm2mCoseObject::Lwm2mCoseObject(uint16_t instanceId)
    : instanceId_(instanceId) {
}

Lwm2mCoseObject::~Lwm2mCoseObject() {
    // Securely clear sensitive data
    clearCredentials();
}

// ============================================================================
// Initialization
// ============================================================================

bool Lwm2mCoseObject::initialize() {
    // Set default values
    config_.shortServerId = instanceId_;
    config_.securityMode = CoseSecurityMode::NONE;
    config_.verifyPeer = true;

    return true;
}

// ============================================================================
// Resource Read Operations
// ============================================================================

bool Lwm2mCoseObject::read(CoseResourceId resourceId, std::string& value) const {
    switch (resourceId) {
        case CoseResourceId::CERTIFICATE_CHAIN:
            value = config_.certificateChain;
            return true;
        case CoseResourceId::CA_CERTIFICATE:
            value = config_.caCertificate;
            return true;
        case CoseResourceId::SNI:
            value = config_.sni;
            return true;
        case CoseResourceId::ALPN:
            value = config_.alpn;
            return true;
        case CoseResourceId::TOKEN_ENDPOINT:
            value = config_.tokenEndpoint;
            return true;
        default:
            return false;
    }
}

bool Lwm2mCoseObject::read(CoseResourceId resourceId, int64_t& value) const {
    switch (resourceId) {
        case CoseResourceId::SHORT_SERVER_ID:
            value = config_.shortServerId;
            return true;
        case CoseResourceId::SECURITY_MODE:
            value = static_cast<int64_t>(config_.securityMode);
            return true;
        default:
            return false;
    }
}

bool Lwm2mCoseObject::read(CoseResourceId resourceId, bool& value) const {
    switch (resourceId) {
        case CoseResourceId::VERIFY_PEER:
            value = config_.verifyPeer;
            return true;
        default:
            return false;
    }
}

bool Lwm2mCoseObject::read(CoseResourceId resourceId, std::vector<uint8_t>& value) const {
    switch (resourceId) {
        case CoseResourceId::PUBLIC_KEY_OR_IDENTITY:
            value = config_.publicKeyOrIdentity;
            return true;
        case CoseResourceId::SERVER_PUBLIC_KEY:
            value = config_.serverPublicKey;
            return true;
        default:
            return false;
    }
}

// ============================================================================
// Resource Write Operations
// ============================================================================

bool Lwm2mCoseObject::write(CoseResourceId resourceId, const std::string& value) {
    if (!isWritable(resourceId)) {
        return false;
    }

    switch (resourceId) {
        case CoseResourceId::CERTIFICATE_CHAIN:
            config_.certificateChain = value;
            break;
        case CoseResourceId::CA_CERTIFICATE:
            config_.caCertificate = value;
            break;
        case CoseResourceId::SNI:
            config_.sni = value;
            break;
        case CoseResourceId::ALPN:
            config_.alpn = value;
            break;
        case CoseResourceId::TOKEN:
            config_.token = value;
            break;
        case CoseResourceId::TOKEN_ENDPOINT:
            config_.tokenEndpoint = value;
            break;
        default:
            return false;
    }

    notifyChange(resourceId);
    return true;
}

bool Lwm2mCoseObject::write(CoseResourceId resourceId, int64_t value) {
    if (!isWritable(resourceId)) {
        return false;
    }

    switch (resourceId) {
        case CoseResourceId::SECURITY_MODE:
            if (value < 0 || value > 4) return false;
            config_.securityMode = static_cast<CoseSecurityMode>(value);
            break;
        default:
            return false;
    }

    notifyChange(resourceId);
    return true;
}

bool Lwm2mCoseObject::write(CoseResourceId resourceId, bool value) {
    if (!isWritable(resourceId)) {
        return false;
    }

    switch (resourceId) {
        case CoseResourceId::VERIFY_PEER:
            config_.verifyPeer = value;
            break;
        default:
            return false;
    }

    notifyChange(resourceId);
    return true;
}

bool Lwm2mCoseObject::write(CoseResourceId resourceId, const std::vector<uint8_t>& value) {
    if (!isWritable(resourceId)) {
        return false;
    }

    switch (resourceId) {
        case CoseResourceId::PUBLIC_KEY_OR_IDENTITY:
            config_.publicKeyOrIdentity = value;
            break;
        case CoseResourceId::SECRET_KEY:
            config_.secretKey = value;
            break;
        case CoseResourceId::SERVER_PUBLIC_KEY:
            config_.serverPublicKey = value;
            break;
        default:
            return false;
    }

    notifyChange(resourceId);
    return true;
}

// ============================================================================
// Configuration Access
// ============================================================================

CoseConfig Lwm2mCoseObject::getConfig() const {
    return config_;
}

void Lwm2mCoseObject::setConfig(const CoseConfig& config) {
    config_ = config;
}

// ============================================================================
// Security Operations
// ============================================================================

void Lwm2mCoseObject::setPskCredentials(const std::vector<uint8_t>& identity,
                                          const std::vector<uint8_t>& key) {
    config_.securityMode = CoseSecurityMode::PSK;
    config_.publicKeyOrIdentity = identity;
    config_.secretKey = key;
    notifyChange(CoseResourceId::SECURITY_MODE);
}

void Lwm2mCoseObject::setCertificateCredentials(const std::string& certChainPath,
                                                  const std::string& caCertPath) {
    config_.securityMode = CoseSecurityMode::CERTIFICATE;
    config_.certificateChain = certChainPath;
    config_.caCertificate = caCertPath;
    notifyChange(CoseResourceId::SECURITY_MODE);
}

void Lwm2mCoseObject::setTokenCredentials(const std::string& token,
                                            const std::string& tokenEndpoint) {
    config_.securityMode = CoseSecurityMode::TOKEN;
    config_.token = token;
    config_.tokenEndpoint = tokenEndpoint;
    notifyChange(CoseResourceId::SECURITY_MODE);
}

void Lwm2mCoseObject::clearCredentials() {
    // Securely clear sensitive data
    std::fill(config_.secretKey.begin(), config_.secretKey.end(), 0);
    config_.secretKey.clear();

    std::fill(config_.publicKeyOrIdentity.begin(), config_.publicKeyOrIdentity.end(), 0);
    config_.publicKeyOrIdentity.clear();

    std::fill(config_.serverPublicKey.begin(), config_.serverPublicKey.end(), 0);
    config_.serverPublicKey.clear();

    // Clear token (fill with zeros first)
    std::fill(config_.token.begin(), config_.token.end(), '\0');
    config_.token.clear();

    config_.securityMode = CoseSecurityMode::NONE;
}

// ============================================================================
// Callbacks
// ============================================================================

void Lwm2mCoseObject::setConfigChangeCallback(CoseConfigChangeCallback callback) {
    configChangeCallback_ = std::move(callback);
}

void Lwm2mCoseObject::notifyChange(CoseResourceId resourceId) {
    if (configChangeCallback_) {
        configChangeCallback_(instanceId_, resourceId);
    }
}

// ============================================================================
// Resource Metadata
// ============================================================================

bool Lwm2mCoseObject::isReadable(CoseResourceId resourceId) {
    // SECRET_KEY and TOKEN are write-only
    return resourceId != CoseResourceId::SECRET_KEY &&
           resourceId != CoseResourceId::TOKEN;
}

bool Lwm2mCoseObject::isWritable(CoseResourceId resourceId) {
    // SHORT_SERVER_ID is read-only
    return resourceId != CoseResourceId::SHORT_SERVER_ID;
}

bool Lwm2mCoseObject::isMandatory(CoseResourceId resourceId) {
    switch (resourceId) {
        case CoseResourceId::SHORT_SERVER_ID:
        case CoseResourceId::SECURITY_MODE:
            return true;
        default:
            return false;
    }
}

// ============================================================================
// Factory
// ============================================================================

std::unique_ptr<Lwm2mCoseObject> Lwm2mCoseObjectFactory::create(uint16_t instanceId) {
    auto obj = std::make_unique<Lwm2mCoseObject>(instanceId);
    obj->initialize();
    return obj;
}

std::unique_ptr<Lwm2mCoseObject> Lwm2mCoseObjectFactory::create(const CoseConfig& config,
                                                                  uint16_t instanceId) {
    auto obj = std::make_unique<Lwm2mCoseObject>(instanceId);
    obj->initialize();
    obj->setConfig(config);
    return obj;
}

std::unique_ptr<Lwm2mCoseObject> Lwm2mCoseObjectFactory::createWithPsk(uint16_t shortServerId,
                                                                        const std::vector<uint8_t>& identity,
                                                                        const std::vector<uint8_t>& key,
                                                                        uint16_t instanceId) {
    auto obj = std::make_unique<Lwm2mCoseObject>(instanceId);
    obj->initialize();

    CoseConfig config;
    config.shortServerId = shortServerId;
    config.securityMode = CoseSecurityMode::PSK;
    config.publicKeyOrIdentity = identity;
    config.secretKey = key;
    obj->setConfig(config);

    return obj;
}

} // namespace objects
} // namespace lwm2m
