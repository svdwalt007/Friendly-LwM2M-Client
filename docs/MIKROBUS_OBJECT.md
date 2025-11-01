# MIKROBUS Object (ID 34608)

Complete documentation for the Walt Technologies MIKROBUS LwM2M object.

---

## Overview

The MIKROBUS object provides comprehensive management and monitoring of MIKROBUS sockets and MikroElektronika Click boards. MIKROBUS is an add-on board standard that defines a specific pinout with SPI, I2C, UART, PWM, GPIO, and analog interfaces.

**Object ID:** 34608 (Walt Technologies Custom Object Range)
**URN:** `urn:oma:lwm2m:x:34608:1.0`
**Version:** 1.0
**Instance Type:** Multiple Instances (one per MIKROBUS socket)
**Mandatory:** Optional

---

## Key Features

- **50+ Resources** organized into 8 functional groups
- **Multiple Instance Support**: One instance per MIKROBUS socket
- **Click Board Detection**: Automatic detection via I2C EEPROM manifest
- **Interface Configuration**: SPI, I2C, UART with full parameter control
- **GPIO Control**: AN (analog), PWM, INT, RST, CS pins
- **Power Management**: 3.3V/5V voltage selection and power control
- **Status Monitoring**: Communication status, error tracking, uptime
- **Control Actions**: Initialize, reset, shutdown, read manifest

---

## Resource Groups

### Socket Information (Resources 0-6)

| Resource | Name | Type | Ops | Description |
|----------|------|------|-----|-------------|
| 0 | Socket ID | Integer | R | Socket number (0, 1, 2, ...) |
| 1 | Socket Name | String | RW | Human-readable socket name |
| 2 | Socket Enabled | Boolean | RW | Socket enable/disable state |
| 3 | Click Board Present | Boolean | R | True if Click board detected |
| 4 | Click Board Type | String | R | Board type/category |
| 5 | Power Voltage | Integer | RW | Voltage in mV (3300=3.3V, 5000=5V) |
| 6 | Power State | Boolean | RW | Power enabled/disabled |

### Click Board Information (Resources 10-14)

| Resource | Name | Type | Ops | Description |
|----------|------|------|-----|-------------|
| 10 | Board Manufacturer | String | R | Manufacturer name |
| 11 | Board Model | String | R | Model number/name |
| 12 | Board Firmware Version | String | R | Firmware version |
| 13 | Board Serial Number | String | R | Unique serial number |
| 14 | Board Description | String | R | Board description |

### Interface Configuration (Resources 20-27)

| Resource | Name | Type | Ops | Range | Description |
|----------|------|------|-----|-------|-------------|
| 20 | Active Interface | Integer | RW | 0-3 | 0=None, 1=SPI, 2=I2C, 3=UART |
| 21 | I2C Address | Integer | RW | 0-127 | I2C slave address (7-bit) |
| 22 | SPI Mode | Integer | RW | 0-3 | SPI mode (CPOL/CPHA) |
| 23 | SPI Speed | Integer | RW | - | SPI clock speed (Hz) |
| 24 | UART Baud Rate | Integer | RW | - | Baud rate (bps) |
| 25 | UART Data Bits | Integer | RW | 7-8 | Data bits |
| 26 | UART Parity | Integer | RW | 0-2 | 0=None, 1=Odd, 2=Even |
| 27 | UART Stop Bits | Integer | RW | 1-2 | Stop bits |

### GPIO and Analog (Resources 31-36)

| Resource | Name | Type | Ops | Range | Description |
|----------|------|------|-----|-------|-------------|
| 31 | AN Value | Float | R | 0-3.3V | Analog input voltage |
| 32 | PWM Duty Cycle | Float | RW | 0-100% | PWM duty cycle |
| 33 | PWM Frequency | Integer | RW | Hz | PWM frequency |
| 34 | INT Pin State | Boolean | R | - | Interrupt pin state |
| 35 | RST Pin State | Boolean | RW | - | Reset pin (active low) |
| 36 | CS Pin State | Boolean | RW | - | Chip select (active low) |

### Status and Diagnostics (Resources 40-44)

| Resource | Name | Type | Ops | Description |
|----------|------|------|-----|-------------|
| 40 | Communication Status | Integer | R | 0=OK, 1=Error, 2=Timeout, 3=Not Configured |
| 41 | Last Error | String | R | Last error message |
| 42 | Transaction Count | Integer | R | Total I/O transactions |
| 43 | Error Count | Integer | R | Total error count |
| 44 | Uptime | Integer | R | Socket uptime (seconds) |

### Control Actions (Resources 101-103, 110-111)

| Resource | Name | Type | Ops | Description |
|----------|------|------|-----|-------------|
| 101 | Init Click Board | - | E | Initialize Click board |
| 102 | Reset Click Board | - | E | Reset via RST pin |
| 103 | Shutdown Click Board | - | E | Shutdown and disable power |
| 110 | Read Manifest | - | E | Read Click board manifest |
| 111 | Manifest Data | String | R | Manifest data (JSON) |

---

## Configuration

### Enable MIKROBUS Object

**wpp/configs/wpp_config.cmake:**
```cmake
# Enable MIKROBUS object
set(WPP_DEFINITIONS ${WPP_DEFINITIONS} OBJ_O_34608_MIKROBUS)
```

### Initialize MIKROBUS Sockets

**examples/objects.cpp:**
```cpp
#ifdef OBJ_O_34608_MIKROBUS
void mikrobusInit(WppClient &client) {
    client.registry().registerObj(Mikrobus::object(client));

    // Create instance for socket 1
    Instance *mikrobus1 = Mikrobus::createInst(client, 0);
    mikrobus1->set<STRING_T>(Mikrobus::SOCKET_NAME_1, "MIKROBUS-1");
    
    // Create instance for socket 2
    Instance *mikrobus2 = Mikrobus::createInst(client, 1);
    mikrobus2->set<STRING_T>(Mikrobus::SOCKET_NAME_1, "MIKROBUS-2");
}
#endif
```

---

## Usage Examples

### Configure SPI Interface

```cpp
Instance* mikrobus = Mikrobus::instance(client, 0);
if (mikrobus) {
    // Set active interface to SPI
    mikrobus->set<INT_T>(Mikrobus::ACTIVE_INTERFACE_20, Mikrobus::INTERFACE_SPI);
    
    // Configure SPI parameters
    mikrobus->set<INT_T>(Mikrobus::SPI_MODE_22, 0);           // Mode 0
    mikrobus->set<INT_T>(Mikrobus::SPI_SPEED_23, 1000000);    // 1 MHz
    
    // Enable power
    mikrobus->set<BOOL_T>(Mikrobus::POWER_STATE_6, true);
}
```

### Configure I2C Interface

```cpp
Instance* mikrobus = Mikrobus::instance(client, 0);
if (mikrobus) {
    // Set active interface to I2C
    mikrobus->set<INT_T>(Mikrobus::ACTIVE_INTERFACE_20, Mikrobus::INTERFACE_I2C);
    
    // Set I2C address
    mikrobus->set<INT_T>(Mikrobus::I2C_ADDRESS_21, 0x48);
    
    // Enable power at 3.3V
    mikrobus->set<INT_T>(Mikrobus::POWER_VOLTAGE_5, 3300);
    mikrobus->set<BOOL_T>(Mikrobus::POWER_STATE_6, true);
}
```

### Read Analog Input

```cpp
Instance* mikrobus = Mikrobus::instance(client, 0);
if (mikrobus) {
    float voltage = mikrobus->get<FLOAT_T>(Mikrobus::AN_VALUE_31);
    std::cout << "Analog input: " << voltage << "V\n";
}
```

### Control PWM Output

```cpp
Instance* mikrobus = Mikrobus::instance(client, 0);
if (mikrobus) {
    // Set 50% duty cycle at 1kHz
    mikrobus->set<FLOAT_T>(Mikrobus::PWM_DUTY_CYCLE_32, 50.0f);
    mikrobus->set<INT_T>(Mikrobus::PWM_FREQUENCY_33, 1000);
}
```

### Reset Click Board

```cpp
Instance* mikrobus = Mikrobus::instance(client, 0);
if (mikrobus) {
    OPAQUE_T data;
    EXECUTE_RESULT result = mikrobus->execute(
        Mikrobus::RESET_CLICK_BOARD_102, data);
    
    if (result == EXECUTE_RESULT::EXECUTE_RESULT_SUCCESS) {
        std::cout << "Click board reset successfully\n";
    }
}
```

---

## MIKROBUS Standard

### Pin Layout

```
  AN  RST
  CS  SCK
MISO MOSI
 3V3  5V
 GND GND

 PWM INT
  RX  TX
 SCL SDA
  5V GND
```

### Standard Interfaces

- **SPI**: CS, SCK, MISO, MOSI
- **I2C**: SDA, SCL  
- **UART**: RX, TX
- **GPIO**: AN (analog), RST, INT, PWM, CS
- **Power**: 3.3V or 5V selectable

---

## OpenWRT Integration

### Hardware Access

**GPIO Control:**
```bash
# Export GPIO
echo mikrobus0_rst > /sys/class/gpio/export

# Set direction
echo out > /sys/class/gpio/mikrobus0_rst/direction

# Write value
echo 1 > /sys/class/gpio/mikrobus0_rst/value
```

**Analog Input:**
```bash
# Read ADC value
cat /sys/bus/iio/devices/iio:device0/in_voltage0_raw
```

**PWM Control:**
```bash
# Configure PWM
echo 0 > /sys/class/pwm/pwmchip0/export
echo 1000000 > /sys/class/pwm/pwmchip0/pwm0/period
echo 500000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle
echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable
```

---

## Click Board Manifest

### Manifest Format (JSON)

```json
{
  "manufacturer": "MikroElektronika",
  "model": "TEMP&HUM Click",
  "serial": "TH-2024-001234",
  "firmware": "1.0.0",
  "capabilities": ["I2C", "INT"],
  "i2c_address": "0x40",
  "description": "Temperature and Humidity sensor"
}
```

### Reading Manifest

Manifests are stored in I2C EEPROM (typically at address 0x50):

```cpp
// Execute read manifest action
OPAQUE_T data;
mikrobus->execute(Mikrobus::READ_MANIFEST_110, data);

// Get manifest data
std::string manifest = mikrobus->get<STRING_T>(
    Mikrobus::MANIFEST_DATA_111);
```

---

## Troubleshooting

### Click Board Not Detected

```bash
# Check I2C bus
i2cdetect -y 0

# Verify power
cat /sys/class/gpio/mikrobus0_power/value

# Check manifest EEPROM
i2cget -y 0 0x50 0x00
```

### SPI Communication Failures

- Verify CS pin state (should be high when inactive)
- Check SPI mode matches Click board requirements
- Verify clock speed is within Click board specs
- Ensure proper power voltage (3.3V vs 5V)

### UART Not Working

- Check baud rate matches Click board
- Verify data bits, parity, stop bits configuration
- Check TX/RX aren't swapped

---

## See Also

- [Walt Technologies Object Range Documentation](README.md)
- [OpenWRT Integration Guide](OPENWRT_INTEGRATION.md)
- [MIKROBUS Standard](https://www.mikroe.com/mikrobus)
- [MikroElektronika Click Boards](https://www.mikroe.com/click)
