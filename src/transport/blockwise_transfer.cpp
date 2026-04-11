/**
 * @file blockwise_transfer.cpp
 * @brief CoAP Block-wise Transfer Implementation
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#include "transport/blockwise_transfer.h"

#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstring>

#include <openssl/sha.h>

namespace lwm2m {
namespace transport {

// ============================================================================
// Constructor / Destructor
// ============================================================================

BlockwiseTransfer::BlockwiseTransfer(const Config& config)
    : config_(config) {
}

BlockwiseTransfer::BlockwiseTransfer()
    : config_() {
}

BlockwiseTransfer::~BlockwiseTransfer() {
    std::lock_guard<std::mutex> lock(mutex_);
    transfers_.clear();
}

// ============================================================================
// Transfer Management
// ============================================================================

BlockResult BlockwiseTransfer::startTransfer(const std::string& resource,
                                              const std::vector<uint8_t>& data,
                                              bool resumable) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string transferId = generateTransferId(resource);

    // Check if transfer already exists
    if (transfers_.find(transferId) != transfers_.end()) {
        return BlockResult::ERROR_UNKNOWN;
    }

    auto ctx = std::make_unique<TransferContext>();
    ctx->transferId = transferId;
    ctx->resourcePath = resource;
    ctx->isRequest = true;
    ctx->totalSize = data.size();
    ctx->buffer = data;
    ctx->resumable = resumable;
    ctx->startTime = std::chrono::steady_clock::now();
    ctx->lastActivityTime = ctx->startTime;
    ctx->retransmitCount = 0;
    ctx->currentBlock = 0;

    // Determine block size
    if (config_.bertEnabled && data.size() > 1024) {
        ctx->useBERT = true;
        ctx->blockSize = config_.bertBlockSize;
    } else {
        ctx->useBERT = false;
        ctx->blockSize = getBlockSizeBytes(config_.preferredBlockSize);
    }

    // Calculate total blocks
    ctx->totalBlocks = (data.size() + ctx->blockSize - 1) / ctx->blockSize;

    // Generate ETag for resumable transfers
    if (resumable) {
        ctx->checksum = calculateChecksum(data);
        // Use first 8 bytes of checksum as ETag
        std::stringstream ss;
        for (int i = 0; i < 8 && i < static_cast<int>(ctx->checksum.size()); i++) {
            ss << std::hex << std::setfill('0') << std::setw(2) 
               << static_cast<int>(ctx->checksum[i]);
        }
        ctx->etag = ss.str();
    }

    transfers_[transferId] = std::move(ctx);

    return BlockResult::SUCCESS;
}

std::string BlockwiseTransfer::startReceive(const std::string& resource,
                                             uint64_t expectedSize) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string transferId = generateTransferId(resource);

    auto ctx = std::make_unique<TransferContext>();
    ctx->transferId = transferId;
    ctx->resourcePath = resource;
    ctx->isRequest = false;
    ctx->totalSize = expectedSize;
    ctx->resumable = true;
    ctx->startTime = std::chrono::steady_clock::now();
    ctx->lastActivityTime = ctx->startTime;
    ctx->retransmitCount = 0;
    ctx->currentBlock = 0;

    // Use preferred block size
    if (config_.bertEnabled && (expectedSize == 0 || expectedSize > 1024)) {
        ctx->useBERT = true;
        ctx->blockSize = config_.bertBlockSize;
    } else {
        ctx->useBERT = false;
        ctx->blockSize = getBlockSizeBytes(config_.preferredBlockSize);
    }

    if (expectedSize > 0) {
        ctx->totalBlocks = (expectedSize + ctx->blockSize - 1) / ctx->blockSize;
        ctx->buffer.reserve(expectedSize);
    }

    transfers_[transferId] = std::move(ctx);

    return transferId;
}

BlockResult BlockwiseTransfer::resumeTransfer(const std::string& transferId,
                                               const std::string& etag) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = transfers_.find(transferId);
    if (it == transfers_.end()) {
        return BlockResult::ERROR_TRANSFER_NOT_FOUND;
    }

    auto& ctx = it->second;

    // Verify ETag matches
    if (!etag.empty() && ctx->etag != etag) {
        return BlockResult::ERROR_CHECKSUM_MISMATCH;
    }

    // Find first missing block
    auto missingBlocks = getMissingBlocks(*ctx);
    if (!missingBlocks.empty()) {
        ctx->currentBlock = *missingBlocks.begin();
    }

    ctx->lastActivityTime = std::chrono::steady_clock::now();
    ctx->retransmitCount = 0;

    return BlockResult::SUCCESS;
}

BlockResult BlockwiseTransfer::cancelTransfer(const std::string& transferId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = transfers_.find(transferId);
    if (it == transfers_.end()) {
        return BlockResult::ERROR_TRANSFER_NOT_FOUND;
    }

    transfers_.erase(it);
    return BlockResult::SUCCESS;
}

TransferStats BlockwiseTransfer::getStats(const std::string& transferId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    TransferStats stats{};

    auto it = transfers_.find(transferId);
    if (it == transfers_.end()) {
        return stats;
    }

    const auto& ctx = it->second;

    stats.totalBytes = ctx->totalSize;
    stats.transferredBytes = ctx->buffer.size();
    stats.totalBlocks = ctx->totalBlocks;
    stats.receivedBlocks = ctx->receivedBlockNumbers.size();
    stats.retransmittedBlocks = ctx->retransmitCount;

    auto missingBlocks = getMissingBlocks(*ctx);
    stats.missingBlocks = missingBlocks.size();

    if (ctx->totalSize > 0) {
        stats.progressPercent = (ctx->buffer.size() * 100.0) / ctx->totalSize;
    }

    auto now = std::chrono::steady_clock::now();
    stats.elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - ctx->startTime);

    if (stats.elapsed.count() > 0) {
        stats.throughput = (ctx->buffer.size() * 1000.0) / stats.elapsed.count();
    }

    if (stats.throughput > 0 && ctx->totalSize > ctx->buffer.size()) {
        uint64_t remaining = ctx->totalSize - ctx->buffer.size();
        stats.estimatedRemaining = std::chrono::milliseconds(
            static_cast<int64_t>((remaining * 1000.0) / stats.throughput));
    }

    return stats;
}

bool BlockwiseTransfer::isComplete(const std::string& transferId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = transfers_.find(transferId);
    if (it == transfers_.end()) {
        return false;
    }

    const auto& ctx = it->second;
    return ctx->buffer.size() >= ctx->totalSize && ctx->totalSize > 0;
}

std::vector<uint8_t> BlockwiseTransfer::getData(
    const std::string& transferId) const {
    
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = transfers_.find(transferId);
    if (it == transfers_.end()) {
        return {};
    }

    return it->second->buffer;
}

// ============================================================================
// Block Operations
// ============================================================================

BlockResult BlockwiseTransfer::getNextBlock(const std::string& transferId,
                                             uint32_t& blockNum,
                                             std::vector<uint8_t>& blockData,
                                             bool& moreBlocks) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = transfers_.find(transferId);
    if (it == transfers_.end()) {
        return BlockResult::ERROR_TRANSFER_NOT_FOUND;
    }

    auto& ctx = it->second;

    if (ctx->currentBlock >= ctx->totalBlocks) {
        return BlockResult::ERROR_TRANSFER_COMPLETE;
    }

    blockNum = ctx->currentBlock;

    // Calculate block offset and size
    uint64_t offset = static_cast<uint64_t>(blockNum) * ctx->blockSize;
    uint32_t size = std::min(static_cast<uint64_t>(ctx->blockSize),
                             ctx->totalSize - offset);

    // Extract block data
    blockData.assign(ctx->buffer.begin() + offset,
                     ctx->buffer.begin() + offset + size);

    moreBlocks = (ctx->currentBlock + 1) < ctx->totalBlocks;

    ctx->lastActivityTime = std::chrono::steady_clock::now();

    return BlockResult::SUCCESS;
}

BlockResult BlockwiseTransfer::receiveBlock(const std::string& transferId,
                                             uint32_t blockNum,
                                             const std::vector<uint8_t>& data,
                                             bool moreBlocks) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = transfers_.find(transferId);
    if (it == transfers_.end()) {
        return BlockResult::ERROR_TRANSFER_NOT_FOUND;
    }

    auto& ctx = it->second;

    // Check for duplicate block
    if (ctx->receivedBlockNumbers.count(blockNum) > 0) {
        return BlockResult::SUCCESS;  // Ignore duplicate
    }

    // Calculate expected offset
    uint64_t expectedOffset = static_cast<uint64_t>(blockNum) * ctx->blockSize;

    // Resize buffer if needed
    if (expectedOffset + data.size() > ctx->buffer.size()) {
        ctx->buffer.resize(expectedOffset + data.size());
    }

    // Copy block data
    std::memcpy(ctx->buffer.data() + expectedOffset, data.data(), data.size());

    // Mark block as received
    ctx->receivedBlockNumbers.insert(blockNum);
    ctx->lastActivityTime = std::chrono::steady_clock::now();

    // Update block size if server indicates different size
    if (data.size() < ctx->blockSize && moreBlocks) {
        // Server is using smaller blocks
        ctx->blockSize = data.size();
    }

    // Check if transfer complete
    if (!moreBlocks) {
        ctx->totalSize = expectedOffset + data.size();
        ctx->totalBlocks = blockNum + 1;

        // Verify checksum if available
        if (config_.verifyChecksum && !ctx->checksum.empty()) {
            auto actualChecksum = calculateChecksum(ctx->buffer);
            if (actualChecksum != ctx->checksum) {
                return BlockResult::ERROR_CHECKSUM_MISMATCH;
            }
        }

        // Notify completion
        if (completeCallback_) {
            completeCallback_(BlockResult::SUCCESS, ctx->buffer);
        }
    }

    // Update progress
    if (progressCallback_) {
        updateStats(*ctx);
        progressCallback_(getStats(transferId));
    }

    return BlockResult::SUCCESS;
}

BlockResult BlockwiseTransfer::acknowledgeBlock(const std::string& transferId,
                                                 uint32_t blockNum) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = transfers_.find(transferId);
    if (it == transfers_.end()) {
        return BlockResult::ERROR_TRANSFER_NOT_FOUND;
    }

    auto& ctx = it->second;

    if (blockNum != ctx->currentBlock) {
        return BlockResult::ERROR_OUT_OF_ORDER;
    }

    ctx->currentBlock++;
    ctx->lastActivityTime = std::chrono::steady_clock::now();

    // Check if transfer complete
    if (ctx->currentBlock >= ctx->totalBlocks) {
        if (completeCallback_) {
            completeCallback_(BlockResult::SUCCESS, ctx->buffer);
        }
    }

    return BlockResult::SUCCESS;
}

BlockResult BlockwiseTransfer::requestRetransmit(const std::string& transferId,
                                                  uint32_t blockNum) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = transfers_.find(transferId);
    if (it == transfers_.end()) {
        return BlockResult::ERROR_TRANSFER_NOT_FOUND;
    }

    auto& ctx = it->second;

    if (ctx->retransmitCount >= config_.maxRetransmits) {
        return BlockResult::ERROR_MAX_RETRIES;
    }

    ctx->currentBlock = blockNum;
    ctx->retransmitCount++;
    ctx->lastActivityTime = std::chrono::steady_clock::now();

    return BlockResult::SUCCESS;
}

// ============================================================================
// Block Option Encoding/Decoding
// ============================================================================

uint32_t BlockwiseTransfer::encodeBlockOption(uint32_t blockNum, bool more,
                                               BlockSize szx) {
    // Block option format: NUM (variable) + M (1 bit) + SZX (3 bits)
    // Lower 4 bits: M (bit 3) + SZX (bits 0-2)
    // Upper bits: block number
    
    uint32_t option = (blockNum << 4) | 
                      (more ? 0x08 : 0x00) | 
                      (static_cast<uint8_t>(szx) & 0x07);
    
    return option;
}

void BlockwiseTransfer::decodeBlockOption(uint32_t optionValue,
                                           uint32_t& blockNum,
                                           bool& more,
                                           BlockSize& szx) {
    blockNum = optionValue >> 4;
    more = (optionValue & 0x08) != 0;
    szx = static_cast<BlockSize>(optionValue & 0x07);
}

uint32_t BlockwiseTransfer::getBlockSizeBytes(BlockSize szx) {
    if (szx == BlockSize::BERT) {
        return 1024;  // Minimum BERT, actual may be larger
    }
    return 1 << (4 + static_cast<uint8_t>(szx));
}

BlockSize BlockwiseTransfer::getBlockSizeFromBytes(uint32_t size) {
    if (size >= 2048) return BlockSize::BERT;
    if (size >= 1024) return BlockSize::BLOCK_1024;
    if (size >= 512) return BlockSize::BLOCK_512;
    if (size >= 256) return BlockSize::BLOCK_256;
    if (size >= 128) return BlockSize::BLOCK_128;
    if (size >= 64) return BlockSize::BLOCK_64;
    if (size >= 32) return BlockSize::BLOCK_32;
    return BlockSize::BLOCK_16;
}

// ============================================================================
// Configuration
// ============================================================================

void BlockwiseTransfer::setPreferredBlockSize(BlockSize size) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.preferredBlockSize = size;
}

void BlockwiseTransfer::enableBERT(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.bertEnabled = enabled;
}

void BlockwiseTransfer::setBERTBlockSize(uint32_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.bertBlockSize = size;
}

void BlockwiseTransfer::setTransferTimeout(std::chrono::seconds timeout) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.transferTimeout = timeout;
}

void BlockwiseTransfer::setMaxRetransmits(uint32_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.maxRetransmits = count;
}

// ============================================================================
// Callbacks
// ============================================================================

void BlockwiseTransfer::setProgressCallback(BlockProgressCallback callback) {
    progressCallback_ = callback;
}

void BlockwiseTransfer::setCompleteCallback(BlockCompleteCallback callback) {
    completeCallback_ = callback;
}

// ============================================================================
// Utility
// ============================================================================

void BlockwiseTransfer::cleanup() {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = transfers_.begin();
    while (it != transfers_.end()) {
        if (isTransferExpired(*it->second)) {
            it = transfers_.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<std::string> BlockwiseTransfer::getActiveTransfers() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> ids;
    for (const auto& [id, ctx] : transfers_) {
        ids.push_back(id);
    }
    return ids;
}

// ============================================================================
// Private Methods
// ============================================================================

std::string BlockwiseTransfer::generateTransferId(const std::string& resource) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    ss << std::hex;

    // Add resource hash
    std::hash<std::string> hasher;
    ss << std::setfill('0') << std::setw(8) << (hasher(resource) & 0xFFFFFFFF);
    ss << "-";

    // Add random component
    for (int i = 0; i < 8; i++) {
        ss << dis(gen);
    }

    // Add timestamp
    auto now = std::chrono::steady_clock::now();
    auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    ss << "-" << std::setfill('0') << std::setw(8) << (ts & 0xFFFFFFFF);

    return ss.str();
}

std::vector<uint8_t> BlockwiseTransfer::calculateChecksum(
    const std::vector<uint8_t>& data) {
    
    std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);
    SHA256(data.data(), data.size(), hash.data());
    return hash;
}

void BlockwiseTransfer::updateStats(TransferContext& ctx) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - ctx.startTime);

    // Update throughput calculation, etc.
    // Stats are computed on-demand in getStats()
}

bool BlockwiseTransfer::isTransferExpired(const TransferContext& ctx) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - ctx.lastActivityTime);

    return elapsed > config_.transferTimeout;
}

std::set<uint32_t> BlockwiseTransfer::getMissingBlocks(
    const TransferContext& ctx) const {
    
    std::set<uint32_t> missing;

    if (ctx.totalBlocks == 0) {
        return missing;
    }

    for (uint32_t i = 0; i < ctx.totalBlocks; i++) {
        if (ctx.receivedBlockNumbers.count(i) == 0) {
            missing.insert(i);
        }
    }

    return missing;
}

} // namespace transport
} // namespace lwm2m
