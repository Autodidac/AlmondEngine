#!/bin/bash
# Usage: ./install.sh [gcc|clang] [Debug|Release]

# Determine compiler and assign a friendly name for the install folder
if [ "$1" == "gcc" ]; then
  COMPILER_NAME="GCC"
elif [ "$1" == "clang" ]; then
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

# Build directory matching the naming convention from the build script
BUILD_DIR="Bin/${COMPILER_NAME}-${BUILD_TYPE}"

# Define the install prefix within built/bin, creating a subfolder for each configuration
INSTALL_PREFIX="${PWD}/built/bin/${COMPILER_NAME}-${BUILD_TYPE}"
mkdir -p "$INSTALL_PREFIX"

# For multi-config generators, specify the configuration if provided, and override the install prefix
cmake --install "$BUILD_DIR" --config "$BUILD_TYPE" --prefix "$INSTALL_PREFIX"
