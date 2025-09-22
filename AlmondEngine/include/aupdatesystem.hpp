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

#include "abuildsystem.hpp"
#include "aupdateconfig.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

namespace almondnamespace
{
    namespace updater
    {
        // 🔄 **Replace binary with the new compiled version**
        inline void replace_binary_from_script(const std::string& new_binary) {
            std::cout << "[INFO] Replacing current binary...\n";

#if defined(_WIN32)
            //clean_up_build_files();

            std::ofstream batchFile(REPLACE_RUNNING_EXE_SCRIPT_NAME());
            if (!batchFile) {
                std::cerr << "[ERROR] Failed to create batch file for self-replacement.\n";
                return;
            }

            batchFile << "@echo off\n"
                << "timeout /t 2 >nul\n"
                << "taskkill /IM updater.exe /F >nul 2>&1\n"
                << "timeout /t 1 >nul\n"
                << "del updater.exe >nul 2>&1\n"
                << "if exist updater.exe (\n"
                << "    echo [ERROR] File lock detected. Retrying...\n"
                << "    timeout /t 1 >nul\n"
                << "    goto retry\n"
                << ")\n"
                << "rename \"" << new_binary << "\" \"updater.exe\"\n"
                << "if exist updater.exe (\n"
                << "    echo [INFO] Update successful! Restarting updater...\n"
                << "    timeout / t 4 > nul\n"
                << "    start updater.exe\n"
                << "    exit\n"
                << ") else (\n"
                << "    echo [ERROR] Failed to rename new binary. Update aborted!\n"
                << "    exit\n"
                << ")\n";

            batchFile.close();

            std::cout << "[INFO] Running replacement script...\n";
            system(("start /min " + std::string(REPLACE_RUNNING_EXE_SCRIPT_NAME())).c_str());
            exit(0);

#elif defined(__linux__) || defined(__APPLE__)
            clean_up_build_files();

            std::ofstream scriptFile("replace_and_restart.sh");
            if (!scriptFile) {
                std::cerr << "[ERROR] Failed to create shell script for self-replacement.\n";
                return;
            }

            scriptFile << "#!/bin/sh\n"
                << "sleep 2\n"
                << "pkill -f updater\n"
                << "sleep 1\n"
                << "if pgrep -f updater; then\n"
                << "    echo \"[WARNING] Process still running, forcing kill...\"\n"
                << "    sleep 1\n"
                << "    pkill -9 -f updater\n"
                << "fi\n"
                << "if [ -f updater ]; then\n"
                << "    rm -f updater\n"
                << "fi\n"
                << "mv \"" << new_binary << "\" updater\n"
                << "chmod +x updater\n"
                << "echo \"[INFO] Update successful! Restarting updater...\"\n"
                << "./updater &\n"
                << "rm -- \"$0\"\n";

            scriptFile.close();

            std::cout << "[INFO] Running replacement script...\n";
            system("chmod +x replace_and_restart.sh && nohup ./replace_and_restart.sh &");
            exit(0);
#endif
        }

        // 🔄 **Replace the Current Binary**
        inline void replace_binary(const std::string& new_binary) {
            std::cout << "[INFO] Replacing binary...\n";
            clean_up_build_files();
#if defined(_WIN32)
            replace_binary_from_script(new_binary);
            //system(("move /Y " + new_binary + " updater.exe").c_str());
#elif defined(__linux__) || defined(__APPLE__)
            replace_binary_from_script(new_binary);
            // system(("mv " + new_binary + " updater").c_str());
#endif
            return;
        }

        // 🔍 **Check for Updates**
        bool check_for_updates(const std::string& remote_config_url) {
            std::cout << "[INFO] Checking for updates...\n";

            std::string temp_config_path = "temp_config.hpp";

            // ✅ Download `config.hpp` using `wget` or `curl`, but don't keep it permanently
#if defined(_WIN32)
            std::string command = "curl -L --fail --silent --show-error -o \"" + temp_config_path + "\" \"" + remote_config_url + "\"";
#else
            std::string command = "wget --quiet --output-document=" + temp_config_path + " " + remote_config_url;
#endif

            if (system(command.c_str()) != 0) {
                std::cerr << "[ERROR] Failed to download config.hpp!\n";
                return false;
            }

            // ✅ Read `config.hpp` from the temporary file
            std::ifstream config_file(temp_config_path);
            if (!config_file) {
                std::cerr << "[ERROR] Failed to open downloaded config.hpp for reading.\n";
                return false;
            }

            std::string line, latest_version;
            std::regex version_regex(R"(inline\s+std::string\s+PROJECT_VERSION\s*=\s*\"([0-9]+\.[0-9]+\.[0-9]+)\";)");

            while (std::getline(config_file, line)) {
                std::smatch match;
                if (std::regex_search(line, match, version_regex)) {
                    latest_version = match[1]; // Extract version number
                    break;
                }
            }

            config_file.close();

            // ✅ Delete temporary file immediately after reading
#if defined(_WIN32)
            system(("del /F /Q " + temp_config_path + " >nul 2>&1").c_str());
#else
            system(("rm -f " + temp_config_path).c_str());
#endif

            if (latest_version.empty()) {
                std::cerr << "[ERROR] Failed to extract version from config.hpp!\n";
                return false;
            }

            std::cout << "[INFO] Local Version: " << PROJECT_VERSION << "\n";
            std::cout << "[INFO] Remote Version: " << latest_version << "\n";

            // ✅ Compare remote version with local version
            return latest_version != PROJECT_VERSION;
        }

        // 🚀 **Install Updater from Binary**
        inline bool install_from_binary(const std::string& binary_url) {
            std::cout << "[INFO] Installing from precompiled binary...\n";

            const std::string output_binary = OUTPUT_BINARY();

            if (!download_file(binary_url, output_binary)) {
                std::cerr << "[ERROR] Failed to download precompiled binary!\n";
                return false;
            }
            replace_binary(output_binary);

            return true;
        }

        // 🚀 **Install Updater from Source**
        inline void install_from_source(const std::string& binary_url) {
            std::cout << "[INFO] Installing from source...\n";

            if (!setup_7zip()) {
                std::cerr << "[ERROR] 7-Zip installation failed. Cannot continue.\n";
                return;
            }

            if (!download_file(PROJECT_SOURCE_URL(), "source_code.zip") || !extract_archive("source_code.zip")) {
                std::cerr << "[ERROR] Failed to download or extract source archive.\n";
                return;
            }

            std::string extracted_folder = REPO + "-main";

            // ✅ Debug: Check if files exist before moving
            std::cout << "[DEBUG] Listing extracted files before moving:\n";
#if defined(_WIN32)
            system(("dir /s /b " + extracted_folder).c_str());
#else
            system(("ls -l " + extracted_folder).c_str());
#endif

#if defined(_WIN32)
            // ✅ Use `xcopy` instead of `robocopy`
            std::string move_command = "xcopy /E /Y /Q " + extracted_folder + "\\* .";
#else
            // ✅ Use `mv -T` to move the entire directory correctly
            std::string move_command = "mv -T " + extracted_folder + " ./";
#endif

            if (system(move_command.c_str()) != 0) {
                std::cerr << "[ERROR] Failed to move extracted files.\n";
                return;
            }

#if defined(_WIN32)
            std::string remove_command = "cmd.exe /C \"rmdir /s /q " + extracted_folder + "\"";
#else
            std::string remove_command = "rm -rf " + extracted_folder;
#endif

            if (system(remove_command.c_str()) != 0) {
                std::cerr << "[WARNING] Failed to delete extracted folder: " << extracted_folder << std::endl;
            }

            if (!setup_llvm_clang() || !setup_ninja()) {
                std::cerr << "[ERROR] LLVM/Ninja installation failed. Aborting.\n";
                return;
            }

            if (!generate_ninja_build_file()) {
                std::cerr << "[ERROR] Failed to generate Ninja build file. Aborting.\n";
                return;
            }

            std::cout << "[INFO] Running Ninja...\n";
            if (system("ninja") != 0) {
                std::cerr << "[ERROR] Compilation failed! Update aborted.\n";
                return;
            }

            // you can reverse the redunant fallback order from "binary to source" to "source to binary" in download/usage order
            // by using this and altering the update_from_source() function differently, add binary_url to install_from_source(const std::string& binary_url) 
            //install_from_binary(binary_url);
            replace_binary(OUTPUT_BINARY());
        }

        // 🔄 **Update from Source (Minimal)**
        void update_project(const std::string& source_url, const std::string& binary_url) {

            //    if (!check_for_updates(source_url)) {
            //#if defined(_WIN32)
            //        system("cls");  // Windows
            //#else
            //        system("clear"); // Linux/macOS
            //#endif
            //        std::cout << "[INFO] No updates available.\n";
            //        return;
            //    }

                // get rid of this line if you're looking to reverse redundant fallback order
            if (!install_from_binary(binary_url))
            {
                std::cout << "[INFO] Updating from source...\n";
                install_from_source(binary_url);
            }
        }
    }
}