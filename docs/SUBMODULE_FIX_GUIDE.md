# Complete Submodule Fix Guide

## Quick Fix (Recommended)

Run the automated fix script:

```bash
cd /path/to/Friendly-LwM2M-Client
./scripts/fix_submodules.sh
```

This script will:
- ✅ Sync all submodule URLs
- ✅ Initialize main submodules (Wakaama, libcoap)
- ✅ Initialize nested submodules (tinycbor, tinydtls)
- ✅ Checkout correct commits for each submodule
- ✅ Verify everything is properly initialized

---

## What's Wrong Currently

Your submodules have these issues:

### 1. Wakaama tinydtls
- **Current commit:** `f5e0ca7` (newer version)
- **Expected commit:** `86f23c65` (older version)
- **Status:** Initialized but at wrong commit

### 2. libcoap
- **Status:** Initialized and working correctly

### 3. libcoap tinydtls
- **Status:** Initialized and working correctly

---

## Manual Fix (If Script Fails)

If the automated script doesn't work, follow these steps:

### Step 1: Clean Submodule State

```bash
cd /path/to/Friendly-LwM2M-Client

# Deinitialize all submodules
git submodule deinit -f --all

# Clean submodule directories
rm -rf .git/modules/2305-Wakaama
rm -rf .git/modules/examples/coap/libcoap

# Remove submodule working directories
rm -rf 2305-Wakaama
rm -rf examples/coap/libcoap
```

### Step 2: Re-initialize from Scratch

```bash
# Sync URLs
git submodule sync --recursive

# Initialize main submodules
git submodule update --init 2305-Wakaama
git submodule update --init examples/coap/libcoap
```

### Step 3: Initialize Wakaama Nested Submodules

```bash
cd 2305-Wakaama

# Initialize tinycbor (CBOR library - required)
git submodule update --init libs/tinycbor

# Initialize tinydtls (DTLS library - optional)
git submodule update --init examples/shared/tinydtls

# If tinydtls fails, try fetching first
cd examples/shared/tinydtls
git fetch --all --tags

# Try to checkout the expected commit
git checkout 86f23c65ffde9d46b6487d2210d98d58a41257d5

# If that fails, use v0.9-rc1 tag
git checkout v0.9-rc1

cd ../../..  # Back to project root
```

### Step 4: Initialize libcoap Nested Submodules

```bash
cd examples/coap/libcoap

# Initialize libcoap's tinydtls
git submodule update --init ext/tinydtls

# Checkout correct commit
cd ext/tinydtls
git fetch --all --tags
git checkout f1d47d533b70ac10e4ff0ae3a088dfe4f265d242

# If that fails, use v0.9-rc1 tag
git checkout v0.9-rc1

cd ../../../  # Back to project root
```

### Step 5: Verify Everything

```bash
cd /path/to/Friendly-LwM2M-Client

# Check submodule status
git submodule status --recursive
```

**Expected output:**
```
 1b80c3f21584f9747b418e25d92e49e6bb49627f 2305-Wakaama (1b80c3f)
 86f23c65ffde9d46b6487d2210d98d58a41257d5 2305-Wakaama/examples/shared/tinydtls (v0.9-rc1-209-g86f23c6)
 0d5538277d9975c448bd819572ca961542777aff 2305-Wakaama/libs/tinycbor (v0.6.0-21-g0d55382)
 5fd2f89ef068214130e5d60b7087ef48711fa615 examples/coap/libcoap (v4.2.1-1008-g5fd2f89e)
 f1d47d533b70ac10e4ff0ae3a088dfe4f265d242 examples/coap/libcoap/ext/tinydtls (v0.9-rc1-192-gf1d47d5)
```

**No `+` signs means everything is at the correct commit!**

---

## Alternative: Use SSH Instead of HTTPS

If you're having authentication issues, configure submodules to use SSH:

```bash
# Edit .gitmodules in Wakaama
cd 2305-Wakaama
nano .gitmodules

# Change HTTPS URLs to SSH:
# FROM: url = https://github.com/eclipse/tinydtls.git
# TO:   url = git@github.com:eclipse/tinydtls.git

# Sync the changes
git submodule sync

# Update
git submodule update --init --recursive
```

---

## Common Issues and Solutions

### Issue 1: "fatal: Unable to checkout"

**Cause:** The specific commit doesn't exist or network issues

**Solution:**
```bash
cd path/to/problematic/submodule

# Fetch everything
git fetch --all --tags

# List available tags
git tag

# Checkout a stable tag instead
git checkout v0.9-rc1  # or latest stable tag
```

### Issue 2: "No url found for submodule"

**Cause:** Submodule configuration out of sync

**Solution:**
```bash
# Sync submodule URLs
git submodule sync --recursive

# Then update
git submodule update --init --recursive
```

### Issue 3: "reference is not a tree"

**Cause:** Commit doesn't exist in the remote repository

**Solution:**
```bash
cd path/to/submodule

# Fetch all branches and tags
git fetch --all --tags

# Use the latest tag
git checkout $(git describe --tags --abbrev=0)

# Or use main/master branch
git checkout main  # or master
```

### Issue 4: Submodule shows as "modified" (+)

**Cause:** Submodule is at a different commit than expected

**Solution 1 - Accept the difference (recommended):**
- The `+` is harmless if the build works
- Modern commits are often newer and better

**Solution 2 - Force to expected commit:**
```bash
cd path/to/submodule
EXPECTED=$(git rev-parse $(git config -f ../.gitmodules submodule.$(basename $(pwd)).commit))
git checkout $EXPECTED
```

**Solution 3 - Update parent to use new commit:**
```bash
cd path/to/submodule
# Already at new commit
cd ..
git add path/to/submodule
git commit -m "Update submodule to newer commit"
```

---

## Understanding Submodule Status

When you run `git submodule status --recursive`:

| Symbol | Meaning | Action |
|--------|---------|--------|
| (space) | Submodule at correct commit | ✅ Good - no action needed |
| `+` | Submodule at different commit | ⚠️ Works but differs from expected |
| `-` | Submodule not initialized | ❌ Need to run `git submodule update --init` |
| `U` | Submodule has merge conflicts | ❌ Need to resolve conflicts |

---

## Submodule Tree Structure

```
Friendly-LwM2M-Client/
├── 2305-Wakaama/                          (main submodule)
│   ├── libs/tinycbor/                     (nested - required)
│   └── examples/shared/tinydtls/          (nested - optional)
└── examples/coap/libcoap/                 (main submodule)
    └── ext/tinydtls/                      (nested - optional)
```

**Required submodules:**
- ✅ `2305-Wakaama` - Core Wakaama LwM2M implementation
- ✅ `2305-Wakaama/libs/tinycbor` - CBOR encoding (required)

**Optional submodules:**
- ⚠️ `2305-Wakaama/examples/shared/tinydtls` - DTLS for examples (not needed for main library)
- ⚠️ `examples/coap/libcoap` - CoAP examples (not needed for main library)
- ⚠️ `examples/coap/libcoap/ext/tinydtls` - DTLS for CoAP examples

---

## What You Actually Need

For **Friendly-LwM2M-Client development**, you only need:

### Essential:
- ✅ `2305-Wakaama` (main code)
- ✅ `2305-Wakaama/libs/tinycbor` (CBOR support)

### Optional:
- ⚠️ `2305-Wakaama/examples/shared/tinydtls` - Only if building Wakaama examples with DTLS
- ⚠️ `examples/coap/libcoap/*` - Only if building CoAP examples

**The main library uses mbedTLS for DTLS, NOT tinydtls!**

---

## Testing the Fix

After fixing submodules, test the build:

```bash
# Run the complete setup
./scripts/dev_setup_complete.sh

# Or just build
cd build
cmake ..
make -j$(nproc)
```

**If the build succeeds, your submodules are properly configured!**

---

## Preventing Future Issues

### Update the Updated Script

The `dev_setup_complete.sh` script has been updated to handle submodule issues gracefully. It now:
- Initializes main submodules first
- Handles nested submodules individually
- Makes tinydtls optional (won't fail if it's unavailable)
- Provides clear warnings instead of failing

### Keep Submodules Updated

```bash
# Update all submodules to latest commits
git submodule update --remote --recursive

# Or update specific submodule
git submodule update --remote 2305-Wakaama
```

### Lock to Specific Commits (Recommended)

Don't track branches in submodules, use specific commits:

```bash
cd 2305-Wakaama
git checkout <specific-commit-hash>
cd ..
git add 2305-Wakaama
git commit -m "Pin Wakaama to specific stable commit"
```

---

## Summary

**Quick Fix:**
```bash
./scripts/fix_submodules.sh
```

**Manual Fix:**
```bash
# Clean state
git submodule deinit -f --all
rm -rf 2305-Wakaama examples/coap/libcoap

# Re-initialize
git submodule update --init --recursive

# If tinydtls fails, that's OK - it's optional
```

**Verify:**
```bash
git submodule status --recursive
# No lines should start with '-' (uninitialized)
```

**Build:**
```bash
./scripts/dev_setup_complete.sh
```

---

## Getting Help

If you still have issues:

1. **Check network connection** - Submodules require internet access to GitHub
2. **Check SSH keys** - If using SSH URLs, ensure your SSH key is configured
3. **Use HTTPS** - Edit `.gitmodules` to use HTTPS instead of SSH
4. **Skip optional submodules** - Build without tinydtls (it's not required)

**The build will work even if tinydtls submodules fail!**

---

**Status: Ready to Fix**

Run `./scripts/fix_submodules.sh` to initialize everything properly once and for all!
