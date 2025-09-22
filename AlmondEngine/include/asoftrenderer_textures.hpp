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
 // SoftRenderer - Textures
#pragma once

#include "aplatform.hpp"
#include "aengineconfig.hpp"     // <windows.h>, etc.

#include "aatlastexture.hpp"     // TextureAtlas
#include "asoftrenderer_state.hpp" // SoftRendState (framebuffer, width, height)
#include "ainput.hpp"

#include <vector>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <unordered_map>

namespace almondnamespace::anativecontext
{
    // ─── Texture container for software backend ───────────────
    struct Texture
    {
        int width = 0;
        int height = 0;
        std::vector<uint32_t> pixels; // RGBA8

        Texture() = default;
        Texture(int w, int h, uint32_t fill = 0xFFFFFFFF)
            : width(w), height(h), pixels(w* h, fill) {
        }

        uint32_t sample(int x, int y) const
        {
            x = std::clamp(x, 0, width - 1);
            y = std::clamp(y, 0, height - 1);
            return pixels[static_cast<size_t>(y) * width + x];
        }
    };

    using TexturePtr = std::shared_ptr<Texture>;

    inline TexturePtr create_texture(int w, int h, uint32_t fill = 0xFFFFFFFF)
    {
        return std::make_shared<Texture>(w, h, fill);
    }


    // ─── BackendData for Software Renderer ─────────────────────
   // almondnamespace::anativecontext::SoftRendState;
    struct BackendData
    {
        // Map atlas → texture for caching
        std::unordered_map<const TextureAtlas*, TexturePtr> textures;

        // Renderer state (framebuffer, dimensions, etc.)
        almondnamespace::anativecontext::SoftRendState srState;
    };

} // namespace almondnamespace::anativecontext
