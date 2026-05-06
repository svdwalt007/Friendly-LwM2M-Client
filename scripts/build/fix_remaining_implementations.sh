#!/bin/bash
# Fix remaining object implementations comprehensively

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

echo "======================================"
echo "Fixing Remaining Object Implementations"
echo "======================================"
echo ""

# Define files and their registry methods
declare -A FILES=(
    ["wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization"]="wifiChannelOptimization"
    ["wpp/registry/objects/w_10529_matter_bridge/MatterBridge"]="matterBridge"
    ["wpp/registry/objects/w_10530_matter_device/MatterDevice"]="matterDevice"
    ["wpp/registry/objects/w_10531_thread_network/ThreadNetwork"]="threadNetwork"
)

for file_base in "${!FILES[@]}"; do
    registry_method="${FILES[$file_base]}"
    class=$(basename "$file_base")
    cpp="${file_base}.cpp"

    if [ ! -f "$cpp" ]; then
        echo "⚠ Skipping $cpp - not found"
        continue
    fi

    echo "Fixing $class.cpp..."

    # Backup
    cp "$cpp" "$cpp.bak2"

    # Fix static methods
    echo "  - Fixing static methods..."

    # Fix object() method
    sed -i "s/client\.registry()\.getObject.*/${registry_method}();/g" "$cpp"
    sed -i "s/return client\.registry()\.${registry_method}/return ctx.registry().${registry_method}/g" "$cpp"

    # Fix createInst()
    sed -i "s/Instance\* ${class}::createInst/${class}* ${class}::createInst/g" "$cpp"
    sed -i "s/return object(client)\.createInstance/Instance *inst = ctx.registry().${registry_method}().createInstance/g" "$cpp"

    # Add NULL check and cast after createInstance call
    perl -i -pe "s/(Instance \*inst = ctx\.registry\(\)\.${registry_method}\(\)\.createInstance\([^)]+\);)/\$1\n    if (!inst) return NULL;\n    return static_cast<${class}*>(inst);/" "$cpp"

    # Fix instance() method
    sed -i "s/Instance\* ${class}::instance/${class}* ${class}::instance/g" "$cpp"
    sed -i "s/return object(client)\.instance/Instance *inst = ctx.registry().${registry_method}().instance/g" "$cpp"

    # Add NULL check and cast after instance call
    perl -i -pe "s/(Instance \*inst = ctx\.registry\(\)\.${registry_method}\(\)\.instance\([^)]+\);)/\$1\n    if (!inst) return NULL;\n    return static_cast<${class}*>(inst);/" "$cpp"

    # Fix removeInst() method
    sed -i "s/return object(client)\.remove/return ctx.registry().${registry_method}().remove/g" "$cpp"

    # Fix any remaining 'client' references
    sed -i "s/object(client)/object(ctx)/g" "$cpp"

    # Fix constructor signature
    echo "  - Fixing constructor..."
    sed -i "s/${class}::${class}(Object& object, ID_T instId)/${class}::${class}(lwm2m_context_t\& context, const OBJ_LINK_T\& id)/g" "$cpp"
    sed -i "s/: Instance(object, instId)/: Instance(context, id)/g" "$cpp"

    # Add resourcesCreate/Init calls to constructor
    perl -i -pe "s/(${class}::${class}\(lwm2m_context_t& context.*?\{)/\$1\n\n    resourcesCreate();\n    resourcesInit();/s" "$cpp"

    # Add operation notifiers
    echo "  - Adding operation notifiers..."
    if ! grep -q "void ${class}::serverOperationNotifier" "$cpp"; then
        perl -i -pe "s/(${class}::~${class}\(\).*?\})/\$1\n\n\/\* Operation notification handlers \*\/\nvoid ${class}::serverOperationNotifier(ItemOp \*operation) {\n    \/\/ Handle server operations if needed\n}\n\nvoid ${class}::userOperationNotifier(ItemOp \*operation) {\n    \/\/ Handle user operations if needed\n}/s" "$cpp"
    fi

    # Rename initResources to resourcesCreate and resourcesInit
    echo "  - Splitting initResources..."

    # Find and replace initResources with resourcesCreate
    sed -i "s/bool ${class}::initResources(ItemOp \*itemOp)/void ${class}::resourcesCreate()/g" "$cpp"

    # Remove validate() method
    echo "  - Removing validate()..."
    perl -i -0777 -pe "s/\/\* Validation \*\/.*?bool ${class}::validate\(.*?\{.*?\n\s*return true;\n\}//gs" "$cpp"

    # Fix setExecute to setExecuteHandler
    sed -i "s/setExecute(/resource(/g" "$cpp"
    sed -i "s/resource(\([A-Z_0-9]*\), \([a-zA-Z]*\))/resource(\1)->setExecuteHandler(\2)/g" "$cpp"

    # Convert remaining set<> patterns
    sed -i 's/\([[:space:]]*\)set<\([A-Z_]*\)>(\([A-Z_0-9]*\),/\1resource(\3)->set<\2>(/g' "$cpp"

    echo "  ✓ Fixed $class.cpp"
done

echo ""
echo "======================================"
echo "Manual Steps Needed"
echo "======================================"
echo ""
echo "Each file still needs:"
echo "1. Add resourcesInit() method by copying content from resourcesCreate"
echo "2. Move initial value setting to resourcesInit()"
echo "3. Move execute handlers and validators to resourcesCreate()"
echo "4. Change return type from 'return true' to void in resourcesCreate"
echo "5. Convert any validate() logic to setDataVerifier() calls"
echo ""
echo "Use VpnConfiguration.cpp and LanConfiguration.cpp as templates"
echo ""
