#!/bin/bash
# Usage: ./build.sh [gcc|clang] [Debug|Release]

# Set the installation prefix (this remains the same across build and install)
INSTALL_PREFIX="${PWD}/built"

# Select compiler and assign a friendly name for the build folder
if [ "$1" == "gcc" ]; then
  COMPILER_C="gcc"
  COMPILER_CXX="g++"
  COMPILER_NAME="GCC"
elif [ "$1" == "clang" ]; then
  COMPILER_C="clang"
  COMPILER_CXX="clang++"
  COMPILER_NAME="Clang"
else
  echo "Usage: $0 [gcc|clang] [Debug|Release]"
  exit 1
fi

# Set build type (default to Debug if not provided)
if [ -z "$2" ]; then
  BUILD_TYPE="Debug"
else
  BUILD_TYPE="$2"
fi

# Define the build directory including both compiler and build type
BUILD_DIR="Bin/${COMPILER_NAME}-${BUILD_TYPE}"

# Create the build and install directories if they don't exist
mkdir -p "$BUILD_DIR"
mkdir -p "$INSTALL_PREFIX"

# Build the project (verbose output)
cmake --build "$BUILD_DIR" --verbose
