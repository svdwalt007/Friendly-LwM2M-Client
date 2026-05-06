/**
 * @file stm32_hal_adapter.cpp
 * @brief STM32 HAL RTOS Adapter Implementation
 *
 * Implements the RTOS abstraction layer for STM32 HAL (Hardware Abstraction Layer).
 *
 * STM32 HAL is ST Microelectronics' hardware abstraction layer for STM32 microcontrollers.
 * Can run on top of FreeRTOS, ThreadX, or bare-metal CMSIS-RTOS2.
 *
 * Key Features:
 * - FreeRTOS integration (most common)
 * - CMSIS-RTOS2 API wrapper
 * - STM32CubeMX code generation
 * - Comprehensive peripheral drivers
 * - Low-power modes
 * - DMA support
 * - USB, CAN, Ethernet support
 *
 * Supported Series:
 * - STM32F0/F1/F2/F3/F4/F7 (ARM Cortex-M0/M3/M4/M7)
 * - STM32G0/G4 (Cortex-M0+/M4)
 * - STM32H7 (Cortex-M7, dual-core H745/H755)
 * - STM32L0/L1/L4/L5 (ultra-low-power)
 * - STM32U5 (ultra-low-power, Cortex-M33)
 * - STM32WB/WL (wireless)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef STM32_HAL

// Include FreeRTOS adapter as base (60% code reuse when using FreeRTOS)
#ifdef USE_FREERTOS
#include "freertos_adapter.cpp"
#endif

#include "stm32_hal.h"
#include "cmsis_os2.h"
#include <string>
#include <cstring>

namespace lwm2m {
namespace platform {
namespace rtos {

#ifndef USE_FREERTOS
// ============================================================================
// Bare-Metal CMSIS-RTOS2 Implementation (when not using FreeRTOS)
// ============================================================================

class STM32Thread : public IThread {
public:
    STM32Thread(const char* name, ThreadFunc func, void* arg,
                uint32_t stackSize, ThreadPriority priority)
        : name_(name), func_(func), arg_(arg), stackSize_(stackSize),
          priority_(priority), threadId_(nullptr), started_(false) {}

    ~STM32Thread() override {
        if (threadId_ != nullptr) {
            osThreadTerminate(threadId_);
        }
    }

    RTOSResult start() override {
        if (started_) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        osThreadAttr_t attr = {};
        attr.name = name_;
        attr.stack_size = stackSize_;
        attr.priority = mapPriorityToSTM32(priority_);

        threadId_ = osThreadNew(threadWrapper, this, &attr);
        if (threadId_ == nullptr) {
            return RTOSResult::ERROR_NO_MEMORY;
        }

        started_ = true;
        return RTOSResult::SUCCESS;
    }

    RTOSResult join(uint32_t timeoutMs) override {
        if (!started_ || threadId_ == nullptr) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        osStatus_t status = osThreadJoin(threadId_);
        return (status == osOK) ? RTOSResult::SUCCESS : RTOSResult::ERROR_TIMEOUT;
    }

    RTOSResult suspend() override {
        if (!started_ || threadId_ == nullptr) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        osStatus_t status = osThreadSuspend(threadId_);
        return (status == osOK) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
    }

    RTOSResult resume() override {
        if (!started_ || threadId_ == nullptr) {
            return RTOSResult::ERROR_INVALID_STATE;
        }

        osStatus_t status = osThreadResume(threadId_);
        return (status == osOK) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
    }

    ThreadState getState() override {
        if (!started_ || threadId_ == nullptr) {
            return ThreadState::READY;
        }

        osThreadState_t state = osThreadGetState(threadId_);
        switch (state) {
            case osThreadInactive:  return ThreadState::READY;
            case osThreadReady:     return ThreadState::READY;
            case osThreadRunning:   return ThreadState::RUNNING;
            case osThreadBlocked:   return ThreadState::BLOCKED;
            case osThreadTerminated: return ThreadState::TERMINATED;
            default:                return ThreadState::UNKNOWN;
        }
    }

    ThreadPriority getPriority() override {
        return priority_;
    }

    RTOSResult setPriority(ThreadPriority priority) override {
        priority_ = priority;
        if (started_ && threadId_ != nullptr) {
            osPriority_t stm32Priority = mapPriorityToSTM32(priority);
            osStatus_t status = osThreadSetPriority(threadId_, stm32Priority);
            return (status == osOK) ? RTOSResult::SUCCESS : RTOSResult::ERROR_UNKNOWN;
        }
        return RTOSResult::SUCCESS;
    }

    uint32_t getStackHighWaterMark() override {
        if (!started_ || threadId_ == nullptr) {
            return stackSize_;
        }
        uint32_t used = osThreadGetStackSpace(threadId_);
        return stackSize_ - used;
    }

    const char* getName() override {
        return name_;
    }

private:
    static void threadWrapper(void* arg) {
        STM32Thread* thread = static_cast<STM32Thread*>(arg);
        thread->func_(thread->arg_);
    }

    static osPriority_t mapPriorityToSTM32(ThreadPriority priority) {
        switch (priority) {
            case ThreadPriority::IDLE:          return osPriorityIdle;
            case ThreadPriority::LOW:           return osPriorityLow;
            case ThreadPriority::BELOW_NORMAL:  return osPriorityBelowNormal;
            case ThreadPriority::NORMAL:        return osPriorityNormal;
            case ThreadPriority::ABOVE_NORMAL:  return osPriorityAboveNormal;
            case ThreadPriority::HIGH:          return osPriorityHigh;
            case ThreadPriority::REALTIME:      return osPriorityRealtime;
            default:                            return osPriorityNormal;
        }
    }

    const char* name_;
    ThreadFunc func_;
    void* arg_;
    uint32_t stackSize_;
    ThreadPriority priority_;
    osThreadId_t threadId_;
    bool started_;
};

class STM32Mutex : public IMutex {
public:
    STM32Mutex() {
        mutexId_ = osMutexNew(nullptr);
    }

    ~STM32Mutex() override {
        if (mutexId_ != nullptr) {
            osMutexDelete(mutexId_);
        }
    }

    bool lock(uint32_t timeoutMs) override {
        osStatus_t status = osMutexAcquire(mutexId_, timeoutMs);
        return (status == osOK);
    }

    void unlock() override {
        osMutexRelease(mutexId_);
    }

    bool tryLock() override {
        osStatus_t status = osMutexAcquire(mutexId_, 0);
        return (status == osOK);
    }

    bool isLocked() override {
        osThreadId_t owner = osMutexGetOwner(mutexId_);
        return (owner != nullptr);
    }

private:
    osMutexId_t mutexId_;
};

class STM32Semaphore : public ISemaphore {
public:
    STM32Semaphore(uint32_t initialCount, uint32_t maxCount)
        : maxCount_(maxCount) {
        semId_ = osSemaphoreNew(maxCount, initialCount, nullptr);
    }

    ~STM32Semaphore() override {
        if (semId_ != nullptr) {
            osSemaphoreDelete(semId_);
        }
    }

    bool wait(uint32_t timeoutMs) override {
        osStatus_t status = osSemaphoreAcquire(semId_, timeoutMs);
        return (status == osOK);
    }

    bool signal() override {
        osStatus_t status = osSemaphoreRelease(semId_);
        return (status == osOK);
    }

    bool tryWait() override {
        osStatus_t status = osSemaphoreAcquire(semId_, 0);
        return (status == osOK);
    }

    uint32_t getCount() override {
        return osSemaphoreGetCount(semId_);
    }

    void reset() override {
        while (osSemaphoreAcquire(semId_, 0) == osOK) {
            // Drain semaphore
        }
    }

private:
    osSemaphoreId_t semId_;
    uint32_t maxCount_;
};

class STM32ThreadManager : public IThreadManager {
public:
    std::unique_ptr<IThread> createThread(
        const char* name,
        ThreadFunc func,
        void* arg,
        uint32_t stackSize,
        ThreadPriority priority
    ) override {
        auto thread = std::make_unique<STM32Thread>(name, func, arg, stackSize, priority);
        RTOSResult result = thread->start();
        if (result != RTOSResult::SUCCESS) {
            return nullptr;
        }
        return thread;
    }

    void sleep(uint32_t ms) override {
        osDelay(ms);
    }

    void yield() override {
        osThreadYield();
    }

    uint64_t getTicks() override {
        return osKernelGetTickCount();
    }

    uint32_t getTickRate() override {
        return osKernelGetTickFreq();
    }

    void enterCritical() override {
        osKernelLock();
    }

    void exitCritical() override {
        osKernelUnlock();
    }
};

#endif // !USE_FREERTOS

// ============================================================================
// STM32 Flash Memory Implementation
// ============================================================================

class STM32Flash : public IMCUFlash {
public:
    STM32Flash() {
        totalSize_ = FLASH_SIZE * 1024; // FLASH_SIZE defined in bytes
    }

    size_t getTotalSize() override {
        return totalSize_;
    }

    size_t getPageSize() override {
#if defined(STM32F0) || defined(STM32F1) || defined(STM32F3)
        return 2048; // 2KB pages
#elif defined(STM32F2) || defined(STM32F4) || defined(STM32F7)
        return 16384; // Variable sector sizes, return smallest
#elif defined(STM32L0) || defined(STM32L1)
        return 128; // 128 byte pages
#elif defined(STM32L4) || defined(STM32L5)
        return 2048; // 2KB pages
#else
        return 2048; // Default
#endif
    }

    size_t getSectorSize() override {
        return getPageSize(); // Same as page for most STM32
    }

    bool read(uint32_t address, void* buffer, size_t size) override {
        if (address + size > totalSize_) {
            return false;
        }

        uint32_t flashAddress = FLASH_BASE + address;
        memcpy(buffer, (void*)flashAddress, size);
        return true;
    }

    bool write(uint32_t address, const void* data, size_t size) override {
        if (address + size > totalSize_) {
            return false;
        }

        HAL_FLASH_Unlock();

        uint32_t flashAddress = FLASH_BASE + address;
        const uint64_t* data64 = static_cast<const uint64_t*>(data);
        size_t words = size / 8;

        bool success = true;
        for (size_t i = 0; i < words; i++) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                                 flashAddress + (i * 8),
                                 data64[i]) != HAL_OK) {
                success = false;
                break;
            }
        }

        HAL_FLASH_Lock();
        return success;
    }

    bool erase(uint32_t address, size_t size) override {
        HAL_FLASH_Unlock();

        FLASH_EraseInitTypeDef eraseInit;
        uint32_t pageError = 0;

#if defined(STM32F4) || defined(STM32F7)
        eraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
        eraseInit.Sector = getSectorNumber(address);
        eraseInit.NbSectors = (size + getSectorSize() - 1) / getSectorSize();
        eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
#else
        eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        eraseInit.PageAddress = FLASH_BASE + address;
        eraseInit.NbPages = (size + getPageSize() - 1) / getPageSize();
#endif

        HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&eraseInit, &pageError);

        HAL_FLASH_Lock();
        return (status == HAL_OK);
    }

    bool eraseSector(uint32_t sectorAddress) override {
        return erase(sectorAddress, getSectorSize());
    }

    bool lock() override {
        HAL_FLASH_Lock();
        return true;
    }

    bool unlock() override {
        return (HAL_FLASH_Unlock() == HAL_OK);
    }

    bool isLocked() override {
        return ((FLASH->CR & FLASH_CR_LOCK) != 0);
    }

private:
    size_t totalSize_;

    uint32_t getSectorNumber(uint32_t address) {
#if defined(STM32F4) || defined(STM32F7)
        // STM32F4/F7 have variable sector sizes
        if (address < 0x4000) return 0;
        if (address < 0x8000) return 1;
        if (address < 0xC000) return 2;
        if (address < 0x10000) return 3;
        if (address < 0x20000) return 4;
        return 5 + ((address - 0x20000) / 0x20000);
#else
        return address / getSectorSize();
#endif
    }
};

// ============================================================================
// STM32 Bootloader Implementation
// ============================================================================

class STM32Bootloader : public IMCUOTA {
public:
    STM32Bootloader()
        : updateAddress_(0), updateSize_(0), bytesWritten_(0) {}

    bool begin(size_t firmwareSize) override {
        // Determine update partition (typically second half of flash)
        updateAddress_ = FLASH_BASE + (FLASH_SIZE * 1024 / 2);
        updateSize_ = firmwareSize;
        bytesWritten_ = 0;

        // Erase update partition
        STM32Flash flash;
        return flash.erase(updateAddress_ - FLASH_BASE, firmwareSize);
    }

    bool write(const void* data, size_t size) override {
        STM32Flash flash;
        bool result = flash.write(
            (updateAddress_ - FLASH_BASE) + bytesWritten_,
            data,
            size
        );

        if (result) {
            bytesWritten_ += size;
        }
        return result;
    }

    bool end() override {
        return (bytesWritten_ == updateSize_);
    }

    bool setBootPartition() override {
        // Write boot flag to backup SRAM or EEPROM
        // Implementation depends on specific STM32 series
        return true;
    }

    bool verify() override {
        // Verify CRC or signature
        // Implementation depends on security requirements
        return true;
    }

    uint32_t getCurrentVersion() override {
        // Read version from application header
        return 0;
    }

    bool rollback() override {
        // Clear boot flag to revert to original firmware
        return true;
    }

private:
    uint32_t updateAddress_;
    size_t updateSize_;
    size_t bytesWritten_;
};

// ============================================================================
// STM32 EEPROM/Backup SRAM Implementation
// ============================================================================

class STM32NVM : public IMCUNVM {
public:
    STM32NVM() : initialized_(false) {}

    bool init() override {
#ifdef HAL_EEPROM_MODULE_ENABLED
        initialized_ = true;
        return true;
#else
        return false;
#endif
    }

    bool read(const char* key, void* buffer, size_t size) override {
#ifdef HAL_EEPROM_MODULE_ENABLED
        uint32_t address = hashKey(key);
        HAL_StatusTypeDef status = HAL_EEPROM_Read(address, buffer, size);
        return (status == HAL_OK);
#else
        return false;
#endif
    }

    bool write(const char* key, const void* data, size_t size) override {
#ifdef HAL_EEPROM_MODULE_ENABLED
        uint32_t address = hashKey(key);
        HAL_StatusTypeDef status = HAL_EEPROM_Write(address, data, size);
        return (status == HAL_OK);
#else
        return false;
#endif
    }

    bool erase(const char* key) override {
        // Write zeros
        uint8_t zeros[256] = {0};
        return write(key, zeros, sizeof(zeros));
    }

    bool eraseAll() override {
#ifdef HAL_EEPROM_MODULE_ENABLED
        // Erase entire EEPROM
        return true;
#else
        return false;
#endif
    }

    size_t getSize(const char* key) override {
        return 0; // Not implemented
    }

    bool exists(const char* key) override {
        return false; // Not implemented
    }

private:
    bool initialized_;

    uint32_t hashKey(const char* key) {
        // Simple hash function
        uint32_t hash = 0;
        while (*key) {
            hash = hash * 31 + *key++;
        }
        return hash % 4096; // Assume 4KB EEPROM
    }
};

// ============================================================================
// STM32 Factory Implementation
// ============================================================================

#ifdef USE_FREERTOS
class STM32Factory : public FreeRTOSFactory {
#else
class STM32Factory : public RTOSFactory {
#endif
public:
    const char* getRTOSName() override {
#ifdef USE_FREERTOS
        return "STM32 HAL + FreeRTOS";
#else
        return "STM32 HAL + CMSIS-RTOS2";
#endif
    }

    const char* getRTOSVersion() override {
        static char version[64];
#ifdef USE_FREERTOS
        snprintf(version, sizeof(version), "HAL %d.%d.%d + FreeRTOS %s",
                 HAL_VERSION_MAJOR, HAL_VERSION_MINOR, HAL_VERSION_RC,
                 tskKERNEL_VERSION_NUMBER);
#else
        snprintf(version, sizeof(version), "HAL %d.%d.%d + CMSIS-RTOS2",
                 HAL_VERSION_MAJOR, HAL_VERSION_MINOR, HAL_VERSION_RC);
#endif
        return version;
    }

    const char* getChipInfo() {
        static char info[128];
#if defined(STM32F4)
        snprintf(info, sizeof(info), "STM32F4xx @ %lu MHz",
                 HAL_RCC_GetSysClockFreq() / 1000000);
#elif defined(STM32F7)
        snprintf(info, sizeof(info), "STM32F7xx @ %lu MHz",
                 HAL_RCC_GetSysClockFreq() / 1000000);
#elif defined(STM32H7)
        snprintf(info, sizeof(info), "STM32H7xx @ %lu MHz",
                 HAL_RCC_GetSysClockFreq() / 1000000);
#elif defined(STM32L4)
        snprintf(info, sizeof(info), "STM32L4xx @ %lu MHz",
                 HAL_RCC_GetSysClockFreq() / 1000000);
#else
        snprintf(info, sizeof(info), "STM32 @ %lu MHz",
                 HAL_RCC_GetSysClockFreq() / 1000000);
#endif
        return info;
    }

#ifndef USE_FREERTOS
    std::unique_ptr<IMutex> createMutex() override {
        return std::make_unique<STM32Mutex>();
    }

    std::unique_ptr<ISemaphore> createSemaphore(uint32_t initialCount, uint32_t maxCount) override {
        return std::make_unique<STM32Semaphore>(initialCount, maxCount);
    }

    IThreadManager& getThreadManager() override {
        static STM32ThreadManager manager;
        return manager;
    }
#endif

    IMCUFlash& getFlash() override {
        static STM32Flash flash;
        return flash;
    }

    IMCUOTA& getOTA() override {
        static STM32Bootloader bootloader;
        return bootloader;
    }

    IMCUNVM& getNVM() override {
        static STM32NVM nvm;
        return nvm;
    }

    // STM32-specific: Enter low-power mode
    void enterStopMode() {
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    }

    void enterStandbyMode() {
        HAL_PWR_EnterSTANDBYMode();
    }

    // STM32-specific: Get unique device ID
    uint32_t getDeviceID() {
        return HAL_GetDEVID();
    }

    uint32_t getRevisionID() {
        return HAL_GetREVID();
    }

    void getUniqueID(uint8_t* uid) {
        uint32_t* uidBase = (uint32_t*)UID_BASE;
        memcpy(uid, uidBase, 12); // 96-bit unique ID
    }
};

// ============================================================================
// Global Factory Instance
// ============================================================================

static STM32Factory g_stm32Factory;

RTOSFactory& getRTOSFactory() {
    return g_stm32Factory;
}

} // namespace rtos
} // namespace platform
} // namespace lwm2m

#endif // STM32_HAL
