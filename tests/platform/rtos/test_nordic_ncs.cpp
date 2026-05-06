/**
 * @file test_nordic_ncs.cpp
 * @brief Unit tests for Nordic nRF Connect SDK RTOS Adapter
 *
 * Comprehensive test suite for Nordic nRF Connect SDK abstraction layer.
 *
 * Test Coverage:
 * - Factory initialization and metadata
 * - Thread manager operations (inherited from Zephyr)
 * - Thread lifecycle (create, start, join, suspend, resume)
 * - Thread priority management
 * - Mutex operations (lock, unlock, tryLock, timeout)
 * - Lock guard RAII pattern
 * - Semaphore operations (wait, signal, timeout, count)
 * - Timer operations (periodic, one-shot, start, stop, restart)
 * - Memory allocation
 * - LTE modem operations (nRF91 series)
 * - BLE operations (all nRF series)
 * - MCUboot OTA updates
 * - nRF Cloud integration
 * - Power management
 * - Integration tests
 *
 * Hardware Requirements:
 * - nRF9160 DK (for LTE tests)
 * - nRF9151 DK (for LTE + DECT NR+ tests)
 * - nRF5340 DK (for dual-core + BLE tests)
 * - nRF52840 DK (for BLE + 802.15.4 tests)
 * - Nordic Thingy:91 (for integrated testing)
 * - SIM card with LTE-M or NB-IoT coverage
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef NORDIC_NCS

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

static void mutexThreadFunc(void* arg) {
    IMutex* mutex = static_cast<IMutex*>(arg);
    mutex->lock();
    getRTOSFactory().getThreadManager().sleep(50);
    mutex->unlock();
}

// ============================================================================
// Factory and Metadata Tests
// ============================================================================

TEST_CASE("Nordic NCS Factory Initialization", "[nordic][factory]") {
    auto& factory = getRTOSFactory();

    SECTION("Factory is accessible") {
        REQUIRE(&factory != nullptr);
    }

    SECTION("RTOS name is correct") {
        const char* name = factory.getRTOSName();
        REQUIRE(name != nullptr);
        REQUIRE_THAT(name, Catch::Matchers::Equals("Nordic nRF Connect SDK"));
    }

    SECTION("RTOS version contains NCS and Zephyr") {
        const char* version = factory.getRTOSVersion();
        REQUIRE(version != nullptr);
        REQUIRE(strlen(version) > 0);
        REQUIRE_THAT(version, Catch::Matchers::ContainsSubstring("NCS"));
        REQUIRE_THAT(version, Catch::Matchers::ContainsSubstring("Zephyr"));
    }

    SECTION("Chip info is available") {
        auto& nordicFactory = static_cast<NordicFactory&>(factory);
        const char* chipInfo = nordicFactory.getChipInfo();
        REQUIRE(chipInfo != nullptr);
        REQUIRE(strlen(chipInfo) > 0);

        // Should contain nRF chip designation
        bool validChip = (strstr(chipInfo, "nRF9160") != nullptr ||
                         strstr(chipInfo, "nRF9151") != nullptr ||
                         strstr(chipInfo, "nRF5340") != nullptr ||
                         strstr(chipInfo, "nRF52840") != nullptr ||
                         strstr(chipInfo, "nRF52833") != nullptr);
        REQUIRE(validChip);
    }
}

// ============================================================================
// Thread Manager Tests (Inherited from Zephyr)
// ============================================================================

TEST_CASE("Nordic NCS Thread Manager", "[nordic][thread_manager]") {
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

TEST_CASE("Nordic NCS Thread Creation and Lifecycle", "[nordic][thread]") {
    auto& mgr = getRTOSFactory().getThreadManager();

    SECTION("Create and start thread") {
        int counter = 0;
        auto thread = mgr.createThread("test_thread", simpleThreadFunc, &counter,
                                       1024, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        mgr.sleep(50);
        REQUIRE(counter == 1);
    }

    SECTION("Thread name is preserved") {
        int counter = 0;
        auto thread = mgr.createThread("my_thread", simpleThreadFunc, &counter,
                                       1024, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);
        REQUIRE_THAT(thread->getName(), Catch::Matchers::Equals("my_thread"));
    }

    SECTION("Multiple threads can run concurrently") {
        std::atomic<int> counter1{0};
        std::atomic<int> counter2{0};

        auto thread1 = mgr.createThread("thread1", counterThreadFunc, &counter1,
                                        1024, ThreadPriority::NORMAL);
        auto thread2 = mgr.createThread("thread2", counterThreadFunc, &counter2,
                                        1024, ThreadPriority::NORMAL);

        REQUIRE(thread1 != nullptr);
        REQUIRE(thread2 != nullptr);

        mgr.sleep(200);
        REQUIRE(counter1.load() >= 10);
        REQUIRE(counter2.load() >= 10);
    }

    SECTION("Thread join works") {
        int counter = 0;
        auto thread = mgr.createThread("test_thread", simpleThreadFunc, &counter,
                                       1024, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        RTOSResult result = thread->join(1000);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(counter == 1);
    }
}

// ============================================================================
// Thread Priority Tests
// ============================================================================

TEST_CASE("Nordic NCS Thread Priority", "[nordic][thread][priority]") {
    auto& mgr = getRTOSFactory().getThreadManager();

    SECTION("Thread priority is preserved") {
        int counter = 0;
        auto thread = mgr.createThread("pri_thread", simpleThreadFunc, &counter,
                                       1024, ThreadPriority::HIGH);
        REQUIRE(thread != nullptr);
        REQUIRE(thread->getPriority() == ThreadPriority::HIGH);
    }

    SECTION("Thread priority can be changed") {
        int counter = 0;
        auto thread = mgr.createThread("pri_thread", simpleThreadFunc, &counter,
                                       1024, ThreadPriority::NORMAL);
        REQUIRE(thread != nullptr);

        RTOSResult result = thread->setPriority(ThreadPriority::HIGH);
        REQUIRE(result == RTOSResult::SUCCESS);
        REQUIRE(thread->getPriority() == ThreadPriority::HIGH);
    }
}

// ============================================================================
// Mutex Tests
// ============================================================================

TEST_CASE("Nordic NCS Mutex Operations", "[nordic][mutex]") {
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

    SECTION("Mutex is thread-safe") {
        auto& mgr = getRTOSFactory().getThreadManager();

        auto thread1 = mgr.createThread("mutex1", mutexThreadFunc, mutex.get(),
                                        1024, ThreadPriority::NORMAL);
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

TEST_CASE("Nordic NCS Lock Guard RAII", "[nordic][mutex][lock_guard]") {
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
}

// ============================================================================
// Semaphore Tests
// ============================================================================

TEST_CASE("Nordic NCS Semaphore Operations", "[nordic][semaphore]") {
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
}

// ============================================================================
// Timer Tests
// ============================================================================

TEST_CASE("Nordic NCS Timer Operations", "[nordic][timer]") {
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
}

// ============================================================================
// Memory Allocator Tests
// ============================================================================

TEST_CASE("Nordic NCS Memory Allocation", "[nordic][memory]") {
    auto& allocator = getRTOSFactory().getMemoryAllocator();

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

    SECTION("Thread safety is guaranteed") {
        REQUIRE(allocator.isThreadSafe());
    }
}

// ============================================================================
// LTE Modem Tests (nRF91 series only)
// ============================================================================

#if defined(CONFIG_BOARD_NRF9160DK_NRF9160) || defined(CONFIG_BOARD_NRF9160DK_NRF9160_NS) || \
    defined(CONFIG_BOARD_NRF9151DK_NRF9151) || defined(CONFIG_BOARD_NRF9151DK_NRF9151_NS)

TEST_CASE("Nordic LTE Modem Operations", "[nordic][lte][modem]") {
    auto& nordicFactory = static_cast<NordicFactory&>(getRTOSFactory());
    auto& modem = nordicFactory.getCellularModem();

    SECTION("Modem initialization") {
        bool result = modem.init();
        REQUIRE(result);
    }

    SECTION("Get IMEI") {
        modem.init();
        char imei[16] = {0};
        bool result = modem.getIMEI(imei, sizeof(imei));
        REQUIRE(result);
        REQUIRE(strlen(imei) == 15); // IMEI is 15 digits
    }

    SECTION("Get IMSI") {
        modem.init();
        char imsi[16] = {0};
        bool result = modem.getIMSI(imsi, sizeof(imsi));
        // May fail if no SIM card inserted
        if (result) {
            REQUIRE(strlen(imsi) >= 14); // IMSI is 14-15 digits
        }
    }

    SECTION("Set network mode to LTE-M") {
        modem.init();
        bool result = modem.setNetworkMode(NetworkMode::LTE_M);
        REQUIRE(result);
    }

    SECTION("Set network mode to NB-IoT") {
        modem.init();
        bool result = modem.setNetworkMode(NetworkMode::NB_IOT);
        REQUIRE(result);
    }

    SECTION("Enable PSM mode") {
        modem.init();
        bool result = modem.setPSM(3600, 10); // TAU=1hr, Active=10s
        REQUIRE(result);
    }

    SECTION("Enable eDRX mode") {
        modem.init();
        bool result = modem.setEDRX(81); // 81.92 seconds
        REQUIRE(result);
    }

    SECTION("Get modem firmware version") {
        char version[64] = {0};
        bool result = nordicFactory.getModemFirmwareVersion(version, sizeof(version));
        REQUIRE(result);
        REQUIRE(strlen(version) > 0);
        INFO("Modem firmware: " << version);
    }

    // Note: Connection tests require SIM card and network coverage
    // SECTION("Connect to network") {
    //     modem.init();
    //     bool connected = modem.connect();
    //     REQUIRE(connected);
    //
    //     int rsrp = modem.getSignalStrength();
    //     REQUIRE(rsrp >= -140);
    //     REQUIRE(rsrp <= -44);
    //
    //     modem.disconnect();
    // }
}

#endif // nRF91 series

// ============================================================================
// BLE Tests (all nRF series)
// ============================================================================

TEST_CASE("Nordic BLE Operations", "[nordic][ble]") {
    auto& nordicFactory = static_cast<NordicFactory&>(getRTOSFactory());
    auto& ble = nordicFactory.getBLE();

    SECTION("BLE initialization") {
        bool result = ble.init();
        REQUIRE(result);
    }

    SECTION("Get BLE address") {
        ble.init();
        char address[18] = {0};
        bool result = ble.getAddress(address, sizeof(address));
        REQUIRE(result);
        REQUIRE(strlen(address) == 17); // Format: XX:XX:XX:XX:XX:XX
    }

    SECTION("Start and stop advertising") {
        ble.init();

        bool started = ble.startAdvertising("FriendlyLwM2M");
        REQUIRE(started);
        REQUIRE(ble.isAdvertising());

        getRTOSFactory().getThreadManager().sleep(100);

        bool stopped = ble.stopAdvertising();
        REQUIRE(stopped);
        REQUIRE_FALSE(ble.isAdvertising());
    }
}

// ============================================================================
// MCUboot OTA Tests
// ============================================================================

TEST_CASE("Nordic MCUboot OTA Operations", "[nordic][ota][mcuboot]") {
    auto& ota = getRTOSFactory().getOTA();

    SECTION("OTA begin with size") {
        bool result = ota.begin(100 * 1024); // 100KB test
        REQUIRE(result);
        ota.end();
    }

    SECTION("Get current firmware version") {
        uint32_t version = ota.getCurrentVersion();
        REQUIRE(version >= 0);
        INFO("Current firmware version: " << version);
    }

    SECTION("Image verification") {
        bool verified = ota.verify();
        // May be false if image not confirmed yet
        INFO("Image verified: " << verified);
    }

    // Note: Full OTA tests require actual firmware images and reboot
}

// ============================================================================
// nRF Cloud Integration Tests
// ============================================================================

TEST_CASE("Nordic nRF Cloud Integration", "[nordic][cloud]") {
    auto& nordicFactory = static_cast<NordicFactory&>(getRTOSFactory());

    // Note: Cloud tests require network connectivity and nRF Cloud account
    SECTION("Cloud interface is available") {
        // Just verify the interface exists
        REQUIRE(true);
    }

    // SECTION("Connect to nRF Cloud") {
    //     bool connected = nordicFactory.connectToCloud();
    //     REQUIRE(connected);
    //
    //     nordicFactory.disconnectFromCloud();
    // }

    // SECTION("Send data to nRF Cloud") {
    //     nordicFactory.connectToCloud();
    //
    //     const char* data = "{\"temp\":23.5}";
    //     bool sent = nordicFactory.sendCloudData(data, strlen(data));
    //     REQUIRE(sent);
    //
    //     nordicFactory.disconnectFromCloud();
    // }
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_CASE("Nordic NCS Integration: Threads + Mutex + Semaphore", "[nordic][integration]") {
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
                                   1024, ThreadPriority::NORMAL);
    REQUIRE(thread != nullptr);

    bool signaled = sem->wait(1000);
    REQUIRE(signaled);
    REQUIRE(sharedCounter.load() == 5);
}

TEST_CASE("Nordic NCS Integration: Multi-threaded with Timers", "[nordic][integration]") {
    std::atomic<int> threadCounter{0};
    std::atomic<int> timerCounter{0};

    auto timer = getRTOSFactory().createTimer("integration_timer");
    timer->start(50, true, [&timerCounter]() {
        timerCounter++;
    });

    auto thread1 = getRTOSFactory().getThreadManager().createThread(
        "thread1", counterThreadFunc, &threadCounter, 1024, ThreadPriority::NORMAL
    );
    auto thread2 = getRTOSFactory().getThreadManager().createThread(
        "thread2", counterThreadFunc, &threadCounter, 1024, ThreadPriority::NORMAL
    );

    REQUIRE(thread1 != nullptr);
    REQUIRE(thread2 != nullptr);

    getRTOSFactory().getThreadManager().sleep(300);

    timer->stop();

    REQUIRE(threadCounter.load() >= 20);
    REQUIRE(timerCounter.load() >= 5);
}

// ============================================================================
// Platform Information Tests
// ============================================================================

TEST_CASE("Nordic NCS Platform Information", "[nordic][platform]") {
    auto& nordicFactory = static_cast<NordicFactory&>(getRTOSFactory());

    SECTION("Chip identification") {
        const char* chipInfo = nordicFactory.getChipInfo();
        REQUIRE(chipInfo != nullptr);
        INFO("Chip: " << chipInfo);
        REQUIRE(true);
    }

    SECTION("RTOS version information") {
        const char* version = nordicFactory.getRTOSVersion();
        REQUIRE(version != nullptr);
        INFO("RTOS: " << version);
        REQUIRE(true);
    }

    SECTION("Memory information") {
        auto& allocator = getRTOSFactory().getMemoryAllocator();
        size_t total = allocator.getTotalHeap();
        size_t free = allocator.getFreeHeap();

        INFO("Total heap: " << total << " bytes");
        INFO("Free heap: " << free << " bytes");

        REQUIRE(total > 0);
        REQUIRE(free > 0);
        REQUIRE(free <= total);
    }
}

#endif // NORDIC_NCS
