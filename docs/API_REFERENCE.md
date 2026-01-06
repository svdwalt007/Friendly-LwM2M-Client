# API Reference

Complete API reference for the Friendly LwM2M Client v1.1.

## Table of Contents

1. [Transport Layer](#transport-layer)
2. [Firmware Management](#firmware-management)
3. [Edge AI Inference](#edge-ai-inference)
4. [Platform Abstraction](#platform-abstraction)
5. [LwM2M Objects](#lwm2m-objects)
6. [Utility Classes](#utility-classes)
7. [Constants and Enums](#constants-and-enums)

---

## Transport Layer

### MqttTransport

MQTT transport binding implementation (OMA LwM2M v1.2.1 Section 8).

```cpp
#include "transport/mqtt_transport.h"

namespace lwm2m::transport {

class MqttTransport {
public:
    // Configuration structure
    struct Config {
        MqttServerConfig server;
        MqttCoseConfig security;
    };

    // Constructor/Destructor
    explicit MqttTransport(const Config& config);
    ~MqttTransport();

    // Connection management
    MqttResult connect();
    MqttResult disconnect(bool graceful = true);
    bool isConnected() const;
    MqttResult reconnect();

    // Registration interface
    MqttResult registerClient(const MqttRegistrationParams& params);
    MqttResult updateRegistration(
        uint32_t lifetime = 0,
        const std::string& binding = "",
        const std::vector<std::string>& objectLinks = {}
    );
    MqttResult deregister();

    // Bootstrap interface
    MqttResult bootstrapRequest();
    void onBootstrapWrite(BootstrapWriteCallback callback);
    void onBootstrapDiscover(BootstrapDiscoverCallback callback);
    void onBootstrapFinish(BootstrapFinishCallback callback);

    // Device Management callbacks
    void onRead(ReadCallback callback);
    void onWrite(WriteCallback callback);
    void onExecute(ExecuteCallback callback);
    void onCreate(CreateCallback callback);
    void onDelete(DeleteCallback callback);
    void onDiscover(DiscoverCallback callback);
    void onWriteAttributes(WriteAttributesCallback callback);
    void onReadComposite(ReadCompositeCallback callback);
    void onWriteComposite(WriteCompositeCallback callback);

    // Information Reporting
    void onObserve(ObserveCallback callback);
    void onCancelObserve(CancelObserveCallback callback);
    MqttResult sendNotification(
        const std::string& path,
        const std::vector<uint8_t>& data,
        ContentFormat format,
        MqttQoS qos = MqttQoS::AT_LEAST_ONCE
    );
    MqttResult sendData(
        const std::vector<std::string>& paths,
        const std::vector<uint8_t>& data,
        ContentFormat format
    );

    // Message processing
    int processMessages(int timeoutMs);

    // Configuration
    void setDefaultQoS(MqttQoS qos);
    void setLogLevel(MqttLogLevel level);
    void setLogCallback(LogCallback callback);
    void setTokenRefreshCallback(TokenRefreshCallback callback);

    // Statistics
    MqttStatistics getStatistics() const;
    void resetStatistics();
};

}  // namespace lwm2m::transport
```

#### Callback Types

```cpp
// Response function type
using RespondFunc = std::function<void(const LwM2MPayload&)>;

// Callbacks
using ReadCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using WriteCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using ExecuteCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using CreateCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using DeleteCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using DiscoverCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using WriteAttributesCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using ReadCompositeCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using WriteCompositeCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using ObserveCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using CancelObserveCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using BootstrapWriteCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using BootstrapDiscoverCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using BootstrapFinishCallback = std::function<void(const LwM2MPayload&, RespondFunc)>;
using LogCallback = std::function<void(MqttLogLevel, const std::string&)>;
using TokenRefreshCallback = std::function<std::string()>;
```

---

### BlockwiseTransfer

CoAP block-wise transfer handler (RFC 7959).

```cpp
#include "transport/blockwise_transfer.h"

namespace lwm2m::transport {

class BlockwiseTransfer {
public:
    // Configuration
    struct Config {
        size_t preferredBlockSize = 1024;
        bool enableBERT = true;
        size_t bertBlockSize = 8192;
        int retransmitCount = 5;
        int transferTimeoutSec = 600;
    };

    explicit BlockwiseTransfer(const Config& config = {});
    ~BlockwiseTransfer();

    // Transfer management
    std::string startTransfer(
        const std::string& resourcePath,
        const std::vector<uint8_t>& data,
        bool resumable = true
    );

    TransferResult resumeTransfer(
        const std::string& transferId,
        const std::string& etag
    );

    TransferResult receiveBlock(
        const std::string& transferId,
        uint32_t blockNum,
        const std::vector<uint8_t>& blockData,
        bool moreBlocks
    );

    std::vector<uint8_t> getNextBlock(
        const std::string& transferId,
        uint32_t blockNum,
        size_t blockSize
    );

    bool cancelTransfer(const std::string& transferId);

    // Configuration
    void setPreferredBlockSize(size_t size);
    void enableBERT(bool enabled);
    void setBERTBlockSize(size_t size);

    // Statistics
    TransferStatistics getStats(const std::string& transferId) const;
    std::vector<std::string> getActiveTransfers() const;
};

}  // namespace lwm2m::transport
```

---

## Firmware Management

### DeltaAlgorithms

Delta firmware update algorithms.

```cpp
#include "firmware/delta_algorithms.h"

namespace lwm2m::firmware {

class DeltaAlgorithms {
public:
    DeltaAlgorithms();
    ~DeltaAlgorithms();

    // Generate delta patch
    DeltaResult generatePatch(
        const std::string& oldFilePath,
        const std::string& newFilePath,
        const std::string& patchFilePath,
        DeltaAlgorithm algorithm = DeltaAlgorithm::BSDIFF
    );

    DeltaResult generatePatch(
        const std::vector<uint8_t>& oldData,
        const std::vector<uint8_t>& newData,
        std::vector<uint8_t>& patchData,
        DeltaAlgorithm algorithm = DeltaAlgorithm::BSDIFF
    );

    // Apply delta patch
    DeltaResult applyPatch(
        const std::string& oldFilePath,
        const std::string& patchFilePath,
        const std::string& newFilePath,
        DeltaAlgorithm algorithm = DeltaAlgorithm::BSDIFF
    );

    DeltaResult applyPatch(
        const std::vector<uint8_t>& oldData,
        const std::vector<uint8_t>& patchData,
        std::vector<uint8_t>& newData,
        DeltaAlgorithm algorithm = DeltaAlgorithm::BSDIFF
    );

    // Compression
    CompressionResult compress(
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        CompressionAlgorithm algorithm,
        int level = -1
    );

    CompressionResult decompress(
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        CompressionAlgorithm algorithm
    );

    // Validation
    bool validateChecksum(
        const std::string& filePath,
        const std::string& expectedChecksum,
        ChecksumAlgorithm algorithm = ChecksumAlgorithm::SHA256
    );

    std::string calculateChecksum(
        const std::string& filePath,
        ChecksumAlgorithm algorithm = ChecksumAlgorithm::SHA256
    );

    // Algorithm detection
    static DeltaAlgorithm detectAlgorithm(const std::vector<uint8_t>& patchData);
    static std::vector<DeltaAlgorithm> getSupportedAlgorithms();
};

}  // namespace lwm2m::firmware
```

---

### RollbackManager

A/B partition and rollback management.

```cpp
#include "firmware/rollback_manager.h"

namespace lwm2m::firmware {

class RollbackManager {
public:
    struct Config {
        std::string snapshotDir = "/var/lib/lwm2m/snapshots";
        size_t maxSnapshots = 3;
        bool enableABPartition = true;
        std::string partitionA = "/dev/mmcblk0p2";
        std::string partitionB = "/dev/mmcblk0p3";
    };

    explicit RollbackManager(const Config& config = {});
    ~RollbackManager();

    // Snapshot management
    RollbackResult createSnapshot(const std::string& version);
    RollbackResult rollback(const std::string& targetVersion);
    RollbackResult rollbackToLatest();
    std::vector<SnapshotInfo> getAvailableSnapshots() const;
    RollbackResult deleteSnapshot(const std::string& version);
    void pruneSnapshots();

    // A/B partition management
    PartitionSlot getActiveSlot() const;
    PartitionSlot getInactiveSlot() const;
    RollbackResult switchSlot();
    bool isSlotBootable(PartitionSlot slot) const;
    RollbackResult markSlotBootable(PartitionSlot slot, bool bootable);
    RollbackResult writeToInactiveSlot(
        const std::vector<uint8_t>& data,
        size_t offset = 0
    );

    // Recovery
    RollbackResult enterRecoveryMode();
    bool isInRecoveryMode() const;
    RollbackResult exitRecoveryMode();

    // Boot management
    void incrementBootCounter();
    void resetBootCounter();
    int getBootCounter() const;
    void setMaxBootAttempts(int attempts);

    // State callbacks
    void setStateChangeCallback(StateChangeCallback callback);
};

}  // namespace lwm2m::firmware
```

---

## Edge AI Inference

### EdgeAIInferenceObject

Edge AI inference object (Object ID: 33410).

```cpp
#include "objects/edge_ai_inference_object.h"

namespace lwm2m::objects {

class EdgeAIInferenceObject {
public:
    struct Config {
        std::string modelStoragePath = "/var/lib/lwm2m/models";
        size_t maxModelSize = 100 * 1024 * 1024;
        bool enableGPU = false;
        int defaultNumThreads = 4;
        size_t maxWorkingMemory = 256 * 1024 * 1024;
    };

    EdgeAIInferenceObject(uint16_t instanceId, const Config& config = {});
    ~EdgeAIInferenceObject();

    // Initialization
    bool initialize();
    void shutdown();

    // Model management
    ModelLoadResult loadModel(
        const std::string& filePath,
        ModelFormat format = ModelFormat::AUTO
    );
    ModelLoadResult loadModelFromMemory(
        const std::vector<uint8_t>& modelData,
        ModelFormat format
    );
    void unloadModel();
    ModelLoadResult swapModel(
        const std::string& filePath,
        ModelFormat format = ModelFormat::AUTO
    );

    // Model download
    void setModelUri(const std::string& uri);
    void setModelChecksum(const std::string& checksum);
    void downloadModel(DownloadProgressCallback callback = nullptr);
    void cancelDownload();

    // Inference
    InferenceResult runInference(
        const std::vector<Tensor>& inputs,
        std::vector<Tensor>& outputs,
        const InferenceConfig& config = {}
    );

    void runInferenceAsync(
        const std::vector<Tensor>& inputs,
        InferenceCallback callback,
        const InferenceConfig& config = {}
    );

    // Streaming inference
    void startStreaming(
        StreamingCallback callback,
        const InferenceConfig& config = {}
    );
    void pushStreamingFrame(
        const std::vector<Tensor>& inputs,
        int frameId
    );
    void stopStreaming();

    // Result decoding
    std::vector<std::pair<std::string, float>> decodeClassification(
        const Tensor& output,
        int topK = 5
    );

    std::vector<DetectionResult> decodeDetection(
        const std::vector<Tensor>& outputs,
        float confidenceThreshold = 0.5f,
        bool applyNMS = true
    );

    // Configuration
    void setAccelerator(AcceleratorType type);
    void setNumThreads(int threads);
    void setPreprocessConfig(const PrePostProcessConfig& config);
    void setPostprocessConfig(const PrePostProcessConfig& config);
    void setLabels(const std::vector<std::string>& labels);

    // State and info
    ModelState getState() const;
    ModelMetadata getModelMetadata() const;
    std::string getLastError() const;
    HardwareCapabilities getHardwareCapabilities() const;

    // Statistics
    InferenceStatistics getStatistics() const;
    void resetStatistics();

    // Profiling and benchmarking
    void enableProfiling(bool enable);
    ProfileData getProfileData() const;
    BenchmarkResult runBenchmark(int iterations = 100, int warmupRuns = 10);

    // LwM2M resource handlers
    LwM2MResult readResource(uint16_t resourceId, std::vector<uint8_t>& data);
    LwM2MResult writeResource(uint16_t resourceId, const std::vector<uint8_t>& data);
    LwM2MResult executeResource(uint16_t resourceId, const std::string& args);
};

}  // namespace lwm2m::objects
```

---

### Tensor

Multi-dimensional array for ML data.

```cpp
namespace lwm2m::objects {

class Tensor {
public:
    // Static creation
    static Tensor create(
        const std::vector<int>& dimensions,
        TensorDataType dataType
    );
    static Tensor create(const TensorShape& shape);

    // Accessors
    template<typename T>
    T* data();

    template<typename T>
    const T* data() const;

    size_t size() const;           // Number of elements
    size_t sizeBytes() const;      // Size in bytes
    TensorShape shape() const;
    TensorDataType dataType() const;
    int numDimensions() const;
    int dimension(int index) const;

    // Operations
    Tensor reshape(const std::vector<int>& newShape) const;
    Tensor slice(int start, int end, int axis = 0) const;
    Tensor copy() const;

    // Validation
    bool isValid() const;
    bool isCompatible(const Tensor& other) const;
};

struct TensorShape {
    std::vector<int> dimensions;
    TensorDataType dataType;

    size_t totalElements() const;
    size_t sizeBytes() const;
    std::string toString() const;
};

}  // namespace lwm2m::objects
```

---

## Platform Abstraction

### PlatformInterface

Abstract platform interface.

```cpp
#include "platform/platform_interface.h"

namespace lwm2m::platform {

class PlatformInterface {
public:
    virtual ~PlatformInterface() = default;

    // Initialization
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    // Device information
    virtual DeviceInfo getDeviceInfo() const = 0;
    virtual std::string getSerialNumber() const = 0;
    virtual std::string getFirmwareVersion() const = 0;

    // System operations
    virtual void reboot() = 0;
    virtual void factoryReset() = 0;
    virtual uint64_t getUptime() const = 0;
    virtual uint64_t getCurrentTime() const = 0;

    // Memory
    virtual MemoryInfo getMemoryInfo() const = 0;

    // Storage
    virtual StorageInfo getStorageInfo() const = 0;
    virtual bool readFile(const std::string& path, std::vector<uint8_t>& data) = 0;
    virtual bool writeFile(const std::string& path, const std::vector<uint8_t>& data) = 0;

    // Network
    virtual NetworkInfo getNetworkInfo() const = 0;

    // Power
    virtual PowerInfo getPowerInfo() const = 0;

    // Utilities
    virtual void sleep(uint32_t milliseconds) = 0;
    virtual std::string generateUUID() = 0;
};

}  // namespace lwm2m::platform
```

### LinuxPlatform

Linux platform implementation.

```cpp
#include "platform/linux_platform.h"

namespace lwm2m::platform {

class LinuxPlatform : public PlatformInterface {
public:
    LinuxPlatform();
    ~LinuxPlatform() override;

    // PlatformInterface implementation
    bool initialize() override;
    void shutdown() override;
    DeviceInfo getDeviceInfo() const override;
    // ... all other methods ...

    // Linux-specific
    void setDeviceTreePath(const std::string& path);
    void setSerialNumberFile(const std::string& path);
};

}  // namespace lwm2m::platform
```

### OpenWRTPlatform

OpenWRT platform implementation.

```cpp
#include "platform/openwrt_platform.h"

namespace lwm2m::platform {

class OpenWRTPlatform : public PlatformInterface {
public:
    OpenWRTPlatform();
    ~OpenWRTPlatform() override;

    // PlatformInterface implementation
    bool initialize() override;
    void shutdown() override;
    // ... all other methods ...

    // OpenWRT-specific
    std::string uciGet(const std::string& key) const;
    bool uciSet(const std::string& key, const std::string& value);
    bool uciCommit(const std::string& config);

    // MTD operations
    bool mtdRead(const std::string& partition, std::vector<uint8_t>& data, 
                 size_t offset = 0, size_t size = 0);
    bool mtdWrite(const std::string& partition, const std::vector<uint8_t>& data,
                  size_t offset = 0);
    bool mtdErase(const std::string& partition);
};

}  // namespace lwm2m::platform
```

---

## LwM2M Objects

### AdvancedFirmwareUpdateObject

Advanced firmware update object (Object ID: 33405).

```cpp
#include "objects/advanced_firmware_update_object.h"

namespace lwm2m::objects {

class AdvancedFirmwareUpdateObject {
public:
    struct Config {
        std::string downloadPath = "/tmp/firmware";
        size_t maxPackageSize = 256 * 1024 * 1024;
        bool enableDelta = true;
        bool enableRollback = true;
        std::vector<DeltaAlgorithm> supportedAlgorithms = {
            DeltaAlgorithm::BSDIFF,
            DeltaAlgorithm::VCDIFF
        };
    };

    AdvancedFirmwareUpdateObject(uint16_t instanceId, const Config& config = {});
    ~AdvancedFirmwareUpdateObject();

    // State management
    FirmwareState getState() const;
    FirmwareUpdateResult getUpdateResult() const;

    // Package management
    void setPackageUri(const std::string& uri);
    void setDeltaPackageUri(const std::string& uri);
    void downloadPackage();
    void applyUpdate();
    void cancelUpdate();

    // Progress
    uint8_t getDownloadProgress() const;
    uint8_t getUpdateProgress() const;

    // Version info
    std::string getCurrentVersion() const;
    std::string getTargetVersion() const;

    // Callbacks
    void setStateChangeCallback(FirmwareStateCallback callback);
    void setProgressCallback(ProgressCallback callback);

    // LwM2M resource handlers
    LwM2MResult readResource(uint16_t resourceId, std::vector<uint8_t>& data);
    LwM2MResult writeResource(uint16_t resourceId, const std::vector<uint8_t>& data);
    LwM2MResult executeResource(uint16_t resourceId, const std::string& args);
};

}  // namespace lwm2m::objects
```

---

## Utility Classes

### LwM2MPayload

LwM2M message payload structure.

```cpp
namespace lwm2m::transport {

struct LwM2MPayload {
    LwM2MOperation operation;
    uint32_t token;
    std::string path;
    ContentFormat contentFormat;
    std::vector<uint8_t> data;
    LwM2MResponseCode responseCode;
    std::string errorMessage;

    // Registration-specific
    std::string endpointName;
    uint32_t lifetime;
    std::string lwm2mVersion;
    std::string bindingMode;
    bool queueMode;
    std::vector<std::string> objectLinks;
    std::string alternativePath;

    // Notification-specific
    uint32_t observeSequence;
    uint64_t timestamp;

    // Composite operations
    std::vector<std::string> compositePaths;
};

}  // namespace lwm2m::transport
```

---

## Constants and Enums

### Transport Enums

```cpp
namespace lwm2m::transport {

enum class MqttResult {
    SUCCESS = 0,
    ERROR_TIMEOUT = -1,
    ERROR_NETWORK = -2,
    ERROR_TLS = -3,
    ERROR_AUTH = -4,
    ERROR_PROTOCOL = -5,
    ERROR_NOT_CONNECTED = -6,
    ERROR_INVALID_PARAM = -7
};

enum class MqttQoS {
    AT_MOST_ONCE = 0,
    AT_LEAST_ONCE = 1,
    EXACTLY_ONCE = 2
};

enum class MqttSecurityMode {
    NO_SECURITY = 0,
    PSK = 1,
    RAW_PUBLIC_KEY = 2,
    CERTIFICATE = 3,
    TOKEN = 4
};

enum class LwM2MOperation {
    BOOTSTRAP_REQUEST = 0,
    BOOTSTRAP_WRITE = 1,
    BOOTSTRAP_DISCOVER = 2,
    BOOTSTRAP_FINISH = 3,
    REGISTER = 10,
    UPDATE = 11,
    DEREGISTER = 12,
    READ = 20,
    WRITE = 21,
    EXECUTE = 22,
    CREATE = 23,
    DELETE = 24,
    DISCOVER = 25,
    WRITE_ATTRIBUTES = 26,
    READ_COMPOSITE = 27,
    WRITE_COMPOSITE = 28,
    OBSERVE = 30,
    CANCEL_OBSERVE = 31,
    NOTIFY = 32,
    SEND = 33
};

enum class LwM2MResponseCode {
    CREATED = 201,
    DELETED = 202,
    CHANGED = 204,
    CONTENT = 205,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    INTERNAL_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504
};

enum class ContentFormat {
    TEXT_PLAIN = 0,
    LINK_FORMAT = 40,
    OPAQUE = 42,
    CBOR = 60,
    LWM2M_TLV = 11542,
    LWM2M_JSON = 11543,
    SENML_JSON = 110,
    SENML_CBOR = 112,
    LWM2M_CBOR = 11544
};

}  // namespace lwm2m::transport
```

### Firmware Enums

```cpp
namespace lwm2m::firmware {

enum class DeltaAlgorithm {
    BSDIFF = 1,
    VCDIFF = 2,
    COURGETTE = 3
};

enum class CompressionAlgorithm {
    NONE = 0,
    ZLIB = 1,
    LZMA = 2,
    BROTLI = 3,
    ZSTD = 4
};

enum class FirmwareState {
    IDLE = 0,
    DOWNLOADING = 1,
    DOWNLOADED = 2,
    VALIDATING = 3,
    APPLYING = 4,
    UPDATING = 5,
    REBOOTING = 6
};

enum class FirmwareUpdateResult {
    SUCCESS = 0,
    NO_MEMORY = 1,
    CONNECTION_LOST = 2,
    INTEGRITY_FAILURE = 3,
    UNSUPPORTED_TYPE = 4,
    INVALID_URI = 5,
    FAILED_UPDATE = 6,
    UNSUPPORTED_PROTOCOL = 7,
    DELTA_MISMATCH = 8,
    ROLLBACK_REQUIRED = 9
};

enum class PartitionSlot {
    SLOT_A = 0,
    SLOT_B = 1
};

}  // namespace lwm2m::firmware
```

### Edge AI Enums

```cpp
namespace lwm2m::objects {

enum class ModelFormat {
    AUTO = -1,
    TFLITE = 0,
    ONNX = 1,
    PYTORCH_MOBILE = 2,
    CORE_ML = 3,
    TENSORRT = 4,
    OPENVINO = 5
};

enum class ModelState {
    IDLE = 0,
    DOWNLOADING = 1,
    VALIDATING = 2,
    LOADING = 3,
    READY = 4,
    RUNNING = 5,
    ERROR = 6,
    UPDATING = 7
};

enum class AcceleratorType {
    AUTO = -1,
    CPU = 0,
    GPU = 1,
    NPU = 2,
    DSP = 3,
    TPU = 4
};

enum class TensorDataType {
    FLOAT32 = 0,
    FLOAT16 = 1,
    INT32 = 2,
    INT16 = 3,
    INT8 = 4,
    UINT8 = 5,
    INT4 = 6,
    BOOL = 7
};

enum class InferenceResult {
    SUCCESS = 0,
    ERROR_MODEL_NOT_LOADED = -1,
    ERROR_INVALID_INPUT = -2,
    ERROR_INPUT_SHAPE_MISMATCH = -3,
    ERROR_OUTPUT_ALLOCATION = -4,
    ERROR_EXECUTION_FAILED = -5,
    ERROR_TIMEOUT = -6,
    ERROR_OUT_OF_MEMORY = -7,
    ERROR_CANCELLED = -8
};

}  // namespace lwm2m::objects
```

---

## Object IDs

| Object ID | Name | Class |
|-----------|------|-------|
| 0 | LwM2M Security | - |
| 1 | LwM2M Server | - |
| 3 | Device | - |
| 5 | Firmware Update | - |
| 21 | OSCORE | - |
| 23 | LwM2M COSE | `MqttCoseObject` |
| 24 | MQTT Server | `MqttServerObject` |
| 33405 | Advanced Firmware Update | `AdvancedFirmwareUpdateObject` |
| 33410 | Edge AI Inference | `EdgeAIInferenceObject` |

---

*For usage examples, see the [Getting Started Guide](GETTING_STARTED.md) and feature-specific guides.*
