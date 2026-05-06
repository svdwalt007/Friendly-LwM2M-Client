#!/bin/bash
# Install dependencies required for the MINIMAL libcurl built from source
#
# IMPORTANT: This script is for projects using a custom minimal libcurl
# built from source with the following configure options:
#   --disable-ldap --disable-ldaps --without-libssh --without-libssh2
#   --without-libidn2 --without-librtmp --without-libpsl
#
# This reduces dependencies from 28 libraries to ~8 libraries (71% reduction).
#
# The minimal libcurl only requires:
# - OpenSSL (for HTTPS/TLS)
# - zlib (for compression)
# - Brotli (for brotli compression - optional but recommended)
# - nghttp2 (for HTTP/2 - optional but recommended)
# - zstd (for zstandard compression - optional but recommended)
# - Kerberos/GSSAPI (if libcurl was built with GSSAPI support - optional)
#
# For build instructions, see: docs/MINIMAL_CURL_BUILD.md

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${GREEN}Installing minimal libcurl dependencies...${NC}"
echo -e "${BLUE}This is for use with custom minimal libcurl from /usr/local/lib${NC}"
echo ""

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    VER=$VERSION_ID
else
    echo -e "${RED}Cannot detect OS. Please install manually.${NC}"
    exit 1
fi

case "$OS" in
    ubuntu|debian)
        echo -e "${YELLOW}Detected Ubuntu/Debian${NC}"
        sudo apt-get update
        sudo apt-get install -y \
            libssl-dev \
            zlib1g-dev \
            libbrotli-dev \
            libnghttp2-dev \
            libzstd-dev \
            libkrb5-dev
        ;;

    rhel|centos|fedora|rocky|almalinux)
        echo -e "${YELLOW}Detected RHEL/Fedora${NC}"
        sudo dnf install -y \
            openssl-devel \
            zlib-devel \
            brotli-devel \
            libnghttp2-devel \
            libzstd-devel \
            krb5-devel
        ;;

    arch|manjaro)
        echo -e "${YELLOW}Detected Arch Linux${NC}"
        sudo pacman -S --needed \
            openssl \
            zlib \
            brotli \
            libnghttp2 \
            zstd \
            krb5
        ;;

    alpine)
        echo -e "${YELLOW}Detected Alpine Linux${NC}"
        sudo apk add \
            openssl-dev \
            zlib-dev \
            brotli-dev \
            nghttp2-dev \
            zstd-dev \
            krb5-dev
        ;;

    *)
        echo -e "${RED}Unsupported OS: $OS${NC}"
        echo "Please install the following libraries manually:"
        echo "  Required:"
        echo "    - openssl-dev (libssl-dev)"
        echo "    - zlib-dev (zlib1g-dev)"
        echo "  Optional but recommended:"
        echo "    - brotli-dev (libbrotli-dev)"
        echo "    - nghttp2-dev (libnghttp2-dev)"
        echo "    - zstd-dev (libzstd-dev)"
        echo "    - krb5-dev (libkrb5-dev)"
        exit 1
        ;;
esac

echo ""
echo -e "${GREEN}Verifying installations...${NC}"

# Verify each library
LIBS=(
    "ssl:openssl:required"
    "crypto:openssl:required"
    "z:zlib:required"
    "brotlicommon:libbrotli:optional"
    "brotlidec:libbrotli:optional"
    "brotlienc:libbrotli:optional"
    "nghttp2:libnghttp2:optional"
    "zstd:libzstd:optional"
    "gssapi_krb5:krb5:optional"
)

MISSING_REQUIRED=()
MISSING_OPTIONAL=()

for lib_info in "${LIBS[@]}"; do
    lib_name="${lib_info%%:*}"
    rest="${lib_info#*:}"
    pkg_name="${rest%%:*}"
    req_type="${rest##*:}"

    # Try to find the library
    if ldconfig -p 2>/dev/null | grep -q "lib${lib_name}.so"; then
        echo -e "  ${GREEN}✓${NC} lib${lib_name} found (${pkg_name})"
    elif [ -f "/usr/lib/x86_64-linux-gnu/lib${lib_name}.so" ] || \
         [ -f "/usr/lib/lib${lib_name}.so" ] || \
         [ -f "/usr/lib64/lib${lib_name}.so" ]; then
        echo -e "  ${GREEN}✓${NC} lib${lib_name} found (${pkg_name})"
    else
        echo -e "  ${RED}✗${NC} lib${lib_name} NOT found (${pkg_name})"
        if [ "$req_type" = "required" ]; then
            MISSING_REQUIRED+=("$pkg_name")
        else
            MISSING_OPTIONAL+=("$pkg_name")
        fi
    fi
done

echo ""

if [ ${#MISSING_REQUIRED[@]} -eq 0 ]; then
    echo -e "${GREEN}All required dependencies installed successfully!${NC}"

    if [ ${#MISSING_OPTIONAL[@]} -gt 0 ]; then
        echo -e "${YELLOW}Optional dependencies missing (build will work but may have reduced functionality):${NC}"
        for missing_pkg in "${MISSING_OPTIONAL[@]}"; do
            echo "  - $missing_pkg"
        done
        echo ""
    fi

    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}Next steps:${NC}"
    echo ""
    echo "1. Build minimal libcurl (if not already done):"
    echo "   See: docs/MINIMAL_CURL_BUILD.md"
    echo ""
    echo "2. Build the project:"
    echo "   cd build"
    echo "   rm -rf *  # Clean build recommended"
    echo "   cmake .."
    echo "   make -j\$(nproc)"
    echo ""
    echo -e "${GREEN}Expected dependency count: ~8 libraries (vs 28 with full libcurl)${NC}"
    echo -e "${BLUE}========================================${NC}"
    exit 0
else
    echo -e "${RED}ERROR: Required dependencies are missing:${NC}"
    for missing_pkg in "${MISSING_REQUIRED[@]}"; do
        echo "  - $missing_pkg"
    done
    echo ""
    echo "Please install them and try again."
    exit 1
fi
