#!/bin/bash
###############################################################################
# Documentation Rationalization Script
# For Friendly LwM2M Client v1.2.0
#
# This script archives obsolete documentation and rationalizes the structure
###############################################################################

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Documentation Rationalization${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

cd "$(dirname "$0")/.."
DOCS_DIR="docs"
ARCHIVE_DIR="docs/archive"

# Create archive structure
mkdir -p "$ARCHIVE_DIR"/{outdated,platform-specific,integration-guides,analysis-docs,legacy-html}

echo -e "${GREEN}Step 1: Archiving outdated documentation...${NC}"

# Outdated docs (old object IDs, old version info)
mv "$DOCS_DIR/OBJECT_RENUMBERING.md" "$ARCHIVE_DIR/outdated/" 2>/dev/null || true
mv "$DOCS_DIR/QUICKSTART.md" "$ARCHIVE_DIR/outdated/" 2>/dev/null || true
mv "$DOCS_DIR/WALT_TECHNOLOGIES_OBJECTS_IMPLEMENTATION.md" "$ARCHIVE_DIR/outdated/" 2>/dev/null || true
mv "$DOCS_DIR/PROJECT_SUMMARY.md" "$ARCHIVE_DIR/outdated/" 2>/dev/null || true

echo -e "${GREEN}Step 2: Archiving platform-specific guides...${NC}"

# Niche platform guides
mv "$DOCS_DIR/PRPL_INTEGRATION.md" "$ARCHIVE_DIR/platform-specific/" 2>/dev/null || true
mv "$DOCS_DIR/OPENWRT_ONE_LWM2M_DESIGN.md" "$ARCHIVE_DIR/platform-specific/" 2>/dev/null || true

echo -e "${GREEN}Step 3: Archiving integration guides...${NC}"

# Future integration guides
mv "$DOCS_DIR/MATTER_INTEGRATION.md" "$ARCHIVE_DIR/integration-guides/" 2>/dev/null || true
mv "$DOCS_DIR/ZIGBEE_INTEGRATION.md" "$ARCHIVE_DIR/integration-guides/" 2>/dev/null || true

echo -e "${GREEN}Step 4: Archiving analysis documents...${NC}"

# Historical analysis docs
mv "$DOCS_DIR/COMPOSITE_OPERATIONS_ANALYSIS.md" "$ARCHIVE_DIR/analysis-docs/" 2>/dev/null || true
mv "$DOCS_DIR/LWM2M_SERVER_GAP_ANALYSIS.md" "$ARCHIVE_DIR/analysis-docs/" 2>/dev/null || true

echo -e "${GREEN}Step 5: Archiving legacy HTML...${NC}"

# Move all HTML files
mv "$DOCS_DIR"/*.html "$ARCHIVE_DIR/legacy-html/" 2>/dev/null || true
mv "$DOCS_DIR"/*.css "$ARCHIVE_DIR/legacy-html/" 2>/dev/null || true
mv "$DOCS_DIR"/*.js "$ARCHIVE_DIR/legacy-html/" 2>/dev/null || true
mv "$DOCS_DIR"/*.png "$ARCHIVE_DIR/legacy-html/" 2>/dev/null || true
mv "$DOCS_DIR"/*.svg "$ARCHIVE_DIR/legacy-html/" 2>/dev/null || true

echo -e "${GREEN}Step 6: Creating archive README...${NC}"

cat > "$ARCHIVE_DIR/README.md" <<'EOF'
# Archived Documentation

This directory contains archived documentation that is no longer relevant for v1.2.0 of the Friendly LwM2M Client.

## Directory Structure

### outdated/
Documentation superseded by newer versions:
- `OBJECT_RENUMBERING.md` - Migration from 26200-26205 to 10513-10518 (now 10512-10537)
- `QUICKSTART.md` - Old quick start guide (replaced by root QUICK_START.md)
- `WALT_TECHNOLOGIES_OBJECTS_IMPLEMENTATION.md` - Old object implementation details
- `PROJECT_SUMMARY.md` - Historical project summary

### platform-specific/
Niche platform integrations:
- `PRPL_INTEGRATION.md` - prpl Foundation integration
- `OPENWRT_ONE_LWM2M_DESIGN.md` - OpenWrt One specific design docs

### integration-guides/
Future/planned integrations:
- `MATTER_INTEGRATION.md` - Matter/Thread integration
- `ZIGBEE_INTEGRATION.md` - Zigbee integration

### analysis-docs/
Historical analysis and planning documents:
- `COMPOSITE_OPERATIONS_ANALYSIS.md` - Composite operations analysis
- `LWM2M_SERVER_GAP_ANALYSIS.md` - Server gap analysis

### legacy-html/
Old Doxygen-generated HTML documentation.
To generate fresh documentation:
```bash
cd build
cmake -DBUILD_DOCS=ON ..
make docs
```

## Current Documentation

For current v1.2.0 documentation, see:
- **[Quick Start Guide](../../QUICK_START.md)** - Get started in 5 minutes
- **[Getting Started](../../GETTING_STARTED.md)** - Comprehensive tutorial
- **[Build Examples](../../BUILD_EXAMPLES.md)** - All build configurations
- **[docs/README.md](../README.md)** - Documentation index

Last Updated: April 2026
EOF

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}✓ Documentation Rationalization Complete${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Current documentation structure:"
echo "  - docs/README.md (updated index)"
echo "  - docs/ (current docs only)"
echo "  - docs/archive/ (obsolete content)"
echo ""
echo "Remaining documentation:"
ls -1 "$DOCS_DIR"/*.md 2>/dev/null | sed 's/.*\//  - /'
echo ""
