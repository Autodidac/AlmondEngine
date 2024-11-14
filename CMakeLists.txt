cmake_minimum_required(VERSION 3.20)
project(Example_CMake_AlmondCore)

# Include shared items project
add_subdirectory(shared_items)

# Include static library project
add_subdirectory(static_lib)

# Include DLL project
add_subdirectory(dll_lib)

# Include Windows/console example project
add_subdirectory(example)
