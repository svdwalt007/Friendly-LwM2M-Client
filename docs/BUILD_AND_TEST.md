# Building and Testing the Configuration Loader

## Quick Start

### 1. Build the Project

From the project root directory:

```bash
cd build
cmake ..
make friendly_example_client
```

Or use the build script if available:
```bash
./build.sh
```

### 2. Prepare Configuration File

Copy the configuration file to the build directory:

```bash
cp examples/lwm2m_objects_config.json build/examples/
```

Or create a symbolic link:
```bash
cd build/examples
ln -s ../../examples/lwm2m_objects_config.json .
```

### 3. Run the Client

```bash
cd build/examples
./friendly_example_client
```

## Expected Output

When the client starts with the configuration loader integrated, you should see:

```
---- Creating required components ----
---- Creating WppClient ----
WppClient name: friendly-lwm2m-client

---- Initialization wpp Server ----
---- Initialization wpp Security ----
---- Initialization wpp Device ----
---- Initialization wpp ConnectivityMonitoring ----
---- Initialization wpp Location ----
---- Initialization wpp WLAN Connectivity ----
---- Initialization wpp Bearer Selection ----
---- Initialization wpp AudioClip ----
---- Initialization wpp HardwareWatchdog ----
---- Initialization Walt Technologies StarlinkTerminal ----
---- Initialization Walt Technologies MIKROBUS ----
---- Initialization wpp RouterManagement ----
---- Initialization wpp EthernetInterface ----
---- Initialization wpp GpioControl ----
---- Initialization wpp UsbManagement ----
---- Initialization wpp StorageManagement ----
---- Initialization wpp SystemMonitor ----
---- Initialization wpp FirewallConfig ----
---- Initialization wpp PoeManagement ----

---- Loading configuration from JSON ----
Loaded configuration for 15 objects
Configuring Object 0 (LwM2M Security)
  Instance 0: X resources configured
Configuring Object 1 (LwM2M Server)
  Instance 0: X resources configured
Configuring Object 3 (Device)
  Instance 0: X resources configured
Configuring Object 4 (Connectivity Monitoring)
  Instance 0: X resources configured
... (more objects)
Configuration applied: 15 objects configured, 0 failed
Configuration applied successfully

---- Starting Connection thread ----
```

## Testing the Configuration

### Verify Resources Were Set

You can verify that resources were properly configured by:

1. **Using LwM2M Server**: Connect to a LwM2M server (like Leshan) and observe the resource values
2. **Using Debug Logs**: Enable WPP logging to see resource operations
3. **Using Read Operations**: Server can read resources to verify values

### Example: Test with Leshan

1. Start Leshan server:
```bash
java -jar leshan-server-demo.jar
```

2. Access Leshan web UI at http://localhost:8080

3. Start the client (pointing to Leshan):
```bash
./friendly_example_client --server-uri "coap://localhost:5683" --endpoint-name "test-client"
```

4. In Leshan UI, navigate to the registered client and read resources from various objects:
   - Object 3 (Device): Check Manufacturer, Model Number, Serial Number
   - Object 4 (Connectivity Monitoring): Check Network Bearer, IP Addresses
   - Object 10513 (Router Management): Check Router Name, Firmware Version

## Troubleshooting

### Configuration File Not Found

**Symptom:**
```
Warning: Could not load configuration file, using defaults
```

**Solution:**
- Ensure `lwm2m_objects_config.json` is in the current working directory when running the client
- Use absolute path or copy file to build/examples/

### JSON Parse Error

**Symptom:**
```
Failed to parse JSON configuration
```

**Solution:**
- Validate JSON syntax using online tool (jsonlint.com)
- Check for missing commas, brackets, or quotes
- Ensure proper escaping of special characters

### Some Objects Not Configured

**Symptom:**
```
Object X (ObjectName) not registered
```

**Solution:**
- Verify object is enabled in CMake configuration
- Check that object initialization function was called in main.cpp
- Ensure object was registered with ConfigLoader

### Resources Not Set

**Symptom:**
```
Instance 0: 0 resources configured
```

This can happen if:
- Resources don't exist in the object implementation
- Resource IDs in JSON don't match object definition
- Resource types don't match (e.g., string value for integer resource)

**Solution:**
- Check which resources are actually implemented in the object
- Verify resource IDs match the object definition
- Check resource type compatibility

## Advanced Testing

### Custom Configuration

Create a custom configuration file for testing specific scenarios:

```json
{
  "lwm2m_objects": {
    "3": {
      "name": "Device",
      "instances": [
        {
          "id": 0,
          "resources": {
            "0": "Test Manufacturer",
            "1": "Test Model",
            "2": "TEST-SN-001"
          }
        }
      ]
    }
  }
}
```

Save as `test_config.json` and modify main.cpp to load it.

### Testing Multiple Instances

Some objects support multiple instances (e.g., Ethernet Interface, GPIO Control):

```json
{
  "lwm2m_objects": {
    "10514": {
      "name": "Ethernet Interface",
      "instances": [
        {
          "id": 0,
          "resources": {
            "0": "eth0",
            "1": true,
            "3": "192.168.1.100"
          }
        },
        {
          "id": 1,
          "resources": {
            "0": "eth1",
            "1": false,
            "3": "192.168.2.100"
          }
        }
      ]
    }
  }
}
```

### Testing Resource Updates

After initial configuration, you can test if resources can be updated from the server:

1. Server writes new value to a writable resource
2. Verify the write succeeds
3. Server reads the resource back
4. Verify the new value is returned

## Performance Testing

### Startup Time

Measure the impact of configuration loading on startup time:

```bash
time ./friendly_example_client
```

Compare with and without configuration file present.

### Memory Usage

Monitor memory usage with configuration loaded:

```bash
valgrind --leak-check=full ./friendly_example_client
```

Or use system tools:
```bash
ps aux | grep friendly_example_client
```

## Integration Testing

### Full System Test

1. Build with all objects enabled
2. Populate comprehensive configuration
3. Connect to LwM2M server
4. Execute full test suite:
   - Registration
   - Read all objects
   - Write writable resources
   - Execute operations
   - Observe resources
   - Firmware update (if applicable)

### Automated Testing

Create a test script:

```bash
#!/bin/bash

echo "Starting LwM2M client test..."

# Start client in background
./friendly_example_client &
CLIENT_PID=$!

# Wait for startup
sleep 5

# Check if client is running
if ps -p $CLIENT_PID > /dev/null; then
    echo "Client started successfully"

    # Add more tests here (e.g., using lwm2m-client-test tool)

    # Cleanup
    kill $CLIENT_PID
    echo "Test completed"
else
    echo "Client failed to start"
    exit 1
fi
```

## Debugging

### Enable WPP Logging

To see detailed logs of configuration loading and resource operations:

1. Ensure WPP_ENABLE_LOGS is defined in build
2. Set log level to DEBUG
3. Recompile and run

### GDB Debugging

Debug configuration loading:

```bash
gdb ./friendly_example_client
(gdb) break ConfigLoader::loadFromFile
(gdb) run
(gdb) step
```

### Verbose Output

Modify ConfigLoader.cpp to add more verbose output for debugging:
- Print each JSON key/value pair
- Print each resource set attempt
- Print resource types and values

## Next Steps

After successful testing:

1. **Customize Configuration**: Modify JSON to match your deployment
2. **Add More Objects**: Enable additional objects as needed
3. **Dynamic Updates**: Consider implementing runtime configuration reload
4. **Validation**: Add JSON schema validation
5. **Security**: Consider encrypting sensitive configuration data
6. **Distribution**: Package configuration with firmware/software release
