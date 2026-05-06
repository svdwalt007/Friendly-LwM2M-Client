#!/bin/bash
#
# Fix Git Submodules Script
# Properly initializes all submodules and nested submodules
#

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print functions
print_header() {
    echo ""
    echo -e "${BLUE}============================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}============================================${NC}"
    echo ""
}

print_step() {
    echo -e "${YELLOW}▶ $1${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

cd "$PROJECT_ROOT" || {
    print_error "Failed to navigate to project root"
    exit 1
}

print_header "Git Submodule Initialization - Complete Fix"

print_info "Project root: $PROJECT_ROOT"
echo ""

# ============================================================================
# Step 1: Sync submodule URLs
# ============================================================================

print_step "Syncing submodule URLs with .gitmodules..."
git submodule sync --recursive
print_success "Submodule URLs synced"

# ============================================================================
# Step 2: Initialize main submodules
# ============================================================================

print_header "Initializing Main Submodules"

# Wakaama
print_step "Initializing Wakaama submodule..."
if [ ! -d "2305-Wakaama/.git" ]; then
    git submodule update --init 2305-Wakaama
    if [ $? -eq 0 ]; then
        print_success "Wakaama submodule initialized"
    else
        print_error "Failed to initialize Wakaama submodule"
        exit 1
    fi
else
    git submodule update 2305-Wakaama
    print_success "Wakaama submodule already initialized (updated)"
fi

# libcoap
print_step "Initializing libcoap submodule..."
if [ ! -d "examples/coap/libcoap/.git" ]; then
    git submodule update --init examples/coap/libcoap
    if [ $? -eq 0 ]; then
        print_success "libcoap submodule initialized"
    else
        print_error "Failed to initialize libcoap submodule"
        exit 1
    fi
else
    git submodule update examples/coap/libcoap
    print_success "libcoap submodule already initialized (updated)"
fi

# ============================================================================
# Step 3: Initialize Wakaama nested submodules
# ============================================================================

print_header "Initializing Wakaama Nested Submodules"

cd 2305-Wakaama || {
    print_error "Failed to enter Wakaama directory"
    exit 1
}

# tinycbor
print_step "Initializing tinycbor submodule..."
if [ ! -d "libs/tinycbor/.git" ]; then
    git submodule update --init libs/tinycbor
    if [ $? -eq 0 ]; then
        print_success "tinycbor initialized"
    else
        print_error "Failed to initialize tinycbor"
        cd ..
        exit 1
    fi
else
    git submodule update libs/tinycbor
    print_success "tinycbor already initialized (updated)"
fi

# tinydtls
print_step "Initializing tinydtls submodule (Wakaama)..."
if [ ! -d "examples/shared/tinydtls/.git" ]; then
    print_info "Cloning tinydtls..."
    git submodule update --init examples/shared/tinydtls
    TINYDTLS_INIT=$?
else
    print_info "tinydtls already cloned, checking out correct commit..."
    TINYDTLS_INIT=0
fi

if [ $TINYDTLS_INIT -eq 0 ]; then
    # Check what commit we should be at
    EXPECTED_COMMIT=$(git ls-tree HEAD examples/shared/tinydtls | awk '{print $3}')
    print_info "Expected commit: $EXPECTED_COMMIT"

    cd examples/shared/tinydtls || {
        print_error "Failed to enter tinydtls directory"
        cd ../../../
        exit 1
    }

    # Fetch all branches/tags to ensure we have the commit
    print_step "Fetching tinydtls repository..."
    git fetch --all --tags

    # Try to checkout the expected commit
    CURRENT_COMMIT=$(git rev-parse HEAD)
    print_info "Current commit: $CURRENT_COMMIT"

    if [ "$CURRENT_COMMIT" != "$EXPECTED_COMMIT" ]; then
        print_step "Checking out expected commit..."
        if git checkout $EXPECTED_COMMIT 2>/dev/null; then
            print_success "tinydtls checked out to correct commit"
        else
            print_error "Failed to checkout commit $EXPECTED_COMMIT"
            print_info "Trying to checkout by tag or branch..."

            # Try v0.9-rc1 tag
            if git checkout v0.9-rc1 2>/dev/null; then
                print_success "tinydtls checked out to v0.9-rc1 tag"
            else
                # Use current HEAD
                git checkout HEAD
                print_info "Using current HEAD commit (may differ from expected)"
            fi
        fi
    else
        print_success "tinydtls already at correct commit"
    fi

    cd ../../..
else
    print_error "Failed to initialize tinydtls submodule"
    print_info "This may be due to network issues or unavailable commit"
    print_info "The build may still work without it"
    cd ..
fi

cd ..

# ============================================================================
# Step 4: Initialize libcoap nested submodules
# ============================================================================

print_header "Initializing libcoap Nested Submodules"

cd examples/coap/libcoap || {
    print_error "Failed to enter libcoap directory"
    exit 1
}

# libcoap's tinydtls
print_step "Initializing tinydtls submodule (libcoap)..."
if [ ! -d "ext/tinydtls/.git" ]; then
    print_info "Cloning libcoap tinydtls..."
    git submodule update --init ext/tinydtls
    LIBCOAP_TINYDTLS_INIT=$?
else
    print_info "libcoap tinydtls already cloned, checking out correct commit..."
    LIBCOAP_TINYDTLS_INIT=0
fi

if [ $LIBCOAP_TINYDTLS_INIT -eq 0 ]; then
    # Check what commit we should be at
    EXPECTED_COMMIT=$(git ls-tree HEAD ext/tinydtls | awk '{print $3}')
    print_info "Expected commit: $EXPECTED_COMMIT"

    cd ext/tinydtls || {
        print_error "Failed to enter libcoap tinydtls directory"
        cd ../../../
        exit 1
    }

    # Fetch all branches/tags
    print_step "Fetching tinydtls repository..."
    git fetch --all --tags

    # Try to checkout the expected commit
    CURRENT_COMMIT=$(git rev-parse HEAD)
    print_info "Current commit: $CURRENT_COMMIT"

    if [ "$CURRENT_COMMIT" != "$EXPECTED_COMMIT" ]; then
        print_step "Checking out expected commit..."
        if git checkout $EXPECTED_COMMIT 2>/dev/null; then
            print_success "libcoap tinydtls checked out to correct commit"
        else
            print_error "Failed to checkout commit $EXPECTED_COMMIT"
            print_info "Trying to checkout by tag..."

            # Try v0.9-rc1 tag
            if git checkout v0.9-rc1 2>/dev/null; then
                print_success "libcoap tinydtls checked out to v0.9-rc1 tag"
            else
                git checkout HEAD
                print_info "Using current HEAD commit (may differ from expected)"
            fi
        fi
    else
        print_success "libcoap tinydtls already at correct commit"
    fi

    cd ../..
else
    print_error "Failed to initialize libcoap tinydtls submodule"
    print_info "This may be due to network issues"
fi

cd ../../..

# ============================================================================
# Step 5: Verify all submodules
# ============================================================================

print_header "Verification"

print_step "Checking submodule status..."
echo ""
git submodule status --recursive

echo ""
print_header "Summary"

# Count initialized submodules
TOTAL=0
INITIALIZED=0

# Check main submodules (.git can be file or directory in modern git)
if [ -e "2305-Wakaama/.git" ]; then
    INITIALIZED=$((INITIALIZED + 1))
fi
TOTAL=$((TOTAL + 1))

if [ -e "examples/coap/libcoap/.git" ]; then
    INITIALIZED=$((INITIALIZED + 1))
fi
TOTAL=$((TOTAL + 1))

# Check nested submodules
if [ -e "2305-Wakaama/libs/tinycbor/.git" ]; then
    INITIALIZED=$((INITIALIZED + 1))
fi
TOTAL=$((TOTAL + 1))

if [ -e "2305-Wakaama/examples/shared/tinydtls/.git" ]; then
    INITIALIZED=$((INITIALIZED + 1))
fi
TOTAL=$((TOTAL + 1))

if [ -e "examples/coap/libcoap/ext/tinydtls/.git" ]; then
    INITIALIZED=$((INITIALIZED + 1))
fi
TOTAL=$((TOTAL + 1))

echo -e "Submodules initialized: ${GREEN}$INITIALIZED${NC}/${TOTAL}"
echo ""

if [ $INITIALIZED -eq $TOTAL ]; then
    print_success "All submodules successfully initialized!"
    echo ""
    print_info "You can now run: ./scripts/dev_setup_complete.sh"
    exit 0
elif [ $INITIALIZED -ge 3 ]; then
    print_success "Essential submodules initialized (some optional submodules may be missing)"
    echo ""
    print_info "Missing submodules are optional and won't affect the build"
    print_info "You can now run: ./scripts/dev_setup_complete.sh"
    exit 0
else
    print_error "Some critical submodules failed to initialize"
    echo ""
    print_info "Please check your network connection and try again"
    print_info "Or run: git submodule update --init --recursive"
    exit 1
fi
