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
 // asnakelike.hpp
#pragma once

#include "aplatform.hpp"
#include "aplatformpump.hpp"
#include "arobusttime.hpp"
#include "acontext.hpp"
#include "agamecore.hpp"
#include "aecs.hpp"
#include "aeventsystem.hpp"
#include "ainput.hpp"
#include "aatlasmanager.hpp"
#include "aopengltextures.hpp"

#include <deque>
#include <random>
#include <stdexcept>
#include <array>
#include <vector>
#include <iostream>
#include <span>

namespace almondnamespace::snake
{
    constexpr int GRID_W = 40;
    constexpr int GRID_H = 30;
    constexpr double STEP_S = 3.0;

    struct Position { int x, y; };

    using almondnamespace::ecs::Entity;
    namespace events = almondnamespace::events;

    inline bool run_snake_ecs(std::shared_ptr<core::Context> ctx)
    {
        using namespace almondnamespace;

        // Load images once and convert to Texture
        const auto headImg = a_loadImage("assets/snake/head.ppm", true);
        const auto bodyImg = a_loadImage("assets/snake/body.ppm", true);
        const auto foodImg = a_loadImage("assets/snake/food.ppm");
        const auto tongueUpImg = a_loadImage("assets/snake/tongue_up.ppm");
        const auto tongueDownImg = a_loadImage("assets/snake/tongue_down.ppm");
        const auto tongueLeftImg = a_loadImage("assets/snake/tongue_left.ppm");
        const auto tongueRightImg = a_loadImage("assets/snake/tongue_right.ppm");

        if (headImg.pixels.empty() || bodyImg.pixels.empty() || foodImg.pixels.empty()
            || tongueUpImg.pixels.empty() || tongueDownImg.pixels.empty()
            || tongueLeftImg.pixels.empty() || tongueRightImg.pixels.empty())
            throw std::runtime_error("Failed to load snake textures");

        Texture headTex{
            .width = static_cast<u32>(headImg.width),
            .height = static_cast<u32>(headImg.height),
            .pixels = headImg.pixels
        };

        Texture bodyTex{
            .width = static_cast<u32>(bodyImg.width),
            .height = static_cast<u32>(bodyImg.height),
            .pixels = bodyImg.pixels
        };

        Texture foodTex{
            .width = static_cast<u32>(foodImg.width),
            .height = static_cast<u32>(foodImg.height),
            .pixels = foodImg.pixels
        };

        Texture tongueUpTex{
            .width = static_cast<u32>(tongueUpImg.width),
            .height = static_cast<u32>(tongueUpImg.height),
            .pixels = tongueUpImg.pixels
        };

        Texture tongueDownTex{
            .width = static_cast<u32>(tongueDownImg.width),
            .height = static_cast<u32>(tongueDownImg.height),
            .pixels = tongueDownImg.pixels
        };

        Texture tongueLeftTex{
            .width = static_cast<u32>(tongueLeftImg.width),
            .height = static_cast<u32>(tongueLeftImg.height),
            .pixels = tongueLeftImg.pixels
        };

        Texture tongueRightTex{
            .width = static_cast<u32>(tongueRightImg.width),
            .height = static_cast<u32>(tongueRightImg.height),
            .pixels = tongueRightImg.pixels
        };

        if (!almondnamespace::atlasmanager::create_atlas({
            .name = "snakeatlas",
            .width = 1024,
            .height = 1024,
            .generate_mipmaps = false }))
        {
            throw std::runtime_error("Failed to create atlas");
        }

        atlasmanager::AtlasRegistrar& registrar = *almondnamespace::atlasmanager::get_registrar("snakeatlas");
        TextureAtlas& atlas = registrar.atlas;

        auto headHandleOpt = registrar.register_atlas_sprites_by_image("snake_head", headTex.pixels, headTex.width, headTex.height, atlas);
        auto bodyHandleOpt = registrar.register_atlas_sprites_by_image("snake_body", bodyTex.pixels, bodyTex.width, bodyTex.height, atlas);
        auto foodHandleOpt = registrar.register_atlas_sprites_by_image("snake_food", foodTex.pixels, foodTex.width, foodTex.height, atlas);

        auto tongueUpHandleOpt = registrar.register_atlas_sprites_by_image("snake_tongue_up", tongueUpTex.pixels, tongueUpTex.width, tongueUpTex.height, atlas);
        auto tongueDownHandleOpt = registrar.register_atlas_sprites_by_image("snake_tongue_down", tongueDownTex.pixels, tongueDownTex.width, tongueDownTex.height, atlas);
        auto tongueLeftHandleOpt = registrar.register_atlas_sprites_by_image("snake_tongue_left", tongueLeftTex.pixels, tongueLeftTex.width, tongueLeftTex.height, atlas);
        auto tongueRightHandleOpt = registrar.register_atlas_sprites_by_image("snake_tongue_right", tongueRightTex.pixels, tongueRightTex.width, tongueRightTex.height, atlas);

        if (!headHandleOpt || !bodyHandleOpt || !foodHandleOpt
            || !tongueUpHandleOpt || !tongueDownHandleOpt || !tongueLeftHandleOpt || !tongueRightHandleOpt)
            throw std::runtime_error("Atlas packing failed or invalid sprite handles");

        SpriteHandle headHandle = *headHandleOpt;
        SpriteHandle bodyHandle = *bodyHandleOpt;
        SpriteHandle foodHandle = *foodHandleOpt;
        SpriteHandle tongueUpHandle = *tongueUpHandleOpt;
        SpriteHandle tongueDownHandle = *tongueDownHandleOpt;
        SpriteHandle tongueLeftHandle = *tongueLeftHandleOpt;
        SpriteHandle tongueRightHandle = *tongueRightHandleOpt;

        // Validate handles
        if (!spritepool::is_alive(headHandle) || !spritepool::is_alive(bodyHandle)
            || !spritepool::is_alive(foodHandle) || !spritepool::is_alive(tongueUpHandle)
            || !spritepool::is_alive(tongueDownHandle) || !spritepool::is_alive(tongueLeftHandle)
            || !spritepool::is_alive(tongueRightHandle))
        {
            throw std::runtime_error("One or more sprite handles invalid");
        }

        atlas.rebuild_pixels();
        atlasmanager::ensure_uploaded(atlas);

        std::deque<Entity> snakeSegments;
        auto world = ecs::make_registry<Position>();

        auto spawn = [&](int gx, int gy) {
            auto e = ecs::create_entity(world);
            ecs::add_component(world, e, Position{ gx, gy });
            snakeSegments.push_back(e);
            };

        spawn(GRID_W / 2, GRID_H / 2);

        auto food = ecs::create_entity(world);
        ecs::add_component(world, food, Position{ 0, 0 });

        auto occupied = gamecore::make_grid<bool>(GRID_W, GRID_H, false);
        auto mark = [&](int x, int y, bool v) { gamecore::at(occupied, GRID_W, GRID_H, x, y) = v; };
        auto isOcc = [&](int x, int y) { return gamecore::at(occupied, GRID_W, GRID_H, x, y); };

        {
            auto& p = ecs::get_component<Position>(world, snakeSegments.back());
            mark(p.x, p.y, true);
        }

        std::mt19937_64 rng{ std::random_device{}() };
        std::uniform_int_distribution<int> dX(0, GRID_W - 1), dY(0, GRID_H - 1);

        auto place_food = [&] {
            int fx, fy;
            do { fx = dX(rng); fy = dY(rng); } while (isOcc(fx, fy));
            ecs::get_component<Position>(world, food) = { fx, fy };
            };
        place_food();

        Position dir{ -1, 0 };
        Position lastdir{ 0, 0 };

        time::Timer timer = time::createTimer(0.25);
        time::setScale(timer, 0.25);

        double acc = 0.0;
        bool game_over = false;

        auto& atlasVec = almondnamespace::atlasmanager::get_atlas_vector();
        std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

        int tongueFrame = 0;
        constexpr int tongueFrameCount = 2;
        double tongueTimer = 0.0;
        constexpr double tongueStep = 0.25; // flicker every 0.25s

        while (!game_over && platform::pump_events())
        {
            input::poll_input();
            events::pump();

            if (ctx->is_key_held(input::Key::Escape)) game_over = true;

            if ((ctx->is_key_held(input::Key::A) || ctx->is_key_down(input::Key::Left)) && lastdir.x != 1)
                dir = { -1, 0 };
            if ((ctx->is_key_down(input::Key::D) || ctx->is_key_down(input::Key::Right)) && lastdir.x != -1)
                dir = { 1, 0 };
            if ((ctx->is_key_down(input::Key::W) || ctx->is_key_down(input::Key::Up)) && lastdir.y != 1)
                dir = { 0, -1 };
            if ((ctx->is_key_down(input::Key::S) || ctx->is_key_down(input::Key::Down)) && lastdir.y != -1)
                dir = { 0, 1 };

            lastdir = dir;

            advance(timer, 0.016);
            acc += time::elapsed(timer);
            tongueTimer += time::elapsed(timer);
            reset(timer);

            // Advance tongue animation frame every tongueStep seconds
            if (tongueTimer >= tongueStep)
            {
                tongueTimer -= tongueStep;
                tongueFrame = (tongueFrame + 1) % tongueFrameCount;
            }

            while (acc >= STEP_S)
            {
                acc -= STEP_S;

                auto headE = snakeSegments.back();
                auto headP = ecs::get_component<Position>(world, headE);

                int nx = (headP.x + dir.x + GRID_W) % GRID_W;
                int ny = (headP.y + dir.y + GRID_H) % GRID_H;

                if (isOcc(nx, ny))
                {
                    game_over = true;
                    break;
                }

                spawn(nx, ny);
                mark(nx, ny, true);

                auto& fp = ecs::get_component<Position>(world, food);

                if (nx == fp.x && ny == fp.y)
                {
                    place_food();
                }
                else
                {
                    auto tailE = snakeSegments.front();
                    auto tailP = ecs::get_component<Position>(world, tailE);
                    mark(tailP.x, tailP.y, false);
                    ecs::destroy_entity(world, tailE);
                    snakeSegments.pop_front();
                }
            }

            ctx->clear_safe(ctx);

            float cw = float(ctx->get_width_safe()) / GRID_W;
            float ch = float(ctx->get_height_safe()) / GRID_H;

            // Draw snake body segments (not including head)
            for (auto it = snakeSegments.begin(); it != snakeSegments.end(); ++it)
            {
                auto& p = ecs::get_component<Position>(world, *it);
                bool isHead = (std::next(it) == snakeSegments.end());
                if (!isHead)
                    ctx->draw_sprite_safe(bodyHandle, atlasSpan, p.x * cw, p.y * ch, cw, ch);
            }

            // Draw head
            {
                auto& p = ecs::get_component<Position>(world, snakeSegments.back());
                ctx->draw_sprite_safe(headHandle, atlasSpan, p.x * cw, p.y * ch, cw, ch);

                // Calculate tongue position (in front of head, based on dir)
                int tx = p.x + dir.x;
                int ty = p.y + dir.y;

                // Wrap grid for tongue pos
                tx = (tx + GRID_W) % GRID_W;
                ty = (ty + GRID_H) % GRID_H;

                // Choose tongue sprite based on direction and current tongue frame
                SpriteHandle tongueHandle;

                switch (dir.x)
                {
                case -1: // left
                    tongueHandle = tongueLeftHandle;
                    break;
                case 1: // right
                    tongueHandle = tongueRightHandle;
                    break;
                default:
                    if (dir.y == -1)
                        tongueHandle = tongueUpHandle;
                    else
                        tongueHandle = tongueDownHandle;
                    break;
                }

                // You can add tongueFrame variation by adjusting UVs or switching sprite versions if you have multiple frames per direction.
                // For now, just flicker the tongue sprite visibility every frame:
                if (tongueFrame == 1)
                    ctx->draw_sprite_safe(tongueHandle, atlasSpan, tx * cw, ty * ch, cw, ch);
            }

            // Draw food
            {
                auto& p = ecs::get_component<Position>(world, food);
                ctx->draw_sprite_safe(foodHandle, atlasSpan, p.x * cw, p.y * ch, cw, ch);
            }

            ctx->present_safe();
        }

        return game_over;
    }
}
