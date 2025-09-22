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
 // aentity.hpp
#pragma once

#include "aplatform.hpp"   // must always come first

 // High‑level entity helpers (header‑only, functional)
#include "aecs.hpp"                 // reg_ex<…>
#include "aentitycomponents.hpp"    // Position, History, LoggerComponent
#include "aeventsystem.hpp"         // push_event
#include "alogger.hpp"
#include "arobusttime.hpp"

#include <string_view>
#include <format>

namespace almondnamespace::ecs
{
    // ─── spawn_entity ──────────────────────────────────────────────────
    template<typename... Cs>
    inline Entity spawn_entity(reg_ex<Cs...>& R, std::string_view logfile, almondnamespace::LogLevel lvl, time::Timer& clock)
    {
        Entity e = create_entity(R);

        add_component<Position>(R, e, {});
        add_component<History >(R, e, { {{0.f,0.f}} });
        add_component<LoggerComponent>(R, e, { std::string(logfile),lvl,&clock });

        // logging
        if (R.log && R.clk) R.log->log(std::format("[ECS] Entity {} spawned at {}", e, time::getCurrentTimeString()));

        events::push_event({ events::EventType::Custom,
                             {{"action","spawn"},
                              {"entity",std::to_string(e)}} });
        return e;
    }

    // ─── move_entity ──────────────────────────────────────────────────
    template<typename... Cs>
    inline void move_entity(reg_ex<Cs...>& R, Entity e, float dx, float dy)
    {
        auto& pos = get_component<Position>(R, e);
        auto& hist = get_component<History >(R, e);
        hist.states.emplace_back(pos.x, pos.y);        // snapshot

        pos.x += dx;
        pos.y += dy;

        auto& lc = get_component<LoggerComponent>(R, e);
        Logger logger{ lc.file, *lc.clock, lc.level };
        std::string ts = lc.clock->getCurrentTimeString();
        logger.log(std::format("[ECS] Entity {} moved to ({:.2f},{:.2f}) at {}", e, pos.x, pos.y, ts));

        events::push_event({ events::EventType::Custom,
                             {{"action","move"},
                              {"entity",std::to_string(e)},
                              {"x",std::to_string(pos.x)},
                              {"y",std::to_string(pos.y)},
                              {"time",ts}}, pos.x,pos.y });
    }

    // ─── rewind_entity ────────────────────────────────────────────────
    template<typename... Cs>
    inline bool rewind_entity(reg_ex<Cs...>& R, Entity e)
    {
        auto& hist = get_component<History>(R, e);
        if (hist.states.size() <= 1) return false;

        hist.states.pop_back();
        auto [px, py] = hist.states.back();
        auto& pos = get_component<Position>(R, e);
        pos.x = px; pos.y = py;

        auto& lc = get_component<LoggerComponent>(R, e);
        Logger logger{ lc.file, *lc.clock, lc.level };
        std::string ts = lc.clock->getCurrentTimeString();
        logger.log(std::format("[ECS] Entity {} rewound to ({:.2f},{:.2f}) at {}", e, pos.x, pos.y, ts));

        events::push_event({ events::EventType::Custom,
                             {{"action","rewind"},
                              {"entity",std::to_string(e)},
                              {"x",std::to_string(pos.x)},
                              {"y",std::to_string(pos.y)},
                              {"time",ts}}, pos.x,pos.y });
        return true;
    }
} // namespace almondnamespace::ecs
