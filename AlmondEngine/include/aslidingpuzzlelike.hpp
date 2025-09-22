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
 // aslidingpuzzlelike.hpp
#pragma once

#include "acontext.hpp"
#include "agamecore.hpp"
#include "ainput.hpp"
#include "aplatformpump.hpp"
#include "arobusttime.hpp"
#include "aatlasmanager.hpp"
#include "aspritepool.hpp"
#include "aimageloader.hpp"

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <random>
#include <unordered_map>

namespace almondnamespace::sliding
{
    constexpr int GRID_W = 4;
    constexpr int GRID_H = 4;
    constexpr double MOVE_S = 0.15; // 150 ms per move

    inline bool run_sliding(std::shared_ptr<core::Context> ctx)
    {
        using namespace almondnamespace;

        struct State {
            gamecore::grid_t<int> grid;
            State()
                : grid(gamecore::make_grid<int>(GRID_W, GRID_H, 0))
            {
                std::vector<int> tiles(GRID_W * GRID_H);
                for (int i = 0; i < (int)tiles.size(); ++i)
                    tiles[i] = i;

                std::mt19937 rng(std::random_device{}());
                std::shuffle(tiles.begin(), tiles.end(), rng);

                for (int y = 0; y < GRID_H; ++y)
                    for (int x = 0; x < GRID_W; ++x)
                        grid[y * GRID_W + x] = tiles[y * GRID_W + x];
            }
        } s;

        auto tileImg = a_loadImage("assets/atestimage.ppm", false);
        if (tileImg.pixels.empty())
        {
            std::cerr << "[Sliding] Failed to load tile image\n";
            return false;
        }

        if (!atlasmanager::create_atlas({
            .name = "sliding_atlas",
            .width = 512,
            .height = 512,
            .generate_mipmaps = false }))
        {
            std::cerr << "[Sliding] Failed to create atlas\n";
            return false;
        }

        auto* registrar = atlasmanager::get_registrar("sliding_atlas");
        if (!registrar)
        {
            std::cerr << "[Sliding] Missing atlas registrar\n";
            return false;
        }

        TextureAtlas& atlas = registrar->atlas;

        int tileWidth = tileImg.width / GRID_W;
        int tileHeight = tileImg.height / GRID_H;

        std::unordered_map<int, SpriteHandle> tiles;

        for (int tileId = 1; tileId < GRID_W * GRID_H; ++tileId)
        {
            int tx = tileId % GRID_W;
            int ty = tileId / GRID_W;

            std::vector<unsigned char> tilePixels(tileWidth * tileHeight * 4);

            for (int y = 0; y < tileHeight; ++y)
            {
                int srcY = ty * tileHeight + y;
                for (int x = 0; x < tileWidth; ++x)
                {
                    int srcX = tx * tileWidth + x;
                    int srcIdx = (srcY * tileImg.width + srcX) * 4;
                    int dstIdx = (y * tileWidth + x) * 4;
                    for (int c = 0; c < 4; ++c)
                        tilePixels[dstIdx + c] = tileImg.pixels[srcIdx + c];
                }
            }

            auto handleOpt = registrar->register_atlas_sprites_by_image(
                "tile" + std::to_string(tileId),
                tilePixels,
                tileWidth,
                tileHeight,
                atlas);

            if (!handleOpt)
            {
                std::cerr << "[Sliding] Failed to register tile " << tileId << "\n";
                return false;
            }

            tiles[tileId] = *handleOpt;
        }

        atlas.rebuild_pixels();
        atlasmanager::ensure_uploaded(atlas);

        auto& atlasVec = atlasmanager::get_atlas_vector();
        std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

        time::Timer timer = time::createTimer(0.25);
        time::setScale(timer, 0.25);
        double acc = 0.0;
        bool movedThisFrame = false;
        bool game_over = false;

        while (!game_over)
        {
            platform::pump_events();

            if (ctx->is_key_down(input::Key::Escape))
                break;

            int dx = 0, dy = 0;
            if (ctx->is_key_down(input::Key::Left))  dx = -1;
            else if (ctx->is_key_down(input::Key::Right)) dx = 1;
            else if (ctx->is_key_down(input::Key::Up))    dy = -1;
            else if (ctx->is_key_down(input::Key::Down))  dy = 1;

            if (!movedThisFrame && (dx != 0 || dy != 0))
            {
                int emptyX = -1, emptyY = -1;
                for (int y = 0; y < GRID_H; ++y)
                {
                    for (int x = 0; x < GRID_W; ++x)
                    {
                        if (s.grid[y * GRID_W + x] == 0)
                        {
                            emptyX = x;
                            emptyY = y;
                            break;
                        }
                    }
                    if (emptyX != -1) break;
                }

                int fromX = emptyX + dx;
                int fromY = emptyY + dy;

                if (fromX >= 0 && fromX < GRID_W && fromY >= 0 && fromY < GRID_H)
                {
                    std::swap(s.grid[emptyY * GRID_W + emptyX], s.grid[fromY * GRID_W + fromX]);
                    movedThisFrame = true;
                }
            }

            advance(timer, 0.016);
            acc += time::elapsed(timer);
            reset(timer);

            if (acc >= MOVE_S)
            {
                acc -= MOVE_S;
                movedThisFrame = false;
            }

            ctx->clear();

            float cw = float(ctx->get_width()) / GRID_W;
            float ch = float(ctx->get_height()) / GRID_H;

            for (int y = 0; y < GRID_H; ++y)
                for (int x = 0; x < GRID_W; ++x)
                {
                    int tileId = s.grid[y * GRID_W + x];
                    if (tileId == 0) continue;

                    auto it = tiles.find(tileId);
                    if (it != tiles.end())
                    {
                        ctx->draw_sprite(it->second, atlasSpan,
                            x * cw, y * ch, cw, ch);
                    }
                }

            ctx->present();
        }

        return game_over;
    }
} // namespace almondnamespace::sliding
