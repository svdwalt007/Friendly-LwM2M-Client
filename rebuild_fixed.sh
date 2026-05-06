#!/bin/bash
# Rebuild script with fixes applied and WPP objects disabled

echo "==================================="
echo "Rebuilding with Fixes Applied"
echo "==================================="
echo ""

cd build || exit 1

echo "Step 1: Cleaning CMake cache..."
rm -f CMakeCache.txt
rm -rf CMakeFiles/

echo ""
echo "Step 2: Reconfiguring CMake (disabling WPP objects, TinyDTLS, and tests)..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_WAN_FAILOVER=OFF \
    -DWITH_MULTIWAN_HEALTH=OFF \
    -DWITH_WIFI_CLIENT_MGMT=OFF \
    -DWITH_WIFI_CHANNEL_OPT=OFF \
    -DWITH_LAN_CONFIG=OFF \
    -DWITH_ROUTING_TABLE=OFF \
    -DWITH_VPN_CONFIG=OFF \
    -DDTLS_PROVIDER_TINYDTLS=OFF \
    -DBUILD_TESTS=OFF

if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: CMake configuration failed!"
    exit 1
fi

echo ""
echo "Step 3: Building the project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: Build failed!"
    exit 1
fi

echo ""
echo "==================================="
echo "Build Successful!"
echo "==================================="
echo ""
echo "The following fixes have been applied:"
echo "  ✓ Fix #1: Duplicate ACK response bug (Wakaama packet.c)"
echo "  ✓ Fix #2: ACL configuration (lwm2m_minimal_config.json)"
echo ""
echo "To test the fixes:"
echo "  cd examples"
echo "  ./friendly_example_client --config ../../examples/lwm2m_minimal_config.json -B -x Svdw007-777 -n svdwat007-777 -s none --disable-cid --dtls-provider=tinydtls -v -u coap://dem-iot.friendly-tech.com:5683"
echo ""
