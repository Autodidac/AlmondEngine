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
 // aopenglcontextinput.hpp
//#pragma once
//
//#include "aplatform.hpp"
//#include "aengineconfig.hpp"    // <windows.h>, glad, etc.
//
//#include "aopenglcontextstate.hpp"  // brings in the actual inline s_state
//
//#include <iostream>
//
//namespace almondnamespace::openglcontext
//{
//    using almondnamespace::opengl::s_state;
//     window procedure hook for input handling
//    inline LRESULT CALLBACK WndProcHook(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
//    {
//        auto log_mouse = [](const char* btn, const char* state) {
//#if defined(DEBUG_INPUT)
//            std::cout << "[WndProc] " << btn << " Mouse Button " << state << "\n";
//#endif
//            };
//
//        auto log_key = [](WPARAM key, const char* state) {
//#if defined(DEBUG_INPUT)
//            std::cout << "[WndProc] Key " << key << " " << state << "\n";
//#endif
//            };
//
//        auto log_char = [](WPARAM key) {
//#if defined(DEBUG_INPUT)
//            if (key >= 32 && key < 127) {
//                std::cout << "[WndProc] CHAR '" << static_cast<char>(key) << "' (" << key << ")\n";
//            }
//            else {
//                std::cout << "[WndProc] CHAR (code " << key << ") [non-printable]\n";
//            }
//#endif
//            };
//
//        auto log_move = [](int x, int y) {
//#if defined(DEBUG_INPUT)
//            std::cout << "[WndProc] Mouse MOVE to (" << x << ", " << y << ")\n";
//#endif
//            };
//
//        switch (msg) {
//        case WM_LBUTTONDOWN: s_state.mouse.down[0] = true; log_mouse("Left", "DOWN"); break;
//        case WM_LBUTTONUP:   s_state.mouse.down[0] = false; log_mouse("Left", "UP"); break;
//        case WM_RBUTTONDOWN: s_state.mouse.down[1] = true; log_mouse("Right", "DOWN"); break;
//        case WM_RBUTTONUP:   s_state.mouse.down[1] = false; log_mouse("Right", "UP"); break;
//        case WM_MBUTTONDOWN: s_state.mouse.down[2] = true; log_mouse("Middle", "DOWN"); break;
//        case WM_MBUTTONUP:   s_state.mouse.down[2] = false; log_mouse("Middle", "UP"); break;
//
//        case WM_XBUTTONDOWN: {
//            int idx = GET_XBUTTON_WPARAM(w) == XBUTTON1 ? 3 : 4;
//            s_state.mouse.down[idx] = true;
//            log_mouse(idx == 3 ? "X1" : "X2", "DOWN");
//            break;
//        }
//
//        case WM_XBUTTONUP: {
//            int idx = GET_XBUTTON_WPARAM(w) == XBUTTON1 ? 3 : 4;
//            s_state.mouse.down[idx] = false;
//            log_mouse(idx == 3 ? "X1" : "X2", "UP");
//            break;
//        }
//
//        case WM_MOUSEMOVE: {
//            int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
//            if (x != s_state.mouse.lastX || y != s_state.mouse.lastY) {
//                s_state.mouse.lastX = x;
//                s_state.mouse.lastY = y;
//                log_move(x, y);
//            }
//            break;
//        }
//
//        case WM_NCMOUSEMOVE:
//#if defined(DEBUG_INPUT)
//            std::cout << "[WndProc] Non-client mouse move\n";
//#endif
//            break;
//
//        case WM_SETCURSOR:
//#if defined(DEBUG_INPUT)
//            std::cout << "[WndProc] Set Cursor\n";
//#endif
//            return DefWindowProc(hwnd, msg, w, l);
//
//        case WM_KEYDOWN:
//            if (w < 256) s_state.keyboard.down[w] = true;
//            log_key(w, "DOWN");
//            break;
//
//        case WM_KEYUP:
//            if (w < 256) s_state.keyboard.down[w] = false;
//            log_key(w, "UP");
//            break;
//
//        case WM_CHAR:
//            log_char(w);
//            break;
//
//        default:
//#if defined(DEBUG_INPUT)
//            std::cout << "[WndProc] Unhandled message: " << msg << "\n";
//#endif
//            break;
//        }
//
//        return s_state.oldWndProc
//            ? CallWindowProc(s_state.oldWndProc, hwnd, msg, w, l)
//            : DefWindowProc(hwnd, msg, w, l);
//    }
//
//} // namespace almondnamespace
