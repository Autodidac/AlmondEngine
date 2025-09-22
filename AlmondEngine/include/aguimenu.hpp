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
// aguimenu.hpp
#pragma once

#include "aplatform.hpp"          // must be first
#include "aengineconfig.hpp"
#include "atypes.hpp"

#include "acontext.hpp"
#include "acontextmultiplexer.hpp"
#include "ainput.hpp"
#include "aatlasmanager.hpp"
#include "aimageloader.hpp"
#include "aspriteregistry.hpp"
#include "aspritepool.hpp"

#include <vector>
#include <string>
#include <span>
#include <optional>
#include <tuple>
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>

namespace almondnamespace::menu 
{
    using almondnamespace::SpriteHandle;
    using almondnamespace::spritepool::is_alive;
    using almondnamespace::atlasmanager::registry;

    enum class Choice 
    {
        Snake, Tetris, Pacman, Sokoban,
        Minesweep, Puzzle, Bejeweled, Fourty,
        Sandsim, Cellular, Settings, Exit
    };

    struct SliceEntry 
    {
        std::string name;
        int x, y, width, height;
        SpriteHandle handle{};
    };

    struct SlicePair { SliceEntry normal; SliceEntry hover; };

    struct MenuOverlay 
    {
        std::vector<SlicePair> slicePairs;
        size_t selection = 0;
        bool prevUp = false, prevDown = false, prevLeft = false, prevRight = false, prevEnter = false;
        bool wasMousePressed = false;
        bool initialized = false;

        std::vector<std::pair<int, int>> cachedPositions; // x,y per item
        std::vector<float> colWidths, rowHeights;
        int cachedWidth = -1;
        int cachedHeight = -1;
        int columns = 1;
        int rows = 0;

        static constexpr float LayoutSpacing = 32.f;

        void recompute_layout(std::shared_ptr<core::Context> ctx) {
            const int totalItems = static_cast<int>(slicePairs.size());
            if (totalItems == 0) {
                cachedPositions.clear();
                colWidths.clear();
                rowHeights.clear();
                columns = 1;
                rows = 0;
                return;
            }

            cachedWidth = ctx->get_width_safe();
            cachedHeight = ctx->get_height_safe();

            float maxItemWidth = 0.f;
            float maxItemHeight = 0.f;
            for (const auto& pair : slicePairs) {
                maxItemWidth = std::max(maxItemWidth, float(pair.normal.width));
                maxItemHeight = std::max(maxItemHeight, float(pair.normal.height));
            }

            const float spacing = LayoutSpacing;
            int computedCols = totalItems;
            if (maxItemWidth > 0.f) {
                const float availableWidth = static_cast<float>(std::max(1, cachedWidth));
                const float denom = maxItemWidth + spacing;
                if (denom > 0.f) {
                    computedCols = static_cast<int>(std::floor((availableWidth + spacing) / denom));
                    computedCols = std::clamp(computedCols, 1, totalItems);
                }
            }
            columns = std::max(1, computedCols);
            rows = (totalItems + columns - 1) / columns;

            colWidths.assign(columns, 0.f);
            rowHeights.assign(rows, 0.f);
            for (int idx = 0; idx < totalItems; ++idx) {
                const int row = idx / columns;
                const int col = idx % columns;
                const auto& slice = slicePairs[idx].normal;
                colWidths[col] = std::max(colWidths[col], float(slice.width));
                rowHeights[row] = std::max(rowHeights[row], float(slice.height));
            }

            float totalWidth = spacing * std::max(0, columns - 1);
            for (float w : colWidths) totalWidth += w;
            float totalHeight = spacing * std::max(0, rows - 1);
            for (float h : rowHeights) totalHeight += h;

            const float baseX = std::max(0.f, (static_cast<float>(cachedWidth) - totalWidth) * 0.5f);
            const float baseY = std::max(0.f, (static_cast<float>(cachedHeight) - totalHeight) * 0.5f);

            cachedPositions.resize(totalItems);
            float yPos = baseY;
            for (int r = 0; r < rows; ++r) {
                float xPos = baseX;
                for (int c = 0; c < columns; ++c) {
                    const int idx = r * columns + c;
                    if (idx < totalItems) {
                        cachedPositions[idx] = {
                            static_cast<int>(std::round(xPos)),
                            static_cast<int>(std::round(yPos))
                        };
                    }
                    xPos += colWidths[c] + spacing;
                }
                yPos += rowHeights[r] + spacing;
            }
        }

        void initialize(std::shared_ptr<core::Context> ctx) 
        {
            if (initialized) return;

            spritepool::initialize(128);
            spritepool::reset();

            // Load the menu buttons image
            auto image = a_loadImage("assets/menu/menubuttons.tga");
            if (image.pixels.empty())
                throw std::runtime_error("Failed to load menu button image");

            if (image.width <= 0 || image.height <= 0)
                throw std::runtime_error("Menu button image has invalid dimensions");

            if (image.height % 2 != 0)
                throw std::runtime_error("Menu button image height must be divisible by 2 for normal/hover halves");

            // Create atlas in manager
            if (!atlasmanager::create_atlas({
                .name = "menubuttons",
                .width = static_cast<u32>(image.width),
                .height = static_cast<u32>(image.height),
                .generate_mipmaps = false
                })) throw std::runtime_error("Failed to create atlas in manager");

            // Get registrar for sprite registration
            auto registrar = atlasmanager::get_registrar("menubuttons");
            if (!registrar) throw std::runtime_error("Failed to get registrar");

            // Move pixels into manager atlas
            registrar->atlas.pixel_data = std::move(image.pixels);
            registrar->atlas.width = image.width;
            registrar->atlas.height = image.height;

            // Add atlas to rendering context
            if (ctx->add_atlas_safe(registrar->atlas) == -1)
                throw std::runtime_error("Failed to add atlas to context");

            // Upload to GPU once so it’s ready
            //openglcontext::ensure_uploaded(registrar->atlas);
            //if (ctx->type == core::ContextType::OpenGL && ctx->hdc && ctx->hglrc) {
            //    if (wglMakeCurrent(ctx->hdc, ctx->hglrc)) {
            //        opengltextures::ensure_uploaded(registrar->atlas);
            //        wglMakeCurrent(nullptr, nullptr);
            //    }
            //    else {
            //        std::cerr << "[Menu] Failed to make GL context current\n";
            //    }
            //}

            if (auto* win = ctx->windowData) {
                win->commandQueue.enqueue([atlas = &registrar->atlas]() {
                    opengltextures::ensure_uploaded(*atlas);
                    });
            }

            // Define menu slices based on the new layout
            constexpr int expectedColumns = 4;
            constexpr int expectedRowsPerHalf = 3;
            const int totalButtons = expectedColumns * expectedRowsPerHalf;

            const int halfHeight = image.height / 2;
            if (image.width % expectedColumns != 0)
                throw std::runtime_error("Menu button image width must be divisible by column count");
            if (halfHeight % expectedRowsPerHalf != 0)
                throw std::runtime_error("Menu button image half-height must be divisible by row count");

            const int spriteW = image.width / expectedColumns;
            const int spriteH = halfHeight / expectedRowsPerHalf;

            if (spriteW <= 0 || spriteH <= 0)
                throw std::runtime_error("Derived menu button size is invalid");

            const std::array<std::string_view, totalButtons> buttonNames = {
                "snake", "tetris", "pacman", "sokoban",
                "minesweep", "puzzle", "match3", "2048",
                "sandsim", "automata", "settings", "quit"
            };

            slicePairs.clear();
            slicePairs.reserve(buttonNames.size());

            for (size_t idx = 0; idx < buttonNames.size(); ++idx) {
                const int row = static_cast<int>(idx) / expectedColumns;
                const int col = static_cast<int>(idx) % expectedColumns;

                const int normalX = col * spriteW;
                const int normalY = row * spriteH;
                const int hoverX = normalX;
                const int hoverY = halfHeight + normalY;

                SlicePair pair{
                    .normal = SliceEntry{
                        .name = std::string(buttonNames[idx]) + "_normal",
                        .x = normalX,
                        .y = normalY,
                        .width = spriteW,
                        .height = spriteH,
                        .handle = {}
                    },
                    .hover = SliceEntry{
                        .name = std::string(buttonNames[idx]) + "_hover",
                        .x = hoverX,
                        .y = hoverY,
                        .width = spriteW,
                        .height = spriteH,
                        .handle = {}
                    }
                };

                slicePairs.emplace_back(std::move(pair));
            }

            // Register slices
            std::vector<std::tuple<std::string, int, int, int, int>> sliceRects;
            for (auto& pair : slicePairs) {
                sliceRects.emplace_back(pair.normal.name, pair.normal.x, pair.normal.y, pair.normal.width, pair.normal.height);
                sliceRects.emplace_back(pair.hover.name, pair.hover.x, pair.hover.y, pair.hover.width, pair.hover.height);
            }

            if (!registrar->register_atlas_sprites_by_custom_sizes(sliceRects))
                throw std::runtime_error("Failed to register menu sprite slices");

            // Assign handles from registry
            for (auto& pair : slicePairs) {
                for (auto* slice : { &pair.normal, &pair.hover }) {
                    auto spriteOpt = registry.get(slice->name);
                    if (!spriteOpt) throw std::runtime_error("Missing sprite handle: " + slice->name);
                    slice->handle = std::get<0>(*spriteOpt);
                    if (!is_alive(slice->handle))
                        throw std::runtime_error("Invalid sprite handle after registration: " + slice->name);
                }
            }

            recompute_layout(ctx);

            initialized = true;
            std::cout << "[Menu] Initialized " << slicePairs.size() << " entries\n";
        }

        std::optional<Choice> update_and_draw(std::shared_ptr<core::Context> ctx, core::WindowData* win) 
        {
            if (!initialized) return std::nullopt;

            if (ctx->get_width_safe() != cachedWidth || ctx->get_height_safe() != cachedHeight)
                recompute_layout(ctx);

            input::poll_input();

            int mx = 0, my = 0;
            ctx->get_mouse_position_safe(mx, my);
            bool mouseLeftDown = input::mouseDown.test(input::MouseButton::MouseLeft);
            bool upPressed = input::keyPressed.test(input::Key::Up);
            bool downPressed = input::keyPressed.test(input::Key::Down);
            bool leftPressed = input::keyPressed.test(input::Key::Left);
            bool rightPressed = input::keyPressed.test(input::Key::Right);
            bool enterPressed = input::keyPressed.test(input::Key::Enter);

            const int totalItems = int(slicePairs.size());
            if (totalItems == 0 || cachedPositions.size() != static_cast<size_t>(totalItems))
                return std::nullopt;

            // Hover
            int hover = -1;
            for (int i = 0; i < totalItems; ++i) {
                const auto& slice = slicePairs[i].normal;
                const auto& pos = cachedPositions[i];
                if (mx >= pos.first && mx <= pos.first + slice.width &&
                    my >= pos.second && my <= pos.second + slice.height) {
                    hover = i;
                    break;
                }
            }

            // Keyboard navigation
            const int cols = std::max(1, columns);
            const int rowsLocal = std::max(1, rows);

            if (leftPressed && !prevLeft) selection = (selection == 0) ? totalItems - 1 : selection - 1;
            if (rightPressed && !prevRight) selection = (selection + 1) % totalItems;
            if (upPressed && !prevUp) selection = (selection < cols) ? selection + (rowsLocal - 1) * cols : selection - cols;
            if (downPressed && !prevDown) selection = (selection + cols >= totalItems) ? selection % cols : selection + cols;
            if (!upPressed && !downPressed && !leftPressed && !rightPressed && hover >= 0) selection = hover;

            prevUp = upPressed; prevDown = downPressed;
            prevLeft = leftPressed; prevRight = rightPressed;

            // --- Queue rendering commands ---
            win->commandQueue.enqueue([ctx]() { ctx->clear_safe(ctx); });

            auto& atlasVec = atlasmanager::get_atlas_vector();
            std::span<const TextureAtlas* const> atlasSpan(atlasVec.data(), atlasVec.size());

            const int hoveredIndex = hover;

            for (int i = 0; i < totalItems; ++i) {
                const bool isHighlighted = (i == selection) || (i == hoveredIndex);
                const auto& slice = isHighlighted ? slicePairs[i].hover : slicePairs[i].normal;
                if (!is_alive(slice.handle)) continue;
                const auto& pos = cachedPositions[i];

                win->commandQueue.enqueue([=]() {
                    ctx->draw_sprite_safe(slice.handle, atlasSpan,
                        float(pos.first), float(pos.second),
                        float(slice.width), float(slice.height));
                    });
            }

            win->commandQueue.enqueue([ctx]() { ctx->present_safe(); });

            // --- Input selection ---
            const bool triggeredByEnter = enterPressed && !prevEnter;

            if (hover >= 0 && mouseLeftDown && !wasMousePressed) {
                wasMousePressed = true;
                prevEnter = enterPressed;
                return static_cast<Choice>(hover);
            }
            wasMousePressed = mouseLeftDown;
            if (triggeredByEnter) {
                prevEnter = enterPressed;
                return static_cast<Choice>(selection);
            }

            prevEnter = enterPressed;

            return std::nullopt;
        }

        void cleanup() {
            for (auto& pair : slicePairs) {
                spritepool::free(pair.normal.handle);
                spritepool::free(pair.hover.handle);
            }
            slicePairs.clear();
            cachedPositions.clear();
            colWidths.clear();
            rowHeights.clear();
            cachedWidth = -1;
            cachedHeight = -1;
            columns = 1;
            rows = 0;
            initialized = false;
        }
    };

} // namespace almondnamespace::menu
