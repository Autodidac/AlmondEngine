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
 // asdlcontext.hpp
#pragma once

#include "aengineconfig.hpp"

#if defined(ALMOND_USING_SDL)

// Add the following definition at the top of the file, after the includes, to define SDL_WINDOW_FULLSCREEN_DESKTOP if it is not already defined.
#ifndef SDL_WINDOW_FULLSCREEN_DESKTOP
#define SDL_WINDOW_FULLSCREEN_DESKTOP 1  // SDL3 equivalent for fullscreen desktop mode
#endif

#include "aplatform.hpp"

#define SDL_MAIN_HANDLED // Allow SDL to handle main function, useful for SDL applications

#include "acontext.hpp"
#include "acontextwindow.hpp"
#include "asdlcontextrenderer.hpp"
#include "asdltextures.hpp"
#include "asdlstate.hpp"

#include <stdexcept>
#include <iostream>

namespace almondnamespace::sdlcontext
{
   // using namespace almondnamespace::contextwindow;
   // extern state::SDL3State s_sdlstate;

    //inline void initialize_with_sdl_window()
    //{
    //    std::cout << "SDL version: " << SDL_GetRevision()
    //        << " " << SDL_GetVersion() << "\n";

    //    int result = SDL_Init(SDL_INIT_VIDEO);
    //    if (result < 0) {
    //        // Handle error
    //        throw std::runtime_error(SDL_GetError());
    //    }


    //    s_sdlstate.window.sdl_window = SDL_CreateWindow(
    //        "AlmondEngine",
    //        800,
    //        600,
    //        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    //    );

    //    if (!s_sdlstate.window.sdl_window)
    //        throw std::runtime_error(SDL_GetError());

    //    sdl_renderer.renderer = SDL_CreateRenderer(s_sdlstate.window.sdl_window, nullptr);
    //    if (!sdl_renderer.renderer)
    //    {
    //        SDL_DestroyWindow(s_sdlstate.window.sdl_window);
    //        SDL_Quit();
    //        throw std::runtime_error(SDL_GetError());
    //    }

    //    SDL_SetRenderDrawColor(sdl_renderer.renderer, 0, 0, 0, 255);
    //}

    struct SDLState {
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;

        HWND parent = nullptr;
        HWND hwnd = nullptr;
        bool running = false;
        int width = 400;
        int height = 300;
        std::function<void(int, int)> onResize;
    };
    
    inline SDLState sdlcontext;

    inline bool sdl_initialize(std::shared_ptr<core::Context> ctx, HWND parentWnd = nullptr, int w = 400, int h = 300, std::function<void(int, int)> onResize = nullptr)
    {
        sdlcontext.onResize = std::move(onResize);
        sdlcontext.width = w;
        sdlcontext.height = h;
        sdlcontext.parent = parentWnd;

        if (SDL_Init(SDL_INIT_VIDEO) == 0) {
            throw std::runtime_error("[SDL] Failed to initialize SDL: " + std::string(SDL_GetError()));
        }

        // Create properties object
        SDL_PropertiesID props = SDL_CreateProperties();
        if (!props) {
            std::cerr << "[SDL] SDL_CreateProperties failed: " << SDL_GetError() << "\n";
            SDL_Quit();
            return false;
        }

        // Set window properties
        SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "SDL3 Window");
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, sdlcontext.width);
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, sdlcontext.height);
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);

        // Embed into existing HWND if parentWnd != nullptr
        if (parentWnd) {
            HWND parentHwnd = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, nullptr);
        }

        // Create window with properties
        sdlcontext.window = SDL_CreateWindowWithProperties(props);
        SDL_DestroyProperties(props);

        // SDL_Window* parentWindow = SDL_CreateWindow("Parent Window", 800, 600, 0);

        if (!sdlcontext.window) {
            std::cerr << "[SDL] SDL_CreateWindowWithProperties failed: " << SDL_GetError() << "\n";
            SDL_Quit();
            return false;
        }

        // Retrieve HWND from window properties
        SDL_PropertiesID windowProps = SDL_GetWindowProperties(sdlcontext.window);
        if (!windowProps) {
            std::cerr << "[SDL] SDL_GetWindowProperties failed: " << SDL_GetError() << "\n";
            SDL_DestroyWindow(sdlcontext.window);
            SDL_Quit();
            return false;
        }

        HWND hwnd = (HWND)SDL_GetPointerProperty(windowProps, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        sdlcontext.hwnd = hwnd;
        if (!sdlcontext.hwnd) {
            std::cerr << "[SDL] Failed to retrieve HWND\n";
            SDL_DestroyWindow(sdlcontext.window);
            SDL_Quit();
            return false;
        }

        // Create renderer
        sdlcontext.renderer = SDL_CreateRenderer(sdlcontext.window, nullptr);
        if (!sdlcontext.renderer) {
            std::cerr << "[SDL] SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
            SDL_DestroyWindow(sdlcontext.window);
            SDL_Quit();
            return false;
        }

        if (sdlcontext.parent) {
            // SDL_SetWindowParent(ctx.window, parentWindow);
			std::cout << "[SDL] Setting parent window: " << sdlcontext.parent << "\n";
            SetParent(hwnd, sdlcontext.parent);
            LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
            style &= ~WS_OVERLAPPEDWINDOW;
            style |= WS_CHILD | WS_VISIBLE;
            SetWindowLongPtr(hwnd, GWL_STYLE, style);
            SetWindowPos(sdlcontext.hwnd, nullptr, 0, sdlcontext.height,
                sdlcontext.width,
                sdlcontext.height, 
                SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

            RECT rc;
            GetClientRect(sdlcontext.parent, &rc);
            PostMessage(sdlcontext.parent, WM_SIZE, 0, MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));

        }
        // SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_ShowWindow(sdlcontext.window);
        sdlcontext.running = true;


        //SDL_Window* sdlParent = SDL_GetWindowParent(ctx.window);
        //if (sdlParent) {
        //    // It has an SDL parent
        //    std::cerr << "[SDLContext] parent failed: " << SDL_GetWindowParent(ctx.window) << "\n";
        //}

        return true;

        //std::cout << "SDL version: " << SDL_GetRevision()
        //    << " " << SDL_GetVersion() << "\n";

        //int result = SDL_Init(SDL_INIT_VIDEO);
        //if (result < 0) {
        //    // Handle error
        //    throw std::runtime_error(SDL_GetError());
        //}


        //HWND hwnd = almondnamespace::contextwindow::WindowData::getWindowHandle();
        //SDL_PropertiesID props = SDL_CreateProperties();

        //if (!props)
        //    throw std::runtime_error("Failed to create SDL properties");

        //SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "AlmondEngine");
        //SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, s_sdlstate.window.get_width());
        //SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, s_sdlstate.window.get_height());
        //SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);

        //// 👉 If you *already* have HWND:
        //if (hwnd) {
        //    SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, hwnd);
        //    // SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, hwnd);
        //}

        //// reposition after load is the best we can do with HWND (WinMain)
        //// but this cures the issue of SDL not centering the window
        //// it also cures the issue of SDL using winmain and not breaking it with it's titlebar formatting
        //s_sdlstate.window.sdl_window = SDL_CreateWindowWithProperties(props);
        //SDL_SetWindowSize(s_sdlstate.window.sdl_window, s_sdlstate.window.get_width(), s_sdlstate.window.get_height());
        ////SDL_SetWindowPosition(s_sdlstate.window.sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

        //SDL_DestroyProperties(props);

        //if (!s_sdlstate.window.sdl_window)
        //    throw std::runtime_error(SDL_GetError());

        //sdl_renderer.renderer = SDL_CreateRenderer(s_sdlstate.window.sdl_window, nullptr);
        //if (!sdl_renderer.renderer)
        //{
        //    SDL_DestroyWindow(s_sdlstate.window.sdl_window);
        //    SDL_Quit();
        //    throw std::runtime_error(SDL_GetError());
        //}

        //SDL_SetRenderDrawColor(sdl_renderer.renderer, 0, 0, 0, 255);
        ////initialize_with_sdl_window();
        //return true;
    }

    inline bool sdl_process(core::Context& ctx, core::CommandQueue& queue) {
        if (!sdlcontext.running || !sdlcontext.renderer) return false;
       // SDL_PumpEvents(); // Update SDL's internal state
        const bool* keys = SDL_GetKeyboardState(NULL);
        SDL_Event e;
        static int i = 0;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) 
            {
                sdlcontext.running = false;
                std::cout << "SDL Quit Event: " 
                    << e.type 
                    << "\n";

            }
            if (keys[SDL_SCANCODE_ESCAPE]) {
                sdlcontext.running = false; // Exit on Escape key
                std::cout << "Escape Key Event Count: " << ++i << '\n';
		    }
			return true; // Continue processing
        }

        static auto* bgTimer = almondnamespace::time::getTimer("menu", "bg_color");
        if (!bgTimer)
            bgTimer = &almondnamespace::time::createNamedTimer("menu", "bg_color");

        double t = almondnamespace::time::elapsed(*bgTimer); // seconds

        // smooth rainbow values
        Uint8 r = static_cast<Uint8>((0.5 + 0.5 * std::sin(t * 1.0)) * 255);
        Uint8 g = static_cast<Uint8>((0.5 + 0.5 * std::sin(t * 0.7 + 2.0)) * 255);
        Uint8 b = static_cast<Uint8>((0.5 + 0.5 * std::sin(t * 1.3 + 4.0)) * 255);

        SDL_SetRenderDrawColor(sdlcontext.renderer, r, g, b, 255);

//        std::cout << "yellow: " << ++i << '\n';
       // SDL_SetRenderDrawColor(sdlcontext.renderer, 255, 255, 0, 255); //yellow
        SDL_RenderClear(sdlcontext.renderer);
        SDL_RenderPresent(sdlcontext.renderer);
        //while (SDL_PollEvent(&s_sdlstate.sdl_event))
        //{
        //    if (s_sdlstate.sdl_event.type == SDL_EVENT_QUIT)
        //        return false;
        //    // Translate input...
        //}
        return true;
    }

    //inline void clear()
//{
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//}

//inline void present()
//{
//    SDL_GL_SwapWindow(s_state.window.sdl_window);
//}

    inline void sdl_clear()
    {
        SDL_SetRenderDrawColor(sdl_renderer.renderer, 0, 0, 0, 255);
        SDL_RenderClear(sdl_renderer.renderer);
    }

    inline void sdl_present()
    {
        SDL_RenderPresent(sdl_renderer.renderer);
    }

    inline void sdl_cleanup(std::shared_ptr<almondnamespace::core::Context>& ctx) {
        if (sdlcontext.renderer) {
            SDL_DestroyRenderer(sdlcontext.renderer);
            sdlcontext.renderer = nullptr;
        }
        if (sdlcontext.window) {
            SDL_DestroyWindow(sdlcontext.window);
            sdlcontext.window = nullptr;
        }
        SDL_Quit();
        sdlcontext.running = false;
        //SDL_GL_DestroyContext(s_sdlstate.window.sdl_glrc);
        //SDL_DestroyWindow(s_sdlstate.window.sdl_window);
        //SDL_Quit();
    }

    inline void set_window_position_centered()
    {
        if (s_sdlstate.window.sdl_window)
        {
            int w, h;
            SDL_GetWindowSize(s_sdlstate.window.sdl_window, &w, &h);
            SDL_SetWindowPosition(s_sdlstate.window.sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
    }

    inline void set_window_position(int x, int y)
    {
        if (s_sdlstate.window.sdl_window)
            SDL_SetWindowPosition(s_sdlstate.window.sdl_window, x, y);
    }
    inline void set_window_fullscreen(bool fullscreen)
    {
        if (s_sdlstate.window.sdl_window)
        {
            if (fullscreen)
                SDL_SetWindowFullscreen(s_sdlstate.window.sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            else
                SDL_SetWindowFullscreen(s_sdlstate.window.sdl_window, 0);
        }
    }
    inline void set_window_borderless(bool borderless)
    {
        if (s_sdlstate.window.sdl_window)
        {
            SDL_SetWindowBordered(s_sdlstate.window.sdl_window, !borderless);
        }
    }
    inline void set_window_is_resizable(bool resizable)
    {
        if (s_sdlstate.window.sdl_window)
        {
            if (resizable)
                SDL_SetWindowResizable(s_sdlstate.window.sdl_window, true);
            else
                SDL_SetWindowResizable(s_sdlstate.window.sdl_window, false);
        }
    }
    inline void set_window_minimized()
    {
        if (s_sdlstate.window.sdl_window)
        {
            SDL_MinimizeWindow(s_sdlstate.window.sdl_window);
        }
    }
    inline void set_window_size(int width, int height)
    {
        if (s_sdlstate.window.sdl_window)
            SDL_SetWindowSize(s_sdlstate.window.sdl_window, width, height);
	}

    inline void sdl_set_window_title(const std::string& title)
    {
        if (s_sdlstate.window.sdl_window)
            SDL_SetWindowTitle(s_sdlstate.window.sdl_window, title.c_str());
	}

    inline std::pair<int,int> get_size() noexcept
    {
        int w = 0, h = 0;
        if (sdl_renderer.renderer)
            SDL_GetCurrentRenderOutputSize(sdl_renderer.renderer, &w, &h);
        else if (s_sdlstate.window.sdl_window)
            SDL_GetWindowSize(s_sdlstate.window.sdl_window, &w, &h);
        return {w, h};
    }

    inline int sdl_get_width() noexcept { return get_size().first; }
    inline int sdl_get_height() noexcept { return get_size().second; }

    inline bool SDLIsRunning(std::shared_ptr<core::Context> ctx) {
        return sdlcontext.running;
    }

}

#endif // ALMOND_USING_SDL
