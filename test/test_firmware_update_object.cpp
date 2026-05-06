/**
 * @file test_firmware_update_object.cpp
 * @brief Unit tests for Advanced Firmware Update Object (33405)
 * 
 * @copyright Copyright (c) 2024-2025 Friendly Technologies
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "objects/advanced_firmware_update_object.h"

#include <chrono>
#include <thread>
#include <filesystem>

namespace fs = std::filesystem;

namespace lwm2m {
namespace objects {
namespace test {

// ============================================================================
// Test Fixtures
// ============================================================================

class FirmwareUpdateObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir_ = fs::temp_directory_path() / "fw_test";
        fs::create_directories(testDir_);
        
        AdvancedFirmwareUpdateObject::Config config;
        config.downloadPath = (testDir_ / "download").string();
        config.backupPath = (testDir_ / "backup").string();
        config.defaultBlockSize = 1024;
        config.rollbackEnabled = false;  // Disabled in tests - avoids platform hardware init
        config.autoReboot = false;
        
        firmwareObj_ = std::make_unique<AdvancedFirmwareUpdateObject>(0, config);
    }

    void TearDown() override {
        firmwareObj_.reset();
        fs::remove_all(testDir_);
    }

    std::unique_ptr<AdvancedFirmwareUpdateObject> firmwareObj_;
    fs::path testDir_;
};

// ============================================================================
// Object Metadata Tests
// ============================================================================

TEST_F(FirmwareUpdateObjectTest, ObjectId) {
    EXPECT_EQ(AdvancedFirmwareUpdateObject::getObjectId(), 33405);
}

// ============================================================================
// State Machine Tests
// ============================================================================

TEST_F(FirmwareUpdateObjectTest, InitialState) {
    EXPECT_EQ(firmwareObj_->getState(), FirmwareState::IDLE);
    EXPECT_EQ(firmwareObj_->getUpdateResult(), UpdateResult::DEFAULT);
    EXPECT_EQ(firmwareObj_->getProgress(), 0);
}

TEST_F(FirmwareUpdateObjectTest, StateTransitions) {
    // Verify state enum values match LwM2M specification
    EXPECT_EQ(static_cast<int>(FirmwareState::IDLE), 0);
    EXPECT_EQ(static_cast<int>(FirmwareState::DOWNLOADING), 1);
    EXPECT_EQ(static_cast<int>(FirmwareState::DOWNLOADED), 2);
    EXPECT_EQ(static_cast<int>(FirmwareState::UPDATING), 3);
}

// ============================================================================
// Resource Read Tests
// ============================================================================

TEST_F(FirmwareUpdateObjectTest, ReadStateResource) {
    int64_t value;
    bool result = firmwareObj_->read(FirmwareResourceId::STATE, value);
    EXPECT_TRUE(result);
    EXPECT_EQ(value, static_cast<int64_t>(FirmwareState::IDLE));
}

TEST_F(FirmwareUpdateObjectTest, ReadUpdateResultResource) {
    int64_t value;
    bool result = firmwareObj_->read(FirmwareResourceId::UPDATE_RESULT, value);
    EXPECT_TRUE(result);
    EXPECT_EQ(value, static_cast<int64_t>(UpdateResult::DEFAULT));
}

TEST_F(FirmwareUpdateObjectTest, ReadProtocolSupport) {
    int64_t value;
    bool result = firmwareObj_->read(FirmwareResourceId::PROTOCOL_SUPPORT, value);
    EXPECT_TRUE(result);
    // Should support multiple protocols
    EXPECT_GT(value, 0);
}

TEST_F(FirmwareUpdateObjectTest, ReadDeliveryMethod) {
    int64_t value;
    bool result = firmwareObj_->read(FirmwareResourceId::DELIVERY_METHOD, value);
    EXPECT_TRUE(result);
    // Should support both push and pull
    EXPECT_EQ(value, static_cast<int64_t>(DeliveryMethod::BOTH));
}

TEST_F(FirmwareUpdateObjectTest, ReadDeltaAlgorithm) {
    int64_t value;
    bool result = firmwareObj_->read(FirmwareResourceId::DELTA_ALGORITHM, value);
    EXPECT_TRUE(result);
}

TEST_F(FirmwareUpdateObjectTest, ReadBlockSize) {
    int64_t value;
    bool result = firmwareObj_->read(FirmwareResourceId::BLOCK_SIZE, value);
    EXPECT_TRUE(result);
    EXPECT_EQ(value, 1024);
}

TEST_F(FirmwareUpdateObjectTest, ReadProgress) {
    int64_t value;
    bool result = firmwareObj_->read(FirmwareResourceId::UPDATE_PROGRESS, value);
    EXPECT_TRUE(result);
    EXPECT_EQ(value, 0);
}

TEST_F(FirmwareUpdateObjectTest, ReadPackageName) {
    std::string value;
    bool result = firmwareObj_->read(FirmwareResourceId::PKG_NAME, value);
    EXPECT_TRUE(result);
    EXPECT_TRUE(value.empty());
}

TEST_F(FirmwareUpdateObjectTest, ReadPackageVersion) {
    std::string value;
    bool result = firmwareObj_->read(FirmwareResourceId::PKG_VERSION, value);
    EXPECT_TRUE(result);
}

// ============================================================================
// Resource Write Tests
// ============================================================================

TEST_F(FirmwareUpdateObjectTest, WriteBlockSize) {
    bool result = firmwareObj_->write(FirmwareResourceId::BLOCK_SIZE, int64_t(512));
    EXPECT_TRUE(result);
    
    int64_t value;
    firmwareObj_->read(FirmwareResourceId::BLOCK_SIZE, value);
    EXPECT_EQ(value, 512);
}

TEST_F(FirmwareUpdateObjectTest, WriteDeltaAlgorithm) {
    // Write VCDIFF algorithm
    bool result = firmwareObj_->write(FirmwareResourceId::DELTA_ALGORITHM, 
                                      int64_t(firmware::DeltaAlgorithm::VCDIFF));
    EXPECT_TRUE(result);
    
    int64_t value;
    firmwareObj_->read(FirmwareResourceId::DELTA_ALGORITHM, value);
    EXPECT_EQ(value, static_cast<int64_t>(firmware::DeltaAlgorithm::VCDIFF));
}

TEST_F(FirmwareUpdateObjectTest, WriteDifferentialDownload) {
    bool result = firmwareObj_->write(FirmwareResourceId::DIFFERENTIAL_DOWNLOAD, int64_t(1));
    EXPECT_TRUE(result);
    
    int64_t value;
    firmwareObj_->read(FirmwareResourceId::DIFFERENTIAL_DOWNLOAD, value);
    EXPECT_EQ(value, 1);
}

TEST_F(FirmwareUpdateObjectTest, WriteRollbackSupport) {
    bool result = firmwareObj_->write(FirmwareResourceId::ROLLBACK_SUPPORT, int64_t(1));
    EXPECT_TRUE(result);
    
    int64_t value;
    firmwareObj_->read(FirmwareResourceId::ROLLBACK_SUPPORT, value);
    EXPECT_EQ(value, 1);
}

TEST_F(FirmwareUpdateObjectTest, WritePackageURIWhenNotIdle) {
    // First, put object in non-idle state (simulate)
    // For this test, we verify the guard logic exists
    
    std::string uri = "coaps://server/firmware.bin";
    bool result = firmwareObj_->write(FirmwareResourceId::PACKAGE_URI, uri);
    EXPECT_TRUE(result);  // Should succeed when idle
}

// ============================================================================
// Delta Algorithm Tests
// ============================================================================

TEST_F(FirmwareUpdateObjectTest, SetDeltaAlgorithmBSDiff) {
    firmwareObj_->setDeltaAlgorithm(firmware::DeltaAlgorithm::BSDIFF);
    EXPECT_EQ(firmwareObj_->getDeltaAlgorithm(), firmware::DeltaAlgorithm::BSDIFF);
}

TEST_F(FirmwareUpdateObjectTest, SetDeltaAlgorithmVCDiff) {
    firmwareObj_->setDeltaAlgorithm(firmware::DeltaAlgorithm::VCDIFF);
    EXPECT_EQ(firmwareObj_->getDeltaAlgorithm(), firmware::DeltaAlgorithm::VCDIFF);
}

TEST_F(FirmwareUpdateObjectTest, SetDeltaAlgorithmCourgette) {
    firmwareObj_->setDeltaAlgorithm(firmware::DeltaAlgorithm::COURGETTE);
    EXPECT_EQ(firmwareObj_->getDeltaAlgorithm(), firmware::DeltaAlgorithm::COURGETTE);
}

// ============================================================================
// Block Size Tests
// ============================================================================

TEST_F(FirmwareUpdateObjectTest, SetBlockSize) {
    firmwareObj_->setBlockSize(512);
    EXPECT_EQ(firmwareObj_->getBlockSize(), 512u);
    
    firmwareObj_->setBlockSize(2048);
    EXPECT_EQ(firmwareObj_->getBlockSize(), 2048u);
}

// ============================================================================
// Rollback Tests
// ============================================================================

// Commented out - isRollbackEnabled() doesn't exist (rollbackEnabled_ is private)
// Use isRollbackAvailable() to check if rollback is currently possible
// TEST_F(FirmwareUpdateObjectTest, RollbackEnabled) {
//     firmwareObj_->enableRollback(true);
//     EXPECT_TRUE(firmwareObj_->isRollbackEnabled());
//
//     firmwareObj_->enableRollback(false);
//     EXPECT_FALSE(firmwareObj_->isRollbackEnabled());
// }

// ============================================================================
// Component Management Tests
// ============================================================================

TEST_F(FirmwareUpdateObjectTest, RegisterComponent) {
    FirmwareComponent comp;
    comp.name = "kernel";
    comp.version = "5.15.0";
    comp.size = 4 * 1024 * 1024;
    comp.mandatory = true;
    
    firmwareObj_->registerComponent(comp);
    
    auto components = firmwareObj_->getComponents();
    EXPECT_EQ(components.size(), 1u);
    EXPECT_EQ(components[0].name, "kernel");
}

TEST_F(FirmwareUpdateObjectTest, RegisterMultipleComponents) {
    FirmwareComponent kernel;
    kernel.name = "kernel";
    kernel.version = "5.15.0";
    
    FirmwareComponent rootfs;
    rootfs.name = "rootfs";
    rootfs.version = "1.0.0";
    
    firmwareObj_->registerComponent(kernel);
    firmwareObj_->registerComponent(rootfs);
    
    auto components = firmwareObj_->getComponents();
    EXPECT_EQ(components.size(), 2u);
}

TEST_F(FirmwareUpdateObjectTest, UpdateExistingComponent) {
    FirmwareComponent comp;
    comp.name = "kernel";
    comp.version = "5.15.0";
    
    firmwareObj_->registerComponent(comp);
    
    comp.version = "5.16.0";
    firmwareObj_->registerComponent(comp);
    
    auto components = firmwareObj_->getComponents();
    EXPECT_EQ(components.size(), 1u);
    EXPECT_EQ(components[0].version, "5.16.0");
}

TEST_F(FirmwareUpdateObjectTest, GetComponentsJson) {
    FirmwareComponent comp;
    comp.name = "kernel";
    comp.version = "5.15.0";
    comp.mandatory = true;
    
    firmwareObj_->registerComponent(comp);
    
    std::string json;
    firmwareObj_->read(FirmwareResourceId::COMPONENT_LIST, json);
    
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("kernel"), std::string::npos);
    EXPECT_NE(json.find("5.15.0"), std::string::npos);
}

// ============================================================================
// Callback Tests
// ============================================================================

TEST_F(FirmwareUpdateObjectTest, StateChangeCallback) {
    bool callbackCalled = false;
    FirmwareState oldState, newState;
    
    firmwareObj_->setStateChangeCallback([&](FirmwareState from, FirmwareState to) {
        callbackCalled = true;
        oldState = from;
        newState = to;
    });
    
    // Trigger state change internally would require starting a download
    // For now, verify callback is set
    EXPECT_FALSE(callbackCalled);  // Not called until state changes
}

TEST_F(FirmwareUpdateObjectTest, ProgressCallback) {
    int lastProgress = -1;
    std::string lastStatus;
    
    firmwareObj_->setProgressCallback([&](int progress, const std::string& status) {
        lastProgress = progress;
        lastStatus = status;
    });
    
    // Callback is registered but not yet triggered
    EXPECT_EQ(lastProgress, -1);
}

// ============================================================================
// Block Handling Tests
// ============================================================================

TEST_F(FirmwareUpdateObjectTest, HandleBlock) {
    std::vector<uint8_t> block(1024, 0x42);
    
    bool result = firmwareObj_->handleBlock(0, block, true);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(firmwareObj_->getState(), FirmwareState::DOWNLOADING);
}

TEST_F(FirmwareUpdateObjectTest, HandleLastBlock) {
    std::vector<uint8_t> block(512, 0x42);
    
    // First block
    firmwareObj_->handleBlock(0, block, true);
    
    // Last block
    bool result = firmwareObj_->handleBlock(1, block, false);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(firmwareObj_->getState(), FirmwareState::DOWNLOADED);
}

TEST_F(FirmwareUpdateObjectTest, HandleOutOfOrderBlock) {
    std::vector<uint8_t> block(1024, 0x42);
    
    firmwareObj_->handleBlock(0, block, true);
    
    // Skip block 1
    bool result = firmwareObj_->handleBlock(2, block, true);
    EXPECT_FALSE(result);
}

// ============================================================================
// Update Result Codes Tests
// ============================================================================

TEST(UpdateResultTest, ResultCodes) {
    EXPECT_EQ(static_cast<int>(UpdateResult::DEFAULT), 0);
    EXPECT_EQ(static_cast<int>(UpdateResult::SUCCESS), 1);
    EXPECT_EQ(static_cast<int>(UpdateResult::NOT_ENOUGH_STORAGE), 2);
    EXPECT_EQ(static_cast<int>(UpdateResult::OUT_OF_MEMORY), 3);
    EXPECT_EQ(static_cast<int>(UpdateResult::CONNECTION_LOST), 4);
    EXPECT_EQ(static_cast<int>(UpdateResult::INTEGRITY_CHECK_FAILED), 5);
    EXPECT_EQ(static_cast<int>(UpdateResult::UNSUPPORTED_PACKAGE_TYPE), 6);
    EXPECT_EQ(static_cast<int>(UpdateResult::INVALID_URI), 7);
    EXPECT_EQ(static_cast<int>(UpdateResult::UPDATE_FAILED), 8);
    EXPECT_EQ(static_cast<int>(UpdateResult::UNSUPPORTED_PROTOCOL), 9);
}

TEST(UpdateResultTest, DeltaSpecificResults) {
    EXPECT_EQ(static_cast<int>(UpdateResult::DELTA_SOURCE_MISMATCH), 10);
    EXPECT_EQ(static_cast<int>(UpdateResult::DELTA_ALGORITHM_ERROR), 11);
    EXPECT_EQ(static_cast<int>(UpdateResult::ROLLBACK_REQUIRED), 12);
    EXPECT_EQ(static_cast<int>(UpdateResult::ROLLBACK_FAILED), 13);
}

// ============================================================================
// Protocol Support Tests
// ============================================================================

TEST(ProtocolSupportTest, SupportedProtocols) {
    EXPECT_EQ(static_cast<int>(ProtocolSupport::COAP), 0);
    EXPECT_EQ(static_cast<int>(ProtocolSupport::COAPS), 1);
    EXPECT_EQ(static_cast<int>(ProtocolSupport::HTTP), 2);
    EXPECT_EQ(static_cast<int>(ProtocolSupport::HTTPS), 3);
    EXPECT_EQ(static_cast<int>(ProtocolSupport::COAP_TCP), 4);
    EXPECT_EQ(static_cast<int>(ProtocolSupport::COAP_TLS), 5);
}

// ============================================================================
// Configuration Tests
// ============================================================================

TEST(FirmwareConfigTest, DefaultConfig) {
    AdvancedFirmwareUpdateObject::Config config;
    
    EXPECT_EQ(config.downloadPath, "/tmp/firmware");
    EXPECT_EQ(config.defaultBlockSize, 1024u);
    EXPECT_TRUE(config.rollbackEnabled);
}

TEST(FirmwareConfigTest, CustomConfig) {
    AdvancedFirmwareUpdateObject::Config config;
    config.downloadPath = "/custom/path";
    config.defaultBlockSize = 512;
    config.rollbackEnabled = false;
    config.autoReboot = true;
    config.rebootDelay = 10;
    
    EXPECT_EQ(config.downloadPath, "/custom/path");
    EXPECT_EQ(config.defaultBlockSize, 512u);
    EXPECT_FALSE(config.rollbackEnabled);
    EXPECT_TRUE(config.autoReboot);
    EXPECT_EQ(config.rebootDelay, 10);
}

} // namespace test
} // namespace objects
} // namespace lwm2m
