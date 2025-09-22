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

#include "aplatform.hpp"

#undef max

#include <vector>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <algorithm>
#include <iostream>
#include <limits>

namespace almondnamespace
{
    struct ImageData
    {
        std::vector<uint8_t> pixels;
        int width = 0;
        int height = 0;
        int channels = 0;

        ImageData(std::vector<uint8_t> p, int w, int h, int c)
            : pixels(std::move(p)), width(w), height(h), channels(c) {
        }
    };

    inline void a_listSupportedImageTypes()
    {
        std::cout << "Supported image types: BMP, TGA, PPM\n";
    }

    inline ImageData a_loadBMP(const std::filesystem::path& path, bool flipVertically);
    inline ImageData a_loadTGA(const std::filesystem::path& path, bool flipVertically);
    inline ImageData a_loadPPM(const std::filesystem::path& path, bool flipVertically);

    inline ImageData a_loadImage(const std::filesystem::path& filepath, bool flipVertically = false)
    {
        auto ext = filepath.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".bmp") return a_loadBMP(filepath, flipVertically);
        if (ext == ".tga") return a_loadTGA(filepath, flipVertically);
        if (ext == ".ppm") return a_loadPPM(filepath, flipVertically);

        throw std::runtime_error("Unsupported image format: " + filepath.string());
    }

    inline ImageData a_loadBMP(const std::filesystem::path& filepath, bool flipVertically)
    {
        std::ifstream f(filepath, std::ios::binary);
        if (!f) throw std::runtime_error("Cannot open BMP: " + filepath.string());

        char hdr[54];
        f.read(hdr, 54);
        if (std::memcmp(hdr, "BM", 2) != 0)
            throw std::runtime_error("Invalid BMP header: " + filepath.string());

        int w = *reinterpret_cast<int*>(&hdr[18]);
        int h = *reinterpret_cast<int*>(&hdr[22]);
        int bpp = *reinterpret_cast<short*>(&hdr[28]);
        int ch = bpp / 8;
        int pad = (4 - (w * ch) % 4) % 4;

        std::vector<uint8_t> raw(size_t(w) * size_t(h) * ch);

        f.seekg(*reinterpret_cast<int*>(&hdr[10]));
        for (int y = 0; y < h; ++y) {
            f.read(reinterpret_cast<char*>(raw.data() + y * w * ch), w * ch);
            f.ignore(pad);
        }

        std::vector<uint8_t> out(size_t(w) * size_t(h) * 4);
        for (int y = 0; y < h; ++y) {
            int srcY = flipVertically ? (h - 1 - y) : y;
            for (int x = 0; x < w; ++x) {
                int srcIdx = (srcY * w + x) * ch;
                int dstIdx = (y * w + x) * 4;
                out[dstIdx + 0] = raw[srcIdx + 2];
                out[dstIdx + 1] = raw[srcIdx + 1];
                out[dstIdx + 2] = raw[srcIdx + 0];
                out[dstIdx + 3] = (ch == 4) ? raw[srcIdx + 3] : 255;
            }
        }

        return ImageData(std::move(out), w, h, 4);
    }

    inline ImageData a_loadTGA(const std::filesystem::path& filepath, bool flipVertically)
    {
        std::ifstream f(filepath, std::ios::binary);
        if (!f) throw std::runtime_error("Cannot open TGA: " + filepath.string());

        uint8_t hdr[18];
        f.read(reinterpret_cast<char*>(hdr), 18);

        uint8_t  idLen = hdr[0];
        uint8_t  imgType = hdr[2];
        uint16_t w = *reinterpret_cast<uint16_t*>(&hdr[12]);
        uint16_t h = *reinterpret_cast<uint16_t*>(&hdr[14]);
        uint8_t  bpp = hdr[16];
        uint8_t  desc = hdr[17];

        if ((imgType != 2 && imgType != 10) || (bpp != 24 && bpp != 32))
            throw std::runtime_error("Unsupported TGA format: " + filepath.string());

        int ch = bpp / 8;
        bool originTopLeft = (desc & 0x20) != 0;

        f.seekg(idLen, std::ios::cur);

        const size_t pixelCount = size_t(w) * size_t(h);
        std::vector<uint8_t> raw(pixelCount * ch);

        if (imgType == 2) {
            f.read(reinterpret_cast<char*>(raw.data()), raw.size());
        }
        else {
            size_t totalRead = 0;
            while (totalRead < pixelCount) {
                uint8_t header = 0;
                f.read(reinterpret_cast<char*>(&header), 1);

                int count = (header & 0x7F) + 1;
                if (header & 0x80) {
                    std::vector<uint8_t> pixel(ch);
                    f.read(reinterpret_cast<char*>(pixel.data()), ch);
                    for (int i = 0; i < count; ++i)
                        std::copy(pixel.begin(), pixel.end(), raw.begin() + (totalRead + i) * ch);
                }
                else {
                    f.read(reinterpret_cast<char*>(raw.data() + totalRead * ch), count * ch);
                }
                totalRead += count;
            }
        }

        std::vector<uint8_t> out(pixelCount * 4);
        for (uint16_t y = 0; y < h; ++y) {
            uint16_t srcY = (originTopLeft ^ flipVertically) ? y : (h - 1 - y);
            const uint8_t* srcRow = raw.data() + size_t(srcY) * w * ch;
            uint8_t* dstRow = out.data() + size_t(y) * w * 4;

            for (uint16_t x = 0; x < w; ++x) {
                dstRow[x * 4 + 0] = srcRow[x * ch + 2];
                dstRow[x * 4 + 1] = srcRow[x * ch + 1];
                dstRow[x * 4 + 2] = srcRow[x * ch + 0];
                dstRow[x * 4 + 3] = (ch == 4) ? srcRow[x * ch + 3] : 255;
            }
        }

        return ImageData(std::move(out), int(w), int(h), 4);
    }

    inline ImageData a_loadPPM(const std::filesystem::path& filepath, bool flipVertically)
    {
        std::ifstream f{ filepath, std::ios::binary };
        if (!f) throw std::runtime_error("Cannot open PPM: " + filepath.string());

        auto nextToken = [&](std::string& out) {
            out.clear();
            char c;
            while (f.get(c)) {
                if (std::isspace(static_cast<unsigned char>(c))) continue;
                if (c == '#') {
                    f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }
                f.unget();
                break;
            }
            while (f.get(c)) {
                if (std::isspace(static_cast<unsigned char>(c))) break;
                out.push_back(c);
            }
            if (out.empty())
                throw std::runtime_error("PPM parse error: " + filepath.string());
            };

        std::string magic, wstr, hstr, maxvstr;
        nextToken(magic); nextToken(wstr); nextToken(hstr); nextToken(maxvstr);
        if (magic != "P6" && magic != "P3")
            throw std::runtime_error("Unsupported PPM format: " + filepath.string());

        const int w = std::stoi(wstr);
        const int h = std::stoi(hstr);
        const int maxv = std::stoi(maxvstr);
        if (maxv != 255)
            throw std::runtime_error("PPM maxval!=255: " + filepath.string());

        while (true) {
            int c = f.peek();
            if (c == EOF) throw std::runtime_error("Unexpected EOF: " + filepath.string());
            if (!std::isspace(static_cast<unsigned char>(c))) break;
            f.get();
        }

        const size_t pixelCount = size_t(w) * size_t(h);
        std::vector<uint8_t> rawData(pixelCount * 3);

        if (magic == "P6") {
            f.read(reinterpret_cast<char*>(rawData.data()), rawData.size());
        }
        else {
            std::string tok;
            for (size_t i = 0; i < pixelCount * 3; ++i) {
                nextToken(tok);
                rawData[i] = static_cast<uint8_t>(std::stoi(tok));
            }
        }

        std::vector<uint8_t> pixels(pixelCount * 4);
        for (size_t i = 0; i < pixelCount; ++i) {
            pixels[i * 4 + 0] = rawData[i * 3 + 0];
            pixels[i * 4 + 1] = rawData[i * 3 + 1];
            pixels[i * 4 + 2] = rawData[i * 3 + 2];
            pixels[i * 4 + 3] = 255;
        }

        if (flipVertically) {
            const size_t rowBytes = size_t(w) * 4;
            std::vector<uint8_t> tmp(rowBytes);
            for (int y = 0; y < h / 2; ++y) {
                auto top = pixels.data() + y * rowBytes;
                auto bot = pixels.data() + (h - 1 - y) * rowBytes;
                std::memcpy(tmp.data(), top, rowBytes);
                std::memcpy(top, bot, rowBytes);
                std::memcpy(bot, tmp.data(), rowBytes);
            }
        }

        return ImageData{ std::move(pixels), w, h, 4 };
    }
} // namespace almondnamespace
