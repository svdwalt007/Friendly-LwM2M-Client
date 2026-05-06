#!/bin/bash
# Fix remaining header files for advanced objects

OBJECTS=(
    "MatterDevice:10530"
    "ThreadNetwork:10531"
    "ZigbeeCoordinator:10532"
    "ZigbeeDevice:10533"
    "ZigbeeGroup:10534"
    "RoutingTable:10536"
)

for obj_spec in "${OBJECTS[@]}"; do
    IFS=':' read -r class_name obj_id <<< "$obj_spec"

    # Find the header file
    header_file=$(find /d/Dev/Friendly-LwM2M-Client/wpp/registry/objects -name "${class_name}.h" | grep -v Config | grep -v Info)

    if [ -z "$header_file" ]; then
        echo "⚠️  Could not find header for $class_name"
        continue
    fi

    echo "Fixing: $header_file"

    # Fix static method return types
    sed -i "s/static Instance\* createInst/static ${class_name}* createInst/g" "$header_file"
    sed -i "s/static Instance\* instance(WppClient& ctx, ID_T instId);/static ${class_name}* instance(WppClient\& ctx, ID_T instId = ID_T_MAX_VAL);/g" "$header_file"

    # Fix constructor signature
    sed -i "s/${class_name}(Object& object, ID_T instId);/${class_name}(lwm2m_context_t\& context, const OBJ_LINK_T\& id);/g" "$header_file"

    # Remove override from destructor
    sed -i "s/~${class_name}() override;/~${class_name}();/g" "$header_file"

    # Remove validate and initResources methods, add correct ones
    sed -i '/bool validate(ID_T resId, const void \*data, size_t size) override;/d' "$header_file"
    sed -i '/bool initResources(ItemOp \*) override;/d' "$header_file"

    # Check if we need to add the correct methods
    if ! grep -q "serverOperationNotifier" "$header_file"; then
        # Find protected: section and add methods after it
        sed -i '/^protected:/a\    void serverOperationNotifier(Instance \*securityInst, ItemOp::TYPE type, const ResLink \&resLink) override;\n    void userOperationNotifier(ItemOp::TYPE type, const ResLink \&resLink) override;\n' "$header_file"
    fi

    # Add resourcesCreate and resourcesInit in private section if not present
    if ! grep -q "resourcesCreate" "$header_file"; then
        sed -i '/^private:/a\    \/\* Private methods \*\/\n    void resourcesCreate();\n    void resourcesInit();\n' "$header_file"
    fi

    echo "✅ Fixed $class_name"
done

echo ""
echo "All header files fixed!"
