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

#include <string_view>
#include <cstdint>

namespace almondnamespace::ui
{
    struct vec2 {
        float x, y;

        // Overload the "+" operator to allow addition of two vec2 objects  
        vec2 operator+(const vec2& other) const {
            return { x + other.x, y + other.y };
        }
    };

    enum class event_type {
        none,
        mouse_down,
        mouse_up,
        mouse_move
    };

    struct input_event {
        event_type type;
        vec2 mouse_pos;
        uint8_t mouse_button;
    };

    void begin_frame(float dt, vec2 mouse_pos, bool mouse_down);
    void end_frame();

    void begin_window(std::string_view title, vec2 pos, vec2 size);
    void end_window();

    bool button(std::string_view label, vec2 size);
    void label(std::string_view text);

    void push_input(const input_event& e);

} // namespace aui
