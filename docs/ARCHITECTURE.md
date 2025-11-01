# Architecture Overview

System architecture and design of the Friendly LwM2M Client.

---

## System Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                     LwM2M Client Application                     │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌──────────────┐  ┌──────────────┐  ┌─────────────────────┐   │
│  │   Examples   │  │  Custom App  │  │  Service Daemon     │   │
│  └──────┬───────┘  └──────┬───────┘  └──────────┬──────────┘   │
│         └──────────────────┴───────────────────┬─┘              │
│                                                 │                │
│  ┌──────────────────────────────────────────────▼──────────┐   │
│  │                  WPP Client Library                      │   │
│  ├──────────────────────────────────────────────────────────┤   │
│  │  WppClient  │  WppRegistry  │  WppTaskQueue  │ WppLogs  │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                 │                │
│  ┌──────────────────────────────────────────────▼──────────┐   │
│  │                   Object Registry                        │   │
│  ├──────────────────────────────────────────────────────────┤   │
│  │  Device  │  Location  │  Connectivity  │  Custom Objects│   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                 │                │
│  ┌──────────────────────────────────────────────▼──────────┐   │
│  │                 Wakaama LwM2M Stack                      │   │
│  ├──────────────────────────────────────────────────────────┤   │
│  │  CoAP  │  DTLS  │  Observe  │  Bootstrap  │  FOTA       │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                 │                │
└─────────────────────────────────────────────────┼────────────────┘
                                                  │
                         ┌────────────────────────▼──────────┐
                         │      Network Transport (UDP)       │
                         └────────────────────────────────────┘
                                                  │
                         ┌────────────────────────▼──────────┐
                         │         LwM2M Server               │
                         └────────────────────────────────────┘
```

---

## Core Components

### 1. WppClient

**Purpose:** Main client coordinator

**Responsibilities:**
- Initialize Wakaama stack
- Manage object registry
- Handle server connections
- Coordinate lifecycle

**Key Methods:**
```cpp
class WppClient {
public:
    bool connect();
    bool disconnect();
    void process(int timeout_ms);
    WppRegistry& registry();
    bool send(const DataLink& data);
    void giveOwnership();
};
```

### 2. WppRegistry

**Purpose:** Object management and discovery

**Responsibilities:**
- Register/deregister objects
- Provide object accessors
- Manage object lifecycle
- Object discovery

**Key Methods:**
```cpp
class WppRegistry {
public:
    bool registerObj(Object& object);
    bool deregisterObj(Object& object);
    Object* object(OBJ_ID objId);
    std::vector<Object*>& objects();

    // Object accessors
    Object& device();
    Object& location();
    Object& connectivityMonitoring();
};
```

### 3. Object Hierarchy

```
┌─────────────────┐
│     Object      │  (Abstract base)
│                 │
│ + getObjectID() │
│ + instance()    │
│ + createInst()  │
└────────┬────────┘
         │
         ├── ObjectImpl<T>  (Template implementation)
         │
         └── Manages instances
                    │
         ┌──────────▼─────────┐
         │      Instance      │  (Abstract base)
         │                    │
         │ + resource()       │
         │ + set<T>()         │
         │ + get<T>()         │
         │ + notifyResChanged()│
         └──────────┬─────────┘
                    │
         ┌──────────▼────────┐
         │   Device          │
         │   Location        │
         │   Connectivity... │
         │   (Concrete impls)│
         └───────────────────┘
```

### 4. WppTaskQueue

**Purpose:** Asynchronous task scheduling

**Responsibilities:**
- Schedule periodic tasks
- Execute callbacks
- Resource cleanup

**Usage:**
```cpp
// Add periodic task (returns every 60s)
task_id_t id = WppTaskQueue::addTask(60, [](WppClient& client, void* ctx) {
    // Update logic
    return false;  // false = keep running, true = remove
});

// Remove task
WppTaskQueue::requestToRemoveTask(id);
```

---

## Data Flow

### Registration Flow

```
┌──────────┐     ┌──────────┐     ┌───────────┐     ┌──────────┐
│  Client  │────►│ Registry │────►│ Wakaama   │────►│  Server  │
└──────────┘     └──────────┘     └───────────┘     └──────────┘
     │                │                  │                 │
     │ registerObj()  │                  │                 │
     ├───────────────►│                  │                 │
     │                │ lwm2m_add_object │                 │
     │                ├─────────────────►│                 │
     │                │                  │                 │
     │ connect()      │                  │                 │
     ├───────────────────────────────────►│                 │
     │                │                  │ REGISTER (CoAP) │
     │                │                  ├────────────────►│
     │                │                  │  2.01 Created   │
     │                │                  │◄────────────────┤
     │  Connected!    │                  │                 │
     │◄───────────────────────────────────┤                 │
```

### Resource Read Flow

```
┌──────────┐     ┌───────────┐     ┌──────────┐     ┌──────────┐
│  Server  │────►│  Wakaama  │────►│ Instance │────►│ Resource │
└──────────┘     └───────────┘     └──────────┘     └──────────┘
     │                 │                  │                │
     │ READ /6/0/0     │                  │                │
     ├────────────────►│                  │                │
     │                 │ read_callback()  │                │
     │                 ├─────────────────►│                │
     │                 │                  │ get<FLOAT_T>(0)│
     │                 │                  ├───────────────►│
     │                 │                  │  Returns value │
     │                 │                  │◄───────────────┤
     │                 │  value           │                │
     │                 │◄─────────────────┤                │
     │  2.05 Content   │                  │                │
     │◄────────────────┤                  │                │
```

### Observe Flow

```
┌──────────┐     ┌───────────┐     ┌──────────┐     ┌────────────┐
│  Server  │     │  Wakaama  │     │ Instance │     │ TaskQueue  │
└──────────┘     └───────────┘     └──────────┘     └────────────┘
     │                 │                  │                 │
     │ OBSERVE /6/0/0  │                  │                 │
     ├────────────────►│                  │                 │
     │  2.05 Content   │                  │                 │
     │◄────────────────┤                  │                 │
     │  (with observe) │                  │                 │
     │                 │                  │                 │
     │                 │                  │ Periodic update │
     │                 │                  │◄────────────────┤
     │                 │                  │                 │
     │                 │  notifyResChanged()               │
     │                 │◄─────────────────┤                 │
     │ NOTIFY 2.05     │                  │                 │
     │◄────────────────┤                  │                 │
```

---

## Object Lifecycle

### Creation Sequence

```cpp
// 1. Application starts
int main() {
    // 2. Create WppClient
    WppClient* client = new WppClient(...);

    // 3. Register objects
    client->registry().registerObj(Location::object(*client));

    // 4. Create instances
    Location::createInst(*client);

    // 5. Initialize resources (automatic in createInst)
    // - resourcesCreate() defines resources
    // - resourcesInit() sets initial values

    // 6. Give ownership to registry
    client->giveOwnership();

    // 7. Main loop
    while(running) {
        client->process(1000);
    }
}
```

### Instance Creation Details

```cpp
Location* Location::createInst(WppClient& ctx, ID_T instId) {
    // 1. Get object from registry
    Instance* inst = ctx.registry().location().createInstance(instId);

    // 2. Constructor called
    //    Location::Location(...) {
    //        resourcesCreate();  // Define resources
    //        resourcesInit();     // Set initial values
    //    }

    // 3. Return typed pointer
    return static_cast<Location*>(inst);
}
```

---

## Resource Management

### Resource Definition

```cpp
void Location::resourcesCreate() {
    std::vector<Resource> resources = {
        {LATITUDE_0,   // Resource ID
         ItemOp(ItemOp::READ),              // Operations
         IS_SINGLE::SINGLE,                 // Single/Multiple
         IS_MANDATORY::MANDATORY,           // Mandatory/Optional
         TYPE_ID::FLOAT},                   // Data type
        // ... more resources
    };
    setupResources(std::move(resources));
}
```

### Resource Access

```cpp
// Write
resource(LATITUDE_0)->set<FLOAT_T>(40.7128);

// Read
float lat = resource(LATITUDE_0)->get<FLOAT_T>();

// Multiple instance
resource(IP_ADDRESSES_4)->set<STRING_T>("192.168.1.1", 0);  // Index 0
resource(IP_ADDRESSES_4)->set<STRING_T>("10.0.0.1", 1);     // Index 1
```

---

## Threading Model

### Single-Threaded Design

The client uses a **single-threaded, event-driven** architecture:

```cpp
// Main loop
while (running) {
    // Process LwM2M events (non-blocking)
    client->process(1000);  // 1000ms timeout

    // Task queue runs in same thread
    // Tasks execute during process()
}
```

### Task Execution

```cpp
WppTaskQueue::addTask(interval_sec, callback);

// Tasks execute in main thread during process()
// No synchronization needed for resource access
```

**Benefits:**
- Simple, predictable
- No race conditions
- Low overhead

**Limitations:**
- Long-running tasks block
- No parallel processing

---

## Memory Management

### Object Ownership

```cpp
// Objects owned by registry
WppRegistry::WppRegistry(...) {
    _objects.push_back(new ObjectImpl<Location>(...));
    // Registry owns objects
}

WppRegistry::~WppRegistry() {
    for (auto obj : _objects) {
        delete obj;  // Cleanup
    }
}
```

### Instance Ownership

```cpp
// Instances owned by objects
Object::~Object() {
    // Wakaama manages instance list
    // Instances deleted when object deleted
}
```

### Resource Cleanup

```cpp
Instance::~Instance() override {
    // Resources cleaned up automatically
    // Task queue tasks removed
    WppTaskQueue::requestToRemoveTask(_taskId);
}
```

---

## OpenWRT Integration Architecture

```
┌─────────────────────────────────────────────────────┐
│              LwM2M Client Process                    │
├─────────────────────────────────────────────────────┤
│                                                       │
│  ┌────────────────┐         ┌──────────────────┐    │
│  │ Location Object│────────►│ OpenWrtLocInfo   │    │
│  └────────────────┘         └────────┬─────────┘    │
│                                       │              │
│  ┌────────────────┐         ┌────────▼─────────┐    │
│  │ Device Object  │────────►│ OpenWrtDeviceInfo│    │
│  └────────────────┘         └────────┬─────────┘    │
│                                       │              │
│  ┌────────────────┐         ┌────────▼─────────┐    │
│  │ Connectivity   │────────►│ OpenWrtConnInfo  │    │
│  └────────────────┘         └────────┬─────────┘    │
│                                       │              │
└───────────────────────────────────────┼──────────────┘
                                        │
        ┌───────────────────────────────▼──────────────┐
        │           OpenWRT System                      │
        ├───────────────────────────────────────────────┤
        │                                               │
        │  /proc/meminfo  /sys/class/net/  /etc/config/│
        │  /proc/uptime   /proc/cpuinfo    uci commands│
        │  /proc/loadavg  gpsd daemon      Starlink API│
        │                                               │
        └───────────────────────────────────────────────┘
```

---

## Extension Points

### Adding New Objects

1. **Create object files** in `wpp/registry/objects/o_<ID>_<name>/`
2. **Add to ObjectID.h** enum
3. **Register in WppRegistry**
4. **Enable in wpp_config.cmake**
5. **Initialize in examples/**

See [IMPLEMENTATION_GUIDE.md](IMPLEMENTATION_GUIDE.md) for details.

### Custom Resource Types

```cpp
// Define custom type
struct GpsCoordinate {
    double latitude;
    double longitude;
    double altitude;
};

// Serialize/deserialize
void toOpaque(const GpsCoordinate& coord, OPAQUE_T& data);
void fromOpaque(const OPAQUE_T& data, GpsCoordinate& coord);
```

### Platform Abstraction

```cpp
#ifdef OPENWRT_BUILD
    // OpenWRT implementation
    #include "OpenWrtLocationInfo.h"
#elif defined(LINUX_BUILD)
    // Linux implementation
    #include "LinuxLocationInfo.h"
#else
    // Generic implementation
    #include "GenericLocationInfo.h"
#endif
```

---

## Performance Characteristics

### Resource Usage

| Component | Memory | CPU (idle) | CPU (active) |
|-----------|--------|------------|--------------|
| Base client | ~500 KB | 0.1% | 1-2% |
| Per object | ~200 bytes | - | - |
| Per instance | ~500 bytes | - | - |
| Per resource | ~50 bytes | - | - |
| Task queue | ~100 bytes/task | 0.01% | 0.1% |

### Scalability

- **Objects:** Tested with 15+ objects
- **Instances:** 100+ instances per object
- **Resources:** 100+ resources per instance
- **Observations:** 50+ concurrent observations
- **Tasks:** 20+ periodic tasks

---

## Design Patterns

### 1. Template Pattern (ObjectImpl)

```cpp
template<typename T>
class ObjectImpl : public Object {
    // Provides common object functionality
    // T = concrete instance type (e.g., Location)
};
```

### 2. Factory Pattern (createInst)

```cpp
static Location* createInst(WppClient& ctx, ID_T instId);
// Encapsulates instance creation logic
```

### 3. Observer Pattern (Observations)

```cpp
// Server observes resources
// Client notifies on changes
notifyResChanged(RESOURCE_ID);
```

### 4. Strategy Pattern (Platform Helpers)

```cpp
#ifdef OPENWRT_BUILD
    OpenWrtLocationInfo::getLatitude();
#else
    DefaultLocationInfo::getLatitude();
#endif
```

---

## Security Architecture

### DTLS Integration

```
┌─────────────┐
│   Client    │
└──────┬──────┘
       │ CoAP over DTLS
       │
┌──────▼──────┐
│   Wakaama   │
│  DTLS Layer │
└──────┬──────┘
       │ Encrypted
       │
┌──────▼──────┐
│  UDP Socket │
└─────────────┘
```

### Credential Management

```cpp
// PSK mode
security->set<OPAQUE_T>(PUBLIC_KEY_OR_IDENTITY_3, identity);
security->set<OPAQUE_T>(SECRET_KEY_5, psk);

// Certificate mode
security->set<OPAQUE_T>(PUBLIC_KEY_OR_IDENTITY_3, certificate);
security->set<OPAQUE_T>(SECRET_KEY_5, private_key);
```

---

## Future Enhancements

### Planned Architecture Changes

1. **Multi-threading support**
   - Background task execution
   - Non-blocking I/O

2. **WebSocket transport**
   - Alternative to UDP
   - Better NAT traversal

3. **MQTT bridge**
   - Publish observations to MQTT
   - Subscribe to commands

4. **Plugin system**
   - Dynamic object loading
   - Runtime object registration

---

## References

- [Wakaama Architecture](https://github.com/eclipse/wakaama/wiki/Client-Architecture)
- [LwM2M Technical Specification](https://www.openmobilealliance.org/release/LightweightM2M/)
- [CoAP RFC 7252](https://www.rfc-editor.org/rfc/rfc7252)
- [DTLS 1.2 RFC 6347](https://www.rfc-editor.org/rfc/rfc6347)
