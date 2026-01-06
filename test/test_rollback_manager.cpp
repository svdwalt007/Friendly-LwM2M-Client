/**
 * @file test_rollback_manager.cpp
 * @brief Unit tests for Rollback and Recovery Manager
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "firmware/rollback_manager.h"
#include "platform/platform_abstraction.h"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

namespace lwm2m {
namespace firmware {
namespace test {

// ============================================================================
// Mock Platform Implementations
// ============================================================================

class MockPartitionManager : public platform::IPartitionManager {
public:
    platform::PlatformResult initialize() override {
        return platform::PlatformResult::SUCCESS;
    }

    std::vector<platform::PartitionInfo> getPartitions() override {
        std::vector<platform::PartitionInfo> partitions;
        
        platform::PartitionInfo slotA;
        slotA.name = "rootfs_a";
        slotA.device = "/dev/mock0";
        slotA.slot = platform::PartitionSlot::SLOT_A;
        slotA.size = 64 * 1024 * 1024;
        slotA.isActive = (activeSlot_ == platform::PartitionSlot::SLOT_A);
        slotA.isBootable = slotABootable_;
        slotA.version = "1.0.0";
        partitions.push_back(slotA);
        
        platform::PartitionInfo slotB;
        slotB.name = "rootfs_b";
        slotB.device = "/dev/mock1";
        slotB.slot = platform::PartitionSlot::SLOT_B;
        slotB.size = 64 * 1024 * 1024;
        slotB.isActive = (activeSlot_ == platform::PartitionSlot::SLOT_B);
        slotB.isBootable = slotBBootable_;
        slotB.version = "1.1.0";
        partitions.push_back(slotB);
        
        return partitions;
    }

    std::optional<platform::PartitionInfo> getPartition(platform::PartitionSlot slot) override {
        auto partitions = getPartitions();
        for (const auto& p : partitions) {
            if (p.slot == slot) return p;
        }
        return std::nullopt;
    }

    platform::PartitionSlot getActiveSlot() override {
        return activeSlot_;
    }

    bool isSlotBootable(platform::PartitionSlot slot) override {
        if (slot == platform::PartitionSlot::SLOT_A) return slotABootable_;
        if (slot == platform::PartitionSlot::SLOT_B) return slotBBootable_;
        return false;
    }

    platform::PlatformResult setSlotBootable(platform::PartitionSlot slot, bool bootable) override {
        if (slot == platform::PartitionSlot::SLOT_A) slotABootable_ = bootable;
        else if (slot == platform::PartitionSlot::SLOT_B) slotBBootable_ = bootable;
        return platform::PlatformResult::SUCCESS;
    }

    platform::PlatformResult switchSlot(platform::PartitionSlot targetSlot) override {
        if (!isSlotBootable(targetSlot)) {
            return platform::PlatformResult::ERROR_INVALID_PARTITION;
        }
        nextBootSlot_ = targetSlot;
        switchCount_++;
        return platform::PlatformResult::SUCCESS;
    }

    platform::PartitionSlot getInactiveSlot() override {
        return (activeSlot_ == platform::PartitionSlot::SLOT_A) ?
               platform::PartitionSlot::SLOT_B : platform::PartitionSlot::SLOT_A;
    }

    platform::PlatformResult verifyPartition(platform::PartitionSlot slot,
                                              const std::vector<uint8_t>& expectedChecksum) override {
        return platform::PlatformResult::SUCCESS;
    }

    std::vector<uint8_t> calculateChecksum(platform::PartitionSlot slot) override {
        return std::vector<uint8_t>(32, static_cast<uint8_t>(slot));
    }

    // Test helpers
    void setActiveSlot(platform::PartitionSlot slot) { activeSlot_ = slot; }
    void setSlotABootable(bool bootable) { slotABootable_ = bootable; }
    void setSlotBBootable(bool bootable) { slotBBootable_ = bootable; }
    platform::PartitionSlot getNextBootSlot() const { return nextBootSlot_; }
    int getSwitchCount() const { return switchCount_; }

private:
    platform::PartitionSlot activeSlot_ = platform::PartitionSlot::SLOT_A;
    platform::PartitionSlot nextBootSlot_ = platform::PartitionSlot::SLOT_A;
    bool slotABootable_ = true;
    bool slotBBootable_ = true;
    int switchCount_ = 0;
};

// ============================================================================
// Test Fixtures
// ============================================================================

class RollbackManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temp directory for snapshots
        testDir_ = fs::temp_directory_path() / "lwm2m_test";
        fs::create_directories(testDir_);
        
        // Configure rollback manager
        RollbackManager::Config config;
        config.snapshotStoragePath = testDir_.string();
        config.maxSnapshots = 5;
        config.autoSnapshot = false;
        config.maxBootAttempts = 3;
        
        manager_ = std::make_unique<RollbackManager>(config);
        
        // Initialize with mock would require platform factory modifications
        // For now, test the logic directly
    }

    void TearDown() override {
        manager_.reset();
        fs::remove_all(testDir_);
    }

    std::unique_ptr<RollbackManager> manager_;
    fs::path testDir_;
};

// ============================================================================
// Initialization Tests
// ============================================================================

TEST_F(RollbackManagerTest, InitialState) {
    EXPECT_FALSE(manager_->isInitialized());
    EXPECT_FALSE(manager_->isRollbackInProgress());
}

// ============================================================================
// Snapshot Tests
// ============================================================================

TEST_F(RollbackManagerTest, SnapshotStoragePathCreated) {
    EXPECT_TRUE(fs::exists(testDir_));
}

// ============================================================================
// Slot Management Tests (Mock-based)
// ============================================================================

TEST(MockPartitionTest, InitialActiveSlot) {
    MockPartitionManager mock;
    EXPECT_EQ(mock.getActiveSlot(), platform::PartitionSlot::SLOT_A);
}

TEST(MockPartitionTest, GetInactiveSlot) {
    MockPartitionManager mock;
    EXPECT_EQ(mock.getInactiveSlot(), platform::PartitionSlot::SLOT_B);
    
    mock.setActiveSlot(platform::PartitionSlot::SLOT_B);
    EXPECT_EQ(mock.getInactiveSlot(), platform::PartitionSlot::SLOT_A);
}

TEST(MockPartitionTest, SlotBootability) {
    MockPartitionManager mock;
    
    EXPECT_TRUE(mock.isSlotBootable(platform::PartitionSlot::SLOT_A));
    EXPECT_TRUE(mock.isSlotBootable(platform::PartitionSlot::SLOT_B));
    
    mock.setSlotABootable(false);
    EXPECT_FALSE(mock.isSlotBootable(platform::PartitionSlot::SLOT_A));
    EXPECT_TRUE(mock.isSlotBootable(platform::PartitionSlot::SLOT_B));
}

TEST(MockPartitionTest, SwitchToBootableSlot) {
    MockPartitionManager mock;
    
    auto result = mock.switchSlot(platform::PartitionSlot::SLOT_B);
    EXPECT_EQ(result, platform::PlatformResult::SUCCESS);
    EXPECT_EQ(mock.getNextBootSlot(), platform::PartitionSlot::SLOT_B);
    EXPECT_EQ(mock.getSwitchCount(), 1);
}

TEST(MockPartitionTest, SwitchToNonBootableSlotFails) {
    MockPartitionManager mock;
    mock.setSlotBBootable(false);
    
    auto result = mock.switchSlot(platform::PartitionSlot::SLOT_B);
    EXPECT_EQ(result, platform::PlatformResult::ERROR_INVALID_PARTITION);
}

TEST(MockPartitionTest, GetPartitions) {
    MockPartitionManager mock;
    auto partitions = mock.getPartitions();
    
    ASSERT_EQ(partitions.size(), 2u);
    EXPECT_EQ(partitions[0].slot, platform::PartitionSlot::SLOT_A);
    EXPECT_EQ(partitions[1].slot, platform::PartitionSlot::SLOT_B);
}

TEST(MockPartitionTest, GetSpecificPartition) {
    MockPartitionManager mock;
    
    auto slotA = mock.getPartition(platform::PartitionSlot::SLOT_A);
    ASSERT_TRUE(slotA.has_value());
    EXPECT_EQ(slotA->name, "rootfs_a");
    
    auto slotB = mock.getPartition(platform::PartitionSlot::SLOT_B);
    ASSERT_TRUE(slotB.has_value());
    EXPECT_EQ(slotB->name, "rootfs_b");
}

TEST(MockPartitionTest, CalculateChecksum) {
    MockPartitionManager mock;
    
    auto checksumA = mock.calculateChecksum(platform::PartitionSlot::SLOT_A);
    auto checksumB = mock.calculateChecksum(platform::PartitionSlot::SLOT_B);
    
    EXPECT_EQ(checksumA.size(), 32u);
    EXPECT_EQ(checksumB.size(), 32u);
    EXPECT_NE(checksumA, checksumB);
}

// ============================================================================
// Snapshot Serialization Tests
// ============================================================================

TEST(SnapshotSerializationTest, CreateSnapshotFile) {
    fs::path testDir = fs::temp_directory_path() / "snapshot_test";
    fs::create_directories(testDir);
    
    // Create a test snapshot file
    std::string snapshotContent = R"({
        "id": "test-1234",
        "version": "1.0.0",
        "description": "Test snapshot",
        "timestamp": 1704067200,
        "slot": 0,
        "verified": false,
        "stateChecksum": "0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"
    })";
    
    std::ofstream file(testDir / "test-1234.json");
    file << snapshotContent;
    file.close();
    
    EXPECT_TRUE(fs::exists(testDir / "test-1234.json"));
    
    fs::remove_all(testDir);
}

// ============================================================================
// Component Info Tests
// ============================================================================

TEST(ComponentInfoTest, CreateComponent) {
    ComponentInfo component;
    component.name = "kernel";
    component.version = "5.15.0";
    component.device = "/dev/mtd2";
    component.size = 4 * 1024 * 1024;
    component.mandatory = true;
    component.updatePriority = 0;
    
    EXPECT_EQ(component.name, "kernel");
    EXPECT_EQ(component.version, "5.15.0");
    EXPECT_TRUE(component.mandatory);
}

// ============================================================================
// Result Code Tests
// ============================================================================

TEST(RollbackResultTest, ResultCodes) {
    EXPECT_EQ(static_cast<int>(RollbackResult::SUCCESS), 0);
    EXPECT_NE(RollbackResult::SUCCESS, RollbackResult::ERROR_NOT_INITIALIZED);
    EXPECT_NE(RollbackResult::ERROR_SNAPSHOT_NOT_FOUND, 
              RollbackResult::ERROR_PARTITION_NOT_BOOTABLE);
}

// ============================================================================
// Event Callback Tests
// ============================================================================

TEST(RollbackEventTest, EventTypes) {
    std::vector<RollbackEvent> events = {
        RollbackEvent::SNAPSHOT_CREATED,
        RollbackEvent::SNAPSHOT_DELETED,
        RollbackEvent::ROLLBACK_STARTED,
        RollbackEvent::ROLLBACK_COMPLETED,
        RollbackEvent::ROLLBACK_FAILED,
        RollbackEvent::SLOT_SWITCHED,
        RollbackEvent::RECOVERY_ENTERED,
        RollbackEvent::RECOVERY_EXITED,
        RollbackEvent::BOOT_VERIFIED
    };
    
    EXPECT_EQ(events.size(), 9u);
}

// ============================================================================
// Configuration Tests
// ============================================================================

TEST(RollbackConfigTest, DefaultConfig) {
    RollbackManager::Config config;
    
    EXPECT_EQ(config.maxSnapshots, 5u);
    EXPECT_TRUE(config.autoSnapshot);
    EXPECT_TRUE(config.autoRollbackOnFailure);
    EXPECT_EQ(config.maxBootAttempts, 3);
    EXPECT_TRUE(config.verifyAfterRollback);
}

TEST(RollbackConfigTest, CustomConfig) {
    RollbackManager::Config config;
    config.snapshotStoragePath = "/custom/path";
    config.maxSnapshots = 10;
    config.autoSnapshot = false;
    config.maxBootAttempts = 5;
    
    EXPECT_EQ(config.snapshotStoragePath, "/custom/path");
    EXPECT_EQ(config.maxSnapshots, 10u);
    EXPECT_FALSE(config.autoSnapshot);
    EXPECT_EQ(config.maxBootAttempts, 5);
}

// ============================================================================
// Integration Scenario Tests
// ============================================================================

TEST(ABPartitionScenarioTest, NormalUpdateFlow) {
    MockPartitionManager mock;
    
    // Initial state: Active on A, B is bootable
    EXPECT_EQ(mock.getActiveSlot(), platform::PartitionSlot::SLOT_A);
    EXPECT_TRUE(mock.isSlotBootable(platform::PartitionSlot::SLOT_B));
    
    // Simulate update to slot B
    // 1. Write firmware to inactive slot (B)
    // 2. Mark B as bootable
    mock.setSlotBBootable(true);
    
    // 3. Switch boot slot
    auto result = mock.switchSlot(platform::PartitionSlot::SLOT_B);
    EXPECT_EQ(result, platform::PlatformResult::SUCCESS);
    
    // Verify next boot will be slot B
    EXPECT_EQ(mock.getNextBootSlot(), platform::PartitionSlot::SLOT_B);
}

TEST(ABPartitionScenarioTest, RollbackFlow) {
    MockPartitionManager mock;
    
    // Simulate failed update - roll back to A
    mock.setActiveSlot(platform::PartitionSlot::SLOT_B);  // Currently booted B
    
    // Rollback to A
    auto result = mock.switchSlot(platform::PartitionSlot::SLOT_A);
    EXPECT_EQ(result, platform::PlatformResult::SUCCESS);
    EXPECT_EQ(mock.getNextBootSlot(), platform::PartitionSlot::SLOT_A);
}

TEST(ABPartitionScenarioTest, FailedSlotBootCheck) {
    MockPartitionManager mock;
    
    // Slot B is corrupted
    mock.setSlotBBootable(false);
    
    // Attempt to switch to B should fail
    auto result = mock.switchSlot(platform::PartitionSlot::SLOT_B);
    EXPECT_EQ(result, platform::PlatformResult::ERROR_INVALID_PARTITION);
    
    // Should still be able to boot A
    result = mock.switchSlot(platform::PartitionSlot::SLOT_A);
    EXPECT_EQ(result, platform::PlatformResult::SUCCESS);
}

} // namespace test
} // namespace firmware
} // namespace lwm2m
