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
 // aspritehandle.hpp
#pragma once

#undef max

#include "aplatform.hpp"

#include <cstdint>
#include <compare>
#include <limits>

namespace almondnamespace {

    struct SpriteHandle {
        uint32_t id = std::numeric_limits<uint32_t>::max();
        uint32_t generation = 0;   // generation count for recycling validation
        uint32_t atlasIndex = 0;   // which atlas texture a sprite lives in
        uint32_t localIndex = 0;   // which sprite in that atlas

        constexpr SpriteHandle() noexcept = default;

        constexpr SpriteHandle(uint32_t id_, uint32_t generation_) noexcept
            : id(id_), generation(generation_), atlasIndex(0), localIndex(0) {
        }

        constexpr SpriteHandle(uint32_t id_, uint32_t generation_, uint32_t atlasIndex_, uint32_t localIndex_) noexcept
            : id(id_), generation(generation_), atlasIndex(atlasIndex_), localIndex(localIndex_) {
        }

        [[nodiscard]]
        constexpr bool is_valid() const noexcept {
            return id != std::numeric_limits<uint32_t>::max();
        }

        [[nodiscard]]
        constexpr uint64_t pack() const noexcept {
            // Pack all four fields into 64 bits.
            // Layout: [atlasIndex:16][localIndex:16][generation:16][id:16]
            return (static_cast<uint64_t>(atlasIndex & 0xFFFF) << 48)
                | (static_cast<uint64_t>(localIndex & 0xFFFF) << 32)
                | (static_cast<uint64_t>(generation & 0xFFFF) << 16)
                | (static_cast<uint64_t>(id & 0xFFFF));
        }

        static constexpr SpriteHandle invalid() noexcept {
            return {};
        }

        auto operator<=>(const SpriteHandle&) const = default;

        explicit operator uint32_t() const noexcept { return id; }
    };

    struct SpriteHandleHash {
        [[nodiscard]]
        size_t operator()(const SpriteHandle& handle) const noexcept {
            return static_cast<size_t>(handle.pack());
        }
    };


} // namespace almondnamespace
