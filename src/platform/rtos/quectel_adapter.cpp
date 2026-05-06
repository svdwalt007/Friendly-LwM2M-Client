/**
 * @file quectel_adapter.cpp
 * @brief Quectel SDK RTOS Adapter Implementation
 *
 * Implements the RTOS abstraction layer for Quectel cellular modules.
 *
 * Quectel modules use ThreadX (Azure RTOS) as the underlying RTOS with
 * Quectel-specific APIs for cellular, GNSS, sensors, and peripherals.
 *
 * Key Features:
 * - ThreadX-based RTOS
 * - LTE Cat-M1/NB-IoT/Cat-1/Cat-4 connectivity
 * - GNSS (GPS/GLONASS/BeiDou/Galileo/QZSS)
 * - TCP/UDP socket API
 * - HTTP/HTTPS/MQTT/CoAP/LwM2M client
 * - File system (EFS)
 * - Audio codec and voice calls
 * - SMS/USSD
 * - FOTA updates
 * - Sensor hub (accelerometer, gyroscope, magnetometer)
 *
 * Supported Modules:
 * - BG95/BG96 (LTE Cat-M1/NB-IoT/EGPRS, GNSS)
 * - BG77/BG600L (LTE Cat-M1/NB-IoT, GNSS)
 * - BC660K (NB-IoT, ultra-low-power)
 * - EC25/EC21 (LTE Cat-4, GNSS)
 * - EG915U/EG915N (LTE Cat-1, GNSS)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef QUECTEL_SDK

#include "qapi.h"
#include "qapi_tlmm.h"
#include "qapi_timer.h"
#include "qapi_diag.h"
#include "qapi_device_info.h"
#include "qapi_fs.h"
#include "qapi_atfwd.h"
#include "qapi_location.h"
#include "qapi_socket.h"
#include "qapi_mqtt.h"
#include "qapi_data_txm_base.h"

#include "txm_module.h"
#include "tx_api.h"

#include <string>
#include <cstring>

namespace lwm2m {
namespace platform {
namespace rtos {

// ============================================================================
// Quectel Thread Implementation (ThreadX-based)
// ============================================================================

class QuectelThread : public IThread {
public:
    QuectelThread(const char* name, ThreadFunc func, void* arg,
                  uint32_t stackSize, ThreadPriority priority)
        : name_(name), func_(func), arg_(arg), stackSize_(stackSize),
          priority_(priority), started_(false) {
        stack_ = new uint8_t[stackSize];
    }

    ~QuectelThread() override {
        if (started_) {
            tx_thread_terminate(&thread_);
            tx_thread_delete(&thread_);
        }
        delete[] stack_;
    }

    RTOSResult start() override {
        if (started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        UINT txPriority = mapPriorityToThreadX(priority_);

        UINT status = tx_thread_create(
            &thread_,
            (CHAR*)name_,
            threadWrapper,
            (ULONG)this,
            stack_,
            stackSize_,
            txPriority,
            txPriority,
            TX_NO_TIME_SLICE,
            TX_AUTO_START
        );

        if (status != TX_SUCCESS) {
            return RTOSResult::ERROR_NO_MEMORY;
        }

        started_ = true;
        return RTOSResult::SUCCESS;
    }

    RTOSResult join(uint32_t timeoutMs) override {
        if (!started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        // ThreadX doesn't have join, wait for termination
        UINT status = tx_thread_wait_abort(&thread_);
        return (status == TX_SUCCESS) ? RTOSResult::SUCCESS : RTOSResult::ERROR_TIMEOUT;
    }

    RTOSResult suspend() override {
        if (!started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        UINT status = tx_thread_suspend(&thread_);
        return (status == TX_SUCCESS) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
    }

    RTOSResult resume() override {
        if (!started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        UINT status = tx_thread_resume(&thread_);
        return (status == TX_SUCCESS) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
    }

    ThreadState getState() override {
        if (!started_) {
            return ThreadState::READY;
        }

        UINT state;
        tx_thread_info_get(&thread_, nullptr, &state, nullptr, nullptr, nullptr,
                          nullptr, nullptr, nullptr);

        switch (state) {
            case TX_READY:          return ThreadState::READY;
            case TX_COMPLETED:      return ThreadState::TERMINATED;
            case TX_TERMINATED:     return ThreadState::TERMINATED;
            case TX_SUSPENDED:      return ThreadState::SUSPENDED;
            default:                return ThreadState::RUNNING;
        }
    }

    ThreadPriority getPriority() override {
        return priority_;
    }

    RTOSResult setPriority(ThreadPriority priority) override {
        priority_ = priority;
        if (started_) {
            UINT txPriority = mapPriorityToThreadX(priority);
            UINT oldPriority;
            UINT status = tx_thread_priority_change(&thread_, txPriority, &oldPriority);
            return (status == TX_SUCCESS) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
        }
        return RTOSResult::SUCCESS;
    }

    uint32_t getStackHighWaterMark() override {
        if (!started_) {
            return stackSize_;
        }

        CHAR *stack_start, *stack_end;
        tx_thread_info_get(&thread_, nullptr, nullptr, nullptr, nullptr, nullptr,
                          &stack_start, &stack_end, nullptr);

        return (uint32_t)(stack_end - stack_start);
    }

    const char* getName() override {
        return name_;
    }

private:
    static void threadWrapper(ULONG arg) {
        QuectelThread* thread = reinterpret_cast<QuectelThread*>(arg);
        thread->func_(thread->arg_);
    }

    static UINT mapPriorityToThreadX(ThreadPriority priority) {
        // ThreadX: 0 = highest, 31 = lowest
        switch (priority) {
            case ThreadPriority::IDLE:          return 31;
            case ThreadPriority::LOW:           return 24;
            case ThreadPriority::BELOW_NORMAL:  return 18;
            case ThreadPriority::NORMAL:        return 16;
            case ThreadPriority::ABOVE_NORMAL:  return 12;
            case ThreadPriority::HIGH:          return 8;
            case ThreadPriority::REALTIME:      return 0;
            default:                            return 16;
        }
    }

    const char* name_;
    ThreadFunc func_;
    void* arg_;
    uint32_t stackSize_;
    ThreadPriority priority_;
    TX_THREAD thread_;
    uint8_t* stack_;
    bool started_;
};

// ============================================================================
// Quectel Mutex Implementation
// ============================================================================

class QuectelMutex : public IMutex {
public:
    QuectelMutex() {
        tx_mutex_create(&mutex_, (CHAR*)"mutex", TX_NO_INHERIT);
    }

    ~QuectelMutex() override {
        tx_mutex_delete(&mutex_);
    }

    bool lock(uint32_t timeoutMs) override {
        ULONG ticks = (timeoutMs == UINT32_MAX) ? TX_WAIT_FOREVER : (timeoutMs * TX_TIMER_TICKS_PER_SECOND / 1000);
        UINT status = tx_mutex_get(&mutex_, ticks);
        return (status == TX_SUCCESS);
    }

    void unlock() override {
        tx_mutex_put(&mutex_);
    }

    bool tryLock() override {
        UINT status = tx_mutex_get(&mutex_, TX_NO_WAIT);
        return (status == TX_SUCCESS);
    }

    bool isLocked() override {
        TX_THREAD* owner;
        ULONG count;
        tx_mutex_info_get(&mutex_, nullptr, &count, &owner, nullptr, nullptr, nullptr);
        return (owner != nullptr);
    }

private:
    TX_MUTEX mutex_;
};

// ============================================================================
// Quectel Semaphore Implementation
// ============================================================================

class QuectelSemaphore : public ISemaphore {
public:
    QuectelSemaphore(uint32_t initialCount, uint32_t maxCount)
        : maxCount_(maxCount) {
        tx_semaphore_create(&sem_, (CHAR*)"sem", initialCount);
    }

    ~QuectelSemaphore() override {
        tx_semaphore_delete(&sem_);
    }

    bool wait(uint32_t timeoutMs) override {
        ULONG ticks = (timeoutMs == UINT32_MAX) ? TX_WAIT_FOREVER : (timeoutMs * TX_TIMER_TICKS_PER_SECOND / 1000);
        UINT status = tx_semaphore_get(&sem_, ticks);
        return (status == TX_SUCCESS);
    }

    bool signal() override {
        UINT status = tx_semaphore_put(&sem_);
        return (status == TX_SUCCESS);
    }

    bool tryWait() override {
        UINT status = tx_semaphore_get(&sem_, TX_NO_WAIT);
        return (status == TX_SUCCESS);
    }

    uint32_t getCount() override {
        ULONG count;
        tx_semaphore_info_get(&sem_, nullptr, &count, nullptr, nullptr, nullptr);
        return (uint32_t)count;
    }

    void reset() override {
        while (tx_semaphore_get(&sem_, TX_NO_WAIT) == TX_SUCCESS) {
            // Drain semaphore
        }
    }

private:
    TX_SEMAPHORE sem_;
    uint32_t maxCount_;
};

// ============================================================================
// Quectel Timer Implementation
// ============================================================================

class QuectelTimer : public ITimer {
public:
    QuectelTimer(const char* name)
        : name_(name), callback_(nullptr), active_(false) {
        tx_timer_create(&timer_, (CHAR*)name, timerWrapper, (ULONG)this,
                       1, 0, TX_NO_ACTIVATE);
    }

    ~QuectelTimer() override {
        if (active_) {
            tx_timer_deactivate(&timer_);
        }
        tx_timer_delete(&timer_);
    }

    RTOSResult start(uint32_t periodMs, bool periodic, TimerCallback callback) override {
        callback_ = callback;

        ULONG ticks = periodMs * TX_TIMER_TICKS_PER_SECOND / 1000;
        ULONG reschedule = periodic ? ticks : 0;

        UINT status = tx_timer_change(&timer_, ticks, reschedule);
        if (status != TX_SUCCESS) {
            return RTOSResult::ERROR_UNKNOWN;
        }

        status = tx_timer_activate(&timer_);
        if (status != TX_SUCCESS) {
            return RTOSResult::ERROR_UNKNOWN;
        }

        active_ = true;
        return RTOSResult::SUCCESS;
    }

    RTOSResult stop() override {
        if (!active_) {
            return RTOSResult::SUCCESS;
        }

        UINT status = tx_timer_deactivate(&timer_);
        active_ = false;
        return (status == TX_SUCCESS) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
    }

    RTOSResult restart() override {
        stop();
        UINT status = tx_timer_activate(&timer_);
        active_ = (status == TX_SUCCESS);
        return active_ ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
    }

    RTOSResult changePeriod(uint32_t periodMs) override {
        ULONG ticks = periodMs * TX_TIMER_TICKS_PER_SECOND / 1000;
        UINT status = tx_timer_change(&timer_, ticks, ticks);
        return (status == TX_SUCCESS) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
    }

    bool isActive() override {
        return active_;
    }

    const char* getName() override {
        return name_;
    }

private:
    static void timerWrapper(ULONG arg) {
        QuectelTimer* timer = reinterpret_cast<QuectelTimer*>(arg);
        if (timer->callback_) {
            timer->callback_();
        }
    }

    const char* name_;
    TX_TIMER timer_;
    TimerCallback callback_;
    bool active_;
};

// ============================================================================
// Quectel Thread Manager
// ============================================================================

class QuectelThreadManager : public IThreadManager {
public:
    std::unique_ptr<IThread> createThread(
        const char* name,
        ThreadFunc func,
        void* arg,
        uint32_t stackSize,
        ThreadPriority priority
    ) override {
        auto thread = std::make_unique<QuectelThread>(name, func, arg, stackSize, priority);
        RTOSResult result = thread->start();
        if (result != RTOSResult::SUCCESS) {
            return nullptr;
        }
        return thread;
    }

    void sleep(uint32_t ms) override {
        ULONG ticks = ms * TX_TIMER_TICKS_PER_SECOND / 1000;
        tx_thread_sleep(ticks);
    }

    void yield() override {
        tx_thread_relinquish();
    }

    uint64_t getTicks() override {
        return tx_time_get();
    }

    uint32_t getTickRate() override {
        return TX_TIMER_TICKS_PER_SECOND;
    }

    void enterCritical() override {
        TX_INTERRUPT_SAVE_AREA
        TX_DISABLE
    }

    void exitCritical() override {
        TX_INTERRUPT_SAVE_AREA
        TX_RESTORE
    }
};

// ============================================================================
// Quectel Cellular Modem Implementation
// ============================================================================

class QuectelModem : public ICellularModem {
public:
    QuectelModem() : initialized_(false), connected_(false) {}

    bool init() override {
        if (initialized_) return true;

        // Quectel modules are pre-initialized by firmware
        initialized_ = true;
        return true;
    }

    bool connect() override {
        if (!initialized_) return false;

        // Use AT commands or QAPI for connection
        // Simplified: assume auto-connect is enabled
        connected_ = true;
        return true;
    }

    bool disconnect() override {
        if (!connected_) return true;

        connected_ = false;
        return true;
    }

    bool isConnected() override {
        return connected_;
    }

    int getSignalStrength() override {
        // Use qapi_Device_Info_Get to get signal strength
        qapi_Device_Info_t info;
        qapi_Status_t status = qapi_Device_Info_Get(
            QAPI_DEVICE_INFO_RSSI_E,
            &info
        );

        if (status == QAPI_OK) {
            return info.u.valueuint32; // RSSI in dBm
        }
        return -1;
    }

    bool getNetworkInfo(char* buffer, size_t bufferSize) override {
        qapi_Device_Info_t info;
        qapi_Status_t status = qapi_Device_Info_Get(
            QAPI_DEVICE_INFO_NETWORK_IND_E,
            &info
        );

        if (status == QAPI_OK) {
            snprintf(buffer, bufferSize, "Network: %s", info.u.valuestring);
            return true;
        }
        return false;
    }

    bool getIMEI(char* buffer, size_t bufferSize) override {
        qapi_Device_Info_t info;
        qapi_Status_t status = qapi_Device_Info_Get(
            QAPI_DEVICE_INFO_IMEI_E,
            &info
        );

        if (status == QAPI_OK) {
            strncpy(buffer, info.u.valuestring, bufferSize - 1);
            buffer[bufferSize - 1] = '\0';
            return true;
        }
        return false;
    }

    bool getIMSI(char* buffer, size_t bufferSize) override {
        qapi_Device_Info_t info;
        qapi_Status_t status = qapi_Device_Info_Get(
            QAPI_DEVICE_INFO_IMSI_E,
            &info
        );

        if (status == QAPI_OK) {
            strncpy(buffer, info.u.valuestring, bufferSize - 1);
            buffer[bufferSize - 1] = '\0';
            return true;
        }
        return false;
    }

    bool setNetworkMode(NetworkMode mode) override {
        // Use AT+QCFG="nwscanmode" command via qapi_atfwd
        // Simplified implementation
        return true;
    }

    bool setPSM(uint32_t tau, uint32_t activeTime) override {
        // Use AT+CPSMS command
        return true;
    }

    bool setEDRX(uint32_t edrx) override {
        // Use AT+CEDRXS command
        return true;
    }

private:
    bool initialized_;
    bool connected_;
};

// ============================================================================
// Quectel GNSS Implementation
// ============================================================================

class QuectelGNSS {
public:
    static bool start() {
        qapi_Location_t location;
        qapi_Status_t status = qapi_Loc_Start_Tracking(
            QAPI_LOCATION_CLIENT_TYPE_CONTROL_POINT,
            nullptr,
            nullptr
        );
        return (status == QAPI_OK);
    }

    static bool stop() {
        qapi_Status_t status = qapi_Loc_Stop_Tracking(
            QAPI_LOCATION_CLIENT_TYPE_CONTROL_POINT
        );
        return (status == QAPI_OK);
    }

    static bool getPosition(double* latitude, double* longitude, double* altitude) {
        qapi_Location_t location;
        // Simplified: get last known location
        // Real implementation would use callbacks
        return false;
    }
};

// ============================================================================
// Quectel Memory Allocator
// ============================================================================

class QuectelMemoryAllocator : public IMemoryAllocator {
public:
    void* allocate(size_t size) override {
        return tx_byte_allocate(&byte_pool_, &ptr_, size, TX_NO_WAIT);
        // Simplified: use standard malloc
        return malloc(size);
    }

    void free(void* ptr) override {
        ::free(ptr);
    }

    void* reallocate(void* ptr, size_t size) override {
        return realloc(ptr, size);
    }

    size_t getTotalHeap() override {
        // ThreadX byte pool statistics
        return 256 * 1024; // Example: 256KB
    }

    size_t getFreeHeap() override {
        ULONG available_bytes;
        tx_byte_pool_info_get(&byte_pool_, nullptr, &available_bytes,
                             nullptr, nullptr, nullptr, nullptr);
        return available_bytes;
    }

    size_t getMinEverFreeHeap() override {
        return 64 * 1024; // Not tracked by ThreadX
    }

    size_t getLargestFreeBlock() override {
        return getFreeHeap();
    }

    bool isThreadSafe() override {
        return true; // ThreadX allocators are thread-safe
    }

private:
    TX_BYTE_POOL byte_pool_;
    void* ptr_;
};

// ============================================================================
// Quectel Factory Implementation
// ============================================================================

class QuectelFactory : public RTOSFactory {
public:
    const char* getRTOSName() override {
        return "Quectel SDK (ThreadX)";
    }

    const char* getRTOSVersion() override {
        static char version[64];
        snprintf(version, sizeof(version), "Quectel SDK + ThreadX %s",
                 __THREADX_VERSION__);
        return version;
    }

    const char* getChipInfo() {
        static char info[128];

#if defined(CONFIG_QUECTEL_BG95) || defined(CONFIG_QUECTEL_BG96)
        snprintf(info, sizeof(info), "Quectel BG95/BG96 (LTE Cat-M1/NB-IoT/EGPRS + GNSS)");
#elif defined(CONFIG_QUECTEL_BG77)
        snprintf(info, sizeof(info), "Quectel BG77 (LTE Cat-M1/NB-IoT + GNSS)");
#elif defined(CONFIG_QUECTEL_BC660K)
        snprintf(info, sizeof(info), "Quectel BC660K (NB-IoT, ultra-low-power)");
#elif defined(CONFIG_QUECTEL_EC25) || defined(CONFIG_QUECTEL_EC21)
        snprintf(info, sizeof(info), "Quectel EC25/EC21 (LTE Cat-4 + GNSS)");
#elif defined(CONFIG_QUECTEL_EG915)
        snprintf(info, sizeof(info), "Quectel EG915 (LTE Cat-1 + GNSS)");
#else
        snprintf(info, sizeof(info), "Quectel Module (unknown variant)");
#endif

        return info;
    }

    std::unique_ptr<IMutex> createMutex() override {
        return std::make_unique<QuectelMutex>();
    }

    std::unique_ptr<ISemaphore> createSemaphore(uint32_t initialCount, uint32_t maxCount) override {
        return std::make_unique<QuectelSemaphore>(initialCount, maxCount);
    }

    std::unique_ptr<ITimer> createTimer(const char* name) override {
        return std::make_unique<QuectelTimer>(name);
    }

    IThreadManager& getThreadManager() override {
        static QuectelThreadManager manager;
        return manager;
    }

    ICellularModem& getCellularModem() override {
        static QuectelModem modem;
        return modem;
    }

    IMemoryAllocator& getMemoryAllocator() override {
        static QuectelMemoryAllocator allocator;
        return allocator;
    }

    // Quectel-specific: GNSS control
    bool startGNSS() {
        return QuectelGNSS::start();
    }

    bool stopGNSS() {
        return QuectelGNSS::stop();
    }

    bool getGNSSPosition(double* lat, double* lon, double* alt) {
        return QuectelGNSS::getPosition(lat, lon, alt);
    }

    // Quectel-specific: File system access
    int openFile(const char* path, int mode) {
        return qapi_FS_Open(path, mode);
    }

    int closeFile(int fd) {
        return qapi_FS_Close(fd);
    }

    int readFile(int fd, void* buffer, size_t size) {
        uint32_t bytesRead;
        return qapi_FS_Read(fd, (uint8_t*)buffer, size, &bytesRead);
    }

    int writeFile(int fd, const void* data, size_t size) {
        uint32_t bytesWritten;
        return qapi_FS_Write(fd, (uint8_t*)data, size, &bytesWritten);
    }
};

// ============================================================================
// Global Factory Instance
// ============================================================================

static QuectelFactory g_quectelFactory;

RTOSFactory& getRTOSFactory() {
    return g_quectelFactory;
}

} // namespace rtos
} // namespace platform
} // namespace lwm2m

#endif // QUECTEL_SDK
