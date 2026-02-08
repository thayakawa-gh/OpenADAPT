#!/bin/bash

# OpenADAPT Build Script with Clang
# This script builds OpenADAPT using Clang compiler for better performance

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}OpenADAPT - Building with Clang${NC}"
echo "======================================="

# Check if clang is installed
if ! command -v clang++ &> /dev/null; then
    echo -e "${RED}Error: clang++ not found${NC}"
    echo "Please install Clang:"
    echo "  Ubuntu/Debian: sudo apt install clang"
    echo "  Fedora/RHEL:   sudo dnf install clang"
    echo "  macOS:         xcode-select --install"
    exit 1
fi

# Display Clang version
echo -e "\n${YELLOW}Using Clang version:${NC}"
clang++ --version | head -n 1

# Parse command line arguments
BUILD_TYPE="Release"
BUILD_EXAMPLES=OFF
BUILD_TESTS=OFF
ENABLE_MODULE=OFF
BUILD_DIR="build_clang"
INSTALL_PREFIX=""
CLEAN=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --examples)
            BUILD_EXAMPLES=ON
            shift
            ;;
        --tests)
            BUILD_TESTS=ON
            shift
            ;;
        --module)
            ENABLE_MODULE=ON
            shift
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --debug          Build in Debug mode (default: Release)"
            echo "  --examples       Build example programs"
            echo "  --tests          Build test programs (requires GTest)"
            echo "  --module         Enable C++20 module support (experimental)"
            echo "  --build-dir DIR  Specify build directory (default: build_clang)"
            echo "  --prefix PATH    Set installation prefix"
            echo "  --clean          Clean build directory before building"
            echo "  --help           Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Clean build directory if requested
if [ "$CLEAN" = true ] && [ -d "$BUILD_DIR" ]; then
    echo -e "\n${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
echo -e "\n${YELLOW}Creating build directory: $BUILD_DIR${NC}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Prepare CMake arguments
CMAKE_ARGS=(
    "-DCMAKE_C_COMPILER=clang"
    "-DCMAKE_CXX_COMPILER=clang++"
    "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    "-DBUILD_EXAMPLES=$BUILD_EXAMPLES"
    "-DBUILD_TEST=$BUILD_TESTS"
    "-DENABLE_MODULE=$ENABLE_MODULE"
)

if [ -n "$INSTALL_PREFIX" ]; then
    CMAKE_ARGS+=("-DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX")
fi

# Configure
echo -e "\n${YELLOW}Configuring with CMake...${NC}"
echo "Build type: $BUILD_TYPE"
echo "Examples: $BUILD_EXAMPLES"
echo "Tests: $BUILD_TESTS"
echo "Module: $ENABLE_MODULE"
if [ -n "$INSTALL_PREFIX" ]; then
    echo "Install prefix: $INSTALL_PREFIX"
fi

CC=clang CXX=clang++ cmake .. "${CMAKE_ARGS[@]}"

# Build
echo -e "\n${YELLOW}Building...${NC}"
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)
echo "Using $NPROC parallel jobs"

make -j"$NPROC"

echo -e "\n${GREEN}Build completed successfully!${NC}"
echo -e "\nBuild artifacts are in: ${BUILD_DIR}"

if [ "$BUILD_EXAMPLES" = "ON" ]; then
    echo -e "\n${YELLOW}Example executables:${NC}"
    find . -type f -executable -name "Examples_*" 2>/dev/null || true
fi

if [ "$BUILD_TESTS" = "ON" ]; then
    echo -e "\n${YELLOW}Test executables:${NC}"
    find . -type f -executable -name "test_*" 2>/dev/null || true
fi

echo -e "\n${GREEN}To install, run:${NC} cd $BUILD_DIR && make install"
