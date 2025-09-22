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

#include "aplatform.hpp"      // must always come first
#include "alogger.hpp"        // Logger
#include "arobusttime.hpp"    // almondnamespace::time::RobustTime

#include <unordered_map>
#include <vector>
#include <mutex>
#include <format>

namespace almondnamespace::ecs
{
    /// History container: map from EntityID → list of past (x,y) states
    using History = std::unordered_map<EntityID, std::vector<std::pair<float, float>>>;

    /**
     * save_state
     *   Record a new (x,y) snapshot for entity `id`.
     *   Logs the action via `logger` and `clock`.
     */
    inline void save_state(History& history,
        EntityID id,
        float x,
        float y,
        Logger& logger)
    {
        static std::mutex mtx;
        {
            std::lock_guard lock(mtx);
            history[id].emplace_back(x, y);
        }
        logger.log(std::format(
            "[ECS] Saved state for EntityID {} at {} -> ({:.2f}, {:.2f})",
            id,
            time::getCurrentTimeString(),
            x, y
        ));
    }

    /**
     * rewind_state
     *   Rewind one step for entity `id`.
     *   Returns true if successful (and outputs x,y), false if no history.
     *   Logs the action via `logger` and `clock`.
     */
    inline bool rewind_state(History& history,
        EntityID id,
        float& x,
        float& y,
        Logger& logger)
    {
        static std::mutex mtx;
        bool ok = false;
        {
            std::lock_guard lock(mtx);
            auto it = history.find(id);
            if (it != history.end() && it->second.size() > 1) {
                it->second.pop_back();
                auto const& prev = it->second.back();
                x = prev.first;
                y = prev.second;
                ok = true;
            }
        }
        if (ok) {
            logger.log(std::format(
                "[ECS] EntityID {} rewound to ({:.2f}, {:.2f}) at {}",
                id, x, y, time::getCurrentTimeString()
            ));
        }
        else {
            logger.log(std::format(
                "[ECS] EntityID {} rewind failed (no history) at {}",
                id, time::getCurrentTimeString()
            ));
        }
        return ok;
    }

} // namespace almondnamespace::ecs
