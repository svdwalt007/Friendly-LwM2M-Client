/**
 * @file rtos_abstraction.h
 * @brief RTOS Abstraction Layer for Friendly LwM2M Client
 *
 * Provides RTOS-agnostic interfaces for threading, synchronization, timers,
 * and memory management. Enables portability across FreeRTOS, Zephyr, Mbed OS,
 * RIOT, and bare-metal environments.
 *
 * Supported RTOSes:
 * - FreeRTOS (most popular, 35% market share)
 * - Zephyr (Linux Foundation, 25% market share)
 * - Mbed OS (ARM ecosystem, 10% market share)
 * - RIOT (IoT-focused, 8% market share)
 * - Bare-metal (custom schedulers, 10% market share)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_RTOS_ABSTRACTION_H
#define FRIENDLY_LWM2M_RTOS_ABSTRACTION_H

#include <cstdint>
#include <functional>
#include <memory>

namespace lwm2m {
namespace platform {
namespace rtos {

/**
 * @brief Result codes for RTOS operations
 */
enum class RTOSResult {
    SUCCESS = 0,
    ERROR_TIMEOUT,
    ERROR_NO_MEMORY,
    ERROR_INVALID_PARAMETER,
    ERROR_NOT_INITIALIZED,
    ERROR_ALREADY_LOCKED,
    ERROR_NOT_LOCKED,
    ERROR_UNKNOWN
};

/**
 * @brief Thread priority levels (normalized across RTOSes)
 *
 * Priority mapping:
 * - FreeRTOS: 0 (lowest) to configMAX_PRIORITIES-1 (highest)
 * - Zephyr: 0 (highest coop) to 14 (lowest coop), negative (preempt)
 * - Mbed OS: osPriorityIdle to osPriorityRealtime
 * - RIOT: THREAD_PRIORITY_MAIN +/- offsets
 */
enum class ThreadPriority : uint8_t {
    IDLE = 0,           ///< Idle priority (background tasks)
    LOW = 1,            ///< Low priority
    BELOW_NORMAL = 2,   ///< Below normal priority
    NORMAL = 3,         ///< Normal priority (default)
    ABOVE_NORMAL = 4,   ///< Above normal priority
    HIGH = 5,           ///< High priority
    REALTIME = 6        ///< Real-time priority (time-critical tasks)
};

/**
 * @brief Thread state enumeration
 */
enum class ThreadState {
    READY,      ///< Ready to run
    RUNNING,    ///< Currently executing
    BLOCKED,    ///< Blocked (waiting for resource)
    SUSPENDED,  ///< Suspended
    TERMINATED  ///< Terminated
};

// Forward declarations
class IThread;
class IMutex;
class ISemaphore;
class ITimer;
class IMemoryAllocator;

/**
 * @brief Thread function signature
 */
using ThreadFunc = void (*)(void* arg);

/**
 * @brief Timer callback function signature
 */
using TimerCallback = std::function<void()>;

/**
 * @brief Abstract thread interface for RTOS portability
 *
 * Provides cross-RTOS thread management with consistent API across:
 * - FreeRTOS (xTaskCreate)
 * - Zephyr (k_thread_create)
 * - Mbed OS (rtos::Thread)
 * - RIOT (thread_create)
 */
class IThread {
public:
    virtual ~IThread() = default;

    /**
     * @brief Start the thread
     * @return RTOS result code
     */
    virtual RTOSResult start() = 0;

    /**
     * @brief Join (wait for) the thread to complete
     * @param timeoutMs Timeout in milliseconds (0 = infinite)
     * @return RTOS result code
     */
    virtual RTOSResult join(uint32_t timeoutMs = 0) = 0;

    /**
     * @brief Detach the thread (let it run independently)
     * @return RTOS result code
     */
    virtual RTOSResult detach() = 0;

    /**
     * @brief Suspend the thread
     * @return RTOS result code
     */
    virtual RTOSResult suspend() = 0;

    /**
     * @brief Resume the thread
     * @return RTOS result code
     */
    virtual RTOSResult resume() = 0;

    /**
     * @brief Get thread state
     * @return Current thread state
     */
    virtual ThreadState getState() = 0;

    /**
     * @brief Get thread priority
     * @return Current priority level
     */
    virtual ThreadPriority getPriority() = 0;

    /**
     * @brief Set thread priority
     * @param priority New priority level
     * @return RTOS result code
     */
    virtual RTOSResult setPriority(ThreadPriority priority) = 0;

    /**
     * @brief Get stack high water mark (minimum free stack)
     * @return Minimum free stack in bytes, 0 if not supported
     */
    virtual uint32_t getStackHighWaterMark() = 0;

    /**
     * @brief Get thread name
     * @return Thread name string
     */
    virtual const char* getName() = 0;
};

/**
 * @brief Thread manager factory interface
 */
class IThreadManager {
public:
    virtual ~IThreadManager() = default;

    /**
     * @brief Create a new thread
     * @param name Thread name (for debugging)
     * @param func Thread entry function
     * @param arg Argument passed to thread function
     * @param stackSize Stack size in bytes
     * @param priority Thread priority
     * @return Pointer to thread object, nullptr on error
     */
    virtual std::unique_ptr<IThread> createThread(
        const char* name,
        ThreadFunc func,
        void* arg,
        uint32_t stackSize,
        ThreadPriority priority = ThreadPriority::NORMAL
    ) = 0;

    /**
     * @brief Sleep current thread
     * @param ms Milliseconds to sleep
     */
    virtual void sleep(uint32_t ms) = 0;

    /**
     * @brief Yield CPU to other threads
     */
    virtual void yield() = 0;

    /**
     * @brief Get system tick count
     * @return Tick count (typically milliseconds since boot)
     */
    virtual uint64_t getTicks() = 0;

    /**
     * @brief Get tick rate (ticks per second)
     * @return Ticks per second (typically 1000 for 1ms tick)
     */
    virtual uint32_t getTickRate() = 0;

    /**
     * @brief Enter critical section (disable interrupts)
     */
    virtual void enterCritical() = 0;

    /**
     * @brief Exit critical section (enable interrupts)
     */
    virtual void exitCritical() = 0;

    /**
     * @brief Start RTOS scheduler (for bare-metal)
     * @note Only needed for RTOSes that require manual scheduler start
     */
    virtual void startScheduler() = 0;
};

/**
 * @brief Abstract mutex interface
 *
 * Provides cross-RTOS mutual exclusion with consistent API across:
 * - FreeRTOS (xSemaphoreCreateMutex)
 * - Zephyr (k_mutex)
 * - Mbed OS (rtos::Mutex)
 * - RIOT (mutex_t)
 */
class IMutex {
public:
    virtual ~IMutex() = default;

    /**
     * @brief Lock the mutex
     * @param timeoutMs Timeout in milliseconds (UINT32_MAX = infinite)
     * @return true if locked successfully, false on timeout
     */
    virtual bool lock(uint32_t timeoutMs = UINT32_MAX) = 0;

    /**
     * @brief Unlock the mutex
     */
    virtual void unlock() = 0;

    /**
     * @brief Try to lock the mutex without blocking
     * @return true if locked successfully, false if already locked
     */
    virtual bool tryLock() = 0;

    /**
     * @brief Check if mutex is locked
     * @return true if locked, false otherwise
     */
    virtual bool isLocked() = 0;
};

/**
 * @brief RAII lock guard for automatic mutex management
 */
class LockGuard {
public:
    explicit LockGuard(IMutex& mutex) : mutex_(mutex), locked_(false) {
        locked_ = mutex_.lock();
    }

    ~LockGuard() {
        if (locked_) {
            mutex_.unlock();
        }
    }

    // Non-copyable
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

    bool isLocked() const { return locked_; }

private:
    IMutex& mutex_;
    bool locked_;
};

/**
 * @brief Abstract semaphore interface (counting semaphore)
 */
class ISemaphore {
public:
    virtual ~ISemaphore() = default;

    /**
     * @brief Wait on semaphore (decrement count)
     * @param timeoutMs Timeout in milliseconds (UINT32_MAX = infinite)
     * @return true if acquired, false on timeout
     */
    virtual bool wait(uint32_t timeoutMs = UINT32_MAX) = 0;

    /**
     * @brief Signal semaphore (increment count)
     * @return true on success
     */
    virtual bool signal() = 0;

    /**
     * @brief Try to wait without blocking
     * @return true if acquired, false if count is zero
     */
    virtual bool tryWait() = 0;

    /**
     * @brief Get current count
     * @return Semaphore count
     */
    virtual uint32_t getCount() = 0;

    /**
     * @brief Reset semaphore to initial count
     */
    virtual void reset() = 0;
};

/**
 * @brief Abstract timer interface
 */
class ITimer {
public:
    virtual ~ITimer() = default;

    /**
     * @brief Start the timer
     * @param periodMs Timer period in milliseconds
     * @param periodic true for periodic timer, false for one-shot
     * @param callback Callback function to execute on timer expiry
     * @return RTOS result code
     */
    virtual RTOSResult start(uint32_t periodMs, bool periodic, TimerCallback callback) = 0;

    /**
     * @brief Stop the timer
     * @return RTOS result code
     */
    virtual RTOSResult stop() = 0;

    /**
     * @brief Restart the timer with same parameters
     * @return RTOS result code
     */
    virtual RTOSResult restart() = 0;

    /**
     * @brief Check if timer is active
     * @return true if running, false otherwise
     */
    virtual bool isActive() = 0;

    /**
     * @brief Change timer period (while running or stopped)
     * @param periodMs New period in milliseconds
     * @return RTOS result code
     */
    virtual RTOSResult changePeriod(uint32_t periodMs) = 0;

    /**
     * @brief Get timer period
     * @return Period in milliseconds
     */
    virtual uint32_t getPeriod() = 0;
};

/**
 * @brief Abstract memory allocator interface
 *
 * Provides RTOS-aware memory allocation with heap tracking
 */
class IMemoryAllocator {
public:
    virtual ~IMemoryAllocator() = default;

    /**
     * @brief Allocate memory
     * @param size Size in bytes
     * @return Pointer to allocated memory, nullptr on failure
     */
    virtual void* allocate(size_t size) = 0;

    /**
     * @brief Free memory
     * @param ptr Pointer to memory to free
     */
    virtual void free(void* ptr) = 0;

    /**
     * @brief Reallocate memory
     * @param ptr Pointer to existing memory (nullptr = allocate)
     * @param newSize New size in bytes
     * @return Pointer to reallocated memory, nullptr on failure
     */
    virtual void* reallocate(void* ptr, size_t newSize) = 0;

    /**
     * @brief Get total heap size
     * @return Total heap size in bytes
     */
    virtual size_t getTotalHeap() = 0;

    /**
     * @brief Get current free heap
     * @return Free heap size in bytes
     */
    virtual size_t getFreeHeap() = 0;

    /**
     * @brief Get minimum ever free heap (low water mark)
     * @return Minimum free heap size in bytes
     */
    virtual size_t getMinEverFreeHeap() = 0;

    /**
     * @brief Get largest free block
     * @return Size of largest contiguous free block in bytes
     */
    virtual size_t getLargestFreeBlock() = 0;

    /**
     * @brief Check if heap is thread-safe
     * @return true if malloc/free are thread-safe
     */
    virtual bool isThreadSafe() = 0;
};

/**
 * @brief Message queue interface (for inter-thread communication)
 */
template<typename T>
class IMessageQueue {
public:
    virtual ~IMessageQueue() = default;

    /**
     * @brief Send message to queue
     * @param message Message to send
     * @param timeoutMs Timeout in milliseconds (UINT32_MAX = infinite)
     * @return true if sent successfully, false on timeout
     */
    virtual bool send(const T& message, uint32_t timeoutMs = UINT32_MAX) = 0;

    /**
     * @brief Receive message from queue
     * @param message Output message
     * @param timeoutMs Timeout in milliseconds (UINT32_MAX = infinite)
     * @return true if received successfully, false on timeout
     */
    virtual bool receive(T& message, uint32_t timeoutMs = UINT32_MAX) = 0;

    /**
     * @brief Get number of messages in queue
     * @return Message count
     */
    virtual uint32_t getCount() = 0;

    /**
     * @brief Get queue capacity
     * @return Maximum number of messages
     */
    virtual uint32_t getCapacity() = 0;

    /**
     * @brief Check if queue is empty
     * @return true if empty
     */
    virtual bool isEmpty() = 0;

    /**
     * @brief Check if queue is full
     * @return true if full
     */
    virtual bool isFull() = 0;

    /**
     * @brief Clear all messages from queue
     */
    virtual void clear() = 0;
};

/**
 * @brief RTOS platform factory
 *
 * Creates RTOS-specific implementations of abstract interfaces
 */
class RTOSFactory {
public:
    virtual ~RTOSFactory() = default;

    /**
     * @brief Get thread manager instance
     * @return Thread manager
     */
    virtual IThreadManager& getThreadManager() = 0;

    /**
     * @brief Create mutex
     * @return Unique pointer to mutex
     */
    virtual std::unique_ptr<IMutex> createMutex() = 0;

    /**
     * @brief Create semaphore
     * @param initialCount Initial count value
     * @param maxCount Maximum count value
     * @return Unique pointer to semaphore
     */
    virtual std::unique_ptr<ISemaphore> createSemaphore(
        uint32_t initialCount = 0,
        uint32_t maxCount = 1
    ) = 0;

    /**
     * @brief Create timer
     * @param name Timer name (for debugging)
     * @return Unique pointer to timer
     */
    virtual std::unique_ptr<ITimer> createTimer(const char* name) = 0;

    /**
     * @brief Get memory allocator instance
     * @return Memory allocator
     */
    virtual IMemoryAllocator& getMemoryAllocator() = 0;

    /**
     * @brief Get RTOS name
     * @return RTOS name string ("FreeRTOS", "Zephyr", etc.)
     */
    virtual const char* getRTOSName() = 0;

    /**
     * @brief Get RTOS version
     * @return Version string
     */
    virtual const char* getRTOSVersion() = 0;
};

/**
 * @brief Get global RTOS factory instance
 *
 * Implementation provided by platform-specific adapter
 * (freertos_adapter.cpp, zephyr_adapter.cpp, etc.)
 */
RTOSFactory& getRTOSFactory();

} // namespace rtos
} // namespace platform
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_RTOS_ABSTRACTION_H
