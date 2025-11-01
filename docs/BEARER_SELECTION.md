# Bearer Selection Object (ID 13)

Complete reference documentation for the OMA LwM2M Bearer Selection object.

---

## Overview

The Bearer Selection object (ID 13) manages preferred network bearer selection and configuration. It allows devices to automatically select the best available network bearer based on availability, signal strength, and operator preferences.

**Object ID:** 13
**URN:** urn:oma:lwm2m:oma:13
**Multiple Instances:** No (Single instance)
**Mandatory:** No

---

## Key Features

- **Bearer Preference Management:** Define priority order for network bearers
- **Signal Threshold Configuration:** Set minimum acceptable signal levels
- **Operator Management:** Whitelist/blacklist operators (PLMN)
- **Automatic Bearer Switching:** Based on availability and signal quality
- **Hysteresis Control:** Prevent frequent bearer switching
- **Multi-Technology Support:** Cellular (GSM/UMTS/LTE), WiFi, Ethernet

---

## Resource Summary

| ID | Name | Type | Operations | Description |
|----|------|------|------------|-------------|
| 0 | Preferred Communications Bearer | String | RW | Bearer preference list (comma-separated IDs) |
| 1 | Acceptable RSSI (GSM) | Integer | RW | Minimum GSM signal strength (dBm) |
| 2 | Acceptable RSCP (UMTS) | Integer | RW | Minimum UMTS signal strength (dBm) |
| 3 | Acceptable RSRP (LTE) | Integer | RW | Minimum LTE signal strength (dBm) |
| 4 | Acceptable RSSI (WLAN) | Integer | RW | Minimum WiFi signal strength (dBm) |
| 5 | Cell Lock List | String | RW | Locked cells (comma-separated) |
| 6 | Operator List | String | RW | Operator PLMN codes (MCC-MNC format) |
| 7 | Operator List Mode | Integer | RW | 0=Whitelist, 1=Blacklist |
| 8 | Available Network Bearers | String | R | Currently available bearers |
| 9 | Acceptable Signal Strength Variation | Integer | RW | Hysteresis value (dB) |
| 10 | Higher Priority PLMN Search Timer | Integer | RW | Search interval (seconds) |
| 11 | Attach Without PDN Connection | Boolean | RW | Attach mode for cellular |

---

## Network Bearer Types

The object supports the following network bearers (from Connectivity Monitoring specification):

### Cellular Bearers

| Value | Bearer | Description |
|-------|--------|-------------|
| 0 | GSM | 2G GSM/GPRS/EDGE |
| 1 | TD-SCDMA | 3G TD-SCDMA |
| 2 | WCDMA | 3G UMTS/HSPA |
| 3 | CDMA2000 | 3G CDMA2000 |
| 4 | WiMAX | WiMAX |
| 5 | LTE TDD | 4G LTE Time Division Duplex |
| 6 | LTE FDD | 4G LTE Frequency Division Duplex |
| 7 | LTE-M | 4G LTE Machine Type Communication |
| 8 | NB-IoT | Narrowband IoT |

### Non-Cellular Bearers

| Value | Bearer | Description |
|-------|--------|-------------|
| 21 | WLAN | WiFi/802.11 |
| 22 | Bluetooth | Bluetooth |
| 23 | IEEE 802.15.4 | Zigbee/6LoWPAN |
| 41 | Ethernet | Wired Ethernet |
| 42 | DSL | Digital Subscriber Line |
| 43 | PLC | Power Line Communication |

---

## Complete Resource Reference

### Resource 0: Preferred Communications Bearer

**Type:** String
**Operations:** RW
**Format:** Comma-separated list of bearer IDs in order of preference

**Example:**
```
"21,41,6,2,0"  // Prefer: WiFi > Ethernet > LTE > WCDMA > GSM
```

**Default:** WiFi > Ethernet > LTE > WCDMA > GSM

### Resources 1-4: Signal Strength Thresholds

| Resource | Bearer | Valid Range (dBm) | Default | Description |
|----------|--------|-------------------|---------|-------------|
| 1 | GSM | -110 to -48 | -70 | RSSI threshold for GSM |
| 2 | UMTS | -120 to -25 | -85 | RSCP threshold for UMTS/WCDMA |
| 3 | LTE | -140 to -44 | -95 | RSRP threshold for LTE |
| 4 | WLAN | -100 to -30 | -70 | RSSI threshold for WiFi |

### Resource 5: Cell Lock List

**Type:** String
**Operations:** RW
**Format:** Comma-separated list of cell IDs

**Purpose:** Lock device to specific cells (useful for fixed installations)

**Example:**
```
"12345,67890"
```

### Resource 6: Operator List

**Type:** String
**Operations:** RW
**Format:** Comma-separated MCC-MNC codes

**Example:**
```
"310-410,311-480"  // AT&T and Verizon in US
```

### Resource 7: Operator List Mode

**Type:** Integer
**Operations:** RW
**Values:**
- **0:** Whitelist (only connect to listed operators)
- **1:** Blacklist (avoid listed operators)

### Resource 8: Available Network Bearers

**Type:** String
**Operations:** R (Read-only)
**Format:** Comma-separated list of currently available bearer IDs

**Example:**
```
"21,41,6"  // WiFi, Ethernet, and LTE available
```

### Resource 9: Acceptable Signal Strength Variation

**Type:** Integer
**Operations:** RW
**Range:** 0-20 dB
**Default:** 5 dB

**Purpose:** Hysteresis to prevent frequent bearer switching (ping-pong effect)

### Resource 10: Higher Priority PLMN Search Timer

**Type:** Integer
**Operations:** RW
**Range:** 60-65535 seconds
**Default:** 600 seconds (10 minutes)

**Purpose:** How often to search for higher priority operators

### Resource 11: Attach Without PDN Connection

**Type:** Boolean
**Operations:** RW
**Default:** false

**Purpose:** For cellular bearers, attach to network without establishing PDN connection

---

## Configuration Examples

### Example 1: WiFi Preferred, Cellular Fallback

```cpp
Instance *bearer = BearerSelection::instance(client);

// Prefer WiFi, then Ethernet, then LTE
bearer->set<STRING_T>(BearerSelection::PREFERRED_COMMS_BEARER_0, "21,41,6");

// Set minimum WiFi signal strength
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSSI_WLAN_4, -75);

// Set minimum LTE signal strength
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSRP_LTE_3, -100);

// Set hysteresis to prevent frequent switching
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_SIGNAL_STRENGTH_VAR_9, 10);
```

### Example 2: Operator Whitelist (Roaming Control)

```cpp
Instance *bearer = BearerSelection::instance(client);

// Only connect to specific operators
bearer->set<STRING_T>(BearerSelection::OPERATOR_LIST_6, "310-410,310-260");
bearer->set<INT_T>(BearerSelection::OPERATOR_LIST_MODE_7,
                    BearerSelection::WHITELIST);
```

### Example 3: Ethernet Only (Fixed Installation)

```cpp
Instance *bearer = BearerSelection::instance(client);

// Use only Ethernet
bearer->set<STRING_T>(BearerSelection::PREFERRED_COMMS_BEARER_0, "41");

// Disable other bearers by setting impossible thresholds
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSSI_WLAN_4, -30);  // Very high
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSRP_LTE_3, -50);   // Very high
```

### Example 4: LTE Preferred with Fallback

```cpp
Instance *bearer = BearerSelection::instance(client);

// Prefer LTE, then WCDMA, then GSM
bearer->set<STRING_T>(BearerSelection::PREFERRED_COMMS_BEARER_0, "6,2,0");

// Aggressive LTE threshold
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSRP_LTE_3, -110);

// Conservative 3G threshold
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSCP_UMTS_2, -95);
```

---

## OpenWRT Integration

### UCI Configuration

Store bearer preferences in UCI:

```bash
# Create bearer configuration
uci set network.bearer=bearer
uci set network.bearer.preference='21,41,6'
uci set network.bearer.wlan_rssi='-70'
uci set network.bearer.lte_rsrp='-95'
uci commit network
```

### Reading Configuration

The object automatically loads from UCI on initialization:

```cpp
void BearerSelection::loadFromUCI() {
    // Read bearer preference
    std::string cmd = "uci get network.bearer.preference 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    // Parse and set PREFERRED_COMMS_BEARER_0
}
```

### Detecting Available Bearers

```bash
# Check Ethernet
cat /sys/class/net/eth0/operstate
# Output: up or down

# Check WiFi
cat /sys/class/net/wlan0/operstate

# Check cellular modem
mmcli -m 0 --output-keyvalue | grep modem.generic.access-technologies
```

---

## Bearer Selection Logic

The device automatically selects the best bearer using this algorithm:

1. **Check Bearer Preference List:** Iterate through preferred bearers
2. **Check Availability:** Is the bearer available?
3. **Check Signal Strength:** Does it meet minimum threshold?
4. **Check Operator:** Is it allowed (whitelist/blacklist)?
5. **Apply Hysteresis:** Only switch if signal difference exceeds variation threshold
6. **Select Bearer:** Use highest priority bearer that meets all criteria

### Hysteresis Example

```
Current: LTE (-95 dBm)
Threshold: -90 dBm
Hysteresis: 5 dB

WiFi Available: -85 dBm

Decision: Stay on LTE
Reason: WiFi signal (-85) is not 5 dB better than LTE (-95)
         |-85| - |-95| = 10 dB difference, but LTE is below threshold

If LTE drops to -100 dBm:
Decision: Switch to WiFi
Reason: LTE below threshold AND WiFi meets requirements
```

---

## Use Cases

### 1. IoT Gateway with WiFi Backup

```cpp
// Prefer Ethernet for reliability, WiFi as backup
bearer->set<STRING_T>(BearerSelection::PREFERRED_COMMS_BEARER_0, "41,21");
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSSI_WLAN_4, -80);
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_SIGNAL_STRENGTH_VAR_9, 5);
```

### 2. Mobile Router with Smart Failover

```cpp
// LTE primary, WiFi when available, Ethernet if connected
bearer->set<STRING_T>(BearerSelection::PREFERRED_COMMS_BEARER_0, "21,6,41");
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSRP_LTE_3, -105);
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_SIGNAL_STRENGTH_VAR_9, 8);
```

### 3. Roaming-Aware Device

```cpp
// Only connect to home operator
bearer->set<STRING_T>(BearerSelection::OPERATOR_LIST_6, "310-410");
bearer->set<INT_T>(BearerSelection::OPERATOR_LIST_MODE_7,
                    BearerSelection::WHITELIST);
```

### 4. Power-Saving Configuration

```cpp
// Prefer low-power bearers
bearer->set<STRING_T>(BearerSelection::PREFERRED_COMMS_BEARER_0, "8,7,21");
// NB-IoT (8), LTE-M (7), WiFi (21)

// Reduce PLMN search frequency to save power
bearer->set<INT_T>(BearerSelection::HIGHER_PRIORITY_PLMN_SEARCH_TIMER_10, 3600);
```

---

## Monitoring

### Check Current Bearer

```cpp
#ifdef OBJ_O_4_CONNECTIVITY_MONITORING
// Get current bearer from Connectivity Monitoring object
Instance *connMon = ConnectivityMonitoring::instance(client);
INT_T currentBearer = connMon->get<INT_T>(
    ConnectivityMonitoring::NETWORK_BEARER_0);

// Map to bearer name
const char* bearerName;
switch (currentBearer) {
    case 21: bearerName = "WiFi"; break;
    case 41: bearerName = "Ethernet"; break;
    case 6: bearerName = "LTE"; break;
    default: bearerName = "Unknown"; break;
}
#endif
```

### Check Available Bearers

```cpp
Instance *bearer = BearerSelection::instance(client);
STRING_T available = bearer->get<STRING_T>(
    BearerSelection::AVAILABLE_NETWORK_BEARERS_8);

// Parse comma-separated list
// Example: "21,41,6" means WiFi, Ethernet, LTE available
```

---

## Troubleshooting

### Issue: Not Switching to Better Bearer

**Causes:**
1. Hysteresis too high
2. Signal threshold too strict
3. Bearer not in preference list

**Solution:**
```cpp
// Reduce hysteresis
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_SIGNAL_STRENGTH_VAR_9, 3);

// Relax signal thresholds
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_RSSI_WLAN_4, -85);
```

### Issue: Frequent Bearer Switching

**Causes:**
1. Hysteresis too low
2. Competing bearers with similar signal

**Solution:**
```cpp
// Increase hysteresis
bearer->set<INT_T>(BearerSelection::ACCEPTABLE_SIGNAL_STRENGTH_VAR_9, 10);
```

### Issue: Cannot Connect to Preferred Operator

**Check:**
```bash
# Verify operator list
uci get network.bearer.operators

# Check operator list mode
uci get network.bearer.operator_mode
```

**Solution:**
```cpp
// Verify operator code format
bearer->set<STRING_T>(BearerSelection::OPERATOR_LIST_6, "310-410");
// MCC-MNC format: MCC=310 (USA), MNC=410 (AT&T)
```

---

## Best Practices

1. **Set Realistic Thresholds:**
   - Don't set thresholds too high
   - Consider typical signal levels in deployment area

2. **Use Appropriate Hysteresis:**
   - Mobile deployments: 5-8 dB
   - Fixed deployments: 3-5 dB

3. **Define Clear Preferences:**
   - Order bearers by reliability and cost
   - Consider power consumption

4. **Test Failover:**
   - Simulate bearer failures
   - Verify automatic switching

5. **Monitor Bearer Changes:**
   - Log bearer switching events
   - Analyze switching patterns

---

## Integration with Other Objects

### Connectivity Monitoring (ID 4)

```cpp
// Bearer Selection sets preferences
// Connectivity Monitoring reports current bearer

Instance *bearer = BearerSelection::instance(client);
Instance *connMon = ConnectivityMonitoring::instance(client);

// Get preferred bearers
STRING_T preferred = bearer->get<STRING_T>(
    BearerSelection::PREFERRED_COMMS_BEARER_0);

// Get current active bearer
INT_T current = connMon->get<INT_T>(
    ConnectivityMonitoring::NETWORK_BEARER_0);
```

### WLAN Connectivity (ID 12)

```cpp
// Use WiFi signal strength for bearer selection
Instance *wlan = WlanConnectivity::instance(client, 0);
Instance *bearer = BearerSelection::instance(client);

// Read WiFi statistics
INT_T wifiAvailable = wlan->get<INT_T>(WlanConnectivity::STATUS_3);

if (wifiAvailable == WlanConnectivity::UP) {
    // WiFi is available for bearer selection
}
```

---

## References

- [OMA LwM2M Object 13 Specification](https://github.com/OpenMobileAlliance/lwm2m-registry/blob/prod/13.xml)
- [3GPP PLMN Selection](https://www.3gpp.org/technologies/plmn)
- [Network Bearer Types](https://github.com/OpenMobileAlliance/lwm2m-registry/blob/prod/4.xml)
