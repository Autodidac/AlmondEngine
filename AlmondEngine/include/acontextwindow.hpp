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
// acontextwindow.hpp
#pragma once

#include "aengineconfig.hpp"   // All platform-specific includes assumed here
#include "aengine.hpp" // For DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT

#include <string_view>
#include <stdexcept>

namespace almondnamespace::contextwindow
{
    struct WindowData final {
#if defined(_WIN32)
#ifndef ALMOND_MAIN_HEADLESS
        HWND hwnd = nullptr;
        HWND hwndChild = nullptr;
        HDC hdc = nullptr;
        HGLRC glrc = nullptr;

        static inline WindowData* s_instance = nullptr;
#endif
#endif

#if defined(ALMOND_USING_SDL)
        SDL_Window* sdl_window = nullptr;
        SDL_GLContext sdl_glrc = nullptr;
#endif

#if defined(ALMOND_USING_SFML)
        sf::RenderWindow* sfml_window = nullptr;
        sf::Context sfml_context; // Placeholder for SFML GL context
#endif

        int width = DEFAULT_WINDOW_WIDTH;
        int height = DEFAULT_WINDOW_HEIGHT;

        bool should_close = false;

      //  static inline WindowData* s_instance = nullptr;

        static void set_global_instance(WindowData* instance) {
            s_instance = instance;
        }

        static WindowData* get_global_instance() noexcept {
            return s_instance;
        }

        // Platform-specific setters
#ifndef ALMOND_MAIN_HEADLESS
        void setParentHandle(HWND hwndValue) noexcept { hwnd = hwndValue; }
        void setChildHandle(HWND hwnd) noexcept { hwndChild = hwnd; }

        HWND getParentHandle() const noexcept { return hwnd; }
#endif

#if defined(_WIN32)
        static HWND getWindowHandle() noexcept {
            return s_instance ? s_instance->hwnd : nullptr;
        }

        static HWND getChildHandle() noexcept {
            return s_instance ? s_instance->hwndChild : nullptr;
        }
#endif

        void set_size(int w, int h) noexcept {
            width = w;
            height = h;
        }

        void set_should_close(bool value) noexcept {
            should_close = value;
        }

        bool get_should_close() const noexcept {
            return should_close;
        }

        void set_window_title(std::string_view title) noexcept {
            // Implementation here if needed
        }

#if defined(ALMOND_USING_SDL)
        static SDL_Window* getSDLWindow() noexcept {
            return s_instance ? s_instance->sdl_window : nullptr;
        }
#endif

#if defined(ALMOND_USING_SFML)
        sf::RenderWindow* getSFMLWindow() {
            if (!s_instance) throw std::runtime_error("WindowData is null!");
            return s_instance->sfml_window;
        }
#endif

        [[nodiscard]] int get_width() const noexcept { return width; }
        [[nodiscard]] int get_height() const noexcept { return height; }

    };
} // namespace almondnamespace::contextwindow


//#ifdef _WIN32
//extern "C" LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//#endif
