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
 // apacmanlike.hpp
#pragma once

#include "aplatformpump.hpp"
#include "arobusttime.hpp"
#include "acontext.hpp"
#include "ainput.hpp"
#include "agamecore.hpp"
#include "aatlasmanager.hpp"
#include "aspriteregistry.hpp"
#include "aimageloader.hpp"

#include <deque>
#include <optional>
#include <iostream>
#include <span>

namespace almondnamespace::pacman
{
    constexpr int GRID_W = 28;
    constexpr int GRID_H = 31;

    enum Tile : int { EMPTY, WALL, PELLET };

    struct GameState
    {
        gamecore::grid_t<int> map;
        gamecore::grid_t<bool> pellet;
        int px = 14, py = 23;
        std::deque<std::pair<int, int>> ghosts;

        GameState()
            : map(gamecore::make_grid<int>(GRID_W, GRID_H, EMPTY)),
            pellet(gamecore::make_grid<bool>(GRID_W, GRID_H, true))
        {
            for (int y = 0; y < GRID_H; ++y)
                for (int x = 0; x < GRID_W; ++x)
                {
                    if (x == 0 || x == GRID_W - 1 || y == 0 || y == GRID_H - 1)
                    {
                        map[gamecore::idx(GRID_W, x, y)] = WALL;
                        pellet[gamecore::idx(GRID_W, x, y)] = false;
                    }
                }

            ghosts = { {13, 14}, {14, 14}, {13, 15}, {14, 15} };
            for (auto [gx, gy] : ghosts)
                pellet[gamecore::idx(GRID_W, gx, gy)] = false;
        }

        bool pellets_remaining() const
        {
            for (size_t i = 0; i < pellet.size(); ++i)
                if (pellet[i]) return true;
            return false;
        }
    };

    inline bool load_assets()
    {
        if (!atlasmanager::create_atlas({
            .name = "pacman_atlas",
            .width = 512,
            .height = 512,
            .generate_mipmaps = false }))
        {
            std::cerr << "[Pacman] Failed to create atlas\n";
            return false;
        }

        auto* registrar = atlasmanager::get_registrar("pacman_atlas");
        if (!registrar)
        {
            std::cerr << "[Pacman] Failed to get atlas registrar\n";
            return false;
        }

        const std::array ids = { "pacman", "ghost", "pellet", "wall" };
        for (auto id : ids)
        {
            auto img = a_loadImage("assets/" + std::string(id) + ".ppm", false);
            if (img.pixels.empty()) {
                std::cerr << "[Pacman] Failed to load image '" << id << "'\n";
                return false;
            }
            if (!registrar->register_atlas_sprites_by_image(id, img.pixels, img.width, img.height, registrar->atlas)) {
                std::cerr << "[Pacman] Failed to register sprite '" << id << "'\n";
                return false;
            }
        }

        registrar->atlas.rebuild_pixels();
        atlasmanager::ensure_uploaded(registrar->atlas);

        return true;
    }

    inline bool run_pacman(std::shared_ptr<core::Context> ctx)
    {
        if (!load_assets())
            return false;

        GameState state;

        auto timer = time::createTimer(0.25);
        time::setScale(timer, 0.25);

        auto& atlasVec = atlasmanager::get_atlas_vector();
        std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

        bool running = true;

        while (running)
        {
            platform::pump_events();
            if (ctx->is_key_down_safe(input::Key::Escape))
                break;

            int nx = state.px, ny = state.py;
            if (ctx->is_key_down_safe(input::Key::Left))  --nx;
            if (ctx->is_key_down_safe(input::Key::Right)) ++nx;
            if (ctx->is_key_down_safe(input::Key::Up))    --ny;
            if (ctx->is_key_down_safe(input::Key::Down))  ++ny;

            if (gamecore::in_bounds(GRID_W, GRID_H, nx, ny) &&
                state.map[gamecore::idx(GRID_W, nx, ny)] != WALL)
            {
                state.px = nx;
                state.py = ny;

                if (state.pellet[gamecore::idx(GRID_W, nx, ny)])
                {
                    state.pellet[gamecore::idx(GRID_W, nx, ny)] = false;
                    if (!state.pellets_remaining())
                        break; // win
                }
            }

            ctx->clear_safe(ctx);

            const float cw = float(ctx->get_width_safe()) / GRID_W;
            const float ch = float(ctx->get_height_safe()) / GRID_H;

            auto draw_grid = [&](std::string_view id, auto pred) {
                auto entryOpt = atlasmanager::registry.get(std::string(id));
                if (!entryOpt) return;

                auto [handle, u0, v0, u1, v1, pivotX, pivotY] = *entryOpt;
                if (!spritepool::is_alive(handle)) return;

                for (int y = 0; y < GRID_H; ++y)
                    for (int x = 0; x < GRID_W; ++x)
                        if (pred(x, y))
                            ctx->draw_sprite_safe(handle, atlasSpan, x * cw, y * ch, cw, ch);
                };

            draw_grid("pellet", [&](int x, int y) { return state.pellet[gamecore::idx(GRID_W, x, y)]; });
            draw_grid("wall", [&](int x, int y) { return state.map[gamecore::idx(GRID_W, x, y)] == WALL; });

            if (auto pacOpt = atlasmanager::registry.get("pacman"); pacOpt && spritepool::is_alive(std::get<0>(*pacOpt)))
                ctx->draw_sprite_safe(std::get<0>(*pacOpt), atlasSpan, state.px * cw, state.py * ch, cw, ch);

            if (auto ghostOpt = atlasmanager::registry.get("ghost"); ghostOpt && spritepool::is_alive(std::get<0>(*ghostOpt)))
                for (auto [gx, gy] : state.ghosts)
                    ctx->draw_sprite_safe(std::get<0>(*ghostOpt), atlasSpan, gx * cw, gy * ch, cw, ch);

            ctx->present_safe();
        }

        return true;
    }
} // namespace almondnamespace::pacman
