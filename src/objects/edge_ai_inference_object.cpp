/**
 * @file edge_ai_inference_object.cpp
 * @brief OMA LwM2M Edge AI Inference Object (ID: 33410) Implementation
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "objects/edge_ai_inference_object.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include <thread>
#include <future>

// Optional TensorFlow Lite support
#ifdef WITH_TFLITE
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"
#endif

// Optional ONNX Runtime support
#ifdef WITH_ONNX
#include "onnxruntime_cxx_api.h"
#endif

namespace lwm2m {
namespace objects {

// ============================================================================
// Constructor / Destructor
// ============================================================================

EdgeAIInferenceObject::EdgeAIInferenceObject(uint16_t instanceId, const Config& config)
    : instanceId_(instanceId)
    , config_(config)
    , accelerator_(config.accelerator)
    , numThreads_(config.numThreads)
    , inferenceTimeout_(config.inferenceTimeout)
    , batchSize_(config.batchSize)
    , profilingEnabled_(config.enableProfiling) {
}

EdgeAIInferenceObject::EdgeAIInferenceObject(uint16_t instanceId)
    : EdgeAIInferenceObject(instanceId, Config()) {
}

EdgeAIInferenceObject::~EdgeAIInferenceObject() {
    stopAllThreads();
    unloadModel();
}

// ============================================================================
// Initialization
// ============================================================================

bool EdgeAIInferenceObject::initialize() {
    // Initialize default preprocessing
    preprocessConfig_.normalize = false;
    preprocessConfig_.mean = {0.0f, 0.0f, 0.0f};
    preprocessConfig_.stddev = {1.0f, 1.0f, 1.0f};

    // Initialize default postprocessing
    postprocessConfig_.applySoftmax = false;
    postprocessConfig_.confidenceThreshold = 0.5f;
    postprocessConfig_.topK = 5;
    postprocessConfig_.nmsEnabled = false;
    postprocessConfig_.nmsIouThreshold = 0.45f;
    postprocessConfig_.nmsScoreThreshold = 0.25f;
    postprocessConfig_.maxDetections = 100;

    return true;
}

// ============================================================================
// Resource Read Operations
// ============================================================================

bool EdgeAIInferenceObject::read(EdgeAIResourceId resourceId, std::string& value) const {
    switch (resourceId) {
        case EdgeAIResourceId::MODEL_NAME:
            value = modelName_;
            return true;
        case EdgeAIResourceId::MODEL_VERSION:
            value = modelVersion_;
            return true;
        case EdgeAIResourceId::MODEL_URI:
            value = modelUri_;
            return true;
        case EdgeAIResourceId::MODEL_DESCRIPTION:
            value = modelDescription_;
            return true;
        case EdgeAIResourceId::MODEL_AUTHOR:
            value = modelAuthor_;
            return true;
        case EdgeAIResourceId::MODEL_LICENSE:
            value = modelLicense_;
            return true;
        case EdgeAIResourceId::MIN_RUNTIME_VERSION:
#ifdef WITH_TFLITE
            value = "TFLite 2.14.0";
#elif defined(WITH_ONNX)
            value = "ONNX Runtime 1.16.0";
#else
            value = "No runtime";
#endif
            return true;
        case EdgeAIResourceId::LABELS: {
            std::ostringstream oss;
            for (size_t i = 0; i < labels_.size(); ++i) {
                if (i > 0) oss << ",";
                oss << labels_[i];
            }
            value = oss.str();
            return true;
        }
        case EdgeAIResourceId::LAST_ERROR:
            value = stats_.lastError;
            return true;
        case EdgeAIResourceId::MODEL_METADATA: {
            // Return JSON metadata
            std::ostringstream oss;
            oss << "{\"name\":\"" << modelName_ << "\","
                << "\"version\":\"" << modelVersion_ << "\","
                << "\"format\":" << static_cast<int>(modelFormat_) << ","
                << "\"state\":" << static_cast<int>(modelState_.load()) << ","
                << "\"size\":" << modelSize_ << "}";
            value = oss.str();
            return true;
        }
        case EdgeAIResourceId::HARDWARE_CAPABILITIES: {
            std::ostringstream oss;
            oss << "{\"cpu\":true";
#ifdef WITH_TFLITE
            oss << ",\"tflite\":true";
#endif
#ifdef WITH_ONNX
            oss << ",\"onnx\":true";
#endif
            oss << ",\"threads\":" << std::thread::hardware_concurrency() << "}";
            value = oss.str();
            return true;
        }
        default:
            return false;
    }
}

bool EdgeAIInferenceObject::read(EdgeAIResourceId resourceId, int64_t& value) const {
    switch (resourceId) {
        case EdgeAIResourceId::MODEL_FORMAT:
            value = static_cast<int64_t>(modelFormat_);
            return true;
        case EdgeAIResourceId::MODEL_STATE:
            value = static_cast<int64_t>(modelState_.load());
            return true;
        case EdgeAIResourceId::MODEL_SIZE:
            value = static_cast<int64_t>(modelSize_);
            return true;
        case EdgeAIResourceId::DOWNLOAD_PROGRESS:
            value = downloadProgress_.load();
            return true;
        case EdgeAIResourceId::CREATION_DATE:
            value = creationDate_;
            return true;
        case EdgeAIResourceId::LABEL_COUNT:
            value = static_cast<int64_t>(labels_.size());
            return true;
        case EdgeAIResourceId::INPUT_COUNT:
            value = static_cast<int64_t>(inputTensors_.size());
            return true;
        case EdgeAIResourceId::OUTPUT_COUNT:
            value = static_cast<int64_t>(outputTensors_.size());
            return true;
        case EdgeAIResourceId::INFERENCE_MODE:
            value = static_cast<int64_t>(inferenceMode_);
            return true;
        case EdgeAIResourceId::ACCELERATOR_TYPE:
            value = static_cast<int64_t>(accelerator_);
            return true;
        case EdgeAIResourceId::NUM_THREADS:
            value = numThreads_;
            return true;
        case EdgeAIResourceId::INFERENCE_TIMEOUT:
            value = inferenceTimeout_;
            return true;
        case EdgeAIResourceId::QUANTIZATION_TYPE:
            value = static_cast<int64_t>(quantization_);
            return true;
        case EdgeAIResourceId::BATCH_SIZE:
            value = batchSize_;
            return true;
        case EdgeAIResourceId::PRIORITY:
            value = priority_;
            return true;
        case EdgeAIResourceId::INFERENCE_COUNT:
            value = static_cast<int64_t>(stats_.inferenceCount);
            return true;
        case EdgeAIResourceId::MEMORY_USAGE:
            value = static_cast<int64_t>(stats_.memoryUsage);
            return true;
        case EdgeAIResourceId::ERROR_COUNT:
            value = static_cast<int64_t>(stats_.errorCount);
            return true;
        case EdgeAIResourceId::TOP_K_RESULTS:
            value = postprocessConfig_.topK;
            return true;
        case EdgeAIResourceId::MAX_DETECTIONS:
            value = postprocessConfig_.maxDetections;
            return true;
        case EdgeAIResourceId::GPU_MEMORY_LIMIT:
            value = static_cast<int64_t>(config_.gpuMemoryLimit);
            return true;
        default:
            return false;
    }
}

bool EdgeAIInferenceObject::read(EdgeAIResourceId resourceId, double& value) const {
    switch (resourceId) {
        case EdgeAIResourceId::LATENCY_AVG:
            value = stats_.latencyAvg;
            return true;
        case EdgeAIResourceId::LATENCY_MIN:
            value = stats_.latencyMin;
            return true;
        case EdgeAIResourceId::LATENCY_MAX:
            value = stats_.latencyMax;
            return true;
        case EdgeAIResourceId::THROUGHPUT:
            value = stats_.throughput;
            return true;
        case EdgeAIResourceId::POWER_CONSUMPTION:
            value = stats_.powerConsumption;
            return true;
        case EdgeAIResourceId::LATENCY_P50:
            value = stats_.latencyP50;
            return true;
        case EdgeAIResourceId::LATENCY_P95:
            value = stats_.latencyP95;
            return true;
        case EdgeAIResourceId::LATENCY_P99:
            value = stats_.latencyP99;
            return true;
        case EdgeAIResourceId::CONFIDENCE_THRESHOLD:
            value = postprocessConfig_.confidenceThreshold;
            return true;
        case EdgeAIResourceId::NMS_IOU_THRESHOLD:
            value = postprocessConfig_.nmsIouThreshold;
            return true;
        case EdgeAIResourceId::NMS_SCORE_THRESHOLD:
            value = postprocessConfig_.nmsScoreThreshold;
            return true;
        default:
            return false;
    }
}

bool EdgeAIInferenceObject::read(EdgeAIResourceId resourceId, bool& value) const {
    switch (resourceId) {
        case EdgeAIResourceId::NORMALIZE_INPUT:
            value = preprocessConfig_.normalize;
            return true;
        case EdgeAIResourceId::APPLY_SOFTMAX:
            value = postprocessConfig_.applySoftmax;
            return true;
        case EdgeAIResourceId::NMS_ENABLED:
            value = postprocessConfig_.nmsEnabled;
            return true;
        case EdgeAIResourceId::ENABLE_PROFILING:
            value = profilingEnabled_;
            return true;
        case EdgeAIResourceId::ALLOW_FP16:
            value = config_.allowFP16;
            return true;
        default:
            return false;
    }
}

bool EdgeAIInferenceObject::read(EdgeAIResourceId resourceId, std::vector<uint8_t>& value) const {
    switch (resourceId) {
        case EdgeAIResourceId::MODEL_CHECKSUM:
            value = modelChecksum_;
            return true;
        case EdgeAIResourceId::INPUT_TENSOR_INFO: {
            // Serialize tensor info to JSON
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < inputTensors_.size(); ++i) {
                if (i > 0) oss << ",";
                const auto& t = inputTensors_[i];
                oss << "{\"name\":\"" << t.name << "\",\"shape\":[";
                for (size_t j = 0; j < t.shape.size(); ++j) {
                    if (j > 0) oss << ",";
                    oss << t.shape[j];
                }
                oss << "],\"dtype\":\"" << t.dtype << "\",\"size\":" << t.sizeBytes << "}";
            }
            oss << "]";
            std::string json = oss.str();
            value.assign(json.begin(), json.end());
            return true;
        }
        case EdgeAIResourceId::OUTPUT_TENSOR_INFO: {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < outputTensors_.size(); ++i) {
                if (i > 0) oss << ",";
                const auto& t = outputTensors_[i];
                oss << "{\"name\":\"" << t.name << "\",\"shape\":[";
                for (size_t j = 0; j < t.shape.size(); ++j) {
                    if (j > 0) oss << ",";
                    oss << t.shape[j];
                }
                oss << "],\"dtype\":\"" << t.dtype << "\",\"size\":" << t.sizeBytes << "}";
            }
            oss << "]";
            std::string json = oss.str();
            value.assign(json.begin(), json.end());
            return true;
        }
        case EdgeAIResourceId::OUTPUT_DATA:
            // Return last output data (would be set after inference)
            return true;
        case EdgeAIResourceId::SUPPORTED_FORMATS: {
            auto formats = EdgeAIInferenceObjectFactory::getSupportedFormats();
            value.clear();
            for (auto f : formats) {
                value.push_back(static_cast<uint8_t>(f));
            }
            return true;
        }
        case EdgeAIResourceId::SUPPORTED_ACCELERATORS: {
            auto accs = EdgeAIInferenceObjectFactory::getAvailableAccelerators();
            value.clear();
            for (auto a : accs) {
                value.push_back(static_cast<uint8_t>(a));
            }
            return true;
        }
        case EdgeAIResourceId::PROFILE_DATA:
            value = profileData_;
            return true;
        case EdgeAIResourceId::INPUT_MEAN: {
            // Convert floats to bytes
            value.resize(preprocessConfig_.mean.size() * sizeof(float));
            std::memcpy(value.data(), preprocessConfig_.mean.data(),
                       preprocessConfig_.mean.size() * sizeof(float));
            return true;
        }
        case EdgeAIResourceId::INPUT_STDDEV: {
            value.resize(preprocessConfig_.stddev.size() * sizeof(float));
            std::memcpy(value.data(), preprocessConfig_.stddev.data(),
                       preprocessConfig_.stddev.size() * sizeof(float));
            return true;
        }
        default:
            return false;
    }
}

// ============================================================================
// Resource Write Operations
// ============================================================================

bool EdgeAIInferenceObject::write(EdgeAIResourceId resourceId, const std::string& value) {
    if (!isWritable(resourceId)) return false;

    switch (resourceId) {
        case EdgeAIResourceId::MODEL_URI:
            modelUri_ = value;
            return true;
        default:
            return false;
    }
}

bool EdgeAIInferenceObject::write(EdgeAIResourceId resourceId, int64_t value) {
    if (!isWritable(resourceId)) return false;

    switch (resourceId) {
        case EdgeAIResourceId::INFERENCE_MODE:
            if (value < 0 || value > 3) return false;
            inferenceMode_ = static_cast<InferenceMode>(value);
            return true;
        case EdgeAIResourceId::ACCELERATOR_TYPE:
            if (value < 0 || value > 7) return false;
            accelerator_ = static_cast<AcceleratorType>(value);
            return true;
        case EdgeAIResourceId::NUM_THREADS:
            if (value < 1 || value > 64) return false;
            numThreads_ = static_cast<int>(value);
            return true;
        case EdgeAIResourceId::INFERENCE_TIMEOUT:
            if (value < 0) return false;
            inferenceTimeout_ = static_cast<uint32_t>(value);
            return true;
        case EdgeAIResourceId::BATCH_SIZE:
            if (value < 1 || value > 256) return false;
            batchSize_ = static_cast<int>(value);
            return true;
        case EdgeAIResourceId::PRIORITY:
            if (value < -10 || value > 10) return false;
            priority_ = static_cast<int>(value);
            return true;
        case EdgeAIResourceId::TOP_K_RESULTS:
            if (value < 1 || value > 1000) return false;
            postprocessConfig_.topK = static_cast<int>(value);
            return true;
        case EdgeAIResourceId::MAX_DETECTIONS:
            if (value < 1 || value > 10000) return false;
            postprocessConfig_.maxDetections = static_cast<int>(value);
            return true;
        case EdgeAIResourceId::GPU_MEMORY_LIMIT:
            if (value < 0) return false;
            config_.gpuMemoryLimit = static_cast<uint64_t>(value);
            return true;
        default:
            return false;
    }
}

bool EdgeAIInferenceObject::write(EdgeAIResourceId resourceId, double value) {
    if (!isWritable(resourceId)) return false;

    switch (resourceId) {
        case EdgeAIResourceId::CONFIDENCE_THRESHOLD:
            if (value < 0.0 || value > 1.0) return false;
            postprocessConfig_.confidenceThreshold = static_cast<float>(value);
            return true;
        case EdgeAIResourceId::NMS_IOU_THRESHOLD:
            if (value < 0.0 || value > 1.0) return false;
            postprocessConfig_.nmsIouThreshold = static_cast<float>(value);
            return true;
        case EdgeAIResourceId::NMS_SCORE_THRESHOLD:
            if (value < 0.0 || value > 1.0) return false;
            postprocessConfig_.nmsScoreThreshold = static_cast<float>(value);
            return true;
        default:
            return false;
    }
}

bool EdgeAIInferenceObject::write(EdgeAIResourceId resourceId, bool value) {
    if (!isWritable(resourceId)) return false;

    switch (resourceId) {
        case EdgeAIResourceId::NORMALIZE_INPUT:
            preprocessConfig_.normalize = value;
            return true;
        case EdgeAIResourceId::APPLY_SOFTMAX:
            postprocessConfig_.applySoftmax = value;
            return true;
        case EdgeAIResourceId::NMS_ENABLED:
            postprocessConfig_.nmsEnabled = value;
            return true;
        case EdgeAIResourceId::ENABLE_PROFILING:
            profilingEnabled_ = value;
            return true;
        case EdgeAIResourceId::ALLOW_FP16:
            config_.allowFP16 = value;
            return true;
        default:
            return false;
    }
}

bool EdgeAIInferenceObject::write(EdgeAIResourceId resourceId, const std::vector<uint8_t>& value) {
    if (!isWritable(resourceId)) return false;

    switch (resourceId) {
        case EdgeAIResourceId::MODEL_CHECKSUM:
            modelChecksum_ = value;
            return true;
        case EdgeAIResourceId::INPUT_DATA:
            // Store input data for next inference
            return true;
        case EdgeAIResourceId::INPUT_MEAN: {
            if (value.size() % sizeof(float) != 0) return false;
            size_t count = value.size() / sizeof(float);
            preprocessConfig_.mean.resize(count);
            std::memcpy(preprocessConfig_.mean.data(), value.data(), value.size());
            return true;
        }
        case EdgeAIResourceId::INPUT_STDDEV: {
            if (value.size() % sizeof(float) != 0) return false;
            size_t count = value.size() / sizeof(float);
            preprocessConfig_.stddev.resize(count);
            std::memcpy(preprocessConfig_.stddev.data(), value.data(), value.size());
            return true;
        }
        case EdgeAIResourceId::CALIBRATION_DATA:
            // Store calibration data for quantization
            return true;
        default:
            return false;
    }
}

// ============================================================================
// Execute Operations
// ============================================================================

bool EdgeAIInferenceObject::execute(EdgeAIResourceId resourceId, const std::string& args) {
    if (!isExecutable(resourceId)) return false;

    switch (resourceId) {
        case EdgeAIResourceId::LOAD_MODEL:
            if (!args.empty()) {
                return loadModel(args);
            }
            return loadModel(config_.modelPath);

        case EdgeAIResourceId::UNLOAD_MODEL:
            unloadModel();
            return true;

        case EdgeAIResourceId::DOWNLOAD_MODEL:
            if (!args.empty()) {
                return downloadModel(args);
            }
            return downloadModel(modelUri_);

        case EdgeAIResourceId::RUN_INFERENCE:
            if (modelState_ != ModelState::READY) {
                return false;
            }
            // Would run inference with stored input data
            return true;

        case EdgeAIResourceId::RESET_STATISTICS:
            resetStatistics();
            return true;

        case EdgeAIResourceId::RUN_BENCHMARK: {
            int iterations = 100;
            if (!args.empty()) {
                try {
                    iterations = std::stoi(args);
                } catch (...) {}
            }
            runBenchmark(iterations);
            return true;
        }

        default:
            return false;
    }
}

// ============================================================================
// Model Management
// ============================================================================

bool EdgeAIInferenceObject::downloadModel(const std::string& uri) {
    if (modelState_ == ModelState::DOWNLOADING ||
        modelState_ == ModelState::LOADING) {
        return false;
    }

    modelUri_ = uri;
    setModelState(ModelState::DOWNLOADING);
    downloadProgress_ = 0;

    // Stop any existing download thread
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (downloadThread_ && downloadThread_->joinable()) {
            shouldStop_ = true;
            downloadThread_->join();
            shouldStop_ = false;
        }

        // Start new download in managed thread
        downloadThread_ = std::make_unique<std::thread>([this, uri]() {
            // Simulate download progress
            for (int i = 0; i <= 100 && !shouldStop_; i += 10) {
                if (modelState_ != ModelState::DOWNLOADING) break;
                downloadProgress_ = i;
                if (downloadProgressCallback_) {
                    downloadProgressCallback_(i, i < 100 ? "Downloading..." : "Complete");
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            if (!shouldStop_ && modelState_ == ModelState::DOWNLOADING) {
                // In real implementation, save downloaded model and load it
                setModelState(ModelState::VALIDATING);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                setModelState(ModelState::IDLE);
            }
        });
    }

    return true;
}

bool EdgeAIInferenceObject::loadModel(const std::string& path) {
    if (modelState_ == ModelState::LOADING ||
        modelState_ == ModelState::RUNNING) {
        return false;
    }

    setModelState(ModelState::LOADING);

    // Read model file
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        setModelState(ModelState::ERROR);
        stats_.lastError = "Failed to open model file: " + path;
        return false;
    }

    modelSize_ = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> modelData(modelSize_);
    if (!file.read(reinterpret_cast<char*>(modelData.data()), modelSize_)) {
        setModelState(ModelState::ERROR);
        stats_.lastError = "Failed to read model file";
        return false;
    }

    // Detect format from extension
    ModelFormat format = ModelFormat::TFLITE;
    if (path.find(".onnx") != std::string::npos) {
        format = ModelFormat::ONNX;
    } else if (path.find(".tflite") != std::string::npos) {
        format = ModelFormat::TFLITE;
    }

    return loadModel(modelData, format);
}

bool EdgeAIInferenceObject::loadModel(const std::vector<uint8_t>& data, ModelFormat format) {
    modelFormat_ = format;
    modelSize_ = data.size();

    // Verify model based on format
    setModelState(ModelState::VALIDATING);

#ifdef WITH_TFLITE
    if (format == ModelFormat::TFLITE) {
        // Parse TFLite model
        auto model = tflite::FlatBufferModel::BuildFromBuffer(
            reinterpret_cast<const char*>(data.data()), data.size());
        if (!model) {
            setModelState(ModelState::ERROR);
            stats_.lastError = "Invalid TFLite model";
            return false;
        }

        // Build interpreter
        tflite::ops::builtin::BuiltinOpResolver resolver;
        std::unique_ptr<tflite::Interpreter> interpreter;
        tflite::InterpreterBuilder(*model, resolver)(&interpreter);

        if (!interpreter) {
            setModelState(ModelState::ERROR);
            stats_.lastError = "Failed to build TFLite interpreter";
            return false;
        }

        interpreter->SetNumThreads(numThreads_);
        if (interpreter->AllocateTensors() != kTfLiteOk) {
            setModelState(ModelState::ERROR);
            stats_.lastError = "Failed to allocate tensors";
            return false;
        }

        // Extract tensor info
        inputTensors_.clear();
        for (int i = 0; i < interpreter->inputs().size(); ++i) {
            TensorInfo info;
            auto* tensor = interpreter->input_tensor(i);
            info.name = tensor->name;
            for (int j = 0; j < tensor->dims->size; ++j) {
                info.shape.push_back(tensor->dims->data[j]);
            }
            info.sizeBytes = tensor->bytes;
            inputTensors_.push_back(info);
        }

        outputTensors_.clear();
        for (int i = 0; i < interpreter->outputs().size(); ++i) {
            TensorInfo info;
            auto* tensor = interpreter->output_tensor(i);
            info.name = tensor->name;
            for (int j = 0; j < tensor->dims->size; ++j) {
                info.shape.push_back(tensor->dims->data[j]);
            }
            info.sizeBytes = tensor->bytes;
            outputTensors_.push_back(info);
        }

        stats_.memoryUsage = data.size();
        setModelState(ModelState::READY);
        return true;
    }
#endif

#ifdef WITH_ONNX
    if (format == ModelFormat::ONNX) {
        try {
            Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "EdgeAI");
            Ort::SessionOptions session_options;
            session_options.SetIntraOpNumThreads(numThreads_);

            Ort::Session session(env, data.data(), data.size(), session_options);

            // Extract input info
            inputTensors_.clear();
            Ort::AllocatorWithDefaultOptions allocator;
            for (size_t i = 0; i < session.GetInputCount(); ++i) {
                TensorInfo info;
                auto name = session.GetInputNameAllocated(i, allocator);
                info.name = name.get();
                auto type_info = session.GetInputTypeInfo(i);
                auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
                info.shape = tensor_info.GetShape();
                inputTensors_.push_back(info);
            }

            // Extract output info
            outputTensors_.clear();
            for (size_t i = 0; i < session.GetOutputCount(); ++i) {
                TensorInfo info;
                auto name = session.GetOutputNameAllocated(i, allocator);
                info.name = name.get();
                auto type_info = session.GetOutputTypeInfo(i);
                auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
                info.shape = tensor_info.GetShape();
                outputTensors_.push_back(info);
            }

            stats_.memoryUsage = data.size();
            setModelState(ModelState::READY);
            return true;
        } catch (const Ort::Exception& e) {
            setModelState(ModelState::ERROR);
            stats_.lastError = std::string("ONNX error: ") + e.what();
            return false;
        }
    }
#endif

    // Fallback: store model data for stub implementation
    modelName_ = "model_" + std::to_string(instanceId_);
    modelVersion_ = "1.0.0";

    // Create dummy tensor info for testing
    TensorInfo input;
    input.name = "input";
    input.shape = {1, 224, 224, 3};
    input.dtype = "float32";
    input.sizeBytes = 1 * 224 * 224 * 3 * sizeof(float);
    inputTensors_ = {input};

    TensorInfo output;
    output.name = "output";
    output.shape = {1, 1000};
    output.dtype = "float32";
    output.sizeBytes = 1 * 1000 * sizeof(float);
    outputTensors_ = {output};

    stats_.memoryUsage = data.size();
    setModelState(ModelState::READY);
    return true;
}

void EdgeAIInferenceObject::unloadModel() {
    if (modelState_ == ModelState::RUNNING) {
        cancelInference();
    }

    inputTensors_.clear();
    outputTensors_.clear();
    labels_.clear();
    modelSize_ = 0;
    modelChecksum_.clear();
    stats_.memoryUsage = 0;

    setModelState(ModelState::IDLE);
}

// ============================================================================
// Inference Operations
// ============================================================================

InferenceResult EdgeAIInferenceObject::runInference(const std::vector<uint8_t>& inputData) {
    InferenceResult result;
    result.success = false;

    if (modelState_ != ModelState::READY) {
        result.errorMessage = "Model not ready";
        updateStatistics(0, false, result.errorMessage);
        return result;
    }

    setModelState(ModelState::RUNNING);
    auto startTime = std::chrono::high_resolution_clock::now();

    try {
        // Preprocess input
        auto processedInput = preprocess(inputData);

#ifdef WITH_TFLITE
        if (modelFormat_ == ModelFormat::TFLITE) {
            // TFLite inference would go here
        }
#endif

#ifdef WITH_ONNX
        if (modelFormat_ == ModelFormat::ONNX) {
            // ONNX inference would go here
        }
#endif

        // Stub implementation: generate dummy output
        result.outputData.resize(outputTensors_.empty() ? 4000 : outputTensors_[0].sizeBytes);
        std::fill(result.outputData.begin(), result.outputData.end(), 0);

        // Simulate some computation time
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        result.success = true;

    } catch (const std::exception& e) {
        result.errorMessage = e.what();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.latencyMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();

    // Postprocess and get top-k results
    if (result.success) {
        result = postprocess(result.outputData);
        result.latencyMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();
    }

    updateStatistics(result.latencyMs, result.success, result.errorMessage);
    setModelState(ModelState::READY);

    return result;
}

bool EdgeAIInferenceObject::runInferenceAsync(const std::vector<uint8_t>& inputData,
                                               InferenceCallback callback) {
    if (modelState_ != ModelState::READY) {
        return false;
    }

    // Stop any existing inference thread
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (inferenceThread_ && inferenceThread_->joinable()) {
            inferenceThread_->join();
        }

        // Start new inference in managed thread
        inferenceThread_ = std::make_unique<std::thread>([this, inputData, callback]() {
            if (!shouldStop_) {
                auto result = runInference(inputData);
                if (callback && !shouldStop_) {
                    callback(result);
                }
            }
        });
    }

    return true;
}

std::vector<InferenceResult> EdgeAIInferenceObject::runBatchInference(
    const std::vector<std::vector<uint8_t>>& batchData) {
    std::vector<InferenceResult> results;
    results.reserve(batchData.size());

    for (const auto& input : batchData) {
        results.push_back(runInference(input));
    }

    return results;
}

void EdgeAIInferenceObject::cancelInference() {
    // In real implementation, would signal inference thread to stop
    if (modelState_ == ModelState::RUNNING) {
        setModelState(ModelState::READY);
    }
}

// ============================================================================
// Tensor Information
// ============================================================================

std::vector<TensorInfo> EdgeAIInferenceObject::getInputTensorInfo() const {
    return inputTensors_;
}

std::vector<TensorInfo> EdgeAIInferenceObject::getOutputTensorInfo() const {
    return outputTensors_;
}

std::vector<std::string> EdgeAIInferenceObject::getLabels() const {
    return labels_;
}

// ============================================================================
// Configuration
// ============================================================================

void EdgeAIInferenceObject::setAccelerator(AcceleratorType accelerator) {
    accelerator_ = accelerator;
}

void EdgeAIInferenceObject::setNumThreads(int threads) {
    if (threads >= 1 && threads <= 64) {
        numThreads_ = threads;
    }
}

void EdgeAIInferenceObject::setInferenceMode(InferenceMode mode) {
    inferenceMode_ = mode;
}

void EdgeAIInferenceObject::setPreprocessConfig(const PreprocessConfig& config) {
    preprocessConfig_ = config;
}

void EdgeAIInferenceObject::setPostprocessConfig(const PostprocessConfig& config) {
    postprocessConfig_ = config;
}

// ============================================================================
// Statistics
// ============================================================================

InferenceStatistics EdgeAIInferenceObject::getStatistics() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

void EdgeAIInferenceObject::resetStatistics() {
    std::lock_guard<std::mutex> lock(statsMutex_);
    stats_ = InferenceStatistics{};
    latencyHistory_.clear();
}

std::string EdgeAIInferenceObject::runBenchmark(int iterations) {
    if (modelState_ != ModelState::READY) {
        return "{\"error\":\"Model not ready\"}";
    }

    resetStatistics();

    // Generate dummy input
    size_t inputSize = inputTensors_.empty() ? 224 * 224 * 3 * 4 : inputTensors_[0].sizeBytes;
    std::vector<uint8_t> dummyInput(inputSize, 0);

    auto benchStart = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        runInference(dummyInput);
    }

    auto benchEnd = std::chrono::high_resolution_clock::now();
    float totalMs = std::chrono::duration<float, std::milli>(benchEnd - benchStart).count();

    std::ostringstream result;
    result << "{\"iterations\":" << iterations
           << ",\"total_ms\":" << totalMs
           << ",\"avg_ms\":" << stats_.latencyAvg
           << ",\"min_ms\":" << stats_.latencyMin
           << ",\"max_ms\":" << stats_.latencyMax
           << ",\"p50_ms\":" << stats_.latencyP50
           << ",\"p95_ms\":" << stats_.latencyP95
           << ",\"p99_ms\":" << stats_.latencyP99
           << ",\"throughput\":" << stats_.throughput
           << ",\"memory_bytes\":" << stats_.memoryUsage
           << "}";

    return result.str();
}

// ============================================================================
// Callbacks
// ============================================================================

void EdgeAIInferenceObject::setModelStateCallback(ModelStateCallback callback) {
    modelStateCallback_ = std::move(callback);
}

void EdgeAIInferenceObject::setDownloadProgressCallback(DownloadProgressCallback callback) {
    downloadProgressCallback_ = std::move(callback);
}

// ============================================================================
// Resource Metadata
// ============================================================================

bool EdgeAIInferenceObject::isReadable(EdgeAIResourceId resourceId) {
    switch (resourceId) {
        case EdgeAIResourceId::INPUT_DATA:
        case EdgeAIResourceId::CALIBRATION_DATA:
            return false;  // Write-only
        case EdgeAIResourceId::LOAD_MODEL:
        case EdgeAIResourceId::UNLOAD_MODEL:
        case EdgeAIResourceId::DOWNLOAD_MODEL:
        case EdgeAIResourceId::RUN_INFERENCE:
        case EdgeAIResourceId::RESET_STATISTICS:
        case EdgeAIResourceId::RUN_BENCHMARK:
            return false;  // Execute-only
        default:
            return true;
    }
}

bool EdgeAIInferenceObject::isWritable(EdgeAIResourceId resourceId) {
    switch (resourceId) {
        case EdgeAIResourceId::MODEL_URI:
        case EdgeAIResourceId::MODEL_CHECKSUM:
        case EdgeAIResourceId::INPUT_DATA:
        case EdgeAIResourceId::INFERENCE_MODE:
        case EdgeAIResourceId::ACCELERATOR_TYPE:
        case EdgeAIResourceId::NUM_THREADS:
        case EdgeAIResourceId::INFERENCE_TIMEOUT:
        case EdgeAIResourceId::BATCH_SIZE:
        case EdgeAIResourceId::PRIORITY:
        case EdgeAIResourceId::NORMALIZE_INPUT:
        case EdgeAIResourceId::INPUT_MEAN:
        case EdgeAIResourceId::INPUT_STDDEV:
        case EdgeAIResourceId::APPLY_SOFTMAX:
        case EdgeAIResourceId::CONFIDENCE_THRESHOLD:
        case EdgeAIResourceId::TOP_K_RESULTS:
        case EdgeAIResourceId::NMS_ENABLED:
        case EdgeAIResourceId::NMS_IOU_THRESHOLD:
        case EdgeAIResourceId::NMS_SCORE_THRESHOLD:
        case EdgeAIResourceId::MAX_DETECTIONS:
        case EdgeAIResourceId::ENABLE_PROFILING:
        case EdgeAIResourceId::GPU_MEMORY_LIMIT:
        case EdgeAIResourceId::ALLOW_FP16:
        case EdgeAIResourceId::CALIBRATION_DATA:
            return true;
        default:
            return false;
    }
}

bool EdgeAIInferenceObject::isExecutable(EdgeAIResourceId resourceId) {
    switch (resourceId) {
        case EdgeAIResourceId::LOAD_MODEL:
        case EdgeAIResourceId::UNLOAD_MODEL:
        case EdgeAIResourceId::DOWNLOAD_MODEL:
        case EdgeAIResourceId::RUN_INFERENCE:
        case EdgeAIResourceId::RESET_STATISTICS:
        case EdgeAIResourceId::RUN_BENCHMARK:
            return true;
        default:
            return false;
    }
}

// ============================================================================
// Private Methods
// ============================================================================

void EdgeAIInferenceObject::setModelState(ModelState newState) {
    ModelState oldState = modelState_.exchange(newState);
    if (modelStateCallback_ && oldState != newState) {
        modelStateCallback_(oldState, newState);
    }
}

void EdgeAIInferenceObject::updateStatistics(float latencyMs, bool success,
                                              const std::string& error) {
    std::lock_guard<std::mutex> lock(statsMutex_);

    stats_.inferenceCount++;

    if (success) {
        // Update latency stats
        latencyHistory_.push_back(latencyMs);
        if (latencyHistory_.size() > MAX_LATENCY_HISTORY) {
            latencyHistory_.erase(latencyHistory_.begin());
        }

        // Running average
        stats_.latencyAvg = (stats_.latencyAvg * (stats_.inferenceCount - 1) + latencyMs)
                            / stats_.inferenceCount;

        if (latencyMs < stats_.latencyMin) stats_.latencyMin = latencyMs;
        if (latencyMs > stats_.latencyMax) stats_.latencyMax = latencyMs;

        calculatePercentiles();

        // Throughput (inferences per second)
        if (stats_.latencyAvg > 0) {
            stats_.throughput = 1000.0f / stats_.latencyAvg;
        }
    } else {
        stats_.errorCount++;
        stats_.lastError = error;
    }
}

void EdgeAIInferenceObject::calculatePercentiles() {
    if (latencyHistory_.empty()) return;

    std::vector<float> sorted = latencyHistory_;
    std::sort(sorted.begin(), sorted.end());

    size_t n = sorted.size();
    stats_.latencyP50 = sorted[std::min(static_cast<size_t>(n * 0.50), n - 1)];
    stats_.latencyP95 = sorted[std::min(static_cast<size_t>(n * 0.95), n - 1)];
    stats_.latencyP99 = sorted[std::min(static_cast<size_t>(n * 0.99), n - 1)];
}

std::vector<uint8_t> EdgeAIInferenceObject::preprocess(const std::vector<uint8_t>& input) const {
    if (!preprocessConfig_.normalize) {
        return input;
    }

    // Convert to float, normalize, convert back
    // This is a simplified implementation
    std::vector<uint8_t> output = input;

    // In real implementation, would apply normalization:
    // (x - mean) / stddev for each channel

    return output;
}

InferenceResult EdgeAIInferenceObject::postprocess(const std::vector<uint8_t>& output) const {
    InferenceResult result;
    result.outputData = output;
    result.success = true;

    // Interpret output as float array for classification
    if (output.size() >= sizeof(float)) {
        const float* scores = reinterpret_cast<const float*>(output.data());
        size_t numClasses = output.size() / sizeof(float);

        // Find top-k results
        std::vector<std::pair<int, float>> indexedScores;
        for (size_t i = 0; i < numClasses; ++i) {
            float score = scores[i];
            if (postprocessConfig_.applySoftmax) {
                // Would apply softmax here
            }
            if (score >= postprocessConfig_.confidenceThreshold) {
                indexedScores.emplace_back(static_cast<int>(i), score);
            }
        }

        // Sort by score descending
        std::partial_sort(
            indexedScores.begin(),
            indexedScores.begin() + std::min(static_cast<size_t>(postprocessConfig_.topK),
                                              indexedScores.size()),
            indexedScores.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        // Keep top-k
        result.topKResults.assign(
            indexedScores.begin(),
            indexedScores.begin() + std::min(static_cast<size_t>(postprocessConfig_.topK),
                                              indexedScores.size()));
    }

    return result;
}

// ============================================================================
// Factory
// ============================================================================

std::unique_ptr<EdgeAIInferenceObject> EdgeAIInferenceObjectFactory::create(uint16_t instanceId) {
    auto obj = std::make_unique<EdgeAIInferenceObject>(instanceId);
    obj->initialize();
    return obj;
}

std::unique_ptr<EdgeAIInferenceObject> EdgeAIInferenceObjectFactory::create(
    const EdgeAIInferenceObject::Config& config, uint16_t instanceId) {
    auto obj = std::make_unique<EdgeAIInferenceObject>(instanceId, config);
    obj->initialize();
    return obj;
}

bool EdgeAIInferenceObjectFactory::isTFLiteAvailable() {
#ifdef WITH_TFLITE
    return true;
#else
    return false;
#endif
}

bool EdgeAIInferenceObjectFactory::isONNXAvailable() {
#ifdef WITH_ONNX
    return true;
#else
    return false;
#endif
}

std::vector<ModelFormat> EdgeAIInferenceObjectFactory::getSupportedFormats() {
    std::vector<ModelFormat> formats;
#ifdef WITH_TFLITE
    formats.push_back(ModelFormat::TFLITE);
#endif
#ifdef WITH_ONNX
    formats.push_back(ModelFormat::ONNX);
#endif
    // Always support stub mode
    if (formats.empty()) {
        formats.push_back(ModelFormat::TFLITE);
        formats.push_back(ModelFormat::ONNX);
    }
    return formats;
}

std::vector<AcceleratorType> EdgeAIInferenceObjectFactory::getAvailableAccelerators() {
    std::vector<AcceleratorType> accelerators;
    accelerators.push_back(AcceleratorType::CPU);  // Always available

#ifdef __ANDROID__
    accelerators.push_back(AcceleratorType::NNAPI);
#endif

#ifdef __APPLE__
    accelerators.push_back(AcceleratorType::COREML);
#endif

    // GPU detection would require runtime checks
    return accelerators;
}

// ============================================================================
// Thread Management
// ============================================================================

void EdgeAIInferenceObject::stopAllThreads() {
    shouldStop_ = true;

    // Stop and join download thread
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (downloadThread_ && downloadThread_->joinable()) {
            downloadThread_->join();
            downloadThread_.reset();
        }

        // Stop and join inference thread
        if (inferenceThread_ && inferenceThread_->joinable()) {
            inferenceThread_->join();
            inferenceThread_.reset();
        }
    }

    shouldStop_ = false;
}

} // namespace objects
} // namespace lwm2m
