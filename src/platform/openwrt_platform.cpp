/**
 * @file openwrt_platform.cpp
 * @brief OpenWRT Platform Implementation for Friendly LwM2M Client
 * 
 * Provides concrete implementations of platform interfaces for OpenWRT,
 * including MTD flash operations, U-Boot environment control, and
 * A/B partition management.
 * 
 * Tested on: OpenWRT v24.10.4
 * Supported boards: OpenWRT One Router, generic x86_64, ARM64
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#include "platform/platform_abstraction.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cstring>
#include <array>
#include <chrono>
#include <thread>

// Linux headers
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <sys/sysinfo.h>
#include <sys/mount.h>
#include <linux/reboot.h>
#include <mtd/mtd-user.h>
#include <openssl/sha.h>

namespace lwm2m {
namespace platform {
namespace openwrt {

// ============================================================================
// Constants and Configuration
// ============================================================================

constexpr const char* UBOOT_ENV_DEVICE = "/dev/mtd1";
constexpr const char* UBOOT_ENV_CONFIG = "/etc/fw_env.config";
constexpr const char* PROC_MTD = "/proc/mtd";
constexpr const char* PROC_CMDLINE = "/proc/cmdline";
constexpr const char* VERSION_FILE = "/etc/openwrt_release";
constexpr const char* BOARD_JSON = "/etc/board.json";
constexpr int MAX_BOOT_ATTEMPTS = 3;
constexpr size_t SHA256_LENGTH = 32;

// U-Boot environment variable names for A/B boot
constexpr const char* UBOOT_ACTIVE_SLOT = "active_slot";
constexpr const char* UBOOT_BOOT_SLOT = "boot_slot";
constexpr const char* UBOOT_BOOT_ATTEMPTS_A = "boot_attempts_a";
constexpr const char* UBOOT_BOOT_ATTEMPTS_B = "boot_attempts_b";
constexpr const char* UBOOT_BOOTABLE_A = "slot_a_bootable";
constexpr const char* UBOOT_BOOTABLE_B = "slot_b_bootable";
constexpr const char* UBOOT_UPGRADE_AVAILABLE = "upgrade_available";

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Validate command string for safety
 * Only allows alphanumeric characters, spaces, and safe punctuation
 */
static bool isCommandSafe(const std::string& cmd) {
    // Allow only safe characters: alphanumeric, space, dash, underscore, dot, forward slash
    static const std::regex safePattern("^[a-zA-Z0-9 ._/-]+$");
    return std::regex_match(cmd, safePattern);
}

/**
 * @brief Execute command and capture output
 * @warning Only accepts validated safe commands to prevent injection
 */
static int execCommand(const std::string& cmd, std::string& output) {
    // Validate command before execution
    if (!isCommandSafe(cmd)) {
        output = "Error: Command contains unsafe characters";
        return -1;
    }

    std::array<char, 4096> buffer;
    output.clear();

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        return -1;
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        buffer[buffer.size() - 1] = '\0';  // Ensure null-termination
        output += buffer.data();
    }

    return pclose(pipe);
}

/**
 * @brief Read file contents
 */
static bool readFile(const std::string& path, std::string& content) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    return true;
}

/**
 * @brief Parse /proc/mtd to get MTD device information
 */
static std::map<std::string, std::pair<uint64_t, std::string>> parseProcMtd() {
    std::map<std::string, std::pair<uint64_t, std::string>> devices;
    
    std::ifstream mtdFile(PROC_MTD);
    if (!mtdFile.is_open()) {
        return devices;
    }
    
    std::string line;
    std::regex mtdRegex(R"(mtd(\d+):\s+([0-9a-fA-F]+)\s+[0-9a-fA-F]+\s+\"([^\"]+)\")");

    while (std::getline(mtdFile, line)) {
        std::smatch match;
        if (std::regex_search(line, match, mtdRegex)) {
            std::string devNum = match[1].str();
            uint64_t size = std::stoull(match[2].str(), nullptr, 16);
            std::string name = match[3].str();
            devices["/dev/mtd" + devNum] = {size, name};
        }
    }
    
    return devices;
}

/**
 * @brief Calculate SHA256 checksum of data
 */
static std::vector<uint8_t> calculateSHA256(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> hash(SHA256_LENGTH);
    SHA256(data.data(), data.size(), hash.data());
    return hash;
}

/**
 * @brief Calculate SHA256 checksum of file/device
 */
static std::vector<uint8_t> calculateFileSHA256(const std::string& path, uint64_t size) {
    std::vector<uint8_t> hash(SHA256_LENGTH);
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        return {};
    }
    
    std::vector<uint8_t> buffer(65536);
    uint64_t remaining = size;
    
    while (remaining > 0) {
        size_t toRead = std::min(remaining, static_cast<uint64_t>(buffer.size()));
        ssize_t bytesRead = read(fd, buffer.data(), toRead);
        
        if (bytesRead <= 0) {
            close(fd);
            return {};
        }
        
        SHA256_Update(&ctx, buffer.data(), bytesRead);
        remaining -= bytesRead;
    }
    
    close(fd);
    SHA256_Final(hash.data(), &ctx);
    return hash;
}

// ============================================================================
// OpenWRT Partition Manager Implementation
// ============================================================================

class OpenWRTPartitionManager : public IPartitionManager {
public:
    OpenWRTPartitionManager() = default;
    ~OpenWRTPartitionManager() override = default;

    PlatformResult initialize() override {
        // Parse MTD devices
        mtdDevices_ = parseProcMtd();
        if (mtdDevices_.empty()) {
            return PlatformResult::ERROR_NOT_FOUND;
        }
        
        // Detect partition layout
        detectPartitionLayout();
        
        // Determine active slot from boot parameters
        detectActiveSlot();
        
        initialized_ = true;
        return PlatformResult::SUCCESS;
    }

    std::vector<PartitionInfo> getPartitions() override {
        std::vector<PartitionInfo> partitions;
        
        for (const auto& [device, info] : mtdDevices_) {
            PartitionInfo pinfo;
            pinfo.device = device;
            pinfo.name = info.second;
            pinfo.size = info.first;
            pinfo.offset = 0;  // Would need to parse from DTS
            pinfo.slot = detectSlotFromName(info.second);
            pinfo.isActive = (pinfo.slot == activeSlot_);
            pinfo.isBootable = checkBootable(pinfo.slot);
            pinfo.isWriteProtected = false;
            
            partitions.push_back(pinfo);
        }
        
        return partitions;
    }

    std::optional<PartitionInfo> getPartition(PartitionSlot slot) override {
        auto partitions = getPartitions();
        
        for (const auto& p : partitions) {
            if (p.slot == slot) {
                return p;
            }
        }
        
        return std::nullopt;
    }

    PartitionSlot getActiveSlot() override {
        return activeSlot_;
    }

    bool isSlotBootable(PartitionSlot slot) override {
        return checkBootable(slot);
    }

    PlatformResult setSlotBootable(PartitionSlot slot, bool bootable) override {
        std::string varName = (slot == PartitionSlot::SLOT_A) ? 
                              UBOOT_BOOTABLE_A : UBOOT_BOOTABLE_B;
        
        std::string cmd = "fw_setenv " + varName + " " + (bootable ? "1" : "0");
        std::string output;
        
        if (execCommand(cmd, output) != 0) {
            return PlatformResult::ERROR_BOOTLOADER_ERROR;
        }
        
        return PlatformResult::SUCCESS;
    }

    PlatformResult switchSlot(PartitionSlot targetSlot) override {
        if (!isSlotBootable(targetSlot)) {
            return PlatformResult::ERROR_INVALID_PARTITION;
        }
        
        // Set the boot slot for next reboot
        std::string slotValue = (targetSlot == PartitionSlot::SLOT_A) ? "a" : "b";
        
        std::string cmd = "fw_setenv " + std::string(UBOOT_BOOT_SLOT) + " " + slotValue;
        std::string output;
        
        if (execCommand(cmd, output) != 0) {
            return PlatformResult::ERROR_BOOTLOADER_ERROR;
        }
        
        // Reset boot attempts for the target slot
        std::string attemptsVar = (targetSlot == PartitionSlot::SLOT_A) ?
                                  UBOOT_BOOT_ATTEMPTS_A : UBOOT_BOOT_ATTEMPTS_B;
        
        cmd = "fw_setenv " + attemptsVar + " " + std::to_string(MAX_BOOT_ATTEMPTS);
        if (execCommand(cmd, output) != 0) {
            return PlatformResult::ERROR_BOOTLOADER_ERROR;
        }
        
        // Set upgrade available flag
        cmd = "fw_setenv " + std::string(UBOOT_UPGRADE_AVAILABLE) + " 1";
        execCommand(cmd, output);
        
        return PlatformResult::SUCCESS;
    }

    PartitionSlot getInactiveSlot() override {
        return (activeSlot_ == PartitionSlot::SLOT_A) ? 
               PartitionSlot::SLOT_B : PartitionSlot::SLOT_A;
    }

    PlatformResult verifyPartition(PartitionSlot slot, 
                                   const std::vector<uint8_t>& expectedChecksum) override {
        auto partition = getPartition(slot);
        if (!partition) {
            return PlatformResult::ERROR_NOT_FOUND;
        }
        
        auto actualChecksum = calculateFileSHA256(partition->device, partition->size);
        if (actualChecksum.empty()) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        if (!expectedChecksum.empty() && actualChecksum != expectedChecksum) {
            return PlatformResult::ERROR_CHECKSUM_MISMATCH;
        }
        
        return PlatformResult::SUCCESS;
    }

    std::vector<uint8_t> calculateChecksum(PartitionSlot slot) override {
        auto partition = getPartition(slot);
        if (!partition) {
            return {};
        }
        
        return calculateFileSHA256(partition->device, partition->size);
    }

private:
    void detectPartitionLayout() {
        // Look for common A/B partition naming patterns
        for (const auto& [device, info] : mtdDevices_) {
            const std::string& name = info.second;
            
            if (name.find("rootfs") != std::string::npos ||
                name.find("firmware") != std::string::npos ||
                name.find("kernel") != std::string::npos) {
                
                if (name.find("_a") != std::string::npos || 
                    name.find("-a") != std::string::npos ||
                    name.find("0") != std::string::npos) {
                    slotADevice_ = device;
                } else if (name.find("_b") != std::string::npos ||
                           name.find("-b") != std::string::npos ||
                           name.find("1") != std::string::npos) {
                    slotBDevice_ = device;
                }
            }
            
            if (name.find("recovery") != std::string::npos) {
                recoveryDevice_ = device;
            }
        }
    }

    void detectActiveSlot() {
        // Try to read from U-Boot environment
        std::string output;
        if (execCommand("fw_printenv " + std::string(UBOOT_ACTIVE_SLOT), output) == 0) {
            if (output.find("=b") != std::string::npos) {
                activeSlot_ = PartitionSlot::SLOT_B;
                return;
            }
        }
        
        // Fall back to parsing /proc/cmdline
        std::string cmdline;
        if (readFile(PROC_CMDLINE, cmdline)) {
            if (cmdline.find("root=/dev/mtdblock") != std::string::npos) {
                // Parse which MTD block is being used
                std::regex rootRegex(R"(root=/dev/mtdblock(\d+))");
                std::smatch match;
                if (std::regex_search(cmdline, match, rootRegex)) {
                    std::string mtdDev = "/dev/mtd" + match[1].str();
                    if (mtdDev == slotBDevice_) {
                        activeSlot_ = PartitionSlot::SLOT_B;
                        return;
                    }
                }
            }
        }
        
        activeSlot_ = PartitionSlot::SLOT_A;  // Default
    }

    bool checkBootable(PartitionSlot slot) {
        std::string varName = (slot == PartitionSlot::SLOT_A) ? 
                              UBOOT_BOOTABLE_A : UBOOT_BOOTABLE_B;
        
        std::string output;
        if (execCommand("fw_printenv " + varName, output) == 0) {
            return output.find("=1") != std::string::npos;
        }
        
        // If variable doesn't exist, check if partition has valid content
        auto partition = getPartition(slot);
        if (partition) {
            return verifyPartitionHeader(partition->device);
        }
        
        return false;
    }

    bool verifyPartitionHeader(const std::string& device) {
        // Check for common firmware image headers
        int fd = open(device.c_str(), O_RDONLY);
        if (fd < 0) {
            return false;
        }
        
        uint8_t header[64];
        if (read(fd, header, sizeof(header)) != sizeof(header)) {
            close(fd);
            return false;
        }
        close(fd);
        
        // Check for common magic numbers
        // UBI: "UBI#"
        if (header[0] == 'U' && header[1] == 'B' && header[2] == 'I' && header[3] == '#') {
            return true;
        }
        
        // SquashFS: 0x73717368 (hsqs)
        if (header[0] == 'h' && header[1] == 's' && header[2] == 'q' && header[3] == 's') {
            return true;
        }
        
        // Linux kernel: ARM64 Image magic
        if (header[0] == 'M' && header[1] == 'Z') {
            return true;
        }
        
        // uImage header
        if (header[0] == 0x27 && header[1] == 0x05 && 
            header[2] == 0x19 && header[3] == 0x56) {
            return true;
        }
        
        return false;
    }

    PartitionSlot detectSlotFromName(const std::string& name) {
        if (name.find("_a") != std::string::npos || 
            name.find("-a") != std::string::npos) {
            return PartitionSlot::SLOT_A;
        }
        if (name.find("_b") != std::string::npos ||
            name.find("-b") != std::string::npos) {
            return PartitionSlot::SLOT_B;
        }
        if (name.find("recovery") != std::string::npos) {
            return PartitionSlot::RECOVERY;
        }
        if (name.find("boot") != std::string::npos ||
            name.find("u-boot") != std::string::npos) {
            return PartitionSlot::BOOTLOADER;
        }
        return PartitionSlot::SLOT_A;  // Default
    }

    std::map<std::string, std::pair<uint64_t, std::string>> mtdDevices_;
    std::string slotADevice_;
    std::string slotBDevice_;
    std::string recoveryDevice_;
    PartitionSlot activeSlot_ = PartitionSlot::SLOT_A;
    bool initialized_ = false;
};

// ============================================================================
// OpenWRT Flash Manager Implementation
// ============================================================================

class OpenWRTFlashManager : public IFlashManager {
public:
    OpenWRTFlashManager() = default;
    ~OpenWRTFlashManager() override = default;

    PlatformResult initialize() override {
        return PlatformResult::SUCCESS;
    }

    PlatformResult read(const std::string& device, uint64_t offset, 
                        uint64_t size, std::vector<uint8_t>& buffer) override {
        int fd = open(device.c_str(), O_RDONLY);
        if (fd < 0) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
            close(fd);
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        buffer.resize(size);
        ssize_t bytesRead = ::read(fd, buffer.data(), size);
        close(fd);
        
        if (bytesRead != static_cast<ssize_t>(size)) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        return PlatformResult::SUCCESS;
    }

    PlatformResult write(const std::string& device, uint64_t offset,
                         const std::vector<uint8_t>& data,
                         FlashProgressCallback progress) override {
        // Get MTD device info
        uint64_t eraseSize, totalSize;
        if (getDeviceInfo(device, eraseSize, totalSize) != PlatformResult::SUCCESS) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        // Check if data fits
        if (offset + data.size() > totalSize) {
            return PlatformResult::ERROR_INSUFFICIENT_SPACE;
        }
        
        // Open device
        int fd = open(device.c_str(), O_RDWR | O_SYNC);
        if (fd < 0) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        // Erase required blocks
        uint64_t eraseStart = (offset / eraseSize) * eraseSize;
        uint64_t eraseEnd = ((offset + data.size() + eraseSize - 1) / eraseSize) * eraseSize;
        
        for (uint64_t pos = eraseStart; pos < eraseEnd; pos += eraseSize) {
            erase_info_t erase;
            erase.start = pos;
            erase.length = eraseSize;
            
            if (ioctl(fd, MEMERASE, &erase) != 0) {
                close(fd);
                return PlatformResult::ERROR_IO_FAILURE;
            }
        }
        
        // Write data
        if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
            close(fd);
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        uint64_t written = 0;
        const size_t chunkSize = 65536;  // 64KB chunks
        
        while (written < data.size()) {
            size_t toWrite = std::min(chunkSize, data.size() - written);
            ssize_t result = ::write(fd, data.data() + written, toWrite);
            
            if (result <= 0) {
                close(fd);
                return PlatformResult::ERROR_IO_FAILURE;
            }
            
            written += result;
            
            if (progress) {
                progress(written, data.size());
            }
        }
        
        // Sync and close
        fsync(fd);
        close(fd);
        
        return PlatformResult::SUCCESS;
    }

    PlatformResult erase(const std::string& device, uint64_t offset, uint64_t size) override {
        int fd = open(device.c_str(), O_RDWR);
        if (fd < 0) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        // Get erase block size
        mtd_info_t mtdInfo;
        if (ioctl(fd, MEMGETINFO, &mtdInfo) != 0) {
            close(fd);
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        // Align to erase blocks
        uint64_t eraseSize = mtdInfo.erasesize;
        uint64_t eraseStart = (offset / eraseSize) * eraseSize;
        uint64_t eraseEnd = ((offset + size + eraseSize - 1) / eraseSize) * eraseSize;
        
        for (uint64_t pos = eraseStart; pos < eraseEnd; pos += eraseSize) {
            erase_info_t erase;
            erase.start = pos;
            erase.length = eraseSize;
            
            if (ioctl(fd, MEMERASE, &erase) != 0) {
                close(fd);
                return PlatformResult::ERROR_IO_FAILURE;
            }
        }
        
        close(fd);
        return PlatformResult::SUCCESS;
    }

    PlatformResult getDeviceInfo(const std::string& device,
                                  uint64_t& eraseSize, uint64_t& totalSize) override {
        int fd = open(device.c_str(), O_RDONLY);
        if (fd < 0) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        mtd_info_t mtdInfo;
        if (ioctl(fd, MEMGETINFO, &mtdInfo) != 0) {
            close(fd);
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        eraseSize = mtdInfo.erasesize;
        totalSize = mtdInfo.size;
        
        close(fd);
        return PlatformResult::SUCCESS;
    }

    PlatformResult lock(const std::string& device, uint64_t offset, uint64_t size) override {
        int fd = open(device.c_str(), O_RDWR);
        if (fd < 0) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        erase_info_t lockInfo;
        lockInfo.start = offset;
        lockInfo.length = size;
        
        int result = ioctl(fd, MEMLOCK, &lockInfo);
        close(fd);
        
        return (result == 0) ? PlatformResult::SUCCESS : PlatformResult::ERROR_IO_FAILURE;
    }

    PlatformResult unlock(const std::string& device, uint64_t offset, uint64_t size) override {
        int fd = open(device.c_str(), O_RDWR);
        if (fd < 0) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        erase_info_t unlockInfo;
        unlockInfo.start = offset;
        unlockInfo.length = size;
        
        int result = ioctl(fd, MEMUNLOCK, &unlockInfo);
        close(fd);
        
        return (result == 0) ? PlatformResult::SUCCESS : PlatformResult::ERROR_IO_FAILURE;
    }
};

// ============================================================================
// OpenWRT Bootloader Control Implementation (U-Boot)
// ============================================================================

class OpenWRTBootloaderControl : public IBootloaderControl {
public:
    OpenWRTBootloaderControl() = default;
    ~OpenWRTBootloaderControl() override = default;

    PlatformResult initialize() override {
        // Check if fw_printenv/fw_setenv are available
        std::string output;
        if (execCommand("which fw_printenv", output) != 0) {
            return PlatformResult::ERROR_NOT_FOUND;
        }
        
        initialized_ = true;
        return PlatformResult::SUCCESS;
    }

    std::string getBootloaderType() override {
        return "u-boot";
    }

    std::optional<std::string> getEnv(const std::string& name) override {
        std::string output;
        if (execCommand("fw_printenv " + name + " 2>/dev/null", output) != 0) {
            return std::nullopt;
        }
        
        // Parse "name=value" format
        size_t eqPos = output.find('=');
        if (eqPos != std::string::npos) {
            std::string value = output.substr(eqPos + 1);
            // Remove trailing newline
            while (!value.empty() && (value.back() == '\n' || value.back() == '\r')) {
                value.pop_back();
            }
            return value;
        }
        
        return std::nullopt;
    }

    PlatformResult setEnv(const std::string& name, const std::string& value) override {
        std::string cmd = "fw_setenv " + name + " '" + value + "'";
        std::string output;
        
        if (execCommand(cmd, output) != 0) {
            return PlatformResult::ERROR_BOOTLOADER_ERROR;
        }
        
        return PlatformResult::SUCCESS;
    }

    std::map<std::string, std::string> getAllEnv() override {
        std::map<std::string, std::string> env;
        std::string output;
        
        if (execCommand("fw_printenv", output) == 0) {
            std::istringstream stream(output);
            std::string line;
            
            while (std::getline(stream, line)) {
                size_t eqPos = line.find('=');
                if (eqPos != std::string::npos) {
                    std::string name = line.substr(0, eqPos);
                    std::string value = line.substr(eqPos + 1);
                    env[name] = value;
                }
            }
        }
        
        return env;
    }

    PlatformResult saveEnv() override {
        // U-Boot fw_setenv automatically saves
        return PlatformResult::SUCCESS;
    }

    PlatformResult setBootSlot(PartitionSlot slot) override {
        std::string slotValue = (slot == PartitionSlot::SLOT_A) ? "a" : "b";
        return setEnv(UBOOT_BOOT_SLOT, slotValue);
    }

    PartitionSlot getBootSlot() override {
        auto value = getEnv(UBOOT_BOOT_SLOT);
        if (value && *value == "b") {
            return PartitionSlot::SLOT_B;
        }
        return PartitionSlot::SLOT_A;
    }

    PlatformResult setBootAttempts(PartitionSlot slot, int attempts) override {
        std::string varName = (slot == PartitionSlot::SLOT_A) ?
                              UBOOT_BOOT_ATTEMPTS_A : UBOOT_BOOT_ATTEMPTS_B;
        return setEnv(varName, std::to_string(attempts));
    }

    int getBootAttempts(PartitionSlot slot) override {
        std::string varName = (slot == PartitionSlot::SLOT_A) ?
                              UBOOT_BOOT_ATTEMPTS_A : UBOOT_BOOT_ATTEMPTS_B;
        
        auto value = getEnv(varName);
        if (value) {
            try {
                return std::stoi(*value);
            } catch (...) {
                return 0;
            }
        }
        return 0;
    }

    PlatformResult markBootSuccessful() override {
        // Get current active slot
        auto activeSlot = getEnv(UBOOT_ACTIVE_SLOT);
        PartitionSlot slot = (activeSlot && *activeSlot == "b") ?
                             PartitionSlot::SLOT_B : PartitionSlot::SLOT_A;
        
        // Mark slot as bootable
        std::string varName = (slot == PartitionSlot::SLOT_A) ?
                              UBOOT_BOOTABLE_A : UBOOT_BOOTABLE_B;
        
        auto result = setEnv(varName, "1");
        if (result != PlatformResult::SUCCESS) {
            return result;
        }
        
        // Clear upgrade available flag
        setEnv(UBOOT_UPGRADE_AVAILABLE, "0");
        
        return PlatformResult::SUCCESS;
    }

    PlatformResult requestRecoveryBoot() override {
        return setEnv("boot_recovery", "1");
    }

private:
    bool initialized_ = false;
};

// ============================================================================
// OpenWRT System Manager Implementation
// ============================================================================

class OpenWRTSystemManager : public ISystemManager {
public:
    OpenWRTSystemManager() = default;
    ~OpenWRTSystemManager() override = default;

    SystemInfo getSystemInfo() override {
        SystemInfo info;
        info.platform = "openwrt";
        
        // Get architecture
        std::string output;
        if (execCommand("uname -m", output) == 0) {
            info.architecture = output;
            // Trim whitespace
            info.architecture.erase(info.architecture.find_last_not_of(" \n\r\t") + 1);
        }
        
        // Get kernel version
        if (execCommand("uname -r", output) == 0) {
            info.kernelVersion = output;
            info.kernelVersion.erase(info.kernelVersion.find_last_not_of(" \n\r\t") + 1);
        }
        
        // Get board name
        std::string boardContent;
        if (readFile(BOARD_JSON, boardContent)) {
            std::regex boardRegex(R"(\"model\":\s*{\s*\"id\":\s*\"([^\"]+)\")");
            std::smatch match;
            if (std::regex_search(boardContent, match, boardRegex)) {
                info.boardName = match[1].str();
            }
        }
        
        // Get memory info
        struct sysinfo si;
        if (sysinfo(&si) == 0) {
            info.totalRam = si.totalram * si.mem_unit;
            info.freeRam = si.freeram * si.mem_unit;
        }
        
        // Estimate flash info from MTD
        auto mtdDevices = parseProcMtd();
        info.totalFlash = 0;
        for (const auto& [dev, devInfo] : mtdDevices) {
            info.totalFlash += devInfo.first;
        }
        
        info.bootloaderType = "u-boot";
        
        return info;
    }

    PlatformResult reboot(int delay) override {
        if (delay > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(delay));
        }
        
        sync();
        
        std::string output;
        if (execCommand("reboot", output) == 0) {
            return PlatformResult::SUCCESS;
        }
        
        // Fallback to direct reboot
        ::reboot(LINUX_REBOOT_CMD_RESTART);
        return PlatformResult::SUCCESS;
    }

    PlatformResult shutdown(int delay) override {
        if (delay > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(delay));
        }
        
        sync();
        
        std::string output;
        if (execCommand("poweroff", output) == 0) {
            return PlatformResult::SUCCESS;
        }
        
        // Fallback
        ::reboot(LINUX_REBOOT_CMD_POWER_OFF);
        return PlatformResult::SUCCESS;
    }

    PlatformResult enterRecoveryMode() override {
        // Set recovery boot flag
        std::string output;
        if (execCommand("fw_setenv boot_recovery 1", output) != 0) {
            return PlatformResult::ERROR_BOOTLOADER_ERROR;
        }
        
        // Reboot to recovery
        return reboot(0);
    }

    bool isRecoveryMode() override {
        std::string cmdline;
        if (readFile(PROC_CMDLINE, cmdline)) {
            return cmdline.find("recovery") != std::string::npos;
        }
        return false;
    }

    PlatformResult syncFilesystems() override {
        sync();
        return PlatformResult::SUCCESS;
    }

    std::string getFirmwareVersion() override {
        std::string content;
        if (readFile(VERSION_FILE, content)) {
            std::regex versionRegex(R"(DISTRIB_RELEASE='([^']+)')");
            std::smatch match;
            if (std::regex_search(content, match, versionRegex)) {
                return match[1].str();
            }
        }
        return "unknown";
    }

    int executeCommand(const std::string& command, std::string& output) override {
        return execCommand(command, output);
    }
};

} // namespace openwrt

// ============================================================================
// Platform Factory - Static Members
// ============================================================================

std::shared_ptr<IPartitionManager> PlatformFactory::partitionManager_;
std::shared_ptr<IFlashManager> PlatformFactory::flashManager_;
std::shared_ptr<IBootloaderControl> PlatformFactory::bootloaderControl_;
std::shared_ptr<ISystemManager> PlatformFactory::systemManager_;
std::string PlatformFactory::platformName_;
bool PlatformFactory::initialized_ = false;

bool PlatformFactory::detectPlatform() {
    // Check for OpenWRT
    std::string content;
    if (openwrt::readFile("/etc/openwrt_release", content)) {
        platformName_ = "openwrt";
        
        partitionManager_ = std::make_shared<openwrt::OpenWRTPartitionManager>();
        flashManager_ = std::make_shared<openwrt::OpenWRTFlashManager>();
        bootloaderControl_ = std::make_shared<openwrt::OpenWRTBootloaderControl>();
        systemManager_ = std::make_shared<openwrt::OpenWRTSystemManager>();
        
        // Initialize all managers
        partitionManager_->initialize();
        flashManager_->initialize();
        bootloaderControl_->initialize();
        
        initialized_ = true;
        return true;
    }
    
    // Add detection for other platforms here (generic Linux, etc.)
    
    return false;
}

std::shared_ptr<IPartitionManager> PlatformFactory::getPartitionManager() {
    if (!initialized_) detectPlatform();
    return partitionManager_;
}

std::shared_ptr<IFlashManager> PlatformFactory::getFlashManager() {
    if (!initialized_) detectPlatform();
    return flashManager_;
}

std::shared_ptr<IBootloaderControl> PlatformFactory::getBootloaderControl() {
    if (!initialized_) detectPlatform();
    return bootloaderControl_;
}

std::shared_ptr<ISystemManager> PlatformFactory::getSystemManager() {
    if (!initialized_) detectPlatform();
    return systemManager_;
}

std::string PlatformFactory::getPlatformName() {
    if (!initialized_) detectPlatform();
    return platformName_;
}

} // namespace platform
} // namespace lwm2m
