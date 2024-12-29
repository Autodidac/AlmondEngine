# Add executable
add_executable(Example_CMake_Project1
    src/main.cpp
    # other source files
)

target_sources(AlmondEngine PRIVATE
    ../../src/alsEntryPoint.cpp 
    ../../src/alsEngine.cpp 
    ../../src/alsContextFunctions.cpp
    ../../src/alsGLFWContext.cpp
    ../../src/alsSDL3Context.cpp
    ../../src/alsImageLoader.cpp
    ../../src/alsMesh.cpp
    )

# If you're targeting X11, make sure that the X11-specific source is added conditionally
if(UNIX AND NOT APPLE)  # Assuming this targets Linux/X11
    target_sources(AlmondEngine PRIVATE ../../src/alsEntryPoint_X11.cpp)
endif()

# Link Engine library
target_link_libraries(Example_CMake_Project1 PRIVATE AlmondEngine)

# Include directories for Example_CMake_Project1
target_include_directories(Example_CMake_Project1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Explicitly tell the linker to use the `main` symbol from AlmondEngine
  target_link_options(AlmondEngine PRIVATE -Wl,--allow-shlib-undefined)
  #  target_link_options(AlmondEngine PRIVATE -Wl,--undefined=main)
endif()

# Compiler-specific options
#if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  #  target_compile_options(Example_CMake_Project1 PRIVATE -Wall -Wextra -Wpedantic -Wl,-e,main)
        # Tell the linker to allow undefined symbols (like main) when creating the executable
        #target_link_options(Example_CMake_Project1 PRIVATE )
       # target_link_options(AlmondEngine PRIVATE -Wl,-e,main)
        # Executable CMakeLists.txt
#target_link_options(Example_CMake_Project1 PRIVATE -Wl,-e,main)

#endif()
