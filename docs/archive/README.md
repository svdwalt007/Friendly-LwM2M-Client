# Archived Documentation

This directory contains archived documentation that is no longer relevant for v1.2.0 of the Friendly LwM2M Client.

## Directory Structure

### outdated/
Documentation superseded by newer versions:
- `QUICKSTART.md` - Old quick start guide (replaced by root [QUICK_START.md](../../QUICK_START.md))
- `WALT_TECHNOLOGIES_OBJECTS_IMPLEMENTATION.md` - Old object implementation details (now 10512-10537 range)
- `PROJECT_SUMMARY.md` - Historical project summary (v1.0-1.1 era)

### platform-specific/
Niche platform integrations:
- `PRPL_INTEGRATION.md` - prpl Foundation integration (niche use case)
- `OPENWRT_ONE_LWM2M_DESIGN.md` - OpenWrt One specific design docs (specialized)

### integration-guides/
Future/planned integrations not yet implemented:
- `MATTER_INTEGRATION.md` - Matter/Thread integration (planned feature)
- `ZIGBEE_INTEGRATION.md` - Zigbee integration (future consideration)

### analysis-docs/
Historical analysis and planning documents:
- `COMPOSITE_OPERATIONS_ANALYSIS.md` - Composite operations analysis (planning phase)
- `LWM2M_SERVER_GAP_ANALYSIS.md` - Server gap analysis (historical)

### legacy-html/
Old Doxygen-generated HTML documentation.

**To generate fresh documentation:**
```bash
cd build
cmake -DBUILD_DOCS=ON ..
make docs
# Open build/docs/html/index.html
```

## Current Documentation (v1.2.0)

For current documentation, see:

### Getting Started
- **[Quick Start Guide](../../QUICK_START.md)** - Get started in 5 minutes
- **[Getting Started](../../GETTING_STARTED.md)** - Comprehensive tutorial
- **[Build Examples](../../BUILD_EXAMPLES.md)** - All build configurations

### Core Documentation
- **[docs/README.md](../README.md)** - Documentation index
- **[docs/API_REFERENCE.md](../API_REFERENCE.md)** - API documentation
- **[docs/ARCHITECTURE.md](../ARCHITECTURE.md)** - System architecture
- **[docs/CONFIGURATION.md](../CONFIGURATION.md)** - Configuration guide
- **[docs/TROUBLESHOOTING.md](../TROUBLESHOOTING.md)** - Common issues

### Platform Integration
- **[docs/OPENWRT_INTEGRATION.md](../OPENWRT_INTEGRATION.md)** - OpenWrt deployment
- **[docs/RPI4_ARM64_PORTING_GUIDE.md](../RPI4_ARM64_PORTING_GUIDE.md)** - Raspberry Pi guide
- **[docs/PRPLOS_PORTING_GUIDE.md](../PRPLOS_PORTING_GUIDE.md)** - prplOS porting

## Why These Were Archived

### Outdated Content
- **Object ID changes**: v1.2.0 uses different object ID ranges (10512-10537, 33405)
- **New quick start**: Root directory now has comprehensive QUICK_START.md
- **Version mismatch**: Docs referenced v1.0-1.1 features/object IDs

### Platform-Specific
- **Niche platforms**: prpl Foundation integration is specialized
- **Device-specific**: OpenWrt One design is too specific for general docs

### Future Features
- **Not implemented**: Matter and Zigbee integrations are planned but not built
- **Speculative**: Better to document when actually implemented

### Historical Analysis
- **Planning docs**: These were useful during development but not for users
- **Gap analysis**: Server analysis was for internal development planning

## Restoration

If you need any of these documents, they're preserved here. To restore:

```bash
# From project root
cp docs/archive/outdated/FILENAME.md docs/
```

## Last Updated

April 11, 2026 - v1.2.0 documentation rationalization
