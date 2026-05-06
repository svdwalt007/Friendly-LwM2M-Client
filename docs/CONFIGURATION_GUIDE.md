# Build Configuration Guide

This guide explains how to configure and build the Friendly LwM2M Client with the exact objects and features you need.

## Quick Start

### Option 1: Interactive Configuration (Recommended for First-Time Users)

```bash
./configure.sh
```

This interactive script will guide you through all configuration options with yes/no prompts.

### Option 2: Preset Configuration (Fastest)

```bash
./configure_preset.sh
```

Choose from presets:
1. **Minimal** - Basic LwM2M client only
2. **Standard** - Core features (DTLS + MQTT + Delta FOTA)
3. **OpenWRT** - Router features (WAN failover, WiFi management, etc.)
4. **Full** - All features enabled
5. **Development** - Full features + tests + coverage

### Option 3: Command-Line Configuration (Most Flexible)

```bash
./configure_cli.sh [options]
```

**Examples:**

```bash
# Standard build with specific objects
./configure_cli.sh --dtls --mqtt --wan-failover --lan-config

# OpenWRT preset
./configure_cli.sh --preset=openwrt

# All custom objects
./configure_cli.sh --all-objects --tests

# Minimal build
./configure_cli.sh --preset=minimal
```

## LwM2M Objects Reference

### Mandatory Objects (Always Included)
- **Object 0** - LwM2M Security
- **Object 1** - LwM2M Server
- **Object 2** - Access Control
- **Object 3** - Device

### Standard Optional Objects (Automatically Included)
- **Object 4** - Connectivity Monitoring
- **Object 5** - Firmware Update
- **Object 6** - Location
- **Object 7** - Connectivity Statistics
- **Objects 8-13** - Various standard objects

### Custom Objects (Walt Technologies)

#### Router Management Objects
| Object ID | Name | CMake Flag | CLI Option |
|-----------|------|------------|------------|
| 10535 | LAN Configuration | `-DWITH_LAN_CONFIG=ON` | `--lan-config` |
| 10536 | Routing Table | `-DWITH_ROUTING_TABLE=ON` | `--routing-table` |
| 10537 | VPN Configuration | `-DWITH_VPN_CONFIG=ON` | `--vpn-config` |

#### Multi-WAN and Failover Objects
| Object ID | Name | CMake Flag | CLI Option |
|-----------|------|------------|------------|
| 10525 | WAN Failover Policy | `-DWITH_WAN_FAILOVER=ON` | `--wan-failover` |
| 10526 | Multi-WAN Health Check | `-DWITH_MULTIWAN_HEALTH=ON` | `--multiwan-health` |

#### WiFi Management Objects
| Object ID | Name | CMake Flag | CLI Option |
|-----------|------|------------|------------|
| 10527 | WiFi Client Management | `-DWITH_WIFI_CLIENT_MGMT=ON` | `--wifi-client-mgmt` |
| 10528 | WiFi Channel Optimization | `-DWITH_WIFI_CHANNEL_OPT=ON` | `--wifi-channel-opt` |

**Note:** Custom objects (10525-10537) require Wakaama submodule headers.

## Manual CMake Configuration

If you prefer direct CMake control:

```bash
mkdir -p build
cd build

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_DTLS=ON \
    -DWITH_MBEDTLS=ON \
    -DWITH_MQTT=ON \
    -DWITH_WAN_FAILOVER=ON \
    -DWITH_LAN_CONFIG=ON \
    -DBUILD_TESTS=ON

make -j$(nproc)
```

## All CMake Options

### Core Features
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_DTLS` | ON | Enable DTLS support |
| `WITH_MBEDTLS` | ON | Use mbedTLS library |
| `WITH_TINYDTLS` | OFF | Use TinyDTLS library |
| `WITH_MQTT` | ON | Enable MQTT transport |
| `WITH_DELTA_FOTA` | ON | Delta firmware updates |
| `WITH_BSDIFF` | ON | BSDIFF algorithm |
| `WITH_VCDIFF` | ON | VCDIFF algorithm |
| `WITH_COURGETTE` | ON | Courgette algorithm |
| `WITH_ROLLBACK` | ON | A/B partition rollback |
| `WITH_EDGE_AI` | ON | Edge AI inference |
| `WITH_COMPRESSION` | ON | Compression support |

### Starlink Integration
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_STARLINK` | ON | Starlink integration |
| `WITH_STARLINK_GRPC` | ON | Starlink gRPC support |

### OpenWRT Integration
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_OPENWRT_EXTENDED` | ON | OpenWRT integration |
| `WITH_LUCI_MODULE` | ON | LuCI web interface |
| `WITH_HOTPLUG_HANDLERS` | ON | Hotplug handlers |

### Router Management Objects
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_LAN_CONFIG` | OFF | LAN Configuration (10535) |
| `WITH_ROUTING_TABLE` | OFF | Routing Table (10536) |
| `WITH_VPN_CONFIG` | OFF | VPN Configuration (10537) |

### Multi-WAN Objects
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_WAN_FAILOVER` | OFF | WAN Failover Policy (10525) |
| `WITH_MULTIWAN_HEALTH` | OFF | Multi-WAN Health (10526) |

### WiFi Objects
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_WIFI_CLIENT_MGMT` | OFF | WiFi Client Mgmt (10527) |
| `WITH_WIFI_CHANNEL_OPT` | OFF | WiFi Channel Opt (10528) |

### Matter Integration
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_MATTER` | OFF | Matter/Thread support |
| `WITH_MATTER_BRIDGE` | OFF | Matter Bridge (33450) |
| `WITH_THREAD_BR` | OFF | Thread Border Router |

### Zigbee Integration
| Option | Default | Description |
|--------|---------|-------------|
| `WITH_ZIGBEE` | OFF | Zigbee integration |
| `WITH_ZIGBEE_EZSP` | OFF | Silicon Labs EZSP |
| `WITH_ZIGBEE_ZSTACK` | OFF | TI Z-Stack |

### Build Options
| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | ON | Build unit tests |
| `BUILD_EXAMPLES` | ON | Build examples |
| `ENABLE_COVERAGE` | ON | Code coverage |
| `ENABLE_SANITIZERS` | ON | Address/UB sanitizers |
| `CMAKE_BUILD_TYPE` | Release | Debug/Release/RelWithDebInfo |

## Command-Line Quick Reference

### configure_cli.sh Options

**Presets:**
```bash
--preset=minimal     # Minimal build
--preset=standard    # Standard features (default)
--preset=openwrt     # OpenWRT configuration
--preset=full        # All features
--preset=dev         # Development build
```

**Individual Features:**
```bash
--dtls               # Enable DTLS
--mqtt               # Enable MQTT
--delta-fota         # Delta firmware updates
--edge-ai            # Edge AI inference
--starlink           # Starlink integration
--openwrt            # OpenWRT integration
```

**Object Groups:**
```bash
--all-wan-objects    # All WAN/failover objects
--all-wifi-objects   # All WiFi objects
--all-router-objects # All router management
--all-objects        # All custom objects
```

**Build Options:**
```bash
--debug              # Debug build
--release            # Release build (default)
--tests              # Build tests
--coverage           # Enable coverage
--sanitizers         # Enable sanitizers
--clean              # Clean before configure
```

## Building

After configuration:

```bash
cd build

# Build everything
make -j$(nproc)

# Build specific targets
make friendly_example_client  # Main application
make friendly_lwm2m           # Library only
make test                      # Run tests
```

## JSON Configuration

The client can load object data from JSON at startup:

```bash
cd build/examples
./friendly_example_client --config ../../examples/lwm2m_objects_config.json
```

See `examples/lwm2m_objects_config.json` for a complete example with all 37 objects populated.

## Troubleshooting

### Objects require Wakaama submodule

If you enable objects 10525-10537 and get errors about missing `liblwm2m.h`:

1. Ensure the `2305-Wakaama` submodule is initialized
2. The build system should automatically add Wakaama includes when these objects are enabled
3. If issues persist, verify: `ls 2305-Wakaama/include/liblwm2m.h`

### Clean rebuild

```bash
cd build
make clean
# OR for complete clean
rm -rf *
cmake ..
make -j$(nproc)
```

### See all options

```bash
./configure_cli.sh --help
```

## Examples

### Example 1: Minimal IoT Device
```bash
./configure_cli.sh --preset=minimal
cd build && make -j$(nproc)
```

### Example 2: OpenWRT Router
```bash
./configure_cli.sh \
    --preset=openwrt \
    --all-wan-objects \
    --all-wifi-objects \
    --tests

cd build && make -j$(nproc)
```

### Example 3: Development Build
```bash
./configure_cli.sh \
    --debug \
    --all-objects \
    --coverage \
    --sanitizers \
    --tests

cd build && make -j$(nproc)
```

### Example 4: Custom Selection
```bash
./configure_cli.sh \
    --dtls \
    --mbedtls \
    --mqtt \
    --delta-fota \
    --starlink \
    --wan-failover \
    --lan-config \
    --vpn-config

cd build && make -j$(nproc)
```

## Further Information

- For detailed object descriptions, see `OBJECTS_10525-10537_README.md`
- For build fixes, see `BUILD_FIXES_SUMMARY.md`
- For CMake options reference, see `CMAKE_OPTIONS_REFERENCE.md`
