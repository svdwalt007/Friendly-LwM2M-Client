/**
 * @file mbed_adapter.cpp
 * @brief Mbed OS RTOS Adapter Implementation
 *
 * Implements the RTOS abstraction layer for Mbed OS (ARM's IoT operating system).
 *
 * Mbed OS is ARM's platform operating system designed for IoT devices,
 * providing a C++ RTOS API, connectivity, security, and device management.
 *
 * Key Features:
 * - C++ RTOS API (rtos::Thread, rtos::Mutex, etc.)
 * - CMSIS-RTOS2 underneath (based on RTX5)
 * - Built-in TLS/DTLS (Mbed TLS)
 * - Connectivity: BLE, LoRaWAN, Cellular, WiFi, Ethernet
 * - ARM Pelion device management
 * - Strong focus on security (PSA, Arm TrustZone)
 *
 * Supported Platforms:
 * - STM32 Nucleo boards (F429ZI, L476RG, F746ZG, etc.)
 * - STM32 Discovery kits
 * - Nordic nRF52 DK
 * - NXP boards (FRDM-K64F, etc.)
 * - Raspberry Pi Pico (RP2040)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef RTOS_MBED

#include "platform/rtos_abstraction.h"
#include "mbed.h"
#include "rtos.h"
#include <string>
#include <cstring>

namespace lwm2m {
namespace platform {
namespace rtos {

// ============================================================================
// Mbed Thread Implementation
// ============================================================================

class MbedThread : public IThread {
public:
    MbedThread(const char* name, ThreadFunc func, void* arg,
               uint32_t stackSize, ThreadPriority priority)
        : name_(name), func_(func), arg_(arg), stackSize_(stackSize),
          priority_(priority), thread_(nullptr), started_(false) {}

    ~MbedThread() override {
        if (thread_ != nullptr) {
            thread_->terminate();
            delete thread_;
        }
    }

    RTOSResult start() override {
        if (started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        osPriority_t mbedPriority = mapPriorityToMbed(priority_);

        thread_ = new ::rtos::Thread(mbedPriority, stackSize_, nullptr, name_);
        if (thread_ == nullptr) {
            return RTOSResult::ERROR_NO_MEMORY;
        }

        osStatus status = thread_->start(callback(this, &MbedThread::threadWrapper));
        if (status != osOK) {
            delete thread_;
            thread_ = nullptr;
            return RTOSResult::ERROR_UNKNOWN;
        }

        started_ = true;
        return RTOSResult::SUCCESS;
    }

    RTOSResult join(uint32_t timeoutMs) override {
        if (!started_ || thread_ == nullptr) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        osStatus status = thread_->join();
        return (status == osOK) ? RTOSResult::SUCCESS : RTOSResult::ERROR_TIMEOUT;
    }

    RTOSResult suspend() override {
        if (!started_ || thread_ == nullptr) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        osStatus status = thread_->flags_set(THREAD_FLAG_SUSPENDED);
        return (status >= 0) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
    }

    RTOSResult resume() override {
        if (!started_ || thread_ == nullptr) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        osStatus status = thread_->flags_clear(THREAD_FLAG_SUSPENDED);
        return (status >= 0) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
    }

    ThreadState getState() override {
        if (!started_ || thread_ == nullptr) {
            return ThreadState::READY;
        }

        ::rtos::Thread::State state = thread_->get_state();
        switch (state) {
            case ::rtos::Thread::Inactive:  return ThreadState::READY;
            case ::rtos::Thread::Ready:     return ThreadState::READY;
            case ::rtos::Thread::Running:   return ThreadState::RUNNING;
            case ::rtos::Thread::WaitingDelay:
            case ::rtos::Thread::WaitingThreadFlag:
            case ::rtos::Thread::WaitingEventFlag:
            case ::rtos::Thread::WaitingMutex:
            case ::rtos::Thread::WaitingSemaphore:
            case ::rtos::Thread::WaitingMemoryPool:
            case ::rtos::Thread::WaitingMessageGet:
            case ::rtos::Thread::WaitingMessagePut:
                return ThreadState::BLOCKED;
            case ::rtos::Thread::Deleted:   return ThreadState::TERMINATED;
            default:                        return ThreadState::UNKNOWN;
        }
    }

    ThreadPriority getPriority() override {
        return priority_;
    }

    RTOSResult setPriority(ThreadPriority priority) override {
        priority_ = priority;
        if (started_ && thread_ != nullptr) {
            osPriority_t mbedPriority = mapPriorityToMbed(priority);
            osStatus status = thread_->set_priority(mbedPriority);
            return (status == osOK) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
        }
        return RTOSResult::SUCCESS;
    }

    uint32_t getStackHighWaterMark() override {
        if (!started_ || thread_ == nullptr) {
            return stackSize_;
        }
        return thread_->stack_size() - thread_->used_stack();
    }

    const char* getName() override {
        return name_;
    }

private:
    void threadWrapper() {
        func_(arg_);
    }

    // Map normalized priority (0-6) to Mbed OS priority
    static osPriority_t mapPriorityToMbed(ThreadPriority priority) {
        switch (priority) {
            case ThreadPriority::IDLE:          return osPriorityIdle;
            case ThreadPriority::LOW:           return osPriorityLow;
            case ThreadPriority::BELOW_NORMAL:  return osPriorityBelowNormal;
            case ThreadPriority::NORMAL:        return osPriorityNormal;
            case ThreadPriority::ABOVE_NORMAL:  return osPriorityAboveNormal;
            case ThreadPriority::HIGH:          return osPriorityHigh;
            case ThreadPriority::REALTIME:      return osPriorityRealtime;
            default:                            return osPriorityNormal;
        }
    }

    static constexpr uint32_t THREAD_FLAG_SUSPENDED = (1 << 0);

    const char* name_;
    ThreadFunc func_;
    void* arg_;
    uint32_t stackSize_;
    ThreadPriority priority_;
    ::rtos::Thread* thread_;
    bool started_;
};

// ============================================================================
// Mbed Mutex Implementation
// ============================================================================

class MbedMutex : public IMutex {
public:
    MbedMutex() = default;
    ~MbedMutex() override = default;

    bool lock(uint32_t timeoutMs) override {
        std::chrono::milliseconds timeout(timeoutMs == UINT32_MAX
            ? osWaitForever
            : timeoutMs);

        bool acquired = mutex_.try_lock_for(timeout);
        return acquired;
    }

    void unlock() override {
        mutex_.unlock();
    }

    bool tryLock() override {
        return mutex_.try_lock();
    }

    bool isLocked() override {
        if (mutex_.try_lock()) {
            mutex_.unlock();
            return false;
        }
        return true;
    }

private:
    ::rtos::Mutex mutex_;
};

// ============================================================================
// Mbed Semaphore Implementation
// ============================================================================

class MbedSemaphore : public ISemaphore {
public:
    MbedSemaphore(uint32_t initialCount, uint32_t maxCount)
        : sem_(maxCount, initialCount), maxCount_(maxCount) {}

    ~MbedSemaphore() override = default;

    bool wait(uint32_t timeoutMs) override {
        std::chrono::milliseconds timeout(timeoutMs == UINT32_MAX
            ? osWaitForever
            : timeoutMs);

        bool acquired = sem_.try_acquire_for(timeout);
        return acquired;
    }

    bool signal() override {
        sem_.release();
        return true;
    }

    bool tryWait() override {
        return sem_.try_acquire();
    }

    uint32_t getCount() override {
        // Mbed doesn't provide count getter, approximate
        return 0; // Not available in Mbed OS API
    }

    void reset() override {
        // Drain the semaphore
        while (sem_.try_acquire()) {
            // Empty
        }
    }

private:
    ::rtos::Semaphore sem_;
    uint32_t maxCount_;
};

// ============================================================================
// Mbed Timer Implementation
// ============================================================================

class MbedTimer : public ITimer {
public:
    explicit MbedTimer(const char* name)
        : name_(name), periodMs_(0), periodic_(false),
          active_(false), callback_(nullptr) {}

    ~MbedTimer() override {
        stop();
    }

    RTOSResult start(uint32_t periodMs, bool periodic, TimerCallback callback) override {
        periodMs_ = periodMs;
        periodic_ = periodic;
        callback_ = callback;

        if (periodic) {
            ticker_.attach(callback(this, &MbedTimer::timerCallbackWrapper),
                          std::chrono::milliseconds(periodMs));
        } else {
            timeout_.attach(callback(this, &MbedTimer::timerCallbackWrapper),
                           std::chrono::milliseconds(periodMs));
        }

        active_ = true;
        return RTOSResult::SUCCESS;
    }

    RTOSResult stop() override {
        if (!active_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        if (periodic_) {
            ticker_.detach();
        } else {
            timeout_.detach();
        }

        active_ = false;
        return RTOSResult::SUCCESS;
    }

    RTOSResult restart() override {
        if (!active_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        stop();
        return start(periodMs_, periodic_, callback_);
    }

    RTOSResult changePeriod(uint32_t periodMs) override {
        periodMs_ = periodMs;
        if (active_) {
            return restart();
        }
        return RTOSResult::SUCCESS;
    }

    bool isActive() override {
        return active_;
    }

    uint32_t getPeriod() override {
        return periodMs_;
    }

private:
    void timerCallbackWrapper() {
        if (callback_) {
            callback_();
        }

        // For one-shot timers, mark as inactive after firing
        if (!periodic_) {
            active_ = false;
        }
    }

    const char* name_;
    uint32_t periodMs_;
    bool periodic_;
    bool active_;
    TimerCallback callback_;
    ::Ticker ticker_;      // For periodic timers
    ::Timeout timeout_;    // For one-shot timers
};

// ============================================================================
// Mbed Memory Allocator Implementation
// ============================================================================

class MbedMemoryAllocator : public IMemoryAllocator {
public:
    void* allocate(size_t size) override {
        return malloc(size);
    }

    void free(void* ptr) override {
        ::free(ptr);
    }

    void* reallocate(void* ptr, size_t newSize) override {
        return realloc(ptr, newSize);
    }

    size_t getTotalHeap() override {
        mbed_stats_heap_t stats;
        mbed_stats_heap_get(&stats);
        return stats.reserved_size;
    }

    size_t getFreeHeap() override {
        mbed_stats_heap_t stats;
        mbed_stats_heap_get(&stats);
        return stats.reserved_size - stats.current_size;
    }

    size_t getMinEverFreeHeap() override {
        mbed_stats_heap_t stats;
        mbed_stats_heap_get(&stats);
        return stats.reserved_size - stats.max_size;
    }

    size_t getLargestFreeBlock() override {
        // Mbed doesn't provide this directly
        return 0;
    }

    bool isThreadSafe() override {
        return true; // Mbed heap is thread-safe
    }
};

// ============================================================================
// Mbed Thread Manager Implementation
// ============================================================================

class MbedThreadManager : public IThreadManager {
public:
    std::unique_ptr<IThread> createThread(
        const char* name,
        ThreadFunc func,
        void* arg,
        uint32_t stackSize,
        ThreadPriority priority
    ) override {
        auto thread = std::make_unique<MbedThread>(name, func, arg, stackSize, priority);

        // Auto-start thread
        RTOSResult result = thread->start();
        if (result != RTOSResult::SUCCESS) {
            return nullptr;
        }

        return thread;
    }

    void sleep(uint32_t ms) override {
        ThisThread::sleep_for(std::chrono::milliseconds(ms));
    }

    void yield() override {
        ThisThread::yield();
    }

    uint64_t getTicks() override {
        return Kernel::Clock::now().time_since_epoch().count();
    }

    uint32_t getTickRate() override {
        return osKernelGetTickFreq();
    }

    void enterCritical() override {
        criticalSection_.lock();
    }

    void exitCritical() override {
        criticalSection_.unlock();
    }

private:
    CriticalSectionLock criticalSection_;
};

// ============================================================================
// Mbed Factory Implementation
// ============================================================================

class MbedFactory : public RTOSFactory {
public:
    const char* getRTOSName() override {
        return "Mbed OS";
    }

    const char* getRTOSVersion() override {
        static char version[32];
        snprintf(version, sizeof(version), "%d.%d.%d",
                 MBED_MAJOR_VERSION,
                 MBED_MINOR_VERSION,
                 MBED_PATCH_VERSION);
        return version;
    }

    std::unique_ptr<IMutex> createMutex() override {
        return std::make_unique<MbedMutex>();
    }

    std::unique_ptr<ISemaphore> createSemaphore(uint32_t initialCount, uint32_t maxCount) override {
        return std::make_unique<MbedSemaphore>(initialCount, maxCount);
    }

    std::unique_ptr<ITimer> createTimer(const char* name) override {
        return std::make_unique<MbedTimer>(name);
    }

    IThreadManager& getThreadManager() override {
        static MbedThreadManager manager;
        return manager;
    }

    IMemoryAllocator& getMemoryAllocator() override {
        static MbedMemoryAllocator allocator;
        return allocator;
    }
};

// ============================================================================
// Global Factory Instance
// ============================================================================

static MbedFactory g_mbedFactory;

RTOSFactory& getRTOSFactory() {
    return g_mbedFactory;
}

} // namespace rtos
} // namespace platform
} // namespace lwm2m

#endif // RTOS_MBED
