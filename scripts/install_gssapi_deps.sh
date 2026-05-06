#!/bin/bash
# Install GSSAPI/Kerberos dependencies for static linking
# This script detects your Linux distribution and installs the appropriate packages

set -e

echo "=========================================="
echo "GSSAPI/Kerberos Dependency Installer"
echo "=========================================="
echo ""

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    VERSION=$VERSION_ID
else
    echo "Cannot detect OS. Please install manually."
    exit 1
fi

echo "Detected OS: $OS $VERSION"
echo ""

# Install based on OS
case "$OS" in
    ubuntu|debian)
        echo "Installing packages for Ubuntu/Debian..."
        sudo apt-get update
        sudo apt-get install -y \
            libkrb5-dev \
            libgssapi-krb5-2 \
            libkeyutils-dev \
            libcom-err2 \
            pkg-config
        ;;

    rhel|centos|fedora)
        if command -v dnf &> /dev/null; then
            echo "Installing packages for Fedora/RHEL 8+..."
            sudo dnf install -y \
                krb5-devel \
                keyutils-libs-devel \
                libcom_err-devel \
                pkgconfig
        else
            echo "Installing packages for RHEL/CentOS 7..."
            sudo yum install -y \
                krb5-devel \
                keyutils-libs-devel \
                libcom_err-devel \
                pkgconfig
        fi
        ;;

    arch|manjaro)
        echo "Installing packages for Arch Linux..."
        sudo pacman -S --noconfirm krb5 keyutils pkgconf
        ;;

    alpine)
        echo "Installing packages for Alpine Linux..."
        apk add \
            krb5-dev \
            keyutils-dev \
            e2fsprogs-dev \
            pkgconfig
        ;;

    *)
        echo "Unsupported OS: $OS"
        echo ""
        echo "Please install the following packages manually:"
        echo "  - Kerberos 5 development files (libkrb5-dev or krb5-devel)"
        echo "  - GSSAPI libraries (usually included with Kerberos)"
        echo "  - Keyutils development files"
        echo "  - pkg-config"
        exit 1
        ;;
esac

echo ""
echo "=========================================="
echo "Verifying Installation"
echo "=========================================="
echo ""

# Verify installation using pkg-config
if pkg-config --exists krb5-gssapi; then
    echo "✓ GSSAPI found via pkg-config"
    echo "  Version: $(pkg-config --modversion krb5-gssapi)"
    echo "  Libs: $(pkg-config --libs krb5-gssapi)"
else
    echo "⚠ GSSAPI not found via pkg-config, checking libraries directly..."
fi

# Check for library files
LIBS_FOUND=0
for lib in libgssapi_krb5 libkrb5 libk5crypto libcom_err libkeyutils; do
    if ldconfig -p | grep -q "$lib"; then
        echo "  ✓ Found $lib"
        LIBS_FOUND=$((LIBS_FOUND + 1))
    else
        echo "  ✗ Missing $lib"
    fi
done

echo ""
if [ $LIBS_FOUND -ge 2 ]; then
    echo "✅ Installation successful!"
    echo ""
    echo "Next steps:"
    echo "  1. Reconfigure your build: cd build && cmake .."
    echo "  2. Rebuild: make -j\$(nproc)"
    echo ""
    echo "The build should now link GSSAPI libraries automatically."
else
    echo "⚠ Installation may be incomplete."
    echo "  Please check your package manager and try manual installation."
fi

echo ""
echo "=========================================="
echo "Testing pkg-config detection"
echo "=========================================="
echo ""

# Create a test CMake file to verify detection
cat > /tmp/test_gssapi.cmake << 'EOF'
cmake_minimum_required(VERSION 3.14)
project(TestGSSAPI)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(KRB5_GSSAPI QUIET krb5-gssapi)
    if(KRB5_GSSAPI_FOUND)
        message(STATUS "✓ CMake can detect GSSAPI via pkg-config")
        message(STATUS "  Libraries: ${KRB5_GSSAPI_LIBRARIES}")
    else()
        message(STATUS "✗ CMake cannot detect GSSAPI via pkg-config")
    endif()
else
    message(STATUS "✗ pkg-config not available to CMake")
endif()

find_library(GSSAPI_KRB5_LIB gssapi_krb5)
if(GSSAPI_KRB5_LIB)
    message(STATUS "✓ CMake can find libgssapi_krb5: ${GSSAPI_KRB5_LIB}")
else()
    message(STATUS "✗ CMake cannot find libgssapi_krb5")
endif()
EOF

cmake -P /tmp/test_gssapi.cmake
rm /tmp/test_gssapi.cmake

echo ""
echo "Installation complete!"
