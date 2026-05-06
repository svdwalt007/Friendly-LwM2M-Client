# Edge AI Inference Engine Integration Guide

**Status:** Experimental Framework Ready
**Version:** v1.2.2
**Last Updated:** 2026-04-26

---

## Overview

The Edge AI Inference Engine (LwM2M Object 33700) provides a complete framework for running machine learning inference on IoT devices. The current implementation includes:

✅ **Complete LwM2M Object Implementation**
- Model lifecycle management (download, load, unload)
- Inference execution (synchronous, asynchronous, batch)
- Resource management and monitoring
- Statistics collection and reporting

✅ **Experimental Stub Infrastructure**
- Integration points for TensorFlow Lite
- Integration points for ONNX Runtime
- Fallback stub for testing without ML libraries
- Complete API surface ready for integration

⚠️ **ML Library Integration Required for Production**
- TensorFlow Lite and ONNX Runtime are NOT currently linked
- Stub implementations return dummy data for testing
- Integration requires adding ML library dependencies

---

## Current Capabilities (Without ML Libraries)

The Edge AI object is fully functional for:

1. **LwM2M Server Integration**
   - Server can manage model lifecycle via LwM2M protocol
   - Download models from package URIs
   - Monitor model state and statistics
   - Trigger inference operations

2. **Application Testing**
   - Test inference workflow without ML libraries
   - Profile memory usage and threading behavior
   - Validate LwM2M protocol compliance
   - Measure overhead of inference framework

3. **Infrastructure Validation**
   - Thread safety verification
   - Resource lifecycle management
   - Error handling paths
   - Statistics collection

**What Doesn't Work (Yet):**
- Actual inference execution (returns zeros)
- Real model loading (accepts but doesn't parse)
- GPU/NPU acceleration (framework ready, not connected)

---

## Integration Option 1: TensorFlow Lite

### 1. Install TensorFlow Lite

**For Ubuntu/Debian:**
```bash
# Install from source (recommended for embedded)
git clone https://github.com/tensorflow/tensorflow.git
cd tensorflow
./tensorflow/lite/tools/make/download_dependencies.sh
./tensorflow/lite/tools/make/build_lib.sh

# Or use package manager (if available)
sudo apt-get install tensorflow-lite-dev
```

**For OpenWRT:**
```bash
# Add TensorFlow Lite feed to feeds.conf
src-git tflite https://github.com/openembedded/meta-tensorflow-lite.git

# Update and install
./scripts/feeds update tflite
./scripts/feeds install tensorflow-lite
```

### 2. Update CMakeLists.txt

Add TensorFlow Lite dependency to `CMakeLists.txt`:

```cmake
# TensorFlow Lite integration (optional)
option(WITH_TFLITE "Enable TensorFlow Lite inference" ON)

if(WITH_TFLITE)
    # Find TensorFlow Lite
    find_package(tensorflow-lite QUIET)

    if(tensorflow-lite_FOUND)
        message(STATUS "TensorFlow Lite found: ${TENSORFLOW_LITE_LIBRARIES}")
        target_compile_definitions(friendly_lwm2m PRIVATE WITH_TFLITE)
        target_link_libraries(friendly_lwm2m PRIVATE ${TENSORFLOW_LITE_LIBRARIES})
        target_include_directories(friendly_lwm2m PRIVATE ${TENSORFLOW_LITE_INCLUDE_DIRS})
    else()
        # Manual configuration if find_package fails
        set(TENSORFLOW_LITE_DIR "/path/to/tensorflow/lite" CACHE PATH "TensorFlow Lite directory")

        if(EXISTS "${TENSORFLOW_LITE_DIR}/libtensorflow-lite.a")
            message(STATUS "Using manual TensorFlow Lite configuration")
            target_compile_definitions(friendly_lwm2m PRIVATE WITH_TFLITE)
            target_link_libraries(friendly_lwm2m PRIVATE
                ${TENSORFLOW_LITE_DIR}/libtensorflow-lite.a
                dl pthread
            )
            target_include_directories(friendly_lwm2m PRIVATE
                ${TENSORFLOW_LITE_DIR}/../../
            )
        else()
            message(WARNING "TensorFlow Lite not found, using stub implementation")
        endif()
    endif()
endif()
```

### 3. Add TFLite Headers to edge_ai_inference_object.cpp

Add includes at the top of `src/objects/edge_ai_inference_object.cpp`:

```cpp
#ifdef WITH_TFLITE
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>
#include <tensorflow/lite/optional_debug_tools.h>
#endif
```

### 4. Add Interpreter Member to EdgeAIInferenceObject

In `include/objects/edge_ai_inference_object.h`:

```cpp
class EdgeAIInferenceObject {
private:
    // ... existing members ...

#ifdef WITH_TFLITE
    std::unique_ptr<tflite::FlatBufferModel> tfliteModel_;
    std::unique_ptr<tflite::Interpreter> tfliteInterpreter_;
#endif
};
```

### 5. Implement Model Loading (loadModel)

Replace the stub in `loadModel()` around line 735:

```cpp
bool EdgeAIInferenceObject::loadModel() {
    if (modelData_.empty()) {
        return false;
    }

    setModelState(ModelState::LOADING);

    try {
#ifdef WITH_TFLITE
        if (modelFormat_ == ModelFormat::TFLITE) {
            // Load TFLite model from buffer
            tfliteModel_ = tflite::FlatBufferModel::BuildFromBuffer(
                reinterpret_cast<const char*>(modelData_.data()),
                modelData_.size()
            );

            if (!tfliteModel_) {
                throw std::runtime_error("Failed to load TFLite model");
            }

            // Build interpreter
            tflite::ops::builtin::BuiltinOpResolver resolver;
            tflite::InterpreterBuilder builder(*tfliteModel_, resolver);
            builder(&tfliteInterpreter_);

            if (!tfliteInterpreter_) {
                throw std::runtime_error("Failed to build TFLite interpreter");
            }

            // Allocate tensors
            if (tfliteInterpreter_->AllocateTensors() != kTfLiteOk) {
                throw std::runtime_error("Failed to allocate TFLite tensors");
            }

            // Extract tensor information
            inputTensors_.clear();
            for (int i = 0; i < tfliteInterpreter_->inputs().size(); ++i) {
                TfLiteTensor* tensor = tfliteInterpreter_->input_tensor(i);
                TensorInfo info;
                info.name = tensor->name ? tensor->name : "input_" + std::to_string(i);
                info.dataType = TensorDataType::FLOAT32; // Adjust based on tensor->type

                for (int dim = 0; dim < tensor->dims->size; ++dim) {
                    info.shape.push_back(tensor->dims->data[dim]);
                }

                info.sizeBytes = tensor->bytes;
                inputTensors_.push_back(info);
            }

            outputTensors_.clear();
            for (int i = 0; i < tfliteInterpreter_->outputs().size(); ++i) {
                TfLiteTensor* tensor = tfliteInterpreter_->output_tensor(i);
                TensorInfo info;
                info.name = tensor->name ? tensor->name : "output_" + std::to_string(i);
                info.dataType = TensorDataType::FLOAT32; // Adjust based on tensor->type

                for (int dim = 0; dim < tensor->dims->size; ++dim) {
                    info.shape.push_back(tensor->dims->data[dim]);
                }

                info.sizeBytes = tensor->bytes;
                outputTensors_.push_back(info);
            }

            setModelState(ModelState::READY);
            return true;
        }
#endif

        // Fallback for unsupported formats
        throw std::runtime_error("Model format not supported or ML library not compiled");

    } catch (const std::exception& e) {
        setModelState(ModelState::ERROR);
        return false;
    }
}
```

### 6. Implement Inference (runInference)

The integration point is already documented in the code at line 813. Replace the stub section:

```cpp
#ifdef WITH_TFLITE
if (modelFormat_ == ModelFormat::TFLITE) {
    // Validate input size
    TfLiteTensor* inputTensor = tfliteInterpreter_->input_tensor(0);
    if (processedInput.size() != inputTensor->bytes) {
        throw std::runtime_error("Input size mismatch");
    }

    // Copy input data to input tensor
    std::memcpy(inputTensor->data.raw, processedInput.data(), processedInput.size());

    // Run inference
    if (tfliteInterpreter_->Invoke() != kTfLiteOk) {
        throw std::runtime_error("TFLite inference failed");
    }

    // Extract output
    TfLiteTensor* outputTensor = tfliteInterpreter_->output_tensor(0);
    result.outputData.resize(outputTensor->bytes);
    std::memcpy(result.outputData.data(), outputTensor->data.raw, outputTensor->bytes);
    result.success = true;

    // No need to return here, let normal flow continue
}
#endif
```

### 7. Build with TensorFlow Lite

```bash
mkdir -p build && cd build
cmake .. -DWITH_TFLITE=ON -DTENSORFLOW_LITE_DIR=/path/to/tensorflow/lite
make -j$(nproc)
```

### 8. Test with Real Model

```bash
# Download a TFLite model (example: MobileNet)
wget https://storage.googleapis.com/download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_224_quant.tgz
tar -xzf mobilenet_v1_1.0_224_quant.tgz

# The model file is mobilenet_v1_1.0_224_quant.tflite
# Configure LwM2M to use this model via server or CLI
```

---

## Integration Option 2: ONNX Runtime

### 1. Install ONNX Runtime

**For Ubuntu/Debian:**
```bash
# Download pre-built release
wget https://github.com/microsoft/onnxruntime/releases/download/v1.16.3/onnxruntime-linux-x64-1.16.3.tgz
tar -xzf onnxruntime-linux-x64-1.16.3.tgz

# Or build from source for embedded
git clone --recursive https://github.com/microsoft/onnxruntime.git
cd onnxruntime
./build.sh --config Release --build_shared_lib --parallel
```

**For OpenWRT:**
```bash
# Cross-compile ONNX Runtime for OpenWRT
# See: https://onnxruntime.ai/docs/build/custom.html
```

### 2. Update CMakeLists.txt

```cmake
# ONNX Runtime integration (optional)
option(WITH_ONNX "Enable ONNX Runtime inference" ON)

if(WITH_ONNX)
    find_package(onnxruntime QUIET)

    if(onnxruntime_FOUND)
        message(STATUS "ONNX Runtime found")
        target_compile_definitions(friendly_lwm2m PRIVATE WITH_ONNX)
        target_link_libraries(friendly_lwm2m PRIVATE onnxruntime::onnxruntime)
    else()
        # Manual configuration
        set(ONNXRUNTIME_DIR "/path/to/onnxruntime" CACHE PATH "ONNX Runtime directory")

        if(EXISTS "${ONNXRUNTIME_DIR}/lib/libonnxruntime.so")
            message(STATUS "Using manual ONNX Runtime configuration")
            target_compile_definitions(friendly_lwm2m PRIVATE WITH_ONNX)
            target_link_libraries(friendly_lwm2m PRIVATE
                ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so
            )
            target_include_directories(friendly_lwm2m PRIVATE
                ${ONNXRUNTIME_DIR}/include
            )
        else()
            message(WARNING "ONNX Runtime not found, using stub implementation")
        endif()
    endif()
endif()
```

### 3. Add ONNX Runtime Headers

Add to `src/objects/edge_ai_inference_object.cpp`:

```cpp
#ifdef WITH_ONNX
#include <onnxruntime_cxx_api.h>
#endif
```

### 4. Add Session Member

In `include/objects/edge_ai_inference_object.h`:

```cpp
class EdgeAIInferenceObject {
private:
    // ... existing members ...

#ifdef WITH_ONNX
    std::unique_ptr<Ort::Env> ortEnv_;
    std::unique_ptr<Ort::Session> ortSession_;
    std::vector<const char*> inputNames_;
    std::vector<const char*> outputNames_;
#endif
};
```

### 5. Implement Model Loading for ONNX

In `loadModel()`:

```cpp
#ifdef WITH_ONNX
if (modelFormat_ == ModelFormat::ONNX) {
    // Initialize ONNX Runtime environment
    ortEnv_ = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "EdgeAIInference");

    // Configure session options
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads(1);
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    // Load model from memory
    ortSession_ = std::make_unique<Ort::Session>(
        *ortEnv_,
        modelData_.data(),
        modelData_.size(),
        sessionOptions
    );

    // Extract input tensor information
    inputTensors_.clear();
    size_t numInputs = ortSession_->GetInputCount();
    for (size_t i = 0; i < numInputs; ++i) {
        Ort::AllocatorWithDefaultOptions allocator;

        TensorInfo info;
        info.name = ortSession_->GetInputNameAllocated(i, allocator).get();
        inputNames_.push_back(info.name.c_str());

        auto typeInfo = ortSession_->GetInputTypeInfo(i);
        auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();

        // Get shape
        auto shape = tensorInfo.GetShape();
        for (auto dim : shape) {
            info.shape.push_back(dim);
        }

        // Calculate size
        info.sizeBytes = tensorInfo.GetElementCount() * sizeof(float);
        info.dataType = TensorDataType::FLOAT32;

        inputTensors_.push_back(info);
    }

    // Extract output tensor information
    outputTensors_.clear();
    size_t numOutputs = ortSession_->GetOutputCount();
    for (size_t i = 0; i < numOutputs; ++i) {
        Ort::AllocatorWithDefaultOptions allocator;

        TensorInfo info;
        info.name = ortSession_->GetOutputNameAllocated(i, allocator).get();
        outputNames_.push_back(info.name.c_str());

        auto typeInfo = ortSession_->GetOutputTypeInfo(i);
        auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();

        auto shape = tensorInfo.GetShape();
        for (auto dim : shape) {
            info.shape.push_back(dim);
        }

        info.sizeBytes = tensorInfo.GetElementCount() * sizeof(float);
        info.dataType = TensorDataType::FLOAT32;

        outputTensors_.push_back(info);
    }

    setModelState(ModelState::READY);
    return true;
}
#endif
```

### 6. Implement Inference for ONNX

Replace the stub at line 820:

```cpp
#ifdef WITH_ONNX
if (modelFormat_ == ModelFormat::ONNX) {
    // Prepare input tensor
    std::vector<int64_t> inputShape;
    for (auto dim : inputTensors_[0].shape) {
        inputShape.push_back(dim);
    }

    size_t inputTensorSize = processedInput.size() / sizeof(float);
    std::vector<float> inputTensorValues(
        reinterpret_cast<const float*>(processedInput.data()),
        reinterpret_cast<const float*>(processedInput.data()) + inputTensorSize
    );

    // Create input tensor
    auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
        memoryInfo,
        inputTensorValues.data(),
        inputTensorSize,
        inputShape.data(),
        inputShape.size()
    );

    // Run inference
    auto outputTensors = ortSession_->Run(
        Ort::RunOptions{nullptr},
        inputNames_.data(),
        &inputTensor,
        1,
        outputNames_.data(),
        1
    );

    // Extract output
    float* outputData = outputTensors[0].GetTensorMutableData<float>();
    size_t outputSize = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount() * sizeof(float);

    result.outputData.resize(outputSize);
    std::memcpy(result.outputData.data(), outputData, outputSize);
    result.success = true;
}
#endif
```

### 7. Build with ONNX Runtime

```bash
mkdir -p build && cd build
cmake .. -DWITH_ONNX=ON -DONNXRUNTIME_DIR=/path/to/onnxruntime
make -j$(nproc)
```

### 8. Test with ONNX Model

```bash
# Download an ONNX model (example: ResNet)
wget https://github.com/onnx/models/raw/main/vision/classification/resnet/model/resnet50-v2-7.onnx

# Configure LwM2M to use this model
```

---

## Integration Option 3: Both Libraries

You can enable both TensorFlow Lite and ONNX Runtime simultaneously:

```bash
cmake .. -DWITH_TFLITE=ON -DWITH_ONNX=ON \
    -DTENSORFLOW_LITE_DIR=/path/to/tflite \
    -DONNXRUNTIME_DIR=/path/to/onnxruntime
make -j$(nproc)
```

The framework will select the appropriate engine based on `modelFormat_`.

---

## Hardware Acceleration

### GPU Acceleration (TensorFlow Lite)

```cpp
// In loadModel(), after creating interpreter:
tfliteInterpreter_->SetNumThreads(4);

// Use GPU delegate if available
#ifdef TFLITE_GPU_DELEGATE
auto options = TfLiteGpuDelegateOptionsV2Default();
auto delegate = TfLiteGpuDelegateV2Create(&options);
if (tfliteInterpreter_->ModifyGraphWithDelegate(delegate) != kTfLiteOk) {
    TfLiteGpuDelegateV2Delete(delegate);
    // Fall back to CPU
}
#endif
```

### GPU Acceleration (ONNX Runtime)

```cpp
// In loadModel(), configure session options:
sessionOptions.AppendExecutionProvider_CUDA(0); // GPU 0
// Or for other accelerators:
// sessionOptions.AppendExecutionProvider_TensorRT(...);
// sessionOptions.AppendExecutionProvider_OpenVINO(...);
```

### NPU/DSP Acceleration

Consult your hardware vendor's documentation for custom delegates.

---

## Testing Without ML Libraries

The current stub implementation allows full testing of:

1. **LwM2M Protocol Integration**
   ```bash
   # Server can:
   # - Write package URI to trigger download
   # - Execute model load operation
   # - Execute inference operation
   # - Read inference results (zeros)
   # - Monitor statistics
   ```

2. **Memory Management**
   ```cpp
   // Test model loading with large files
   // Verify cleanup on unload
   // Check for memory leaks
   ```

3. **Threading Behavior**
   ```cpp
   // Test async inference
   // Verify thread safety
   // Check cancellation
   ```

4. **Error Handling**
   ```cpp
   // Test invalid models
   // Test inference on unloaded model
   // Test concurrent operations
   ```

---

## Production Deployment Checklist

Before deploying to production with ML inference:

- [ ] Choose ML library (TFLite, ONNX, or both)
- [ ] Install ML library dependencies
- [ ] Update CMakeLists.txt with library paths
- [ ] Implement model loading for chosen library
- [ ] Implement inference for chosen library
- [ ] Test with representative models
- [ ] Profile memory usage
- [ ] Profile inference latency
- [ ] Configure hardware acceleration if available
- [ ] Test error handling (invalid models, OOM, etc.)
- [ ] Verify thread safety under load
- [ ] Document model requirements (format, size, etc.)
- [ ] Create deployment guide for your platform

---

## Performance Tuning

### Memory Optimization

```cpp
// Reduce model memory footprint
sessionOptions.SetMemoryPatternOptimization(true);
sessionOptions.AddConfigEntry("session.use_env_allocators", "1");

// Or for TFLite
tfliteInterpreter_->SetAllowFp16PrecisionForFp32(true);
```

### Latency Optimization

```cpp
// Increase thread count for CPU inference
tfliteInterpreter_->SetNumThreads(std::thread::hardware_concurrency());

// Or for ONNX
sessionOptions.SetIntraOpNumThreads(std::thread::hardware_concurrency());
sessionOptions.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
```

### Power Optimization

```cpp
// Reduce power consumption
sessionOptions.AddConfigEntry("session.intra_op.allow_spinning", "0");
```

---

## Troubleshooting

### Build Errors

**"tensorflow/lite/model.h: No such file"**
- Ensure TensorFlow Lite include path is correct
- Check `TENSORFLOW_LITE_DIR` CMake variable

**"undefined reference to Ort::..."**
- Link against libonnxruntime.so
- Check `ONNXRUNTIME_DIR` CMake variable

### Runtime Errors

**"Failed to load TFLite model"**
- Verify model file is valid TFLite format
- Check model file size matches modelData_.size()
- Try loading model with Python to verify it's not corrupted

**"TFLite inference failed"**
- Verify input tensor size matches model expectations
- Check input data type (float32 vs uint8)
- Use `tflite::PrintInterpreterState(tfliteInterpreter_.get())` for debugging

**"ONNX Runtime error"**
- Enable verbose logging: `ORT_LOGGING_LEVEL_VERBOSE`
- Check input tensor shapes match model
- Verify ONNX opset version is supported

---

## Example Models

### TensorFlow Lite Models
- [MobileNet](https://tfhub.dev/tensorflow/lite-model/mobilenet_v2_1.0_224/1/default/1)
- [EfficientNet](https://tfhub.dev/tensorflow/lite-model/efficientnet/lite0/uint8/2)
- [YOLO](https://github.com/tensorflow/models/tree/master/research/object_detection)

### ONNX Models
- [ONNX Model Zoo](https://github.com/onnx/models)
- [ResNet](https://github.com/onnx/models/tree/main/vision/classification/resnet)
- [MobileNet](https://github.com/onnx/models/tree/main/vision/classification/mobilenet)

---

## Support and Resources

- **TensorFlow Lite Documentation:** https://www.tensorflow.org/lite
- **ONNX Runtime Documentation:** https://onnxruntime.ai/docs/
- **LwM2M Edge AI Object Spec:** See `docs/specifications/lwm2m_object_33700.md`
- **Implementation Reference:** `src/objects/edge_ai_inference_object.cpp`
- **Test Suite:** `tests/objects/test_edge_ai_inference.cpp`

---

## License

This integration guide is part of the Friendly LwM2M Client project and is licensed under the MIT License.

---

**Ready to integrate ML inference?** Start with TensorFlow Lite for simplicity, or ONNX Runtime for maximum model compatibility. Both are production-ready and well-supported.
