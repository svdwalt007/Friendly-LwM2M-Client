/**
 * @file test_mbed.cpp
 * @brief Comprehensive tests for Mbed OS RTOS Adapter
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
 * - Mbed-specific features
 *
 * Target Platforms:
 * - STM32 Nucleo boards (F429ZI, L476RG, F746ZG)
 * - STM32 Discovery boards (F407, F746)
 * - Nordic nRF52840 DK
 * - NXP FRDM-K64F
 * - Raspberry Pi Pico (RP2040)
 *
 * Build with Mbed CLI 2:
 *   mbed-tools compile -m NUCLEO_F429ZI -t GCC_ARM
 *   mbed-tools compile -m NRF52840_DK -t GCC_ARM
 *
 * @note Requires Mbed OS 6.x with C++ support
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef RTOS_MBED

#include "../../catch2/catch_amalgamated.hpp"
#include "platform/rtos_abstraction.h"
#include <atomic>
#include <chrono>
#include <vector>

using namespace lwm2m::platform::rtos;

// ============================================================================
// Test Utilities
// ============================================================================

static std::atomic<int> g_threadCounter{0};

void simpleThreadFunc(void* arg) {
    int* counter = static_cast<int*>(arg);
    (*counter)++;
}

void atomicThreadFunc(void* arg) {
    (void)arg;
    g_threadCounter++;
}

void delayedThreadFunc(void* arg) {
    auto& mgr = getRTOSFactory().getThreadManager();
    mgr.sleep(100);
    g_threadCounter++;
}

void semaphoreSignalFunc(void* arg) {
    auto* sem = static_cast<ISemaphore*>(arg);
    auto& mgr = getRTOSFactory().getThreadManager();
    mgr.sleep(50);
    sem->signal();
}

// ============================================================================
// Factory Tests
// ============================================================================

TEST_CASE("Mbed Factory", "[mbed][factory]") {
    SECTION("Get factory instance") {
        auto& factory = getRTOSFactory();
        REQUIRE(factory.getRTOSName() != nullptr);
        REQUIRE(std::string(factory.getRTOSName()) == "Mbed OS");
    }

    SECTION("Get RTOS version") {
        auto& factory = getRTOSFactory();
        REQUIRE(factory.getRTOSVersion() != nullptr);
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

TEST_CASE("Mbed Thread Manager", "[mbed][thread_manager]") {
    auto& mgr = getRTOSFactory().getThreadManager();

    SECTION("Sleep function") {
        uint64_t startTicks = mgr.getTicks();
        mgr.sleep(100);
        uint64_t endTicks = mgr.getTicks();

        uint64_t elapsedTicks = endTicks - startTicks;
        uint32_t tickRate = mgr.getTickRate();
        uint64_t expectedTicks = (100 * tickRate) / 1000;

        REQUIRE(elapsedTicks >= expectedTicks * 0.9);
        REQUIRE(elapsedTicks <= expectedTicks * 1.1);
    }

    SECTION("Yield function") {
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
        REQUIRE(rate >= 100);
        REQUIRE(rate <= 10000);
    }

    SECTION("Critical section") {
        mgr.enterCritical();
        mgr.exitCritical();
        REQUIRE(true);
    }
}

// ============================================================================
// Thread Tests
// ============================================================================

TEST_CASE("Mbed Thread Creation and Lifecycle", "[mbed][thread]") {
    auto& mgr = getRTOSFactory().getThreadManager();

    SECTION("Create and start thread") {
        int counter = 0;
        auto thread = mgr.createThread("test_thread", simpleThreadFunc, &counter, 2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);
        mgr.sleep(50);
        REQUIRE(counter == 1);
    }

    SECTION("Thread name") {
        int counter = 0;
        auto thread = mgr.createThread("named_thread", simpleThreadFunc, &counter, 2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);
        REQUIRE(std::string(thread->getName()) == "named_thread");
    }

    SECTION("Thread priorities") {
        g_threadCounter = 0;
        auto highThread = mgr.createThread("high_priority", atomicThreadFunc, nullptr, 2048, ThreadPriority::HIGH);
        auto lowThread = mgr.createThread("low_priority", atomicThreadFunc, nullptr, 2048, ThreadPriority::LOW);

        REQUIRE(highThread != nullptr);
        REQUIRE(lowThread != nullptr);
        REQUIRE(highThread->getPriority() == ThreadPriority::HIGH);
        REQUIRE(lowThread->getPriority() == ThreadPriority::LOW);

        mgr.sleep(100);
        REQUIRE(g_threadCounter == 2);
    }

    SECTION("All priority levels") {
        std::vector<ThreadPriority> priorities = {
            ThreadPriority::IDLE,
            ThreadPriority::LOW,
            ThreadPriority::BELOW_NORMAL,
            ThreadPriority::NORMAL,
            ThreadPriority::ABOVE_NORMAL,
            ThreadPriority::HIGH,
            ThreadPriority::REALTIME
        };

        for (auto priority : priorities) {
            g_threadCounter = 0;
            auto thread = mgr.createThread("prio_test", atomicThreadFunc, nullptr, 2048, priority);
            REQUIRE(thread != nullptr);
            REQUIRE(thread->getPriority() == priority);
            mgr.sleep(50);
            REQUIRE(g_threadCounter == 1);
        }
    }

    SECTION("Thread state transitions") {
        g_threadCounter = 0;
        auto thread = mgr.createThread("state_thread", delayedThreadFunc, nullptr, 2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        ThreadState state = thread->getState();
        REQUIRE((state == ThreadState::RUNNING || state == ThreadState::READY));

        mgr.sleep(200);
        REQUIRE(g_threadCounter == 1);
    }

    SECTION("Change thread priority") {
        int counter = 0;
        auto thread = mgr.createThread("priority_change", simpleThreadFunc, &counter, 2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);
        REQUIRE(thread->getPriority() == ThreadPriority::NORMAL);

        RTOSResult result = thread->setPriority(ThreadPriority::HIGH);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(thread->getPriority() == ThreadPriority::HIGH);
    }

    SECTION("Stack high water mark") {
        int counter = 0;
        auto thread = mgr.createThread("stack_test", simpleThreadFunc, &counter, 2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);
        mgr.sleep(50);

        uint32_t highWaterMark = thread->getStackHighWaterMark();
        REQUIRE(highWaterMark > 0);
        REQUIRE(highWaterMark < 2048);
    }

    SECTION("Thread join") {
        g_threadCounter = 0;
        auto thread = mgr.createThread("join_thread", delayedThreadFunc, nullptr, 2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);
        REQUIRE(g_threadCounter == 0);

        RTOSResult result = thread->join(500);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(g_threadCounter == 1);
    }
}

// ============================================================================
// Mutex Tests
// ============================================================================

TEST_CASE("Mbed Mutex Operations", "[mbed][mutex]") {
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

        bool locked = mutex->tryLock();
        REQUIRE(locked == true);

        bool lockedAgain = mutex->tryLock();
        REQUIRE(lockedAgain == false);

        mutex->unlock();

        locked = mutex->tryLock();
        REQUIRE(locked == true);
        mutex->unlock();
    }

    SECTION("Lock timeout") {
        auto mutex = factory.createMutex();
        REQUIRE(mutex != nullptr);

        mutex->lock();

        uint64_t startTime = mgr.getTicks();
        bool locked = mutex->lock(100);
        uint64_t endTime = mgr.getTicks();

        REQUIRE(locked == false);

        uint64_t elapsed = endTime - startTime;
        uint32_t tickRate = mgr.getTickRate();
        uint64_t expectedTicks = (100 * tickRate) / 1000;
        REQUIRE(elapsed >= expectedTicks * 0.9);

        mutex->unlock();
    }
}

// ============================================================================
// RAII Lock Guard Tests
// ============================================================================

TEST_CASE("Mbed Lock Guard RAII", "[mbed][lockguard]") {
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
        REQUIRE(mutex->isLocked() == false);
    }

    SECTION("Exception safety") {
        REQUIRE(mutex->isLocked() == false);
        try {
            LockGuard lock(*mutex);
            REQUIRE(mutex->isLocked() == true);
            throw std::runtime_error("Test exception");
        } catch (...) {
            REQUIRE(mutex->isLocked() == false);
        }
    }

    SECTION("Early return safety") {
        auto testFunc = [&mutex](bool earlyReturn) {
            LockGuard lock(*mutex);
            REQUIRE(mutex->isLocked() == true);
            if (earlyReturn) return;
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

TEST_CASE("Mbed Semaphore Operations", "[mbed][semaphore]") {
    auto& factory = getRTOSFactory();
    auto& mgr = factory.getThreadManager();

    SECTION("Create semaphore") {
        auto sem = factory.createSemaphore(0, 10);
        REQUIRE(sem != nullptr);
    }

    SECTION("Signal and wait") {
        auto sem = factory.createSemaphore(0, 10);
        REQUIRE(sem != nullptr);

        bool acquired = sem->wait(10);
        REQUIRE(acquired == false);

        bool signaled = sem->signal();
        REQUIRE(signaled == true);

        acquired = sem->wait(10);
        REQUIRE(acquired == true);
    }

    SECTION("Try wait") {
        auto sem = factory.createSemaphore(0, 10);
        REQUIRE(sem != nullptr);

        bool acquired = sem->tryWait();
        REQUIRE(acquired == false);

        sem->signal();

        acquired = sem->tryWait();
        REQUIRE(acquired == true);
    }

    SECTION("Counting semaphore") {
        auto sem = factory.createSemaphore(0, 10);
        REQUIRE(sem != nullptr);

        for (int i = 0; i < 5; i++) {
            sem->signal();
        }

        for (int i = 0; i < 3; i++) {
            bool acquired = sem->wait(10);
            REQUIRE(acquired == true);
        }
    }

    SECTION("Thread synchronization with semaphore") {
        auto sem = factory.createSemaphore(0, 1);
        REQUIRE(sem != nullptr);

        auto thread = mgr.createThread("sem_signal_thread", semaphoreSignalFunc, sem.get(), 2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        uint64_t startTime = mgr.getTicks();
        bool acquired = sem->wait(200);
        uint64_t endTime = mgr.getTicks();

        REQUIRE(acquired == true);

        uint64_t elapsedTicks = endTime - startTime;
        uint32_t tickRate = mgr.getTickRate();
        uint64_t expectedTicks = (50 * tickRate) / 1000;
        REQUIRE(elapsedTicks >= expectedTicks * 0.8);
    }

    SECTION("Semaphore reset") {
        auto sem = factory.createSemaphore(0, 10);
        REQUIRE(sem != nullptr);

        sem->signal();
        sem->signal();
        sem->signal();

        sem->reset();

        bool acquired = sem->tryWait();
        REQUIRE(acquired == false);
    }
}

// ============================================================================
// Timer Tests
// ============================================================================

TEST_CASE("Mbed Timer Operations", "[mbed][timer]") {
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

        mgr.sleep(100);
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

        mgr.sleep(250);

        REQUIRE(callbackCount >= 3);
        REQUIRE(callbackCount <= 7);

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
        mgr.sleep(30);

        RTOSResult result = timer->stop();
        REQUIRE(result == RTOSResult::SUCCESS);

        int countBeforeStop = callbackCount.load();
        mgr.sleep(100);
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
        mgr.sleep(50);

        RTOSResult result = timer->restart();
        REQUIRE(result == RTOSResult::SUCCESS);

        mgr.sleep(60);
        REQUIRE(callbackCount == 0);

        mgr.sleep(50);
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

        RTOSResult result = timer->changePeriod(50);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(timer->getPeriod() == 50);

        mgr.sleep(200);
        REQUIRE(callbackCount >= 2);
        REQUIRE(callbackCount <= 6);

        timer->stop();
    }
}

// ============================================================================
// Memory Allocator Tests
// ============================================================================

TEST_CASE("Mbed Memory Allocator", "[mbed][memory]") {
    auto& allocator = getRTOSFactory().getMemoryAllocator();

    SECTION("Get heap info") {
        size_t totalHeap = allocator.getTotalHeap();
        size_t freeHeap = allocator.getFreeHeap();
        size_t minFreeHeap = allocator.getMinEverFreeHeap();

        REQUIRE(totalHeap > 0);
        REQUIRE(freeHeap > 0);
        REQUIRE(freeHeap <= totalHeap);
        REQUIRE(minFreeHeap > 0);
        REQUIRE(minFreeHeap <= freeHeap);
    }

    SECTION("Allocate and free") {
        size_t freeBefore = allocator.getFreeHeap();

        void* ptr = allocator.allocate(1024);
        REQUIRE(ptr != nullptr);

        size_t freeDuring = allocator.getFreeHeap();
        REQUIRE(freeDuring < freeBefore);

        allocator.free(ptr);

        size_t freeAfter = allocator.getFreeHeap();
        REQUIRE(freeAfter >= freeDuring);
    }

    SECTION("Reallocate") {
        void* ptr = allocator.allocate(512);
        REQUIRE(ptr != nullptr);

        memset(ptr, 0xAA, 512);

        void* newPtr = allocator.reallocate(ptr, 1024);
        REQUIRE(newPtr != nullptr);

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
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_CASE("Mbed Thread Synchronization Integration", "[mbed][integration]") {
    auto& factory = getRTOSFactory();
    auto& mgr = factory.getThreadManager();

    SECTION("Producer-consumer") {
        auto mutex = factory.createMutex();
        auto semEmpty = factory.createSemaphore(5, 5);
        auto semFull = factory.createSemaphore(0, 5);

        std::vector<int> buffer;
        std::atomic<bool> done{false};

        auto producerFunc = [&](void* arg) {
            for (int i = 0; i < 10; i++) {
                semEmpty->wait();
                {
                    LockGuard lock(*mutex);
                    buffer.push_back(i);
                }
                semFull->signal();
                mgr.sleep(10);
            }
            done = true;
        };

        std::atomic<int> consumedCount{0};
        auto consumerFunc = [&](void* arg) {
            while (!done || semFull->getCount() > 0) {
                if (semFull->wait(20)) {
                    {
                        LockGuard lock(*mutex);
                        if (!buffer.empty()) {
                            buffer.erase(buffer.begin());
                            consumedCount++;
                        }
                    }
                    semEmpty->signal();
                }
            }
        };

        auto producer = mgr.createThread("producer", producerFunc, nullptr, 2048, ThreadPriority::NORMAL);
        auto consumer = mgr.createThread("consumer", consumerFunc, nullptr, 2048, ThreadPriority::NORMAL);

        mgr.sleep(500);

        REQUIRE(consumedCount == 10);
        REQUIRE(buffer.size() == 0);
    }
}

// ============================================================================
// Mbed-Specific Tests
// ============================================================================

TEST_CASE("Mbed Specific Features", "[mbed][specific]") {
    auto& factory = getRTOSFactory();

    SECTION("Heap statistics") {
        auto& allocator = factory.getMemoryAllocator();

        size_t totalHeap = allocator.getTotalHeap();
        size_t freeBefore = allocator.getFreeHeap();

        void* ptr = allocator.allocate(4096);
        REQUIRE(ptr != nullptr);

        size_t freeAfter = allocator.getFreeHeap();
        REQUIRE(freeAfter < freeBefore);

        size_t minFree = allocator.getMinEverFreeHeap();
        REQUIRE(minFree <= freeAfter);

        allocator.free(ptr);
    }

    SECTION("Version string format") {
        const char* version = factory.getRTOSVersion();
        REQUIRE(version != nullptr);

        int major, minor, patch;
        int parsed = sscanf(version, "%d.%d.%d", &major, &minor, &patch);
        REQUIRE(parsed == 3);
        REQUIRE(major >= 6);
    }
}

#endif // RTOS_MBED
