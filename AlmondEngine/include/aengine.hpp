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
// aengine.hpp
#pragma once

#include "aplatform.hpp"
// leave this here, avoid auto sorting incorrect order, platform always comes first
#include "aengineconfig.hpp"

// Window dimensions
//inline constexpr int DEFAULT_WINDOW_WIDTH = 1024;
//inline constexpr int DEFAULT_WINDOW_HEIGHT = 768;
inline constexpr int DEFAULT_WINDOW_WIDTH = 1920;
inline constexpr int DEFAULT_WINDOW_HEIGHT = 1080;
//inline constexpr int DEFAULT_WINDOW_WIDTH = 2048;
//inline constexpr int DEFAULT_WINDOW_HEIGHT = 1080;
//inline constexpr int DEFAULT_WINDOW_WIDTH = 2732;
//inline constexpr int DEFAULT_WINDOW_HEIGHT = 1536;

//inline constexpr int DEFAULT_WINDOW_WIDTH = 3840;
//inline constexpr int DEFAULT_WINDOW_HEIGHT = 2160;
//inline constexpr int DEFAULT_WINDOW_WIDTH = 4096;
//inline constexpr int DEFAULT_WINDOW_HEIGHT = 2160;

#ifdef ALMOND_USING_WINMAIN

// Max string length for title and class name
#define MAX_LOADSTRING 100

namespace almondnamespace::core
{
    // Forward declarations of Win32 functions
    ATOM RegisterWindowClass(HINSTANCE hInstance, LPCWSTR window_name, LPCWSTR child_name);
    void PrintLastWin32Error(const wchar_t* lpszFunction);
    void ShowConsole();
    void RunEngine();
}

#endif
HWND InitWindowInstance(HINSTANCE hInstance, int nCmdShow, LPCWSTR szWindowClass, LPCWSTR szTitle, int32_t windowWidth, int32_t windowHeight);

