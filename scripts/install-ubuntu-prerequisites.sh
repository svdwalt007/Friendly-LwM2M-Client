#!/bin/bash
###############################################################################
# Ubuntu Prerequisites Installation Script
# For Friendly LwM2M Client v1.2.0
#
# This script installs all required dependencies for building and running
# the Friendly LwM2M Client on Ubuntu 20.04, 22.04, and 24.04.
#
# Usage:
#   chmod +x install-ubuntu-prerequisites.sh
#   ./install-ubuntu-prerequisites.sh
#
# Or run directly:
#   bash install-ubuntu-prerequisites.sh
###############################################################################

set -e  # Exit on error

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Friendly LwM2M Client - Prerequisites${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# Check if running as root
if [ "$EUID" -eq 0 ]; then
    echo -e "${RED}Please do not run this script as root${NC}"
    echo "Run as a normal user - sudo will be requested when needed"
    exit 1
fi

# Detect Ubuntu version
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$NAME
    VER=$VERSION_ID
    echo -e "${GREEN}Detected: $OS $VER${NC}"
else
    echo -e "${RED}Cannot detect OS version${NC}"
    exit 1
fi

# Verify Ubuntu
if [[ ! "$OS" =~ "Ubuntu" ]]; then
    echo -e "${YELLOW}Warning: This script is designed for Ubuntu${NC}"
    read -p "Continue anyway? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo ""
echo -e "${GREEN}Step 1: Updating package lists...${NC}"
sudo apt-get update

echo ""
echo -e "${GREEN}Step 2: Installing build essentials...${NC}"
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    pkg-config

echo ""
echo -e "${GREEN}Step 3: Installing required libraries...${NC}"
sudo apt-get install -y \
    libssl-dev \
    libcurl4-openssl-dev \
    zlib1g-dev

echo ""
echo -e "${GREEN}Step 4: Installing optional libraries (recommended)...${NC}"
sudo apt-get install -y \
    libmosquitto-dev \
    mosquitto-clients \
    libprotobuf-dev \
    protobuf-compiler \
    grpc++ \
    libgrpc++-dev \
    protobuf-compiler-grpc

echo ""
echo -e "${GREEN}Step 5: Installing testing frameworks...${NC}"
sudo apt-get install -y \
    libgtest-dev \
    libgmock-dev \
    gcovr \
    lcov

# Check for Google Test installation
if [ -d "/usr/src/gtest" ] && [ ! -f "/usr/lib/libgtest.a" ]; then
    echo ""
    echo -e "${YELLOW}Building Google Test from source...${NC}"
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    sudo cp lib/*.a /usr/lib/ 2>/dev/null || sudo cp *.a /usr/lib/
    echo -e "${GREEN}Google Test built successfully${NC}"
fi

echo ""
echo -e "${GREEN}Step 6: Installing code quality tools...${NC}"
sudo apt-get install -y \
    clang-format \
    clang-tidy \
    cppcheck \
    valgrind

echo ""
echo -e "${GREEN}Step 7: Installing documentation tools...${NC}"
sudo apt-get install -y \
    doxygen \
    graphviz \
    python3-sphinx \
    python3-pip

echo ""
echo -e "${GREEN}Step 8: Installing version control tools...${NC}"
sudo apt-get install -y \
    git-lfs \
    tig

echo ""
echo -e "${GREEN}Step 9: Verifying installations...${NC}"

# Check critical tools
MISSING_TOOLS=0

check_tool() {
    if command -v $1 &> /dev/null; then
        echo -e "  ${GREEN}✓${NC} $1: $(command -v $1)"
        if [ "$2" = "version" ]; then
            echo "    Version: $($1 --version | head -n1)"
        fi
    else
        echo -e "  ${RED}✗${NC} $1: NOT FOUND"
        MISSING_TOOLS=$((MISSING_TOOLS + 1))
    fi
}

check_library() {
    if pkg-config --exists $1 2>/dev/null; then
        echo -e "  ${GREEN}✓${NC} $1: $(pkg-config --modversion $1)"
    else
        echo -e "  ${YELLOW}?${NC} $1: Not found via pkg-config (may still be installed)"
    fi
}

echo ""
echo "Build Tools:"
check_tool gcc version
check_tool g++ version
check_tool cmake version
check_tool make
check_tool ninja

echo ""
echo "Libraries:"
check_library openssl
check_library libcurl
check_library zlib
check_library libmosquitto
check_library protobuf

echo ""
echo "Testing Tools:"
check_tool gcovr version
check_tool lcov version

echo ""
echo "Version Control:"
check_tool git version

echo ""
echo -e "${GREEN}========================================${NC}"
if [ $MISSING_TOOLS -eq 0 ]; then
    echo -e "${GREEN}✓ Installation Complete!${NC}"
    echo ""
    echo "All prerequisites have been installed successfully."
    echo ""
    echo "Next steps:"
    echo "  1. Clone the repository (if not already done):"
    echo "     git clone --recurse-submodules https://github.com/friendly-tech/lwm2m-client.git"
    echo ""
    echo "  2. Build the project:"
    echo "     cd lwm2m-client"
    echo "     mkdir build && cd build"
    echo "     cmake .."
    echo "     make -j\$(nproc)"
    echo ""
    echo "  3. Run tests:"
    echo "     ctest -V"
    echo ""
    echo "See QUICK_START.md for detailed instructions."
else
    echo -e "${RED}✗ Installation completed with $MISSING_TOOLS missing tool(s)${NC}"
    echo ""
    echo "Please check the output above and install missing tools manually."
fi
echo -e "${GREEN}========================================${NC}"
