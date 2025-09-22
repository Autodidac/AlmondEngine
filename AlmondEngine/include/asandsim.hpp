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
 // asandsim.hpp
#pragma once

#include "acontext.hpp"
#include "agamecore.hpp"
#include "ainput.hpp"
#include "aplatformpump.hpp"
#include "arobusttime.hpp"
#include "aatlasmanager.hpp"
#include "aimageloader.hpp" // For a_loadImage

#include <chrono>
#include <span>
#include <iostream>

namespace almondnamespace::sandsim
{
    static SpriteRegistry registry;

    constexpr int W = 120, H = 80;
    constexpr double STEP_S = 0.016;

    inline bool run_sand(std::shared_ptr<core::Context> ctx)
    {
        using namespace almondnamespace;

        // Create the atlas
        if (!atlasmanager::create_atlas({
            .name = "sand_atlas",
            .width = 64,
            .height = 64,
            .generate_mipmaps = false }))
        {
            std::cerr << "[SandSim] Failed to create atlas\n";
            return false;
        }

        auto* registrar = atlasmanager::get_registrar("sand_atlas");
        if (!registrar) {
            std::cerr << "[SandSim] Missing atlas registrar\n";
            return false;
        }

        TextureAtlas& atlas = registrar->atlas;

        // Load sand sprite image
        auto img = a_loadImage("assets/sand/sand.ppm", false);
        if (img.pixels.empty()) {
            std::cerr << "[SandSim] Failed to load sand.ppm\n";
            return false;
        }

        // Register sand sprite in atlas and get handle
        auto handleOpt = registrar->register_atlas_sprites_by_image("sand", img.pixels, img.width, img.height, atlas);
        if (!handleOpt) {
            std::cerr << "[SandSim] Failed to register sand sprite\n";
            return false;
        }
        SpriteHandle sandHandle = *handleOpt;

        atlas.rebuild_pixels();
        atlasmanager::ensure_uploaded(atlas);

        // Correct span construction: from vector of atlas pointers
        auto& atlasVec = atlasmanager::get_atlas_vector();
        if (atlasVec.empty()) {
            std::cerr << "[SandSim] Atlas vector empty\n";
            return false;
        }
        std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

        auto grid = gamecore::make_grid<bool>(W, H, false);

        time::Timer time = time::createTimer(0.25);
        time::setScale(time, 0.25);

        double acc = 0;
        bool game_over = false;

        while (!game_over)
        {
            platform::pump_events();
            if (ctx->is_key_down(input::Key::Escape)) break;

            int mx, my;
            ctx->get_mouse_position(mx, my);
            int gx = mx * W / ctx->get_width();
            int gy = my * H / ctx->get_height();

            if (ctx->is_mouse_button_down_safe(input::MouseButton::MouseLeft) && gamecore::in_bounds(W, H, gx, gy)) {
                gamecore::at(grid, W, H, gx, gy) = true;
            }

            advance(time, STEP_S);
            acc += time::elapsed(time);
            reset(time);

            while (acc >= STEP_S)
            {
                acc -= STEP_S;

                auto next = grid;
                for (int y = H - 1; y >= 0; --y) {
                    for (int x = 0; x < W; ++x) {
                        if (gamecore::at(grid, W, H, x, y)) {
                            if (gamecore::is_free(grid, W, H, x, y + 1, false)) {
                                gamecore::at(next, W, H, x, y + 1) = true;
                                gamecore::at(next, W, H, x, y) = false;
                            }
                            else if (gamecore::is_free(grid, W, H, x - 1, y + 1, false)) {
                                gamecore::at(next, W, H, x - 1, y + 1) = true;
                                gamecore::at(next, W, H, x, y) = false;
                            }
                            else if (gamecore::is_free(grid, W, H, x + 1, y + 1, false)) {
                                gamecore::at(next, W, H, x + 1, y + 1) = true;
                                gamecore::at(next, W, H, x, y) = false;
                            }
                        }
                    }
                }
                grid = next;

                ctx->clear_safe(ctx);

                float cw = float(ctx->get_width()) / W;
                float ch = float(ctx->get_height()) / H;

                if (!spritepool::is_alive(sandHandle)) {
                    ctx->present();
                    continue;
                }

                // No need to draw dummy sprite, draw only what you want visible
                for (int y = 0; y < H; ++y)
                {
                    for (int x = 0; x < W; ++x)
                    {
                        if (gamecore::at(grid, W, H, x, y)) {
                            ctx->draw_sprite(sandHandle, atlasSpan,
                                x * cw, y * ch, cw, ch);
                        }
                    }
                }

                ctx->present();
            }
        }

        return game_over;
    }
}
