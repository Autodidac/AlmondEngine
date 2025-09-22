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
// aplatformpump.hpp
#pragma once

#include "aengineconfig.hpp"   // All ENGINE-specific includes

#if defined(_WIN32)
    #include "aframework.hpp"
#elif defined(__APPLE__) && defined(__MACH__)
    #import <Cocoa/Cocoa.h>   // compile as ObjC++
#elif defined(__linux__)
    #include <X11/Xlib.h>      // ensure you link -lX11
#elif defined(__ANDROID__)
    #include <android_native_app_glue.h>
#elif defined(__EMSCRIPTEN__)
    #include <emscripten.h>
#else
    #error "Unsupported platform for pump_events"
#endif

namespace almondnamespace::platform 
{
    inline bool pump_events()  // Returns false if the user closed the window / requested quit
    {
#if defined(_WIN32)
#ifndef ALMOND_MAIN_HEADLESS
#ifndef ALMOND_USING_RAYLIB

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                return false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // 🔑 After dispatching, update global input states
        almondnamespace::input::poll_input();

        return true;

#endif // !ALMOND_USING_RAYLIB
#endif // !ALMOND_MAIN_HEADLESS

#elif defined(__APPLE__) && defined(__MACH__)
        @autoreleasepool{
            NSEvent * ev = nil;
            while ((ev = [NSApp nextEventMatchingMask : NSEventMaskAny
                                             untilDate : [NSDate distantPast]
                                                inMode : NSDefaultRunLoopMode
                                               dequeue : YES])) {
                if (ev.type == NSEventTypeApplicationDefined /* or your quit type */)
                    return false;
                [NSApp sendEvent : ev] ;
            }
        }

        almondnamespace::input::poll_input(); // macOS stub version

        return true;

#elif defined(__linux__)
        extern Display* global_display;  // defined in your linux_context.cpp
        extern ::Window global_window;
        while (global_display && XPending(global_display)) {
            XEvent ev;
            XNextEvent(global_display, &ev);
            if (ev.type == ClientMessage) {
                return false;
            }
            // TODO: feed events into almondnamespace::input bitsets
        }

        almondnamespace::input::poll_input(global_display, global_window);

        return true;

#elif defined(__ANDROID__)
        extern struct android_app* global_android_app;
        int events;
        struct android_poll_source* source;
        while (ALooper_pollAll(0, nullptr, &events, (void**)&source) >= 0) {
            if (source) source->process(global_android_app, source);
        }
        // TODO: integrate Android key/touch input → almondnamespace::input
        return !global_android_app->destroyRequested;

#elif defined(__EMSCRIPTEN__)
        // EMSCRIPTEN uses callback hooks; nothing to poll here
        return true;
#endif

        return true;  // Fallback
    }

} // namespace almondnamespace::platform