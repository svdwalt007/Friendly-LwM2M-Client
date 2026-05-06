#!/bin/bash
#
# Update LwM2M Object IDs from 10525-10537 to 10525-10537
#
# Old ID → New ID   Object Name
# 10525 → 10525     WAN Failover Policy
# 10526 → 10526     Multi-WAN Health Check
# 10527 → 10527     WiFi Client Management
# 10528 → 10528     WiFi Channel Optimization
# 10529 → 10529     Matter Bridge
# 10530 → 10530     Matter Device
# 10531 → 10531     Thread Network
# 10532 → 10532     Zigbee Coordinator
# 10533 → 10533     Zigbee Device
# 10534 → 10534     Zigbee Group
# 10535 → 10535     LAN Configuration
# 10536 → 10536     Routing Table
# 10537 → 10537     VPN Configuration
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${PROJECT_ROOT}"

echo "=================================================="
echo "LwM2M Object ID Update: 10525-10537 → 10525-10537"
echo "=================================================="
echo ""

# Define the mappings
declare -A ID_MAP=(
    ["10525"]="10525"  # WAN Failover Policy
    ["10526"]="10526"  # Multi-WAN Health Check
    ["10527"]="10527"  # WiFi Client Management
    ["10528"]="10528"  # WiFi Channel Optimization
    ["10529"]="10529"  # Matter Bridge
    ["10530"]="10530"  # Matter Device
    ["10531"]="10531"  # Thread Network
    ["10532"]="10532"  # Zigbee Coordinator
    ["10533"]="10533"  # Zigbee Device
    ["10534"]="10534"  # Zigbee Group
    ["10535"]="10535"  # LAN Configuration
    ["10536"]="10536"  # Routing Table
    ["10537"]="10537"  # VPN Configuration
)

declare -A OBJECT_NAMES=(
    ["10525"]="WAN Failover Policy"
    ["10526"]="Multi-WAN Health Check"
    ["10527"]="WiFi Client Management"
    ["10528"]="WiFi Channel Optimization"
    ["10529"]="Matter Bridge"
    ["10530"]="Matter Device"
    ["10531"]="Thread Network"
    ["10532"]="Zigbee Coordinator"
    ["10533"]="Zigbee Device"
    ["10534"]="Zigbee Group"
    ["10535"]="LAN Configuration"
    ["10536"]="Routing Table"
    ["10537"]="VPN Configuration"
)

echo "Step 1: Finding all files with old object IDs..."
echo ""

# Find all files containing old IDs (excluding build/, .git/, binary files)
FILES=$(grep -rlE '3461[1-9]|3462[0-3]' \
    --exclude-dir=build \
    --exclude-dir=.git \
    --exclude-dir=node_modules \
    --exclude="*.o" \
    --exclude="*.a" \
    --exclude="*.so" \
    --exclude="*.hex" \
    --exclude="*.bin" \
    . 2>/dev/null || true)

if [ -z "$FILES" ]; then
    echo "No files found with old object IDs"
    exit 0
fi

FILE_COUNT=$(echo "$FILES" | wc -l)
echo "Found ${FILE_COUNT} files containing old object IDs"
echo ""

echo "Step 2: Backing up files..."
BACKUP_DIR="${PROJECT_ROOT}/backup_object_id_update_$(date +%Y%m%d_%H%M%S)"
mkdir -p "${BACKUP_DIR}"

for file in $FILES; do
    if [ -f "$file" ]; then
        backup_path="${BACKUP_DIR}/${file}"
        mkdir -p "$(dirname "${backup_path}")"
        cp "$file" "${backup_path}"
    fi
done

echo "✅ Backup created: ${BACKUP_DIR}"
echo ""

echo "Step 3: Updating object IDs in files..."
echo ""

UPDATED_COUNT=0

for file in $FILES; do
    if [ ! -f "$file" ]; then
        continue
    fi

    # Check if file contains any old IDs
    if ! grep -qE '3461[1-9]|3462[0-3]' "$file" 2>/dev/null; then
        continue
    fi

    echo "Updating: $file"

    # Create temp file
    TEMP_FILE="${file}.tmp"
    cp "$file" "$TEMP_FILE"

    # Apply all replacements
    for old_id in "${!ID_MAP[@]}"; do
        new_id="${ID_MAP[$old_id]}"

        # Replace in various contexts:
        # - Plain number: 10525 → 10525
        # - With quotes: "10525" → "10525"
        # - In XML: <ObjectID>10525</ObjectID>
        # - In comments: Object 10525 → Object 10525
        # - In paths: /10525/ → /10525/

        sed -i "s/${old_id}/${new_id}/g" "$TEMP_FILE"
    done

    # Only replace if file actually changed
    if ! cmp -s "$file" "$TEMP_FILE"; then
        mv "$TEMP_FILE" "$file"
        UPDATED_COUNT=$((UPDATED_COUNT + 1))
    else
        rm "$TEMP_FILE"
    fi
done

echo ""
echo "✅ Updated ${UPDATED_COUNT} files"
echo ""

echo "Step 4: Renaming XML files in utils/object_maker/..."
echo ""

cd "${PROJECT_ROOT}/utils/object_maker"

for old_id in "${!ID_MAP[@]}"; do
    new_id="${ID_MAP[$old_id]}"
    old_file="${old_id}.xml"
    new_file="${new_id}.xml"

    if [ -f "$old_file" ]; then
        echo "Renaming: ${old_file} → ${new_file}"
        mv "$old_file" "$new_file"
    fi
done

echo ""
echo "✅ XML files renamed"
echo ""

cd "${PROJECT_ROOT}"

echo "Step 5: Verification..."
echo ""

# Check for any remaining old IDs
REMAINING=$(grep -rn -E '3461[1-9]|3462[0-3]' \
    --exclude-dir=build \
    --exclude-dir=.git \
    --exclude-dir=backup_object_id_update_* \
    --exclude="*.o" \
    --exclude="*.a" \
    --exclude="*.so" \
    --exclude="*.hex" \
    --exclude="*.bin" \
    . 2>/dev/null || true)

if [ -n "$REMAINING" ]; then
    echo "⚠️  WARNING: Found remaining old object IDs:"
    echo "$REMAINING"
    echo ""
    echo "These may be in binary files, build artifacts, or intentional references."
else
    echo "✅ No remaining old object IDs found (excluding backups)"
fi

echo ""
echo "Step 6: Summary of changes..."
echo ""

echo "Object ID Mappings Applied:"
echo "----------------------------"
for old_id in $(echo "${!ID_MAP[@]}" | tr ' ' '\n' | sort); do
    new_id="${ID_MAP[$old_id]}"
    object_name="${OBJECT_NAMES[$old_id]}"
    printf "  %5s → %5s   %s\n" "$old_id" "$new_id" "$object_name"
done

echo ""
echo "=================================================="
echo "✅ Object ID Update Complete"
echo "=================================================="
echo ""
echo "Files updated: ${UPDATED_COUNT}"
echo "Backup location: ${BACKUP_DIR}"
echo ""
echo "Next steps:"
echo "  1. Review changes: git diff"
echo "  2. Test build: cd build && cmake .. && make"
echo "  3. Run tests: ctest --output-on-failure"
echo "  4. Commit changes: git add -A && git commit -m 'refactor: Update LwM2M object IDs 10525-10537 to 10525-10537'"
echo ""
echo "To restore from backup:"
echo "  cp -r ${BACKUP_DIR}/* ."
echo ""
