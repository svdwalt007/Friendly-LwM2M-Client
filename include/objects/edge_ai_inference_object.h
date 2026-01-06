/**
 * @file edge_ai_inference_object.h
 * @brief LwM2M Edge AI Inference Object (Custom Object 33410)
 * 
 * Provides on-device machine learning inference capabilities for LwM2M clients.
 * Supports multiple ML frameworks: TensorFlow Lite, ONNX Runtime, and custom models.
 * 
 * Features:
 * - Model management (download, validate, load, unload)
 * - Multi-format support (TFLite, ONNX, custom binary)
 * - Hardware acceleration (GPU, NPU, DSP delegates)
 * - Quantization support (INT8, FP16, dynamic)
 * - Real-time inference with async/sync modes
 * - Model versioning and A/B testing
 * - Resource monitoring (latency, memory, power)
 * - Federated learning support (gradients upload)
 * 
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_EDGE_AI_INFERENCE_OBJECT_H
#define FRIENDLY_LWM2M_EDGE_AI_INFERENCE_OBJECT_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>
#include <chrono>
#include <optional>
#include <variant>

namespace lwm2m {
namespace objects {

// ============================================================================
// Constants and Enumerations
// ============================================================================

/**
 * @brief Supported ML model formats
 */
enum class ModelFormat : uint8_t {
    TFLITE = 0,           // TensorFlow Lite (.tflite)
    ONNX = 1,             // Open Neural Network Exchange (.onnx)
    PYTORCH_MOBILE = 2,   // PyTorch Mobile (.pt, .ptl)
    CORE_ML = 3,          // Apple Core ML (.mlmodel, .mlpackage)
    TENSOR_RT = 4,        // NVIDIA TensorRT (.engine, .plan)
    OPENVINO = 5,         // Intel OpenVINO (.xml, .bin)
    CUSTOM = 255          // Custom binary format
};

/**
 * @brief Model quantization types
 */
enum class QuantizationType : uint8_t {
    NONE = 0,             // Full precision (FP32)
    FLOAT16 = 1,          // Half precision (FP16)
    INT8 = 2,             // 8-bit integer quantization
    INT4 = 3,             // 4-bit integer quantization
    DYNAMIC = 4,          // Dynamic range quantization
    MIXED = 5             // Mixed precision
};

/**
 * @brief Hardware acceleration backends
 */
enum class AcceleratorType : uint8_t {
    CPU = 0,              // CPU only
    GPU = 1,              // GPU acceleration
    NPU = 2,              // Neural Processing Unit
    DSP = 3,              // Digital Signal Processor
    TPU = 4,              // Tensor Processing Unit
    FPGA = 5,             // Field Programmable Gate Array
    AUTO = 255            // Automatic selection
};

/**
 * @brief Model lifecycle states
 */
enum class ModelState : uint8_t {
    IDLE = 0,             // No model loaded
    DOWNLOADING = 1,      // Model being downloaded
    VALIDATING = 2,       // Model being validated
    LOADING = 3,          // Model being loaded into memory
    READY = 4,            // Model ready for inference
    RUNNING = 5,          // Inference in progress
    ERROR = 6,            // Error state
    UPDATING = 7          // Model being updated
};

/**
 * @brief Inference execution mode
 */
enum class InferenceMode : uint8_t {
    SYNCHRONOUS = 0,      // Block until complete
    ASYNCHRONOUS = 1,     // Return immediately, callback on complete
    STREAMING = 2,        // Continuous inference (video/audio)
    BATCHED = 3           // Batch multiple inputs
};

/**
 * @brief Tensor data types
 */
enum class TensorDataType : uint8_t {
    FLOAT32 = 0,
    FLOAT16 = 1,
    INT32 = 2,
    INT16 = 3,
    INT8 = 4,
    UINT8 = 5,
    INT64 = 6,
    BOOL = 7,
    STRING = 8,
    COMPLEX64 = 9
};

/**
 * @brief Inference result codes
 */
enum class InferenceResult : int {
    SUCCESS = 0,
    ERROR_MODEL_NOT_LOADED = -1,
    ERROR_INVALID_INPUT = -2,
    ERROR_INPUT_SHAPE_MISMATCH = -3,
    ERROR_OUTPUT_BUFFER_TOO_SMALL = -4,
    ERROR_EXECUTION_FAILED = -5,
    ERROR_TIMEOUT = -6,
    ERROR_OUT_OF_MEMORY = -7,
    ERROR_ACCELERATOR_UNAVAILABLE = -8,
    ERROR_MODEL_CORRUPTED = -9,
    ERROR_UNSUPPORTED_OPERATION = -10
};

// ============================================================================
// Data Structures
// ============================================================================

/**
 * @brief Tensor shape specification
 */
struct TensorShape {
    std::vector<int64_t> dimensions;
    TensorDataType dataType = TensorDataType::FLOAT32;
    std::string name;
    
    /**
     * @brief Calculate total number of elements
     */
    int64_t numElements() const {
        int64_t total = 1;
        for (auto dim : dimensions) {
            total *= dim;
        }
        return total;
    }
    
    /**
     * @brief Calculate size in bytes
     */
    size_t sizeBytes() const {
        static const size_t typeSizes[] = {4, 2, 4, 2, 1, 1, 8, 1, 0, 8};
        return numElements() * typeSizes[static_cast<int>(dataType)];
    }
};

/**
 * @brief Tensor data container
 */
struct Tensor {
    TensorShape shape;
    std::vector<uint8_t> data;
    
    /**
     * @brief Access data as typed pointer
     */
    template<typename T>
    T* as() { return reinterpret_cast<T*>(data.data()); }
    
    template<typename T>
    const T* as() const { return reinterpret_cast<const T*>(data.data()); }
    
    /**
     * @brief Create tensor with given shape
     */
    static Tensor create(const TensorShape& shape) {
        Tensor t;
        t.shape = shape;
        t.data.resize(shape.sizeBytes());
        return t;
    }
};

/**
 * @brief Model metadata
 */
struct ModelMetadata {
    std::string name;                    // Model name
    std::string version;                 // Semantic version
    std::string description;             // Human-readable description
    std::string author;                  // Model author/organization
    std::string license;                 // License type
    ModelFormat format;                  // Model format
    QuantizationType quantization;       // Quantization type
    std::vector<TensorShape> inputs;     // Input tensor specifications
    std::vector<TensorShape> outputs;    // Output tensor specifications
    std::vector<std::string> labels;     // Output class labels (if applicable)
    std::map<std::string, std::string> customMetadata;  // Additional metadata
    
    // Resource requirements
    size_t modelSizeBytes = 0;           // Model file size
    size_t runtimeMemoryBytes = 0;       // Estimated runtime memory
    float estimatedLatencyMs = 0;        // Estimated inference latency
};

/**
 * @brief Model download configuration
 */
struct ModelDownloadConfig {
    std::string uri;                     // Model URI (http/https/coap/file)
    std::vector<uint8_t> checksum;       // SHA256 checksum
    std::string checksumType = "SHA256"; // Checksum algorithm
    size_t expectedSize = 0;             // Expected file size
    bool resumable = true;               // Support resume on interruption
    uint32_t timeout = 300;              // Download timeout (seconds)
    uint32_t retries = 3;                // Maximum retry attempts
    std::map<std::string, std::string> headers;  // HTTP headers
};

/**
 * @brief Inference configuration
 */
struct InferenceConfig {
    InferenceMode mode = InferenceMode::SYNCHRONOUS;
    AcceleratorType accelerator = AcceleratorType::AUTO;
    uint32_t numThreads = 0;             // 0 = auto
    uint32_t timeout = 5000;             // Inference timeout (ms)
    bool allowFallback = true;           // Fallback to CPU if accelerator fails
    bool profileMode = false;            // Enable profiling
    uint32_t batchSize = 1;              // Batch size for BATCHED mode
    float powerPriority = 0.5f;          // 0.0=save power, 1.0=max performance
};

/**
 * @brief Inference statistics
 */
struct InferenceStatistics {
    uint64_t totalInferences = 0;        // Total inference count
    uint64_t successfulInferences = 0;   // Successful inferences
    uint64_t failedInferences = 0;       // Failed inferences
    double avgLatencyMs = 0;             // Average latency
    double minLatencyMs = 0;             // Minimum latency
    double maxLatencyMs = 0;             // Maximum latency
    double avgThroughput = 0;            // Inferences per second
    size_t peakMemoryBytes = 0;          // Peak memory usage
    double avgPowerMw = 0;               // Average power consumption
    std::chrono::system_clock::time_point lastInference;
    AcceleratorType lastAccelerator;     // Accelerator used
};

/**
 * @brief Pre/post processing configuration
 */
struct PrePostProcessConfig {
    // Input preprocessing
    bool normalizeInput = false;         // Normalize to [0,1] or [-1,1]
    std::vector<float> mean;             // Mean subtraction (per channel)
    std::vector<float> stddev;           // Standard deviation division
    bool resizeInput = false;            // Auto-resize input
    std::string resizeMethod = "bilinear"; // Resize interpolation
    
    // Output postprocessing
    bool applySoftmax = false;           // Apply softmax to output
    float confidenceThreshold = 0.5f;    // Confidence threshold
    int topK = 5;                         // Return top-K results
    bool nonMaxSuppression = false;      // NMS for object detection
    float nmsThreshold = 0.5f;           // NMS IoU threshold
};

// ============================================================================
// Callback Types
// ============================================================================

/**
 * @brief Callback for inference completion
 */
using InferenceCallback = std::function<void(InferenceResult result,
                                              const std::vector<Tensor>& outputs,
                                              double latencyMs)>;

/**
 * @brief Callback for model download progress
 */
using DownloadProgressCallback = std::function<void(uint64_t downloaded,
                                                     uint64_t total,
                                                     double speedBps)>;

/**
 * @brief Callback for model state changes
 */
using StateChangeCallback = std::function<void(ModelState oldState,
                                                ModelState newState,
                                                const std::string& reason)>;

/**
 * @brief Callback for streaming inference
 */
using StreamingCallback = std::function<void(int frameId,
                                              const std::vector<Tensor>& outputs,
                                              double latencyMs)>;

// ============================================================================
// Edge AI Inference Object (Object ID: 33410)
// ============================================================================

/**
 * @brief LwM2M Edge AI Inference Object Implementation
 * 
 * Custom LwM2M object providing on-device ML inference capabilities.
 */
class EdgeAIInferenceObject {
public:
    /**
     * @brief Object ID
     */
    static constexpr uint16_t OBJECT_ID = 33410;

    /**
     * @brief Resource IDs
     */
    enum class ResourceId : uint16_t {
        // Model Management (0-19)
        MODEL_NAME = 0,
        MODEL_VERSION = 1,
        MODEL_URI = 2,
        MODEL_FORMAT = 3,
        MODEL_STATE = 4,
        MODEL_SIZE = 5,
        MODEL_CHECKSUM = 6,
        LOAD_MODEL = 7,              // Execute
        UNLOAD_MODEL = 8,            // Execute
        DOWNLOAD_MODEL = 9,          // Execute
        DOWNLOAD_PROGRESS = 10,
        
        // Input/Output Configuration (20-39)
        INPUT_TENSOR_INFO = 20,
        OUTPUT_TENSOR_INFO = 21,
        INPUT_DATA = 22,             // Write
        OUTPUT_DATA = 23,            // Read
        LABELS = 24,
        
        // Inference Control (40-59)
        RUN_INFERENCE = 40,          // Execute
        INFERENCE_MODE = 41,
        ACCELERATOR_TYPE = 42,
        NUM_THREADS = 43,
        INFERENCE_TIMEOUT = 44,
        QUANTIZATION_TYPE = 45,
        
        // Statistics and Monitoring (60-79)
        INFERENCE_COUNT = 60,
        INFERENCE_LATENCY_AVG = 61,
        INFERENCE_LATENCY_MIN = 62,
        INFERENCE_LATENCY_MAX = 63,
        INFERENCE_THROUGHPUT = 64,
        MEMORY_USAGE = 65,
        POWER_CONSUMPTION = 66,
        ERROR_COUNT = 67,
        LAST_ERROR = 68,
        RESET_STATISTICS = 69,       // Execute
        
        // Pre/Post Processing (80-99)
        NORMALIZE_INPUT = 80,
        INPUT_MEAN = 81,
        INPUT_STDDEV = 82,
        APPLY_SOFTMAX = 83,
        CONFIDENCE_THRESHOLD = 84,
        TOP_K_RESULTS = 85,
        NMS_ENABLED = 86,
        NMS_THRESHOLD = 87,
        
        // Advanced Features (100-119)
        MODEL_METADATA = 100,
        SUPPORTED_FORMATS = 101,
        SUPPORTED_ACCELERATORS = 102,
        HARDWARE_CAPABILITIES = 103,
        BENCHMARK_RESULT = 104,
        RUN_BENCHMARK = 105,         // Execute
        ENABLE_PROFILING = 106,
        PROFILE_DATA = 107
    };

    /**
     * @brief Configuration structure
     */
    struct Config {
        std::string modelStoragePath = "/var/lib/lwm2m/models";
        size_t maxModelSize = 100 * 1024 * 1024;  // 100MB
        size_t maxRuntimeMemory = 512 * 1024 * 1024;  // 512MB
        bool enableGPU = true;
        bool enableNPU = true;
        uint32_t defaultNumThreads = 4;
        uint32_t defaultTimeout = 5000;
        bool autoLoadLastModel = false;
    };

    /**
     * @brief Constructor
     * @param instanceId Object instance ID
     * @param config Configuration
     */
    explicit EdgeAIInferenceObject(uint16_t instanceId = 0,
                                    const Config& config = Config{});

    /**
     * @brief Destructor
     */
    ~EdgeAIInferenceObject();

    // Prevent copying
    EdgeAIInferenceObject(const EdgeAIInferenceObject&) = delete;
    EdgeAIInferenceObject& operator=(const EdgeAIInferenceObject&) = delete;

    /**
     * @brief Initialize the object
     * @return true on success
     */
    bool initialize();

    // ========================================================================
    // LwM2M Resource Interface
    // ========================================================================

    /**
     * @brief Read string resource
     */
    bool read(ResourceId resourceId, std::string& value);

    /**
     * @brief Read integer resource
     */
    bool read(ResourceId resourceId, int64_t& value);

    /**
     * @brief Read float resource
     */
    bool read(ResourceId resourceId, double& value);

    /**
     * @brief Read opaque resource
     */
    bool read(ResourceId resourceId, std::vector<uint8_t>& value);

    /**
     * @brief Write string resource
     */
    bool write(ResourceId resourceId, const std::string& value);

    /**
     * @brief Write integer resource
     */
    bool write(ResourceId resourceId, int64_t value);

    /**
     * @brief Write float resource
     */
    bool write(ResourceId resourceId, double value);

    /**
     * @brief Write opaque resource
     */
    bool write(ResourceId resourceId, const std::vector<uint8_t>& value);

    /**
     * @brief Execute resource
     */
    bool execute(ResourceId resourceId, const std::string& arguments = "");

    // ========================================================================
    // Model Management
    // ========================================================================

    /**
     * @brief Download model from URI
     * @param config Download configuration
     * @param callback Progress callback
     * @return true if download started
     */
    bool downloadModel(const ModelDownloadConfig& config,
                        DownloadProgressCallback callback = nullptr);

    /**
     * @brief Load model from file
     * @param filePath Path to model file
     * @param format Model format (or AUTO to detect)
     * @return true on success
     */
    bool loadModel(const std::string& filePath,
                    ModelFormat format = ModelFormat::TFLITE);

    /**
     * @brief Load model from memory
     * @param data Model binary data
     * @param format Model format
     * @return true on success
     */
    bool loadModelFromMemory(const std::vector<uint8_t>& data,
                              ModelFormat format);

    /**
     * @brief Unload current model
     * @return true on success
     */
    bool unloadModel();

    /**
     * @brief Get current model state
     */
    ModelState getModelState() const;

    /**
     * @brief Get model metadata
     */
    std::optional<ModelMetadata> getModelMetadata() const;

    /**
     * @brief Validate model integrity
     * @param checksum Expected SHA256 checksum
     * @return true if valid
     */
    bool validateModel(const std::vector<uint8_t>& checksum);

    // ========================================================================
    // Inference
    // ========================================================================

    /**
     * @brief Run synchronous inference
     * @param inputs Input tensors
     * @param outputs Output tensors (populated on return)
     * @param config Inference configuration
     * @return Result code
     */
    InferenceResult runInference(const std::vector<Tensor>& inputs,
                                  std::vector<Tensor>& outputs,
                                  const InferenceConfig& config = InferenceConfig{});

    /**
     * @brief Run asynchronous inference
     * @param inputs Input tensors
     * @param callback Completion callback
     * @param config Inference configuration
     * @return true if inference started
     */
    bool runInferenceAsync(const std::vector<Tensor>& inputs,
                            InferenceCallback callback,
                            const InferenceConfig& config = InferenceConfig{});

    /**
     * @brief Start streaming inference
     * @param callback Callback for each frame result
     * @param config Inference configuration
     * @return true if streaming started
     */
    bool startStreaming(StreamingCallback callback,
                         const InferenceConfig& config = InferenceConfig{});

    /**
     * @brief Push frame to streaming inference
     * @param inputs Input tensors for this frame
     * @param frameId Frame identifier
     * @return true if frame queued
     */
    bool pushStreamingFrame(const std::vector<Tensor>& inputs, int frameId);

    /**
     * @brief Stop streaming inference
     */
    void stopStreaming();

    /**
     * @brief Cancel pending async inference
     */
    void cancelInference();

    // ========================================================================
    // Pre/Post Processing
    // ========================================================================

    /**
     * @brief Set preprocessing configuration
     */
    void setPreprocessConfig(const PrePostProcessConfig& config);

    /**
     * @brief Get preprocessing configuration
     */
    PrePostProcessConfig getPreprocessConfig() const;

    /**
     * @brief Preprocess input tensor
     * @param input Raw input tensor
     * @return Preprocessed tensor
     */
    Tensor preprocessInput(const Tensor& input);

    /**
     * @brief Postprocess output tensor
     * @param output Raw output tensor
     * @return Postprocessed tensor
     */
    Tensor postprocessOutput(const Tensor& output);

    /**
     * @brief Decode classification output
     * @param output Output tensor
     * @param topK Number of top results
     * @return Vector of (label, confidence) pairs
     */
    std::vector<std::pair<std::string, float>> decodeClassification(
        const Tensor& output, int topK = 5);

    /**
     * @brief Decode object detection output
     * @param outputs Output tensors (boxes, scores, classes)
     * @param confidenceThreshold Minimum confidence
     * @param nmsThreshold NMS IoU threshold
     * @return Vector of detection boxes
     */
    struct DetectionBox {
        float x1, y1, x2, y2;
        float confidence;
        int classId;
        std::string label;
    };
    std::vector<DetectionBox> decodeDetection(
        const std::vector<Tensor>& outputs,
        float confidenceThreshold = 0.5f,
        float nmsThreshold = 0.5f);

    // ========================================================================
    // Statistics and Monitoring
    // ========================================================================

    /**
     * @brief Get inference statistics
     */
    InferenceStatistics getStatistics() const;

    /**
     * @brief Reset statistics
     */
    void resetStatistics();

    /**
     * @brief Run benchmark
     * @param iterations Number of inference iterations
     * @param warmupIterations Warmup iterations (not counted)
     * @return Benchmark results
     */
    struct BenchmarkResult {
        double avgLatencyMs;
        double minLatencyMs;
        double maxLatencyMs;
        double p50LatencyMs;
        double p95LatencyMs;
        double p99LatencyMs;
        double throughput;
        size_t memoryUsedBytes;
        AcceleratorType acceleratorUsed;
    };
    BenchmarkResult runBenchmark(int iterations = 100, int warmupIterations = 10);

    /**
     * @brief Enable profiling
     */
    void enableProfiling(bool enable);

    /**
     * @brief Get profiling data
     */
    struct ProfileData {
        struct LayerProfile {
            std::string name;
            std::string type;
            double timeMs;
            size_t memoryBytes;
        };
        std::vector<LayerProfile> layers;
        double totalTimeMs;
        size_t totalMemoryBytes;
    };
    std::optional<ProfileData> getProfileData() const;

    // ========================================================================
    // Hardware and Capability Detection
    // ========================================================================

    /**
     * @brief Get supported model formats
     */
    std::vector<ModelFormat> getSupportedFormats() const;

    /**
     * @brief Get available accelerators
     */
    std::vector<AcceleratorType> getAvailableAccelerators() const;

    /**
     * @brief Get hardware capabilities
     */
    struct HardwareCapabilities {
        std::string cpuName;
        int cpuCores;
        size_t ramBytes;
        bool hasGPU;
        std::string gpuName;
        size_t gpuMemoryBytes;
        bool hasNPU;
        std::string npuName;
        int npuTops;  // Tera operations per second
        bool hasDSP;
    };
    HardwareCapabilities getHardwareCapabilities() const;

    // ========================================================================
    // Callbacks
    // ========================================================================

    /**
     * @brief Set state change callback
     */
    void setStateChangeCallback(StateChangeCallback callback);

    /**
     * @brief Get last error message
     */
    std::string getLastError() const;

private:
    // Internal implementation
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================================
// Model Format Detection
// ============================================================================

/**
 * @brief Utility class for model format detection
 */
class ModelFormatDetector {
public:
    /**
     * @brief Detect format from file extension
     */
    static ModelFormat fromExtension(const std::string& filename);

    /**
     * @brief Detect format from file magic bytes
     */
    static ModelFormat fromMagic(const std::vector<uint8_t>& data);

    /**
     * @brief Get file extension for format
     */
    static std::string getExtension(ModelFormat format);

    /**
     * @brief Get human-readable name for format
     */
    static std::string getName(ModelFormat format);

    /**
     * @brief Check if format is supported on current platform
     */
    static bool isSupported(ModelFormat format);
};

// ============================================================================
// TensorFlow Lite Backend
// ============================================================================

/**
 * @brief TensorFlow Lite inference backend
 */
class TFLiteBackend {
public:
    /**
     * @brief Check if TFLite is available
     */
    static bool isAvailable();

    /**
     * @brief Create interpreter from model data
     */
    static std::unique_ptr<TFLiteBackend> create(const std::vector<uint8_t>& modelData);

    /**
     * @brief Get input tensor info
     */
    virtual std::vector<TensorShape> getInputShapes() const = 0;

    /**
     * @brief Get output tensor info
     */
    virtual std::vector<TensorShape> getOutputShapes() const = 0;

    /**
     * @brief Run inference
     */
    virtual InferenceResult invoke(const std::vector<Tensor>& inputs,
                                    std::vector<Tensor>& outputs) = 0;

    /**
     * @brief Set delegate (GPU, NNAPI, etc.)
     */
    virtual bool setDelegate(AcceleratorType accelerator) = 0;

    /**
     * @brief Set number of threads
     */
    virtual void setNumThreads(int threads) = 0;

    virtual ~TFLiteBackend() = default;
};

// ============================================================================
// ONNX Runtime Backend
// ============================================================================

/**
 * @brief ONNX Runtime inference backend
 */
class ONNXBackend {
public:
    /**
     * @brief Check if ONNX Runtime is available
     */
    static bool isAvailable();

    /**
     * @brief Create session from model data
     */
    static std::unique_ptr<ONNXBackend> create(const std::vector<uint8_t>& modelData);

    /**
     * @brief Get input tensor info
     */
    virtual std::vector<TensorShape> getInputShapes() const = 0;

    /**
     * @brief Get output tensor info
     */
    virtual std::vector<TensorShape> getOutputShapes() const = 0;

    /**
     * @brief Run inference
     */
    virtual InferenceResult invoke(const std::vector<Tensor>& inputs,
                                    std::vector<Tensor>& outputs) = 0;

    /**
     * @brief Set execution provider (CPU, CUDA, TensorRT, etc.)
     */
    virtual bool setExecutionProvider(AcceleratorType accelerator) = 0;

    /**
     * @brief Set session options
     */
    virtual void setIntraOpThreads(int threads) = 0;
    virtual void setInterOpThreads(int threads) = 0;

    virtual ~ONNXBackend() = default;
};

} // namespace objects
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_EDGE_AI_INFERENCE_OBJECT_H
