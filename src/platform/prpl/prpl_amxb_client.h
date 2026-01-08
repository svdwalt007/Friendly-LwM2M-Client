/**
 * @file prpl_amxb_client.h
 * @brief Ambiorix Bus Client for prplOS Integration
 *
 * Provides abstraction over Ambiorix (amxb/amxd) bus operations including:
 * - Bus connection management (ubus, pcb, etc.)
 * - Object registration and discovery
 * - Method invocation (synchronous and asynchronous)
 * - Event subscription and handling
 * - Data model queries and updates
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_PRPL_AMXB_CLIENT_H
#define FRIENDLY_LWM2M_PRPL_AMXB_CLIENT_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>

// Forward declarations for Ambiorix types
struct _amxb_bus_ctx_t;
typedef struct _amxb_bus_ctx_t amxb_bus_ctx_t;

struct _amxc_var_t;
typedef struct _amxc_var_t amxc_var_t;

struct _amxd_object_t;
typedef struct _amxd_object_t amxd_object_t;

namespace lwm2m {
namespace platform {
namespace prpl {

/**
 * @brief Ambiorix variant value wrapper
 *
 * Simplified wrapper around amxc_var_t for type-safe access
 */
class AmxVariant {
public:
    enum class Type {
        INVALID,
        STRING,
        INTEGER,
        BOOLEAN,
        DOUBLE,
        LIST,
        OBJECT
    };

    AmxVariant();
    explicit AmxVariant(const std::string& value);
    explicit AmxVariant(int64_t value);
    explicit AmxVariant(bool value);
    explicit AmxVariant(double value);
    ~AmxVariant();

    // Copy and move operations
    AmxVariant(const AmxVariant& other);
    AmxVariant& operator=(const AmxVariant& other);
    AmxVariant(AmxVariant&& other) noexcept;
    AmxVariant& operator=(AmxVariant&& other) noexcept;

    Type getType() const;
    bool isValid() const;

    std::string asString() const;
    int64_t asInteger() const;
    bool asBoolean() const;
    double asDouble() const;

    // Internal Ambiorix variant access
    amxc_var_t* getRaw();
    const amxc_var_t* getRaw() const;

private:
    amxc_var_t* var_;
    void cleanup();
};

/**
 * @brief Event subscription callback
 */
using EventCallback = std::function<void(const std::string& event,
                                         const std::string& object,
                                         const std::map<std::string, std::string>& data)>;

/**
 * @brief Ambiorix bus client
 *
 * Manages connection to Ambiorix data model bus and provides
 * high-level operations for TR-181 and custom data model access.
 */
class PrplAmxbClient {
public:
    /**
     * @brief Constructor
     */
    PrplAmxbClient();

    /**
     * @brief Destructor
     */
    ~PrplAmxbClient();

    // Prevent copying
    PrplAmxbClient(const PrplAmxbClient&) = delete;
    PrplAmxbClient& operator=(const PrplAmxbClient&) = delete;

    // ========================================================================
    // Connection Management
    // ========================================================================

    /**
     * @brief Connect to Ambiorix bus
     * @param uri Bus URI (e.g., "ubus:/var/run/ubus/ubus.sock")
     * @return true on success
     */
    bool connect(const std::string& uri);

    /**
     * @brief Disconnect from bus
     */
    void disconnect();

    /**
     * @brief Check if connected to bus
     * @return true if connected
     */
    bool isConnected() const;

    /**
     * @brief Get connected bus type
     * @return Bus type string (ubus, pcb, etc.)
     */
    std::string getBusType() const;

    // ========================================================================
    // Object Operations
    // ========================================================================

    /**
     * @brief List objects matching a search path
     * @param path Search path (supports wildcards)
     * @return Vector of object paths
     */
    std::vector<std::string> list(const std::string& path);

    /**
     * @brief Get object parameters
     * @param path Object path
     * @return Map of parameter name to value
     */
    std::map<std::string, std::string> get(const std::string& path);

    /**
     * @brief Get specific parameter value
     * @param path Object path
     * @param parameter Parameter name
     * @return Parameter value or empty string
     */
    std::string getValue(const std::string& path, const std::string& parameter);

    /**
     * @brief Set object parameters
     * @param path Object path
     * @param parameters Map of parameter name to value
     * @return true on success
     */
    bool set(const std::string& path, const std::map<std::string, std::string>& parameters);

    /**
     * @brief Set single parameter value
     * @param path Object path
     * @param parameter Parameter name
     * @param value Parameter value
     * @return true on success
     */
    bool setValue(const std::string& path, const std::string& parameter,
                  const std::string& value);

    /**
     * @brief Add object instance
     * @param path Object path
     * @param index Instance index (0 for auto-assign)
     * @param parameters Initial parameters
     * @return Instance path or empty string on error
     */
    std::string add(const std::string& path, uint32_t index = 0,
                    const std::map<std::string, std::string>& parameters = {});

    /**
     * @brief Delete object instance
     * @param path Object instance path
     * @return true on success
     */
    bool del(const std::string& path);

    // ========================================================================
    // Method Invocation
    // ========================================================================

    /**
     * @brief Invoke object method
     * @param path Object path
     * @param method Method name
     * @param arguments Method arguments
     * @param result Method result
     * @return true on success
     */
    bool invoke(const std::string& path, const std::string& method,
                const std::map<std::string, std::string>& arguments,
                std::map<std::string, std::string>& result);

    /**
     * @brief Invoke method without result
     * @param path Object path
     * @param method Method name
     * @param arguments Method arguments
     * @return true on success
     */
    bool invoke(const std::string& path, const std::string& method,
                const std::map<std::string, std::string>& arguments = {});

    // ========================================================================
    // Event Subscription
    // ========================================================================

    /**
     * @brief Subscribe to object events
     * @param path Object path (supports wildcards)
     * @param event Event name (or "*" for all events)
     * @param callback Event callback function
     * @return Subscription ID or 0 on error
     */
    uint32_t subscribe(const std::string& path, const std::string& event,
                       EventCallback callback);

    /**
     * @brief Unsubscribe from events
     * @param subscriptionId Subscription ID from subscribe()
     * @return true on success
     */
    bool unsubscribe(uint32_t subscriptionId);

    /**
     * @brief Process pending events
     * @param timeoutMs Timeout in milliseconds (0 for non-blocking)
     * @return Number of events processed
     */
    int processEvents(int timeoutMs = 0);

    // ========================================================================
    // Advanced Operations
    // ========================================================================

    /**
     * @brief Resolve object path wildcards
     * @param path Path with wildcards (e.g., "Device.WiFi.Radio.*.Enable")
     * @return Vector of resolved paths
     */
    std::vector<std::string> resolve(const std::string& path);

    /**
     * @brief Get object description
     * @param path Object path
     * @return Object description including parameters and methods
     */
    std::map<std::string, std::string> describe(const std::string& path);

    /**
     * @brief Check if object exists
     * @param path Object path
     * @return true if object exists
     */
    bool exists(const std::string& path);

    /**
     * @brief Get object instance count
     * @param path Object path
     * @return Number of instances
     */
    uint32_t getInstanceCount(const std::string& path);

    // ========================================================================
    // Batch Operations
    // ========================================================================

    /**
     * @brief Start batch transaction
     * @return true on success
     */
    bool beginTransaction();

    /**
     * @brief Commit batch transaction
     * @return true on success
     */
    bool commitTransaction();

    /**
     * @brief Abort batch transaction
     */
    void abortTransaction();

    /**
     * @brief Check if in transaction mode
     * @return true if in transaction
     */
    bool isInTransaction() const;

    // ========================================================================
    // Utility
    // ========================================================================

    /**
     * @brief Get last error message
     * @return Error message string
     */
    std::string getLastError() const;

    /**
     * @brief Enable/disable debug logging
     * @param enable Enable flag
     */
    void setDebugMode(bool enable);

private:
    amxb_bus_ctx_t* busContext_;
    std::string busUri_;
    std::string busType_;
    bool connected_;
    bool inTransaction_;
    std::string lastError_;
    bool debugMode_;
    mutable std::mutex mutex_;

    // Event subscription management
    struct Subscription {
        uint32_t id;
        std::string path;
        std::string event;
        EventCallback callback;
    };
    std::map<uint32_t, Subscription> subscriptions_;
    uint32_t nextSubscriptionId_;

    // Internal helpers
    bool initializeBus();
    void cleanupBus();
    void setError(const std::string& error);
    void logDebug(const std::string& message);

    // Ambiorix conversion helpers
    std::map<std::string, std::string> variantToMap(const amxc_var_t* var);
    amxc_var_t* mapToVariant(const std::map<std::string, std::string>& map);
    std::string variantToString(const amxc_var_t* var);
    void freeVariant(amxc_var_t* var);
};

} // namespace prpl
} // namespace platform
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_PRPL_AMXB_CLIENT_H
