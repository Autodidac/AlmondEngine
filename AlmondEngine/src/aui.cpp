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
#include "pch.h"

#include "aui.hpp"

#include <string_view>

void draw_rect(float x, float y, float w, float h, float r, float g, float b) {}
void draw_text(std::string_view txt, float x, float y) {}

namespace almondnamespace 
{
    ui::vec2 g_cursor = {0, 0};
    ui::vec2 g_origin = {0, 0};
    ui::vec2 g_mouse_pos = {0, 0};
    bool g_mouse_down = false;
    bool g_inside_window = false;

namespace ui {

void push_input(const input_event& e) {
    g_mouse_pos = e.mouse_pos;
    if (e.type == event_type::mouse_down) g_mouse_down = true;
    if (e.type == event_type::mouse_up) g_mouse_down = false;
}

void begin_frame(float, vec2 mouse, bool down) {
    g_mouse_pos = mouse;
    g_mouse_down = down;
    g_cursor = {0, 0};
}

void end_frame() {}

void begin_window(std::string_view title, vec2 pos, vec2 size) {
    g_origin = pos;
    g_cursor = pos;
    draw_rect(pos.x, pos.y, size.x, size.y, 0.2f, 0.2f, 0.2f);
    draw_text(title, pos.x + 4, pos.y + 4);
    g_cursor.y += 20.0f;
    g_inside_window = true;
}

void end_window() {
    g_inside_window = false;
}

bool button(std::string_view label, vec2 size) {
    if (!g_inside_window) return false;

    float x = g_cursor.x, y = g_cursor.y, w = size.x, h = size.y;
    draw_rect(x, y, w, h, 0.5f, 0.5f, 0.5f);
    draw_text(label, x + 4, y + 4);

    bool hovered = g_mouse_pos.x >= x && g_mouse_pos.x <= x + w &&
                   g_mouse_pos.y >= y && g_mouse_pos.y <= y + h;

    g_cursor.y += h + 4.0f;
    return hovered && g_mouse_down;
}

void label(std::string_view text) {
    if (!g_inside_window) return;
    draw_text(text, g_cursor.x + 4, g_cursor.y);
    g_cursor.y += 16.0f;
}

} // namespace ui
}
