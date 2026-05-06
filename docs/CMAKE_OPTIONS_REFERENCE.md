# CMake Options Quick Reference

## Quick Commands

### Enable ALL Features
```bash
./configure_full_build.sh        # Linux/macOS
configure_full_build.bat         # Windows
```

### Enable Specific Feature Groups
```bash
# Network Management (Objects 10525-10528, 10535-10537)
cmake .. -DWITH_WAN_FAILOVER=ON -DWITH_MULTIWAN_HEALTH=ON \
         -DWITH_WIFI_CLIENT_MGMT=ON -DWITH_WIFI_CHANNEL_OPT=ON \
         -DWITH_LAN_CONFIG=ON -DWITH_ROUTING_TABLE=ON -DWITH_VPN_CONFIG=ON

# Smart Home (Objects 10529-10534)
cmake .. -DWITH_MATTER_BRIDGE=ON -DWITH_ZIGBEE=ON

# All Walt Technologies Objects (10512-10537)
cmake .. -DWITH_STARLINK=ON -DWITH_WAN_FAILOVER=ON -DWITH_MULTIWAN_HEALTH=ON \
         -DWITH_WIFI_CLIENT_MGMT=ON -DWITH_WIFI_CHANNEL_OPT=ON \
         -DWITH_MATTER_BRIDGE=ON -DWITH_ZIGBEE=ON \
         -DWITH_LAN_CONFIG=ON -DWITH_ROUTING_TABLE=ON -DWITH_VPN_CONFIG=ON
```

## Complete Options List

### Core Features
| Option | Default | Description |
|--------|---------|-------------|
| `DTLS` | ON | DTLS support |
| `MBEDTLS` | ON | Use mbedTLS |
| `TINYDTLS` | OFF | Use TinyDTLS (standalone) |

### DTLS Multi-Provider
| Option | Default | Description |
|--------|---------|-------------|
| `DTLS_TINYDTLS_PROVIDER` | ON | TinyDTLS provider |
| `DTLS_MBEDTLS_PROVIDER` | ON | mbedTLS provider |
| `DTLS_OPENSSL_PROVIDER` | OFF | OpenSSL provider |
| `DTLS_WOLFSSL_PROVIDER` | OFF | wolfSSL provider |
| `DTLS_DEFAULT_PROVIDER` | mbedtls | Default provider |
| `DTLS_STATIC_LINKING` | ON | Static linking |

### Delta FOTA
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_DELTA_FOTA` | ON | Delta firmware updates |
| `WITH_BSDIFF` | ON | Binary diff |
| `WITH_VCDIFF` | ON | Version control diff |
| `WITH_COURGETTE` | ON | Courgette diff |
| `WITH_COMPRESSION` | ON | Compression support |
| `WITH_ROLLBACK` | ON | Rollback protection |

### Transport & AI
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_MQTT_TRANSPORT` | ON | MQTT transport |
| `WITH_EDGE_AI` | ON | Edge AI inference |
| `WITH_TENSORFLOW_LITE` | ON | TensorFlow Lite |
| `WITH_ONNX_RUNTIME` | ON | ONNX Runtime |

### OpenWRT Extended
| Option | Default | LwM2M Objects | Description |
|--------|---------|---------------|-------------|
| `WITH_OPENWRT_EXTENDED` | ON | - | OpenWRT extensions |
| `WITH_LUCI_MODULE` | ON | - | LuCI web interface |
| `WITH_HOTPLUG_HANDLERS` | ON | - | Hotplug handlers |
| `WITH_LAN_CONFIG` | ON | 10535 | LAN configuration |
| `WITH_ROUTING_TABLE` | ON | 10536 | Routing table |
| `WITH_VPN_CONFIG` | ON | 10537 | VPN configuration |
| `WITH_WAN_FAILOVER` | ON | 10525 | WAN failover policy |
| `WITH_MULTIWAN_HEALTH` | ON | 10526 | Multi-WAN health |
| `WITH_WIFI_CLIENT_MGMT` | ON | 10527 | WiFi client mgmt |
| `WITH_WIFI_CHANNEL_OPT` | ON | 10528 | WiFi channel opt |

### Starlink
| Option | Default | LwM2M Objects | Description |
|--------|---------|---------------|-------------|
| `WITH_STARLINK` | ON | 10512 | Starlink terminal |
| `WITH_STARLINK_GRPC` | ON | - | Starlink gRPC |

### prpl Platform
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_PRPL_INTEGRATION` | ON | prpl integration |
| `WITH_PRPL_EASYMESH` | ON | EasyMesh WiFi |
| `WITH_PRPL_AMBIORIX` | ON | Ambiorix framework |

### Matter/Thread/Zigbee
| Option | Default | LwM2M Objects | Description |
|--------|---------|---------------|-------------|
| `WITH_MATTER_THREAD` | ON | - | Matter over Thread |
| `WITH_MATTER_BRIDGE` | ON | 10529-10531 | Matter bridge |
| `WITH_THREAD_BR` | ON | - | Thread border router |
| `WITH_ZIGBEE` | ON | 10532-10534 | Zigbee support |
| `WITH_ZIGBEE_EZSP` | ON | - | Zigbee EZSP protocol |
| `WITH_ZIGBEE_ZSTACK` | ON | - | Zigbee Z-Stack |

### Build Options
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_TESTS` | ON | Build unit tests |
| `WITH_EXAMPLES` | ON | Build examples |
| `WITH_DOCUMENTATION` | ON | Generate docs |
| `WITH_COVERAGE` | ON | Code coverage |
| `WITH_SANITIZERS` | ON | Memory sanitizers |
| `CMAKE_BUILD_TYPE` | Release | Build type |

## LwM2M Object to CMake Flag Mapping

| Object ID | Object Name | CMake Flag |
|-----------|-------------|------------|
| 0-3 | Mandatory Objects | Always enabled |
| 10512 | Starlink Terminal | `WITH_STARLINK` |
| 10513-10522 | Router Management | Various (enabled by default) |
| 10525 | WAN Failover Policy | `WITH_WAN_FAILOVER` |
| 10526 | Multi-WAN Health Check | `WITH_MULTIWAN_HEALTH` |
| 10527 | WiFi Client Management | `WITH_WIFI_CLIENT_MGMT` |
| 10528 | WiFi Channel Optimization | `WITH_WIFI_CHANNEL_OPT` |
| 10529 | Matter Bridge | `WITH_MATTER_BRIDGE` |
| 10530 | Matter Device | `WITH_MATTER_BRIDGE` |
| 10531 | Thread Network | `WITH_MATTER_BRIDGE` |
| 10532 | Zigbee Coordinator | `WITH_ZIGBEE` |
| 10533 | Zigbee Device | `WITH_ZIGBEE` |
| 10534 | Zigbee Group | `WITH_ZIGBEE` |
| 10535 | LAN Configuration | `WITH_LAN_CONFIG` |
| 10536 | Routing Table | `WITH_ROUTING_TABLE` |
| 10537 | VPN Configuration | `WITH_VPN_CONFIG` |

## Common Build Scenarios

### Scenario 1: Production Router
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DWITH_OPENWRT_EXTENDED=ON \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_VPN_CONFIG=ON
```

### Scenario 2: Smart Home Hub
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DWITH_MATTER_BRIDGE=ON \
  -DWITH_ZIGBEE=ON \
  -DWITH_THREAD_BR=ON
```

### Scenario 3: Development/Testing
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DWITH_TESTS=ON \
  -DWITH_COVERAGE=ON \
  -DWITH_SANITIZERS=ON
```

### Scenario 4: Starlink Gateway
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DWITH_STARLINK=ON \
  -DWITH_STARLINK_GRPC=ON \
  -DWITH_MQTT_TRANSPORT=ON
```

### Scenario 5: Minimal Embedded
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DDTLS=ON \
  -DMBEDTLS=ON \
  -DWITH_EXAMPLES=OFF \
  -DWITH_TESTS=OFF
```

## Build Commands

### Configure
```bash
cd build
cmake .. [OPTIONS]
```

### Build
```bash
# All targets
make -j$(nproc)                    # Linux/macOS
cmake --build . --parallel         # Cross-platform

# Specific target
make friendly_example_client
cmake --build . --target friendly_example_client
```

### Test
```bash
make test
ctest
ctest --verbose
```

### Install
```bash
sudo make install
cmake --install .
```

### Clean
```bash
make clean              # Clean build artifacts
rm -rf build/*          # Full clean
```

## Checking Configuration

### View Enabled Features
```bash
cd build
cmake .. [OPTIONS] 2>&1 | grep "ON\|OFF"
```

### View CMake Cache
```bash
cat build/CMakeCache.txt | grep "WITH_"
```

### CCMake (Interactive)
```bash
cd build
ccmake ..
```

## Troubleshooting Quick Fixes

### Reset Configuration
```bash
rm -rf build/*
cmake .. [OPTIONS]
```

### Check Missing Dependencies
```bash
cmake .. 2>&1 | grep -i "not found\|missing\|error"
```

### Verbose Build
```bash
make VERBOSE=1
cmake --build . --verbose
```

### Check Installed Libraries
```bash
# Linux
ldconfig -p | grep <library>
pkg-config --list-all | grep <library>

# macOS
brew list | grep <library>
```

## Platform-Specific Notes

### Linux
- Most features supported
- OpenWRT features require OpenWRT SDK

### macOS
- OpenWRT features not available
- Use Homebrew for dependencies

### Windows
- Limited platform integration support
- Use MSYS2/MinGW or Visual Studio

### OpenWRT
- Cross-compile with OpenWRT toolchain
- Use OpenWRT SDK configuration

## See Also
- `CMAKE_CONFIGURATION_GUIDE.md` - Detailed guide
- `configure_full_build.sh` - Auto-configuration script
- `CONFIGURATION_SUMMARY.md` - Overall summary
