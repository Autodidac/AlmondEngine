/**************************************************************
 *   █████╗ ██╗     ███╗   ███╗   ███╗   ██╗    ██╗██████╗    *
 *  ██╔══██╗██║     ████╗ ████║ ██╔═══██╗████╗  ██║██╔══██╗   *
 *  ███████║██║     ██╔████╔██║ ██║   ██║██╔██╗ ██║██║  ██║   *
 *  ██╔══██║██║     ██║╚██╔╝██║ ██║   ██║██║╚██╗██║██║  ██║   *
 *  ██║  ██║███████╗██║ ╚═╝ ██║ ╚██████╔╝██║ ╚████║██████╔╝   *
 *  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝    *
 *                                                            *
 *   This file is part of the Almond Project.                 *
 *   AlmondEngine - Modular C++ Game Engine                   *
 *                                                            *
 *   SPDX-License-Identifier: LicenseRef-MIT-NoSell           *
 *                                                            *
 *   Provided "AS IS", without warranty of any kind.          *
 *   Use permitted for non-commercial purposes only           *
 *   without prior commercial licensing agreement.            *
 *                                                            *
 *   Redistribution allowed with this notice.                 *
 *   No obligation to disclose modifications.                 *
 *   See LICENSE file for full terms.                         *
 **************************************************************/
#pragma once

#include <string>

namespace almondnamespace
{
    namespace updater 
    {
        // ⚙️ **Installation Method**
#define LEAVE_NO_FILES_ALWAYS_REDOWNLOAD

// 🌍 **Individual Project Information**
        inline std::string OWNER = "Autodidac";
        inline std::string REPO = "Cpp20_Ultimate_Project_Updater";
        inline std::string BRANCH = "main";
        inline std::string PROJECT_VERSION = "0.0.1"; // Change to match the current version

        // 🔨 **Build Settings**
        // NOT ACTUALLY CONFIGURABLE this gets renamed at the end 
        inline std::string OUTPUT_BINARY() {
#if defined(_WIN32)
            return "updater_new.exe";  // ✅ Ensure Windows builds use `.exe`
#else
            return "updater_new";
#endif
        }

        // Internals
        inline std::string SOURCE_MAIN_FILE() { return "src/update.cpp"; }  // 📂 Dynamic Main Source File
        inline std::string REPLACE_RUNNING_EXE_SCRIPT_NAME() { return "replace_updater.bat"; }

        // 🌐 **GitHub Base URLs**
        inline std::string GITHUB_BASE() { return "https://github.com/"; }
        inline std::string GITHUB_RAW_BASE() { return "https://raw.githubusercontent.com/"; }

        // 🔧 **Project URLs**
        inline std::string PROJECT_VERSION_URL() { return GITHUB_RAW_BASE() + OWNER + "/" + REPO + "/" + BRANCH + "/include/config.hpp"; }
        inline std::string PROJECT_SOURCE_URL() { return GITHUB_BASE() + OWNER + "/" + REPO + "/archive/refs/heads/" + BRANCH + ".zip"; }
        inline std::string PROJECT_BINARY_URL() { return GITHUB_BASE() + OWNER + "/" + REPO + "/releases/latest/download/update.exe"; }

        // 🔢 **LLVM Configuration**
        inline std::string LLVM_VERSION = "20.1.0"; // Change to match the desired LLVM version

        // 🔧 **LLVM & Ninja URLs (Auto-Generated)**
#if defined(_WIN32)
        inline std::string LLVM_SOURCE_URL() { return GITHUB_BASE() + "llvm/llvm-project/archive/refs/tags/llvmorg-" + LLVM_VERSION + ".zip"; }
        inline std::string LLVM_EXE_URL() { return GITHUB_BASE() + "llvm/llvm-project/releases/download/llvmorg-" + LLVM_VERSION + "/LLVM-" + LLVM_VERSION + "-win64.exe"; } //github.com/llvm/llvm-project/releases/download/llvmorg-20.1.0/LLVM-20.1.0-win64.exe
        inline std::string NINJA_ZIP_URL() { return GITHUB_BASE() + "ninja-build/ninja/releases/latest/download/ninja-win.zip"; }
        inline std::string NINJA_EXE_URL() { return GITHUB_BASE() + "ninja-build/ninja/releases/latest/download/ninja.exe"; }
        inline std::string LLVM_BIN_PATH() { return "C:/Program Files/LLVM/bin"; }  // 🛠 Dynamic LLVM Bin Path
#elif defined(__linux__)
        inline std::string LLVM_SOURCE_URL() { return GITHUB_BASE() + "llvm/llvm-project/archive/refs/tags/llvmorg-" + LLVM_VERSION + "/LLVM-" + LLVM_VERSION + "-linux.tar.xz"; }
        inline std::string NINJA_ZIP_URL() { return GITHUB_BASE() + "ninja-build/ninja/releases/latest/download/ninja-linux.zip"; }
        inline std::string LLVM_BIN_PATH() { return "/usr/local/bin"; }  // 🛠 Dynamic LLVM Bin Path
#elif defined(__APPLE__)
        inline std::string LLVM_SOURCE_URL() { return GITHUB_BASE() + "llvm/llvm-project/archive/refs/tags/llvmorg-" + LLVM_VERSION + "/LLVM-" + LLVM_VERSION + "-macos.tar.xz"; }
        inline std::string NINJA_ZIP_URL() { return GITHUB_BASE() + "ninja-build/ninja/releases/latest/download/ninja-mac.zip"; }
        inline std::string LLVM_BIN_PATH() { return "/usr/local/bin"; }  // 🛠 Dynamic LLVM Bin Path
#endif

        // 🛠 **7-Zip Configuration (GitHub Repository - EXE, not MSI)**
        inline std::string SEVEN_ZIP_VERSION = "24.09";  // Use latest stable release
        inline std::string SEVEN_ZIP_VERSION_NAMETAG = "2409";  // Use latest stable release
#if defined(_WIN32)
        inline std::string SEVEN_ZIP_SOURCE_URL() { return GITHUB_BASE() + "ip7z/7zip/archive/refs/tags/" + SEVEN_ZIP_VERSION + ".zip"; }
        inline std::string SEVEN_ZIP_EXE_URL() { return GITHUB_BASE() + "ip7z/7zip/releases/latest/download/" + "/7z" + SEVEN_ZIP_VERSION_NAMETAG + "-x64.exe"; }
        inline std::string SEVEN_ZIP_LOCAL_BINARY() { return "C:\\Program Files\\7-Zip\\7z.exe"; }
        inline std::string SEVEN_ZIP_BINARY = "7z.exe";
        //inline std::string SEVEN_ZIP_INSTALL_CMD() { return "7z_installer.exe /S"; } // this was for the msi
#elif defined(__linux__)
        inline std::string SEVEN_ZIP_SOURCE_URL{ return GITHUB_BASE() + "ip7z/7zip/archive/refs/tags/" + SEVEN_ZIP_VERSION + "/7z" + SEVEN_ZIP_VERSION_NAMETAG + "-linux-x64.tar.xz"; } //7z2409-linux-x64.tar.xz
        inline std::string SEVEN_ZIP_BINARY = "7z";
        inline std::string SEVEN_ZIP_INSTALL_CMD() { return "sudo apt-get install -y 7z2409-linux-x64"; }
#elif defined(__APPLE__)
        inline std::string SEVEN_ZIP_SOURCE_URL() { return GITHUB_BASE() + "ip7z/7zip/releases/latest/download/" + SEVEN_ZIP_VERSION + "/p7zip-mac.tar.gz"; } //releases/download/24.09/7z2409-mac.tar.xz
        inline std::string SEVEN_ZIP_BINARY = "7z";
        inline std::string SEVEN_ZIP_INSTALL_CMD() { return "brew install p7zip"; }
#endif

    } // namespace config
}