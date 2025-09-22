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
 // aentitycomponents.hpp  – simple data‑only components
#pragma once

#include "aplatform.hpp"      // Must always come first for platform defines

#include "alogger.hpp"        // LogLevel
#include "arobusttime.hpp"    // RobustTime

#include <vector>
#include <string>
#include <utility>

namespace almondnamespace::ecs {

    // ─── Position ─────────────────────────────────────────────────────────
    struct Position { float x{ 0 }, y{ 0 }; };

    // ─── Velocity ─────────────────────────────────────────────────────────
    struct Velocity { float dx{ 0 }, dy{ 0 }; };

    // ─── History ──────────────────────────────────────────────────────────
    // Each entity that needs rewind support keeps its past states here
   // struct History { std::vector<std::pair<float, float>> states; };

    // ─── LoggerComponent ─────────────────────────────────────────────────
    // Stores per‑entity logging preferences (file, level, clock pointer)
    struct LoggerComponent {
        std::string       file;
        LogLevel          level{ LogLevel::INFO };
        time::Timer* clock{};
    };

} // namespace almondnamespace::ecs


//#include "almond/aentitycomponents.hpp"
//#include "almond/aentity_ops.hpp"
//
//// define registry with the components you use
//using Reg = almondnamespace::ecs::reg_ex<
//    almondnamespace::ecs::Position,
//    almondnamespace::ecs::History,
//    almondnamespace::ecs::LoggerComponent>;
//
//Reg R = almondnamespace::ecs::make_registry();
//
//// spawn/move/rewind
//auto id = almondnamespace::ecs::spawn_entity(R, "engine.log", LogLevel::Info, clock);
//almondnamespace::ecs::move_entity(R, id, 1.0f, 0.0f);
//almondnamespace::ecs::rewind_entity(R, id);