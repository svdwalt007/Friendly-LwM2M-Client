# Implementation Guide

Comprehensive guide for implementing and extending the Friendly LwM2M Client.

---

## Table of Contents

1. [Build System](#build-system)
2. [Object Development](#object-development)
3. [OpenWRT Cross-Compilation](#openwrt-cross-compilation)
4. [Adding Custom Objects](#adding-custom-objects)
5. [Testing](#testing)
6. [Deployment](#deployment)

---

## Build System

### CMake Structure

```
Friendly-LwM2M-Client/
├── CMakeLists.txt                 # Root CMake configuration
├── wpp/
│   ├── CMakeLists.txt            # WPP library build
│   ├── configs/
│   │   └── wpp_config.cmake       # Object and feature configuration
│   └── registry/
│       └── objects/
│           ├── CMakeLists.txt     # Auto-discovers object directories
│           └── */CMakeLists.txt   # Per-object build files
└── examples/
    └── CMakeLists.txt             # Example application build
```

### Build Targets

```bash
# All targets
make                    # Build everything
make lwm2m_client      # Build only the client executable
make wpp_static        # Build WPP library
make clean             # Clean build artifacts
```

### Build Options

```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..

# OpenWRT build
cmake -DOPENWRT_BUILD=ON ..

# Enable verbose output
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
make VERBOSE=1
```

---

## Object Development

### Object Lifecycle

1. **Define Object** - Create object specification (XML)
2. **Generate Code** - Use object_maker or write manually
3. **Implement Logic** - Add custom behavior
4. **Register Object** - Add to WppRegistry
5. **Initialize Object** - Create instances in examples/

### Creating a New LwM2M Object

#### Step 1: Define Object Structure

Create header file: `wpp/registry/objects/o_<ID>_<name>/<Name>.h`

```cpp
#ifndef WPP_O_<ID>_<NAME>_H
#define WPP_O_<ID>_<NAME>_H

#include "<Name>Config.h"
#include "<Name>Info.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class <Name> : public Instance {
public:
    // Resource IDs
    enum ID: ID_T {
        RESOURCE_0 = 0,
        RESOURCE_1 = 1,
        // ... more resources
    };

    <Name>(lwm2m_context_t &context, const OBJ_LINK_T &id);
    ~<Name>();

    static Object & object(WppClient &ctx);
    static <Name> * instance(WppClient &ctx, ID_T instId = ID_T_MAX_VAL);
    static <Name> * createInst(WppClient &ctx, ID_T instId = ID_T_MAX_VAL);
    static bool removeInst(WppClient &ctx, ID_T instId);

protected:
    void serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type,
                                  const ResLink &resLink) override;
    void userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) override;

private:
    void resourcesCreate();
    void resourcesInit();
};

} // namespace wpp

#endif
```

#### Step 2: Implement Object Logic

Create implementation file: `wpp/registry/objects/o_<ID>_<name>/<Name>.cpp`

```cpp
#include "o_<ID>_<name>/<Name>.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

#define TAG "<Name>"

namespace wpp {

<Name>::<Name>(lwm2m_context_t &context, const OBJ_LINK_T &id)
    : Instance(context, id) {
    resourcesCreate();
    resourcesInit();
}

<Name>::~<Name>() {
    // Cleanup
}

Object & <Name>::object(WppClient &ctx) {
    return ctx.registry().<objectName>();
}

<Name> * <Name>::instance(WppClient &ctx, ID_T instId) {
    Instance *inst = ctx.registry().<objectName>().instance(instId);
    if (!inst) return NULL;
    return static_cast<<Name>*>(inst);
}

<Name> * <Name>::createInst(WppClient &ctx, ID_T instId) {
    Instance *inst = ctx.registry().<objectName>().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<<Name>*>(inst);
}

bool <Name>::removeInst(WppClient &ctx, ID_T instId) {
    return ctx.registry().<objectName>().remove(instId);
}

void <Name>::serverOperationNotifier(Instance *securityInst,
                                      ItemOp::TYPE type,
                                      const ResLink &resLink) {
    operationNotify(*this, resLink, type);
}

void <Name>::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
    if (type == ItemOp::WRITE || type == ItemOp::DELETE)
        notifyResChanged(resLink.resId, resLink.resInstId);
}

void <Name>::resourcesCreate() {
    std::vector<Resource> resources = {
        {RESOURCE_0, ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,
         IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {RESOURCE_1, ItemOp(ItemOp::READ|ItemOp::WRITE), IS_SINGLE::SINGLE,
         IS_MANDATORY::OPTIONAL, TYPE_ID::INT},
        // ... more resources
    };
    setupResources(std::move(resources));
}

void <Name>::resourcesInit() {
    // Initialize resource values
    resource(RESOURCE_0)->set<STRING_T>("default value");
    resource(RESOURCE_1)->set<INT_T>(0);

    // Set execute handlers
    resource(EXECUTE_RES)->set<EXECUTE_T>(
        [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
            WPP_LOGI(TAG, "Execute called");
            return true;
        }
    );
}

} // namespace wpp
```

#### Step 3: Create Configuration Files

**<Name>Config.h:**
```cpp
#ifndef WPP_<NAME>_CONFIG_H
#define WPP_<NAME>_CONFIG_H

#ifdef OBJ_O_<ID>_<NAME>

// Optional resource configuration
#define RES_<ID>_0 1  // Enable resource 0
#define RES_<ID>_1 1  // Enable resource 1

#endif
#endif
```

**<Name>Info.h:**
```cpp
#ifndef WPP_<NAME>_INFO_H
#define WPP_<NAME>_INFO_H

#include "ObjectInfo.h"

#ifdef OBJ_O_<ID>_<NAME>

namespace wpp {

inline const ObjectInfo <NAME>_OBJ_INFO = {
    "<Name>",                    // Name
    OBJ_ID::<NAME>,              // Object ID
    "urn:oma:lwm2m:oma:<ID>:1.0", // URN
    {1,0},                       // Object version
    {1,0},                       // LwM2m version
    IS_SINGLE::SINGLE,           // Single instance
    IS_MANDATORY::OPTIONAL,      // Optional
    ItemOp(ItemOp::READ|ItemOp::WRITE|ItemOp::DISCOVER|ItemOp::EXECUTE),
};

} // namespace wpp

#endif
#endif
```

#### Step 4: Add to Registry

**wpp/registry/ObjectID.h:**
```cpp
enum OBJ_ID: ID_T {
    // ...
    #ifdef OBJ_O_<ID>_<NAME>
    <NAME> = <ID>,
    #endif
    // ...
};
```

**wpp/registry/WppRegistry.h:**
```cpp
// Include block
#ifdef OBJ_O_<ID>_<NAME>
#include "o_<ID>_<name>/<Name>.h"
#endif

// Prototype block
#ifdef OBJ_O_<ID>_<NAME>
Object & <objectName>();
#endif
```

**wpp/registry/WppRegistry.cpp:**
```cpp
// Constructor init block
#ifdef OBJ_O_<ID>_<NAME>
_objects.push_back(new ObjectImpl<<Name>>(_context, <NAME>_OBJ_INFO));
#endif

// Method block
#ifdef OBJ_O_<ID>_<NAME>
Object & WppRegistry::<objectName>() {
    return *object(OBJ_ID::<NAME>);
}
#endif
```

#### Step 5: Enable in Configuration

**wpp/configs/wpp_config.cmake:**
```cmake
# Include optional <Name> object in the build (Object ID: <ID>)
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_<ID>_<NAME>)
```

#### Step 6: Initialize in Application

**examples/objects.h:**
```cpp
#ifdef OBJ_O_<ID>_<NAME>
void <objectName>Init(WppClient &client);
#endif
```

**examples/objects.cpp:**
```cpp
#ifdef OBJ_O_<ID>_<NAME>
void <objectName>Init(WppClient &client) {
    client.registry().registerObj(<Name>::object(client));
    <Name>::createInst(client);

    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    Lwm2mAccessControl::create(<Name>::object(client),
                               Lwm2mAccessControl::ALL_OBJ_RIGHTS);
    #endif
}
#endif
```

**examples/main.cpp:**
```cpp
#ifdef OBJ_O_<ID>_<NAME>
cout << endl << "---- Initialization wpp <Name> ----" << endl;
<objectName>Init(*client);
#endif
```

---

## OpenWRT Cross-Compilation

### Setup Toolchain

```bash
# Download OpenWRT SDK for your platform
wget https://downloads.openwrt.org/releases/23.05.0/targets/mediatek/filogic/openwrt-sdk-23.05.0-mediatek-filogic_gcc-12.3.0_musl.Linux-x86_64.tar.xz

# Extract
tar xf openwrt-sdk-*.tar.xz
cd openwrt-sdk-*/

# Set environment variables
export STAGING_DIR=$(pwd)/staging_dir
export PATH=$STAGING_DIR/toolchain-*/bin:$PATH
```

### Create Toolchain File

Create `cmake/toolchain-openwrt.cmake`:

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR mips)

# Specify the cross compiler
set(CMAKE_C_COMPILER   mipsel-openwrt-linux-gcc)
set(CMAKE_CXX_COMPILER mipsel-openwrt-linux-g++)

# Search paths
set(CMAKE_FIND_ROOT_PATH ${STAGING_DIR}/target-mipsel_24kc_musl)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

### Build for OpenWRT

```bash
mkdir build-openwrt
cd build-openwrt

cmake \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-openwrt.cmake \
    -DOPENWRT_BUILD=ON \
    -DCMAKE_BUILD_TYPE=Release \
    ..

make -j$(nproc)
```

---

## Adding Custom Objects

### Example: Temperature Sensor Object

```cpp
// TemperatureSensor.h
class TemperatureSensor : public Instance {
public:
    enum ID: ID_T {
        SENSOR_VALUE_5700 = 5700,      // Current temperature
        MIN_MEASURED_5601 = 5601,      // Minimum value
        MAX_MEASURED_5602 = 5602,      // Maximum value
        RESET_5605 = 5605,             // Reset min/max
        SENSOR_UNITS_5701 = 5701,      // Units (Celsius)
    };

private:
    float _minTemp = 1000.0f;
    float _maxTemp = -1000.0f;

    void updateTemperature();
    EXECUTE_RESULT resetMinMax(Instance& inst, ID_T resId, const OPAQUE_T& data);
};

// TemperatureSensor.cpp
void TemperatureSensor::resourcesInit() {
    // Initialize values
    resource(SENSOR_VALUE_5700)->set<FLOAT_T>(25.0f);
    resource(MIN_MEASURED_5601)->set<FLOAT_T>(_minTemp);
    resource(MAX_MEASURED_5602)->set<FLOAT_T>(_maxTemp);
    resource(SENSOR_UNITS_5701)->set<STRING_T>("Cel");

    // Set reset handler
    resource(RESET_5605)->set<EXECUTE_T>(
        [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
            return resetMinMax(inst, resId, data);
        }
    );

    // Periodic temperature updates
    WppTaskQueue::addTask(10, [this](WppClient &client, void *ctx) {
        updateTemperature();
        return false; // Keep running
    });
}

void TemperatureSensor::updateTemperature() {
    #ifdef OPENWRT_BUILD
    // Read from hardware sensor
    float temp = readHardwareTemp();
    #else
    // Simulate temperature
    float temp = 20.0f + (rand() % 100) / 10.0f;
    #endif

    // Update current value
    resource(SENSOR_VALUE_5700)->set<FLOAT_T>(temp);
    notifyResChanged(SENSOR_VALUE_5700);

    // Update min/max
    if (temp < _minTemp) {
        _minTemp = temp;
        resource(MIN_MEASURED_5601)->set<FLOAT_T>(_minTemp);
        notifyResChanged(MIN_MEASURED_5601);
    }
    if (temp > _maxTemp) {
        _maxTemp = temp;
        resource(MAX_MEASURED_5602)->set<FLOAT_T>(_maxTemp);
        notifyResChanged(MAX_MEASURED_5602);
    }
}

EXECUTE_RESULT TemperatureSensor::resetMinMax(Instance& inst, ID_T resId,
                                               const OPAQUE_T& data) {
    _minTemp = resource(SENSOR_VALUE_5700)->get<FLOAT_T>();
    _maxTemp = _minTemp;

    resource(MIN_MEASURED_5601)->set<FLOAT_T>(_minTemp);
    resource(MAX_MEASURED_5602)->set<FLOAT_T>(_maxTemp);

    notifyResChanged(MIN_MEASURED_5601);
    notifyResChanged(MAX_MEASURED_5602);

    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
}
```

---

### Example: Starlink Terminal Object (ID 34600)

The Starlink Terminal object demonstrates a complex custom object with 70+ resources, multiple functional groups, gRPC integration, and execute resources.

```cpp
// StarlinkTerminal.h
class StarlinkTerminal : public Instance {
public:
    /* Resource IDs - Organized by functional groups */
    enum ID: ID_T {
        /* Device Information (0-9) */
        DEVICE_ID_0 = 0,                     // Serial number
        HARDWARE_VERSION_1 = 1,              // Hardware version
        SOFTWARE_VERSION_2 = 2,              // Firmware version
        CONNECTION_STATE_3 = 3,              // CONNECTED, SEARCHING, etc.
        UPTIME_4 = 4,                        // Seconds since reboot

        /* Network Performance (10-29) */
        DOWNLINK_THROUGHPUT_10 = 10,         // Download speed (bps)
        UPLINK_THROUGHPUT_11 = 11,           // Upload speed (bps)
        POP_PING_LATENCY_12 = 12,            // Latency (ms)
        POP_PING_DROP_RATE_13 = 13,          // Packet loss (0.0-1.0)
        SNR_15 = 15,                         // Signal-to-noise ratio (dB)

        /* Obstruction Monitoring (30-39) */
        FRACTION_OBSTRUCTED_30 = 30,         // Sky obstruction (0.0-1.0)
        CURRENTLY_OBSTRUCTED_31 = 31,        // Current obstruction status
        WEDGES_FRACTION_OBSTRUCTED_35 = 35,  // JSON array of 12 wedges

        /* Dish Alignment (40-49) */
        DIRECTION_AZIMUTH_40 = 40,           // Azimuth (0-360 degrees)
        DIRECTION_ELEVATION_41 = 41,         // Elevation (0-90 degrees)
        DISH_STOWED_42 = 42,                 // Stow state (RW)

        /* GPS Location (80-85) */
        GPS_READY_80 = 80,                   // GPS fix available
        LATITUDE_83 = 83,                    // Latitude (degrees)
        LONGITUDE_84 = 84,                   // Longitude (degrees)
        ALTITUDE_85 = 85,                    // Altitude (meters)

        /* Control Actions (100-107) */
        REBOOT_TERMINAL_100 = 100,           // Execute: Reboot
        START_SPEEDTEST_101 = 101,           // Execute: Speed test
        SPEEDTEST_STATUS_102 = 102,          // Speed test results (JSON)
        REFRESH_TELEMETRY_106 = 106,         // Execute: Refresh data
        GRPC_ENDPOINT_107 = 107,             // gRPC endpoint (RW)
    };

    /* Connection State Enumeration */
    enum ConnState: INT_T {
        STATE_CONNECTED = 1,
        STATE_SEARCHING = 3,
        STATE_OBSTRUCTED = 7,
        // ... more states
    };

private:
    std::string _grpcEndpoint = "192.168.100.1:9200";
    WppTaskQueue::task_id_t _telemetryTaskId;

    bool updateTelemetry();
    EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
    EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
};

// StarlinkTerminal.cpp - Key Implementation Patterns

void StarlinkTerminal::resourcesCreate() {
    std::vector<Resource> resources = {
        // Mandatory device info
        {DEVICE_ID_0,        ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,
         IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {CONNECTION_STATE_3, ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,
         IS_MANDATORY::MANDATORY, TYPE_ID::STRING},

        // Network performance
        {DOWNLINK_THROUGHPUT_10, ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,
         IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {UPLINK_THROUGHPUT_11,   ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,
         IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},

        // Read/Write resources
        {DISH_STOWED_42,     ItemOp(ItemOp::READ | ItemOp::WRITE),
         IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL, TYPE_ID::BOOL},
        {GRPC_ENDPOINT_107,  ItemOp(ItemOp::READ | ItemOp::WRITE),
         IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL, TYPE_ID::STRING},

        // Execute resources
        {REBOOT_TERMINAL_100,  ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,
         IS_MANDATORY::OPTIONAL, TYPE_ID::NONE},
        {START_SPEEDTEST_101,  ItemOp(ItemOp::EXECUTE), IS_SINGLE::SINGLE,
         IS_MANDATORY::OPTIONAL, TYPE_ID::NONE},
    };
    setupResources(std::move(resources));
}

void StarlinkTerminal::resourcesInit() {
    // Initialize device information
    resource(DEVICE_ID_0)->set<STRING_T>("UT01000000-00000000-00000001");
    resource(HARDWARE_VERSION_1)->set<STRING_T>("rev3_proto2");
    resource(CONNECTION_STATE_3)->set<STRING_T>("SEARCHING");

    // Initialize network metrics with defaults
    resource(DOWNLINK_THROUGHPUT_10)->set<FLOAT_T>(0.0f);
    resource(UPLINK_THROUGHPUT_11)->set<FLOAT_T>(0.0f);
    resource(POP_PING_LATENCY_12)->set<FLOAT_T>(0.0f);

    // Initialize GPS (if available)
    resource(GPS_READY_80)->set<BOOL_T>(false);
    resource(LATITUDE_83)->set<FLOAT_T>(0.0);
    resource(LONGITUDE_84)->set<FLOAT_T>(0.0);

    // Set gRPC endpoint
    resource(GRPC_ENDPOINT_107)->set<STRING_T>(_grpcEndpoint);

    // Set execute handlers
    resource(REBOOT_TERMINAL_100)->set<EXECUTE_T>(
        [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
            return executeReboot(inst, resId, data);
        }
    );

    resource(START_SPEEDTEST_101)->set<EXECUTE_T>(
        [this](Instance& inst, ID_T resId, const OPAQUE_T& data) {
            return executeSpeedTest(inst, resId, data);
        }
    );

    // Set up periodic telemetry updates (every 10 seconds)
    _telemetryTaskId = WppTaskQueue::addTask(10, [this](WppClient &client, void *ctx) {
        updateTelemetry();
        return false; // Keep running
    });
}

bool StarlinkTerminal::updateTelemetry() {
    #ifdef STARLINK_GRPC_ENABLED
    // Connect to Starlink dish via gRPC
    auto channel = grpc::CreateChannel(_grpcEndpoint,
                                       grpc::InsecureChannelCredentials());
    auto stub = SpaceX::API::Device::Device::NewStub(channel);

    // Request status
    grpc::ClientContext context;
    SpaceX::API::Device::Request request;
    request.mutable_get_status();
    SpaceX::API::Device::Response response;

    grpc::Status status = stub->Handle(&context, request, &response);

    if (status.ok() && response.has_dish_get_status()) {
        auto dishStatus = response.dish_get_status();

        // Update connection state
        std::string state = parseState(dishStatus.state());
        resource(CONNECTION_STATE_3)->set<STRING_T>(state);

        // Update network performance
        resource(DOWNLINK_THROUGHPUT_10)->set<FLOAT_T>(
            dishStatus.downlink_throughput_bps());
        resource(UPLINK_THROUGHPUT_11)->set<FLOAT_T>(
            dishStatus.uplink_throughput_bps());
        resource(POP_PING_LATENCY_12)->set<FLOAT_T>(
            dishStatus.pop_ping_latency_ms());

        // Update obstruction data
        resource(FRACTION_OBSTRUCTED_30)->set<FLOAT_T>(
            dishStatus.obstruction_stats().fraction_obstructed());
        resource(CURRENTLY_OBSTRUCTED_31)->set<BOOL_T>(
            dishStatus.obstruction_stats().currently_obstructed());

        // Update dish alignment
        resource(DIRECTION_AZIMUTH_40)->set<FLOAT_T>(
            dishStatus.boresight_azimuth_deg());
        resource(DIRECTION_ELEVATION_41)->set<FLOAT_T>(
            dishStatus.boresight_elevation_deg());

        // Update GPS location (if available)
        if (dishStatus.has_gps_stats()) {
            resource(GPS_READY_80)->set<BOOL_T>(dishStatus.gps_stats().gps_valid());
            resource(LATITUDE_83)->set<FLOAT_T>(dishStatus.gps_stats().latitude());
            resource(LONGITUDE_84)->set<FLOAT_T>(dishStatus.gps_stats().longitude());
            resource(ALTITUDE_85)->set<FLOAT_T>(dishStatus.gps_stats().altitude());
        }

        // Notify observers of changes
        notifyResChanged(CONNECTION_STATE_3);
        notifyResChanged(DOWNLINK_THROUGHPUT_10);
        notifyResChanged(UPLINK_THROUGHPUT_11);

        return true;
    }
    #endif

    return false;
}

EXECUTE_RESULT StarlinkTerminal::executeReboot(Instance& inst, ID_T resId,
                                                 const OPAQUE_T& data) {
    WPP_LOGI("StarlinkTerminal", "Executing reboot command");

    #ifdef STARLINK_GRPC_ENABLED
    // Send reboot command via gRPC
    auto channel = grpc::CreateChannel(_grpcEndpoint,
                                       grpc::InsecureChannelCredentials());
    auto stub = SpaceX::API::Device::Device::NewStub(channel);

    grpc::ClientContext context;
    SpaceX::API::Device::Request request;
    request.mutable_reboot();
    SpaceX::API::Device::Response response;

    grpc::Status status = stub->Handle(&context, request, &response);

    if (status.ok()) {
        return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
    }
    #endif

    return EXECUTE_RESULT::EXECUTE_RESULT_ERROR;
}

EXECUTE_RESULT StarlinkTerminal::executeSpeedTest(Instance& inst, ID_T resId,
                                                    const OPAQUE_T& data) {
    WPP_LOGI("StarlinkTerminal", "Starting speed test");

    // Update status to running
    resource(SPEEDTEST_STATUS_102)->set<STRING_T>("{\"status\":\"running\"}");
    notifyResChanged(SPEEDTEST_STATUS_102);

    #ifdef STARLINK_GRPC_ENABLED
    // Initiate speed test via gRPC (async)
    // Speed test results will be polled via updateTelemetry()
    #endif

    return EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS;
}

// Destructor - cleanup
StarlinkTerminal::~StarlinkTerminal() {
    // Remove periodic telemetry task
    if (_telemetryTaskId != WppTaskQueue::INVALID_TASK_ID) {
        WppTaskQueue::removeTask(_telemetryTaskId);
    }
}
```

**Key Implementation Patterns:**

1. **Resource Organization**: Group related resources (0-9: device info, 10-29: network, etc.)
2. **Execute Resources**: Use lambda handlers for control actions
3. **Periodic Updates**: WppTaskQueue for automatic telemetry refresh
4. **gRPC Integration**: Conditional compilation for platform-specific features
5. **Proper Cleanup**: Remove tasks in destructor
6. **Observer Notifications**: Call `notifyResChanged()` after updates

---

## Testing

### Unit Testing

```bash
# Build with tests
cmake -DBUILD_TESTS=ON ..
make
ctest --verbose
```

### Integration Testing

```bash
# Start Leshan server (Docker)
docker run -d -p 8080:8080 -p 5683:5683/udp --name leshan eclipselwm2m/leshan

# Run client
./lwm2m_client

# Open browser to Leshan UI
open http://localhost:8080
```

### Hardware Testing (OpenWRT)

```bash
# Deploy to device
scp lwm2m_client root@192.168.1.1:/usr/bin/

# SSH and run
ssh root@192.168.1.1
/usr/bin/lwm2m_client

# Monitor logs
logread -f | grep lwm2m
```

---

## Deployment

### Create OpenWRT Package

Create `Makefile`:

```makefile
include $(TOPDIR)/rules.mk

PKG_NAME:=lwm2m-client
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/lwm2m-client
  SECTION:=net
  CATEGORY:=Network
  TITLE:=LwM2M Client with Location Support
  DEPENDS:=+libstdcpp +libpthread
endef

define Package/lwm2m-client/description
  Friendly LwM2M Client with OpenWRT integration
endef

define Build/Compile
  $(MAKE) -C $(PKG_BUILD_DIR) \
    CC="$(TARGET_CC)" \
    CXX="$(TARGET_CXX)" \
    CFLAGS="$(TARGET_CFLAGS)" \
    LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/lwm2m-client/install
  $(INSTALL_DIR) $(1)/usr/bin
  $(INSTALL_BIN) $(PKG_BUILD_DIR)/lwm2m_client $(1)/usr/bin/
  $(INSTALL_DIR) $(1)/etc/init.d
  $(INSTALL_BIN) ./files/lwm2m.init $(1)/etc/init.d/lwm2m
  $(INSTALL_DIR) $(1)/etc/config
  $(INSTALL_CONF) ./files/lwm2m.config $(1)/etc/config/lwm2m
endef

$(eval $(call BuildPackage,lwm2m-client))
```

### Build Package

```bash
# In OpenWRT buildroot
make package/lwm2m-client/compile
```

---

## Best Practices

1. **Error Handling**
   - Always check return values
   - Use WPP_LOG* macros for logging
   - Provide meaningful error messages

2. **Resource Management**
   - Clean up in destructors
   - Remove task queue tasks
   - Free allocated memory

3. **Thread Safety**
   - Use WppClient mutex when needed
   - Avoid blocking operations in callbacks

4. **Performance**
   - Minimize memory allocations
   - Use task queue for periodic updates
   - Batch notifications when possible

5. **OpenWRT Integration**
   - Use conditional compilation (#ifdef OPENWRT_BUILD)
   - Provide fallbacks for non-OpenWRT builds
   - Test on actual hardware

---

**Next Steps:**
- See [Location Object Documentation](LOCATION_OBJECT.md) for a complete GPS implementation example
- See [Starlink Terminal Documentation](STARLINK_TERMINAL.md) for advanced gRPC integration and complex object patterns
