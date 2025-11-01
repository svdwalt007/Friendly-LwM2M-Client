#!/bin/bash
#
# OpenWRT One LwM2M Object Generation Script
# Generates boilerplate code for all required LwM2M objects
# Walt Technologies
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
OBJECTS_DIR="$PROJECT_ROOT/wpp/registry/objects"

echo "=== OpenWRT One LwM2M Object Generator ==="
echo "Project Root: $PROJECT_ROOT"
echo "Objects Directory: $OBJECTS_DIR"
echo ""

# Function to create object directory structure
create_object_dir() {
    local obj_id=$1
    local obj_name=$2
    local obj_prefix=$3  # o_ for OMA, w_ for Walt Tech

    local dir_name="${obj_prefix}_${obj_id}_${obj_name}"
    local full_path="$OBJECTS_DIR/$dir_name"

    echo "Creating object: $obj_name (ID: $obj_id) at $dir_name"
    mkdir -p "$full_path"
    echo "  ✓ Directory created"
}

# OMA Standard Objects
echo "--- Creating OMA Standard Objects ---"
create_object_dir "12" "wlan_connectivity" "o"
create_object_dir "13" "bearer_selection" "o"
create_object_dir "3331" "memory" "o"

# Walt Technologies Custom Objects
echo ""
echo "--- Creating Walt Technologies Custom Objects ---"
create_object_dir "34601" "router_management" "w"
create_object_dir "34602" "ethernet_interface" "w"
create_object_dir "34603" "gpio_control" "w"
create_object_dir "34604" "usb_management" "w"
create_object_dir "34605" "storage_management" "w"
create_object_dir "34606" "system_monitor" "w"

echo ""
echo "=== Object directories created successfully ==="
echo ""
echo "Next steps:"
echo "1. Implement object classes in each directory"
echo "2. Add CMakeLists.txt for each object"
echo "3. Update wpp/registry/WppRegistry.h to include new objects"
echo "4. Update examples/objects.cpp to initialize new objects"
echo ""
