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
 // asfmltextures.hpp
#pragma once

#include "aplatform.hpp"
#include "aengineconfig.hpp"
#include "atypes.hpp"

#if defined(ALMOND_USING_SFML)

#include "aatlasmanager.hpp"
#include "aatlastexture.hpp"
#include "aimageloader.hpp"
#include "atexture.hpp"

#include <atomic>
#include <filesystem>
#include <format>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

//#include <SFML/Graphics.hpp>
//#include <SFML/Graphics/Texture.hpp>

namespace almondnamespace::sfmlcontext
{
    using Handle = uint32_t;

    struct AtlasGPU
    {
        sf::Texture texture{};
        u64 version = static_cast<u64>(-1);  // force mismatch first time
        u32 width = 0;
        u32 height = 0;
    };

    struct TextureAtlasPtrHash {
        size_t operator()(const TextureAtlas* atlas) const noexcept {
            return std::hash<const TextureAtlas*>{}(atlas);
        }
    };

    struct TextureAtlasPtrEqual {
        bool operator()(const TextureAtlas* lhs, const TextureAtlas* rhs) const noexcept {
            return lhs == rhs;
        }
    };

    inline std::unordered_map<const TextureAtlas*, AtlasGPU, TextureAtlasPtrHash, TextureAtlasPtrEqual> sfml_gpu_atlases;

    inline std::atomic_uint8_t s_generation{ 0 };
    inline std::atomic_uint32_t s_dumpSerial{ 0 };

    [[nodiscard]]
    inline Handle make_handle(int atlasIdx, int localIdx) noexcept {
        return (Handle(s_generation.load(std::memory_order_relaxed)) << 24)
            | ((atlasIdx & 0xFFF) << 12)
            | (localIdx & 0xFFF);
    }

    //[[nodiscard]]
    //inline bool is_handle_live(Handle h) noexcept {
    //    return uint8_t(h >> 24) == s_generation.load(std::memory_order_relaxed);
    //}

    [[nodiscard]]
    inline ImageData ensure_rgba(const ImageData& img) {
        const size_t pixelCount = static_cast<size_t>(img.width) * img.height;
        const size_t channels = img.pixels.size() / pixelCount;

        if (channels == 4) return img;

        if (channels != 3)
            throw std::runtime_error("ensure_rgba(): Unsupported channel count: " + std::to_string(channels));

        std::vector<uint8_t> rgba(pixelCount * 4);
        const uint8_t* src = img.pixels.data();
        uint8_t* dst = rgba.data();

        for (size_t i = 0; i < pixelCount; ++i) {
            dst[4 * i + 0] = src[3 * i + 0];
            dst[4 * i + 1] = src[3 * i + 1];
            dst[4 * i + 2] = src[3 * i + 2];
            dst[4 * i + 3] = 255;
        }

        return { std::move(rgba), img.width, img.height, 4 };
    }

    inline std::string make_dump_name(int atlasIdx, std::string_view tag) {
        std::filesystem::create_directories("atlas_dump");
        return std::format("atlas_dump/{}_{}_{}.ppm", tag, atlasIdx, s_dumpSerial.fetch_add(1, std::memory_order_relaxed));
    }

    inline void dump_atlas(const TextureAtlas& atlas, int atlasIdx) {
        std::string filename = make_dump_name(atlasIdx, atlas.name);
        std::ofstream out(filename, std::ios::binary);
        out << "P6\n" << atlas.width << " " << atlas.height << "\n255\n";
        for (size_t i = 0; i < atlas.pixel_data.size(); i += 4) {
            out.put(atlas.pixel_data[i]);
            out.put(atlas.pixel_data[i + 1]);
            out.put(atlas.pixel_data[i + 2]);
        }
        std::cerr << "[Dump] Wrote: " << filename << "\n";
    }

    inline void upload_atlas_to_gpu(const TextureAtlas& atlas) {
        if (atlas.pixel_data.empty()) {
            const_cast<TextureAtlas&>(atlas).rebuild_pixels();
        }

        auto& gpu = sfml_gpu_atlases[&atlas];

        if (gpu.version == atlas.version && gpu.texture.getSize().x > 0) {
            std::cerr << "[SFML] SKIPPING upload for '" << atlas.name
                << "' version = " << atlas.version << "\n";
            return;
        }

        sf::Image image({ atlas.width, atlas.height }, atlas.pixel_data.data());

        if (!gpu.texture.loadFromImage(image)) {
            throw std::runtime_error("[SFML] Failed to load GPU texture from pixel_data for atlas: " + atlas.name);
        }

        gpu.width = atlas.width;
        gpu.height = atlas.height;
        gpu.version = atlas.version;

        std::cerr << "[SFML] Uploaded atlas '" << atlas.name
            << "' (" << gpu.width << "x" << gpu.height << ")\n";
    }


    inline void ensure_uploaded(const TextureAtlas& atlas) {
        auto it = sfml_gpu_atlases.find(&atlas);
        if (it != sfml_gpu_atlases.end()) {
            if (it->second.version == atlas.version && it->second.texture.getSize().x > 0)
                return;
        }
        upload_atlas_to_gpu(atlas);
    }

    inline void clear_gpu_atlases() noexcept {
        sfml_gpu_atlases.clear();
        s_generation.fetch_add(1, std::memory_order_relaxed);
    }

    inline Handle load_atlas(TextureAtlas& atlas, int atlasIndex = 0) {
        upload_atlas_to_gpu(atlas);
        return make_handle(atlasIndex, 0);
    }

    inline Handle atlas_add_texture(TextureAtlas& atlas, const std::string& id, const ImageData& img) {
        auto rgba = ensure_rgba(img);

        Texture texture{
            .width = static_cast<uint32_t>(rgba.width),
            .height = static_cast<uint32_t>(rgba.height),
            .pixels = std::move(rgba.pixels)
        };

        if (!atlas.add_entry(id, texture)) {
            throw std::runtime_error("atlas_add_texture: Failed to add: " + id);
        }

        upload_atlas_to_gpu(atlas);

        int localIdx = static_cast<int>(atlas.entries.size() - 1);
        return make_handle(0, localIdx);
    }


    // ──────────────────────────────────────────────
// Draw a sprite from an atlas
// ──────────────────────────────────────────────
    inline void draw_sprite(SpriteHandle handle, std::span<const TextureAtlas* const> atlases,
        float x, float y, int width, int height) noexcept
    {
        if (!handle.is_valid())
        {
            std::cerr << "[SFML_DrawSprite] Invalid sprite handle.\n";
            return;
        }

        const int atlasIdx = int(handle.atlasIndex);
        const int localIdx = int(handle.localIndex);

        if (atlasIdx < 0 || atlasIdx >= int(atlases.size()))
        {
            std::cerr << "[SFML_DrawSprite] Atlas index out of bounds: " << atlasIdx << '\n';
            return;
        }

        const TextureAtlas* atlas = atlases[atlasIdx];
        if (!atlas)
        {
            std::cerr << "[SFML_DrawSprite] Null atlas pointer at index: " << atlasIdx << '\n';
            return;
        }

        if (localIdx < 0 || localIdx >= int(atlas->entries.size()))
        {
            std::cerr << "[SFML_DrawSprite] Sprite index out of bounds: " << localIdx << '\n';
            return;
        }

        ensure_uploaded(*atlas);

        auto it = sfml_gpu_atlases.find(atlas);
        if (it == sfml_gpu_atlases.end())
        {
            std::cerr << "[SFML_DrawSprite] GPU texture not found for atlas '" << atlas->name << "'\n";
            return;
        }

        const auto& gpu = it->second;
        const auto& entry = atlas->entries[localIdx];

        // SFML 3.x requires constructing sprite with texture — no default ctor
        sf::Sprite sprite(gpu.texture);

        // sf::IntRect doesn't accept unsigned types directly; convert explicitly
        sf::IntRect rect(
            sf::Vector2i(static_cast<int>(entry.region.x), static_cast<int>(entry.region.y)),
            sf::Vector2i(static_cast<int>(entry.region.width), static_cast<int>(entry.region.height))
        );

        sprite.setTextureRect(rect);
        sprite.setPosition(sf::Vector2f(x, y));

        if (width > 0.f && height > 0.f)
        {
            float scaleX = width / float(entry.region.width);
            float scaleY = height / float(entry.region.height);
            sprite.setScale(sf::Vector2f(scaleX, scaleY));
        }

        // Draw to the window — assuming s_state.window exposes sf::RenderWindow directly
        // Replace this with your actual window reference
        if (!s_sfmlstate.window.sfml_window->isOpen())
        {
            std::cerr << "[SFML_DrawSprite] Render window is not open.\n";
            return;
        }
        s_sfmlstate.window.sfml_window->draw(sprite);
        //draw test rectangle for debugging
//sf::RectangleShape test(sf::Vector2f(50, 50));
//test.setFillColor(sf::Color::Red);
//s_state.window.sfml_window->draw(test);

    }
}

#endif // ALMOND_USING_SFML
