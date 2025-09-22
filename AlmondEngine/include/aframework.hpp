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

#ifdef _WIN32

#ifndef ALMOND_MAIN_HEADLESS
 // Almond Entry Point No-Op
#define WIN32_LEAN_AND_MEAN
//#define NOGDI
#define NOMINMAX
//#define NOUSER
#include <windows.h>
#include <windowsx.h>         // for GET_X_LPARAM / GET_Y_LPARAM
// used in aengine.hpp
#include <shellapi.h>
//#undef Rectangle


#else
//#include <minwindef.h>

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <windowsx.h>         // for GET_X_LPARAM / GET_Y_LPARAM
#undef Rectangle

#endif // !ALMOND_MAIN_HEADLESS
/*
* this was for raylib, direct calls to winapi commands to avoid namespace pollution
* it was abandoned because raylib directly conflicts with int main the heart of the library,
  having it's own internal main and me wanting out of the box easy solutions

  update: I have figured out a way with this new engine design to disable the entry point like SDL does when I include it,
  which means it's entirely possible to see raylib and sfml in concert with almond in the future! yay!
*/
/*
//#define NOMINMAX
//#define NOGDI

#define _WIN64

//just define essential types
typedef void* HMODULE;
typedef unsigned long DWORD;
typedef const char* LPCSTR;
typedef unsigned long HRESULT;
#define S_OK ((HRESULT)0L)

#ifndef NULL
#define NULL 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef struct tagMSG {
    void* hwnd;        // Handle to the window
    unsigned int message; // Message identifier
    unsigned int wParam;  // Additional message information
    long lParam;          // Additional message information
    unsigned int time;    // Timestamp
    long pt_x;            // X-coordinate of the cursor position
    long pt_y;            // Y-coordinate of the cursor position
} MSG;
*/
#endif // _WIN32

