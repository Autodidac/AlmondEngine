cmake_minimum_required(VERSION 3.21)
project(AlmondEngine VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add Engine library
add_subdirectory(AlmondEngine)
# Add all example projects in Examples directory
add_subdirectory(AlmondEngine/examples)
