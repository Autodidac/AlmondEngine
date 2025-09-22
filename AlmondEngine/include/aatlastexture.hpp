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
 // aatlastexture.hpp
#pragma once

#include "aplatform.hpp"
#include "atexture.hpp"
#include "aimageloader.hpp"

#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

namespace almondnamespace 
{
    using u8 = uint8_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    struct AtlasRegion
    {
        float u1, v1; // normalized top-left UV coordinates (0..1)
        float u2, v2; // normalized bottom-right UV coordinates (0..1)
        u32 x, y;     // pixel top-left in atlas
        u32 width, height; // size in pixels

        // Helper: width and height in UV space
        float uv_width() const { return u2 - u1; }
        float uv_height() const { return v2 - v1; }
    };

    struct AtlasEntry 
    {
        int index = -1; // <-- NEW: unique index in the atlas entries vector
        std::string name;
        AtlasRegion region;
        std::vector<u8> pixels;
        u32 texWidth = 0;
        u32 texHeight = 0;

        AtlasEntry() = default;
        AtlasEntry(int idx, std::string name_, AtlasRegion region_, std::vector<u8> pixels_, u32 w, u32 h)
            : index(idx), name(std::move(name_)), region(region_), pixels(std::move(pixels_)), texWidth(w), texHeight(h) {
        }
    };

    struct AtlasConfig 
    {
        std::string name;
        u32 width = 2048;
        u32 height = 2048;
        bool generate_mipmaps = false;
        int index = 0; // <-- NEW: so you can assign index at creation
    };

    struct TextureAtlas {
        std::string name;
        int index = -1; // <-- NEW: unique atlas index
        u32 width = 0;
        u32 height = 0;
        bool has_mipmaps = false;

        mutable u64 version = 0;
        mutable std::vector<u8> pixel_data;

        std::vector<AtlasEntry> entries;

        static TextureAtlas create(const AtlasConfig& config) 
        {
            TextureAtlas atlas;
            atlas.name = config.name;
            atlas.index = config.index; // assign index
            atlas.width = config.width;
            atlas.height = config.height;
            atlas.has_mipmaps = config.generate_mipmaps;
            atlas.pixel_data.resize(static_cast<size_t>(atlas.width) * atlas.height * 4, 0);
            atlas.occupancy.assign(atlas.height, std::vector<bool>(atlas.width, false));
#if defined(DEBUG_TEXTURE_RENDERING_VERBOSE)
            std::cerr << "[Atlas] Created '" << atlas.name << "' ("
                << atlas.width << "x" << atlas.height
                << ") Index=" << atlas.index << "\n";
#endif
            return atlas;
        }

        int get_index() const noexcept { return index; }

        std::optional<AtlasEntry> add_entry(const std::string& id, const Texture& tex) 
        {
            if (tex.width == 0 || tex.height == 0 || tex.pixels.empty()) {
                std::cerr << "[Atlas] Rejected empty texture '" << id << "'\n";
                return std::nullopt;
            }

            if (lookup.contains(id)) {
                std::cerr << "[Atlas] Duplicate ID: '" << id << "'\n";
                return std::nullopt;
            }

            auto pos = try_pack(tex.width, tex.height);
            if (!pos) {
                std::cerr << "[Atlas] Failed to pack '" << id << "'\n";
                return std::nullopt;
            }

            auto [x, y] = *pos;
            const u32 stride = width * 4;

            //for (u32 row = 0; row < tex.height; ++row) {
            //    u32 flipped_row = tex.height - 1 - row;
            //    u8* dst = pixel_data.data() + ((y + row) * stride) + (x * 4);
            //    const u8* src = tex.pixels.data() + (flipped_row * tex.width * 4);
            //    std::copy_n(src, tex.width * 4, dst);
            //}

            //AtlasRegion region{
            //    .u1 = static_cast<float>(x) / width,
            //    .v1 = static_cast<float>(y + tex.height) / height, // FLIPPED
            //    .u2 = static_cast<float>(x + tex.width) / width,
            //    .v2 = static_cast<float>(y) / height,              // FLIPPED
            //    .x = x,
            //    .y = y,

            //    .width = tex.width,
            //    .height = tex.height
            //};
            for (u32 row = 0; row < tex.height; ++row) {
                u8* dst = pixel_data.data() + ((y + row) * stride) + (x * 4);
                const u8* src = tex.pixels.data() + (row * tex.width * 4);
                std::copy_n(src, tex.width * 4, dst);
            }

            // Flip V coords for OpenGL bottom-left origin
            AtlasRegion region{
                .u1 = static_cast<float>(x) / width,
                .v1 = static_cast<float>(height - (y + tex.height)) / height,  // flipped V start
                .u2 = static_cast<float>(x + tex.width) / width,
                .v2 = static_cast<float>(height - y) / height,                 // flipped V end
                .x = x,
                .y = y,
                .width = tex.width,
                .height = tex.height
            };

            int entryIndex = static_cast<int>(entries.size());
            AtlasEntry entry{ entryIndex, id, region, tex.pixels, tex.width, tex.height };
            entries.push_back(entry);
            lookup.emplace(id, region);
            ++version;
#if defined(DEBUG_TEXTURE_RENDERING_VERBOSE)
            std::cerr << "[Atlas] Added '" << id << "' at (" << x << ", " << y
                << ") EntryIndex=" << entryIndex << "\n";
#endif
            return entry;
        }

		/// Adds a slice entry without new pixel data, just references existing pixels.
        std::optional<AtlasEntry> add_slice_entry(const std::string& id, int x, int y, int w, int h)
        {
            if (w <= 0 || h <= 0) {
                std::cerr << "[Atlas] Invalid slice size for '" << id << "'\n";
                return std::nullopt;
            }

            if (lookup.contains(id)) {
                std::cerr << "[Atlas] Duplicate ID: '" << id << "'\n";
                return std::nullopt;
            }

            // Atlas UVs: flip Y because OpenGL/DirectX texture origins differ.
            AtlasRegion region{
                .u1 = static_cast<float>(x) / static_cast<float>(width),
                .v1 = static_cast<float>(height - (y + h)) / static_cast<float>(height), // top-left origin
                .u2 = static_cast<float>(x + w) / static_cast<float>(width),
                .v2 = static_cast<float>(height - y) / static_cast<float>(height),

                .x = static_cast<u32>(x),
                .y = static_cast<u32>(y),
                .width = static_cast<u32>(w),
                .height = static_cast<u32>(h)
            };

            const int entryIndex = static_cast<int>(entries.size());

            AtlasEntry entry{
                entryIndex,
                id,
                region,
                {}, // no new pixel data — slice only references existing pixels
                static_cast<u32>(w),
                static_cast<u32>(h)
            };

            entries.emplace_back(entry);
            lookup.emplace(id, region);
            ++version;

#if defined(DEBUG_TEXTURE_RENDERING_VERBOSE)
            std::cerr << "[Atlas] Added slice entry '" << id << "' at ("
                << x << ", " << y << ") size [" << w << "x" << h << "] "
                << "EntryIndex=" << entryIndex << "\n";
#endif

            return entry;
        }

        std::optional<AtlasRegion> get_region(const std::string& id) const {
            auto it = lookup.find(id);
            return (it != lookup.end()) ? std::optional{ it->second } : std::nullopt;
        }

        void rebuild_pixels() const {
            const size_t size = static_cast<size_t>(width) * height * 4;
            if (pixel_data.size() != size)
                pixel_data.resize(size);

            std::fill(pixel_data.begin(), pixel_data.end(), 0);
            const size_t stride = static_cast<size_t>(width) * 4;

            for (const auto& entry : entries) {
                for (u32 row = 0; row < entry.texHeight; ++row) {
                    auto dst = pixel_data.data() + ((entry.region.y + row) * stride) + (entry.region.x * 4);
                    auto src = entry.pixels.data() + (row * entry.texWidth * 4);
                    std::copy_n(src, entry.texWidth * 4, dst);
                }
            }

            ++version;
        }

    private:
        std::unordered_map<std::string, AtlasRegion> lookup;
        std::vector<std::vector<bool>> occupancy;

        std::optional<std::pair<u32, u32>> try_pack(u32 w, u32 h) {
            for (u32 y = 0; y + h <= height; ++y) {
                for (u32 x = 0; x + w <= width; ++x) {
                    if (can_place(x, y, w, h)) {
                        mark_used(x, y, w, h);
                        return { std::pair{x, y} };
                    }
                }
            }
            return std::nullopt;
        }

        bool can_place(u32 x, u32 y, u32 w, u32 h) const {
            for (u32 dy = 0; dy < h; ++dy)
                for (u32 dx = 0; dx < w; ++dx)
                    if (occupancy[y + dy][x + dx]) return false;
            return true;
        }

        void mark_used(u32 x, u32 y, u32 w, u32 h) {
            for (u32 dy = 0; dy < h; ++dy)
                for (u32 dx = 0; dx < w; ++dx)
                    occupancy[y + dy][x + dx] = true;
        }
    };

} // namespace almondnamespace
