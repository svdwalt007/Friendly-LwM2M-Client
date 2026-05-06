#!/bin/bash
# Template for restoring validation logic to advanced WPP objects
# This script shows the pattern - each object needs custom validation rules

cat << 'EOF'
================================================================================
Validation Restoration Template for Advanced WPP Objects
================================================================================

This template shows how to restore validation logic that was removed when
the invalid validate() override method was deleted.

IMPORTANT: Each object needs custom validation rules based on its resources.
This is a TEMPLATE - copy and modify for each specific object.

--------------------------------------------------------------------------------
Step 1: Identify Original Validation Rules
--------------------------------------------------------------------------------

Look in the object's header file for resource IDs and understand what each
resource represents. Common validation patterns:

1. Interface names: Check if interface exists in /sys/class/net/
2. Integer ranges: Min/max bounds (weights: 1-100, thresholds: 1-20, etc.)
3. Enum values: MODE_FAILOVER, MODE_LOADBALANCE, etc.
4. String formats: "interface:protocol:port", IP addresses, etc.
5. File paths: Check if paths exist
6. Port numbers: 1-65535 range

--------------------------------------------------------------------------------
Step 2: Add Data Verifiers in resourcesCreate()
--------------------------------------------------------------------------------

PATTERN:
--------
void ClassName::resourcesCreate() {
    std::vector<Resource> resources = {
        // ... resource definitions
    };
    setupResources(std::move(resources));

    // Add data verifiers AFTER setupResources()
    resource(RESOURCE_ID)->setDataVerifier([this](const TYPE& value) {
        if (!isValid(value)) {
            WPP_LOGE(TAG, "Validation error message");
            return false;
        }
        return true;
    });
}

EXAMPLE (from WanFailoverPolicy):
----------------------------------
void WanFailoverPolicy::resourcesCreate() {
    std::vector<Resource> resources = {
        {PRIMARY_WAN_1, ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING},
        {MODE_4, ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT},
        {PRIMARY_WEIGHT_5, ItemOp(ItemOp::READ | ItemOp::WRITE), IS_SINGLE::SINGLE, IS_MANDATORY::OPTIONAL, TYPE_ID::INT},
        // ... more resources
    };
    setupResources(std::move(resources));

    // Interface name validation
    resource(PRIMARY_WAN_1)->setDataVerifier([this](const STRING_T& ifname) {
        if (ifname.empty()) return true;
        if (!isValidInterface(ifname)) {
            WPP_LOGE(TAG, "Invalid interface: %s", ifname.c_str());
            return false;
        }
        return true;
    });

    // Integer range validation
    resource(MODE_4)->setDataVerifier([](const INT_T& mode) {
        if (mode < MODE_FAILOVER || mode > MODE_ROUNDROBIN) {
            WPP_LOGE(TAG, "Invalid mode: %lld", mode);
            return false;
        }
        return true;
    });

    // Weight range validation
    resource(PRIMARY_WEIGHT_5)->setDataVerifier([](const INT_T& weight) {
        if (weight < 1 || weight > 100) {
            WPP_LOGE(TAG, "Invalid weight: %lld (1-100)", weight);
            return false;
        }
        return true;
    });
}

--------------------------------------------------------------------------------
Step 3: Common Validation Patterns
--------------------------------------------------------------------------------

1. STRING VALIDATION - Interface Names:
   -------------------------------------
   resource(INTERFACE_RES)->setDataVerifier([this](const STRING_T& ifname) {
       if (ifname.empty()) return true;  // Allow empty for optional resources
       return isValidInterface(ifname);
   });

2. INT VALIDATION - Range Check:
   -------------------------------
   resource(THRESHOLD_RES)->setDataVerifier([](const INT_T& value) {
       return value >= MIN_VALUE && value <= MAX_VALUE;
   });

3. INT VALIDATION - Enum/State:
   -----------------------------
   resource(STATE_RES)->setDataVerifier([](const INT_T& state) {
       return state == STATE_A || state == STATE_B || state == STATE_C;
   });

4. STRING VALIDATION - Format Check:
   ----------------------------------
   resource(RULE_RES)->setDataVerifier([](const STRING_T& rule) {
       // Example: "interface:protocol:port" format
       size_t colon1 = rule.find(':');
       size_t colon2 = rule.find(':', colon1 + 1);
       return colon1 != std::string::npos && colon2 != std::string::npos;
   });

5. STRING VALIDATION - IP Address:
   --------------------------------
   resource(IP_RES)->setDataVerifier([](const STRING_T& ip) {
       // Basic IPv4 format check (for more robust, use inet_pton)
       std::regex ipv4_regex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
       return std::regex_match(ip, ipv4_regex);
   });

6. INT VALIDATION - Port Number:
   ------------------------------
   resource(PORT_RES)->setDataVerifier([](const INT_T& port) {
       return port >= 1 && port <= 65535;
   });

7. STRING VALIDATION - File Path:
   -------------------------------
   resource(PATH_RES)->setDataVerifier([](const STRING_T& path) {
       if (path.empty()) return true;
       struct stat buffer;
       return stat(path.c_str(), &buffer) == 0;
   });

8. BOOL VALIDATION (usually no validation needed):
   ------------------------------------------------
   // Boolean resources don't usually need validation
   // They're either true or false

9. TIME VALIDATION - Timestamp:
   -----------------------------
   resource(TIME_RES)->setDataVerifier([](const TIME_T& timestamp) {
       // Ensure timestamp is not in the future by more than a threshold
       time_t now = time(nullptr);
       return timestamp <= now + 3600;  // Allow 1 hour future for clock skew
   });

10. OPAQUE VALIDATION - Binary Data:
    ---------------------------------
    resource(BINARY_RES)->setDataVerifier([](const OPAQUE_T& data) {
        // Check size constraints
        return data.size() >= MIN_SIZE && data.size() <= MAX_SIZE;
    });

--------------------------------------------------------------------------------
Step 4: Lambda Capture Rules
--------------------------------------------------------------------------------

When to use [this]:
- When you need to call member functions: isValidInterface(), checkSomething()
- When you need to access member variables

When to use []:
- When validation only uses the parameter value and constants
- When validation is purely algorithmic

Example:
   resource(RES)->setDataVerifier([this](const STRING_T& val) {
       return this->memberFunction(val);  // Needs [this]
   });

   resource(RES)->setDataVerifier([](const INT_T& val) {
       return val >= 1 && val <= 100;  // No [this] needed
   });

--------------------------------------------------------------------------------
Step 5: Testing Validation
--------------------------------------------------------------------------------

After adding validation, test with:

1. Valid values (should succeed)
2. Invalid values (should fail with error log)
3. Boundary values (min, max, min-1, max+1)
4. Empty strings (for optional resources)
5. Null/zero values

Example test in LwM2M server:
   Write /10525/0/5 100     # Should succeed (max weight)
   Write /10525/0/5 101     # Should fail (over max)
   Write /10525/0/5 0       # Should fail (under min)
   Write /10525/0/1 "eth0"  # Should succeed if eth0 exists
   Write /10525/0/1 "fake0" # Should fail if fake0 doesn't exist

--------------------------------------------------------------------------------
Step 6: Object-Specific Implementation Checklist
--------------------------------------------------------------------------------

For each of the 13 advanced objects:

□ 10525 WanFailoverPolicy - DONE (reference implementation)
□ 10526 MultiWanHealthCheck - TODO
□ 10527 WifiClientManagement - TODO
□ 10528 WifiChannelOptimization - TODO
□ 10529 MatterBridge - TODO
□ 10530 MatterDevice - TODO
□ 10531 ThreadNetwork - TODO
□ 10532 ZigbeeCoordinator - TODO
□ 10533 ZigbeeDevice - TODO
□ 10534 ZigbeeGroup - TODO
□ 10535 LanConfiguration - TODO
□ 10536 RoutingTable - TODO
□ 10537 VpnConfiguration - TODO

For each object:
1. Review resource definitions in *Config.h header
2. Identify which resources need validation
3. Add appropriate setDataVerifier() calls in resourcesCreate()
4. Test with valid and invalid values
5. Mark as complete

--------------------------------------------------------------------------------
REFERENCE: Working Example from WanFailoverPolicy.cpp
--------------------------------------------------------------------------------

See: wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.cpp

Complete implementation with 11 data verifiers covering:
- Interface name validation (3 resources)
- Integer range validation (7 resources)
- String format validation (1 resource)

This is the gold standard - use it as a reference for other objects.

================================================================================
End of Template
================================================================================
EOF
