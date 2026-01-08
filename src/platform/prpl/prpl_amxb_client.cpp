/**
 * @file prpl_amxb_client.cpp
 * @brief Ambiorix Bus Client Implementation
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "prpl_amxb_client.h"

#include <cstring>
#include <iostream>
#include <sstream>

// Ambiorix headers (conditional compilation)
#ifdef HAVE_AMBIORIX
#include <amxb/amxb.h>
#include <amxc/amxc.h>
#include <amxc/amxc_variant.h>
#include <amxd/amxd_dm.h>
#else
// Stub definitions when Ambiorix is not available
typedef struct _amxb_bus_ctx_t { int dummy; } amxb_bus_ctx_t;
typedef struct _amxc_var_t { int dummy; } amxc_var_t;
typedef struct _amxd_object_t { int dummy; } amxd_object_t;
#endif

namespace lwm2m {
namespace platform {
namespace prpl {

// ============================================================================
// AmxVariant Implementation
// ============================================================================

AmxVariant::AmxVariant() : var_(nullptr) {
#ifdef HAVE_AMBIORIX
    var_ = (amxc_var_t*)calloc(1, sizeof(amxc_var_t));
    if (var_) {
        amxc_var_init(var_);
    }
#endif
}

AmxVariant::AmxVariant(const std::string& value) : AmxVariant() {
#ifdef HAVE_AMBIORIX
    if (var_) {
        amxc_var_set(cstring_t, var_, value.c_str());
    }
#endif
}

AmxVariant::AmxVariant(int64_t value) : AmxVariant() {
#ifdef HAVE_AMBIORIX
    if (var_) {
        amxc_var_set(int64_t, var_, value);
    }
#endif
}

AmxVariant::AmxVariant(bool value) : AmxVariant() {
#ifdef HAVE_AMBIORIX
    if (var_) {
        amxc_var_set(bool, var_, value);
    }
#endif
}

AmxVariant::AmxVariant(double value) : AmxVariant() {
#ifdef HAVE_AMBIORIX
    if (var_) {
        amxc_var_set(double, var_, value);
    }
#endif
}

AmxVariant::~AmxVariant() {
    cleanup();
}

void AmxVariant::cleanup() {
#ifdef HAVE_AMBIORIX
    if (var_) {
        amxc_var_clean(var_);
        free(var_);
        var_ = nullptr;
    }
#endif
}

AmxVariant::AmxVariant(const AmxVariant& other) : var_(nullptr) {
#ifdef HAVE_AMBIORIX
    var_ = (amxc_var_t*)calloc(1, sizeof(amxc_var_t));
    if (var_ && other.var_) {
        amxc_var_copy(var_, other.var_);
    }
#endif
}

AmxVariant& AmxVariant::operator=(const AmxVariant& other) {
    if (this != &other) {
        cleanup();
#ifdef HAVE_AMBIORIX
        var_ = (amxc_var_t*)calloc(1, sizeof(amxc_var_t));
        if (var_ && other.var_) {
            amxc_var_copy(var_, other.var_);
        }
#endif
    }
    return *this;
}

AmxVariant::AmxVariant(AmxVariant&& other) noexcept : var_(other.var_) {
    other.var_ = nullptr;
}

AmxVariant& AmxVariant::operator=(AmxVariant&& other) noexcept {
    if (this != &other) {
        cleanup();
        var_ = other.var_;
        other.var_ = nullptr;
    }
    return *this;
}

AmxVariant::Type AmxVariant::getType() const {
#ifdef HAVE_AMBIORIX
    if (!var_) return Type::INVALID;

    uint32_t type = amxc_var_type_of(var_);
    switch (type) {
        case AMXC_VAR_ID_CSTRING:
        case AMXC_VAR_ID_CSV_STRING:
        case AMXC_VAR_ID_SSV_STRING:
            return Type::STRING;
        case AMXC_VAR_ID_INT8:
        case AMXC_VAR_ID_INT16:
        case AMXC_VAR_ID_INT32:
        case AMXC_VAR_ID_INT64:
        case AMXC_VAR_ID_UINT8:
        case AMXC_VAR_ID_UINT16:
        case AMXC_VAR_ID_UINT32:
        case AMXC_VAR_ID_UINT64:
            return Type::INTEGER;
        case AMXC_VAR_ID_BOOL:
            return Type::BOOLEAN;
        case AMXC_VAR_ID_FLOAT:
        case AMXC_VAR_ID_DOUBLE:
            return Type::DOUBLE;
        case AMXC_VAR_ID_LIST:
            return Type::LIST;
        case AMXC_VAR_ID_HTABLE:
            return Type::OBJECT;
        default:
            return Type::INVALID;
    }
#else
    return Type::INVALID;
#endif
}

bool AmxVariant::isValid() const {
    return getType() != Type::INVALID;
}

std::string AmxVariant::asString() const {
#ifdef HAVE_AMBIORIX
    if (!var_) return "";
    const char* str = amxc_var_constcast(cstring_t, var_);
    return str ? str : "";
#else
    return "";
#endif
}

int64_t AmxVariant::asInteger() const {
#ifdef HAVE_AMBIORIX
    if (!var_) return 0;
    return amxc_var_constcast(int64_t, var_);
#else
    return 0;
#endif
}

bool AmxVariant::asBoolean() const {
#ifdef HAVE_AMBIORIX
    if (!var_) return false;
    return amxc_var_constcast(bool, var_);
#else
    return false;
#endif
}

double AmxVariant::asDouble() const {
#ifdef HAVE_AMBIORIX
    if (!var_) return 0.0;
    return amxc_var_constcast(double, var_);
#else
    return 0.0;
#endif
}

amxc_var_t* AmxVariant::getRaw() {
    return var_;
}

const amxc_var_t* AmxVariant::getRaw() const {
    return var_;
}

// ============================================================================
// PrplAmxbClient Implementation
// ============================================================================

PrplAmxbClient::PrplAmxbClient()
    : busContext_(nullptr)
    , connected_(false)
    , inTransaction_(false)
    , debugMode_(false)
    , nextSubscriptionId_(1) {
}

PrplAmxbClient::~PrplAmxbClient() {
    disconnect();
}

bool PrplAmxbClient::connect(const std::string& uri) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (connected_) {
        return true;
    }

#ifdef HAVE_AMBIORIX
    // Initialize Ambiorix library
    if (!initializeBus()) {
        setError("Failed to initialize Ambiorix library");
        return false;
    }

    // Parse bus type from URI
    // Format: <type>:<path>
    // Example: ubus:/var/run/ubus/ubus.sock
    size_t colonPos = uri.find(':');
    if (colonPos != std::string::npos) {
        busType_ = uri.substr(0, colonPos);
    } else {
        busType_ = "ubus"; // Default
    }

    busUri_ = uri;

    // Connect to the bus
    busContext_ = (amxb_bus_ctx_t*)calloc(1, sizeof(amxb_bus_ctx_t));
    if (!busContext_) {
        setError("Failed to allocate bus context");
        return false;
    }

    int result = amxb_connect(busContext_, uri.c_str());
    if (result != 0) {
        setError("Failed to connect to bus: " + std::string(strerror(-result)));
        free(busContext_);
        busContext_ = nullptr;
        return false;
    }

    connected_ = true;
    logDebug("Connected to Ambiorix bus: " + uri);
    return true;
#else
    setError("Ambiorix support not compiled in");
    return false;
#endif
}

void PrplAmxbClient::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_) {
        return;
    }

#ifdef HAVE_AMBIORIX
    // Unsubscribe from all events
    subscriptions_.clear();

    // Disconnect from bus
    if (busContext_) {
        amxb_disconnect(busContext_);
        amxb_free(&busContext_);
        busContext_ = nullptr;
    }

    cleanupBus();
#endif

    connected_ = false;
    logDebug("Disconnected from Ambiorix bus");
}

bool PrplAmxbClient::isConnected() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_;
}

std::string PrplAmxbClient::getBusType() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return busType_;
}

std::vector<std::string> PrplAmxbClient::list(const std::string& path) {
    std::vector<std::string> result;

#ifdef HAVE_AMBIORIX
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_ || !busContext_) {
        setError("Not connected to bus");
        return result;
    }

    amxc_var_t ret;
    amxc_var_init(&ret);

    int status = amxb_get(busContext_, path.c_str(), 0, &ret, 5);
    if (status != 0) {
        setError("Failed to list objects at path: " + path);
        amxc_var_clean(&ret);
        return result;
    }

    // Parse result and extract object paths
    const amxc_htable_t* table = amxc_var_constcast(amxc_htable_t, &ret);
    if (table) {
        amxc_htable_for_each(it, table) {
            const char* key = amxc_htable_it_get_key(it);
            if (key) {
                result.push_back(key);
            }
        }
    }

    amxc_var_clean(&ret);
#endif

    return result;
}

std::map<std::string, std::string> PrplAmxbClient::get(const std::string& path) {
    std::map<std::string, std::string> result;

#ifdef HAVE_AMBIORIX
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_ || !busContext_) {
        setError("Not connected to bus");
        return result;
    }

    amxc_var_t ret;
    amxc_var_init(&ret);

    int status = amxb_get(busContext_, path.c_str(), 0, &ret, 5);
    if (status != 0) {
        setError("Failed to get object: " + path);
        amxc_var_clean(&ret);
        return result;
    }

    result = variantToMap(&ret);
    amxc_var_clean(&ret);
#endif

    return result;
}

std::string PrplAmxbClient::getValue(const std::string& path, const std::string& parameter) {
    auto params = get(path);
    auto it = params.find(parameter);
    return (it != params.end()) ? it->second : "";
}

bool PrplAmxbClient::set(const std::string& path,
                         const std::map<std::string, std::string>& parameters) {
#ifdef HAVE_AMBIORIX
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_ || !busContext_) {
        setError("Not connected to bus");
        return false;
    }

    amxc_var_t args;
    amxc_var_init(&args);
    amxc_var_set_type(&args, AMXC_VAR_ID_HTABLE);

    // Build parameter hash table
    for (const auto& [key, value] : parameters) {
        amxc_var_t* param = amxc_var_add_key(cstring_t, &args, key.c_str(), value.c_str());
    }

    amxc_var_t ret;
    amxc_var_init(&ret);

    int status = amxb_set(busContext_, path.c_str(), &args, &ret, 5);

    amxc_var_clean(&args);
    amxc_var_clean(&ret);

    if (status != 0) {
        setError("Failed to set parameters on: " + path);
        return false;
    }

    logDebug("Set parameters on: " + path);
    return true;
#else
    return false;
#endif
}

bool PrplAmxbClient::setValue(const std::string& path, const std::string& parameter,
                              const std::string& value) {
    std::map<std::string, std::string> params;
    params[parameter] = value;
    return set(path, params);
}

std::string PrplAmxbClient::add(const std::string& path, uint32_t index,
                                const std::map<std::string, std::string>& parameters) {
#ifdef HAVE_AMBIORIX
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_ || !busContext_) {
        setError("Not connected to bus");
        return "";
    }

    amxc_var_t args;
    amxc_var_init(&args);
    amxc_var_set_type(&args, AMXC_VAR_ID_HTABLE);

    if (index > 0) {
        amxc_var_add_key(uint32_t, &args, "index", index);
    }

    for (const auto& [key, value] : parameters) {
        amxc_var_add_key(cstring_t, &args, key.c_str(), value.c_str());
    }

    amxc_var_t ret;
    amxc_var_init(&ret);

    int status = amxb_add(busContext_, path.c_str(), &args, &ret, 5);

    amxc_var_clean(&args);

    if (status != 0) {
        setError("Failed to add instance to: " + path);
        amxc_var_clean(&ret);
        return "";
    }

    // Extract the created instance path
    std::string instancePath = variantToString(&ret);

    amxc_var_clean(&ret);

    logDebug("Added instance: " + instancePath);
    return instancePath;
#else
    return "";
#endif
}

bool PrplAmxbClient::del(const std::string& path) {
#ifdef HAVE_AMBIORIX
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_ || !busContext_) {
        setError("Not connected to bus");
        return false;
    }

    amxc_var_t args;
    amxc_var_init(&args);

    amxc_var_t ret;
    amxc_var_init(&ret);

    int status = amxb_del(busContext_, path.c_str(), &args, &ret, 5);

    amxc_var_clean(&args);
    amxc_var_clean(&ret);

    if (status != 0) {
        setError("Failed to delete: " + path);
        return false;
    }

    logDebug("Deleted: " + path);
    return true;
#else
    return false;
#endif
}

bool PrplAmxbClient::invoke(const std::string& path, const std::string& method,
                            const std::map<std::string, std::string>& arguments,
                            std::map<std::string, std::string>& result) {
#ifdef HAVE_AMBIORIX
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_ || !busContext_) {
        setError("Not connected to bus");
        return false;
    }

    amxc_var_t args;
    amxc_var_init(&args);
    amxc_var_set_type(&args, AMXC_VAR_ID_HTABLE);

    for (const auto& [key, value] : arguments) {
        amxc_var_add_key(cstring_t, &args, key.c_str(), value.c_str());
    }

    amxc_var_t ret;
    amxc_var_init(&ret);

    int status = amxb_call(busContext_, path.c_str(), method.c_str(), &args, &ret, 5);

    amxc_var_clean(&args);

    if (status != 0) {
        setError("Failed to invoke method: " + method + " on: " + path);
        amxc_var_clean(&ret);
        return false;
    }

    result = variantToMap(&ret);
    amxc_var_clean(&ret);

    logDebug("Invoked method: " + method + " on: " + path);
    return true;
#else
    return false;
#endif
}

bool PrplAmxbClient::invoke(const std::string& path, const std::string& method,
                            const std::map<std::string, std::string>& arguments) {
    std::map<std::string, std::string> result;
    return invoke(path, method, arguments, result);
}

uint32_t PrplAmxbClient::subscribe(const std::string& path, const std::string& event,
                                   EventCallback callback) {
#ifdef HAVE_AMBIORIX
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_ || !busContext_) {
        setError("Not connected to bus");
        return 0;
    }

    // Store subscription
    Subscription sub;
    sub.id = nextSubscriptionId_++;
    sub.path = path;
    sub.event = event;
    sub.callback = callback;

    subscriptions_[sub.id] = sub;

    // Subscribe via Ambiorix
    // Note: Actual subscription implementation depends on Ambiorix event API

    logDebug("Subscribed to event: " + event + " on: " + path);
    return sub.id;
#else
    return 0;
#endif
}

bool PrplAmxbClient::unsubscribe(uint32_t subscriptionId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = subscriptions_.find(subscriptionId);
    if (it == subscriptions_.end()) {
        return false;
    }

#ifdef HAVE_AMBIORIX
    // Unsubscribe via Ambiorix
    // Note: Actual unsubscribe implementation depends on Ambiorix event API
#endif

    subscriptions_.erase(it);
    logDebug("Unsubscribed from subscription ID: " + std::to_string(subscriptionId));
    return true;
}

int PrplAmxbClient::processEvents(int timeoutMs) {
#ifdef HAVE_AMBIORIX
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_ || !busContext_) {
        return 0;
    }

    // Process pending events
    // Note: Implementation depends on Ambiorix event loop API
    // This is a simplified stub

    return 0;
#else
    return 0;
#endif
}

std::vector<std::string> PrplAmxbClient::resolve(const std::string& path) {
    std::vector<std::string> result;

#ifdef HAVE_AMBIORIX
    // Use list operation to resolve wildcards
    result = list(path);
#endif

    return result;
}

std::map<std::string, std::string> PrplAmxbClient::describe(const std::string& path) {
    std::map<std::string, std::string> result;

#ifdef HAVE_AMBIORIX
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_ || !busContext_) {
        setError("Not connected to bus");
        return result;
    }

    amxc_var_t ret;
    amxc_var_init(&ret);

    int status = amxb_describe(busContext_, path.c_str(), &ret, 5);
    if (status == 0) {
        result = variantToMap(&ret);
    }

    amxc_var_clean(&ret);
#endif

    return result;
}

bool PrplAmxbClient::exists(const std::string& path) {
    auto objects = list(path);
    return !objects.empty();
}

uint32_t PrplAmxbClient::getInstanceCount(const std::string& path) {
    auto instances = list(path + ".*");
    return static_cast<uint32_t>(instances.size());
}

bool PrplAmxbClient::beginTransaction() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (inTransaction_) {
        return false;
    }
    inTransaction_ = true;
    return true;
}

bool PrplAmxbClient::commitTransaction() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!inTransaction_) {
        return false;
    }
    // Commit logic here
    inTransaction_ = false;
    return true;
}

void PrplAmxbClient::abortTransaction() {
    std::lock_guard<std::mutex> lock(mutex_);
    inTransaction_ = false;
}

bool PrplAmxbClient::isInTransaction() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return inTransaction_;
}

std::string PrplAmxbClient::getLastError() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return lastError_;
}

void PrplAmxbClient::setDebugMode(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    debugMode_ = enable;
}

// ============================================================================
// Private Helper Methods
// ============================================================================

bool PrplAmxbClient::initializeBus() {
#ifdef HAVE_AMBIORIX
    // Initialize Ambiorix backend
    int result = amxb_be_load_multiple(nullptr);
    if (result != 0) {
        return false;
    }
    return true;
#else
    return false;
#endif
}

void PrplAmxbClient::cleanupBus() {
#ifdef HAVE_AMBIORIX
    amxb_be_remove_all();
#endif
}

void PrplAmxbClient::setError(const std::string& error) {
    lastError_ = error;
    if (debugMode_) {
        std::cerr << "PrplAmxbClient Error: " << error << std::endl;
    }
}

void PrplAmxbClient::logDebug(const std::string& message) {
    if (debugMode_) {
        std::cout << "PrplAmxbClient: " << message << std::endl;
    }
}

std::map<std::string, std::string> PrplAmxbClient::variantToMap(const amxc_var_t* var) {
    std::map<std::string, std::string> result;

#ifdef HAVE_AMBIORIX
    if (!var) return result;

    const amxc_htable_t* table = amxc_var_constcast(amxc_htable_t, var);
    if (table) {
        amxc_htable_for_each(it, table) {
            const char* key = amxc_htable_it_get_key(it);
            amxc_var_t* value = amxc_var_from_htable_it(it);

            if (key && value) {
                const char* str = amxc_var_constcast(cstring_t, value);
                result[key] = str ? str : "";
            }
        }
    }
#endif

    return result;
}

amxc_var_t* PrplAmxbClient::mapToVariant(const std::map<std::string, std::string>& map) {
#ifdef HAVE_AMBIORIX
    amxc_var_t* var = (amxc_var_t*)calloc(1, sizeof(amxc_var_t));
    if (!var) return nullptr;

    amxc_var_init(var);
    amxc_var_set_type(var, AMXC_VAR_ID_HTABLE);

    for (const auto& [key, value] : map) {
        amxc_var_add_key(cstring_t, var, key.c_str(), value.c_str());
    }

    return var;
#else
    return nullptr;
#endif
}

std::string PrplAmxbClient::variantToString(const amxc_var_t* var) {
#ifdef HAVE_AMBIORIX
    if (!var) return "";
    const char* str = amxc_var_constcast(cstring_t, var);
    return str ? str : "";
#else
    return "";
#endif
}

void PrplAmxbClient::freeVariant(amxc_var_t* var) {
#ifdef HAVE_AMBIORIX
    if (var) {
        amxc_var_clean(var);
        free(var);
    }
#endif
}

} // namespace prpl
} // namespace platform
} // namespace lwm2m
