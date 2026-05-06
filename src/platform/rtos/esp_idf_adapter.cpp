/**
 * @file esp_idf_adapter.cpp
 * @brief ESP-IDF RTOS Adapter Implementation
 *
 * Implements the RTOS abstraction layer for ESP-IDF (Espressif IoT Development Framework).
 *
 * ESP-IDF is Espressif's official IoT development framework for ESP32, ESP32-S, ESP32-C series.
 * It is built on top of FreeRTOS but extends it with ESP32-specific features.
 *
 * Key Features:
 * - FreeRTOS-based (dual-core support on ESP32)
 * - Built-in WiFi, Bluetooth, BLE
 * - ESP-NOW (peer-to-peer wireless)
 * - Secure Boot & Flash Encryption
 * - OTA firmware updates
 * - NVS (Non-Volatile Storage)
 * - ESP-TLS (Mbed TLS wrapper)
 * - Power management (deep sleep, light sleep)
 *
 * Supported Chips:
 * - ESP32 (dual-core Xtensa LX6)
 * - ESP32-S2 (single-core Xtensa LX7, USB)
 * - ESP32-S3 (dual-core Xtensa LX7, AI acceleration)
 * - ESP32-C3 (single-core RISC-V, BLE 5.0)
 * - ESP32-C6 (RISC-V, WiFi 6, Zigbee, Thread)
 * - ESP32-H2 (RISC-V, Zigbee, Thread, BLE 5.2)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef ESP_IDF

// Include FreeRTOS adapter as base (70% code reuse)
#include "freertos_adapter.cpp"

#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_ota_ops.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"

namespace lwm2m {
namespace platform {
namespace rtos {

// ============================================================================
// ESP-IDF Flash Memory Implementation
// ============================================================================

class ESPFlash : public IMCUFlash {
public:
    ESPFlash() {
        esp_flash_get_size(nullptr, &totalSize_);
    }

    size_t getTotalSize() override {
        return totalSize_;
    }

    size_t getPageSize() override {
        return 4096; // ESP32 flash page size
    }

    size_t getSectorSize() override {
        return 4096; // ESP32 flash sector size
    }

    bool read(uint32_t address, void* buffer, size_t size) override {
        esp_err_t err = esp_flash_read(nullptr, buffer, address, size);
        return (err == ESP_OK);
    }

    bool write(uint32_t address, const void* data, size_t size) override {
        esp_err_t err = esp_flash_write(nullptr, data, address, size);
        return (err == ESP_OK);
    }

    bool erase(uint32_t address, size_t size) override {
        esp_err_t err = esp_flash_erase_region(nullptr, address, size);
        return (err == ESP_OK);
    }

    bool eraseSector(uint32_t sectorAddress) override {
        return erase(sectorAddress, getSectorSize());
    }

    bool lock() override {
        // ESP-IDF flash operations are already thread-safe
        return true;
    }

    bool unlock() override {
        return true;
    }

    bool isLocked() override {
        return false;
    }

private:
    size_t totalSize_;
};

// ============================================================================
// ESP-IDF OTA Update Implementation
// ============================================================================

class ESPOTA : public IMCUOTA {
public:
    ESPOTA() : updatePartition_(nullptr), updateHandle_(0) {}

    bool begin(size_t firmwareSize) override {
        updatePartition_ = esp_ota_get_next_update_partition(nullptr);
        if (updatePartition_ == nullptr) {
            return false;
        }

        esp_err_t err = esp_ota_begin(updatePartition_, firmwareSize, &updateHandle_);
        return (err == ESP_OK);
    }

    bool write(const void* data, size_t size) override {
        esp_err_t err = esp_ota_write(updateHandle_, data, size);
        return (err == ESP_OK);
    }

    bool end() override {
        esp_err_t err = esp_ota_end(updateHandle_);
        return (err == ESP_OK);
    }

    bool setBootPartition() override {
        esp_err_t err = esp_ota_set_boot_partition(updatePartition_);
        return (err == ESP_OK);
    }

    bool verify() override {
        // Verification happens automatically in esp_ota_end()
        return true;
    }

    uint32_t getCurrentVersion() override {
        const esp_app_desc_t* app_desc = esp_ota_get_app_description();
        return 0; // Parse from app_desc->version if needed
    }

    bool rollback() override {
        const esp_partition_t* partition = esp_ota_get_last_invalid_partition();
        if (partition == nullptr) {
            return false;
        }
        esp_err_t err = esp_ota_set_boot_partition(partition);
        return (err == ESP_OK);
    }

private:
    const esp_partition_t* updatePartition_;
    esp_ota_handle_t updateHandle_;
};

// ============================================================================
// ESP-IDF NVS (Non-Volatile Storage) Implementation
// ============================================================================

class ESPNVS : public IMCUNVM {
public:
    ESPNVS() : handle_(0), initialized_(false) {}

    bool init() override {
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            nvs_flash_erase();
            err = nvs_flash_init();
        }

        if (err == ESP_OK) {
            err = nvs_open("lwm2m", NVS_READWRITE, &handle_);
        }

        initialized_ = (err == ESP_OK);
        return initialized_;
    }

    bool read(const char* key, void* buffer, size_t size) override {
        if (!initialized_) return false;

        esp_err_t err = nvs_get_blob(handle_, key, buffer, &size);
        return (err == ESP_OK);
    }

    bool write(const char* key, const void* data, size_t size) override {
        if (!initialized_) return false;

        esp_err_t err = nvs_set_blob(handle_, key, data, size);
        if (err == ESP_OK) {
            err = nvs_commit(handle_);
        }
        return (err == ESP_OK);
    }

    bool erase(const char* key) override {
        if (!initialized_) return false;

        esp_err_t err = nvs_erase_key(handle_, key);
        if (err == ESP_OK) {
            err = nvs_commit(handle_);
        }
        return (err == ESP_OK);
    }

    bool eraseAll() override {
        if (!initialized_) return false;

        esp_err_t err = nvs_erase_all(handle_);
        if (err == ESP_OK) {
            err = nvs_commit(handle_);
        }
        return (err == ESP_OK);
    }

    size_t getSize(const char* key) override {
        if (!initialized_) return 0;

        size_t size = 0;
        nvs_get_blob(handle_, key, nullptr, &size);
        return size;
    }

    bool exists(const char* key) override {
        return (getSize(key) > 0);
    }

private:
    nvs_handle_t handle_;
    bool initialized_;
};

// ============================================================================
// ESP-IDF WiFi Interface Implementation
// ============================================================================

class ESPWiFi : public IWiFiInterface {
public:
    ESPWiFi() : initialized_(false), connected_(false) {}

    bool init() override {
        if (initialized_) return true;

        esp_err_t err = esp_netif_init();
        if (err != ESP_OK) return false;

        err = esp_event_loop_create_default();
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return false;

        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        err = esp_wifi_init(&cfg);
        if (err != ESP_OK) return false;

        err = esp_wifi_set_mode(WIFI_MODE_STA);
        if (err != ESP_OK) return false;

        initialized_ = true;
        return true;
    }

    bool connect(const char* ssid, const char* password) override {
        if (!initialized_) return false;

        wifi_config_t wifi_config = {};
        strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
        strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);

        esp_err_t err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
        if (err != ESP_OK) return false;

        err = esp_wifi_start();
        if (err != ESP_OK) return false;

        err = esp_wifi_connect();
        if (err == ESP_OK) {
            connected_ = true;
        }
        return (err == ESP_OK);
    }

    bool disconnect() override {
        if (!connected_) return true;

        esp_err_t err = esp_wifi_disconnect();
        if (err == ESP_OK) {
            connected_ = false;
        }
        return (err == ESP_OK);
    }

    bool isConnected() override {
        return connected_;
    }

    bool getIPAddress(char* buffer, size_t bufferSize) override {
        if (!connected_) return false;

        esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (netif == nullptr) return false;

        esp_netif_ip_info_t ip_info;
        esp_err_t err = esp_netif_get_ip_info(netif, &ip_info);
        if (err != ESP_OK) return false;

        snprintf(buffer, bufferSize, IPSTR, IP2STR(&ip_info.ip));
        return true;
    }

    int getRSSI() override {
        if (!connected_) return -100;

        wifi_ap_record_t ap_info;
        esp_err_t err = esp_wifi_sta_get_ap_info(&ap_info);
        if (err != ESP_OK) return -100;

        return ap_info.rssi;
    }

private:
    bool initialized_;
    bool connected_;
};

// ============================================================================
// ESP-IDF Memory Allocator (extends FreeRTOS with heap caps)
// ============================================================================

class ESPMemoryAllocator : public FreeRTOSMemoryAllocator {
public:
    void* allocate(size_t size) override {
        // Use MALLOC_CAP_DEFAULT for general purpose allocation
        return heap_caps_malloc(size, MALLOC_CAP_DEFAULT);
    }

    void* allocateDMA(size_t size) {
        // DMA-capable memory for WiFi, SPI, etc.
        return heap_caps_malloc(size, MALLOC_CAP_DMA);
    }

    void* allocateSPIRAM(size_t size) {
        // External SPIRAM if available
        return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
    }

    size_t getTotalHeap() override {
        return heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    }

    size_t getFreeHeap() override {
        return heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    }

    size_t getMinEverFreeHeap() override {
        return heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
    }

    size_t getLargestFreeBlock() override {
        return heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT);
    }

    size_t getSPIRAMSize() {
        return heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    }

    size_t getFreeSPIRAM() {
        return heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    }
};

// ============================================================================
// ESP-IDF Factory Implementation (extends FreeRTOS)
// ============================================================================

class ESPFactory : public FreeRTOSFactory {
public:
    const char* getRTOSName() override {
        return "ESP-IDF";
    }

    const char* getRTOSVersion() override {
        static char version[64];
        snprintf(version, sizeof(version), "%s (FreeRTOS %s)",
                 esp_get_idf_version(),
                 tskKERNEL_VERSION_NUMBER);
        return version;
    }

    const char* getChipInfo() {
        static char info[128];
        esp_chip_info_t chip_info;
        esp_chip_info(&chip_info);

        const char* model = "Unknown";
        switch (chip_info.model) {
            case CHIP_ESP32:   model = "ESP32"; break;
            case CHIP_ESP32S2: model = "ESP32-S2"; break;
            case CHIP_ESP32S3: model = "ESP32-S3"; break;
            case CHIP_ESP32C3: model = "ESP32-C3"; break;
            default: break;
        }

        snprintf(info, sizeof(info), "%s rev%d (%d cores, WiFi%s%s)",
                 model,
                 chip_info.revision,
                 chip_info.cores,
                 (chip_info.features & CHIP_FEATURE_BT) ? "+BT" : "",
                 (chip_info.features & CHIP_FEATURE_BLE) ? "+BLE" : "");
        return info;
    }

    IMCUFlash& getFlash() override {
        static ESPFlash flash;
        return flash;
    }

    IMCUOTA& getOTA() override {
        static ESPOTA ota;
        return ota;
    }

    IMCUNVM& getNVM() override {
        static ESPNVS nvs;
        return nvs;
    }

    IWiFiInterface& getWiFi() override {
        static ESPWiFi wifi;
        return wifi;
    }

    IMemoryAllocator& getMemoryAllocator() override {
        static ESPMemoryAllocator allocator;
        return allocator;
    }

    // ESP32-specific: Dual-core support
    bool runOnCore(uint8_t coreID, TaskFunction_t task, const char* name, void* param) {
        if (coreID > 1) return false;

        BaseType_t result = xTaskCreatePinnedToCore(
            task,
            name,
            4096,
            param,
            tskIDLE_PRIORITY + 1,
            nullptr,
            coreID
        );
        return (result == pdPASS);
    }

    // ESP32-specific: Deep sleep
    void deepSleep(uint64_t timeUs) {
        esp_sleep_enable_timer_wakeup(timeUs);
        esp_deep_sleep_start();
    }

    // ESP32-specific: Light sleep
    void lightSleep(uint64_t timeUs) {
        esp_sleep_enable_timer_wakeup(timeUs);
        esp_light_sleep_start();
    }
};

// ============================================================================
// Global Factory Instance
// ============================================================================

static ESPFactory g_espFactory;

RTOSFactory& getRTOSFactory() {
    return g_espFactory;
}

} // namespace rtos
} // namespace platform
} // namespace lwm2m

#endif // ESP_IDF
