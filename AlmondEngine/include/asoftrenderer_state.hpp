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
 // SoftRenderer - State
#pragma once

#include "aplatform.hpp"
#include "aengineconfig.hpp"    // brings in <windows.h>, <glad/glad.h>, etc.
#include "arobusttime.hpp"      // your time namespace

//#include "asoftrenderer_renderer.hpp"
//#include "asoftrenderer_textures.hpp"

#include <array>
#include <bitset>
#include <functional>
#include <memory>

#if defined(ALMOND_USING_SOFTWARE_RENDERER)

namespace almondnamespace::anativecontext
{
    struct SoftRendState
    {
        // Framebuffer and cube texture

#ifdef ALMOND_USING_WINMAIN
        HWND hwnd{};    // Window handle
        HDC hdc{};      // Device context
        HWND parent{};  // Parent window
        WNDPROC oldWndProc{};
        BITMAPINFO bmi{};  // <--- Add this
#endif
        std::function<void(int, int)> onResize;
        int width{ 400 };
        int height{ 300 };
        bool running{ false };
        std::vector<uint32_t> framebuffer;

        // Input states
        struct MouseState
        {
            std::array<bool, 5> down{};
            std::array<bool, 5> pressed{};
            std::array<bool, 5> prevDown{};
            int lastX = 0, lastY = 0;
        } mouse;

        struct KeyboardState
        {
            std::bitset<256> down;
            std::bitset<256> pressed;
            std::bitset<256> prevDown;
        } keyboard;

        // Timing
        time::Timer pollTimer = time::createTimer(1.0);
        time::Timer fpsTimer = time::createTimer(1.0);
        int frameCount = 0;

        // Cube rotation
        float angle = 0.f;
    };

    // Inline global state instance
    inline SoftRendState s_softrendererstate{};
}

#endif // ALMOND_USING_SOFTWARE_RENDERER
