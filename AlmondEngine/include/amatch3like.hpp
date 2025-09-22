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
 // amatch3game.hpp
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
#include <random>
#include <iostream>

namespace almondnamespace::match3
{
    constexpr int GRID_W = 8;
    constexpr int GRID_H = 8;
    constexpr int MAX_GEM_TYPE = 5;

    struct GameState {
        gamecore::grid_t<int> grid;
        int selectedX = -1, selectedY = -1;

        GameState()
            : grid(gamecore::make_grid<int>(GRID_W, GRID_H, 0))
        {
            std::mt19937_64 rng{ std::random_device{}() };
            std::uniform_int_distribution<int> dist(0, MAX_GEM_TYPE);

            for (int y = 0; y < GRID_H; ++y)
                for (int x = 0; x < GRID_W; ++x)
                    grid[gamecore::idx(GRID_W, x, y)] = dist(rng);
        }
    };

    inline bool run_match3(std::shared_ptr<core::Context> ctx)
    {
        using namespace almondnamespace;

        // === Setup Atlas ===
        if (!atlasmanager::create_atlas({
            .name = "match3_atlas",
            .width = 512,
            .height = 512,
            .generate_mipmaps = false }))
        {
            std::cerr << "[Match3] Failed to create atlas\n";
            return false;
        }

        auto* registrar = atlasmanager::get_registrar("match3_atlas");
        if (!registrar)
        {
            std::cerr << "[Match3] Missing atlas registrar\n";
            return false;
        }

        TextureAtlas& atlas = registrar->atlas;

        // --- Load each gem sprite ---
        for (int i = 0; i <= MAX_GEM_TYPE; ++i)
        {
            auto name = "gem" + std::to_string(i);
            auto img = a_loadImage("assets/" + name + ".ppm", false);
            if (img.pixels.empty()) {
                std::cerr << "[Match3] Failed to load " << name << "\n";
                return false;
            }
            if (!registrar->register_atlas_sprites_by_image(name, img.pixels, img.width, img.height, atlas)) {
                return false;
            }
        }

        atlas.rebuild_pixels();
        atlasmanager::ensure_uploaded(atlas);

        auto& atlasVec = atlasmanager::get_atlas_vector();
        std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

        // --- Fetch handles from registry ---
        std::unordered_map<int, SpriteHandle> sprites;
        for (int i = 0; i <= MAX_GEM_TYPE; ++i) {
            auto name = "gem" + std::to_string(i);
            auto opt = atlasmanager::registry.get(name);
            if (!opt) {
                std::cerr << "[Match3] Registry missing " << name << "\n";
                return false;
            }
            sprites[i] = std::get<0>(*opt);
        }

        GameState s;
        bool game_over = false;

        while (!game_over)
        {
            platform::pump_events();
            if (ctx->is_key_down_safe(input::Key::Escape)) break;

            int mx = 0, my = 0;
            ctx->get_mouse_position_safe(mx, my);

            if (ctx->is_mouse_button_down_safe(input::MouseButton::MouseLeft))
            {
                int x = int(mx / (float(ctx->get_width_safe()) / GRID_W));
                int y = int(my / (float(ctx->get_height_safe()) / GRID_H));

                if (gamecore::in_bounds(GRID_W, GRID_H, x, y))
                {
                    if (s.selectedX == -1) {
                        s.selectedX = x; s.selectedY = y;
                    }
                    else if ((abs(x - s.selectedX) + abs(y - s.selectedY)) == 1) {
                        std::swap(
                            s.grid[gamecore::idx(GRID_W, x, y)],
                            s.grid[gamecore::idx(GRID_W, s.selectedX, s.selectedY)]
                        );
                        s.selectedX = -1; s.selectedY = -1;
                    }
                    else {
                        s.selectedX = x; s.selectedY = y;
                    }
                }
            }

            ctx->clear_safe(ctx);

            float cw = float(ctx->get_width_safe()) / GRID_W;
            float ch = float(ctx->get_height_safe()) / GRID_H;

            for (int y = 0; y < GRID_H; ++y)
            {
                for (int x = 0; x < GRID_W; ++x)
                {
                    int t = s.grid[gamecore::idx(GRID_W, x, y)];
                    auto it = sprites.find(t);
                    if (it != sprites.end() && spritepool::is_alive(it->second))
                    {
                        ctx->draw_sprite_safe(it->second, atlasSpan,
                            x * cw, y * ch, cw, ch);
                    }
                }
            }

            ctx->present_safe();
        }
        return game_over;
    }
}
