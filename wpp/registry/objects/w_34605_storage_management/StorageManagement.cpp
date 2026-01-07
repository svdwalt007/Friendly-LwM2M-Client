/*
 * StorageManagement (Object 34605)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_34605_storage_management/StorageManagement.h"
#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

#define TAG "StorageManagement"

namespace wpp {

StorageManagement::StorageManagement(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "Creating StorageManagement instance %d", instId);
}

StorageManagement::~StorageManagement() {
    WPP_LOGD(TAG, "Destroying StorageManagement instance");
}

Object& StorageManagement::object(WppClient& client) {
    return client.registry().storageManagement();
}

Instance* StorageManagement::instance(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().storageManagement().instance(instId);
    if (!inst) return NULL;
    return static_cast<StorageManagement*>(inst);
}

Instance* StorageManagement::createInst(WppClient& client, INST_T instId) {
    Instance *inst = client.registry().storageManagement().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<StorageManagement*>(inst);
}

bool StorageManagement::remove(WppClient& client, INST_T instId) {
    return client.registry().storageManagement().remove(instId);
}

bool StorageManagement::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing resources");

    std::vector<Resource> resources = {
        {STORAGE_NAME_0,       ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {STORAGE_TYPE_1,       ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {MOUNT_POINT_2,        ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {TOTAL_CAPACITY_3,     ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {USED_CAPACITY_4,      ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {AVAILABLE_CAPACITY_5, ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {USAGE_PERCENTAGE_6,   ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {FILESYSTEM_TYPE_7,    ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING},
        {IS_MOUNTED_8,         ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::BOOL},
        {IS_BOOTABLE_9,        ItemOp(ItemOp::READ),     IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::BOOL},
        {MOUNT_10,             ItemOp(ItemOp::EXECUTE),  IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
        {UNMOUNT_11,           ItemOp(ItemOp::EXECUTE),  IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
        {FORMAT_12,            ItemOp(ItemOp::EXECUTE),  IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::EXECUTE},
    };

    setupResources(std::move(resources));

    // Set default values
    resource(STORAGE_NAME_0)->set<STRING_T>("NAND");
    resource(STORAGE_TYPE_1)->set<INT_T>(STORAGE_NAND);
    resource(MOUNT_POINT_2)->set<STRING_T>("/");
    resource(TOTAL_CAPACITY_3)->set<INT_T>(256*1024*1024); // 256 MB
    resource(USED_CAPACITY_4)->set<INT_T>(0);
    resource(AVAILABLE_CAPACITY_5)->set<INT_T>(256*1024*1024);
    resource(USAGE_PERCENTAGE_6)->set<INT_T>(0);
    resource(FILESYSTEM_TYPE_7)->set<STRING_T>("squashfs");
    resource(IS_MOUNTED_8)->set<BOOL_T>(true);
    resource(IS_BOOTABLE_9)->set<BOOL_T>(true);

    // Set execute handlers
    resource(MOUNT_10)->set<EXECUTE_T>(mountStorage);
    resource(UNMOUNT_11)->set<EXECUTE_T>(unmountStorage);
    resource(FORMAT_12)->set<EXECUTE_T>(formatStorage);

    // Set validators
    resource(STORAGE_TYPE_1)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= STORAGE_NAND && value <= STORAGE_SD;
        }
    );

    resource(USAGE_PERCENTAGE_6)->setDataVerifier(
        (VERIFY_INT_T)[](const INT_T& value) {
            return value >= 0 && value <= 100;
        }
    );

    return true;
}

bool StorageManagement::validate(ID_T resId, const void *data, size_t size) {
    return true;
}

bool StorageManagement::mountStorage(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Execute: Mount Storage");
    // TODO: Implement mount logic
    return true;
}

bool StorageManagement::unmountStorage(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Execute: Unmount Storage");
    // TODO: Implement unmount logic
    return true;
}

bool StorageManagement::formatStorage(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Execute: Format Storage");
    // TODO: Implement format logic (dangerous - requires confirmation)
    return true;
}

} // namespace wpp
