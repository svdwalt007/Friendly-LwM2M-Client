# API Reference

Complete API documentation for the Friendly LwM2M Client.

---

## Core Classes

### WppClient

Main client class for LwM2M operations.

#### Methods

```cpp
class WppClient {
public:
    // Constructor
    WppClient(const std::string& endpoint, uint16_t localPort = 0);

    // Connection management
    bool connect();
    bool disconnect();
    void process(int timeout_ms);

    // Registry access
    WppRegistry& registry();

    // Data operations
    bool send(const DataLink& data);
    bool send(const DataLink data[], size_t count);

    // Ownership transfer
    void giveOwnership();

    // Client state
    bool isConnected();
    const std::string& getEndpoint();
};
```

#### Example Usage

```cpp
// Create client
WppClient* client = new WppClient("my-device-001", 56830);

// Initialize objects
client->registry().registerObj(Location::object(*client));
Location::createInst(*client);

// Start client
client->connect();

// Main loop
while (running) {
    client->process(1000);  // Process for 1 second
}

// Cleanup
client->disconnect();
delete client;
```

---

### WppRegistry

Object registry for managing LwM2M objects.

#### Methods

```cpp
class WppRegistry {
public:
    // Object management
    bool registerObj(Object& object);
    bool deregisterObj(Object& object);
    bool isObjRegistered(Object& object);
    bool isExist(OBJ_ID objId);

    // Object access
    Object* object(OBJ_ID objId);
    std::vector<Object*>& objects();

    // Standard object accessors
    Object& device();
    Object& lwm2mServer();
    Object& lwm2mSecurity();
    Object& connectivityMonitoring();
    Object& location();
    Object& firmwareUpdate();

    // Walt Technologies objects
    Object& starlinkTerminal();
    Object& routerManagement();
    Object& ethernetInterface();
    // ... more accessors
};
```

#### Example Usage

```cpp
// Register object
client.registry().registerObj(Location::object(client));

// Get object
Object& locObj = client.registry().location();

// Check if object exists
if (client.registry().isExist(OBJ_ID::LOCATION)) {
    // Object is available
}
```

---

### Instance

Base class for all LwM2M object instances.

#### Methods

```cpp
class Instance {
public:
    // Resource access
    Resource* resource(ID_T resId);

    // Value setters (single instance resources)
    template<typename T>
    bool set(ID_T resId, const T& value);

    // Value setters (multiple instance resources)
    template<typename T>
    bool set(ID_T resId, const T& value, ID_T resInstId);

    // Value getters (single instance resources)
    template<typename T>
    T get(ID_T resId);

    // Value getters (multiple instance resources)
    template<typename T>
    T get(ID_T resId, ID_T resInstId);

    // Notifications
    void notifyResChanged(ID_T resId, ID_T resInstId = 0);
    void notifyResChanged(const std::vector<ID_T>& resIds);

    // Execute handlers
    using ExecuteCallback = std::function<bool(Instance&, ID_T, const OPAQUE_T&)>;
    void setExecute(ID_T resId, ExecuteCallback callback);

    // Instance info
    OBJ_ID getObjectID();
    OBJ_INST_ID_T getInstanceID();
};
```

#### Example Usage

```cpp
// Create instance
Location* loc = Location::createInst(client);

// Set values
loc->set<FLOAT_T>(Location::LATITUDE_0, 40.7128);
loc->set<FLOAT_T>(Location::LONGITUDE_1, -74.0060);

// Get values
float lat = loc->get<FLOAT_T>(Location::LATITUDE_0);

// Multiple instance resource
loc->set<STRING_T>(Location::IP_ADDRESSES_4, "192.168.1.1", 0);
loc->set<STRING_T>(Location::IP_ADDRESSES_4, "10.0.0.1", 1);

// Notify server of changes
loc->notifyResChanged(Location::LATITUDE_0);

// Set execute handler
loc->setExecute(Location::RESET_RESOURCE,
    [](Instance& inst, ID_T resId, const OPAQUE_T& data) {
        // Handle execute
        return true;
    }
);
```

---

## Location Object API

### Class: Location

Implementation of OMA LwM2M Location object (ID 6).

#### Resource IDs

```cpp
enum ID: ID_T {
    LATITUDE_0 = 0,          // Float, degrees
    LONGITUDE_1 = 1,         // Float, degrees
    ALTITUDE_2 = 2,          // Float, meters (optional)
    RADIUS_3 = 3,            // Float, meters (optional)
    VELOCITY_4 = 4,          // Opaque (optional)
    TIMESTAMP_5 = 5,         // Time, Unix timestamp
    SPEED_6 = 6,             // Float, m/s (optional)
};
```

#### Static Methods

```cpp
class Location : public Instance {
public:
    // Object access
    static Object& object(WppClient& ctx);

    // Instance management
    static Location* instance(WppClient& ctx, ID_T instId = ID_T_MAX_VAL);
    static Location* createInst(WppClient& ctx, ID_T instId = ID_T_MAX_VAL);
    static bool removeInst(WppClient& ctx, ID_T instId);

    // Update location (manual trigger)
    bool updateLocation();
};
```

#### Example Usage

```cpp
// Create location instance
Location* loc = Location::createInst(client);

// Read location
float lat = loc->get<FLOAT_T>(Location::LATITUDE_0);
float lon = loc->get<FLOAT_T>(Location::LONGITUDE_1);
time_t timestamp = loc->get<TIME_T>(Location::TIMESTAMP_5);

printf("Location: %f, %f at %ld\n", lat, lon, timestamp);

// Force location update
loc->updateLocation();

// Read updated values
float newLat = loc->get<FLOAT_T>(Location::LATITUDE_0);
```

---

## Device Object API

### Class: Device

Implementation of OMA LwM2M Device object (ID 3).

#### Resource IDs

```cpp
enum ID: ID_T {
    MANUFACTURER_0 = 0,              // String
    MODEL_NUMBER_1 = 1,              // String
    SERIAL_NUMBER_2 = 2,             // String
    FIRMWARE_VERSION_3 = 3,          // String
    REBOOT_4 = 4,                    // Execute
    FACTORY_RESET_5 = 5,             // Execute
    AVAILABLE_POWER_SOURCES_6 = 6,   // Int (multiple)
    POWER_SOURCE_VOLTAGE_7 = 7,      // Int (multiple)
    POWER_SOURCE_CURRENT_8 = 8,      // Int (multiple)
    BATTERY_LEVEL_9 = 9,             // Int (%)
    MEMORY_FREE_10 = 10,             // Int (KB)
    ERROR_CODE_11 = 11,              // Int (multiple)
    CURRENT_TIME_13 = 13,            // Time
    SUPPORTED_BINDING_AND_MODES_16 = 16, // String
    DEVICE_TYPE_17 = 17,             // String
    HARDWARE_VERSION_18 = 18,        // String
    // ... more resources
};
```

#### Power Source Enum

```cpp
enum PwrSrcs: uint8_t {
    DC = 0,                 // DC power
    INTERN_BAT = 1,         // Internal battery
    EXTERN_BAT = 2,         // External battery
    FUEL_CELL = 3,          // Fuel cell
    PWR_OVER_ETHERNET = 4,  // PoE
    USB = 5,                // USB
    AC = 6,                 // AC (mains) power
    SOLAR = 7,              // Solar
};
```

#### Example Usage

```cpp
// Get device info
Device* dev = Device::instance(client);

std::string manufacturer = dev->get<STRING_T>(Device::MANUFACTURER_0);
std::string model = dev->get<STRING_T>(Device::MODEL_NUMBER_1);
int memFree = dev->get<INT_T>(Device::MEMORY_FREE_10);

// Trigger reboot
dev->resource(Device::REBOOT_4)->execute(OPAQUE_T{});
```

---

## Connectivity Monitoring API

### Class: ConnectivityMonitoring

Implementation of OMA LwM2M Connectivity Monitoring object (ID 4).

#### Resource IDs

```cpp
enum ID: ID_T {
    NETWORK_BEARER_0 = 0,            // Int
    AVAILABLE_NETWORK_BEARER_1 = 1,  // Int (multiple)
    RADIO_SIGNAL_STRENGTH_2 = 2,     // Int (dBm)
    LINK_QUALITY_3 = 3,              // Int (%)
    IP_ADDRESSES_4 = 4,              // String (multiple)
    ROUTER_IP_ADDRESSES_5 = 5,       // String (multiple)
    LINK_UTILIZATION_6 = 6,          // Int (%)
    APN_7 = 7,                       // String (multiple)
    CELL_ID_8 = 8,                   // Int
    SMNC_9 = 9,                      // Int
    SMCC_10 = 10,                    // Int
};
```

#### Network Bearer Enum

```cpp
enum NtwrkBrr: INT_T {
    GSM = 0,
    TD_SCDMA = 1,
    WCDMA = 2,
    CDMA2000 = 3,
    WIMAX = 4,
    LTE_TDD = 5,
    LTE_FDD = 6,
    LTE_M = 7,
    NBIOT = 8,
    ETHERNET = 41,
    WLAN = 21,
};
```

#### Example Usage

```cpp
ConnectivityMonitoring* conn = ConnectivityMonitoring::instance(client);

// Get network info
int bearer = conn->get<INT_T>(ConnectivityMonitoring::NETWORK_BEARER_0);
int signalStrength = conn->get<INT_T>(ConnectivityMonitoring::RADIO_SIGNAL_STRENGTH_2);

// Get IP addresses (multiple instance resource)
std::string ip0 = conn->get<STRING_T>(ConnectivityMonitoring::IP_ADDRESSES_4, 0);
std::string ip1 = conn->get<STRING_T>(ConnectivityMonitoring::IP_ADDRESSES_4, 1);
```

---

## Task Queue API

### WppTaskQueue

Asynchronous task scheduling.

#### Methods

```cpp
class WppTaskQueue {
public:
    using TaskCallback = std::function<bool(WppClient&, void*)>;

    // Add task
    static task_id_t addTask(
        int interval_sec,
        TaskCallback callback,
        void* context = nullptr
    );

    // Remove task
    static void requestToRemoveTask(task_id_t taskId);

    // Process tasks (called by WppClient::process)
    static void processTasks(WppClient& client);
};
```

#### Example Usage

```cpp
// Add periodic task (every 60 seconds)
task_id_t taskId = WppTaskQueue::addTask(60,
    [](WppClient& client, void* ctx) {
        // Update logic here
        float temp = readTemperature();
        // ...
        return false;  // false = keep running, true = remove task
    }
);

// Remove task later
WppTaskQueue::requestToRemoveTask(taskId);

// One-time task (returns true)
WppTaskQueue::addTask(0, [](WppClient& client, void* ctx) {
    // Execute once immediately
    return true;  // Remove after execution
});
```

---

## Data Types

### Common Types

```cpp
using ID_T = uint16_t;                    // Resource/Object ID
using OBJ_ID = ID_T;                      // Object ID
using OBJ_INST_ID_T = uint16_t;          // Instance ID
using STRING_T = std::string;             // String type
using INT_T = int64_t;                    // Integer type
using FLOAT_T = double;                   // Float type
using BOOL_T = bool;                      // Boolean type
using TIME_T = time_t;                    // Time type
using OPAQUE_T = std::vector<uint8_t>;   // Opaque data type
using EXECUTE_T = ExecuteCallback;        // Execute handler type

// Object link
struct OBJ_LINK_T {
    OBJ_ID objId;
    OBJ_INST_ID_T instId;
};
```

### DataLink Structure

```cpp
struct DataLink {
    OBJ_LINK_T object;        // Object and instance
    std::vector<ID_T> resources;  // Resources to send

    // Example
    DataLink link = {
        {OBJ_ID::LOCATION, 0},           // Object Location, instance 0
        {Location::LATITUDE_0, Location::LONGITUDE_1}  // Resources
    };
};
```

---

## Helper Classes (OpenWRT)

### OpenWrtDeviceInfo

```cpp
class OpenWrtDeviceInfo {
public:
    static std::string getManufacturer();
    static std::string getModelNumber();
    static std::string getSerialNumber();
    static std::string getFirmwareVersion();
    static std::string getHardwareVersion();

    static std::vector<int> getAvailablePowerSources();
    static int getPowerSourceVoltage(int source);
    static int getPowerSourceCurrent(int source);

    static int getMemoryFree();
    static int getMemoryTotal();

    static bool performReboot();
};
```

### OpenWrtLocationInfo

```cpp
class OpenWrtLocationInfo {
public:
    static double getLatitude();
    static double getLongitude();
    static double getAltitude();
    static double getRadius();
    static time_t getTimestamp();
    static double getSpeed();

    static bool isLocationAvailable();
    static bool getAllLocation(double& lat, double& lon, double& alt);
};
```

### OpenWrtConnectivityInfo

```cpp
class OpenWrtConnectivityInfo {
public:
    static int getNetworkBearer();
    static std::vector<int> getAvailableNetworkBearers();

    static int getRadioSignalStrength();
    static int getLinkQuality();
    static int getLinkUtilization();

    static std::vector<std::string> getIpAddresses();
    static std::vector<std::string> getRouterIpAddresses();
    static std::vector<std::string> getAPN();

    static int getCellId();
    static int getSMNC();
    static int getSMCC();
};
```

---

## Logging API

### WppLogs

```cpp
// Log macros
#define WPP_LOGD(tag, fmt, ...)  // Debug
#define WPP_LOGI(tag, fmt, ...)  // Info
#define WPP_LOGW(tag, fmt, ...)  // Warning
#define WPP_LOGE(tag, fmt, ...)  // Error

// Usage
#define TAG "MyObject"

WPP_LOGD(TAG, "Debug message: value=%d", value);
WPP_LOGI(TAG, "Info message: %s", str.c_str());
WPP_LOGW(TAG, "Warning: resource %d not found", resId);
WPP_LOGE(TAG, "Error occurred: %s", error.c_str());
```

---

## Callback Types

### Execute Callback

```cpp
using ExecuteCallback = std::function<bool(Instance&, ID_T, const OPAQUE_T&)>;

// Example
bool executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Reboot requested");
    system("reboot");
    return true;  // Success
}

// Set callback
instance->setExecute(REBOOT_RESOURCE, executeReboot);

// Or lambda
instance->setExecute(REBOOT_RESOURCE,
    [](Instance& inst, ID_T resId, const OPAQUE_T& data) {
        return performReboot();
    }
);
```

### Task Callback

```cpp
using TaskCallback = std::function<bool(WppClient&, void*)>;

// Example
bool updateTask(WppClient& client, void* ctx) {
    // Perform update
    return false;  // Keep running
}

// Add task
WppTaskQueue::addTask(60, updateTask);
```

---

## Error Handling

### Return Values

Most methods return `bool`:
- `true` = Success
- `false` = Failure

```cpp
if (client->connect()) {
    WPP_LOGI(TAG, "Connected successfully");
} else {
    WPP_LOGE(TAG, "Connection failed");
}
```

### Exception Safety

The client is **exception-safe**:
- No exceptions thrown from API
- All errors returned via return values
- Safe to use in embedded environments

---

## Best Practices

### 1. Always Check Return Values

```cpp
// Good
if (!client->connect()) {
    handleError();
    return;
}

// Bad
client->connect();  // Ignores errors!
```

### 2. Use RAII for Cleanup

```cpp
class MyClient {
    WppClient* client;
public:
    MyClient() : client(new WppClient("endpoint")) {}
    ~MyClient() { delete client; }
};
```

### 3. Batch Notifications

```cpp
// Good - batch notifications
instance->set<FLOAT_T>(RES_1, val1);
instance->set<FLOAT_T>(RES_2, val2);
instance->set<FLOAT_T>(RES_3, val3);
instance->notifyResChanged({RES_1, RES_2, RES_3});

// Less efficient - multiple notifications
instance->set<FLOAT_T>(RES_1, val1);
instance->notifyResChanged(RES_1);
instance->set<FLOAT_T>(RES_2, val2);
instance->notifyResChanged(RES_2);
```

### 4. Use Appropriate Log Levels

```cpp
WPP_LOGD(TAG, "Detailed debug info");    // Development only
WPP_LOGI(TAG, "Normal operation");        // Production
WPP_LOGW(TAG, "Warning condition");       // Production
WPP_LOGE(TAG, "Error occurred");          // Always
```

---

## Complete Example

```cpp
#include "WppClient.h"
#include "o_6_location/Location.h"

#define TAG "MyApp"

int main() {
    // Create client
    WppClient* client = new WppClient("my-device", 56830);

    // Register and initialize objects
    client->registry().registerObj(Location::object(*client));
    Location* loc = Location::createInst(*client);

    // Set initial values
    loc->set<FLOAT_T>(Location::LATITUDE_0, 40.7128);
    loc->set<FLOAT_T>(Location::LONGITUDE_1, -74.0060);

    // Add periodic update task
    WppTaskQueue::addTask(60, [loc](WppClient& client, void* ctx) {
        loc->updateLocation();
        WPP_LOGI(TAG, "Location updated");
        return false;  // Continue running
    });

    // Transfer ownership
    client->giveOwnership();

    // Connect
    if (!client->connect()) {
        WPP_LOGE(TAG, "Failed to connect");
        return 1;
    }

    // Main loop
    bool running = true;
    while (running) {
        client->process(1000);

        // Check for exit condition
        // ...
    }

    // Cleanup
    client->disconnect();
    delete client;

    return 0;
}
```

---

## See Also

- [Implementation Guide](IMPLEMENTATION_GUIDE.md) - Detailed implementation
- [Architecture Overview](ARCHITECTURE.md) - System design
- [Location Object Documentation](LOCATION_OBJECT.md) - Location API details
