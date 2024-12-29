#pragma once

//#include "aleExports_DLL.h"

#include <string>
#include <vector>
#include <cstdint>

namespace almond {

    class ImageLoader {
    public:
        struct ImageData {
            int width;
            int height;
            int channels; // Number of color channels (e.g., 3 for RGB, 4 for RGBA)
            std::vector<uint8_t> pixels; // Pixel data in row-major order
        };

        // Loads an image from file and returns its data
        static ImageData LoadAlmondImage(const std::string& filepath);

    private:
        // Helper methods for specific formats
        static ImageData LoadBMP(const std::string& filepath);
        static ImageData LoadPNG(const std::string& filepath);

        // Add support for other formats as needed
    };

} // namespace almond
