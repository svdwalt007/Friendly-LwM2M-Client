# Hardware Watchdog Object (34607)

## Quick Start

This LWM2M object provides hardware watchdog timer management for embedded Linux systems, particularly OpenWRT.

### Object ID

**34607** (Walt Technologies custom object range: 34600-34649)

### Enable in Build

```cmake
set(OBJ_O_34607_HARDWARE_WATCHDOG ON)
```

### Initialize in Application

```cpp
#include "objects.h"

// In main() or initialization function
hardwareWatchdogInit(client);
```

### Basic Usage

```cpp
// Get watchdog instance
auto* wd = HardwareWatchdog::instance(client);

// Configure
wd->resource(HardwareWatchdog::TIMEOUT_1)->set<INT_T>(30);        // 30 second timeout
wd->resource(HardwareWatchdog::PET_INTERVAL_2)->set<INT_T>(5);    // Pet every 5 seconds

// Enable
wd->resource(HardwareWatchdog::WATCHDOG_ENABLE_0)->set<BOOL_T>(true);
wd->startWatchdog();
```

### LWM2M Server Commands

```
# Enable watchdog
WRITE /34607/0/0 true

# Set 60 second timeout
WRITE /34607/0/1 60

# Set 10 second pet interval
WRITE /34607/0/2 10

# Check status (0=inactive, 1=active, 2=triggered, 3=error)
READ /34607/0/3

# Manually pet
EXECUTE /34607/0/6

# View statistics
READ /34607/0/8    # Pet count
READ /34607/0/9    # Trigger count
READ /34607/0/12   # Boot count
```

## Resources

| ID | Name | Type | Ops | Default | Description |
|----|------|------|-----|---------|-------------|
| 0 | Watchdog Enable | Bool | R/W | false | Enable/disable watchdog |
| 1 | Timeout | Int | R/W | 30 | Timeout in seconds (1-300) |
| 2 | Pet Interval | Int | R/W | 5 | Pet interval in seconds (1-60) |
| 3 | Status | Int | R | 0 | 0=Inactive, 1=Active, 2=Triggered, 3=Error |
| 4* | Last Pet Time | Time | R | 0 | Unix timestamp of last pet |
| 5* | Magic Close Enable | Bool | R/W | true | Enable magic close on device |
| 6* | Pet Watchdog | Exec | E | - | Manually pet watchdog |
| 7* | Reset Watchdog | Exec | E | - | Reset to defaults |
| 8* | Pet Count | Int | R | 0 | Number of pets since boot |
| 9* | Trigger Count | Int | R | 0 | Number of watchdog triggers |
| 10* | Device Path | String | R | "/dev/watchdog" | Watchdog device path |
| 11* | Hardware Timeout | Int | R | 0 | Actual HW timeout value |
| 12* | Boot Count | Int | R | 0 | Total system boots |

\* = Optional resource (enable via CMake RES_34607_X)

## Implementation Details

### Threading

- Uses background thread for automatic petting
- Thread-safe with mutex protection
- Graceful shutdown on disable

### OpenWRT Integration

- Directly accesses `/dev/watchdog`
- Supports WDIOC_SETTIMEOUT/WDIOC_GETTIMEOUT ioctls
- Magic close support (write 'V' before close)
- Compatible with procd watchdog

### Files Used

- `/tmp/lwm2m_wd_triggers` - Persistent trigger count
- `/tmp/lwm2m_wd_boots` - Boot count

Change paths in `HardwareWatchdog.h` for permanent storage.

## OpenWRT Setup

If procd is managing the watchdog, stop it first:

```bash
# Stop procd watchdog
ubus call system watchdog '{"stop": true}'

# Enable magic close
ubus call system watchdog '{"magicclose": true}'
```

## Safety

- **Default: Disabled** - Watchdog starts disabled for safety
- **Enable remotely** via LWM2M server or programmatically
- **Magic close enabled** by default to prevent unwanted reboots
- **Validation** - Timeout and interval ranges enforced

## Troubleshooting

**Can't open /dev/watchdog**
- Check device exists: `ls -l /dev/watchdog`
- Check permissions (may need root)
- Check if procd is using it

**System reboots unexpectedly**
- Ensure interval < timeout
- Check status resource for errors
- Review pet count (should increment regularly)

**Watchdog won't start**
- Check Status resource (3 = error)
- Verify /dev/watchdog exists
- Check logs for error messages

## Documentation

See `/docs/WATCHDOG.md` for complete documentation including:
- Detailed resource descriptions
- OpenWRT integration guide
- Usage examples
- Advanced topics

## License

BSD 3-Clause License

## Object Definition

Generated from `/utils/object_maker/34607.xml`

URN: `urn:oma:lwm2m:x:34607`
