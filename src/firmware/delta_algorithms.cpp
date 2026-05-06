/**
 * @file delta_algorithms.cpp
 * @brief Complete Delta Algorithm Implementations
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#include "firmware/delta_algorithms.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <map>
#include <unordered_map>

// Compression libraries
#include <zlib.h>

#ifdef HAS_LZMA
#include <lzma.h>
#endif

#ifdef HAS_BROTLI
#include <brotli/encode.h>
#include <brotli/decode.h>
#endif

#ifdef HAS_ZSTD
#include <zstd.h>
#endif

#ifdef HAS_BZIP2
#include <bzlib.h>
#endif

namespace lwm2m {
namespace firmware {

// ============================================================================
// BSDIFF Implementation
// ============================================================================

// BSDIFF magic header
constexpr uint8_t BSDIFF_MAGIC[] = {'B', 'S', 'D', 'I', 'F', 'F', '4', '0'};

struct BSDiffAlgorithm::Impl {
    // Minimum match length to consider a hash-table hit useful. Below this,
    // greedily emitting matches inflates the control block more than the
    // diff savings, so the matcher treats short hits as literal copies.
    static constexpr int64_t MIN_MATCH = 8;

    // Width of the rolling key used to seed the hash table.
    static constexpr size_t HASH_WIDTH = 4;

    // Cap on the number of candidate positions checked per hash bucket.
    // Random data with HASH_WIDTH=4 gives ~256-deep buckets on average for
    // 64KB sources, which is safe; larger caps can push createDelta past
    // the test timeout without measurably improving compression.
    static constexpr size_t MAX_BUCKET_CANDIDATES = 64;

    static uint32_t hash4(const uint8_t* p) {
        return (uint32_t(p[0]) << 24) |
               (uint32_t(p[1]) << 16) |
               (uint32_t(p[2]) << 8)  |
                uint32_t(p[3]);
    }

    // Match length helper used by both the hash matcher and back-extension.
    static int64_t matchLength(const uint8_t* source, int64_t sourceLen,
                               const uint8_t* target, int64_t targetLen) {
        int64_t i = 0;
        while (i < sourceLen && i < targetLen && source[i] == target[i]) {
            i++;
        }
        return i;
    }

    // Build a lookup table mapping HASH_WIDTH-byte keys to source positions.
    // Each bucket is capped at MAX_BUCKET_CANDIDATES entries; this trades a
    // small amount of compression ratio for bounded createDelta runtime.
    static void buildHashTable(
        const uint8_t* source, int64_t sourceLen,
        std::unordered_map<uint32_t, std::vector<int64_t>>& table) {
        if (sourceLen < (int64_t)HASH_WIDTH) {
            return;
        }
        for (int64_t i = 0; i + (int64_t)HASH_WIDTH <= sourceLen; i++) {
            uint32_t key = hash4(source + i);
            auto& bucket = table[key];
            if (bucket.size() < MAX_BUCKET_CANDIDATES) {
                bucket.push_back(i);
            }
        }
    }

    // Find the longest match for target[scan..] anywhere in source.
    // Returns the match length (>=0) and writes the source position to *pos.
    static int64_t findBestMatch(
        const uint8_t* source, int64_t sourceLen,
        const uint8_t* target, int64_t targetLen,
        int64_t scan,
        const std::unordered_map<uint32_t, std::vector<int64_t>>& table,
        int64_t* pos) {
        *pos = 0;
        if (scan + (int64_t)HASH_WIDTH > targetLen) {
            return 0;
        }
        uint32_t key = hash4(target + scan);
        auto it = table.find(key);
        if (it == table.end()) {
            return 0;
        }

        int64_t bestLen = 0;
        int64_t bestPos = 0;
        for (int64_t srcPos : it->second) {
            int64_t len = matchLength(
                source + srcPos, sourceLen - srcPos,
                target + scan, targetLen - scan);
            if (len > bestLen) {
                bestLen = len;
                bestPos = srcPos;
            }
        }

        *pos = bestPos;
        return bestLen;
    }

    // Write 64-bit value in BSDIFF format
    static void writeOffset(std::vector<uint8_t>& out, int64_t value) {
        uint64_t uval = (value < 0) ? (uint64_t)(-value) | 0x8000000000000000ULL : value;
        
        for (int i = 0; i < 8; i++) {
            out.push_back(uval & 0xFF);
            uval >>= 8;
        }
    }

    // Read 64-bit value from BSDIFF format
    static int64_t readOffset(const uint8_t* data) {
        uint64_t value = 0;
        for (int i = 7; i >= 0; i--) {
            value = (value << 8) | data[i];
        }
        
        if (value & 0x8000000000000000ULL) {
            return -(int64_t)(value & 0x7FFFFFFFFFFFFFFFULL);
        }
        return (int64_t)value;
    }
};

BSDiffAlgorithm::BSDiffAlgorithm() : impl_(std::make_unique<Impl>()) {}
BSDiffAlgorithm::~BSDiffAlgorithm() = default;

DeltaResult BSDiffAlgorithm::createDelta(const std::vector<uint8_t>& source,
                                          const std::vector<uint8_t>& target,
                                          std::vector<uint8_t>& delta,
                                          DeltaProgressCallback progress) {
    if (source.empty() || target.empty()) {
        return DeltaResult::ERROR_INVALID_INPUT;
    }

    const int64_t sourceLen = static_cast<int64_t>(source.size());
    const int64_t targetLen = static_cast<int64_t>(target.size());

    // Build a hash index over source. The previous suffix-array-based
    // matcher was O(n^2 log n) on random data and exceeded the 30s test
    // timeout for 64 KiB inputs; this hash index is O(n) to build and
    // bounds per-position lookup work via MAX_BUCKET_CANDIDATES.
    std::unordered_map<uint32_t, std::vector<int64_t>> hashTable;
    Impl::buildHashTable(source.data(), sourceLen, hashTable);

    // Delta components
    std::vector<uint8_t> diffBlock;
    std::vector<uint8_t> extraBlock;
    std::vector<uint8_t> controlBlock;

    // BSDIFF wire format: each control record encodes (addLen, copyLen,
    // seekLen). The decoder applies addLen bytes from diffBlock added to
    // source[oldPos..], then copies copyLen bytes verbatim from extraBlock,
    // then advances oldPos by seekLen.
    //
    // We emit records by walking the target with a greedy matcher: every
    // time we find a strong match in source, the run of unmatched bytes
    // since the previous match becomes the "extra" block of the previous
    // record, and the new match becomes the "add" portion of the next.
    int64_t targetPos = 0;            // next target byte to encode
    int64_t pendingAddLen = 0;        // length of the current add segment
    int64_t pendingAddSrcPos = 0;     // source offset for the add segment
    int64_t pendingExtraStart = 0;    // first target byte that is "extra"
    int64_t pendingExtraLen = 0;      // accumulated extra bytes after the
                                      // current match
    int64_t prevSrcEnd = 0;           // source position immediately after
                                      // the last match (used for seekLen)
    bool havePending = false;

    auto flushPending = [&](int64_t nextSrcPos) {
        // Emit a control tuple for the currently buffered add+extra pair.
        int64_t seekLen = nextSrcPos - prevSrcEnd;
        Impl::writeOffset(controlBlock, pendingAddLen);
        Impl::writeOffset(controlBlock, pendingExtraLen);
        Impl::writeOffset(controlBlock, seekLen);

        for (int64_t i = 0; i < pendingAddLen; i++) {
            uint8_t s = source[pendingAddSrcPos + i];
            uint8_t t = target[(targetPos - pendingAddLen - pendingExtraLen) + i];
            // Wire format stores t - s as an unsigned byte; underflow wraps
            // mod 256, which is reversed by the apply path adding s back.
            diffBlock.push_back(static_cast<uint8_t>(t - s));
        }

        for (int64_t i = 0; i < pendingExtraLen; i++) {
            extraBlock.push_back(target[pendingExtraStart + i]);
        }

        prevSrcEnd = pendingAddSrcPos + pendingAddLen;
    };

    while (targetPos < targetLen) {
        int64_t bestPos = 0;
        int64_t bestLen = Impl::findBestMatch(
            source.data(), sourceLen,
            target.data(), targetLen,
            targetPos, hashTable, &bestPos);

        if (bestLen >= Impl::MIN_MATCH) {
            if (havePending) {
                flushPending(bestPos);
            }
            // Start a new pending record anchored on this match.
            pendingAddSrcPos = bestPos;
            pendingAddLen = bestLen;
            pendingExtraStart = targetPos + bestLen;
            pendingExtraLen = 0;
            havePending = true;
            targetPos += bestLen;
        } else {
            // No strong match here: byte becomes part of the extra block
            // for the most recent record. If we have not produced any
            // record yet, start a synthetic record with addLen=0 anchored
            // at source offset 0.
            if (!havePending) {
                pendingAddSrcPos = 0;
                pendingAddLen = 0;
                pendingExtraStart = targetPos;
                pendingExtraLen = 0;
                havePending = true;
            }
            pendingExtraLen++;
            targetPos++;
        }

        if (progress && (targetPos & 0xFFF) == 0) {
            progress(targetPos, targetLen);
        }
    }

    if (havePending) {
        // Final flush; seekLen for the last record points back to the end
        // of its own match (no further advance needed).
        flushPending(pendingAddSrcPos + pendingAddLen);
    }

    // Compress blocks
    std::vector<uint8_t> compressedCtrl, compressedDiff, compressedExtra;
    
    DeltaCompression::compress(controlBlock, compressedCtrl, CompressionType::BZIP2);
    DeltaCompression::compress(diffBlock, compressedDiff, CompressionType::BZIP2);
    DeltaCompression::compress(extraBlock, compressedExtra, CompressionType::BZIP2);

    // Build final delta
    delta.clear();
    
    // Magic header
    delta.insert(delta.end(), BSDIFF_MAGIC, BSDIFF_MAGIC + 8);
    
    // Control block size
    Impl::writeOffset(delta, compressedCtrl.size());
    
    // Diff block size
    Impl::writeOffset(delta, compressedDiff.size());
    
    // Target size
    Impl::writeOffset(delta, targetLen);
    
    // Compressed blocks
    delta.insert(delta.end(), compressedCtrl.begin(), compressedCtrl.end());
    delta.insert(delta.end(), compressedDiff.begin(), compressedDiff.end());
    delta.insert(delta.end(), compressedExtra.begin(), compressedExtra.end());

    if (progress) {
        progress(targetLen, targetLen);
    }

    return DeltaResult::SUCCESS;
}

DeltaResult BSDiffAlgorithm::applyDelta(const std::vector<uint8_t>& source,
                                         const std::vector<uint8_t>& delta,
                                         std::vector<uint8_t>& target,
                                         DeltaProgressCallback progress) {
    if (delta.size() < 32) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    // Verify magic
    if (std::memcmp(delta.data(), BSDIFF_MAGIC, 8) != 0) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    // Read header
    int64_t ctrlLen = Impl::readOffset(delta.data() + 8);
    int64_t diffLen = Impl::readOffset(delta.data() + 16);
    int64_t targetLen = Impl::readOffset(delta.data() + 24);

    if (ctrlLen < 0 || diffLen < 0 || targetLen < 0) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    // Decompress blocks
    std::vector<uint8_t> ctrlBlock, diffBlock, extraBlock;

    const uint8_t* ctrlStart = delta.data() + 32;
    const uint8_t* diffStart = ctrlStart + ctrlLen;
    const uint8_t* extraStart = diffStart + diffLen;
    uint64_t extraLen = delta.size() - 32 - ctrlLen - diffLen;

    DeltaCompression::decompress(
        std::vector<uint8_t>(ctrlStart, ctrlStart + ctrlLen),
        ctrlBlock, CompressionType::BZIP2);
    
    DeltaCompression::decompress(
        std::vector<uint8_t>(diffStart, diffStart + diffLen),
        diffBlock, CompressionType::BZIP2);
    
    DeltaCompression::decompress(
        std::vector<uint8_t>(extraStart, extraStart + extraLen),
        extraBlock, CompressionType::BZIP2);

    // Apply delta
    target.resize(targetLen);
    
    int64_t oldPos = 0;
    int64_t newPos = 0;
    int64_t ctrlPos = 0;
    int64_t diffPos = 0;
    int64_t extraPos = 0;

    while (newPos < targetLen) {
        // Read control tuple
        int64_t addLen = Impl::readOffset(ctrlBlock.data() + ctrlPos);
        ctrlPos += 8;
        int64_t copyLen = Impl::readOffset(ctrlBlock.data() + ctrlPos);
        ctrlPos += 8;
        int64_t seekLen = Impl::readOffset(ctrlBlock.data() + ctrlPos);
        ctrlPos += 8;

        // Apply diff
        for (int64_t i = 0; i < addLen; i++) {
            target[newPos + i] = source[oldPos + i] + diffBlock[diffPos + i];
        }
        newPos += addLen;
        oldPos += addLen;
        diffPos += addLen;

        // Copy extra
        for (int64_t i = 0; i < copyLen; i++) {
            target[newPos + i] = extraBlock[extraPos + i];
        }
        newPos += copyLen;
        extraPos += copyLen;

        // Seek
        oldPos += seekLen;

        if (progress) {
            progress(newPos, targetLen);
        }
    }

    return DeltaResult::SUCCESS;
}

DeltaResult BSDiffAlgorithm::validateDelta(const std::vector<uint8_t>& delta,
                                            uint64_t expectedSourceSize) {
    if (delta.size() < 32) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    if (std::memcmp(delta.data(), BSDIFF_MAGIC, 8) != 0) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    return DeltaResult::SUCCESS;
}

// ============================================================================
// VCDIFF Implementation (RFC 3284)
// ============================================================================

constexpr uint8_t VCDIFF_MAGIC[] = {0xD6, 0xC3, 0xC4, 0x00};

// VCDIFF instruction types
enum VCDiffInstruction : uint8_t {
    VCD_NOOP = 0,
    VCD_ADD = 1,
    VCD_RUN = 2,
    VCD_COPY = 3
};

struct VCDiffAlgorithm::Impl {
    // Hash table for finding matches
    static constexpr size_t HASH_SIZE = 65536;
    
    static uint32_t hash(const uint8_t* data, size_t len) {
        uint32_t h = 0;
        for (size_t i = 0; i < len && i < 4; i++) {
            h = (h << 8) | data[i];
        }
        return h % HASH_SIZE;
    }

    // Variable-length integer encoding
    static void writeVarInt(std::vector<uint8_t>& out, uint64_t value) {
        std::vector<uint8_t> bytes;
        do {
            bytes.push_back(value & 0x7F);
            value >>= 7;
        } while (value > 0);
        
        for (size_t i = bytes.size(); i > 0; i--) {
            uint8_t b = bytes[i - 1];
            if (i > 1) b |= 0x80;
            out.push_back(b);
        }
    }

    static uint64_t readVarInt(const uint8_t*& data) {
        uint64_t value = 0;
        while (true) {
            uint8_t b = *data++;
            value = (value << 7) | (b & 0x7F);
            if (!(b & 0x80)) break;
        }
        return value;
    }
};

VCDiffAlgorithm::VCDiffAlgorithm() : impl_(std::make_unique<Impl>()) {}
VCDiffAlgorithm::~VCDiffAlgorithm() = default;

DeltaResult VCDiffAlgorithm::createDelta(const std::vector<uint8_t>& source,
                                          const std::vector<uint8_t>& target,
                                          std::vector<uint8_t>& delta,
                                          DeltaProgressCallback progress) {
    if (target.empty()) {
        return DeltaResult::ERROR_INVALID_INPUT;
    }

    delta.clear();
    
    // Write header
    delta.insert(delta.end(), VCDIFF_MAGIC, VCDIFF_MAGIC + 4);
    delta.push_back(0);  // Hdr_Indicator (no compression)

    // Build hash table for source
    std::vector<std::vector<size_t>> hashTable(Impl::HASH_SIZE);
    for (size_t i = 0; i + 4 <= source.size(); i++) {
        uint32_t h = Impl::hash(source.data() + i, 4);
        hashTable[h].push_back(i);
    }

    // Process target
    std::vector<uint8_t> dataSection;
    std::vector<uint8_t> instSection;
    std::vector<uint8_t> addrSection;

    size_t targetPos = 0;
    size_t lastCopyAddr = 0;

    while (targetPos < target.size()) {
        size_t bestLen = 0;
        size_t bestPos = 0;
        bool fromSource = false;

        // Search in source
        if (targetPos + 4 <= target.size()) {
            uint32_t h = Impl::hash(target.data() + targetPos, 4);
            
            for (size_t srcPos : hashTable[h]) {
                size_t len = 0;
                while (targetPos + len < target.size() &&
                       srcPos + len < source.size() &&
                       target[targetPos + len] == source[srcPos + len]) {
                    len++;
                }
                
                if (len > bestLen) {
                    bestLen = len;
                    bestPos = srcPos;
                    fromSource = true;
                }
            }
        }

        if (bestLen >= 4) {
            // COPY instruction
            instSection.push_back((VCD_COPY << 6) | std::min(bestLen, (size_t)63));
            if (bestLen >= 63) {
                Impl::writeVarInt(instSection, bestLen);
            }
            
            // Address encoding (self mode)
            Impl::writeVarInt(addrSection, bestPos);
            
            targetPos += bestLen;
            lastCopyAddr = bestPos + bestLen;
        } else {
            // ADD instruction - find run of non-matching bytes
            size_t addLen = 0;
            size_t addStart = targetPos;
            
            while (targetPos < target.size()) {
                // Check if we can start matching
                if (targetPos + 4 <= target.size()) {
                    uint32_t h = Impl::hash(target.data() + targetPos, 4);
                    bool foundMatch = false;
                    
                    for (size_t srcPos : hashTable[h]) {
                        if (srcPos < source.size() && 
                            target[targetPos] == source[srcPos]) {
                            foundMatch = true;
                            break;
                        }
                    }
                    
                    if (foundMatch) break;
                }
                
                targetPos++;
                addLen++;
            }
            
            if (addLen > 0) {
                instSection.push_back((VCD_ADD << 6) | std::min(addLen, (size_t)63));
                if (addLen >= 63) {
                    Impl::writeVarInt(instSection, addLen);
                }
                
                dataSection.insert(dataSection.end(),
                                   target.begin() + addStart,
                                   target.begin() + addStart + addLen);
            }
        }

        if (progress) {
            progress(targetPos, target.size());
        }
    }

    // Write window header
    delta.push_back(1);  // Win_Indicator: VCD_SOURCE
    Impl::writeVarInt(delta, source.size());
    Impl::writeVarInt(delta, 0);  // Source window position

    // Write section lengths
    uint64_t deltaLen = dataSection.size() + instSection.size() + addrSection.size();
    Impl::writeVarInt(delta, deltaLen);
    Impl::writeVarInt(delta, target.size());
    
    delta.push_back(0);  // Delta_Indicator (no compression)
    Impl::writeVarInt(delta, dataSection.size());
    Impl::writeVarInt(delta, instSection.size());
    Impl::writeVarInt(delta, addrSection.size());

    // Write sections
    delta.insert(delta.end(), dataSection.begin(), dataSection.end());
    delta.insert(delta.end(), instSection.begin(), instSection.end());
    delta.insert(delta.end(), addrSection.begin(), addrSection.end());

    return DeltaResult::SUCCESS;
}

DeltaResult VCDiffAlgorithm::applyDelta(const std::vector<uint8_t>& source,
                                         const std::vector<uint8_t>& delta,
                                         std::vector<uint8_t>& target,
                                         DeltaProgressCallback progress) {
    if (delta.size() < 5) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    // Verify magic
    if (std::memcmp(delta.data(), VCDIFF_MAGIC, 4) != 0) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    const uint8_t* ptr = delta.data() + 4;
    uint8_t hdrIndicator = *ptr++;

    // Read window header
    uint8_t winIndicator = *ptr++;
    uint64_t sourceLen = Impl::readVarInt(ptr);
    uint64_t sourcePos = Impl::readVarInt(ptr);

    // Read sizes
    uint64_t deltaLen = Impl::readVarInt(ptr);
    uint64_t targetLen = Impl::readVarInt(ptr);
    uint8_t deltaIndicator = *ptr++;
    uint64_t dataLen = Impl::readVarInt(ptr);
    uint64_t instLen = Impl::readVarInt(ptr);
    uint64_t addrLen = Impl::readVarInt(ptr);

    const uint8_t* dataPtr = ptr;
    const uint8_t* instPtr = ptr + dataLen;
    const uint8_t* addrPtr = ptr + dataLen + instLen;

    target.resize(targetLen);
    size_t targetPos = 0;
    size_t dataPos = 0;

    while (targetPos < targetLen && instPtr < delta.data() + delta.size()) {
        uint8_t inst = *instPtr++;
        uint8_t type = (inst >> 6) & 0x03;
        size_t size = inst & 0x3F;
        
        if (size == 0 || size >= 63) {
            size = Impl::readVarInt(instPtr);
        }

        switch (type) {
            case VCD_ADD:
                std::memcpy(target.data() + targetPos, dataPtr + dataPos, size);
                dataPos += size;
                targetPos += size;
                break;
                
            case VCD_COPY: {
                uint64_t addr = Impl::readVarInt(addrPtr);
                std::memcpy(target.data() + targetPos, source.data() + addr, size);
                targetPos += size;
                break;
            }
            
            case VCD_RUN:
                std::memset(target.data() + targetPos, dataPtr[dataPos], size);
                dataPos++;
                targetPos += size;
                break;
        }

        if (progress) {
            progress(targetPos, targetLen);
        }
    }

    return DeltaResult::SUCCESS;
}

DeltaResult VCDiffAlgorithm::validateDelta(const std::vector<uint8_t>& delta,
                                            uint64_t expectedSourceSize) {
    if (delta.size() < 5) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    if (std::memcmp(delta.data(), VCDIFF_MAGIC, 4) != 0) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    return DeltaResult::SUCCESS;
}

// ============================================================================
// Courgette Implementation
// ============================================================================

constexpr uint8_t COURGETTE_MAGIC[] = {'C', 'O', 'U', 'R', 'G', 'E', 'T', 'E'};

// ELF header structure
#pragma pack(push, 1)
struct ElfHeader64 {
    uint8_t magic[4];
    uint8_t class_;
    uint8_t endian;
    uint8_t version;
    uint8_t osabi;
    uint8_t abiversion;
    uint8_t pad[7];
    uint16_t type;
    uint16_t machine;
    uint32_t version2;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
};

struct ElfSection64 {
    uint32_t name;
    uint32_t type;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entsize;
};
#pragma pack(pop)

struct CourgetteAlgorithm::Impl {
    // Disassemble ARM64 code and normalize addresses
    static std::vector<uint8_t> disassembleARM64(const std::vector<uint8_t>& code,
                                                  uint64_t baseAddr,
                                                  std::vector<uint64_t>& addresses) {
        std::vector<uint8_t> normalized = code;
        addresses.clear();

        for (size_t i = 0; i + 4 <= code.size(); i += 4) {
            uint32_t inst = code[i] | (code[i+1] << 8) | 
                           (code[i+2] << 16) | (code[i+3] << 24);

            // Check for branch instructions (B, BL)
            if ((inst & 0xFC000000) == 0x14000000 ||  // B
                (inst & 0xFC000000) == 0x94000000) {  // BL
                
                int32_t offset = (inst & 0x03FFFFFF) << 2;
                if (offset & 0x08000000) {
                    offset |= 0xF0000000;  // Sign extend
                }
                
                uint64_t targetAddr = baseAddr + i + offset;
                addresses.push_back(targetAddr);
                
                // Normalize to relative offset 0
                uint32_t normalizedInst = inst & 0xFC000000;
                normalized[i] = normalizedInst & 0xFF;
                normalized[i+1] = (normalizedInst >> 8) & 0xFF;
                normalized[i+2] = (normalizedInst >> 16) & 0xFF;
                normalized[i+3] = (normalizedInst >> 24) & 0xFF;
            }
            
            // ADR/ADRP instructions
            else if ((inst & 0x9F000000) == 0x10000000 ||  // ADR
                     (inst & 0x9F000000) == 0x90000000) {  // ADRP
                
                uint32_t immlo = (inst >> 29) & 0x3;
                uint32_t immhi = (inst >> 5) & 0x7FFFF;
                int64_t offset = (immhi << 2) | immlo;
                
                if (inst & 0x80000000) {  // ADRP
                    offset <<= 12;
                }
                
                uint64_t targetAddr = baseAddr + (i & ~0xFFF) + offset;
                addresses.push_back(targetAddr);
                
                // Normalize
                uint32_t normalizedInst = inst & 0x9F00001F;
                normalized[i] = normalizedInst & 0xFF;
                normalized[i+1] = (normalizedInst >> 8) & 0xFF;
                normalized[i+2] = (normalizedInst >> 16) & 0xFF;
                normalized[i+3] = (normalizedInst >> 24) & 0xFF;
            }
        }

        return normalized;
    }

    // Reassemble ARM64 code with new addresses
    static std::vector<uint8_t> reassembleARM64(const std::vector<uint8_t>& normalized,
                                                 uint64_t baseAddr,
                                                 const std::vector<uint64_t>& addresses) {
        std::vector<uint8_t> code = normalized;
        size_t addrIdx = 0;

        for (size_t i = 0; i + 4 <= code.size(); i += 4) {
            uint32_t inst = code[i] | (code[i+1] << 8) |
                           (code[i+2] << 16) | (code[i+3] << 24);

            // Branch instructions
            if ((inst & 0xFC000000) == 0x14000000 ||
                (inst & 0xFC000000) == 0x94000000) {
                
                if (addrIdx < addresses.size()) {
                    int64_t offset = addresses[addrIdx++] - (baseAddr + i);
                    offset >>= 2;
                    
                    inst |= (offset & 0x03FFFFFF);
                    code[i] = inst & 0xFF;
                    code[i+1] = (inst >> 8) & 0xFF;
                    code[i+2] = (inst >> 16) & 0xFF;
                    code[i+3] = (inst >> 24) & 0xFF;
                }
            }
            // ADR/ADRP
            else if ((inst & 0x9F000000) == 0x10000000 ||
                     (inst & 0x9F000000) == 0x90000000) {
                
                if (addrIdx < addresses.size()) {
                    int64_t offset = addresses[addrIdx++] - (baseAddr + (i & ~0xFFF));
                    
                    if (inst & 0x80000000) {  // ADRP
                        offset >>= 12;
                    }
                    
                    uint32_t immlo = offset & 0x3;
                    uint32_t immhi = (offset >> 2) & 0x7FFFF;
                    
                    inst |= (immlo << 29) | (immhi << 5);
                    code[i] = inst & 0xFF;
                    code[i+1] = (inst >> 8) & 0xFF;
                    code[i+2] = (inst >> 16) & 0xFF;
                    code[i+3] = (inst >> 24) & 0xFF;
                }
            }
        }

        return code;
    }

    // Extract code sections from ELF
    static bool extractCodeSections(const std::vector<uint8_t>& elf,
                                    std::vector<std::pair<uint64_t, std::vector<uint8_t>>>& sections) {
        if (elf.size() < sizeof(ElfHeader64)) {
            return false;
        }

        const ElfHeader64* header = reinterpret_cast<const ElfHeader64*>(elf.data());
        
        if (header->magic[0] != 0x7F || header->magic[1] != 'E' ||
            header->magic[2] != 'L' || header->magic[3] != 'F') {
            return false;
        }

        // Read section headers
        for (uint16_t i = 0; i < header->shnum; i++) {
            const ElfSection64* section = reinterpret_cast<const ElfSection64*>(
                elf.data() + header->shoff + i * header->shentsize);

            // Check for executable sections (SHF_EXECINSTR = 0x4)
            if (section->flags & 0x4) {
                std::vector<uint8_t> data(
                    elf.begin() + section->offset,
                    elf.begin() + section->offset + section->size);
                
                sections.push_back({section->addr, std::move(data)});
            }
        }

        return true;
    }
};

CourgetteAlgorithm::CourgetteAlgorithm() : impl_(std::make_unique<Impl>()) {}
CourgetteAlgorithm::~CourgetteAlgorithm() = default;

CourgetteAlgorithm::ExecutableFormat CourgetteAlgorithm::detectFormat(
    const std::vector<uint8_t>& data) {
    
    if (data.size() < 4) {
        return ExecutableFormat::UNKNOWN;
    }

    // ELF
    if (data[0] == 0x7F && data[1] == 'E' && data[2] == 'L' && data[3] == 'F') {
        if (data.size() >= 5) {
            return (data[4] == 2) ? ExecutableFormat::ELF64 : ExecutableFormat::ELF32;
        }
    }

    // PE (MZ header)
    if (data[0] == 'M' && data[1] == 'Z') {
        if (data.size() >= 64) {
            uint32_t peOffset = *reinterpret_cast<const uint32_t*>(data.data() + 60);
            if (peOffset + 6 < data.size() && 
                data[peOffset] == 'P' && data[peOffset+1] == 'E') {
                uint16_t machine = *reinterpret_cast<const uint16_t*>(
                    data.data() + peOffset + 4);
                return (machine == 0x8664) ? ExecutableFormat::PE64 : ExecutableFormat::PE32;
            }
        }
    }

    // Mach-O
    if (data.size() >= 4) {
        uint32_t magic = *reinterpret_cast<const uint32_t*>(data.data());
        if (magic == 0xFEEDFACE) return ExecutableFormat::MACHO32;
        if (magic == 0xFEEDFACF) return ExecutableFormat::MACHO64;
    }

    return ExecutableFormat::UNKNOWN;
}

bool CourgetteAlgorithm::isExecutable(const std::vector<uint8_t>& data) {
    return detectFormat(data) != ExecutableFormat::UNKNOWN;
}

DeltaResult CourgetteAlgorithm::createDelta(const std::vector<uint8_t>& source,
                                             const std::vector<uint8_t>& target,
                                             std::vector<uint8_t>& delta,
                                             DeltaProgressCallback progress) {
    // Detect format
    auto format = detectFormat(source);
    if (format == ExecutableFormat::UNKNOWN) {
        // Fall back to BSDIFF for non-executables
        BSDiffAlgorithm bsdiff;
        return bsdiff.createDelta(source, target, delta, progress);
    }

    if (progress) progress(0, 100);

    // Extract and normalize code sections
    std::vector<std::pair<uint64_t, std::vector<uint8_t>>> sourceSections;
    std::vector<std::pair<uint64_t, std::vector<uint8_t>>> targetSections;

    Impl::extractCodeSections(source, sourceSections);
    Impl::extractCodeSections(target, targetSections);

    if (progress) progress(20, 100);

    // Disassemble and normalize addresses
    std::vector<uint64_t> sourceAddrs, targetAddrs;
    std::vector<uint8_t> normalizedSource, normalizedTarget;

    for (const auto& [addr, code] : sourceSections) {
        auto norm = Impl::disassembleARM64(code, addr, sourceAddrs);
        normalizedSource.insert(normalizedSource.end(), norm.begin(), norm.end());
    }

    for (const auto& [addr, code] : targetSections) {
        auto norm = Impl::disassembleARM64(code, addr, targetAddrs);
        normalizedTarget.insert(normalizedTarget.end(), norm.begin(), norm.end());
    }

    if (progress) progress(40, 100);

    // Create delta of normalized code using BSDIFF
    std::vector<uint8_t> codeDelta;
    BSDiffAlgorithm bsdiff;
    auto result = bsdiff.createDelta(normalizedSource, normalizedTarget, codeDelta);
    
    if (result != DeltaResult::SUCCESS) {
        return result;
    }

    if (progress) progress(70, 100);

    // Encode address tables
    std::vector<uint8_t> addrDelta;
    // Simple encoding: store target addresses
    for (uint64_t addr : targetAddrs) {
        for (int i = 0; i < 8; i++) {
            addrDelta.push_back((addr >> (i * 8)) & 0xFF);
        }
    }

    // Compress address delta
    std::vector<uint8_t> compressedAddrs;
    DeltaCompression::compress(addrDelta, compressedAddrs, CompressionType::ZLIB);

    if (progress) progress(85, 100);

    // Build final delta
    delta.clear();
    
    // Magic
    delta.insert(delta.end(), COURGETTE_MAGIC, COURGETTE_MAGIC + 8);
    
    // Format
    delta.push_back(static_cast<uint8_t>(format));
    
    // Sizes
    uint64_t codeSize = codeDelta.size();
    uint64_t addrSize = compressedAddrs.size();
    
    for (int i = 0; i < 8; i++) delta.push_back((codeSize >> (i * 8)) & 0xFF);
    for (int i = 0; i < 8; i++) delta.push_back((addrSize >> (i * 8)) & 0xFF);
    
    // Data
    delta.insert(delta.end(), codeDelta.begin(), codeDelta.end());
    delta.insert(delta.end(), compressedAddrs.begin(), compressedAddrs.end());

    if (progress) progress(100, 100);

    return DeltaResult::SUCCESS;
}

DeltaResult CourgetteAlgorithm::applyDelta(const std::vector<uint8_t>& source,
                                            const std::vector<uint8_t>& delta,
                                            std::vector<uint8_t>& target,
                                            DeltaProgressCallback progress) {
    if (delta.size() < 25) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    // Verify magic
    if (std::memcmp(delta.data(), COURGETTE_MAGIC, 8) != 0) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    if (progress) progress(0, 100);

    // Read header
    ExecutableFormat format = static_cast<ExecutableFormat>(delta[8]);
    
    uint64_t codeSize = 0, addrSize = 0;
    for (int i = 0; i < 8; i++) codeSize |= (uint64_t)delta[9 + i] << (i * 8);
    for (int i = 0; i < 8; i++) addrSize |= (uint64_t)delta[17 + i] << (i * 8);

    const uint8_t* codeStart = delta.data() + 25;
    const uint8_t* addrStart = codeStart + codeSize;

    // Normalize source
    std::vector<std::pair<uint64_t, std::vector<uint8_t>>> sourceSections;
    Impl::extractCodeSections(source, sourceSections);

    std::vector<uint64_t> sourceAddrs;
    std::vector<uint8_t> normalizedSource;

    for (const auto& [addr, code] : sourceSections) {
        auto norm = Impl::disassembleARM64(code, addr, sourceAddrs);
        normalizedSource.insert(normalizedSource.end(), norm.begin(), norm.end());
    }

    if (progress) progress(30, 100);

    // Apply code delta
    std::vector<uint8_t> normalizedTarget;
    std::vector<uint8_t> codeDelta(codeStart, codeStart + codeSize);
    
    BSDiffAlgorithm bsdiff;
    auto result = bsdiff.applyDelta(normalizedSource, codeDelta, normalizedTarget);
    
    if (result != DeltaResult::SUCCESS) {
        return result;
    }

    if (progress) progress(60, 100);

    // Decompress addresses
    std::vector<uint8_t> compressedAddrs(addrStart, addrStart + addrSize);
    std::vector<uint8_t> addrData;
    DeltaCompression::decompress(compressedAddrs, addrData, CompressionType::ZLIB);

    // Parse addresses
    std::vector<uint64_t> targetAddrs;
    for (size_t i = 0; i + 8 <= addrData.size(); i += 8) {
        uint64_t addr = 0;
        for (int j = 0; j < 8; j++) {
            addr |= (uint64_t)addrData[i + j] << (j * 8);
        }
        targetAddrs.push_back(addr);
    }

    if (progress) progress(80, 100);

    // Reassemble with target addresses
    // For simplicity, assume single code section at 0x400000
    target = Impl::reassembleARM64(normalizedTarget, 0x400000, targetAddrs);

    if (progress) progress(100, 100);

    return DeltaResult::SUCCESS;
}

DeltaResult CourgetteAlgorithm::validateDelta(const std::vector<uint8_t>& delta,
                                               uint64_t expectedSourceSize) {
    if (delta.size() < 25) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    if (std::memcmp(delta.data(), COURGETTE_MAGIC, 8) != 0) {
        return DeltaResult::ERROR_INVALID_DELTA;
    }

    return DeltaResult::SUCCESS;
}

// ============================================================================
// Factory and Utilities
// ============================================================================

std::unique_ptr<IDeltaAlgorithm> DeltaAlgorithmFactory::create(DeltaAlgorithm algorithm) {
    switch (algorithm) {
        case DeltaAlgorithm::BSDIFF:
            return std::make_unique<BSDiffAlgorithm>();
        case DeltaAlgorithm::VCDIFF:
            return std::make_unique<VCDiffAlgorithm>();
        case DeltaAlgorithm::COURGETTE:
            return std::make_unique<CourgetteAlgorithm>();
        default:
            return nullptr;
    }
}

DeltaAlgorithm DeltaAlgorithmFactory::recommendAlgorithm(
    const std::vector<uint8_t>& source,
    const std::vector<uint8_t>& target) {
    
    // Use Courgette for executables
    if (CourgetteAlgorithm::isExecutable(source) && 
        CourgetteAlgorithm::isExecutable(target)) {
        return DeltaAlgorithm::COURGETTE;
    }
    
    // Default to BSDIFF
    return DeltaAlgorithm::BSDIFF;
}

std::vector<DeltaAlgorithm> DeltaAlgorithmFactory::getAvailableAlgorithms() {
    return {DeltaAlgorithm::BSDIFF, DeltaAlgorithm::VCDIFF, DeltaAlgorithm::COURGETTE};
}

// ============================================================================
// Compression
// ============================================================================

DeltaResult DeltaCompression::compress(const std::vector<uint8_t>& data,
                                        std::vector<uint8_t>& compressed,
                                        CompressionType type,
                                        int level) {
    switch (type) {
        case CompressionType::NONE:
            compressed = data;
            return DeltaResult::SUCCESS;

        case CompressionType::ZLIB: {
            uLongf destLen = compressBound(data.size());
            compressed.resize(destLen);
            
            int result = compress2(compressed.data(), &destLen,
                                   data.data(), data.size(), level);
            
            if (result != Z_OK) {
                return DeltaResult::ERROR_COMPRESSION_FAILED;
            }
            
            compressed.resize(destLen);
            return DeltaResult::SUCCESS;
        }

#ifdef HAS_BZIP2
        case CompressionType::BZIP2: {
            unsigned int destLen = data.size() + data.size() / 100 + 600;
            compressed.resize(destLen);
            
            int result = BZ2_bzBuffToBuffCompress(
                reinterpret_cast<char*>(compressed.data()), &destLen,
                const_cast<char*>(reinterpret_cast<const char*>(data.data())),
                data.size(), level, 0, 30);
            
            if (result != BZ_OK) {
                return DeltaResult::ERROR_COMPRESSION_FAILED;
            }
            
            compressed.resize(destLen);
            return DeltaResult::SUCCESS;
        }
#endif

#ifdef HAS_ZSTD
        case CompressionType::ZSTD: {
            size_t destLen = ZSTD_compressBound(data.size());
            compressed.resize(destLen);
            
            size_t result = ZSTD_compress(compressed.data(), destLen,
                                          data.data(), data.size(), level);
            
            if (ZSTD_isError(result)) {
                return DeltaResult::ERROR_COMPRESSION_FAILED;
            }
            
            compressed.resize(result);
            return DeltaResult::SUCCESS;
        }
#endif

        default:
            return DeltaResult::ERROR_UNSUPPORTED_ALGORITHM;
    }
}

DeltaResult DeltaCompression::decompress(const std::vector<uint8_t>& compressed,
                                          std::vector<uint8_t>& data,
                                          CompressionType type,
                                          uint64_t expectedSize) {
    switch (type) {
        case CompressionType::NONE:
            data = compressed;
            return DeltaResult::SUCCESS;

        case CompressionType::ZLIB: {
            if (expectedSize == 0) {
                expectedSize = compressed.size() * 10;  // Estimate
            }
            
            data.resize(expectedSize);
            uLongf destLen = expectedSize;
            
            int result = uncompress(data.data(), &destLen,
                                    compressed.data(), compressed.size());
            
            while (result == Z_BUF_ERROR) {
                expectedSize *= 2;
                data.resize(expectedSize);
                destLen = expectedSize;
                result = uncompress(data.data(), &destLen,
                                    compressed.data(), compressed.size());
            }
            
            if (result != Z_OK) {
                return DeltaResult::ERROR_DECOMPRESSION_FAILED;
            }
            
            data.resize(destLen);
            return DeltaResult::SUCCESS;
        }

#ifdef HAS_BZIP2
        case CompressionType::BZIP2: {
            if (expectedSize == 0) {
                expectedSize = compressed.size() * 10;
            }
            
            data.resize(expectedSize);
            unsigned int destLen = expectedSize;
            
            int result = BZ2_bzBuffToBuffDecompress(
                reinterpret_cast<char*>(data.data()), &destLen,
                const_cast<char*>(reinterpret_cast<const char*>(compressed.data())),
                compressed.size(), 0, 0);
            
            if (result != BZ_OK) {
                return DeltaResult::ERROR_DECOMPRESSION_FAILED;
            }
            
            data.resize(destLen);
            return DeltaResult::SUCCESS;
        }
#endif

#ifdef HAS_ZSTD
        case CompressionType::ZSTD: {
            uint64_t destLen = ZSTD_getFrameContentSize(compressed.data(), 
                                                         compressed.size());
            
            if (destLen == ZSTD_CONTENTSIZE_ERROR ||
                destLen == ZSTD_CONTENTSIZE_UNKNOWN) {
                destLen = expectedSize > 0 ? expectedSize : compressed.size() * 10;
            }
            
            data.resize(destLen);
            
            size_t result = ZSTD_decompress(data.data(), destLen,
                                            compressed.data(), compressed.size());
            
            if (ZSTD_isError(result)) {
                return DeltaResult::ERROR_DECOMPRESSION_FAILED;
            }
            
            data.resize(result);
            return DeltaResult::SUCCESS;
        }
#endif

        default:
            return DeltaResult::ERROR_UNSUPPORTED_ALGORITHM;
    }
}

uint64_t DeltaCompression::estimateCompressedSize(const std::vector<uint8_t>& data,
                                                   CompressionType type) {
    switch (type) {
        case CompressionType::NONE:
            return data.size();
        case CompressionType::ZLIB:
            return compressBound(data.size());
#ifdef HAS_ZSTD
        case CompressionType::ZSTD:
            return ZSTD_compressBound(data.size());
#endif
        default:
            return data.size();
    }
}

bool DeltaCompression::isAvailable(CompressionType type) {
    switch (type) {
        case CompressionType::NONE:
        case CompressionType::ZLIB:
            return true;
#ifdef HAS_BZIP2
        case CompressionType::BZIP2:
            return true;
#endif
#ifdef HAS_ZSTD
        case CompressionType::ZSTD:
            return true;
#endif
#ifdef HAS_LZMA
        case CompressionType::LZMA:
            return true;
#endif
#ifdef HAS_BROTLI
        case CompressionType::BROTLI:
            return true;
#endif
        default:
            return false;
    }
}

} // namespace firmware
} // namespace lwm2m
