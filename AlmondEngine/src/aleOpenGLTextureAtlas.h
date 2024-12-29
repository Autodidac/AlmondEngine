#pragma once

#include "aleOpenGLTexture.h"
#include "aleImageLoader.h"  // Assuming ImageLoader is defined elsewhere

#ifdef ALMOND_USING_OPENGLTEXTURE

namespace almond {

    class OpenGLTextureAtlas {
    public:
        OpenGLTextureAtlas(int width, int height)
            : atlasWidth(width), atlasHeight(height) {
            glGenTextures(1, &atlasID);
            glBindTexture(GL_TEXTURE_2D, atlasID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);  // Unbind after initialization
        }

        ~OpenGLTextureAtlas() {
            glDeleteTextures(1, &atlasID);
        }

        void AddTexture(const std::string& filepath, int xOffset, int yOffset) {
            auto image = ImageLoader::LoadAlmondImage(filepath);
            if (!image.pixels.data()) {
                throw std::runtime_error("Failed to load image: " + filepath);
            }

            // Debug info
            std::cout << "Atlas Size: " << atlasWidth << "x" << atlasHeight << "\n";
            std::cout << "Adding Texture: " << filepath << " at (" << xOffset << ", " << yOffset << ")\n";
            std::cout << "Texture Size: " << image.width << "x" << image.height << "\n";

            if (xOffset + image.width > atlasWidth || yOffset + image.height > atlasHeight) {
                throw std::runtime_error("Texture exceeds atlas boundaries.");
            }

            glBindTexture(GL_TEXTURE_2D, atlasID);
            glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset,
                image.width, image.height,
                GL_RGBA, GL_UNSIGNED_BYTE,
                image.pixels.data());
            glBindTexture(GL_TEXTURE_2D, 0);
        }


        void Bind() const {
            glBindTexture(GL_TEXTURE_2D, atlasID);
        }

        void Unbind() const {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    private:
        unsigned int atlasID;
        int atlasWidth, atlasHeight;
    };

} // namespace almond
#endif
