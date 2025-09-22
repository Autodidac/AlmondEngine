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
 // asdlstate.hpp
#pragma once

#include "aplatform.hpp"
#include "aengineconfig.hpp"

#if defined(ALMOND_USING_SDL)

#include "arobusttime.hpp"
#include "acontextwindow.hpp"

#include <array>
#include <bitset>
#include <functional>

namespace almondnamespace::sdlcontext::state
{
    struct SDL3State
    {
        SDL3State()
        {
            window.width = DEFAULT_WINDOW_WIDTH;
            window.height = DEFAULT_WINDOW_HEIGHT;
            window.should_close = false;
            screenWidth = window.width;
            screenHeight = window.height;
        }

        almondnamespace::contextwindow::WindowData window{};

        SDL_Event sdl_event{};

        bool shouldClose{ false };
        int screenWidth{ DEFAULT_WINDOW_WIDTH };
        int screenHeight{ DEFAULT_WINDOW_HEIGHT };
        bool running{ false };

        std::function<void(int, int)> onResize{};

        struct MouseState
        {
            std::array<bool, 5> down{};
            std::array<bool, 5> pressed{};
            std::array<bool, 5> prevDown{};
            int lastX = 0;
            int lastY = 0;
        } mouse{};

        struct KeyboardState
        {
            std::bitset<SDL_SCANCODE_COUNT> down;
            std::bitset<SDL_SCANCODE_COUNT> pressed;
            std::bitset<SDL_SCANCODE_COUNT> prevDown;
        } keyboard{};

        time::Timer pollTimer = time::createTimer(1.0);
        time::Timer fpsTimer = time::createTimer(1.0);
        int frameCount = 0;

#ifdef _WIN32
    private:
        WNDPROC oldWndProc_ = nullptr;
        HWND parent_ = nullptr;

    public:
        WNDPROC getOldWndProc() const noexcept { return oldWndProc_; }
        void setOldWndProc(WNDPROC proc) noexcept { oldWndProc_ = proc; }

        void setParent(HWND parent) noexcept { parent_ = parent; }
        HWND getParent() const noexcept { return parent_; }
#endif

        void mark_should_close(bool value) noexcept
        {
            shouldClose = value;
            window.should_close = value;
        }

        void set_dimensions(int w, int h) noexcept
        {
            screenWidth = w;
            screenHeight = h;
            window.set_size(w, h);
        }

        HWND hwnd() const noexcept { return window.hwnd; }
        HDC hdc() const noexcept { return window.hdc; }
        HGLRC glrc() const noexcept { return window.glrc; }
        SDL_Window* sdl_window() const noexcept { return window.sdl_window; }
    };

    inline SDL3State s_sdlstate{};
}

#endif // ALMOND_USING_SDL
