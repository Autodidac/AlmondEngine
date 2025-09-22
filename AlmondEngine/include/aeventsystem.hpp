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

#include "aplatform.hpp"      // Must always come first for platform defines

#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>       // std::move
#include <vector>

namespace almondnamespace::events {

    // ─── Event definitions ─────────────────────────────────────────────
    enum class EventType : uint8_t {
        MouseButtonClick,
        MouseMove,
        KeyPress,
        TextInput,
        Custom,
        Unknown
    };

    struct Event {
        EventType                                   type{ EventType::Unknown };
        std::unordered_map<std::string, std::string> data{};
        float                                       x{ 0 }, y{ 0 };
        uint32_t                                    key{ 0 };
        char32_t                                    text{ 0 };
    };

    // ─── Enum ↔ string helpers (constexpr) ────────────────────────────
    [[nodiscard]] constexpr std::string_view event_type_to_string(EventType t) noexcept {
        switch (t) {
        case EventType::MouseButtonClick: return "MouseButtonClick";
        case EventType::MouseMove:        return "MouseMove";
        case EventType::KeyPress:         return "KeyPress";
        case EventType::TextInput:        return "TextInput";
        case EventType::Custom:           return "Custom";
        default:                          return "Unknown";
        }
    }
    [[nodiscard]] constexpr EventType event_type_from(std::string_view s) noexcept {
        if (s == "MouseButtonClick") return EventType::MouseButtonClick;
        if (s == "MouseMove")        return EventType::MouseMove;
        if (s == "KeyPress")         return EventType::KeyPress;
        if (s == "TextInput")        return EventType::TextInput;
        if (s == "Custom")           return EventType::Custom;
        return EventType::Unknown;
    }

    // ─── Lock‑free MPSC ring buffer ───────────────────────────────────
    template<std::size_t N = 4096>
    struct mpsc_ring {
        static_assert((N& (N - 1)) == 0,
            "Capacity must be a power of two");
        std::array<Event, N>          buf{};
        std::atomic<std::size_t>     head{ 0 };
        std::atomic<std::size_t>     tail{ 0 };

        bool enqueue(const Event& e) noexcept {
            auto h = head.fetch_add(1, std::memory_order_acq_rel);
            while (h - tail.load(std::memory_order_acquire) >= N) {}  // spin if full
            buf[h & (N - 1)] = e;
            return true;
        }
        bool dequeue(Event& out) noexcept {
            auto t = tail.load(std::memory_order_relaxed);
            if (t == head.load(std::memory_order_acquire)) return false; // empty
            out = std::move(buf[t & (N - 1)]);
            tail.store(t + 1, std::memory_order_release);
            return true;
        }
    };

    // ─── Globals (header‑only) + public API ───────────────────────────
    inline mpsc_ring<>                    g_queue;
    using Callback = std::function<void(const Event&)>;
    inline std::vector<Callback>& g_callbacks() {
        static std::vector<Callback> v;
        return v;
    }

    inline void register_callback(Callback cb) { g_callbacks().push_back(std::move(cb)); }
    inline void push_event(const Event& e) noexcept { g_queue.enqueue(e); }
    inline void pump() noexcept {
        Event e;
        while (g_queue.dequeue(e))
            for (auto& fn : g_callbacks()) fn(e);
    }

} // namespace almondnamespace::events
