#include "catch_amalgamated.hpp"
#include "SafeQueue.h"

TEST_CASE("SafeQueue: Constructor", "[constructor]") {
    SafeQueue<int, 5> queue;
    REQUIRE(queue.is_empty());
    REQUIRE(queue.size() == 0);
}

TEST_CASE("SafeQueue: push and pop", "[push][pop]") {
    SafeQueue<int, 5> queue;
    int values[5] = {1, 2, 3, 4, 5};

    SECTION("Push one element into the queue") {
        REQUIRE(queue.is_empty());
        REQUIRE(queue.push(values));
        REQUIRE(queue.size() == 1);
        REQUIRE(*queue.at(0) == values[0]);
    }

    SECTION("Push multiple elements into the queue as array") {
        REQUIRE(queue.is_empty());
        REQUIRE(queue.push(values, 5));
        for (int i = 0; i < 5; ++i) {
            REQUIRE(*queue.at(i) == values[i]);
        }
    }

    SECTION("Push multiple elements into the queue one by one") {
        REQUIRE(queue.is_empty());
        for (size_t i = 0; i < 5; ++i) {
            REQUIRE(queue.push(&values[i]));
        }
        for (int i = 0; i < 5; ++i) {
            REQUIRE(*queue.at(i) == values[i]);
        }
    }

    SECTION("Push into a full queue one element") {
        int value = 256;
        REQUIRE(queue.is_empty());
        REQUIRE(queue.push(values, 5));
        REQUIRE(queue.size() == 5);
        REQUIRE_FALSE(queue.push(&value));
    }

    SECTION("Push into a queue array of elements with overhead") {
        REQUIRE(queue.is_empty());
        REQUIRE(queue.push(values, 3));
        REQUIRE(queue.size() == 3);
        REQUIRE_FALSE(queue.push(values, 5));
        REQUIRE(queue.size() == 3);
        REQUIRE(queue.push(values, 2));
        REQUIRE(queue.size() == 5);
        REQUIRE_FALSE(queue.push(values, 5));
        REQUIRE(queue.size() == 5);
    }

    SECTION("Pop one element from the queue") {
        REQUIRE(queue.is_empty());
        queue.push(values, 5);
        REQUIRE(queue.size() == 5);
        REQUIRE(queue.pop());
        REQUIRE(queue.size() == 4);
        for (int i = 1; i < 5; ++i) {
            REQUIRE(*queue.at(i-1) == values[i]);
        }
    }

    SECTION("Pop elements from the queue one by one") {
        REQUIRE(queue.is_empty());
        queue.push(values, 5);
        REQUIRE(queue.size() == 5);
        REQUIRE(queue.pop());
        REQUIRE(queue.size() == 4);
        REQUIRE(queue.pop());
        REQUIRE(queue.size() == 3);
        REQUIRE(queue.pop());
        REQUIRE(queue.size() == 2);
        REQUIRE(queue.pop());
        REQUIRE(queue.size() == 1);
        REQUIRE(queue.pop());
        REQUIRE(queue.size() == 0);
    }

    SECTION("Pop elements from the queue as set") {
        REQUIRE(queue.is_empty());
        queue.push(values, 5);
        REQUIRE(queue.size() == 5);
        REQUIRE(queue.pop(3));
        REQUIRE(queue.size() == 2);
        for (int i = 3; i < 5; ++i) {
            REQUIRE(*queue.at(i-3) == values[i]);
        }
        REQUIRE(queue.pop(2));
        REQUIRE(queue.size() == 0);
    }

    SECTION("Pop from an empty queue") {
        REQUIRE(queue.is_empty());
        REQUIRE_FALSE(queue.pop());
    }

    SECTION("Pop from an empty queue") {
        REQUIRE(queue.is_empty());
        queue.push(values, 5);
    }
}

TEST_CASE("SafeQueue: to_vector", "[to_vector]") {
    SafeQueue<int, 5> queue;

    SECTION("Convert empty queue to vector") {
        REQUIRE(queue.size() == 0);
        auto vec = queue.to_vector();
        REQUIRE(vec.size() == 0);
    }

    SECTION("Convert not empty queue to vector") {
        int values[7] = {1, 2, 3, 4, 5, 6, 7};
        REQUIRE(queue.push(values, 4));
        REQUIRE(queue.size() == 4);
        for (int i = 0; i < 4; ++i) {
            REQUIRE(*queue.at(i) == values[i]);
        }
        
        auto vec = queue.to_vector(4);
        REQUIRE(vec.size() == 4);
        REQUIRE(std::equal(vec.begin(), vec.end(), values));

        REQUIRE(queue.push(values + 4));
        vec = queue.to_vector(5);
        REQUIRE(vec.size() == 5);
        REQUIRE(std::equal(vec.begin(), vec.end(), values));

        REQUIRE(queue.pop());
        REQUIRE(queue.pop());
        REQUIRE(queue.size() == 3);
        REQUIRE(queue.push(values + 5, 2));
        REQUIRE(queue.size() == 5);

        vec = queue.to_vector(5);
        REQUIRE(vec.size() == 5);
        REQUIRE(std::equal(vec.begin(), vec.end(), values + 2));
    }
}

TEST_CASE("SafeQueue: element access", "[front][back][at]") {
    SafeQueue<int, 5> queue;
     SafeQueue<int, 6> notFullQueue;
    int values[5] = {1, 2, 3, 4, 5};
    REQUIRE(queue.push(values, 5));
    REQUIRE(notFullQueue.push(values, 5));

    SECTION("Access front element by ptr") {
        REQUIRE(*queue.front() == 1);
        REQUIRE(*notFullQueue.front() == 1);
    }

    SECTION("Access back element by ptr") {
        REQUIRE(*queue.back() == 5);
        REQUIRE(*notFullQueue.back() == 5);
    }

    SECTION("Access element at index by ptr") {
        REQUIRE(*queue.at(3) == 4);
        REQUIRE(*notFullQueue.at(3) == 4);
    }

    SECTION("Access element at out of range index by ptr") {
        REQUIRE(queue.at(256) == NULL);
        REQUIRE(notFullQueue.at(256) == NULL);
    }

    SECTION("Access front element by value") {
        int value = 0;
        REQUIRE(queue.front(&value));
        REQUIRE(value == 1);
        value = 0;
        REQUIRE(notFullQueue.front(&value));
        REQUIRE(value == 1);
    }

    SECTION("Access back element by value") {
        int value = 0;
        REQUIRE(queue.back(&value));
        REQUIRE(value == 5);
        value = 0;
        REQUIRE(notFullQueue.back(&value));
        REQUIRE(value == 5);
    }

    SECTION("Access element at index by value") {
        int value = 0;
        REQUIRE(queue.at(3, &value));
        REQUIRE(value == 4);
        value = 0;
        REQUIRE(notFullQueue.at(3, &value));
        REQUIRE(value == 4);
    }

    SECTION("Access element at out of range index by value") {
        int value = 0;
        REQUIRE_FALSE(queue.at(256, &value));
        REQUIRE_FALSE(notFullQueue.at(256, &value));
    }

    SECTION("Access element in empty queue by ptr") {
        SafeQueue<int, 5> emptyQueue;
        REQUIRE(emptyQueue.front() == NULL);
        REQUIRE(emptyQueue.back() == NULL);
        REQUIRE(emptyQueue.at(0) == NULL);
    }

    SECTION("Access element in empty queue by value") {
        SafeQueue<int, 5> emptyQueue;
        int value = 0;
        REQUIRE_FALSE(emptyQueue.front(&value));
        REQUIRE_FALSE(emptyQueue.back(&value));
        REQUIRE_FALSE(emptyQueue.at(0, &value));
    }
}

TEST_CASE("SafeQueue: state checks", "[is_empty][is_full][size][available_space][element_size]") {
    SafeQueue<int, 5> queue;
    int values[5] = {1, 2, 3, 4, 5};

    SECTION("Check empty state") {
        REQUIRE(queue.is_empty());
        queue.push(values);
        REQUIRE_FALSE(queue.is_empty());
    }

    SECTION("Check full state") {
        REQUIRE_FALSE(queue.is_full());
        REQUIRE(queue.push(values));
        REQUIRE_FALSE(queue.is_full());
        REQUIRE(queue.push(values, 4));
        REQUIRE(queue.is_full());
    }

    SECTION("Check size") {
        REQUIRE(queue.size() == 0);
        REQUIRE(queue.push(values, 3));
        REQUIRE(queue.size() == 3);
        REQUIRE(queue.push(values));
        REQUIRE(queue.size() == 4);
    }

    SECTION("Check available space") {
        REQUIRE(queue.available_space() == 5);
        REQUIRE(queue.push(values, 3));
        REQUIRE(queue.available_space() == 2);
        REQUIRE(queue.push(values, 2));
        REQUIRE(queue.available_space() == 0);
    }

    SECTION("Check element size") {
        struct TestStruct {
            int a;
            int b;
            char c[5];
            float f;
        };
        SafeQueue<TestStruct, 5> queueOfStructs;
        REQUIRE(queueOfStructs.element_size() == sizeof(TestStruct));
        REQUIRE(queue.element_size() == sizeof(int));
    }
}

TEST_CASE("SafeQueue: clear", "[clear]") {
    SafeQueue<int, 5> queue;
    int values[5] = {1, 2, 3, 4, 5};
    REQUIRE(queue.push(values, 5));
    REQUIRE(queue.size() == 5);
    REQUIRE_FALSE(queue.is_empty());
    queue.clear();
    REQUIRE(queue.is_empty());
    REQUIRE(queue.size() == 0);
}

// ============================================================================
// Thread Safety Tests
// ============================================================================

TEST_CASE("SafeQueue: concurrent push from multiple threads", "[threading][push]") {
    SafeQueue<int, 10000> queue;
    constexpr int NUM_PRODUCERS = 10;
    constexpr int ITEMS_PER_PRODUCER = 100;
    std::vector<std::thread> producers;
    std::atomic<int> pushCount{0};

    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back([&queue, &pushCount, i]() {
            for (int j = 0; j < ITEMS_PER_PRODUCER; ++j) {
                int value = i * ITEMS_PER_PRODUCER + j;
                if (queue.push(&value, 1)) {
                    ++pushCount;
                }
            }
        });
    }

    for (auto& t : producers) {
        t.join();
    }

    REQUIRE(pushCount.load() == NUM_PRODUCERS * ITEMS_PER_PRODUCER);
    REQUIRE(queue.size() == NUM_PRODUCERS * ITEMS_PER_PRODUCER);
}

TEST_CASE("SafeQueue: concurrent push/pop producer-consumer pattern", "[threading][push][pop]") {
    SafeQueue<int, 5000> queue;
    std::atomic<int> itemsProduced{0};
    std::atomic<int> itemsConsumed{0};
    std::atomic<bool> stopConsumers{false};
    constexpr int TARGET_ITEMS = 1000;

    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < TARGET_ITEMS; ++i) {
            while (!queue.push(&i, 1)) {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
            ++itemsProduced;
        }
        stopConsumers = true;
    });

    // Consumer threads
    std::vector<std::thread> consumers;
    for (int i = 0; i < 3; ++i) {
        consumers.emplace_back([&]() {
            int value;
            while (!stopConsumers || !queue.is_empty()) {
                if (queue.pop(&value, 1)) {
                    ++itemsConsumed;
                } else {
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                }
            }
        });
    }

    producer.join();
    for (auto& c : consumers) {
        c.join();
    }

    REQUIRE(itemsProduced == TARGET_ITEMS);
    REQUIRE(itemsConsumed == TARGET_ITEMS);
    REQUIRE(queue.is_empty());
}

TEST_CASE("SafeQueue: stress test with many threads and operations", "[threading][stress]") {
    SafeQueue<int, 20000> queue;
    std::atomic<int> pushCount{0};
    std::atomic<int> popCount{0};
    constexpr int NUM_THREADS = 20;
    constexpr int OPS_PER_THREAD = 200;
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < OPS_PER_THREAD; ++j) {
                if (i % 2 == 0) {
                    // Even threads push
                    int value = i * OPS_PER_THREAD + j;
                    if (queue.push(&value, 1)) {
                        ++pushCount;
                    }
                } else {
                    // Odd threads pop
                    int value;
                    if (queue.pop(&value, 1)) {
                        ++popCount;
                    }
                }
                // Small delay to increase contention
                std::this_thread::yield();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Some items may remain in queue
    REQUIRE(pushCount.load() >= popCount.load());
    REQUIRE(queue.size() == (pushCount.load() - popCount.load()));
}

TEST_CASE("SafeQueue: concurrent clear operations", "[threading][clear]") {
    SafeQueue<int, 10000> queue;

    // Fill queue
    for (int i = 0; i < 1000; ++i) {
        queue.push(&i, 1);
    }

    REQUIRE(queue.size() == 1000);

    // Multiple threads clearing simultaneously
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&]() {
            queue.clear();
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    REQUIRE(queue.is_empty());
    REQUIRE(queue.size() == 0);
}

TEST_CASE("SafeQueue: thread safety with mixed operations", "[threading][mixed]") {
    SafeQueue<int, 5000> queue;
    std::atomic<bool> stop{false};
    std::atomic<int> totalPushed{0};
    std::atomic<int> totalPopped{0};

    // Pusher threads
    std::vector<std::thread> pushers;
    for (int i = 0; i < 3; ++i) {
        pushers.emplace_back([&, i]() {
            int count = 0;
            while (!stop && count < 500) {
                int value = i * 1000 + count;
                if (queue.push(&value, 1)) {
                    ++totalPushed;
                    ++count;
                }
                std::this_thread::yield();
            }
        });
    }

    // Popper threads
    std::vector<std::thread> poppers;
    for (int i = 0; i < 3; ++i) {
        poppers.emplace_back([&]() {
            while (!stop || !queue.is_empty()) {
                int value;
                if (queue.pop(&value, 1)) {
                    ++totalPopped;
                }
                std::this_thread::yield();
            }
        });
    }

    // Let pushers finish
    for (auto& t : pushers) {
        t.join();
    }

    stop = true;

    // Let poppers finish
    for (auto& t : poppers) {
        t.join();
    }

    REQUIRE(totalPushed.load() == 1500); // 3 threads * 500 items
    REQUIRE(totalPopped.load() == totalPushed.load());
    REQUIRE(queue.is_empty());
}

TEST_CASE("SafeQueue: concurrent size queries don't block operations", "[threading][size]") {
    SafeQueue<int, 5000> queue;
    std::atomic<bool> stop{false};
    std::atomic<int> sizeQueries{0};

    // Worker thread doing push/pop
    std::thread worker([&]() {
        for (int i = 0; i < 1000 && !stop; ++i) {
            queue.push(&i, 1);
            int value;
            queue.pop(&value, 1);
        }
    });

    // Query threads
    std::vector<std::thread> queriers;
    for (int i = 0; i < 5; ++i) {
        queriers.emplace_back([&]() {
            while (!stop) {
                queue.size();
                queue.is_empty();
                queue.is_full();
                ++sizeQueries;
                std::this_thread::yield();
            }
        });
    }

    worker.join();
    stop = true;

    for (auto& t : queriers) {
        t.join();
    }

    INFO("Performed " << sizeQueries.load() << " size queries concurrently");
    REQUIRE(sizeQueries.load() > 0);
}

TEST_CASE("SafeQueue: no data races under thread sanitizer", "[threading][sanitizer]") {
    // This test is designed to be run with ThreadSanitizer (-fsanitize=thread)
    // to detect any data races in SafeQueue implementation

    SafeQueue<int, 1000> queue;
    constexpr int NUM_THREADS = 10;
    constexpr int OPS_PER_THREAD = 100;
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&, i]() {
            // Mix of all operations
            for (int j = 0; j < OPS_PER_THREAD; ++j) {
                int value = i * OPS_PER_THREAD + j;

                switch (j % 5) {
                    case 0:
                        queue.push(&value, 1);
                        break;
                    case 1: {
                        int tmp;
                        queue.pop(&tmp, 1);
                        break;
                    }
                    case 2:
                        queue.size();
                        break;
                    case 3:
                        queue.is_empty();
                        break;
                    case 4:
                        queue.is_full();
                        break;
                }

                std::this_thread::yield();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // If we reach here without ThreadSanitizer warnings, thread safety is good
    REQUIRE(true);
}
