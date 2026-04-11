/**
 * @file blockwise_transfer.h
 * @brief CoAP Block-wise Transfer Handler (RFC 7959 / RFC 8323)
 * 
 * Complete implementation of CoAP block-wise transfers with:
 * - Configurable block sizes (16 bytes to 1024 bytes)
 * - BERT support (Block-wise Extension for Robust Transport)
 * - Resumable transfers with ETag
 * - Transfer statistics and progress tracking
 * - Automatic retry and missing block recovery
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_BLOCKWISE_TRANSFER_H
#define FRIENDLY_LWM2M_BLOCKWISE_TRANSFER_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <mutex>
#include <chrono>
#include <set>

namespace lwm2m {
namespace transport {

/**
 * @brief Result codes for block transfer operations
 */
enum class BlockResult {
    SUCCESS = 0,
    ERROR_INVALID_BLOCK,
    ERROR_OUT_OF_ORDER,
    ERROR_TRANSFER_NOT_FOUND,
    ERROR_TRANSFER_COMPLETE,
    ERROR_CHECKSUM_MISMATCH,
    ERROR_TIMEOUT,
    ERROR_MAX_RETRIES,
    ERROR_SIZE_EXCEEDED,
    ERROR_UNKNOWN
};

/**
 * @brief Block size options (SZX values per RFC 7959)
 */
enum class BlockSize : uint8_t {
    BLOCK_16 = 0,       ///< 16 bytes (2^4)
    BLOCK_32 = 1,       ///< 32 bytes (2^5)
    BLOCK_64 = 2,       ///< 64 bytes (2^6)
    BLOCK_128 = 3,      ///< 128 bytes (2^7)
    BLOCK_256 = 4,      ///< 256 bytes (2^8)
    BLOCK_512 = 5,      ///< 512 bytes (2^9)
    BLOCK_1024 = 6,     ///< 1024 bytes (2^10)
    BERT = 7            ///< BERT mode (2048+ bytes)
};

/**
 * @brief Transfer statistics
 */
struct TransferStats {
    uint64_t totalBytes;
    uint64_t transferredBytes;
    uint32_t totalBlocks;
    uint32_t receivedBlocks;
    uint32_t retransmittedBlocks;
    uint32_t missingBlocks;
    double progressPercent;
    double throughput;              ///< Bytes per second
    std::chrono::milliseconds elapsed;
    std::chrono::milliseconds estimatedRemaining;
};

/**
 * @brief Transfer context
 */
struct TransferContext {
    std::string transferId;
    std::string resourcePath;
    bool isRequest;                 ///< true = sending, false = receiving
    uint64_t totalSize;
    uint32_t blockSize;
    uint32_t currentBlock;
    uint32_t totalBlocks;
    std::vector<uint8_t> buffer;
    std::string etag;               ///< For resumable transfers
    std::vector<uint8_t> checksum;
    bool resumable;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastActivityTime;
    uint32_t retransmitCount;
    std::set<uint32_t> receivedBlockNumbers;
    bool useBERT;
    uint32_t bertBlockSize;
};

/**
 * @brief Block transfer progress callback
 */
using BlockProgressCallback = std::function<void(const TransferStats& stats)>;

/**
 * @brief Block transfer completion callback
 */
using BlockCompleteCallback = std::function<void(BlockResult result,
                                                  const std::vector<uint8_t>& data)>;

/**
 * @brief Complete Block-wise Transfer Implementation
 */
class BlockwiseTransfer {
public:
    /**
     * @brief Configuration
     */
    struct Config {
        BlockSize preferredBlockSize = BlockSize::BLOCK_1024;
        bool bertEnabled = true;
        uint32_t bertBlockSize = 8192;      ///< Default BERT block size
        uint32_t maxRetransmits = 5;
        std::chrono::seconds transferTimeout{600};
        std::chrono::seconds blockTimeout{30};
        bool autoRetry = true;
        bool verifyChecksum = true;
    };

    /**
     * @brief Constructor
     */
    explicit BlockwiseTransfer(const Config& config = {});

    /**
     * @brief Destructor
     */
    ~BlockwiseTransfer();

    // ========================================================================
    // Transfer Management
    // ========================================================================

    /**
     * @brief Start a new block transfer (sending)
     * @param resource Resource path
     * @param data Data to send
     * @param resumable Enable resumable transfer
     * @return Result code
     */
    BlockResult startTransfer(const std::string& resource,
                              const std::vector<uint8_t>& data,
                              bool resumable = true);

    /**
     * @brief Start receiving a block transfer
     * @param resource Resource path
     * @param expectedSize Expected total size (0 if unknown)
     * @return Transfer ID
     */
    std::string startReceive(const std::string& resource, 
                             uint64_t expectedSize = 0);

    /**
     * @brief Resume an interrupted transfer
     * @param transferId Transfer ID
     * @param etag ETag from previous transfer
     * @return Result code
     */
    BlockResult resumeTransfer(const std::string& transferId,
                               const std::string& etag);

    /**
     * @brief Cancel a transfer
     * @param transferId Transfer ID
     * @return Result code
     */
    BlockResult cancelTransfer(const std::string& transferId);

    /**
     * @brief Get transfer status
     * @param transferId Transfer ID
     * @return Transfer stats
     */
    TransferStats getStats(const std::string& transferId) const;

    /**
     * @brief Check if transfer is complete
     * @param transferId Transfer ID
     * @return true if complete
     */
    bool isComplete(const std::string& transferId) const;

    /**
     * @brief Get transfer data
     * @param transferId Transfer ID
     * @return Data buffer
     */
    std::vector<uint8_t> getData(const std::string& transferId) const;

    // ========================================================================
    // Block Operations
    // ========================================================================

    /**
     * @brief Get next block to send
     * @param transferId Transfer ID
     * @param blockNum Output: block number
     * @param blockData Output: block data
     * @param moreBlocks Output: more blocks flag
     * @return Result code
     */
    BlockResult getNextBlock(const std::string& transferId,
                             uint32_t& blockNum,
                             std::vector<uint8_t>& blockData,
                             bool& moreBlocks);

    /**
     * @brief Receive a block
     * @param transferId Transfer ID
     * @param blockNum Block number
     * @param data Block data
     * @param moreBlocks More blocks expected
     * @return Result code
     */
    BlockResult receiveBlock(const std::string& transferId,
                             uint32_t blockNum,
                             const std::vector<uint8_t>& data,
                             bool moreBlocks);

    /**
     * @brief Acknowledge a sent block
     * @param transferId Transfer ID
     * @param blockNum Acknowledged block number
     * @return Result code
     */
    BlockResult acknowledgeBlock(const std::string& transferId,
                                 uint32_t blockNum);

    /**
     * @brief Request retransmission of a block
     * @param transferId Transfer ID
     * @param blockNum Block to retransmit
     * @return Result code
     */
    BlockResult requestRetransmit(const std::string& transferId,
                                  uint32_t blockNum);

    // ========================================================================
    // Block Option Encoding/Decoding
    // ========================================================================

    /**
     * @brief Encode Block1/Block2 option
     * @param blockNum Block number
     * @param more More blocks flag
     * @param szx Block size exponent
     * @return Encoded option value
     */
    static uint32_t encodeBlockOption(uint32_t blockNum, bool more, 
                                       BlockSize szx);

    /**
     * @brief Decode Block1/Block2 option
     * @param optionValue Encoded option
     * @param blockNum Output: block number
     * @param more Output: more blocks flag
     * @param szx Output: block size exponent
     */
    static void decodeBlockOption(uint32_t optionValue,
                                   uint32_t& blockNum,
                                   bool& more,
                                   BlockSize& szx);

    /**
     * @brief Get block size in bytes from SZX value
     * @param szx Block size exponent
     * @return Size in bytes
     */
    static uint32_t getBlockSizeBytes(BlockSize szx);

    /**
     * @brief Get SZX value from block size
     * @param size Block size in bytes
     * @return SZX value
     */
    static BlockSize getBlockSizeFromBytes(uint32_t size);

    // ========================================================================
    // Configuration
    // ========================================================================

    /**
     * @brief Set preferred block size
     * @param size Block size
     */
    void setPreferredBlockSize(BlockSize size);

    /**
     * @brief Get preferred block size
     * @return Block size
     */
    BlockSize getPreferredBlockSize() const { return config_.preferredBlockSize; }

    /**
     * @brief Enable/disable BERT
     * @param enabled Enable state
     */
    void enableBERT(bool enabled);

    /**
     * @brief Check if BERT is enabled
     * @return true if enabled
     */
    bool isBERTEnabled() const { return config_.bertEnabled; }

    /**
     * @brief Set BERT block size
     * @param size Block size (typically 2048-65536)
     */
    void setBERTBlockSize(uint32_t size);

    /**
     * @brief Set transfer timeout
     * @param timeout Timeout duration
     */
    void setTransferTimeout(std::chrono::seconds timeout);

    /**
     * @brief Set max retransmits
     * @param count Max retransmit count
     */
    void setMaxRetransmits(uint32_t count);

    // ========================================================================
    // Callbacks
    // ========================================================================

    /**
     * @brief Set progress callback
     * @param callback Callback function
     */
    void setProgressCallback(BlockProgressCallback callback);

    /**
     * @brief Set completion callback
     * @param callback Callback function
     */
    void setCompleteCallback(BlockCompleteCallback callback);

    // ========================================================================
    // Utility
    // ========================================================================

    /**
     * @brief Clean up completed/expired transfers
     */
    void cleanup();

    /**
     * @brief Get list of active transfers
     * @return Vector of transfer IDs
     */
    std::vector<std::string> getActiveTransfers() const;

private:
    // Internal methods
    std::string generateTransferId(const std::string& resource);
    std::vector<uint8_t> calculateChecksum(const std::vector<uint8_t>& data);
    void updateStats(TransferContext& ctx);
    bool isTransferExpired(const TransferContext& ctx) const;
    std::set<uint32_t> getMissingBlocks(const TransferContext& ctx) const;

    // Configuration
    Config config_;

    // Active transfers
    std::map<std::string, std::unique_ptr<TransferContext>> transfers_;

    // Callbacks
    BlockProgressCallback progressCallback_;
    BlockCompleteCallback completeCallback_;

    // Thread safety
    mutable std::mutex mutex_;
};

} // namespace transport
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_BLOCKWISE_TRANSFER_H
