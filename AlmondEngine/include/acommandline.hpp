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
// acommandline.hpp ─ AlmondShell command‑line utilities (header‑only)
// C++20, no .inl, functional free‑functions only.
#pragma once

#include "aversion.hpp"          // GetEngineName(), GetEngineVersion()

#include <filesystem>
#include <iostream>
#include <string_view>

namespace almondnamespace::core::cli {

    // ─── public, mutable knobs (inline globals) ────────────────────────
    inline int  window_width = DEFAULT_WINDOW_WIDTH;
    inline int  window_height = DEFAULT_WINDOW_HEIGHT;
    inline std::filesystem::path exe_path;

    // ─── helpers ───────────────────────────────────────────────────────
    inline void print_engine_info() {
        std::cout << GetEngineName() << " v" << GetEngineVersion() << '\n';
    }

    // ─── parse() ───────────────────────────────────────────────────────
    inline void parse(int argc, char* argv[])
    {
        using namespace std::string_view_literals;
        if (argc < 1) {
            std::cerr << "No command-line arguments provided.\n";
            return;
		}
        else
        { 
            exe_path = argv[0];
            std::cout << "Commandline for " << exe_path.filename().string() << ":\n";
            for (int i = 1; i < argc; ++i) {
                std::string_view arg{ argv[i] };

                if (arg == "--help"sv || arg == "-h"sv) {
                    std::cout <<
                        "  --help, -h            Show this help message\n"
                        "  --version, -v         Display the engine version\n"
                        "  --width  <value>      Set window width\n"
                        "  --height <value>      Set window height\n";
                }
                else if (arg == "--version"sv || arg == "-v"sv) {
                    print_engine_info();
                }
                else if (arg == "--width"sv && i + 1 < argc) {
                    window_width = std::stoi(argv[++i]);
                }
                else if (arg == "--height"sv && i + 1 < argc) {
                    window_height = std::stoi(argv[++i]);
                }
                else {
                    std::cerr << "Unknown arg: " << arg << '\n';
                }
            }
            std::cout << '\n';
        }
    }

} // namespace almondnamespace::core::cli
