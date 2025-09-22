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
// aopenglstate.hpp
#pragma once

#include "aplatform.hpp"
#include "aengineconfig.hpp"    // brings in <windows.h>, <glad/glad.h>, etc.
#include "arobusttime.hpp"  // your time namespace

#include <array>
#include <bitset>
#include <functional>


#if defined (ALMOND_USING_OPENGL)

namespace almondnamespace::openglcontext
{
    struct OpenGL4State
    {
#ifdef ALMOND_USING_WINMAIN
        HWND hwnd{};
        HDC hdc = nullptr;
        HGLRC hglrc = nullptr;

        HGLRC glContext{}; // Store GL context created
        WNDPROC oldWndProc = nullptr;
        WNDPROC getOldWndProc() const noexcept { return oldWndProc; }

        HWND parent = nullptr;
        std::function<void(int, int)> onResize;
        unsigned int width{ 400 };
        unsigned int height{ 300 };

        bool running{ false };
#endif
        struct MouseState {
            std::array<bool, 5> down{};
            std::array<bool, 5> pressed{};
            std::array<bool, 5> prevDown{};
            int lastX = 0, lastY = 0;
        } mouse;

        struct KeyboardState {
            std::bitset<256> down;
            std::bitset<256> pressed;
            std::bitset<256> prevDown;
        } keyboard;

        time::Timer pollTimer = time::createTimer(1.0);
        time::Timer fpsTimer = time::createTimer(1.0);
        int frameCount = 0;

        GLuint shader = 0;
        GLint uUVRegionLoc = -1;
        GLint uTransformLoc = -1;
        GLint uSamplerLoc = -1;
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLuint fbo = 0;
    };

    // Inline global instance, header-only style
    inline OpenGL4State s_openglstate{};
}
#endif // ALMOND_USING_OPENGL