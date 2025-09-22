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
 // araylibcontextinput.hpp
#pragma once

#include "aengineconfig.hpp" // Make sure this is included!

#if defined(ALMOND_USING_RAYLIB)
#include "ainput.hpp"

namespace almondnamespace::raylibcontext {

    inline void poll_input()
    {
        using namespace almondnamespace::input;

        // Reset pressed states
        keyPressed.reset();
        mousePressed.reset();
        mouseWheel = 0;

        // --- Keyboard ---
        for (int k = 0; k < Key::Count; ++k)
        {
            int raylibKey = 0;
            switch (static_cast<Key>(k))
            {
            case Key::A: raylibKey = KEY_A; break;
            case Key::B: raylibKey = KEY_B; break;
            case Key::C: raylibKey = KEY_C; break;
            case Key::D: raylibKey = KEY_D; break;
            case Key::E: raylibKey = KEY_E; break;
            case Key::F: raylibKey = KEY_F; break;
            case Key::G: raylibKey = KEY_G; break;
            case Key::H: raylibKey = KEY_H; break;
            case Key::I: raylibKey = KEY_I; break;
            case Key::J: raylibKey = KEY_J; break;
            case Key::K: raylibKey = KEY_K; break;
            case Key::L: raylibKey = KEY_L; break;
            case Key::M: raylibKey = KEY_M; break;
            case Key::N: raylibKey = KEY_N; break;
            case Key::O: raylibKey = KEY_O; break;
            case Key::P: raylibKey = KEY_P; break;
            case Key::Q: raylibKey = KEY_Q; break;
            case Key::R: raylibKey = KEY_R; break;
            case Key::S: raylibKey = KEY_S; break;
            case Key::T: raylibKey = KEY_T; break;
            case Key::U: raylibKey = KEY_U; break;
            case Key::V: raylibKey = KEY_V; break;
            case Key::W: raylibKey = KEY_W; break;
            case Key::X: raylibKey = KEY_X; break;
            case Key::Y: raylibKey = KEY_Y; break;
            case Key::Z: raylibKey = KEY_Z; break;
            case Key::Num0: raylibKey = KEY_ZERO; break;
            case Key::Num1: raylibKey = KEY_ONE; break;
            case Key::Num2: raylibKey = KEY_TWO; break;
            case Key::Num3: raylibKey = KEY_THREE; break;
            case Key::Num4: raylibKey = KEY_FOUR; break;
            case Key::Num5: raylibKey = KEY_FIVE; break;
            case Key::Num6: raylibKey = KEY_SIX; break;
            case Key::Num7: raylibKey = KEY_SEVEN; break;
            case Key::Num8: raylibKey = KEY_EIGHT; break;
            case Key::Num9: raylibKey = KEY_NINE; break;
            case Key::Space: raylibKey = KEY_SPACE; break;
            case Key::Enter: raylibKey = KEY_ENTER; break;
            case Key::Escape: raylibKey = KEY_ESCAPE; break;
            case Key::Tab: raylibKey = KEY_TAB; break;
            case Key::Backspace: raylibKey = KEY_BACKSPACE; break;
            case Key::Left: raylibKey = KEY_LEFT; break;
            case Key::Right: raylibKey = KEY_RIGHT; break;
            case Key::Up: raylibKey = KEY_UP; break;
            case Key::Down: raylibKey = KEY_DOWN; break;
                // Add more mappings as needed...
            default: raylibKey = 0; break;
            }
            if (raylibKey == 0) continue;

            bool down = IsKeyDown(raylibKey);
            keyPressed[k] = down && !keyDown[k];
            keyDown[k] = down;
        }

        // --- Mouse ---
        for (int b = 0; b < MouseButton::MouseCount; ++b)
        {
            int raylibBtn = 0;
            switch (static_cast<MouseButton>(b))
            {
            case MouseButton::MouseLeft:   raylibBtn = MOUSE_BUTTON_LEFT; break;
            case MouseButton::MouseRight:  raylibBtn = MOUSE_BUTTON_RIGHT; break;
            case MouseButton::MouseMiddle: raylibBtn = MOUSE_BUTTON_MIDDLE; break;
            case MouseButton::MouseButton4: raylibBtn = MOUSE_BUTTON_SIDE; break;
            case MouseButton::MouseButton5: raylibBtn = MOUSE_BUTTON_EXTRA; break;
            default: raylibBtn = 0; break;
            }
            if (raylibBtn == 0) continue;

            bool down = IsMouseButtonDown(raylibBtn);
            mousePressed[b] = down && !mouseDown[b];
            mouseDown[b] = down;
        }

        // Mouse position
        mouseX = GetMouseX();
        mouseY = GetMouseY();

        // Mouse wheel (Raylib returns float, use int for your API)
        mouseWheel = static_cast<int>(GetMouseWheelMove());
    }

} // namespace almondnamespace::raylibcontext

#endif // Non-Raylib context, use generic input handling