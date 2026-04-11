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
