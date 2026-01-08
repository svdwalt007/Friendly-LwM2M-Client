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

#include <sys/statvfs.h>
#include <sys/mount.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include <set>

#define TAG "StorageManagement"

namespace wpp {

// Critical partitions that should never be formatted
static const std::set<std::string> PROTECTED_MOUNT_POINTS = {
    "/", "/boot", "/boot/efi", "/etc", "/usr", "/lib", "/bin", "/sbin"
};

// Helper structure to hold mount information
struct MountInfo {
    std::string device;
    std::string mountPoint;
    std::string fsType;
    std::string options;
    bool isMounted;
};

// Helper function to read mount information from /proc/mounts
static bool readMountInfo(const std::string& mountPoint, MountInfo& info) {
    std::ifstream mounts("/proc/mounts");
    if (!mounts.is_open()) {
        WPP_LOGE(TAG, "Failed to open /proc/mounts");
        return false;
    }

    std::string line;
    while (std::getline(mounts, line)) {
        std::istringstream iss(line);
        std::string device, mount, fstype, options;

        if (!(iss >> device >> mount >> fstype >> options)) {
            continue;
        }

        if (mount == mountPoint) {
            info.device = device;
            info.mountPoint = mount;
            info.fsType = fstype;
            info.options = options;
            info.isMounted = true;
            mounts.close();
            return true;
        }
    }

    mounts.close();
    info.isMounted = false;
    return false;
}

// Helper function to get capacity information using statvfs
static bool getCapacityInfo(const std::string& path,
                           uint64_t& totalCapacity,
                           uint64_t& usedCapacity,
                           uint64_t& availableCapacity,
                           int& usagePercentage) {
    struct statvfs stat;

    if (statvfs(path.c_str(), &stat) != 0) {
        WPP_LOGE(TAG, "Failed to get filesystem stats for %s: %s",
                 path.c_str(), strerror(errno));
        return false;
    }

    // Calculate capacities in bytes
    totalCapacity = static_cast<uint64_t>(stat.f_blocks) * stat.f_frsize;
    availableCapacity = static_cast<uint64_t>(stat.f_bavail) * stat.f_frsize;
    usedCapacity = totalCapacity - (static_cast<uint64_t>(stat.f_bfree) * stat.f_frsize);

    // Calculate usage percentage
    if (totalCapacity > 0) {
        usagePercentage = static_cast<int>((usedCapacity * 100) / totalCapacity);
    } else {
        usagePercentage = 0;
    }

    WPP_LOGD(TAG, "Capacity for %s - Total: %llu, Used: %llu, Available: %llu, Usage: %d%%",
             path.c_str(),
             (unsigned long long)totalCapacity,
             (unsigned long long)usedCapacity,
             (unsigned long long)availableCapacity,
             usagePercentage);

    return true;
}

// Helper function to check if a mount point is protected
static bool isProtectedMountPoint(const std::string& mountPoint) {
    return PROTECTED_MOUNT_POINTS.find(mountPoint) != PROTECTED_MOUNT_POINTS.end();
}

// Helper function to execute a system command and get output
static bool executeCommand(const std::string& cmd, std::string& output) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        WPP_LOGE(TAG, "Failed to execute command: %s", cmd.c_str());
        return false;
    }

    char buffer[256];
    output.clear();
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

    int status = pclose(pipe);
    return (status == 0);
}

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

    // Read initial storage information
    std::string mountPoint = "/";  // Default to root filesystem
    MountInfo info;

    // Try to read actual mount information
    if (readMountInfo(mountPoint, info)) {
        resource(STORAGE_NAME_0)->set<STRING_T>(info.device);
        resource(MOUNT_POINT_2)->set<STRING_T>(info.mountPoint);
        resource(FILESYSTEM_TYPE_7)->set<STRING_T>(info.fsType);
        resource(IS_MOUNTED_8)->set<BOOL_T>(info.isMounted);
    } else {
        // Set default values if unable to read
        resource(STORAGE_NAME_0)->set<STRING_T>("NAND");
        resource(MOUNT_POINT_2)->set<STRING_T>(mountPoint);
        resource(FILESYSTEM_TYPE_7)->set<STRING_T>("unknown");
        resource(IS_MOUNTED_8)->set<BOOL_T>(false);
    }

    // Set storage type based on device name
    resource(STORAGE_TYPE_1)->set<INT_T>(STORAGE_NAND);

    // Determine if bootable (root filesystem is typically bootable)
    resource(IS_BOOTABLE_9)->set<BOOL_T>(mountPoint == "/");

    // Get capacity information
    uint64_t totalCapacity = 0;
    uint64_t usedCapacity = 0;
    uint64_t availableCapacity = 0;
    int usagePercentage = 0;

    if (getCapacityInfo(mountPoint, totalCapacity, usedCapacity,
                       availableCapacity, usagePercentage)) {
        resource(TOTAL_CAPACITY_3)->set<INT_T>(static_cast<INT_T>(totalCapacity));
        resource(USED_CAPACITY_4)->set<INT_T>(static_cast<INT_T>(usedCapacity));
        resource(AVAILABLE_CAPACITY_5)->set<INT_T>(static_cast<INT_T>(availableCapacity));
        resource(USAGE_PERCENTAGE_6)->set<INT_T>(usagePercentage);
    } else {
        // Set default values if unable to get capacity
        resource(TOTAL_CAPACITY_3)->set<INT_T>(0);
        resource(USED_CAPACITY_4)->set<INT_T>(0);
        resource(AVAILABLE_CAPACITY_5)->set<INT_T>(0);
        resource(USAGE_PERCENTAGE_6)->set<INT_T>(0);
    }

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

    // Note: For periodic capacity updates, you can:
    // 1. Call updateCapacityInfo() manually when needed
    // 2. Set up a timer in your application to call it periodically
    // 3. Hook into the application's main loop to update at regular intervals

    return true;
}

bool StorageManagement::validate(ID_T resId, const void *data, size_t size) {
    return true;
}

void StorageManagement::updateCapacityInfo() {
    // Get current mount point
    Resource* mountPointRes = resource(MOUNT_POINT_2);
    if (!mountPointRes) return;

    std::string mountPoint = mountPointRes->get<STRING_T>();

    // Check if mounted
    MountInfo info;
    bool isMounted = readMountInfo(mountPoint, info);
    resource(IS_MOUNTED_8)->set<BOOL_T>(isMounted);

    if (isMounted) {
        // Update filesystem type
        resource(FILESYSTEM_TYPE_7)->set<STRING_T>(info.fsType);

        // Update capacity information
        uint64_t totalCapacity = 0;
        uint64_t usedCapacity = 0;
        uint64_t availableCapacity = 0;
        int usagePercentage = 0;

        if (getCapacityInfo(mountPoint, totalCapacity, usedCapacity,
                           availableCapacity, usagePercentage)) {
            resource(TOTAL_CAPACITY_3)->set<INT_T>(static_cast<INT_T>(totalCapacity));
            resource(USED_CAPACITY_4)->set<INT_T>(static_cast<INT_T>(usedCapacity));
            resource(AVAILABLE_CAPACITY_5)->set<INT_T>(static_cast<INT_T>(availableCapacity));
            resource(USAGE_PERCENTAGE_6)->set<INT_T>(usagePercentage);

            // Notify server about resource value changes
            notifyResChanged(TOTAL_CAPACITY_3);
            notifyResChanged(USED_CAPACITY_4);
            notifyResChanged(AVAILABLE_CAPACITY_5);
            notifyResChanged(USAGE_PERCENTAGE_6);
        }
    }
}

bool StorageManagement::mountStorage(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Execute: Mount Storage");

    StorageManagement& storage = static_cast<StorageManagement&>(inst);

    // Get mount point
    Resource* mountPointRes = storage.resource(MOUNT_POINT_2);
    if (!mountPointRes) {
        WPP_LOGE(TAG, "Mount point resource not found");
        return false;
    }

    std::string mountPoint = mountPointRes->get<STRING_T>();

    // Check if already mounted
    MountInfo info;
    if (readMountInfo(mountPoint, info) && info.isMounted) {
        WPP_LOGW(TAG, "Storage already mounted at %s", mountPoint.c_str());
        return true;
    }

    // Parse execute payload for device and filesystem type
    // Expected format: "device=<device>;fstype=<type>;options=<options>"
    std::string payload(reinterpret_cast<const char*>(data.data()), data.size());
    std::string device, fstype = "auto", options = "defaults";

    std::istringstream iss(payload);
    std::string token;
    while (std::getline(iss, token, ';')) {
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);

            if (key == "device") {
                device = value;
            } else if (key == "fstype") {
                fstype = value;
            } else if (key == "options") {
                options = value;
            }
        }
    }

    if (device.empty()) {
        // Try to get device from storage name
        Resource* nameRes = storage.resource(STORAGE_NAME_0);
        if (nameRes) {
            device = nameRes->get<STRING_T>();
        } else {
            WPP_LOGE(TAG, "No device specified for mount operation");
            return false;
        }
    }

    WPP_LOGI(TAG, "Mounting %s at %s with fstype %s and options %s",
             device.c_str(), mountPoint.c_str(), fstype.c_str(), options.c_str());

    // Create mount point directory if it doesn't exist
    std::string mkdirCmd = "mkdir -p " + mountPoint;
    std::string output;
    executeCommand(mkdirCmd, output);

    // Attempt to mount using mount system call
    // For simplicity and compatibility, use system command
    std::string mountCmd = "mount";
    if (fstype != "auto") {
        mountCmd += " -t " + fstype;
    }
    mountCmd += " -o " + options;
    mountCmd += " " + device + " " + mountPoint;

    if (!executeCommand(mountCmd, output)) {
        WPP_LOGE(TAG, "Mount failed: %s", output.c_str());
        return false;
    }

    // Update mount status
    storage.resource(IS_MOUNTED_8)->set<BOOL_T>(true);
    if (fstype != "auto") {
        storage.resource(FILESYSTEM_TYPE_7)->set<STRING_T>(fstype);
    }

    // Update capacity information
    storage.updateCapacityInfo();

    WPP_LOGI(TAG, "Successfully mounted %s at %s", device.c_str(), mountPoint.c_str());
    return true;
}

bool StorageManagement::unmountStorage(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Execute: Unmount Storage");

    StorageManagement& storage = static_cast<StorageManagement&>(inst);

    // Get mount point
    Resource* mountPointRes = storage.resource(MOUNT_POINT_2);
    if (!mountPointRes) {
        WPP_LOGE(TAG, "Mount point resource not found");
        return false;
    }

    std::string mountPoint = mountPointRes->get<STRING_T>();

    // Check if it's a protected mount point
    if (isProtectedMountPoint(mountPoint)) {
        WPP_LOGE(TAG, "Cannot unmount protected mount point: %s", mountPoint.c_str());
        return false;
    }

    // Check if currently mounted
    MountInfo info;
    if (!readMountInfo(mountPoint, info) || !info.isMounted) {
        WPP_LOGW(TAG, "Storage not mounted at %s", mountPoint.c_str());
        return true;
    }

    WPP_LOGI(TAG, "Unmounting %s", mountPoint.c_str());

    // Parse execute payload for force option
    // Expected format: "force=true" or "force=false"
    bool force = false;
    std::string payload(reinterpret_cast<const char*>(data.data()), data.size());
    if (payload.find("force=true") != std::string::npos) {
        force = true;
    }

    // Try to unmount
    std::string umountCmd = "umount";
    if (force) {
        umountCmd += " -f";  // Force unmount
    }
    umountCmd += " " + mountPoint;

    std::string output;
    if (!executeCommand(umountCmd, output)) {
        WPP_LOGE(TAG, "Unmount failed: %s", output.c_str());

        // Check if filesystem is busy
        if (output.find("busy") != std::string::npos ||
            output.find("target is busy") != std::string::npos) {
            WPP_LOGE(TAG, "Filesystem is busy. Try closing applications using it.");

            // Show what's using the mount point
            std::string lsofCmd = "lsof +f -- " + mountPoint + " 2>/dev/null | tail -n +2";
            std::string lsofOutput;
            executeCommand(lsofCmd, lsofOutput);
            if (!lsofOutput.empty()) {
                WPP_LOGI(TAG, "Processes using %s:\n%s", mountPoint.c_str(), lsofOutput.c_str());
            }
        }

        return false;
    }

    // Update mount status
    storage.resource(IS_MOUNTED_8)->set<BOOL_T>(false);

    // Reset capacity information
    storage.resource(TOTAL_CAPACITY_3)->set<INT_T>(0);
    storage.resource(USED_CAPACITY_4)->set<INT_T>(0);
    storage.resource(AVAILABLE_CAPACITY_5)->set<INT_T>(0);
    storage.resource(USAGE_PERCENTAGE_6)->set<INT_T>(0);

    WPP_LOGI(TAG, "Successfully unmounted %s", mountPoint.c_str());
    return true;
}

bool StorageManagement::formatStorage(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WPP_LOGI(TAG, "Execute: Format Storage");

    StorageManagement& storage = static_cast<StorageManagement&>(inst);

    // Get mount point and device
    Resource* mountPointRes = storage.resource(MOUNT_POINT_2);
    Resource* nameRes = storage.resource(STORAGE_NAME_0);

    if (!mountPointRes || !nameRes) {
        WPP_LOGE(TAG, "Required resources not found");
        return false;
    }

    std::string mountPoint = mountPointRes->get<STRING_T>();
    std::string device = nameRes->get<STRING_T>();

    // SAFETY CHECK: Prevent formatting critical partitions
    if (isProtectedMountPoint(mountPoint)) {
        WPP_LOGE(TAG, "CRITICAL: Cannot format protected mount point: %s", mountPoint.c_str());
        return false;
    }

    // Parse execute payload for filesystem type and confirmation
    // Expected format: "fstype=<type>;confirm=YES_FORMAT_DEVICE"
    std::string payload(reinterpret_cast<const char*>(data.data()), data.size());
    std::string fstype = "ext4";  // Default filesystem
    bool confirmed = false;

    std::istringstream iss(payload);
    std::string token;
    while (std::getline(iss, token, ';')) {
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);

            if (key == "fstype") {
                fstype = value;
            } else if (key == "confirm") {
                confirmed = (value == "YES_FORMAT_DEVICE");
            }
        }
    }

    // Require explicit confirmation
    if (!confirmed) {
        WPP_LOGE(TAG, "Format operation requires explicit confirmation: confirm=YES_FORMAT_DEVICE");
        return false;
    }

    // Validate filesystem type
    static const std::set<std::string> SUPPORTED_FILESYSTEMS = {
        "ext2", "ext3", "ext4", "vfat", "fat", "ntfs", "exfat"
    };

    if (SUPPORTED_FILESYSTEMS.find(fstype) == SUPPORTED_FILESYSTEMS.end()) {
        WPP_LOGE(TAG, "Unsupported filesystem type: %s", fstype.c_str());
        return false;
    }

    WPP_LOGW(TAG, "WARNING: Formatting %s (%s) with %s filesystem - ALL DATA WILL BE LOST",
             device.c_str(), mountPoint.c_str(), fstype.c_str());

    // Unmount if currently mounted
    MountInfo info;
    if (readMountInfo(mountPoint, info) && info.isMounted) {
        WPP_LOGI(TAG, "Unmounting %s before format", mountPoint.c_str());
        std::string umountCmd = "umount " + mountPoint;
        std::string output;
        if (!executeCommand(umountCmd, output)) {
            WPP_LOGE(TAG, "Failed to unmount before format: %s", output.c_str());
            return false;
        }
        storage.resource(IS_MOUNTED_8)->set<BOOL_T>(false);
    }

    // Build format command based on filesystem type
    std::string formatCmd;
    if (fstype == "ext2" || fstype == "ext3" || fstype == "ext4") {
        formatCmd = "mkfs." + fstype + " -F " + device;
    } else if (fstype == "vfat" || fstype == "fat") {
        formatCmd = "mkfs.vfat " + device;
    } else if (fstype == "ntfs") {
        formatCmd = "mkfs.ntfs -f " + device;
    } else if (fstype == "exfat") {
        formatCmd = "mkfs.exfat " + device;
    }

    WPP_LOGI(TAG, "Executing format command: %s", formatCmd.c_str());

    std::string output;
    if (!executeCommand(formatCmd, output)) {
        WPP_LOGE(TAG, "Format failed: %s", output.c_str());
        return false;
    }

    // Update filesystem type
    storage.resource(FILESYSTEM_TYPE_7)->set<STRING_T>(fstype);

    // Reset capacity (will be updated after mount)
    storage.resource(TOTAL_CAPACITY_3)->set<INT_T>(0);
    storage.resource(USED_CAPACITY_4)->set<INT_T>(0);
    storage.resource(AVAILABLE_CAPACITY_5)->set<INT_T>(0);
    storage.resource(USAGE_PERCENTAGE_6)->set<INT_T>(0);

    WPP_LOGI(TAG, "Successfully formatted %s with %s filesystem", device.c_str(), fstype.c_str());
    WPP_LOGI(TAG, "Format output: %s", output.c_str());

    return true;
}

} // namespace wpp
