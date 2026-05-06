#!/bin/bash
# Install ALL libcurl static linking dependencies
# This script installs every optional library that Ubuntu's libcurl might use

set -e

echo "=========================================="
echo "Complete libcurl Dependencies Installer"
echo "=========================================="
echo ""

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
else
    echo "Cannot detect OS. Assuming Ubuntu/Debian."
    OS="ubuntu"
fi

echo "Detected OS: $OS"
echo ""

# Full list of packages
case "$OS" in
    ubuntu|debian)
        echo "Installing ALL libcurl dependencies for Ubuntu/Debian..."
        echo "This may take a few minutes..."
        echo ""

        sudo apt-get update
        sudo apt-get install -y \
            libkrb5-dev \
            libgssapi-krb5-2 \
            libkeyutils-dev \
            libpsl-dev \
            librtmp-dev \
            libidn2-dev \
            libldap2-dev \
            libbrotli-dev \
            libnghttp2-dev \
            libzstd-dev \
            libssh-dev \
            zlib1g-dev \
            pkg-config
        ;;

    rhel|centos|fedora)
        if command -v dnf &> /dev/null; then
            echo "Installing ALL libcurl dependencies for Fedora/RHEL 8+..."
            sudo dnf install -y \
                krb5-devel \
                keyutils-libs-devel \
                libpsl-devel \
                librtmp-devel \
                libidn2-devel \
                openldap-devel \
                brotli-devel \
                libnghttp2-devel \
                libzstd-devel \
                libssh-devel \
                zlib-devel \
                pkgconfig
        else
            echo "Installing ALL libcurl dependencies for RHEL/CentOS 7..."
            sudo yum install -y \
                krb5-devel \
                keyutils-libs-devel \
                libpsl-devel \
                librtmp-devel \
                libidn2-devel \
                openldap-devel \
                brotli-devel \
                libnghttp2-devel \
                libzstd-devel \
                libssh-devel \
                zlib-devel \
                pkgconfig
        fi
        ;;

    arch|manjaro)
        echo "Installing ALL libcurl dependencies for Arch Linux..."
        sudo pacman -S --noconfirm \
            krb5 \
            keyutils \
            libpsl \
            librtmp \
            libidn2 \
            libldap \
            brotli \
            libnghttp2 \
            zstd \
            libssh \
            zlib \
            pkgconf
        ;;

    alpine)
        echo "Installing ALL libcurl dependencies for Alpine Linux..."
        apk add \
            krb5-dev \
            keyutils-dev \
            libpsl-dev \
            librtmp-dev \
            libidn2-dev \
            openldap-dev \
            brotli-dev \
            nghttp2-dev \
            zstd-dev \
            libssh-dev \
            zlib-dev \
            pkgconfig
        ;;

    *)
        echo "Unsupported OS: $OS"
        echo ""
        echo "Please install these packages manually:"
        echo "  - Kerberos (libkrb5-dev)"
        echo "  - GSSAPI (usually with Kerberos)"
        echo "  - libpsl (Public Suffix List)"
        echo "  - librtmp (RTMP streaming)"
        echo "  - libidn2 (Internationalized Domain Names)"
        echo "  - libldap + liblber (LDAP protocol)"
        echo "  - libbrotli (Brotli compression)"
        echo "  - libnghttp2 (HTTP/2)"
        echo "  - libzstd (Zstandard compression)"
        echo "  - libssh (SSH/SFTP)"
        echo "  - zlib (gzip compression)"
        echo "  - pkg-config"
        exit 1
        ;;
esac

echo ""
echo "=========================================="
echo "Verification"
echo "=========================================="
echo ""

# Verify critical libraries
MISSING=0
LIBS_TO_CHECK="gssapi_krb5 krb5 psl rtmp idn2 ldap lber brotlicommon brotlidec nghttp2 zstd ssh z"

for lib in $LIBS_TO_CHECK; do
    if ldconfig -p 2>/dev/null | grep -q "lib${lib}"; then
        echo "  ✓ lib${lib}"
    else
        echo "  ✗ lib${lib} (missing)"
        MISSING=$((MISSING + 1))
    fi
done

echo ""
if [ $MISSING -eq 0 ]; then
    echo "✅ All libraries installed successfully!"
else
    echo "⚠️  $MISSING libraries still missing - may need manual installation"
fi

echo ""
echo "=========================================="
echo "Next Steps"
echo "=========================================="
echo ""
echo "1. Reconfigure CMake:"
echo "   cd build && rm -rf * && cmake .."
echo ""
echo "2. Rebuild:"
echo "   make -j\$(nproc)"
echo ""
echo "The build should now complete without undefined reference errors."
