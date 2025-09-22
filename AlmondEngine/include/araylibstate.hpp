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
 // araylibstate.hpp
#pragma once

#include "aplatform.hpp"
#include "aengineconfig.hpp"
#include "arobusttime.hpp"
#include "acommandline.hpp"

#include <array>
#include <bitset>
#include <functional>

#if defined(ALMOND_USING_RAYLIB)

namespace almondnamespace::raylibcontext
{
    struct RaylibState
    {
#ifdef ALMOND_USING_WINMAIN
        HWND hwnd = nullptr;
        HDC hdc = nullptr;
        HGLRC hglrc = nullptr;

        HGLRC glContext{}; // Store GL context created
      //  WNDPROC oldWndProc = nullptr;
       // WNDPROC getOldWndProc() const noexcept { return oldWndProc; }
        HWND parent = nullptr;
        std::function<void(int, int)> onResize;
        unsigned int width{ 400 };
        unsigned int height{ 300 };

        bool running{ false };
#endif

        
		bool shouldClose = false; // Set to true when the window should close
        // Raylib manages window internally, but track width & height for consistency
       // int screenWidth = 800;
        //int screenHeight = 600;
        int screenWidth = DEFAULT_WINDOW_WIDTH;
        int screenHeight = DEFAULT_WINDOW_HEIGHT;
        // Mouse state (if you want to hook raw input logic)
        struct MouseState {
            std::array<bool, 5> down{};     // 5 mouse buttons
            std::array<bool, 5> pressed{};
            std::array<bool, 5> prevDown{};
            int lastX = 0, lastY = 0;
        } mouse;

        // Keyboard state (Raylib already has IsKeyDown etc — wrap if you want consistent logic)
        struct KeyboardState {
            std::bitset<512> down;      // Raylib keycodes go above 256
            std::bitset<512> pressed;
            std::bitset<512> prevDown;
        } keyboard;

        // Timing — reuse your robusttime
        time::Timer pollTimer = time::createTimer(1.0);
        time::Timer fpsTimer = time::createTimer(1.0);
        int frameCount = 0;

        // If you want shaders — Raylib supports them.
      //  Shader shader = { 0 };
        int uTransformLoc = -1;
        int uUVRegionLoc = -1;
        int uSamplerLoc = -1;

        // If you want VAOs/VBOs, stick to OpenGL path — Raylib abstracts most of that.
    };

    // Inline global instance — same pattern.
    inline RaylibState s_raylibstate{};
}

#endif // ALMON
