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
 // SoftRenderer - Context / Quad Backend
#pragma once

#include "aplatform.hpp"
#include "aengineconfig.hpp"

#if defined(ALMOND_USING_SOFTWARE_RENDERER)

#include "acontext.hpp"
#include "asoftrenderer_state.hpp"
#include "asoftrenderer_textures.hpp"
#include "asoftrenderer_renderer.hpp"

#include <memory>
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#undef max

#ifdef _WIN32
#include <windows.h>
#endif

namespace almondnamespace::anativecontext
{
    inline TexturePtr cubeTexture;
    inline SoftwareRenderer renderer;

    // Initialize the software renderer
    inline bool softrenderer_initialize(std::shared_ptr<core::Context> ctx,
        HWND parentWnd = nullptr,
        unsigned int w = 400, unsigned int h = 300,
        std::function<void(int, int)> onResize = nullptr)
    {
        if (!ctx || !ctx->hwnd) {
            std::cerr << "[SoftRenderer] Invalid context hwnd\n";
            return false;
        }

        s_softrendererstate.width = static_cast<int>(w);
        s_softrendererstate.height = static_cast<int>(h);
        s_softrendererstate.running = true;
        s_softrendererstate.parent = parentWnd ? parentWnd : ctx->hwnd;
        s_softrendererstate.onResize = std::move(onResize);
        s_softrendererstate.hwnd = ctx->hwnd;

        // Allocate framebuffer inside state
        s_softrendererstate.framebuffer.resize(size_t(w) * size_t(h), 0xFF000000);

        if (!cubeTexture) {
            cubeTexture = std::make_shared<Texture>(64, 64);
            for (int y = 0; y < cubeTexture->height; ++y) {
                for (int x = 0; x < cubeTexture->width; ++x) {
                    cubeTexture->pixels[size_t(y) * size_t(cubeTexture->width) + size_t(x)] =
                        ((x / 8 + y / 8) % 2) ? 0xFFFF0000 : 0xFF00FF00; // checkerboard
                }
            }
        }

#ifdef _WIN32
        ZeroMemory(&s_softrendererstate.bmi, sizeof(BITMAPINFO));
        s_softrendererstate.bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        s_softrendererstate.bmi.bmiHeader.biWidth = static_cast<int>(w);
        s_softrendererstate.bmi.bmiHeader.biHeight = -static_cast<int>(h); // top-down
        s_softrendererstate.bmi.bmiHeader.biPlanes = 1;
        s_softrendererstate.bmi.bmiHeader.biBitCount = 32;
        s_softrendererstate.bmi.bmiHeader.biCompression = BI_RGB;
#endif

        std::cout << "[SoftRenderer] Initialized on HWND=" << ctx->hwnd << "\n";
        return true;
    }

    // Draw a textured quad
    inline void softrenderer_draw_quad(SoftRendState& softstate)
    {
        if (atlasmanager::atlas_vector.empty()) return;
        const auto* atlas = atlasmanager::atlas_vector[0];
        if (!atlas) return;

        int w = atlas->width;
        int h = atlas->height;
        if (w <= 0 || h <= 0 || atlas->pixel_data.empty()) return;

        for (int y = 0; y < softstate.height; ++y) {
            for (int x = 0; x < softstate.width; ++x) {
                float u = float(x) / float(std::max(1, softstate.width));
                float v = float(y) / float(std::max(1, softstate.height));

                int texX = static_cast<int>(u * (w - 1));
                int texY = static_cast<int>(v * (h - 1));

                size_t idx = size_t(texY) * size_t(w) + size_t(texX);
                if (idx >= atlas->pixel_data.size()) {
                    throw std::runtime_error("[Software] Pixel idx out of range");
                }

                if (idx < atlas->pixel_data.size()) {
                    uint32_t color = atlas->pixel_data[idx];
                    softstate.framebuffer[size_t(y) * size_t(softstate.width) + size_t(x)] = color;
                }
            }
        }
    }

    // Main process loop
    inline bool softrenderer_process(core::Context& ctx, core::CommandQueue& queue) {
        auto& sr = s_softrendererstate;

        // Clear framebuffer
        std::fill(sr.framebuffer.begin(), sr.framebuffer.end(), 0xFF000000);

        // Draw quad
//        softrenderer_draw_quad(sr);

        // Drain queued commands
        queue.drain();

#ifdef _WIN32
        // Present framebuffer to window
        HDC hdc = ctx.hdc;
        if (hdc) {
            BITMAPINFO bmi{};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = sr.width;
            bmi.bmiHeader.biHeight = -sr.height; // top-down
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;

            StretchDIBits(hdc,
                0, 0, sr.width, sr.height,
                0, 0, sr.width, sr.height,
                sr.framebuffer.data(),
                &bmi, DIB_RGB_COLORS, SRCCOPY);
        }
#endif
        return true;
    }

    inline void softrenderer_cleanup(std::shared_ptr<almondnamespace::core::Context>& ctx)
    {
        auto& sr = s_softrendererstate;

        sr.framebuffer.clear();
        cubeTexture.reset();
#ifdef _WIN32
        if (s_softrendererstate.hwnd) {
            DestroyWindow(s_softrendererstate.hwnd);
            s_softrendererstate.hwnd = nullptr;
        }
#endif
        s_softrendererstate = {};
        std::cout << "[SoftRenderer] Cleanup complete\n";
    }

    inline int get_width() { return s_softrendererstate.width; }
    inline int get_height() { return s_softrendererstate.height; }
}

#endif // ALMOND_USING_SOFTWARE_RENDERER
