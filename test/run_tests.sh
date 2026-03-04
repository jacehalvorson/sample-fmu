#!/bin/bash
# Quick build and test script for FMU test harness

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

echo "========================================="
echo "Building FMU and Test Harness"
echo "========================================="

# Build the FMU first
echo "Building FMU library..."
cd "$PROJECT_DIR"
mkdir -p build
cd build
cmake ..
cmake --build . --config Release

# Build the test harness
echo ""
echo "Building test harness..."
cd "$SCRIPT_DIR"
mkdir -p build
cd build
cmake ..
cmake --build .

# Run tests
echo ""
echo "========================================="
echo "Running FMU Tests"
echo "========================================="
echo ""

TEST_BIN="./test_fmu"
FMU_LIB="../../../build/binaries/libHelloWorld.so"

if [ ! -f "$TEST_BIN" ]; then
    echo "ERROR: Test executable not found: $TEST_BIN"
    exit 1
fi

if [ ! -f "$FMU_LIB" ]; then
    echo "ERROR: FMU library not found: $FMU_LIB"
    echo "Make sure to build the FMU first"
    exit 1
fi

"$TEST_BIN" "$FMU_LIB"
