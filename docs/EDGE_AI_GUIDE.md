# Edge AI Integration Guide

Complete guide for using the Edge AI Inference Object in the Friendly LwM2M Client.

## Table of Contents

1. [Overview](#overview)
2. [Supported Frameworks](#supported-frameworks)
3. [Installation](#installation)
4. [Basic Usage](#basic-usage)
5. [Model Management](#model-management)
6. [Inference Modes](#inference-modes)
7. [Pre/Post Processing](#prepost-processing)
8. [Hardware Acceleration](#hardware-acceleration)
9. [Performance Optimization](#performance-optimization)
10. [LwM2M Integration](#lwm2m-integration)
11. [Examples](#examples)
12. [Troubleshooting](#troubleshooting)

---

## Overview

### What is Edge AI?

Edge AI enables running machine learning models directly on IoT devices, providing:

- **Low latency**: No network round-trip for inference
- **Privacy**: Data stays on-device
- **Reliability**: Works offline
- **Bandwidth savings**: Only send results, not raw data

### Use Cases

| Application | Model Type | Example |
|-------------|------------|---------|
| Predictive maintenance | Anomaly detection | Vibration analysis |
| Visual inspection | Object detection | Defect detection |
| Smart metering | Time series | Usage pattern analysis |
| Environmental monitoring | Classification | Air quality index |
| Asset tracking | Location prediction | Movement patterns |
| Voice control | Speech recognition | Wake word detection |

### Edge AI Object (33410)

The Edge AI Inference Object provides LwM2M-based management of ML models:

- Remote model deployment
- Inference execution control
- Performance monitoring
- Hardware utilization

---

## Supported Frameworks

### TensorFlow Lite

| Feature | Support |
|---------|---------|
| Model format | `.tflite` |
| Quantization | FP32, FP16, INT8, INT4 |
| Delegates | CPU, GPU (OpenGL/Vulkan), NNAPI, EdgeTPU |
| Operations | 150+ supported ops |

### ONNX Runtime

| Feature | Support |
|---------|---------|
| Model format | `.onnx` |
| Execution providers | CPU, CUDA, TensorRT, OpenVINO |
| Quantization | FP32, FP16, INT8 |
| Dynamic shapes | Full support |

### Planned Support

- PyTorch Mobile (`.pt`, `.ptl`)
- Apple Core ML (`.mlmodel`)
- NVIDIA TensorRT (`.engine`)

---

## Installation

### TensorFlow Lite

```bash
# Ubuntu/Debian
sudo apt-get install libtensorflow-lite-dev

# Or build from source
git clone https://github.com/tensorflow/tensorflow.git
cd tensorflow
./tensorflow/lite/tools/make/download_dependencies.sh
./tensorflow/lite/tools/make/build_lib.sh
```

### ONNX Runtime

```bash
# Download pre-built
wget https://github.com/microsoft/onnxruntime/releases/download/v1.16.0/onnxruntime-linux-x64-1.16.0.tgz
tar xzf onnxruntime-linux-x64-1.16.0.tgz
sudo cp -r onnxruntime-linux-x64-1.16.0/* /usr/local/
```

### Build with Edge AI

```bash
cmake .. \
    -DWITH_EDGE_AI=ON \
    -DWITH_TFLITE=ON \
    -DWITH_ONNX=ON

make -j$(nproc)
```

---

## Basic Usage

### Initialize Edge AI Object

```cpp
#include "objects/edge_ai_inference_object.h"

using namespace lwm2m::objects;

// Configuration
EdgeAIInferenceObject::Config config;
config.modelStoragePath = "/var/lib/lwm2m/models";
config.maxModelSize = 100 * 1024 * 1024;  // 100 MB
config.enableGPU = true;
config.defaultNumThreads = 4;

// Create object instance
EdgeAIInferenceObject ai(0, config);  // Instance 0
ai.initialize();
```

### Load a Model

```cpp
// Load from file
auto result = ai.loadModel("/path/to/model.tflite", ModelFormat::TFLITE);

if (result != ModelLoadResult::SUCCESS) {
    std::cerr << "Failed to load model: " << ai.getLastError() << std::endl;
    return;
}

// Load from memory
std::vector<uint8_t> modelData = readFile("/path/to/model.onnx");
result = ai.loadModelFromMemory(modelData, ModelFormat::ONNX);
```

### Run Inference

```cpp
// Create input tensor
TensorShape inputShape;
inputShape.dimensions = {1, 224, 224, 3};  // Batch, Height, Width, Channels
inputShape.dataType = TensorDataType::FLOAT32;

Tensor input = Tensor::create(inputShape);

// Fill input data (e.g., preprocessed image)
float* inputData = input.data<float>();
// ... populate with image data ...

// Run inference
std::vector<Tensor> outputs;
auto inferResult = ai.runInference({input}, outputs);

if (inferResult == InferenceResult::SUCCESS) {
    // Process outputs
    float* outputData = outputs[0].data<float>();
    // ...
}
```

### Get Results

```cpp
// For classification models
auto classifications = ai.decodeClassification(outputs[0], 5);  // Top 5

for (const auto& [label, confidence] : classifications) {
    std::cout << label << ": " << (confidence * 100) << "%" << std::endl;
}

// For object detection models
auto detections = ai.decodeDetection(outputs, 0.5f, true);  // 50% threshold, NMS

for (const auto& det : detections) {
    std::cout << "Class: " << det.classId 
              << " Confidence: " << det.confidence
              << " Box: [" << det.x << ", " << det.y 
              << ", " << det.width << ", " << det.height << "]" 
              << std::endl;
}
```

---

## Model Management

### Download Model from Server

```cpp
// Set model URI
ai.setModelUri("https://models.example.com/mobilenet_v2.tflite");

// Set expected checksum
ai.setModelChecksum("sha256:a1b2c3d4...");

// Start download
ai.downloadModel([](uint8_t progress) {
    std::cout << "Download progress: " << (int)progress << "%" << std::endl;
});

// Check state
while (ai.getState() == ModelState::DOWNLOADING) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

if (ai.getState() == ModelState::DOWNLOADED) {
    ai.loadModel();  // Load downloaded model
}
```

### Model Metadata

```cpp
// Get model information
auto metadata = ai.getModelMetadata();

std::cout << "Name: " << metadata.name << std::endl;
std::cout << "Version: " << metadata.version << std::endl;
std::cout << "Format: " << metadata.formatName << std::endl;
std::cout << "Size: " << metadata.sizeBytes << " bytes" << std::endl;

// Input/output shapes
for (const auto& shape : metadata.inputShapes) {
    std::cout << "Input: " << shape.toString() << std::endl;
}

for (const auto& shape : metadata.outputShapes) {
    std::cout << "Output: " << shape.toString() << std::endl;
}
```

### Hot-Swap Models

```cpp
// Unload current model
ai.unloadModel();

// Load new model
ai.loadModel("/path/to/new_model.tflite", ModelFormat::TFLITE);

// Or atomic swap
ai.swapModel("/path/to/new_model.tflite", ModelFormat::TFLITE);
```

---

## Inference Modes

### Synchronous Inference

```cpp
// Simple synchronous call (blocks until complete)
std::vector<Tensor> outputs;
auto result = ai.runInference({input}, outputs);
```

### Asynchronous Inference

```cpp
// Non-blocking inference
ai.runInferenceAsync({input}, [](InferenceResult result, 
                                  std::vector<Tensor> outputs) {
    if (result == InferenceResult::SUCCESS) {
        // Process outputs in callback
        processResults(outputs);
    }
});

// Continue with other work...
```

### Batched Inference

```cpp
// Process multiple inputs at once
TensorShape batchShape;
batchShape.dimensions = {8, 224, 224, 3};  // Batch size 8

Tensor batchInput = Tensor::create(batchShape);
// Fill batch data...

std::vector<Tensor> outputs;
ai.runInference({batchInput}, outputs);

// Output shape: [8, num_classes]
```

### Streaming Inference

```cpp
// Start streaming mode (for video/audio)
ai.startStreaming([](int frameId, std::vector<Tensor> outputs) {
    std::cout << "Frame " << frameId << " processed" << std::endl;
    processFrame(frameId, outputs);
});

// Push frames continuously
int frameId = 0;
while (capturing) {
    Tensor frame = captureFrame();
    ai.pushStreamingFrame({frame}, frameId++);
}

ai.stopStreaming();
```

---

## Pre/Post Processing

### Input Preprocessing

```cpp
PrePostProcessConfig preConfig;

// Normalization (ImageNet style)
preConfig.normalizeInput = true;
preConfig.inputMean = {0.485f, 0.456f, 0.406f};     // Per-channel mean
preConfig.inputStddev = {0.229f, 0.224f, 0.225f};   // Per-channel stddev

// Apply configuration
ai.setPreprocessConfig(preConfig);

// Now inference automatically normalizes:
// output = (input - mean) / stddev
```

### Output Postprocessing

```cpp
PrePostProcessConfig postConfig;

// Softmax for classification
postConfig.applySoftmax = true;

// Confidence threshold
postConfig.confidenceThreshold = 0.5f;

// Top-K results
postConfig.topKResults = 5;

ai.setPostprocessConfig(postConfig);
```

### Object Detection NMS

```cpp
PrePostProcessConfig detectConfig;

// Enable Non-Maximum Suppression
detectConfig.nmsEnabled = true;
detectConfig.nmsThreshold = 0.45f;  // IoU threshold
detectConfig.confidenceThreshold = 0.25f;

ai.setPostprocessConfig(detectConfig);
```

### Custom Processing Pipeline

```cpp
// Register custom preprocessor
ai.setCustomPreprocessor([](Tensor& input) {
    // Custom normalization
    float* data = input.data<float>();
    size_t size = input.size();
    
    for (size_t i = 0; i < size; i++) {
        data[i] = (data[i] / 255.0f - 0.5f) * 2.0f;
    }
});

// Register custom postprocessor
ai.setCustomPostprocessor([](std::vector<Tensor>& outputs) {
    // Custom output processing
    // e.g., decode anchor boxes, apply custom NMS
});
```

---

## Hardware Acceleration

### Auto-Select Best Accelerator

```cpp
// Let the system choose the best available
ai.setAccelerator(AcceleratorType::AUTO);
```

### CPU Optimization

```cpp
ai.setAccelerator(AcceleratorType::CPU);
ai.setNumThreads(4);  // Use 4 threads

// Enable XNNPACK delegate (TFLite)
ai.enableXNNPACK(true);
```

### GPU Acceleration

```cpp
ai.setAccelerator(AcceleratorType::GPU);

// TFLite: OpenGL ES delegate
// ONNX: CUDA execution provider
```

### NPU/DSP Acceleration

```cpp
// Android NNAPI
ai.setAccelerator(AcceleratorType::NPU);

// Qualcomm Hexagon DSP
ai.setAccelerator(AcceleratorType::DSP);
```

### Check Available Accelerators

```cpp
auto capabilities = ai.getHardwareCapabilities();

std::cout << "CPU cores: " << capabilities.cpuCores << std::endl;
std::cout << "RAM: " << capabilities.totalMemoryMB << " MB" << std::endl;
std::cout << "GPU available: " << capabilities.hasGPU << std::endl;
std::cout << "NPU available: " << capabilities.hasNPU << std::endl;
std::cout << "DSP available: " << capabilities.hasDSP << std::endl;

for (const auto& acc : capabilities.availableAccelerators) {
    std::cout << "  - " << acc << std::endl;
}
```

---

## Performance Optimization

### Quantization

```cpp
// Check model quantization
auto metadata = ai.getModelMetadata();
std::cout << "Quantization: " << metadata.quantizationType << std::endl;

// Prefer quantized models for better performance
// INT8 models are 4x smaller and often 2-4x faster
```

### Benchmarking

```cpp
// Run benchmark
int iterations = 100;
int warmupRuns = 10;

auto benchResult = ai.runBenchmark(iterations, warmupRuns);

std::cout << "Latency (avg): " << benchResult.avgLatencyMs << " ms" << std::endl;
std::cout << "Latency (min): " << benchResult.minLatencyMs << " ms" << std::endl;
std::cout << "Latency (max): " << benchResult.maxLatencyMs << " ms" << std::endl;
std::cout << "Latency (p50): " << benchResult.p50LatencyMs << " ms" << std::endl;
std::cout << "Latency (p95): " << benchResult.p95LatencyMs << " ms" << std::endl;
std::cout << "Latency (p99): " << benchResult.p99LatencyMs << " ms" << std::endl;
std::cout << "Throughput: " << benchResult.throughputFps << " fps" << std::endl;
std::cout << "Memory: " << benchResult.peakMemoryMB << " MB" << std::endl;
```

### Profiling

```cpp
// Enable layer-by-layer profiling
ai.enableProfiling(true);

// Run inference
ai.runInference({input}, outputs);

// Get profile data
auto profile = ai.getProfileData();

for (const auto& layer : profile.layers) {
    std::cout << layer.name << ": " << layer.timeMs << " ms ("
              << layer.percentage << "%)" << std::endl;
}
```

### Memory Management

```cpp
// Set memory limits
config.maxModelSize = 50 * 1024 * 1024;  // 50 MB max model
config.maxWorkingMemory = 100 * 1024 * 1024;  // 100 MB working memory

// Monitor memory usage
auto stats = ai.getStatistics();
std::cout << "Peak memory: " << stats.peakMemoryBytes / 1024 / 1024 << " MB" << std::endl;
```

---

## LwM2M Integration

### Object Resources

| Resource ID | Name | Type | Description |
|-------------|------|------|-------------|
| 0 | Model Name | R | Current model name |
| 1 | Model Version | R | Model version string |
| 2 | Model URI | RW | Download URI |
| 3 | Model Format | R | Format ID (TFLite=0, ONNX=1) |
| 4 | Model State | R | State machine value |
| 7 | Load Model | E | Execute to load |
| 8 | Unload Model | E | Execute to unload |
| 9 | Download Model | E | Execute to download |
| 40 | Run Inference | E | Execute inference |
| 60-68 | Statistics | R | Performance metrics |
| 69 | Reset Statistics | E | Reset counters |

### Remote Model Deployment

```cpp
// Server sends Write to Model URI (resource 2)
// onWrite handler:
ai.onResourceWrite(2, [&ai](const std::vector<uint8_t>& data) {
    std::string uri(data.begin(), data.end());
    ai.setModelUri(uri);
    return LwM2MResponseCode::CHANGED;
});

// Server sends Execute to Download (resource 9)
ai.onResourceExecute(9, [&ai](const std::string& args) {
    ai.downloadModel();
    return LwM2MResponseCode::CHANGED;
});

// Server sends Execute to Load (resource 7)
ai.onResourceExecute(7, [&ai](const std::string& args) {
    ai.loadModel();
    return LwM2MResponseCode::CHANGED;
});
```

### Remote Inference Trigger

```cpp
// Server writes input data to resource 22
ai.onResourceWrite(22, [&ai](const std::vector<uint8_t>& data) {
    // Store input for inference
    ai.setInputData(data);
    return LwM2MResponseCode::CHANGED;
});

// Server executes inference (resource 40)
ai.onResourceExecute(40, [&ai](const std::string& args) {
    ai.runInference();
    return LwM2MResponseCode::CHANGED;
});

// Server reads output from resource 23
ai.onResourceRead(23, [&ai]() -> std::vector<uint8_t> {
    return ai.getOutputData();
});
```

### Observe Statistics

```cpp
// Server observes latency resource (61)
// Client sends notifications when value changes
ai.setStatisticsNotifyCallback([](int resourceId, float value) {
    // Send LwM2M notification
    sendNotification("/33410/0/" + std::to_string(resourceId), value);
});
```

---

## Examples

### Image Classification

```cpp
#include "objects/edge_ai_inference_object.h"
#include <opencv2/opencv.hpp>  // For image loading

int main() {
    EdgeAIInferenceObject ai(0, {});
    ai.initialize();
    ai.loadModel("mobilenet_v2_1.0_224.tflite", ModelFormat::TFLITE);

    // Load and preprocess image
    cv::Mat image = cv::imread("cat.jpg");
    cv::resize(image, image, cv::Size(224, 224));
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
    image.convertTo(image, CV_32F, 1.0/255.0);

    // Create input tensor
    Tensor input = Tensor::create({1, 224, 224, 3}, TensorDataType::FLOAT32);
    memcpy(input.data<float>(), image.data, input.sizeBytes());

    // Run inference
    std::vector<Tensor> outputs;
    ai.runInference({input}, outputs);

    // Get top-5 results
    auto results = ai.decodeClassification(outputs[0], 5);
    for (const auto& [label, confidence] : results) {
        std::cout << label << ": " << confidence * 100 << "%" << std::endl;
    }

    return 0;
}
```

### Object Detection

```cpp
int main() {
    EdgeAIInferenceObject ai(0, {});
    ai.initialize();
    ai.loadModel("ssd_mobilenet_v2.tflite", ModelFormat::TFLITE);

    // Configure NMS
    PrePostProcessConfig config;
    config.nmsEnabled = true;
    config.nmsThreshold = 0.5f;
    config.confidenceThreshold = 0.3f;
    ai.setPostprocessConfig(config);

    // Load image (300x300 for SSD)
    cv::Mat image = cv::imread("street.jpg");
    cv::resize(image, image, cv::Size(300, 300));
    
    Tensor input = imageToTensor(image);

    // Run detection
    std::vector<Tensor> outputs;
    ai.runInference({input}, outputs);

    // Decode detections
    auto detections = ai.decodeDetection(outputs, 0.3f, true);

    // Draw boxes
    for (const auto& det : detections) {
        cv::rectangle(image, 
            cv::Point(det.x, det.y),
            cv::Point(det.x + det.width, det.y + det.height),
            cv::Scalar(0, 255, 0), 2);
    }

    cv::imwrite("output.jpg", image);
    return 0;
}
```

### Anomaly Detection

```cpp
int main() {
    EdgeAIInferenceObject ai(0, {});
    ai.initialize();
    ai.loadModel("anomaly_detector.tflite", ModelFormat::TFLITE);

    // Read sensor data (e.g., vibration)
    std::vector<float> sensorData = readSensorData();

    // Create input (1D time series)
    Tensor input = Tensor::create({1, 256, 1}, TensorDataType::FLOAT32);
    memcpy(input.data<float>(), sensorData.data(), sensorData.size() * sizeof(float));

    // Run inference
    std::vector<Tensor> outputs;
    ai.runInference({input}, outputs);

    // Get anomaly score
    float anomalyScore = outputs[0].data<float>()[0];

    if (anomalyScore > 0.8f) {
        std::cout << "ANOMALY DETECTED! Score: " << anomalyScore << std::endl;
        // Trigger alert...
    }

    return 0;
}
```

---

## Troubleshooting

### Common Issues

| Error | Cause | Solution |
|-------|-------|----------|
| Model load failed | Incompatible format | Check TFLite/ONNX version |
| Out of memory | Model too large | Use quantized model |
| Slow inference | No GPU delegate | Enable hardware acceleration |
| Wrong output shape | Model mismatch | Verify input preprocessing |

### Debug Logging

```cpp
ai.setLogLevel(AILogLevel::DEBUG);

ai.setLogCallback([](AILogLevel level, const std::string& msg) {
    std::cout << "[AI] " << msg << std::endl;
});
```

### Validate Model

```bash
# TensorFlow Lite
python -c "import tensorflow as tf; tf.lite.Interpreter('model.tflite').allocate_tensors()"

# ONNX
python -c "import onnx; onnx.checker.check_model('model.onnx')"
```

### Check Supported Operations

```cpp
// List unsupported ops (if any)
auto unsupported = ai.getUnsupportedOperations();
for (const auto& op : unsupported) {
    std::cout << "Unsupported: " << op << std::endl;
}
```

---

## Best Practices

1. **Use quantized models** - INT8 models are 4x smaller and 2-4x faster
2. **Match input size** - Don't upscale images beyond model input size
3. **Batch when possible** - Processing multiple inputs together is more efficient
4. **Profile first** - Identify bottlenecks before optimizing
5. **Use appropriate accelerator** - GPU for large models, CPU for small ones
6. **Monitor memory** - Embedded devices have limited resources
7. **Warm up model** - First inference is slower due to initialization

---

*For API details, see [API_REFERENCE.md](API_REFERENCE.md). For the LwM2M object specification, see [FEATURE_SPECIFICATION_v1.1.md](FEATURE_SPECIFICATION_v1.1.md).*
