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
 // atetrislike.hpp
#pragma once

#include "aplatformpump.hpp"
#include "arobusttime.hpp"
#include "acontext.hpp"
#include "ainput.hpp"
#include "agamecore.hpp"
#include "aatlasmanager.hpp"
#include "aopengltextures.hpp"
#include "aspritepool.hpp"

#include <array>
#include <random>
#include <stdexcept>

namespace almondnamespace::tetris
{
    using almondnamespace::SpriteHandle;

    static SpriteRegistry registry;

    constexpr int GRID_W = 40;
    constexpr int GRID_H = 20;
    constexpr double STEP_S = 30.0;

    constexpr std::array<std::array<std::array<int, 16>, 4>, 7> TETRAMINOS = { {
            // I
            {{{0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0},
              {0,0,0,0, 1,1,1,1, 0,0,0,0, 0,0,0,0},
              {0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0},
              {0,0,0,0, 0,0,0,0, 1,1,1,1, 0,0,0,0}}},
              // O
              {{{0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0},
                {0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0},
                {0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0},
                {0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0}}},
                // T
                {{{0,0,0,0, 1,1,1,0, 0,1,0,0, 0,0,0,0},
                  {0,0,1,0, 0,1,1,0, 0,0,1,0, 0,0,0,0},
                  {0,0,0,0, 0,1,0,0, 1,1,1,0, 0,0,0,0},
                  {0,1,0,0, 1,1,0,0, 0,1,0,0, 0,0,0,0}}},
                  // S
                  {{{0,0,0,0, 0,1,1,0, 1,1,0,0, 0,0,0,0},
                    {0,1,0,0, 0,1,1,0, 0,0,1,0, 0,0,0,0},
                    {0,0,0,0, 0,1,1,0, 1,1,0,0, 0,0,0,0},
                    {0,1,0,0, 0,1,1,0, 0,0,1,0, 0,0,0,0}}},
                    // Z
                    {{{0,0,0,0, 1,1,0,0, 0,1,1,0, 0,0,0,0},
                      {0,0,1,0, 0,1,1,0, 0,1,0,0, 0,0,0,0},
                      {0,0,0,0, 1,1,0,0, 0,1,1,0, 0,0,0,0},
                      {0,0,1,0, 0,1,1,0, 0,1,0,0, 0,0,0,0}}},
                      // J
                      {{{0,0,0,0, 1,0,0,0, 1,1,1,0, 0,0,0,0},
                        {0,0,1,1, 0,0,1,0, 0,0,1,0, 0,0,0,0},
                        {0,0,0,0, 1,1,1,0, 0,0,1,0, 0,0,0,0},
                        {0,0,1,0, 0,0,1,0, 0,1,1,0, 0,0,0,0}}},
                        // L
                        {{{0,0,0,0, 0,0,1,0, 1,1,1,0, 0,0,0,0},
                          {0,0,1,0, 0,0,1,0, 0,0,1,1, 0,0,0,0},
                          {0,0,0,0, 1,1,1,0, 1,0,0,0, 0,0,0,0},
                          {0,1,1,0, 0,0,1,0, 0,0,1,0, 0,0,0,0}}}
                    } };

    struct State
    {
        gamecore::grid_t<int> grid;
        int shape, rot;
        int px, py;
        std::mt19937_64 rng;
        std::uniform_int_distribution<int> dist;
        double accumulator = 0.0;

        State()
            : grid(gamecore::make_grid<int>(GRID_W, GRID_H, 0))
            , shape(0), rot(0), px(GRID_W / 2 - 2), py(0)
            , rng{ std::random_device{}() }
            , dist(0, 6)
        {
            shape = dist(rng);
        }
    };

    inline bool collides(const State& s, int nx, int ny, int r)
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
            {
                int v = TETRAMINOS[s.shape][r][i * 4 + j];
                if (!v) continue;

                int gx = nx + j;
                int gy = ny + i;
                if (!gamecore::in_bounds(GRID_W, GRID_H, gx, gy) || gamecore::at(s.grid, GRID_W, GRID_H, gx, gy))
                    return true;
            }
        return false;
    }

    inline void place(State& s)
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
            {
                if (TETRAMINOS[s.shape][s.rot][i * 4 + j])
                {
                    int gx = s.px + j;
                    int gy = s.py + i;
                    if (gamecore::in_bounds(GRID_W, GRID_H, gx, gy))
                        gamecore::at(s.grid, GRID_W, GRID_H, gx, gy) = s.shape + 1;
                }
            }
    }

    inline void clear_lines(State& s)
    {
        for (int y = GRID_H - 1; y >= 0; --y)
        {
            bool full = true;
            for (int x = 0; x < GRID_W; ++x)
            {
                if (!gamecore::at(s.grid, GRID_W, GRID_H, x, y))
                {
                    full = false;
                    break;
                }
            }
            if (full)
            {
                for (int ny = y; ny > 0; --ny)
                    for (int x = 0; x < GRID_W; ++x)
                        gamecore::at(s.grid, GRID_W, GRID_H, x, ny) = gamecore::at(s.grid, GRID_W, GRID_H, x, ny - 1);

                for (int x = 0; x < GRID_W; ++x)
                    gamecore::at(s.grid, GRID_W, GRID_H, x, 0) = 0;

                ++y; // re-check line since lines shifted down
            }
        }
    }

    inline bool setup_sprites()
    {
        // Load texture for Tetris blocks
        const auto blockImg = almondnamespace::a_loadImage("assets/atestimage.ppm", true);
        if (blockImg.pixels.empty())
            return false;

        Texture blockTex{
            .width = static_cast<u32>(blockImg.width),
            .height = static_cast<u32>(blockImg.height),
            .pixels = blockImg.pixels
        };

        if (!almondnamespace::atlasmanager::create_atlas({
            .name = "tetrisatlas",
            .width = 1024,
            .height = 1024,
            .generate_mipmaps = false
            }))
            return false;

        auto& registrar = *almondnamespace::atlasmanager::get_registrar("tetrisatlas");
        TextureAtlas& atlas = registrar.atlas;

        auto handleOpt = registrar.register_atlas_sprites_by_image("tetris_block", blockTex.pixels, blockTex.width, blockTex.height, atlas);
        if (!handleOpt)
            return false;

        SpriteHandle handle = *handleOpt;
        if (!spritepool::is_alive(handle))
            return false;

        atlas.rebuild_pixels();
        atlasmanager::ensure_uploaded(atlas);

        // Register sprite handle in registry
        registry.add("tetris_block", handle, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f);
        registry.atlas_ptr = &atlas;

        return true;
    }

    inline bool run_tetris(std::shared_ptr<core::Context> ctx)
    {
        if (!setup_sprites())
            throw std::runtime_error("Failed to setup Tetris sprites");

        State s;

        time::Timer timer = time::createTimer(0.25);
        time::setScale(timer, 0.25);

        double acc = 0;
        bool game_over = false;

        while (!game_over)
        {
            platform::pump_events();

            if (ctx->is_key_down_safe(input::Key::Escape))
                break;

            if (ctx->is_key_down_safe(input::Key::Left) && !collides(s, s.px - 1, s.py, s.rot))
                --s.px;

            if (ctx->is_key_down_safe(input::Key::Right) && !collides(s, s.px + 1, s.py, s.rot))
                ++s.px;

            if (ctx->is_key_down_safe(input::Key::Up))
                s.rot = (s.rot + 1) % 4;

            if (ctx->is_key_down_safe(input::Key::Down) && !collides(s, s.px, s.py + 1, s.rot))
                ++s.py;

            advance(timer, 0.016);
            acc += time::elapsed(timer);
            reset(timer);

            while (acc >= STEP_S)
            {
                acc -= STEP_S;

                if (!collides(s, s.px, s.py + 1, s.rot))
                {
                    ++s.py;
                }
                else
                {
                    place(s);
                    clear_lines(s);
                    s.shape = s.dist(s.rng);
                    s.rot = 0;
                    s.px = GRID_W / 2 - 2;
                    s.py = 0;

                    if (collides(s, s.px, s.py, s.rot))
                        return false; // game over
                }
            }

            ctx->clear_safe(ctx);

            float cw = float(ctx->get_width_safe()) / GRID_W;
            float ch = float(ctx->get_height_safe()) / GRID_H;

            auto entry = registry.get("tetris_block");
            if (entry && registry.atlas_ptr)
            {
                auto& [handle, u0, v0, u1, v1, px, py] = *entry;

                if (spritepool::is_alive(handle))
                {
                    // Create stable atlas span once
                    auto& atlasVec = almondnamespace::atlasmanager::get_atlas_vector();
                    std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

                    // Draw placed blocks (no Y flip)
                    for (int y = 0; y < GRID_H; ++y)
                    {
                        float py = y * ch;  // Direct mapping, no flipping
                        for (int x = 0; x < GRID_W; ++x)
                        {
                            if (gamecore::at(s.grid, GRID_W, GRID_H, x, y))
                            {
                                ctx->draw_sprite_safe(handle, atlasSpan, x * cw, py, cw, ch);
                            }
                        }
                    }

                    // Draw current falling tetramino (no Y flip)
                    for (int i = 0; i < 4; ++i)
                    {
                        for (int j = 0; j < 4; ++j)
                        {
                            if (TETRAMINOS[s.shape][s.rot][i * 4 + j])
                            {
                                float dx = (s.px + j) * cw;
                                float dy = (s.py + i) * ch;  // No flipping here either
                                ctx->draw_sprite_safe(handle, atlasSpan, dx, dy, cw, ch);
                            }
                        }
                    }

                }
            }
            ctx->present_safe();
        }
        return game_over;
    }
} // namespace almondnamespace::tetris
