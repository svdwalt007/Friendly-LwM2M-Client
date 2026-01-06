/**
 * @file edge_ai_inference_object.cpp
 * @brief LwM2M Edge AI Inference Object Implementation
 * 
 * Provides on-device machine learning inference capabilities.
 * Supports TensorFlow Lite, ONNX Runtime, and custom model formats.
 * 
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "objects/edge_ai_inference_object.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <condition_variable>
#include <queue>
#include <filesystem>

// Conditional includes for ML backends
#ifdef WITH_TFLITE
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>
#include <tensorflow/lite/delegates/gpu/delegate.h>
#endif

#ifdef WITH_ONNX
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>
#endif

namespace lwm2m {
namespace objects {

// ============================================================================
// Model File Magic Numbers
// ============================================================================

namespace magic {
    // TFLite: "TFL3"
    constexpr uint8_t TFLITE[] = {0x54, 0x46, 0x4C, 0x33};
    // ONNX: starts with protobuf wire type
    constexpr uint8_t ONNX[] = {0x08, 0x00};  // Simplified
    // PyTorch: "PK" (ZIP format)
    constexpr uint8_t PYTORCH[] = {0x50, 0x4B};
    // TensorRT: serialized engine
    constexpr uint8_t TENSORRT[] = {0x70, 0x74, 0x72, 0x74};
}

// ============================================================================
// Helper Functions
// ============================================================================

namespace {

/**
 * @brief Calculate SHA256 checksum
 */
std::vector<uint8_t> calculateSha256(const std::vector<uint8_t>& data) {
    // Simplified - in production use OpenSSL or similar
    std::vector<uint8_t> hash(32, 0);
    // SHA256 implementation would go here
    return hash;
}

/**
 * @brief Get current timestamp in milliseconds
 */
double getCurrentTimeMs() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0;
}

/**
 * @brief Apply softmax to float array
 */
void applySoftmax(float* data, size_t size) {
    float maxVal = *std::max_element(data, data + size);
    float sum = 0.0f;
    
    for (size_t i = 0; i < size; i++) {
        data[i] = std::exp(data[i] - maxVal);
        sum += data[i];
    }
    
    for (size_t i = 0; i < size; i++) {
        data[i] /= sum;
    }
}

/**
 * @brief Non-maximum suppression for object detection
 */
std::vector<int> nonMaxSuppression(const std::vector<EdgeAIInferenceObject::DetectionBox>& boxes,
                                    float iouThreshold) {
    std::vector<int> indices;
    std::vector<bool> suppressed(boxes.size(), false);
    
    // Sort by confidence
    std::vector<int> order(boxes.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&boxes](int a, int b) {
        return boxes[a].confidence > boxes[b].confidence;
    });
    
    auto calculateIoU = [](const EdgeAIInferenceObject::DetectionBox& a,
                           const EdgeAIInferenceObject::DetectionBox& b) -> float {
        float x1 = std::max(a.x1, b.x1);
        float y1 = std::max(a.y1, b.y1);
        float x2 = std::min(a.x2, b.x2);
        float y2 = std::min(a.y2, b.y2);
        
        float intersection = std::max(0.0f, x2 - x1) * std::max(0.0f, y2 - y1);
        float areaA = (a.x2 - a.x1) * (a.y2 - a.y1);
        float areaB = (b.x2 - b.x1) * (b.y2 - b.y1);
        float unionArea = areaA + areaB - intersection;
        
        return intersection / (unionArea + 1e-6f);
    };
    
    for (int i : order) {
        if (suppressed[i]) continue;
        
        indices.push_back(i);
        
        for (int j : order) {
            if (suppressed[j] || i == j) continue;
            
            if (calculateIoU(boxes[i], boxes[j]) > iouThreshold) {
                suppressed[j] = true;
            }
        }
    }
    
    return indices;
}

} // anonymous namespace

// ============================================================================
// EdgeAIInferenceObject::Impl
// ============================================================================

class EdgeAIInferenceObject::Impl {
public:
    explicit Impl(uint16_t instanceId, const Config& config)
        : instanceId_(instanceId)
        , config_(config)
        , modelState_(ModelState::IDLE)
        , profilingEnabled_(false)
        , streamingActive_(false)
    {
        // Initialize storage path
        std::filesystem::create_directories(config_.modelStoragePath);
    }
    
    ~Impl() {
        stopStreaming();
        unloadModel();
    }
    
    bool initialize() {
        // Detect hardware capabilities
        detectHardwareCapabilities();
        
        // Check available backends
        detectAvailableBackends();
        
        // Auto-load last model if configured
        if (config_.autoLoadLastModel) {
            std::string lastModelPath = config_.modelStoragePath + "/last_model";
            if (std::filesystem::exists(lastModelPath)) {
                // Load the saved model path and try to load it
            }
        }
        
        return true;
    }
    
    // LwM2M Resource Interface
    bool read(ResourceId resourceId, std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        switch (resourceId) {
            case ResourceId::MODEL_NAME:
                value = metadata_.name;
                return true;
            case ResourceId::MODEL_VERSION:
                value = metadata_.version;
                return true;
            case ResourceId::MODEL_URI:
                value = modelUri_;
                return true;
            case ResourceId::MODEL_CHECKSUM:
                // Convert to hex string
                value = bytesToHex(modelChecksum_);
                return true;
            case ResourceId::LABELS:
                value = labelsToString(metadata_.labels);
                return true;
            case ResourceId::LAST_ERROR:
                value = lastError_;
                return true;
            case ResourceId::MODEL_METADATA:
                value = metadataToJson();
                return true;
            default:
                return false;
        }
    }
    
    bool read(ResourceId resourceId, int64_t& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        switch (resourceId) {
            case ResourceId::MODEL_FORMAT:
                value = static_cast<int64_t>(metadata_.format);
                return true;
            case ResourceId::MODEL_STATE:
                value = static_cast<int64_t>(modelState_.load());
                return true;
            case ResourceId::MODEL_SIZE:
                value = static_cast<int64_t>(metadata_.modelSizeBytes);
                return true;
            case ResourceId::INFERENCE_MODE:
                value = static_cast<int64_t>(inferenceConfig_.mode);
                return true;
            case ResourceId::ACCELERATOR_TYPE:
                value = static_cast<int64_t>(inferenceConfig_.accelerator);
                return true;
            case ResourceId::NUM_THREADS:
                value = inferenceConfig_.numThreads;
                return true;
            case ResourceId::INFERENCE_TIMEOUT:
                value = inferenceConfig_.timeout;
                return true;
            case ResourceId::QUANTIZATION_TYPE:
                value = static_cast<int64_t>(metadata_.quantization);
                return true;
            case ResourceId::INFERENCE_COUNT:
                value = static_cast<int64_t>(stats_.totalInferences);
                return true;
            case ResourceId::MEMORY_USAGE:
                value = static_cast<int64_t>(stats_.peakMemoryBytes);
                return true;
            case ResourceId::ERROR_COUNT:
                value = static_cast<int64_t>(stats_.failedInferences);
                return true;
            case ResourceId::DOWNLOAD_PROGRESS:
                value = downloadProgress_;
                return true;
            case ResourceId::TOP_K_RESULTS:
                value = preprocessConfig_.topK;
                return true;
            default:
                return false;
        }
    }
    
    bool read(ResourceId resourceId, double& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        switch (resourceId) {
            case ResourceId::INFERENCE_LATENCY_AVG:
                value = stats_.avgLatencyMs;
                return true;
            case ResourceId::INFERENCE_LATENCY_MIN:
                value = stats_.minLatencyMs;
                return true;
            case ResourceId::INFERENCE_LATENCY_MAX:
                value = stats_.maxLatencyMs;
                return true;
            case ResourceId::INFERENCE_THROUGHPUT:
                value = stats_.avgThroughput;
                return true;
            case ResourceId::POWER_CONSUMPTION:
                value = stats_.avgPowerMw;
                return true;
            case ResourceId::CONFIDENCE_THRESHOLD:
                value = preprocessConfig_.confidenceThreshold;
                return true;
            case ResourceId::NMS_THRESHOLD:
                value = preprocessConfig_.nmsThreshold;
                return true;
            default:
                return false;
        }
    }
    
    bool read(ResourceId resourceId, std::vector<uint8_t>& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        switch (resourceId) {
            case ResourceId::OUTPUT_DATA:
                if (!lastOutputs_.empty()) {
                    value = lastOutputs_[0].data;
                    return true;
                }
                return false;
            case ResourceId::INPUT_TENSOR_INFO:
                value = serializeTensorInfo(metadata_.inputs);
                return true;
            case ResourceId::OUTPUT_TENSOR_INFO:
                value = serializeTensorInfo(metadata_.outputs);
                return true;
            case ResourceId::PROFILE_DATA:
                if (lastProfile_) {
                    value = serializeProfileData(*lastProfile_);
                    return true;
                }
                return false;
            default:
                return false;
        }
    }
    
    bool write(ResourceId resourceId, const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        switch (resourceId) {
            case ResourceId::MODEL_URI:
                modelUri_ = value;
                return true;
            default:
                return false;
        }
    }
    
    bool write(ResourceId resourceId, int64_t value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        switch (resourceId) {
            case ResourceId::INFERENCE_MODE:
                inferenceConfig_.mode = static_cast<InferenceMode>(value);
                return true;
            case ResourceId::ACCELERATOR_TYPE:
                inferenceConfig_.accelerator = static_cast<AcceleratorType>(value);
                return true;
            case ResourceId::NUM_THREADS:
                inferenceConfig_.numThreads = static_cast<uint32_t>(value);
                return true;
            case ResourceId::INFERENCE_TIMEOUT:
                inferenceConfig_.timeout = static_cast<uint32_t>(value);
                return true;
            case ResourceId::TOP_K_RESULTS:
                preprocessConfig_.topK = static_cast<int>(value);
                return true;
            default:
                return false;
        }
    }
    
    bool write(ResourceId resourceId, double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        switch (resourceId) {
            case ResourceId::CONFIDENCE_THRESHOLD:
                preprocessConfig_.confidenceThreshold = static_cast<float>(value);
                return true;
            case ResourceId::NMS_THRESHOLD:
                preprocessConfig_.nmsThreshold = static_cast<float>(value);
                return true;
            default:
                return false;
        }
    }
    
    bool write(ResourceId resourceId, const std::vector<uint8_t>& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        switch (resourceId) {
            case ResourceId::INPUT_DATA:
                lastInputData_ = value;
                return true;
            case ResourceId::MODEL_CHECKSUM:
                modelChecksum_ = value;
                return true;
            case ResourceId::INPUT_MEAN:
                preprocessConfig_.mean = bytesToFloats(value);
                return true;
            case ResourceId::INPUT_STDDEV:
                preprocessConfig_.stddev = bytesToFloats(value);
                return true;
            default:
                return false;
        }
    }
    
    bool execute(ResourceId resourceId, const std::string& arguments) {
        switch (resourceId) {
            case ResourceId::LOAD_MODEL:
                return loadModelFromStorage(arguments);
                
            case ResourceId::UNLOAD_MODEL:
                return unloadModel();
                
            case ResourceId::DOWNLOAD_MODEL:
                return startModelDownload(arguments);
                
            case ResourceId::RUN_INFERENCE:
                return runInferenceFromResource();
                
            case ResourceId::RESET_STATISTICS:
                resetStatistics();
                return true;
                
            case ResourceId::RUN_BENCHMARK:
                {
                    int iterations = 100;
                    int warmup = 10;
                    if (!arguments.empty()) {
                        // Parse iterations,warmup from arguments
                    }
                    runBenchmark(iterations, warmup);
                    return true;
                }
                
            default:
                return false;
        }
    }
    
    // Model Management
    bool downloadModel(const ModelDownloadConfig& config,
                        DownloadProgressCallback callback) {
        if (modelState_ == ModelState::DOWNLOADING) {
            lastError_ = "Download already in progress";
            return false;
        }
        
        setState(ModelState::DOWNLOADING);
        downloadConfig_ = config;
        downloadCallback_ = callback;
        downloadProgress_ = 0;
        
        // Start download thread
        downloadThread_ = std::thread([this]() {
            performDownload();
        });
        downloadThread_.detach();
        
        return true;
    }
    
    bool loadModel(const std::string& filePath, ModelFormat format) {
        // Read file
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            lastError_ = "Failed to open model file: " + filePath;
            setState(ModelState::ERROR);
            return false;
        }
        
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
        file.close();
        
        // Detect format if not specified
        if (format == ModelFormat::TFLITE) {
            format = ModelFormatDetector::fromMagic(data);
        }
        
        return loadModelFromMemory(data, format);
    }
    
    bool loadModelFromMemory(const std::vector<uint8_t>& data, ModelFormat format) {
        if (data.empty()) {
            lastError_ = "Model data is empty";
            setState(ModelState::ERROR);
            return false;
        }
        
        setState(ModelState::LOADING);
        
        // Validate checksum if provided
        if (!modelChecksum_.empty()) {
            auto calculated = calculateSha256(data);
            if (calculated != modelChecksum_) {
                lastError_ = "Model checksum mismatch";
                setState(ModelState::ERROR);
                return false;
            }
        }
        
        // Create appropriate backend
        bool success = false;
        
        switch (format) {
            case ModelFormat::TFLITE:
                success = loadTFLiteModel(data);
                break;
                
            case ModelFormat::ONNX:
                success = loadONNXModel(data);
                break;
                
            case ModelFormat::PYTORCH_MOBILE:
                success = loadPyTorchModel(data);
                break;
                
            default:
                lastError_ = "Unsupported model format";
                setState(ModelState::ERROR);
                return false;
        }
        
        if (success) {
            modelData_ = data;
            metadata_.format = format;
            metadata_.modelSizeBytes = data.size();
            setState(ModelState::READY);
            
            // Configure accelerator
            configureAccelerator();
        }
        
        return success;
    }
    
    bool unloadModel() {
        if (modelState_ == ModelState::RUNNING) {
            cancelInference();
        }
        
        stopStreaming();
        
        // Clean up backends
        tfliteBackend_.reset();
        onnxBackend_.reset();
        
        modelData_.clear();
        metadata_ = ModelMetadata{};
        
        setState(ModelState::IDLE);
        return true;
    }
    
    ModelState getModelState() const {
        return modelState_;
    }
    
    std::optional<ModelMetadata> getModelMetadata() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (modelState_ == ModelState::READY || modelState_ == ModelState::RUNNING) {
            return metadata_;
        }
        return std::nullopt;
    }
    
    bool validateModel(const std::vector<uint8_t>& checksum) {
        if (modelData_.empty()) {
            return false;
        }
        
        auto calculated = calculateSha256(modelData_);
        return calculated == checksum;
    }
    
    // Inference
    InferenceResult runInference(const std::vector<Tensor>& inputs,
                                  std::vector<Tensor>& outputs,
                                  const InferenceConfig& config) {
        if (modelState_ != ModelState::READY) {
            return InferenceResult::ERROR_MODEL_NOT_LOADED;
        }
        
        setState(ModelState::RUNNING);
        
        double startTime = getCurrentTimeMs();
        InferenceResult result = InferenceResult::SUCCESS;
        
        // Preprocess inputs if configured
        std::vector<Tensor> processedInputs;
        for (const auto& input : inputs) {
            processedInputs.push_back(preprocessInput(input));
        }
        
        // Run inference based on backend
        if (tfliteBackend_) {
            result = tfliteBackend_->invoke(processedInputs, outputs);
        } else if (onnxBackend_) {
            result = onnxBackend_->invoke(processedInputs, outputs);
        } else {
            result = InferenceResult::ERROR_MODEL_NOT_LOADED;
        }
        
        double endTime = getCurrentTimeMs();
        double latency = endTime - startTime;
        
        // Postprocess outputs if configured
        if (result == InferenceResult::SUCCESS) {
            for (auto& output : outputs) {
                output = postprocessOutput(output);
            }
            lastOutputs_ = outputs;
        }
        
        // Update statistics
        updateStatistics(result, latency);
        
        setState(ModelState::READY);
        return result;
    }
    
    bool runInferenceAsync(const std::vector<Tensor>& inputs,
                            InferenceCallback callback,
                            const InferenceConfig& config) {
        if (modelState_ != ModelState::READY) {
            return false;
        }
        
        asyncThread_ = std::thread([this, inputs, callback, config]() {
            std::vector<Tensor> outputs;
            double startTime = getCurrentTimeMs();
            
            InferenceResult result = runInference(inputs, outputs, config);
            
            double latency = getCurrentTimeMs() - startTime;
            
            if (callback) {
                callback(result, outputs, latency);
            }
        });
        asyncThread_.detach();
        
        return true;
    }
    
    bool startStreaming(StreamingCallback callback, const InferenceConfig& config) {
        if (modelState_ != ModelState::READY) {
            return false;
        }
        
        streamingCallback_ = callback;
        streamingConfig_ = config;
        streamingActive_ = true;
        
        streamingThread_ = std::thread([this]() {
            runStreamingLoop();
        });
        
        return true;
    }
    
    bool pushStreamingFrame(const std::vector<Tensor>& inputs, int frameId) {
        if (!streamingActive_) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(streamingMutex_);
        streamingQueue_.push({frameId, inputs});
        streamingCv_.notify_one();
        
        return true;
    }
    
    void stopStreaming() {
        streamingActive_ = false;
        streamingCv_.notify_all();
        
        if (streamingThread_.joinable()) {
            streamingThread_.join();
        }
    }
    
    void cancelInference() {
        // Signal cancellation
        inferenceCancelled_ = true;
    }
    
    // Pre/Post Processing
    void setPreprocessConfig(const PrePostProcessConfig& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        preprocessConfig_ = config;
    }
    
    PrePostProcessConfig getPreprocessConfig() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return preprocessConfig_;
    }
    
    Tensor preprocessInput(const Tensor& input) {
        Tensor result = input;
        
        if (input.shape.dataType != TensorDataType::FLOAT32) {
            return result;
        }
        
        float* data = result.as<float>();
        size_t size = input.shape.numElements();
        
        if (preprocessConfig_.normalizeInput) {
            // Normalize to [0, 1]
            for (size_t i = 0; i < size; i++) {
                data[i] /= 255.0f;
            }
        }
        
        // Apply mean subtraction and stddev division per channel
        if (!preprocessConfig_.mean.empty() && !preprocessConfig_.stddev.empty()) {
            size_t channels = preprocessConfig_.mean.size();
            size_t pixelsPerChannel = size / channels;
            
            for (size_t c = 0; c < channels; c++) {
                for (size_t i = 0; i < pixelsPerChannel; i++) {
                    size_t idx = c * pixelsPerChannel + i;
                    data[idx] = (data[idx] - preprocessConfig_.mean[c]) / 
                                preprocessConfig_.stddev[c];
                }
            }
        }
        
        return result;
    }
    
    Tensor postprocessOutput(const Tensor& output) {
        Tensor result = output;
        
        if (output.shape.dataType != TensorDataType::FLOAT32) {
            return result;
        }
        
        float* data = result.as<float>();
        size_t size = output.shape.numElements();
        
        if (preprocessConfig_.applySoftmax) {
            applySoftmax(data, size);
        }
        
        return result;
    }
    
    std::vector<std::pair<std::string, float>> decodeClassification(
        const Tensor& output, int topK) {
        
        std::vector<std::pair<std::string, float>> results;
        
        if (output.shape.dataType != TensorDataType::FLOAT32) {
            return results;
        }
        
        const float* data = output.as<float>();
        size_t numClasses = output.shape.numElements();
        
        // Create index array
        std::vector<size_t> indices(numClasses);
        std::iota(indices.begin(), indices.end(), 0);
        
        // Sort by confidence
        std::partial_sort(indices.begin(), 
                          indices.begin() + std::min(static_cast<size_t>(topK), numClasses),
                          indices.end(),
                          [data](size_t a, size_t b) {
                              return data[a] > data[b];
                          });
        
        // Get top-K results
        for (int i = 0; i < topK && i < static_cast<int>(numClasses); i++) {
            size_t idx = indices[i];
            float confidence = data[idx];
            
            if (confidence < preprocessConfig_.confidenceThreshold) {
                break;
            }
            
            std::string label;
            if (idx < metadata_.labels.size()) {
                label = metadata_.labels[idx];
            } else {
                label = "class_" + std::to_string(idx);
            }
            
            results.emplace_back(label, confidence);
        }
        
        return results;
    }
    
    std::vector<DetectionBox> decodeDetection(
        const std::vector<Tensor>& outputs,
        float confidenceThreshold,
        float nmsThreshold) {
        
        std::vector<DetectionBox> boxes;
        
        // Simplified YOLO-style output decoding
        // In production, this would be model-specific
        
        if (outputs.empty()) return boxes;
        
        const Tensor& output = outputs[0];
        const float* data = output.as<float>();
        
        // Assume output format: [batch, num_boxes, 5+num_classes]
        // where 5 = [x, y, w, h, objectness]
        
        size_t numBoxes = output.shape.dimensions.size() > 1 ? 
                          output.shape.dimensions[1] : output.shape.dimensions[0] / 85;
        size_t numClasses = metadata_.labels.size();
        if (numClasses == 0) numClasses = 80;  // Default COCO classes
        
        for (size_t i = 0; i < numBoxes; i++) {
            size_t offset = i * (5 + numClasses);
            
            float x = data[offset + 0];
            float y = data[offset + 1];
            float w = data[offset + 2];
            float h = data[offset + 3];
            float objectness = data[offset + 4];
            
            if (objectness < confidenceThreshold) continue;
            
            // Find best class
            int bestClass = 0;
            float bestClassScore = 0;
            
            for (size_t c = 0; c < numClasses; c++) {
                float classScore = data[offset + 5 + c];
                if (classScore > bestClassScore) {
                    bestClassScore = classScore;
                    bestClass = c;
                }
            }
            
            float confidence = objectness * bestClassScore;
            if (confidence < confidenceThreshold) continue;
            
            DetectionBox box;
            box.x1 = x - w / 2;
            box.y1 = y - h / 2;
            box.x2 = x + w / 2;
            box.y2 = y + h / 2;
            box.confidence = confidence;
            box.classId = bestClass;
            
            if (bestClass < static_cast<int>(metadata_.labels.size())) {
                box.label = metadata_.labels[bestClass];
            } else {
                box.label = "class_" + std::to_string(bestClass);
            }
            
            boxes.push_back(box);
        }
        
        // Apply NMS
        if (preprocessConfig_.nonMaxSuppression && !boxes.empty()) {
            auto keepIndices = nonMaxSuppression(boxes, nmsThreshold);
            
            std::vector<DetectionBox> filteredBoxes;
            for (int idx : keepIndices) {
                filteredBoxes.push_back(boxes[idx]);
            }
            boxes = std::move(filteredBoxes);
        }
        
        return boxes;
    }
    
    // Statistics
    InferenceStatistics getStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return stats_;
    }
    
    void resetStatistics() {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_ = InferenceStatistics{};
    }
    
    BenchmarkResult runBenchmark(int iterations, int warmupIterations) {
        BenchmarkResult result{};
        
        if (modelState_ != ModelState::READY) {
            lastError_ = "Model not loaded";
            return result;
        }
        
        // Create dummy input
        std::vector<Tensor> inputs;
        for (const auto& inputShape : metadata_.inputs) {
            inputs.push_back(Tensor::create(inputShape));
        }
        
        std::vector<double> latencies;
        
        // Warmup
        for (int i = 0; i < warmupIterations; i++) {
            std::vector<Tensor> outputs;
            runInference(inputs, outputs, inferenceConfig_);
        }
        
        // Benchmark
        for (int i = 0; i < iterations; i++) {
            std::vector<Tensor> outputs;
            
            double startTime = getCurrentTimeMs();
            runInference(inputs, outputs, inferenceConfig_);
            double endTime = getCurrentTimeMs();
            
            latencies.push_back(endTime - startTime);
        }
        
        // Calculate statistics
        std::sort(latencies.begin(), latencies.end());
        
        result.minLatencyMs = latencies.front();
        result.maxLatencyMs = latencies.back();
        result.avgLatencyMs = std::accumulate(latencies.begin(), latencies.end(), 0.0) / 
                               latencies.size();
        result.p50LatencyMs = latencies[latencies.size() / 2];
        result.p95LatencyMs = latencies[static_cast<size_t>(latencies.size() * 0.95)];
        result.p99LatencyMs = latencies[static_cast<size_t>(latencies.size() * 0.99)];
        result.throughput = 1000.0 / result.avgLatencyMs;
        result.memoryUsedBytes = stats_.peakMemoryBytes;
        result.acceleratorUsed = stats_.lastAccelerator;
        
        lastBenchmark_ = result;
        return result;
    }
    
    void enableProfiling(bool enable) {
        profilingEnabled_ = enable;
    }
    
    std::optional<ProfileData> getProfileData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return lastProfile_;
    }
    
    // Hardware
    std::vector<ModelFormat> getSupportedFormats() const {
        return supportedFormats_;
    }
    
    std::vector<AcceleratorType> getAvailableAccelerators() const {
        return availableAccelerators_;
    }
    
    HardwareCapabilities getHardwareCapabilities() const {
        return hardwareCapabilities_;
    }
    
    void setStateChangeCallback(StateChangeCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        stateChangeCallback_ = std::move(callback);
    }
    
    std::string getLastError() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return lastError_;
    }

private:
    uint16_t instanceId_;
    Config config_;
    std::atomic<ModelState> modelState_;
    
    mutable std::mutex mutex_;
    std::string lastError_;
    
    // Model data
    std::vector<uint8_t> modelData_;
    std::vector<uint8_t> modelChecksum_;
    std::string modelUri_;
    ModelMetadata metadata_;
    
    // Backends
    std::unique_ptr<TFLiteBackend> tfliteBackend_;
    std::unique_ptr<ONNXBackend> onnxBackend_;
    
    // Configuration
    InferenceConfig inferenceConfig_;
    PrePostProcessConfig preprocessConfig_;
    
    // Download
    ModelDownloadConfig downloadConfig_;
    DownloadProgressCallback downloadCallback_;
    std::atomic<int64_t> downloadProgress_{0};
    std::thread downloadThread_;
    
    // Async inference
    std::thread asyncThread_;
    std::atomic<bool> inferenceCancelled_{false};
    
    // Streaming
    std::atomic<bool> streamingActive_{false};
    std::thread streamingThread_;
    std::mutex streamingMutex_;
    std::condition_variable streamingCv_;
    std::queue<std::pair<int, std::vector<Tensor>>> streamingQueue_;
    StreamingCallback streamingCallback_;
    InferenceConfig streamingConfig_;
    
    // Results
    std::vector<Tensor> lastOutputs_;
    std::vector<uint8_t> lastInputData_;
    
    // Statistics
    InferenceStatistics stats_;
    BenchmarkResult lastBenchmark_;
    bool profilingEnabled_;
    std::optional<ProfileData> lastProfile_;
    
    // Hardware
    std::vector<ModelFormat> supportedFormats_;
    std::vector<AcceleratorType> availableAccelerators_;
    HardwareCapabilities hardwareCapabilities_;
    
    // Callbacks
    StateChangeCallback stateChangeCallback_;
    
    void setState(ModelState newState) {
        ModelState oldState = modelState_.exchange(newState);
        
        if (stateChangeCallback_ && oldState != newState) {
            stateChangeCallback_(oldState, newState, lastError_);
        }
    }
    
    void detectHardwareCapabilities() {
        // CPU detection
        hardwareCapabilities_.cpuCores = std::thread::hardware_concurrency();
        
        #if defined(__x86_64__) || defined(_M_X64)
            hardwareCapabilities_.cpuName = "x86_64";
        #elif defined(__aarch64__) || defined(_M_ARM64)
            hardwareCapabilities_.cpuName = "ARM64";
        #else
            hardwareCapabilities_.cpuName = "Unknown";
        #endif
        
        // RAM detection (simplified)
        #ifdef __linux__
            std::ifstream meminfo("/proc/meminfo");
            std::string line;
            while (std::getline(meminfo, line)) {
                if (line.find("MemTotal:") == 0) {
                    size_t kb;
                    sscanf(line.c_str(), "MemTotal: %zu kB", &kb);
                    hardwareCapabilities_.ramBytes = kb * 1024;
                    break;
                }
            }
        #endif
        
        // GPU detection would require platform-specific code
        hardwareCapabilities_.hasGPU = false;
        hardwareCapabilities_.hasNPU = false;
        hardwareCapabilities_.hasDSP = false;
        
        // Check for GPU via delegate availability
        #ifdef WITH_TFLITE
            // TFLite GPU delegate availability check would go here
        #endif
    }
    
    void detectAvailableBackends() {
        availableAccelerators_.push_back(AcceleratorType::CPU);
        
        #ifdef WITH_TFLITE
            supportedFormats_.push_back(ModelFormat::TFLITE);
            // Check for GPU delegate
            // Check for NNAPI delegate
        #endif
        
        #ifdef WITH_ONNX
            supportedFormats_.push_back(ModelFormat::ONNX);
        #endif
    }
    
    bool loadTFLiteModel(const std::vector<uint8_t>& data) {
        #ifdef WITH_TFLITE
            tfliteBackend_ = TFLiteBackend::create(data);
            if (tfliteBackend_) {
                metadata_.inputs = tfliteBackend_->getInputShapes();
                metadata_.outputs = tfliteBackend_->getOutputShapes();
                return true;
            }
        #endif
        
        // Fallback implementation for testing
        lastError_ = "TFLite backend not available";
        return createMockModel(data, ModelFormat::TFLITE);
    }
    
    bool loadONNXModel(const std::vector<uint8_t>& data) {
        #ifdef WITH_ONNX
            onnxBackend_ = ONNXBackend::create(data);
            if (onnxBackend_) {
                metadata_.inputs = onnxBackend_->getInputShapes();
                metadata_.outputs = onnxBackend_->getOutputShapes();
                return true;
            }
        #endif
        
        lastError_ = "ONNX backend not available";
        return createMockModel(data, ModelFormat::ONNX);
    }
    
    bool loadPyTorchModel(const std::vector<uint8_t>& data) {
        lastError_ = "PyTorch backend not implemented";
        return createMockModel(data, ModelFormat::PYTORCH_MOBILE);
    }
    
    bool createMockModel(const std::vector<uint8_t>& data, ModelFormat format) {
        // Create a mock model for testing without actual ML backends
        metadata_.name = "MockModel";
        metadata_.version = "1.0.0";
        metadata_.format = format;
        metadata_.modelSizeBytes = data.size();
        
        // Default input/output shapes
        TensorShape inputShape;
        inputShape.name = "input";
        inputShape.dimensions = {1, 224, 224, 3};
        inputShape.dataType = TensorDataType::FLOAT32;
        metadata_.inputs.push_back(inputShape);
        
        TensorShape outputShape;
        outputShape.name = "output";
        outputShape.dimensions = {1, 1000};
        outputShape.dataType = TensorDataType::FLOAT32;
        metadata_.outputs.push_back(outputShape);
        
        return true;
    }
    
    void configureAccelerator() {
        AcceleratorType accel = inferenceConfig_.accelerator;
        
        if (accel == AcceleratorType::AUTO) {
            // Select best available accelerator
            if (std::find(availableAccelerators_.begin(), 
                          availableAccelerators_.end(), 
                          AcceleratorType::NPU) != availableAccelerators_.end()) {
                accel = AcceleratorType::NPU;
            } else if (std::find(availableAccelerators_.begin(),
                                  availableAccelerators_.end(),
                                  AcceleratorType::GPU) != availableAccelerators_.end()) {
                accel = AcceleratorType::GPU;
            } else {
                accel = AcceleratorType::CPU;
            }
        }
        
        if (tfliteBackend_) {
            tfliteBackend_->setDelegate(accel);
            if (inferenceConfig_.numThreads > 0) {
                tfliteBackend_->setNumThreads(inferenceConfig_.numThreads);
            }
        }
        
        if (onnxBackend_) {
            onnxBackend_->setExecutionProvider(accel);
            if (inferenceConfig_.numThreads > 0) {
                onnxBackend_->setIntraOpThreads(inferenceConfig_.numThreads);
            }
        }
        
        stats_.lastAccelerator = accel;
    }
    
    void updateStatistics(InferenceResult result, double latencyMs) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        stats_.totalInferences++;
        stats_.lastInference = std::chrono::system_clock::now();
        
        if (result == InferenceResult::SUCCESS) {
            stats_.successfulInferences++;
            
            // Update latency stats
            if (stats_.successfulInferences == 1) {
                stats_.minLatencyMs = latencyMs;
                stats_.maxLatencyMs = latencyMs;
                stats_.avgLatencyMs = latencyMs;
            } else {
                stats_.minLatencyMs = std::min(stats_.minLatencyMs, latencyMs);
                stats_.maxLatencyMs = std::max(stats_.maxLatencyMs, latencyMs);
                
                // Running average
                double n = static_cast<double>(stats_.successfulInferences);
                stats_.avgLatencyMs = stats_.avgLatencyMs * (n - 1) / n + latencyMs / n;
            }
            
            stats_.avgThroughput = 1000.0 / stats_.avgLatencyMs;
        } else {
            stats_.failedInferences++;
        }
    }
    
    void performDownload() {
        // HTTP download implementation
        // In production, use libcurl or similar
        
        // Simulate download for now
        const size_t chunkSize = 65536;
        size_t totalSize = downloadConfig_.expectedSize;
        if (totalSize == 0) totalSize = 1000000;  // Assume 1MB
        
        std::vector<uint8_t> data;
        data.reserve(totalSize);
        
        for (size_t downloaded = 0; downloaded < totalSize; downloaded += chunkSize) {
            // Simulate chunk download
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            size_t thisChunk = std::min(chunkSize, totalSize - downloaded);
            for (size_t i = 0; i < thisChunk; i++) {
                data.push_back(0);  // Placeholder data
            }
            
            downloadProgress_ = (downloaded * 100) / totalSize;
            
            if (downloadCallback_) {
                downloadCallback_(downloaded, totalSize, chunkSize * 100.0);
            }
        }
        
        // Validate and save
        setState(ModelState::VALIDATING);
        
        if (!downloadConfig_.checksum.empty()) {
            auto calculated = calculateSha256(data);
            if (calculated != downloadConfig_.checksum) {
                lastError_ = "Download checksum mismatch";
                setState(ModelState::ERROR);
                return;
            }
        }
        
        // Save to storage
        std::string filename = config_.modelStoragePath + "/downloaded_model";
        std::ofstream file(filename, std::ios::binary);
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
        
        // Load the model
        ModelFormat format = ModelFormatDetector::fromMagic(data);
        loadModelFromMemory(data, format);
    }
    
    bool loadModelFromStorage(const std::string& filename) {
        std::string path = filename;
        if (path.find('/') == std::string::npos) {
            path = config_.modelStoragePath + "/" + filename;
        }
        
        ModelFormat format = ModelFormatDetector::fromExtension(path);
        return loadModel(path, format);
    }
    
    bool startModelDownload(const std::string& uri) {
        ModelDownloadConfig config;
        config.uri = uri.empty() ? modelUri_ : uri;
        return downloadModel(config, nullptr);
    }
    
    bool runInferenceFromResource() {
        if (lastInputData_.empty()) {
            lastError_ = "No input data provided";
            return false;
        }
        
        // Convert input data to tensor
        Tensor inputTensor;
        inputTensor.shape = metadata_.inputs[0];
        inputTensor.data = lastInputData_;
        
        std::vector<Tensor> outputs;
        InferenceResult result = runInference({inputTensor}, outputs, inferenceConfig_);
        
        return result == InferenceResult::SUCCESS;
    }
    
    void runStreamingLoop() {
        while (streamingActive_) {
            std::pair<int, std::vector<Tensor>> frame;
            
            {
                std::unique_lock<std::mutex> lock(streamingMutex_);
                streamingCv_.wait(lock, [this]() {
                    return !streamingQueue_.empty() || !streamingActive_;
                });
                
                if (!streamingActive_) break;
                
                frame = std::move(streamingQueue_.front());
                streamingQueue_.pop();
            }
            
            // Run inference
            std::vector<Tensor> outputs;
            double startTime = getCurrentTimeMs();
            
            runInference(frame.second, outputs, streamingConfig_);
            
            double latency = getCurrentTimeMs() - startTime;
            
            if (streamingCallback_) {
                streamingCallback_(frame.first, outputs, latency);
            }
        }
    }
    
    // Helper functions
    static std::string bytesToHex(const std::vector<uint8_t>& bytes) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (uint8_t b : bytes) {
            oss << std::setw(2) << static_cast<int>(b);
        }
        return oss.str();
    }
    
    static std::string labelsToString(const std::vector<std::string>& labels) {
        std::ostringstream oss;
        for (size_t i = 0; i < labels.size(); i++) {
            if (i > 0) oss << ",";
            oss << labels[i];
        }
        return oss.str();
    }
    
    static std::vector<float> bytesToFloats(const std::vector<uint8_t>& bytes) {
        std::vector<float> result;
        if (bytes.size() % sizeof(float) != 0) return result;
        
        size_t count = bytes.size() / sizeof(float);
        result.resize(count);
        std::memcpy(result.data(), bytes.data(), bytes.size());
        return result;
    }
    
    std::string metadataToJson() const {
        std::ostringstream oss;
        oss << "{";
        oss << "\"name\":\"" << metadata_.name << "\",";
        oss << "\"version\":\"" << metadata_.version << "\",";
        oss << "\"format\":" << static_cast<int>(metadata_.format) << ",";
        oss << "\"quantization\":" << static_cast<int>(metadata_.quantization) << ",";
        oss << "\"sizeBytes\":" << metadata_.modelSizeBytes << ",";
        oss << "\"runtimeMemory\":" << metadata_.runtimeMemoryBytes << ",";
        oss << "\"estimatedLatency\":" << metadata_.estimatedLatencyMs;
        oss << "}";
        return oss.str();
    }
    
    static std::vector<uint8_t> serializeTensorInfo(const std::vector<TensorShape>& shapes) {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < shapes.size(); i++) {
            if (i > 0) oss << ",";
            oss << "{\"name\":\"" << shapes[i].name << "\",";
            oss << "\"dtype\":" << static_cast<int>(shapes[i].dataType) << ",";
            oss << "\"shape\":[";
            for (size_t j = 0; j < shapes[i].dimensions.size(); j++) {
                if (j > 0) oss << ",";
                oss << shapes[i].dimensions[j];
            }
            oss << "]}";
        }
        oss << "]";
        std::string str = oss.str();
        return std::vector<uint8_t>(str.begin(), str.end());
    }
    
    static std::vector<uint8_t> serializeProfileData(const ProfileData& profile) {
        std::ostringstream oss;
        oss << "{\"totalTimeMs\":" << profile.totalTimeMs << ",";
        oss << "\"totalMemoryBytes\":" << profile.totalMemoryBytes << ",";
        oss << "\"layers\":[";
        for (size_t i = 0; i < profile.layers.size(); i++) {
            if (i > 0) oss << ",";
            oss << "{\"name\":\"" << profile.layers[i].name << "\",";
            oss << "\"type\":\"" << profile.layers[i].type << "\",";
            oss << "\"timeMs\":" << profile.layers[i].timeMs << ",";
            oss << "\"memoryBytes\":" << profile.layers[i].memoryBytes << "}";
        }
        oss << "]}";
        std::string str = oss.str();
        return std::vector<uint8_t>(str.begin(), str.end());
    }
};

// ============================================================================
// EdgeAIInferenceObject Public Implementation
// ============================================================================

EdgeAIInferenceObject::EdgeAIInferenceObject(uint16_t instanceId, const Config& config)
    : impl_(std::make_unique<Impl>(instanceId, config)) {
}

EdgeAIInferenceObject::~EdgeAIInferenceObject() = default;

bool EdgeAIInferenceObject::initialize() {
    return impl_->initialize();
}

bool EdgeAIInferenceObject::read(ResourceId resourceId, std::string& value) {
    return impl_->read(resourceId, value);
}

bool EdgeAIInferenceObject::read(ResourceId resourceId, int64_t& value) {
    return impl_->read(resourceId, value);
}

bool EdgeAIInferenceObject::read(ResourceId resourceId, double& value) {
    return impl_->read(resourceId, value);
}

bool EdgeAIInferenceObject::read(ResourceId resourceId, std::vector<uint8_t>& value) {
    return impl_->read(resourceId, value);
}

bool EdgeAIInferenceObject::write(ResourceId resourceId, const std::string& value) {
    return impl_->write(resourceId, value);
}

bool EdgeAIInferenceObject::write(ResourceId resourceId, int64_t value) {
    return impl_->write(resourceId, value);
}

bool EdgeAIInferenceObject::write(ResourceId resourceId, double value) {
    return impl_->write(resourceId, value);
}

bool EdgeAIInferenceObject::write(ResourceId resourceId, const std::vector<uint8_t>& value) {
    return impl_->write(resourceId, value);
}

bool EdgeAIInferenceObject::execute(ResourceId resourceId, const std::string& arguments) {
    return impl_->execute(resourceId, arguments);
}

bool EdgeAIInferenceObject::downloadModel(const ModelDownloadConfig& config,
                                           DownloadProgressCallback callback) {
    return impl_->downloadModel(config, callback);
}

bool EdgeAIInferenceObject::loadModel(const std::string& filePath, ModelFormat format) {
    return impl_->loadModel(filePath, format);
}

bool EdgeAIInferenceObject::loadModelFromMemory(const std::vector<uint8_t>& data,
                                                  ModelFormat format) {
    return impl_->loadModelFromMemory(data, format);
}

bool EdgeAIInferenceObject::unloadModel() {
    return impl_->unloadModel();
}

ModelState EdgeAIInferenceObject::getModelState() const {
    return impl_->getModelState();
}

std::optional<ModelMetadata> EdgeAIInferenceObject::getModelMetadata() const {
    return impl_->getModelMetadata();
}

bool EdgeAIInferenceObject::validateModel(const std::vector<uint8_t>& checksum) {
    return impl_->validateModel(checksum);
}

InferenceResult EdgeAIInferenceObject::runInference(const std::vector<Tensor>& inputs,
                                                      std::vector<Tensor>& outputs,
                                                      const InferenceConfig& config) {
    return impl_->runInference(inputs, outputs, config);
}

bool EdgeAIInferenceObject::runInferenceAsync(const std::vector<Tensor>& inputs,
                                                InferenceCallback callback,
                                                const InferenceConfig& config) {
    return impl_->runInferenceAsync(inputs, callback, config);
}

bool EdgeAIInferenceObject::startStreaming(StreamingCallback callback,
                                            const InferenceConfig& config) {
    return impl_->startStreaming(callback, config);
}

bool EdgeAIInferenceObject::pushStreamingFrame(const std::vector<Tensor>& inputs, 
                                                 int frameId) {
    return impl_->pushStreamingFrame(inputs, frameId);
}

void EdgeAIInferenceObject::stopStreaming() {
    impl_->stopStreaming();
}

void EdgeAIInferenceObject::cancelInference() {
    impl_->cancelInference();
}

void EdgeAIInferenceObject::setPreprocessConfig(const PrePostProcessConfig& config) {
    impl_->setPreprocessConfig(config);
}

PrePostProcessConfig EdgeAIInferenceObject::getPreprocessConfig() const {
    return impl_->getPreprocessConfig();
}

Tensor EdgeAIInferenceObject::preprocessInput(const Tensor& input) {
    return impl_->preprocessInput(input);
}

Tensor EdgeAIInferenceObject::postprocessOutput(const Tensor& output) {
    return impl_->postprocessOutput(output);
}

std::vector<std::pair<std::string, float>> EdgeAIInferenceObject::decodeClassification(
    const Tensor& output, int topK) {
    return impl_->decodeClassification(output, topK);
}

std::vector<EdgeAIInferenceObject::DetectionBox> EdgeAIInferenceObject::decodeDetection(
    const std::vector<Tensor>& outputs,
    float confidenceThreshold,
    float nmsThreshold) {
    return impl_->decodeDetection(outputs, confidenceThreshold, nmsThreshold);
}

InferenceStatistics EdgeAIInferenceObject::getStatistics() const {
    return impl_->getStatistics();
}

void EdgeAIInferenceObject::resetStatistics() {
    impl_->resetStatistics();
}

EdgeAIInferenceObject::BenchmarkResult EdgeAIInferenceObject::runBenchmark(
    int iterations, int warmupIterations) {
    return impl_->runBenchmark(iterations, warmupIterations);
}

void EdgeAIInferenceObject::enableProfiling(bool enable) {
    impl_->enableProfiling(enable);
}

std::optional<EdgeAIInferenceObject::ProfileData> EdgeAIInferenceObject::getProfileData() const {
    return impl_->getProfileData();
}

std::vector<ModelFormat> EdgeAIInferenceObject::getSupportedFormats() const {
    return impl_->getSupportedFormats();
}

std::vector<AcceleratorType> EdgeAIInferenceObject::getAvailableAccelerators() const {
    return impl_->getAvailableAccelerators();
}

EdgeAIInferenceObject::HardwareCapabilities EdgeAIInferenceObject::getHardwareCapabilities() const {
    return impl_->getHardwareCapabilities();
}

void EdgeAIInferenceObject::setStateChangeCallback(StateChangeCallback callback) {
    impl_->setStateChangeCallback(std::move(callback));
}

std::string EdgeAIInferenceObject::getLastError() const {
    return impl_->getLastError();
}

// ============================================================================
// ModelFormatDetector Implementation
// ============================================================================

ModelFormat ModelFormatDetector::fromExtension(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos) {
        return ModelFormat::CUSTOM;
    }
    
    std::string ext = filename.substr(pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".tflite") return ModelFormat::TFLITE;
    if (ext == ".onnx") return ModelFormat::ONNX;
    if (ext == ".pt" || ext == ".ptl") return ModelFormat::PYTORCH_MOBILE;
    if (ext == ".mlmodel" || ext == ".mlpackage") return ModelFormat::CORE_ML;
    if (ext == ".engine" || ext == ".plan") return ModelFormat::TENSOR_RT;
    if (ext == ".xml" || ext == ".bin") return ModelFormat::OPENVINO;
    
    return ModelFormat::CUSTOM;
}

ModelFormat ModelFormatDetector::fromMagic(const std::vector<uint8_t>& data) {
    if (data.size() < 4) {
        return ModelFormat::CUSTOM;
    }
    
    // Check TFLite magic
    if (std::equal(std::begin(magic::TFLITE), std::end(magic::TFLITE), data.begin())) {
        return ModelFormat::TFLITE;
    }
    
    // Check PyTorch (ZIP format)
    if (std::equal(std::begin(magic::PYTORCH), std::end(magic::PYTORCH), data.begin())) {
        return ModelFormat::PYTORCH_MOBILE;
    }
    
    // Check TensorRT
    if (std::equal(std::begin(magic::TENSORRT), std::end(magic::TENSORRT), data.begin())) {
        return ModelFormat::TENSOR_RT;
    }
    
    // ONNX uses protobuf, harder to detect by magic
    // Default to ONNX if it looks like protobuf
    if (data[0] == 0x08 || data[0] == 0x0A) {
        return ModelFormat::ONNX;
    }
    
    return ModelFormat::CUSTOM;
}

std::string ModelFormatDetector::getExtension(ModelFormat format) {
    switch (format) {
        case ModelFormat::TFLITE: return ".tflite";
        case ModelFormat::ONNX: return ".onnx";
        case ModelFormat::PYTORCH_MOBILE: return ".ptl";
        case ModelFormat::CORE_ML: return ".mlmodel";
        case ModelFormat::TENSOR_RT: return ".engine";
        case ModelFormat::OPENVINO: return ".xml";
        default: return ".bin";
    }
}

std::string ModelFormatDetector::getName(ModelFormat format) {
    switch (format) {
        case ModelFormat::TFLITE: return "TensorFlow Lite";
        case ModelFormat::ONNX: return "ONNX";
        case ModelFormat::PYTORCH_MOBILE: return "PyTorch Mobile";
        case ModelFormat::CORE_ML: return "Core ML";
        case ModelFormat::TENSOR_RT: return "TensorRT";
        case ModelFormat::OPENVINO: return "OpenVINO";
        default: return "Custom";
    }
}

bool ModelFormatDetector::isSupported(ModelFormat format) {
    switch (format) {
        case ModelFormat::TFLITE:
            #ifdef WITH_TFLITE
                return true;
            #else
                return false;
            #endif
            
        case ModelFormat::ONNX:
            #ifdef WITH_ONNX
                return true;
            #else
                return false;
            #endif
            
        default:
            return false;
    }
}

// ============================================================================
// TFLiteBackend Stub Implementation
// ============================================================================

bool TFLiteBackend::isAvailable() {
    #ifdef WITH_TFLITE
        return true;
    #else
        return false;
    #endif
}

std::unique_ptr<TFLiteBackend> TFLiteBackend::create(const std::vector<uint8_t>& modelData) {
    #ifdef WITH_TFLITE
        // Real TFLite implementation would go here
        return nullptr;
    #else
        return nullptr;
    #endif
}

// ============================================================================
// ONNXBackend Stub Implementation
// ============================================================================

bool ONNXBackend::isAvailable() {
    #ifdef WITH_ONNX
        return true;
    #else
        return false;
    #endif
}

std::unique_ptr<ONNXBackend> ONNXBackend::create(const std::vector<uint8_t>& modelData) {
    #ifdef WITH_ONNX
        // Real ONNX Runtime implementation would go here
        return nullptr;
    #else
        return nullptr;
    #endif
}

} // namespace objects
} // namespace lwm2m
