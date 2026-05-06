# Quick Reference: Build Fixes

**Quick reference for common build issues and their solutions**

---

## Recent Fixes (2026-04-30)

### 1. Brotli Linking Error ✅

**Symptom**:
```
undefined reference to `_kBrotliPrefixCodeRanges'
undefined reference to `BrotliSharedDictionaryAttach'
```

**Cause**: Missing BROTLI_COMMON_LIB in CMakeLists.txt

**Fix**: Added ${BROTLI_COMMON_LIB} to line 637 of CMakeLists.txt

**See**: Issues/b09_fix.md, Issues/b09_COMPLETE.md

---

### 2. Test Hang ✅

**Symptom**: test_firmware_update_object hangs indefinitely

**Cause**: Rollback enabled in test, triggers hardware initialization

**Fix**: Set `rollbackEnabled = false` in test config (line 36)

**See**: Issues/test_hang_fix.md, Issues/b10_FIXES_SUMMARY.md

---

### 3. Script Line Endings ✅

**Symptom**:
```
/bin/bash^M: bad interpreter: No such file or directory
```

**Cause**: DOS (CRLF) line endings in shell scripts

**Fix**:
1. Converted all scripts: `dos2unix *.sh`
2. Created .gitattributes: `*.sh text eol=lf`
3. Verification script: `./scripts/verify_script_line_endings.sh`

**See**: docs/SCRIPT_LINE_ENDINGS.md, Issues/SCRIPT_LINE_ENDINGS_FIX.md

---

## Build Commands

### Clean Build
```bash
cd build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### Run Tests
```bash
cd build
./test_firmware_update_object
./test_delta_algorithms
./test_rollback_manager
./test_blockwise_transfer
```

### Verify Scripts
```bash
./scripts/verify_script_line_endings.sh
```

---

## Common Issues

### Brotli Linking
**Check**: `cat build/CMakeFiles/test_firmware_update_object.dir/link.txt | grep brotli`
**Expected**: libbrotlienc.a, libbrotlidec.a, libbrotlicommon.a (in order)

### Test Hangs
**Check**: Test config has `rollbackEnabled = false`
**Location**: test/test_firmware_update_object.cpp:36

### DOS Line Endings
**Check**: `file script.sh` (should NOT say "CRLF")
**Fix**: `dos2unix script.sh`

---

## Documentation Index

| Issue | Quick Fix | Detailed Docs |
|-------|-----------|---------------|
| Brotli linking | Issues/b09_fix.md | Issues/b09_COMPLETE.md |
| Test hang | Issues/test_hang_fix.md | Issues/b10_FIXES_SUMMARY.md |
| Line endings | docs/SCRIPT_LINE_ENDINGS.md | Issues/SCRIPT_LINE_ENDINGS_FIX.md |
| Session summary | - | Issues/SESSION_SUMMARY_2026-04-30.md |

---

## Quick Checks

```bash
# Check Brotli libraries
grep -n "BROTLI_COMMON_LIB" CMakeLists.txt

# Check test config
grep -n "rollbackEnabled" test/test_firmware_update_object.cpp

# Check script line endings
./scripts/verify_script_line_endings.sh

# Check build status
cd build && make -j$(nproc) 2>&1 | tee ../Issues/latest.log
```

---

**Last Updated**: 2026-04-30
**Build Version**: v1.2.2
**Status**: All fixes applied ✅
