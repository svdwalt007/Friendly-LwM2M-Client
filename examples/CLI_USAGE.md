# CLI Usage Guide for Friendly LwM2M Client

This guide documents the command-line interface (CLI) options for the Friendly LwM2M Client example application.

## Overview

The LwM2M client now supports runtime configuration via command-line options, eliminating the need to recompile for different configurations. All previous hardcoded values (endpoint name, server URI, security credentials, etc.) can now be specified at runtime.

## Backward Compatibility

Running the client with no arguments will use the same default values as before:
- Endpoint Name: `walttech888`
- Server URI: `coap://demo-iot.friendly-tech.com:5680`
- Local Port: `56830`
- Lifetime: `25` seconds
- Security Mode: `none` (NO_SEC)
- Bootstrap: `enabled`
- Address Family: `IPv4`

```bash
./lwm2m_client
# Behaves identically to the previous hardcoded version
```

## Command-Line Options

### Connection Options

| Option | Short | Long | Description | Default |
|--------|-------|------|-------------|---------|
| Endpoint Name | `-n` | `--name` | LwM2M endpoint identifier | `walttech888` |
| Server URI | `-u` | `--uri` | Full server URI (coap:// or coaps://) | `coap://demo-iot.friendly-tech.com:5680` |
| Local Port | `-p` | `--port` | Local UDP port to bind | `56830` |
| Lifetime | `-l` | `--lifetime` | Registration lifetime in seconds | `25` |
| IPv4 | `-4` | `--ipv4` | Use IPv4 address family | ✓ |
| IPv6 | `-6` | `--ipv6` | Use IPv6 address family | |
| Bootstrap | `-b` | `--bootstrap` | Enable bootstrap mode | ✓ |
| No Bootstrap | `-B` | `--no-bootstrap` | Disable bootstrap mode | |

### Security Options

| Option | Short | Long | Description | Default |
|--------|-------|------|-------------|---------|
| Security Mode | `-s` | `--security` | Security mode: `none`, `psk`, `rpk`, `cert` | `none` |
| PSK Identity | `-i` | `--psk-identity` | PSK identity string | |
| PSK Key | `-k` | `--psk-key` | PSK key as hex string | |
| RPK Public Key | | `--rpk-public` | RPK public key as hex string | |
| RPK Private Key | | `--rpk-private` | RPK private key as hex string | |
| Certificate | | `--cert` | Certificate file path | |
| Private Key | | `--key` | Private key file path | |
| CA Certificate | | `--ca` | CA certificate file path | |

### Protocol Options

| Option | Short | Long | Description | Default |
|--------|-------|------|-------------|---------|
| Block Size | | `--block-size` | CoAP block size (16-1024, power of 2) | `1024` |

### Application Options

| Option | Short | Long | Description | Default |
|--------|-------|------|-------------|---------|
| Verbose | `-v` | `--verbose` | Increase verbosity (repeatable) | `0` |
| Quiet | `-q` | `--quiet` | Suppress non-error output | |
| Daemon | `-d` | `--daemon` | Run as daemon | |
| Config File | `-c` | `--config` | Load configuration from file | |

### Information Options

| Option | Short | Long | Description |
|--------|-------|------|-------------|
| Help | `-h` | `--help` | Show usage message and exit |
| Version | `-V` | `--version` | Show version information and exit |

## Usage Examples

### Basic Usage

```bash
# Show help
./lwm2m_client --help

# Show version
./lwm2m_client --version

# Run with default settings (backward compatible)
./lwm2m_client
```

### Custom Endpoint and Server

```bash
# Connect to a different server
./lwm2m_client -n mydevice01 -u coap://lwm2m.example.com:5683

# Bootstrap to Friendly demo server
./lwm2m_client -n openwrt-gw-001 -b -u coap://demo-iot.friendly-tech.com:5680
```

### Security Modes

#### PSK (Pre-Shared Key)

```bash
# PSK security with custom credentials
./lwm2m_client \
  -n mydevice01 \
  -u coaps://demo-iot.friendly-tech.com:5684 \
  -s psk \
  -i "MY_PSK_IDENTITY" \
  -k "00112233445566778899998877665544"
```

The PSK key must be provided as a hex string (even number of characters, 0-9, a-f, A-F).

#### RPK (Raw Public Key)

```bash
# RPK security mode
./lwm2m_client \
  -s rpk \
  --rpk-public "3059301306072a8648ce3d020106082a8648ce3d030107034200..." \
  --rpk-private "92045322a5b34562e1ffec4bcdcc257b9ecfc3478bfaea4b6b..."
```

#### Certificate Mode

```bash
# Certificate-based security
./lwm2m_client \
  -s cert \
  --cert /path/to/client.crt \
  --key /path/to/client.key \
  --ca /path/to/ca.crt
```

### Network Configuration

```bash
# IPv6 with custom lifetime
./lwm2m_client -n gateway01 -6 -t 300 -u coap://[2001:db8::1]:5683

# Custom local port
./lwm2m_client -p 12345

# Direct registration (no bootstrap)
./lwm2m_client -B -u coap://lwm2m.example.com:5683
```

### Logging and Debug

```bash
# Verbose output (level 1)
./lwm2m_client -v

# Very verbose (level 3)
./lwm2m_client -vvv

# Quiet mode (suppress non-error output)
./lwm2m_client -q
```

### Advanced Examples

```bash
# Production deployment: PSK security, IPv6, long lifetime
./lwm2m_client \
  -n prod-device-12345 \
  -6 \
  -t 86400 \
  -u coaps://[2001:db8::100]:5684 \
  -s psk \
  -i "prod-device-12345" \
  -k "aabbccdd00112233445566778899ffee" \
  -q

# Development setup: local server, verbose logging
./lwm2m_client \
  -n dev-test \
  -u coap://localhost:5683 \
  -B \
  -vvv

# Custom block size for constrained networks
./lwm2m_client --block-size 256
```

## Validation and Error Handling

The CLI parser includes extensive validation:

### URI Scheme Validation
- Warns if using `coaps://` with `--security none`
- Warns if using `coap://` with DTLS security modes
- Auto-corrects scheme when possible

### Security Mode Dependencies
- PSK mode requires both `--psk-identity` and `--psk-key`
- RPK mode requires both `--rpk-public` and `--rpk-private`
- Certificate mode requires `--cert` and `--key`

### Hex String Validation
- PSK and RPK keys must be valid hex strings
- Must have even length
- Only characters 0-9, a-f, A-F allowed

### Block Size Validation
- Must be power of 2
- Range: 16-1024 bytes

## Configuration Priority

When both compile-time defines and CLI options are present:
1. CLI options take precedence over compile-time defaults
2. Compile-time defines serve as fallback defaults
3. CLI validation ensures consistency (e.g., URI scheme vs security mode)

## Migrating from Hardcoded Configuration

### Before (Compile-time configuration)

```cpp
// Required editing source code:
string clientName = "walttech888";
Connection connection("56830", AF_INET);
// ... rebuild required for every change
```

### After (Runtime configuration)

```bash
# No recompilation needed:
./lwm2m_client -n newdevice -p 56831 -u coap://newserver:5683
```

## Environment-Specific Configurations

### Development
```bash
./lwm2m_client -n dev-$(hostname) -u coap://localhost:5683 -B -vvv
```

### Staging
```bash
./lwm2m_client -n staging-device -u coaps://staging.example.com:5684 -s psk -i "staging-id" -k "$PSK_KEY"
```

### Production
```bash
./lwm2m_client -n prod-device -u coaps://prod.example.com:5684 -s psk -i "$DEVICE_ID" -k "$PSK_KEY" -t 3600 -q
```

## Integration with OpenWRT

The CLI options complement the existing OpenWRT UCI configuration:

```bash
# Use UCI config
lwm2m_client  # Reads from /etc/config/lwm2m

# Override UCI with CLI
lwm2m_client -n override-name -u coap://override-server:5683
```

## Troubleshooting

### "Error: PSK mode requires --psk-identity"
You specified `-s psk` but didn't provide the identity. Both identity and key are required.

### "Error: Hex string must have even length"
PSK/RPK keys must be hex strings with an even number of characters.

### "Warning: Using coap:// with DTLS security mode"
You're using a non-secure URI scheme with a security mode that requires DTLS. Use `coaps://` instead.

### "Error: Block size must be a power of 2 between 16 and 1024"
The `--block-size` value must be: 16, 32, 64, 128, 256, 512, or 1024.

## See Also

- [CONFIGURATION.md](../docs/CONFIGURATION.md) - Complete configuration reference
- [QUICKSTART.md](../docs/QUICKSTART.md) - Getting started guide
- [README.md](../README.md) - Main project documentation
