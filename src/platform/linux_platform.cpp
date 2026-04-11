/**
 * @file linux_platform.cpp
 * @brief Generic Linux Platform Implementation for Friendly LwM2M Client
 * 
 * Provides platform implementations for standard Linux systems including:
 * - Raspberry Pi
 * - Generic x86_64/ARM64 Linux
 * - Systems using GRUB bootloader
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
#include <filesystem>

// Linux headers
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <linux/reboot.h>
#include <mntent.h>
// Optional: blkid support for unmounted partition detection
// #include <blkid/blkid.h>
#include <openssl/sha.h>

namespace fs = std::filesystem;

namespace lwm2m {
namespace platform {
namespace linux_generic {

// ============================================================================
// Constants
// ============================================================================

constexpr const char* GRUB_ENV_FILE = "/boot/grub/grubenv";
constexpr const char* GRUB_CFG_FILE = "/boot/grub/grub.cfg";
constexpr const char* OS_RELEASE = "/etc/os-release";
constexpr const char* PROC_CMDLINE = "/proc/cmdline";
constexpr const char* PROC_MOUNTS = "/proc/mounts";
constexpr int MAX_BOOT_ATTEMPTS = 3;
constexpr size_t SHA256_LENGTH = 32;

// GRUB environment variable names
constexpr const char* GRUB_DEFAULT = "saved_entry";
constexpr const char* GRUB_BOOT_SUCCESS = "boot_success";
constexpr const char* GRUB_BOOT_COUNTER = "boot_counter";

// ============================================================================
// Utility Functions
// ============================================================================

static int execCommand(const std::string& cmd, std::string& output) {
    std::array<char, 4096> buffer;
    output.clear();
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        return -1;
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        output += buffer.data();
    }
    
    return pclose(pipe);
}

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

static bool writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    return true;
}

static std::vector<uint8_t> calculateFileSHA256(const std::string& path, uint64_t size = 0) {
    std::vector<uint8_t> hash(SHA256_LENGTH);
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }
    
    std::vector<char> buffer(65536);
    uint64_t totalRead = 0;
    
    while (file.good() && (size == 0 || totalRead < size)) {
        size_t toRead = buffer.size();
        if (size > 0 && totalRead + toRead > size) {
            toRead = size - totalRead;
        }
        
        file.read(buffer.data(), toRead);
        size_t bytesRead = file.gcount();
        
        if (bytesRead > 0) {
            SHA256_Update(&ctx, buffer.data(), bytesRead);
            totalRead += bytesRead;
        }
    }
    
    SHA256_Final(hash.data(), &ctx);
    return hash;
}

// ============================================================================
// Linux Partition Manager
// ============================================================================

class LinuxPartitionManager : public IPartitionManager {
public:
    LinuxPartitionManager() = default;
    ~LinuxPartitionManager() override = default;

    PlatformResult initialize() override {
        // Detect partition layout
        detectPartitions();
        detectActiveSlot();
        
        initialized_ = true;
        return PlatformResult::SUCCESS;
    }

    std::vector<PartitionInfo> getPartitions() override {
        return partitions_;
    }

    std::optional<PartitionInfo> getPartition(PartitionSlot slot) override {
        for (const auto& p : partitions_) {
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
        auto partition = getPartition(slot);
        if (!partition) {
            return false;
        }
        
        // Check if partition exists and has valid filesystem
        return fs::exists(partition->device);
    }

    PlatformResult setSlotBootable(PartitionSlot slot, bool bootable) override {
        // For generic Linux, we manage this through GRUB or similar
        std::string slotName = (slot == PartitionSlot::SLOT_A) ? "slot_a" : "slot_b";
        std::string cmd = "grub-editenv " + std::string(GRUB_ENV_FILE) + 
                          " set " + slotName + "_bootable=" + (bootable ? "1" : "0");
        
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
        
        // Update GRUB default entry
        std::string entryName = (targetSlot == PartitionSlot::SLOT_A) ? 
                                "slot_a" : "slot_b";
        
        std::string cmd = "grub-editenv " + std::string(GRUB_ENV_FILE) + 
                          " set " + std::string(GRUB_DEFAULT) + "=" + entryName;
        
        std::string output;
        if (execCommand(cmd, output) != 0) {
            return PlatformResult::ERROR_BOOTLOADER_ERROR;
        }
        
        // Set boot counter for automatic fallback
        cmd = "grub-editenv " + std::string(GRUB_ENV_FILE) + 
              " set " + std::string(GRUB_BOOT_COUNTER) + "=" + 
              std::to_string(MAX_BOOT_ATTEMPTS);
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
        
        if (expectedChecksum.empty()) {
            // Just check if readable
            if (fs::exists(partition->device)) {
                return PlatformResult::SUCCESS;
            }
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        auto actualChecksum = calculateFileSHA256(partition->device, partition->size);
        if (actualChecksum != expectedChecksum) {
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
    void detectPartitions() {
        partitions_.clear();
        
        // Read /proc/mounts to find mounted partitions
        FILE* mounts = setmntent(PROC_MOUNTS, "r");
        if (!mounts) {
            return;
        }
        
        struct mntent* entry;
        while ((entry = getmntent(mounts)) != nullptr) {
            // Look for root filesystems that might be A/B
            if (std::string(entry->mnt_dir) == "/" ||
                std::string(entry->mnt_dir).find("/rootfs") != std::string::npos) {
                
                PartitionInfo info;
                info.device = entry->mnt_fsname;
                info.name = entry->mnt_dir;
                
                // Determine slot from device name
                std::string devName = entry->mnt_fsname;
                if (devName.find("_a") != std::string::npos ||
                    devName.find("p1") != std::string::npos ||
                    devName.find("1") != std::string::npos) {
                    info.slot = PartitionSlot::SLOT_A;
                } else if (devName.find("_b") != std::string::npos ||
                           devName.find("p2") != std::string::npos ||
                           devName.find("2") != std::string::npos) {
                    info.slot = PartitionSlot::SLOT_B;
                } else {
                    info.slot = PartitionSlot::SLOT_A;
                }
                
                // Get partition size
                struct statvfs stat;
                if (statvfs(entry->mnt_dir, &stat) == 0) {
                    info.size = stat.f_blocks * stat.f_frsize;
                    info.usedSize = (stat.f_blocks - stat.f_bfree) * stat.f_frsize;
                }
                
                info.isActive = (std::string(entry->mnt_dir) == "/");
                info.isBootable = true;
                info.isWriteProtected = (std::string(entry->mnt_opts).find("ro") != std::string::npos);
                
                partitions_.push_back(info);
            }
        }
        
        endmntent(mounts);

        // Note: blkid support disabled - install libblkid-dev to enable
        // detectUnmountedPartitions();
    }

    // Disabled: requires libblkid-dev (sudo apt install libblkid-dev)
    /*
    void detectUnmountedPartitions() {
        blkid_cache cache = nullptr;
        if (blkid_get_cache(&cache, nullptr) != 0) {
            return;
        }

        blkid_dev_iterate iter = blkid_dev_iterate_begin(cache);
        blkid_dev dev;

        while (blkid_dev_next(iter, &dev) == 0) {
            const char* devname = blkid_dev_devname(dev);

            // Check if already in our list
            bool found = false;
            for (const auto& p : partitions_) {
                if (p.device == devname) {
                    found = true;
                    break;
                }
            }

            if (!found && devname) {
                std::string name = devname;
                // Look for potential A/B partition naming
                if (name.find("rootfs") != std::string::npos ||
                    name.find("system") != std::string::npos) {

                    PartitionInfo info;
                    info.device = devname;
                    info.name = name;

                    if (name.find("_b") != std::string::npos) {
                        info.slot = PartitionSlot::SLOT_B;
                    } else {
                        info.slot = PartitionSlot::SLOT_A;
                    }

                    info.isActive = false;
                    info.isBootable = true;

                    partitions_.push_back(info);
                }
            }
        }

        blkid_dev_iterate_end(iter);
        blkid_put_cache(cache);
    }
    */

    void detectActiveSlot() {
        // Check GRUB environment
        std::string output;
        if (execCommand("grub-editenv " + std::string(GRUB_ENV_FILE) + " list", output) == 0) {
            if (output.find("saved_entry=slot_b") != std::string::npos) {
                activeSlot_ = PartitionSlot::SLOT_B;
                return;
            }
        }
        
        // Check kernel command line
        std::string cmdline;
        if (readFile(PROC_CMDLINE, cmdline)) {
            if (cmdline.find("root=") != std::string::npos) {
                if (cmdline.find("_b") != std::string::npos ||
                    cmdline.find("slot_b") != std::string::npos) {
                    activeSlot_ = PartitionSlot::SLOT_B;
                    return;
                }
            }
        }
        
        activeSlot_ = PartitionSlot::SLOT_A;
    }

    std::vector<PartitionInfo> partitions_;
    PartitionSlot activeSlot_ = PartitionSlot::SLOT_A;
    bool initialized_ = false;
};

// ============================================================================
// Linux Flash Manager (Block Device Operations)
// ============================================================================

class LinuxFlashManager : public IFlashManager {
public:
    LinuxFlashManager() = default;
    ~LinuxFlashManager() override = default;

    PlatformResult initialize() override {
        return PlatformResult::SUCCESS;
    }

    PlatformResult read(const std::string& device, uint64_t offset, 
                        uint64_t size, std::vector<uint8_t>& buffer) override {
        std::ifstream file(device, std::ios::binary);
        if (!file.is_open()) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        file.seekg(offset);
        if (!file.good()) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        buffer.resize(size);
        file.read(reinterpret_cast<char*>(buffer.data()), size);
        
        if (static_cast<uint64_t>(file.gcount()) != size) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        return PlatformResult::SUCCESS;
    }

    PlatformResult write(const std::string& device, uint64_t offset,
                         const std::vector<uint8_t>& data,
                         FlashProgressCallback progress) override {
        std::ofstream file(device, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open()) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        file.seekp(offset);
        if (!file.good()) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        const size_t chunkSize = 1024 * 1024;  // 1MB chunks
        uint64_t written = 0;
        
        while (written < data.size()) {
            size_t toWrite = std::min(chunkSize, data.size() - written);
            file.write(reinterpret_cast<const char*>(data.data() + written), toWrite);
            
            if (!file.good()) {
                return PlatformResult::ERROR_IO_FAILURE;
            }
            
            written += toWrite;
            
            if (progress) {
                progress(written, data.size());
            }
        }
        
        file.flush();
        return PlatformResult::SUCCESS;
    }

    PlatformResult erase(const std::string& device, uint64_t offset, uint64_t size) override {
        // For block devices, we write zeros
        std::vector<uint8_t> zeros(size, 0);
        return write(device, offset, zeros, nullptr);
    }

    PlatformResult getDeviceInfo(const std::string& device,
                                  uint64_t& eraseSize, uint64_t& totalSize) override {
        struct stat st;
        if (stat(device.c_str(), &st) != 0) {
            return PlatformResult::ERROR_IO_FAILURE;
        }
        
        eraseSize = 4096;  // Typical block size
        
        if (S_ISBLK(st.st_mode)) {
            // Block device - get size via ioctl
            int fd = open(device.c_str(), O_RDONLY);
            if (fd >= 0) {
                uint64_t size;
                if (ioctl(fd, BLKGETSIZE64, &size) == 0) {
                    totalSize = size;
                }
                close(fd);
            }
        } else {
            totalSize = st.st_size;
        }
        
        return PlatformResult::SUCCESS;
    }

    PlatformResult lock(const std::string& device, uint64_t offset, uint64_t size) override {
        // Block devices don't typically support locking regions
        return PlatformResult::ERROR_NOT_SUPPORTED;
    }

    PlatformResult unlock(const std::string& device, uint64_t offset, uint64_t size) override {
        return PlatformResult::ERROR_NOT_SUPPORTED;
    }
};

// ============================================================================
// Linux Bootloader Control (GRUB)
// ============================================================================

class LinuxBootloaderControl : public IBootloaderControl {
public:
    LinuxBootloaderControl() = default;
    ~LinuxBootloaderControl() override = default;

    PlatformResult initialize() override {
        // Check if grub-editenv is available
        std::string output;
        if (execCommand("which grub-editenv", output) != 0) {
            // Try grub2-editenv
            if (execCommand("which grub2-editenv", output) != 0) {
                return PlatformResult::ERROR_NOT_FOUND;
            }
            grubEditEnv_ = "grub2-editenv";
        } else {
            grubEditEnv_ = "grub-editenv";
        }
        
        return PlatformResult::SUCCESS;
    }

    std::string getBootloaderType() override {
        return "grub";
    }

    std::optional<std::string> getEnv(const std::string& name) override {
        std::string output;
        std::string cmd = grubEditEnv_ + " " + std::string(GRUB_ENV_FILE) + " list";
        
        if (execCommand(cmd, output) != 0) {
            return std::nullopt;
        }
        
        // Parse "name=value" lines
        std::istringstream stream(output);
        std::string line;
        std::string searchKey = name + "=";
        
        while (std::getline(stream, line)) {
            if (line.find(searchKey) == 0) {
                return line.substr(searchKey.length());
            }
        }
        
        return std::nullopt;
    }

    PlatformResult setEnv(const std::string& name, const std::string& value) override {
        std::string cmd = grubEditEnv_ + " " + std::string(GRUB_ENV_FILE) + 
                          " set " + name + "=" + value;
        std::string output;
        
        if (execCommand(cmd, output) != 0) {
            return PlatformResult::ERROR_BOOTLOADER_ERROR;
        }
        
        return PlatformResult::SUCCESS;
    }

    std::map<std::string, std::string> getAllEnv() override {
        std::map<std::string, std::string> env;
        std::string output;
        std::string cmd = grubEditEnv_ + " " + std::string(GRUB_ENV_FILE) + " list";
        
        if (execCommand(cmd, output) == 0) {
            std::istringstream stream(output);
            std::string line;
            
            while (std::getline(stream, line)) {
                size_t eqPos = line.find('=');
                if (eqPos != std::string::npos) {
                    env[line.substr(0, eqPos)] = line.substr(eqPos + 1);
                }
            }
        }
        
        return env;
    }

    PlatformResult saveEnv() override {
        // GRUB editenv saves automatically
        return PlatformResult::SUCCESS;
    }

    PlatformResult setBootSlot(PartitionSlot slot) override {
        std::string slotName = (slot == PartitionSlot::SLOT_A) ? "slot_a" : "slot_b";
        return setEnv(GRUB_DEFAULT, slotName);
    }

    PartitionSlot getBootSlot() override {
        auto value = getEnv(GRUB_DEFAULT);
        if (value && value->find("slot_b") != std::string::npos) {
            return PartitionSlot::SLOT_B;
        }
        return PartitionSlot::SLOT_A;
    }

    PlatformResult setBootAttempts(PartitionSlot slot, int attempts) override {
        return setEnv(GRUB_BOOT_COUNTER, std::to_string(attempts));
    }

    int getBootAttempts(PartitionSlot slot) override {
        auto value = getEnv(GRUB_BOOT_COUNTER);
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
        auto result = setEnv(GRUB_BOOT_SUCCESS, "1");
        if (result != PlatformResult::SUCCESS) {
            return result;
        }
        
        // Reset boot counter
        return setEnv(GRUB_BOOT_COUNTER, "0");
    }

    PlatformResult requestRecoveryBoot() override {
        return setEnv(GRUB_DEFAULT, "recovery");
    }

private:
    std::string grubEditEnv_ = "grub-editenv";
};

// ============================================================================
// Linux System Manager
// ============================================================================

class LinuxSystemManager : public ISystemManager {
public:
    LinuxSystemManager() = default;
    ~LinuxSystemManager() override = default;

    SystemInfo getSystemInfo() override {
        SystemInfo info;
        info.platform = "linux";
        
        // Architecture
        std::string output;
        if (execCommand("uname -m", output) == 0) {
            info.architecture = output;
            info.architecture.erase(info.architecture.find_last_not_of(" \n\r\t") + 1);
        }
        
        // Kernel version
        if (execCommand("uname -r", output) == 0) {
            info.kernelVersion = output;
            info.kernelVersion.erase(info.kernelVersion.find_last_not_of(" \n\r\t") + 1);
        }
        
        // Board name from device tree or DMI
        std::string boardName;
        if (readFile("/sys/firmware/devicetree/base/model", boardName)) {
            info.boardName = boardName;
        } else if (readFile("/sys/class/dmi/id/product_name", boardName)) {
            info.boardName = boardName;
        }
        info.boardName.erase(info.boardName.find_last_not_of(" \n\r\t\0") + 1);
        
        // Memory info
        struct sysinfo si;
        if (sysinfo(&si) == 0) {
            info.totalRam = si.totalram * si.mem_unit;
            info.freeRam = si.freeram * si.mem_unit;
        }
        
        // Disk info
        struct statvfs stat;
        if (statvfs("/", &stat) == 0) {
            info.totalFlash = stat.f_blocks * stat.f_frsize;
            info.freeFlash = stat.f_bfree * stat.f_frsize;
        }
        
        info.bootloaderType = "grub";
        
        return info;
    }

    PlatformResult reboot(int delay) override {
        if (delay > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(delay));
        }
        
        sync();
        
        std::string output;
        if (execCommand("systemctl reboot", output) == 0) {
            return PlatformResult::SUCCESS;
        }
        
        // Fallback
        if (execCommand("reboot", output) == 0) {
            return PlatformResult::SUCCESS;
        }
        
        // Direct syscall
        ::reboot(LINUX_REBOOT_CMD_RESTART);
        return PlatformResult::SUCCESS;
    }

    PlatformResult shutdown(int delay) override {
        if (delay > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(delay));
        }
        
        sync();
        
        std::string output;
        if (execCommand("systemctl poweroff", output) == 0) {
            return PlatformResult::SUCCESS;
        }
        
        // Fallback
        ::reboot(LINUX_REBOOT_CMD_POWER_OFF);
        return PlatformResult::SUCCESS;
    }

    PlatformResult enterRecoveryMode() override {
        // Set GRUB to boot recovery
        std::string output;
        execCommand("grub-editenv " + std::string(GRUB_ENV_FILE) + 
                    " set saved_entry=recovery", output);
        
        return reboot(0);
    }

    bool isRecoveryMode() override {
        std::string cmdline;
        if (readFile(PROC_CMDLINE, cmdline)) {
            return cmdline.find("recovery") != std::string::npos ||
                   cmdline.find("single") != std::string::npos;
        }
        return false;
    }

    PlatformResult syncFilesystems() override {
        sync();
        return PlatformResult::SUCCESS;
    }

    std::string getFirmwareVersion() override {
        std::string content;
        if (readFile(OS_RELEASE, content)) {
            std::regex versionRegex(R"(VERSION_ID="?([^"\n]+)"?)");
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

} // namespace linux_generic

// ============================================================================
// Platform Factory - Extended to support Generic Linux
// ============================================================================

bool PlatformFactory::detectPlatform() {
    std::string content;
    
    // Check for OpenWRT first
    if (readFile("/etc/openwrt_release", content)) {
        platformName_ = "openwrt";
        // OpenWRT initialization handled in openwrt_platform.cpp
        return true;
    }
    
    // Check for generic Linux
    if (readFile("/etc/os-release", content)) {
        platformName_ = "linux";
        
        partitionManager_ = std::make_shared<linux_generic::LinuxPartitionManager>();
        flashManager_ = std::make_shared<linux_generic::LinuxFlashManager>();
        bootloaderControl_ = std::make_shared<linux_generic::LinuxBootloaderControl>();
        systemManager_ = std::make_shared<linux_generic::LinuxSystemManager>();
        
        partitionManager_->initialize();
        flashManager_->initialize();
        bootloaderControl_->initialize();
        
        initialized_ = true;
        return true;
    }
    
    return false;
}

} // namespace platform
} // namespace lwm2m
