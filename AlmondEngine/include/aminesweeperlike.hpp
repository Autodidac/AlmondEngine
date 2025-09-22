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
 // aminesweeperlike.hpp
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
#include <tuple>
#include <unordered_map>
#include <string>
#include <random>
#include <iostream>

namespace almondnamespace::minesweeper
{
    constexpr int GRID_W = 16;
    constexpr int GRID_H = 16;
    constexpr int MINES = 40;

    struct GameState
    {
        gamecore::grid_t<bool> mine;
        gamecore::grid_t<bool> revealed;
        gamecore::grid_t<int>  count;

        GameState()
            : mine(gamecore::make_grid<bool>(GRID_W, GRID_H, false)),
            revealed(gamecore::make_grid<bool>(GRID_W, GRID_H, false)),
            count(gamecore::make_grid<int>(GRID_W, GRID_H, 0))
        {
            int placed = 0;
            std::mt19937_64 rng{ std::random_device{}() };
            std::uniform_int_distribution<int> dx(0, GRID_W - 1), dy(0, GRID_H - 1);
            while (placed < MINES)
            {
                int x = dx(rng), y = dy(rng);
                auto idx = gamecore::idx(GRID_W, x, y);
                if (!mine[idx]) { mine[idx] = true; ++placed; }
            }

            for (int y = 0; y < GRID_H; ++y)
                for (int x = 0; x < GRID_W; ++x)
                    if (!mine[gamecore::idx(GRID_W, x, y)])
                    {
                        int c = 0;
                        for (auto [nx, ny] : gamecore::neighbors(GRID_W, GRID_H, x, y))
                            if (mine[gamecore::idx(GRID_W, nx, ny)]) ++c;
                        count[gamecore::idx(GRID_W, x, y)] = c;
                    }
        }

        bool all_clear() const
        {
            for (size_t i = 0; i < revealed.size(); ++i)
                if (!revealed[i] && !mine[i]) return false;
            return true;
        }
    };

    inline bool run_minesweeper(std::shared_ptr<core::Context> ctx)
    {
        using namespace almondnamespace;

        // === Setup Atlas ===
        if (!atlasmanager::create_atlas({
            .name = "minesweeper_atlas",
            .width = 512,
            .height = 512,
            .generate_mipmaps = false }))
        {
            std::cerr << "[Minesweeper] Failed to create atlas\n";
            return false;
        }

        auto* registrar = atlasmanager::get_registrar("minesweeper_atlas");
        if (!registrar)
        {
            std::cerr << "[Minesweeper] Missing atlas registrar\n";
            return false;
        }

        TextureAtlas& atlas = registrar->atlas;

        // === Load Sprites into Atlas ===
        for (int i = 0; i <= 8; ++i) {
            auto id = std::to_string(i);
            auto img = a_loadImage("assets/" + id + ".ppm", false);
            if (img.pixels.empty()) {
                std::cerr << "[Minesweeper] Missing image " << id << "\n";
                return false;
            }
            if (!registrar->register_atlas_sprites_by_image(id, img.pixels, img.width, img.height, atlas))
                return false;
        }

        for (auto id : { "covered", "mine" }) {
            auto img = a_loadImage("assets/" + std::string(id) + ".ppm", false);
            if (img.pixels.empty()) {
                std::cerr << "[Minesweeper] Missing image " << id << "\n";
                return false;
            }
            if (!registrar->register_atlas_sprites_by_image(id, img.pixels, img.width, img.height, atlas))
                return false;
        }

        atlas.rebuild_pixels();
        atlasmanager::ensure_uploaded(atlas);

        auto& atlasVec = atlasmanager::get_atlas_vector();
        std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

        // === Fetch from Registry ===
        std::unordered_map<std::string, SpriteHandle> sprites;
        for (int i = 0; i <= 8; ++i) {
            auto id = std::to_string(i);
            auto opt = atlasmanager::registry.get(id);
            if (!opt) return false;
            sprites[id] = std::get<0>(*opt);
        }
        for (auto id : { "covered", "mine" }) {
            auto opt = atlasmanager::registry.get(id);
            if (!opt) return false;
            sprites[id] = std::get<0>(*opt);
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
                    auto idx = gamecore::idx(GRID_W, x, y);
                    if (!s.revealed[idx])
                    {
                        s.revealed[idx] = true;
                        if (s.mine[idx]) { game_over = true; }
                        else if (s.count[idx] == 0)
                        {
                            auto flood = [&](auto&& self, int fx, int fy) -> void {
                                if (!gamecore::in_bounds(GRID_W, GRID_H, fx, fy)) return;
                                auto i = gamecore::idx(GRID_W, fx, fy);
                                if (s.revealed[i]) return;
                                s.revealed[i] = true;
                                if (s.count[i] == 0)
                                    for (auto [nx, ny] : gamecore::neighbors(GRID_W, GRID_H, fx, fy))
                                        self(self, (int)nx, (int)ny);
                                };
                            flood(flood, x, y);
                        }
                    }
                }
            }

            if (s.all_clear()) break;

            ctx->clear_safe(ctx);

            const float cw = float(ctx->get_width_safe()) / GRID_W;
            const float ch = float(ctx->get_height_safe()) / GRID_H;

            for (int y = 0; y < GRID_H; ++y)
                for (int x = 0; x < GRID_W; ++x)
                {
                    size_t idx = gamecore::idx(GRID_W, x, y);
                    std::string key = !s.revealed[idx] ? "covered" :
                        (s.mine[idx] ? "mine" : std::to_string(s.count[idx]));

                    auto it = sprites.find(key);
                    if (it != sprites.end() && spritepool::is_alive(it->second))
                    {
                        ctx->draw_sprite_safe(it->second, atlasSpan,
                            x * cw, y * ch, cw, ch);
                    }
                }

            ctx->present_safe();
        }

        return game_over;
    }
} // namespace almondnamespace::minesweeper
