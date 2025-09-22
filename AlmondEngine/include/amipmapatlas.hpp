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
 // amipmapatlas.hpp
#pragma once

#include "aplatform.hpp"

#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <memory>

namespace almondnamespace 
{
    struct MipLevel {
        int width;
        int height;
        std::vector<unsigned char> pixels; // RGBA8
    };

    struct MipmapEntry {
        int id;
        std::string name;
        std::vector<MipLevel> levels;
    };

    struct MipmapAtlas {
        struct Full : std::exception {
            const char* what() const noexcept override { return "MipmapAtlas full"; }
        };

        int baseWidth;
        int baseHeight;
        int maxMipLevels;

        std::vector<MipmapEntry> entries;

        explicit MipmapAtlas(int baseW, int baseH)
            : baseWidth(baseW), baseHeight(baseH), maxMipLevels(calculate_mip_levels(baseW, baseH)) {
        }

        static int calculate_mip_levels(int w, int h) {
            return static_cast<int>(std::floor(std::log2(std::max(w, h)))) + 1;
        }

        void clear() noexcept {
            entries.clear();
        }

        int add_mipmap(std::string const& name, std::vector<std::vector<unsigned char>> const& mip_chain, int w, int h) {
            if (static_cast<int>(mip_chain.size()) != maxMipLevels)
                throw std::invalid_argument("Incorrect number of mip levels");

            std::vector<MipLevel> levels;
            int currentW = w;
            int currentH = h;

            for (std::size_t i = 0; i < mip_chain.size(); ++i) {
                levels.push_back({ currentW, currentH, mip_chain[i] });
                currentW = std::max(1, currentW / 2);
                currentH = std::max(1, currentH / 2);
            }

            int id = static_cast<int>(entries.size());
            entries.push_back({ id, name, std::move(levels) });
            return id;
        }

        [[nodiscard]] const MipmapEntry& get(int id) const {
            if (id < 0 || id >= static_cast<int>(entries.size()))
                throw std::out_of_range("Invalid mipmap ID");
            return entries[id];
        }

        [[nodiscard]] std::size_t count() const noexcept {
            return entries.size();
        }

        [[nodiscard]] int mip_levels() const noexcept { return maxMipLevels; }
        [[nodiscard]] int width() const noexcept { return baseWidth; }
        [[nodiscard]] int height() const noexcept { return baseHeight; }
    };

} // namespace almondnamespace
