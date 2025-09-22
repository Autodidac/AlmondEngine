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

#include <chrono>
#include <unordered_map>
#include <string>
#include <string_view>

namespace almondnamespace::time {

    using Clock = std::chrono::steady_clock;

    struct Timer {
        Clock::time_point baseStart{ Clock::now() };
        double accumulatedTime = 0.0;
        double timeScale = 1.0;
        bool paused = false;
        double manualElapsed = 0.0;
    };

    //── Timer logic ──────────────────────────────────────────────//

    inline Timer createTimer(double scale = 1.0) {
        Timer t;
        t.timeScale = scale;
        return t;
    }

    inline double elapsed(const Timer& t) {
        if (t.paused) return t.accumulatedTime;
        auto now = Clock::now();
        auto realElapsed = std::chrono::duration<double>(now - t.baseStart).count();
        return t.accumulatedTime + (realElapsed * t.timeScale) + t.manualElapsed;
    }

    inline double realElapsed(const Timer& t) {
        if (t.paused) return t.accumulatedTime / t.timeScale;
        return std::chrono::duration<double>(Clock::now() - t.baseStart).count();
    }

    inline void pause(Timer& t) {
        if (!t.paused) {
            t.accumulatedTime = elapsed(t);
            t.paused = true;
        }
    }

    inline void resume(Timer& t) {
        if (t.paused) {
            t.baseStart = Clock::now();
            t.paused = false;
        }
    }

    inline void reset(Timer& t) {
        t.baseStart = Clock::now();
        t.accumulatedTime = 0.0;
        t.manualElapsed = 0.0;
        t.paused = false;
    }

    inline void advance(Timer& t, double dt) {
        t.manualElapsed += dt * t.timeScale;
    }

    inline void setScale(Timer& t, double newScale) {
        auto unscaled = elapsed(t);
        t.timeScale = newScale;
        t.baseStart = Clock::now();
        t.accumulatedTime = unscaled;
        t.manualElapsed = 0.0;
    }

    //── Named registry support ──────────────────────────────────//

    struct ScopedTimers {
        std::unordered_map<std::string, Timer> timers;
    };

    inline std::unordered_map<std::string, ScopedTimers>& timerRegistry() {
        static std::unordered_map<std::string, ScopedTimers> registry;
        return registry;
    }

    inline Timer& createNamedTimer(std::string_view group, std::string_view name, double scale = 1.0) {
        return timerRegistry()[std::string(group)].timers[std::string(name)] = createTimer(scale);
    }

    inline Timer* getTimer(std::string_view group, std::string_view name) {
        auto it = timerRegistry().find(std::string(group));
        if (it != timerRegistry().end()) {
            auto jt = it->second.timers.find(std::string(name));
            if (jt != it->second.timers.end()) return &jt->second;
        }
        return nullptr;
    }

    inline void removeTimer(std::string_view group, std::string_view name) {
        auto it = timerRegistry().find(std::string(group));
        if (it != timerRegistry().end()) {
            it->second.timers.erase(std::string(name));
            if (it->second.timers.empty())
                timerRegistry().erase(it);
        }
    }

    //── Current Time as a String ──────────────────────────────────//

    inline std::string getCurrentTimeString() {
        const auto now = std::chrono::system_clock::now();
        const auto local = std::chrono::zoned_time{ std::chrono::current_zone(), now };
        return std::format("{:%Y-%m-%d %H:%M:%S}", local);
    }


} // namespace almondnamespace::time
