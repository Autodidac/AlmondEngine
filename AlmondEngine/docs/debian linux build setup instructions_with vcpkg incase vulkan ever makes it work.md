 
Here’s a clean setup for getting **vcpkg** working on **Linux with MSVC** through **WSL** (Windows Subsystem for Linux):

---

### ✅ **Step 1: Install Required Dependencies**

Run the following to install all needed packages:
```bash
sudo apt-get update && sudo apt-get install -y curl zip unzip tar cmake ninja-build git build-essential
```

---

### ✅ **Step 2: Clone and Bootstrap vcpkg**

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
```

> This will compile `vcpkg` and prepare it for package installation.

---

### ✅ **Step 3: Integrate vcpkg with CMake**

Optionally integrate with CMake globally:
```bash
./vcpkg integrate install
```

If you want to use **vcpkg** on a per-project basis, use:
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

---

### ✅ **Step 4: Setting up MSVC for Linux (via WSL)**

To use **MSVC inside WSL**, install the necessary cross-compilation tools:

1. On **Windows** (PowerShell):
   ```powershell
   wsl --install
   ```

2. On **WSL terminal** (Linux):
   ```bash
   sudo apt-get install build-essential clang lldb lld
   ```

3. Install **MSVC tools** via **Visual Studio Installer** (on Windows):
   - Ensure you have the **"MSVC v143 - VS 2022 C++ x64/x86 build tools"** and **"Linux development with C++"** workloads installed.

---

### ✅ **Step 5: Export Environment Variables for MSVC**

On **WSL**, export MSVC toolchain paths:
```bash
export CC=clang
export CXX=clang++
```

For using MSVC directly, you’ll need to set up the cross-compilation environment via **Visual Studio’s command prompt** and bridge it with WSL.

---

### ✅ **Step 6: Example: Install SDL2 and Vulkan with vcpkg**

```bash
./vcpkg install sdl3:x64-linux
./vcpkg install vulkan:x64-linux
```

---

### ✅ **Step 7: Using vcpkg in Your Project**

In your **CMakeLists.txt**:
```cmake
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
find_package(SDL3 CONFIG REQUIRED)
find_package(Vulkan CONFIG REQUIRED)
```

Then build:
```bash
cmake -B build -S .
cmake --build build
```

---

socat is a critical dependency for forwarding network connections (often used for debugging adapters, etc.). To install it on your Linux system, simply run:

```bash
sudo apt update
sudo apt install socat
```
Once installed, you can verify by running:

```bash
socat -V
```
This should display the version of socat, confirming its installation.


🔔 **Done!**
This setup gets **vcpkg** running with **Linux + MSVC via WSL**, along with **SDL3** and **Vulkan** integration. 😈
