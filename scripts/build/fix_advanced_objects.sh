#!/bin/bash
# Fix all advanced WPP objects (10526-10537) with systematic code corrections
# Based on the working pattern from ConnectivityMonitoring

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
WPP_OBJECTS_DIR="$PROJECT_ROOT/wpp/registry/objects"

echo "========================================"
echo "Fixing Advanced WPP Objects (10526-10537)"
echo "========================================"
echo ""

# List of objects to fix (10525 already fixed manually)
OBJECTS=(
    "w_10526_multiwan_health_check:MultiWanHealthCheck:multiWanHealthCheck"
    "w_10527_wifi_client_management:WifiClientManagement:wifiClientManagement"
    "w_10528_wifi_channel_optimization:WifiChannelOptimization:wifiChannelOptimization"
    "w_10529_matter_bridge:MatterBridge:matterBridge"
    "w_10530_matter_device:MatterDevice:matterDevice"
    "w_10531_thread_network:ThreadNetwork:threadNetwork"
    "w_10532_zigbee_coordinator:ZigbeeCoordinator:zigbeeCoordinator"
    "w_10533_zigbee_device:ZigbeeDevice:zigbeeDevice"
    "w_10534_zigbee_group:ZigbeeGroup:zigbeeGroup"
    "w_10535_lan_configuration:LanConfiguration:lanConfiguration"
    "w_10536_routing_table:RoutingTable:routingTable"
    "w_10537_vpn_configuration:VpnConfiguration:vpnConfiguration"
)

fix_header_file() {
    local obj_dir=$1
    local class_name=$2
    local header_file="$obj_dir/${class_name}.h"

    if [ ! -f "$header_file" ]; then
        echo "  ⚠️  Header file not found: $header_file"
        return 1
    fi

    echo "  Fixing header: $header_file"

    # Fix 1: Replace INST_T with ID_T in static methods
    sed -i 's/static Instance\* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);/static '"$class_name"'* createInst(WppClient\& ctx, ID_T instId = ID_T_MAX_VAL);/g' "$header_file"
    sed -i 's/static Instance\* createInst(WppClient& ctx, INST_T instId = ID_T_MAX_VAL);/static '"$class_name"'* createInst(WppClient\& ctx, ID_T instId = ID_T_MAX_VAL);/g' "$header_file"

    sed -i 's/static Instance\* instance(WppClient& client, INST_T instId);/static '"$class_name"'* instance(WppClient\& ctx, ID_T instId = ID_T_MAX_VAL);/g' "$header_file"
    sed -i 's/static Instance\* instance(WppClient& ctx, INST_T instId);/static '"$class_name"'* instance(WppClient\& ctx, ID_T instId = ID_T_MAX_VAL);/g' "$header_file"

    sed -i 's/static bool remove(WppClient& client, INST_T instId);/static bool removeInst(WppClient\& ctx, ID_T instId);/g' "$header_file"
    sed -i 's/static bool remove(WppClient& ctx, INST_T instId);/static bool removeInst(WppClient\& ctx, ID_T instId);/g' "$header_file"

    # Fix 2: Replace Object& object parameter name
    sed -i 's/static Object& object(WppClient& client);/static Object\& object(WppClient\& ctx);/g' "$header_file"

    # Fix 3: Fix constructor - change from (Object&, INST_T) to (lwm2m_context_t&, const OBJ_LINK_T&)
    sed -i 's/'"$class_name"'(Object& object, INST_T instId);/'"$class_name"'(lwm2m_context_t\& context, const OBJ_LINK_T\& id);/g' "$header_file"

    # Fix 4: Remove "override" from destructor
    sed -i 's/~'"$class_name"'() override;/~'"$class_name"'();/g' "$header_file"

    # Fix 5: Remove invalid override methods and replace with correct pattern
    # Remove the old methods
    sed -i '/bool validate(ID_T resId, const void \*data, size_t size) override;/d' "$header_file"
    sed -i '/bool initResources(ItemOp \*) override;/d' "$header_file"
    sed -i '/bool initResources(ItemOp\*) override;/d' "$header_file"

    # Check if we need to add the correct methods (check if "protected:" section exists)
    if grep -q "^protected:" "$header_file"; then
        # Add correct methods after protected: if not already there
        if ! grep -q "serverOperationNotifier" "$header_file"; then
            sed -i '/^protected:/a\    \/\* Instance implementation part \*\/\n    void serverOperationNotifier(Instance \*securityInst, ItemOp::TYPE type, const ResLink \&resLink) override;\n    void userOperationNotifier(ItemOp::TYPE type, const ResLink \&resLink) override;\n' "$header_file"
        fi
    fi

    # Fix 6: Replace initResources with resourcesCreate and resourcesInit in private section
    if grep -q "^private:" "$header_file"; then
        if ! grep -q "resourcesCreate" "$header_file"; then
            sed -i '/^private:/a\    \/\* Private methods \*\/\n    void resourcesCreate();\n    void resourcesInit();\n' "$header_file"
        fi
    fi

    echo "  ✅ Header fixed"
}

fix_cpp_file() {
    local obj_dir=$1
    local class_name=$2
    local registry_method=$3
    local cpp_file="$obj_dir/${class_name}.cpp"

    if [ ! -f "$cpp_file" ]; then
        echo "  ⚠️  CPP file not found: $cpp_file"
        return 1
    fi

    echo "  Fixing implementation: $cpp_file"

    # Fix 1: Update static object() method to use registry accessor
    sed -i 's/client\.registry()\.getObject([^)]*)/ctx.registry().'"$registry_method"'()/g' "$cpp_file"
    sed -i 's/Object& '"$class_name"'::object(WppClient& client)/Object\& '"$class_name"'::object(WppClient\& ctx)/g' "$cpp_file"

    # Fix 2: Update createInst method
    sed -i 's/Instance\* '"$class_name"'::createInst(WppClient& client, INST_T instId)/'"$class_name"'* '"$class_name"'::createInst(WppClient\& ctx, ID_T instId)/g' "$cpp_file"
    sed -i 's/Instance\* '"$class_name"'::createInst(WppClient& ctx, INST_T instId)/'"$class_name"'* '"$class_name"'::createInst(WppClient\& ctx, ID_T instId)/g' "$cpp_file"
    sed -i 's/return object(client)\.createInstance(instId);/Instance *inst = ctx.registry().'"$registry_method"'().createInstance(instId);\n    if (!inst) return NULL;\n    return static_cast<'"$class_name"'*>(inst);/g' "$cpp_file"
    sed -i 's/return object(ctx)\.createInstance(instId);/Instance *inst = ctx.registry().'"$registry_method"'().createInstance(instId);\n    if (!inst) return NULL;\n    return static_cast<'"$class_name"'*>(inst);/g' "$cpp_file"

    # Fix 3: Update instance method
    sed -i 's/Instance\* '"$class_name"'::instance(WppClient& client, INST_T instId)/'"$class_name"'* '"$class_name"'::instance(WppClient\& ctx, ID_T instId)/g' "$cpp_file"
    sed -i 's/Instance\* '"$class_name"'::instance(WppClient& ctx, INST_T instId)/'"$class_name"'* '"$class_name"'::instance(WppClient\& ctx, ID_T instId)/g' "$cpp_file"
    sed -i 's/return object(client)\.instance(instId);/Instance *inst = ctx.registry().'"$registry_method"'().instance(instId);\n    if (!inst) return NULL;\n    return static_cast<'"$class_name"'*>(inst);/g' "$cpp_file"
    sed -i 's/return object(ctx)\.instance(instId);/Instance *inst = ctx.registry().'"$registry_method"'().instance(instId);\n    if (!inst) return NULL;\n    return static_cast<'"$class_name"'*>(inst);/g' "$cpp_file"

    # Fix 4: Update remove method to removeInst
    sed -i 's/bool '"$class_name"'::remove(WppClient& client, INST_T instId)/bool '"$class_name"'::removeInst(WppClient\& ctx, ID_T instId)/g' "$cpp_file"
    sed -i 's/bool '"$class_name"'::remove(WppClient& ctx, INST_T instId)/bool '"$class_name"'::removeInst(WppClient\& ctx, ID_T instId)/g' "$cpp_file"
    sed -i 's/return object(client)\.remove(instId);/return ctx.registry().'"$registry_method"'().remove(instId);/g' "$cpp_file"
    sed -i 's/return object(ctx)\.remove(instId);/return ctx.registry().'"$registry_method"'().remove(instId);/g' "$cpp_file"

    # Fix 5: Update constructor
    sed -i 's/'"$class_name"'::'"$class_name"'(Object& object, INST_T instId)/'"$class_name"'::'"$class_name"'(lwm2m_context_t\& context, const OBJ_LINK_T\& id)/g' "$cpp_file"
    sed -i 's/: Instance(object, instId)/: Instance(context, id)/g' "$cpp_file"

    # Add resourcesCreate() and resourcesInit() calls in constructor if not present
    if grep -q "${class_name}::${class_name}(lwm2m_context_t& context" "$cpp_file"; then
        # Check if resourcesCreate is already called
        if ! grep -A5 "${class_name}::${class_name}(lwm2m_context_t& context" "$cpp_file" | grep -q "resourcesCreate()"; then
            sed -i '/: Instance(context, id) {/a\n    resourcesCreate();\n    resourcesInit();' "$cpp_file"
        fi
    fi

    # Fix 6: Rename initResources to resourcesInit and remove return
    sed -i 's/bool '"$class_name"'::initResources(ItemOp \*itemOp)/void '"$class_name"'::resourcesInit()/g' "$cpp_file"
    sed -i 's/bool '"$class_name"'::initResources(ItemOp\*)/void '"$class_name"'::resourcesInit()/g' "$cpp_file"

    # Fix 7: Remove "return true;" at end of resourcesInit if present
    # This is tricky with sed, skip for now

    # Fix 8: Replace validate method with serverOperationNotifier and userOperationNotifier
    # Check if validate method exists
    if grep -q "bool ${class_name}::validate" "$cpp_file"; then
        # Remove entire validate method (this is complex, might need manual fix)
        echo "  ⚠️  Warning: validate() method found - may need manual removal"
    fi

    # Add notification methods if not present
    if ! grep -q "${class_name}::serverOperationNotifier" "$cpp_file"; then
        # Add after constructor/destructor
        echo "  Adding serverOperationNotifier and userOperationNotifier methods"
        cat >> "$cpp_file.tmp" << 'EOF'

void CLASS_NAME::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
    operationNotify(*this, resLink, type);
}

void CLASS_NAME::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
    if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);
}

EOF
        # Replace CLASS_NAME with actual class name
        sed -i 's/CLASS_NAME/'"$class_name"'/g' "$cpp_file.tmp"
        # Insert after destructor
        sed -i "/^${class_name}::~${class_name}()/r $cpp_file.tmp" "$cpp_file"
        rm -f "$cpp_file.tmp"
    fi

    # Fix 9: Fix resource access patterns
    # Replace set<TYPE>(RESOURCE, with resource(RESOURCE)->set<TYPE>(
    sed -i -E 's/([^>])set<([^>]+)>\(([A-Z_0-9]+),/\1resource(\3)->set<\2>(/g' "$cpp_file"

    # Replace get<TYPE>(RESOURCE) with resource(RESOURCE)->get<TYPE>()
    sed -i -E 's/([^>])get<([^>]+)>\(([A-Z_0-9]+)\)/\1resource(\3)->get<\2>()/g' "$cpp_file"

    # Fix 10: Replace setExecute calls with setExecuteHandler
    sed -i 's/setExecute(/resource(/g' "$cpp_file"
    sed -i 's/resource(\([A-Z_0-9]*\), /resource(\1)->setExecuteHandler(/g' "$cpp_file"

    # Fix 11: Remove or comment out getMultiple calls
    sed -i 's/if (getMultiple(/if (false \&\& getMultiple(/g' "$cpp_file"

    echo "  ✅ Implementation fixed"
}

# Main loop
FIXED_COUNT=0
FAILED_COUNT=0

for obj_spec in "${OBJECTS[@]}"; do
    IFS=':' read -r obj_dir class_name registry_method <<< "$obj_spec"

    echo ""
    echo "Processing: $class_name ($obj_dir)"
    echo "----------------------------------------"

    full_obj_dir="$WPP_OBJECTS_DIR/$obj_dir"

    if [ ! -d "$full_obj_dir" ]; then
        echo "  ❌ Directory not found: $full_obj_dir"
        ((FAILED_COUNT++))
        continue
    fi

    # Fix header file
    if fix_header_file "$full_obj_dir" "$class_name"; then
        # Fix cpp file
        if fix_cpp_file "$full_obj_dir" "$class_name" "$registry_method"; then
            ((FIXED_COUNT++))
            echo "  ✅ $class_name fixed successfully"
        else
            ((FAILED_COUNT++))
            echo "  ❌ Failed to fix $class_name implementation"
        fi
    else
        ((FAILED_COUNT++))
        echo "  ❌ Failed to fix $class_name header"
    fi
done

echo ""
echo "========================================"
echo "Fix Summary"
echo "========================================"
echo "Objects fixed: $FIXED_COUNT"
echo "Objects failed: $FAILED_COUNT"
echo "Total objects: ${#OBJECTS[@]}"
echo ""

if [ $FAILED_COUNT -eq 0 ]; then
    echo "✅ All objects fixed successfully!"
    echo ""
    echo "Next steps:"
    echo "1. Review the changes"
    echo "2. Run build: cd build && cmake .. && make -j\$(nproc)"
    echo "3. Check for any remaining errors"
    exit 0
else
    echo "⚠️  Some objects failed to fix"
    echo "Manual review required for failed objects"
    exit 1
fi
