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
// aopenglfreetype.hpp
#pragma once

#include "aplatform.hpp"
// leave this here, avoid auto sorting incorrect order, platform always comes first
#ifdef ALMOND_USING_OPENGL

#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
//#include <glm/glm.hpp> // glm::ivec2, glm::vec2
//#include <gl/GL.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace almondnamespace::openglcontext {

    class FontManager {
    public:
        FontManager(const std::filesystem::path& fontPath, unsigned int fontSize = 48,
            int atlasWidth = 4096, int atlasHeight = 4096, int padding = 2)
            : fontSize(fontSize), atlasWidth(atlasWidth), atlasHeight(atlasHeight), padding(padding) {
            if (!std::filesystem::exists(fontPath)) {
                throw std::runtime_error("Font file does not exist: " + fontPath.string());
            }

            if (FT_Init_FreeType(&ftLibrary)) {
                throw std::runtime_error("Failed to initialize FreeType Library");
            }

            if (FT_New_Face(ftLibrary, fontPath.string().c_str(), 0, &ftFace)) {
                throw std::runtime_error("Failed to load font: " + fontPath.string());
            }

            FT_Set_Pixel_Sizes(ftFace, 0, fontSize);

            initTextureAtlas();
        }

        ~FontManager() noexcept {
            glDeleteTextures(1, &textureAtlas);
            FT_Done_Face(ftFace);
            FT_Done_FreeType(ftLibrary);
        }

        GLuint getTextureAtlas() const noexcept {
            return textureAtlas;
        }

        struct Character {
            GLuint textureID;            // Texture ID
            glm::ivec2 size;             // Size of the glyph in pixels
            glm::ivec2 bearing;          // Offset from baseline to top-left of the glyph
            int advance;                 // Horizontal offset to advance to the next glyph
            glm::vec2 texCoordStart;     // UV coordinates for the start
            glm::vec2 texCoordEnd;       // UV coordinates for the end
        };

        const Character& getCharacter(char c) {
            auto it = characterMap.find(c);
            if (it != characterMap.end()) {
                return it->second;
            }

            // Lazy load character if not already loaded
            loadCharacterToAtlas(c);
            return characterMap.at(c);
        }

    private:
        FT_Library ftLibrary{};
        FT_Face ftFace{};
        unsigned int fontSize;
        GLuint textureAtlas = 0;
        unsigned int atlasWidth;
        unsigned int atlasHeight;
        unsigned int padding;

        unsigned int xOffset = padding;
        unsigned int yOffset = padding;
        int rowHeight = 0;

        std::unordered_map<char, Character> characterMap;

        void initTextureAtlas() {
            glGenTextures(1, &textureAtlas);
            glBindTexture(GL_TEXTURE_2D, textureAtlas);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

            if (glGetError() != GL_NO_ERROR) {
                throw std::runtime_error("OpenGL texture initialization failed");
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void loadCharacterToAtlas(char c) {
            if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER)) {
                std::cerr << "Failed to load character: " << c << std::endl;
                return;
            }

            FT_GlyphSlot glyph = ftFace->glyph;

            if (xOffset + glyph->bitmap.width + padding > atlasWidth) {
                xOffset = padding;
                yOffset += rowHeight + padding;
                rowHeight = 0;

                if (yOffset + glyph->bitmap.rows + padding > atlasHeight) {
                    throw std::runtime_error("Texture atlas size is insufficient for all glyphs.");
                }
            }

            glBindTexture(GL_TEXTURE_2D, textureAtlas);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, glyph->bitmap.width, glyph->bitmap.rows,
                GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

            rowHeight = std::max(rowHeight, static_cast<int>(glyph->bitmap.rows));

            Character character = {
                0,  // textureID is unused here; textureAtlas handles this globally
                glm::ivec2(glyph->bitmap.width, glyph->bitmap.rows),
                glm::ivec2(glyph->bitmap_left, glyph->bitmap_top),
                static_cast<int>(glyph->advance.x >> 6),
                glm::vec2(static_cast<float>(xOffset) / atlasWidth, static_cast<float>(yOffset) / atlasHeight),
                glm::vec2(static_cast<float>(xOffset + glyph->bitmap.width) / atlasWidth,
                          static_cast<float>(yOffset + glyph->bitmap.rows) / atlasHeight)
            };

            characterMap[c] = character;
            xOffset += glyph->bitmap.width + padding;

            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };
}
#endif