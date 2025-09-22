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
 // asokobangame.hpp
#pragma once

#include "acontext.hpp"
#include "aplatformpump.hpp"
#include "arobusttime.hpp"
#include "ainput.hpp"
#include "agamecore.hpp"
#include "aatlasmanager.hpp"
#include "aimageloader.hpp"

#include <iostream>
#include <optional>
#include <vector>
#include <span>

namespace almondnamespace::sokoban
{
    constexpr int GRID_W = 16;
    constexpr int GRID_H = 12;
    constexpr double STEP_S = 0.16; // 160ms per move

    enum Cell { FLOOR, WALL, GOAL };

    struct State
    {
        almondnamespace::gamecore::grid_t<int> map;
        almondnamespace::gamecore::grid_t<bool> box;
        int px, py;

        State()
            : map(almondnamespace::gamecore::make_grid<int>(GRID_W, GRID_H, FLOOR)),
            box(almondnamespace::gamecore::make_grid<bool>(GRID_W, GRID_H, false)),
            px(1), py(1)
        {
            // Walls around edges
            for (int x = 0; x < GRID_W; ++x) {
                map[x + 0 * GRID_W] = WALL;
                map[x + (GRID_H - 1) * GRID_W] = WALL;
            }
            for (int y = 0; y < GRID_H; ++y) {
                map[0 + y * GRID_W] = WALL;
                map[(GRID_W - 1) + y * GRID_W] = WALL;
            }

            // Sample goal and box positions
            map[7 + 5 * GRID_W] = GOAL;
            box[6 + 5 * GRID_W] = true;

            px = 4;
            py = 5;
        }
    };

    inline bool run_sokoban(std::shared_ptr<core::Context> ctx)
    {
        State s;

        // Load each sprite's image separately
        auto wallImg = almondnamespace::a_loadImage("assets/atestimage.ppm", false);
        auto floorImg = almondnamespace::a_loadImage("assets/default.bmp", false);
        auto goalImg = almondnamespace::a_loadImage("assets/atestimage.ppm", false);
        auto boxImg = almondnamespace::a_loadImage("assets/atestimage.ppm", false);
        auto playerImg = almondnamespace::a_loadImage("assets/yellow.ppm", false);

        // Verify all loaded
        if (wallImg.pixels.empty() || floorImg.pixels.empty() || goalImg.pixels.empty() ||
            boxImg.pixels.empty() || playerImg.pixels.empty())
        {
            std::cerr << "[Sokoban] Failed to load one or more sprite images\n";
            return false;
        }

        // Create atlas for all sprites
        if (!almondnamespace::atlasmanager::create_atlas({
            .name = "sokoban_atlas",
            .width = 512,
            .height = 512,
            .generate_mipmaps = false }))
        {
            std::cerr << "[Sokoban] Failed to create atlas\n";
            return false;
        }

        auto* registrar = almondnamespace::atlasmanager::get_registrar("sokoban_atlas");
        if (!registrar) {
            std::cerr << "[Sokoban] Failed to get atlas registrar\n";
            return false;
        }

        // Register sprites by image pixel data
        auto wallHandleOpt = registrar->register_atlas_sprites_by_image("wall", wallImg.pixels, wallImg.width, wallImg.height, registrar->atlas);
        auto floorHandleOpt = registrar->register_atlas_sprites_by_image("floor", floorImg.pixels, floorImg.width, floorImg.height, registrar->atlas);
        auto goalHandleOpt = registrar->register_atlas_sprites_by_image("goal", goalImg.pixels, goalImg.width, goalImg.height, registrar->atlas);
        auto boxHandleOpt = registrar->register_atlas_sprites_by_image("box", boxImg.pixels, boxImg.width, boxImg.height, registrar->atlas);
        auto playerHandleOpt = registrar->register_atlas_sprites_by_image("player", playerImg.pixels, playerImg.width, playerImg.height, registrar->atlas);

        if (!wallHandleOpt || !floorHandleOpt || !goalHandleOpt || !boxHandleOpt || !playerHandleOpt) {
            std::cerr << "[Sokoban] Failed to register one or more sprites\n";
            return false;
        }

        // Upload atlas pixels and GPU upload
        registrar->atlas.rebuild_pixels();
        almondnamespace::atlasmanager::ensure_uploaded(registrar->atlas);

        SpriteHandle wallH = *wallHandleOpt;
        SpriteHandle floorH = *floorHandleOpt;
        SpriteHandle goalH = *goalHandleOpt;
        SpriteHandle boxH = *boxHandleOpt;
        SpriteHandle playerH = *playerHandleOpt;

        auto& atlasVec = almondnamespace::atlasmanager::get_atlas_vector();
        std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

        time::Timer timer = time::createTimer(0.25);
        double acc = 0.0;
        bool game_over = false;

        while (!game_over)
        {
            platform::pump_events();

            if (ctx->is_key_down_safe(input::Key::Escape)) break;

            int dx = 0, dy = 0;
            if (ctx->is_key_down_safe(input::Key::A) || ctx->is_key_down_safe(input::Key::Left))  dx = -1;
            if (ctx->is_key_down_safe(input::Key::D) || ctx->is_key_down_safe(input::Key::Right)) dx = 1;
            if (ctx->is_key_down_safe(input::Key::W) || ctx->is_key_down_safe(input::Key::Up))    dy = -1;
            if (ctx->is_key_down_safe(input::Key::S) || ctx->is_key_down_safe(input::Key::Down))  dy = 1;

            int nx = s.px + dx;
            int ny = s.py + dy;

            bool blocked = false;
            if (nx < 0 || nx >= GRID_W || ny < 0 || ny >= GRID_H) blocked = true;
            else if (s.map[nx + ny * GRID_W] == WALL) blocked = true;

            if (!blocked && s.box[nx + ny * GRID_W]) {
                int bx = nx + dx;
                int by = ny + dy;

                bool boxBlocked = (bx < 0 || bx >= GRID_W || by < 0 || by >= GRID_H) ||
                    (s.map[bx + by * GRID_W] == WALL) ||
                    s.box[bx + by * GRID_W];

                if (!boxBlocked) {
                    s.box[bx + by * GRID_W] = true;
                    s.box[nx + ny * GRID_W] = false;
                }
                else {
                    blocked = true;
                }
            }

            if (!blocked) {
                s.px = nx;
                s.py = ny;
            }

            advance(timer, 0.016);
            acc += time::elapsed(timer);
            reset(timer);

            while (acc >= STEP_S)
            {
                acc -= STEP_S;
                // No extra per-step logic here yet
            }

            ctx->clear_safe(ctx);

            float cw = float(ctx->get_width_safe()) / GRID_W;
            float ch = float(ctx->get_height_safe()) / GRID_H;

            // Draw tiles
            for (int y = 0; y < GRID_H; ++y) {
                for (int x = 0; x < GRID_W; ++x) {
                    SpriteHandle h;
                    switch (s.map[x + y * GRID_W]) {
                    case FLOOR: h = floorH; break;
                    case WALL:  h = wallH;  break;
                    case GOAL:  h = goalH;  break;
                    default:    h = floorH; break;
                    }
                    ctx->draw_sprite_safe(h, atlasSpan, x * cw, y * ch, cw, ch);

                    if (s.box[x + y * GRID_W]) {
                        ctx->draw_sprite_safe(boxH, atlasSpan, x * cw, y * ch, cw, ch);
                    }
                }
            }

            // Draw player
            ctx->draw_sprite_safe(playerH, atlasSpan, s.px * cw, s.py * ch, cw, ch);

            ctx->present_safe();
        }

        return game_over;
    }
} // namespace almondnamespace::sokoban
