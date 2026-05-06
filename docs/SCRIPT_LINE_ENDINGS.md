# Shell Script Line Ending Requirements

**Version**: 1.0
**Date**: 2026-04-30
**Critical**: All shell scripts MUST use Unix (LF) line endings

---

## Executive Summary

All shell script files (`*.sh`) in this repository **MUST** use Unix line endings (LF, `\n`) and **NOT** Windows/DOS line endings (CRLF, `\r\n`).

**Why this matters**:
- Scripts with DOS line endings fail with cryptic errors like `/bin/bash^M: bad interpreter`
- The `^M` represents the carriage return character (`\r`)
- Bash treats `\r` as part of the shebang path, breaking script execution
- This is a common issue when developing on Windows and deploying to Linux

---

## The Problem

### Symptom
```bash
$ ./build_script.sh
bash: ./build_script.sh: /bin/bash^M: bad interpreter: No such file or directory
```

### Root Cause
The script file has Windows line endings (CRLF):
```
#!/bin/bash\r\n
echo "Hello"\r\n
```

Bash reads the shebang as `#!/bin/bash\r` instead of `#!/bin/bash`, and tries to execute `/bin/bash\r` which doesn't exist.

### Why It Happens
- Windows text editors (Notepad, some IDEs) default to CRLF
- Git may convert line endings based on `core.autocrlf` setting
- Copy-pasting code from Windows to Linux
- Editing files on Windows that will run on Linux

---

## The Solution

### 1. Convert Existing Scripts

**Using dos2unix** (recommended):
```bash
# Install dos2unix
sudo apt-get install dos2unix

# Convert a single file
dos2unix script.sh

# Convert all shell scripts in project (from project root)
find . -type f -name "*.sh" -not -path "*/2305-Wakaama/*" -not -path "*/examples/coap/*" -exec dos2unix {} \;
```

**Using sed**:
```bash
sed -i 's/\r$//' script.sh
```

**Using tr**:
```bash
tr -d '\r' < script.sh > script_fixed.sh
mv script_fixed.sh script.sh
```

**In Vim/Vi**:
```vim
:set fileformat=unix
:wq
```

### 2. Configure Git

**Option A: Repository-wide** (recommended)

Create/update `.gitattributes` in project root:
```gitattributes
# Force LF for shell scripts
*.sh text eol=lf

# Force LF for common script files
*.bash text eol=lf
*.zsh text eol=lf

# Force LF for other Linux files
*.service text eol=lf
*.conf text eol=lf
Makefile text eol=lf
```

**Option B: User-level**

Configure Git to not convert line endings:
```bash
git config --global core.autocrlf false
```

Or for this repository only:
```bash
git config core.autocrlf false
```

### 3. IDE/Editor Configuration

**VS Code** (`.vscode/settings.json`):
```json
{
    "files.eol": "\n",
    "[shellscript]": {
        "files.eol": "\n"
    }
}
```

**Vim** (`~/.vimrc`):
```vim
" Use Unix line endings
set fileformat=unix
```

**Sublime Text**:
```
View → Line Endings → Unix
```

**Notepad++**:
```
Edit → EOL Conversion → Unix (LF)
```

---

## Verification

### Check Line Endings of a File

**Using file command**:
```bash
file script.sh
```
Output:
- **Correct**: `script.sh: Bourne-Again shell script, ASCII text executable`
- **Wrong**: `script.sh: Bourne-Again shell script, ASCII text executable, with CRLF line terminators`

**Using od (octal dump)**:
```bash
od -c script.sh | head -5
```
Look for `\r \n` (wrong) vs just `\n` (correct)

**Using cat with special characters**:
```bash
cat -A script.sh | head -5
```
Look for:
- **Wrong**: Lines end with `^M$`
- **Correct**: Lines end with just `$`

### Automated Verification Script

Use the provided verification script:
```bash
./scripts/verify_script_line_endings.sh
```

---

## Prevention

### 1. Pre-Commit Hook

Create `.git/hooks/pre-commit`:
```bash
#!/bin/bash
# Check for DOS line endings in shell scripts

echo "Checking for DOS line endings in shell scripts..."

bad_files=$(find . -type f -name "*.sh" -not -path "*/2305-Wakaama/*" -not -path "*/examples/coap/*" -exec file {} \; | grep -i "CRLF" | cut -d: -f1)

if [ -n "$bad_files" ]; then
    echo "ERROR: The following shell scripts have DOS line endings:"
    echo "$bad_files"
    echo ""
    echo "Fix with: dos2unix <file>"
    exit 1
fi

echo "✓ All shell scripts have Unix line endings"
exit 0
```

Make it executable:
```bash
chmod +x .git/hooks/pre-commit
```

### 2. CI/CD Check

Add to CI pipeline (GitHub Actions example):
```yaml
- name: Check shell script line endings
  run: |
    if find . -type f -name "*.sh" -not -path "*/2305-Wakaama/*" -exec file {} \; | grep -i "CRLF"; then
      echo "ERROR: Shell scripts with DOS line endings found"
      exit 1
    fi
```

### 3. EditorConfig

Create `.editorconfig` in project root:
```ini
# Top-most EditorConfig file
root = true

# Unix-style line endings for shell scripts
[*.sh]
end_of_line = lf
charset = utf-8
trim_trailing_whitespace = true
insert_final_newline = true

# Unix-style for other script types
[*.{bash,zsh}]
end_of_line = lf
charset = utf-8
```

---

## Current Status

### Scripts Converted (2026-04-30)

All 33 project-specific shell scripts have been converted to Unix format:

**Root directory**:
- configure.sh
- configure_cli.sh
- configure_preset.sh
- rebuild_fixed.sh
- test_build38_fix.sh

**scripts/ directory** (28 files):
- build/*.sh (17 files)
- *.sh (11 files)

**Other directories**:
- tests/tests.sh
- utils/container/wppstart.sh
- wpp/build_utils/wpp_headers_gen.sh

**Excluded** (external submodules):
- 2305-Wakaama/**/*.sh (Wakaama submodule)
- examples/coap/**/*.sh (libcoap submodule)
- doxg/**/*.sh (Doxygen setup)

### Git Configuration

**Required**: Create `.gitattributes` with:
```gitattributes
*.sh text eol=lf
```

This ensures Git always uses LF for shell scripts, regardless of platform.

---

## Troubleshooting

### Issue: Script still fails after conversion

**Check 1**: Verify conversion worked
```bash
file script.sh
```

**Check 2**: Verify shebang is correct
```bash
head -1 script.sh | od -c
```
Should show: `# ! / b i n / b a s h \n`

**Check 3**: File has execute permission
```bash
chmod +x script.sh
```

### Issue: Git converts line endings back to CRLF

**Solution**: Check Git configuration
```bash
git config --list | grep autocrlf
```

If `core.autocrlf=true`, either:
1. Set it to `false`: `git config core.autocrlf false`
2. OR use `.gitattributes` (recommended)

### Issue: IDE keeps changing line endings

**Solution**: Configure IDE (see IDE Configuration section above)

### Issue: Only some scripts have issues

**Solution**: Run batch conversion
```bash
cd /path/to/project
find . -name "*.sh" -exec dos2unix {} \;
```

---

## Best Practices

### ✅ DO

1. **Always use Unix line endings for shell scripts**
2. **Configure your IDE** to use LF for `.sh` files
3. **Use `.gitattributes`** to enforce LF in Git
4. **Run verification** before committing scripts
5. **Document** line ending requirements for contributors
6. **Use pre-commit hooks** to catch issues early

### ❌ DON'T

1. **Don't edit shell scripts in basic Windows text editors** (use VS Code, Vim, etc. with proper config)
2. **Don't rely on Git autocrlf** - use explicit `.gitattributes`
3. **Don't copy scripts** from Windows to Linux without checking
4. **Don't ignore file command warnings** about CRLF

---

## Platform-Specific Notes

### Windows (WSL/Git Bash/Cygwin)

When developing on Windows:
1. Use WSL2 for Linux compatibility
2. Configure Git: `git config core.autocrlf false`
3. Use IDE with LF support (VS Code recommended)
4. Install dos2unix: `sudo apt install dos2unix`

### Linux/macOS

Usually no issues, but:
1. Still use `.gitattributes` for team consistency
2. Verify imported scripts from Windows
3. Configure IDE defaults to LF

### CI/CD Environments

Always verify line endings in pipeline:
```bash
#!/bin/bash
set -e

echo "Verifying shell script line endings..."
bad_files=$(find . -name "*.sh" -exec file {} \; | grep CRLF || true)

if [ -n "$bad_files" ]; then
    echo "ERROR: Scripts with DOS line endings detected:"
    echo "$bad_files"
    exit 1
fi
```

---

## Quick Reference

| Command | Purpose |
|---------|---------|
| `dos2unix file.sh` | Convert DOS to Unix |
| `file file.sh` | Check line ending type |
| `cat -A file.sh` | Show line endings visually |
| `od -c file.sh` | Show line endings as characters |
| `git config core.autocrlf false` | Disable Git conversion |
| `chmod +x file.sh` | Make script executable |

| Line Ending | Symbol | Hex | Name |
|-------------|--------|-----|------|
| Unix (LF) | `\n` | `0x0A` | Line Feed |
| Windows (CRLF) | `\r\n` | `0x0D 0x0A` | Carriage Return + Line Feed |
| Old Mac (CR) | `\r` | `0x0D` | Carriage Return (obsolete) |

---

## Summary

**Critical requirement**: All `*.sh` files MUST use Unix (LF) line endings.

**Enforcement**:
1. ✅ All current scripts converted (33 files)
2. ⏳ TODO: Create `.gitattributes`
3. ⏳ TODO: Add pre-commit hook
4. ⏳ TODO: Add CI/CD verification

**For developers**:
- Configure your IDE to use LF for shell scripts
- Run `dos2unix` on any new scripts
- Use the verification script before committing

**For build systems**:
- Scripts will fail immediately if they have DOS line endings
- Error message: `/bin/bash^M: bad interpreter`
- Fix: `dos2unix script.sh`

---

**Last Updated**: 2026-04-30
**Converted Scripts**: 33 project files
**Status**: ✅ All project scripts now use Unix format
