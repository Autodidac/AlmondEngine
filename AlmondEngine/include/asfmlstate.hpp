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
 // asfmlstate.hpp
#pragma once

#include "aengineconfig.hpp"

#if defined(ALMOND_USING_SFML)

#include "arobusttime.hpp"
#include "acontextwindow.hpp"

#include <array>
#include <bitset>
#include <functional>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace almondnamespace::sfmlcontext::state
{
    struct SFML3State
    {
        SFML3State()
        {
            window.width = DEFAULT_WINDOW_WIDTH;
            window.height = DEFAULT_WINDOW_HEIGHT;
            window.should_close = false;
            screenWidth = window.width;
            screenHeight = window.height;
        }

        almondnamespace::contextwindow::WindowData window{};

        sf::Event event{};

        bool shouldClose{ false };
        int screenWidth{ DEFAULT_WINDOW_WIDTH };
        int screenHeight{ DEFAULT_WINDOW_HEIGHT };
        bool running{ false };

        std::function<void(int, int)> onResize{};

        struct MouseState
        {
            std::array<bool, static_cast<std::size_t>(sf::Mouse::ButtonCount)> down{};
            std::array<bool, static_cast<std::size_t>(sf::Mouse::ButtonCount)> pressed{};
            std::array<bool, static_cast<std::size_t>(sf::Mouse::ButtonCount)> prevDown{};
            int lastX = 0;
            int lastY = 0;
        } mouse{};

        struct KeyboardState
        {
            std::bitset<sf::Keyboard::KeyCount> down;
            std::bitset<sf::Keyboard::KeyCount> pressed;
            std::bitset<sf::Keyboard::KeyCount> prevDown;
        } keyboard{};

        time::Timer pollTimer = time::createTimer(1.0);
        time::Timer fpsTimer = time::createTimer(1.0);
        int frameCount = 0;

        [[nodiscard]] sf::RenderWindow* get_sfml_window() const noexcept
        {
            return window.sfml_window;
        }

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
    };

    inline SFML3State s_sfmlstate{};
}

#endif // ALMOND_USING_SFML
