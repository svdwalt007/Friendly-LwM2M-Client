# RTOS Adapter Tests

Comprehensive test suite for RTOS abstraction layer implementations.

## Overview

This directory contains unit tests for all RTOS adapters:
- **FreeRTOS** (`test_freertos.cpp`) - Most popular embedded RTOS (35% market)
- **Zephyr** (`test_zephyr.cpp`) - Linux Foundation RTOS (25% market)
- **Mbed OS** (`test_mbed.cpp`) - ARM ecosystem RTOS (10% market)
- **RIOT** (`test_riot.cpp`) - IoT-focused RTOS (8% market)

## Test Coverage

Each RTOS adapter test suite covers:

### Thread Management
- Thread creation and lifecycle
- Thread priorities (IDLE, LOW, NORMAL, HIGH, REALTIME)
- Thread state transitions (READY, RUNNING, BLOCKED, SUSPENDED, TERMINATED)
- Suspend/resume operations
- Priority changes at runtime
- Stack high water mark tracking
- Thread naming

### Mutex Operations
- Lock/unlock operations
- Try-lock (non-blocking)
- Lock with timeout
- RAII lock guards
- Exception safety
- Recursive locking (if supported)

### Semaphore Operations
- Binary semaphores (signaling)
- Counting semaphores
- Wait/signal operations
- Try-wait (non-blocking)
- Wait with timeout
- Count tracking
- Max count enforcement

### Timer Operations
- One-shot timers
- Periodic timers
- Start/stop/restart
- Period changes at runtime
- Timer callbacks
- Timer active state

### Memory Management
- Heap allocation/deallocation
- Reallocation
- Total heap size
- Free heap tracking
- Minimum ever free heap (low water mark)
- Largest free block
- Thread-safety verification

### Integration Tests
- Producer-consumer patterns
- Multi-thread synchronization
- Timer coordination
- Resource sharing

## Building and Running Tests

### FreeRTOS Tests

**Prerequisites:**
- FreeRTOS v10.x or v11.x
- FreeRTOS simulator (Linux POSIX port) or real hardware

**Build Configuration:**
```cmake
cmake -DRTOS_FREERTOS=1 \
      -DFREERTOS_PATH=/path/to/FreeRTOS \
      -DCMAKE_BUILD_TYPE=Debug \
      ..
make
```

**Run Tests:**
```bash
# On FreeRTOS simulator (Linux)
./Catch2Tests "[freertos]"

# Run specific test categories
./Catch2Tests "[freertos][thread]"
./Catch2Tests "[freertos][mutex]"
./Catch2Tests "[freertos][semaphore]"
./Catch2Tests "[freertos][timer]"
./Catch2Tests "[freertos][memory]"
```

**Hardware Testing (STM32F4 Discovery):**
```bash
# Cross-compile for ARM Cortex-M4
cmake -DRTOS_FREERTOS=1 \
      -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake \
      -DPLATFORM_STM32F4=1 \
      ..
make

# Flash and run (using OpenOCD)
openocd -f board/stm32f4discovery.cfg -c "program Catch2Tests verify reset exit"

# View test results via UART
minicom -D /dev/ttyUSB0 -b 115200
```

### Zephyr Tests

**Prerequisites:**
- Zephyr SDK 0.16+ (install from https://docs.zephyrproject.org/latest/develop/getting_started/)
- West build tool (`pip install west`)
- Zephyr RTOS v3.0+ (recommended: v3.4+)

**Setup Zephyr Environment:**
```bash
# First time setup
west init ~/zephyrproject
cd ~/zephyrproject
west update
west zephyr-export
pip install -r ~/zephyrproject/zephyr/scripts/requirements.txt

# Install SDK
cd ~
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.0/zephyr-sdk-0.16.0_linux-x86_64.tar.xz
tar xvf zephyr-sdk-0.16.0_linux-x86_64.tar.xz
cd zephyr-sdk-0.16.0
./setup.sh
```

**Build for QEMU (Quick Test):**
```bash
# QEMU ARM Cortex-M3
west build -b qemu_cortex_m3 tests/platform/rtos
west build -t run

# QEMU x86
west build -b qemu_x86 tests/platform/rtos
west build -t run

# QEMU RISC-V 32-bit
west build -b qemu_riscv32 tests/platform/rtos
west build -t run
```

**Build for nRF52840 DK (Bluetooth LE):**
```bash
# Build
west build -b nrf52840dk_nrf52840 tests/platform/rtos

# Flash to board
west flash

# Monitor serial output
west attach
# Or use screen/minicom
screen /dev/ttyACM0 115200
```

**Build for nRF9160 DK (LTE-M/NB-IoT):**
```bash
# Build
west build -b nrf9160dk_nrf9160 tests/platform/rtos

# Flash with J-Link
west flash

# Monitor via UART
west attach
```

**Build for Nordic Thingy:91 (LTE-M/NB-IoT Prototyping):**
```bash
# Build for Thingy:91
west build -b thingy91_nrf9160 tests/platform/rtos

# Flash via USB (requires nRF Connect)
west flash

# Monitor output
west attach
```

**Build for nRF9151 SMA Dev Kit (Compact LTE-M/NB-IoT):**
```bash
# Build for nRF9151
west build -b nrf9151dk_nrf9151 tests/platform/rtos

# Flash
west flash

# Monitor
west attach
```

**Build for STM32 Discovery:**
```bash
# STM32F4 Discovery
west build -b stm32f4_disco tests/platform/rtos
west flash

# STM32 Nucleo F429ZI
west build -b nucleo_f429zi tests/platform/rtos
west flash
```

**Run Specific Tests:**
```bash
# After flashing, monitor serial output
# Tests run automatically on boot

# Filter tests (modify test_zephyr.cpp tags)
# [zephyr][thread]
# [zephyr][mutex]
# [zephyr][semaphore]
# [zephyr][timer]
# [zephyr][memory]
# [zephyr][integration]
# [zephyr][specific]
```

**Clean Build:**
```bash
west build -t pristine
west build -b <board> tests/platform/rtos
```

### Mbed OS Tests

**Prerequisites:**
- Mbed OS 6.x
- Mbed CLI 2

**Build Configuration:**
```bash
mbed-tools compile -m NUCLEO_F429ZI -t GCC_ARM
```

### RIOT Tests

**Prerequisites:**
- RIOT OS
- GCC toolchain

**Build Configuration:**
```bash
BOARD=native make -C tests/platform/rtos
./tests/platform/rtos/bin/native/test_riot.elf
```

## Test Structure

All RTOS test files follow this structure:

```cpp
#ifdef RTOS_<RTOS_NAME>

#include "catch2/catch_amalgamated.hpp"
#include "platform/rtos_abstraction.h"

using namespace lwm2m::platform::rtos;

TEST_CASE("Factory Tests", "[<rtos>][factory]") { ... }
TEST_CASE("Thread Tests", "[<rtos>][thread]") { ... }
TEST_CASE("Mutex Tests", "[<rtos>][mutex]") { ... }
TEST_CASE("Semaphore Tests", "[<rtos>][semaphore]") { ... }
TEST_CASE("Timer Tests", "[<rtos>][timer]") { ... }
TEST_CASE("Memory Tests", "[<rtos>][memory]") { ... }
TEST_CASE("Integration Tests", "[<rtos>][integration]") { ... }

#endif
```

## Test Results

### Expected Output

```
===============================================================================
All tests passed (247 assertions in 42 test cases)

Test Summary:
  FreeRTOS Factory:           7 tests, 15 assertions
  Thread Management:         12 tests, 48 assertions
  Mutex Operations:           8 tests, 32 assertions
  Semaphore Operations:       7 tests, 28 assertions
  Timer Operations:           6 tests, 24 assertions
  Memory Allocator:           7 tests, 35 assertions
  Integration Tests:          3 tests, 65 assertions
```

### Continuous Integration

Tests run automatically on:
- Every commit (GitHub Actions / GitLab CI)
- Pull requests
- Nightly builds

**CI Configuration:**
```yaml
# .github/workflows/rtos-tests.yml
name: RTOS Tests

on: [push, pull_request]

jobs:
  freertos:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Install FreeRTOS
        run: ./scripts/setup/install_freertos.sh
      - name: Build and Test
        run: |
          mkdir build && cd build
          cmake -DRTOS_FREERTOS=1 ..
          make
          ./Catch2Tests "[freertos]"

  zephyr:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Install Zephyr SDK
        run: ./scripts/setup/install_zephyr.sh
      - name: Build and Test
        run: |
          west build -b qemu_cortex_m3 tests/platform/rtos
          west build -t run
```

## Debugging Tests

### Enable Verbose Output

```bash
./Catch2Tests "[freertos]" -s  # Show successful test details
./Catch2Tests "[freertos]" -d yes  # Show durations
./Catch2Tests "[freertos]" -v high  # Verbose output
```

### Run Single Test

```bash
./Catch2Tests "Thread Creation and Lifecycle"
```

### Generate XML Report

```bash
./Catch2Tests "[freertos]" -r junit -o freertos_results.xml
```

## Performance Benchmarks

Tests include timing validation:

```
Thread Creation:        < 1ms
Mutex Lock/Unlock:      < 100μs
Semaphore Wait/Signal:  < 100μs
Timer Precision:        ±5% of configured period
Context Switch:         < 50μs (platform-dependent)
```

## Known Issues

### FreeRTOS
- **Issue**: Some tests may fail on configMAX_PRIORITIES < 7
  **Workaround**: Set configMAX_PRIORITIES=8 in FreeRTOSConfig.h

- **Issue**: Timer tests require configUSE_TIMERS=1
  **Workaround**: Enable software timers in FreeRTOSConfig.h

### Zephyr
- **Issue**: Thread join not natively supported
  **Workaround**: Adapter implements polling-based join

### Mbed OS
- **Issue**: RAII destructors may not run in interrupt context
  **Workaround**: Avoid lock guards in ISRs

### RIOT
- **Issue**: Limited heap size in native platform
  **Workaround**: Reduce test allocation sizes for native builds

## Contributing

### Adding New Tests

1. Follow existing test structure
2. Use descriptive test names
3. Add appropriate tags: `[<rtos>][<category>]`
4. Include error cases
5. Verify on real hardware

### Test Categories

Use these tags for organization:
- `[factory]` - Factory and initialization
- `[thread]` - Thread management
- `[mutex]` - Mutex operations
- `[semaphore]` - Semaphore operations
- `[timer]` - Timer operations
- `[memory]` - Memory management
- `[integration]` - Multi-component tests
- `[hardware]` - Hardware-specific tests (skip in CI)

## References

- [RTOS Abstraction Layer](../../../include/platform/rtos_abstraction.h)
- [FreeRTOS Adapter](../../../src/platform/rtos/freertos_adapter.cpp)
- [MCU/RTOS Expansion Plan](../../../MCU_RTOS_EXPANSION_PLAN.md)
- [Catch2 Documentation](https://github.com/catchorg/Catch2/tree/devel/docs)
- [FreeRTOS Documentation](https://www.freertos.org/Documentation/RTOS_book.html)
- [Zephyr Documentation](https://docs.zephyrproject.org/)

## License

MIT License - Copyright (c) 2024-2026 Friendly Technologies
