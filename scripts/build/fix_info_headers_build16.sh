#!/bin/bash

# Fix Info.h files for build16 - Convert from Lwm2mObjectMetaInfo to ObjectInfo structure
# This script fixes all Info.h files to use the correct WPP framework ObjectInfo structure

set -e

echo "==================================="
echo "Fixing Info.h Files for Build16"
echo "==================================="
echo ""

# Color codes
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counter
FILES_FIXED=0

# Function to fix an Info.h file
fix_info_file() {
    local info_file=$1
    local object_name=$2
    local object_id=$3
    local display_name=$4
    local is_multiple=$5  # "MULTIPLE" or "SINGLE"

    echo -e "${BLUE}Fixing: ${info_file}${NC}"

    # Read the file content
    content=$(cat "$info_file")

    # Extract the URN from the old structure
    urn=$(echo "$content" | grep -oP 'urn:waltech:lwm2m:[0-9]+' | head -1)

    # Create new content with correct ObjectInfo structure
    cat > "$info_file" << EOF
/*
 * ${object_name} Info (Object ${object_id})
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_${object_name^^}_INFO_H
#define WPP_${object_name^^}_INFO_H
#include "ObjectInfo.h"


#ifdef OBJ_W_${object_id}_${object_name^^}

namespace wpp {

inline const ObjectInfo ${object_name^^}_OBJ_INFO = {
    /* Name */
    "${display_name}",
    /* Object ID */
    static_cast<OBJ_ID>(${object_id}),
    /* URN */
    "${urn}",
    /* Object version */
    {1, 0},
    /* LwM2M version */
    {1, 1},
    /* Is single */
    IS_SINGLE::${is_multiple},
    /* Is mandatory */
    IS_MANDATORY::OPTIONAL,
    /* Object supported operations */
    ItemOp(ItemOp::READ | ItemOp::WRITE | ItemOp::DISCOVER | ItemOp::EXECUTE)
};

} /* namespace wpp */

#endif // OBJ_W_${object_id}_${object_name^^}
#endif // WPP_${object_name^^}_INFO_H
EOF

    echo -e "${GREEN}✓ Fixed ${info_file}${NC}"
    ((FILES_FIXED++))
}

# Function to add OBJECT_ID to Config.h if not present
add_object_id_to_config() {
    local config_file=$1
    local object_name=$2
    local object_id=$3

    # Check if OBJECT_ID already defined
    if grep -q "${object_name^^}_OBJECT_ID" "$config_file"; then
        echo "  Object ID already defined in $(basename $config_file)"
        return
    fi

    echo -e "${BLUE}Adding OBJECT_ID to: ${config_file}${NC}"

    # Find the line with "Resource enable/disable flags"
    # Insert the OBJECT_ID definition before it
    sed -i "/\/\/ Resource enable\/disable flags/i \\
// Object ID\\
#define ${object_name^^}_OBJECT_ID ${object_id}\\
" "$config_file"

    echo -e "${GREEN}✓ Added OBJECT_ID to ${config_file}${NC}"
}

# Base directory
BASE="wpp/registry/objects"

echo "Processing WifiClientManagement (10527)..."
fix_info_file \
    "${BASE}/w_10527_wifi_client_management/WifiClientManagementInfo.h" \
    "WifiClientManagement" \
    "10527" \
    "WiFi Client Management" \
    "MULTIPLE"
# Object ID already added in previous fix

echo ""
echo "Processing WifiChannelOptimization (10528)..."
fix_info_file \
    "${BASE}/w_10528_wifi_channel_optimization/WifiChannelOptimizationInfo.h" \
    "WifiChannelOptimization" \
    "10528" \
    "WiFi Channel Optimization" \
    "SINGLE"
# Object ID already added in previous fix

echo ""
echo "Processing MatterBridge (10529)..."
if [ -f "${BASE}/w_10529_matter_bridge/MatterBridgeInfo.h" ]; then
    fix_info_file \
        "${BASE}/w_10529_matter_bridge/MatterBridgeInfo.h" \
        "MatterBridge" \
        "10529" \
        "Matter Bridge" \
        "SINGLE"
    add_object_id_to_config \
        "${BASE}/w_10529_matter_bridge/MatterBridgeConfig.h" \
        "MatterBridge" \
        "10529"
fi

echo ""
echo "Processing MatterDevice (10530)..."
if [ -f "${BASE}/w_10530_matter_device/MatterDeviceInfo.h" ]; then
    fix_info_file \
        "${BASE}/w_10530_matter_device/MatterDeviceInfo.h" \
        "MatterDevice" \
        "10530" \
        "Matter Device" \
        "MULTIPLE"
    add_object_id_to_config \
        "${BASE}/w_10530_matter_device/MatterDeviceConfig.h" \
        "MatterDevice" \
        "10530"
fi

echo ""
echo "Processing ThreadNetwork (10531)..."
if [ -f "${BASE}/w_10531_thread_network/ThreadNetworkInfo.h" ]; then
    fix_info_file \
        "${BASE}/w_10531_thread_network/ThreadNetworkInfo.h" \
        "ThreadNetwork" \
        "10531" \
        "Thread Network" \
        "SINGLE"
    add_object_id_to_config \
        "${BASE}/w_10531_thread_network/ThreadNetworkConfig.h" \
        "ThreadNetwork" \
        "10531"
fi

echo ""
echo "Processing ZigbeeCoordinator (10532)..."
if [ -f "${BASE}/w_10532_zigbee_coordinator/ZigbeeCoordinatorInfo.h" ]; then
    fix_info_file \
        "${BASE}/w_10532_zigbee_coordinator/ZigbeeCoordinatorInfo.h" \
        "ZigbeeCoordinator" \
        "10532" \
        "Zigbee Coordinator" \
        "SINGLE"
    add_object_id_to_config \
        "${BASE}/w_10532_zigbee_coordinator/ZigbeeCoordinatorConfig.h" \
        "ZigbeeCoordinator" \
        "10532"
fi

echo ""
echo "Processing ZigbeeDevice (10533)..."
if [ -f "${BASE}/w_10533_zigbee_device/ZigbeeDeviceInfo.h" ]; then
    fix_info_file \
        "${BASE}/w_10533_zigbee_device/ZigbeeDeviceInfo.h" \
        "ZigbeeDevice" \
        "10533" \
        "Zigbee Device" \
        "MULTIPLE"
    add_object_id_to_config \
        "${BASE}/w_10533_zigbee_device/ZigbeeDeviceConfig.h" \
        "ZigbeeDevice" \
        "10533"
fi

echo ""
echo "Processing ZigbeeGroup (10534)..."
if [ -f "${BASE}/w_10534_zigbee_group/ZigbeeGroupInfo.h" ]; then
    fix_info_file \
        "${BASE}/w_10534_zigbee_group/ZigbeeGroupInfo.h" \
        "ZigbeeGroup" \
        "10534" \
        "Zigbee Group" \
        "MULTIPLE"
    add_object_id_to_config \
        "${BASE}/w_10534_zigbee_group/ZigbeeGroupConfig.h" \
        "ZigbeeGroup" \
        "10534"
fi

echo ""
echo "Processing LanConfiguration (10535)..."
if [ -f "${BASE}/w_10535_lan_configuration/LanConfigurationInfo.h" ]; then
    fix_info_file \
        "${BASE}/w_10535_lan_configuration/LanConfigurationInfo.h" \
        "LanConfiguration" \
        "10535" \
        "LAN Configuration" \
        "MULTIPLE"
    add_object_id_to_config \
        "${BASE}/w_10535_lan_configuration/LanConfigurationConfig.h" \
        "LanConfiguration" \
        "10535"
fi

echo ""
echo "Processing RoutingTable (10536)..."
if [ -f "${BASE}/w_10536_routing_table/RoutingTableInfo.h" ]; then
    fix_info_file \
        "${BASE}/w_10536_routing_table/RoutingTableInfo.h" \
        "RoutingTable" \
        "10536" \
        "Routing Table" \
        "MULTIPLE"
    add_object_id_to_config \
        "${BASE}/w_10536_routing_table/RoutingTableConfig.h" \
        "RoutingTable" \
        "10536"
fi

echo ""
echo "Processing VpnConfiguration (10537)..."
if [ -f "${BASE}/w_10537_vpn_configuration/VpnConfigurationInfo.h" ]; then
    fix_info_file \
        "${BASE}/w_10537_vpn_configuration/VpnConfigurationInfo.h" \
        "VpnConfiguration" \
        "10537" \
        "VPN Configuration" \
        "MULTIPLE"
    add_object_id_to_config \
        "${BASE}/w_10537_vpn_configuration/VpnConfigurationConfig.h" \
        "VpnConfiguration" \
        "10537"
fi

echo ""
echo "==================================="
echo -e "${GREEN}✓ Info.h Fix Complete${NC}"
echo "==================================="
echo "Files fixed: ${FILES_FIXED}"
echo ""
echo "Next: Run build to generate build17.log"
