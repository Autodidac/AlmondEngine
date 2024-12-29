#pragma once

#include "aleEngineConfig.h"

#include <string>
#include <memory>
#include <vector>  // Include to use std::vector for texture data

namespace almond {

    class Texture {
    public:
        // Supported texture formats
        enum class Format {
            RGBA8,   // 8 bits per channel RGBA
            RGB8,    // 8 bits per channel RGB
            GREY8,   // 8 bits per channel grayscale
            DXT1,
            ASTC,
            ETC2
        };

        virtual ~Texture() = default;

        // Virtual interface for texture operations
        virtual void Bind(unsigned int slot = 0) const = 0;
        virtual void Unbind() const = 0;
#ifdef ALMOND_USING_GLFW

        virtual void SetFiltering(GLenum minFilter, GLenum magFilter) const = 0;
#endif
        // Getters for texture properties
        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
        virtual unsigned int GetID() const = 0;
        virtual std::string GetPath() const = 0;


        // Retrieve raw texture data (to be implemented by the derived class)
        virtual std::vector<unsigned char> GetData() const = 0;
    };

} // namespace almond
