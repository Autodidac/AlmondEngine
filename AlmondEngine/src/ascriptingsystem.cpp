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
// ascriptingsystem.cpp
#include "pch.h"

#include "aplatform.hpp"
#include "aengineconfig.hpp"

#include "aenginebindings.hpp"
#include "ascriptingsystem.hpp"
#include "acompiler.hpp"
#include "aenginesystems.hpp"

#include <filesystem>
#include <iostream>

#ifdef _WIN32
#ifndef ALMOND_MAIN_HEADLESS
//    #include <Windows.h>
    static HMODULE lastLib = nullptr;
#else
	// windows no main fallback, used for raylib or other contexts that require their own main
    static void* lastLib = nullptr;
#endif
#else
    #include <dlfcn.h>
    static void* lastLib = nullptr;
#endif

using namespace almondnamespace;
using namespace almondnamespace::scripting;
using namespace std::literals;

namespace fs = std::filesystem;
using run_script_fn = void(*)(ScriptScheduler&);

// —————————————————————————————————————————————————————————————————
// Coroutine Task: Compile and Load Script DLL
// —————————————————————————————————————————————————————————————————
static almondnamespace::Task do_load_script(const std::string& scriptName, ScriptScheduler& scheduler) {
    try {
        const fs::path sourcePath = fs::path("src/scripts") / (scriptName + ".ascript.cpp");
        const fs::path dllPath = fs::path("src/scripts") / (scriptName + ".dll");

        if (lastLib) {
#ifdef _WIN32
#ifndef ALMOND_MAIN_HEADLESS
            FreeLibrary(lastLib);
#endif  
#else
            dlclose(lastLib);
#endif
            lastLib = nullptr;
        }

        if (!compiler::compile_script_to_dll(sourcePath, dllPath)) {
            std::cerr << "[script] Compilation failed: " << sourcePath << "\n";
            co_return;
        }

#ifdef _WIN32
#ifndef ALMOND_MAIN_HEADLESS
        lastLib = LoadLibraryA(dllPath.string().c_str());
        if (!lastLib) {
            std::cerr << "[script] LoadLibrary failed: " << dllPath << "\n";
            co_return;
        }
        auto entry = reinterpret_cast<run_script_fn>(GetProcAddress(lastLib, "run_script"));
#else
      //  auto entry = reinterpret_cast<run_script_fn>("somethingel", "run_script");
#endif
#else
        lastLib = dlopen(dllPath.string().c_str(), RTLD_NOW);
        if (!lastLib) {
            std::cerr << "[script] dlopen failed: " << dllPath << "\n";
            co_return;
        }
        auto entry = reinterpret_cast<run_script_fn>(dlsym(lastLib, "run_script"));
#endif
#ifndef ALMOND_MAIN_HEADLESS
        if (!entry) {
            std::cerr << "[script] Missing run_script symbol in: " << dllPath << "\n";
            co_return;
        }

        entry(scheduler);
#endif
    }
    catch (const std::exception& e) {
        std::cerr << "[script] Exception during script load: " << e.what() << "\n";
    }
    catch (...) {
        std::cerr << "[script] Unknown exception during script load\n";
    }
    co_return;
}

// —————————————————————————————————————————————————————————————————
// External entry point (engine-facing)
// —————————————————————————————————————————————————————————————————
bool almondnamespace::scripting::load_or_reload_script(const std::string& scriptName, ScriptScheduler& scheduler) {
    try {
        almondnamespace::Task t = do_load_script(scriptName, scheduler);
        auto node = std::make_unique<taskgraph::Node>(std::move(t));
        scheduler.AddNode(std::move(node));
        scheduler.Execute();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[script] Scheduling exception: " << e.what() << "\n";
        return false;
    }
}