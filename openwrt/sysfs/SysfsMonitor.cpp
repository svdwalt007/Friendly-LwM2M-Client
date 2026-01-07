/*
 * SysfsMonitor.cpp
 *
 * OpenWRT sysfs access layer implementation
 */

#include "SysfsMonitor.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <sys/statvfs.h>
#include <unistd.h>

namespace wpp {

// Helper function to trim whitespace
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

// Static helper functions
std::string SysfsMonitor::readSysfsFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::string content;
    std::getline(file, content);
    file.close();

    return trim(content);
}

bool SysfsMonitor::writeSysfsFile(const std::string& path, const std::string& value) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    file << value;
    file.close();

    return file.good();
}

int SysfsMonitor::readSysfsInt(const std::string& path, int defaultValue) {
    std::string value = readSysfsFile(path);
    if (value.empty()) {
        return defaultValue;
    }

    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

uint64_t SysfsMonitor::readSysfsUint64(const std::string& path, uint64_t defaultValue) {
    std::string value = readSysfsFile(path);
    if (value.empty()) {
        return defaultValue;
    }

    try {
        return std::stoull(value);
    } catch (...) {
        return defaultValue;
    }
}

// Battery Information
bool SysfsMonitor::hasBattery(int batteryIndex) const {
    std::string batteryPath = "/sys/class/power_supply/battery" + std::to_string(batteryIndex);

    // Alternative paths for different systems
    if (access(batteryPath.c_str(), F_OK) != 0) {
        batteryPath = "/sys/class/power_supply/BAT" + std::to_string(batteryIndex);
    }

    return access(batteryPath.c_str(), F_OK) == 0;
}

SysfsMonitor::BatteryInfo SysfsMonitor::getBatteryInfo(int batteryIndex) const {
    BatteryInfo info = {0, 0, 0, 0, "Unknown", "Unknown"};

    std::string batteryPath = "/sys/class/power_supply/battery" + std::to_string(batteryIndex);
    if (access(batteryPath.c_str(), F_OK) != 0) {
        batteryPath = "/sys/class/power_supply/BAT" + std::to_string(batteryIndex);
    }

    if (access(batteryPath.c_str(), F_OK) != 0) {
        return info;
    }

    // Read battery level (capacity)
    info.level = readSysfsInt(batteryPath + "/capacity", 0);

    // Read voltage (in microvolts, convert to millivolts)
    int voltage_uv = readSysfsInt(batteryPath + "/voltage_now", 0);
    info.voltage = voltage_uv / 1000;

    // Read current (in microamps, convert to milliamps)
    int current_ua = readSysfsInt(batteryPath + "/current_now", 0);
    info.current = current_ua / 1000;

    // Read temperature (in 0.1°C)
    info.temperature = readSysfsInt(batteryPath + "/temp", 0);

    // Read status
    info.status = readSysfsFile(batteryPath + "/status");
    if (info.status.empty()) {
        info.status = "Unknown";
    }

    // Read health
    info.health = readSysfsFile(batteryPath + "/health");
    if (info.health.empty()) {
        info.health = "Good";
    }

    return info;
}

// Memory Information
std::map<std::string, uint64_t> SysfsMonitor::parseMeminfo() const {
    std::map<std::string, uint64_t> meminfo;
    std::ifstream file("/proc/meminfo");

    if (!file.is_open()) {
        return meminfo;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = trim(line.substr(0, colon));
            std::string value = trim(line.substr(colon + 1));

            // Remove " kB" suffix
            size_t kb = value.find(" kB");
            if (kb != std::string::npos) {
                value = value.substr(0, kb);
            }

            try {
                meminfo[key] = std::stoull(value);
            } catch (...) {
                // Skip invalid entries
            }
        }
    }

    return meminfo;
}

SysfsMonitor::MemoryInfo SysfsMonitor::getMemoryInfo() const {
    MemoryInfo info = {0, 0, 0, 0, 0};

    auto meminfo = parseMeminfo();

    info.totalKB = meminfo["MemTotal"];
    info.freeKB = meminfo["MemFree"];
    info.availableKB = meminfo["MemAvailable"];
    info.buffersKB = meminfo["Buffers"];
    info.cachedKB = meminfo["Cached"];

    // If MemAvailable is not present (older kernels), estimate it
    if (info.availableKB == 0) {
        info.availableKB = info.freeKB + info.buffersKB + info.cachedKB;
    }

    return info;
}

// CPU Information
std::map<std::string, std::string> SysfsMonitor::parseCpuinfo() const {
    std::map<std::string, std::string> cpuinfo;
    std::ifstream file("/proc/cpuinfo");

    if (!file.is_open()) {
        return cpuinfo;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = trim(line.substr(0, colon));
            std::string value = trim(line.substr(colon + 1));
            cpuinfo[key] = value;
        }
    }

    return cpuinfo;
}

double SysfsMonitor::parseCpuUsage() const {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        return 0.0;
    }

    std::string line;
    std::getline(file, line);

    // Parse: cpu  user nice system idle iowait irq softirq
    std::istringstream iss(line);
    std::string cpu;
    uint64_t user, nice, system, idle, iowait, irq, softirq;

    iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;

    uint64_t total = user + nice + system + idle + iowait + irq + softirq;
    uint64_t active = user + nice + system + irq + softirq;

    if (total == 0) {
        return 0.0;
    }

    return (static_cast<double>(active) / static_cast<double>(total)) * 100.0;
}

SysfsMonitor::CpuInfo SysfsMonitor::getCpuInfo() const {
    CpuInfo info = {"Unknown", 0, 0.0, 0.0, 0.0};

    auto cpuinfo = parseCpuinfo();

    // Get CPU model
    if (cpuinfo.find("model name") != cpuinfo.end()) {
        info.model = cpuinfo["model name"];
    } else if (cpuinfo.find("Processor") != cpuinfo.end()) {
        info.model = cpuinfo["Processor"];
    } else if (cpuinfo.find("Hardware") != cpuinfo.end()) {
        info.model = cpuinfo["Hardware"];
    }

    // Count CPU cores
    DIR* dir = opendir("/sys/devices/system/cpu");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (std::strncmp(entry->d_name, "cpu", 3) == 0 &&
                std::isdigit(entry->d_name[3])) {
                info.cores++;
            }
        }
        closedir(dir);
    }

    // Get current CPU frequency (from cpu0)
    int freq_khz = readSysfsInt("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", 0);
    info.frequency = static_cast<double>(freq_khz) / 1000.0; // Convert to MHz

    // Get CPU temperature
    info.temperature = getThermalZoneTemperature(0);

    // Get CPU usage
    info.usage = parseCpuUsage();

    return info;
}

// Network Information
std::map<std::string, SysfsMonitor::NetworkInfo> SysfsMonitor::parseNetDev() const {
    std::map<std::string, NetworkInfo> interfaces;
    std::ifstream file("/proc/net/dev");

    if (!file.is_open()) {
        return interfaces;
    }

    std::string line;
    // Skip header lines
    std::getline(file, line);
    std::getline(file, line);

    while (std::getline(file, line)) {
        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;

        std::string ifname = trim(line.substr(0, colon));
        std::string stats = line.substr(colon + 1);

        std::istringstream iss(stats);
        NetworkInfo info;
        info.name = ifname;

        iss >> info.rxBytes >> info.rxPackets;
        // Skip other RX fields
        for (int i = 0; i < 6; i++) { uint64_t dummy; iss >> dummy; }
        iss >> info.txBytes >> info.txPackets;

        interfaces[ifname] = info;
    }

    return interfaces;
}

SysfsMonitor::NetworkInfo SysfsMonitor::getWirelessInfo(const std::string& interfaceName) const {
    NetworkInfo info;
    info.name = interfaceName;
    info.signalStrength = -100; // Default to very weak

    // Read wireless info from /proc/net/wireless
    std::ifstream file("/proc/net/wireless");
    if (!file.is_open()) {
        return info;
    }

    std::string line;
    // Skip header lines
    std::getline(file, line);
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.find(interfaceName) != std::string::npos) {
            std::istringstream iss(line);
            std::string iface;
            int status, quality, signal, noise;

            iss >> iface >> status >> quality >> signal >> noise;
            info.signalStrength = signal;
            break;
        }
    }

    // Try to read SSID from sysfs (for some drivers)
    std::string ssidPath = "/sys/class/net/" + interfaceName + "/ssid";
    info.ssid = readSysfsFile(ssidPath);

    return info;
}

std::vector<std::string> SysfsMonitor::getNetworkInterfaces() const {
    std::vector<std::string> interfaces;

    DIR* dir = opendir("/sys/class/net");
    if (!dir) {
        return interfaces;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] != '.') {
            interfaces.push_back(entry->d_name);
        }
    }
    closedir(dir);

    return interfaces;
}

std::vector<SysfsMonitor::NetworkInfo> SysfsMonitor::getNetworkInfo(const std::string& interfaceName) const {
    std::vector<NetworkInfo> infoList;
    auto netdev = parseNetDev();

    std::vector<std::string> interfaces;
    if (interfaceName.empty()) {
        interfaces = getNetworkInterfaces();
    } else {
        interfaces.push_back(interfaceName);
    }

    for (const auto& iface : interfaces) {
        NetworkInfo info;

        // Get statistics from /proc/net/dev
        if (netdev.find(iface) != netdev.end()) {
            info = netdev[iface];
        } else {
            info.name = iface;
        }

        // Read MAC address
        std::string macPath = "/sys/class/net/" + iface + "/address";
        info.mac = readSysfsFile(macPath);

        // Check if interface is up
        std::string operstatePath = "/sys/class/net/" + iface + "/operstate";
        std::string operstate = readSysfsFile(operstatePath);
        info.isUp = (operstate == "up");

        // Get IP addresses (simplified - would need netlink for proper implementation)
        // This is a basic implementation using system commands
        // In production, use netlink socket or libubox

        // Check if wireless interface
        std::string wirelessPath = "/sys/class/net/" + iface + "/wireless";
        if (access(wirelessPath.c_str(), F_OK) == 0) {
            NetworkInfo winfo = getWirelessInfo(iface);
            info.signalStrength = winfo.signalStrength;
            info.ssid = winfo.ssid;
        }

        infoList.push_back(info);
    }

    return infoList;
}

// Storage Information
std::vector<SysfsMonitor::StorageInfo> SysfsMonitor::parseMounts() const {
    std::vector<StorageInfo> storages;
    std::ifstream file("/proc/mounts");

    if (!file.is_open()) {
        return storages;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string device, mountPoint, fsType;

        iss >> device >> mountPoint >> fsType;

        // Skip virtual filesystems
        if (fsType == "proc" || fsType == "sysfs" || fsType == "devpts" ||
            fsType == "tmpfs" || fsType == "devtmpfs" || fsType == "cgroup") {
            continue;
        }

        StorageInfo info = getFilesystemStats(mountPoint);
        info.device = device;
        info.mountPoint = mountPoint;

        storages.push_back(info);
    }

    return storages;
}

SysfsMonitor::StorageInfo SysfsMonitor::getFilesystemStats(const std::string& mountPoint) const {
    StorageInfo info = {"", mountPoint, 0, 0, 0};

    struct statvfs stat;
    if (statvfs(mountPoint.c_str(), &stat) != 0) {
        return info;
    }

    uint64_t blockSize = stat.f_frsize;
    info.totalKB = (stat.f_blocks * blockSize) / 1024;
    info.freeKB = (stat.f_bfree * blockSize) / 1024;
    info.usedKB = info.totalKB - info.freeKB;

    return info;
}

std::vector<SysfsMonitor::StorageInfo> SysfsMonitor::getStorageInfo(const std::string& mountPoint) const {
    if (mountPoint.empty()) {
        return parseMounts();
    } else {
        std::vector<StorageInfo> result;
        result.push_back(getFilesystemStats(mountPoint));
        return result;
    }
}

// System Information
SysfsMonitor::SystemInfo SysfsMonitor::getSystemInfo() const {
    SystemInfo info = {"", "", "", 0, 0.0, 0.0, 0.0};

    // Get hostname
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        info.hostname = hostname;
    }

    // Get kernel version
    info.kernel = readSysfsFile("/proc/sys/kernel/osrelease");

    // Get device model
    info.model = readSysfsFile("/proc/device-tree/model");
    if (info.model.empty()) {
        auto cpuinfo = parseCpuinfo();
        if (cpuinfo.find("Hardware") != cpuinfo.end()) {
            info.model = cpuinfo["Hardware"];
        }
    }

    // Get uptime
    std::ifstream uptimeFile("/proc/uptime");
    if (uptimeFile.is_open()) {
        double uptime;
        uptimeFile >> uptime;
        info.uptimeSeconds = static_cast<uint64_t>(uptime);
    }

    // Get load average
    std::ifstream loadavgFile("/proc/loadavg");
    if (loadavgFile.is_open()) {
        loadavgFile >> info.loadAverage1 >> info.loadAverage5 >> info.loadAverage15;
    }

    return info;
}

// LED Control
int SysfsMonitor::getLedState(const std::string& ledName) const {
    std::string ledPath = "/sys/class/leds/" + ledName + "/brightness";
    return readSysfsInt(ledPath, -1);
}

bool SysfsMonitor::setLedState(const std::string& ledName, int brightness) {
    if (brightness < 0 || brightness > 255) {
        return false;
    }

    std::string ledPath = "/sys/class/leds/" + ledName + "/brightness";
    return writeSysfsFile(ledPath, std::to_string(brightness));
}

// GPIO Control
int SysfsMonitor::getGpioValue(int gpioNumber) const {
    std::string gpioPath = "/sys/class/gpio/gpio" + std::to_string(gpioNumber) + "/value";
    return readSysfsInt(gpioPath, -1);
}

bool SysfsMonitor::setGpioValue(int gpioNumber, int value) {
    if (value != 0 && value != 1) {
        return false;
    }

    std::string gpioPath = "/sys/class/gpio/gpio" + std::to_string(gpioNumber) + "/value";
    return writeSysfsFile(gpioPath, std::to_string(value));
}

// Thermal Information
double SysfsMonitor::getThermalZoneTemperature(int zoneIndex) const {
    std::string tempPath = "/sys/class/thermal/thermal_zone" + std::to_string(zoneIndex) + "/temp";
    int temp_millidegrees = readSysfsInt(tempPath, -273150);

    return static_cast<double>(temp_millidegrees) / 1000.0;
}

} // namespace wpp
