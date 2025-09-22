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
 // acellularsim.hpp
#pragma once

#include "acontext.hpp"
#include "agamecore.hpp"
#include "ainput.hpp"
#include "aplatformpump.hpp"
#include "arobusttime.hpp"
#include "aatlasmanager.hpp"
#include "aimageloader.hpp"

#include <random>
#include <iostream>
#include <vector>
#include <span>

namespace almondnamespace::cellular {

    constexpr int W = 80, H = 60;

    inline bool run_cellular(std::shared_ptr<core::Context> ctx)
    {
        using namespace almondnamespace;

        // === Setup Atlas ===
        if (!atlasmanager::create_atlas({
            .name = "cell_atlas",
            .width = 64,
            .height = 64,
            .generate_mipmaps = false }))
        {
            std::cerr << "[Cellular] Failed to create atlas\n";
            return false;
        }

        auto* registrar = atlasmanager::get_registrar("cell_atlas");
        if (!registrar) {
            std::cerr << "[Cellular] Missing registrar\n";
            return false;
        }

        auto img = a_loadImage("assets/cellular/cell.ppm", false);
        if (img.pixels.empty()) {
            std::cerr << "[Cellular] Missing cell.ppm\n";
            return false;
        }

        auto handleOpt = registrar->register_atlas_sprites_by_image(
            "cell", img.pixels, img.width, img.height, registrar->atlas);
        if (!handleOpt) {
            std::cerr << "[Cellular] Failed to register 'cell' sprite\n";
            return false;
        }

        registrar->atlas.rebuild_pixels();
        atlasmanager::ensure_uploaded(registrar->atlas);

        auto& atlasVec = atlasmanager::get_atlas_vector();
        std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

        SpriteHandle handle = *handleOpt;

        // === Simulation Grid ===
        auto grid = gamecore::make_grid<bool>(W, H, false);
        std::mt19937_64 rng{ std::random_device{}() };
        std::bernoulli_distribution d{ 0.2 };
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                grid[gamecore::idx(W, x, y)] = d(rng);

        bool game_over = false;

        while (!game_over)
        {
            platform::pump_events();
            if (ctx->is_key_down_safe(input::Key::Escape)) break;

            // Step simulation
            auto step = [&](auto g) {
                auto next = g;
                for (int y = 0; y < H; ++y) {
                    for (int x = 0; x < W; ++x) {
                        int live = 0;
                        for (auto [nx, ny] : gamecore::neighbors(W, H, x, y))
                            if (g[gamecore::idx(W, nx, ny)]) ++live;
                        bool alive = g[gamecore::idx(W, x, y)];
                        next[gamecore::idx(W, x, y)] = (live == 3) || (alive && live == 2);
                    }
                }
                return next;
                };
            grid = step(grid);

            // Render
            ctx->clear_safe(ctx);

            float cw = float(ctx->get_width_safe()) / W;
            float ch = float(ctx->get_height_safe()) / H;

            for (int y = 0; y < H; ++y) {
                for (int x = 0; x < W; ++x) {
                    if (grid[gamecore::idx(W, x, y)]) {
                        ctx->draw_sprite_safe(handle, atlasSpan,
                            x * cw, y * ch, cw, ch);
                    }
                }
            }

            ctx->present_safe();
        }

        return game_over;
    }

} // namespace almondnamespace::cellular
