#!/bin/bash
# Build script for HelloWorld FMU
# Usage: ./build.sh [linux|arm|arm64|clean|help]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

print_usage() {
    echo "Usage: $0 [command]"
    echo ""
    echo "Commands:"
    echo "  linux     Build for Linux x86_64 (native)"
    echo "  arm       Build for ARM 32-bit (cross-compile)"
    echo "  arm64     Build for ARM 64-bit (cross-compile)"
    echo "  freertos  Build for FreeRTOS on ARM"
    echo "  qnx       Build for QNX on ARM"
    echo "  clean     Remove all build directories"
    echo "  help      Show this message"
    echo ""
    echo "Examples:"
    echo "  ./build.sh linux      # Linux x86_64 native build"
    echo "  ./build.sh arm        # NXP i.MX95 32-bit cross-compile"
    echo "  ./build.sh arm64      # NXP i.MX95 64-bit cross-compile"
}

build_linux() {
    echo "========================================="
    echo "Building for Linux x86_64 (native)..."
    echo "========================================="
    rm -rf "${BUILD_DIR}-linux"
    mkdir -p "${BUILD_DIR}-linux"
    cd "${BUILD_DIR}-linux"
    cmake ..
    cmake --build . --config Release
    echo ""
    echo "✅ Linux build complete: ${BUILD_DIR}-linux/binaries/libHelloWorld.so"
}

build_arm() {
    echo "========================================="
    echo "Building for ARM 32-bit (cross-compile)..."
    echo "========================================="
    
    if ! command -v arm-linux-gnueabihf-gcc &> /dev/null; then
        echo "❌ arm-linux-gnueabihf-gcc not found. Install with:"
        echo "   sudo apt-get install gcc-arm-linux-gnueabihf"
        exit 1
    fi
    
    rm -rf "${BUILD_DIR}-arm"
    mkdir -p "${BUILD_DIR}-arm"
    cd "${BUILD_DIR}-arm"
    cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-imx95.cmake ..
    cmake --build .
    echo ""
    echo "✅ ARM 32-bit build complete: ${BUILD_DIR}-arm/binaries/libHelloWorld.so"
}

build_arm64() {
    echo "========================================="
    echo "Building for ARM 64-bit (cross-compile)..."
    echo "========================================="
    
    if ! command -v aarch64-linux-gnu-gcc &> /dev/null; then
        echo "❌ aarch64-linux-gnu-gcc not found. Install with:"
        echo "   sudo apt-get install gcc-aarch64-linux-gnu"
        exit 1
    fi
    
    rm -rf "${BUILD_DIR}-arm64"
    mkdir -p "${BUILD_DIR}-arm64"
    cd "${BUILD_DIR}-arm64"
    
    # Create temporary toolchain for aarch64
    cat > toolchain.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++")
set(CMAKE_AR "aarch64-linux-gnu-ar")
set(CMAKE_RANLIB "aarch64-linux-gnu-ranlib")
set(CMAKE_C_FLAGS "-march=armv8-a")
EOF
    
    cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake ..
    cmake --build .
    echo ""
    echo "✅ ARM 64-bit build complete: ${BUILD_DIR}-arm64/binaries/libHelloWorld.so"
}

build_freertos() {
    echo "========================================="
    echo "Building for FreeRTOS on ARM..."
    echo "========================================="
    
    if [ -z "$FREERTOS_DIR" ]; then
        echo "❌ FREERTOS_DIR environment variable not set"
        echo "   Set with: export FREERTOS_DIR=/path/to/freertos"
        exit 1
    fi
    
    rm -rf "${BUILD_DIR}-freertos"
    mkdir -p "${BUILD_DIR}-freertos"
    cd "${BUILD_DIR}-freertos"
    cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-imx95.cmake \
          -DFREERTOS_DIR="$FREERTOS_DIR" ..
    cmake --build .
    echo ""
    echo "✅ FreeRTOS build complete: ${BUILD_DIR}-freertos/binaries/libHelloWorld.so"
}

build_qnx() {
    echo "========================================="
    echo "Building for QNX on ARM..."
    echo "========================================="
    
    if [ -z "$QNX_BASE" ]; then
        echo "❌ QNX_BASE environment variable not set"
        echo "   Set with: export QNX_BASE=/opt/qnx"
        exit 1
    fi
    
    rm -rf "${BUILD_DIR}-qnx"
    mkdir -p "${BUILD_DIR}-qnx"
    cd "${BUILD_DIR}-qnx"
    cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-imx95.cmake \
          -DQNX_BASE="$QNX_BASE" ..
    cmake --build .
    echo ""
    echo "✅ QNX build complete: ${BUILD_DIR}-qnx/binaries/libHelloWorld.so"
}

clean() {
    echo "Cleaning build directories..."
    rm -rf "${BUILD_DIR}"* 
    echo "✅ Clean complete"
}

# Main script logic
if [ $# -eq 0 ]; then
    print_usage
    exit 0
fi

case "$1" in
    linux)
        build_linux
        ;;
    arm)
        build_arm
        ;;
    arm64)
        build_arm64
        ;;
    freertos)
        build_freertos
        ;;
    qnx)
        build_qnx
        ;;
    clean)
        clean
        ;;
    help|-h|--help)
        print_usage
        ;;
    *)
        echo "Unknown command: $1"
        print_usage
        exit 1
        ;;
esac
