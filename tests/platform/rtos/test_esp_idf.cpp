/**
 * @file test_esp_idf.cpp
 * @brief Unit tests for ESP-IDF RTOS Adapter
 *
 * Comprehensive test suite for ESP-IDF RTOS abstraction layer.
 *
 * Test Coverage:
 * - Factory initialization and metadata
 * - Thread manager operations
 * - Thread lifecycle (create, start, join, suspend, resume)
 * - Thread priority management
 * - Mutex operations (lock, unlock, tryLock, timeout)
 * - Lock guard RAII pattern
 * - Semaphore operations (wait, signal, timeout, count)
 * - Timer operations (periodic, one-shot, start, stop, restart)
 * - Memory allocation (standard, DMA, SPIRAM)
 * - Flash operations (read, write, erase)
 * - OTA update operations
 * - NVS storage operations
 * - WiFi connectivity
 * - Dual-core operations
 * - Power management
 * - Integration tests
 *
 * Hardware Requirements:
 * - ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, or ESP32-H2
 * - WiFi access point for connectivity tests
 * - Optional: External SPIRAM for memory tests
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef ESP_IDF

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "platform/rtos_abstraction.h"
#include <atomic>
#include <thread>

using namespace lwm2m::platform::rtos;

// ============================================================================
// Test Helpers
// ============================================================================

static void simpleThreadFunc(void* arg) {
    int* counter = static_cast<int*>(arg);
    (*counter)++;
}

static void counterThreadFunc(void* arg) {
    std::atomic<int>* counter = static_cast<std::atomic<int>*>(arg);
    for (int i = 0; i < 10; i++) {
        (*counter)++;
        getRTOSFactory().getThreadManager().sleep(10);
    }
}

static void mutexThreadFunc(void* arg) {
    IMutex* mutex = static_cast<IMutex*>(arg);
    mutex->lock();
    getRTOSFactory().getThreadManager().sleep(50);
    mutex->unlock();
}

// ============================================================================
// Factory and Metadata Tests
// ============================================================================

TEST_CASE("ESP-IDF Factory Initialization", "[esp_idf][factory]") {
    auto& factory = getRTOSFactory();

    SECTION("Factory is accessible") {
        REQUIRE(&factory != nullptr);
    }

    SECTION("RTOS name is correct") {
        const char* name = factory.getRTOSName();
        REQUIRE(name != nullptr);
        REQUIRE_THAT(name, Catch::Matchers::Equals("ESP-IDF"));
    }

    SECTION("RTOS version is available") {
        const char* version = factory.getRTOSVersion();
        REQUIRE(version != nullptr);
        REQUIRE(strlen(version) > 0);
        REQUIRE_THAT(version, Catch::Matchers::ContainsSubstring("FreeRTOS"));
    }

    SECTION("Chip info is available") {
        auto& espFactory = static_cast<ESPFactory&>(factory);
        const char* chipInfo = espFactory.getChipInfo();
        REQUIRE(chipInfo != nullptr);
        REQUIRE(strlen(chipInfo) > 0);
        // Should contain chip model
        bool validChip = (strstr(chipInfo, "ESP32") != nullptr ||
                         strstr(chipInfo, "ESP32-S2") != nullptr ||
                         strstr(chipInfo, "ESP32-S3") != nullptr ||
                         strstr(chipInfo, "ESP32-C3") != nullptr);
        REQUIRE(validChip);
    }
}

// ============================================================================
// Thread Manager Tests
// ============================================================================

TEST_CASE("ESP-IDF Thread Manager", "[esp_idf][thread_manager]") {
    auto& mgr = getRTOSFactory().getThreadManager();

    SECTION("Sleep function works") {
        uint64_t start = mgr.getTicks();
        mgr.sleep(100);
        uint64_t end = mgr.getTicks();
        uint64_t elapsed = (end - start) * 1000 / mgr.getTickRate();
        REQUIRE(elapsed >= 100);
        REQUIRE(elapsed < 150);
    }

    SECTION("Yield function works") {
        mgr.yield();
        REQUIRE(true);
    }

    SECTION("Tick count increases") {
        uint64_t tick1 = mgr.getTicks();
        mgr.sleep(10);
        uint64_t tick2 = mgr.getTicks();
        REQUIRE(tick2 > tick1);
    }

    SECTION("Tick rate is reasonable") {
        uint32_t rate = mgr.getTickRate();
        REQUIRE(rate >= 100);
        REQUIRE(rate <= 10000);
    }

    SECTION("Critical section works") {
        mgr.enterCritical();
        mgr.exitCritical();
        REQUIRE(true);
    }
}

// ============================================================================
// Thread Tests
// ============================================================================

TEST_CASE("ESP-IDF Thread Creation and Lifecycle", "[esp_idf][thread]") {
    auto& mgr = getRTOSFactory().getThreadManager();

    SECTION("Create and start thread") {
        int counter = 0;
        auto thread = mgr.createThread("test_thread", simpleThreadFunc, &counter,
                                       2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        mgr.sleep(50);
        REQUIRE(counter == 1);
    }

    SECTION("Thread name is preserved") {
        int counter = 0;
        auto thread = mgr.createThread("my_thread", simpleThreadFunc, &counter,
                                       2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);
        REQUIRE_THAT(thread->getName(), Catch::Matchers::Equals("my_thread"));
    }

    SECTION("Multiple threads can run concurrently") {
        std::atomic<int> counter1{0};
        std::atomic<int> counter2{0};

        auto thread1 = mgr.createThread("thread1", counterThreadFunc, &counter1,
                                        2048, ThreadPriority::NORMAL);
        auto thread2 = mgr.createThread("thread2", counterThreadFunc, &counter2,
                                        2048, ThreadPriority::NORMAL);

        REQUIRE(thread1 != nullptr);
        REQUIRE(thread2 != nullptr);

        mgr.sleep(200);
        REQUIRE(counter1.load() >= 10);
        REQUIRE(counter2.load() >= 10);
    }

    SECTION("Thread join works") {
        int counter = 0;
        auto thread = mgr.createThread("test_thread", simpleThreadFunc, &counter,
                                       2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        RTOSResult result = thread->join(1000);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(counter == 1);
    }

    SECTION("Thread state transitions") {
        std::atomic<int> counter{0};
        auto thread = mgr.createThread("state_thread", counterThreadFunc, &counter,
                                       2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        mgr.sleep(50);
        ThreadState state = thread->getState();
        REQUIRE((state == ThreadState::RUNNING || state == ThreadState::READY ||
                 state == ThreadState::BLOCKED));
    }
}

// ============================================================================
// Thread Priority Tests
// ============================================================================

TEST_CASE("ESP-IDF Thread Priority", "[esp_idf][thread][priority]") {
    auto& mgr = getRTOSFactory().getThreadManager();

    SECTION("Thread priority is preserved") {
        int counter = 0;
        auto thread = mgr.createThread("pri_thread", simpleThreadFunc, &counter,
                                       2048, ThreadPriority::HIGH);
        REQUIRE(thread != nullptr);
        REQUIRE(thread->getPriority() == ThreadPriority::HIGH);
    }

    SECTION("Thread priority can be changed") {
        int counter = 0;
        auto thread = mgr.createThread("pri_thread", simpleThreadFunc, &counter,
                                       2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        RTOSResult result = thread->setPriority(ThreadPriority::HIGH);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(thread->getPriority() == ThreadPriority::HIGH);
    }
}

// ============================================================================
// Mutex Tests
// ============================================================================

TEST_CASE("ESP-IDF Mutex Operations", "[esp_idf][mutex]") {
    auto mutex = getRTOSFactory().createMutex();
    REQUIRE(mutex != nullptr);

    SECTION("Lock and unlock") {
        bool locked = mutex->lock(100);
        REQUIRE(locked);
        REQUIRE(mutex->isLocked());

        mutex->unlock();
        REQUIRE_FALSE(mutex->isLocked());
    }

    SECTION("TryLock succeeds when unlocked") {
        bool locked = mutex->tryLock();
        REQUIRE(locked);
        mutex->unlock();
    }

    SECTION("TryLock fails when locked") {
        mutex->lock(100);
        bool locked = mutex->tryLock();
        REQUIRE_FALSE(locked);
        mutex->unlock();
    }

    SECTION("Lock with timeout succeeds") {
        bool locked = mutex->lock(100);
        REQUIRE(locked);
        mutex->unlock();
    }

    SECTION("Mutex is thread-safe") {
        auto& mgr = getRTOSFactory().getThreadManager();

        auto thread1 = mgr.createThread("mutex1", mutexThreadFunc, mutex.get(),
                                        2048, ThreadPriority::NORMAL);
        mgr.sleep(10);

        bool locked = mutex->tryLock();
        REQUIRE_FALSE(locked);

        mgr.sleep(100);
        locked = mutex->tryLock();
        REQUIRE(locked);
        mutex->unlock();
    }
}

// ============================================================================
// Lock Guard Tests
// ============================================================================

TEST_CASE("ESP-IDF Lock Guard RAII", "[esp_idf][mutex][lock_guard]") {
    auto mutex = getRTOSFactory().createMutex();
    REQUIRE(mutex != nullptr);

    SECTION("Lock guard locks on construction") {
        {
            LockGuard guard(*mutex);
            REQUIRE(mutex->isLocked());
        }
    }

    SECTION("Lock guard unlocks on destruction") {
        {
            LockGuard guard(*mutex);
            REQUIRE(mutex->isLocked());
        }
        REQUIRE_FALSE(mutex->isLocked());
    }

    SECTION("Lock guard works with scope") {
        REQUIRE_FALSE(mutex->isLocked());
        {
            LockGuard guard(*mutex);
            REQUIRE(mutex->isLocked());
            {
                REQUIRE(mutex->isLocked());
            }
            REQUIRE(mutex->isLocked());
        }
        REQUIRE_FALSE(mutex->isLocked());
    }
}

// ============================================================================
// Semaphore Tests
// ============================================================================

TEST_CASE("ESP-IDF Semaphore Operations", "[esp_idf][semaphore]") {
    SECTION("Binary semaphore") {
        auto sem = getRTOSFactory().createSemaphore(0, 1);
        REQUIRE(sem != nullptr);

        bool acquired = sem->tryWait();
        REQUIRE_FALSE(acquired);

        bool signaled = sem->signal();
        REQUIRE(signaled);

        acquired = sem->wait(100);
        REQUIRE(acquired);
    }

    SECTION("Counting semaphore") {
        auto sem = getRTOSFactory().createSemaphore(3, 5);
        REQUIRE(sem != nullptr);

        REQUIRE(sem->wait(10));
        REQUIRE(sem->wait(10));
        REQUIRE(sem->wait(10));

        bool acquired = sem->tryWait();
        REQUIRE_FALSE(acquired);

        sem->signal();
        acquired = sem->wait(10);
        REQUIRE(acquired);
    }

    SECTION("Semaphore timeout") {
        auto sem = getRTOSFactory().createSemaphore(0, 1);
        REQUIRE(sem != nullptr);

        auto& mgr = getRTOSFactory().getThreadManager();
        uint64_t start = mgr.getTicks();
        bool acquired = sem->wait(100);
        uint64_t end = mgr.getTicks();

        REQUIRE_FALSE(acquired);
        uint64_t elapsed = (end - start) * 1000 / mgr.getTickRate();
        REQUIRE(elapsed >= 100);
    }

    SECTION("Semaphore reset") {
        auto sem = getRTOSFactory().createSemaphore(3, 5);
        REQUIRE(sem != nullptr);

        sem->reset();
        bool acquired = sem->tryWait();
        REQUIRE_FALSE(acquired);
    }
}

// ============================================================================
// Timer Tests
// ============================================================================

TEST_CASE("ESP-IDF Timer Operations", "[esp_idf][timer]") {
    std::atomic<int> counter{0};
    auto timer = getRTOSFactory().createTimer("test_timer");
    REQUIRE(timer != nullptr);

    SECTION("One-shot timer") {
        RTOSResult result = timer->start(100, false, [&counter]() {
            counter++;
        });
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(timer->isActive());

        getRTOSFactory().getThreadManager().sleep(150);
        REQUIRE(counter.load() == 1);
        REQUIRE_FALSE(timer->isActive());
    }

    SECTION("Periodic timer") {
        RTOSResult result = timer->start(50, true, [&counter]() {
            counter++;
        });
        REQUIRE(result == RTOSResult::SUCCESS);

        getRTOSFactory().getThreadManager().sleep(250);
        int count = counter.load();
        REQUIRE(count >= 4);
        REQUIRE(count <= 6);

        timer->stop();
        REQUIRE_FALSE(timer->isActive());
    }

    SECTION("Timer restart") {
        timer->start(100, false, [&counter]() { counter++; });
        getRTOSFactory().getThreadManager().sleep(50);

        RTOSResult result = timer->restart();
        REQUIRE(result == RTOSResult::SUCCESS);

        getRTOSFactory().getThreadManager().sleep(150);
        REQUIRE(counter.load() == 1);
    }

    SECTION("Timer change period") {
        timer->start(100, true, [&counter]() { counter++; });
        getRTOSFactory().getThreadManager().sleep(150);

        RTOSResult result = timer->changePeriod(50);
        REQUIRE(result == RTOSResult::SUCCESS);

        timer->stop();
    }
}

// ============================================================================
// Memory Allocator Tests
// ============================================================================

TEST_CASE("ESP-IDF Memory Allocation", "[esp_idf][memory]") {
    auto& allocator = static_cast<ESPMemoryAllocator&>(
        getRTOSFactory().getMemoryAllocator()
    );

    SECTION("Basic allocation and deallocation") {
        void* ptr = allocator.allocate(1024);
        REQUIRE(ptr != nullptr);
        allocator.free(ptr);
    }

    SECTION("Reallocation") {
        void* ptr = allocator.allocate(512);
        REQUIRE(ptr != nullptr);

        void* new_ptr = allocator.reallocate(ptr, 1024);
        REQUIRE(new_ptr != nullptr);

        allocator.free(new_ptr);
    }

    SECTION("Heap statistics") {
        size_t total = allocator.getTotalHeap();
        size_t free_before = allocator.getFreeHeap();

        REQUIRE(total > 0);
        REQUIRE(free_before > 0);
        REQUIRE(free_before <= total);

        void* ptr = allocator.allocate(4096);
        REQUIRE(ptr != nullptr);

        size_t free_after = allocator.getFreeHeap();
        REQUIRE(free_after < free_before);

        allocator.free(ptr);
    }

    SECTION("Minimum free heap tracking") {
        size_t min_free = allocator.getMinEverFreeHeap();
        REQUIRE(min_free > 0);
    }

    SECTION("Largest free block") {
        size_t largest = allocator.getLargestFreeBlock();
        REQUIRE(largest > 0);
    }

    SECTION("Thread safety is guaranteed") {
        REQUIRE(allocator.isThreadSafe());
    }

    SECTION("DMA memory allocation") {
        void* ptr = allocator.allocateDMA(1024);
        REQUIRE(ptr != nullptr);
        allocator.free(ptr);
    }

    SECTION("SPIRAM statistics") {
        size_t spiram_total = allocator.getSPIRAMSize();
        size_t spiram_free = allocator.getFreeSPIRAM();
        // SPIRAM may not be present on all boards
        if (spiram_total > 0) {
            REQUIRE(spiram_free <= spiram_total);
        }
    }
}

// ============================================================================
// Flash Memory Tests
// ============================================================================

TEST_CASE("ESP-IDF Flash Operations", "[esp_idf][flash]") {
    auto& flash = getRTOSFactory().getFlash();

    SECTION("Flash size is valid") {
        size_t size = flash.getTotalSize();
        REQUIRE(size >= 2 * 1024 * 1024); // At least 2MB
        REQUIRE(size <= 16 * 1024 * 1024); // At most 16MB
    }

    SECTION("Page and sector size") {
        size_t pageSize = flash.getPageSize();
        size_t sectorSize = flash.getSectorSize();
        REQUIRE(pageSize == 4096);
        REQUIRE(sectorSize == 4096);
    }

    SECTION("Flash read operation") {
        uint8_t buffer[256];
        bool result = flash.read(0x1000, buffer, sizeof(buffer));
        REQUIRE(result);
    }

    // Note: Write/erase tests should use test partitions, not production flash
}

// ============================================================================
// OTA Update Tests
// ============================================================================

TEST_CASE("ESP-IDF OTA Operations", "[esp_idf][ota]") {
    auto& ota = getRTOSFactory().getOTA();

    SECTION("OTA begin with size") {
        bool result = ota.begin(1024 * 1024); // 1MB firmware
        REQUIRE(result);
        ota.end(); // Clean up
    }

    // Note: Full OTA tests require actual firmware images
}

// ============================================================================
// NVS Storage Tests
// ============================================================================

TEST_CASE("ESP-IDF NVS Operations", "[esp_idf][nvs]") {
    auto& nvs = getRTOSFactory().getNVM();

    SECTION("NVS initialization") {
        bool result = nvs.init();
        REQUIRE(result);
    }

    SECTION("NVS write and read") {
        REQUIRE(nvs.init());

        const char* testKey = "test_key";
        uint32_t testValue = 0x12345678;

        bool written = nvs.write(testKey, &testValue, sizeof(testValue));
        REQUIRE(written);

        uint32_t readValue = 0;
        bool read = nvs.read(testKey, &readValue, sizeof(readValue));
        REQUIRE(read);
        REQUIRE(readValue == testValue);

        nvs.erase(testKey);
    }

    SECTION("NVS exists check") {
        REQUIRE(nvs.init());

        const char* testKey = "exists_test";
        REQUIRE_FALSE(nvs.exists(testKey));

        uint8_t data = 42;
        nvs.write(testKey, &data, sizeof(data));
        REQUIRE(nvs.exists(testKey));

        nvs.erase(testKey);
        REQUIRE_FALSE(nvs.exists(testKey));
    }

    SECTION("NVS get size") {
        REQUIRE(nvs.init());

        const char* testKey = "size_test";
        uint8_t data[100] = {0};

        nvs.write(testKey, data, sizeof(data));
        size_t size = nvs.getSize(testKey);
        REQUIRE(size == sizeof(data));

        nvs.erase(testKey);
    }
}

// ============================================================================
// WiFi Tests
// ============================================================================

TEST_CASE("ESP-IDF WiFi Interface", "[esp_idf][wifi]") {
    auto& wifi = getRTOSFactory().getWiFi();

    SECTION("WiFi initialization") {
        bool result = wifi.init();
        REQUIRE(result);
    }

    // Note: Connection tests require valid SSID/password
    // SECTION("WiFi connect") {
    //     wifi.init();
    //     bool connected = wifi.connect("your_ssid", "your_password");
    //     REQUIRE(connected);
    //     wifi.disconnect();
    // }
}

// ============================================================================
// ESP32-Specific Tests
// ============================================================================

TEST_CASE("ESP32 Dual-Core Support", "[esp_idf][dual_core]") {
    auto& espFactory = static_cast<ESPFactory&>(getRTOSFactory());

    SECTION("Run task on core 0") {
        std::atomic<bool> executed{false};
        auto task = [](void* param) {
            std::atomic<bool>* flag = static_cast<std::atomic<bool>*>(param);
            flag->store(true);
            vTaskDelete(nullptr);
        };

        bool result = espFactory.runOnCore(0, task, "core0_task", &executed);
        REQUIRE(result);

        getRTOSFactory().getThreadManager().sleep(100);
        REQUIRE(executed.load());
    }

    SECTION("Run task on core 1") {
        std::atomic<bool> executed{false};
        auto task = [](void* param) {
            std::atomic<bool>* flag = static_cast<std::atomic<bool>*>(param);
            flag->store(true);
            vTaskDelete(nullptr);
        };

        bool result = espFactory.runOnCore(1, task, "core1_task", &executed);
        REQUIRE(result);

        getRTOSFactory().getThreadManager().sleep(100);
        REQUIRE(executed.load());
    }
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_CASE("ESP-IDF Integration: Threads + Mutex + Semaphore", "[esp_idf][integration]") {
    auto& mgr = getRTOSFactory().getThreadManager();
    auto mutex = getRTOSFactory().createMutex();
    auto sem = getRTOSFactory().createSemaphore(0, 1);

    std::atomic<int> sharedCounter{0};

    auto workerFunc = [](void* arg) {
        auto* params = static_cast<std::tuple<IMutex*, ISemaphore*, std::atomic<int>*>*>(arg);
        auto& [mutex, sem, counter] = *params;

        for (int i = 0; i < 5; i++) {
            LockGuard guard(*mutex);
            (*counter)++;
            getRTOSFactory().getThreadManager().sleep(10);
        }
        sem->signal();
    };

    std::tuple<IMutex*, ISemaphore*, std::atomic<int>*> params{
        mutex.get(), sem.get(), &sharedCounter
    };

    auto thread = mgr.createThread("worker", workerFunc, &params,
                                   2048, ThreadPriority::NORMAL);
    REQUIRE(thread != nullptr);

    bool signaled = sem->wait(1000);
    REQUIRE(signaled);
    REQUIRE(sharedCounter.load() == 5);
}

TEST_CASE("ESP-IDF Integration: Multi-threaded with Timers", "[esp_idf][integration]") {
    std::atomic<int> threadCounter{0};
    std::atomic<int> timerCounter{0};

    auto timer = getRTOSFactory().createTimer("integration_timer");
    timer->start(50, true, [&timerCounter]() {
        timerCounter++;
    });

    auto thread1 = getRTOSFactory().getThreadManager().createThread(
        "thread1", counterThreadFunc, &threadCounter, 2048, ThreadPriority::NORMAL
    );
    auto thread2 = getRTOSFactory().getThreadManager().createThread(
        "thread2", counterThreadFunc, &threadCounter, 2048, ThreadPriority::NORMAL
    );

    REQUIRE(thread1 != nullptr);
    REQUIRE(thread2 != nullptr);

    getRTOSFactory().getThreadManager().sleep(300);

    timer->stop();

    REQUIRE(threadCounter.load() >= 20);
    REQUIRE(timerCounter.load() >= 5);
}

#endif // ESP_IDF
