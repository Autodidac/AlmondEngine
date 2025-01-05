AlmondShell [![wakatime](https://wakatime.com/badge/user/17eacdf5-0954-4233-89e5-cf09a2e230eb/project/d3d1992b-0b79-49fc-b226-34f025ec1ad9.svg)](https://wakatime.com/badge/user/17eacdf5-0954-4233-89e5-cf09a2e230eb/project/d3d1992b-0b79-49fc-b226-34f025ec1ad9) 
+ 
AlmondEngine 
 [![wakatime](https://wakatime.com/badge/user/17eacdf5-0954-4233-89e5-cf09a2e230eb/project/69be8a22-0968-4157-a25c-ba228e92d6de.svg)](https://wakatime.com/badge/user/17eacdf5-0954-4233-89e5-cf09a2e230eb/project/69be8a22-0968-4157-a25c-ba228e92d6de)

<img align="left" src="images/AlmondEngine.jpg" width="100px"/>



# Almond Engine - x64 Cross-platform C++20 - Complete 4D Rendering Engine - Focused on Flexability, Future Proofing, and Maximum Performance.




Almond Engine is a contemporary cross-platform library designed for creating and managing Games across multiple platforms. It abstracts platform-specific Game Management code into a unified interface, allowing you to create and manipulate AlmondEngine in a platform-agnostic way. 


<img align="left" src="images/gpt.jpg" width="40px"/>GPT-4





## Features

- Modern Hybrid Design: Combines header-only functional programming with targeted object-oriented principles using C++20 features like DOD, RAII, and concepts.
- Cross-Platform: Supports Windows, macOS, Linux, Android, iOS, WebAssembly, PS5, Xbox, and No-op/Console mode for non-rendering builds.
- Namespace Isolation: Encapsulated within namespace "almond" to prevent global code pollution.
- Custom Memory Management: Efficient memory allocators tailored for game-specific workloads.
- Modular Rendering System: Supports OpenGL, Vulkan, DirectX, and texture pooling for optimal batching and resource usage.
- Split-Screen & VR: Native support for local multiplayer and VR integration. (planned)
- Asset Efficiency: Implements texture pools, texture atlasing, and sprite banks for high-performance asset management.
- GUI Framework: Custom UI system with elements like buttons, sliders, and text links, integrated with the rendering pipeline.
- Binary Save System: Includes time-traversal for seamless gameplay replay.
- Future-Proof: Designed with scalability, multi-threading, and async loading for modern game development.




## Tested

- Tested in Windows 10/11, and Linux Kubuntu (Ubuntu Debian/KDE)

<img align="left" src="images/1.png" width="200px"/>

<img align="left" src="images/4.png" width="200px"/>

```cpp



## Supported Platforms

- **Console** (Headless mode)
- **Windows** (Win32)
- **Linux** (XCB or XLib) (XCB planned, XLib Working!)

- **macOS** (Cocoa) (planned)
- **Android** (Android) (planned)
- **iOS** (UIKit) (planned)
- **WebAssembly** (Emscripten) (planned)
- **PlayStation 5** (PS5) (planned)
- **Xbox** (Xbox) (planned)

```

## Installation



### Prerequisites

- vcpkg (Windows and Linux)
- MSVC with C++20 or later, or VSCode, VSCodium on XLib
- CMake 3.10 or later




### Building

1. Clone the repository:
```sh
   git clone https://github.com/Autodidac/AlmondEngine.git
   cd AlmondEngine
```




### Build using WIN32 (Windows 11 64bit) and vs2022:
Simply run the provided vs solution file `AlmondEngine.sln` in the main folder

### Build using WIN32 (Windows 11 64bit) and VSCode:
Simply open the top project folder containing `AlmondEngine.sln` in VSCode or VSCodium on Linux


## CMake works through VSCodium on Linux, CMake is not used directly but should work.

### Generate build files using CMake:
Run cmake to generate the build files, then compile:





### Build the project:
Basic Build:
```sh
mkdir build
cd build
cmake ..
cmake --build .
```


### Usage


##### Example Console Batch
```batch
@echo off
Example_ConsoleApplication.exe 
pause
```
##### Example Console Bash
```bash
./Example_CMake_Project1 
```



##### Example - This is outdated until this part is finalized
```cpp
#include "AlmondEngine.h"

int main() {
    auto AlmondEngine = almondengine::renderer::createAlmondEngine();
    if (AlmondEngine->init()) {
        AlmondEngine->createWindow(L"Example Game", 800, 600);
        // Main loop or additional logic here
        AlmondEngine->shutdown();
    }
    return 0;
}
```


### API Reference

```cpp
almondengine::AlmondEngine
static std::unique_ptr<AlmondEngine> createAlmondEngine()
bool init()
bool createWindow(const wchar_t* title, int width, int height)
void shutdown()

```



### Platform-specific Implementations
The platform-specific implementation files are organized into separate source files. Each implementation file contains the initialization, window creation, and shutdown code for a specific platform.

-Windows: `aleEntryPoint_Win32.cpp` and `aleEntryPoint_Win32.h`

-macOS: `aleEntryPoint_Mac.cpp` and `aleEntryPoint_Mac.h`

-Linux: `aleEntryPoint_Linux.cpp` and `aleEntryPoint_Linux.h`

-Android: `aleEntryPoint_Android.cpp` and `aleEntryPoint_Android.h`

-iOS: `aleEntryPoint_IOS.cpp` and `aleEntryPoint_IOS.h`

-WebAssembly: `aleEntryPoint_WebAssembly.cpp` and `aleEntryPoint_WebAssembly.h`

-PlayStation 5: `aleEntryPoint_PS5.cpp` and `aleEntryPoint_PS5.h`

-Xbox: `aleEntryPoint_Xbox.cpp` and `aleEntryPoint_Xbox.h`

-No-op mode: `aleEntryPoint_Console.cpp` and `aleEntryPoint_Console.h`





### License
Almond Engine is licensed under the MIT License. See the LICENSE file for details.

If you're not aware of the MIT License get familiar with it.



### Contributing
Contributions are welcome! Please see the CONTRIBUTING.md file for guidelines.




## Acknowledgements
This project was developed with the support of OpenAI's GPT-4, which provided valuable assistance in generating code and design ideas. 

I appreciate the power and capabilities of GPT-4 in aiding the creation of this project and others.



<img align="left" src="images/AlmondEngine.jpg" width="80px"/>
<img align="center" src="images/gpt4all.png" width="80px"/>

Almond Engine - A GPT-4 Cross-platform Library For All! Almost Entirely Built and Designed by AI - Even much of this readme and the CMakeLists.txt are written by GPT-4.. what a wonderful tool Thank You Again!

