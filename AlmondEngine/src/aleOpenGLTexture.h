#pragma once

#include "aleTexture.h"
#include "aleImageLoader.h"
#include "aleEngineConfig.h"

#ifdef ALMOND_USING_OPENGLTEXTURE

#include <stdexcept>
#include <iostream>
#include <memory>

namespace almond {

    class OpenGLTexture : public almond::Texture {
    public:
        OpenGLTexture() {}
        OpenGLTexture(const std::string filepath, Format format, bool generateMipmaps = true) : format(format), generateMipmaps(generateMipmaps) {
            LoadTexture(filepath);
        } 


        ~OpenGLTexture() override {
            if (id != 0) {
                glDeleteTextures(1, &id);
            }
        }

        void Bind(unsigned int slot = 0) const override {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, id);
        }

        void Unbind() const override {
            glBindTexture(GL_TEXTURE_2D, id);
        }

        int GetWidth() const override { return width; }
        int GetHeight() const override { return height; }
        unsigned int GetID() const override { return id; }
        std::string GetPath() const override { return filepath; };

        // Implement SetFiltering to modify texture parameters
        void SetFiltering(GLenum minFilter, GLenum magFilter) const override {
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void checkOpenGLError(const char* location) {
            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cerr << "OpenGL Error at " << location << ": " << err << std::endl;
            }
        }

        std::vector<unsigned char> GetData() const override {
            // Ensure the texture is bound to the current context
            glBindTexture(GL_TEXTURE_2D, id);

            // Create a buffer to hold the texture data
            std::vector<unsigned char> data(width * height * 4); // RGBA format, so 4 bytes per pixel

            // Read the pixels from OpenGL into the data buffer
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

            //checkOpenGLError("glGetTexImage");
            
            return data;
        }

        //almond::Texture* Create(const std::string& filepath, almond::Texture::Format format, bool generateMipmaps) const;
       // [[nodiscard]] std::unique_ptr<Texture> Create();

    private:
        GLuint id;
        int width;
        int height;
        Format format;
        bool generateMipmaps;
        ImageLoader::ImageData image;
        const std::string filepath;

        void LoadTexture(const std::string& filepath) {
            // Load the image using your custom image loader
            image = ImageLoader::LoadAlmondImage(filepath);

            // Check if image is loaded correctly
            if (image.pixels.empty()) {
                throw std::runtime_error("Failed to load image: " + filepath);
            }
            else { std::cout << "Image loaded...  " << filepath << "  Width: " << width << ", Height: " << height << "\n"; }

            // After loading the image, flip it vertically
            for (int y = 0; y < height / 2; ++y) {
                for (int x = 0; x < width; ++x) {
                    std::swap(image.pixels[(y * width + x) * 4], image.pixels[((height - 1 - y) * width + x) * 4]);
                    std::swap(image.pixels[(y * width + x) * 4 + 1], image.pixels[((height - 1 - y) * width + x) * 4 + 1]);
                    std::swap(image.pixels[(y * width + x) * 4 + 2], image.pixels[((height - 1 - y) * width + x) * 4 + 2]);
                    std::swap(image.pixels[(y * width + x) * 4 + 3], image.pixels[((height - 1 - y) * width + x) * 4 + 3]);
                }
            }
            std::string widthstr = std::to_string(width);
            std::cout << "Image flipped... id: " << id << " Width: " << widthstr << ", Height: " << height << "\n";

            width = image.width;
            height = image.height;

            // Generate OpenGL texture and configure it
            glGenTextures(1, &id);
            checkOpenGLError("glGenTextures");

            glBindTexture(GL_TEXTURE_2D, id);

            // Set texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Set internal and data formats based on image channels
            GLenum internalFormat = (format == Format::RGBA8) ? GL_RGBA : (format == Format::RGB8) ? GL_RGB : GL_RGBA;
            GLenum dataFormat = (image.channels == 4) ? GL_RGBA : GL_RGB;

            // Upload texture data to GPU
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, image.pixels.data());
            checkOpenGLError("glTexImage2D");

            // Optionally generate mipmaps
            if (generateMipmaps) {
                glGenerateMipmap(GL_TEXTURE_2D);
                checkOpenGLError("glGenerateMipmap");
            }

            // Check for OpenGL errors
            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                throw std::runtime_error("OpenGL error during texture creation: " + std::to_string(error));
            }
            //id++;
        }
    };


/* */ 
} // namespace almond
#endif
