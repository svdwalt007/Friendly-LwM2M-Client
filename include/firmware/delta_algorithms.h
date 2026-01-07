/**
 * @file delta_algorithms.h
 * @brief Delta Firmware Update Algorithms for Friendly LwM2M Client
 * 
 * Complete implementations of delta/differential update algorithms:
 * - BSDIFF: Binary diff algorithm for general firmware
 * - VCDIFF: Generic diff format (RFC 3284)
 * - Courgette: Optimized for executable files
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_DELTA_ALGORITHMS_H
#define FRIENDLY_LWM2M_DELTA_ALGORITHMS_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace lwm2m {
namespace firmware {

/**
 * @brief Delta algorithm identifiers
 */
enum class DeltaAlgorithm : uint8_t {
    BSDIFF = 1,
    VCDIFF = 2,
    COURGETTE = 3
};

/**
 * @brief Compression algorithms for delta packages
 */
enum class CompressionType : uint8_t {
    NONE = 0,
    ZLIB = 1,
    LZMA = 2,
    BROTLI = 3,
    ZSTD = 4,
    BZIP2 = 5
};

/**
 * @brief Result codes for delta operations
 */
enum class DeltaResult {
    SUCCESS = 0,
    ERROR_INVALID_INPUT,
    ERROR_INVALID_DELTA,
    ERROR_SIZE_MISMATCH,
    ERROR_CHECKSUM_MISMATCH,
    ERROR_DECOMPRESSION_FAILED,
    ERROR_COMPRESSION_FAILED,
    ERROR_OUT_OF_MEMORY,
    ERROR_IO_FAILURE,
    ERROR_UNSUPPORTED_ALGORITHM,
    ERROR_UNKNOWN
};

/**
 * @brief Delta package header structure
 */
struct DeltaHeader {
    uint32_t magic;                 ///< Magic number for identification
    uint8_t version;                ///< Delta format version
    DeltaAlgorithm algorithm;       ///< Delta algorithm used
    CompressionType compression;    ///< Compression type
    uint64_t sourceSize;            ///< Expected source size
    uint64_t targetSize;            ///< Expected target size
    uint64_t deltaSize;             ///< Compressed delta size
    std::vector<uint8_t> sourceChecksum;  ///< SHA256 of source
    std::vector<uint8_t> targetChecksum;  ///< SHA256 of target
};

/**
 * @brief Progress callback for delta operations
 */
using DeltaProgressCallback = std::function<void(uint64_t current, uint64_t total)>;

/**
 * @brief Abstract base class for delta algorithms
 */
class IDeltaAlgorithm {
public:
    virtual ~IDeltaAlgorithm() = default;

    /**
     * @brief Get algorithm identifier
     */
    virtual DeltaAlgorithm getAlgorithm() const = 0;

    /**
     * @brief Get algorithm name
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Create delta between two data sets
     * @param source Original data
     * @param target New data
     * @param delta Output delta data
     * @param progress Progress callback
     * @return Result code
     */
    virtual DeltaResult createDelta(const std::vector<uint8_t>& source,
                                     const std::vector<uint8_t>& target,
                                     std::vector<uint8_t>& delta,
                                     DeltaProgressCallback progress = nullptr) = 0;

    /**
     * @brief Apply delta to source data
     * @param source Original data
     * @param delta Delta data
     * @param target Output patched data
     * @param progress Progress callback
     * @return Result code
     */
    virtual DeltaResult applyDelta(const std::vector<uint8_t>& source,
                                    const std::vector<uint8_t>& delta,
                                    std::vector<uint8_t>& target,
                                    DeltaProgressCallback progress = nullptr) = 0;

    /**
     * @brief Validate delta package
     * @param delta Delta data
     * @param expectedSourceSize Expected source size
     * @return Result code
     */
    virtual DeltaResult validateDelta(const std::vector<uint8_t>& delta,
                                       uint64_t expectedSourceSize) = 0;
};

// ============================================================================
// BSDIFF Algorithm
// ============================================================================

/**
 * @brief BSDIFF delta algorithm implementation
 * 
 * Binary diff algorithm that produces compact patches for binary files.
 * Particularly effective for executables and firmware images.
 */
class BSDiffAlgorithm : public IDeltaAlgorithm {
public:
    BSDiffAlgorithm();
    ~BSDiffAlgorithm() override;

    DeltaAlgorithm getAlgorithm() const override { return DeltaAlgorithm::BSDIFF; }
    std::string getName() const override { return "BSDIFF"; }

    DeltaResult createDelta(const std::vector<uint8_t>& source,
                            const std::vector<uint8_t>& target,
                            std::vector<uint8_t>& delta,
                            DeltaProgressCallback progress = nullptr) override;

    DeltaResult applyDelta(const std::vector<uint8_t>& source,
                           const std::vector<uint8_t>& delta,
                           std::vector<uint8_t>& target,
                           DeltaProgressCallback progress = nullptr) override;

    DeltaResult validateDelta(const std::vector<uint8_t>& delta,
                              uint64_t expectedSourceSize) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================================
// VCDIFF Algorithm
// ============================================================================

/**
 * @brief VCDIFF delta algorithm implementation (RFC 3284)
 * 
 * Generic differencing and compression format suitable for any binary data.
 */
class VCDiffAlgorithm : public IDeltaAlgorithm {
public:
    VCDiffAlgorithm();
    ~VCDiffAlgorithm() override;

    DeltaAlgorithm getAlgorithm() const override { return DeltaAlgorithm::VCDIFF; }
    std::string getName() const override { return "VCDIFF"; }

    DeltaResult createDelta(const std::vector<uint8_t>& source,
                            const std::vector<uint8_t>& target,
                            std::vector<uint8_t>& delta,
                            DeltaProgressCallback progress = nullptr) override;

    DeltaResult applyDelta(const std::vector<uint8_t>& source,
                           const std::vector<uint8_t>& delta,
                           std::vector<uint8_t>& target,
                           DeltaProgressCallback progress = nullptr) override;

    DeltaResult validateDelta(const std::vector<uint8_t>& delta,
                              uint64_t expectedSourceSize) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================================
// Courgette Algorithm
// ============================================================================

/**
 * @brief Courgette delta algorithm implementation
 * 
 * Specialized algorithm for executable files that:
 * - Disassembles executables to normalize addresses
 * - Creates more compact deltas for code changes
 * - Supports ELF (Linux), PE (Windows), and Mach-O (macOS)
 * 
 * Particularly effective for:
 * - ARM64 executables (OpenWRT, embedded Linux)
 * - x86/x86_64 executables
 * - Shared libraries
 */
class CourgetteAlgorithm : public IDeltaAlgorithm {
public:
    /**
     * @brief Executable format types
     */
    enum class ExecutableFormat {
        UNKNOWN,
        ELF32,
        ELF64,
        PE32,
        PE64,
        MACHO32,
        MACHO64
    };

    CourgetteAlgorithm();
    ~CourgetteAlgorithm() override;

    DeltaAlgorithm getAlgorithm() const override { return DeltaAlgorithm::COURGETTE; }
    std::string getName() const override { return "Courgette"; }

    DeltaResult createDelta(const std::vector<uint8_t>& source,
                            const std::vector<uint8_t>& target,
                            std::vector<uint8_t>& delta,
                            DeltaProgressCallback progress = nullptr) override;

    DeltaResult applyDelta(const std::vector<uint8_t>& source,
                           const std::vector<uint8_t>& delta,
                           std::vector<uint8_t>& target,
                           DeltaProgressCallback progress = nullptr) override;

    DeltaResult validateDelta(const std::vector<uint8_t>& delta,
                              uint64_t expectedSourceSize) override;

    /**
     * @brief Detect executable format
     * @param data Binary data
     * @return Detected format
     */
    static ExecutableFormat detectFormat(const std::vector<uint8_t>& data);

    /**
     * @brief Check if data is a supported executable
     * @param data Binary data
     * @return true if executable format is supported
     */
    static bool isExecutable(const std::vector<uint8_t>& data);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================================
// Delta Algorithm Factory
// ============================================================================

/**
 * @brief Factory for creating delta algorithm instances
 */
class DeltaAlgorithmFactory {
public:
    /**
     * @brief Create algorithm instance by type
     * @param algorithm Algorithm type
     * @return Algorithm instance
     */
    static std::unique_ptr<IDeltaAlgorithm> create(DeltaAlgorithm algorithm);

    /**
     * @brief Auto-detect best algorithm for given data
     * @param source Source data
     * @param target Target data
     * @return Recommended algorithm
     */
    static DeltaAlgorithm recommendAlgorithm(const std::vector<uint8_t>& source,
                                              const std::vector<uint8_t>& target);

    /**
     * @brief Get list of available algorithms
     * @return Vector of available algorithm types
     */
    static std::vector<DeltaAlgorithm> getAvailableAlgorithms();
};

// ============================================================================
// Compression Utilities
// ============================================================================

/**
 * @brief Compression utility class
 */
class DeltaCompression {
public:
    /**
     * @brief Compress data
     * @param data Input data
     * @param compressed Output compressed data
     * @param type Compression type
     * @param level Compression level (0-9)
     * @return Result code
     */
    static DeltaResult compress(const std::vector<uint8_t>& data,
                                 std::vector<uint8_t>& compressed,
                                 CompressionType type,
                                 int level = 6);

    /**
     * @brief Decompress data
     * @param compressed Input compressed data
     * @param data Output decompressed data
     * @param type Compression type
     * @param expectedSize Expected output size (0 = unknown)
     * @return Result code
     */
    static DeltaResult decompress(const std::vector<uint8_t>& compressed,
                                   std::vector<uint8_t>& data,
                                   CompressionType type,
                                   uint64_t expectedSize = 0);

    /**
     * @brief Estimate compressed size
     * @param data Input data
     * @param type Compression type
     * @return Estimated compressed size
     */
    static uint64_t estimateCompressedSize(const std::vector<uint8_t>& data,
                                            CompressionType type);

    /**
     * @brief Check if compression type is available
     * @param type Compression type
     * @return true if available
     */
    static bool isAvailable(CompressionType type);
};

} // namespace firmware
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_DELTA_ALGORITHMS_H
