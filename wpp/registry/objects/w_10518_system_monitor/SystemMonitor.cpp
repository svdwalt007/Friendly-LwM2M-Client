/*
 * SystemMonitor (Object 10518)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_10518_system_monitor/SystemMonitor.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"
#include "WppTaskQueue.h"

#include <fstream>
#include <sstream>
#include <string>
#include <dirent.h>
#include <cctype>

#define TAG "SystemMonitor"

namespace wpp {

// Helper function to read a file into a string
static std::string readFileToString(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        WPP_LOGW(TAG, "Failed to open file: %s", path.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

SystemMonitor::SystemMonitor(Object& object, INST_T instId)
    : Instance(object, instId)
    , _updateTaskId(WPP_ERR_TASK_ID)
    , _prevCpuUser(0)
    , _prevCpuNice(0)
    , _prevCpuSystem(0)
    , _prevCpuIdle(0)
    , _prevCpuIowait(0)
    , _prevCpuIrq(0)
    , _prevCpuSoftirq(0) {
    WPP_LOGD(TAG, "Creating SystemMonitor instance %d", instId);
}

SystemMonitor::~SystemMonitor() {
    WPP_LOGD(TAG, "Destroying SystemMonitor instance");
    WppTaskQueue::requestToRemoveTask(_updateTaskId);
}

Object& SystemMonitor::object(WppClient& client) {
    return client.registry().systemMonitor();
}

Instance* SystemMonitor::instance(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().systemMonitor().instance(instId);
    if (!inst) return NULL;
    return static_cast<SystemMonitor*>(inst);
}

Instance& SystemMonitor::createInst(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().systemMonitor().createInstance(instId);
    if (!inst) {
        WPP_LOGE(TAG, "Failed to create SystemMonitor instance");
        throw std::runtime_error("Failed to create SystemMonitor instance");
    }
    return *static_cast<SystemMonitor*>(inst);
}

bool SystemMonitor::remove(WppClient& client, INST_T instId) {
    return client.registry().systemMonitor().remove(instId);
}

void SystemMonitor::updateCpuUsage() {
    std::string content = readFileToString("/proc/stat");
    if (content.empty()) return;

    std::istringstream iss(content);
    std::string cpuLabel;
    unsigned long long user, nice, system, idle, iowait, irq, softirq;

    // Read the first line (aggregate CPU stats)
    if (iss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq) {
        if (cpuLabel == "cpu") {
            // Calculate deltas
            unsigned long long prevTotal = _prevCpuUser + _prevCpuNice + _prevCpuSystem +
                                          _prevCpuIdle + _prevCpuIowait + _prevCpuIrq + _prevCpuSoftirq;
            unsigned long long currTotal = user + nice + system + idle + iowait + irq + softirq;

            unsigned long long prevIdle = _prevCpuIdle + _prevCpuIowait;
            unsigned long long currIdle = idle + iowait;

            unsigned long long totalDiff = currTotal - prevTotal;
            unsigned long long idleDiff = currIdle - prevIdle;

            // Calculate CPU usage percentage
            int cpuUsage = 0;
            if (totalDiff > 0) {
                cpuUsage = (int)(100.0 * (totalDiff - idleDiff) / totalDiff);
            }

            // Update resource
            resource(CPU_USAGE_PERCENTAGE_0)->set<INT_T>(cpuUsage);
            notifyResChanged(CPU_USAGE_PERCENTAGE_0);

            // Store current values for next calculation
            _prevCpuUser = user;
            _prevCpuNice = nice;
            _prevCpuSystem = system;
            _prevCpuIdle = idle;
            _prevCpuIowait = iowait;
            _prevCpuIrq = irq;
            _prevCpuSoftirq = softirq;

            WPP_LOGD(TAG, "CPU Usage: %d%%", cpuUsage);
        }
    }
}

void SystemMonitor::updateCpuFrequency() {
    std::string content = readFileToString("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    if (content.empty()) return;

    try {
        // Frequency is in kHz, convert to MHz
        unsigned long freqKHz = std::stoul(content);
        int freqMHz = (int)(freqKHz / 1000);
        resource(CPU_FREQUENCY_1)->set<INT_T>(freqMHz);
        notifyResChanged(CPU_FREQUENCY_1);
        WPP_LOGD(TAG, "CPU Frequency: %d MHz", freqMHz);
    } catch (const std::exception& e) {
        WPP_LOGW(TAG, "Failed to parse CPU frequency: %s", e.what());
    }
}

void SystemMonitor::updateCpuTemperature() {
    std::string content = readFileToString("/sys/class/thermal/thermal_zone0/temp");
    if (content.empty()) return;

    try {
        // Temperature is in millidegrees Celsius
        int tempMilliC = std::stoi(content);
        float tempC = tempMilliC / 1000.0f;
        resource(CPU_TEMPERATURE_2)->set<FLOAT_T>(tempC);
        notifyResChanged(CPU_TEMPERATURE_2);
        WPP_LOGD(TAG, "CPU Temperature: %.1f C", tempC);
    } catch (const std::exception& e) {
        WPP_LOGW(TAG, "Failed to parse CPU temperature: %s", e.what());
    }
}

void SystemMonitor::updateMemoryInfo() {
    std::string content = readFileToString("/proc/meminfo");
    if (content.empty()) return;

    std::istringstream iss(content);
    std::string line;

    INT_T memTotal = 0, memFree = 0, memAvailable = 0;
    INT_T cached = 0, buffers = 0;
    INT_T swapTotal = 0, swapFree = 0;

    while (std::getline(iss, line)) {
        std::istringstream lineStream(line);
        std::string key;
        INT_T value;
        std::string unit;

        if (lineStream >> key >> value >> unit) {
            // Convert kB to bytes
            value *= 1024;

            if (key == "MemTotal:") memTotal = value;
            else if (key == "MemFree:") memFree = value;
            else if (key == "MemAvailable:") memAvailable = value;
            else if (key == "Cached:") cached = value;
            else if (key == "Buffers:") buffers = value;
            else if (key == "SwapTotal:") swapTotal = value;
            else if (key == "SwapFree:") swapFree = value;
        }
    }

    // Calculate used memory
    INT_T memUsed = memTotal - memFree - buffers - cached;
    INT_T swapUsed = swapTotal - swapFree;

    // Update resources
    resource(RAM_TOTAL_3)->set<INT_T>(memTotal);
    resource(RAM_USED_4)->set<INT_T>(memUsed);
    resource(RAM_FREE_5)->set<INT_T>(memFree);
    resource(RAM_CACHED_6)->set<INT_T>(cached);
    resource(RAM_BUFFERS_7)->set<INT_T>(buffers);
    resource(SWAP_TOTAL_8)->set<INT_T>(swapTotal);
    resource(SWAP_USED_9)->set<INT_T>(swapUsed);

    notifyResChanged(RAM_TOTAL_3);
    notifyResChanged(RAM_USED_4);
    notifyResChanged(RAM_FREE_5);
    notifyResChanged(RAM_CACHED_6);
    notifyResChanged(RAM_BUFFERS_7);
    notifyResChanged(SWAP_TOTAL_8);
    notifyResChanged(SWAP_USED_9);

    WPP_LOGD(TAG, "Memory - Total: %lld MB, Used: %lld MB, Free: %lld MB",
             memTotal/1024/1024, memUsed/1024/1024, memFree/1024/1024);
}

void SystemMonitor::updateLoadAverage() {
    std::string content = readFileToString("/proc/loadavg");
    if (content.empty()) return;

    std::istringstream iss(content);
    float load1, load5, load15;

    if (iss >> load1 >> load5 >> load15) {
        resource(LOAD_AVERAGE_1MIN_10)->set<FLOAT_T>(load1);
        resource(LOAD_AVERAGE_5MIN_11)->set<FLOAT_T>(load5);
        resource(LOAD_AVERAGE_15MIN_12)->set<FLOAT_T>(load15);

        notifyResChanged(LOAD_AVERAGE_1MIN_10);
        notifyResChanged(LOAD_AVERAGE_5MIN_11);
        notifyResChanged(LOAD_AVERAGE_15MIN_12);

        WPP_LOGD(TAG, "Load Average: %.2f, %.2f, %.2f", load1, load5, load15);
    }
}

void SystemMonitor::updateUptime() {
    std::string content = readFileToString("/proc/uptime");
    if (content.empty()) return;

    std::istringstream iss(content);
    double uptime;

    if (iss >> uptime) {
        resource(UPTIME_13)->set<INT_T>((INT_T)uptime);
        notifyResChanged(UPTIME_13);
        WPP_LOGD(TAG, "Uptime: %d seconds", (int)uptime);
    }
}

void SystemMonitor::updateProcessCount() {
    DIR* dir = opendir("/proc");
    if (!dir) {
        WPP_LOGW(TAG, "Failed to open /proc directory");
        return;
    }

    int processCount = 0;
    struct dirent* entry;

    while ((entry = readdir(dir)) != nullptr) {
        // Check if directory name is numeric (PID)
        const char* name = entry->d_name;
        bool isNumeric = true;

        for (int i = 0; name[i] != '\0'; i++) {
            if (!std::isdigit(name[i])) {
                isNumeric = false;
                break;
            }
        }

        if (isNumeric && name[0] != '\0') {
            processCount++;
        }
    }

    closedir(dir);

    resource(PROCESS_COUNT_14)->set<INT_T>(processCount);
    notifyResChanged(PROCESS_COUNT_14);
    WPP_LOGD(TAG, "Process Count: %d", processCount);
}

void SystemMonitor::updateAllMetrics() {
    updateCpuUsage();
    updateCpuFrequency();
    updateCpuTemperature();
    updateMemoryInfo();
    updateLoadAverage();
    updateUptime();
    updateProcessCount();
}

bool SystemMonitor::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing resources");

    std::vector<Resource> resources = {
        {CPU_USAGE_PERCENTAGE_0, ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {CPU_FREQUENCY_1,        ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {CPU_TEMPERATURE_2,      ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::FLOAT},
        {RAM_TOTAL_3,            ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {RAM_USED_4,             ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {RAM_FREE_5,             ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {RAM_CACHED_6,           ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {RAM_BUFFERS_7,          ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {SWAP_TOTAL_8,           ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {SWAP_USED_9,            ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
        {LOAD_AVERAGE_1MIN_10,   ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {LOAD_AVERAGE_5MIN_11,   ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {LOAD_AVERAGE_15MIN_12,  ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::FLOAT},
        {UPTIME_13,              ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {PROCESS_COUNT_14,       ItemOp(ItemOp::READ), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::INT},
    };

    setupResources(std::move(resources));

    // Set initial default values
    resource(CPU_USAGE_PERCENTAGE_0)->set<INT_T>(0);
    resource(CPU_FREQUENCY_1)->set<INT_T>(0);
    resource(CPU_TEMPERATURE_2)->set<FLOAT_T>(0.0);
    resource(RAM_TOTAL_3)->set<INT_T>(0);
    resource(RAM_USED_4)->set<INT_T>(0);
    resource(RAM_FREE_5)->set<INT_T>(0);
    resource(RAM_CACHED_6)->set<INT_T>(0);
    resource(RAM_BUFFERS_7)->set<INT_T>(0);
    resource(SWAP_TOTAL_8)->set<INT_T>(0);
    resource(SWAP_USED_9)->set<INT_T>(0);
    resource(LOAD_AVERAGE_1MIN_10)->set<FLOAT_T>(0.0);
    resource(LOAD_AVERAGE_5MIN_11)->set<FLOAT_T>(0.0);
    resource(LOAD_AVERAGE_15MIN_12)->set<FLOAT_T>(0.0);
    resource(UPTIME_13)->set<INT_T>(0);
    resource(PROCESS_COUNT_14)->set<INT_T>(0);

    // Set validators
    resource(CPU_USAGE_PERCENTAGE_0)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= 0 && value <= 100;
        }
    );

    resource(CPU_FREQUENCY_1)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= 0 && value <= 5000; // 0 MHz to 5 GHz
        }
    );

    resource(CPU_TEMPERATURE_2)->setDataVerifier(
        (VERIFY_FLOAT_T)[](const FLOAT_T& value) {
            return value >= -40.0 && value <= 125.0; // Reasonable temperature range
        }
    );

    // Perform initial update to populate values from /proc filesystem
    updateAllMetrics();

    // Set up periodic updates every 5 seconds
    _updateTaskId = WppTaskQueue::addTask(5, [this](WppClient& client, void* ctx) -> bool {
        updateAllMetrics();
        return false; // Return false to keep task running
    });

    if (_updateTaskId == WPP_ERR_TASK_ID) {
        WPP_LOGE(TAG, "Failed to create periodic update task");
        return false;
    }

    WPP_LOGD(TAG, "SystemMonitor initialized with periodic updates (task ID: %u)", _updateTaskId);
    return true;
}

bool SystemMonitor::validate(ID_T resId, const void *data, size_t size) {
    return true;
}

} // namespace wpp
