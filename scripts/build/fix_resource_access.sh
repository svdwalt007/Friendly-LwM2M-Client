#!/bin/bash
# Fix resource access patterns in advanced object implementation files
# Changes get<TYPE>(RES) to resource(RES)->get<TYPE>()
# Changes set<TYPE>(RES, val) to resource(RES)->set<TYPE>(val)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "======================================"
echo "Fixing Resource Access Patterns"
echo "======================================"
echo ""

# Files to fix
FILES=(
    "wpp/registry/objects/w_10537_vpn_configuration/VpnConfiguration.cpp"
    "wpp/registry/objects/w_10535_lan_configuration/LanConfiguration.cpp"
    "wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.cpp"
    "wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.cpp"
    "wpp/registry/objects/w_10529_matter_bridge/MatterBridge.cpp"
    "wpp/registry/objects/w_10530_matter_device/MatterDevice.cpp"
    "wpp/registry/objects/w_10531_thread_network/ThreadNetwork.cpp"
)

cd "$PROJECT_ROOT"

for file in "${FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo "⚠ File not found: $file"
        continue
    fi

    echo "Processing: $file"

    # Pattern 1: vpn.set<TYPE>(RES, val) -> vpn.resource(RES)->set<TYPE>(val)
    # This is complex because val can be a string with commas, function calls, etc.
    # We'll handle common simple cases

    # Pattern 2: vpn.get<TYPE>(RES) -> vpn.resource(RES)->get<TYPE>()
    # Pattern 3: this->get<TYPE>(RES) -> resource(RES)->get<TYPE>()
    # Pattern 4: set<TYPE>(RES, val) -> resource(RES)->set<TYPE>(val)
    # Pattern 5: get<TYPE>(RES) -> resource(RES)->get<TYPE>()

    # Backup
    cp "$file" "$file.bak"

    # Use perl for more sophisticated regex
    # Fix: vpn.get<TYPE>(RES) -> vpn.resource(RES)->get<TYPE>()
    perl -i -pe 's/(\w+)\.get<(\w+)>\(([A-Z_0-9]+)\)/$1.resource($3)->get<$2>()/g' "$file"

    # Fix: get<TYPE>(RES) -> resource(RES)->get<TYPE>() (without object prefix)
    perl -i -pe 's/(?<!\.)\bget<(\w+)>\(([A-Z_0-9]+)\)/resource($2)->get<$1>()/g' "$file"

    # Fix simple set patterns: vpn.set<TYPE>(RES, simple_value)
    # This handles: set<TYPE>(RES, 0), set<TYPE>(RES, false), set<TYPE>(RES, true), set<TYPE>(RES, STATE_XXX)
    perl -i -pe 's/(\w+)\.set<(\w+)>\(([A-Z_0-9]+),\s*([0-9]+|true|false|[A-Z_][A-Z_0-9]*)\)/$1.resource($3)->set<$2>($4)/g' "$file"

    # Fix: set<TYPE>(RES, simple_value) without object prefix
    perl -i -pe 's/(?<!\.)\bset<(\w+)>\(([A-Z_0-9]+),\s*([0-9]+|true|false|[A-Z_][A-Z_0-9]*)\)/resource($2)->set<$1>($3)/g' "$file"

    # Fix string sets: set<STRING_T>(RES, "") -> resource(RES)->set<STRING_T>("")
    perl -i -pe 's/(\w+)\.set<STRING_T>\(([A-Z_0-9]+),\s*"([^"]*)"\)/$1.resource($2)->set<STRING_T>("$3")/g' "$file"
    perl -i -pe 's/(?<!\.)\bset<STRING_T>\(([A-Z_0-9]+),\s*"([^"]*)"\)/resource($1)->set<STRING_T>("$2")/g' "$file"

    echo "✓ Fixed $file"
done

echo ""
echo "======================================"
echo "Manual Fixes Still Needed"
echo "======================================"
echo ""
echo "The following patterns need manual fixing:"
echo "1. Complex set<> calls with function arguments"
echo "2. Complex set<> calls with string concatenation"
echo "3. Complex set<> calls with expressions"
echo ""
echo "Search for remaining patterns like:"
echo "  - set<TYPE>(RES,"
echo "  - get<TYPE>(RES)"
echo ""
echo "And manually convert them to:"
echo "  - resource(RES)->set<TYPE>(...)"
echo "  - resource(RES)->get<TYPE>()"
echo ""
echo "Backup files saved as .bak"
echo ""
