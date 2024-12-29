
#include "aleImageLoader.h"

#include <fstream>
#include <stdexcept>
#include <cstring>

namespace almond {

    ImageLoader::ImageData ImageLoader::LoadAlmondImage(const std::string& filepath) {
        if (filepath.ends_with(".bmp")) {
            return LoadBMP(filepath);
        }
        else if (filepath.ends_with(".png")) {
            return LoadPNG(filepath);
        }
        else {
            throw std::runtime_error("Unsupported file format: " + filepath);
        }
    }

    ImageLoader::ImageData ImageLoader::LoadBMP(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open BMP file: " + filepath);
        }

        // BMP Header
        char header[54];
        file.read(header, 54);

        if (std::memcmp(header, "BM", 2) != 0) {
            throw std::runtime_error("Invalid BMP file: " + filepath);
        }

        // Extract image dimensions
        int width = *reinterpret_cast<int*>(&header[18]);
        int height = *reinterpret_cast<int*>(&header[22]);
        int bitsPerPixel = *reinterpret_cast<short*>(&header[28]);

        if (bitsPerPixel != 24 && bitsPerPixel != 32) {
            throw std::runtime_error("Unsupported BMP bit depth: " + filepath);
        }

        int channels = bitsPerPixel / 8;

        // Calculate row padding (BMP files often pad rows to 4-byte boundaries)
        int rowPadding = (4 - (width * channels) % 4) % 4;

        // Read pixel data
        int dataSize = width * height * channels;
        std::vector<uint8_t> pixels(dataSize);
        file.seekg(*reinterpret_cast<int*>(&header[10])); // Pixel data offset

        // BMP stores pixels bottom-to-top; we must handle padding and flip the image
        for (int y = 0; y < height; ++y) {
            file.read(reinterpret_cast<char*>(&pixels[(height - y - 1) * width * channels]), width * channels);
            file.ignore(rowPadding); // Skip padding at the end of the row
        }

        // If the image is 24-bit (RGB), add an alpha channel (fully opaque)
        if (channels == 3) {
            std::vector<uint8_t> rgbaData(width * height * 4);
            for (int i = 0; i < width * height; ++i) {
                rgbaData[i * 4 + 0] = pixels[i * 3 + 0]; // Red
                rgbaData[i * 4 + 1] = pixels[i * 3 + 1]; // Green
                rgbaData[i * 4 + 2] = pixels[i * 3 + 2]; // Blue
                rgbaData[i * 4 + 3] = 255; // Alpha (fully opaque)
            }
            return { width, height, 4, rgbaData };
        }

        // If the image is 32-bit (RGBA), no modification is needed
        return { width, height, 4, pixels };
    }


    ImageLoader::ImageData ImageLoader::LoadPNG(const std::string& filepath) {
        // For simplicity, let's support PNG loading later
        // PNG decoding requires parsing the format (chunks, filtering, etc.)
        throw std::runtime_error("PNG loading not implemented yet.");
    }

} // namespace almond
