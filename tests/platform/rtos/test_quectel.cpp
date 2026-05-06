/**
 * @file test_quectel.cpp
 * @brief Unit tests for Quectel SDK RTOS Adapter
 *
 * Comprehensive test suite for Quectel cellular module abstraction layer.
 *
 * Test Coverage:
 * - Factory initialization and metadata
 * - Thread manager operations (ThreadX-based)
 * - Thread lifecycle
 * - Thread priority management
 * - Mutex operations
 * - Lock guard RAII pattern
 * - Semaphore operations
 * - Timer operations
 * - Memory allocation
 * - Cellular modem operations
 * - GNSS operations
 * - File system operations
 * - Integration tests
 *
 * Hardware Requirements:
 * - Quectel BG95/BG96 EVB
 * - Quectel BC660K EVB
 * - Quectel EC25 EVB
 * - SIM card with data plan
 * - GNSS antenna (for positioning tests)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef QUECTEL_SDK

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "platform/rtos_abstraction.h"
#include <atomic>

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

// ============================================================================
// Factory and Metadata Tests
// ============================================================================

TEST_CASE("Quectel Factory Initialization", "[quectel][factory]") {
    auto& factory = getRTOSFactory();

    SECTION("Factory is accessible") {
        REQUIRE(&factory != nullptr);
    }

    SECTION("RTOS name contains Quectel and ThreadX") {
        const char* name = factory.getRTOSName();
        REQUIRE(name != nullptr);
        REQUIRE_THAT(name, Catch::Matchers::ContainsSubstring("Quectel"));
        REQUIRE_THAT(name, Catch::Matchers::ContainsSubstring("ThreadX"));
    }

    SECTION("Chip info is available") {
        auto& quectelFactory = static_cast<QuectelFactory&>(factory);
        const char* chipInfo = quectelFactory.getChipInfo();
        REQUIRE(chipInfo != nullptr);
        REQUIRE_THAT(chipInfo, Catch::Matchers::ContainsSubstring("Quectel"));
    }
}

// ============================================================================
// Thread Tests
// ============================================================================

TEST_CASE("Quectel Thread Operations", "[quectel][thread]") {
    auto& mgr = getRTOSFactory().getThreadManager();

    SECTION("Create and start thread") {
        int counter = 0;
        auto thread = mgr.createThread("test_thread", simpleThreadFunc, &counter,
                                       2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        mgr.sleep(50);
        REQUIRE(counter == 1);
    }

    SECTION("Multiple threads") {
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

    SECTION("Thread priority management") {
        int counter = 0;
        auto thread = mgr.createThread("pri_thread", simpleThreadFunc, &counter,
                                       2048, ThreadPriority::HIGH);
        REQUIRE(thread != nullptr);
        REQUIRE(thread->getPriority() == ThreadPriority::HIGH);

        RTOSResult result = thread->setPriority(ThreadPriority::LOW);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(thread->getPriority() == ThreadPriority::LOW);
    }

    SECTION("Thread suspend and resume") {
        std::atomic<int> counter{0};
        auto thread = mgr.createThread("suspend_thread", counterThreadFunc, &counter,
                                       2048, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        mgr.sleep(50);
        int count1 = counter.load();

        RTOSResult result = thread->suspend();
        REQUIRE(result == RTOSResult::SUCCESS);

        mgr.sleep(100);
        int count2 = counter.load();
        REQUIRE(count2 == count1);

        result = thread->resume();
        REQUIRE(result == RTOSResult::SUCCESS);

        mgr.sleep(100);
        int count3 = counter.load();
        REQUIRE(count3 > count2);
    }
}

// ============================================================================
// Mutex Tests
// ============================================================================

TEST_CASE("Quectel Mutex Operations", "[quectel][mutex]") {
    auto mutex = getRTOSFactory().createMutex();
    REQUIRE(mutex != nullptr);

    SECTION("Lock and unlock") {
        bool locked = mutex->lock(100);
        REQUIRE(locked);
        REQUIRE(mutex->isLocked());

        mutex->unlock();
        REQUIRE_FALSE(mutex->isLocked());
    }

    SECTION("TryLock") {
        bool locked = mutex->tryLock();
        REQUIRE(locked);

        bool locked2 = mutex->tryLock();
        REQUIRE_FALSE(locked2);

        mutex->unlock();
    }
}

// ============================================================================
// Semaphore Tests
// ============================================================================

TEST_CASE("Quectel Semaphore Operations", "[quectel][semaphore]") {
    SECTION("Binary semaphore") {
        auto sem = getRTOSFactory().createSemaphore(0, 1);
        REQUIRE(sem != nullptr);

        bool acquired = sem->tryWait();
        REQUIRE_FALSE(acquired);

        sem->signal();
        acquired = sem->wait(100);
        REQUIRE(acquired);
    }

    SECTION("Counting semaphore") {
        auto sem = getRTOSFactory().createSemaphore(3, 5);
        REQUIRE(sem != nullptr);

        uint32_t count = sem->getCount();
        REQUIRE(count == 3);

        sem->wait(10);
        count = sem->getCount();
        REQUIRE(count == 2);

        sem->signal();
        count = sem->getCount();
        REQUIRE(count == 3);
    }
}

// ============================================================================
// Timer Tests
// ============================================================================

TEST_CASE("Quectel Timer Operations", "[quectel][timer]") {
    std::atomic<int> counter{0};
    auto timer = getRTOSFactory().createTimer("test_timer");
    REQUIRE(timer != nullptr);

    SECTION("One-shot timer") {
        RTOSResult result = timer->start(100, false, [&counter]() {
            counter++;
        });
        REQUIRE(result == RTOSResult::SUCCESS);

        getRTOSFactory().getThreadManager().sleep(150);
        REQUIRE(counter.load() == 1);
    }

    SECTION("Periodic timer") {
        RTOSResult result = timer->start(50, true, [&counter]() {
            counter++;
        });
        REQUIRE(result == RTOSResult::SUCCESS);

        getRTOSFactory().getThreadManager().sleep(250);
        int count = counter.load();
        REQUIRE(count >= 4);

        timer->stop();
    }
}

// ============================================================================
// Memory Tests
// ============================================================================

TEST_CASE("Quectel Memory Allocation", "[quectel][memory]") {
    auto& allocator = getRTOSFactory().getMemoryAllocator();

    SECTION("Basic allocation") {
        void* ptr = allocator.allocate(1024);
        REQUIRE(ptr != nullptr);
        allocator.free(ptr);
    }

    SECTION("Heap statistics") {
        size_t total = allocator.getTotalHeap();
        size_t free = allocator.getFreeHeap();

        REQUIRE(total > 0);
        REQUIRE(free > 0);
        REQUIRE(free <= total);
    }

    SECTION("Thread safety") {
        REQUIRE(allocator.isThreadSafe());
    }
}

// ============================================================================
// Cellular Modem Tests
// ============================================================================

TEST_CASE("Quectel Cellular Modem Operations", "[quectel][modem]") {
    auto& quectelFactory = static_cast<QuectelFactory&>(getRTOSFactory());
    auto& modem = quectelFactory.getCellularModem();

    SECTION("Modem initialization") {
        bool result = modem.init();
        REQUIRE(result);
    }

    SECTION("Get IMEI") {
        modem.init();
        char imei[16] = {0};
        bool result = modem.getIMEI(imei, sizeof(imei));
        if (result) {
            REQUIRE(strlen(imei) == 15);
            INFO("IMEI: " << imei);
        }
    }

    SECTION("Get IMSI") {
        modem.init();
        char imsi[16] = {0};
        bool result = modem.getIMSI(imsi, sizeof(imsi));
        if (result) {
            REQUIRE(strlen(imsi) >= 14);
            INFO("IMSI: " << imsi);
        }
    }

    SECTION("Get signal strength") {
        modem.init();
        int rssi = modem.getSignalStrength();
        if (rssi != -1) {
            REQUIRE(rssi >= -120);
            REQUIRE(rssi <= -30);
            INFO("RSSI: " << rssi << " dBm");
        }
    }

    // Connection tests require SIM and network
    // SECTION("Connect to network") {
    //     modem.init();
    //     bool connected = modem.connect();
    //     REQUIRE(connected);
    //     modem.disconnect();
    // }
}

// ============================================================================
// GNSS Tests
// ============================================================================

TEST_CASE("Quectel GNSS Operations", "[quectel][gnss]") {
    auto& quectelFactory = static_cast<QuectelFactory&>(getRTOSFactory());

    SECTION("Start and stop GNSS") {
        bool started = quectelFactory.startGNSS();
        REQUIRE(started);

        getRTOSFactory().getThreadManager().sleep(1000);

        bool stopped = quectelFactory.stopGNSS();
        REQUIRE(stopped);
    }

    // Position tests require GNSS antenna and satellite lock
    // SECTION("Get position") {
    //     quectelFactory.startGNSS();
    //     getRTOSFactory().getThreadManager().sleep(30000); // Wait for fix
    //
    //     double lat, lon, alt;
    //     bool result = quectelFactory.getGNSSPosition(&lat, &lon, &alt);
    //     if (result) {
    //         INFO("Position: " << lat << ", " << lon << ", " << alt);
    //     }
    //
    //     quectelFactory.stopGNSS();
    // }
}

// ============================================================================
// File System Tests
// ============================================================================

TEST_CASE("Quectel File System Operations", "[quectel][fs]") {
    auto& quectelFactory = static_cast<QuectelFactory&>(getRTOSFactory());

    SECTION("File write and read") {
        const char* testFile = "/test.txt";
        const char* testData = "Hello Quectel!";

        int fd = quectelFactory.openFile(testFile, QAPI_FS_O_CREAT | QAPI_FS_O_WRONLY);
        if (fd >= 0) {
            int written = quectelFactory.writeFile(fd, testData, strlen(testData));
            REQUIRE(written > 0);
            quectelFactory.closeFile(fd);

            fd = quectelFactory.openFile(testFile, QAPI_FS_O_RDONLY);
            REQUIRE(fd >= 0);

            char buffer[64] = {0};
            int read = quectelFactory.readFile(fd, buffer, sizeof(buffer));
            REQUIRE(read > 0);
            REQUIRE_THAT(buffer, Catch::Matchers::Equals(testData));

            quectelFactory.closeFile(fd);
        }
    }
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_CASE("Quectel Integration Test", "[quectel][integration]") {
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

#endif // QUECTEL_SDK
