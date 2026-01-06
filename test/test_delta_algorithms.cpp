/**
 * @file test_delta_algorithms.cpp
 * @brief Unit tests for Delta Firmware Update Algorithms
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "firmware/delta_algorithms.h"

#include <random>
#include <algorithm>
#include <cstring>

namespace lwm2m {
namespace firmware {
namespace test {

// ============================================================================
// Test Fixtures
// ============================================================================

class DeltaAlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create reproducible random data
        std::mt19937 gen(42);
        std::uniform_int_distribution<> dis(0, 255);
        
        // Generate source data (simulated firmware)
        sourceData_.resize(64 * 1024);  // 64KB
        for (auto& byte : sourceData_) {
            byte = dis(gen);
        }
        
        // Generate target data (modified firmware - ~10% changes)
        targetData_ = sourceData_;
        std::uniform_int_distribution<> changeDis(0, 9);
        for (size_t i = 0; i < targetData_.size(); i++) {
            if (changeDis(gen) == 0) {  // 10% chance to change
                targetData_[i] = dis(gen);
            }
        }
    }

    void TearDown() override {
        sourceData_.clear();
        targetData_.clear();
    }

    std::vector<uint8_t> sourceData_;
    std::vector<uint8_t> targetData_;
};

class BSDiffTest : public DeltaAlgorithmTest {
protected:
    void SetUp() override {
        DeltaAlgorithmTest::SetUp();
        algorithm_ = std::make_unique<BSDiffAlgorithm>();
    }

    std::unique_ptr<BSDiffAlgorithm> algorithm_;
};

class VCDiffTest : public DeltaAlgorithmTest {
protected:
    void SetUp() override {
        DeltaAlgorithmTest::SetUp();
        algorithm_ = std::make_unique<VCDiffAlgorithm>();
    }

    std::unique_ptr<VCDiffAlgorithm> algorithm_;
};

class CourgetteTest : public DeltaAlgorithmTest {
protected:
    void SetUp() override {
        DeltaAlgorithmTest::SetUp();
        algorithm_ = std::make_unique<CourgetteAlgorithm>();
    }

    std::unique_ptr<CourgetteAlgorithm> algorithm_;
};

// ============================================================================
// BSDIFF Algorithm Tests
// ============================================================================

TEST_F(BSDiffTest, GetAlgorithmInfo) {
    EXPECT_EQ(algorithm_->getAlgorithm(), DeltaAlgorithm::BSDIFF);
    EXPECT_EQ(algorithm_->getName(), "BSDIFF");
}

TEST_F(BSDiffTest, CreateAndApplyDelta) {
    std::vector<uint8_t> delta;
    std::vector<uint8_t> result;
    
    // Create delta
    auto createResult = algorithm_->createDelta(sourceData_, targetData_, delta);
    ASSERT_EQ(createResult, DeltaResult::SUCCESS);
    EXPECT_GT(delta.size(), 0u);
    
    // Delta should be smaller than full target
    EXPECT_LT(delta.size(), targetData_.size());
    
    // Apply delta
    auto applyResult = algorithm_->applyDelta(sourceData_, delta, result);
    ASSERT_EQ(applyResult, DeltaResult::SUCCESS);
    
    // Result should match target
    ASSERT_EQ(result.size(), targetData_.size());
    EXPECT_EQ(result, targetData_);
}

TEST_F(BSDiffTest, IdenticalSourceAndTarget) {
    std::vector<uint8_t> delta;
    std::vector<uint8_t> result;
    
    // Create delta for identical data
    auto createResult = algorithm_->createDelta(sourceData_, sourceData_, delta);
    ASSERT_EQ(createResult, DeltaResult::SUCCESS);
    
    // Delta should be very small for identical data
    EXPECT_LT(delta.size(), sourceData_.size() / 10);
    
    // Apply and verify
    auto applyResult = algorithm_->applyDelta(sourceData_, delta, result);
    ASSERT_EQ(applyResult, DeltaResult::SUCCESS);
    EXPECT_EQ(result, sourceData_);
}

TEST_F(BSDiffTest, EmptySourceReturnsError) {
    std::vector<uint8_t> empty;
    std::vector<uint8_t> delta;
    
    auto result = algorithm_->createDelta(empty, targetData_, delta);
    EXPECT_EQ(result, DeltaResult::ERROR_INVALID_INPUT);
}

TEST_F(BSDiffTest, EmptyTargetReturnsError) {
    std::vector<uint8_t> empty;
    std::vector<uint8_t> delta;
    
    auto result = algorithm_->createDelta(sourceData_, empty, delta);
    EXPECT_EQ(result, DeltaResult::ERROR_INVALID_INPUT);
}

TEST_F(BSDiffTest, InvalidDeltaReturnsError) {
    std::vector<uint8_t> invalidDelta = {0x00, 0x01, 0x02, 0x03};
    std::vector<uint8_t> result;
    
    auto applyResult = algorithm_->applyDelta(sourceData_, invalidDelta, result);
    EXPECT_EQ(applyResult, DeltaResult::ERROR_INVALID_DELTA);
}

TEST_F(BSDiffTest, ValidateDelta) {
    std::vector<uint8_t> delta;
    algorithm_->createDelta(sourceData_, targetData_, delta);
    
    auto result = algorithm_->validateDelta(delta, sourceData_.size());
    EXPECT_EQ(result, DeltaResult::SUCCESS);
}

TEST_F(BSDiffTest, ProgressCallback) {
    std::vector<uint8_t> delta;
    int callbackCount = 0;
    uint64_t lastProgress = 0;
    
    auto callback = [&](uint64_t current, uint64_t total) {
        callbackCount++;
        EXPECT_GE(current, lastProgress);
        EXPECT_LE(current, total);
        lastProgress = current;
    };
    
    algorithm_->createDelta(sourceData_, targetData_, delta, callback);
    EXPECT_GT(callbackCount, 0);
}

TEST_F(BSDiffTest, LargeDataHandling) {
    // Test with larger data (1MB)
    std::mt19937 gen(123);
    std::uniform_int_distribution<> dis(0, 255);
    
    std::vector<uint8_t> largeSource(1024 * 1024);
    for (auto& byte : largeSource) {
        byte = dis(gen);
    }
    
    std::vector<uint8_t> largeTarget = largeSource;
    // Modify 5% of bytes
    for (size_t i = 0; i < largeTarget.size(); i += 20) {
        largeTarget[i] = dis(gen);
    }
    
    std::vector<uint8_t> delta;
    std::vector<uint8_t> result;
    
    auto createResult = algorithm_->createDelta(largeSource, largeTarget, delta);
    ASSERT_EQ(createResult, DeltaResult::SUCCESS);
    
    auto applyResult = algorithm_->applyDelta(largeSource, delta, result);
    ASSERT_EQ(applyResult, DeltaResult::SUCCESS);
    
    EXPECT_EQ(result, largeTarget);
}

// ============================================================================
// VCDIFF Algorithm Tests
// ============================================================================

TEST_F(VCDiffTest, GetAlgorithmInfo) {
    EXPECT_EQ(algorithm_->getAlgorithm(), DeltaAlgorithm::VCDIFF);
    EXPECT_EQ(algorithm_->getName(), "VCDIFF");
}

TEST_F(VCDiffTest, CreateAndApplyDelta) {
    std::vector<uint8_t> delta;
    std::vector<uint8_t> result;
    
    auto createResult = algorithm_->createDelta(sourceData_, targetData_, delta);
    ASSERT_EQ(createResult, DeltaResult::SUCCESS);
    EXPECT_GT(delta.size(), 0u);
    
    auto applyResult = algorithm_->applyDelta(sourceData_, delta, result);
    ASSERT_EQ(applyResult, DeltaResult::SUCCESS);
    
    ASSERT_EQ(result.size(), targetData_.size());
    EXPECT_EQ(result, targetData_);
}

TEST_F(VCDiffTest, ValidateMagicHeader) {
    std::vector<uint8_t> delta;
    algorithm_->createDelta(sourceData_, targetData_, delta);
    
    // VCDIFF magic: 0xD6 0xC3 0xC4 0x00
    ASSERT_GE(delta.size(), 4u);
    EXPECT_EQ(delta[0], 0xD6);
    EXPECT_EQ(delta[1], 0xC3);
    EXPECT_EQ(delta[2], 0xC4);
    EXPECT_EQ(delta[3], 0x00);
}

TEST_F(VCDiffTest, TextOptimization) {
    // VCDIFF is optimized for text-like data
    std::string sourceText = 
        "The quick brown fox jumps over the lazy dog. "
        "This is a test of the VCDIFF algorithm for text data.";
    std::string targetText = 
        "The quick brown fox jumps over the lazy cat. "
        "This is a test of the VCDIFF algorithm for text data!";
    
    std::vector<uint8_t> source(sourceText.begin(), sourceText.end());
    std::vector<uint8_t> target(targetText.begin(), targetText.end());
    std::vector<uint8_t> delta;
    std::vector<uint8_t> result;
    
    algorithm_->createDelta(source, target, delta);
    algorithm_->applyDelta(source, delta, result);
    
    EXPECT_EQ(result, target);
}

// ============================================================================
// Courgette Algorithm Tests
// ============================================================================

TEST_F(CourgetteTest, GetAlgorithmInfo) {
    EXPECT_EQ(algorithm_->getAlgorithm(), DeltaAlgorithm::COURGETTE);
    EXPECT_EQ(algorithm_->getName(), "Courgette");
}

TEST_F(CourgetteTest, DetectELF64Format) {
    // Create minimal ELF64 header
    std::vector<uint8_t> elfData = {
        0x7F, 'E', 'L', 'F',  // Magic
        2,                     // 64-bit
        1,                     // Little endian
        1,                     // ELF version
        0,                     // OS/ABI
    };
    elfData.resize(64);  // Minimum ELF header size
    
    auto format = CourgetteAlgorithm::detectFormat(elfData);
    EXPECT_EQ(format, CourgetteAlgorithm::ExecutableFormat::ELF64);
}

TEST_F(CourgetteTest, DetectELF32Format) {
    std::vector<uint8_t> elfData = {
        0x7F, 'E', 'L', 'F',
        1,  // 32-bit
        1, 1, 0,
    };
    elfData.resize(64);
    
    auto format = CourgetteAlgorithm::detectFormat(elfData);
    EXPECT_EQ(format, CourgetteAlgorithm::ExecutableFormat::ELF32);
}

TEST_F(CourgetteTest, DetectPEFormat) {
    // Create minimal PE header
    std::vector<uint8_t> peData(256, 0);
    peData[0] = 'M';
    peData[1] = 'Z';
    // PE offset at byte 60
    peData[60] = 64;  // PE header at offset 64
    peData[64] = 'P';
    peData[65] = 'E';
    peData[66] = 0;
    peData[67] = 0;
    // Machine type at PE+4 (x64 = 0x8664)
    peData[68] = 0x64;
    peData[69] = 0x86;
    
    auto format = CourgetteAlgorithm::detectFormat(peData);
    EXPECT_EQ(format, CourgetteAlgorithm::ExecutableFormat::PE64);
}

TEST_F(CourgetteTest, DetectUnknownFormat) {
    auto format = CourgetteAlgorithm::detectFormat(sourceData_);
    EXPECT_EQ(format, CourgetteAlgorithm::ExecutableFormat::UNKNOWN);
}

TEST_F(CourgetteTest, IsExecutable) {
    std::vector<uint8_t> elfData = {0x7F, 'E', 'L', 'F', 2, 1, 1, 0};
    elfData.resize(64);
    
    EXPECT_TRUE(CourgetteAlgorithm::isExecutable(elfData));
    EXPECT_FALSE(CourgetteAlgorithm::isExecutable(sourceData_));
}

TEST_F(CourgetteTest, FallbackToBSDiff) {
    // For non-executable data, Courgette should fall back to BSDIFF
    std::vector<uint8_t> delta;
    std::vector<uint8_t> result;
    
    auto createResult = algorithm_->createDelta(sourceData_, targetData_, delta);
    ASSERT_EQ(createResult, DeltaResult::SUCCESS);
    
    auto applyResult = algorithm_->applyDelta(sourceData_, delta, result);
    ASSERT_EQ(applyResult, DeltaResult::SUCCESS);
    
    EXPECT_EQ(result, targetData_);
}

// ============================================================================
// Factory Tests
// ============================================================================

TEST(DeltaAlgorithmFactoryTest, CreateBSDiff) {
    auto algorithm = DeltaAlgorithmFactory::create(DeltaAlgorithm::BSDIFF);
    ASSERT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->getAlgorithm(), DeltaAlgorithm::BSDIFF);
}

TEST(DeltaAlgorithmFactoryTest, CreateVCDiff) {
    auto algorithm = DeltaAlgorithmFactory::create(DeltaAlgorithm::VCDIFF);
    ASSERT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->getAlgorithm(), DeltaAlgorithm::VCDIFF);
}

TEST(DeltaAlgorithmFactoryTest, CreateCourgette) {
    auto algorithm = DeltaAlgorithmFactory::create(DeltaAlgorithm::COURGETTE);
    ASSERT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->getAlgorithm(), DeltaAlgorithm::COURGETTE);
}

TEST(DeltaAlgorithmFactoryTest, GetAvailableAlgorithms) {
    auto algorithms = DeltaAlgorithmFactory::getAvailableAlgorithms();
    EXPECT_GE(algorithms.size(), 3u);
    
    EXPECT_NE(std::find(algorithms.begin(), algorithms.end(), DeltaAlgorithm::BSDIFF),
              algorithms.end());
    EXPECT_NE(std::find(algorithms.begin(), algorithms.end(), DeltaAlgorithm::VCDIFF),
              algorithms.end());
    EXPECT_NE(std::find(algorithms.begin(), algorithms.end(), DeltaAlgorithm::COURGETTE),
              algorithms.end());
}

TEST(DeltaAlgorithmFactoryTest, RecommendAlgorithmForExecutable) {
    std::vector<uint8_t> elfSource = {0x7F, 'E', 'L', 'F', 2, 1, 1, 0};
    elfSource.resize(64);
    std::vector<uint8_t> elfTarget = elfSource;
    
    auto recommended = DeltaAlgorithmFactory::recommendAlgorithm(elfSource, elfTarget);
    EXPECT_EQ(recommended, DeltaAlgorithm::COURGETTE);
}

TEST(DeltaAlgorithmFactoryTest, RecommendBSDiffForBinary) {
    std::vector<uint8_t> binaryData(1000, 0x42);
    auto recommended = DeltaAlgorithmFactory::recommendAlgorithm(binaryData, binaryData);
    EXPECT_EQ(recommended, DeltaAlgorithm::BSDIFF);
}

// ============================================================================
// Compression Tests
// ============================================================================

TEST(DeltaCompressionTest, ZlibCompression) {
    std::vector<uint8_t> data(10000);
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = i % 256;
    }
    
    std::vector<uint8_t> compressed;
    auto result = DeltaCompression::compress(data, compressed, CompressionType::ZLIB);
    ASSERT_EQ(result, DeltaResult::SUCCESS);
    
    // Compressed should be smaller (repetitive data compresses well)
    EXPECT_LT(compressed.size(), data.size());
    
    std::vector<uint8_t> decompressed;
    result = DeltaCompression::decompress(compressed, decompressed, CompressionType::ZLIB, data.size());
    ASSERT_EQ(result, DeltaResult::SUCCESS);
    
    EXPECT_EQ(decompressed, data);
}

TEST(DeltaCompressionTest, NoCompression) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    std::vector<uint8_t> compressed;
    
    auto result = DeltaCompression::compress(data, compressed, CompressionType::NONE);
    ASSERT_EQ(result, DeltaResult::SUCCESS);
    EXPECT_EQ(compressed, data);
    
    std::vector<uint8_t> decompressed;
    result = DeltaCompression::decompress(compressed, decompressed, CompressionType::NONE);
    ASSERT_EQ(result, DeltaResult::SUCCESS);
    EXPECT_EQ(decompressed, data);
}

TEST(DeltaCompressionTest, CheckAvailability) {
    EXPECT_TRUE(DeltaCompression::isAvailable(CompressionType::NONE));
    EXPECT_TRUE(DeltaCompression::isAvailable(CompressionType::ZLIB));
}

TEST(DeltaCompressionTest, EstimateCompressedSize) {
    std::vector<uint8_t> data(1000);
    auto estimate = DeltaCompression::estimateCompressedSize(data, CompressionType::ZLIB);
    EXPECT_GT(estimate, 0u);
}

#ifdef HAS_BZIP2
TEST(DeltaCompressionTest, BZip2Compression) {
    std::vector<uint8_t> data(10000);
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = i % 256;
    }
    
    std::vector<uint8_t> compressed;
    auto result = DeltaCompression::compress(data, compressed, CompressionType::BZIP2);
    ASSERT_EQ(result, DeltaResult::SUCCESS);
    
    std::vector<uint8_t> decompressed;
    result = DeltaCompression::decompress(compressed, decompressed, CompressionType::BZIP2, data.size());
    ASSERT_EQ(result, DeltaResult::SUCCESS);
    
    EXPECT_EQ(decompressed, data);
}
#endif

#ifdef HAS_ZSTD
TEST(DeltaCompressionTest, ZstdCompression) {
    std::vector<uint8_t> data(10000);
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = i % 256;
    }
    
    std::vector<uint8_t> compressed;
    auto result = DeltaCompression::compress(data, compressed, CompressionType::ZSTD);
    ASSERT_EQ(result, DeltaResult::SUCCESS);
    
    std::vector<uint8_t> decompressed;
    result = DeltaCompression::decompress(compressed, decompressed, CompressionType::ZSTD);
    ASSERT_EQ(result, DeltaResult::SUCCESS);
    
    EXPECT_EQ(decompressed, data);
}
#endif

} // namespace test
} // namespace firmware
} // namespace lwm2m
