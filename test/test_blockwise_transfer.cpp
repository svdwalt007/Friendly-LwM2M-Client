/**
 * @file test_blockwise_transfer.cpp
 * @brief Unit tests for CoAP Block-wise Transfer (RFC 7959)
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "transport/blockwise_transfer.h"

#include <random>
#include <chrono>
#include <thread>

namespace lwm2m {
namespace transport {
namespace test {

// ============================================================================
// Test Fixtures
// ============================================================================

class BlockwiseTransferTest : public ::testing::Test {
protected:
    void SetUp() override {
        transfer_ = std::make_unique<BlockwiseTransfer>();
        
        // Create test data
        std::mt19937 gen(42);
        std::uniform_int_distribution<> dis(0, 255);
        
        testData_.resize(100 * 1024);  // 100KB
        for (auto& byte : testData_) {
            byte = dis(gen);
        }
    }

    void TearDown() override {
        transfer_.reset();
        testData_.clear();
    }

    std::unique_ptr<BlockwiseTransfer> transfer_;
    std::vector<uint8_t> testData_;
};

// ============================================================================
// Basic Configuration Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, DefaultConfiguration) {
    EXPECT_EQ(transfer_->getPreferredBlockSize(), BlockwiseTransfer::BlockSize::BLOCK_1024);
    EXPECT_FALSE(transfer_->isBERTEnabled());
}

TEST_F(BlockwiseTransferTest, SetBlockSize) {
    transfer_->setPreferredBlockSize(BlockwiseTransfer::BlockSize::BLOCK_512);
    EXPECT_EQ(transfer_->getPreferredBlockSize(), BlockwiseTransfer::BlockSize::BLOCK_512);
    
    transfer_->setPreferredBlockSize(BlockwiseTransfer::BlockSize::BLOCK_256);
    EXPECT_EQ(transfer_->getPreferredBlockSize(), BlockwiseTransfer::BlockSize::BLOCK_256);
}

TEST_F(BlockwiseTransferTest, EnableBERT) {
    transfer_->enableBERT(true);
    EXPECT_TRUE(transfer_->isBERTEnabled());
    
    transfer_->enableBERT(false);
    EXPECT_FALSE(transfer_->isBERTEnabled());
}

TEST_F(BlockwiseTransferTest, SetTransferTimeout) {
    transfer_->setTransferTimeout(std::chrono::seconds(300));
    // Verify through behavior in actual transfer
}

TEST_F(BlockwiseTransferTest, SetMaxRetransmits) {
    transfer_->setMaxRetransmits(10);
    // Verify through behavior in actual transfer
}

// ============================================================================
// Block Size Calculation Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, BlockSizeValues) {
    EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_16), 16u);
    EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_32), 32u);
    EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_64), 64u);
    EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_128), 128u);
    EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_256), 256u);
    EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_512), 512u);
    EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_1024), 1024u);
}

TEST_F(BlockwiseTransferTest, SZXValues) {
    EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_16), 0);
    EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_32), 1);
    EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_64), 2);
    EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_128), 3);
    EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_256), 4);
    EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_512), 5);
    EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_1024), 6);
}

// ============================================================================
// Transfer Initiation Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, StartTransfer) {
    auto result = transfer_->startTransfer("/test/resource", testData_, true);
    EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
}

TEST_F(BlockwiseTransferTest, StartTransferEmptyData) {
    std::vector<uint8_t> empty;
    auto result = transfer_->startTransfer("/test/resource", empty, false);
    EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_INVALID_DATA);
}

TEST_F(BlockwiseTransferTest, StartTransferEmptyResource) {
    auto result = transfer_->startTransfer("", testData_, false);
    EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_INVALID_RESOURCE);
}

// ============================================================================
// ETag Generation Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, ETagGeneration) {
    transfer_->startTransfer("/test/resource", testData_, true);
    
    auto transferId = transfer_->getActiveTransferId();
    EXPECT_FALSE(transferId.empty());
    
    auto etag = transfer_->getETag(transferId);
    EXPECT_FALSE(etag.empty());
    EXPECT_LE(etag.size(), 8u);  // ETag should be at most 8 bytes
}

TEST_F(BlockwiseTransferTest, UniqueETagsForDifferentData) {
    std::vector<uint8_t> data1 = {1, 2, 3, 4, 5};
    std::vector<uint8_t> data2 = {5, 4, 3, 2, 1};
    
    transfer_->startTransfer("/resource1", data1, true);
    auto id1 = transfer_->getActiveTransferId();
    auto etag1 = transfer_->getETag(id1);
    
    transfer_->cancelTransfer(id1);
    
    transfer_->startTransfer("/resource2", data2, true);
    auto id2 = transfer_->getActiveTransferId();
    auto etag2 = transfer_->getETag(id2);
    
    EXPECT_NE(etag1, etag2);
}

// ============================================================================
// Block Reception Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, ReceiveBlocks) {
    // Simulate receiving a transfer
    transfer_->setPreferredBlockSize(BlockwiseTransfer::BlockSize::BLOCK_1024);
    
    std::string resourcePath = "/firmware/update";
    uint64_t totalSize = 4096;  // 4KB = 4 blocks of 1024
    
    auto result = transfer_->beginReceive(resourcePath, totalSize);
    EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
    
    auto transferId = transfer_->getActiveTransferId();
    
    // Receive blocks
    for (uint32_t i = 0; i < 4; i++) {
        std::vector<uint8_t> block(1024, static_cast<uint8_t>(i));
        bool moreBlocks = (i < 3);
        
        result = transfer_->receiveBlock(transferId, i, block, moreBlocks);
        EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
    }
    
    // Check transfer complete
    EXPECT_TRUE(transfer_->isTransferComplete(transferId));
}

TEST_F(BlockwiseTransferTest, OutOfOrderBlockRejected) {
    transfer_->setPreferredBlockSize(BlockwiseTransfer::BlockSize::BLOCK_1024);
    
    transfer_->beginReceive("/resource", 2048);
    auto transferId = transfer_->getActiveTransferId();
    
    std::vector<uint8_t> block(1024, 0);
    
    // Receive block 0
    transfer_->receiveBlock(transferId, 0, block, true);
    
    // Try to receive block 2 (skipping 1)
    auto result = transfer_->receiveBlock(transferId, 2, block, true);
    EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_BLOCK_OUT_OF_ORDER);
}

// ============================================================================
// Resume Transfer Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, ResumeTransfer) {
    transfer_->startTransfer("/resource", testData_, true);
    auto transferId = transfer_->getActiveTransferId();
    auto etag = transfer_->getETag(transferId);
    
    // Simulate interruption - cancel without completing
    transfer_->cancelTransfer(transferId);
    
    // Resume with same ETag
    auto result = transfer_->resumeTransfer(transferId, etag);
    EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
}

TEST_F(BlockwiseTransferTest, ResumeWithWrongETag) {
    transfer_->startTransfer("/resource", testData_, true);
    auto transferId = transfer_->getActiveTransferId();
    
    transfer_->cancelTransfer(transferId);
    
    std::string wrongEtag = "wrongetag";
    auto result = transfer_->resumeTransfer(transferId, wrongEtag);
    EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_ETAG_MISMATCH);
}

// ============================================================================
// Statistics Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, TransferStatistics) {
    transfer_->beginReceive("/resource", 4096);
    auto transferId = transfer_->getActiveTransferId();
    
    std::vector<uint8_t> block(1024, 0);
    transfer_->receiveBlock(transferId, 0, block, true);
    transfer_->receiveBlock(transferId, 1, block, true);
    
    auto stats = transfer_->getStats(transferId);
    
    EXPECT_EQ(stats.totalSize, 4096u);
    EXPECT_EQ(stats.transferredSize, 2048u);
    EXPECT_EQ(stats.blockSize, 1024u);
    EXPECT_EQ(stats.blocksTransferred, 2u);
    EXPECT_EQ(stats.progressPercent, 50);
}

TEST_F(BlockwiseTransferTest, ThroughputCalculation) {
    transfer_->beginReceive("/resource", 10240);
    auto transferId = transfer_->getActiveTransferId();
    
    std::vector<uint8_t> block(1024, 0);
    
    // Simulate receiving blocks with delay
    for (int i = 0; i < 10; i++) {
        transfer_->receiveBlock(transferId, i, block, i < 9);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    auto stats = transfer_->getStats(transferId);
    
    // Should have some measured throughput
    EXPECT_GT(stats.throughputBytesPerSec, 0.0);
}

// ============================================================================
// BERT (Block-wise Extension for Robust Transport) Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, BERTBlockSize) {
    transfer_->enableBERT(true);
    transfer_->setBERTBlockSize(8192);
    
    EXPECT_EQ(transfer_->getBERTBlockSize(), 8192u);
}

TEST_F(BlockwiseTransferTest, BERTForLargeTransfer) {
    transfer_->enableBERT(true);
    transfer_->setBERTBlockSize(8192);
    
    // For data > 1024 bytes, BERT should be used
    std::vector<uint8_t> largeData(10000, 0x42);
    
    transfer_->startTransfer("/resource", largeData, false);
    auto transferId = transfer_->getActiveTransferId();
    
    auto stats = transfer_->getStats(transferId);
    EXPECT_TRUE(stats.usingBERT);
}

TEST_F(BlockwiseTransferTest, BERTNotUsedForSmallTransfer) {
    transfer_->enableBERT(true);
    
    std::vector<uint8_t> smallData(500, 0x42);
    
    transfer_->startTransfer("/resource", smallData, false);
    auto transferId = transfer_->getActiveTransferId();
    
    auto stats = transfer_->getStats(transferId);
    EXPECT_FALSE(stats.usingBERT);
}

// ============================================================================
// Checksum Verification Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, ChecksumCalculation) {
    transfer_->beginReceive("/resource", 2048);
    auto transferId = transfer_->getActiveTransferId();
    
    std::vector<uint8_t> block1(1024, 0xAA);
    std::vector<uint8_t> block2(1024, 0xBB);
    
    transfer_->receiveBlock(transferId, 0, block1, true);
    transfer_->receiveBlock(transferId, 1, block2, false);
    
    auto checksum = transfer_->getChecksum(transferId);
    EXPECT_EQ(checksum.size(), 32u);  // SHA256
}

TEST_F(BlockwiseTransferTest, VerifyChecksumSuccess) {
    transfer_->startTransfer("/resource", testData_, true);
    auto transferId = transfer_->getActiveTransferId();
    
    // Get expected checksum
    auto expectedChecksum = transfer_->getChecksum(transferId);
    
    // Verify
    auto result = transfer_->verifyChecksum(transferId, expectedChecksum);
    EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
}

TEST_F(BlockwiseTransferTest, VerifyChecksumFailure) {
    transfer_->startTransfer("/resource", testData_, true);
    auto transferId = transfer_->getActiveTransferId();
    
    // Wrong checksum
    std::vector<uint8_t> wrongChecksum(32, 0xFF);
    
    auto result = transfer_->verifyChecksum(transferId, wrongChecksum);
    EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_CHECKSUM_MISMATCH);
}

// ============================================================================
// Cancellation Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, CancelTransfer) {
    transfer_->startTransfer("/resource", testData_, true);
    auto transferId = transfer_->getActiveTransferId();
    
    auto result = transfer_->cancelTransfer(transferId);
    EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
    
    // Transfer should no longer be active
    EXPECT_TRUE(transfer_->getActiveTransferId().empty());
}

TEST_F(BlockwiseTransferTest, CancelNonExistentTransfer) {
    auto result = transfer_->cancelTransfer("nonexistent");
    EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_TRANSFER_NOT_FOUND);
}

// ============================================================================
// Progress Callback Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, ProgressCallback) {
    int callbackCount = 0;
    uint64_t lastCurrent = 0;
    
    transfer_->setProgressCallback([&](uint64_t current, uint64_t total) {
        callbackCount++;
        EXPECT_GE(current, lastCurrent);
        EXPECT_LE(current, total);
        lastCurrent = current;
    });
    
    transfer_->beginReceive("/resource", 4096);
    auto transferId = transfer_->getActiveTransferId();
    
    std::vector<uint8_t> block(1024, 0);
    for (int i = 0; i < 4; i++) {
        transfer_->receiveBlock(transferId, i, block, i < 3);
    }
    
    EXPECT_GT(callbackCount, 0);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(BlockwiseTransferTest, HandleDuplicateBlock) {
    transfer_->beginReceive("/resource", 2048);
    auto transferId = transfer_->getActiveTransferId();
    
    std::vector<uint8_t> block(1024, 0);
    
    // Receive block 0
    transfer_->receiveBlock(transferId, 0, block, true);
    
    // Receive block 0 again (duplicate)
    auto result = transfer_->receiveBlock(transferId, 0, block, true);
    EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);  // Should ignore duplicate
    
    auto stats = transfer_->getStats(transferId);
    EXPECT_EQ(stats.blocksTransferred, 1u);  // Still only 1 block
}

TEST_F(BlockwiseTransferTest, BlockSizeMismatch) {
    transfer_->setPreferredBlockSize(BlockwiseTransfer::BlockSize::BLOCK_1024);
    transfer_->beginReceive("/resource", 2048);
    auto transferId = transfer_->getActiveTransferId();
    
    // Send smaller block than expected (except for last block)
    std::vector<uint8_t> smallBlock(512, 0);
    auto result = transfer_->receiveBlock(transferId, 0, smallBlock, true);
    EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_BLOCK_SIZE_MISMATCH);
}

} // namespace test
} // namespace transport
} // namespace lwm2m
