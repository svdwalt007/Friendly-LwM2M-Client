# Troubleshooting Guide

Common issues and solutions for the Friendly LwM2M Client.

---

## Build Issues

### CMake Configuration Fails

**Symptom:**
```
CMake Error: Could not find CMAKE_ROOT
```

**Solution:**
```bash
# Install or update CMake
sudo apt-get update
sudo apt-get install cmake

# Verify version (need 3.10+)
cmake --version
```

---

### Compiler Not Found

**Symptom:**
```
CMake Error: CMAKE_C_COMPILER not set
```

**Solution:**
```bash
# Install build tools
sudo apt-get install build-essential

# Or specify compiler explicitly
cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
```

---

### Missing Dependencies

**Symptom:**
```
fatal error: pthread.h: No such file or directory
```

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install libpthread-stubs0-dev

# Fedora/RHEL
sudo dnf install glibc-devel
```

---

## Runtime Issues

### Client Won't Connect to Server

**Symptom:**
Client starts but doesn't register with LwM2M server.

**Debugging Steps:**

1. **Check server URI**
   ```cpp
   // In examples/objects.cpp
   security->set<STRING_T>(Lwm2mSecurity::SERVER_URI_0,
                           "coap://correct-server.com:5683");
   ```

2. **Verify network connectivity**
   ```bash
   ping server.example.com
   nc -zvu server.example.com 5683  # UDP port check
   ```

3. **Enable debug logging**
   ```cmake
   # In wpp_config.cmake
   set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=0)
   ```

4. **Check firewall**
   ```bash
   sudo ufw allow 5683/udp
   # or
   sudo iptables -A INPUT -p udp --dport 5683 -j ACCEPT
   ```

5. **Test with Leshan demo server**
   ```cpp
   security->set<STRING_T>(Lwm2mSecurity::SERVER_URI_0,
                           "coap://leshan.eclipseprojects.io:5683");
   ```

---

### Registration Fails with 4.00 Bad Request

**Symptom:**
```
[ERROR] Registration failed: 4.00 Bad Request
```

**Causes & Solutions:**

1. **Mismatched Short Server ID**
   ```cpp
   // These must match!
   server->set<INT_T>(Lwm2mServer::SHORT_SERVER_ID_0, 123);
   security->set<INT_T>(Lwm2mSecurity::SHORT_SERVER_ID_10, 123);
   ```

2. **Invalid Endpoint Name**
   ```cpp
   // Use valid characters only (alphanumeric, -, _)
   const char* ENDPOINT_NAME = "device-001";  // Good
   // Not: "device@001" or "device 001"
   ```

3. **Bootstrap vs Normal Server Confusion**
   ```cpp
   // For normal server (not bootstrap)
   security->set<BOOL_T>(Lwm2mSecurity::BOOTSTRAP_SERVER_1, false);
   ```

---

### Location Object Shows 0.0, 0.0

**Symptom:**
Location resources show default values.

**Debugging Steps:**

1. **Check for OpenWRT build flag**
   ```bash
   # Rebuild with:
   cmake -DOPENWRT_BUILD=ON ..
   ```

2. **Verify GPS sources**
   ```bash
   # Check Starlink
   ping -c 1 192.168.100.1

   # Check gpsd
   ps | grep gpsd
   gpspipe -w -n 5

   # Check UCI
   uci show lwm2m.location
   ```

3. **Enable debug logging**
   ```cpp
   // Look for in logs:
   // "Location initialized from OpenWRT: lat=X, lon=Y"
   // or
   // "No location source available"
   ```

4. **Manually set location for testing**
   ```bash
   uci set lwm2m.location=location
   uci set lwm2m.location.latitude='40.7128'
   uci set lwm2m.location.longitude='-74.0060'
   uci commit lwm2m
   ```

---

### Starlink GPS Not Working

**Symptom:**
Starlink object shows GPS_READY=false

**Solutions:**

1. **Check dish connectivity**
   ```bash
   ping 192.168.100.1
   curl http://192.168.100.1/  # Should get response
   ```

2. **Verify dish has GPS lock**
   - Move dish outdoors
   - Clear sky view required
   - Wait 5-10 minutes for GPS lock

3. **Check grpcurl installation** (if using gRPC method)
   ```bash
   which grpcurl
   # Install if missing:
   go install github.com/fullstorydev/grpcurl/cmd/grpcurl@latest
   ```

4. **Test gRPC manually**
   ```bash
   grpcurl -plaintext -d '{"get_status":{}}' \
       192.168.100.1:9200 \
       SpaceX.API.Device.Device/Handle | grep -i gps
   ```

---

## OpenWRT-Specific Issues

### Permission Denied Errors

**Symptom:**
```
Failed to open /sys/class/net/eth0/carrier: Permission denied
```

**Solutions:**

1. **Run as root (temporarily for testing)**
   ```bash
   sudo /usr/bin/lwm2m_client
   ```

2. **Set proper capabilities**
   ```bash
   setcap cap_net_admin,cap_net_raw=+ep /usr/bin/lwm2m_client
   ```

3. **Fix file permissions**
   ```bash
   chmod +r /sys/class/net/*/carrier
   chmod +r /proc/meminfo
   ```

---

### UCI Commands Not Found

**Symptom:**
```
sh: uci: not found
```

**Solution:**
```bash
# Install UCI
opkg update
opkg install uci

# Verify
which uci
```

---

### Memory Issues on Embedded Systems

**Symptom:**
```
terminate called after throwing an instance of 'std::bad_alloc'
```

**Solutions:**

1. **Reduce enabled objects**
   ```cmake
   # Disable unused objects in wpp_config.cmake
   # set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_3339_AUDIO_CLIP)
   ```

2. **Increase available memory**
   ```bash
   # On OpenWRT, reduce other services
   /etc/init.d/uhttpd stop
   /etc/init.d/dnsmasq stop  # Only if safe
   ```

3. **Check memory usage**
   ```bash
   free -m
   top -b -n 1
   ```

4. **Use Release build**
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ..
   ```

---

## Debug Techniques

### Enable Verbose Logging

```cmake
# In wpp_config.cmake
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=0)  # Debug
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_WITH_LOGS)   # Wakaama logs
```

### Log Output Locations

```bash
# Standard output (when run in foreground)
./lwm2m_client 2>&1 | tee lwm2m.log

# System log (when run as service)
logread | grep lwm2m

# Or
tail -f /var/log/messages | grep lwm2m
```

### Network Traffic Analysis

```bash
# Capture CoAP traffic
tcpdump -i any -w lwm2m.pcap port 5683

# View with Wireshark
wireshark lwm2m.pcap

# Or use tshark
tshark -r lwm2m.pcap -Y coap
```

### Memory Leak Detection

```bash
# Use valgrind
valgrind --leak-check=full --show-leak-kinds=all ./lwm2m_client

# Monitor memory over time
while true; do
    ps aux | grep lwm2m_client | grep -v grep
    sleep 60
done
```

### GDB Debugging

```bash
# Build with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run with GDB
gdb ./lwm2m_client

# GDB commands:
(gdb) run
(gdb) bt        # Backtrace on crash
(gdb) break Location::updateLocation
(gdb) continue
(gdb) print lat
```

---

## Performance Issues

### High CPU Usage

**Debugging:**
```bash
# Identify hot spots
perf record -g ./lwm2m_client
perf report

# Or use top
top -H -p $(pidof lwm2m_client)
```

**Common Causes:**

1. **Too frequent updates**
   ```cpp
   // Increase interval
   WppTaskQueue::addTask(60, [...]);  // Was: 1 second
   ```

2. **Polling instead of events**
   ```cpp
   // Bad: Continuous polling
   while(true) {
       checkGPS();
       usleep(100000);
   }

   // Good: Use task queue
   WppTaskQueue::addTask(60, [](WppClient &client, void *ctx) {
       checkGPS();
       return false;
   });
   ```

3. **Excessive logging**
   ```cmake
   # Reduce log level
   set(WPP_DEFINITIONS ${WPP_DEFINITIONS} WPP_LOGS_LEVEL=2)  # Warning
   ```

---

### Network Issues

**High Bandwidth Usage:**

1. **Check observation settings**
   ```cpp
   // Increase MIN_PERIOD to reduce updates
   server->set<INT_T>(Lwm2mServer::MIN_PERIOD_2, 60);  // Was: 10
   ```

2. **Use SENML format** (more efficient)
   ```cmake
   set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_SUPPORT_SENML_CBOR)
   ```

3. **Cancel unnecessary observations**
   ```cpp
   // On server, cancel observes for unused resources
   ```

**Packet Loss:**

1. **Check MTU settings**
   ```bash
   ip link show eth0
   # Ensure MTU >= 1280 for IPv6
   ```

2. **Adjust CoAP block size**
   ```cmake
   # Reduce if needed
   set(WPP_DEFINITIONS ${WPP_DEFINITIONS} LWM2M_COAP_DEFAULT_BLOCK_SIZE=512)
   ```

---

## Common Error Messages

### "Failed to create instance"

**Cause:** Object not registered before createInst()

**Solution:**
```cpp
// Register object BEFORE creating instance
client.registry().registerObj(Location::object(client));
Location::createInst(client);  // Now works
```

---

### "Resource not found"

**Cause:** Resource disabled in config or wrong ID

**Solution:**
```cpp
// Check LocationConfig.h
#define RES_6_2 1  // Must be 1 to enable

// Use correct resource ID
#if RES_6_2
float alt = loc->get<FLOAT_T>(Location::ALTITUDE_2);
#endif
```

---

### "Observe failed"

**Cause:** Server trying to observe non-readable resource

**Solution:**
```cpp
// Ensure resource has READ permission
{RESOURCE_ID, ItemOp(ItemOp::READ), ...}
```

---

## Getting Help

### Information to Provide

When reporting issues, include:

1. **Build configuration**
   ```bash
   cmake --version
   gcc --version
   uname -a
   ```

2. **Enabled objects**
   ```bash
   grep "set(WPP_DEFINITIONS" wpp/configs/wpp_config.cmake
   ```

3. **Log output**
   ```bash
   # With debug logging enabled
   ./lwm2m_client 2>&1 | tee full-log.txt
   ```

4. **Network capture** (if network issue)
   ```bash
   tcpdump -i any -w issue.pcap port 5683
   ```

5. **Steps to reproduce**
   - Exact commands run
   - Expected vs actual behavior
   - Environment details

### Resources

- [Project Issues](https://github.com/your-org/Friendly-LwM2M-Client/issues)
- [LwM2M Specification](https://www.openmobilealliance.org/release/LightweightM2M/)
- [Wakaama Wiki](https://github.com/eclipse/wakaama/wiki)
- [OpenWRT Forums](https://forum.openwrt.org/)

---

## Quick Fixes Checklist

- [ ] Build with latest code from main branch
- [ ] Enable debug logging (WPP_LOGS_LEVEL=0)
- [ ] Check all server/security configuration matches
- [ ] Verify network connectivity to server
- [ ] Test with Leshan demo server
- [ ] Check firewall/security groups
- [ ] Verify all required objects are enabled
- [ ] Run on target platform (OpenWRT if applicable)
- [ ] Check system resources (memory, CPU)
- [ ] Review logs for specific error messages
