/*
 * StorageManagement (Object 34605)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_W_34605_STORAGE_MANAGEMENT_H
#define WPP_W_34605_STORAGE_MANAGEMENT_H

#include "StorageManagementConfig.h"
#include "StorageManagementInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class StorageManagement : public Instance {
public:
    enum ID: ID_T {
        STORAGE_NAME_0 = 0,
        STORAGE_TYPE_1 = 1,
        MOUNT_POINT_2 = 2,
        TOTAL_CAPACITY_3 = 3,
        USED_CAPACITY_4 = 4,
        AVAILABLE_CAPACITY_5 = 5,
        USAGE_PERCENTAGE_6 = 6,
        FILESYSTEM_TYPE_7 = 7,
        IS_MOUNTED_8 = 8,
        IS_BOOTABLE_9 = 9,
        MOUNT_10 = 10,
        UNMOUNT_11 = 11,
        FORMAT_12 = 12,
    };

    enum StorageType: uint8_t {
        STORAGE_NAND = 0,
        STORAGE_NVME = 1,
        STORAGE_USB = 2,
        STORAGE_SD = 3
    };

    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId = 0);
    static bool remove(WppClient& client, INST_T instId = 0);

    StorageManagement(Object& object, INST_T instId);
    ~StorageManagement() override;

protected:
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    bool initResources(ItemOp *) override;
    void updateCapacityInfo();
    static bool mountStorage(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool unmountStorage(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool formatStorage(Instance& inst, ID_T resId, const OPAQUE_T& data);
};

} // namespace wpp

#endif // WPP_W_34605_STORAGE_MANAGEMENT_H
