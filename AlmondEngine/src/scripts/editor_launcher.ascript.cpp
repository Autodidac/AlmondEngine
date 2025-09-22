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
//#include "ataskscheduler.hpp"
//#include "aenginebindings.hpp"
//
//#include <chrono>
//#include <string>
//#include <sstream>
//
//extern "C" 
//{
//    scriptimport void run_script(TaskScheduler& scheduler) {
//        engine_print("[script] Starting multithreaded job demo...");
//
//        for (int i = 0; i < 8; ++i) {
//            scheduler.submit([i]() {
//                std::ostringstream ss;
//                ss << "[thread] Task " << i << " running on thread " << std::this_thread::get_id();
//                engine_print(ss.str().c_str());
//
//                std::this_thread::sleep_for(std::chrono::milliseconds(100 + i * 50));
//
//                ss.str(""); ss.clear();
//                ss << "[thread] Task " << i << " done.";
//                engine_print(ss.str().c_str());
//                });
//        }
//
//        engine_print("[script] All tasks submitted.");
//    }
//}