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
 // araylibrenderer.hpp
#pragma once

#include "aengineconfig.hpp"

#if defined(ALMOND_USING_RAYLIB)

#include "araylibtextures.hpp"
#include "aspritehandle.hpp"
#include "aatlastexture.hpp"
#include "araylibstate.hpp"

#include <iostream>
//#undef Rectangle // Avoid conflict with raylib Rectangle

#include <raylib.h> // Ensure this is included after platform-specific headers
namespace almondnamespace::raylibcontext
{
     struct Rect {
            float x;
            float y;
            float width;
            float height;

            // Conversion operator to raylib's Rectangle type  
            operator struct Raylib_Rectangle() const {
                return { x, y, width, height };
            }
        };
    struct RendererContext
    {
        enum class RenderMode {
            SingleTexture,
            TextureAtlas
        };

        RenderMode mode = RenderMode::TextureAtlas;
    };

    inline RendererContext raylib_renderer{};

    inline void begin_frame()
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
    }

    inline void end_frame()
    {
        EndDrawing();
    }

    inline void draw_sprite(SpriteHandle handle, std::span<const TextureAtlas* const> atlases, float x, float y, float width, float height) noexcept
    {
        if (!handle.is_valid()) {
            std::cerr << "[Raylib_DrawSprite] Invalid sprite handle.\n";
            return;
        }

        const int atlasIdx = static_cast<int>(handle.atlasIndex);
        const int localIdx = static_cast<int>(handle.localIndex);

        if (atlasIdx < 0 || atlasIdx >= static_cast<int>(atlases.size())) {
            std::cerr << "[Raylib_DrawSprite] Atlas index out of bounds: " << atlasIdx << '\n';
            return;
        }

        const TextureAtlas* atlas = atlases[atlasIdx];
        if (!atlas) {
            std::cerr << "[Raylib_DrawSprite] Null atlas pointer at index: " << atlasIdx << '\n';
            return;
        }

        if (localIdx < 0 || localIdx >= static_cast<int>(atlas->entries.size())) {
            std::cerr << "[Raylib_DrawSprite] Sprite index out of bounds: " << localIdx << '\n';
            return;
        }

       almondnamespace::raylibtextures::ensure_uploaded(*atlas);

        auto texIt = almondnamespace::raylibtextures::raylib_gpu_atlases.find(atlas);
        if (texIt == almondnamespace::raylibtextures::raylib_gpu_atlases.end() || texIt->second.texture.id == 0) {
            std::cerr << "[Raylib_DrawSprite] Atlas not uploaded or invalid texture ID: " << atlas->name << "\n";
            return;
        }

        const Texture2D& texture = texIt->second.texture;
        const auto& entry = atlas->entries[localIdx];

        Rect srcRect{
            static_cast<float>(entry.region.x),
            static_cast<float>(entry.region.y),
            static_cast<float>(entry.region.width),
            static_cast<float>(entry.region.height)
        };

        // If width/height <= 0, use native sprite size
        float drawWidth = (width > 0.f) ? width : static_cast<float>(entry.region.width);
        float drawHeight = (height > 0.f) ? height : static_cast<float>(entry.region.height);

        Rect destRect{
            x,
            y,
            drawWidth,
            drawHeight
        };

        Vector2 origin{ 0.0f, 0.0f };  // No rotation offset
        float rotation = 0.0f;

        DrawTexturePro(texture, srcRect, destRect, origin, rotation, WHITE);
    }

} // namespace almondnamespace::raylibcontext

#endif // ALMOND_USING_RAYLIB
