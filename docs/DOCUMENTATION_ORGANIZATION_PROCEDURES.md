# Documentation Organization Procedures

**Standard procedures for organizing project documentation**

---

## Overview

This document provides reusable procedures for organizing documentation in software projects. These procedures were successfully applied to the Friendly LwM2M Client v1.2.2 project.

---

## Principles

1. **Category-Based Structure**: Organize by purpose/audience, not by file type
2. **Progressive Disclosure**: Easy navigation from general to specific
3. **Clean Root Directory**: Move scattered files into organized subdirectories
4. **Comprehensive Indexing**: Create README.md files for navigation
5. **Archive Historical Content**: Preserve but separate old documentation
6. **100% Accuracy**: All documentation must reflect current build status

---

## Standard Directory Structure

```
project-root/
├── README.md                          # Main project README (essential only)
├── LICENSE
├── CMakeLists.txt
│
├── docs/
│   ├── README.md                      # Documentation hub with quick navigation
│   ├── INDEX.md                       # Complete searchable index (optional)
│   │
│   ├── # Core Documentation
│   ├── BUILDING.md                    # Consolidated build guide
│   ├── GETTING_STARTED.md             # Comprehensive tutorial
│   ├── QUICK_START.md                 # 5-minute quick start
│   │
│   ├── # Feature-Specific Guides
│   ├── FEATURE_*.md                   # Individual feature guides
│   │
│   ├── # Reference Documentation
│   ├── API_REFERENCE.md               # API documentation
│   ├── ARCHITECTURE.md                # System design
│   ├── CONFIGURATION.md               # Configuration options
│   ├── TROUBLESHOOTING.md             # Common issues
│   │
│   ├── # Platform Integration
│   ├── PLATFORM_*.md                  # Platform-specific guides
│   │
│   └── archive/
│       ├── completion-reports/        # Historical completion reports
│       ├── vX.X.X/                    # Version-specific archives
│       └── root-docs/                 # Old root directory docs
│
└── scripts/
    └── install_dependencies.sh
```

---

## Organization Process

### Phase 1: Audit

**Goal**: Understand current documentation state

**Steps**:

1. **Find all documentation files**:
   ```bash
   # Root-level docs
   find . -maxdepth 1 -type f \( -name "*.md" -o -name "*.txt" -o -name "README*" \)

   # Existing docs/ folder
   find docs/ -maxdepth 1 -type f -name "*.md"

   # All markdown files project-wide
   find . -name "*.md" | grep -v node_modules | grep -v ".git"
   ```

2. **Categorize files**:
   - Getting Started guides (QUICK_START, GETTING_STARTED, etc.)
   - Build guides (BUILD_*, BUILDING, SETUP_*, etc.)
   - Feature guides (FEATURE_*, specific feature names)
   - Reference docs (API_*, ARCHITECTURE, CONFIGURATION, etc.)
   - Platform guides (OPENWRT_*, RPI4_*, platform names)
   - Completion reports (*_COMPLETE, *_SUMMARY, *_REPORT, etc.)
   - Historical docs (old versions, deprecated guides)

3. **Identify issues**:
   - Outdated version numbers
   - Incorrect build commands
   - Broken links
   - Duplicate information
   - Missing critical information

### Phase 2: Structure

**Goal**: Create organized directory structure

**Steps**:

1. **Create archive directory**:
   ```bash
   mkdir -p docs/archive/completion-reports
   mkdir -p docs/archive/root-docs
   ```

2. **Verify docs/ exists**:
   ```bash
   [ -d docs ] || mkdir docs
   ```

### Phase 3: Migration

**Goal**: Move files to appropriate locations

**Steps**:

1. **Move completion reports to archive**:
   ```bash
   cd project-root
   mv *_COMPLETE.md *_SUMMARY.md *_REPORT.md PHASE_*.md WEEK_*.md \
      docs/archive/completion-reports/
   ```

2. **Move build-related guides to docs/**:
   ```bash
   mv BUILD_*.md BUILDING.md SETUP_*.md CONFIGURATION_GUIDE.md \
      docs/
   ```

3. **Move feature guides to docs/**:
   ```bash
   mv FEATURE_*.md *_GUIDE.md docs/
   ```

4. **Move platform guides to docs/**:
   ```bash
   mv *OPENWRT*.md *RPI*.md *PLATFORM*.md docs/
   ```

5. **Verify only essential files remain in root**:
   ```bash
   ls -1 *.md
   # Expected: README.md (and maybe CHANGELOG.md, CONTRIBUTING.md)
   ```

### Phase 4: Update

**Goal**: Update documentation for accuracy

**Steps**:

1. **Update main README.md**:
   - Current version number
   - Accurate prerequisites
   - Correct build commands
   - Links to docs/ folder (not root)
   - Emphasize critical requirements

2. **Update docs/README.md**:
   - Current version number
   - Organized navigation sections
   - Links to all major documents
   - Quick start paths
   - "I Want To..." task-based navigation

3. **Create/update BUILDING.md**:
   - Consolidate all build information
   - Complete prerequisites
   - Step-by-step instructions
   - All configuration options
   - Platform-specific builds
   - Comprehensive troubleshooting

4. **Update version numbers everywhere**:
   ```bash
   # Find files with old version
   grep -r "v1\.2\.0" docs/

   # Update to current version
   sed -i 's/v1\.2\.0/v1.2.2/g' docs/*.md
   ```

5. **Update command examples**:
   - Replace deprecated commands
   - Update binary names
   - Fix build flags
   - Correct file paths

6. **Fix broken links**:
   ```bash
   # Find relative links
   grep -r '\]\([^h]' docs/*.md

   # Update to correct paths
   # Example: [Guide](GUIDE.md) → [Guide](docs/GUIDE.md)
   ```

### Phase 5: Documentation

**Goal**: Create navigation and index files

**Steps**:

1. **Create docs/README.md** (if doesn't exist):
   ```markdown
   # Project Documentation

   **Version:** X.X.X
   **Last Updated:** YYYY-MM

   ## Quick Navigation

   **New to the project?** Start here:
   1. [Quick Start Guide](QUICK_START.md)
   2. [Getting Started](GETTING_STARTED.md)
   3. [Building](BUILDING.md)

   ## Documentation Index

   ### Getting Started
   - [Quick Start](QUICK_START.md)
   - [Getting Started](GETTING_STARTED.md)
   - [Building](BUILDING.md)

   ### Reference
   - [API Reference](API_REFERENCE.md)
   - [Architecture](ARCHITECTURE.md)
   - [Configuration](CONFIGURATION.md)

   ### Features
   - [Feature 1](FEATURE_1.md)
   - [Feature 2](FEATURE_2.md)

   ### Platform Integration
   - [Platform 1](PLATFORM_1.md)
   - [Platform 2](PLATFORM_2.md)
   ```

2. **Create category README files** (optional):
   ```bash
   # In each subdirectory
   echo "# Category Name\n\nDescription\n\n## Contents\n\n- [Doc 1](doc1.md)" > docs/category/README.md
   ```

3. **Create reorganization summary**:
   ```markdown
   # Documentation Reorganization Summary

   **Date**: YYYY-MM-DD
   **Version**: X.X.X

   ## Changes
   - Files moved: X
   - Files created: Y
   - Files updated: Z

   ## File Movements
   - OLD_LOCATION.md → docs/NEW_LOCATION.md

   ## Updates
   - Updated version numbers
   - Fixed build commands
   - Corrected links
   ```

### Phase 6: Verification

**Goal**: Ensure organization is complete and correct

**Steps**:

1. **Verify root directory**:
   ```bash
   ls -1 *.md
   # Should show only: README.md (and optionally CHANGELOG.md, CONTRIBUTING.md)
   ```

2. **Verify docs/ structure**:
   ```bash
   tree docs/ -L 2
   # Should show organized structure
   ```

3. **Check for broken links**:
   ```bash
   # Find all markdown links
   grep -r '\]\(' docs/*.md

   # Test each link exists
   # Manual verification or use link checker tool
   ```

4. **Verify version numbers**:
   ```bash
   grep -r "Version:" docs/*.md
   # All should show current version
   ```

5. **Test build commands**:
   ```bash
   # Copy build commands from docs
   # Test in clean environment
   # Verify they work
   ```

---

## Categories Explained

### Core Documentation

| File | Purpose | Audience | Size |
|------|---------|----------|------|
| README.md (root) | Project overview, quick links | Everyone | 100-200 lines |
| docs/README.md | Documentation hub | Everyone | 200-300 lines |
| docs/BUILDING.md | Complete build guide | Developers | 500-700 lines |
| docs/GETTING_STARTED.md | Comprehensive tutorial | New users | 300-500 lines |
| docs/QUICK_START.md | 5-minute setup | Impatient users | 50-100 lines |

### Reference Documentation

| File | Purpose | Audience |
|------|---------|----------|
| API_REFERENCE.md | API documentation | Developers |
| ARCHITECTURE.md | System design | Architects |
| CONFIGURATION.md | Config options | DevOps/Users |
| TROUBLESHOOTING.md | Debug guides | All users |

### Feature-Specific Guides

Individual guides for major features, named descriptively:
- DTLS_PROVIDER_SELECTION.md
- MINIMAL_CURL_BUILD.md
- MQTT_INTEGRATION.md

### Platform Integration

Platform-specific deployment guides:
- OPENWRT_INTEGRATION.md
- RPI4_ARM64_PORTING_GUIDE.md
- DOCKER_DEPLOYMENT.md

### Archive

Historical documents that should be preserved but not prominent:
- Completion reports (*_COMPLETE.md, *_SUMMARY.md)
- Old version docs (docs/archive/v1.0.0/)
- Deprecated guides (docs/archive/deprecated/)

---

## Best Practices

### DO

✅ **Organize by purpose** (Getting Started, Reference, Features, Platform)
✅ **Keep root clean** (only README, LICENSE, essential files)
✅ **Create navigation hubs** (docs/README.md with quick links)
✅ **Archive historical content** (preserve but separate)
✅ **Update version numbers** (everywhere, consistently)
✅ **Fix broken links** (update paths after moving files)
✅ **Test build commands** (verify they actually work)
✅ **Create summary document** (track what changed)

### DON'T

❌ **Leave scattered files** (move everything to docs/)
❌ **Break existing workflows** (update references, test commands)
❌ **Delete historical docs** (archive instead)
❌ **Mix old and new versions** (update consistently)
❌ **Use vague filenames** (be descriptive: FEATURE_MQTT.md not FEATURE1.md)
❌ **Forget to document changes** (create reorganization summary)

---

## Reusable Scripts

### Move completion reports to archive
```bash
#!/bin/bash
mkdir -p docs/archive/completion-reports
for file in *_COMPLETE.md *_SUMMARY.md *_REPORT.md PHASE_*.md WEEK_*.md; do
    [ -f "$file" ] && mv "$file" docs/archive/completion-reports/ && echo "Moved $file"
done
```

### Move guides to docs
```bash
#!/bin/bash
for file in BUILD_*.md *_GUIDE.md CONFIGURATION_*.md SETUP_*.md; do
    [ -f "$file" ] && mv "$file" docs/ && echo "Moved $file"
done
```

### Update version numbers
```bash
#!/bin/bash
OLD_VERSION="1.2.0"
NEW_VERSION="1.2.2"
find docs/ -name "*.md" -exec sed -i "s/${OLD_VERSION}/${NEW_VERSION}/g" {} \;
echo "Updated version numbers from ${OLD_VERSION} to ${NEW_VERSION}"
```

### Verify organization
```bash
#!/bin/bash
echo "=== Root Directory ==="
ls -1 *.md 2>/dev/null || echo "No markdown files (good!)"

echo -e "\n=== Docs Structure ==="
tree docs/ -L 2 -I 'node_modules|.git'

echo -e "\n=== Version Check ==="
grep -r "Version:" docs/*.md | head -5

echo -e "\n=== Archive Contents ==="
ls -1 docs/archive/completion-reports/ 2>/dev/null | wc -l
echo "completion reports archived"
```

---

## Checklist

Use this checklist for each documentation reorganization:

### Planning
- [ ] Audit all documentation files
- [ ] Categorize files by purpose
- [ ] Identify outdated content
- [ ] Create archive plan

### Execution
- [ ] Create docs/archive/ structure
- [ ] Move completion reports to archive
- [ ] Move guides to docs/
- [ ] Move feature docs to docs/
- [ ] Move platform docs to docs/
- [ ] Verify only essential files in root

### Updates
- [ ] Update main README.md
- [ ] Update docs/README.md
- [ ] Create/update BUILDING.md
- [ ] Update version numbers everywhere
- [ ] Fix build commands
- [ ] Fix broken links

### Documentation
- [ ] Create docs/README.md hub
- [ ] Create reorganization summary
- [ ] Update category README files (if any)

### Verification
- [ ] Root directory clean
- [ ] All docs in docs/
- [ ] No broken links
- [ ] Version numbers current
- [ ] Build commands tested
- [ ] Navigation works

---

## Success Criteria

✅ **Clean root directory** (only essential files)
✅ **Organized docs/** (clear categories)
✅ **Complete navigation** (easy to find everything)
✅ **Accurate content** (current version, working commands)
✅ **Preserved history** (archived, not deleted)
✅ **Professional appearance** (production-ready)

---

## Metrics to Track

| Metric | Before | After | Target |
|--------|--------|-------|--------|
| Root directory files | ?? | ?? | ≤ 5 |
| Documentation files | ?? | ?? | All in docs/ |
| Broken links | ?? | 0 | 0 |
| Outdated version refs | ?? | 0 | 0 |
| Build command accuracy | ?? % | 100% | 100% |

---

## Example: Friendly LwM2M Client v1.2.2

Applied these procedures to Friendly LwM2M Client:

**Results**:
- Root files: 33 → 1 (97% reduction)
- Files moved: 25 (10 to docs/, 8 to archive, 7 build guides)
- Files created: 2 (BUILDING.md, DOCUMENTATION_REORGANIZATION.md)
- Files updated: 3 (README.md, docs/README.md, multiple fixes)
- Version accuracy: 100% (all showing v1.2.2)
- Build accuracy: 100% (all commands tested and working)
- Link accuracy: 100% (all links verified)

**Key Achievement**: Emphasized critical minimal libcurl requirement across all documentation

See: `docs/DOCUMENTATION_REORGANIZATION_2026-04-30.md` for complete details

---

## Conclusion

Following these procedures ensures:
- Professional, organized documentation structure
- Easy navigation for all users
- 100% accurate, up-to-date information
- Preserved historical context
- Scalable for future growth

**Remember**: Documentation organization is about making content **easy to find** and **easy to use**. Keep the user's journey in mind: discovery → understanding → action.
