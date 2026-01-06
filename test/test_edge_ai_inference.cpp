/**
 * @file test_edge_ai_inference.cpp
 * @brief Unit tests for LwM2M Edge AI Inference Object
 * 
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "objects/edge_ai_inference_object.h"
#include <thread>
#include <chrono>
#include <cmath>

using namespace lwm2m::objects;

// ============================================================================
// Tensor Tests
// ============================================================================

class TensorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TensorTest, TensorShapeNumElements) {
    TensorShape shape;
    shape.dimensions = {1, 224, 224, 3};
    shape.dataType = TensorDataType::FLOAT32;
    
    EXPECT_EQ(shape.numElements(), 1 * 224 * 224 * 3);
}

TEST_F(TensorTest, TensorShapeSizeBytes) {
    TensorShape shape;
    shape.dimensions = {1, 10};
    shape.dataType = TensorDataType::FLOAT32;
    
    // 10 float32 values = 40 bytes
    EXPECT_EQ(shape.sizeBytes(), 40u);
}

TEST_F(TensorTest, TensorShapeSizeBytesInt8) {
    TensorShape shape;
    shape.dimensions = {1, 100};
    shape.dataType = TensorDataType::INT8;
    
    // 100 int8 values = 100 bytes
    EXPECT_EQ(shape.sizeBytes(), 100u);
}

TEST_F(TensorTest, TensorShapeSizeBytesFloat16) {
    TensorShape shape;
    shape.dimensions = {1, 50};
    shape.dataType = TensorDataType::FLOAT16;
    
    // 50 float16 values = 100 bytes
    EXPECT_EQ(shape.sizeBytes(), 100u);
}

TEST_F(TensorTest, CreateTensor) {
    TensorShape shape;
    shape.dimensions = {1, 10};
    shape.dataType = TensorDataType::FLOAT32;
    
    Tensor tensor = Tensor::create(shape);
    
    EXPECT_EQ(tensor.data.size(), 40u);
    EXPECT_EQ(tensor.shape.dimensions, shape.dimensions);
}

TEST_F(TensorTest, TensorDataAccess) {
    TensorShape shape;
    shape.dimensions = {1, 5};
    shape.dataType = TensorDataType::FLOAT32;
    
    Tensor tensor = Tensor::create(shape);
    float* data = tensor.as<float>();
    
    // Write some values
    for (int i = 0; i < 5; i++) {
        data[i] = static_cast<float>(i) * 1.5f;
    }
    
    // Read them back
    const float* constData = tensor.as<float>();
    EXPECT_FLOAT_EQ(constData[0], 0.0f);
    EXPECT_FLOAT_EQ(constData[1], 1.5f);
    EXPECT_FLOAT_EQ(constData[4], 6.0f);
}

// ============================================================================
// ModelMetadata Tests
// ============================================================================

class ModelMetadataTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ModelMetadataTest, DefaultValues) {
    ModelMetadata metadata;
    
    EXPECT_TRUE(metadata.name.empty());
    EXPECT_TRUE(metadata.version.empty());
    EXPECT_TRUE(metadata.inputs.empty());
    EXPECT_TRUE(metadata.outputs.empty());
    EXPECT_EQ(metadata.modelSizeBytes, 0u);
}

TEST_F(ModelMetadataTest, PopulatedMetadata) {
    ModelMetadata metadata;
    metadata.name = "MobileNetV2";
    metadata.version = "1.0.0";
    metadata.description = "Image classification model";
    metadata.format = ModelFormat::TFLITE;
    metadata.quantization = QuantizationType::INT8;
    
    TensorShape input;
    input.name = "input";
    input.dimensions = {1, 224, 224, 3};
    input.dataType = TensorDataType::FLOAT32;
    metadata.inputs.push_back(input);
    
    TensorShape output;
    output.name = "output";
    output.dimensions = {1, 1000};
    output.dataType = TensorDataType::FLOAT32;
    metadata.outputs.push_back(output);
    
    metadata.labels = {"cat", "dog", "bird"};
    
    EXPECT_EQ(metadata.name, "MobileNetV2");
    EXPECT_EQ(metadata.format, ModelFormat::TFLITE);
    EXPECT_EQ(metadata.quantization, QuantizationType::INT8);
    EXPECT_EQ(metadata.inputs.size(), 1u);
    EXPECT_EQ(metadata.outputs.size(), 1u);
    EXPECT_EQ(metadata.labels.size(), 3u);
}

// ============================================================================
// ModelFormatDetector Tests
// ============================================================================

class ModelFormatDetectorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ModelFormatDetectorTest, DetectTFLiteFromExtension) {
    EXPECT_EQ(ModelFormatDetector::fromExtension("model.tflite"), ModelFormat::TFLITE);
    EXPECT_EQ(ModelFormatDetector::fromExtension("MODEL.TFLITE"), ModelFormat::TFLITE);
    EXPECT_EQ(ModelFormatDetector::fromExtension("/path/to/model.tflite"), ModelFormat::TFLITE);
}

TEST_F(ModelFormatDetectorTest, DetectONNXFromExtension) {
    EXPECT_EQ(ModelFormatDetector::fromExtension("model.onnx"), ModelFormat::ONNX);
    EXPECT_EQ(ModelFormatDetector::fromExtension("MODEL.ONNX"), ModelFormat::ONNX);
}

TEST_F(ModelFormatDetectorTest, DetectPyTorchFromExtension) {
    EXPECT_EQ(ModelFormatDetector::fromExtension("model.pt"), ModelFormat::PYTORCH_MOBILE);
    EXPECT_EQ(ModelFormatDetector::fromExtension("model.ptl"), ModelFormat::PYTORCH_MOBILE);
}

TEST_F(ModelFormatDetectorTest, DetectCoreMLFromExtension) {
    EXPECT_EQ(ModelFormatDetector::fromExtension("model.mlmodel"), ModelFormat::CORE_ML);
    EXPECT_EQ(ModelFormatDetector::fromExtension("model.mlpackage"), ModelFormat::CORE_ML);
}

TEST_F(ModelFormatDetectorTest, DetectTensorRTFromExtension) {
    EXPECT_EQ(ModelFormatDetector::fromExtension("model.engine"), ModelFormat::TENSOR_RT);
    EXPECT_EQ(ModelFormatDetector::fromExtension("model.plan"), ModelFormat::TENSOR_RT);
}

TEST_F(ModelFormatDetectorTest, UnknownExtension) {
    EXPECT_EQ(ModelFormatDetector::fromExtension("model.unknown"), ModelFormat::CUSTOM);
    EXPECT_EQ(ModelFormatDetector::fromExtension("model"), ModelFormat::CUSTOM);
}

TEST_F(ModelFormatDetectorTest, DetectTFLiteFromMagic) {
    std::vector<uint8_t> tfliteMagic = {0x54, 0x46, 0x4C, 0x33, 0x00, 0x00};
    EXPECT_EQ(ModelFormatDetector::fromMagic(tfliteMagic), ModelFormat::TFLITE);
}

TEST_F(ModelFormatDetectorTest, DetectPyTorchFromMagic) {
    std::vector<uint8_t> zipMagic = {0x50, 0x4B, 0x03, 0x04};  // PK (ZIP)
    EXPECT_EQ(ModelFormatDetector::fromMagic(zipMagic), ModelFormat::PYTORCH_MOBILE);
}

TEST_F(ModelFormatDetectorTest, GetExtension) {
    EXPECT_EQ(ModelFormatDetector::getExtension(ModelFormat::TFLITE), ".tflite");
    EXPECT_EQ(ModelFormatDetector::getExtension(ModelFormat::ONNX), ".onnx");
    EXPECT_EQ(ModelFormatDetector::getExtension(ModelFormat::PYTORCH_MOBILE), ".ptl");
}

TEST_F(ModelFormatDetectorTest, GetName) {
    EXPECT_EQ(ModelFormatDetector::getName(ModelFormat::TFLITE), "TensorFlow Lite");
    EXPECT_EQ(ModelFormatDetector::getName(ModelFormat::ONNX), "ONNX");
    EXPECT_EQ(ModelFormatDetector::getName(ModelFormat::PYTORCH_MOBILE), "PyTorch Mobile");
}

// ============================================================================
// EdgeAIInferenceObject Tests
// ============================================================================

class EdgeAIInferenceObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
        EdgeAIInferenceObject::Config config;
        config.modelStoragePath = "/tmp/lwm2m_test_models";
        config.maxModelSize = 10 * 1024 * 1024;  // 10MB
        config.maxRuntimeMemory = 256 * 1024 * 1024;  // 256MB
        config.enableGPU = false;
        config.enableNPU = false;
        config.defaultNumThreads = 2;
        
        object_ = std::make_unique<EdgeAIInferenceObject>(0, config);
        object_->initialize();
    }
    
    void TearDown() override {
        object_.reset();
    }
    
    std::unique_ptr<EdgeAIInferenceObject> object_;
};

TEST_F(EdgeAIInferenceObjectTest, ObjectId) {
    EXPECT_EQ(EdgeAIInferenceObject::OBJECT_ID, 33410);
}

TEST_F(EdgeAIInferenceObjectTest, InitialState) {
    EXPECT_EQ(object_->getModelState(), ModelState::IDLE);
    EXPECT_FALSE(object_->getModelMetadata().has_value());
}

TEST_F(EdgeAIInferenceObjectTest, ResourceIds) {
    using R = EdgeAIInferenceObject::ResourceId;
    
    EXPECT_EQ(static_cast<uint16_t>(R::MODEL_NAME), 0);
    EXPECT_EQ(static_cast<uint16_t>(R::MODEL_VERSION), 1);
    EXPECT_EQ(static_cast<uint16_t>(R::MODEL_URI), 2);
    EXPECT_EQ(static_cast<uint16_t>(R::MODEL_FORMAT), 3);
    EXPECT_EQ(static_cast<uint16_t>(R::MODEL_STATE), 4);
    EXPECT_EQ(static_cast<uint16_t>(R::RUN_INFERENCE), 40);
    EXPECT_EQ(static_cast<uint16_t>(R::INFERENCE_COUNT), 60);
}

TEST_F(EdgeAIInferenceObjectTest, ReadModelState) {
    int64_t state;
    EXPECT_TRUE(object_->read(EdgeAIInferenceObject::ResourceId::MODEL_STATE, state));
    EXPECT_EQ(state, static_cast<int64_t>(ModelState::IDLE));
}

TEST_F(EdgeAIInferenceObjectTest, ReadInferenceCount) {
    int64_t count;
    EXPECT_TRUE(object_->read(EdgeAIInferenceObject::ResourceId::INFERENCE_COUNT, count));
    EXPECT_EQ(count, 0);
}

TEST_F(EdgeAIInferenceObjectTest, WriteInferenceMode) {
    EXPECT_TRUE(object_->write(EdgeAIInferenceObject::ResourceId::INFERENCE_MODE, 
                                static_cast<int64_t>(InferenceMode::ASYNCHRONOUS)));
}

TEST_F(EdgeAIInferenceObjectTest, WriteAcceleratorType) {
    EXPECT_TRUE(object_->write(EdgeAIInferenceObject::ResourceId::ACCELERATOR_TYPE,
                                static_cast<int64_t>(AcceleratorType::CPU)));
}

TEST_F(EdgeAIInferenceObjectTest, WriteNumThreads) {
    EXPECT_TRUE(object_->write(EdgeAIInferenceObject::ResourceId::NUM_THREADS, 4));
    
    int64_t threads;
    EXPECT_TRUE(object_->read(EdgeAIInferenceObject::ResourceId::NUM_THREADS, threads));
    EXPECT_EQ(threads, 4);
}

TEST_F(EdgeAIInferenceObjectTest, WriteConfidenceThreshold) {
    EXPECT_TRUE(object_->write(EdgeAIInferenceObject::ResourceId::CONFIDENCE_THRESHOLD, 0.75));
    
    double threshold;
    EXPECT_TRUE(object_->read(EdgeAIInferenceObject::ResourceId::CONFIDENCE_THRESHOLD, threshold));
    EXPECT_DOUBLE_EQ(threshold, 0.75);
}

TEST_F(EdgeAIInferenceObjectTest, LoadModelFromMemoryMock) {
    // Create mock model data
    std::vector<uint8_t> mockData(1000, 0);
    
    // Load as TFLite (will use mock backend)
    bool result = object_->loadModelFromMemory(mockData, ModelFormat::TFLITE);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(object_->getModelState(), ModelState::READY);
    
    auto metadata = object_->getModelMetadata();
    ASSERT_TRUE(metadata.has_value());
    EXPECT_EQ(metadata->format, ModelFormat::TFLITE);
}

TEST_F(EdgeAIInferenceObjectTest, UnloadModel) {
    // Load first
    std::vector<uint8_t> mockData(1000, 0);
    object_->loadModelFromMemory(mockData, ModelFormat::TFLITE);
    
    // Unload
    EXPECT_TRUE(object_->unloadModel());
    EXPECT_EQ(object_->getModelState(), ModelState::IDLE);
    EXPECT_FALSE(object_->getModelMetadata().has_value());
}

TEST_F(EdgeAIInferenceObjectTest, InferenceWithoutModel) {
    std::vector<Tensor> inputs;
    std::vector<Tensor> outputs;
    
    InferenceResult result = object_->runInference(inputs, outputs);
    
    EXPECT_EQ(result, InferenceResult::ERROR_MODEL_NOT_LOADED);
}

TEST_F(EdgeAIInferenceObjectTest, StatisticsAfterInference) {
    // Load mock model
    std::vector<uint8_t> mockData(1000, 0);
    object_->loadModelFromMemory(mockData, ModelFormat::TFLITE);
    
    // Run inference with mock input
    TensorShape inputShape;
    inputShape.dimensions = {1, 224, 224, 3};
    inputShape.dataType = TensorDataType::FLOAT32;
    Tensor inputTensor = Tensor::create(inputShape);
    
    std::vector<Tensor> outputs;
    object_->runInference({inputTensor}, outputs);
    
    auto stats = object_->getStatistics();
    EXPECT_GE(stats.totalInferences, 1u);
}

TEST_F(EdgeAIInferenceObjectTest, ResetStatistics) {
    // Load and run inference
    std::vector<uint8_t> mockData(1000, 0);
    object_->loadModelFromMemory(mockData, ModelFormat::TFLITE);
    
    TensorShape inputShape;
    inputShape.dimensions = {1, 224, 224, 3};
    inputShape.dataType = TensorDataType::FLOAT32;
    Tensor inputTensor = Tensor::create(inputShape);
    
    std::vector<Tensor> outputs;
    object_->runInference({inputTensor}, outputs);
    
    // Reset statistics
    object_->resetStatistics();
    
    auto stats = object_->getStatistics();
    EXPECT_EQ(stats.totalInferences, 0u);
    EXPECT_EQ(stats.successfulInferences, 0u);
    EXPECT_EQ(stats.failedInferences, 0u);
}

TEST_F(EdgeAIInferenceObjectTest, GetHardwareCapabilities) {
    auto capabilities = object_->getHardwareCapabilities();
    
    EXPECT_GT(capabilities.cpuCores, 0);
    EXPECT_FALSE(capabilities.cpuName.empty());
}

TEST_F(EdgeAIInferenceObjectTest, GetSupportedFormats) {
    auto formats = object_->getSupportedFormats();
    
    // Should have at least mock support
    // Real implementation would check for TFLite/ONNX availability
}

TEST_F(EdgeAIInferenceObjectTest, GetAvailableAccelerators) {
    auto accelerators = object_->getAvailableAccelerators();
    
    // CPU should always be available
    EXPECT_TRUE(std::find(accelerators.begin(), accelerators.end(), 
                           AcceleratorType::CPU) != accelerators.end());
}

// ============================================================================
// PrePostProcessConfig Tests
// ============================================================================

class PrePostProcessConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        EdgeAIInferenceObject::Config config;
        object_ = std::make_unique<EdgeAIInferenceObject>(0, config);
        object_->initialize();
    }
    
    void TearDown() override {
        object_.reset();
    }
    
    std::unique_ptr<EdgeAIInferenceObject> object_;
};

TEST_F(PrePostProcessConfigTest, DefaultConfig) {
    auto config = object_->getPreprocessConfig();
    
    EXPECT_FALSE(config.normalizeInput);
    EXPECT_FALSE(config.applySoftmax);
    EXPECT_FLOAT_EQ(config.confidenceThreshold, 0.5f);
    EXPECT_EQ(config.topK, 5);
    EXPECT_FALSE(config.nonMaxSuppression);
}

TEST_F(PrePostProcessConfigTest, SetCustomConfig) {
    PrePostProcessConfig config;
    config.normalizeInput = true;
    config.mean = {0.485f, 0.456f, 0.406f};  // ImageNet mean
    config.stddev = {0.229f, 0.224f, 0.225f};  // ImageNet stddev
    config.applySoftmax = true;
    config.confidenceThreshold = 0.8f;
    config.topK = 10;
    config.nonMaxSuppression = true;
    config.nmsThreshold = 0.45f;
    
    object_->setPreprocessConfig(config);
    
    auto readConfig = object_->getPreprocessConfig();
    EXPECT_TRUE(readConfig.normalizeInput);
    EXPECT_TRUE(readConfig.applySoftmax);
    EXPECT_FLOAT_EQ(readConfig.confidenceThreshold, 0.8f);
    EXPECT_EQ(readConfig.topK, 10);
    EXPECT_TRUE(readConfig.nonMaxSuppression);
    EXPECT_FLOAT_EQ(readConfig.nmsThreshold, 0.45f);
    EXPECT_EQ(readConfig.mean.size(), 3u);
}

// ============================================================================
// InferenceConfig Tests
// ============================================================================

class InferenceConfigTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(InferenceConfigTest, DefaultValues) {
    InferenceConfig config;
    
    EXPECT_EQ(config.mode, InferenceMode::SYNCHRONOUS);
    EXPECT_EQ(config.accelerator, AcceleratorType::AUTO);
    EXPECT_EQ(config.numThreads, 0u);
    EXPECT_EQ(config.timeout, 5000u);
    EXPECT_TRUE(config.allowFallback);
    EXPECT_FALSE(config.profileMode);
    EXPECT_EQ(config.batchSize, 1u);
}

TEST_F(InferenceConfigTest, CustomValues) {
    InferenceConfig config;
    config.mode = InferenceMode::BATCHED;
    config.accelerator = AcceleratorType::GPU;
    config.numThreads = 4;
    config.timeout = 10000;
    config.allowFallback = false;
    config.profileMode = true;
    config.batchSize = 8;
    config.powerPriority = 0.8f;
    
    EXPECT_EQ(config.mode, InferenceMode::BATCHED);
    EXPECT_EQ(config.accelerator, AcceleratorType::GPU);
    EXPECT_EQ(config.numThreads, 4u);
    EXPECT_EQ(config.timeout, 10000u);
    EXPECT_FALSE(config.allowFallback);
    EXPECT_TRUE(config.profileMode);
    EXPECT_EQ(config.batchSize, 8u);
    EXPECT_FLOAT_EQ(config.powerPriority, 0.8f);
}

// ============================================================================
// ModelDownloadConfig Tests
// ============================================================================

class ModelDownloadConfigTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ModelDownloadConfigTest, DefaultValues) {
    ModelDownloadConfig config;
    
    EXPECT_TRUE(config.uri.empty());
    EXPECT_TRUE(config.checksum.empty());
    EXPECT_EQ(config.checksumType, "SHA256");
    EXPECT_EQ(config.expectedSize, 0u);
    EXPECT_TRUE(config.resumable);
    EXPECT_EQ(config.timeout, 300u);
    EXPECT_EQ(config.retries, 3u);
}

TEST_F(ModelDownloadConfigTest, CustomValues) {
    ModelDownloadConfig config;
    config.uri = "https://example.com/model.tflite";
    config.checksum = {0xAB, 0xCD, 0xEF};
    config.expectedSize = 5000000;
    config.timeout = 600;
    config.retries = 5;
    config.headers["Authorization"] = "Bearer token123";
    
    EXPECT_EQ(config.uri, "https://example.com/model.tflite");
    EXPECT_EQ(config.checksum.size(), 3u);
    EXPECT_EQ(config.expectedSize, 5000000u);
    EXPECT_EQ(config.timeout, 600u);
    EXPECT_EQ(config.retries, 5u);
    EXPECT_EQ(config.headers["Authorization"], "Bearer token123");
}

// ============================================================================
// Detection Box Tests
// ============================================================================

class DetectionBoxTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(DetectionBoxTest, CreateDetectionBox) {
    EdgeAIInferenceObject::DetectionBox box;
    box.x1 = 0.1f;
    box.y1 = 0.2f;
    box.x2 = 0.5f;
    box.y2 = 0.6f;
    box.confidence = 0.95f;
    box.classId = 15;
    box.label = "cat";
    
    EXPECT_FLOAT_EQ(box.x1, 0.1f);
    EXPECT_FLOAT_EQ(box.y1, 0.2f);
    EXPECT_FLOAT_EQ(box.x2, 0.5f);
    EXPECT_FLOAT_EQ(box.y2, 0.6f);
    EXPECT_FLOAT_EQ(box.confidence, 0.95f);
    EXPECT_EQ(box.classId, 15);
    EXPECT_EQ(box.label, "cat");
}

// ============================================================================
// Backend Availability Tests
// ============================================================================

class BackendAvailabilityTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(BackendAvailabilityTest, TFLiteAvailability) {
    bool available = TFLiteBackend::isAvailable();
    // Will be false unless WITH_TFLITE is defined
    #ifdef WITH_TFLITE
        EXPECT_TRUE(available);
    #else
        EXPECT_FALSE(available);
    #endif
}

TEST_F(BackendAvailabilityTest, ONNXAvailability) {
    bool available = ONNXBackend::isAvailable();
    // Will be false unless WITH_ONNX is defined
    #ifdef WITH_ONNX
        EXPECT_TRUE(available);
    #else
        EXPECT_FALSE(available);
    #endif
}

// ============================================================================
// State Change Callback Tests
// ============================================================================

class StateChangeCallbackTest : public ::testing::Test {
protected:
    void SetUp() override {
        EdgeAIInferenceObject::Config config;
        object_ = std::make_unique<EdgeAIInferenceObject>(0, config);
        object_->initialize();
    }
    
    void TearDown() override {
        object_.reset();
    }
    
    std::unique_ptr<EdgeAIInferenceObject> object_;
};

TEST_F(StateChangeCallbackTest, CallbackOnLoadModel) {
    bool callbackInvoked = false;
    ModelState lastOldState = ModelState::IDLE;
    ModelState lastNewState = ModelState::IDLE;
    
    object_->setStateChangeCallback([&](ModelState oldState, ModelState newState, 
                                         const std::string&) {
        callbackInvoked = true;
        lastOldState = oldState;
        lastNewState = newState;
    });
    
    std::vector<uint8_t> mockData(1000, 0);
    object_->loadModelFromMemory(mockData, ModelFormat::TFLITE);
    
    EXPECT_TRUE(callbackInvoked);
}

// ============================================================================
// Execute Resource Tests
// ============================================================================

class ExecuteResourceTest : public ::testing::Test {
protected:
    void SetUp() override {
        EdgeAIInferenceObject::Config config;
        object_ = std::make_unique<EdgeAIInferenceObject>(0, config);
        object_->initialize();
    }
    
    void TearDown() override {
        object_.reset();
    }
    
    std::unique_ptr<EdgeAIInferenceObject> object_;
};

TEST_F(ExecuteResourceTest, ExecuteUnloadModel) {
    // First load a model
    std::vector<uint8_t> mockData(1000, 0);
    object_->loadModelFromMemory(mockData, ModelFormat::TFLITE);
    EXPECT_EQ(object_->getModelState(), ModelState::READY);
    
    // Execute unload
    EXPECT_TRUE(object_->execute(EdgeAIInferenceObject::ResourceId::UNLOAD_MODEL, ""));
    EXPECT_EQ(object_->getModelState(), ModelState::IDLE);
}

TEST_F(ExecuteResourceTest, ExecuteResetStatistics) {
    EXPECT_TRUE(object_->execute(EdgeAIInferenceObject::ResourceId::RESET_STATISTICS, ""));
    
    auto stats = object_->getStatistics();
    EXPECT_EQ(stats.totalInferences, 0u);
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
