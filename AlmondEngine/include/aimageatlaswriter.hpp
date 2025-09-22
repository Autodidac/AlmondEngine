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
#pragma once

#include "aplatform.hpp"    // Platform-specific definitions
#include "aimagewriter.hpp"  // Core writeImage functions you already have

#include <filesystem>
#include <string>
#include <vector>

namespace almondnamespace::atlaswriter
{
    /// Writes a full atlas image (RGBA pixels) to a file.
    /// Supports any format your a_writeImage supports by extension.
    inline bool write_atlas_image(
        const std::filesystem::path& filepath,
        const std::vector<uint8_t>& pixels,
        int width,
        int height,
        bool flipVertically = false)
    {
        // Placeholder for atlas-specific logging or validation.
        return almondnamespace::a_writeImage(filepath, pixels, width, height, flipVertically);
    }

    /// Writes a single slice of an atlas.
    inline bool write_atlas_slice(
        const std::filesystem::path& filepath,
        const std::vector<uint8_t>& slicePixels,
        int sliceWidth,
        int sliceHeight,
        bool flipVertically = false)
    {
        // Simple forward to the main write call.
        return write_atlas_image(filepath, slicePixels, sliceWidth, sliceHeight, flipVertically);
    }

    /// Writes atlas slice with auto-generated filename using slice index.
    /// Example filename: "atlasname_slice_0.ppm"
    inline bool write_atlas_slice_with_index(
        const std::filesystem::path& folder,
        const std::string& atlasName,
        size_t sliceIndex,
        const std::vector<uint8_t>& slicePixels,
        int sliceWidth,
        int sliceHeight,
        bool flipVertically = false)
    {
        auto filename = folder / (atlasName + "_slice_" + std::to_string(sliceIndex) + ".ppm");
        return write_atlas_slice(filename, slicePixels, sliceWidth, sliceHeight, flipVertically);
    }
}
