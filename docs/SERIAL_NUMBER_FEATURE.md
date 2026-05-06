# Serial Number Command-Line Option

This document describes the implementation of the serial number command-line option for the WppExample client.

## Overview

The serial number can now be configured via command-line argument using the `-x` or `--serial` option. This value is used to populate the LwM2M Device object's Serial Number resource (Object ID 3, Resource ID 2).

## Changes Made

### 1. CLI Options Structure (`examples/cli_options.h`)

**Added new field:**
```cpp
// Device configuration
std::string serial_number;      // Device serial number (Object 3, Resource 2)
```

**Default value in constructor:**
```cpp
, serial_number("OPENWRT-ONE-001")
```

**Added command-line option:**
- Short option: `-x`
- Long option: `--serial=NUMBER`

**Updated help text:**
```
Device Options:
  -x, --serial=NUMBER          Device serial number (default: OPENWRT-ONE-001)
```

**Added to long_options array:**
```cpp
{"serial",       required_argument, 0, 'x'},
```

**Added to getopt_long string:**
```cpp
"n:u:p:l:46bBs:i:k:x:vqdc:hV"
                        ^^
```

**Added parsing case:**
```cpp
case 'x':
    options.serial_number = optarg;
    break;
```

**Added to verbose output:**
```cpp
std::cout << "  Serial Number:  " << options.serial_number << "\n"
```

### 2. Device Initialization (`examples/objects.h`)

**Updated function signature:**
```cpp
// Before:
void deviceInit(WppClient &client);

// After:
void deviceInit(WppClient &client, const CliOptions& options);
```

### 3. Device Initialization Implementation (`examples/objects.cpp`)

**Updated function implementation:**
```cpp
// Before:
void deviceInit(WppClient &client) {
    // ...
    device->set<STRING_T>(Device::SERIAL_NUMBER_2, "OPENWRT-ONE-001");
}

// After:
void deviceInit(WppClient &client, const CliOptions& options) {
    // ...
    // CLI: Use serial number from command-line options
    device->set<STRING_T>(Device::SERIAL_NUMBER_2, options.serial_number);
}
```

### 4. Main Application (`examples/main.cpp`)

**Updated deviceInit call:**
```cpp
// Before:
deviceInit(*client);

// After:
// CLI: Pass options to deviceInit
deviceInit(*client, options);
```

**Added to configuration summary:**
```cpp
cout << "Serial Number:    " << options.serial_number << endl;
```

## Usage Examples

### Using Default Serial Number

```bash
./WppExample
```

The default serial number `OPENWRT-ONE-001` will be used.

### Using Custom Serial Number (Short Option)

```bash
./WppExample -x "ABC-12345-XYZ"
```

### Using Custom Serial Number (Long Option)

```bash
./WppExample --serial="MY-DEVICE-001"
```

### Complete Configuration Example

```bash
./WppExample \
    -n mydevice \
    -x "ROUTER-2024-001" \
    -u coap://server.example.com:5683 \
    -p 56830 \
    -l 60 \
    -s none \
    -v
```

This will:
- Set endpoint name to `mydevice`
- Set serial number to `ROUTER-2024-001`
- Connect to `coap://server.example.com:5683`
- Use local port `56830`
- Set registration lifetime to 60 seconds
- Use no security
- Enable verbose output

## LwM2M Object Details

### Device Object (Object ID: 3)

The serial number is stored in:
- **Object ID:** 3 (Device)
- **Instance ID:** 0 (single instance)
- **Resource ID:** 2 (Serial Number)
- **Type:** String
- **Access:** Read-only
- **Mandatory:** Yes (per LwM2M specification)

### Resource Path

The full resource path is: `/3/0/2`

### Server Access

When a LwM2M server performs a Read operation on `/3/0/2`, it will receive the serial number configured via the `-x` option.

Example:
```
Server → Read /3/0/2
Client → 2.05 Content: "ROUTER-2024-001"
```

## Implementation Notes

### Consistency with Other Options

The serial number option follows the same pattern as other CLI options:
1. Defined in `CliOptions` structure
2. Has a sensible default value
3. Can be overridden via command-line
4. Passed to initialization functions via `const CliOptions&`
5. Documented in help text
6. Shown in verbose output

### Default Value

The default serial number `OPENWRT-ONE-001` was chosen to match the previously hardcoded value, ensuring backward compatibility.

### Validation

Currently, no validation is performed on the serial number string. Any string value is accepted. If validation is needed in the future (e.g., format requirements, length limits), it can be added in the parsing section.

Example validation (not implemented):
```cpp
case 'x':
    options.serial_number = optarg;
    if (options.serial_number.empty()) {
        std::cerr << "Error: Serial number cannot be empty" << std::endl;
        return false;
    }
    if (options.serial_number.length() > 64) {
        std::cerr << "Error: Serial number too long (max 64 characters)" << std::endl;
        return false;
    }
    break;
```

### Thread Safety

The serial number is set during initialization before the client gives ownership to the registry, so there are no thread safety concerns.

### Persistence

The serial number is only stored in memory. If persistence is needed (e.g., across reboots), it should be stored in a configuration file and loaded at startup.

## Testing

### Verify Default Serial Number

```bash
# Run the client
./WppExample -v

# Check output for:
Serial Number:    OPENWRT-ONE-001
```

### Verify Custom Serial Number

```bash
# Run with custom serial number
./WppExample -x "TEST-SN-123" -v

# Check output for:
Serial Number:    TEST-SN-123
```

### Verify LwM2M Server Can Read Serial Number

Use a LwM2M server or testing tool to perform a Read operation on `/3/0/2` and verify it returns the configured serial number.

## Future Enhancements

Potential future improvements:

1. **Validation:**
   - Add format validation (e.g., regex pattern)
   - Add length limits
   - Add character set restrictions

2. **Persistence:**
   - Save to configuration file
   - Load from environment variable
   - Support reading from system (e.g., `/proc/cpuinfo` serial)

3. **Auto-generation:**
   - Generate based on MAC address
   - Generate based on device unique ID
   - Generate UUID if not specified

4. **Platform Integration:**
   - Read from OpenWRT UCI config
   - Read from device tree
   - Read from bootloader environment

Example auto-generation:
```cpp
if (options.serial_number.empty()) {
    // Auto-generate from MAC address
    options.serial_number = generateSerialFromMAC();
}
```

## Related Files

- `examples/cli_options.h` - CLI option definitions and parsing
- `examples/objects.h` - Object initialization declarations
- `examples/objects.cpp` - Object initialization implementations
- `examples/main.cpp` - Main application entry point

## LwM2M Specification Reference

Per the OMA LwM2M specification (version 1.1), the Serial Number resource is:
- **Mandatory:** Yes
- **Type:** String
- **Operations:** Read
- **Multiple Instances:** No
- **Description:** "The serial number of the Device"

The implementation fully complies with this specification.

## Backward Compatibility

**Behavior with no `-x` option:**
- Uses default value: `OPENWRT-ONE-001`
- Maintains previous hardcoded behavior
- No breaking changes

**Behavior with `-x` option:**
- Overrides default with user-provided value
- New functionality, fully backward compatible

## Summary

The serial number feature is now fully implemented and integrated into the WppExample client. Users can specify a custom serial number via the `-x` or `--serial` command-line option, which is then properly set in the LwM2M Device object and accessible to LwM2M servers via the standard resource path `/3/0/2`.
