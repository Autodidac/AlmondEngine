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
// atexture.hpp
#pragma once

#include "aplatform.hpp"

#include <string>
#include <vector>
#include <cstdint>

namespace almondnamespace
{
    // Represents a raw texture asset (typically RGBA8) before GPU upload
    struct Texture
    {
        std::uint32_t id = 0;  // Optional handle or backend ID
        std::string   name;         // Identifier or atlas entry name

        std::uint32_t width = 0;
        std::uint32_t height = 0;
        std::uint32_t channels = 4; // Usually 4 (RGBA), but supports grayscale, RGB, etc.

        std::vector<std::uint8_t> pixels;

        [[nodiscard]] bool empty() const noexcept {
            return pixels.empty() || width == 0 || height == 0;
        }

        [[nodiscard]] std::size_t size_bytes() const noexcept {
            return pixels.size();
        }

        [[nodiscard]] std::size_t area() const noexcept {
            return static_cast<std::size_t>(width) * height;
        }

        void clear() {
            width = height = channels = 0;
            pixels.clear();
        }
    };
}
