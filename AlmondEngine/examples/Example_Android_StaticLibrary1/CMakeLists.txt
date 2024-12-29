# Add executable
add_executable(Example_Android_StaticLibrary1 Example_Android_StaticLibrary1.cpp)

# Link Engine library
target_link_libraries(Example_Android_StaticLibrary1 PRIVATE AlmondEngine)

# Include directories for Example_Android_StaticLibrary1
target_include_directories(Example_Android_StaticLibrary1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Example_Android_StaticLibrary1 PRIVATE -Wall -Wextra -Wpedantic)
        # Allow undefined symbols (main will be provided by AlmondEngine)
        target_link_options(Example_Android_StaticLibrary1 PRIVATE -Wl,--allow-shlib-undefined)
endif()
