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
 //aecs.hpp
#pragma once

#include "aentitycomponentmanager.hpp"    // your ComponentStorage + add/get/has/remove
#include "aeventsystem.hpp"         // events::push_event
#include "alogger.hpp"              // Logger, LogLevel
#include "arobusttime.hpp"          // RobustTime
#include "aentityhistory.hpp"

#include <typeinfo>
#include <string_view>
#include <format>
#include <utility>
#include <vector>
#include <cassert>

namespace almondnamespace::ecs {

    using Entity = EntityID;  // alias for your EntityID

    // ─── reg_ex: holds storage, ID counter, optional log/time ──────────────
    template<typename... Cs>
    struct reg_ex {
        ComponentStorage   storage;    // your erased MPMC‑backed storage
        EntityID           nextID{ 1 };  // simple entity ID generator
        Logger* log{ nullptr };
        time::Timer* clk{ nullptr };
    };

    // helper to construct with optional Logger/Clock
    template<typename... Cs>
    [[nodiscard]] inline reg_ex<Cs...> make_registry(
        Logger* L = nullptr,
        time::Timer* C = nullptr)
    {
        return { {}, 1, L, C };
    }

    namespace _detail {
        template<typename... Cs>
        inline void notify(reg_ex<Cs...>& R,
            std::string_view action,
            Entity e,
            std::string_view comp = "")
        {
            if (!R.log || !R.clk) return;
            auto ts = time::getCurrentTimeString();
            R.log->log(std::format("[ECS] {}{} entity={} at {}",
                action,
                comp.empty() ? "" : std::format(":{}", comp),
                e, ts));
            events::push_event(events::Event{
                events::EventType::Custom,
                { {"ecs_action", std::string(action)},
                  {"entity",     std::to_string(e)},
                  {"component",  std::string(comp)},
                  {"time",       ts} },
                0.f, 0.f });
        }
    }

    // ─── CRUD API ───────────────────────────────────────────────────────────

    // create a new entity ID
    template<typename... Cs>
    inline Entity create_entity(reg_ex<Cs...>& R) {
        Entity e = R.nextID++;
        _detail::notify(R, "createEntity", e);
        return e;
    }

    // destroy: erase all components for that entity
    template<typename... Cs>
    inline void destroy_entity(reg_ex<Cs...>& R, Entity e) {
        // erase all types in Cs...
        (remove_component<Cs>(R, e), ...);
        _detail::notify(R, "destroyEntity", e);
    }

    // add a component of type C
    template<typename C, typename... Cs>
    inline void add_component(reg_ex<Cs...>& R, Entity e, C c) {
        ::almondnamespace::ecs::add_component<C>(R.storage, e, std::move(c));
        _detail::notify(R, "addComponent", e, typeid(C).name());
    }

    // remove component C
    template<typename C, typename... Cs>
    inline void remove_component(reg_ex<Cs...>& R, Entity e) {
        ::almondnamespace::ecs::remove_component<C>(R.storage, e);
        _detail::notify(R, "removeComponent", e, typeid(C).name());
    }

    // test for component C
    template<typename C, typename... Cs>
    [[nodiscard]] inline bool has_component(const reg_ex<Cs...>& R, Entity e) {
        return ::almondnamespace::ecs::has_component<C>(R.storage, e);
    }

    // get mutable reference to component C
    template<typename C, typename... Cs>
    [[nodiscard]] inline C& get_component(reg_ex<Cs...>& R, Entity e) {
        return ::almondnamespace::ecs::get_component<C>(R.storage, e);
    }

    // iterate over entities that have all Vs…
    template<typename... Vs, typename... Cs, typename Fn>
    inline void view(reg_ex<Cs...>& R, Fn&& fn) {
        // brute‑force: scan storage for first type, test others
        for (auto& [ent, compMap] : R.storage) {
            if ((has_component<Vs>(R, ent) && ...)) {
                fn(ent, get_component<Vs>(R, ent)...);
            }
        }
    }

} // namespace almondnamespace::ecs