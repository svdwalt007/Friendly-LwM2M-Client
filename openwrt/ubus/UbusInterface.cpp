/*
 * UbusInterface.cpp
 *
 * OpenWRT ubus integration implementation
 */

#include "UbusInterface.h"
#include <cstring>
#include <sstream>

extern "C" {
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
}

namespace wpp {

// Policy for status method (no arguments)
enum {
    STATUS_MAX
};

// Policy for resource_read method
enum {
    READ_OBJECT_ID,
    READ_INSTANCE_ID,
    READ_RESOURCE_ID,
    READ_MAX
};

static const struct blobmsg_policy read_policy[READ_MAX] = {
    [READ_OBJECT_ID] = { .name = "object_id", .type = BLOBMSG_TYPE_INT32 },
    [READ_INSTANCE_ID] = { .name = "instance_id", .type = BLOBMSG_TYPE_INT32 },
    [READ_RESOURCE_ID] = { .name = "resource_id", .type = BLOBMSG_TYPE_INT32 },
};

// Policy for resource_write method
enum {
    WRITE_OBJECT_ID,
    WRITE_INSTANCE_ID,
    WRITE_RESOURCE_ID,
    WRITE_VALUE,
    WRITE_MAX
};

static const struct blobmsg_policy write_policy[WRITE_MAX] = {
    [WRITE_OBJECT_ID] = { .name = "object_id", .type = BLOBMSG_TYPE_INT32 },
    [WRITE_INSTANCE_ID] = { .name = "instance_id", .type = BLOBMSG_TYPE_INT32 },
    [WRITE_RESOURCE_ID] = { .name = "resource_id", .type = BLOBMSG_TYPE_INT32 },
    [WRITE_VALUE] = { .name = "value", .type = BLOBMSG_TYPE_STRING },
};

// ubus method definitions
static const struct ubus_method lwm2m_methods[] = {
    UBUS_METHOD_NOARG("status", UbusInterface::handleStatus),
    UBUS_METHOD("resource_read", UbusInterface::handleResourceRead, read_policy),
    UBUS_METHOD("resource_write", UbusInterface::handleResourceWrite, write_policy),
    UBUS_METHOD_NOARG("register", UbusInterface::handleRegister),
    UBUS_METHOD_NOARG("update", UbusInterface::handleUpdate),
    UBUS_METHOD_NOARG("deregister", UbusInterface::handleDeregister),
    UBUS_METHOD_NOARG("list_objects", UbusInterface::handleListObjects),
    UBUS_METHOD_NOARG("reload_config", UbusInterface::handleReloadConfig),
};

UbusInterface::UbusInterface(WppClient* client, const std::string& objectName)
    : m_ctx(nullptr)
    , m_client(client)
    , m_objectName(objectName)
    , m_lastError("")
    , m_objId(0)
{
    std::memset(&m_ubusObj, 0, sizeof(m_ubusObj));
}

UbusInterface::~UbusInterface() {
    unregisterObject();
    if (m_ctx) {
        ubus_free(m_ctx);
        m_ctx = nullptr;
    }
}

bool UbusInterface::init(const char* socketPath) {
    m_ctx = ubus_connect(socketPath);
    if (!m_ctx) {
        m_lastError = "Failed to connect to ubus";
        return false;
    }

    return true;
}

void UbusInterface::setClient(WppClient* client) {
    m_client = client;
}

bool UbusInterface::registerObject() {
    if (!m_ctx) {
        m_lastError = "ubus context not initialized";
        return false;
    }

    // Setup ubus object
    static struct ubus_object_type lwm2m_object_type =
        UBUS_OBJECT_TYPE("lwm2m", lwm2m_methods);

    m_ubusObj.name = m_objectName.c_str();
    m_ubusObj.type = &lwm2m_object_type;
    m_ubusObj.methods = lwm2m_methods;
    m_ubusObj.n_methods = ARRAY_SIZE(lwm2m_methods);

    int ret = ubus_add_object(m_ctx, &m_ubusObj);
    if (ret) {
        m_lastError = "Failed to add ubus object: " + std::string(ubus_strerror(ret));
        return false;
    }

    m_objId = m_ubusObj.id;
    return true;
}

void UbusInterface::unregisterObject() {
    if (m_ctx && m_objId != 0) {
        ubus_remove_object(m_ctx, &m_ubusObj);
        m_objId = 0;
    }
}

void UbusInterface::process(int timeout) {
    if (m_ctx) {
        ubus_handle_event(m_ctx);
        if (timeout != 0) {
            uloop_run_timeout(timeout);
        }
    }
}

bool UbusInterface::isConnected() const {
    return m_ctx != nullptr;
}

bool UbusInterface::sendEvent(const std::string& eventName, const std::string& data) {
    if (!m_ctx) {
        m_lastError = "ubus context not initialized";
        return false;
    }

    struct blob_buf b;
    std::memset(&b, 0, sizeof(b));
    blob_buf_init(&b, 0);

    // Parse JSON data and add to blob
    if (!data.empty()) {
        blobmsg_add_json_from_string(&b, data.c_str());
    }

    int ret = ubus_send_event(m_ctx, eventName.c_str(), b.head);
    blob_buf_free(&b);

    if (ret) {
        m_lastError = "Failed to send ubus event: " + std::string(ubus_strerror(ret));
        return false;
    }

    return true;
}

std::string UbusInterface::getLastError() const {
    return m_lastError;
}

// Static method handlers
UbusInterface* UbusInterface::getInstance(struct ubus_object* obj) {
    return reinterpret_cast<UbusInterface*>(
        reinterpret_cast<char*>(obj) - offsetof(UbusInterface, m_ubusObj));
}

int UbusInterface::handleStatus(struct ubus_context* ctx,
                                struct ubus_object* obj,
                                struct ubus_request_data* req,
                                const char* method,
                                struct blob_attr* msg) {
    UbusInterface* self = getInstance(obj);
    return self->doStatus(req, msg);
}

int UbusInterface::handleResourceRead(struct ubus_context* ctx,
                                      struct ubus_object* obj,
                                      struct ubus_request_data* req,
                                      const char* method,
                                      struct blob_attr* msg) {
    UbusInterface* self = getInstance(obj);
    return self->doResourceRead(req, msg);
}

int UbusInterface::handleResourceWrite(struct ubus_context* ctx,
                                       struct ubus_object* obj,
                                       struct ubus_request_data* req,
                                       const char* method,
                                       struct blob_attr* msg) {
    UbusInterface* self = getInstance(obj);
    return self->doResourceWrite(req, msg);
}

int UbusInterface::handleRegister(struct ubus_context* ctx,
                                  struct ubus_object* obj,
                                  struct ubus_request_data* req,
                                  const char* method,
                                  struct blob_attr* msg) {
    UbusInterface* self = getInstance(obj);
    return self->doRegister(req, msg);
}

int UbusInterface::handleUpdate(struct ubus_context* ctx,
                               struct ubus_object* obj,
                               struct ubus_request_data* req,
                               const char* method,
                               struct blob_attr* msg) {
    UbusInterface* self = getInstance(obj);
    return self->doUpdate(req, msg);
}

int UbusInterface::handleDeregister(struct ubus_context* ctx,
                                   struct ubus_object* obj,
                                   struct ubus_request_data* req,
                                   const char* method,
                                   struct blob_attr* msg) {
    UbusInterface* self = getInstance(obj);
    return self->doDeregister(req, msg);
}

int UbusInterface::handleListObjects(struct ubus_context* ctx,
                                    struct ubus_object* obj,
                                    struct ubus_request_data* req,
                                    const char* method,
                                    struct blob_attr* msg) {
    UbusInterface* self = getInstance(obj);
    return self->doListObjects(req, msg);
}

int UbusInterface::handleReloadConfig(struct ubus_context* ctx,
                                     struct ubus_object* obj,
                                     struct ubus_request_data* req,
                                     const char* method,
                                     struct blob_attr* msg) {
    UbusInterface* self = getInstance(obj);
    return self->doReloadConfig(req, msg);
}

// Instance method implementations
int UbusInterface::doStatus(struct ubus_request_data* req, struct blob_attr* msg) {
    struct blob_buf b;
    std::memset(&b, 0, sizeof(b));
    blob_buf_init(&b, 0);

    ClientStatus status = getClientStatus();

    blobmsg_add_u8(&b, "registered", status.registered);
    blobmsg_add_string(&b, "server_uri", status.serverUri.c_str());
    blobmsg_add_string(&b, "endpoint", status.endpoint.c_str());
    blobmsg_add_u32(&b, "lifetime", status.lifetime);
    blobmsg_add_u64(&b, "last_update", status.lastUpdate);
    blobmsg_add_string(&b, "state", status.state.c_str());

    ubus_send_reply(m_ctx, req, b.head);
    blob_buf_free(&b);

    return 0;
}

int UbusInterface::doResourceRead(struct ubus_request_data* req, struct blob_attr* msg) {
    struct blob_attr* tb[READ_MAX];
    blobmsg_parse(read_policy, READ_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[READ_OBJECT_ID] || !tb[READ_INSTANCE_ID] || !tb[READ_RESOURCE_ID]) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    int objectId = blobmsg_get_u32(tb[READ_OBJECT_ID]);
    int instanceId = blobmsg_get_u32(tb[READ_INSTANCE_ID]);
    int resourceId = blobmsg_get_u32(tb[READ_RESOURCE_ID]);

    ResourceValue resValue = readResource(objectId, instanceId, resourceId);

    struct blob_buf b;
    std::memset(&b, 0, sizeof(b));
    blob_buf_init(&b, 0);

    blobmsg_add_u32(&b, "object_id", resValue.objectId);
    blobmsg_add_u32(&b, "instance_id", resValue.instanceId);
    blobmsg_add_u32(&b, "resource_id", resValue.resourceId);
    blobmsg_add_string(&b, "value", resValue.value.c_str());
    blobmsg_add_string(&b, "type", resValue.type.c_str());

    ubus_send_reply(m_ctx, req, b.head);
    blob_buf_free(&b);

    return 0;
}

int UbusInterface::doResourceWrite(struct ubus_request_data* req, struct blob_attr* msg) {
    struct blob_attr* tb[WRITE_MAX];
    blobmsg_parse(write_policy, WRITE_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[WRITE_OBJECT_ID] || !tb[WRITE_INSTANCE_ID] ||
        !tb[WRITE_RESOURCE_ID] || !tb[WRITE_VALUE]) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    int objectId = blobmsg_get_u32(tb[WRITE_OBJECT_ID]);
    int instanceId = blobmsg_get_u32(tb[WRITE_INSTANCE_ID]);
    int resourceId = blobmsg_get_u32(tb[WRITE_RESOURCE_ID]);
    std::string value = blobmsg_get_string(tb[WRITE_VALUE]);

    bool success = writeResource(objectId, instanceId, resourceId, value);

    struct blob_buf b;
    std::memset(&b, 0, sizeof(b));
    blob_buf_init(&b, 0);

    blobmsg_add_u8(&b, "success", success);
    if (!success) {
        blobmsg_add_string(&b, "error", m_lastError.c_str());
    }

    ubus_send_reply(m_ctx, req, b.head);
    blob_buf_free(&b);

    return 0;
}

int UbusInterface::doRegister(struct ubus_request_data* req, struct blob_attr* msg) {
    // TODO: Trigger LwM2M registration
    struct blob_buf b;
    std::memset(&b, 0, sizeof(b));
    blob_buf_init(&b, 0);

    blobmsg_add_u8(&b, "success", true);
    blobmsg_add_string(&b, "message", "Registration triggered");

    ubus_send_reply(m_ctx, req, b.head);
    blob_buf_free(&b);

    return 0;
}

int UbusInterface::doUpdate(struct ubus_request_data* req, struct blob_attr* msg) {
    // TODO: Trigger LwM2M update
    struct blob_buf b;
    std::memset(&b, 0, sizeof(b));
    blob_buf_init(&b, 0);

    blobmsg_add_u8(&b, "success", true);
    blobmsg_add_string(&b, "message", "Update triggered");

    ubus_send_reply(m_ctx, req, b.head);
    blob_buf_free(&b);

    return 0;
}

int UbusInterface::doDeregister(struct ubus_request_data* req, struct blob_attr* msg) {
    // TODO: Trigger LwM2M deregistration
    struct blob_buf b;
    std::memset(&b, 0, sizeof(b));
    blob_buf_init(&b, 0);

    blobmsg_add_u8(&b, "success", true);
    blobmsg_add_string(&b, "message", "Deregistration triggered");

    ubus_send_reply(m_ctx, req, b.head);
    blob_buf_free(&b);

    return 0;
}

int UbusInterface::doListObjects(struct ubus_request_data* req, struct blob_attr* msg) {
    // TODO: List all LwM2M objects and instances
    struct blob_buf b;
    std::memset(&b, 0, sizeof(b));
    blob_buf_init(&b, 0);

    void* array = blobmsg_open_array(&b, "objects");

    // Example objects (should query from WppClient)
    void* table = blobmsg_open_table(&b, NULL);
    blobmsg_add_u32(&b, "id", 0);
    blobmsg_add_string(&b, "name", "Security");
    blobmsg_close_table(&b, table);

    table = blobmsg_open_table(&b, NULL);
    blobmsg_add_u32(&b, "id", 1);
    blobmsg_add_string(&b, "name", "Server");
    blobmsg_close_table(&b, table);

    table = blobmsg_open_table(&b, NULL);
    blobmsg_add_u32(&b, "id", 3);
    blobmsg_add_string(&b, "name", "Device");
    blobmsg_close_table(&b, table);

    blobmsg_close_array(&b, array);

    ubus_send_reply(m_ctx, req, b.head);
    blob_buf_free(&b);

    return 0;
}

int UbusInterface::doReloadConfig(struct ubus_request_data* req, struct blob_attr* msg) {
    // TODO: Reload configuration from UCI
    struct blob_buf b;
    std::memset(&b, 0, sizeof(b));
    blob_buf_init(&b, 0);

    blobmsg_add_u8(&b, "success", true);
    blobmsg_add_string(&b, "message", "Configuration reloaded");

    ubus_send_reply(m_ctx, req, b.head);
    blob_buf_free(&b);

    return 0;
}

UbusInterface::ClientStatus UbusInterface::getClientStatus() const {
    ClientStatus status;

    // TODO: Get actual status from WppClient
    status.registered = false;
    status.serverUri = "";
    status.endpoint = "";
    status.lifetime = 0;
    status.lastUpdate = 0;
    status.state = "initialized";

    return status;
}

UbusInterface::ResourceValue UbusInterface::readResource(int objectId,
                                                        int instanceId,
                                                        int resourceId) const {
    ResourceValue value;
    value.objectId = objectId;
    value.instanceId = instanceId;
    value.resourceId = resourceId;

    // TODO: Read actual resource from WppClient
    value.value = "";
    value.type = "unknown";

    return value;
}

bool UbusInterface::writeResource(int objectId, int instanceId, int resourceId,
                                  const std::string& value) {
    // TODO: Write actual resource to WppClient
    m_lastError = "Not implemented";
    return false;
}

} // namespace wpp
