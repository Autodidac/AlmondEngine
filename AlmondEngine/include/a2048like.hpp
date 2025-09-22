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
// a2048like.hpp
#pragma once

#include "acontext.hpp"
#include "ainput.hpp"
#include "agamecore.hpp"
#include "aplatformpump.hpp"
#include "arobusttime.hpp"
#include "aatlasmanager.hpp"
#include "aimageloader.hpp"

#include <string>
#include <random>
#include <iostream>
#include <unordered_map>

namespace almondnamespace::game2048
{
    constexpr int GRID_W = 4;
    constexpr int GRID_H = 4;

    struct GameState {
        gamecore::grid_t<int> grid;
        std::mt19937 rng{ std::random_device{}() };

        GameState() : grid(gamecore::make_grid<int>(GRID_W, GRID_H, 0)) {
            add_tile(); add_tile();
        }
        void add_tile() {
            for (;;) {
                int x = rng() % GRID_W, y = rng() % GRID_H;
                auto& cell = grid[gamecore::idx(GRID_W, x, y)];
                if (cell == 0) { cell = (rng() % 2 + 1) * 2; break; }
            }
        }
    };

    // Optional helper if you want to reset game state when leaving/returning
    inline void reset_2048_state() {
        // Nothing persistent besides the static GameState in run_2048;
        // you can clear it by reassigning a new one if desired.
    }

    // One-frame tick. Return true to keep running, false to exit mini-game.
    inline bool run_2048(std::shared_ptr<core::Context> ctx)
    {
        static bool assets_ready = false;
        static std::unordered_map<int, SpriteHandle> sprites;

        if (!assets_ready) {
            if (!atlasmanager::create_atlas({
                .name = "2048_atlas",
                .width = 256,
                .height = 256,
                .generate_mipmaps = false }))
            {
                std::cerr << "[2048] Failed to create atlas\n";
                return false;
            }

            auto* registrar = atlasmanager::get_registrar("2048_atlas");
            if (!registrar) {
                std::cerr << "[2048] Missing atlas registrar\n";
                return false;
            }
            TextureAtlas& atlas = registrar->atlas;

            // Load tile sprites 2..2048
            for (int val = 2; val <= 2048; val <<= 1) {
                std::string name = std::to_string(val);
                auto img = a_loadImage("assets/2048/" + name + ".ppm", false);
                if (img.pixels.empty()) {
                    std::cerr << "[2048] Missing image " << name << ".ppm\n";
                    return false;
                }
                auto handleOpt = registrar->register_atlas_sprites_by_image(
                    name, img.pixels, img.width, img.height, atlas);
                if (!handleOpt) return false;
                sprites[val] = *handleOpt;
            }

            // Build + upload atlas once
            registrar->atlas.rebuild_pixels();
            atlasmanager::ensure_uploaded(registrar->atlas);
            assets_ready = true;
        }

        // Persistent game state across frames
        static GameState state;
        static bool game_over = false;

        // Input & exit
        platform::pump_events();
        if (almondnamespace::input::is_key_down(almondnamespace::input::Key::Escape))
            return false;

        // TODO: add swipe/merge logic here (left/right/up/down)

        // Render
        ctx->clear_safe(ctx);

        auto& atlasVec = atlasmanager::get_atlas_vector();
        std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

        const float cellW = float(ctx->get_width_safe()) / GRID_W;
        const float cellH = float(ctx->get_height_safe()) / GRID_H;

        for (int y = 0; y < GRID_H; ++y) {
            for (int x = 0; x < GRID_W; ++x) {
                int val = state.grid[gamecore::idx(GRID_W, x, y)];
                if (val == 0) continue;
                auto it = sprites.find(val);
                if (it != sprites.end() && spritepool::is_alive(it->second)) {
                    ctx->draw_sprite_safe(it->second, atlasSpan,
                        x * cellW, y * cellH, cellW, cellH);
                }
            }
        }

        ctx->present_safe();
        return !game_over;
    }
}
