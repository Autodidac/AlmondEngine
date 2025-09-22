cmake_minimum_required(VERSION 3.10...4.0)

cmake_policy(SET CMP0000 NEW)

project(updater LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(updater src/main.cpp)
target_include_directories(updater PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    set(CMAKE_GENERATOR "Visual Studio 17 2022" CACHE STRING "Generator" FORCE)
endif()
