/*
 * UbusInterface.h
 *
 * OpenWRT ubus integration for LwM2M client
 * Provides message bus interface for external control and monitoring
 */

#ifndef WPP_UBUS_INTERFACE_H
#define WPP_UBUS_INTERFACE_H

#include <string>
#include <functional>
#include <memory>
#include <map>

extern "C" {
#include <libubus.h>
}

namespace wpp {

// Forward declaration
class WppClient;

/**
 * @brief ubus Interface for LwM2M Client
 *
 * Provides a ubus interface allowing external applications to:
 * - Query client status
 * - Read/write object resources
 * - Trigger registration/update
 * - Monitor events
 * - Configure client behavior
 */
class UbusInterface {
public:
    /**
     * @brief Client status structure
     */
    struct ClientStatus {
        bool registered;           // Client registration status
        std::string serverUri;     // Current server URI
        std::string endpoint;      // Client endpoint name
        int lifetime;              // Registration lifetime
        uint64_t lastUpdate;       // Last update timestamp
        std::string state;         // Client state string
    };

    /**
     * @brief Resource value structure
     */
    struct ResourceValue {
        int objectId;
        int instanceId;
        int resourceId;
        std::string value;
        std::string type;          // "string", "int", "float", "bool", "opaque"
    };

    /**
     * @brief Constructor
     * @param client Pointer to WppClient instance
     * @param objectName ubus object name (default: "lwm2m")
     */
    explicit UbusInterface(WppClient* client = nullptr,
                          const std::string& objectName = "lwm2m");

    /**
     * @brief Destructor
     */
    ~UbusInterface();

    // Prevent copying
    UbusInterface(const UbusInterface&) = delete;
    UbusInterface& operator=(const UbusInterface&) = delete;

    /**
     * @brief Initialize ubus connection
     * @param socketPath ubus socket path (nullptr for default)
     * @return true if successful
     */
    bool init(const char* socketPath = nullptr);

    /**
     * @brief Set WppClient instance
     * @param client Pointer to WppClient
     */
    void setClient(WppClient* client);

    /**
     * @brief Register ubus object and methods
     * @return true if successful
     */
    bool registerObject();

    /**
     * @brief Unregister ubus object
     */
    void unregisterObject();

    /**
     * @brief Process ubus events (should be called in main loop)
     * @param timeout Timeout in milliseconds (-1 for blocking)
     */
    void process(int timeout = 0);

    /**
     * @brief Check if ubus is connected
     * @return true if connected
     */
    bool isConnected() const;

    /**
     * @brief Send ubus event
     * @param eventName Event name
     * @param data Event data (JSON format)
     * @return true if successful
     */
    bool sendEvent(const std::string& eventName, const std::string& data);

    /**
     * @brief Get last error message
     * @return Error message
     */
    std::string getLastError() const;

private:
    struct ubus_context* m_ctx;
    struct ubus_object m_ubusObj;
    WppClient* m_client;
    std::string m_objectName;
    mutable std::string m_lastError;
    uint32_t m_objId;

    // ubus method handlers (static wrappers)
    static int handleStatus(struct ubus_context* ctx,
                           struct ubus_object* obj,
                           struct ubus_request_data* req,
                           const char* method,
                           struct blob_attr* msg);

    static int handleResourceRead(struct ubus_context* ctx,
                                  struct ubus_object* obj,
                                  struct ubus_request_data* req,
                                  const char* method,
                                  struct blob_attr* msg);

    static int handleResourceWrite(struct ubus_context* ctx,
                                   struct ubus_object* obj,
                                   struct ubus_request_data* req,
                                   const char* method,
                                   struct blob_attr* msg);

    static int handleRegister(struct ubus_context* ctx,
                             struct ubus_object* obj,
                             struct ubus_request_data* req,
                             const char* method,
                             struct blob_attr* msg);

    static int handleUpdate(struct ubus_context* ctx,
                           struct ubus_object* obj,
                           struct ubus_request_data* req,
                           const char* method,
                           struct blob_attr* msg);

    static int handleDeregister(struct ubus_context* ctx,
                               struct ubus_object* obj,
                               struct ubus_request_data* req,
                               const char* method,
                               struct blob_attr* msg);

    static int handleListObjects(struct ubus_context* ctx,
                                struct ubus_object* obj,
                                struct ubus_request_data* req,
                                const char* method,
                                struct blob_attr* msg);

    static int handleReloadConfig(struct ubus_context* ctx,
                                 struct ubus_object* obj,
                                 struct ubus_request_data* req,
                                 const char* method,
                                 struct blob_attr* msg);

    // Instance method handlers
    int doStatus(struct ubus_request_data* req, struct blob_attr* msg);
    int doResourceRead(struct ubus_request_data* req, struct blob_attr* msg);
    int doResourceWrite(struct ubus_request_data* req, struct blob_attr* msg);
    int doRegister(struct ubus_request_data* req, struct blob_attr* msg);
    int doUpdate(struct ubus_request_data* req, struct blob_attr* msg);
    int doDeregister(struct ubus_request_data* req, struct blob_attr* msg);
    int doListObjects(struct ubus_request_data* req, struct blob_attr* msg);
    int doReloadConfig(struct ubus_request_data* req, struct blob_attr* msg);

    /**
     * @brief Get client status
     * @return Client status structure
     */
    ClientStatus getClientStatus() const;

    /**
     * @brief Read resource value
     * @param objectId Object ID
     * @param instanceId Instance ID
     * @param resourceId Resource ID
     * @return Resource value structure
     */
    ResourceValue readResource(int objectId, int instanceId, int resourceId) const;

    /**
     * @brief Write resource value
     * @param objectId Object ID
     * @param instanceId Instance ID
     * @param resourceId Resource ID
     * @param value Value to write
     * @return true if successful
     */
    bool writeResource(int objectId, int instanceId, int resourceId,
                      const std::string& value);

    /**
     * @brief Helper to get UbusInterface from ubus_object
     */
    static UbusInterface* getInstance(struct ubus_object* obj);
};

} // namespace wpp

#endif // WPP_UBUS_INTERFACE_H
