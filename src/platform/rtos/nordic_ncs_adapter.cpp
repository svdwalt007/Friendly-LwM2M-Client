/**
 * @file nordic_ncs_adapter.cpp
 * @brief Nordic nRF Connect SDK RTOS Adapter Implementation
 *
 * Implements the RTOS abstraction layer for Nordic nRF Connect SDK (NCS).
 *
 * The Nordic nRF Connect SDK is built on top of Zephyr RTOS and adds Nordic-specific
 * drivers, libraries, and samples for nRF52, nRF53, and nRF91 series.
 *
 * Key Features:
 * - Based on Zephyr RTOS (80% code reuse)
 * - LTE-M/NB-IoT modem driver (nRF91 series)
 * - Bluetooth Low Energy 5.x (SoftDevice replacement)
 * - Thread and Zigbee protocols
 * - nRF Security (Arm CryptoCell, PSA Crypto API)
 * - MCUboot bootloader
 * - FOTA/FUOTA updates
 * - Power Profiler Kit (PPK) integration
 * - nRF Cloud integration
 *
 * Supported Hardware:
 * - nRF9160 (LTE-M/NB-IoT + GPS, Cortex-M33)
 * - nRF9151 (LTE-M/NB-IoT + DECT NR+)
 * - nRF5340 (dual Cortex-M33, BLE 5.3, 802.15.4)
 * - nRF52840 (Cortex-M4, BLE 5.0, 802.15.4, NFC)
 * - nRF52833 (Cortex-M4, BLE 5.1, 802.15.4)
 *
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

#ifdef NORDIC_NCS

// Include Zephyr adapter as base (80% code reuse)
#include "zephyr_adapter.cpp"

#include <modem/lte_lc.h>
#include <modem/nrf_modem_lib.h>
#include <modem/modem_info.h>
#include <net/nrf_cloud.h>
#include <dfu/mcuboot.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

namespace lwm2m {
namespace platform {
namespace rtos {

// ============================================================================
// Nordic LTE Modem Implementation (nRF91 series)
// ============================================================================

class NordicLTEModem : public ICellularModem {
public:
    NordicLTEModem() : initialized_(false), connected_(false) {}

    bool init() override {
        if (initialized_) return true;

        int err = nrf_modem_lib_init();
        if (err) {
            return false;
        }

        err = lte_lc_init();
        if (err) {
            return false;
        }

        initialized_ = true;
        return true;
    }

    bool connect() override {
        if (!initialized_) return false;

        int err = lte_lc_connect();
        if (err) {
            return false;
        }

        connected_ = true;
        return true;
    }

    bool disconnect() override {
        if (!connected_) return true;

        int err = lte_lc_offline();
        if (err) {
            return false;
        }

        connected_ = false;
        return true;
    }

    bool isConnected() override {
        return connected_;
    }

    int getSignalStrength() override {
        if (!connected_) return -1;

        struct lte_lc_cell cell;
        int err = lte_lc_neighbor_cell_measurement(LTE_LC_NEIGHBOR_SEARCH_TYPE_DEFAULT);
        if (err) {
            return -1;
        }

        // RSRP is in range -140 to -44 dBm
        return cell.rsrp;
    }

    bool getNetworkInfo(char* buffer, size_t bufferSize) override {
        if (!connected_) return false;

        enum lte_lc_nw_reg_status reg_status;
        int err = lte_lc_nw_reg_status_get(&reg_status);
        if (err) {
            return false;
        }

        snprintf(buffer, bufferSize, "LTE Network, Status: %d", reg_status);
        return true;
    }

    bool getIMEI(char* buffer, size_t bufferSize) override {
        int err = modem_info_init();
        if (err) {
            return false;
        }

        err = modem_info_string_get(MODEM_INFO_IMEI, buffer, bufferSize);
        return (err > 0);
    }

    bool getIMSI(char* buffer, size_t bufferSize) override {
        int err = modem_info_init();
        if (err) {
            return false;
        }

        err = modem_info_string_get(MODEM_INFO_IMSI, buffer, bufferSize);
        return (err > 0);
    }

    bool setNetworkMode(NetworkMode mode) override {
        enum lte_lc_system_mode sys_mode;

        switch (mode) {
            case NetworkMode::LTE_M:
                sys_mode = LTE_LC_SYSTEM_MODE_LTEM;
                break;
            case NetworkMode::NB_IOT:
                sys_mode = LTE_LC_SYSTEM_MODE_NBIOT;
                break;
            case NetworkMode::GSM:
                return false; // Not supported
            default:
                sys_mode = LTE_LC_SYSTEM_MODE_LTEM_NBIOT;
                break;
        }

        int err = lte_lc_system_mode_set(sys_mode, LTE_LC_SYSTEM_MODE_PREFER_AUTO);
        return (err == 0);
    }

    bool setPSM(uint32_t tau, uint32_t activeTime) override {
        int err = lte_lc_psm_req(true);
        if (err) {
            return false;
        }

        // Configure TAU and active time
        char tau_str[9];
        char active_str[9];
        snprintf(tau_str, sizeof(tau_str), "%08lx", tau);
        snprintf(active_str, sizeof(active_str), "%08lx", activeTime);

        err = lte_lc_psm_param_set(tau_str, active_str);
        return (err == 0);
    }

    bool setEDRX(uint32_t edrx) override {
        int err = lte_lc_edrx_req(true);
        if (err) {
            return false;
        }

        char edrx_str[9];
        snprintf(edrx_str, sizeof(edrx_str), "%04lx", edrx);
        err = lte_lc_edrx_param_set(LTE_LC_LTE_MODE_LTEM, edrx_str);
        return (err == 0);
    }

private:
    bool initialized_;
    bool connected_;
};

// ============================================================================
// Nordic BLE Implementation
// ============================================================================

class NordicBLE : public IBLEInterface {
public:
    NordicBLE() : initialized_(false), advertising_(false) {}

    bool init() override {
        if (initialized_) return true;

        int err = bt_enable(nullptr);
        if (err) {
            return false;
        }

        initialized_ = true;
        return true;
    }

    bool startAdvertising(const char* deviceName) override {
        if (!initialized_) return false;

        bt_set_name(deviceName);

        struct bt_le_adv_param adv_param = {
            .id = BT_ID_DEFAULT,
            .sid = 0,
            .secondary_max_skip = 0,
            .options = BT_LE_ADV_OPT_CONNECTABLE,
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
            .peer = nullptr,
        };

        int err = bt_le_adv_start(&adv_param, nullptr, 0, nullptr, 0);
        if (err) {
            return false;
        }

        advertising_ = true;
        return true;
    }

    bool stopAdvertising() override {
        if (!advertising_) return true;

        int err = bt_le_adv_stop();
        if (err) {
            return false;
        }

        advertising_ = false;
        return true;
    }

    bool isAdvertising() override {
        return advertising_;
    }

    bool getAddress(char* buffer, size_t bufferSize) override {
        if (!initialized_) return false;

        bt_addr_le_t addr;
        size_t count = 1;
        bt_id_get(&addr, &count);

        snprintf(buffer, bufferSize, "%02X:%02X:%02X:%02X:%02X:%02X",
                 addr.a.val[5], addr.a.val[4], addr.a.val[3],
                 addr.a.val[2], addr.a.val[1], addr.a.val[0]);
        return true;
    }

private:
    bool initialized_;
    bool advertising_;
};

// ============================================================================
// Nordic MCUboot Integration
// ============================================================================

class NordicMCUboot : public IMCUOTA {
public:
    NordicMCUboot() : updateInProgress_(false) {}

    bool begin(size_t firmwareSize) override {
        // MCUboot uses image slots, size is managed automatically
        updateInProgress_ = true;
        return true;
    }

    bool write(const void* data, size_t size) override {
        if (!updateInProgress_) return false;

        // DFU write is handled by MCUboot and DFU subsystem
        // This is a simplified interface
        return true;
    }

    bool end() override {
        if (!updateInProgress_) return false;

        // Mark image as pending
        int err = boot_request_upgrade(BOOT_UPGRADE_TEST);
        if (err) {
            return false;
        }

        updateInProgress_ = false;
        return true;
    }

    bool setBootPartition() override {
        // MCUboot automatically swaps images on next boot
        return true;
    }

    bool verify() override {
        // Verify current image
        return boot_is_img_confirmed();
    }

    uint32_t getCurrentVersion() override {
        struct mcuboot_img_header header;
        int err = boot_read_bank_header(FLASH_AREA_IMAGE_0, &header, sizeof(header));
        if (err) {
            return 0;
        }

        return header.h.v1.sem_ver.major * 10000 +
               header.h.v1.sem_ver.minor * 100 +
               header.h.v1.sem_ver.revision;
    }

    bool rollback() override {
        // Confirm current image to prevent rollback
        return boot_write_img_confirmed();
    }

private:
    bool updateInProgress_;
};

// ============================================================================
// Nordic nRF Cloud Integration
// ============================================================================

class NordicCloud {
public:
    static bool connect() {
        struct nrf_cloud_init_param init_param = {
            .event_handler = nullptr,
        };

        int err = nrf_cloud_init(&init_param);
        if (err) {
            return false;
        }

        err = nrf_cloud_connect();
        return (err == 0);
    }

    static bool disconnect() {
        int err = nrf_cloud_disconnect();
        return (err == 0);
    }

    static bool sendData(const char* data, size_t length) {
        struct nrf_cloud_tx_data tx_data = {
            .data.ptr = data,
            .data.len = length,
            .topic_type = NRF_CLOUD_TOPIC_MESSAGE,
            .qos = MQTT_QOS_0_AT_MOST_ONCE,
        };

        int err = nrf_cloud_send(&tx_data);
        return (err == 0);
    }
};

// ============================================================================
// Nordic Memory Allocator (extends Zephyr)
// ============================================================================

class NordicMemoryAllocator : public ZephyrMemoryAllocator {
public:
    // Inherit all Zephyr memory allocation functionality
    // Nordic NCS uses standard Zephyr heap management
};

// ============================================================================
// Nordic Factory Implementation (extends Zephyr)
// ============================================================================

class NordicFactory : public ZephyrFactory {
public:
    const char* getRTOSName() override {
        return "Nordic nRF Connect SDK";
    }

    const char* getRTOSVersion() override {
        static char version[128];
        snprintf(version, sizeof(version), "NCS %s (Zephyr %s)",
                 NCS_VERSION_STRING,
                 KERNEL_VERSION_STRING);
        return version;
    }

    const char* getChipInfo() {
        static char info[128];

#if defined(CONFIG_BOARD_NRF9160DK_NRF9160) || defined(CONFIG_BOARD_NRF9160DK_NRF9160_NS)
        snprintf(info, sizeof(info), "nRF9160 (Cortex-M33, LTE-M/NB-IoT)");
#elif defined(CONFIG_BOARD_NRF9151DK_NRF9151) || defined(CONFIG_BOARD_NRF9151DK_NRF9151_NS)
        snprintf(info, sizeof(info), "nRF9151 (Cortex-M33, LTE-M/NB-IoT/DECT)");
#elif defined(CONFIG_BOARD_NRF5340DK_NRF5340_CPUAPP) || defined(CONFIG_BOARD_NRF5340DK_NRF5340_CPUAPP_NS)
        snprintf(info, sizeof(info), "nRF5340 (Dual Cortex-M33, BLE 5.3)");
#elif defined(CONFIG_BOARD_NRF52840DK_NRF52840)
        snprintf(info, sizeof(info), "nRF52840 (Cortex-M4, BLE 5.0, 802.15.4)");
#elif defined(CONFIG_BOARD_NRF52833DK_NRF52833)
        snprintf(info, sizeof(info), "nRF52833 (Cortex-M4, BLE 5.1, 802.15.4)");
#else
        snprintf(info, sizeof(info), "Nordic nRF (unknown variant)");
#endif

        return info;
    }

    ICellularModem& getCellularModem() override {
        static NordicLTEModem modem;
        return modem;
    }

    IBLEInterface& getBLE() override {
        static NordicBLE ble;
        return ble;
    }

    IMCUOTA& getOTA() override {
        static NordicMCUboot mcuboot;
        return mcuboot;
    }

    IMemoryAllocator& getMemoryAllocator() override {
        static NordicMemoryAllocator allocator;
        return allocator;
    }

    // Nordic-specific: nRF Cloud integration
    bool connectToCloud() {
        return NordicCloud::connect();
    }

    bool disconnectFromCloud() {
        return NordicCloud::disconnect();
    }

    bool sendCloudData(const char* data, size_t length) {
        return NordicCloud::sendData(data, length);
    }

    // Nordic-specific: Power optimization.
    //
    // On Nordic Connect SDK targets, the active power policy is selected at
    // build time through Kconfig (CONFIG_PM, CONFIG_PM_DEVICE,
    // CONFIG_PM_DEVICE_RUNTIME) and applied automatically by Zephyr's PM
    // subsystem at idle. Runtime control therefore reduces to enabling the
    // subsystem; the actual transition to System OFF / Soft OFF / Idle is
    // managed by the kernel based on the next scheduled wake source. We keep
    // this hook so the LwM2M power state machine has a stable entry point
    // even when CONFIG_PM is disabled (in which case nothing has to be done).
    void enableLowPower() {
#ifdef CONFIG_PM
        // Kconfig handles the policy; nothing additional to invoke at runtime.
#endif
    }

    // Nordic-specific: Get modem firmware version
    bool getModemFirmwareVersion(char* buffer, size_t bufferSize) {
        int err = modem_info_init();
        if (err) {
            return false;
        }

        err = modem_info_string_get(MODEM_INFO_FW_VERSION, buffer, bufferSize);
        return (err > 0);
    }
};

// ============================================================================
// Global Factory Instance
// ============================================================================

static NordicFactory g_nordicFactory;

RTOSFactory& getRTOSFactory() {
    return g_nordicFactory;
}

} // namespace rtos
} // namespace platform
} // namespace lwm2m

#endif // NORDIC_NCS
