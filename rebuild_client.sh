#!/bin/bash
# Quick rebuild of friendly_example_client with current configuration

echo "==================================="
echo "Rebuilding friendly_example_client"
echo "==================================="
echo ""

cd build || exit 1

echo "Rebuilding friendly_example_client..."
if command -v make &> /dev/null; then
    make friendly_example_client -j$(nproc)
elif command -v mingw32-make &> /dev/null; then
    mingw32-make friendly_example_client
elif command -v cmake &> /dev/null; then
    cmake --build . --target friendly_example_client
else
    echo "ERROR: No build tool found (make, mingw32-make, or cmake)"
    exit 1
fi

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Build successful!"
    echo ""
    echo "Client location: $(pwd)/examples/friendly_example_client"
    ls -lh examples/friendly_example_client
else
    echo ""
    echo "❌ Build failed!"
    exit 1
fi
