/**
 * @file zephyr_adapter.cpp
 * @brief Zephyr RTOS Adapter Implementation
 *
 * Implements the RTOS abstraction layer for Zephyr RTOS.
 *
 * Zephyr is a Linux Foundation project providing a scalable RTOS
 * for resource-constrained devices. It has native LwM2M support
 * but this adapter allows using Friendly LwM2M features.
 *
 * Key Differences from FreeRTOS:
 * - Inverted priority system (0 = highest cooperative, negative = preemptive)
 * - No native thread join (implemented via polling k_thread_join)
 * - Built-in memory pools in addition to heap
 * - Integrated device tree configuration
 * - Strong focus on safety and security (MISRA-C compliance)
 *
 * Supported Platforms:
 * - nRF52840 DK (Bluetooth Low Energy)
 * - nRF9160 DK / Thingy:91 (LTE-M/NB-IoT)
 * - nRF9151 SMA Dev Kit (LTE-M/NB-IoT)
 * - STM32 boards
 * - QEMU emulation (x86, ARM, RISC-V)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef RTOS_ZEPHYR

#include "platform/rtos_abstraction.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/mem_slab.h>
#include <string>
#include <cstring>

namespace lwm2m {
namespace platform {
namespace rtos {

// ============================================================================
// Zephyr Thread Implementation
// ============================================================================

class ZephyrThread : public IThread {
public:
    ZephyrThread(const char* name, ThreadFunc func, void* arg,
                 uint32_t stackSize, ThreadPriority priority)
        : name_(name), func_(func), arg_(arg), stackSize_(stackSize),
          priority_(priority), started_(false), joined_(false) {

        // Allocate stack memory
        stack_ = static_cast<k_thread_stack_t*>(k_malloc(stackSize));
        if (stack_ == nullptr) {
            printk("ERROR: Failed to allocate thread stack (%u bytes)\n", stackSize);
        }
    }

    ~ZephyrThread() override {
        if (started_ && !joined_) {
            // Abort thread if not joined
            k_thread_abort(&thread_);
        }
        if (stack_ != nullptr) {
            k_free(stack_);
        }
    }

    RTOSResult start() override {
        if (stack_ == nullptr) {
            return RTOSResult::ERROR_NO_MEMORY;
        }

        if (started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        int zephyrPriority = mapPriorityToZephyr(priority_);

        k_tid_t tid = k_thread_create(
            &thread_,                    // Thread control block
            stack_,                      // Stack memory
            stackSize_,                  // Stack size
            threadWrapper,               // Entry function
            this,                        // Parameter 1
            nullptr,                     // Parameter 2
            nullptr,                     // Parameter 3
            zephyrPriority,              // Priority
            0,                           // Options
            K_NO_WAIT                    // Start immediately
        );

        if (tid == nullptr) {
            return RTOSResult::ERROR_UNKNOWN;
        }

        // Set thread name (if supported)
        k_thread_name_set(&thread_, name_);

        started_ = true;
        return RTOSResult::SUCCESS;
    }

    RTOSResult join(uint32_t timeoutMs) override {
        if (!started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        if (joined_) {
            return RTOSResult::SUCCESS; // Already joined
        }

        // Zephyr 3.2+ has k_thread_join
        #if defined(CONFIG_ZEPHYR_VERSION) && CONFIG_ZEPHYR_VERSION >= 0x030200
            k_timeout_t timeout = (timeoutMs == UINT32_MAX)
                ? K_FOREVER
                : K_MSEC(timeoutMs);

            int ret = k_thread_join(&thread_, timeout);
            if (ret == 0) {
                joined_ = true;
                return RTOSResult::SUCCESS;
            } else if (ret == -EAGAIN) {
                return RTOSResult::ERROR_TIMEOUT;
            } else {
                return RTOSResult::ERROR_UNKNOWN;
            }
        #else
            // For older Zephyr versions, poll thread state
            uint64_t startTime = k_uptime_get();
            while (true) {
                uint8_t state = k_thread_state_str(&thread_)[0];
                if (state == 'd' || state == 'D') { // Dead/Terminated
                    joined_ = true;
                    return RTOSResult::SUCCESS;
                }

                if (timeoutMs != UINT32_MAX) {
                    uint64_t elapsed = k_uptime_get() - startTime;
                    if (elapsed >= timeoutMs) {
                        return RTOSResult::ERROR_TIMEOUT;
                    }
                }

                k_sleep(K_MSEC(10)); // Poll every 10ms
            }
        #endif
    }

    RTOSResult suspend() override {
        if (!started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }
        k_thread_suspend(&thread_);
        return RTOSResult::SUCCESS;
    }

    RTOSResult resume() override {
        if (!started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }
        k_thread_resume(&thread_);
        return RTOSResult::SUCCESS;
    }

    ThreadState getState() override {
        if (!started_) {
            return ThreadState::READY;
        }

        // Parse state string from Zephyr
        const char* stateStr = k_thread_state_str(&thread_);
        if (stateStr == nullptr) {
            return ThreadState::UNKNOWN;
        }

        // Zephyr states: "pending", "prestart", "dead", "suspended", "queued", etc.
        char state = stateStr[0];
        switch (state) {
            case 'p': return ThreadState::READY;     // pending/prestart
            case 'd': return ThreadState::TERMINATED; // dead
            case 's': return ThreadState::SUSPENDED;  // suspended
            case 'q': return ThreadState::READY;      // queued
            default:  return ThreadState::RUNNING;    // Assume running
        }
    }

    ThreadPriority getPriority() override {
        return priority_;
    }

    RTOSResult setPriority(ThreadPriority priority) override {
        priority_ = priority;
        if (started_) {
            int zephyrPriority = mapPriorityToZephyr(priority);
            k_thread_priority_set(&thread_, zephyrPriority);
        }
        return RTOSResult::SUCCESS;
    }

    uint32_t getStackHighWaterMark() override {
        if (!started_) {
            return stackSize_;
        }

        #ifdef CONFIG_THREAD_STACK_INFO
            size_t unused;
            int ret = k_thread_stack_space_get(&thread_, &unused);
            if (ret == 0) {
                return static_cast<uint32_t>(unused);
            }
        #endif

        return 0; // Unknown
    }

    const char* getName() override {
        return name_;
    }

private:
    static void threadWrapper(void* p1, void* p2, void* p3) {
        (void)p2;
        (void)p3;
        auto* thread = static_cast<ZephyrThread*>(p1);
        thread->func_(thread->arg_);
    }

    // Map normalized priority (0-6) to Zephyr priority
    // Zephyr uses inverted priorities:
    // - Cooperative threads: 0 (highest) to CONFIG_NUM_COOP_PRIORITIES-1 (lowest)
    // - Preemptible threads: Negative values (-1 to -CONFIG_NUM_PREEMPT_PRIORITIES)
    static int mapPriorityToZephyr(ThreadPriority priority) {
        // Use cooperative threads for most priorities
        // Reserve preemptive for REALTIME
        switch (priority) {
            case ThreadPriority::IDLE:
                // Lowest cooperative priority
                return CONFIG_NUM_COOP_PRIORITIES - 1;

            case ThreadPriority::LOW:
                return (CONFIG_NUM_COOP_PRIORITIES * 5) / 6;

            case ThreadPriority::BELOW_NORMAL:
                return (CONFIG_NUM_COOP_PRIORITIES * 4) / 6;

            case ThreadPriority::NORMAL:
                return CONFIG_NUM_COOP_PRIORITIES / 2;

            case ThreadPriority::ABOVE_NORMAL:
                return (CONFIG_NUM_COOP_PRIORITIES * 2) / 6;

            case ThreadPriority::HIGH:
                return CONFIG_NUM_COOP_PRIORITIES / 6;

            case ThreadPriority::REALTIME:
                // Use preemptive thread for realtime
                return -1; // Highest preemptive priority

            default:
                return CONFIG_NUM_COOP_PRIORITIES / 2; // NORMAL
        }
    }

    const char* name_;
    ThreadFunc func_;
    void* arg_;
    uint32_t stackSize_;
    ThreadPriority priority_;
    struct k_thread thread_;
    k_thread_stack_t* stack_;
    bool started_;
    bool joined_;
};

// ============================================================================
// Zephyr Mutex Implementation
// ============================================================================

class ZephyrMutex : public IMutex {
public:
    ZephyrMutex() {
        k_mutex_init(&mutex_);
    }

    ~ZephyrMutex() override {
        // Zephyr mutexes don't need explicit cleanup
    }

    bool lock(uint32_t timeoutMs) override {
        k_timeout_t timeout = (timeoutMs == UINT32_MAX)
            ? K_FOREVER
            : K_MSEC(timeoutMs);

        int ret = k_mutex_lock(&mutex_, timeout);
        return (ret == 0);
    }

    void unlock() override {
        k_mutex_unlock(&mutex_);
    }

    bool tryLock() override {
        int ret = k_mutex_lock(&mutex_, K_NO_WAIT);
        return (ret == 0);
    }

    bool isLocked() override {
        // Try to lock with no wait, if it fails, it's locked
        if (k_mutex_lock(&mutex_, K_NO_WAIT) == 0) {
            k_mutex_unlock(&mutex_);
            return false;
        }
        return true;
    }

private:
    struct k_mutex mutex_;
};

// ============================================================================
// Zephyr Semaphore Implementation
// ============================================================================

class ZephyrSemaphore : public ISemaphore {
public:
    ZephyrSemaphore(uint32_t initialCount, uint32_t maxCount)
        : maxCount_(maxCount) {
        k_sem_init(&sem_, initialCount, maxCount);
    }

    ~ZephyrSemaphore() override {
        // Zephyr semaphores don't need explicit cleanup
    }

    bool wait(uint32_t timeoutMs) override {
        k_timeout_t timeout = (timeoutMs == UINT32_MAX)
            ? K_FOREVER
            : K_MSEC(timeoutMs);

        int ret = k_sem_take(&sem_, timeout);
        return (ret == 0);
    }

    bool signal() override {
        // Check if we'd exceed max count
        if (k_sem_count_get(&sem_) >= maxCount_) {
            return false;
        }
        k_sem_give(&sem_);
        return true;
    }

    bool tryWait() override {
        int ret = k_sem_take(&sem_, K_NO_WAIT);
        return (ret == 0);
    }

    uint32_t getCount() override {
        return k_sem_count_get(&sem_);
    }

    void reset() override {
        k_sem_reset(&sem_);
    }

private:
    struct k_sem sem_;
    uint32_t maxCount_;
};

// ============================================================================
// Zephyr Timer Implementation
// ============================================================================

class ZephyrTimer : public ITimer {
public:
    explicit ZephyrTimer(const char* name)
        : name_(name), periodMs_(0), periodic_(false),
          active_(false), callback_(nullptr) {
        k_timer_init(&timer_, timerCallbackWrapper, nullptr);
    }

    ~ZephyrTimer() override {
        stop();
    }

    RTOSResult start(uint32_t periodMs, bool periodic, TimerCallback callback) override {
        periodMs_ = periodMs;
        periodic_ = periodic;
        callback_ = callback;

        k_timeout_t duration = K_MSEC(periodMs);
        k_timeout_t period = periodic ? K_MSEC(periodMs) : K_NO_WAIT;

        k_timer_start(&timer_, duration, period);
        active_ = true;

        return RTOSResult::SUCCESS;
    }

    RTOSResult stop() override {
        if (!active_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }
        k_timer_stop(&timer_);
        active_ = false;
        return RTOSResult::SUCCESS;
    }

    RTOSResult restart() override {
        if (!active_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        k_timeout_t duration = K_MSEC(periodMs_);
        k_timeout_t period = periodic_ ? K_MSEC(periodMs_) : K_NO_WAIT;

        k_timer_start(&timer_, duration, period);
        return RTOSResult::SUCCESS;
    }

    RTOSResult changePeriod(uint32_t periodMs) override {
        periodMs_ = periodMs;
        if (active_) {
            return restart();
        }
        return RTOSResult::SUCCESS;
    }

    bool isActive() override {
        return active_ && (k_timer_status_get(&timer_) > 0 || k_timer_remaining_get(&timer_) > 0);
    }

    uint32_t getPeriod() override {
        return periodMs_;
    }

private:
    static void timerCallbackWrapper(struct k_timer* timer) {
        // Get the ZephyrTimer instance from the timer user data
        auto* zTimer = CONTAINER_OF(timer, ZephyrTimer, timer_);
        if (zTimer && zTimer->callback_) {
            zTimer->callback_();
        }
    }

    const char* name_;
    struct k_timer timer_;
    uint32_t periodMs_;
    bool periodic_;
    bool active_;
    TimerCallback callback_;
};

// ============================================================================
// Zephyr Memory Allocator Implementation
// ============================================================================

class ZephyrMemoryAllocator : public IMemoryAllocator {
public:
    void* allocate(size_t size) override {
        return k_malloc(size);
    }

    void free(void* ptr) override {
        k_free(ptr);
    }

    void* reallocate(void* ptr, size_t newSize) override {
        // Zephyr doesn't have realloc, implement manually
        if (ptr == nullptr) {
            return k_malloc(newSize);
        }

        if (newSize == 0) {
            k_free(ptr);
            return nullptr;
        }

        void* newPtr = k_malloc(newSize);
        if (newPtr != nullptr && ptr != nullptr) {
            // Copy old data (we don't know old size, so copy up to newSize)
            // This is a limitation - ideally we'd track allocation sizes
            memcpy(newPtr, ptr, newSize);
            k_free(ptr);
        }

        return newPtr;
    }

    size_t getTotalHeap() override {
        #ifdef CONFIG_HEAP_MEM_POOL_SIZE
            return CONFIG_HEAP_MEM_POOL_SIZE;
        #else
            return 0; // Unknown
        #endif
    }

    size_t getFreeHeap() override {
        #ifdef CONFIG_SYS_HEAP_RUNTIME_STATS
            struct sys_memory_stats stats;
            sys_heap_runtime_stats_get(&_system_heap, &stats);
            return stats.free_bytes;
        #else
            return 0; // Unknown without runtime stats
        #endif
    }

    size_t getMinEverFreeHeap() override {
        #ifdef CONFIG_SYS_HEAP_RUNTIME_STATS
            struct sys_memory_stats stats;
            sys_heap_runtime_stats_get(&_system_heap, &stats);
            return stats.max_allocated_bytes;
        #else
            return 0; // Unknown
        #endif
    }

    size_t getLargestFreeBlock() override {
        // Zephyr doesn't provide this directly
        return 0; // Unknown
    }

    bool isThreadSafe() override {
        return true; // Zephyr heap is thread-safe
    }
};

// ============================================================================
// Zephyr Thread Manager Implementation
// ============================================================================

class ZephyrThreadManager : public IThreadManager {
public:
    std::unique_ptr<IThread> createThread(
        const char* name,
        ThreadFunc func,
        void* arg,
        uint32_t stackSize,
        ThreadPriority priority
    ) override {
        auto thread = std::make_unique<ZephyrThread>(name, func, arg, stackSize, priority);

        // Auto-start thread
        RTOSResult result = thread->start();
        if (result != RTOSResult::SUCCESS) {
            return nullptr;
        }

        return thread;
    }

    void sleep(uint32_t ms) override {
        k_sleep(K_MSEC(ms));
    }

    void yield() override {
        k_yield();
    }

    uint64_t getTicks() override {
        return k_uptime_get();
    }

    uint32_t getTickRate() override {
        return CONFIG_SYS_CLOCK_TICKS_PER_SEC;
    }

    void enterCritical() override {
        // Zephyr uses interrupt locking for critical sections
        criticalKey_ = irq_lock();
    }

    void exitCritical() override {
        irq_unlock(criticalKey_);
    }

private:
    unsigned int criticalKey_;
};

// ============================================================================
// Zephyr Factory Implementation
// ============================================================================

class ZephyrFactory : public RTOSFactory {
public:
    const char* getRTOSName() override {
        return "Zephyr";
    }

    const char* getRTOSVersion() override {
        static char version[32];
        snprintf(version, sizeof(version), "%u.%u.%u",
                 (unsigned)SYS_KERNEL_VER_MAJOR(),
                 (unsigned)SYS_KERNEL_VER_MINOR(),
                 (unsigned)SYS_KERNEL_VER_PATCHLEVEL());
        return version;
    }

    std::unique_ptr<IMutex> createMutex() override {
        return std::make_unique<ZephyrMutex>();
    }

    std::unique_ptr<ISemaphore> createSemaphore(uint32_t initialCount, uint32_t maxCount) override {
        return std::make_unique<ZephyrSemaphore>(initialCount, maxCount);
    }

    std::unique_ptr<ITimer> createTimer(const char* name) override {
        return std::make_unique<ZephyrTimer>(name);
    }

    IThreadManager& getThreadManager() override {
        static ZephyrThreadManager manager;
        return manager;
    }

    IMemoryAllocator& getMemoryAllocator() override {
        static ZephyrMemoryAllocator allocator;
        return allocator;
    }
};

// ============================================================================
// Global Factory Instance
// ============================================================================

static ZephyrFactory g_zephyrFactory;

RTOSFactory& getRTOSFactory() {
    return g_zephyrFactory;
}

} // namespace rtos
} // namespace platform
} // namespace lwm2m

#endif // RTOS_ZEPHYR
