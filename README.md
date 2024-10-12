<img align="left" src="image/AlmondEngine.jpg" width="100px"/>



# Almond Engine - Cross-platform C++20 No-Dependency Engine - Focused on Freedom and Maximum Performance!




AlmondEngine is a contemporary cross-platform library designed for creating and managing Games across multiple platforms. It abstracts platform-specific Game Management code into a unified interface, allowing you to create and manipulate AlmondEngine in a platform-agnostic way. 


<img align="left" src="image/gpt.jpg" width="40px"/>GPT-4



## Features


- Cross-platform support for Windows, macOS, Linux, Android, iOS, WebAssembly, PS5, Xbox, and a No-op/Console mode.
- Game Creation and Management.
- Platform-specific implementation details hidden behind a common interface.
- Namespace 'almondengine' Prevents Global Code Leaks
- Modern C++20 and Efficient Pattern Design
- Platform-Specific Factories
- Custom Efficient Memory Management and Logging, and Error Handling
- Split-Screen and VR Features
In AlmondEngine, Ensure that these features are appropriately implemented in the platform-specific classes where applicable.



## Tested

- Tested in Windows 10/11 With Console and WindowsDesktopApplication

<img align="left" src="image/1.png" width="200px"/>

<img align="left" src="image/4.png" width="200px"/>

```cpp



## Supported Platforms

- **Windows** (Win32)
- **macOS** (Cocoa)
- **Linux** (XCB or XLib)
- **Android**
- **iOS** (UIKit)
- **WebAssembly** (Emscripten)
- **PlayStation 5** (PS5)
- **Xbox**
- **Console** (Headless mode)
```



## Installation



### Prerequisites

- C++20 or later
- CMake 3.10 or later



### Building

1. Clone the repository:
```sh
   git clone https://github.com/Autodidac/AlmondEngine.git
   cd AlmondEngine
```





### Build using WIN32 (Windows 11 64bit) and vs2022:
Simply run the provided vs solution file `AlmondEngine.sln` in the main folder




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
AlmondEngine_Example.exe 
pause
```


##### Example
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

-Windows: `AlmondEngineWin32.cpp` and `AlmondEngineWin32.h`

-macOS: `AlmondEngineMac.cpp` and `AlmondEngineMac.h`

-Linux: `AlmondEngineLinux.cpp` and `AlmondEngineLinux.h`

-Android: `AlmondEngineAndroid.cpp` and `AlmondEngineAndroid.h`

-iOS: `AlmondEngineIOS.cpp` and `AlmondEngineIOS.h`

-WebAssembly: `AlmondEngineWebAssembly.cpp` and `AlmondEngineWebAssembly.h`

-PlayStation 5: `AlmondEnginePS5.cpp` and `AlmondEnginePS5.h`

-Xbox: `AlmondEngineXbox.cpp` and `AlmondEngineXbox.h`

-No-op mode: `AlmondEngineConsole.cpp` and `AlmondEngineConsole.h`





### License
Almond Engine is licensed under the Apache-2.0 License. See the LICENSE file for details.

If you're not aware of the Apache-2.0 License get familiar with it.



### Contributing
Contributions are welcome! Please see the CONTRIBUTING.md file for guidelines.




## Acknowledgements
This project was developed with the support of OpenAI's GPT-4, which provided valuable assistance in generating code and design ideas. 

I appreciate the power and capabilities of GPT-4 in aiding the creation of this project and others.



<img align="left" src="image/AlmondEngine.jpg" width="80px"/>
<img align="center" src="image/gpt4all.png" width="80px"/>

Almond Engine - A GPT-4 Cross-platform Library For All! Almost Entirely Built and Designed by AI - Even much of this readme and the CMakeLists.txt are written by GPT-4.. what a wonderful tool Thank You Again!

