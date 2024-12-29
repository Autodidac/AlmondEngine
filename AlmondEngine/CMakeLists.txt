# Add engine library
add_library(AlmondEngine STATIC)

# Include directories for public and private use
target_include_directories(AlmondEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE
        # Use VCPKG installation directory
        $<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/debug/include,${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/include>
)

# Source files for the engine
target_sources(AlmondEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AlmondEngine.cpp  # Replace with actual sources
)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
# Link Glad with GLFW
target_link_libraries(AlmondEngine PRIVATE glad::glad glfw)

#find_package(SDL3 CONFIG REQUIRED)
# Include SDL3 directories
#target_include_directories(AlmondEngine PRIVATE ${SDL3_INCLUDE_DIRS})
# Link SDL3 with AlmondEngine
#target_link_libraries(AlmondEngine PRIVATE SDL3::SDL3)

# Define AlmondEngine_STATICLIB for static builds
#target_compile_definitions(AlmondEngine PUBLIC AlmondEngine_STATICLIB)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Add warnings and best practices
    target_compile_options(AlmondEngine PRIVATE -Wall -Wextra -Wpedantic -std=c++20)
elseif(MSVC)
    # Specific warnings for MSVC
    target_compile_options(AlmondEngine PRIVATE /W4 /std:c++20)
endif()
