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
 // aimagewriter.hpp
#pragma once

#include "aplatform.hpp"

#include <vector>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <algorithm>
#include <iostream>

namespace almondnamespace
{
    inline bool a_writeBMP(const std::filesystem::path& filepath, const std::vector<uint8_t>& pixels, int width, int height, bool flipVertically)
    {
        if (width <= 0 || height <= 0)
            throw std::runtime_error("a_writeBMP: Invalid dimensions");

        const int channels = 4; // expecting RGBA input
        const int rowBytes = width * 3; // BMP stores as BGR, 3 bytes per pixel (no alpha)
        const int padSize = (4 - (rowBytes % 4)) % 4;
        const int stride = rowBytes + padSize;

        std::vector<uint8_t> bmpData(stride * height, 0);

        for (int y = 0; y < height; ++y)
        {
            int srcY = flipVertically ? y : (height - 1 - y);
            const uint8_t* srcRow = pixels.data() + size_t(srcY) * width * channels;
            uint8_t* dstRow = bmpData.data() + size_t(y) * stride;

            for (int x = 0; x < width; ++x)
            {
                dstRow[x * 3 + 0] = srcRow[x * channels + 2]; // B
                dstRow[x * 3 + 1] = srcRow[x * channels + 1]; // G
                dstRow[x * 3 + 2] = srcRow[x * channels + 0]; // R
            }
        }

        // BMP file header (14 bytes)
        uint8_t fileHeader[14] = {
            'B','M',           // Signature
            0,0,0,0,           // File size (to be filled)
            0,0,               // Reserved1
            0,0,               // Reserved2
            54,0,0,0           // Pixel data offset (54 bytes)
        };

        // BMP info header (40 bytes)
        uint8_t infoHeader[40] = {
            40,0,0,0,          // Header size
            0,0,0,0,           // Width (to be filled)
            0,0,0,0,           // Height (to be filled)
            1,0,               // Planes
            24,0,              // Bits per pixel
            0,0,0,0,           // Compression (none)
            0,0,0,0,           // Image size (can be 0 for BI_RGB)
            0,0,0,0,           // X pixels per meter
            0,0,0,0,           // Y pixels per meter
            0,0,0,0,           // Colors used
            0,0,0,0            // Important colors
        };

        uint32_t fileSize = 54 + static_cast<uint32_t>(bmpData.size());
        // Fill file size
        std::memcpy(&fileHeader[2], &fileSize, 4);
        // Fill width
        std::memcpy(&infoHeader[4], &width, 4);
        // Fill height
        std::memcpy(&infoHeader[8], &height, 4);

        std::ofstream out(filepath, std::ios::binary);
        if (!out) return false;

        out.write(reinterpret_cast<const char*>(fileHeader), sizeof(fileHeader));
        out.write(reinterpret_cast<const char*>(infoHeader), sizeof(infoHeader));
        out.write(reinterpret_cast<const char*>(bmpData.data()), bmpData.size());

        return true;
    }

    inline bool a_writeTGA(const std::filesystem::path& filepath, const std::vector<uint8_t>& pixels, int width, int height, bool flipVertically)
    {
        if (width <= 0 || height <= 0)
            throw std::runtime_error("a_writeTGA: Invalid dimensions");

        const int channels = 4; // expecting RGBA input
        std::ofstream out(filepath, std::ios::binary);
        if (!out) return false;

        uint8_t header[18] = { 0 };
        header[2] = 2; // Uncompressed true-color image
        header[12] = static_cast<uint8_t>(width & 0xFF);
        header[13] = static_cast<uint8_t>((width >> 8) & 0xFF);
        header[14] = static_cast<uint8_t>(height & 0xFF);
        header[15] = static_cast<uint8_t>((height >> 8) & 0xFF);
        header[16] = 32; // bits per pixel
        header[17] = 0x20; // top-left origin

        out.write(reinterpret_cast<const char*>(header), sizeof(header));

        for (int y = 0; y < height; ++y)
        {
            int srcY = flipVertically ? (height - 1 - y) : y;
            const uint8_t* srcRow = pixels.data() + size_t(srcY) * width * channels;

            for (int x = 0; x < width; ++x)
            {
                out.put(static_cast<char>(srcRow[x * channels + 2])); // B
                out.put(static_cast<char>(srcRow[x * channels + 1])); // G
                out.put(static_cast<char>(srcRow[x * channels + 0])); // R
                out.put(static_cast<char>(srcRow[x * channels + 3])); // A
            }
        }
        return true;
    }

    inline bool a_writePPM(const std::filesystem::path& filepath, const std::vector<uint8_t>& pixels, int width, int height, bool flipVertically)
    {
        if (width <= 0 || height <= 0)
            throw std::runtime_error("a_writePPM: Invalid dimensions");

        const int channels = 4; // expecting RGBA input
        std::ofstream out(filepath, std::ios::binary);
        if (!out) return false;

        out << "P6\n" << width << " " << height << "\n255\n";

        for (int y = 0; y < height; ++y)
        {
            int srcY = flipVertically ? (height - 1 - y) : y;
            const uint8_t* srcRow = pixels.data() + size_t(srcY) * width * channels;

            for (int x = 0; x < width; ++x)
            {
                out.put(static_cast<char>(srcRow[x * channels + 0])); // R
                out.put(static_cast<char>(srcRow[x * channels + 1])); // G
                out.put(static_cast<char>(srcRow[x * channels + 2])); // B
            }
        }
        return true;
    }

    inline bool a_writeImage(const std::filesystem::path& filepath, const std::vector<uint8_t>& pixels, int width, int height, bool flipVertically = false)
    {
        auto ext = filepath.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".bmp") return a_writeBMP(filepath, pixels, width, height, flipVertically);
        if (ext == ".tga") return a_writeTGA(filepath, pixels, width, height, flipVertically);
        if (ext == ".ppm") return a_writePPM(filepath, pixels, width, height, flipVertically);

        std::cerr << "Unsupported image format for writing: " << ext << "\n";
        return false;
    }
}
