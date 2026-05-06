/**
 * @file mcu_abstraction.h
 * @brief MCU Abstraction Layer for Friendly LwM2M Client
 *
 * Provides MCU-agnostic interfaces for flash operations, bootloader control,
 * non-volatile storage, and hardware-specific functionality.
 *
 * Supported MCU Families:
 * - STM32 (STMicroelectronics, 35% market share)
 * - ESP32 (Espressif, 20% market share)
 * - nRF52/nRF91 (Nordic Semiconductor, 10% market share)
 * - RP2040 (Raspberry Pi, 5% market share)
 * - NXP, TI, Microchip, Renesas (additional 20%)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifndef FRIENDLY_LWM2M_MCU_ABSTRACTION_H
#define FRIENDLY_LWM2M_MCU_ABSTRACTION_H

#include "platform_abstraction.h"  // For PlatformResult
#include <cstdint>
#include <vector>
#include <functional>
#include <optional>

namespace lwm2m {
namespace platform {
namespace mcu {

/**
 * @brief Flash memory type
 */
enum class FlashType {
    INTERNAL_NOR,      ///< Internal NOR flash (STM32, nRF52)
    INTERNAL_NAND,     ///< Internal NAND flash (rare)
    EXTERNAL_SPI_NOR,  ///< External SPI NOR flash (ESP32, RP2040)
    EXTERNAL_SPI_NAND, ///< External SPI NAND flash
    EXTERNAL_QSPI,     ///< External QSPI flash (higher speed)
    EMMC,              ///< eMMC storage
    SD_CARD            ///< SD card storage
};

/**
 * @brief Flash characteristics for different MCU families
 */
struct FlashCharacteristics {
    FlashType type;                 ///< Flash memory type
    uint32_t pageSize;              ///< Page size in bytes (smallest programmable unit)
    uint32_t sectorSize;            ///< Sector/block size (smallest erasable unit)
    uint32_t totalSize;             ///< Total flash size in bytes
    uint32_t writeAlignment;        ///< Required write alignment (1, 4, 8 bytes)
    uint32_t eraseValue;            ///< Value after erase (0xFF for NOR, 0x00 for some)
    bool requiresErase;             ///< Must erase before write
    bool supportsBufferedWrite;     ///< Supports buffered/deferred writes
    bool supportsPartialPageWrite;  ///< Can write less than full page
    uint32_t maxWriteSize;          ///< Maximum single write operation size
    uint32_t writeTimeMs;           ///< Typical page write time in ms
    uint32_t eraseTimeMs;           ///< Typical sector erase time in ms
};

/**
 * @brief Flash region (partition) information
 */
struct FlashRegion {
    uint32_t startAddress;          ///< Start address in flash
    uint32_t size;                  ///< Region size in bytes
    const char* name;               ///< Region name (e.g., "bootloader", "app", "data")
    bool isProtected;               ///< Write/erase protection status
    bool isBootable;                ///< Contains bootable code
};

/**
 * @brief Flash operation progress callback
 */
using FlashProgressCallback = std::function<void(uint32_t current, uint32_t total)>;

/**
 * @brief Low-level flash interface for MCUs
 *
 * Provides unified interface across:
 * - STM32 HAL (HAL_FLASH_Program, HAL_FLASH_Erase)
 * - ESP-IDF (esp_partition_*, spi_flash_*)
 * - Nordic SDK (nrf_nvmc_*, flash_write)
 * - Pico SDK (flash_range_erase, flash_range_program)
 */
class IMCUFlash {
public:
    virtual ~IMCUFlash() = default;

    /**
     * @brief Initialize flash controller
     * @return Platform result code
     */
    virtual PlatformResult initialize() = 0;

    /**
     * @brief Deinitialize flash controller
     * @return Platform result code
     */
    virtual PlatformResult deinitialize() = 0;

    /**
     * @brief Get flash characteristics
     * @return Flash characteristics structure
     */
    virtual FlashCharacteristics getCharacteristics() = 0;

    /**
     * @brief Get list of flash regions/partitions
     * @return Vector of flash regions
     */
    virtual std::vector<FlashRegion> getRegions() = 0;

    /**
     * @brief Erase a single page
     * @param address Page-aligned address
     * @return Platform result code
     */
    virtual PlatformResult erasePage(uint32_t address) = 0;

    /**
     * @brief Erase a single sector/block
     * @param address Sector-aligned address
     * @return Platform result code
     */
    virtual PlatformResult eraseSector(uint32_t address) = 0;

    /**
     * @brief Erase a range of flash memory
     * @param address Starting address (must be sector-aligned)
     * @param size Size in bytes (must be sector-aligned)
     * @param progressCallback Optional progress callback
     * @return Platform result code
     */
    virtual PlatformResult eraseRange(
        uint32_t address,
        uint32_t size,
        FlashProgressCallback progressCallback = nullptr
    ) = 0;

    /**
     * @brief Erase entire flash (chip erase)
     * @return Platform result code
     */
    virtual PlatformResult eraseChip() = 0;

    /**
     * @brief Write data to flash (page granularity)
     * @param address Address to write (must be page-aligned)
     * @param data Data buffer to write
     * @param size Data size (must be page-aligned)
     * @param progressCallback Optional progress callback
     * @return Platform result code
     */
    virtual PlatformResult write(
        uint32_t address,
        const uint8_t* data,
        uint32_t size,
        FlashProgressCallback progressCallback = nullptr
    ) = 0;

    /**
     * @brief Write a single word to flash
     * @param address Word-aligned address
     * @param data 32-bit data word
     * @return Platform result code
     */
    virtual PlatformResult writeWord(uint32_t address, uint32_t data) = 0;

    /**
     * @brief Write a half-word to flash (16-bit)
     * @param address Half-word-aligned address
     * @param data 16-bit data
     * @return Platform result code
     */
    virtual PlatformResult writeHalfWord(uint32_t address, uint16_t data) = 0;

    /**
     * @brief Write a byte to flash
     * @param address Byte address
     * @param data 8-bit data
     * @return Platform result code
     */
    virtual PlatformResult writeByte(uint32_t address, uint8_t data) = 0;

    /**
     * @brief Read data from flash
     * @param address Address to read from
     * @param buffer Output buffer
     * @param size Number of bytes to read
     * @return Platform result code
     */
    virtual PlatformResult read(
        uint32_t address,
        uint8_t* buffer,
        uint32_t size
    ) = 0;

    /**
     * @brief Verify flash contents against buffer
     * @param address Starting address
     * @param data Expected data
     * @param size Size in bytes
     * @return true if contents match, false otherwise
     */
    virtual bool verify(
        uint32_t address,
        const uint8_t* data,
        uint32_t size
    ) = 0;

    /**
     * @brief Calculate CRC32 of flash region
     * @param address Starting address
     * @param size Size in bytes
     * @return CRC32 checksum
     */
    virtual uint32_t calculateCRC32(uint32_t address, uint32_t size) = 0;

    /**
     * @brief Enable write protection for region
     * @param address Region start address
     * @param size Region size
     * @return Platform result code
     */
    virtual PlatformResult protect(uint32_t address, uint32_t size) = 0;

    /**
     * @brief Disable write protection for region
     * @param address Region start address
     * @param size Region size
     * @return Platform result code
     */
    virtual PlatformResult unprotect(uint32_t address, uint32_t size) = 0;

    /**
     * @brief Check if region is write-protected
     * @param address Region start address
     * @param size Region size
     * @return true if protected, false otherwise
     */
    virtual bool isProtected(uint32_t address, uint32_t size) = 0;

    /**
     * @brief Lock flash controller (prevent writes)
     * @return Platform result code
     */
    virtual PlatformResult lock() = 0;

    /**
     * @brief Unlock flash controller (allow writes)
     * @return Platform result code
     */
    virtual PlatformResult unlock() = 0;
};

/**
 * @brief Bootloader mode enumeration
 */
enum class BootMode {
    NORMAL,             ///< Normal application boot
    DFU,                ///< Device Firmware Update mode (USB/UART)
    RECOVERY,           ///< Recovery/safe mode
    FACTORY_RESET,      ///< Factory reset mode
    BOOTLOADER,         ///< Stay in bootloader
    APPLICATION_A,      ///< Boot from application slot A
    APPLICATION_B       ///< Boot from application slot B
};

/**
 * @brief Boot reason enumeration
 */
enum class BootReason {
    POWER_ON,           ///< Cold boot (power-on)
    HARDWARE_RESET,     ///< Hardware reset button
    SOFTWARE_RESET,     ///< Software-initiated reset
    WATCHDOG_RESET,     ///< Watchdog timeout reset
    BROWNOUT,           ///< Brownout detection reset
    CRASH,              ///< Crash/fault reset
    FIRMWARE_UPDATE,    ///< Reset after firmware update
    UNKNOWN             ///< Unknown/undetected reason
};

/**
 * @brief Bootloader interface for MCU firmware updates
 *
 * Supports:
 * - STM32: Custom bootloaders, ST DFU
 * - ESP32: ESP-IDF OTA (app_0, app_1 partitions)
 * - nRF52: MCUboot (Zephyr bootloader with image verification)
 * - RP2040: UF2 bootloader (USB mass storage)
 */
class IMCUBootloader {
public:
    virtual ~IMCUBootloader() = default;

    /**
     * @brief Initialize bootloader interface
     * @return Platform result code
     */
    virtual PlatformResult initialize() = 0;

    /**
     * @brief Get current boot mode
     * @return Boot mode
     */
    virtual BootMode getBootMode() = 0;

    /**
     * @brief Set boot mode for next reboot
     * @param mode Desired boot mode
     * @return Platform result code
     */
    virtual PlatformResult setBootMode(BootMode mode) = 0;

    /**
     * @brief Get boot reason (why device rebooted)
     * @return Boot reason
     */
    virtual BootReason getBootReason() = 0;

    /**
     * @brief Mark current image as valid/tested
     *
     * Used for A/B updates to confirm new firmware works.
     * If not called within timeout, bootloader reverts to old image.
     *
     * @return Platform result code
     */
    virtual PlatformResult markImageValid() = 0;

    /**
     * @brief Mark current image as invalid (trigger rollback)
     * @return Platform result code
     */
    virtual PlatformResult markImageInvalid() = 0;

    /**
     * @brief Get active application slot
     * @return Slot A or Slot B
     */
    virtual PartitionSlot getActiveSlot() = 0;

    /**
     * @brief Get next boot slot
     * @return Slot that will boot next
     */
    virtual PartitionSlot getNextBootSlot() = 0;

    /**
     * @brief Set next boot slot for A/B updates
     * @param slot Slot to boot next (A or B)
     * @return Platform result code
     */
    virtual PlatformResult setNextBootSlot(PartitionSlot slot) = 0;

    /**
     * @brief Check if slot contains valid bootable image
     * @param slot Slot to check
     * @return true if valid, false otherwise
     */
    virtual bool isSlotValid(PartitionSlot slot) = 0;

    /**
     * @brief Get firmware version from slot
     * @param slot Slot to query
     * @return Version string, empty if invalid
     */
    virtual std::string getSlotVersion(PartitionSlot slot) = 0;

    /**
     * @brief Reboot device
     * @return Platform result code (typically doesn't return)
     */
    virtual PlatformResult reboot() = 0;

    /**
     * @brief Reboot to bootloader mode
     * @return Platform result code (typically doesn't return)
     */
    virtual PlatformResult rebootToBootloader() = 0;

    /**
     * @brief Reboot to DFU mode
     * @return Platform result code (typically doesn't return)
     */
    virtual PlatformResult rebootToDFU() = 0;

    /**
     * @brief Get bootloader version
     * @return Version string
     */
    virtual std::string getBootloaderVersion() = 0;
};

/**
 * @brief Non-Volatile Memory (NVM) interface for configuration storage
 *
 * Provides persistent key-value storage for:
 * - LwM2M endpoint name, server URI, credentials
 * - Application settings
 * - Calibration data
 *
 * Implementations:
 * - STM32: EEPROM emulation in flash
 * - ESP32: NVS (Non-Volatile Storage) API
 * - nRF52: fds (Flash Data Storage)
 * - RP2040: Flash-based key-value store
 */
class INVM {
public:
    virtual ~INVM() = default;

    /**
     * @brief Initialize NVM subsystem
     * @return Platform result code
     */
    virtual PlatformResult init() = 0;

    /**
     * @brief Deinitialize NVM subsystem
     * @return Platform result code
     */
    virtual PlatformResult deinit() = 0;

    /**
     * @brief Set a key-value pair (string value)
     * @param key Key name (null-terminated)
     * @param value String value
     * @return Platform result code
     */
    virtual PlatformResult setString(const char* key, const char* value) = 0;

    /**
     * @brief Get string value
     * @param key Key name
     * @param value Output buffer
     * @param maxSize Buffer size
     * @return Platform result code
     */
    virtual PlatformResult getString(const char* key, char* value, size_t maxSize) = 0;

    /**
     * @brief Set binary value
     * @param key Key name
     * @param value Binary data
     * @param size Data size in bytes
     * @return Platform result code
     */
    virtual PlatformResult setBlob(const char* key, const void* value, size_t size) = 0;

    /**
     * @brief Get binary value
     * @param key Key name
     * @param value Output buffer
     * @param size Input: buffer size, Output: actual data size
     * @return Platform result code
     */
    virtual PlatformResult getBlob(const char* key, void* value, size_t* size) = 0;

    /**
     * @brief Set integer value
     * @param key Key name
     * @param value Integer value
     * @return Platform result code
     */
    virtual PlatformResult setInt(const char* key, int32_t value) = 0;

    /**
     * @brief Get integer value
     * @param key Key name
     * @param value Output value
     * @return Platform result code
     */
    virtual PlatformResult getInt(const char* key, int32_t* value) = 0;

    /**
     * @brief Set unsigned integer value
     * @param key Key name
     * @param value Unsigned integer value
     * @return Platform result code
     */
    virtual PlatformResult setUInt(const char* key, uint32_t value) = 0;

    /**
     * @brief Get unsigned integer value
     * @param key Key name
     * @param value Output value
     * @return Platform result code
     */
    virtual PlatformResult getUInt(const char* key, uint32_t* value) = 0;

    /**
     * @brief Erase a key-value pair
     * @param key Key name
     * @return Platform result code
     */
    virtual PlatformResult erase(const char* key) = 0;

    /**
     * @brief Erase all key-value pairs in namespace
     * @param namespaceId Namespace identifier (nullptr = default)
     * @return Platform result code
     */
    virtual PlatformResult eraseAll(const char* namespaceId = nullptr) = 0;

    /**
     * @brief Commit pending writes to flash
     *
     * Some NVM implementations buffer writes. Call commit() to flush.
     *
     * @return Platform result code
     */
    virtual PlatformResult commit() = 0;

    /**
     * @brief Check if key exists
     * @param key Key name
     * @return true if exists, false otherwise
     */
    virtual bool hasKey(const char* key) = 0;

    /**
     * @brief Get number of stored keys
     * @return Key count
     */
    virtual size_t getKeyCount() = 0;

    /**
     * @brief Get total NVM size
     * @return Size in bytes
     */
    virtual size_t getTotalSize() = 0;

    /**
     * @brief Get used NVM size
     * @return Used size in bytes
     */
    virtual size_t getUsedSize() = 0;

    /**
     * @brief Get free NVM size
     * @return Free size in bytes
     */
    virtual size_t getFreeSize() = 0;
};

/**
 * @brief Watchdog timer interface
 */
class IWatchdog {
public:
    virtual ~IWatchdog() = default;

    /**
     * @brief Initialize watchdog
     * @param timeoutMs Timeout in milliseconds
     * @return Platform result code
     */
    virtual PlatformResult init(uint32_t timeoutMs) = 0;

    /**
     * @brief Start watchdog timer
     * @return Platform result code
     */
    virtual PlatformResult start() = 0;

    /**
     * @brief Stop watchdog timer (if supported)
     * @return Platform result code
     */
    virtual PlatformResult stop() = 0;

    /**
     * @brief Feed (kick) the watchdog
     * @return Platform result code
     */
    virtual PlatformResult feed() = 0;

    /**
     * @brief Get remaining time until timeout
     * @return Milliseconds remaining
     */
    virtual uint32_t getTimeRemaining() = 0;

    /**
     * @brief Check if watchdog is running
     * @return true if running
     */
    virtual bool isRunning() = 0;
};

/**
 * @brief MCU-specific system information
 */
struct MCUInfo {
    const char* family;         ///< MCU family (e.g., "STM32F4", "ESP32")
    const char* model;          ///< Specific model (e.g., "STM32F407VG")
    const char* core;           ///< CPU core (e.g., "Cortex-M4F", "Xtensa LX6")
    uint32_t cpuFrequency;      ///< CPU frequency in Hz
    uint32_t ramSize;           ///< Total RAM in bytes
    uint32_t flashSize;         ///< Total flash in bytes
    uint8_t uniqueId[16];       ///< Unique device ID (96-128 bits)
    size_t uniqueIdLength;      ///< Length of unique ID in bytes
};

/**
 * @brief MCU system interface
 */
class IMCUSystem {
public:
    virtual ~IMCUSystem() = default;

    /**
     * @brief Initialize system
     * @return Platform result code
     */
    virtual PlatformResult initialize() = 0;

    /**
     * @brief Get MCU information
     * @return MCU info structure
     */
    virtual MCUInfo getInfo() = 0;

    /**
     * @brief Get CPU frequency
     * @return Frequency in Hz
     */
    virtual uint32_t getCPUFrequency() = 0;

    /**
     * @brief Set CPU frequency (if supported)
     * @param frequencyHz Desired frequency in Hz
     * @return Platform result code
     */
    virtual PlatformResult setCPUFrequency(uint32_t frequencyHz) = 0;

    /**
     * @brief Get CPU temperature (if supported)
     * @return Temperature in Celsius, or NaN if not supported
     */
    virtual float getCPUTemperature() = 0;

    /**
     * @brief Get supply voltage (Vdd)
     * @return Voltage in millivolts
     */
    virtual uint32_t getSupplyVoltage() = 0;

    /**
     * @brief Enter sleep mode
     * @param durationMs Sleep duration (0 = until interrupt)
     * @return Platform result code
     */
    virtual PlatformResult sleep(uint32_t durationMs = 0) = 0;

    /**
     * @brief Enter deep sleep mode
     * @param durationMs Sleep duration (0 = until interrupt)
     * @return Platform result code
     */
    virtual PlatformResult deepSleep(uint32_t durationMs = 0) = 0;

    /**
     * @brief Get uptime since boot
     * @return Uptime in milliseconds
     */
    virtual uint64_t getUptime() = 0;

    /**
     * @brief Get random number from hardware RNG
     * @return Random number
     */
    virtual uint32_t getRandom() = 0;
};

/**
 * @brief MCU platform factory
 *
 * Creates MCU-specific implementations of abstract interfaces
 */
class MCUFactory {
public:
    virtual ~MCUFactory() = default;

    /**
     * @brief Get flash interface
     * @return Flash interface
     */
    virtual IMCUFlash& getFlash() = 0;

    /**
     * @brief Get bootloader interface
     * @return Bootloader interface
     */
    virtual IMCUBootloader& getBootloader() = 0;

    /**
     * @brief Get NVM interface
     * @return NVM interface
     */
    virtual INVM& getNVM() = 0;

    /**
     * @brief Get watchdog interface
     * @return Watchdog interface
     */
    virtual IWatchdog& getWatchdog() = 0;

    /**
     * @brief Get system interface
     * @return System interface
     */
    virtual IMCUSystem& getSystem() = 0;

    /**
     * @brief Get MCU family name
     * @return MCU family string ("STM32", "ESP32", "nRF52", "RP2040")
     */
    virtual const char* getMCUFamily() = 0;
};

/**
 * @brief Get global MCU factory instance
 *
 * Implementation provided by MCU-specific driver
 * (stm32_platform.cpp, esp32_platform.cpp, etc.)
 */
MCUFactory& getMCUFactory();

} // namespace mcu
} // namespace platform
} // namespace lwm2m

#endif // FRIENDLY_LWM2M_MCU_ABSTRACTION_H
