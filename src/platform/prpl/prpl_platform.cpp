/**
 * @file prpl_platform.cpp
 * @brief prplOS Platform Implementation for Friendly LwM2M Client
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#include "prpl_platform.h"
#include "prpl_amxb_client.h"
#include "prpl_data_model.h"
#include "prpl_easymesh.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <filesystem>
#include <cstring>

// Linux headers
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <linux/reboot.h>
#include <mtd/mtd-user.h>
#include <openssl/sha.h>

namespace fs = std::filesystem;

namespace lwm2m {
namespace platform {
namespace prpl {

// ============================================================================
// Constants
// ============================================================================

constexpr const char* PRPL_OS_RELEASE = "/etc/prpl-release";
constexpr const char* OS_RELEASE = "/etc/os-release";
constexpr const char* PRPL_VERSION_FILE = "/etc/prpl-version";
constexpr const char* PROC_CMDLINE = "/proc/cmdline";
constexpr const char* PROC_MTD = "/proc/mtd";
constexpr const char* MTD_DEVICE_PATH = "/dev/mtd";
constexpr const char* DEFAULT_BUS_URI = "ubus:/var/run/ubus/ubus.sock";
constexpr size_t SHA256_LENGTH = 32;
constexpr size_t FLASH_CHUNK_SIZE = 4096;

// ============================================================================
// Static member initialization
// ============================================================================

std::shared_ptr<PrplAmxbClient> PrplPlatform::amxbClient_;
std::shared_ptr<PrplDataModel> PrplPlatform::dataModel_;
std::shared_ptr<PrplEasyMesh> PrplPlatform::easyMesh_;
std::shared_ptr<PrplPartitionManager> PrplPlatform::partitionManager_;
std::shared_ptr<PrplFlashManager> PrplPlatform::flashManager_;
std::shared_ptr<PrplBootloaderControl> PrplPlatform::bootloaderControl_;
std::shared_ptr<PrplSystemManager> PrplPlatform::systemManager_;
bool PrplPlatform::initialized_ = false;
std::mutex PrplPlatform::initMutex_;

// ============================================================================
// Utility Functions
// ============================================================================

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

static std::vector<uint8_t> calculateSHA256(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> hash(SHA256_LENGTH);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.data(), data.size());
    SHA256_Final(hash.data(), &sha256);
    return hash;
}

// ============================================================================
// PrplPartitionManager Implementation
// ============================================================================

PrplPartitionManager::PrplPartitionManager(std::shared_ptr<PrplAmxbClient> amxbClient)
    : amxbClient_(amxbClient) {
}

PrplPartitionManager::~PrplPartitionManager() = default;

PlatformResult PrplPartitionManager::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!amxbClient_ || !amxbClient_->isConnected()) {
        return PlatformResult::ERROR_NOT_SUPPORTED;
    }

    if (!detectPartitions()) {
        return PlatformResult::ERROR_NOT_FOUND;
    }

    return PlatformResult::SUCCESS;
}

bool PrplPartitionManager::detectPartitions() {
    std::ifstream mtdFile(PROC_MTD);
    if (!mtdFile.is_open()) {
        return false;
    }

    std::string line;
    std::getline(mtdFile, line); // Skip header

    int mtdNum = 0;
    while (std::getline(mtdFile, line)) {
        // Parse MTD partition information
        PartitionInfo info;

        // Example: mtd3: 02000000 00020000 "rootfs_a"
        std::istringstream iss(line);
        std::string mtdDev, sizeStr, eraseSizeStr, name;

        if (iss >> mtdDev >> sizeStr >> eraseSizeStr) {
            // Read the rest as partition name
            std::getline(iss, name);
            name.erase(0, name.find_first_not_of(" \t\""));
            name.erase(name.find_last_not_of(" \t\"") + 1);

            info.name = name;
            info.device = MTD_DEVICE_PATH + std::to_string(mtdNum);
            info.size = std::stoull(sizeStr, nullptr, 16);

            // Determine slot based on partition name
            if (name.find("rootfs_a") != std::string::npos ||
                name.find("firmware_a") != std::string::npos) {
                info.slot = PartitionSlot::SLOT_A;
                partitions_[PartitionSlot::SLOT_A] = info;
            } else if (name.find("rootfs_b") != std::string::npos ||
                       name.find("firmware_b") != std::string::npos) {
                info.slot = PartitionSlot::SLOT_B;
                partitions_[PartitionSlot::SLOT_B] = info;
            } else if (name.find("recovery") != std::string::npos) {
                info.slot = PartitionSlot::RECOVERY;
                partitions_[PartitionSlot::RECOVERY] = info;
            }
        }

        mtdNum++;
    }

    return !partitions_.empty();
}

std::vector<PartitionInfo> PrplPartitionManager::getPartitions() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PartitionInfo> result;

    for (const auto& [slot, info] : partitions_) {
        result.push_back(info);
    }

    return result;
}

std::optional<PartitionInfo> PrplPartitionManager::getPartition(PartitionSlot slot) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = partitions_.find(slot);
    if (it != partitions_.end()) {
        return it->second;
    }

    return std::nullopt;
}

PartitionSlot PrplPartitionManager::getActiveSlot() {
    // Query from bootloader via Ambiorix
    if (amxbClient_ && amxbClient_->isConnected()) {
        auto result = amxbClient_->get("Device.DeviceInfo.BootPartition");
        if (!result.empty() && result.count("BootPartition")) {
            std::string bootPart = result["BootPartition"];
            if (bootPart.find("_a") != std::string::npos) {
                return PartitionSlot::SLOT_A;
            } else if (bootPart.find("_b") != std::string::npos) {
                return PartitionSlot::SLOT_B;
            }
        }
    }

    // Fallback: check /proc/cmdline
    std::string cmdline;
    if (readFile(PROC_CMDLINE, cmdline)) {
        if (cmdline.find("root=/dev/mtdblock") != std::string::npos) {
            // Parse which partition is mounted as root
            // This is platform-specific logic
        }
    }

    return PartitionSlot::SLOT_A; // Default
}

bool PrplPartitionManager::isSlotBootable(PartitionSlot slot) {
    auto partInfo = getPartition(slot);
    return partInfo.has_value() && partInfo->isBootable;
}

PlatformResult PrplPartitionManager::setSlotBootable(PartitionSlot slot, bool bootable) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = partitions_.find(slot);
    if (it == partitions_.end()) {
        return PlatformResult::ERROR_NOT_FOUND;
    }

    it->second.isBootable = bootable;

    // Update bootloader configuration via Ambiorix
    if (amxbClient_ && amxbClient_->isConnected()) {
        std::map<std::string, std::string> params;
        params["Bootable"] = bootable ? "true" : "false";

        std::string slotName = (slot == PartitionSlot::SLOT_A) ? "A" : "B";
        amxbClient_->set("Device.DeviceInfo.BootSlot." + slotName, params);
    }

    return PlatformResult::SUCCESS;
}

PlatformResult PrplPartitionManager::switchSlot(PartitionSlot targetSlot) {
    if (!isSlotBootable(targetSlot)) {
        return PlatformResult::ERROR_INVALID_PARTITION;
    }

    // Set boot slot via Ambiorix
    if (amxbClient_ && amxbClient_->isConnected()) {
        std::map<std::string, std::string> params;
        std::string slotName = (targetSlot == PartitionSlot::SLOT_A) ? "A" : "B";
        params["NextBootSlot"] = slotName;

        amxbClient_->set("Device.DeviceInfo", params);
    }

    return PlatformResult::SUCCESS;
}

PartitionSlot PrplPartitionManager::getInactiveSlot() {
    PartitionSlot active = getActiveSlot();
    return (active == PartitionSlot::SLOT_A) ? PartitionSlot::SLOT_B : PartitionSlot::SLOT_A;
}

PlatformResult PrplPartitionManager::verifyPartition(PartitionSlot slot,
                                                     const std::vector<uint8_t>& expectedChecksum) {
    auto checksum = calculateChecksum(slot);
    if (checksum.empty()) {
        return PlatformResult::ERROR_IO_FAILURE;
    }

    if (!expectedChecksum.empty() && checksum != expectedChecksum) {
        return PlatformResult::ERROR_CHECKSUM_MISMATCH;
    }

    return PlatformResult::SUCCESS;
}

std::vector<uint8_t> PrplPartitionManager::calculateChecksum(PartitionSlot slot) {
    auto partInfo = getPartition(slot);
    if (!partInfo.has_value()) {
        return {};
    }

    int fd = open(partInfo->device.c_str(), O_RDONLY);
    if (fd < 0) {
        return {};
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    std::vector<uint8_t> buffer(FLASH_CHUNK_SIZE);
    ssize_t bytesRead;
    uint64_t totalRead = 0;

    while (totalRead < partInfo->size &&
           (bytesRead = read(fd, buffer.data(), buffer.size())) > 0) {
        SHA256_Update(&sha256, buffer.data(), bytesRead);
        totalRead += bytesRead;
    }

    close(fd);

    std::vector<uint8_t> hash(SHA256_LENGTH);
    SHA256_Final(hash.data(), &sha256);

    return hash;
}

std::string PrplPartitionManager::slotToDevice(PartitionSlot slot) {
    auto partInfo = getPartition(slot);
    return partInfo.has_value() ? partInfo->device : "";
}

// ============================================================================
// PrplFlashManager Implementation
// ============================================================================

PrplFlashManager::PrplFlashManager(std::shared_ptr<PrplAmxbClient> amxbClient)
    : amxbClient_(amxbClient) {
}

PrplFlashManager::~PrplFlashManager() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [device, fd] : deviceHandles_) {
        if (fd >= 0) {
            close(fd);
        }
    }
}

PlatformResult PrplFlashManager::initialize() {
    // Check if MTD is available
    if (!fs::exists(PROC_MTD)) {
        return PlatformResult::ERROR_NOT_SUPPORTED;
    }

    return PlatformResult::SUCCESS;
}

bool PrplFlashManager::isMTDDevice(const std::string& device) {
    return device.find("/dev/mtd") == 0;
}

PlatformResult PrplFlashManager::read(const std::string& device, uint64_t offset,
                                      uint64_t size, std::vector<uint8_t>& buffer) {
    if (!isMTDDevice(device)) {
        return PlatformResult::ERROR_NOT_SUPPORTED;
    }

    int fd = open(device.c_str(), O_RDONLY);
    if (fd < 0) {
        return PlatformResult::ERROR_IO_FAILURE;
    }

    if (lseek(fd, offset, SEEK_SET) != static_cast<off_t>(offset)) {
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

PlatformResult PrplFlashManager::write(const std::string& device, uint64_t offset,
                                       const std::vector<uint8_t>& data,
                                       FlashProgressCallback progress) {
    if (!isMTDDevice(device)) {
        return PlatformResult::ERROR_NOT_SUPPORTED;
    }

    int fd = open(device.c_str(), O_RDWR);
    if (fd < 0) {
        return PlatformResult::ERROR_PERMISSION_DENIED;
    }

    if (lseek(fd, offset, SEEK_SET) != static_cast<off_t>(offset)) {
        close(fd);
        return PlatformResult::ERROR_IO_FAILURE;
    }

    size_t totalWritten = 0;
    while (totalWritten < data.size()) {
        size_t chunkSize = std::min(FLASH_CHUNK_SIZE, data.size() - totalWritten);
        ssize_t written = ::write(fd, data.data() + totalWritten, chunkSize);

        if (written < 0) {
            close(fd);
            return PlatformResult::ERROR_IO_FAILURE;
        }

        totalWritten += written;

        if (progress) {
            progress(totalWritten, data.size());
        }
    }

    close(fd);
    return PlatformResult::SUCCESS;
}

PlatformResult PrplFlashManager::erase(const std::string& device, uint64_t offset, uint64_t size) {
    if (!isMTDDevice(device)) {
        return PlatformResult::ERROR_NOT_SUPPORTED;
    }

    int fd = open(device.c_str(), O_RDWR);
    if (fd < 0) {
        return PlatformResult::ERROR_PERMISSION_DENIED;
    }

    mtd_info_t mtdInfo;
    if (ioctl(fd, MEMGETINFO, &mtdInfo) != 0) {
        close(fd);
        return PlatformResult::ERROR_IO_FAILURE;
    }

    erase_info_t eraseInfo;
    eraseInfo.start = offset;
    eraseInfo.length = size;

    if (ioctl(fd, MEMERASE, &eraseInfo) != 0) {
        close(fd);
        return PlatformResult::ERROR_IO_FAILURE;
    }

    close(fd);
    return PlatformResult::SUCCESS;
}

PlatformResult PrplFlashManager::getDeviceInfo(const std::string& device,
                                               uint64_t& eraseSize, uint64_t& totalSize) {
    if (!isMTDDevice(device)) {
        return PlatformResult::ERROR_NOT_SUPPORTED;
    }

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

PlatformResult PrplFlashManager::lock(const std::string& device, uint64_t offset, uint64_t size) {
    if (!isMTDDevice(device)) {
        return PlatformResult::ERROR_NOT_SUPPORTED;
    }

    int fd = open(device.c_str(), O_RDWR);
    if (fd < 0) {
        return PlatformResult::ERROR_PERMISSION_DENIED;
    }

    erase_info_t lockInfo;
    lockInfo.start = offset;
    lockInfo.length = size;

    if (ioctl(fd, MEMLOCK, &lockInfo) != 0) {
        close(fd);
        return PlatformResult::ERROR_IO_FAILURE;
    }

    close(fd);
    return PlatformResult::SUCCESS;
}

PlatformResult PrplFlashManager::unlock(const std::string& device, uint64_t offset, uint64_t size) {
    if (!isMTDDevice(device)) {
        return PlatformResult::ERROR_NOT_SUPPORTED;
    }

    int fd = open(device.c_str(), O_RDWR);
    if (fd < 0) {
        return PlatformResult::ERROR_PERMISSION_DENIED;
    }

    erase_info_t unlockInfo;
    unlockInfo.start = offset;
    unlockInfo.length = size;

    if (ioctl(fd, MEMUNLOCK, &unlockInfo) != 0) {
        close(fd);
        return PlatformResult::ERROR_IO_FAILURE;
    }

    close(fd);
    return PlatformResult::SUCCESS;
}

// ============================================================================
// PrplBootloaderControl Implementation
// ============================================================================

PrplBootloaderControl::PrplBootloaderControl(std::shared_ptr<PrplAmxbClient> amxbClient)
    : amxbClient_(amxbClient), bootloaderType_("u-boot") {
}

PrplBootloaderControl::~PrplBootloaderControl() = default;

PlatformResult PrplBootloaderControl::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    return loadBootEnv() ? PlatformResult::SUCCESS : PlatformResult::ERROR_IO_FAILURE;
}

bool PrplBootloaderControl::loadBootEnv() {
    // Try to read from Ambiorix first
    if (amxbClient_ && amxbClient_->isConnected()) {
        auto result = amxbClient_->get("Device.DeviceInfo.BootEnvironment");
        if (!result.empty()) {
            bootEnv_.clear();
            for (const auto& [key, value] : result) {
                bootEnv_[key] = value;
            }
            return true;
        }
    }

    // Fallback: read from fw_printenv
    std::string output;
    if (execCommand("fw_printenv", output) == 0) {
        std::istringstream iss(output);
        std::string line;
        while (std::getline(iss, line)) {
            auto pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                bootEnv_[key] = value;
            }
        }
        return true;
    }

    return false;
}

bool PrplBootloaderControl::writeBootEnv() {
    // Write through Ambiorix
    if (amxbClient_ && amxbClient_->isConnected()) {
        for (const auto& [key, value] : bootEnv_) {
            std::map<std::string, std::string> params;
            params[key] = value;
            amxbClient_->set("Device.DeviceInfo.BootEnvironment", params);
        }
        return true;
    }

    return false;
}

std::string PrplBootloaderControl::getBootloaderType() {
    return bootloaderType_;
}

std::optional<std::string> PrplBootloaderControl::getEnv(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = bootEnv_.find(name);
    if (it != bootEnv_.end()) {
        return it->second;
    }

    return std::nullopt;
}

PlatformResult PrplBootloaderControl::setEnv(const std::string& name, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    bootEnv_[name] = value;
    return PlatformResult::SUCCESS;
}

std::map<std::string, std::string> PrplBootloaderControl::getAllEnv() {
    std::lock_guard<std::mutex> lock(mutex_);
    return bootEnv_;
}

PlatformResult PrplBootloaderControl::saveEnv() {
    std::lock_guard<std::mutex> lock(mutex_);
    return writeBootEnv() ? PlatformResult::SUCCESS : PlatformResult::ERROR_IO_FAILURE;
}

PlatformResult PrplBootloaderControl::setBootSlot(PartitionSlot slot) {
    std::string slotName = (slot == PartitionSlot::SLOT_A) ? "a" : "b";
    return setEnv("boot_part", slotName);
}

PartitionSlot PrplBootloaderControl::getBootSlot() {
    auto bootPart = getEnv("boot_part");
    if (bootPart.has_value()) {
        return (bootPart.value() == "a") ? PartitionSlot::SLOT_A : PartitionSlot::SLOT_B;
    }
    return PartitionSlot::SLOT_A;
}

PlatformResult PrplBootloaderControl::setBootAttempts(PartitionSlot slot, int attempts) {
    std::string key = "boot_attempts_" + std::string((slot == PartitionSlot::SLOT_A) ? "a" : "b");
    return setEnv(key, std::to_string(attempts));
}

int PrplBootloaderControl::getBootAttempts(PartitionSlot slot) {
    std::string key = "boot_attempts_" + std::string((slot == PartitionSlot::SLOT_A) ? "a" : "b");
    auto attempts = getEnv(key);
    if (attempts.has_value()) {
        return std::stoi(attempts.value());
    }
    return 3; // Default attempts
}

PlatformResult PrplBootloaderControl::markBootSuccessful() {
    setEnv("boot_success", "1");
    return saveEnv();
}

PlatformResult PrplBootloaderControl::requestRecoveryBoot() {
    setEnv("boot_recovery", "1");
    return saveEnv();
}

// ============================================================================
// PrplSystemManager Implementation
// ============================================================================

PrplSystemManager::PrplSystemManager(std::shared_ptr<PrplAmxbClient> amxbClient,
                                     std::shared_ptr<PrplDataModel> dataModel)
    : amxbClient_(amxbClient), dataModel_(dataModel) {
    detectPrplOS();
}

PrplSystemManager::~PrplSystemManager() = default;

bool PrplSystemManager::detectPrplOS() {
    std::string content;

    // Check for prpl-specific files
    if (readFile(PRPL_OS_RELEASE, content)) {
        prplInfo_.isPresent = true;

        // Parse version and variant
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.find("VERSION=") == 0) {
                prplInfo_.version = line.substr(8);
            } else if (line.find("VARIANT=") == 0) {
                prplInfo_.variant = line.substr(8);
            }
        }
    }

    // Check for Ambiorix
    prplInfo_.hasAmbiorix = (amxbClient_ && amxbClient_->isConnected());

    // Check for USP agent
    std::string output;
    prplInfo_.hasUSPAgent = (execCommand("pidof usp-agent", output) == 0 && !output.empty());

    // Check for EasyMesh
    prplInfo_.hasEasyMesh = (execCommand("pidof ieee1905d", output) == 0 && !output.empty());

    if (prplInfo_.isPresent && dataModel_) {
        queryTR181SystemInfo();
    }

    return prplInfo_.isPresent;
}

void PrplSystemManager::queryTR181SystemInfo() {
    if (!dataModel_) return;

    // Query Device.DeviceInfo
    auto deviceInfo = dataModel_->getDeviceInfo();
    if (deviceInfo.count("ModelName")) {
        prplInfo_.boardName = deviceInfo["ModelName"];
    }
    if (deviceInfo.count("HardwareVersion")) {
        prplInfo_.chipsetModel = deviceInfo["HardwareVersion"];
    }
}

SystemInfo PrplSystemManager::getSystemInfo() {
    std::lock_guard<std::mutex> lock(mutex_);

    SystemInfo info;
    info.platform = "prplOS";

    // Get from TR-181 if available
    if (dataModel_) {
        auto deviceInfo = dataModel_->getDeviceInfo();

        if (deviceInfo.count("SoftwareVersion")) {
            info.kernelVersion = deviceInfo["SoftwareVersion"];
        }
        if (deviceInfo.count("ModelName")) {
            info.boardName = deviceInfo["ModelName"];
        }
    }

    // System memory info
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        info.totalRam = si.totalram * si.mem_unit;
        info.freeRam = si.freeram * si.mem_unit;
    }

    // Architecture
    std::string output;
    if (execCommand("uname -m", output) == 0) {
        info.architecture = output;
        info.architecture.erase(info.architecture.find_last_not_of("\n\r") + 1);
    }

    // Kernel version
    if (execCommand("uname -r", output) == 0) {
        if (info.kernelVersion.empty()) {
            info.kernelVersion = output;
            info.kernelVersion.erase(info.kernelVersion.find_last_not_of("\n\r") + 1);
        }
    }

    info.bootloaderType = "u-boot";
    info.bootloaderVersion = prplInfo_.version;

    return info;
}

PlatformResult PrplSystemManager::reboot(int delay) {
    if (delay > 0) {
        std::thread([delay]() {
            std::this_thread::sleep_for(std::chrono::seconds(delay));
            sync();
            ::reboot(RB_AUTOBOOT);
        }).detach();
    } else {
        sync();
        ::reboot(RB_AUTOBOOT);
    }

    return PlatformResult::SUCCESS;
}

PlatformResult PrplSystemManager::shutdown(int delay) {
    if (delay > 0) {
        std::thread([delay]() {
            std::this_thread::sleep_for(std::chrono::seconds(delay));
            sync();
            ::reboot(RB_POWER_OFF);
        }).detach();
    } else {
        sync();
        ::reboot(RB_POWER_OFF);
    }

    return PlatformResult::SUCCESS;
}

PlatformResult PrplSystemManager::enterRecoveryMode() {
    // Set recovery boot flag and reboot
    if (amxbClient_ && amxbClient_->isConnected()) {
        std::map<std::string, std::string> params;
        params["RecoveryMode"] = "true";
        amxbClient_->set("Device.DeviceInfo", params);
    }

    return reboot(1);
}

bool PrplSystemManager::isRecoveryMode() {
    std::string cmdline;
    if (readFile(PROC_CMDLINE, cmdline)) {
        return cmdline.find("recovery") != std::string::npos;
    }
    return false;
}

PlatformResult PrplSystemManager::syncFilesystems() {
    sync();
    return PlatformResult::SUCCESS;
}

std::string PrplSystemManager::getFirmwareVersion() {
    if (dataModel_) {
        auto deviceInfo = dataModel_->getDeviceInfo();
        if (deviceInfo.count("SoftwareVersion")) {
            return deviceInfo["SoftwareVersion"];
        }
    }

    return prplInfo_.version;
}

int PrplSystemManager::executeCommand(const std::string& command, std::string& output) {
    return execCommand(command, output);
}

PrplOSInfo PrplSystemManager::getPrplOSInfo() {
    std::lock_guard<std::mutex> lock(mutex_);
    return prplInfo_;
}

std::vector<NetworkInterface> PrplSystemManager::getNetworkInterfaces() {
    std::vector<NetworkInterface> interfaces;

    if (!dataModel_) {
        return interfaces;
    }

    // Query TR-181 Device.IP.Interface
    auto ipInterfaces = dataModel_->getIPInterfaces();

    for (const auto& [path, data] : ipInterfaces) {
        NetworkInterface iface;

        if (data.count("Name")) iface.name = data.at("Name");
        if (data.count("Type")) iface.type = data.at("Type");
        if (data.count("Status")) iface.status = data.at("Status");
        if (data.count("Enable")) iface.enabled = (data.at("Enable") == "true");

        interfaces.push_back(iface);
    }

    return interfaces;
}

std::vector<WiFiStation> PrplSystemManager::getWiFiStations() {
    std::vector<WiFiStation> stations;

    if (!dataModel_) {
        return stations;
    }

    // Query TR-181 Device.WiFi.AccessPoint.{i}.AssociatedDevice
    auto wifiStations = dataModel_->getWiFiStations();

    for (const auto& [path, data] : wifiStations) {
        WiFiStation station;

        if (data.count("MACAddress")) station.macAddress = data.at("MACAddress");
        if (data.count("IPAddress")) station.ipAddress = data.at("IPAddress");
        if (data.count("SignalStrength")) station.signalStrength = std::stoi(data.at("SignalStrength"));
        if (data.count("Active")) station.isActive = (data.at("Active") == "true");

        stations.push_back(station);
    }

    return stations;
}

std::optional<ServiceStatus> PrplSystemManager::getServiceStatus(const std::string& serviceName) {
    std::string output;
    if (execCommand("procd list | grep " + serviceName, output) == 0 && !output.empty()) {
        ServiceStatus status;
        status.name = serviceName;
        status.state = "running";
        status.enabled = true;
        return status;
    }

    return std::nullopt;
}

PlatformResult PrplSystemManager::restartService(const std::string& serviceName) {
    std::string output;
    int result = execCommand("/etc/init.d/" + serviceName + " restart", output);
    return (result == 0) ? PlatformResult::SUCCESS : PlatformResult::ERROR_UNKNOWN;
}

// ============================================================================
// PrplPlatform Implementation
// ============================================================================

PrplOSInfo PrplPlatform::detectPrplOS() {
    PrplOSInfo info;

    // Check for prpl-specific indicators
    std::string content;
    if (readFile(PRPL_OS_RELEASE, content)) {
        info.isPresent = true;

        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.find("VERSION=") == 0) {
                info.version = line.substr(8);
            } else if (line.find("VARIANT=") == 0) {
                info.variant = line.substr(8);
            }
        }
    } else if (readFile(OS_RELEASE, content)) {
        // Check if OS_RELEASE mentions prpl
        if (content.find("prpl") != std::string::npos ||
            content.find("PRPL") != std::string::npos) {
            info.isPresent = true;
        }
    }

    // Check for Ambiorix libraries
    info.hasAmbiorix = fs::exists("/usr/lib/libamxb.so") ||
                       fs::exists("/usr/lib/libamxd.so");

    // Check for USP agent
    std::string output;
    info.hasUSPAgent = (execCommand("which usp-agent", output) == 0 && !output.empty());

    // Check for EasyMesh daemon
    info.hasEasyMesh = (execCommand("which ieee1905d", output) == 0 && !output.empty());

    return info;
}

bool PrplPlatform::initialize(const std::string& busUri) {
    std::lock_guard<std::mutex> lock(initMutex_);

    if (initialized_) {
        return true;
    }

    // Detect prplOS
    auto osInfo = detectPrplOS();
    if (!osInfo.isPresent) {
        return false;
    }

    // Initialize Ambiorix client
    std::string uri = busUri.empty() ? DEFAULT_BUS_URI : busUri;
    amxbClient_ = std::make_shared<PrplAmxbClient>();

    if (!amxbClient_->connect(uri)) {
        return false;
    }

    // Initialize data model
    dataModel_ = std::make_shared<PrplDataModel>(amxbClient_);
    if (!dataModel_->initialize()) {
        return false;
    }

    // Initialize EasyMesh if available
    if (osInfo.hasEasyMesh) {
        easyMesh_ = std::make_shared<PrplEasyMesh>(amxbClient_);
        easyMesh_->initialize();
    }

    // Create platform managers
    partitionManager_ = std::make_shared<PrplPartitionManager>(amxbClient_);
    partitionManager_->initialize();

    flashManager_ = std::make_shared<PrplFlashManager>(amxbClient_);
    flashManager_->initialize();

    bootloaderControl_ = std::make_shared<PrplBootloaderControl>(amxbClient_);
    bootloaderControl_->initialize();

    systemManager_ = std::make_shared<PrplSystemManager>(amxbClient_, dataModel_);

    initialized_ = true;
    return true;
}

void PrplPlatform::shutdown() {
    std::lock_guard<std::mutex> lock(initMutex_);

    if (!initialized_) {
        return;
    }

    systemManager_.reset();
    bootloaderControl_.reset();
    flashManager_.reset();
    partitionManager_.reset();
    easyMesh_.reset();
    dataModel_.reset();

    if (amxbClient_) {
        amxbClient_->disconnect();
        amxbClient_.reset();
    }

    initialized_ = false;
}

bool PrplPlatform::isInitialized() {
    std::lock_guard<std::mutex> lock(initMutex_);
    return initialized_;
}

std::shared_ptr<PrplAmxbClient> PrplPlatform::getAmxbClient() {
    return amxbClient_;
}

std::shared_ptr<PrplDataModel> PrplPlatform::getDataModel() {
    return dataModel_;
}

std::shared_ptr<PrplEasyMesh> PrplPlatform::getEasyMesh() {
    return easyMesh_;
}

bool PrplPlatform::registerWithFactory() {
    // This would register prpl implementations with PlatformFactory
    // Implementation depends on how PlatformFactory is structured
    return true;
}

} // namespace prpl
} // namespace platform
} // namespace lwm2m
