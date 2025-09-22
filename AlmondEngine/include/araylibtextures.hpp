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
 // araylibtextures.hpp
#pragma once

#include "aplatform.hpp"
#include "aengineconfig.hpp"
#include "atypes.hpp"

#if defined(ALMOND_USING_RAYLIB)

#include "araylibcontext.hpp"
#include "araylibstate.hpp"
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

#include <raylib.h>

namespace almondnamespace::raylibtextures
{
    using Handle = uint32_t;

    struct AtlasGPU
    {
        Texture2D texture{};
        u64 version = static_cast<u64>(-1);
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

    inline std::unordered_map<const TextureAtlas*, AtlasGPU, TextureAtlasPtrHash, TextureAtlasPtrEqual> raylib_gpu_atlases;

    // forward declare OpenGL4State to avoid pulling in aopenglcontext here
    namespace raylibcontext { struct RaylibState; }
    struct BackendData {
        std::unordered_map<const TextureAtlas*, AtlasGPU,
            TextureAtlasPtrHash, TextureAtlasPtrEqual> gpu_atlases;
        almondnamespace::raylibcontext::RaylibState rlState{};
    };

    inline BackendData& get_raylib_backend() {
        auto& backend = almondnamespace::core::g_backends[almondnamespace::core::ContextType::RayLib];
        if (!backend.data) {
            backend.data = {
                new BackendData(),
                [](void* p) { delete static_cast<BackendData*>(p); }
            };
        }
        return *static_cast<BackendData*>(backend.data.get());
    }

    inline std::atomic_uint8_t s_generation{ 0 };
    inline std::atomic_uint32_t s_dumpSerial{ 0 };

    [[nodiscard]]
    inline Handle make_handle(int atlasIdx, int localIdx) noexcept {
        return (Handle(s_generation.load(std::memory_order_relaxed)) << 24)
            | ((atlasIdx & 0xFFF) << 12)
            | (localIdx & 0xFFF);
    }

    [[nodiscard]]
    inline bool is_handle_live(Handle h) noexcept {
        return uint8_t(h >> 24) == s_generation.load(std::memory_order_relaxed);
    }

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

        auto& gpu = raylib_gpu_atlases[&atlas];

        if (gpu.version == atlas.version && gpu.texture.id != 0) {
            std::cerr << "[Raylib] SKIPPING upload for '" << atlas.name << "' version = " << atlas.version << "\n";
            return;
        }

        if (gpu.texture.id != 0) {
            UnloadTexture(gpu.texture);
        }

        Image img{};
        img.data = const_cast<unsigned char*>(atlas.pixel_data.data());
        img.width = atlas.width;
        img.height = atlas.height;
        img.mipmaps = 1;
        img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

        gpu.texture = LoadTextureFromImage(img);
        gpu.version = atlas.version;
        gpu.width = atlas.width;
        gpu.height = atlas.height;

        dump_atlas(atlas, atlas.index);

        std::cerr << "[Raylib] Uploaded atlas '" << atlas.name << "' (tex id " << gpu.texture.id << ")\n";
    }

    inline void ensure_uploaded(const TextureAtlas& atlas) {
        auto it = raylib_gpu_atlases.find(&atlas);
        if (it != raylib_gpu_atlases.end()) {
            if (it->second.version == atlas.version && it->second.texture.id != 0)
                return;
        }
        upload_atlas_to_gpu(atlas);
    }

    inline void clear_gpu_atlases() noexcept {
        for (auto& [_, gpu] : raylib_gpu_atlases) {
            if (gpu.texture.id != 0) {
                UnloadTexture(gpu.texture);
            }
        }
        raylib_gpu_atlases.clear();
        s_generation.fetch_add(1, std::memory_order_relaxed);
    }

    inline Handle load_atlas(TextureAtlas& atlas, int atlasIndex = 0) {
        upload_atlas_to_gpu(atlas);
        return make_handle(atlasIndex, 0);
    }

    inline Handle atlas_add_texture(TextureAtlas& atlas, const std::string& id, const ImageData& img)
    {
        auto rgba = ensure_rgba(img);

        Texture texture{
            .width = static_cast<uint32_t>(rgba.width),
            .height = static_cast<uint32_t>(rgba.height),
            .pixels = std::move(rgba.pixels)
        };

        if (!atlas.add_entry(id, texture)) {
            throw std::runtime_error("atlas_add_texture: Failed to add texture: " + id);
        }

        upload_atlas_to_gpu(atlas);

        int localIdx = static_cast<int>(atlas.entries.size() - 1);
        return make_handle(0, localIdx);
    }

} // namespace almondnamespace::raylibcontext

#endif // ALMOND_USING_RAYLIB
