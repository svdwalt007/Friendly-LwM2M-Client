#!/bin/bash
# Install all build dependencies for WppExample

set -e

echo "Installing build dependencies for WppExample..."

# Update package list
sudo apt-get update

# Install autotools (needed for tinydtls)
echo "Installing autotools..."
sudo apt-get install -y autoconf automake libtool

# Install other build tools if not present
echo "Installing build essentials..."
sudo apt-get install -y build-essential cmake git pkg-config

# Install required libraries
echo "Installing required libraries..."
sudo apt-get install -y \
    libssl-dev \
    libcurl4-openssl-dev \
    zlib1g-dev

echo ""
echo "All dependencies installed successfully!"
echo ""
echo "You can now build WppExample with:"
echo "  cd _build"
echo "  cmake ../examples -DCMAKE_BUILD_TYPE=MinSizeRel"
echo "  make -j\$(nproc)"
