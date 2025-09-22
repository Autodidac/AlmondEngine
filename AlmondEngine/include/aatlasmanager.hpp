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
// aatlasmanager.hpp
#pragma once

#include "aspritepool.hpp"
#include "aatlastexture.hpp"
#include "aspriteregistry.hpp"
#include "aspritehandle.hpp"

#include <unordered_map>
#include <string>
#include <vector>
#include <tuple>
#include <optional>
#include <iostream>
#include <atomic>

namespace almondnamespace::atlasmanager
{
    using almondnamespace::spritepool::SpriteHandle;
    using almondnamespace::spritepool::allocate;

    using almondnamespace::TextureAtlas;

    inline SpriteRegistry registry;
    inline std::unordered_map<std::string, TextureAtlas> atlas_map;
    inline std::atomic<int> nextAtlasIndex = 0; // unique atlas id allocator

    // --- Global atlas vector for direct atlasIndex lookup ---
    inline std::vector<const TextureAtlas*> atlas_vector;

    struct AtlasRegistrar
    {
        TextureAtlas& atlas;  // Reference to the shared atlas

        explicit AtlasRegistrar(TextureAtlas& atlas_) noexcept
            : atlas(atlas_) {
        }

		// Bulk registration from slices (name, x, y, w, h) when loading from a texture atlas file
        bool register_atlas_sprites_by_custom_sizes(const std::vector<std::tuple<std::string, int, int, int, int>>& sliceRects)
        {
            for (const auto& [name, x, y, w, h] : sliceRects) {
                SpriteHandle handle = allocate();
                if (!handle.is_valid()) {
                    std::cerr << "[AtlasRegistrar] Failed to allocate handle for '" << name << "'\n";
                    return false;
                }

                auto added = atlas.add_slice_entry(name, x, y, w, h);
                if (!added) {
                    std::cerr << "[AtlasRegistrar] Failed to slice '" << name << "' from atlas\n";
                    return false;
                }

                handle.atlasIndex = atlas.get_index();
                handle.localIndex = static_cast<uint32_t>(added->index);

                registry.add(name, handle,
                    added->region.u1,
                    added->region.v1,
                    added->region.u2 - added->region.u1,
                    added->region.v2 - added->region.v1);
            }
            return true;
        }

        //// Bulk registration from slices (name, x, y, w, h)
//bool register_atlas_sprites_by_columns_and_rows(const std::vector<std::tuple<std::string, int, int, int, int>>& sliceRects) {
//    for (const auto& [name, x, y, w, h] : sliceRects) {
//        SpriteHandle handle = allocate();
//        if (!handle.is_valid()) {
//            std::cerr << "[AtlasRegistrar] Failed to allocate handle for '" << name << "'\n";
//            return false;
//        }

//        // Create dummy white texture data (RGBA)
//        Texture dummy
//        {
//            0, // id placeholder
//            name,
//            static_cast<u32>(w),
//            static_cast<u32>(h),
//            4, // channels RGBA
//            std::vector<u8>(size_t(w) * size_t(h) * 4, 255u)
//        };

//        auto added = atlas.add_entry(name, dummy);
//        if (!added) {
//            std::cerr << "[AtlasRegistrar] Failed to add '" << name << "' to atlas\n";
//            return false;
//        }

//        // Now, sync handle's atlasIndex and id to the actual stored sprite indices
//        // Assuming 'added' has an 'index' member for sprite index in atlas
//        handle.atlasIndex = atlas.get_index();  // or whatever your atlas index identifier is
//        handle.id = added->index;

//        // Register sprite with normalized UVs (width/height in UV space)
//        registry.add(name, handle,
//            added->region.u1,
//            added->region.v1,
//            added->region.u2 - added->region.u1,
//            added->region.v2 - added->region.v1);
//    }
//    return true;
//}

//// Bulk registration from slices (name, x, y, w, h)
//bool register_atlas_sprites_by_tile_size(const std::vector<std::tuple<std::string, int, int, int, int>>& sliceRects) {
//    for (const auto& [name, x, y, w, h] : sliceRects) {
//        SpriteHandle handle = allocate();
//        if (!handle.is_valid()) {
//            std::cerr << "[AtlasRegistrar] Failed to allocate handle for '" << name << "'\n";
//            return false;
//        }

//        // Create dummy white texture data (RGBA)
//        Texture dummy
//        {
//            0, // id placeholder
//            name,
//            static_cast<u32>(w),
//            static_cast<u32>(h),
//            4, // channels RGBA
//            std::vector<u8>(size_t(w) * size_t(h) * 4, 255u)
//        };

//        auto added = atlas.add_entry(name, dummy);
//        if (!added) {
//            std::cerr << "[AtlasRegistrar] Failed to add '" << name << "' to atlas\n";
//            return false;
//        }

//        // Now, sync handle's atlasIndex and id to the actual stored sprite indices
//        // Assuming 'added' has an 'index' member for sprite index in atlas
//        handle.atlasIndex = atlas.get_index();  // or whatever your atlas index identifier is
//        handle.id = added->index;

//        // Register sprite with normalized UVs (width/height in UV space)
//        registry.add(name, handle,
//            added->region.u1,
//            added->region.v1,
//            added->region.u2 - added->region.u1,
//            added->region.v2 - added->region.v1);
//    }
//    return true;
//}

        // Single sprite registration into an automatically constructed shared atlas
        std::optional<SpriteHandle> register_atlas_sprites_by_image(const std::string& name, const std::vector<u8>& pixels, u32 width, u32 height, TextureAtlas& sharedAtlas)
        {
            //std::cerr << "SharedAtlas index: " << sharedAtlas.index << "\n";
            //std::cerr << "AtlasMap[name] index: " << atlas_map[name].index << "\n";

            if (auto existing = registry.get(name))
                return std::get<0>(*existing);

            Texture tex{ 0, name, width, height, 4, pixels };
            auto addedOpt = sharedAtlas.add_entry(name, tex);
            if (!addedOpt) {
                std::cerr << "[AtlasRegistrar] Failed to add '" << name << "' to atlas\n";
                return std::nullopt;
            }
            auto& added = *addedOpt;

            int localIndex = -1;
            for (int i = 0; i < static_cast<int>(sharedAtlas.entries.size()); ++i) {
                if (sharedAtlas.entries[i].name == name) {
                    localIndex = i;
                    break;
                }
            }
            if (localIndex < 0) {
                std::cerr << "[AtlasRegistrar] Could not find local sprite index for '" << name << "'\n";
                return std::nullopt;
            }

            auto allocated = allocate();
            if (!allocated.is_valid()) {
                std::cerr << "[AtlasRegistrar] Failed to allocate spritepool handle for '" << name << "'\n";
                return std::nullopt;
            }

            SpriteHandle handle{
                allocated.id,
                allocated.generation,
                static_cast<uint32_t>(sharedAtlas.index),
                static_cast<uint32_t>(localIndex)
            };

            registry.add(name, handle,
                added.region.u1,
                added.region.v1,
                added.region.u2 - added.region.u1,
                added.region.v2 - added.region.v1);

            return handle;
        }
    };

    inline void update_atlas_vector()
    {
        int maxIndex = -1;
        for (const auto& [name, atlas] : atlas_map) {
            std::cerr << "[update_atlas_vector] Atlas '" << name << "' index: " << atlas.index << "\n";
            if (atlas.index > maxIndex)
                maxIndex = atlas.index;
        }

        if (maxIndex >= 0)
        {
            if (atlas_vector.size() <= static_cast<size_t>(maxIndex))
                atlas_vector.resize(static_cast<size_t>(maxIndex) + 1, nullptr);

            for (const auto& [name, atlas] : atlas_map) {
                atlas_vector[atlas.index] = &atlas;
                std::cerr << "[update_atlas_vector] atlas_vector[" << atlas.index << "] assigned for '" << name << "'\n";
            }
        }
    }

	inline std::unordered_map<std::string, AtlasRegistrar> registrar_map; // Registrar map for named atlases

 //   inline int get_next_atlas_index() noexcept
 //   {
 //       return nextAtlasIndex.load(std::memory_order_relaxed);
	//}

    inline bool create_atlas(const AtlasConfig& config)
    {
        AtlasConfig copy = config;
        copy.index = nextAtlasIndex++;  // Assign unique incremental index
        TextureAtlas atlas = TextureAtlas::create(copy);
        std::cerr << "[create_atlas] Created atlas index = " << atlas.index << "\n"; // MUST NOT BE -1

        auto [it, inserted] = atlas_map.emplace(config.name, std::move(atlas));
        if (!inserted) {
            std::cerr << "[create_atlas] Atlas already exists: " << config.name << "\n";
            return false;
        }

        registrar_map.emplace(config.name, AtlasRegistrar{ it->second });

        update_atlas_vector();

        return true;
    }

    inline AtlasRegistrar* get_registrar(const std::string& name)
    {
        auto it = registrar_map.find(name);
        return (it != registrar_map.end()) ? &it->second : nullptr;
    }

    inline const std::vector<const TextureAtlas*>& get_atlas_vector()
    {
        return atlas_vector;
    }

	// Function to set the backend for ensure_uploaded (context dependent texture function)
    inline std::function<void(const TextureAtlas&)> ensure_uploaded_backend;
    inline void ensure_uploaded(const TextureAtlas& atlas) {
        if (ensure_uploaded_backend) {
            ensure_uploaded_backend(atlas); }
        else { 
            throw std::runtime_error("[AtlasManager] No backend set for ensure_uploaded()"); }
    }

} // namespace almondnamespace
