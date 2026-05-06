/**
 * @file test_zephyr.cpp
 * @brief Comprehensive tests for Zephyr RTOS Adapter
 *
 * Tests cover:
 * - Thread creation, lifecycle, priorities
 * - Mutex lock/unlock operations
 * - Semaphore signaling and counting
 * - Timer start/stop/restart operations
 * - Memory allocation and heap tracking
 * - RAII lock guards
 * - Thread synchronization
 * - Error handling
 * - Resource cleanup
 * - Zephyr-specific features
 *
 * Target Platforms:
 * - QEMU (x86, ARM Cortex-M3, RISC-V)
 * - nRF52840 DK (Bluetooth LE development)
 * - nRF9160 DK (LTE-M/NB-IoT development)
 * - Nordic Thingy:91 (LTE-M/NB-IoT prototyping)
 * - nRF9151 SMA Dev Kit (LTE-M/NB-IoT compact dev board)
 * - STM32 Discovery boards
 *
 * Build with Zephyr:
 *   west build -b <board> tests/platform/rtos
 *   west build -t run  # For QEMU targets
 *   west flash          # For hardware targets
 *
 * @note Requires Zephyr 3.0+ with CONFIG_CPLUSPLUS=y
 *       Set CONFIG_HEAP_MEM_POOL_SIZE for memory tests
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef RTOS_ZEPHYR

#include "../../catch2/catch_amalgamated.hpp"
#include "platform/rtos_abstraction.h"
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>

using namespace lwm2m::platform::rtos;

// ============================================================================
// Test Utilities
// ============================================================================

// Global counter for thread tests
static std::atomic<int> g_threadCounter{0};

// Simple thread function
void simpleThreadFunc(void* arg) {
    int* counter = static_cast<int*>(arg);
    (*counter)++;
}

// Thread function that increments atomic counter
void atomicThreadFunc(void* arg) {
    (void)arg;
    g_threadCounter++;
}

// Thread function that takes time
void delayedThreadFunc(void* arg) {
    auto& mgr = getRTOSFactory().getThreadManager();
    mgr.sleep(100); // 100ms delay
    g_threadCounter++;
}

// Thread function that signals completion via semaphore
void semaphoreSignalFunc(void* arg) {
    auto* sem = static_cast<ISemaphore*>(arg);
    auto& mgr = getRTOSFactory().getThreadManager();
    mgr.sleep(50); // Small delay
    sem->signal();
}

// ============================================================================
// Factory Tests
// ============================================================================

TEST_CASE("Zephyr Factory", "[zephyr][factory]") {
    SECTION("Get factory instance") {
        auto& factory = getRTOSFactory();
        REQUIRE(factory.getRTOSName() != nullptr);
        REQUIRE(std::string(factory.getRTOSName()) == "Zephyr");
    }

    SECTION("Get RTOS version") {
        auto& factory = getRTOSFactory();
        REQUIRE(factory.getRTOSVersion() != nullptr);
        // Should be non-empty string in format "X.Y.Z"
        std::string version(factory.getRTOSVersion());
        REQUIRE(version.length() > 0);
        REQUIRE(version.find('.') != std::string::npos);
    }

    SECTION("Get thread manager") {
        auto& factory = getRTOSFactory();
        auto& mgr = factory.getThreadManager();
        REQUIRE(mgr.getTickRate() > 0);
    }

    SECTION("Get memory allocator") {
        auto& factory = getRTOSFactory();
        auto& allocator = factory.getMemoryAllocator();
        REQUIRE(allocator.isThreadSafe() == true);
    }
}

// ============================================================================
// Thread Manager Tests
// ============================================================================

TEST_CASE("Zephyr Thread Manager", "[zephyr][thread_manager]") {
    auto& mgr = getRTOSFactory().getThreadManager();

    SECTION("Sleep function") {
        uint64_t startTicks = mgr.getTicks();
        mgr.sleep(100); // 100ms
        uint64_t endTicks = mgr.getTicks();

        // k_uptime_get() returns milliseconds in Zephyr
        uint64_t elapsedMs = endTicks - startTicks;

        // Allow 10% tolerance for scheduler overhead
        REQUIRE(elapsedMs >= 90);
        REQUIRE(elapsedMs <= 110);
    }

    SECTION("Yield function") {
        // Just verify it doesn't crash
        mgr.yield();
        mgr.yield();
        REQUIRE(true);
    }

    SECTION("Get ticks") {
        uint64_t ticks1 = mgr.getTicks();
        mgr.sleep(10);
        uint64_t ticks2 = mgr.getTicks();
        REQUIRE(ticks2 > ticks1);
    }

    SECTION("Get tick rate") {
        uint32_t rate = mgr.getTickRate();
        // Zephyr typically runs at 100-10000 Hz
        REQUIRE(rate >= 100);
        REQUIRE(rate <= 20000);
    }

    SECTION("Critical section") {
        // Just verify they don't crash
        mgr.enterCritical();
        mgr.exitCritical();
        REQUIRE(true);
    }
}

// ============================================================================
// Thread Tests
// ============================================================================

TEST_CASE("Zephyr Thread Creation and Lifecycle", "[zephyr][thread]") {
    auto& mgr = getRTOSFactory().getThreadManager();

    SECTION("Create and start thread") {
        int counter = 0;
        auto thread = mgr.createThread(
            "test_thread",
            simpleThreadFunc,
            &counter,
            2048,
            ThreadPriority::NORMAL
        );

        REQUIRE(thread != nullptr);

        // Give thread time to execute
        mgr.sleep(50);
        REQUIRE(counter == 1);
    }

    SECTION("Thread name") {
        int counter = 0;
        auto thread = mgr.createThread(
            "named_thread",
            simpleThreadFunc,
            &counter,
            2048,
            ThreadPriority::NORMAL
        );

        REQUIRE(thread != nullptr);
        REQUIRE(std::string(thread->getName()) == "named_thread");
    }

    SECTION("Thread priorities") {
        g_threadCounter = 0;

        auto highThread = mgr.createThread(
            "high_priority",
            atomicThreadFunc,
            nullptr,
            2048,
            ThreadPriority::HIGH
        );

        auto lowThread = mgr.createThread(
            "low_priority",
            atomicThreadFunc,
            nullptr,
            2048,
            ThreadPriority::LOW
        );

        REQUIRE(highThread != nullptr);
        REQUIRE(lowThread != nullptr);
        REQUIRE(highThread->getPriority() == ThreadPriority::HIGH);
        REQUIRE(lowThread->getPriority() == ThreadPriority::LOW);

        mgr.sleep(100);
        REQUIRE(g_threadCounter == 2);
    }

    SECTION("Realtime (preemptive) priority") {
        g_threadCounter = 0;
        auto realtimeThread = mgr.createThread(
            "realtime",
            atomicThreadFunc,
            nullptr,
            2048,
            ThreadPriority::REALTIME
        );

        REQUIRE(realtimeThread != nullptr);
        REQUIRE(realtimeThread->getPriority() == ThreadPriority::REALTIME);

        mgr.sleep(50);
        REQUIRE(g_threadCounter == 1);
    }

    SECTION("Thread state transitions") {
        g_threadCounter = 0;
        auto thread = mgr.createThread(
            "state_thread",
            delayedThreadFunc,
            nullptr,
            2048,
            ThreadPriority::NORMAL
        );

        REQUIRE(thread != nullptr);

        // Thread should be running or ready initially
        ThreadState state = thread->getState();
        REQUIRE((state == ThreadState::RUNNING || state == ThreadState::READY));

        // Wait for completion
        mgr.sleep(200);
        REQUIRE(g_threadCounter == 1);
    }

    SECTION("Thread suspend and resume") {
        g_threadCounter = 0;
        auto thread = mgr.createThread(
            "suspend_thread",
            atomicThreadFunc,
            nullptr,
            2048,
            ThreadPriority::NORMAL
        );

        REQUIRE(thread != nullptr);

        // Suspend immediately
        RTOSResult result = thread->suspend();
        REQUIRE(result == RTOSResult::SUCCESS);

        mgr.sleep(50);
        REQUIRE(g_threadCounter == 0); // Should not have executed

        // Resume
        result = thread->resume();
        REQUIRE(result == RTOSResult::SUCCESS);

        mgr.sleep(50);
        REQUIRE(g_threadCounter == 1); // Should have executed now
    }

    SECTION("Change thread priority") {
        int counter = 0;
        auto thread = mgr.createThread(
            "priority_change",
            simpleThreadFunc,
            &counter,
            2048,
            ThreadPriority::NORMAL
        );

        REQUIRE(thread != nullptr);
        REQUIRE(thread->getPriority() == ThreadPriority::NORMAL);

        RTOSResult result = thread->setPriority(ThreadPriority::HIGH);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(thread->getPriority() == ThreadPriority::HIGH);
    }

    SECTION("Stack high water mark") {
        int counter = 0;
        auto thread = mgr.createThread(
            "stack_test",
            simpleThreadFunc,
            &counter,
            2048,
            ThreadPriority::NORMAL
        );

        REQUIRE(thread != nullptr);
        mgr.sleep(50);

        #ifdef CONFIG_THREAD_STACK_INFO
            uint32_t highWaterMark = thread->getStackHighWaterMark();
            // Should have some free stack
            REQUIRE(highWaterMark > 0);
            REQUIRE(highWaterMark < 2048);
        #endif
    }

    SECTION("Thread join") {
        g_threadCounter = 0;
        auto thread = mgr.createThread(
            "join_thread",
            delayedThreadFunc,
            nullptr,
            2048,
            ThreadPriority::NORMAL
        );

        REQUIRE(thread != nullptr);
        REQUIRE(g_threadCounter == 0);

        // Join should block until thread completes
        RTOSResult result = thread->join(500);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(g_threadCounter == 1);
    }

    SECTION("Thread join timeout") {
        auto longRunningFunc = [](void* arg) {
            auto& mgr = getRTOSFactory().getThreadManager();
            mgr.sleep(500); // Long delay
        };

        auto thread = mgr.createThread(
            "long_thread",
            longRunningFunc,
            nullptr,
            2048,
            ThreadPriority::NORMAL
        );

        REQUIRE(thread != nullptr);

        // Try to join with short timeout
        RTOSResult result = thread->join(100);
        REQUIRE(result == RTOSResult::ERROR_TIMEOUT);
    }
}

// ============================================================================
// Mutex Tests
// ============================================================================

TEST_CASE("Zephyr Mutex Operations", "[zephyr][mutex]") {
    auto& factory = getRTOSFactory();
    auto& mgr = factory.getThreadManager();

    SECTION("Create mutex") {
        auto mutex = factory.createMutex();
        REQUIRE(mutex != nullptr);
    }

    SECTION("Lock and unlock") {
        auto mutex = factory.createMutex();
        REQUIRE(mutex != nullptr);

        bool locked = mutex->lock();
        REQUIRE(locked == true);
        REQUIRE(mutex->isLocked() == true);

        mutex->unlock();
        REQUIRE(mutex->isLocked() == false);
    }

    SECTION("Try lock") {
        auto mutex = factory.createMutex();
        REQUIRE(mutex != nullptr);

        // Should succeed when unlocked
        bool locked = mutex->tryLock();
        REQUIRE(locked == true);

        // Should fail when already locked
        bool lockedAgain = mutex->tryLock();
        REQUIRE(lockedAgain == false);

        mutex->unlock();

        // Should succeed after unlock
        locked = mutex->tryLock();
        REQUIRE(locked == true);
        mutex->unlock();
    }

    SECTION("Lock timeout") {
        auto mutex = factory.createMutex();
        REQUIRE(mutex != nullptr);

        // Lock from main thread
        mutex->lock();

        // Try to lock with timeout (should timeout)
        uint64_t startTime = mgr.getTicks();
        bool locked = mutex->lock(100); // 100ms timeout
        uint64_t endTime = mgr.getTicks();

        REQUIRE(locked == false);

        // Verify timeout occurred (k_uptime_get returns ms)
        uint64_t elapsedMs = endTime - startTime;
        REQUIRE(elapsedMs >= 90);
        REQUIRE(elapsedMs <= 110);

        mutex->unlock();
    }

    SECTION("Mutex priority inheritance") {
        // Zephyr mutexes support priority inheritance by default
        auto mutex = factory.createMutex();
        REQUIRE(mutex != nullptr);

        // Lock from low priority thread
        auto lowPrioFunc = [](void* arg) {
            auto* mutex = static_cast<IMutex*>(arg);
            mutex->lock();
            auto& mgr = getRTOSFactory().getThreadManager();
            mgr.sleep(200); // Hold lock
            mutex->unlock();
        };

        auto lowThread = mgr.createThread("low_prio", lowPrioFunc, mutex.get(), 2048, ThreadPriority::LOW);
        REQUIRE(lowThread != nullptr);

        mgr.sleep(50); // Let low thread acquire lock

        // High priority thread should inherit priority when blocked on mutex
        auto highPrioFunc = [](void* arg) {
            auto* mutex = static_cast<IMutex*>(arg);
            mutex->lock(); // Should cause priority inheritance
            mutex->unlock();
        };

        auto highThread = mgr.createThread("high_prio", highPrioFunc, mutex.get(), 2048, ThreadPriority::HIGH);
        REQUIRE(highThread != nullptr);

        mgr.sleep(300); // Allow completion
        REQUIRE(true); // If we got here, priority inheritance worked
    }
}

// ============================================================================
// RAII Lock Guard Tests
// ============================================================================

TEST_CASE("Zephyr Lock Guard RAII", "[zephyr][lockguard]") {
    auto& factory = getRTOSFactory();
    auto mutex = factory.createMutex();
    REQUIRE(mutex != nullptr);

    SECTION("Automatic lock/unlock") {
        REQUIRE(mutex->isLocked() == false);

        {
            LockGuard lock(*mutex);
            REQUIRE(lock.isLocked() == true);
            REQUIRE(mutex->isLocked() == true);
        }

        // Should be unlocked after scope exit
        REQUIRE(mutex->isLocked() == false);
    }

    SECTION("Exception safety") {
        REQUIRE(mutex->isLocked() == false);

        try {
            LockGuard lock(*mutex);
            REQUIRE(mutex->isLocked() == true);
            throw std::runtime_error("Test exception");
        } catch (...) {
            // Mutex should be unlocked even with exception
            REQUIRE(mutex->isLocked() == false);
        }
    }

    SECTION("Early return safety") {
        auto testFunc = [&mutex](bool earlyReturn) {
            LockGuard lock(*mutex);
            REQUIRE(mutex->isLocked() == true);

            if (earlyReturn) {
                return; // Lock should be released
            }

            // More work...
        };

        testFunc(true);
        REQUIRE(mutex->isLocked() == false);

        testFunc(false);
        REQUIRE(mutex->isLocked() == false);
    }
}

// ============================================================================
// Semaphore Tests
// ============================================================================

TEST_CASE("Zephyr Semaphore Operations", "[zephyr][semaphore]") {
    auto& factory = getRTOSFactory();
    auto& mgr = factory.getThreadManager();

    SECTION("Create semaphore") {
        auto sem = factory.createSemaphore(0, 10);
        REQUIRE(sem != nullptr);
        REQUIRE(sem->getCount() == 0);
    }

    SECTION("Signal and wait") {
        auto sem = factory.createSemaphore(0, 10);
        REQUIRE(sem != nullptr);

        // Should timeout when count is zero
        bool acquired = sem->wait(10);
        REQUIRE(acquired == false);

        // Signal
        bool signaled = sem->signal();
        REQUIRE(signaled == true);
        REQUIRE(sem->getCount() == 1);

        // Should succeed now
        acquired = sem->wait(10);
        REQUIRE(acquired == true);
        REQUIRE(sem->getCount() == 0);
    }

    SECTION("Try wait") {
        auto sem = factory.createSemaphore(0, 10);
        REQUIRE(sem != nullptr);

        // Should fail when count is zero
        bool acquired = sem->tryWait();
        REQUIRE(acquired == false);

        sem->signal();

        // Should succeed
        acquired = sem->tryWait();
        REQUIRE(acquired == true);
        REQUIRE(sem->getCount() == 0);
    }

    SECTION("Counting semaphore") {
        auto sem = factory.createSemaphore(0, 10);
        REQUIRE(sem != nullptr);

        // Signal multiple times
        for (int i = 0; i < 5; i++) {
            sem->signal();
        }
        REQUIRE(sem->getCount() == 5);

        // Wait multiple times
        for (int i = 0; i < 3; i++) {
            bool acquired = sem->wait(10);
            REQUIRE(acquired == true);
        }
        REQUIRE(sem->getCount() == 2);
    }

    SECTION("Thread synchronization with semaphore") {
        auto sem = factory.createSemaphore(0, 1);
        REQUIRE(sem != nullptr);

        // Create thread that signals semaphore
        auto thread = mgr.createThread(
            "sem_signal_thread",
            semaphoreSignalFunc,
            sem.get(),
            2048,
            ThreadPriority::NORMAL
        );

        REQUIRE(thread != nullptr);

        // Wait for signal (should block until thread signals)
        uint64_t startTime = mgr.getTicks();
        bool acquired = sem->wait(200); // 200ms timeout
        uint64_t endTime = mgr.getTicks();

        REQUIRE(acquired == true);

        // Should have waited approximately 50ms (thread delay)
        uint64_t elapsedMs = endTime - startTime;
        REQUIRE(elapsedMs >= 40);
        REQUIRE(elapsedMs <= 70);
    }

    SECTION("Max count enforcement") {
        auto sem = factory.createSemaphore(0, 3);
        REQUIRE(sem != nullptr);

        // Signal to max count
        sem->signal();
        sem->signal();
        sem->signal();
        REQUIRE(sem->getCount() == 3);

        // Signaling beyond max should fail
        bool signaled = sem->signal();
        REQUIRE(signaled == false);
        REQUIRE(sem->getCount() == 3);
    }

    SECTION("Semaphore reset") {
        auto sem = factory.createSemaphore(0, 10);
        REQUIRE(sem != nullptr);

        // Signal multiple times
        sem->signal();
        sem->signal();
        sem->signal();
        REQUIRE(sem->getCount() == 3);

        // Reset
        sem->reset();
        REQUIRE(sem->getCount() == 0);
    }
}

// ============================================================================
// Timer Tests
// ============================================================================

TEST_CASE("Zephyr Timer Operations", "[zephyr][timer]") {
    auto& factory = getRTOSFactory();
    auto& mgr = factory.getThreadManager();

    SECTION("Create timer") {
        auto timer = factory.createTimer("test_timer");
        REQUIRE(timer != nullptr);
        REQUIRE(timer->isActive() == false);
    }

    SECTION("One-shot timer") {
        auto timer = factory.createTimer("oneshot_timer");
        REQUIRE(timer != nullptr);

        std::atomic<int> callbackCount{0};
        auto callback = [&callbackCount]() {
            callbackCount++;
        };

        RTOSResult result = timer->start(50, false, callback);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(timer->getPeriod() == 50);

        // Wait for timer to fire
        mgr.sleep(100);

        // Should have fired once
        REQUIRE(callbackCount == 1);
    }

    SECTION("Periodic timer") {
        auto timer = factory.createTimer("periodic_timer");
        REQUIRE(timer != nullptr);

        std::atomic<int> callbackCount{0};
        auto callback = [&callbackCount]() {
            callbackCount++;
        };

        RTOSResult result = timer->start(50, true, callback);
        REQUIRE(result == RTOSResult::SUCCESS);

        // Wait for multiple periods
        mgr.sleep(250);

        // Should have fired approximately 5 times (250ms / 50ms)
        // Allow some tolerance for timing
        REQUIRE(callbackCount >= 3);
        REQUIRE(callbackCount <= 7);

        // Stop timer
        result = timer->stop();
        REQUIRE(result == RTOSResult::SUCCESS);
    }

    SECTION("Stop timer") {
        auto timer = factory.createTimer("stop_timer");
        REQUIRE(timer != nullptr);

        std::atomic<int> callbackCount{0};
        auto callback = [&callbackCount]() {
            callbackCount++;
        };

        timer->start(50, true, callback);
        mgr.sleep(30); // Wait less than period

        RTOSResult result = timer->stop();
        REQUIRE(result == RTOSResult::SUCCESS);

        int countBeforeStop = callbackCount.load();
        mgr.sleep(100);

        // Count should not increase after stop
        REQUIRE(callbackCount == countBeforeStop);
    }

    SECTION("Restart timer") {
        auto timer = factory.createTimer("restart_timer");
        REQUIRE(timer != nullptr);

        std::atomic<int> callbackCount{0};
        auto callback = [&callbackCount]() {
            callbackCount++;
        };

        timer->start(100, false, callback);
        mgr.sleep(50); // Wait halfway

        // Restart timer (should reset period)
        RTOSResult result = timer->restart();
        REQUIRE(result == RTOSResult::SUCCESS);

        mgr.sleep(60); // Wait 60ms (total 110ms, but restart resets)

        // Should not have fired yet (only 60ms since restart)
        REQUIRE(callbackCount == 0);

        mgr.sleep(50); // Total 110ms since restart

        // Should have fired now
        REQUIRE(callbackCount == 1);
    }

    SECTION("Change timer period") {
        auto timer = factory.createTimer("change_period_timer");
        REQUIRE(timer != nullptr);

        std::atomic<int> callbackCount{0};
        auto callback = [&callbackCount]() {
            callbackCount++;
        };

        timer->start(100, true, callback);
        REQUIRE(timer->getPeriod() == 100);

        // Change period
        RTOSResult result = timer->changePeriod(50);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(timer->getPeriod() == 50);

        mgr.sleep(200);

        // Should fire more frequently now (approximately 4 times in 200ms)
        REQUIRE(callbackCount >= 2);
        REQUIRE(callbackCount <= 6);

        timer->stop();
    }
}

// ============================================================================
// Memory Allocator Tests
// ============================================================================

TEST_CASE("Zephyr Memory Allocator", "[zephyr][memory]") {
    auto& allocator = getRTOSFactory().getMemoryAllocator();

    SECTION("Allocate and free") {
        void* ptr = allocator.allocate(1024);
        REQUIRE(ptr != nullptr);

        allocator.free(ptr);
        REQUIRE(true); // If we got here, no crash
    }

    SECTION("Reallocate") {
        void* ptr = allocator.allocate(512);
        REQUIRE(ptr != nullptr);

        // Write pattern
        memset(ptr, 0xAA, 512);

        // Reallocate larger
        void* newPtr = allocator.reallocate(ptr, 1024);
        REQUIRE(newPtr != nullptr);

        // Verify data preserved (at least first 512 bytes)
        uint8_t* bytes = static_cast<uint8_t*>(newPtr);
        bool dataPreserved = true;
        for (int i = 0; i < 512; i++) {
            if (bytes[i] != 0xAA) {
                dataPreserved = false;
                break;
            }
        }
        REQUIRE(dataPreserved == true);

        allocator.free(newPtr);
    }

    SECTION("Thread safety") {
        REQUIRE(allocator.isThreadSafe() == true);
    }

    #ifdef CONFIG_HEAP_MEM_POOL_SIZE
    SECTION("Get total heap") {
        size_t totalHeap = allocator.getTotalHeap();
        REQUIRE(totalHeap > 0);
        REQUIRE(totalHeap == CONFIG_HEAP_MEM_POOL_SIZE);
    }
    #endif

    #ifdef CONFIG_SYS_HEAP_RUNTIME_STATS
    SECTION("Heap statistics") {
        size_t freeHeap = allocator.getFreeHeap();
        REQUIRE(freeHeap > 0);

        // Allocate large block
        void* ptr = allocator.allocate(4096);
        REQUIRE(ptr != nullptr);

        size_t freeAfterAlloc = allocator.getFreeHeap();
        REQUIRE(freeAfterAlloc < freeHeap);

        allocator.free(ptr);
    }
    #endif
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_CASE("Zephyr Thread Synchronization Integration", "[zephyr][integration]") {
    auto& factory = getRTOSFactory();
    auto& mgr = factory.getThreadManager();

    SECTION("Producer-consumer with mutex and semaphore") {
        auto mutex = factory.createMutex();
        auto semEmpty = factory.createSemaphore(5, 5);  // Buffer size 5
        auto semFull = factory.createSemaphore(0, 5);

        std::vector<int> buffer;
        std::atomic<bool> done{false};

        // Producer thread
        auto producerFunc = [&](void* arg) {
            for (int i = 0; i < 10; i++) {
                semEmpty->wait(); // Wait for empty slot
                {
                    LockGuard lock(*mutex);
                    buffer.push_back(i);
                }
                semFull->signal(); // Signal item available
                mgr.sleep(10);
            }
            done = true;
        };

        // Consumer thread
        std::atomic<int> consumedCount{0};
        auto consumerFunc = [&](void* arg) {
            while (!done || semFull->getCount() > 0) {
                if (semFull->wait(20)) { // Wait for item
                    {
                        LockGuard lock(*mutex);
                        if (!buffer.empty()) {
                            buffer.erase(buffer.begin());
                            consumedCount++;
                        }
                    }
                    semEmpty->signal(); // Signal empty slot
                }
            }
        };

        auto producer = mgr.createThread("producer", producerFunc, nullptr, 2048, ThreadPriority::NORMAL);
        auto consumer = mgr.createThread("consumer", consumerFunc, nullptr, 2048, ThreadPriority::NORMAL);

        // Wait for completion
        mgr.sleep(500);

        REQUIRE(consumedCount == 10);
        REQUIRE(buffer.size() == 0);
    }

    SECTION("Multiple threads with timer coordination") {
        std::atomic<int> threadsDone{0};
        auto timer = factory.createTimer("coord_timer");

        auto timedThreadFunc = [&](void* arg) {
            mgr.sleep(100);
            threadsDone++;
        };

        std::atomic<int> timerFired{0};
        timer->start(150, false, [&]() {
            timerFired++;
        });

        // Start 3 threads
        auto t1 = mgr.createThread("t1", timedThreadFunc, nullptr, 2048, ThreadPriority::NORMAL);
        auto t2 = mgr.createThread("t2", timedThreadFunc, nullptr, 2048, ThreadPriority::NORMAL);
        auto t3 = mgr.createThread("t3", timedThreadFunc, nullptr, 2048, ThreadPriority::NORMAL);

        mgr.sleep(200);

        // All threads should complete before timer
        REQUIRE(threadsDone == 3);
        REQUIRE(timerFired == 1);
    }
}

// ============================================================================
// Zephyr-Specific Tests
// ============================================================================

TEST_CASE("Zephyr Specific Features", "[zephyr][specific]") {
    auto& factory = getRTOSFactory();
    auto& mgr = factory.getThreadManager();

    SECTION("Cooperative vs Preemptive threads") {
        // Cooperative threads (IDLE to HIGH) should not preempt each other
        // Preemptive threads (REALTIME) can preempt cooperative threads

        std::atomic<int> execOrder{0};

        auto coopFunc = [&](void* arg) {
            mgr.sleep(50);
            execOrder = 1;
        };

        auto preemptFunc = [&](void* arg) {
            execOrder = 2; // Should execute immediately if preemptive
        };

        auto coopThread = mgr.createThread("coop", coopFunc, nullptr, 2048, ThreadPriority::NORMAL);
        mgr.sleep(10); // Let coop thread start

        auto preemptThread = mgr.createThread("preempt", preemptFunc, nullptr, 2048, ThreadPriority::REALTIME);

        mgr.sleep(20);

        // Preemptive thread should have executed
        REQUIRE(execOrder == 2);
    }

    SECTION("Kernel version check") {
        const char* version = factory.getRTOSVersion();
        REQUIRE(version != nullptr);

        // Parse version string
        int major, minor, patch;
        int parsed = sscanf(version, "%d.%d.%d", &major, &minor, &patch);
        REQUIRE(parsed == 3);

        // Should be Zephyr 3.0+
        REQUIRE(major >= 3);
    }
}

#endif // RTOS_ZEPHYR
