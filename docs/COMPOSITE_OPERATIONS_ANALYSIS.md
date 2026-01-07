# LwM2M Composite Operations - Implementation Analysis

## Executive Summary

This document provides a comprehensive analysis of OMA LwM2M Composite Operations and the implementation requirements for full support in the Friendly LwM2M Client. Composite operations (READ-COMPOSITE, WRITE-COMPOSITE, OBSERVE-COMPOSITE) were introduced in LwM2M 1.1 to enable efficient multi-resource operations in a single request/response cycle.

**Current Status**: Composite operations are **NOT implemented** in this open-source version. The README.md explicitly states they are "commercially available."

---

## Table of Contents

1. [What are Composite Operations?](#what-are-composite-operations)
2. [Current Implementation Status](#current-implementation-status)
3. [Technical Specifications](#technical-specifications)
4. [Implementation Gaps](#implementation-gaps)
5. [Implementation Requirements](#implementation-requirements)
6. [Estimated Complexity](#estimated-complexity)
7. [References](#references)

---

## What are Composite Operations?

### Overview

Composite operations extend basic LwM2M READ, WRITE, and OBSERVE operations to allow **aggregation of operations on multiple different objects, object instances, and resources** in a single request, sending all acquired parameters in one payload.

### Key Features

- **Multi-Target Operations**: Address multiple resources across different objects in one request
- **Efficient Bandwidth Usage**: Reduce network overhead by combining multiple operations
- **Atomic Semantics**: Operations on multiple targets treated as a single transaction
- **SenML Format**: Uses SenML JSON or SenML CBOR for payload encoding

### LwM2M Version Support

- **LwM2M 1.0**: Does NOT support composite operations
- **LwM2M 1.1**: Introduced READ-COMPOSITE, WRITE-COMPOSITE, OBSERVE-COMPOSITE
- **LwM2M 1.2**: Enhanced composite operations with additional features

### Composite Operation Types

#### 1. READ-COMPOSITE

**Purpose**: Read values from multiple resources/objects/instances in a single request

**CoAP Method**: **FETCH** (RFC 8132)

**URI**: Typically `/` (root) with targets specified in request payload

**Request Payload Example** (SenML JSON):
```json
[
  {"n": "/3/0/0"},
  {"n": "/3/0/1"},
  {"n": "/4/0/0"},
  {"n": "/6/0/0"}
]
```

**Response Payload Example** (SenML JSON):
```json
[
  {"n": "/3/0/0", "vs": "Friendly Device"},
  {"n": "/3/0/1", "vs": "Model-X"},
  {"n": "/4/0/0", "v": 1},
  {"n": "/6/0/0", "v": 37.422}
]
```

**Success Response**: `2.05 Content`

**Error Responses**:
- `4.00 Bad Request` - Invalid payload format
- `4.01 Unauthorized` - Missing read permissions
- `4.04 Not Found` - One or more targets don't exist
- `4.06 Not Acceptable` - Unsupported content format

#### 2. WRITE-COMPOSITE

**Purpose**: Write values to multiple resources/objects/instances in a single request

**CoAP Method**: **iPATCH** (RFC 8132)

**URI**: Typically `/` (root) with targets specified in request payload

**Request Payload Example** (SenML JSON):
```json
[
  {"n": "/3/0/13", "v": 1699564800},
  {"n": "/4/0/8", "v": 80},
  {"n": "/6/0/0", "v": 37.422},
  {"n": "/6/0/1", "v": -122.084}
]
```

**Success Response**: `2.04 Changed`

**Error Responses**:
- `4.00 Bad Request` - Invalid payload or write semantics
- `4.01 Unauthorized` - Missing write permissions
- `4.04 Not Found` - One or more targets don't exist
- `4.05 Method Not Allowed` - Read-only resource targeted

#### 3. OBSERVE-COMPOSITE

**Purpose**: Establish observation on multiple resources/objects/instances

**CoAP Method**: **FETCH** with `Observe` option

**Register Observation**: `FETCH` with `Observe: 0`

**Cancel Observation**: `FETCH` with `Observe: 1`

**Request Payload Example** (SenML JSON):
```json
[
  {"n": "/3/0/9"},
  {"n": "/4/0/2"},
  {"n": "/6/0/0"}
]
```

**Notification Payload**: Same format as READ-COMPOSITE response

**Success Response**: `2.05 Content` with `Observe` option

---

## Current Implementation Status

### Codebase Analysis Results

#### File: `/wpp/registry/data_model/instance/Instance.cpp`

**Lines 424-425** (READ operation):
```cpp
uint8_t Instance::readAsServer(lwm2m_server_t *server, int *numData, lwm2m_data_t **dataArray) {
    // TODO: Read-Composite Operation for now not supported
    if (numData == NULL || dataArray == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
    // ... regular READ implementation
}
```

**Lines 468-469** (WRITE operation):
```cpp
uint8_t Instance::writeAsServer(lwm2m_server_t *server, int numData, lwm2m_data_t *dataArray, lwm2m_write_type_t writeType) {
    // TODO: Write-Composite Operation for now not supported
    if (!numData) return COAP_204_CHANGED;
    // ... regular WRITE implementation
}
```

#### README.md - Line 123

> "Note that additional features such as transport layers (TCP/TLS, SMS binding, MQTT, and HTTP transport binding); **Composite operations**, and Security modes (Certificate and Certificate mode with EST) are commercially available."

### Current Capabilities

**Supported**:
- ✅ LwM2M 1.0 and 1.1 protocol version
- ✅ Standard READ, WRITE, EXECUTE operations
- ✅ OBSERVE and NOTIFY operations (single resource)
- ✅ SenML JSON payload format
- ✅ SenML CBOR payload format
- ✅ CBOR payload format
- ✅ JSON payload format
- ✅ TLV payload format

**NOT Supported**:
- ❌ READ-COMPOSITE operation
- ❌ WRITE-COMPOSITE operation
- ❌ OBSERVE-COMPOSITE operation
- ❌ CANCEL-OBSERVATION-COMPOSITE operation
- ❌ CoAP FETCH method handling
- ❌ CoAP iPATCH method handling
- ❌ Multi-target payload parsing
- ❌ Multi-target response aggregation

---

## Technical Specifications

### CoAP Methods

#### FETCH Method (RFC 8132)

**Method Code**: `0.05`

**Purpose**: Retrieve representation of resource identified by request payload

**Key Characteristics**:
- Request includes payload describing targets
- Safe method (doesn't modify resources)
- Idempotent
- Cacheable

**Usage in LwM2M**:
- READ-COMPOSITE: `FETCH /` with SenML payload listing targets
- OBSERVE-COMPOSITE: `FETCH /` with `Observe: 0` option

#### iPATCH Method (RFC 8132)

**Method Code**: `0.07`

**Purpose**: Apply partial modifications to resource

**Key Characteristics**:
- Request includes payload with values to update
- Not safe (modifies resources)
- Idempotent
- Not cacheable

**Usage in LwM2M**:
- WRITE-COMPOSITE: `iPATCH /` with SenML payload containing target-value pairs

### Content Formats

**Composite operations primarily use**:

| Format | Media Type | Content-Format ID |
|--------|-----------|-------------------|
| SenML JSON | application/senml+json | 110 |
| SenML CBOR | application/senml+cbor | 112 |

**Also supported**:
- CBOR (60)
- JSON (11543)
- TLV (11542)

### SenML Payload Structure

#### Resource Naming Convention

**Format**: `/{ObjectID}/{InstanceID}/{ResourceID}`

**Examples**:
- `/3/0/0` - Device Object, Instance 0, Manufacturer Resource
- `/4/0/2` - Connectivity Monitoring, Instance 0, Radio Signal Strength
- `/6/0/0` - Location, Instance 0, Latitude

#### SenML Record Fields

| Field | JSON Key | CBOR Key | Type | Description |
|-------|----------|----------|------|-------------|
| Base Name | bn | -2 | String | Base URI for all names in array |
| Base Time | bt | -3 | Number | Base timestamp for all records |
| Name | n | 0 | String | Resource URI (relative to bn) |
| Value | v | 2 | Number | Numeric value |
| String Value | vs | 3 | String | String value |
| Boolean Value | vb | 4 | Boolean | Boolean value |
| Data Value | vd | 8 | String | Base64 encoded opaque data |
| Time | t | 6 | Number | Timestamp (relative to bt) |

#### Example: READ-COMPOSITE Request

**SenML JSON**:
```json
[
  {"n": "/3/0/0"},
  {"n": "/3/0/1"},
  {"n": "/4/0/2"}
]
```

**SenML CBOR** (hex):
```
83 A1 00 66 2F 33 2F 30 2F 30 A1 00 66 2F 33 2F 30 2F 31 A1 00 66 2F 34 2F 30 2F 32
```

#### Example: WRITE-COMPOSITE Request

**SenML JSON**:
```json
[
  {"n": "/3/0/13", "v": 1699564800},
  {"n": "/4/0/8", "v": 80},
  {"n": "/6/0/0", "v": 37.422}
]
```

---

## Implementation Gaps

### 1. CoAP Layer - Method Handlers

**Missing**:
- ❌ FETCH method handler (0.05)
- ❌ iPATCH method handler (0.07)

**Location**: Wakaama core CoAP implementation

**Required Changes**:
- Add FETCH and iPATCH to CoAP method enumeration
- Implement method-specific request/response handling
- Route FETCH to composite read handler
- Route iPATCH to composite write handler

**Estimated Complexity**: **Medium** (Wakaama core modification)

---

### 2. Payload Processing - SenML Parser

**Missing**:
- ❌ SenML payload parser for multi-target extraction
- ❌ SenML payload generator for multi-target responses
- ❌ URI path extraction from SenML records
- ❌ Value extraction with type handling (v, vs, vb, vd)

**Location**: New module required

**Required Changes**:
- Create `SenMLCompositeParser` class
  - Parse SenML JSON array
  - Parse SenML CBOR array
  - Extract target URIs (Object/Instance/Resource IDs)
  - Extract values and data types
  - Validate payload structure
  - Handle base name/base time fields

**Estimated Complexity**: **Medium-High**

---

### 3. Request Routing - URI and Target Resolution

**Missing**:
- ❌ Root URI (`/`) composite request detection
- ❌ Multi-target URI extraction from payload
- ❌ Object/Instance/Resource ID parsing from URI strings
- ❌ Target validation and existence checking

**Location**: WppRegistry, Object, Instance classes

**Required Changes**:
- Modify request router to detect composite operations
- Implement URI parser: `/ObjectID/InstanceID/ResourceID` → IDs
- Batch target resolution
- Validate all targets exist before operation
- Return appropriate error if any target invalid

**Estimated Complexity**: **Medium**

---

### 4. Composite READ Implementation

**Missing**:
- ❌ `readCompositeAsServer()` method
- ❌ Iterate over multiple targets
- ❌ Collect values from different objects/instances/resources
- ❌ Aggregate into single SenML response
- ❌ Handle partial read failures

**Location**: Instance class, WppRegistry class

**Required Changes**:
- Add `Instance::readCompositeAsServer(vector<Target> targets)`
- For each target:
  - Resolve Object → Instance → Resource
  - Check read permissions (Access Control)
  - Read resource value
  - Add to response SenML array
- Handle errors:
  - If ANY target fails permission check → `4.01 Unauthorized`
  - If ANY target not found → `4.04 Not Found`
  - If all succeed → `2.05 Content` with SenML payload

**Estimated Complexity**: **Medium-High**

---

### 5. Composite WRITE Implementation

**Missing**:
- ❌ `writeCompositeAsServer()` method
- ❌ Iterate over multiple target-value pairs
- ❌ Validate all writes before applying any
- ❌ Apply writes atomically (all-or-nothing)
- ❌ Handle partial write failures

**Location**: Instance class, WppRegistry class

**Required Changes**:
- Add `Instance::writeCompositeAsServer(vector<TargetValue> operations)`
- **Phase 1 - Validation**:
  - For each target-value pair:
    - Resolve Object → Instance → Resource
    - Check write permissions (Access Control)
    - Check resource is writable
    - Validate value data type
  - If ANY validation fails → abort with error
- **Phase 2 - Execution**:
  - For each target-value pair:
    - Write value to resource
    - Trigger notifications
  - If all succeed → `2.04 Changed`
- **Atomicity Consideration**:
  - Implement rollback mechanism if any write fails
  - OR use two-phase commit pattern

**Estimated Complexity**: **High**

---

### 6. Composite OBSERVE Implementation

**Missing**:
- ❌ `observeCompositeAsServer()` method
- ❌ Multi-target observation registration
- ❌ Composite notification aggregation
- ❌ Observation token management for composite requests
- ❌ Cancel composite observation handling

**Location**: WppClient, Observer classes

**Required Changes**:
- Add `WppClient::observeCompositeAsServer(vector<Target> targets, token)`
- Create `CompositeObservation` class:
  - Store list of observed targets
  - Track observation token
  - Aggregate notifications from multiple resources
- Modify notification system:
  - When ANY observed resource changes:
    - Read ALL targets in composite observation
    - Build SenML notification payload
    - Send `2.05 Content` with Observe option
- Implement cancel:
  - `FETCH` with `Observe: 1` → remove all targets from observation

**Estimated Complexity**: **High**

---

### 7. Access Control Integration

**Missing**:
- ❌ Batch permission checking for multiple targets
- ❌ Per-target access control validation
- ❌ Composite operation permission aggregation

**Location**: Lwm2mAccessControl class, Instance class

**Required Changes**:
- Modify access control to batch-check permissions
- For composite READ:
  - Check READ permission on all targets
  - If ANY fails → `4.01 Unauthorized`
- For composite WRITE:
  - Check WRITE permission on all targets
  - If ANY fails → `4.01 Unauthorized`
- For composite OBSERVE:
  - Check READ permission on all targets
  - If ANY fails → `4.01 Unauthorized`

**Estimated Complexity**: **Low-Medium**

---

### 8. Error Handling and Response Codes

**Missing**:
- ❌ Composite-specific error responses
- ❌ Partial failure handling
- ❌ Detailed error reporting (which target failed)

**Location**: Instance class, Error handling utilities

**Required Changes**:
- Implement error aggregation:
  - If multiple targets fail → choose most severe error
  - Priority: `4.01` > `4.05` > `4.04` > `4.00`
- Consider detailed error responses:
  - Include failed target URIs in response payload
  - Use SenML format to indicate errors per target
  - Example: `{"n": "/3/0/99", "vs": "Not Found"}`

**Estimated Complexity**: **Low-Medium**

---

### 9. Testing and Validation

**Missing**:
- ❌ Unit tests for composite operations
- ❌ Integration tests with LwM2M server
- ❌ Interoperability tests (Leshan, AVSystem Coiote)
- ❌ Performance benchmarks

**Location**: tests/ directory

**Required Changes**:
- Create `CompositeOperationsTest.cpp`:
  - Test READ-COMPOSITE with various target combinations
  - Test WRITE-COMPOSITE with atomic semantics
  - Test OBSERVE-COMPOSITE with notifications
  - Test error conditions (missing targets, permission failures)
  - Test SenML payload parsing/generation
- Integration tests:
  - Test against Leshan server
  - Test against AVSystem Coiote
  - Validate interoperability

**Estimated Complexity**: **Medium**

---

## Implementation Requirements

### Architecture Changes

#### 1. New Classes/Modules

```
wpp/composite/
├── CompositeOperations.h         // Main composite operations handler
├── CompositeOperations.cpp
├── SenMLParser.h                 // SenML payload parser
├── SenMLParser.cpp
├── SenMLGenerator.h              // SenML payload generator
├── SenMLGenerator.cpp
├── CompositeObserver.h           // Composite observation manager
└── CompositeObserver.cpp
```

#### 2. Modified Classes

**Instance.h/cpp**:
- Add `readCompositeAsServer()`
- Add `writeCompositeAsServer()`

**Object.h/cpp**:
- Add composite operation routing

**WppRegistry.h/cpp**:
- Add multi-object composite operation coordinator

**WppClient.h/cpp**:
- Add `observeComposite()`
- Add `cancelObserveComposite()`

**Lwm2mAccessControl.h/cpp**:
- Add batch permission checking

#### 3. Wakaama Core Changes

**core/liblwm2m.h**:
- Add CoAP method codes:
  ```c
  #define COAP_FETCH  0x05
  #define COAP_IPATCH 0x07
  ```

**core/packet.c**:
- Add FETCH method handling
- Add iPATCH method handling

**core/management.c**:
- Route FETCH to composite read
- Route iPATCH to composite write

---

### Data Structures

#### Target Structure

```cpp
namespace wpp {

struct CompositeTarget {
    uint16_t objectId;
    uint16_t instanceId;
    uint16_t resourceId;

    enum TargetLevel {
        OBJECT_LEVEL,      // /ObjectID
        INSTANCE_LEVEL,    // /ObjectID/InstanceID
        RESOURCE_LEVEL     // /ObjectID/InstanceID/ResourceID
    } level;

    std::string toUri() const {
        if (level == OBJECT_LEVEL)
            return "/" + std::to_string(objectId);
        if (level == INSTANCE_LEVEL)
            return "/" + std::to_string(objectId) + "/" + std::to_string(instanceId);
        return "/" + std::to_string(objectId) + "/" + std::to_string(instanceId) + "/" + std::to_string(resourceId);
    }

    static CompositeTarget fromUri(const std::string& uri);
};

struct CompositeValue {
    CompositeTarget target;

    enum ValueType {
        VALUE_INT,
        VALUE_FLOAT,
        VALUE_STRING,
        VALUE_BOOL,
        VALUE_OPAQUE
    } type;

    union {
        int64_t intValue;
        double floatValue;
        bool boolValue;
    };
    std::string stringValue;
    std::vector<uint8_t> opaqueValue;
};

}
```

---

### API Design

#### CompositeOperations Class

```cpp
namespace wpp {

class CompositeOperations {
public:
    static CompositeOperations& instance();

    // READ-COMPOSITE
    uint8_t readComposite(
        lwm2m_context_t* context,
        lwm2m_server_t* server,
        const std::vector<CompositeTarget>& targets,
        std::string& responsePayload,
        ContentFormat format
    );

    // WRITE-COMPOSITE
    uint8_t writeComposite(
        lwm2m_context_t* context,
        lwm2m_server_t* server,
        const std::vector<CompositeValue>& operations,
        ContentFormat format
    );

    // OBSERVE-COMPOSITE
    uint8_t observeComposite(
        lwm2m_context_t* context,
        lwm2m_server_t* server,
        const std::vector<CompositeTarget>& targets,
        uint32_t observeToken,
        ContentFormat format
    );

    // CANCEL-OBSERVATION-COMPOSITE
    uint8_t cancelObserveComposite(
        lwm2m_context_t* context,
        uint32_t observeToken
    );

private:
    CompositeOperations() = default;

    // Helper methods
    bool validateTargets(const std::vector<CompositeTarget>& targets);
    bool checkPermissions(lwm2m_server_t* server, const std::vector<CompositeTarget>& targets, Operation op);
    std::vector<CompositeValue> readTargets(const std::vector<CompositeTarget>& targets);
    bool writeTargets(const std::vector<CompositeValue>& operations);
};

}
```

#### SenMLParser Class

```cpp
namespace wpp {

class SenMLParser {
public:
    // Parse READ-COMPOSITE request (targets only)
    static std::vector<CompositeTarget> parseReadRequest(
        const std::string& payload,
        ContentFormat format
    );

    // Parse WRITE-COMPOSITE request (target-value pairs)
    static std::vector<CompositeValue> parseWriteRequest(
        const std::string& payload,
        ContentFormat format
    );

    // Generate READ-COMPOSITE response
    static std::string generateReadResponse(
        const std::vector<CompositeValue>& values,
        ContentFormat format
    );

private:
    static std::vector<CompositeTarget> parseJsonTargets(const std::string& json);
    static std::vector<CompositeTarget> parseCborTargets(const std::vector<uint8_t>& cbor);
    static std::vector<CompositeValue> parseJsonValues(const std::string& json);
    static std::vector<CompositeValue> parseCborValues(const std::vector<uint8_t>& cbor);
};

}
```

---

### Configuration Changes

#### wpp_config.cmake

Add new composite operations flag:

```cmake
# Enable LwM2M Composite Operations (READ-COMPOSITE, WRITE-COMPOSITE, OBSERVE-COMPOSITE)
# Requires LwM2M 1.1 or higher
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_COMPOSITE_OPERATIONS)
```

---

### Build System Changes

#### wpp/CMakeLists.txt

Add composite operations source files:

```cmake
if(DEFINED LWM2M_SUPPORT_COMPOSITE_OPERATIONS)
    set(WPP_SOURCES ${WPP_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/composite/CompositeOperations.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/composite/SenMLParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/composite/SenMLGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/composite/CompositeObserver.cpp
    )
endif()
```

---

## Estimated Complexity

### Development Effort Breakdown

| Component | Complexity | Estimated Hours | Priority |
|-----------|-----------|-----------------|----------|
| CoAP FETCH/iPATCH method handlers | Medium | 16-24 | High |
| SenML Parser (JSON) | Medium | 20-30 | High |
| SenML Parser (CBOR) | Medium | 20-30 | Medium |
| SenML Generator | Medium | 16-24 | High |
| URI Target Parser | Low | 8-12 | High |
| READ-COMPOSITE Implementation | Medium-High | 30-40 | High |
| WRITE-COMPOSITE Implementation | High | 40-60 | High |
| OBSERVE-COMPOSITE Implementation | High | 50-70 | Medium |
| Access Control Integration | Low-Medium | 12-20 | Medium |
| Error Handling | Low-Medium | 12-16 | Medium |
| Unit Tests | Medium | 30-40 | High |
| Integration Tests | Medium | 20-30 | Medium |
| Documentation | Low | 8-12 | Low |
| **TOTAL** | | **282-408 hours** | |

**Estimated Total**: **35-51 working days** (assuming 8-hour days)

**Recommended Team**: 2-3 developers

**Timeline**: **2-3 months** for full implementation including testing

---

### Complexity Factors

#### High Complexity Items

1. **WRITE-COMPOSITE Atomicity**:
   - Challenge: Ensuring all-or-nothing semantics
   - Solution: Two-phase commit or rollback mechanism
   - Risk: Complex error recovery logic

2. **OBSERVE-COMPOSITE Notification Aggregation**:
   - Challenge: Aggregating notifications from multiple resources
   - Solution: Composite observation manager
   - Risk: Performance impact with many observations

3. **Wakaama Core Modifications**:
   - Challenge: Modifying upstream library
   - Solution: Fork or patch mechanism
   - Risk: Merge conflicts with upstream updates

#### Medium Complexity Items

1. **SenML Payload Processing**:
   - Challenge: Parsing/generating SenML JSON and CBOR
   - Solution: Use existing JSON/CBOR libraries
   - Risk: Performance with large payloads

2. **Multi-Object Coordination**:
   - Challenge: Coordinating operations across multiple objects
   - Solution: Registry-level coordinator
   - Risk: Tight coupling between components

#### Low Complexity Items

1. **Access Control Batch Checking**:
   - Challenge: Extending existing access control
   - Solution: Iterate and check each target
   - Risk: Minimal

2. **Error Response Handling**:
   - Challenge: Mapping errors to CoAP codes
   - Solution: Error priority mapping
   - Risk: Minimal

---

### Risk Assessment

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Wakaama core breaking changes | Medium | High | Minimize core modifications, use callbacks |
| Performance degradation | Low | Medium | Optimize SenML parsing, cache target resolution |
| Interoperability issues | Medium | High | Test with multiple LwM2M servers (Leshan, Coiote) |
| Incomplete atomicity for WRITE | High | Medium | Document limitations, implement rollback |
| Memory consumption increase | Low | Low | Use streaming parsers, limit composite size |

---

## Implementation Roadmap

### Phase 1: Foundation (Weeks 1-2)

**Goals**:
- Implement SenML JSON parser/generator
- Implement URI target parser
- Add CoAP FETCH/iPATCH stubs

**Deliverables**:
- `SenMLParser` class with JSON support
- `CompositeTarget` parsing from URI strings
- CoAP method codes defined

**Testing**:
- Unit tests for SenML parsing
- Unit tests for URI parsing

---

### Phase 2: READ-COMPOSITE (Weeks 3-4)

**Goals**:
- Implement READ-COMPOSITE operation
- Integrate with existing read infrastructure

**Deliverables**:
- `readCompositeAsServer()` method
- Multi-target read coordination
- READ-COMPOSITE response generation

**Testing**:
- Unit tests for composite read
- Integration test with Leshan server

---

### Phase 3: WRITE-COMPOSITE (Weeks 5-7)

**Goals**:
- Implement WRITE-COMPOSITE operation
- Implement validation and atomicity

**Deliverables**:
- `writeCompositeAsServer()` method
- Two-phase write validation
- Atomicity mechanism

**Testing**:
- Unit tests for composite write
- Test atomic semantics
- Integration test with Leshan server

---

### Phase 4: OBSERVE-COMPOSITE (Weeks 8-10)

**Goals**:
- Implement OBSERVE-COMPOSITE operation
- Implement notification aggregation

**Deliverables**:
- `CompositeObserver` class
- Multi-target observation tracking
- Composite notifications

**Testing**:
- Unit tests for composite observe
- Test notification aggregation
- Integration test with Leshan server

---

### Phase 5: Polish & Optimization (Weeks 11-12)

**Goals**:
- Add SenML CBOR support
- Optimize performance
- Complete documentation

**Deliverables**:
- SenML CBOR parser/generator
- Performance optimizations
- User documentation
- Developer guide

**Testing**:
- Performance benchmarks
- Interoperability testing
- Stress testing

---

## References

### OMA LwM2M Specifications

1. **OMA-TS-LightweightM2M_Core-V1_1-20180612-C**
   - LwM2M 1.1 Core Specification (introduced composite operations)
   - URL: https://www.openmobilealliance.org/release/LightweightM2M/V1_1-20180612-C/

2. **OMA-TS-LightweightM2M_Core-V1_2-20201110-A**
   - LwM2M 1.2 Core Specification
   - URL: https://www.openmobilealliance.org/release/LightweightM2M/V1_2-20201110-A/

3. **OMA-TS-LightweightM2M_Core-V1_2_2-20240613-A**
   - LwM2M 1.2.2 Core Specification (latest)
   - URL: https://www.openmobilealliance.org/release/LightweightM2M/V1_2_2-20240613-A/

### IETF RFCs

1. **RFC 8132: PATCH and FETCH Methods for the Constrained Application Protocol (CoAP)**
   - Defines CoAP FETCH and iPATCH methods
   - URL: https://datatracker.ietf.org/doc/html/rfc8132

2. **RFC 8428: Sensor Measurement Lists (SenML)**
   - Defines SenML data format
   - URL: https://datatracker.ietf.org/doc/html/rfc8428

3. **RFC 7252: The Constrained Application Protocol (CoAP)**
   - CoAP protocol specification
   - URL: https://datatracker.ietf.org/doc/html/rfc7252

### Third-Party Implementations

1. **Eclipse Leshan**
   - Java LwM2M server/client with composite operations support
   - GitHub: https://github.com/eclipse-leshan/leshan

2. **AVSystem Anjay**
   - C LwM2M client with LwM2M 1.1 support
   - GitHub: https://github.com/AVSystem/Anjay

3. **Zephyr LwM2M**
   - Zephyr RTOS LwM2M implementation
   - Docs: https://docs.zephyrproject.org/latest/connectivity/networking/api/lwm2m.html

### Additional Resources

1. **LwM2M 1.1 Protocol Overview & New Features** (AVSystem)
   - Presentation on LwM2M 1.1 features including composite operations
   - URL: https://www.slideshare.net/AVSystem/lightweight-m2m-11-lwm2m-11-protocol-overview-new-features

2. **White Paper – Lightweight M2M 1.1** (OMA SpecWorks)
   - Overview of LwM2M 1.1 enhancements
   - URL: https://omaspecworks.org/white-paper-lightweight-m2m-1-1/

3. **Complete Crash Course in Lightweight M2M (LwM2M)** (AVSystem)
   - Comprehensive LwM2M guide
   - URL: https://avsystem.com/crashcourse/lwm2m/

---

## Conclusion

Implementing composite operations in the Friendly LwM2M Client is a **significant undertaking** requiring:

1. **Architecture Changes**: New composite operations module with SenML parsing
2. **Core Modifications**: CoAP FETCH/iPATCH method support in Wakaama
3. **Multi-Object Coordination**: Registry-level operation orchestration
4. **Atomicity Guarantees**: Two-phase commit for WRITE-COMPOSITE
5. **Observation Management**: Composite notification aggregation

**Estimated Effort**: 35-51 working days (282-408 hours)

**Recommended Approach**: Phased implementation over 2-3 months with 2-3 developers

**Priority**:
- **High**: READ-COMPOSITE (most commonly used)
- **Medium**: WRITE-COMPOSITE, OBSERVE-COMPOSITE
- **Low**: Advanced features (CBOR optimization, performance tuning)

**Current Status**: ❌ **NOT IMPLEMENTED** - marked as commercially available feature

**Next Steps** (if implementation is desired):
1. Review and approve this analysis document
2. Allocate development resources (2-3 developers)
3. Set up development environment and testing infrastructure
4. Begin Phase 1 implementation (Foundation)
5. Establish testing strategy with Leshan/Coiote servers

---

**Document Version**: 1.0
**Last Updated**: 2025-11-01
**Author**: Claude AI Assistant
**Status**: Analysis Complete
