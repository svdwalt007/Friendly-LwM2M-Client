# Serial Number Option - Quick Reference

## Command-Line Option

```bash
-x, --serial=NUMBER          Device serial number
```

## Default Value

```
OPENWRT-ONE-001
```

## Usage Examples

```bash
# Use default serial number
./WppExample

# Set custom serial number (short option)
./WppExample -x "MY-DEVICE-123"

# Set custom serial number (long option)
./WppExample --serial="ROUTER-2024-001"

# Combined with other options
./WppExample -n mydevice -x "DEV-001" -u coap://server:5683 -v
```

## LwM2M Resource Path

```
/3/0/2
```

- **Object:** 3 (Device)
- **Instance:** 0
- **Resource:** 2 (Serial Number)

## Files Modified

| File | Change |
|------|--------|
| `cli_options.h` | Added `-x` option, `serial_number` field |
| `objects.h` | Updated `deviceInit()` signature |
| `objects.cpp` | Use `options.serial_number` instead of hardcoded value |
| `main.cpp` | Pass options to `deviceInit()`, show in config summary |

## Code Snippet

```cpp
// In cli_options.h
std::string serial_number;  // Default: "OPENWRT-ONE-001"

// In objects.cpp
void deviceInit(WppClient &client, const CliOptions& options) {
    // ...
    device->set<STRING_T>(Device::SERIAL_NUMBER_2, options.serial_number);
}

// In main.cpp
deviceInit(*client, options);
```

## Verification

```bash
# Build
cd _build && make WppExample

# Test default
./WppExample -v | grep "Serial Number"

# Test custom
./WppExample -x "TEST-123" -v | grep "Serial Number"

# Check help
./WppExample --help | grep -A1 "Device Options"
```

## Expected Output

```
Serial Number:    OPENWRT-ONE-001     (with default)
Serial Number:    TEST-123            (with -x "TEST-123")
```
