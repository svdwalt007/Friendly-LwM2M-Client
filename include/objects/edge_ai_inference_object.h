/**
 * @file edge_ai_inference_object.h
 * @brief OMA LwM2M Edge AI Inference Object (ID: 33410)
 *
 * Custom object for on-device machine learning inference with support for
 * TensorFlow Lite and ONNX Runtime backends.
 *
 * Object ID: 33410
 * Multiple Instances: Yes (one per model)
 *
 * Features:
 * - Model management (download, load, unload)
 * - Inference execution (sync, async, streaming, batched)
 * - Hardware acceleration (CPU, GPU, NPU, TPU)
 * - Quantization support (FP32, FP16, INT8, INT4)
 * - Pre/post processing pipelines
 * - Performance statistics and profiling
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_EDGE_AI_INFERENCE_OBJECT_H
#define FRIENDLY_LWM2M_EDGE_AI_INFERENCE_OBJECT_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>

namespace lwm2m {
namespace objects {

/**
 * @brief Edge AI Inference Object ID
 */
constexpr uint16_t EDGE_AI_INFERENCE_OBJECT_ID = 33410;

/**
 * @brief Resource IDs for Edge AI Inference Object
 */
enum class EdgeAIResourceId : uint16_t {
    // Model Management Resources (0-19)
    MODEL_NAME = 0,                 ///< Model name (String, R)
    MODEL_VERSION = 1,              ///< Model version (String, R)
    MODEL_URI = 2,                  ///< Download URI (String, RW)
    MODEL_FORMAT = 3,               ///< Model format enum (Integer, R)
    MODEL_STATE = 4,                ///< Model state enum (Integer, R)
    MODEL_SIZE = 5,                 ///< Model size in bytes (Integer, R)
    MODEL_CHECKSUM = 6,             ///< SHA256 checksum (Opaque, RW)
    LOAD_MODEL = 7,                 ///< Load model from storage (Exec)
    UNLOAD_MODEL = 8,               ///< Unload model from memory (Exec)
    DOWNLOAD_MODEL = 9,             ///< Download model from URI (Exec)
    DOWNLOAD_PROGRESS = 10,         ///< Download progress 0-100 (Integer, R)
    MODEL_DESCRIPTION = 11,         ///< Human description (String, R)
    MODEL_AUTHOR = 12,              ///< Model author (String, R)
    MODEL_LICENSE = 13,             ///< License info (String, R)
    CREATION_DATE = 14,             ///< Model creation date (Time, R)
    MIN_RUNTIME_VERSION = 15,       ///< Minimum runtime version (String, R)

    // Input/Output Resources (20-39)
    INPUT_TENSOR_INFO = 20,         ///< Input tensor specs JSON (Opaque, R)
    OUTPUT_TENSOR_INFO = 21,        ///< Output tensor specs JSON (Opaque, R)
    INPUT_DATA = 22,                ///< Inference input data (Opaque, W)
    OUTPUT_DATA = 23,               ///< Inference result (Opaque, R)
    LABELS = 24,                    ///< Class labels (String, R)
    LABEL_COUNT = 25,               ///< Number of labels (Integer, R)
    INPUT_COUNT = 26,               ///< Number of inputs (Integer, R)
    OUTPUT_COUNT = 27,              ///< Number of outputs (Integer, R)

    // Inference Control Resources (40-59)
    RUN_INFERENCE = 40,             ///< Execute inference (Exec)
    INFERENCE_MODE = 41,            ///< Mode enum (Integer, RW)
    ACCELERATOR_TYPE = 42,          ///< Backend enum (Integer, RW)
    NUM_THREADS = 43,               ///< Worker threads (Integer, RW)
    INFERENCE_TIMEOUT = 44,         ///< Timeout in ms (Integer, RW)
    QUANTIZATION_TYPE = 45,         ///< Quantization enum (Integer, R)
    BATCH_SIZE = 46,                ///< Batch size (Integer, RW)
    PRIORITY = 47,                  ///< Execution priority (Integer, RW)

    // Statistics Resources (60-79)
    INFERENCE_COUNT = 60,           ///< Total inferences (Integer, R)
    LATENCY_AVG = 61,               ///< Average latency ms (Float, R)
    LATENCY_MIN = 62,               ///< Minimum latency ms (Float, R)
    LATENCY_MAX = 63,               ///< Maximum latency ms (Float, R)
    THROUGHPUT = 64,                ///< Inferences/sec (Float, R)
    MEMORY_USAGE = 65,              ///< Peak memory bytes (Integer, R)
    POWER_CONSUMPTION = 66,         ///< Average power mW (Float, R)
    ERROR_COUNT = 67,               ///< Failed inferences (Integer, R)
    LAST_ERROR = 68,                ///< Error message (String, R)
    RESET_STATISTICS = 69,          ///< Reset all stats (Exec)
    LATENCY_P50 = 70,               ///< 50th percentile ms (Float, R)
    LATENCY_P95 = 71,               ///< 95th percentile ms (Float, R)
    LATENCY_P99 = 72,               ///< 99th percentile ms (Float, R)

    // Pre/Post Processing Resources (80-99)
    NORMALIZE_INPUT = 80,           ///< Enable normalization (Boolean, RW)
    INPUT_MEAN = 81,                ///< Per-channel mean (Opaque, RW)
    INPUT_STDDEV = 82,              ///< Per-channel stddev (Opaque, RW)
    APPLY_SOFTMAX = 83,             ///< Apply softmax (Boolean, RW)
    CONFIDENCE_THRESHOLD = 84,      ///< Min confidence (Float, RW)
    TOP_K_RESULTS = 85,             ///< Top-K count (Integer, RW)
    NMS_ENABLED = 86,               ///< Non-max suppression (Boolean, RW)
    NMS_IOU_THRESHOLD = 87,         ///< IoU threshold (Float, RW)
    NMS_SCORE_THRESHOLD = 88,       ///< Score threshold (Float, RW)
    MAX_DETECTIONS = 89,            ///< Max detection count (Integer, RW)

    // Advanced Features Resources (100-119)
    MODEL_METADATA = 100,           ///< Full JSON metadata (String, R)
    SUPPORTED_FORMATS = 101,        ///< Available formats (Opaque, R)
    SUPPORTED_ACCELERATORS = 102,   ///< Available backends (Opaque, R)
    HARDWARE_CAPABILITIES = 103,    ///< Device capabilities (String, R)
    BENCHMARK_RESULT = 104,         ///< Benchmark data (Opaque, R)
    RUN_BENCHMARK = 105,            ///< Execute benchmark (Exec)
    ENABLE_PROFILING = 106,         ///< Layer profiling (Boolean, RW)
    PROFILE_DATA = 107,             ///< Layer timings (Opaque, R)
    GPU_MEMORY_LIMIT = 108,         ///< GPU memory cap (Integer, RW)
    ALLOW_FP16 = 109,               ///< FP16 acceleration (Boolean, RW)
    CALIBRATION_DATA = 110          ///< Quantization data (Opaque, W)
};

/**
 * @brief Model format types
 */
enum class ModelFormat : uint8_t {
    TFLITE = 0,         ///< TensorFlow Lite (.tflite)
    ONNX = 1,           ///< ONNX (.onnx)
    PYTORCH = 2,        ///< PyTorch Mobile (.pt, .ptl)
    COREML = 3,         ///< Core ML (.mlmodel)
    TENSORRT = 4,       ///< TensorRT (.engine)
    OPENVINO = 5        ///< OpenVINO (.xml/.bin)
};

/**
 * @brief Model state machine
 */
enum class ModelState : uint8_t {
    IDLE = 0,           ///< No model loaded
    DOWNLOADING = 1,    ///< Download in progress
    VALIDATING = 2,     ///< Checksum verification
    LOADING = 3,        ///< Loading into memory
    READY = 4,          ///< Ready for inference
    RUNNING = 5,        ///< Inference in progress
    ERROR = 6,          ///< Error state
    UPDATING = 7        ///< Hot-swap in progress
};

/**
 * @brief Hardware accelerator types
 */
enum class AcceleratorType : uint8_t {
    AUTO = 0,           ///< Automatic selection
    CPU = 1,            ///< CPU inference
    GPU = 2,            ///< GPU delegate
    NPU = 3,            ///< Neural accelerator
    DSP = 4,            ///< DSP delegate
    TPU = 5,            ///< Edge TPU
    NNAPI = 6,          ///< Android NNAPI
    COREML = 7          ///< Core ML delegate
};

/**
 * @brief Quantization types
 */
enum class QuantizationType : uint8_t {
    NONE = 0,           ///< FP32 (no quantization)
    FP16 = 1,           ///< Half precision
    INT8 = 2,           ///< 8-bit integer
    INT4 = 3,           ///< 4-bit integer
    DYNAMIC = 4,        ///< Dynamic quantization
    MIXED = 5           ///< Mixed precision
};

/**
 * @brief Inference modes
 */
enum class InferenceMode : uint8_t {
    SYNCHRONOUS = 0,    ///< Blocking inference
    ASYNCHRONOUS = 1,   ///< Non-blocking with callback
    STREAMING = 2,      ///< Continuous frame processing
    BATCHED = 3         ///< Multiple inputs per call
};

/**
 * @brief Tensor information
 */
struct TensorInfo {
    std::string name;
    std::vector<int64_t> shape;
    std::string dtype;      // "float32", "uint8", "int8", etc.
    size_t sizeBytes;
    float scale = 1.0f;     // For quantized tensors
    int32_t zeroPoint = 0;  // For quantized tensors
};

/**
 * @brief Inference result
 */
struct InferenceResult {
    std::vector<uint8_t> outputData;
    std::vector<std::pair<int, float>> topKResults;  // class_id, confidence
    float latencyMs;
    bool success;
    std::string errorMessage;
};

/**
 * @brief Statistics structure
 */
struct InferenceStatistics {
    uint64_t inferenceCount = 0;
    float latencyAvg = 0.0f;
    float latencyMin = std::numeric_limits<float>::max();
    float latencyMax = 0.0f;
    float latencyP50 = 0.0f;
    float latencyP95 = 0.0f;
    float latencyP99 = 0.0f;
    float throughput = 0.0f;
    uint64_t memoryUsage = 0;
    float powerConsumption = 0.0f;
    uint64_t errorCount = 0;
    std::string lastError;
};

/**
 * @brief Pre-processing configuration
 */
struct PreprocessConfig {
    bool normalize = false;
    std::vector<float> mean;
    std::vector<float> stddev;
};

/**
 * @brief Post-processing configuration
 */
struct PostprocessConfig {
    bool applySoftmax = false;
    float confidenceThreshold = 0.5f;
    int topK = 5;
    bool nmsEnabled = false;
    float nmsIouThreshold = 0.45f;
    float nmsScoreThreshold = 0.25f;
    int maxDetections = 100;
};

/**
 * @brief Callback for inference completion
 */
using InferenceCallback = std::function<void(const InferenceResult& result)>;

/**
 * @brief Callback for model state changes
 */
using ModelStateCallback = std::function<void(ModelState oldState,
                                               ModelState newState)>;

/**
 * @brief Callback for download progress
 */
using DownloadProgressCallback = std::function<void(int progress,
                                                     const std::string& status)>;

/**
 * @brief Edge AI Inference Object Implementation
 */
class EdgeAIInferenceObject {
public:
    /**
     * @brief Configuration structure
     */
    struct Config {
        std::string modelPath;
        ModelFormat format = ModelFormat::TFLITE;
        AcceleratorType accelerator = AcceleratorType::AUTO;
        int numThreads = 4;
        uint32_t inferenceTimeout = 5000;
        int batchSize = 1;
        bool enableProfiling = false;
        uint64_t gpuMemoryLimit = 0;
        bool allowFP16 = true;
    };

    /**
     * @brief Constructor
     * @param instanceId Instance ID
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
    // Resource Operations
    // ========================================================================

    bool read(EdgeAIResourceId resourceId, std::string& value) const;
    bool read(EdgeAIResourceId resourceId, int64_t& value) const;
    bool read(EdgeAIResourceId resourceId, double& value) const;
    bool read(EdgeAIResourceId resourceId, bool& value) const;
    bool read(EdgeAIResourceId resourceId, std::vector<uint8_t>& value) const;

    bool write(EdgeAIResourceId resourceId, const std::string& value);
    bool write(EdgeAIResourceId resourceId, int64_t value);
    bool write(EdgeAIResourceId resourceId, double value);
    bool write(EdgeAIResourceId resourceId, bool value);
    bool write(EdgeAIResourceId resourceId, const std::vector<uint8_t>& value);

    bool execute(EdgeAIResourceId resourceId, const std::string& args = "");

    // ========================================================================
    // Model Management
    // ========================================================================

    /**
     * @brief Download model from URI
     * @param uri Model URI
     * @return true if download started
     */
    bool downloadModel(const std::string& uri);

    /**
     * @brief Load model from file
     * @param path Model file path
     * @return true if loaded successfully
     */
    bool loadModel(const std::string& path);

    /**
     * @brief Load model from memory
     * @param data Model data
     * @param format Model format
     * @return true if loaded successfully
     */
    bool loadModel(const std::vector<uint8_t>& data, ModelFormat format);

    /**
     * @brief Unload current model
     */
    void unloadModel();

    /**
     * @brief Get current model state
     * @return Model state
     */
    ModelState getModelState() const { return modelState_; }

    /**
     * @brief Check if model is ready for inference
     * @return true if ready
     */
    bool isReady() const { return modelState_ == ModelState::READY; }

    // ========================================================================
    // Inference Operations
    // ========================================================================

    /**
     * @brief Run synchronous inference
     * @param inputData Input tensor data
     * @return Inference result
     */
    InferenceResult runInference(const std::vector<uint8_t>& inputData);

    /**
     * @brief Run asynchronous inference
     * @param inputData Input tensor data
     * @param callback Completion callback
     * @return true if started
     */
    bool runInferenceAsync(const std::vector<uint8_t>& inputData,
                           InferenceCallback callback);

    /**
     * @brief Run batched inference
     * @param batchData Vector of input data
     * @return Vector of results
     */
    std::vector<InferenceResult> runBatchInference(
        const std::vector<std::vector<uint8_t>>& batchData);

    /**
     * @brief Cancel ongoing inference
     */
    void cancelInference();

    // ========================================================================
    // Tensor Information
    // ========================================================================

    /**
     * @brief Get input tensor info
     * @return Vector of input tensor information
     */
    std::vector<TensorInfo> getInputTensorInfo() const;

    /**
     * @brief Get output tensor info
     * @return Vector of output tensor information
     */
    std::vector<TensorInfo> getOutputTensorInfo() const;

    /**
     * @brief Get class labels
     * @return Vector of label strings
     */
    std::vector<std::string> getLabels() const;

    // ========================================================================
    // Configuration
    // ========================================================================

    /**
     * @brief Set accelerator type
     * @param accelerator Accelerator to use
     */
    void setAccelerator(AcceleratorType accelerator);

    /**
     * @brief Set number of threads
     * @param threads Thread count
     */
    void setNumThreads(int threads);

    /**
     * @brief Set inference mode
     * @param mode Inference mode
     */
    void setInferenceMode(InferenceMode mode);

    /**
     * @brief Set pre-processing configuration
     * @param config Pre-processing config
     */
    void setPreprocessConfig(const PreprocessConfig& config);

    /**
     * @brief Set post-processing configuration
     * @param config Post-processing config
     */
    void setPostprocessConfig(const PostprocessConfig& config);

    // ========================================================================
    // Statistics
    // ========================================================================

    /**
     * @brief Get inference statistics
     * @return Statistics structure
     */
    InferenceStatistics getStatistics() const;

    /**
     * @brief Reset statistics
     */
    void resetStatistics();

    /**
     * @brief Run benchmark
     * @param iterations Number of iterations
     * @return Benchmark result JSON
     */
    std::string runBenchmark(int iterations = 100);

    // ========================================================================
    // Callbacks
    // ========================================================================

    void setModelStateCallback(ModelStateCallback callback);
    void setDownloadProgressCallback(DownloadProgressCallback callback);

    // ========================================================================
    // Object Information
    // ========================================================================

    static uint16_t getObjectId() { return EDGE_AI_INFERENCE_OBJECT_ID; }
    uint16_t getInstanceId() const { return instanceId_; }

    static bool isReadable(EdgeAIResourceId resourceId);
    static bool isWritable(EdgeAIResourceId resourceId);
    static bool isExecutable(EdgeAIResourceId resourceId);

private:
    uint16_t instanceId_;
    Config config_;

    // Model state
    std::atomic<ModelState> modelState_{ModelState::IDLE};
    std::string modelName_;
    std::string modelVersion_;
    std::string modelUri_;
    ModelFormat modelFormat_ = ModelFormat::TFLITE;
    uint64_t modelSize_ = 0;
    std::vector<uint8_t> modelChecksum_;
    std::string modelDescription_;
    std::string modelAuthor_;
    std::string modelLicense_;
    int64_t creationDate_ = 0;

    // Tensor info
    std::vector<TensorInfo> inputTensors_;
    std::vector<TensorInfo> outputTensors_;
    std::vector<std::string> labels_;

    // Inference configuration
    InferenceMode inferenceMode_ = InferenceMode::SYNCHRONOUS;
    AcceleratorType accelerator_ = AcceleratorType::AUTO;
    QuantizationType quantization_ = QuantizationType::NONE;
    int numThreads_ = 4;
    uint32_t inferenceTimeout_ = 5000;
    int batchSize_ = 1;
    int priority_ = 0;

    // Pre/post processing
    PreprocessConfig preprocessConfig_;
    PostprocessConfig postprocessConfig_;

    // Statistics
    mutable std::mutex statsMutex_;
    InferenceStatistics stats_;
    std::vector<float> latencyHistory_;
    static constexpr size_t MAX_LATENCY_HISTORY = 1000;

    // Profiling
    bool profilingEnabled_ = false;
    std::vector<uint8_t> profileData_;

    // Download state
    std::atomic<int> downloadProgress_{0};

    // Callbacks
    ModelStateCallback modelStateCallback_;
    DownloadProgressCallback downloadProgressCallback_;

    // Internal methods
    void setModelState(ModelState newState);
    void updateStatistics(float latencyMs, bool success, const std::string& error = "");
    void calculatePercentiles();
    std::vector<uint8_t> preprocess(const std::vector<uint8_t>& input) const;
    InferenceResult postprocess(const std::vector<uint8_t>& output) const;
};

/**
 * @brief Factory for creating Edge AI Inference Object instances
 */
class EdgeAIInferenceObjectFactory {
public:
    static std::unique_ptr<EdgeAIInferenceObject> create(uint16_t instanceId = 0);

    static std::unique_ptr<EdgeAIInferenceObject> create(
        const EdgeAIInferenceObject::Config& config,
        uint16_t instanceId = 0);

    /**
     * @brief Check if TensorFlow Lite is available
     */
    static bool isTFLiteAvailable();

    /**
     * @brief Check if ONNX Runtime is available
     */
    static bool isONNXAvailable();

    /**
     * @brief Get supported model formats
     */
    static std::vector<ModelFormat> getSupportedFormats();

    /**
     * @brief Get available accelerators
     */
    static std::vector<AcceleratorType> getAvailableAccelerators();
};

} // namespace objects
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_EDGE_AI_INFERENCE_OBJECT_H
