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
 // afontrenderer.hpp
#pragma once

#include "aplatform.hpp"
// #include "aassetsystem.hpp" // uncomment when asset system is ready
#include "aatlastexture.hpp"
#include "arenderer.hpp"
#include "aui.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace almondnamespace::font
{
    struct UVRect
    {
        ui::vec2 top_left{};
        ui::vec2 bottom_right{};
    };

    struct Glyph
    {
        UVRect uv{};
        ui::vec2 size_px{};     // Glyph pixel size
        ui::vec2 offset_px{};   // Offset from baseline in pixels
        float advance{};        // Cursor advance in pixels after rendering this glyph
    };

    struct FontAsset
    {
        std::string name;
        float size_pt{};

        std::unordered_map<char32_t, Glyph> glyphs;
        AtlasEntry atlas; // lightweight handle to the glyph atlas texture
    };

    class FontRenderer
    {
    public:
        // Load and bake font from TTF file into a glyph atlas
        // Returns false on failure, true on success
        bool load_font(const std::string& name, const std::string& ttf_path, float size_pt)
        {
            FontAsset asset{};
            asset.name = name;
            asset.size_pt = size_pt;

            std::vector<std::pair<char32_t, Glyph>> baked_glyphs{};
            Texture raw_texture{};

            if (!load_and_bake_font(ttf_path, size_pt, baked_glyphs, raw_texture))
                return false; // fail fast

            for (auto& [ch, glyph] : baked_glyphs)
                asset.glyphs.emplace(ch, std::move(glyph));

            // Create or fetch atlas
            static almondnamespace::TextureAtlas atlas = almondnamespace::TextureAtlas::create({
                .name = "font_atlas",
                .width = 1024,
                .height = 1024,
                .generate_mipmaps = false
                });

            auto maybe_entry = atlas.add(name, raw_texture);
            if (!maybe_entry) {
                // packing failed, handle error
                return false;
            }
            asset.atlas = *maybe_entry;

            loaded_fonts_.emplace(name, std::move(asset));
            return true;
        }


        // Render UTF-32 text at pixel coordinates in screen space
        void render_text(const std::string& font_name, const std::u32string& text, ui::vec2 pos_px) const
        {
            auto it = loaded_fonts_.find(font_name);
            if (it == loaded_fonts_.end())
                return; // font not loaded, silently drop text (optionally log warning)

            const FontAsset& font = it->second;
            ui::vec2 cursor = pos_px;

            for (char32_t ch : text)
            {
                auto glyph_it = font.glyphs.find(ch);
                if (glyph_it == font.glyphs.end())
                {
                    // Optional: render fallback glyph or log missing glyph here
                    continue;
                }

                const Glyph& g = glyph_it->second;
                ui::vec2 top_left = cursor + g.offset_px;
                ui::vec2 bottom_right = top_left + g.size_px;

                almondnamespace::Renderer::submit_quad(
                    top_left,
                    bottom_right,
                    font.atlas.name,
                    g.uv.top_left,
                    g.uv.bottom_right
                );

                cursor.x += g.advance;
            }
        }

    private:
        std::unordered_map<std::string, FontAsset> loaded_fonts_{};
    };

} // namespace almond::font
