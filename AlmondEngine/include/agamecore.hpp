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
 // agamecore.hpp
#pragma once

#include "acontext.hpp"   // Context & draw_sprite()

#include <vector>
#include <utility>
#include <cassert>

namespace almondnamespace
{
    namespace gamecore
    {
        template <typename T>
        using grid_t = std::vector<T>;

        template <typename T>
        inline grid_t<T> make_grid(std::size_t width, std::size_t height, T default_value) {
            return grid_t<T>(width * height, default_value);
        }

        inline constexpr bool in_bounds(std::size_t w, std::size_t h, std::size_t x, std::size_t y) noexcept {
            return x < w && y < h;
        }

        template <typename T>
        inline decltype(auto) at(grid_t<T>& grid, std::size_t width, std::size_t height, std::size_t x, std::size_t y) {
            assert(in_bounds(width, height, x, y) && "Grid access out of bounds!");
            return grid[y * width + x];
        }

        template <typename T>
        inline decltype(auto) at(const grid_t<T>& grid, std::size_t width, std::size_t height, std::size_t x, std::size_t y) {
            assert(in_bounds(width, height, x, y) && "Grid access out of bounds!");
            return grid[y * width + x];
        }

        inline constexpr std::size_t idx(std::size_t w, std::size_t x, std::size_t y) noexcept {
            return y * w + x;
        }

        inline std::vector<std::pair<std::size_t, std::size_t>>
            neighbors(std::size_t w, std::size_t h, std::size_t x, std::size_t y) noexcept {
            std::vector<std::pair<std::size_t, std::size_t>> n;
            if (y > 0)          n.emplace_back(x, y - 1);
            if (x + 1 < w)      n.emplace_back(x + 1, y);
            if (y + 1 < h)      n.emplace_back(x, y + 1);
            if (x > 0)          n.emplace_back(x - 1, y);
            return n;
        }

        template<typename T>
        inline bool is_free(const grid_t<T>& grid,
            std::size_t w, std::size_t h,
            std::size_t x, std::size_t y,
            T free_tile_value) noexcept
        {
            return in_bounds(w, h, x, y) && grid[idx(w, x, y)] == free_tile_value;
        }

        // --- Shared atlas pointers storage ---
        inline std::vector<const TextureAtlas*> g_atlases;

        // Register an atlas pointer into the shared vector
        inline void register_atlas(const TextureAtlas& atlas) {
            const size_t idx = static_cast<size_t>(atlas.index);
            if (g_atlases.size() <= idx)
                g_atlases.resize(idx + 1, nullptr);
            g_atlases[idx] = &atlas;
        }

        // Retrieve shared atlas pointers for rendering
        inline std::span<const TextureAtlas* const> get_atlases() noexcept {
            return { g_atlases.data(), g_atlases.size() };
        }
    }
}