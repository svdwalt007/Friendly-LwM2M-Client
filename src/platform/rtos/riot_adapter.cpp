/**
 * @file riot_adapter.cpp
 * @brief RIOT OS RTOS Adapter Implementation
 *
 * Implements the RTOS abstraction layer for RIOT OS - The friendly Operating System for IoT.
 *
 * RIOT is a microkernel-based operating system designed for IoT devices with
 * extremely constrained resources (8KB RAM minimum). It supports native Linux
 * simulation for rapid development without hardware.
 *
 * Key Features:
 * - Microkernel architecture (smallest memory footprint)
 * - Native Linux simulation (no hardware needed!)
 * - Built-in 6LoWPAN, CoAP, MQTT, CBOR
 * - Real-time capable (hard real-time on some platforms)
 * - Extensive hardware support (200+ boards)
 * - Energy-efficient (deep sleep support)
 *
 * Supported Platforms:
 * - Native (Linux/macOS simulation)
 * - STM32 (all families)
 * - ESP32, ESP8266
 * - nRF52, nRF51
 * - ATmega (Arduino)
 * - MSP430
 * - RISC-V boards
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef RTOS_RIOT

#include "platform/rtos_abstraction.h"
#include "thread.h"
#include "mutex.h"
#include "xtimer.h"
#include "ztimer.h"
#include "msg.h"
#include <string>
#include <cstring>

namespace lwm2m {
namespace platform {
namespace rtos {

// ============================================================================
// RIOT Thread Implementation
// ============================================================================

class RIOTThread : public IThread {
public:
    RIOTThread(const char* name, ThreadFunc func, void* arg,
               uint32_t stackSize, ThreadPriority priority)
        : name_(name), func_(func), arg_(arg), stackSize_(stackSize),
          priority_(priority), pid_(KERNEL_PID_UNDEF), started_(false) {

        // Allocate stack
        stack_ = static_cast<char*>(malloc(stackSize));
    }

    ~RIOTThread() override {
        if (stack_ != nullptr) {
            free(stack_);
        }
    }

    RTOSResult start() override {
        if (stack_ == nullptr) {
            return RTOSResult::ERROR_NO_MEMORY;
        }

        if (started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        // Map priority (RIOT: 0 = highest, 15 = lowest)
        uint8_t riotPriority = mapPriorityToRIOT(priority_);

        pid_ = thread_create(
            stack_,
            stackSize_,
            riotPriority,
            THREAD_CREATE_STACKTEST,
            threadWrapper,
            this,
            name_
        );

        if (pid_ <= KERNEL_PID_UNDEF) {
            return RTOSResult::ERROR_UNKNOWN;
        }

        started_ = true;
        return RTOSResult::SUCCESS;
    }

    RTOSResult join(uint32_t timeoutMs) override {
        if (!started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        // RIOT doesn't have native join, use message passing
        msg_t msg;
        if (timeoutMs == UINT32_MAX) {
            msg_receive(&msg);
        } else {
            xtimer_t timeout;
            msg_t timeout_msg = { .type = 0 };
            xtimer_set_msg(&timeout, timeoutMs * US_PER_MS, &timeout_msg, thread_getpid());

            msg_receive(&msg);
            xtimer_remove(&timeout);

            if (msg.type == 0) {
                return RTOSResult::ERROR_TIMEOUT;
            }
        }

        return RTOSResult::SUCCESS;
    }

    RTOSResult suspend() override {
        // RIOT doesn't support suspend/resume natively
        return RTOSResult::ERROR_NOT_SUPPORTED;
    }

    RTOSResult resume() override {
        return RTOSResult::ERROR_NOT_SUPPORTED;
    }

    ThreadState getState() override {
        if (!started_) {
            return ThreadState::READY;
        }

        thread_status_t status = thread_getstatus(pid_);
        switch (status) {
            case STATUS_RUNNING:        return ThreadState::RUNNING;
            case STATUS_PENDING:        return ThreadState::READY;
            case STATUS_SLEEPING:
            case STATUS_MUTEX_BLOCKED:
            case STATUS_RECEIVE_BLOCKED:
            case STATUS_SEND_BLOCKED:
            case STATUS_REPLY_BLOCKED:
            case STATUS_FLAG_BLOCKED_ANY:
            case STATUS_FLAG_BLOCKED_ALL:
                return ThreadState::BLOCKED;
            case STATUS_STOPPED:        return ThreadState::TERMINATED;
            default:                    return ThreadState::UNKNOWN;
        }
    }

    ThreadPriority getPriority() override {
        return priority_;
    }

    RTOSResult setPriority(ThreadPriority priority) override {
        priority_ = priority;
        // RIOT doesn't support runtime priority changes
        return RTOSResult::SUCCESS;
    }

    uint32_t getStackHighWaterMark() override {
        if (!started_ || stack_ == nullptr) {
            return stackSize_;
        }
        return thread_measure_stack_free(stack_);
    }

    const char* getName() override {
        return name_;
    }

private:
    static void* threadWrapper(void* arg) {
        auto* thread = static_cast<RIOTThread*>(arg);
        thread->func_(thread->arg_);
        return nullptr;
    }

    // Map normalized priority (0-6) to RIOT priority (0-15)
    // RIOT: 0 = highest, THREAD_PRIORITY_MAIN = 7, 15 = lowest
    static uint8_t mapPriorityToRIOT(ThreadPriority priority) {
        switch (priority) {
            case ThreadPriority::IDLE:          return 15;  // Lowest
            case ThreadPriority::LOW:           return 12;
            case ThreadPriority::BELOW_NORMAL:  return 9;
            case ThreadPriority::NORMAL:        return THREAD_PRIORITY_MAIN;  // 7
            case ThreadPriority::ABOVE_NORMAL:  return 5;
            case ThreadPriority::HIGH:          return 2;
            case ThreadPriority::REALTIME:      return 0;   // Highest
            default:                            return THREAD_PRIORITY_MAIN;
        }
    }

    const char* name_;
    ThreadFunc func_;
    void* arg_;
    uint32_t stackSize_;
    ThreadPriority priority_;
    kernel_pid_t pid_;
    char* stack_;
    bool started_;
};

// ============================================================================
// RIOT Mutex Implementation
// ============================================================================

class RIOTMutex : public IMutex {
public:
    RIOTMutex() {
        mutex_init(&mutex_);
    }

    ~RIOTMutex() override = default;

    bool lock(uint32_t timeoutMs) override {
        // RIOT mutex doesn't support timeout
        (void)timeoutMs;
        mutex_lock(&mutex_);
        return true;
    }

    void unlock() override {
        mutex_unlock(&mutex_);
    }

    bool tryLock() override {
        return mutex_trylock(&mutex_) == 1;
    }

    bool isLocked() override {
        if (mutex_trylock(&mutex_) == 1) {
            mutex_unlock(&mutex_);
            return false;
        }
        return true;
    }

private:
    mutex_t mutex_;
};

// ============================================================================
// RIOT Semaphore Implementation (using msg queue)
// ============================================================================

class RIOTSemaphore : public ISemaphore {
public:
    RIOTSemaphore(uint32_t initialCount, uint32_t maxCount)
        : count_(initialCount), maxCount_(maxCount) {
        mutex_init(&mutex_);
    }

    ~RIOTSemaphore() override = default;

    bool wait(uint32_t timeoutMs) override {
        mutex_lock(&mutex_);

        if (count_ > 0) {
            count_--;
            mutex_unlock(&mutex_);
            return true;
        }

        mutex_unlock(&mutex_);

        if (timeoutMs == 0) {
            return false;
        }

        // Wait with timeout using xtimer
        if (timeoutMs == UINT32_MAX) {
            // Block indefinitely
            while (count_ == 0) {
                xtimer_usleep(1000); // 1ms
            }
        } else {
            uint32_t elapsed = 0;
            while (count_ == 0 && elapsed < timeoutMs) {
                xtimer_usleep(1000); // 1ms
                elapsed++;
            }

            if (count_ == 0) {
                return false; // Timeout
            }
        }

        mutex_lock(&mutex_);
        count_--;
        mutex_unlock(&mutex_);
        return true;
    }

    bool signal() override {
        mutex_lock(&mutex_);

        if (count_ >= maxCount_) {
            mutex_unlock(&mutex_);
            return false;
        }

        count_++;
        mutex_unlock(&mutex_);
        return true;
    }

    bool tryWait() override {
        mutex_lock(&mutex_);

        if (count_ > 0) {
            count_--;
            mutex_unlock(&mutex_);
            return true;
        }

        mutex_unlock(&mutex_);
        return false;
    }

    uint32_t getCount() override {
        mutex_lock(&mutex_);
        uint32_t count = count_;
        mutex_unlock(&mutex_);
        return count;
    }

    void reset() override {
        mutex_lock(&mutex_);
        count_ = 0;
        mutex_unlock(&mutex_);
    }

private:
    mutex_t mutex_;
    uint32_t count_;
    uint32_t maxCount_;
};

// ============================================================================
// RIOT Timer Implementation (using ztimer)
// ============================================================================

class RIOTTimer : public ITimer {
public:
    explicit RIOTTimer(const char* name)
        : name_(name), periodMs_(0), periodic_(false),
          active_(false), callback_(nullptr) {}

    ~RIOTTimer() override {
        stop();
    }

    RTOSResult start(uint32_t periodMs, bool periodic, TimerCallback callback) override {
        periodMs_ = periodMs;
        periodic_ = periodic;
        callback_ = callback;

        timer_.callback = timerCallbackWrapper;
        timer_.arg = this;

        ztimer_set(ZTIMER_MSEC, &timer_, periodMs);
        active_ = true;

        return RTOSResult::SUCCESS;
    }

    RTOSResult stop() override {
        if (!active_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        ztimer_remove(ZTIMER_MSEC, &timer_);
        active_ = false;
        return RTOSResult::SUCCESS;
    }

    RTOSResult restart() override {
        if (!active_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        ztimer_remove(ZTIMER_MSEC, &timer_);
        ztimer_set(ZTIMER_MSEC, &timer_, periodMs_);
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
        return active_;
    }

    uint32_t getPeriod() override {
        return periodMs_;
    }

private:
    static void timerCallbackWrapper(void* arg) {
        auto* timer = static_cast<RIOTTimer*>(arg);
        if (timer && timer->callback_) {
            timer->callback_();
        }

        // For periodic timers, reschedule
        if (timer->periodic_ && timer->active_) {
            ztimer_set(ZTIMER_MSEC, &timer->timer_, timer->periodMs_);
        } else {
            timer->active_ = false;
        }
    }

    const char* name_;
    ztimer_t timer_;
    uint32_t periodMs_;
    bool periodic_;
    bool active_;
    TimerCallback callback_;
};

// ============================================================================
// RIOT Memory Allocator Implementation
// ============================================================================

class RIOTMemoryAllocator : public IMemoryAllocator {
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
        // RIOT doesn't provide heap size info easily
        return 0;
    }

    size_t getFreeHeap() override {
        return 0;
    }

    size_t getMinEverFreeHeap() override {
        return 0;
    }

    size_t getLargestFreeBlock() override {
        return 0;
    }

    bool isThreadSafe() override {
        return true; // RIOT malloc is thread-safe
    }
};

// ============================================================================
// RIOT Thread Manager Implementation
// ============================================================================

class RIOTThreadManager : public IThreadManager {
public:
    std::unique_ptr<IThread> createThread(
        const char* name,
        ThreadFunc func,
        void* arg,
        uint32_t stackSize,
        ThreadPriority priority
    ) override {
        auto thread = std::make_unique<RIOTThread>(name, func, arg, stackSize, priority);

        RTOSResult result = thread->start();
        if (result != RTOSResult::SUCCESS) {
            return nullptr;
        }

        return thread;
    }

    void sleep(uint32_t ms) override {
        ztimer_sleep(ZTIMER_MSEC, ms);
    }

    void yield() override {
        thread_yield();
    }

    uint64_t getTicks() override {
        return ztimer_now(ZTIMER_MSEC);
    }

    uint32_t getTickRate() override {
        return 1000; // ZTIMER_MSEC is 1000 Hz
    }

    void enterCritical() override {
        irq_disable();
    }

    void exitCritical() override {
        irq_enable();
    }
};

// ============================================================================
// RIOT Factory Implementation
// ============================================================================

class RIOTFactory : public RTOSFactory {
public:
    const char* getRTOSName() override {
        return "RIOT OS";
    }

    const char* getRTOSVersion() override {
        return RIOT_VERSION;
    }

    std::unique_ptr<IMutex> createMutex() override {
        return std::make_unique<RIOTMutex>();
    }

    std::unique_ptr<ISemaphore> createSemaphore(uint32_t initialCount, uint32_t maxCount) override {
        return std::make_unique<RIOTSemaphore>(initialCount, maxCount);
    }

    std::unique_ptr<ITimer> createTimer(const char* name) override {
        return std::make_unique<RIOTTimer>(name);
    }

    IThreadManager& getThreadManager() override {
        static RIOTThreadManager manager;
        return manager;
    }

    IMemoryAllocator& getMemoryAllocator() override {
        static RIOTMemoryAllocator allocator;
        return allocator;
    }
};

// ============================================================================
// Global Factory Instance
// ============================================================================

static RIOTFactory g_riotFactory;

RTOSFactory& getRTOSFactory() {
    return g_riotFactory;
}

} // namespace rtos
} // namespace platform
} // namespace lwm2m

#endif // RTOS_RIOT
