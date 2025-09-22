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
 // afroggerlike.hpp
#pragma once

#include "aplatformpump.hpp"
#include "arobusttime.hpp"
#include "acontext.hpp"
#include "ainput.hpp"
#include "agamecore.hpp"
#include "aatlasmanager.hpp"
#include "aimageloader.hpp"

#include <vector>
#include <chrono>
#include <unordered_map>
#include <string>
#include <iostream>

namespace almondnamespace::frogger {

    constexpr int GRID_W = 16;
    constexpr int GRID_H = 12;

    struct GameState {
        int frogX, frogY;
        GameState() : frogX(GRID_W / 2), frogY(GRID_H - 1) {}
    };

    inline bool run_frogger(std::shared_ptr<core::Context> ctx) {
        using namespace almondnamespace;

        // === Setup Atlas ===
        if (!atlasmanager::create_atlas({
            .name = "frogger_atlas",
            .width = 512,
            .height = 512,
            .generate_mipmaps = false }))
        {
            std::cerr << "[Frogger] Failed to create atlas\n";
            return false;
        }

        auto* registrar = atlasmanager::get_registrar("frogger_atlas");
        if (!registrar) {
            std::cerr << "[Frogger] Missing atlas registrar\n";
            return false;
        }

        TextureAtlas& atlas = registrar->atlas;

        // --- Load Frog Sprite ---
        auto frogImg = a_loadImage("assets/frog.ppm", false);
        if (frogImg.pixels.empty()) {
            std::cerr << "[Frogger] Failed to load frog.ppm\n";
            return false;
        }

        if (!registrar->register_atlas_sprites_by_image(
            "frog", frogImg.pixels, frogImg.width, frogImg.height, atlas))
        {
            std::cerr << "[Frogger] Failed to register frog sprite\n";
            return false;
        }

        atlas.rebuild_pixels();
        atlasmanager::ensure_uploaded(atlas);

        auto& atlasVec = atlasmanager::get_atlas_vector();
        std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

        // --- Ask registry for frog handle ---
        auto frogOpt = atlasmanager::registry.get("frog");
        if (!frogOpt) {
            std::cerr << "[Frogger] Registry missing frog handle\n";
            return false;
        }
        SpriteHandle frogHandle = std::get<0>(*frogOpt);

        GameState s;
        bool game_over = false;

        while (!game_over) {
            platform::pump_events();
            if (ctx->is_key_down_safe(input::Key::Escape)) break;

            if (ctx->is_key_down_safe(input::Key::Left) && s.frogX > 0)         --s.frogX;
            if (ctx->is_key_down_safe(input::Key::Right) && s.frogX < GRID_W - 1)  ++s.frogX;
            if (ctx->is_key_down_safe(input::Key::Up) && s.frogY > 0)         --s.frogY;
            if (ctx->is_key_down_safe(input::Key::Down) && s.frogY < GRID_H - 1)  ++s.frogY;

            if (s.frogY == 0) game_over = true;

            ctx->clear_safe(ctx);

            float cw = static_cast<float>(ctx->get_width_safe()) / GRID_W;
            float ch = static_cast<float>(ctx->get_height_safe()) / GRID_H;

            if (spritepool::is_alive(frogHandle)) {
                ctx->draw_sprite_safe(frogHandle, atlasSpan,
                    s.frogX * cw, s.frogY * ch, cw, ch);
            }

            ctx->present_safe();
        }

        return game_over;
    }

} // namespace almondnamespace::frogger
