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
 // aenduserapplication.hpp  – functional Snake mini‑app
#pragma once

#include "aplatform.hpp"          // must be first

#include "aplatformpump.hpp"
#include "acontext.hpp"
#include "aopenglcontext.hpp"
#include "aeventsystem.hpp"
#include "ainput.hpp"
#include "arobusttime.hpp"
#include "aapplicationmodule.hpp"

#include <deque>
#include <random>

namespace almondnamespace::app
{
    // ─── Fixed‑step config ──────────────────────────────────────────────
	constexpr double STEP_S = 0.150;   // 150 ms per frame
    namespace events = almondnamespace::events;

    // ─── helper: enqueue raw input as events (no coupling) ──────────────
    inline void translate_input(const core::Context& ctx) noexcept
    {
        int mx = 0, my = 0;
        ctx.get_mouse_position(mx, my);
        events::push_event({ events::EventType::MouseMove, {}, float(mx), float(my) });

        if (ctx.is_mouse_button_down(input::MouseButton::MouseLeft))
            events::push_event({ events::EventType::MouseButtonClick,
                                 {{"button","left"}}, float(mx), float(my) });

        if (ctx.is_key_down(input::Key::Escape))
            events::push_event({ events::EventType::KeyPress,
                                 {{"key","escape"}} });
    }

    // ─── Main loop (ECS-free stub – slots neatly into your engine) ─────
    inline bool run_app(std::shared_ptr<almondnamespace::core::Context>& ctx)   
    {
        // reset GL / load sprites
        //opengl::reset_texture_system();
       // Handle bodySlot = opengl::load_and_register(ctx, "assets/yellow.ppm", "snake_body");
       // Handle foodSlot = opengl::load_and_register(ctx, "assets/yellow.ppm", "snake_food");

        // timers
        time::Timer clock;
        //auto tick = clock.createTimer(); tick.start();
        double acc = 0.0;

        // one‑shot init for external modules
        for (auto m : almondnamespace::detail::get_modules()) if (m->init) m->init();

        bool game_over = false;
        //while (!game_over && ctx->process(*ctx))
        //{
        //    platform::pump_events();           // OS‑level
        //    translate_input(*ctx);              // context→event queue
        //    events::pump();                    // dispatch to callbacks

        //    // fixed‑time accumulator
        //   // double dt = tick.elapsed(); tick.restart(); acc += dt;
        //    while (acc >= STEP_S)
        //    {
        //        // --- update gameplay here (snake ECS not shown) ---

        //        // let user modules run
        //        for (auto m : almondnamespace::detail::get_modules())
        //            if (m->update) m->update(static_cast<float>(STEP_S));

        //        acc -= STEP_S;
        //    }

        //    ctx->clear();
        //    // --- draw game here (sprites) ---
        //    // ctx.draw_sprite(bodySlot, ...);
        //    // ctx.draw_sprite(foodSlot, ...);
        //    ctx->present();
        //}

        for (auto m : almondnamespace::detail::get_modules()) if (m->shutdown) m->shutdown();
        return game_over;
    }

} // namespace almondnamespace::app
