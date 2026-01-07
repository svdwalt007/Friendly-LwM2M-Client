# WLAN Connectivity Object (ID 12)

Complete reference documentation for the OMA LwM2M WLAN Connectivity object.

---

## Overview

The WLAN Connectivity object (ID 12) provides comprehensive WiFi interface management for both 2.4GHz and 5GHz bands. It supports multiple instances for managing different WiFi radios and includes configuration, monitoring, and statistics capabilities.

**Object ID:** 12
**URN:** urn:oma:lwm2m:oma:12
**Multiple Instances:** Yes
**Mandatory:** No

---

## Key Features

- **Dual-Band Support:** 2.4GHz and 5GHz WiFi management
- **Multiple Modes:** Access Point, Client, Bridge, Repeater
- **Security:** WPA/WPA2/WPA3, WEP, RADIUS authentication
- **Statistics:** Comprehensive traffic and error counters
- **OpenWRT Integration:** UCI configuration and sysfs statistics
- **Dynamic Configuration:** Runtime WiFi parameter updates

---

## Resource Summary

| Group | Resources | Description |
|-------|-----------|-------------|
| Interface Info | 0-3 | Interface name, enable status |
| SSID Configuration | 4-7 | BSSID, SSID, broadcast settings |
| Channel Management | 8-13 | Channel, auto-channel, regulatory domain |
| WiFi Standard | 14 | 802.11a/b/g/n/ac/ax support |
| Security | 15-28 | Authentication, encryption, keys, RADIUS |
| QoS | 29-30 | WMM support and configuration |
| Access Control | 31-32 | MAC filtering |
| Statistics | 33-47 | Traffic counters and error statistics |

---

## Complete Resource Reference

### Interface Information (0-3)

| ID | Name | Operations | Type | Description |
|----|------|------------|------|-------------|
| 0 | Interface Name | R | String | Network interface name (e.g., "wlan0", "wlan1") |
| 1 | Enable | RW | Boolean | Enable/disable interface |
| 2 | Radio Enabled | RW | Boolean | Enable/disable radio hardware |
| 3 | Status | R | Integer | Interface status: 0=Disabled, 1=Up, 2=Error |

### SSID Configuration (4-7)

| ID | Name | Operations | Type | Description |
|----|------|------------|------|-------------|
| 4 | BSSID | R | String | MAC address of access point |
| 5 | SSID | RW | String | Network name (1-32 characters) |
| 6 | Broadcast SSID | RW | Boolean | Show/hide SSID in beacons |
| 7 | Beacon Enabled | RW | Boolean | Enable beacon transmission |

### Channel Management (8-13)

| ID | Name | Operations | Type | Description |
|----|------|------------|------|-------------|
| 8 | Mode | RW | Integer | Operating mode: 0=AP, 1=Client, 2=Bridge, 3=Repeater |
| 9 | Channel | RW | Integer | WiFi channel number |
| 10 | Auto Channel | RW | Boolean | Automatic channel selection |
| 11 | Supported Channels | R | String | Comma-separated list of supported channels |
| 12 | Channels In Use | R | String | Channels currently in use by nearby APs |
| 13 | Regulatory Domain | RW | String | Country code (e.g., "US", "GB", "JP") |

**2.4GHz Channels:**
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13

**5GHz Channels:**
36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165

### WiFi Standard (14)

| ID | Name | Operations | Type | Description |
|----|------|------------|------|-------------|
| 14 | Standard | RW | Integer | WiFi standard |

**Values:**
- 0 = 802.11a
- 1 = 802.11b
- 2 = 802.11bg
- 3 = 802.11g
- 4 = 802.11n
- 5 = 802.11bgn
- 6 = 802.11ac (WiFi 5)
- 8 = 802.11ax (WiFi 6)

### Security Configuration (15-28)

| ID | Name | Operations | Type | Description |
|----|------|------------|------|-------------|
| 15 | Authentication Mode | RW | Integer | 0=None, 1=PSK, 2=EAP, 3=EAP-PSK, 4=EAPSIM |
| 16 | Encryption Mode | RW | Integer | 0=AES(WPA2), 1=TKIP(WPA), 2=WEP |
| 17 | WPA Pre Shared Key | W | String | WPA pre-shared key |
| 18 | WPA Key Phrase | W | String | WPA passphrase (8-63 characters) |
| 19 | WEP Encryption Type | RW | Integer | 0=None, 1=40-bit, 2=104-bit |
| 20 | WEP Key Index | RW | Integer | Active WEP key index (1-4) |
| 21 | WEP Key Phrase | W | String | WEP passphrase |
| 22-25 | WEP Key 1-4 | W | String | WEP keys |
| 26 | RADIUS Server | RW | String | RADIUS server IP address |
| 27 | RADIUS Server Port | RW | Integer | RADIUS port (default: 1812) |
| 28 | RADIUS Secret | W | String | RADIUS shared secret |

### Quality of Service (29-30)

| ID | Name | Operations | Type | Description |
|----|------|------------|------|-------------|
| 29 | WMM Supported | R | Boolean | WiFi Multimedia support |
| 30 | WMM Enabled | RW | Boolean | Enable WMM |

### MAC Filtering (31-32)

| ID | Name | Operations | Type | Description |
|----|------|------------|------|-------------|
| 31 | MAC Control Enabled | RW | Boolean | Enable MAC filtering |
| 32 | MAC Address List | RW | String | Comma-separated MAC addresses |

### Statistics (33-47)

| ID | Name | Operations | Type | Description |
|----|------|------------|------|-------------|
| 33 | Total Bytes Sent | R | Integer | Total transmitted bytes |
| 34 | Total Bytes Received | R | Integer | Total received bytes |
| 35 | Total Packets Sent | R | Integer | Total transmitted packets |
| 36 | Total Packets Received | R | Integer | Total received packets |
| 37 | Transmit Errors | R | Integer | TX error count |
| 38 | Receive Errors | R | Integer | RX error count |
| 39 | Unicast Packets Sent | R | Integer | Unicast TX packets |
| 40 | Unicast Packets Received | R | Integer | Unicast RX packets |
| 41 | Multicast Packets Sent | R | Integer | Multicast TX packets |
| 42 | Multicast Packets Received | R | Integer | Multicast RX packets |
| 43 | Broadcast Packets Sent | R | Integer | Broadcast TX packets |
| 44 | Broadcast Packets Received | R | Integer | Broadcast RX packets |
| 45 | Discard Packets Sent | R | Integer | Dropped TX packets |
| 46 | Discard Packets Received | R | Integer | Dropped RX packets |
| 47 | Unknown Packets Received | R | Integer | Unknown protocol packets |

---

## Multi-Instance Configuration

### Instance 0: 2.4GHz Radio (wlan0)

```cpp
Instance *wlan0 = WlanConnectivity::createInst(client, 0);

// Configure as Access Point
wlan0->set<STRING_T>(WlanConnectivity::SSID_5, "MyNetwork-2.4G");
wlan0->set<INT_T>(WlanConnectivity::MODE_8, WlanConnectivity::ACCESS_POINT);
wlan0->set<INT_T>(WlanConnectivity::CHANNEL_9, 6);
wlan0->set<INT_T>(WlanConnectivity::STANDARD_14, WlanConnectivity::IEEE_802_11BGN);

// WPA2 Security
wlan0->set<INT_T>(WlanConnectivity::AUTHENTICATION_MODE_15, WlanConnectivity::AUTH_PSK);
wlan0->set<INT_T>(WlanConnectivity::ENCRYPTION_MODE_16, WlanConnectivity::ENC_AES);
wlan0->set<STRING_T>(WlanConnectivity::WPA_KEY_PHRASE_18, "SecurePassword123");
```

### Instance 1: 5GHz Radio (wlan1)

```cpp
Instance *wlan1 = WlanConnectivity::createInst(client, 1);

// Configure as Access Point
wlan1->set<STRING_T>(WlanConnectivity::SSID_5, "MyNetwork-5G");
wlan1->set<INT_T>(WlanConnectivity::MODE_8, WlanConnectivity::ACCESS_POINT);
wlan1->set<INT_T>(WlanConnectivity::CHANNEL_9, 36);
wlan1->set<INT_T>(WlanConnectivity::STANDARD_14, WlanConnectivity::IEEE_802_11AC);

// WPA2 Security
wlan1->set<INT_T>(WlanConnectivity::AUTHENTICATION_MODE_15, WlanConnectivity::AUTH_PSK);
wlan1->set<INT_T>(WlanConnectivity::ENCRYPTION_MODE_16, WlanConnectivity::ENC_AES);
wlan1->set<STRING_T>(WlanConnectivity::WPA_KEY_PHRASE_18, "SecurePassword123");
```

---

## OpenWRT Integration

### UCI Configuration

The object automatically loads configuration from OpenWRT UCI:

```bash
# Read SSID from UCI
uci get wireless.@wifi-iface[0].ssid

# Read channel from radio configuration
uci get wireless.radio0.channel

# Set new SSID
uci set wireless.@wifi-iface[0].ssid='NewNetwork'
uci commit wireless
wifi reload
```

### sysfs Statistics

Statistics are automatically updated from `/sys/class/net/`:

```bash
# Interface statistics path
/sys/class/net/wlan0/statistics/

# Available statistics
tx_bytes      # Resource 33
rx_bytes      # Resource 34
tx_packets    # Resource 35
rx_packets    # Resource 36
tx_errors     # Resource 37
rx_errors     # Resource 38
```

### Interface Status

Interface operational state:

```bash
# Check if interface is up
cat /sys/class/net/wlan0/operstate
# Output: up, down, or unknown
```

---

## Use Cases

### 1. Dual-Band Access Point

```cpp
// 2.4GHz for compatibility
Instance *wlan24 = WlanConnectivity::createInst(client, 0);
wlan24->set<STRING_T>(WlanConnectivity::SSID_5, "MyNetwork");
wlan24->set<INT_T>(WlanConnectivity::CHANNEL_9, 6);
wlan24->set<INT_T>(WlanConnectivity::STANDARD_14, WlanConnectivity::IEEE_802_11BGN);

// 5GHz for performance
Instance *wlan5 = WlanConnectivity::createInst(client, 1);
wlan5->set<STRING_T>(WlanConnectivity::SSID_5, "MyNetwork");
wlan5->set<INT_T>(WlanConnectivity::CHANNEL_9, 36);
wlan5->set<INT_T>(WlanConnectivity::STANDARD_14, WlanConnectivity::IEEE_802_11AC);
```

### 2. Guest Network (Separate SSID)

```cpp
Instance *guest = WlanConnectivity::createInst(client, 2);
guest->set<STRING_T>(WlanConnectivity::SSID_5, "Guest-Network");
guest->set<INT_T>(WlanConnectivity::AUTHENTICATION_MODE_15, WlanConnectivity::AUTH_NONE);
guest->set<BOOL_T>(WlanConnectivity::BROADCAST_SSID_6, true);
```

### 3. MAC Filtering

```cpp
Instance *wlan = WlanConnectivity::instance(client, 0);
wlan->set<BOOL_T>(WlanConnectivity::MAC_CONTROL_ENABLED_31, true);
wlan->set<STRING_T>(WlanConnectivity::MAC_ADDRESS_LIST_32,
                    "AA:BB:CC:DD:EE:FF,11:22:33:44:55:66");
```

### 4. RADIUS Authentication

```cpp
Instance *wlan = WlanConnectivity::instance(client, 0);
wlan->set<INT_T>(WlanConnectivity::AUTHENTICATION_MODE_15, WlanConnectivity::AUTH_EAP);
wlan->set<STRING_T>(WlanConnectivity::RADIUS_SERVER_26, "192.168.1.100");
wlan->set<INT_T>(WlanConnectivity::RADIUS_SERVER_PORT_27, 1812);
wlan->set<STRING_T>(WlanConnectivity::RADIUS_SECRET_28, "radiussecret");
```

---

## Monitoring and Statistics

### Traffic Monitoring

```cpp
Instance *wlan = WlanConnectivity::instance(client, 0);

// Get traffic statistics
INT_T txBytes = wlan->get<INT_T>(WlanConnectivity::TOTAL_BYTES_SENT_33);
INT_T rxBytes = wlan->get<INT_T>(WlanConnectivity::TOTAL_BYTES_RECEIVED_34);

// Calculate throughput
float throughputMbps = (txBytes + rxBytes) * 8.0 / 1000000.0 / periodSeconds;
```

### Error Monitoring

```cpp
// Check for errors
INT_T txErrors = wlan->get<INT_T>(WlanConnectivity::TRANSMIT_ERRORS_37);
INT_T rxErrors = wlan->get<INT_T>(WlanConnectivity::RECEIVE_ERRORS_38);

if (txErrors > threshold || rxErrors > threshold) {
    // Take corrective action
}
```

---

## Troubleshooting

### Issue: Interface Not Coming Up

**Check:**
```bash
# Verify interface exists
ip link show wlan0

# Check radio status
uci get wireless.radio0.disabled

# View system logs
logread | grep wlan
```

**Solution:**
```cpp
wlan->set<BOOL_T>(WlanConnectivity::ENABLE_1, true);
wlan->set<BOOL_T>(WlanConnectivity::RADIO_ENABLED_2, true);
```

### Issue: No Clients Connecting

**Check:**
1. SSID broadcast enabled
2. Security settings match client
3. Channel not congested
4. MAC filtering not blocking clients

### Issue: Poor Performance

**Actions:**
1. Change to less congested channel
2. Use 5GHz instead of 2.4GHz
3. Enable 802.11ac/ax standards
4. Enable WMM for QoS

---

## Best Practices

1. **Channel Selection:**
   - 2.4GHz: Use channels 1, 6, or 11 to avoid overlap
   - 5GHz: Use DFS channels carefully (52-144)

2. **Security:**
   - Always use WPA2 or WPA3
   - Use strong passphrases (12+ characters)
   - Consider RADIUS for enterprise deployments

3. **Performance:**
   - Enable 802.11ac for 5GHz
   - Enable WMM for better QoS
   - Use 40MHz or 80MHz channel width

4. **Monitoring:**
   - Check statistics every 30 seconds
   - Monitor error rates
   - Track client connections

---

## Related Objects

- **Connectivity Monitoring (ID 4):** Network bearer information
- **Bearer Selection (ID 13):** Network bearer selection preferences

---

## References

- [OMA LwM2M Object 12 Specification](https://github.com/OpenMobileAlliance/lwm2m-registry/blob/prod/12.xml)
- [OpenWRT Wireless Configuration](https://openwrt.org/docs/guide-user/network/wifi/basic)
- [IEEE 802.11 Standards](https://www.ieee802.org/11/)
