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
 
#include "aupdateconfig.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>

namespace almondnamespace
{
    namespace updater
    {
        // 🌐 **Download a File (Cross-Platform)**
        inline bool download_file(const std::string& url, const std::string& output_path) {
            std::cout << "[INFO] Downloading: " << url << " -> " << output_path << "\n";

#if defined(_WIN32)
            std::string command = "curl -L --fail --silent --show-error -o \"" + output_path + "\" \"" + url + "\"";
#elif defined(__linux__) || defined(__APPLE__)
            std::string command = "wget --quiet --show-progress --output-document=" + output_path + " " + url;
#endif

            int result = std::system(command.c_str());

            // ✅ If `curl` failed, remove the invalid file
            std::ifstream file_check(output_path, std::ios::binary | std::ios::ate);
            if (result != 0 || !file_check || file_check.tellg() <= 1) {  // File doesn't exist or is too small
                std::cerr << "[ERROR] Download failed or file is empty. Deleting: " << output_path << "\n";
                std::remove(output_path.c_str());  // ✅ Delete invalid file
                return false;
            }

            return true;
        }

        // 🛠 **Check if `7z` is Installed**
        inline bool is_7z_available() {
#if defined(_WIN32)
            std::ifstream file(SEVEN_ZIP_LOCAL_BINARY());
            return file.good(); // ✅ Checks if file exists without <filesystem>
#else
            return std::system("which 7z > /dev/null 2>&1") == 0;
#endif
        }

        // 🛠 **Ensure 7-Zip is Installed**
        inline bool setup_7zip() {
            std::cout << "[INFO] Checking for 7-Zip installation...\n";

            if (is_7z_available()) {
                std::cout << "[INFO] 7-Zip is already installed.\n";
                return true;
            }

            std::cout << "[INFO] 7-Zip not found! Installing latest version...\n";

#if defined(_WIN32)
            constexpr std::string_view installer = "7z_installer.exe";

            if (!download_file(SEVEN_ZIP_EXE_URL(), std::string(installer))) {
                std::cerr << "[ERROR] Failed to download 7-Zip installer!\n";
                return false;
            }

            std::cout << "[INFO] Installing 7-Zip...\n";

            // ✅ Run the installer directly using `cmd.exe /C start /wait`
            std::string install_command = "cmd.exe /C start /wait 7z_installer.exe /S /D=\"C:\\Program Files\\7-Zip\"";
            int install_result = std::system(install_command.c_str());

            // ✅ Wait for installation to complete without blocking
            std::string seven_zip_path = "C:\\Program Files\\7-Zip\\7z.exe";
            int wait_time = 0;
            while (!std::ifstream(seven_zip_path)) {
                std::cout << "[INFO] Waiting for 7-Zip installation to complete... (" << wait_time << "s)\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                wait_time++;

                // Prevent infinite loop (give it up to 60 seconds to install)
                if (wait_time > 60) {
                    std::cerr << "[ERROR] 7-Zip installation timed out!\n";
                    return false;
                }
            }

            std::cout << "[INFO] 7-Zip installed successfully!\n";

#else  // Linux/macOS
            std::cout << "[INFO] Installing p7zip (Linux/macOS)...\n";
            constexpr std::string_view install_cmd =
                "sudo apt install -y p7zip-full || sudo pacman -S --noconfirm p7zip || brew install p7zip";

            int install_result = std::system(install_cmd.data());

            if (install_result != 0 || !is_7z_available()) {
                std::cerr << "[ERROR] Failed to install p7zip. Please install it manually.\n";
                return false;
            }
            std::cout << "[INFO] p7zip installed successfully!\n";
#endif

            return true;
        }


        // 📦 **Extract an Archive using 7-Zip (or Fallback)**
        inline bool extract_archive(const std::string& archive_file, const std::string& destination = ".") {
            std::cout << "[INFO] Extracting " << archive_file << "...\n";

            if (!setup_7zip()) {
                std::cerr << "[ERROR] 7-Zip installation failed. Falling back to system extraction.\n";
#if defined(_WIN32)
                return system(("powershell -Command \"Expand-Archive -Path '" + archive_file + "' -DestinationPath '" + destination + "' -Force\"").c_str()) == 0;
#elif defined(__linux__) || defined(__APPLE__)
                return system(("tar -xf '" + archive_file + "' -C '" + destination + "'").c_str()) == 0;
#endif
            }

            //std::string extract_command = "7z.exe x \"source_code.zip\" -o\".\" -y";
            //int result = std::system(extract_command.c_str());
            //if (result != 0) {
            //    std::cerr << "[ERROR] Failed to extract source archive.\n";
            //}

            std::string extract_command = "cmd.exe /C \"\"" + SEVEN_ZIP_LOCAL_BINARY() + "\" x \"" + archive_file + "\" -o\"" + destination + "\" -y\"";

            //std::string extract_command = "\"" + config::SEVEN_ZIP_LOCAL_BINARY + "\" x \"" + archive_file + "\" -o\"" + destination + "\" -y";
            return system(extract_command.c_str()) == 0;
        }

        // ⚙ **Setup LLVM + Clang**
        inline bool setup_llvm_clang() {
            std::cout << "[INFO] Checking for existing Clang installation...\n";
            std::string clang_binary = LLVM_BIN_PATH() + "/clang++";
#if defined(_WIN32)
            clang_binary += ".exe";
#endif

            if (std::ifstream(clang_binary)) {
                std::cout << "[INFO] Clang is already installed.\n";
                return true;
            }

            // Ensure MSVC is installed (since we're building from source)
#if defined(_WIN32)
            if (system("\"C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 >nul 2>&1") != 0) {
                std::cerr << "[ERROR] MSVC is required to build LLVM from source, but it's missing or not properly set up!\n";
                return false;
            }
#endif

            // Ensure 7-Zip is installed before proceeding
            if (!setup_7zip()) {
                std::cerr << "[ERROR] 7-Zip installation failed. Cannot proceed with LLVM setup.\n";
                return false;
            }

            std::cout << "[INFO] Downloading LLVM + Clang source...\n";
            std::string llvm_zip = "llvm.zip";
            std::string temp_tools_dir = "updaters_xtemp/"; // a very unique name to avoid any collisions with user made directories in end-user-projects

            if (!download_file(LLVM_SOURCE_URL(), llvm_zip)) {
                std::cerr << "[ERROR] Failed to download LLVM + Clang!\n";
                return false;
            }

            // ✅ First Attempt: Extract using 7-Zip
            std::string extract_command = "\"\"" + SEVEN_ZIP_LOCAL_BINARY() + "\" x \"" + llvm_zip + "\" -o\"" + temp_tools_dir + "\" -y\"";
            if (system(extract_command.c_str()) != 0) {
                std::cerr << "[WARNING] 7-Zip extraction failed! Falling back to (very slow) PowerShell, you may want to restart and try again...\n";

                // ✅ Fallback: Use PowerShell if 7-Zip fails
#if defined(_WIN32)
                extract_command = "powershell -Command \"Expand-Archive -Path '" + llvm_zip + "' -DestinationPath '" + temp_tools_dir + "' -Force\"";
#elif defined(__linux__) || defined(__APPLE__)
                extract_command = "tar -xf " + llvm_zip + " -C " + temp_tools_dir;
#endif
                if (system(extract_command.c_str()) != 0) {
                    std::cerr << "[ERROR] Failed to extract LLVM + Clang! Archive might be corrupt.\n";
                    return false;
                }
            }

            std::cout << "[INFO] Cleaning up LLVM installation files...\n";
            std::remove(llvm_zip.c_str());

            std::cout << "[INFO] Building LLVM + Clang from source...\n";
#if defined(_WIN32)
            std::string build_command = "cd llvm && cmake -G \"Visual Studio 17 2022\" -A x64 . && cmake --build . --config Release";
#else
            std::string build_command = "cd llvm && cmake -G Ninja . && ninja";
#endif

            if (system(build_command.c_str()) != 0) {
                std::cerr << "[ERROR] LLVM + Clang compilation failed! Falling back to precompiled binary...\n";

                // ✅ Download precompiled binary as a fallback
                if (!download_file(LLVM_EXE_URL(), "llvm_installer.exe")) {
                    std::cerr << "[ERROR] Failed to download precompiled LLVM binary!\n";
                    return false;
                }

                std::cout << "[INFO] Installing LLVM binary...\n";
#if defined(_WIN32)
                std::string install_command = "llvm_installer.exe /S";
#elif defined(__linux__)
                std::string install_command = "sudo apt install -y clang";
#elif defined(__APPLE__)
                std::string install_command = "brew install llvm";
#endif

                if (system(install_command.c_str()) != 0) {
                    std::cerr << "[ERROR] Failed to install precompiled LLVM binary!\n";
                    return false;
                }
            }

            std::cout << "[INFO] LLVM + Clang installed successfully!\n";
            return true;
        }




        // 🏗 **Setup Ninja Build System**
        inline bool setup_ninja() {
            std::cout << "[INFO] Checking for existing Ninja installation...\n";
            std::string ninja_binary = "ninja";
#if defined(_WIN32)
            ninja_binary += ".exe";
#endif

            if (std::ifstream(ninja_binary)) {
                std::cout << "[INFO] Ninja is already installed.\n";
                return true;
            }

            std::cout << "[INFO] Downloading Ninja Build System...\n";
            return download_file(NINJA_ZIP_URL(), "ninja.zip") && extract_archive("ninja.zip");
        }

        // 🛠 **Generate Ninja Build File**
        inline bool generate_ninja_build_file()
        {
            std::ofstream batchFile("build_with_msvc.bat"); // Create a batch script that sets up MSVC and runs Ninja
            if (!batchFile) {
                std::cerr << "ERROR: Failed to create batch file for MSVC setup.\n";
                return false;
            }

            batchFile << "@echo off\n"
                << "call \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat\"\n"
                << "ninja\n";
            batchFile.close();

            std::ofstream buildFile("build.ninja");
            if (!buildFile) {
                std::cerr << "[ERROR] Failed to create build.ninja file!\n";
                return false;
            }

            std::string llvm_bin = LLVM_BIN_PATH();

#if defined(_WIN32)


            buildFile << "rule cxx\n"
                << " command = " << llvm_bin << "/clang++ -I include -o" << OUTPUT_BINARY()
                << " -std=c++20 -fuse-ld=lld -D_WIN32 " << SOURCE_MAIN_FILE() << "\n"
                << "  description = Building updater...\n"
                << "build " << OUTPUT_BINARY() << ": cxx " << SOURCE_MAIN_FILE() << "\n";
#else
            std::string compiler = "g++";
            std::string include_flag = "-I include"; // Add include path for Linux/macOS
            std::string output_flag = "-o updater_new";

            buildFile << "rule cxx\n"
                << "  command = " << compiler << " " << include_flag << " " << output_flag << " -std=c++20 src/main.cpp\n"
                << "  description = Building updater...\n"
                << "\n"
                << "build updater_new: cxx src/main.cpp\n";
#endif


            buildFile.close();
            std::cout << "[INFO] Ninja build file generated successfully!\n";
            return true;
        }

        // 🔄 **Cleanup Old Files**
        inline void clean_up_build_files()
        {
            std::cout << "[INFO] Cleaning up useless temp files...\n";
#if defined(_WIN32)
#ifdef LEAVE_NO_FILES_ALWAYS_REDOWNLOAD
            system("del /F /Q build.ninja build_with_msvc.bat llvm.zip llvm_installer.exe source_code.zip 7z_installer.exe ninja.exe >nul 2>&1");
            system("rmdir /s /q Cpp_Ultimate_Project_Updater-main >nul 2>&1");
#else
            system("del /F /Q build.ninja build_with_msvc.bat llvm.zip llvm_installer.exe source_code.zip 7z_installer.exe >nul 2>&1");
            system("rmdir /s /q Cpp_Ultimate_Project_Updater-main >nul 2>&1");
#endif
#else
#ifdef LEAVE_NO_FILES_ALWAYS_REDOWNLOAD
            system("rm -rf build.ninja llvm.zip llvm_installer source_code.zip 7z_installer Cpp_Ultimate_Project_Updater-main ninja");
#else
            system("rm -rf build.ninja llvm.zip llvm_installer source_code.zip 7z_installer Cpp_Ultimate_Project_Updater-main");
#endif
#endif
        }
    }
}