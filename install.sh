#!/bin/bash

# FastAlloc Installation / Setup Script

HEADER_FILE="FastAlloc.h"
DEMO_FILE="FastAlloc.cpp"
OUTPUT_BIN="fastalloc_test"

echo "========================================"
echo "      FastAlloc Installer & Setup       "
echo "========================================"

if [ ! -f "$HEADER_FILE" ]; then
    echo "Error: $HEADER_FILE not found in current directory."
    exit 1
fi

echo "[1/3] Verifying environment..."
if command -v clang++ &> /dev/null; then
    COMPILER="clang++"
elif command -v g++ &> /dev/null; then
    COMPILER="g++"
else
    echo "Error: No C++ compiler (clang++ or g++) found."
    exit 1
fi
echo "      Using compiler: $COMPILER"

echo "[2/3] Building Test Demo..."
$COMPILER -O3 -o $OUTPUT_BIN $DEMO_FILE
if [ $? -eq 0 ]; then
    echo "      Build successful."
else
    echo "      Build failed! Please check your code."
    exit 1
fi

echo "[3/3] Installation Options"
echo "----------------------------------------"
echo "OPTION A: Local Project Use (Recommended)"
echo "   1. Copy '$HEADER_FILE' to your project folder."
echo "   2. Add '#include \"$HEADER_FILE\"' to your source."
echo ""
echo "OPTION B: System-wide Install"
echo "   Run the following command to install to /usr/local/include:"
echo "   sudo cp $PWD/$HEADER_FILE /usr/local/include/"
echo "   (Then you can use #include <FastAlloc.h> in any project)"
echo "----------------------------------------"

echo "Done. You can run ./$OUTPUT_BIN to verify performance."
