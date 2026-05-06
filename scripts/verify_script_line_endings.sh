#!/bin/bash
#
# Script Line Ending Verification
# Checks that all shell scripts use Unix (LF) line endings, not Windows (CRLF)
#
# Usage: ./scripts/verify_script_line_endings.sh
#

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${PROJECT_ROOT}"

echo "=================================================="
echo "Shell Script Line Ending Verification"
echo "=================================================="
echo ""
echo "Project: Friendly LwM2M Client"
echo "Checking: All *.sh files for Unix (LF) line endings"
echo "Location: ${PROJECT_ROOT}"
echo ""

# Directories to exclude (submodules, external code)
EXCLUDE_PATTERNS=(
    "*/2305-Wakaama/*"
    "*/examples/coap/*"
    "*/doxg/*"
    "*/.git/*"
    "*/build/*"
)

# Build find command with exclusions
FIND_CMD="find . -type f -name '*.sh'"
for pattern in "${EXCLUDE_PATTERNS[@]}"; do
    FIND_CMD="$FIND_CMD -not -path '$pattern'"
done

echo "Step 1: Finding shell scripts..."
SCRIPT_FILES=$(eval "$FIND_CMD" | sort)
TOTAL_COUNT=$(echo "$SCRIPT_FILES" | wc -l)

if [ -z "$SCRIPT_FILES" ]; then
    echo "✗ No shell scripts found!"
    exit 1
fi

echo "Found ${TOTAL_COUNT} shell script(s)"
echo ""

echo "Step 2: Checking line endings..."
echo ""

# Arrays to track results
declare -a GOOD_FILES
declare -a BAD_FILES
declare -a MISSING_FILES

# Check each file
while IFS= read -r file; do
    if [ ! -f "$file" ]; then
        MISSING_FILES+=("$file")
        continue
    fi

    # Use file command to detect line endings
    FILE_INFO=$(file "$file")

    if echo "$FILE_INFO" | grep -q "CRLF"; then
        # Has DOS/Windows line endings
        BAD_FILES+=("$file")
        echo "✗ DOS (CRLF): $file"
    elif echo "$FILE_INFO" | grep -qE "(text|shell script)"; then
        # Has Unix line endings
        GOOD_FILES+=("$file")
        echo "✓ Unix (LF):  $file"
    else
        # Unknown or binary
        echo "? Unknown:    $file ($(echo $FILE_INFO | cut -d: -f2))"
    fi
done <<< "$SCRIPT_FILES"

echo ""
echo "=================================================="
echo "Results Summary"
echo "=================================================="
echo ""
echo "Total scripts checked: ${TOTAL_COUNT}"
echo "✓ Unix (LF) format:    ${#GOOD_FILES[@]}"
echo "✗ DOS (CRLF) format:   ${#BAD_FILES[@]}"

if [ ${#MISSING_FILES[@]} -gt 0 ]; then
    echo "⚠ Missing files:       ${#MISSING_FILES[@]}"
fi

echo ""

# Exit with error if any files have DOS line endings
if [ ${#BAD_FILES[@]} -gt 0 ]; then
    echo "=================================================="
    echo "ERROR: Scripts with DOS line endings detected!"
    echo "=================================================="
    echo ""
    echo "The following shell scripts have Windows/DOS (CRLF) line endings"
    echo "and will fail when executed on Linux:"
    echo ""

    for file in "${BAD_FILES[@]}"; do
        echo "  - $file"
    done

    echo ""
    echo "To fix these files, run:"
    echo ""
    echo "  # Install dos2unix if needed:"
    echo "  sudo apt-get install dos2unix"
    echo ""
    echo "  # Convert all affected files:"
    for file in "${BAD_FILES[@]}"; do
        echo "  dos2unix $file"
    done
    echo ""
    echo "  # Or convert all at once:"
    echo "  find . -name '*.sh' -exec dos2unix {} \\;"
    echo ""
    echo "See docs/SCRIPT_LINE_ENDINGS.md for more information."
    echo ""

    exit 1
fi

# Warn if any files are missing
if [ ${#MISSING_FILES[@]} -gt 0 ]; then
    echo "=================================================="
    echo "WARNING: Some script files were not found"
    echo "=================================================="
    echo ""

    for file in "${MISSING_FILES[@]}"; do
        echo "  - $file"
    done

    echo ""
    echo "This may indicate deleted files or a find command issue."
    echo ""
fi

echo "=================================================="
echo "✓ SUCCESS: All shell scripts have Unix line endings"
echo "=================================================="
echo ""
echo "All ${#GOOD_FILES[@]} shell script(s) are correctly formatted with Unix (LF) line endings."
echo "Scripts can be executed on Linux without line ending issues."
echo ""

exit 0
