#!/bin/bash
# ============================================================================
# Build All Environments Script
# ============================================================================
# Builds the Friendly LwM2M Client in all four build environments
# Usage: ./scripts/build/build_all_environments.sh [clean]
# ============================================================================

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

cd "${PROJECT_ROOT}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check if clean requested
CLEAN_BUILD=0
if [ "$1" == "clean" ]; then
    CLEAN_BUILD=1
    echo -e "${YELLOW}Clean build requested - removing old build directories${NC}"
fi

# ============================================================================
# Function: Build Environment
# ============================================================================

build_environment() {
    local ENV_NAME=$1
    local BUILD_DIR="build_${ENV_NAME,,}"  # lowercase
    local START_TIME=$(date +%s)

    echo ""
    echo -e "${BLUE}============================================================================${NC}"
    echo -e "${BLUE}Building ${ENV_NAME} Environment${NC}"
    echo -e "${BLUE}============================================================================${NC}"

    # Clean if requested
    if [ ${CLEAN_BUILD} -eq 1 ] && [ -d "${BUILD_DIR}" ]; then
        echo -e "${YELLOW}Cleaning ${BUILD_DIR}${NC}"
        rm -rf "${BUILD_DIR}"
    fi

    # Configure
    echo -e "${GREEN}Configuring ${ENV_NAME} build...${NC}"
    if cmake -B "${BUILD_DIR}" -DBUILD_ENV="${ENV_NAME}" 2>&1 | tee "${BUILD_DIR}/cmake_output.log"; then
        echo -e "${GREEN}✓ Configuration successful${NC}"
    else
        echo -e "${RED}✗ Configuration failed${NC}"
        return 1
    fi

    # Build
    echo -e "${GREEN}Building ${ENV_NAME}...${NC}"
    if cmake --build "${BUILD_DIR}" -j$(nproc) 2>&1 | tee "${BUILD_DIR}/build_output.log"; then
        echo -e "${GREEN}✓ Build successful${NC}"
    else
        echo -e "${RED}✗ Build failed${NC}"
        return 1
    fi

    # Run tests for TEST and DEV environments
    if [ "${ENV_NAME}" == "TEST" ] || [ "${ENV_NAME}" == "DEV" ]; then
        echo -e "${GREEN}Running tests...${NC}"
        if (cd "${BUILD_DIR}" && ctest --output-on-failure 2>&1 | tee test_output.log); then
            echo -e "${GREEN}✓ Tests passed${NC}"
        else
            echo -e "${YELLOW}⚠ Some tests failed (check ${BUILD_DIR}/test_output.log)${NC}"
        fi
    fi

    # Get binary size for friendly_lwm2m library
    if [ -f "${BUILD_DIR}/libfriendly_lwm2m.a" ]; then
        local SIZE=$(du -h "${BUILD_DIR}/libfriendly_lwm2m.a" | cut -f1)
        echo -e "${GREEN}Binary size: ${SIZE}${NC}"
    fi

    local END_TIME=$(date +%s)
    local DURATION=$((END_TIME - START_TIME))
    echo -e "${GREEN}Build time: ${DURATION} seconds${NC}"
    echo ""
}

# ============================================================================
# Main Build Process
# ============================================================================

echo -e "${BLUE}╔══════════════════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   Friendly LwM2M Client - Multi-Environment Build                       ║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${YELLOW}Project: ${PROJECT_ROOT}${NC}"
echo -e "${YELLOW}Parallel jobs: $(nproc)${NC}"
echo ""

OVERALL_START=$(date +%s)

# Build all environments
FAILED_BUILDS=()

if ! build_environment "TEST"; then
    FAILED_BUILDS+=("TEST")
fi

if ! build_environment "DEV"; then
    FAILED_BUILDS+=("DEV")
fi

if ! build_environment "PREPROD"; then
    FAILED_BUILDS+=("PREPROD")
fi

if ! build_environment "PROD"; then
    FAILED_BUILDS+=("PROD")
fi

OVERALL_END=$(date +%s)
OVERALL_DURATION=$((OVERALL_END - OVERALL_START))

# ============================================================================
# Summary
# ============================================================================

echo -e "${BLUE}╔══════════════════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   Build Summary                                                          ║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Binary size comparison
echo -e "${YELLOW}Binary Size Comparison:${NC}"
printf "%-12s %12s %12s\n" "Environment" "Static Lib" "Example App"
echo "------------ ------------ ------------"

for ENV in TEST DEV PREPROD PROD; do
    BUILD_DIR="build_${ENV,,}"
    LIB_SIZE="-"
    APP_SIZE="-"

    if [ -f "${BUILD_DIR}/libfriendly_lwm2m.a" ]; then
        LIB_SIZE=$(du -h "${BUILD_DIR}/libfriendly_lwm2m.a" | cut -f1)
    fi

    if [ -f "${BUILD_DIR}/examples/friendly_example_client" ]; then
        APP_SIZE=$(du -h "${BUILD_DIR}/examples/friendly_example_client" | cut -f1)
    fi

    printf "%-12s %12s %12s\n" "$ENV" "$LIB_SIZE" "$APP_SIZE"
done

echo ""

# Failed builds
if [ ${#FAILED_BUILDS[@]} -gt 0 ]; then
    echo -e "${RED}Failed builds: ${FAILED_BUILDS[*]}${NC}"
    echo ""
else
    echo -e "${GREEN}All builds successful!${NC}"
    echo ""
fi

echo -e "${YELLOW}Total time: ${OVERALL_DURATION} seconds${NC}"
echo ""

# Exit with error if any builds failed
if [ ${#FAILED_BUILDS[@]} -gt 0 ]; then
    exit 1
fi

exit 0
