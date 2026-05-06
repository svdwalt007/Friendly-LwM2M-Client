# Development Setup - Quick Reference

## 🚀 One-Command Setup (Recommended)

```bash
./scripts/dev_setup_complete.sh
```

**What it does:**
- ✅ Checks all prerequisites
- ✅ Initializes git submodules (Wakaama, libcoap)
- ✅ Configures CMake with ALL features enabled
- ✅ Builds the project (~15-30 min)
- ✅ Runs complete test suite
- ✅ Generates usage documentation
- ✅ Creates example applications

**Time:** 20-40 minutes (fully automated)

---

## 📋 Prerequisites

### Ubuntu/Debian
```bash
sudo apt-get install cmake build-essential git pkg-config python3 \
  libmbedtls-dev libssl-dev zlib1g-dev libcurl4-openssl-dev lcov
```

### Fedora/RedHat
```bash
sudo dnf install cmake gcc-c++ git pkg-config python3 \
  mbedtls-devel openssl-devel zlib-devel libcurl-devel lcov
```

### macOS
```bash
brew install cmake git pkg-config mbedtls openssl zlib curl lcov
```

---

## ✨ What Gets Built

### Walt Tech Objects (10525-10537)
- ✅ WAN Failover Policy (10525)
- ✅ Multi-WAN Health Check (10526)
- ✅ WiFi Client Management (10527)
- ✅ WiFi Channel Optimization (10528)
- ✅ Matter Bridge (10529)
- ✅ Matter Device (10530)
- ✅ Thread Network (10531)
- ✅ LAN Configuration (10535)
- ✅ Routing Table (10536)
- ✅ VPN Configuration (10537)

### Core Features
- ✅ Multi-DTLS Providers (TinyDTLS, mbedTLS, OpenSSL)
- ✅ MQTT Transport Binding
- ✅ Delta FOTA + A/B Rollback
- ✅ Edge AI Inference
- ✅ Starlink Integration
- ✅ OpenWRT Extended

### Test Suite
- ✅ Unit tests (delta algorithms, rollback, blockwise)
- ✅ Integration tests (DTLS, MQTT, platform)
- ✅ Benchmark tests (performance, memory)

---

## 🎯 Quick Test

After setup completes:

```bash
cd build/examples

# Test the client
./friendly_example_client -h coaps://leshan.eclipseprojects.io:5684 -n TestDevice

# List DTLS providers
./friendly_example_client --list-dtls-providers

# Run tests
cd .. && ctest
```

View your device at: https://leshan.eclipseprojects.io/

---

## 📚 Documentation

| File | Purpose |
|------|---------|
| `QUICK_START_DEV.md` | Detailed setup guide with all options |
| `FIRST_TIME_USAGE.md` | Auto-generated usage guide after setup |
| `scripts/dev_setup_complete.sh` | The automated setup script |
| `README.md` | Project overview |

---

## 🔧 Manual Setup (Alternative)

If you prefer manual control:

```bash
# 1. Initialize submodules
git submodule update --init --recursive

# 2. Configure
mkdir build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DDTLS_PROVIDER_MBEDTLS=ON \
  -DWITH_WAN_FAILOVER=ON \
  -DWITH_MULTIWAN_HEALTH=ON \
  -DWITH_WIFI_CLIENT_MGMT=ON \
  -DWITH_WIFI_CHANNEL_OPT=ON \
  -DBUILD_TESTS=ON

# 3. Build
make -j$(nproc)

# 4. Test
ctest
```

See `QUICK_START_DEV.md` for all CMake options.

---

## 🐛 Troubleshooting

**Build fails with missing dependencies?**
→ Run: `sudo apt-get install cmake build-essential libmbedtls-dev libssl-dev`

**Submodule errors?**
→ Run: `git submodule update --init --recursive`

**Tests failing?**
→ Clean rebuild: `rm -rf build && ./scripts/dev_setup_complete.sh`

**Can't connect to server?**
→ Check: `ping leshan.eclipseprojects.io` and `nc -zv leshan.eclipseprojects.io 5684`

See `QUICK_START_DEV.md` for complete troubleshooting guide.

---

## 📦 Platform Deployments

### OpenWRT
```bash
# See openwrt/README.md
make -C openwrt package/friendly-lwm2m/compile V=s
```

### prpl
```bash
# See src/platform/prpl/README.md
# Requires prplOS environment
```

---

## ℹ️ Support

**Questions?** Open a GitHub issue
**Documentation:** Check `docs/` directory
**Examples:** See `examples/` directory

---

**Created by:** Development setup automation script
**Version:** 1.2.2
**Date:** 2026-04-28

🎉 **Happy developing!**
