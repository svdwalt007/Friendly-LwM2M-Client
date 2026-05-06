# LwM2M Object Configuration Loader

This implementation adds the ability to populate LwM2M objects with realistic dummy data from a JSON configuration file at startup.

## Files Added

### 1. `examples/lwm2m_objects_config.json`
Comprehensive JSON configuration file containing realistic dummy data for all available LwM2M objects:
- **Mandatory Objects**: Security (0), Server (1), Device (3)
- **Optional Standard Objects**: Connectivity Monitoring (4), Firmware Update (5), Location (6), WLAN (12), Bearer Selection (13), Audio Clip (3339), Hardware Watchdog (10519), Starlink Terminal (10512), Mikrobus (10520)
- **Walt Technologies Custom Objects**: Router Management (10513), Ethernet Interface (10514), GPIO Control (10515), USB Management (10516), Storage Management (10517), System Monitor (10518), Firewall Config (10521), PoE Management (10522), Network Diagnostics (10523), VLAN Configuration (10524)

### 2. `examples/ConfigLoader.h`
Header file defining the ConfigLoader class that handles:
- Loading JSON configuration from file
- Registering LwM2M objects to be configured
- Applying configuration data to object instances

### 3. `examples/ConfigLoader.cpp`
Implementation file containing:
- **SimpleJsonParser**: Lightweight JSON parser (no external dependencies)
- **ConfigLoader**: Main configuration loading logic
- Resource value type handling (string, int, float, bool)
- Instance and resource population logic

## How It Works

1. **Initialization**: After all LwM2M objects are initialized in `main.cpp`, the ConfigLoader is created
2. **Registration**: Each initialized object is registered with the ConfigLoader using its Object ID
3. **Loading**: The JSON configuration file is loaded and parsed
4. **Application**: For each object in the JSON:
   - Find the corresponding registered object
   - For each instance configuration:
     - Locate the instance in the object
     - For each resource in the configuration:
       - Set the resource value based on type (string, int, float, bool)

## Building

The ConfigLoader has been integrated into the CMake build system:

```bash
cd build
cmake ..
make friendly_example_client
```

Or on Windows:
```cmd
cd build
cmake ..
cmake --build . --target friendly_example_client
```

## Configuration File Format

The JSON file follows this structure:

```json
{
  "lwm2m_objects": {
    "<object_id>": {
      "name": "<object_name>",
      "instances": [
        {
          "id": <instance_id>,
          "resources": {
            "<resource_id>": <value>
          }
        }
      ]
    }
  }
}
```

### Example

```json
{
  "lwm2m_objects": {
    "3": {
      "name": "Device",
      "instances": [
        {
          "id": 0,
          "resources": {
            "0": "Walt Technologies",
            "1": "Friendly Router Model X1",
            "2": "SN-FRIENDLY-2024-001",
            "3": "1.2.0"
          }
        }
      ]
    }
  }
}
```

## Supported Resource Types

- **String**: Text values (e.g., manufacturer name, serial number)
- **Integer**: Whole numbers (e.g., battery level, error codes)
- **Float**: Decimal numbers (e.g., temperature, voltage)
- **Boolean**: true/false values (e.g., enabled/disabled flags)

## Usage

### Default Configuration

The client automatically attempts to load `lwm2m_objects_config.json` from the current working directory at startup. If the file is not found or contains errors, the client will use default values and continue execution.

### Custom Configuration File

To use a different configuration file, modify the path in `main.cpp`:

```cpp
if (configLoader.loadFromFile("path/to/your/config.json")) {
    // ...
}
```

### Running the Client

```bash
cd build/examples
./friendly_example_client
```

The console output will show:
1. Object initialization messages
2. Configuration loading status
3. Number of objects configured
4. Number of resources set per instance

Example output:
```
---- Loading configuration from JSON ----
Loaded configuration for 15 objects
Configuring Object 3 (Device)
  Instance 0: 21 resources configured
Configuring Object 4 (Connectivity Monitoring)
  Instance 0: 13 resources configured
Configuration applied: 15 objects configured, 0 failed
Configuration applied successfully
```

## Extending the Configuration

### Adding New Objects

1. Add object definition to `lwm2m_objects_config.json`:
```json
"<new_object_id>": {
  "name": "New Object Name",
  "instances": [
    {
      "id": 0,
      "resources": {
        "0": "resource_value"
      }
    }
  ]
}
```

2. Ensure the object is initialized in `main.cpp` and registered with ConfigLoader

### Adding New Resources

Simply add new resource entries to the "resources" object in the JSON:

```json
"resources": {
  "0": "existing_resource",
  "1": "new_resource",
  "2": 42
}
```

## Error Handling

The ConfigLoader includes robust error handling:
- Missing configuration file: Warning logged, client continues with defaults
- JSON parse errors: Error logged with details
- Missing objects: Warning logged, other objects still configured
- Missing instances: Warning logged for specific instance
- Missing/invalid resources: Silently skipped (expected for unimplemented resources)

## Notes

### Resource Availability
Not all resources defined in the JSON may be implemented in every object. The ConfigLoader silently skips resources that don't exist in the object implementation. This is expected behavior and allows the JSON to define a complete resource set even if some resources are not yet implemented.

### Complex Resource Types
Currently, the ConfigLoader handles simple resource types. Complex types like:
- Multiple resource instances (arrays)
- Object links
- Opaque data

These are stored in the JSON but require special handling in future implementations.

### Performance
The SimpleJsonParser is a lightweight implementation designed for startup configuration. It parses the entire JSON into memory structures before applying values. For very large configuration files, consider using an optimized JSON library.

## Troubleshooting

### Configuration not applied
1. Verify the JSON file is in the correct location (working directory)
2. Check JSON syntax is valid
3. Verify object IDs match between JSON and registered objects
4. Check console output for specific error messages

### Some resources not set
This is normal if:
- Resources are not implemented in the object
- Resource is read-only
- Resource type doesn't match expected type

Check the console output to see which resources were successfully configured per instance.

### Build errors
Ensure:
1. `ConfigLoader.cpp` is added to SOURCES in `examples/CMakeLists.txt`
2. `ConfigLoader.h` is in the include path
3. All required WPP headers are available

## Future Enhancements

Potential improvements for future versions:
1. Support for multiple resource instances
2. Object link resource type handling
3. Opaque data (binary) resource support
4. JSON schema validation
5. Configuration file encryption
6. Hot-reload configuration without restart
7. Configuration export (dump current values to JSON)
8. Environment variable substitution in JSON
9. Include/merge multiple JSON files
