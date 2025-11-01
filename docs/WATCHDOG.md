# Hardware Watchdog LWM2M Object (34607)

## Overview

This document describes the implementation of the Hardware Watchdog LWM2M Object (ID: 34607) for the Friendly LWM2M Client, specifically designed for OpenWRT One Router and similar embedded Linux systems.

## Table of Contents

- [Introduction](#introduction)
- [LWM2M Object Definition](#lwm2m-object-definition)
- [OpenWRT Integration](#openwrt-integration)
- [Resource Details](#resource-details)
- [Usage Examples](#usage-examples)
- [Configuration](#configuration)
- [Troubleshooting](#troubleshooting)

## Introduction

The Hardware Watchdog object provides remote monitoring and management of hardware watchdog timers in embedded systems. A watchdog timer is a hardware timer that must be periodically reset ("petted") to prevent system reboot. This is crucial for:

- **System Reliability**: Automatic recovery from system hangs
- **Remote Monitoring**: Track watchdog activity via LWM2M
- **Configuration**: Remotely configure timeout and interval settings
- **Statistics**: Monitor reboot causes and system stability

## LWM2M Object Definition

### Object Information

- **Object ID**: 34607
- **Object URN**: urn:oma:lwm2m:x:34607
- **Object Name**: Hardware Watchdog
- **Multiple Instances**: Single
- **Mandatory**: Optional
- **Object Version**: 1.0
- **LWM2M Version**: 1.0

### Namespace

This object uses the Walt Technologies custom object ID range (34600-34649). Object IDs 34600-34606 are reserved for other implementations.

## OpenWRT Integration

### Watchdog Device

OpenWRT systems typically expose the hardware watchdog via `/dev/watchdog`. The implementation:

1. **Opens** `/dev/watchdog` when enabled
2. **Writes** periodically to the device to reset the timer
3. **Closes** with magic character 'V' when magic close is enabled
4. **Monitors** hardware timeout values via ioctl

### procd Integration

OpenWRT's `procd` normally manages the watchdog. When using this LWM2M object:

- Stop procd's watchdog management: `ubus call system watchdog '{"stop": true}'`
- Enable magic close: `ubus call system watchdog '{"magicclose": true}'`
- This allows the LWM2M client to take control

### System Requirements

- Linux kernel with watchdog support
- `/dev/watchdog` device available
- Appropriate permissions to access watchdog device
- Optional: ubus for procd integration

## Resource Details

### Mandatory Resources

#### Resource 0: Watchdog Enable
- **Type**: Boolean
- **Operations**: Read/Write
- **Description**: Enable or disable the hardware watchdog
- **Default**: false (disabled for safety)

#### Resource 1: Timeout
- **Type**: Integer
- **Operations**: Read/Write
- **Units**: seconds
- **Range**: 1-300
- **Description**: Watchdog timeout period. System reboots if not reset within this time.
- **Default**: 30 seconds

#### Resource 2: Pet Interval
- **Type**: Integer
- **Operations**: Read/Write
- **Units**: seconds
- **Range**: 1-60
- **Description**: How often the watchdog is automatically reset
- **Default**: 5 seconds
- **Note**: Should be less than timeout value

#### Resource 3: Status
- **Type**: Integer
- **Operations**: Read
- **Range**: 0-3
- **Description**: Current watchdog status
  - 0: Inactive (disabled)
  - 1: Active (running normally)
  - 2: Triggered (reboot in progress)
  - 3: Error (device unavailable)

### Optional Resources

#### Resource 4: Last Pet Time
- **Type**: Time
- **Operations**: Read
- **Description**: Unix timestamp of last successful watchdog reset

#### Resource 5: Magic Close Enable
- **Type**: Boolean
- **Operations**: Read/Write
- **Description**: Enable magic close feature (write 'V' before closing)
- **Default**: true (recommended for OpenWRT)

#### Resource 6: Pet Watchdog
- **Type**: Execute
- **Operations**: Execute
- **Description**: Manually reset the watchdog timer

#### Resource 7: Reset Watchdog
- **Type**: Execute
- **Operations**: Execute
- **Description**: Reset watchdog configuration to defaults and restart

#### Resource 8: Pet Count
- **Type**: Integer
- **Operations**: Read
- **Description**: Total successful watchdog resets since boot

#### Resource 9: Trigger Count
- **Type**: Integer
- **Operations**: Read
- **Description**: Number of watchdog-triggered reboots (persistent across reboots)

#### Resource 10: Device Path
- **Type**: String
- **Operations**: Read
- **Description**: File system path to watchdog device
- **Default**: "/dev/watchdog"

#### Resource 11: Hardware Timeout
- **Type**: Integer
- **Operations**: Read
- **Units**: seconds
- **Description**: Actual hardware timeout value (may differ from requested due to hardware limitations)

#### Resource 12: Boot Count
- **Type**: Integer
- **Operations**: Read
- **Description**: Total system boots detected

## Usage Examples

### Example 1: Enable Watchdog via LWM2M Server

Using CoAP/LWM2M commands:

```
WRITE /34607/0/0 true           # Enable watchdog
WRITE /34607/0/1 60             # Set timeout to 60 seconds
WRITE /34607/0/2 10             # Set interval to 10 seconds
```

### Example 2: Monitor Watchdog Status

```
READ /34607/0/3                 # Get status
READ /34607/0/8                 # Get pet count
READ /34607/0/9                 # Get trigger count
READ /34607/0/12                # Get boot count
```

### Example 3: Manual Pet

```
EXECUTE /34607/0/6              # Manually pet the watchdog
```

### Example 4: Reset to Defaults

```
EXECUTE /34607/0/7              # Reset configuration
```

### Example 5: Programmatic Usage in C++

```cpp
#include "WppClient.h"
#include "o_34607_hardware_watchdog/HardwareWatchdog.h"

// Initialize watchdog
hardwareWatchdogInit(client);

// Get watchdog instance
HardwareWatchdog* wd = HardwareWatchdog::instance(client);

// Enable with custom settings
wd->resource(HardwareWatchdog::TIMEOUT_1)->set<INT_T>(45);
wd->resource(HardwareWatchdog::PET_INTERVAL_2)->set<INT_T>(8);
wd->resource(HardwareWatchdog::WATCHDOG_ENABLE_0)->set<BOOL_T>(true);
wd->startWatchdog();

// Manually pet
wd->petWatchdog();

// Check status
INT_T status = wd->resource(HardwareWatchdog::STATUS_3)->get<INT_T>();
INT_T petCount = wd->resource(HardwareWatchdog::PET_COUNT_8)->get<INT_T>();

// Disable
wd->resource(HardwareWatchdog::WATCHDOG_ENABLE_0)->set<BOOL_T>(false);
wd->stopWatchdog();
```

## Configuration

### CMake Configuration

Enable the watchdog object in your CMake configuration:

```cmake
set(OBJ_O_34607_HARDWARE_WATCHDOG ON)
```

Or use object_maker to enable all optional resources:

```cmake
set(RES_34607_4 ON)   # Last Pet Time
set(RES_34607_5 ON)   # Magic Close Enable
set(RES_34607_6 ON)   # Pet Watchdog Execute
set(RES_34607_7 ON)   # Reset Watchdog Execute
set(RES_34607_8 ON)   # Pet Count
set(RES_34607_9 ON)   # Trigger Count
set(RES_34607_10 ON)  # Device Path
set(RES_34607_11 ON)  # Hardware Timeout
set(RES_34607_12 ON)  # Boot Count
```

### Runtime Configuration Files

The watchdog uses these files for persistent storage:

- `/tmp/lwm2m_wd_triggers` - Watchdog trigger count
- `/tmp/lwm2m_wd_boots` - Boot count

**Note**: These are in `/tmp` by default. For persistence across reboots, modify the paths in HardwareWatchdog.h to use a permanent storage location.

### OpenWRT Configuration

To use this watchdog object, you may need to stop procd's watchdog:

```bash
# Stop procd watchdog
ubus call system watchdog '{"stop": true}'

# Enable magic close (allows proper shutdown)
ubus call system watchdog '{"magicclose": true}'

# Verify watchdog status
ubus call system watchdog
```

## Implementation Details

### Thread Safety

The implementation uses:
- `std::mutex` for thread-safe access to watchdog device
- `std::atomic<bool>` for thread control
- Background thread for automatic petting

### Hardware Communication

```cpp
// Open device
int fd = open("/dev/watchdog", O_WRONLY);

// Set timeout via ioctl
ioctl(fd, WDIOC_SETTIMEOUT, &timeout);

// Get timeout via ioctl
ioctl(fd, WDIOC_GETTIMEOUT, &timeout);

// Pet watchdog (any write)
write(fd, &dummy, 1);

// Magic close (write 'V' before close)
char v = 'V';
write(fd, &v, 1);
close(fd);
```

### Automatic Petting

A background thread runs when the watchdog is enabled:

1. Sleep for `Pet Interval` seconds
2. Write to `/dev/watchdog`
3. Update `Last Pet Time` and `Pet Count`
4. Repeat until disabled

## Troubleshooting

### Watchdog Won't Start

**Symptom**: Status remains at 0 (Inactive) or changes to 3 (Error)

**Possible Causes**:
1. `/dev/watchdog` doesn't exist
   - Check: `ls -l /dev/watchdog`
   - Solution: Load watchdog kernel module

2. Permission denied
   - Check: `ls -l /dev/watchdog`
   - Solution: Run as root or add user to appropriate group

3. Device busy (procd is using it)
   - Check: `ubus call system watchdog`
   - Solution: Stop procd's watchdog management

### Unexpected Reboots

**Symptom**: System reboots without warning

**Possible Causes**:
1. Pet interval ≥ timeout
   - Check resource values
   - Solution: Ensure interval < timeout

2. Application hang
   - Check: Pet count not incrementing
   - Solution: Debug application hang

3. Thread crash
   - Check logs for errors
   - Solution: Review error messages

### Magic Close Not Working

**Symptom**: System reboots when LWM2M client stops

**Possible Causes**:
1. Magic close disabled
   - Check Resource 5
   - Solution: Enable magic close

2. Kernel doesn't support magic close
   - Check kernel config: `CONFIG_WATCHDOG_NOWAYOUT`
   - Solution: Rebuild kernel without NOWAYOUT

## Advanced Topics

### Integration with System Health Monitoring

```cpp
// In your application health check
if (systemHealthy()) {
    // Allow normal petting
} else {
    // Disable watchdog to prevent reboot during maintenance
    watchdog->stopWatchdog();
    performMaintenance();
    watchdog->startWatchdog();
}
```

### Persistent Trigger Counting

To track watchdog triggers across reboots persistently:

1. Modify `TRIGGER_COUNT_FILE` to point to non-volatile storage:
   ```cpp
   static constexpr const char* TRIGGER_COUNT_FILE = "/etc/lwm2m_wd_triggers";
   ```

2. Increment trigger count in early boot script:
   ```bash
   #!/bin/sh
   # /etc/init.d/watchdog-check

   if [ -f /proc/sys/kernel/watchdog_triggered ]; then
       COUNT=$(cat /etc/lwm2m_wd_triggers || echo 0)
       echo $((COUNT + 1)) > /etc/lwm2m_wd_triggers
   fi
   ```

### Custom Device Paths

For systems with multiple watchdog devices:

```cpp
// Before starting, set custom path
watchdog->resource(HardwareWatchdog::DEVICE_PATH_10)->set<STRING_T>("/dev/watchdog1");
// Note: Current implementation uses m_devicePath internally
// This would require modifying openWatchdogDevice() to read from the resource
```

## References

- [Linux Watchdog Documentation](https://www.kernel.org/doc/Documentation/watchdog/watchdog-api.txt)
- [OpenWRT Watchdog](https://openwrt.org/docs/techref/hardware/port.watchdog)
- [OMA LWM2M Specification](http://www.openmobilealliance.org/wp/OMNA/LwM2M/LwM2MRegistry.html)
- [Walt Technologies](https://walt.tech)

## License

BSD 3-Clause License - See source files for details

## Support

For issues or questions:
- GitHub Issues: [Friendly-LwM2M-Client](https://github.com/Friendly-Technologies/Friendly-LwM2M-Client)
- LWM2M Registry: Objects 34600-34649 reserved for Walt Technologies

## Version History

- **v1.0** (2025-11-01): Initial implementation
  - Full OpenWRT integration
  - All 13 resources implemented
  - Thread-safe operation
  - Persistent trigger counting
  - Magic close support
